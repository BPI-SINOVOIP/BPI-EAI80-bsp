
#include <stdio.h>
#include "pes_init.h"
#include <gm_hal_adc.h>


/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

void    adc_st_config(unsigned int adc_base);
void    adc_channel_config(unsigned int adc_base);
void    adc_jchannel_config(unsigned int adc_base);
void    sync_delay(void);
int     adc_to_pes_test(ADC_Device_T *CMSDK_ADC);
static  int adc_channel[16];

int adc2pes(void)
{
    int error = 0;
    int seed;


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
    //ADC reset enabl0x40000080e
    HW32_REG(0x40000070) |= 1 << 12;


    printf("*** ADC1 to PES test begin ***\n");
    error += adc_to_pes_test(ADC_DEV0);
    printf("*** ADC1 to PES test end   ***\n");

    printf("*** ADC2 to PES test begin ***\n");
    error += adc_to_pes_test(ADC_DEV1);
    printf("*** ADC2 to PES test end   ***\n");

    printf("*** ADC3 to PES test begin ***\n");
    error += adc_to_pes_test(ADC_DEV2);
    printf("*** ADC3 to PES test end   ***\n");

    if (error != 0)
    {
        printf("**** ERROR : ADC to PES test failed   ***\n");
    }
    else
    {
        printf("**** SUCCESS : ADC to PES test pass   ***\n");
    }

    while (1);
    return 0;
}


int adc_to_pes_test(ADC_Device_T *CMSDK_ADC)
{
    int i;
    int error   = 0;
    unsigned int adc_base;
    int adc_num = 0;


    if (CMSDK_ADC == ADC_DEV0)
    {
        adc_base = 0x4000c000;
        adc_num = 0;
    }
    else if (CMSDK_ADC == ADC_DEV1)
    {
        adc_base = 0x4000c100;
        adc_num = 5;
    }
    else if (CMSDK_ADC == ADC_DEV2)
    {
        adc_base = 0x4000c200;
        adc_num = 10;
    }

    adc_st_config(adc_base);
    adc_channel_config(adc_base);
    adc_jchannel_config(adc_base);

    for (i = 0; i < 12; i++)
    {
        CMSDK_ADC->CTRL = 0;
        sync_delay();
        CMSDK_ADC->CTRL = (1 << 0 | 1 << 16 | CMSDK_ADC->CTRL);
        CMSDK_ADC->JCHN    |= 2 << 30;
        CMSDK_ADC->RCHN2   |= 2 << 28;
        pes_recieve_initial(i, 6, (0 + adc_num), PES_SYNC_EN, PES_POS_MODE, DETECT_EN);
        CMSDK_ADC->TRIG    |= 1 << 0; //start regular
        error += pes_recieve_detect(i);
        CMSDK_ADC->ST  |= 0xffffffff;

        CMSDK_ADC->CTRL = 0;
        sync_delay();
        CMSDK_ADC->CTRL = (1 << 0 | 1 << 16 | CMSDK_ADC->CTRL);
        pes_recieve_initial(i, 6, (1 + adc_num), PES_SYNC_EN, PES_POS_MODE, DETECT_EN);
        CMSDK_ADC->TRIG    |= 1 << 0;
        error += pes_recieve_detect(i);
        CMSDK_ADC->ST  |= 0xffffffff;

        CMSDK_ADC->CTRL = 0;
        sync_delay();
        CMSDK_ADC->CTRL = (1 << 0 | 1 << 16 | CMSDK_ADC->CTRL);
        pes_recieve_initial(i, 6, (2 + adc_num), PES_SYNC_EN, PES_POS_MODE, DETECT_EN);
        CMSDK_ADC->TRIG    |= 1 << 1;
        error += pes_recieve_detect(i);
        CMSDK_ADC->ST  |= 0xffffffff;

        CMSDK_ADC->CTRL = 0;
        sync_delay();
        CMSDK_ADC->CTRL = (1 << 0 | 1 << 16 | CMSDK_ADC->CTRL);
        pes_recieve_initial(i, 6, (3 + adc_num), PES_SYNC_EN, PES_POS_MODE, DETECT_EN);
        CMSDK_ADC->TRIG    |= 1 << 1;
        error += pes_recieve_detect(i);
        CMSDK_ADC->ST  |= 0xffffffff;

        CMSDK_ADC->CTRL = 0;
        sync_delay();
        CMSDK_ADC->CTRL = (1 << 24 | 1 << 0 | 1 << 16 | CMSDK_ADC->CTRL);
        pes_recieve_initial(i, 6, (4 + adc_num), PES_SYNC_EN, PES_POS_MODE, DETECT_EN);
        CMSDK_ADC->TRIG    |= 1 << 0;
        error += pes_recieve_detect(i);
        CMSDK_ADC->ST  |= 0xffffffff;

        pes_recieve_initial(i, 0, 0, PES_SYNC_EN, PES_POS_MODE, 0);
    }

    return error;
}


void    sync_delay(void)
{
    volatile int a  = 0;
    volatile int i  = 0;

    for (i = 0; i < 100; i++)
    {
        a ++;
    }
    return;
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

    printf("\n********ADC sample time configure begin *******\n");

    for (i = 0; i < 32; i++)
    {
        adc_st[i] = i % 10;
    }
    for (i = 0; i < 4; i = i + 1)
    {
        HW32_REG(adc_base + 0x40 + i * 4)  =   adc_st[i * 8 + 7] << 28 | adc_st[i * 8 + 6] << 24 | adc_st[i * 8 + 5] << 20 | adc_st[i * 8 + 4] << 16 | adc_st[i * 8 + 3] << 12 | adc_st[i * 8 + 2] << 8 | adc_st[i * 8 + 1] << 4 | adc_st[i * 8] << 0;
        printf("------------ADC sample time register %d is 0x%x ---------------\n", i, HW32_REG(adc_base + 0x40 + i * 4));
    }
    printf("\n********ADC sample time configure end *******\n");

    return;
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

    printf("\n-----------------------------------------------------");
    printf("| ******** ADC inject channel configure begin ********|");
    printf("-----------------------------------------------------\n");

    for (i = 0; i < 4; i++)
    {
        adc_channel[i] = i % 32;
        printf("**INFO : Inject channel %d is %d **\n", i, adc_channel[i]);
    }

    HW32_REG(adc_jch_addr)  = adc_channel[0] << 0 | adc_channel[1] << 8 | adc_channel[2] << 16 | adc_channel[3] << 24;

    printf("\n-----------------------------------------------------");
    printf("| ******** ADC inject channel configure end   ********|");
    printf("-----------------------------------------------------\n");

    return;
}

void adc_channel_config(unsigned int adc_base)
{
    unsigned int    adc_ch0_addr, adc_ch1_addr, adc_ch2_addr, adc_jch_addr;
    //int adc_channel[16];
    int i;

    adc_ch0_addr    = adc_base + 0x50;
    adc_ch1_addr    = adc_base + 0x54;
    adc_ch2_addr    = adc_base + 0x58;
    adc_jch_addr    = adc_base + 0x2c;

    printf("\n-----------------------------------------------------");
    printf("| ********ADC regular channel configure begin ********|");
    printf("-----------------------------------------------------\n");

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


    printf("\n-----------------------------------------------------");
    printf("|  ******** ADC regular channel configure end ******** |");
    printf("-----------------------------------------------------\n");


    return;
}

