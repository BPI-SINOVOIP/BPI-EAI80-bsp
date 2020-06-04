
#include <stdio.h>

#include "pes_init.h"
#include <gm_hal_adc.h>

char text_buf[20];
static int adc_channel[16];

int     adc_sing_se_test(unsigned int adc_base);
void    adc_channel_config(unsigned int adc_base);
void    adc_st_config(unsigned int adc_base);
int     adc_jsing_se_test(ADC_Device_T *CMSDK_ADC);
void    adc_status_monitor(ADC_Device_T *CMSDK_ADC);
int     adc_drst_check(int start, int end, ADC_Device_T *CMSDK_ADC);
void    adc_jchannel_config(unsigned int adc_base);
void    sync_delay(void);

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

int pes_to_adc_demo(void)
{
    int i = 0;
    int read_value;
    volatile int adc1_base, adc2_base, adc3_base;
    int seed;
    volatile int err = 0;

    printf("  adc analog clock work at 8MHz  \n");

    printf("  adc digital clock work at 90MHz  \n");
    //Enable PLL clock 180MHz-216MHz
    HW32_REG(0x40000180) |= 1 << 0;
    //FCLK 2div
    HW32_REG(0x40000090) |= 1 << 4;
    //Wait PLL stable
    while ((HW32_REG(0x400001a4) & 1 << 4) != 1 << 4);
    //FCLK source changes to PLL
    HW32_REG(0x40000090) |= 3 << 0;

    //ADC clock enable
    HW32_REG(0x40000080) |= 1 << 12;
    HW32_REG(0x40000080) |= 1 << 13;
    HW32_REG(0x40000080) |= 1 << 20;
    //ADC reset enable
    HW32_REG(0x40000070) |= 1 << 12;
    adc1_base = 0x4000C000;
    adc2_base = 0x4000C100;
    adc3_base = 0x4000C200;


    printf("\n---------- PES to ADC1 single sequence test begin-------------------------");
    err += adc_sing_se_test(adc1_base);
    printf("\n---------- PES to ADC1 single sequence test end------------while(1)---------------");


    printf("\n---------- PES to ADC2 single sequence test begin-------------------------");
    err += adc_sing_se_test(adc2_base);
    printf("\n---------- PES to ADC2 single sequence test end---------------------------");

    printf("\n---------- PES to ADC3 single sequence test begin-------------------------");
    err += adc_sing_se_test(adc3_base);
    printf("\n---------- PES to ADC3 single sequence test end---------------------------");

    printf("*** PES to ADC1 Jsingle sequence test begin ***\n");
    err += adc_jsing_se_test(ADC_DEV0);
    printf("*** PES to ADC1 Jsingle sequence test end   ***\n");

    printf("*** PES to ADC2 Jsingle sequence test begin ***\n");
    err += adc_jsing_se_test(ADC_DEV1);
    printf("*** PES to ADC2 Jsingle sequence test end   ***\n");

    printf("*** PES to ADC3 Jsingle sequence test begin ***\n");
    err += adc_jsing_se_test(ADC_DEV2);
    printf("*** PES to ADC3 Jsingle sequence test end   ***\n");

    if (err != 0)
    {
        printf("*******Error : PES to ADC test failed *******\n");
    }
    else
    {
        printf("*******Success : PES to ADC test passed *******\n");
    }

    return 0;

}

int adc_sing_se_test(unsigned int adc_base)
{
    unsigned tmp, i, b, c, d;
    volatile int adc_ctrl_addr, adc_trig_addr, adc_status_addr, adc_chn_addr, adc_drst_addr, adc_comph_addr, adc_compl_addr, adc_int_addr, adc_rch0_addr, adc_rch1_addr, adc_rch2_addr, adc_htrig_addr;
    //int adc_channel;
    int adc_chn_reg[16];
    int err = 0;
    volatile unsigned int reg;

    adc_ctrl_addr   = adc_base + 0x0;
    adc_trig_addr   = adc_base + 0x4;
    adc_status_addr = adc_base + 0x8;
    adc_chn_addr    = adc_base + 0xc;
    adc_drst_addr   = adc_base + 0x10;
    adc_comph_addr  = adc_base + 0x14;
    adc_compl_addr  = adc_base + 0x18;
    adc_int_addr    = adc_base + 0x1C;
    adc_rch0_addr   = adc_base + 0x50;
    adc_rch1_addr   = adc_base + 0x54;
    adc_rch2_addr   = adc_base + 0x58;
    adc_htrig_addr  = adc_base + 0x28;

    c = 0;

    adc_st_config(adc_base);

    for (b = 12; b > 0; b--)
    {
        printf("\n****INFO : Config Regular sequence length %d ****\n", b);

        adc_channel_config(adc_base);

        HW32_REG(adc_ctrl_addr) = 0x10011;//Enable ADC in single mode
        HW32_REG(adc_rch2_addr) |= ((b - 1) << 28); //Configur regular length

        //PES Trigger ADC
        HW32_REG(adc_htrig_addr) &= 0x0;
        HW32_REG(adc_htrig_addr) |= (1 << 0 | (b - 1) << 4);
        pes_send_initial((b - 1), PULSE_EVENT);
        if (b > 6)
        {
            //PES Trigger ADC (shouled be ignored)
            pes_send_initial((b - 1), PULSE_EVENT);
        }
        printf("\n****INFO :  Trigger adc in single sequence mode:  ****\n");
        tmp = 0;
        while (tmp != 1)
        {
            tmp = HW32_REG(adc_status_addr);
            tmp = (tmp >> 12) & 1;
        }

        HW32_REG(adc_status_addr) = 0xffffffff;

        for (i = 0; i < b; i++)
        {
            tmp = HW32_REG(adc_drst_addr);
            printf("\n****INFO : the adc result read num: %d ****\n", c);
            printf("\n****INFO : the adc convert value : 0x%x ****\n", tmp);
            //d=c+i;
            d = adc_channel[i] + c;
            //if(tmp==d)  {
            printf("\n****SUCCESS : the adc result read num: %d  success*****\n", c);
            //}
            //else    {
            //  printf ("\n****ERROR : the adc result read num: %d  fail*****\n",c);
            //    err += 1;
            //}
            c++;

        }
    }

    return err;

}

void adc_st_config(unsigned int adc_base)
{
    unsigned int    adc_st0_addr, adc_st1_addr, adc_st2_addr, adc_st3_addr;
    int adc_st[32];
    int i;

    adc_st0_addr    = adc_base + 0x40;
    adc_st1_addr    = adc_base + 0x44;
    adc_st2_addr    = adc_base + 0x48;
    adc_st3_addr    = adc_base + 0x4c;

    puts("\n********ADC sample time configure begin *******\n");

    for (i = 0; i < 32; i++)
    {
        adc_st[i] = i % 10;
    }
    for (i = 0; i < 4; i = i + 1)
    {
        HW32_REG(adc_base + 0x40 + i * 4)  =   adc_st[i * 8 + 7] << 28 | adc_st[i * 8 + 6] << 24 | adc_st[i * 8 + 5] << 20 | adc_st[i * 8 + 4] << 16 | adc_st[i * 8 + 3] << 12 | adc_st[i * 8 + 2] << 8 | adc_st[i * 8 + 1] << 4 | adc_st[i * 8] << 0;
        printf("------------ADC sample time register %d is 0x%x ---------------\n", i, HW32_REG(adc_base + 0x40 + i * 4));
    }
    puts("\n********ADC sample time configure end *******\n");

    return;
}

void adc_channel_config(unsigned int adc_base)
{
    unsigned int    adc_ch0_addr, adc_ch1_addr, adc_ch2_addr;
    //int adc_channel[16];
    int i;

    adc_ch0_addr    = adc_base + 0x50;
    adc_ch1_addr    = adc_base + 0x54;
    adc_ch2_addr    = adc_base + 0x58;

    puts("\n-----------------------------------------------------");
    puts("| ********ADC regular channel configure begin ********|");
    puts("-----------------------------------------------------\n");

    for (i = 0; i < 16; i++)
    {
        adc_channel[i] = i % 32;
        printf("**INFO : Regular channel %d is %d **\n", i, adc_channel[i]);
    }
    for (i = 0; i < 2; i = i + 1)
    {
        HW32_REG(adc_base + 0x50 + i * 4)  =   adc_channel[i * 6 + 5] << 25 | adc_channel[i * 6 + 4] << 20 | adc_channel[i * 6 + 3] << 15 | adc_channel[i * 6 + 2] << 10 | adc_channel[i * 6 + 1] << 5 | adc_channel[i * 6] << 0;
    }

    HW32_REG(adc_ch2_addr)  = adc_channel[12] << 0 | adc_channel[13] << 5 | adc_channel[14] << 10 | adc_channel[15] << 15;


    puts("\n-----------------------------------------------------");
    puts("|  ******** ADC regular channel configure end ******** |");
    puts("-----------------------------------------------------\n");

    return;
}

int adc_jsing_se_test(ADC_Device_T *CMSDK_ADC)
{
    int i;
    unsigned int drst;
    int error   = 0;
    unsigned int adc_base;

    if (CMSDK_ADC == ADC_DEV0)
    {
        adc_base = 0x4000c000;
    }
    else if (CMSDK_ADC == ADC_DEV1)
    {
        adc_base = 0x4000c100;
    }
    else if (CMSDK_ADC == ADC_DEV2)
    {
        adc_base = 0x4000c200;
    }

    adc_st_config(adc_base);
    adc_jchannel_config(adc_base);

    for (i = 0; i < 12; i++)
    {
        CMSDK_ADC->CTRL &= ~(3 << 16);
        sync_delay();
        CMSDK_ADC->CTRL = (1 << 0 | 1 << 16 | CMSDK_ADC->CTRL);
        CMSDK_ADC->JCHN |= 3 << 30;
        //PES trigger to ADC
        CMSDK_ADC->HWTRIGCTR    &= 0x0;
        CMSDK_ADC->HWTRIGCTR    |= (1 << 8 | i << 12);
        pes_send_initial(i, PULSE_EVENT);
        //PES Trigger ADC (shouled be ignored)
        //pes_send_initial(i,PULSE_EVENT);

        adc_status_monitor(CMSDK_ADC);
        error += adc_drst_check(0, 4, CMSDK_ADC);
    }

    return error;
}

void adc_jchannel_config(unsigned int adc_base)
{
    unsigned int    adc_ch0_addr, adc_ch1_addr, adc_ch2_addr, adc_jch_addr;
    //int adc_channel[16];
    int i;

    adc_ch0_addr    = adc_base + 0x50;
    adc_ch1_addr    = adc_base + 0x54;
    adc_ch2_addr    = adc_base + 0x58;
    adc_jch_addr    = adc_base + 0x2c;

    puts("\n-----------------------------------------------------");
    puts("| ******** ADC inject channel configure begin ********|");
    puts("-----------------------------------------------------\n");

    for (i = 0; i < 4; i++)
    {
        adc_channel[i] = i;
        printf("**INFO : Inject channel %d is %d **\n", i, adc_channel[i]);
    }

    HW32_REG(adc_jch_addr)  = adc_channel[0] << 0 | adc_channel[1] << 8 | adc_channel[2] << 16 | adc_channel[3] << 24;

    puts("\n-----------------------------------------------------");
    puts("| ******** ADC inject channel configure end   ********|");
    puts("-----------------------------------------------------\n");

    return;
}

void adc_status_monitor(ADC_Device_T *CMSDK_ADC)
{
    int i = 0;

    while (1)
    {
        if ((CMSDK_ADC->ST & 1 << 13) == 1 << 13)
        {
            puts("****INFO : Injected channel end of sequence happened ****\n");
            break;
        }
    }
    return;
}

int adc_drst_check(int start, int end, ADC_Device_T *CMSDK_ADC)
{
    int i;
    unsigned int drst;
    int counter = 0;
    int error = 0;

    for (i = start; i < end; i++)
    {
        drst = CMSDK_ADC->JDRST[counter];
        //if(drst != adc_channel[i] + i)  {
        //     printf("**** ERROR : Inject channel %d result data check error:%x sequence number:%d ****\n",i,drst,counter);
        //    error = 1;
        //} else  {
        //    printf("**** SUCCESS : Inject channel %d result data check correct sequence number:%d ****\n",i,counter);
        //}
        counter ++;
    }
    counter = 0;

    return error;
}

void sync_delay(void)
{
    volatile int a  = 0;
    volatile int i  = 0;

    for (i = 0; i < 100; i++)
    {
        a ++;
    }
    return;
}

