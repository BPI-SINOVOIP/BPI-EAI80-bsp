/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_hal_gpio.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <pinmux.h>
#include <adc.h>

//#define TEST_CASE_APP

//#define TEST_CASE1
//#define TEST_CASE2
//#define TEST_CASE3
//#define TEST_CASE4
//#define TEST_CASE5
//#define TEST_CASE6
//#define TEST_CASE7
//#define TEST_CASE8
//#define TEST_CASE9

//#define TEST_CASE10
//#define TEST_CASE11
//#define TEST_CASE12
//#define TEST_CASE13
//#define TEST_CASE14
//#define TEST_CASE15
//#define TEST_CASE16
//#define TEST_CASE17
//#define TEST_CASE18
//#define TEST_CASE19

//#define TEST_CASE20
//#define TEST_CASE21
//#define TEST_CASE22    1
//#define TEST_CASE23
//#define TEST_CASE24
//#define TEST_CASE25
//#define TEST_CASE26
//#define TEST_CASE27
//#define TEST_CASE28
//#define TEST_CASE29

//#define TEST_CASE30
//#define TEST_CASE31
//#define TEST_CASE32
//#define TEST_CASE33
//#define TEST_CASE34
//#define TEST_CASE35

//#define TEST_CASE36

//#define TEST_CASE36_1

//#define TEST_CASE37
//#define TEST_CASE38
//#define TEST_CASE39

//#define TEST_CASE40
//#define TEST_CASE41
//#define TEST_CASE42
//#define TEST_CASE43
//#define TEST_CASE44
//#define TEST_CASE45
//#define TEST_CASE46
//#define TEST_CASE47
//#define TEST_CASE48
//#define TEST_CASE49

//#define TEST_CASE50
//#define TEST_CASE51
//#define TEST_CASE52
//#define TEST_CASE53
//#define TEST_CASE54
//#define TEST_CASE55
//#define TEST_CASE56
//#define TEST_CASE57

//#define TEST_CASE58
//#define TEST_CASE59

//#define TEST_CASE60
//#define TEST_CASE61
//#define TEST_CASE62
//#define TEST_CASE63

//#define TEST_CASE64
//#define TEST_CASE65
//#define TEST_CASE66
//#define TEST_CASE67

//#define TEST_CASE_68_PES_TO_ADC


#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

#define SysTick_DelayMS(ms)   k_sleep(ms)
#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))
#define USER_SEQUECE_LEN   16 //SEQUECE_LEN_MAX

static ADC_Device_T *pADC1 = ADC_DEV1;
static ADC_Device_T *pADC2 = ADC_DEV2;
static ADC_Common_TypeDef *pComDev = ADC_COM_DEV;
static void udma_reset_release(void);
static void udma_clk_enable(void);
static void gpio_reset_release(void);
static void gpio_clk_enable(void);

struct gls_data
{
    struct adc_seq_entry sample;
    struct adc_seq_table adc_table;
    uint32_t adc_buffer[SEQUECE_LEN_MAX];
};
static int adc_demo(void)
{
    unsigned int data = 0;
    struct device *adc_dev;
    int i = 0;
    int j = 0;
    int k = 0;
    uint32_t adc_buffer[SEQUECE_LEN_MAX];
    struct adc_seq_table adc_table;
    struct adc_seq_entry sample;

    printk("enter func[%s]... \n", __FUNCTION__);
    udma_clk_enable();
    //HW32_REG(0x40000080) |= (1<<2);
    udma_reset_release();
    gpio_clk_enable();
    gpio_reset_release();
    patch_config();
    HAL_ADC_CLOCK(1);
    HAL_ADC_RESET();

    adc_clk_sel(1, 0);
    adc_clk_sel(0, 0);
    adc_clk_sel(2, 0);
    adc_clk_div(0, 0);
    adc_clk_div(1, 0);
    adc_clk_div(2, 0);


    adc_dev = device_get_binding(CONFIG_ADC_1_NAME);
    if (adc_dev == NULL)
    {
        printk("Failed to get ADC device.");
        return -EINVAL;
    }

    sample.buffer = &adc_buffer[0];
    sample.buffer_length = USER_SEQUECE_LEN;
    sample.init.Resolution = ADC_RESOLUTION_12BIT;
    sample.init.NbrOfConversion = sample.buffer_length;
    sample.init.workMode = ADC_SINGLE_MODE;//ADC_SINGLE_MODE;//ADC_REPEAT_MODE;
    sample.init.dma_en =  DISABLE; //DISABLE  ENABLE
    sample.init.trigermode = ADC_HW_TRIGER; /*ADC_SW_TRIGER, ADC_HW_TRIGER*/
    sample.init.timeOut = TIMEOUT_WAIT_FOREVER;
    sample.init.lowpowerEn = DISABLE;

    if (sample.init.trigermode == ADC_HW_TRIGER)
    {
        sample.init.trigersel = ADC_HW_TRIGER_PESCH0;
    }
    for (i = 0; i < sample.init.NbrOfConversion; i++)
    {
        sample.channel_cfg[i].Channel = 0;
        sample.channel_cfg[i].SamplingTime = ADC_SAMPLETIME_3CYCLES;
    }

    adc_table.entries = &sample;

    while (1)
    {

        if ((sample.init.workMode == ADC_REPEAT_MODE) || (sample.init.trigermode == ADC_HW_TRIGER))
        {
            if (j == 0)
            {
                _impl_adc_enable(adc_dev);
                adc_write_cfg(adc_dev, &adc_table);
            }
        }
        else
        {
            _impl_adc_enable(adc_dev);
            adc_write_cfg(adc_dev, &adc_table);
        }

        _impl_adc_read(adc_dev, &adc_table);

        for (k = 0; k < USER_SEQUECE_LEN; k++)
        {
            data = adc_buffer[k];
            printk("user adc_read data[%d]=[%d] \n", k, data);
            //if ((k == 1) && (data != 1))
            //{
            //  printf("adc test failed\n");
            //    return -1;
            //}
        }
        j++;
    }
    printf("exit func[%s]... \n", __FUNCTION__);

    return 0;
}

static void udma_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 2);
}

static void udma_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 2);
}

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbac;
    HW32_REG(0x1ffff0ac) = 0xff1fffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}


//#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
static void gpio_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 1);
}

static void gpio_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 1);
}

void dump_reg(ADC_Device_T *pADC, bool flag)
{
    if (flag != 0)
    {
        PR_DEBUG("pADC->CTRL=0x%x\n", pADC->CTRL);
        PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
        PR_DEBUG("pADC->COM_CHN=0x%x\n", pADC->COM_CHN);
        PR_DEBUG("pADC->TRIG=0x%x\n", pADC->TRIG);
        PR_DEBUG("pADC->IE=0x%x\n", pADC->IE);

        PR_DEBUG("pADC->DMACTR=0x%x\n", pADC->DMACTR);

        PR_DEBUG("pADC->OVSAMPCTR=0x%x\n", pADC->OVSAMPCTR);

        PR_DEBUG("pADC->COMPH=0x%x\n", pADC->COMPH);
        PR_DEBUG("pADC->COMPL=0x%x\n", pADC->COMPL);

        PR_DEBUG("pADC->DMACTR=0x%x\n", pADC->DMACTR);

        PR_DEBUG("pADC->SAMPTM[0]=0x%x\n", pADC->SAMPTM[0]);
        PR_DEBUG("pADC->SAMPTM[1]=0x%x\n", pADC->SAMPTM[1]);
        PR_DEBUG("pADC->SAMPTM[2]=0x%x\n", pADC->SAMPTM[2]);
        PR_DEBUG("pADC->SAMPTM[3]=0x%x\n", pADC->SAMPTM[3]);

        PR_DEBUG("pADC->JCHN=0x%x\n", pADC->JCHN);

        PR_DEBUG("pADC->RCHN0=0x%x\n", pADC->RCHN0);
        PR_DEBUG("pADC->RCHN1=0x%x\n", pADC->RCHN1);
        PR_DEBUG("pADC->RCHN2=0x%x\n", pADC->RCHN2);
    }
}


#if defined(TEST_CASE1)
/*
 1. ADCx config : work mode(single or repeat)/channel FIFO/sample time/channel selcetion is all random(the min,middle and max value must be covered)
 2.Enable ADCx
 3.Trigger ADCx
4.Check whether the value of busy status is 1.

*/
static void ADC_BusyTest(ADC_Device_T *pADC)
{
    dump_reg(pADC, 1);
    pADC->CTRL &= ~ADC_CTRL_WM;

    pADC->SAMPTM[0] |= 0xF;
    pADC->RCHN0 &= ~(0x1F);
    pADC->RCHN2 &= ~(0xF << 28);
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (ADC_CTRL_EN);

    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_NONE);

    SysTick_DelayMS(100);

    if (pADC->ST & 0x1)
    {
        PR_DEBUG("ADC_BusyTest pass ***\n");
    }
    else
    {
        PR_DEBUG("ADC_BusyTest failed ***\n");
    }
    dump_reg(pADC, 1);

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
}
#endif

#if defined(TEST_CASE2)
/*
1.Check ADC DFIFO EMPTY status is 1'b1 when reset
2.Configure to start a regular channel conversion with any sequence length
3.Read all the coverted data from the DFIFO.
4.Check whether ADC DFIFO EMPTY status is 1'b1
*/
static void ADC_DfifoEmptyTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    vint16_t coverted_data = 0;

    dump_reg(pADC, 1);
    if (pADC->ST & ADC_ST_DFIFO_EMPTY)
    {
        PR_DEBUG("ADC DFIFO EMPTY status is 1'b1 when reset \n");
    }
    else
    {
        PR_DEBUG("ADC DFIFO EMPTY status is failed \n");
    }

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28);
    pADC->RCHN0 = 0;
    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_NONE);

    coverted_data = (pADC->DRST & 0xFFFF);

    //SysTick_DelayMS(100);

    if (pADC->ST & ADC_ST_DFIFO_EMPTY)
    {
        PR_DEBUG("ADC_DfifoEmptyTest pass,pADC->ST=0x%x \n", pADC->ST);
    }
    else
    {
        PR_DEBUG("ADC_DfifoEmptyTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    dump_reg(pADC, 0);
}
#endif

#if defined(TEST_CASE3)
/*
1.Configure to start regular group conversion
2.Check whether ADC EOC INT is 1'b1 when one conversion is finshed
3.Write 1 to this bit
4.Check whether ADC EOC INT is 1'b0
*/
static void ADC_EocIntTest(ADC_Device_T *pADC)
{
    dump_reg(pADC, 0);
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28);
    pADC->RCHN0 = 0;
    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    //k_sleep(1000);

    PR_DEBUG("ADC_EocIntTest waiting\n");

    if (pADC->ST & ADC_ST_EOCE)
    {
        PR_DEBUG("ADC_EocIntTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_EocIntTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    pADC->ST |= ADC_ST_EOCE;
    if (pADC->ST & ADC_ST_EOCE)
    {
        PR_DEBUG("ADC_EocIntTest clear interrupt status failed\n");
    }
    else
    {
        PR_DEBUG("ADC_EocIntTest clear interrupt status pass\n");
    }

}
#endif

#if defined(TEST_CASE4)
/*
1.Configure to start regualr group conversion in sequence mode
2.Check whether ADC EOS INT is 1'b1 when all the channels in the CFIFO are finished converting
3.Write 1 to this bit
4.Check whether ADC EOS INT is 1'b0
*/
static void ADC_EosIntTest(ADC_Device_T *pADC)
{
    dump_reg(pADC, 0);
    pADC->CTRL = 0;
    pADC->ST = pADC->ST;
    pADC->RCHN2 = 0;
    pADC->RCHN0 = 0;

    pADC->CTRL &= ~ADC_CTRL_WM;

    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28);
    pADC->RCHN0 = 0;

    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);

    k_sleep(1);

    if (pADC->ST & ADC_ST_EOSI)
    {
        PR_DEBUG("ADC_EosIntTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_EosIntTest failed\n");
    }
    dump_reg(pADC, 0);

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    pADC->ST |= ADC_ST_EOSI;
    if (pADC->ST & ADC_ST_EOSI)
    {
        PR_DEBUG("ADC_EosIntTest clear interrput failed\n");
    }
    else
    {
        PR_DEBUG("ADC_EosIntTest clear interrput pass\n");
    }
}
#endif



#if defined(TEST_CASE5)
/*
1.Configure to start injected group conversion
2.Check whether ADC JEOC INT is 1'b1 when one conversion is finshed
3.Write 1 to this bit
4.Check whether ADC JEOC INT is 1'b0
*/
static void ADC_JEocIntTest(ADC_Device_T *pADC)
{
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x0 << 30); /*Injected sequence length*/
    pADC->JCHN &= (0x1F << 0);
    pADC->JCHN |= (0x0 << 0);/*Channel select bits of 1st conversion in injected sequence*/

    pADC->TRIG = 0;
    HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);

    SysTick_DelayMS(10);

    if (pADC->ST & ADC_ST_JEOC_INT)
    {
        PR_DEBUG("ADC_JEocIntTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_JEocIntTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    pADC->ST |= ADC_ST_JEOC_INT;

    if (pADC->ST & ADC_ST_JEOC_INT)
    {
        PR_DEBUG("ADC_JEocInt clear failed \n");
    }
    else
    {
        PR_DEBUG("ADC_JEocInt clear pass\n");
    }

    PR_DEBUG("after pADC->ST=0x%x\n", pADC->ST);
}
#endif

#if defined(TEST_CASE6)
/*
1.Configure to start injected group conversion (group length = x)
2.Check whether ADC JEOS INT is 1'b1 when  all the injected channels (x) are finshed
3.Write 1 to this bit
4.Check whether ADC JEOS INT is 1'b0
*/
static void ADC_JEosIntTest(ADC_Device_T *pADC)
{
    pADC->CTRL &= ~ADC_CTRL_WM;

    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x3 << 30); /*Injected sequence length*/

    pADC->JCHN &= ~(0x1F << 0);
    //pADC->JCHN |= (0x1 << 0) |(1<<8)|(1<<16)|(1<<24);

    //pADC->JCHN |= (0x1f << 0) |(0x1f<<8)|(0x1f<<16)|(0x1f<<24);

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);

    SysTick_DelayMS(10);
    if (pADC->ST & ADC_ST_JEOS_INT)
    {
        PR_DEBUG("ADC_JEosIntTest pass\n");
        PR_DEBUG("pADC->JDRST[0]=[0x%x]\n", pADC->JDRST[0]);
        PR_DEBUG("pADC->JDRST[1]=[0x%x]\n", pADC->JDRST[1]);
        PR_DEBUG("pADC->JDRST[2]=[0x%x]\n", pADC->JDRST[2]);
        PR_DEBUG("pADC->JDRST[3]=[0x%x]\n", pADC->JDRST[3]);
    }
    else
    {
        PR_DEBUG("ADC_JEosIntTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    pADC->ST |= ADC_ST_JEOS_INT;

    if (pADC->ST & ADC_ST_JEOS_INT)
    {
        PR_DEBUG("ADC_EosInt clear failed\n");
    }
    else
    {
        PR_DEBUG("ADC_EosInt clear pass\n");
    }
}
#endif

#if defined(TEST_CASE7)
/*
1.Configure to start a regular channel conversion at least 16 times
2.Check whether ADC DFIFO FULL INT is 1'b1
3.Write 1 to this bit
4.Check whether ADC EOC INT is 1'b0

*/
static void ADC_DfifoFullIntTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);
    pADC->RCHN0 = 0;

    for (i = 0; i < 16; i++)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    }

    SysTick_DelayMS(10);

    if (pADC->ST & ADC_ST_DOFSI)
    {
        PR_DEBUG("ADC_Dfifo_Full_Int_Test pass\n");
    }
    else
    {
        PR_DEBUG("ADC_Dfifo_Full_Int_Test failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
}
#endif

#if defined(TEST_CASE8)
/*
1.Configure ADC_DFIFO_PREF_EN = X
2.Configure ADC to start converting regular channel (X+1) times
3.Check whether ADC DFIFO PREF INT is 1'b
4.Write 1 to this bit
5.Check whether ADC DFIFO PREF INT is 1'b0

*/
static void ADC_DfifoPrefIntTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x4 << 28);
    pADC->RCHN0 = 0;

    pADC->DMACTR &= ~ADC_DMACTR_DFPN;
    pADC->DMACTR |= (5 << ADC_DMACTR_DFPN_POS);
    for (i = 0; i < 6; i++)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    }

    PR_DEBUG("test pADC->ST=0x%x\n", pADC->ST);

    if (pADC->ST & ADC_ST_DOPSI)
    {
        PR_DEBUG("ADC_DfifoPrefIntTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_DfifoPrefIntTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
}
#endif

#if defined(TEST_CASE9)
/*
1.Configure ADC to convert regular channel at least 17 times
2.Check whether ADC DFIFO OVF INT is 1'b1
3.Write 1 to this bit
4.Check whether ADC DFIFO OVF INT is 1'b0

*/
static void ADC_DfifoOVFIntTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    pADC->ST = 0xFFFFFFFF;
    pADC->CTRL = 0;
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);
    pADC->RCHN0 = 0;

    for (i = 0; i < 17; i++)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
        SysTick_DelayMS(10);
    }

    SysTick_DelayMS(10);

    if (pADC->ST & ADC_ST_DOFI)
    {
        PR_DEBUG("ADC_DfifoOVFIntTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_DfifoOVFIntTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
}
#endif

#if defined(TEST_CASE10)
/*
1.Configure ADC COMP HDATA and ADC COMP LDATA value
2.Configure ADC_COMP_EN =1 or ADC_JCOMP_EN=1
3.Configure to convert regular/injected channel
4.Check whether ADC COM MATCH INT is 1'b1
5.Write 1 to this bit
6.Check whether ADC COM MATCH INT is 1'b0
*/
static void ADC_CompMatIntTest(ADC_Device_T *pADC)
{
    vuint32_t i = 0;
    int compare_flag = 1;

    ADC_Common_TypeDef *pComDev1 = ADC_COM_DEV;

    pADC->ST = 0xFFFFFFFF;
    pADC->CTRL = 0;

    if (compare_flag == 0) /*regular channel*/
    {
        pADC->CTRL &= ~ADC_CTRL_WM;
        pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | ADC_CTRL_EN;

        pADC->CTRL &= ~(3 << ADC_CTRL_CM_POS);
        pADC->CTRL |= 1 << ADC_CTRL_CM_POS;

        pADC->COM_CHN &= ~(0x1F << 16);
        pADC->COM_CHN |= 0 << 16;

        pADC->COMPH = 0xFFFF;
        pADC->COMPL = 0;

        pADC->CTRL |= ADC_CTRL_CE;
        pADC->RCHN2 &= ~(0xF << 28);
        pADC->RCHN2 |= (0 << 28);
        pADC->RCHN0 = 0;

        pADC->IE = 1 | (1 << 20);

        pComDev1->EXCHEN = 1;

        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_PESCH0);

        while (1)
        {
            k_sleep(300);
            if (pADC->ST & ADC_ST_CMI)
            {
                PR_DEBUG("ADC_CompMatIntTest regular channel pass\n");
                pADC->ST |= ADC_ST_CMI;
                break;
            }
            else
            {
                PR_DEBUG("ADC_CompMatIntTest regular channel failed, pADC->ST=0x%x\n", pADC->ST);
            }
        }
    }
    else /*injected channel*/
    {
        pADC->CTRL &= ~ADC_CTRL_WM;
        pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

        pADC->CTRL &= ~(3 << ADC_CTRL_CM_POS);
        pADC->CTRL |= 1 << ADC_CTRL_CM_POS;

        pADC->COM_CHN &= ~(0x1F << 16);
        pADC->COM_CHN |= 0 << 16;

        pADC->COMPH = 0xFFFF;
        pADC->COMPL = 0;

        pADC->CTRL |= ADC_JCOMP_EN;
        pADC->JCHN &= ~(0x3 << 30);
        pADC->JCHN |= (0x0 << 30); /*Injected sequence length*/

        pADC->JCHN &= ~(0x1F << 0);

        pADC->IE = 20 | (1 << 24);

        HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);

        while (1)
        {
            k_sleep(300);
            if (pADC->ST & ADC_ST_CMI)
            {
                PR_DEBUG("ADC_CompMatIntTest injected channel  pass\n");
                pADC->ST |= ADC_ST_CMI;
                break;
            }
            else
            {
                PR_DEBUG("ADC_CompMatIntTest injected channel failed\n");
            }
        }

    }

    PR_DEBUG("pADC->ST=[0x%x], pADC->DRST=[0x%x], pADC_addr=0x%x\n", pADC->ST, pADC->DRST, pADC);

    pADC->ST |= ADC_ST_CMI;

    if (pADC->ST & (ADC_ST_CMI))
    {
        PR_DEBUG("ADC_CompMatIntTest clear interrupt failed\n");
    }
    else
    {
        PR_DEBUG("ADC_CompMatIntTest clear interrupt pass\n");
    }
}
#endif

#if defined(TEST_CASE11)
/*
1.Configure ADC_MODE = 1 and disable discontinuous mode
2.Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Triggre ADC to convert at least 1000 times
4.Check whether ADC can operate properly at following cases:
         a.Sequence length can be 0-16
         b.Channel selection can be 0-31
         c.ADC_ST bit can be 4'b0000 to 4'b1111
         d.Result resolution can be 8/10/12 bit
 The result = convertion counter + channel
*/
static void ADC_SingleModeNbitTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0F << 28); /*Sequence length*/
    //pADC->RCHN0 = 0x1F | 0x1f << 5 | 0x1f << 10| 0x1f << 15|0x1f<<20|0x1f<<25;      /*Channel selection*/
    pADC->RCHN0 = 0x1 | 0x1 << 5 | 0x1 << 10 | 0x1 << 15 | 0x1 << 20 | 0x1 << 25; /*Channel selection*/
    pADC->RCHN1 = 0x1 | 0x1 << 5 | 0x1 << 10 | 0x1 << 15 | 0x1 << 20 | 0x1 << 25;
    //pADC->RCHN2 |= 0x1F | 0x1f << 5 | 0x1f << 10| 0x1f << 15;
    pADC->RCHN2 |= 0x1 | 0x1 << 5 | 0x1 << 10 | 0x1 << 15;

    HAL_ADC_Sample_Time(pADC);

    while (i < 20)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_NONE);
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        //PR_DEBUG("i=[%d],", i);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            printk("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST = ADC_ST_EOCE;
        while (pADC->ST & ADC_ST_EOCE == ADC_ST_EOCE);

        //pADC->RCHN2 &= ~(0xF << 28);
        //pADC->RCHN2 |= (0 << 28); /*Sequence length*/
        //pADC->RCHN0 = 1;    /*Channel selection*/

        i++;
    }
    PR_DEBUG("ADC_SingleModeNbitTest end\n");

}
#endif


#if defined(TEST_CASE12)
/*
1.Configure ADC_MODE = 3 and disable discontinuous mode
2.Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Triggre ADC to convert at least 1000 times
4.Check whether ADC can operate properly
         a.Sequence length can be 0-16
         b.Channel selection can be 0-31
         c.ADC_ST bit can be 4'b0000 to 4'b1111
         d.Result resolution can be 8/10/12 bit
5.Stop repeat mode by configuring ADC_MODE =0/3
6.Check ADC can be stopped from repeat mode

 The result = convertion counter + channel
*/
static void ADC_RegularRepeatModeNbitTest(ADC_Device_T *pADC)
{
#define FIFO_DEPTH 16
    vint16_t i = 0;
    int fifo_depth = 0;
    unsigned int data[200] = {0};
    int cnt = 0;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    PR_DEBUG("enter:[%s] \n", __FUNCTION__);

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (3 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC);

    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    while (i < 5)
    {
        while ((pADC->ST & ADC_ST_DFIFO_FULL) != ADC_ST_DFIFO_FULL);

        while (fifo_depth < FIFO_DEPTH)
        {

            fifo_depth++;
            data[cnt] = pADC->DRST;
            cnt++;
            //PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }

        if ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE)
        {
            pADC->ST |= ADC_ST_EOCE;
            //PR_DEBUG("ADC_RegularRepeatModeNbitTest ok\n");
        }
        else
        {
            PR_DEBUG("ADC_RegularRepeatModeNbitTest failed\n");
        }

        fifo_depth = 0;

        //pADC->CTRL &= ~ADC_CTRL_WM; /*Stop repeat mode*/

        i++;
    }

    for (i = 0; i < 100; i++)
    {
        PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, data[i]);
    }

    PR_DEBUG("ADC_RegularRepeatModeNbitTest end\n");

}
#endif


#if defined(TEST_CASE13)
/*
Regular discontinuous mode

1.Enable discontinuous mode
2.Work mode/Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Triggre ADC to convert at least 1000 times
4.Check whether ADC can operate properly at following cases:
         a.Sequence length can be 0-16
         b.Channel selection can be 0-31
         c.ADC_ST bit can be 4'b0000 to 4'b1111
         d.Result resolution can be 8/10/12 bit

 The result = convertion counter + channel
*/
static void ADC_RegularDiscontinuousModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL |= ADC_CTRL_DCONTEN;

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (3 << 28); /*Sequence length*/
    pADC->RCHN0 = 0 << 0 | 2 << 5 | 3 << 10 | 8 << 15; /*Channel selection*/

    //pADC->SAMPTM[0] |= 0xFFFFFFFF;
    //pADC->SAMPTM[1] |= 0xFFFFFFFF;
    //pADC->SAMPTM[2] |= 0xFFFFFFFF;
    //pADC->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 1028)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        PR_DEBUG("i=[%d], pADC->DRST=[%d] pADC->ST=0x%x\n", i, pADC->DRST, pADC->ST);
        pADC->ST |= ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        i++;
    }
    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE15)
/*
ADCx DFIFO overflow

1.Enable ADC_DFIFO_FWEN
2.Work mode/Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC to convert regular channels at least 17 times
4.Check whether the DFIFO can be writed when it is full

Check signal SOC/JSOC/EOC/JEOC timing diagram
Check whether ADC_PROB is clear as soon as ADC is not busy
*/
static void ADC_DfifoOVFTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);

    pADC->ST = 0xFFFFFFFF;
    pADC->CTRL = 0;
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);
    pADC->RCHN0 = 0;

    for (i = 0; i < 17; i++)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    }

    if (pADC->ST & ADC_ST_DOFI)
    {
        PR_DEBUG("ADC_DfifoOVFIntTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_DfifoOVFIntTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif


#if defined(TEST_CASE16)
/*
Low power

1.Enable ADC_LP_EN
2.Work mode/Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Tirgger ADC to convert regular/injected channels
4.Check the SOC/JSOC/EOC/JEOC/EOS/JEOS/ADC_PROB

Check signal SOC/JSOC/EOC/JEOC timing diagram
Check whether ADC_PROB is clear as soon as ADC is not busy

*/
static void ADC_LOWPOWERTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->CTRL |= ADC_CTRL_LPEN;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0F << 28); /*Sequence length*/
    pADC->RCHN0 = 0x1F | 0x1f << 5 | 0x1f << 10 | 0x1f << 15 | 0x1f << 20 | 0x1f << 25; /*Channel selection*/
    pADC->RCHN1 = 0x1F | 0x1f << 5 | 0x1f << 10 | 0x1f << 15 | 0x1f << 20 | 0x1f << 25;
    //pADC->RCHN2 |= 0x1F | 0x1f << 5 | 0x1f << 10| 0x1f << 15;
    pADC->RCHN2 |= 0x1F | 0x1f << 5 | 0x1f << 10;

    HAL_ADC_Sample_Time(pADC);

    while (i < 1028)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        //PR_DEBUG("i=[%d],", i);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOCE;
        while (pADC->ST & ADC_ST_EOCE == ADC_ST_EOCE);
        i++;
    }
    PR_DEBUG("ADC_SingleModeNbitTest end\n");

}
#endif


#if defined(TEST_CASE17)
/*
ADC hardware oversample and average

1.Enable the ADC_OVSM_EN
2.Configure ADC_OVSM_RAT= X and ADC_OVSM_SHF = Y
3.Work mode/Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
4.Trigger ADC to convert regular/injected channels
5.Check the SOC/EOC/EOS/JSOC/JEOC/JEOS and result data

*/
static void ADC_OverSampleAVRTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    //pADC->CTRL |= ADC_CTRL_LPEN;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0F << 28); /*Sequence length*/
    //pADC->RCHN0 = 0x1F | 0x1f << 5 | 0x1f << 10| 0x1f << 15|0x1f<<20|0x1f<<25;    /*Channel selection*/
    //pADC->RCHN1 = 0x1F | 0x1f << 5 | 0x1f << 10| 0x1f << 15|0x1f<<20|0x1f<<25;
    //pADC->RCHN2 |= 0x1F | 0x1f << 5 | 0x1f << 10| 0x1f << 15;
    //pADC->RCHN2 |= 0x1F | 0x1f << 5 | 0x1f << 10;

    pADC->OVSAMPCTR &=  ~(ADC_OVSAPL_RATIO);
    pADC->OVSAMPCTR |= (0 << ADC_OVSAPL_RATIO_POS);
    pADC->OVSAMPCTR &= ~(ADC_OVSAPL_SFT);
    pADC->OVSAMPCTR |= (0 << ADC_OVSAPL_SFT_POS);

    pADC->OVSAMPCTR |= ADC_OVSAPL_EN;

    HAL_ADC_Sample_Time(pADC);

    while (i < 1028)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        //PR_DEBUG("i=[%d],", i);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOCE;
        while (pADC->ST & ADC_ST_EOCE == ADC_ST_EOCE);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE18)
/*
ADC trigger once
1.Work mode/Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
2.Trigger ADC to convert regular channel
3.Send trigger to ADC before ADC convertion is finished
4.Check whether ADC will ignore the other triggers

*/
static void ADC_TriggerOnceTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("18 enter: func[%s]\n", __FUNCTION__);

    pADC->ST = 0xFFFFFFFF;
    pADC->CTRL = 0;
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);
    pADC->RCHN0 = 0;

    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER); // for debug
    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER); // for debug
    while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

    if (pADC->ST & ADC_ST_EOSI)
    {
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("channel_num=[%d], pADC->DRST=[%d]\n", channel_num, pADC->DRST);
        }

        PR_DEBUG("ADC_TriggerOnceTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_TriggerOnceTest failed\n");
    }
    pADC->ST |= ADC_ST_EOSI;

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE19)
/*
Injected sequence mode

1.Disable the ADC_JDCON_EN
2.Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC to convert injected channels
4.Check whether ADC can operate properly at following cases:
         a.Channel selection can be 0-31
         b.ADC_ST bit can be 4'b0000 to 4'b1111
         c.Result resolution can be 8/10/12 bit
         d.ADC_JSL can be 0/1/2/3
*/
static void ADC_InjectedSequenceModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    //pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x3 << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    HAL_ADC_Sample_Time(pADC);

    while (i < 1028)
    {
        HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);
        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        //PR_DEBUG("i=[%d],", i);
        for (channel_num = 0; channel_num < 4; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        while (pADC->ST & ADC_ST_JEOS_INT == ADC_ST_JEOS_INT);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE20)
/*
Injected no sequence mode

1.Enable the ADC_JDCON_EN
2.Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC to convert injected channels
4.Check whether ADC can operate properly at following cases:
         a.Channel selection can be 0-31
         b.ADC_ST bit can be 4'b0000 to 4'b1111
         c.Result resolution can be 8/10/12 bit
         d.ADC_JSL can be 0/1/2/3
*/
static void ADC_InjectedNoSequenceModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x0 << 30); /*Injected sequence length*/

    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);
    pADC->JCHN |= (0x2 << 0) ;

    HAL_ADC_Sample_Time(pADC);

    while (i < 1028)
    {
        HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);
        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        //PR_DEBUG("i=[%d],", i);
        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE21)
/*
Injected auto mode

1.Enable ADC_JAUTO
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC to start convert regular channels in any work modes
4.Check whether injected conversion starts automactically after the regular channels conversion is finished

*/
static void ADC_InjectedAutoModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    //pADC->SAMPTM[0] |= 0xFFFFFFFF;
    //pADC->SAMPTM[1] |= 0xFFFFFFFF;
    //pADC->SAMPTM[2] |= 0xFFFFFFFF;
    //pADC->SAMPTM[3] |= 0xFFFFFFFF;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x0 << 30); /*Injected sequence length*/

    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);
    pADC->JCHN |= (0x2 << 0) ;

    pADC->CTRL |= ADC_JAUTO_EN;

    while (i < 1028)
    {
        pADC->TRIG |= ADC_TRIG_CT;
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        PR_DEBUG("\n*************************\n");
        pADC->ST = ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        //PR_DEBUG("i=[%d],", i);
        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOC_INT;
        while ((pADC->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);

        i++;
    }
    PR_DEBUG("ADC_InjectedAutoModeTest end\n");

}
#endif

#if defined(TEST_CASE22)
/*
Injected conversion interrrupt regular

1.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
2.Trigger ADC to start convert regular channels in any work modes
3.Send Jtrigger to ADC before the regular channels conversion is finished
4.Check whether injected group channnels will be converted

*/
static void ADC_InjectedIntReguModeTest(ADC_Device_T *pADC)
{

    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x0 << 30); /*Injected sequence length*/

    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);
    pADC->JCHN |= (0x2 << 0) ;

    while (i < 1028)
    {
        pADC->TRIG = ADC_TRIG_CT;
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_JCT;
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        PR_DEBUG("\n*************************\n");
        pADC->ST = ADC_ST_EOCE;
        while (pADC->ST & ADC_ST_EOCE == ADC_ST_EOCE);

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOC_INT;
        while (pADC->ST & ADC_ST_JEOC_INT == ADC_ST_JEOC_INT);

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

#if defined(TEST_CASE23)
/*
Injected conversion priority
1.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
2.Trigger ADC to start convert injected channels in any work modes
3.Send Jtrigger and trigger to ADC before injected channels conversion is not finished
4.Check whether latter Jtrigger and trigger will be ignored

Injected conversion can not be interruptted

*/
static void ADC_InjectedPriorityTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x3 << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    while (i < 50)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
        HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < 4; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        while (pADC->ST & ADC_ST_JEOS_INT == ADC_ST_JEOS_INT);

        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        i++;

    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE24)
/*
Injected conversion with regular trigger

1.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
2.Trigger ADC to start convert injected channels in any work modes
3.Send trigger to ADC before injected channels conversion is not finished
4.Check whether the injected conversion can be interrupt
5.Check whether the regular conversion will be started after the injected conversion is finished

*/
static void ADC_InjectedConvReguTrigModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= (0x0 << 30); /*Injected sequence length*/

    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);
    pADC->JCHN |= (0x2 << 0) ;

    while (i < 1028)
    {
        pADC->TRIG = ADC_TRIG_JCT;
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_CT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);
        PR_DEBUG("\n*************************\n");
        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        pADC->ST = ADC_ST_JEOC_INT;
        while ((pADC->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE25)
/*
Auto Compare mode

1.Enable the auto compare
2.Configure ADC_COMPL_DATA and ADC_COMPH_DATA
3.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
4.Check whether ADC can set ADC_COMP_MAT_INT status at following cases:
      a.ADC_COMP_MODE can be 2'b00/2'b01/2'b10/2'b11
      b.[ADC_COMP_EN,ADC_JCOMP_EN] = 2'b00/2'b01/2'b10/2'b11
Make sure that the injected channels converted result data must not be auto compared is ADC_JCOMP_EN is 1'b0.
Same as the regular channels

*/
static void ADC_AutoCompareModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    PR_DEBUG("25 enter: func[%s]\n", __FUNCTION__);

    pADC->CTRL = 0;
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);

    pADC->RCHN0 = 0 | 1 << 5 | 2 << 10 | 3 << 15 | 4 << 20 | 5 << 25; /*Channel selection*/
    pADC->RCHN1 = 6 | 7 << 5 | 8 << 10 | 9 << 15 | 10 << 20 | 11 << 25;
    pADC->RCHN2 |= 12 | 13 << 5 | 14 << 10 | 15 << 15;

    pADC->CTRL &= ~ADC_CTRL_CM;
    pADC->CTRL |= 2 << ADC_CTRL_CM_POS; /* ADC compare mode filed */

    pADC->CTRL |= ADC_CTRL_CE; /* ADC Enable the auto compare */

    pADC->COMPH = 200;
    pADC->COMPL =   1;

    pADC->COM_CHN &= ~(ADCX_COM_CHN);
    pADC->COM_CHN |= 0 << ADCX_COM_CHN_POS; /* ADC  ADCx Auto Compare Channel Selection Register  pos */

    HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
    while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

    if (pADC->ST & ADC_ST_EOSI)
    {
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("channel_num=[%d], pADC->DRST=[%d]\n", channel_num, pADC->DRST);
        }

        PR_DEBUG("%s pass\n", __FUNCTION__);
    }
    else
    {
        PR_DEBUG("%s  failed\n", __FUNCTION__);
    }
    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);

    if (pADC->ST & ADC_ST_CMI)
    {
        PR_DEBUG("===>ADC_ST_CMI interrput... \n");
    }
    else
    {
        PR_DEBUG("===>NO ADC_ST_CMI int,please check... \n");
    }

    pADC->ST |= ADC_ST_EOSI;

    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE26)
/*
Auto Compare on one channel
1.Enable the auto compare
2.Select a random(the min,middle and max value must be covered) channel to be auto compared
3.Configure ADC_COMPL_DATA and ADC_COMPH_DATA
4.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
5.Check whether ADC can set ADC_COMP_MAT_INT status at following cases:
      a.ADC_COMP_MODE can be 2'b00/2'b01/2'b10/2'b11
      b.[ADC_COMP_EN,ADC_JCOMP_EN] = 2'b00/2'b01/2'b10/2'b11
Make sure that other channels converted result data must not be auto compared
Make sure that the injected channels converted result data must not be auto compared is ADC_JCOMP_EN is 1'b0.
Same as the regular channels

*/
static void ADC_AutoCompareOneChanTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag = 1;/*regular channel, if flag = 0*/

    PR_DEBUG("26 enter: func[%s]\n", __FUNCTION__);

    if (flag == 0)
    {
        pADC->ST = 0xFFFFFFFF;
        pADC->CTRL = 0;
        pADC->CTRL &= ~ADC_CTRL_WM;
        pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

        pADC->RCHN2 &= ~(0xF << 28);
        pADC->RCHN2 |= (0xF << 28);

        pADC->RCHN0 = 0 | 1 << 5 | 2 << 10 | 3 << 15 | 4 << 20 | 5 << 25; /*Channel selection*/
        pADC->RCHN1 = 6 | 7 << 5 | 8 << 10 | 9 << 15 | 10 << 20 | 11 << 25;
        pADC->RCHN2 |= 12 | 13 << 5 | 14 << 10 | 15 << 15;

        pADC->CTRL &= ~ADC_CTRL_CM;
        pADC->CTRL |= 2 << ADC_CTRL_CM_POS; /* ADC compare mode filed */

        pADC->CTRL |= ADC_CTRL_CE; /* ADC Enable the auto compare */

        pADC->CTRL |= 1 << ADC_COMP_SING_EN;

        pADC->COMPH = 200;
        pADC->COMPL =   1;

        pADC->COM_CHN &= ~(ADCX_COM_CHN);
        pADC->COM_CHN |= 0 << ADCX_COM_CHN_POS; /* ADC  ADCx Auto Compare Channel Selection Register  pos */

        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER);
        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        if (pADC->ST & ADC_ST_EOSI)
        {
            for (channel_num = 0; channel_num < 16; channel_num++)
            {
                PR_DEBUG("channel_num=[%d], pADC->DRST=[%d]\n", channel_num, pADC->DRST);
            }

            PR_DEBUG("%s pass\n", __FUNCTION__);
        }
        else
        {
            PR_DEBUG("%s  failed\n", __FUNCTION__);
        }
        PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);

        if (pADC->ST & ADC_ST_CMI)
        {
            PR_DEBUG("===>ADC_ST_CMI interrput... \n");
        }
        else
        {
            PR_DEBUG("===>NO ADC_ST_CMI int,please check... \n");
        }

        pADC->ST |= ADC_ST_EOSI;
    }
    else
    {
        pADC->CTRL = 0;
        pADC->ST |= pADC->ST;

        pADC->CTRL &= ~ADC_CTRL_WM;
        pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

        pADC->CTRL &= ~(3 << 8);
        pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

        pADC->JCHN &= ~(0x3 << 30);
        pADC->JCHN |= (0x3 << 30); /*Injected sequence length*/

        pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

        pADC->CTRL &= ~ADC_CTRL_CM;
        pADC->CTRL |= 1 << ADC_CTRL_CM_POS; /* ADC compare mode filed */

        pADC->CTRL |= ADC_JCOMP_EN; /* ADC Enable the auto compare */

        pADC->CTRL |= 1 << ADC_COMP_SING_EN;

        pADC->COMPH = 200;
        pADC->COMPL =   1;

        pADC->COM_CHN &= ~(ADCX_COM_CHN);
        pADC->COM_CHN |= 0 << ADCX_COM_CHN_POS; /* ADC  ADCx Auto Compare Channel Selection Register  pos */

        HAL_ADC_Triger(pADC, ADC_SW_JTRIG_TRIGER);
        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        if (pADC->ST & ADC_ST_JEOS_INT)
        {
            for (channel_num = 0; channel_num < 4; channel_num++)
            {
                PR_DEBUG("channel_num=[%d], pADC->JDRST=[%d]\n", channel_num, pADC->JDRST[channel_num]);
            }

            PR_DEBUG("%s pass\n", __FUNCTION__);
        }
        else
        {
            PR_DEBUG("%s  failed\n", __FUNCTION__);
        }
        PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);

        if (pADC->ST & ADC_ST_CMI)
        {
            PR_DEBUG("===>Injected ADC_ST_CMI interrput... \n");
        }
        else
        {
            PR_DEBUG("===>NO  Injected ADC_ST_CMI int,please check... \n");
        }

        pADC->ST |= ADC_ST_JEOS_INT;

    }


    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE27)
/*
ADC channel-wise Sample time check
1.Configure each channle with different sample time
2.Configure ADCx in any mode to convert each channel.
3.Check the sample time output to analog is corresponding to the channel sample time configuration.
Each channel has each own sample time.
*/
static void ADC_ChanWiseTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);

    pADC->ST = 0xFFFFFFFF;
    pADC->CTRL = 0;
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);
    pADC->RCHN0 = 0;

    for (i = 0; i < 17; i++)
    {
        pADC->TRIG |= ADC_TRIG_CT;
        k_sleep(10);
    }

    if (pADC->ST & ADC_ST_DOFI)
    {
        PR_DEBUG("ADC_ChanWiseTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_ChanWiseTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE28)
/*
Auto Compare mode

1.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
2.Trigger ADC to start convert injected channels in any work modes
3.Send trigger to ADC before injected channels conversion is not finished
4.Check whether the injected conversion can be interrupt
5.Check whether the regular conversion will be started after the injected conversion is finished
*/
static void ADC_OutputEventToPESTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);

    pADC->ST = 0xFFFFFFFF;
    pADC->CTRL = 0;
    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28);
    pADC->RCHN0 = 0;

    for (i = 0; i < 17; i++)
    {
        pADC->TRIG |= ADC_TRIG_CT;
    }

    if (pADC->ST & ADC_ST_DOFI)
    {
        PR_DEBUG("ADC_LOWPOWERTest pass\n");
    }
    else
    {
        PR_DEBUG("ADC_LOWPOWERTest failed\n");
    }

    PR_DEBUG("pADC->ST=0x%x\n", pADC->ST);
    PR_DEBUG("func:%s end\n", __FUNCTION__);
}
#endif


#if defined(TEST_CASE29)
/*
1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected channels
4.Check whether ADC1 and ADC2 can convert injected channels simultaneously

Check ADC1 and ADC2 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 Jtrigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_InjectedSimultaneousTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);
    //pADC1->JCHN |= (0x2 << 0) ;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 10)
    {
        pADC->TRIG |= ADC_TRIG_JCT;
        //pADC1->TRIG |= ADC_TRIG_JCT; // richard add
        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        pADC1->ST |= ADC_ST_JEOS_INT;
        while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE30)
/*
Regular conversion in Injected simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to start regular channel conversion
4.Send Jtrigger before ADC1 and ADC2 regular channel conversion is finished
5.Check whether ADC1 and ADC2 can convert injected channels simultaneously

ADC1 and ADC2 regular conversion will be interrupted
Check ADC1 and ADC2 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 Jtrigger is valid or not

*/
static void ADC_REGUInjectedSimultaneousTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    ADC_Device_T *l_pADC1 = ADC_DEV1;

    PR_DEBUG("enter30: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN = 0;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24); // debug

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    l_pADC1->CTRL = 0;
    l_pADC1->ST |= pADC->ST;

    l_pADC1->CTRL &= ~ADC_CTRL_WM;
    l_pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    l_pADC1->CTRL &= ~(3 << 8);
    l_pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    l_pADC1->JCHN = 0;

    l_pADC1->JCHN &= ~(0x3 << 30);
    l_pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    l_pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    l_pADC1->RCHN2 &= ~(0xF << 28);
    l_pADC1->RCHN2 |= (0xF << 28); /*Sequence length*/
    l_pADC1->RCHN0 = 0;   /*Channel selection*/
    l_pADC1->RCHN1 = 0;
    l_pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(l_pADC1);
    while (i < 1028)
    {
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        l_pADC1->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        printf("step1\n");
        while ((l_pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        //pADC1->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  l_pADC1->ST);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((l_pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, l_pADC1->JDRST[channel_num]);
        }

        while ((l_pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC1->DRST=[%d]\n", i, l_pADC1->DRST);
        }

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        l_pADC1->ST |= ADC_ST_JEOS_INT;
        while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif


#if defined(TEST_CASE31)
/*
Injected simultaneous mode priority

1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected channels
4.Send trigger/Jtrigger to ADC1 and ADC2
5.Check whether latter triggers will be ignored

Injected conversion can not be interruptted
Latter Jtriggers will be ignored
*/
static void ADC_InjectedSimultaneousPrioTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4
    unsigned int tmp_inject[8];
    unsigned int tmp_inject1[8];

    PR_DEBUG("enter11: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);
    //pADC->JCHN |= (0x2 << 0) ;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);
    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);
#if 1
    while (i < 2)
    {

        PR_DEBUG("\n=======================\n");
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        // pADC1->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);
        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        pADC1->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        //PR_DEBUG("line=%d\n", __LINE__);

        // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            tmp_inject[channel_num] =  pADC->JDRST[channel_num];
        }
        //    PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);
        //PR_DEBUG("line=%d\n", __LINE__);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
            tmp_inject1[channel_num] = pADC1->JDRST[channel_num];
        }

        //PR_DEBUG("\n*************************\n");
        // PR_DEBUG("33pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        pADC->ST = ADC_ST_JEOS_INT | ADC_ST_EOCE;
        pADC1->ST = ADC_ST_JEOS_INT | ADC_ST_EOCE;
        // PR_DEBUG("44pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        // PR_DEBUG("line=%d\n", __LINE__);
        while (1)
        {
            //PR_DEBUG("line=%d\n", __LINE__);
            // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
            while (((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE));
            if (((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI))
            {
                //PR_DEBUG("line=%d\n", __LINE__);
            }
            else
            {
                break;
            }
        }
        PR_DEBUG("line=%d\n", __LINE__);

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        PR_DEBUG("line=%d\n", __LINE__);

        // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            tmp_inject[channel_num + 4] =  pADC->JDRST[channel_num];
        }
        //    PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
            tmp_inject1[channel_num + 4] = pADC1->JDRST[channel_num];
        }
        PR_DEBUG("line=%d\n", __LINE__);

        while (((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI));

        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }

        while (((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI));
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        PR_DEBUG("line=%d\n", __LINE__);

        for (channel_num = 0; channel_num < 8; channel_num++)
        {
            PR_DEBUG("tmp_inject[%d]=[%d]\n", channel_num, tmp_inject[channel_num]);
        }

        for (channel_num = 0; channel_num < 8; channel_num++)
        {
            PR_DEBUG("tmp_inject1[%d]=[%d]\n", channel_num, tmp_inject1[channel_num]);
        }

        pADC1->ST |= pADC1->ST;
        pADC->ST |= pADC->ST;

        PR_DEBUG("\n=======================\n");

        i++;
    }
#endif
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
    while (1); // end
}
#endif

#if defined(TEST_CASE32)
/*
ADC3 in Injected simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC3 to start injected/regular channel conversion before or after triggering  ADC1 and ADC2
4.Check whether ADC3 can work independently at following cases
    a.Trigger ADC3 to start injected/regular channel before trigger ADC1 and ADC2 to convert injected channes simultaneously
    b.Trigger ADC3 to start injected/regular channel after trigger ADC1 and ADC2 to convert injected channes simultaneously
    c.Trigger ADC1,ADC2 to start regular channel conversion and trigger ADC3 to start regular/injected channel conversion,then trigger ADC1 and ADC2 to start injected conversion

Check whether ADC3 can work independently in all any cases
Check whether result is right of each ADC
*/
static void ADC3_InjectedSimultaneousTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    ADC_Device_T *l_pADC1 = ADC_DEV1;
    ADC_Device_T *l_pADC2 = ADC_DEV2;

    PR_DEBUG("enter11: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    l_pADC1->CTRL = 0;
    l_pADC1->ST |= pADC->ST;

    l_pADC1->CTRL &= ~ADC_CTRL_WM;
    l_pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    l_pADC1->CTRL &= ~(3 << 8);
    l_pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    l_pADC1->JCHN &= ~(0x3 << 30);
    l_pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    l_pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    l_pADC1->RCHN2 &= ~(0xF << 28);
    l_pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    l_pADC1->RCHN0 = 0; /*Channel selection*/
    l_pADC1->RCHN1 = 0;
    l_pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(l_pADC1);

    l_pADC2->CTRL = 0;
    l_pADC2->ST |= pADC->ST;

    l_pADC2->CTRL &= ~ADC_CTRL_WM;
    l_pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    l_pADC2->CTRL &= ~(3 << 8);
    l_pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    l_pADC2->JCHN &= ~(0x3 << 30);
    l_pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    l_pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    l_pADC2->RCHN2 &= ~(0xF << 28);
    l_pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    l_pADC2->RCHN0 = 0; /*Channel selection*/
    l_pADC2->RCHN1 = 0;
    l_pADC2->RCHN2 |= 0;

    l_pADC2->SAMPTM[0] |= 0xFFFFFFFF;
    l_pADC2->SAMPTM[1] |= 0xFFFFFFFF;
    l_pADC2->SAMPTM[2] |= 0xFFFFFFFF;
    l_pADC2->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 10)
    {
        if (flag_mode == 0) /*pADC2 inject mode*/
        {
            l_pADC2->TRIG = ADC_TRIG_JCT; /*Trigger ADC2 */
            pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
            l_pADC1->TRIG = ADC_TRIG_JCT;

            while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

            PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  l_pADC1->ST);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
            }

            while ((l_pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

            PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  l_pADC1->ST);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, l_pADC1->JDRST[channel_num]);
            }

            while ((l_pADC2->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, l_pADC2->JDRST[channel_num]);
            }

            PR_DEBUG("\n*************************\n");
            pADC->ST |= ADC_ST_JEOS_INT;
            l_pADC1->ST |= ADC_ST_JEOS_INT;
            l_pADC2->ST |= ADC_ST_JEOS_INT;
            while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        }
        else
        {
            l_pADC2->TRIG = ADC_TRIG_CT; /*Trigger ADC2 */
            pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
            l_pADC1->TRIG = ADC_TRIG_JCT; /*JTrigger ADC1 */

            while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

            PR_DEBUG("regular mode pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  l_pADC1->ST);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
            }

            while ((l_pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

            PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  l_pADC1->ST);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, l_pADC1->JDRST[channel_num]);
            }

            while ((l_pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

            for (channel_num = 0; channel_num < 2; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, l_pADC2->DRST);
            }

            PR_DEBUG("\n*************************\n");
            pADC->ST |= ADC_ST_JEOS_INT;
            l_pADC1->ST |= ADC_ST_JEOS_INT;
            l_pADC2->ST |= ADC_ST_JEOS_INT;
            while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        }
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE33)
/*
1.Configure ADC_MULTI_MODE = 3'b101
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert regular channels
4.Check whether ADC1 and ADC2 can convert regular channels simultaneously

Check ADC1 and ADC2 signal SOC/EOC/EOS timing diagram
Check whether the ADC2 trigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_RegularSimultaneousTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_REGSMUMODEONLY_5;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;


    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC->RCHN1 = 0;
    pADC->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC1);

    while (i < 1028)
    {
        pADC->TRIG |= ADC_TRIG_CT;
        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif


#if defined(TEST_CASE34)
/*
Regular simultaneous mode priority

1.Configure ADC_MULTI_MODE = 3'b101
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected/regular channels before or after ADC1 and ADC2 starting regular conversion
4.Check whether the triggers will be ignored at following cases:
    a.Jtrigger happens before the ADC1 and ADC2 starting the regular conversion
    b.Jtrigger happens after the ADC1 and ADC2 starting the regular conversion
    c.Trigger happens after the ADC1 and ADC2 starting the regular conversion

Regular conversion can not be interruptted
All the Jtriggers will be ignored
Later triggers will be ignored
*/
static void ADC_RegularSimultaneousPrioTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_REGSMUMODEONLY_5;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);


    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC->RCHN1 = 0;
    pADC->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC1);

    while (i < 1028)
    {
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        pADC1->TRIG = ADC_TRIG_JCT;
        pADC->TRIG |= ADC_TRIG_CT;

        //pADC1->TRIG |= ADC_TRIG_CT;
        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        //while((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        //PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE35)
/*
ADC3 in Regular simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b101
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC3 to start injected/regular channel conversion before or after triggering  ADC1 and ADC2
4.Check whether ADC3 can work independently at following cases
    a.Trigger ADC3 to start injected/regular channel before trigger ADC1 and ADC2 to convert regular channes simultaneously
    b.Trigger ADC3 to start injected/regular channel after trigger ADC1 and ADC2 to convert regular channes simultaneously

Check whether ADC3 can work independently in all any cases
Check whether result is right of each ADC
*/
static void ADC3_RegularSimultaneousTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter11: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_REGSMUMODEONLY_5;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);
    //pADC1->JCHN |= (0x2 << 0) ;

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    pADC2->SAMPTM[0] |= 0xFFFFFFFF;
    pADC2->SAMPTM[1] |= 0xFFFFFFFF;
    pADC2->SAMPTM[2] |= 0xFFFFFFFF;
    pADC2->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 10)
    {
        if (flag_mode == 0) /*pADC2 Regular mode*/
        {
            pADC2->TRIG = ADC_TRIG_CT; /*Trigger ADC2 */
            pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
            pADC1->TRIG = ADC_TRIG_CT;

            while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

            PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 2; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->DRST);
            }

            while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

            PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 2; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->DRST);
            }

            while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

            for (channel_num = 0; channel_num < 2; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->DRST);
            }

            PR_DEBUG("\n*************************\n");
            pADC->ST |= ADC_ST_JEOS_INT;
            pADC1->ST |= ADC_ST_JEOS_INT;
            pADC2->ST |= ADC_ST_JEOS_INT;
            while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        }
        else
        {
            pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
            pADC1->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */
            pADC2->TRIG = ADC_TRIG_JCT; /*JTrigger ADC2 */

            while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

            PR_DEBUG("regular mode pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 2; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0->RST=[%d]\n", i, pADC->DRST);
            }

            while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

            PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 2; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->DRST);
            }

            while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
            }

            PR_DEBUG("\n*************************\n");
            pADC->ST |= ADC_ST_EOSI;
            pADC1->ST |= ADC_ST_EOSI;
            pADC2->ST |= ADC_ST_EOSI;
            while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        }
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE36)
/*
Regular Interleaved mode

1.Configure ADC_MULTI_MODE = 3'b110
2.Configure ADC_INTER_DELAY
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert regular channels
4.Check whether ADC1 and ADC2 can operate expectanly as SPEC

Check ADC1 and ADC2 signal SOC/EOC/EOS timing diagram
Check whether the ADC2 trigger is valid or not
Check whether the delay time is right
Check whether result is right of each ADC
*/
static void ADC_RegularInterleavedRepeatTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4
    int tmp_data[200];
    int tmp_data1[200];
    int cnt = 0;
    int j = 0;

    PR_DEBUG("enter36: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INTERLEAVEDMODEONLY_NUM_6;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;

    pADC->CTRL |= (3 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 2;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (3 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 2;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

    while (i < 100)
    {


        //PR_DEBUG("\n*************************\n");

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        //PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        //for (channel_num = 0; channel_num < 1; channel_num++)
        //PR_DEBUG("i=[%d], pADC0->DRST=[%d],pADC1->DRST=[%d]\n", i, pADC->DRST, pADC1->DRST);

        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        //PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC0->DRST=[%d],pADC1->DRST=[%d]\n", i, pADC->DRST, pADC1->DRST);
            //PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
            tmp_data[channel_num + cnt] = pADC->DRST;
            tmp_data1[channel_num + cnt] = pADC1->DRST;
        }
        cnt += 2;

        // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        pADC->ST |= ADC_ST_EOCE;
        pADC1->ST |= ADC_ST_EOCE;
        // while((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        // while((pADC1->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        //PR_DEBUG("\n*************************\n");

        i++;
    }

    for (j = 0; j < 100; j++)
    {
        PR_DEBUG("j=[%d], pADC0->DRST=[%d]\n", j, tmp_data[j]);
    }

    for (j = 0; j < 100; j++)
    {
        PR_DEBUG("j=[%d], pADC1->DRST=[%d]\n", j, tmp_data1[j]);
    }


    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE36_1)
/*
Regular Interleaved mode

1.Configure ADC_MULTI_MODE = 3'b110
2.Configure ADC_INTER_DELAY
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert regular channels
4.Check whether ADC1 and ADC2 can operate expectanly as SPEC

Check ADC1 and ADC2 signal SOC/EOC/EOS timing diagram
Check whether the ADC2 trigger is valid or not
Check whether the delay time is right
Check whether result is right of each ADC
*/
static void ADC_RegularInterleavedTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter36_1xx: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INTERLEAVEDMODEONLY_NUM_6;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 2;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 2;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 10)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d],pADC1->DRST=[%d]\n", i, pADC->DRST, pADC1->DRST);
        }


        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        pADC->ST |= ADC_ST_EOCE;
        pADC1->ST |= ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        PR_DEBUG("\n*************************\n");
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE37)
/*
Regular interleaved mode priority

1.Configure ADC_MULTI_MODE = 3'b110
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected/regular channels before or after ADC1 and ADC2 starting regular conversion
4.Check whether the triggers will be ignored at following cases:
    a.Jtrigger happens before the ADC1 and ADC2 starting the regular conversion
    b.Jtrigger happens after the ADC1 and ADC2 starting the regular conversion
    c.Trigger happens after the ADC1 and ADC2 starting the regular conversion

Regular conversion can not be interruptted
All the Jtriggers will be ignored
Later triggers will be ignored
*/
static void ADC_RegularInterleavedPrioTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4
    int tmp_data[200];
    int tmp_data1[200];
    int cnt = 0;
    int j = 0;

    PR_DEBUG("TEST_CASE37: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INTERLEAVEDMODEONLY_NUM_6;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 2;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 2;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 50)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        pADC->TRIG = ADC_TRIG_JCT;
        pADC1->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            tmp_data[channel_num + cnt] = pADC->DRST;
        }

        cnt += 1;

        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            tmp_data[channel_num + cnt] = pADC->DRST;
        }
        cnt += 1;


        pADC->ST |= ADC_ST_EOCE;
        pADC1->ST |= ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        PR_DEBUG("\n*************************\n");
        i++;
    }


    for (j = 0; j < 100; j++)
    {
        PR_DEBUG("j=[%d], pADC0->DRST=[%d]\n", j, tmp_data[j]);
    }

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE38)
/*
ADC3 in Interleaved mode

1.Configure ADC_MULTI_MODE = 3'b110
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC3 to start injected/regular channel conversion before or after triggering  ADC1 and ADC2
4.Check whether ADC3 can work independently at following cases
    a.Trigger ADC3 to start injected/regular channel before trigger ADC1 and ADC2 to convert regular channes
    b.Trigger ADC3 to start injected/regular channel after trigger ADC1 and ADC2 to convert regular channes

Check whether ADC3 can work independently in all any cases
Check whether result is right of each ADC
*/
static void ADC3_InInterleavedModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter38: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INTERLEAVEDMODEONLY_NUM_6;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    pADC2->SAMPTM[0] |= 0xFFFFFFFF;
    pADC2->SAMPTM[1] |= 0xFFFFFFFF;
    pADC2->SAMPTM[2] |= 0xFFFFFFFF;
    pADC2->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 80)
    {
        if (flag_mode == 0) /*pADC2 Regular mode*/
        {
            pADC2->TRIG = ADC_TRIG_CT; /*Trigger ADC2 */
            pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
            //pADC1->TRIG = ADC_TRIG_CT;

            while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

            //PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 1; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
            }

            while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

            //PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 1; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0(1)->DRST=[%d]\n", i, pADC->DRST);
            }

            while ((pADC2->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

            for (channel_num = 0; channel_num < 1; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
            }

            PR_DEBUG("\n*************************\n");
            pADC->ST |= ADC_ST_EOCE;
            pADC1->ST |= ADC_ST_EOCE;
            pADC2->ST |= ADC_ST_EOCE;
            while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        }
        else
        {
            pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
            //pADC1->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */
            pADC2->TRIG = ADC_TRIG_JCT; /*JTrigger ADC2 */

            while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

            //PR_DEBUG("regular mode pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 1; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
            }

            while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

            //PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

            for (channel_num = 0; channel_num < 1; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC0(1)->DRST=[%d]\n", i, pADC->DRST);
            }

            while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

            for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
            {
                PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
            }

            PR_DEBUG("\n*************************\n");
            pADC->ST |= ADC_ST_EOSI;
            pADC1->ST |= ADC_ST_EOSI;
            pADC2->ST |= ADC_ST_JEOC_INT;
            while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        }
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE39)
/*
Injected alternate trigger mode

1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.First trigger to ADC1 and ADC2 to start ADC1 injected conversion
4.Second trigger ADC1 and ADC2 to start ADC2 injected conversion
5.And so on
6.Check whether ADC1 and ADC2 can operate expectantly as SPEC
Check ADC1 and ADC2 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 trigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_InjectAlternateTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter39: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);


    while (i < 50)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_JCT; /*Trigger ADC0 */
        pADC->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        pADC->ST |= ADC_ST_JEOC_INT;
        pADC1->ST |= ADC_ST_JEOC_INT;
        while ((pADC->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE40)
/*
Regular conversion in Injected alternate trigger mode

1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to start regular channel conversion
4.Send Jtrigger many times before ADC1 and ADC2 regular channel conversion is finished
5.Check whether ADC1 and ADC2 can operate expectantly as SPEC

ADC1 and ADC2 regular conversion will be interrupted
Check ADC1 and ADC2 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 Jtrigger is valid or not
*/
static void ADC_RegConvInInjectAlternateTrigTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter40[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    //pADC1->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 10)
    {
        PR_DEBUG("\n*************************\n");

#if 1
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_JCT; /*alternate trigger JTrigger ADC1 */
        k_sleep(10);
        pADC1->TRIG = ADC_TRIG_CT;

        //pADC1->TRIG = ADC_TRIG_JCT;

#endif

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }


        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE41)
/*
Injected alternate trigger mode priority

1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected channels
4.Send trigger/Jtrigger to ADC1 and ADC2
5.Check whether latter triggers will be ignored

Injected conversion can not be interruptted
Latter Jtriggers will be ignored
*/
static void ADC_InjectAlternateTrigPrioTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter41[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 10)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_JCT; /*Injected alternate trigger  ADC0 */
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_JCT; /*Injected alternate  Trigger ADC1*/
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        k_sleep(10);
        pADC1->TRIG = ADC_TRIG_CT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }


        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }
        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

#if defined(TEST_CASE42)
/*
ADC3 in Injected alternate trigger mode
1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC3 to start injected/regular channel conversion before or after triggering  ADC1 and ADC2
4.Check whether ADC3 can work independently at following cases
    a.Trigger ADC3 to start injected/regular channel before trigger ADC1 and ADC2 to convert injected channes
    b.Trigger ADC3 to start injected/regular channel after trigger ADC1 and ADC2 to convert injected channes
    c.Trigger ADC1,ADC2 to start regular channel conversion and trigger ADC3 to start regular/injected channel conversion,then trigger ADC1 and ADC2 to start injected conversion

Check whether ADC3 can work independently in all any cases
Check whether result is right of each ADC

*/
static void ADC3_InInJectAltTriModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter42[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;
    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    pADC2->SAMPTM[0] |= 0xFFFFFFFF;
    pADC2->SAMPTM[1] |= 0xFFFFFFFF;
    pADC2->SAMPTM[2] |= 0xFFFFFFFF;
    pADC2->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 10)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_JCT; /*Injected alternate trigger  ADC0 */
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_JCT; /*Injected alternate  Trigger ADC1*/
        k_sleep(10);
        pADC2->TRIG = ADC_TRIG_CT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE43)
/*
Combined regular simultaneous + injected simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b001
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert regular channels
4.Send Jtrigger to ADC1 and ADC2 to start injected conversion
5.Check whether ADC1 and ADC2 can operate expectantly as SPEC

Check ADC1 and ADC2 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 Jtrigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_CombReguSimuInjectSimuTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter43[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUINJ_1;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;


    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);
    while (i < 30)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        //pADC1->TRIG = ADC_TRIG_CT;
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        //pADC1->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }
        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE44)
/*
Combined regular simultaneous + injected simultaneous mode priority

1.Configure ADC_MULTI_MODE = 3'b001
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected/regular channels
4.Send trigger/Jtrigger to ADC1 and ADC2
5.Check whether ADC1 and ADC2 can operate expectanly as SPEC

Injected conversion can not be interruptted
Latter Jtriggers will be ignored during injected conversion
Regular conversion can be interrupt by Jtrigger
Later regular trigger will be ignored during regular conversion
*/
static void ADC_CombReguSimuInjectSimuPrioTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter44[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUINJ_1;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);
    while (i < 30)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        //pADC1->TRIG = ADC_TRIG_JCT;
        k_sleep(10);

        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        // pADC1->TRIG = ADC_TRIG_CT;


        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }
        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }


        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif


#if defined(TEST_CASE45)
/*
ADC3 in Combined regular simultaneous
+ injected simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b001
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC3 to start injected/regular channel conversion before or after triggering  ADC1 and ADC2
4.Check whether ADC3 can work independently at following cases
    a.Trigger ADC3 to start injected/regular channel before trigger ADC1 and ADC2 to convert injected/regular channes
    b.Trigger ADC3 to start injected/regular channel after trigger ADC1 and ADC2 to convert injected/regular  channes
    c.Trigger ADC1,ADC2 to start regular channel conversion and trigger ADC3 to start regular/injected channel conversion,then trigger ADC1 and ADC2 to start injected conversion

Check whether ADC3 can work independently in all any cases
Check whether result is right of each ADC
*/
static void ADC3_CombReguSimuInjectSimuModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter11: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUINJ_1;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);


    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    pADC2->SAMPTM[0] |= 0xFFFFFFFF;
    pADC2->SAMPTM[1] |= 0xFFFFFFFF;
    pADC2->SAMPTM[2] |= 0xFFFFFFFF;
    pADC2->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 30)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        k_sleep(1);
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        k_sleep(1);
        pADC2->TRIG = ADC_TRIG_JCT;


        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }
        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }



        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

#if defined(TEST_CASE46)
/*
Combined regular simultaneous + injected alternate mode

1.Configure ADC_MULTI_MODE = 3'b010
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert regular channels
4.Send Jtrigger to ADC1 and ADC2 to start injected conversion
5.Check whether ADC1 and ADC2 can operate expectantly as SPEC

Check ADC1 and ADC2 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 Jtrigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_CombReguSimuInjectAltModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter46[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUALT_NUM_2;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);
    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 20)
    {
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 ReguSimu */

        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC0 */

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);
        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC1 */

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        PR_DEBUG("i=[%d], pADC->ST=[0x%x], pADC1->ST=[0x%x]\n", i, pADC->ST, pADC1->ST);

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }


        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("regular mode pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;

        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE47)
/*
Combined regular simultaneous
+ injected alternate mode priority

1.Configure ADC_MULTI_MODE = 3'b010
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 and ADC2 to convert injected/regular channels
4.Send trigger/Jtrigger to ADC1 and ADC2
5.Check whether ADC1 and ADC2 can operate expectanly as SPEC

Injected conversion can not be interruptted
Latter Jtriggers will be ignored during injected conversion
Regular conversion can be interrupt by Jtrigger
Check whether the ADC1 and ADC2 regular conversion is interrupted simultaneously
Later regular trigger will be ignored during regular conversion

*/
static void ADC_CombReguSimuInjectAltSimuPrioModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter47[line=%d]: func[%s]\n", __LINE__, __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUALT_NUM_2;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    while (i < 30)
    {
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC1 */

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }


        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("regular mode pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE48)
/*
ADC3 in Combined regular simultaneous+ injected alternate mode

1.Configure ADC_MULTI_MODE = 3'b010
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC3 to start injected/regular channel conversion before or after triggering  ADC1 and ADC2
4.Check whether ADC3 can work independently at following cases
    a.Trigger ADC3 to start injected/regular channel before trigger ADC1 and ADC2 to convert injected/regular channes
    b.Trigger ADC3 to start injected/regular channel after trigger ADC1 and ADC2 to convert injected/regular  channes
    c.Trigger ADC1,ADC2 to start regular channel conversion and trigger ADC3 to start regular/injected channel conversion,then trigger ADC1 and ADC2 to start injected conversion

Check whether ADC3 can work independently in all any cases
Check whether result is right of each ADC

*/
static void ADC3_CombReguSimuInjectAltModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter48: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUALT_NUM_2;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 1)
    {
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        //PR_DEBUG("test: func[%s]\n", __FUNCTION__);

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */

        pADC2->TRIG = ADC_TRIG_CT;  //trigger ADC3 to regular

        while ((pADC2->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        pADC2->TRIG = ADC_TRIG_JCT; /*JTrigger ADC2 */


        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC1 */

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("regular mode pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC2->ST =[0x%x]\n", pADC->ST,  pADC2->ST);

        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE49)
/*
Triple ADC mode (DCN)
(ADC_MULTI_NUM=1)

1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected/regular channels
4.Send trigger/Jtrigger to ADC1 ADC2 and ADC3
5.Check whether ADC1 ADC2 and ADC3 can operate expectanly as SPEC
Injected conversion can not be interruptted
Latter Jtriggers will be ignored during injected conversion
Regular conversion can be interrupt by Jtrigger
Check whether the ADC1 ADC2 and ADC3 regular conversion is interrupted simultaneously
Later regular trigger will be ignored during regular conversion
*/
static void ADC_InjectedSimultaneousTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter49: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    HAL_ADC_Sample_Time(pADC);
    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    HAL_ADC_Sample_Time(pADC2);

    while (i < 10)
    {
        pADC->TRIG |= ADC_TRIG_JCT;
        //pADC1->TRIG |= ADC_TRIG_JCT;
        //pADC2->TRIG |= ADC_TRIG_JCT;
        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        pADC1->ST |= ADC_ST_JEOS_INT;
        pADC2->ST |= ADC_ST_JEOS_INT;
        while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        while ((pADC1->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        while ((pADC2->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        i++;
    }

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE50)
/*
Regular conversion in Injected simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1,ADC2 and ADC3 to start regular channel conversion
4.Send Jtrigger before ADC1.ADC2 and ADC3 regular channel conversion is finished
5.Check whether ADC1,ADC2 and ADC3 can convert injected channels simultaneously

ADC1,ADC2 and ADC3 regular conversion will be interrupted
Check ADC1,ADC2 and ADC3 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 and ADC3 Jtrigger is valid or not

*/
static void ADC_REGUInInjectedSimultaneousTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter50: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN = 0;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN = 0;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    //pADC1->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN = 0;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    //pADC->JCHN |= (0x2 << 0) |(0x2<<8)|(0x2<<16)|(0x2<<24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 3)
    {
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        pADC1->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */
        pADC2->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC2->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        //pADC1->TRIG = ADC_TRIG_JCT;
        //pADC2->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x],pADC2->ST =[0x%x]\n", pADC->ST,  pADC1->ST,  pADC2->ST);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_JEOS_INT;
        pADC1->ST |= ADC_ST_JEOS_INT;
        pADC2->ST |= ADC_ST_JEOS_INT;
        while ((pADC->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        while ((pADC1->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        while ((pADC2->ST & ADC_ST_JEOS_INT) == ADC_ST_JEOS_INT);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE51)
/*
Injected simultaneous mode priority Triple

1.Configure ADC_MULTI_MODE = 3'b100
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1,ADC2 and ADC3 to convert injected channels
4.Send trigger/Jtrigger to ADC1,ADC2 and ADC3
5.Check whether latter triggers will be ignored

Injected conversion can not be interruptted
Latter Jtriggers will be ignored

*/
static void ADC_InjectedSimultaneousPrioTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4
    unsigned int tmp_inject[8];
    unsigned int tmp_inject1[8];
    unsigned int tmp_inject2[8];

    PR_DEBUG("enter51: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INJSMUMODEONLY_4;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);
    while (i < 3)
    {
        PR_DEBUG("\n=======================\n");
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        // pADC1->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);
        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);
        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        pADC1->TRIG = ADC_TRIG_CT; /*Trigger ADC1 */
        pADC2->TRIG = ADC_TRIG_CT; /*Trigger ADC2 */

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        //PR_DEBUG("line=%d\n", __LINE__);

        // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            tmp_inject[channel_num] =  pADC->JDRST[channel_num];
        }
        //    PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);
        //PR_DEBUG("line=%d\n", __LINE__);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
            tmp_inject1[channel_num] = pADC1->JDRST[channel_num];
        }

        while ((pADC2->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);
        //PR_DEBUG("line=%d\n", __LINE__);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
            tmp_inject2[channel_num] = pADC2->JDRST[channel_num];
        }


        //PR_DEBUG("\n*************************\n");
        // PR_DEBUG("33pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        pADC->ST = ADC_ST_JEOS_INT | ADC_ST_EOCE;
        pADC1->ST = ADC_ST_JEOS_INT | ADC_ST_EOCE;
        pADC2->ST = ADC_ST_JEOS_INT | ADC_ST_EOCE;
        // PR_DEBUG("44pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        // PR_DEBUG("line=%d\n", __LINE__);
        while (1)
        {
            //PR_DEBUG("line=%d\n", __LINE__);
            // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
            while (((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE));
            if (((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI))
            {
                //PR_DEBUG("line=%d\n", __LINE__);
            }
            else
            {
                break;
            }
        }
        PR_DEBUG("line=%d\n", __LINE__);

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */

        while ((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        PR_DEBUG("line=%d\n", __LINE__);

        // PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            tmp_inject[channel_num + 4] =  pADC->JDRST[channel_num];
        }
        //    PR_DEBUG("i=[%d], pADC0->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);

        while ((pADC1->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
            tmp_inject1[channel_num + 4] = pADC1->JDRST[channel_num];
        }
        PR_DEBUG("line=%d\n", __LINE__);

        while ((pADC2->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            //PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
            tmp_inject2[channel_num + 4] = pADC2->JDRST[channel_num];
        }


        while (((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI));

        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC->DRST=[%d]\n", i, pADC->DRST);
        }

        while (((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI));
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while (((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI));
        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("***i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("line=%d\n", __LINE__);

        for (channel_num = 0; channel_num < 8; channel_num++)
        {
            PR_DEBUG("tmp_inject[%d]=[%d]\n", channel_num, tmp_inject[channel_num]);
        }

        for (channel_num = 0; channel_num < 8; channel_num++)
        {
            PR_DEBUG("tmp_inject1[%d]=[%d]\n", channel_num, tmp_inject1[channel_num]);
        }

        for (channel_num = 0; channel_num < 8; channel_num++)
        {
            PR_DEBUG("tmp_inject2[%d]=[%d]\n", channel_num, tmp_inject2[channel_num]);
        }

        pADC1->ST |= pADC1->ST;
        pADC->ST |= pADC->ST;
        pADC2->ST |= pADC2->ST;

        PR_DEBUG("\n=======================\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

#if defined(TEST_CASE52)
/*
1.Configure ADC_MULTI_MODE = 3'b101
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1,ADC2 and ADC3 to convert regular channels
4.Check whether ADC1,ADC2 and ADC3 can convert regular channels simultaneously

Check ADC1,ADC2 and ADC3 signal SOC/EOC/EOS timing diagram
Check whether the ADC2 and ADC3 trigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_RegularSimultaneousTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("52 enter: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_REGSMUMODEONLY_5;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;
    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC->RCHN1 = 0;
    pADC->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN0 = 0;   /*Channel selection*/

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC2);

    while (i < 10)
    {
        pADC->TRIG |= ADC_TRIG_CT;
        //pADC1->TRIG |= ADC_TRIG_CT;
        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        while ((pADC1->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        while ((pADC2->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE53)
/*
Regular simultaneous mode priority

1.Configure ADC_MULTI_MODE = 3'b101
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected/regular channels before or after ADC1 ADC2 and ADC3 starting regular conversion
4.Check whether the triggers will be ignored at following cases:
    a.Jtrigger happens before the ADC1 ADC2 and ADC3 starting the regular conversion
    b.Jtrigger happens after the ADC1 ADC2 and ADC3 starting the regular conversion
    c.Trigger happens after the ADC1 ADC2 and ADC3 starting the regular conversion
Regular conversion can not be interruptted
All the Jtriggers will be ignored
Later triggers will be ignored
*/
static void ADC_RegularSimultaneousPrioTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("53 enter: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_REGSMUMODEONLY_5;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);


    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC->RCHN1 = 0;
    pADC->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN0 = 0;    /*Channel selection*/

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);


    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (1 << 28); /*Sequence length*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN0 = 0;    /*Channel selection*/

    pADC2->SAMPTM[0] |= 0xFFFFFFFF;
    pADC2->SAMPTM[1] |= 0xFFFFFFFF;
    pADC2->SAMPTM[2] |= 0xFFFFFFFF;
    pADC2->SAMPTM[3] |= 0xFFFFFFFF;

    while (i < 20)
    {
        pADC->TRIG = ADC_TRIG_CT;
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        k_sleep(10);
        pADC1->TRIG = ADC_TRIG_JCT;
        k_sleep(10);
        pADC2->TRIG = ADC_TRIG_JCT;

        //pADC1->TRIG |= ADC_TRIG_CT;
        //pADC2->TRIG |= ADC_TRIG_CT;
        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;

        //while((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT);
        //for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        //{
        //  PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        //}


        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        while ((pADC1->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        while ((pADC2->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE54)
/*
Regular Interleaved mode

1.Configure ADC_MULTI_MODE = 3'b110
2.Configure ADC_INTER_DELAY
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected channels
4.Check whether ADC1 ADC2 and ADC3 can operate expectanly as SPEC

Check ADC1 ADC2 and ADC3 signal SOC/EOC/EOS timing diagram
Check whether the ADC2 and ADC3 trigger is valid or not
Check whether the delay time is right
Check whether result is right of each ADC
*/
static void ADC_RegularInterleavedTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("TEST_CASE54: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INTERLEAVEDMODEONLY_NUM_6;


    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 2;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 2;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC2->RCHN0 = 2;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 100)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC2->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 3; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d],pADC1->DRST=[%d], pADC2->DRST=[%d]\n", i, pADC->DRST, pADC1->DRST,  pADC2->DRST);
        }


        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x], pADC2->ST =[0x%x]\n", pADC->ST,  pADC1->ST,  pADC2->ST);

        pADC->ST |= ADC_ST_EOCE;
        pADC1->ST |= ADC_ST_EOCE;
        pADC2->ST |= ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        while ((pADC2->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        PR_DEBUG("\n*************************\n");
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

#if defined(TEST_CASE55)
/*
Regular interleaved mode priority

1.Configure ADC_MULTI_MODE = 3'b110
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected/regular channels before or after ADC1 ADC2 and ADC3 starting regular conversion
4.Check whether the triggers will be ignored at following cases:
    a.Jtrigger happens before the ADC1 ADC2 and ADC3 starting the regular conversion
    b.Jtrigger happens after the ADC1 ADC2 and ADC3 starting the regular conversion
    c.Trigger happens after the ADC1 ADC2 and ADC3 starting the regular conversion
Regular conversion can not be interruptted
All the Jtriggers will be ignored
Later triggers will be ignored
*/
static void ADC_RegularInterleavedPrioTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("TEST_CASE54: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_INTERLEAVEDMODEONLY_NUM_6;


    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC->RCHN0 = 2;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC1->RCHN0 = 2;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x0 << 28); /*Sequence length*/
    pADC2->RCHN0 = 2;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 100)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        pADC->TRIG = ADC_TRIG_JCT;
        pADC1->TRIG = ADC_TRIG_JCT;
        pADC2->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) != ADC_ST_EOCE);
        while ((pADC2->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);

        for (channel_num = 0; channel_num < 3; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d],pADC1->DRST=[%d], pADC2->DRST=[%d]\n", i, pADC->DRST, pADC1->DRST,  pADC2->DRST);
        }


        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x], pADC2->ST =[0x%x]\n", pADC->ST,  pADC1->ST,  pADC2->ST);


        //while((pADC->ST & ADC_ST_JEOS_INT) != ADC_ST_JEOS_INT); //All the Jtriggers will be ignored

        pADC->ST |= ADC_ST_EOCE;
        pADC1->ST |= ADC_ST_EOCE;
        pADC2->ST |= ADC_ST_EOCE;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        while ((pADC1->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        while ((pADC2->ST & ADC_ST_EOCE) == ADC_ST_EOCE);

        PR_DEBUG("\n*************************\n");
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE56)
/*
Injected alternate trigger mode

1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.First trigger to ADC1 ADC2 and ADC3 to start ADC1 injected conversion
4.Second trigger ADC1 ADC2 and ADC3 to start ADC2 injected conversion
5.And so on
6.Check whether ADC1 ADC2 and ADC3 can operate expectantly as SPEC
Check ADC1 ADC2 and ADC3 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 and ADC3 trigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_InjectAlternateTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter56: func[%s]\n", __FUNCTION__);


    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);


    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);


    while (i < 50)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_JCT; /*Trigger ADC0 */
        pADC->TRIG = ADC_TRIG_JCT;
        pADC->TRIG = ADC_TRIG_JCT;

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        pADC->ST |= ADC_ST_JEOC_INT;
        pADC1->ST |= ADC_ST_JEOC_INT;
        pADC2->ST |= ADC_ST_JEOC_INT;
        while ((pADC->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        while ((pADC1->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        while ((pADC2->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        PR_DEBUG("\n*************************\n");

        i++;
    }

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE57)
/*
Regular conversion in Injected alternate trigger mode

1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to start regular channel conversion
4.Send Jtrigger many times before ADC1 ADC2 and ADC3 regular channel conversion is finished
5.Check whether ADC1 ADC2 and ADC3 can operate expectantly as SPEC
ADC1 ADC2 and ADC3 regular conversion will be interrupted
Check ADC1 ADC2 and ADC3 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 and ADC3 Jtrigger is valid or not
*/
static void ADC_RegConvInjectAlternateTrigTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter57: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 50)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_CT;
        pADC1->TRIG = ADC_TRIG_CT;
        pADC2->TRIG = ADC_TRIG_CT;

        pADC->TRIG = ADC_TRIG_JCT; /*alternate JTrigger ADC0 notice */
        pADC->TRIG = ADC_TRIG_JCT; /*alternate JTrigger ADC1 notice,refer spec*/
        pADC->TRIG = ADC_TRIG_JCT; /*alternate JTrigger ADC2 notice*/

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC2->DRST);
        }


        pADC->ST |= ADC_ST_JEOC_INT;
        pADC1->ST |= ADC_ST_JEOC_INT;
        pADC2->ST |= ADC_ST_JEOC_INT;
        while ((pADC->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        while ((pADC1->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        while ((pADC2->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE59)
/*
Injected alternate trigger mode priority

1.Configure ADC_MULTI_MODE = 3'b111
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected channels
4.Send trigger/Jtrigger to ADC1 ADC2 and ADC3
5.Check whether latter triggers will be ignored

Injected conversion can not be interruptted
Latter Jtriggers will be ignored
*/
static void ADC_InjectAlternateTrigPrioTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter59: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_ALTTRIGMODEONLY_7;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 50)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_JCT; /*alternate JTrigger ADC0 notice */
        pADC->TRIG = ADC_TRIG_JCT; /*alternate JTrigger ADC1 notice,refer spec*/
        pADC->TRIG = ADC_TRIG_JCT; /*alternate JTrigger ADC2 notice*/

        pADC->TRIG = ADC_TRIG_CT;
        pADC1->TRIG = ADC_TRIG_CT;
        pADC2->TRIG = ADC_TRIG_CT;

        pADC1->TRIG = ADC_TRIG_JCT; /*Latter Jtriggers will be ignored*/
        pADC2->TRIG = ADC_TRIG_JCT; /*Latter Jtriggers will be ignored*/

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC2->DRST);
        }


        pADC->ST |= ADC_ST_JEOC_INT;
        pADC1->ST |= ADC_ST_JEOC_INT;
        pADC2->ST |= ADC_ST_JEOC_INT;
        while ((pADC->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        while ((pADC1->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        while ((pADC2->ST & ADC_ST_JEOC_INT) == ADC_ST_JEOC_INT);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE60)
/*
Combined regular simultaneous + injected simultaneous mode

1.Configure ADC_MULTI_MODE = 3'b001
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert regular channels
4.Send Jtrigger to ADC1 ADC2 and ADC3 to start injected conversion
5.Check whether ADC1 ADC2 and ADC3 can operate expectantly as SPEC
Check ADC1 ADC2 and ADC3 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 and ADC3 Jtrigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_CombReguSimuInjectSimuTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter60: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUINJ_1;
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 30)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */
        k_sleep(1);

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }
        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC2->DRST);
        }

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }


        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE61)
/*
Combined regular simultaneous+ injected simultaneous mode priority

1.Configure ADC_MULTI_MODE = 3'b001
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected/regular channels
4.Send trigger/Jtrigger to ADC1 ADC2 and ADC3
5.Check whether ADC1 ADC2 and ADC3 can operate expectanly as SPEC
Injected conversion can not be interruptted
Latter Jtriggers will be ignored during injected conversion
Regular conversion can be interrupt by Jtrigger
Later regular trigger will be ignored during regular conversion
*/
static void ADC_CombReguSimuInjectSimuPrioTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 0;
#define SEQUENCE_LEN   4

    PR_DEBUG("enter61: func[%s]\n", __FUNCTION__);
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUINJ_1;
    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;

    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 30)
    {
        PR_DEBUG("\n*************************\n");

        pADC->TRIG = ADC_TRIG_JCT; /*JTrigger ADC0 */
        k_sleep(1);
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 */

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        PR_DEBUG("pADC->ST =[0x%x], pADC1->ST =[0x%x]\n", pADC->ST,  pADC1->ST);
        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }
        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC2->DRST);
        }

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }


        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;
        while ((pADC->ST & ADC_ST_EOCE) == ADC_ST_EOCE);
        PR_DEBUG("\n*************************\n");

        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

#if defined(TEST_CASE62)
/*
Combined regular simultaneous + injected alternate mode
1.Configure ADC_MULTI_MODE = 3'b010
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert regular channels
4.Send Jtrigger to ADC1 ADC2 and ADC3 to start injected conversion
5.Check whether ADC1 ADC2 and ADC3 can operate expectantly as SPEC
Check ADC1 ADC2 and ADC3 signal JSOC/JEOC/JEOS timing diagram
Check whether the ADC2 and ADC3 Jtrigger is valid or not
Check whether result is right of each ADC
*/
static void ADC_CombReguSimuInjectAltModeTripleTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter62: func[%s]\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUALT_NUM_2;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;


    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0xF << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 20)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 ReguSimu */

        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC0 */

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC1 */



        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC2 */


        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);



        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);



        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);
        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);


        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }


        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }


        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }



        for (channel_num = 0; channel_num < 16; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;

        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE63)
/*
Combined regular simultaneous + injected alternate mode priority

1.Configure ADC_MULTI_MODE = 3'b010
2.ADC_MODE/Sequence length/Jchannel selection/ADC_JSL/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Trigger ADC1 ADC2 and ADC3 to convert injected/regular channels
4.Send trigger/Jtrigger to ADC1 ADC2 and ADC3
5.Check whether ADC1 ADC2 and ADC3 can operate expectanly as SPEC
Injected conversion can not be interruptted
Latter Jtriggers will be ignored during injected conversion
Regular conversion can be interrupt by Jtrigger
Check whether the ADC1 ADC2 and ADC3 regular conversion is interrupted simultaneously
Later regular trigger will be ignored during regular conversion
*/
static void ADC_CombReguSimuInjectAltSimuPrioTripleModeTest(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int flag_mode = 1;
#define SEQUENCE_LEN   1

    PR_DEBUG("enter63: func[%s]\r\n", __FUNCTION__);

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_MODE);
    pComDev->ADC_MCTRL |= ADC_COMBINDREGSMUALT_NUM_2;

    pComDev->ADC_MCTRL &= ~(ADC_MULTI_NUM);
    pComDev->ADC_MCTRL |= ADC_TRIPLE << ADC_MULTI_NUM_POS;


    pComDev->ADC_MCTRL &= ~(ADC_INTER_DELAY);
    pComDev->ADC_MCTRL |= (ADC_INTER_DELAY_15 << ADC_INTER_DELAY_POS);

    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->JCHN &= ~(0x3 << 30);
    pADC->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/
    pADC->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC->RCHN0 = 0;   /*Channel selection*/
    pADC->RCHN1 = 0;
    pADC->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC);

    pADC1->CTRL = 0;
    pADC1->ST |= pADC1->ST;

    pADC1->CTRL &= ~ADC_CTRL_WM;
    pADC1->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC1->CTRL &= ~(3 << 8);
    pADC1->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC1->JCHN &= ~(0x3 << 30);
    pADC1->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC1->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC1->RCHN2 &= ~(0xF << 28);
    pADC1->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC1->RCHN0 = 0;   /*Channel selection*/
    pADC1->RCHN1 = 0;
    pADC1->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC1);

    pADC2->CTRL = 0;
    pADC2->ST |= pADC2->ST;

    pADC2->CTRL &= ~ADC_CTRL_WM;
    pADC2->CTRL |= (1 << ADC_CTRL_WM_POS) | (1 << ADC_CTRL_EN_POS);

    pADC2->CTRL &= ~(3 << 8);
    pADC2->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC2->JCHN &= ~(0x3 << 30);
    pADC2->JCHN |= ((SEQUENCE_LEN - 1) << 30); /*Injected sequence length*/

    pADC2->JCHN |= (0x2 << 0) | (0x2 << 8) | (0x2 << 16) | (0x2 << 24);

    pADC2->RCHN2 &= ~(0xF << 28);
    pADC2->RCHN2 |= (0x1 << 28); /*Sequence length*/
    pADC2->RCHN0 = 0;   /*Channel selection*/
    pADC2->RCHN1 = 0;
    pADC2->RCHN2 |= 0;

    HAL_ADC_Sample_Time(pADC2);

    while (i < 20)
    {
        PR_DEBUG("\n*************************\n");
        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC0 */
        k_sleep(10);
        pADC->TRIG = ADC_TRIG_CT; /*Trigger ADC0 ReguSimu */

        while ((pADC->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC1 */
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC->JDRST=[%d]\n", i, pADC->JDRST[channel_num]);
        }

        while ((pADC1->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        pADC->TRIG = ADC_TRIG_JCT; /*injected alternate JTrigger ADC2 */
        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->JDRST=[%d]\n", i, pADC1->JDRST[channel_num]);
        }

        while ((pADC2->ST & ADC_ST_JEOC_INT) != ADC_ST_JEOC_INT);

        for (channel_num = 0; channel_num < SEQUENCE_LEN; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->JDRST=[%d]\n", i, pADC2->JDRST[channel_num]);
        }

        while ((pADC->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC0->DRST=[%d]\n", i, pADC->DRST);
        }

        while ((pADC1->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC1->DRST=[%d]\n", i, pADC1->DRST);
        }

        while ((pADC2->ST & ADC_ST_EOSI) != ADC_ST_EOSI);

        for (channel_num = 0; channel_num < 2; channel_num++)
        {
            PR_DEBUG("i=[%d], pADC2->DRST=[%d]\n", i, pADC2->DRST);
        }

        PR_DEBUG("\n*************************\n");
        pADC->ST |= ADC_ST_EOSI;
        pADC1->ST |= ADC_ST_EOSI;
        pADC2->ST |= ADC_ST_EOSI;

        while ((pADC->ST & ADC_ST_EOSI) == ADC_ST_EOSI);
        i++;
    }
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif


#if 0
GM6721 ADC / DAC / ACMP Function2a¨º ? D¨¨ ? ¨®¨¨ ?? ? ¡êo
1.  ADC ¡ê¡§3 ?? ¡ê ?
1)  D_ADC_EN ? ¨¦ ?? ??
2)  D_ADC_CLK ? ¨¦ ?? ?? ¡ê¡§32M / 16M / 8M / 4M / 1MHz¡ê ?
3)  D_ADC_CHSEL[4:0] ? ¨¦ ?? ??
4)  ¡ä ?? ¨¤¡ä ? 2¨¦ ? ¨´ ? ¨® ?? ? ¨´1 | ? ¨¹
5)  ¨º ? 3 ? ¡Áa ?? ? ¨¢1 ? ADC_OUT_D[11¡êo0] ?? ? ¨´ ? ¦Ì

2.  DAC ¡ê¡§2 ?? ¡ê ?
1)  D_DAC_EN ? ¨¦ ?? ??
2)  D_DAC_SHEN ? ¨¦ ?? ??
3)  D_DAC_CODE[11¡êo0] ? ¨¦ ?? ??

3.  ACMP ¡ê¡§1 ?? ¡ê ?
1¡ê ?    D_ACMP_EN ? ¨¦ ?? ??
2¡ê ?    D_ACMP_PSEL[1:0] ? ¨¦ ?? ??
3¡ê ?    D_ACMP_NSEL[2¡êo0] ? ¨¦ ?? ??
4¡ê ?    ¨º ? 3 ? ¡À¨¨ ?? ? ¨¢1 ? ACMP_OUT_D

#endif

void adc_clk_sel(int adc_sel, int clk_val)
{
    if (clk_val < 0 || clk_val > 3)
    {
        return;
    }
    if (adc_sel == 0)
    {
        HW32_REG(0x4000009c) &= ~(3 << 0);
        HW32_REG(0x4000009c) |= (clk_val << 0); // ADC0 CLK select
    }
    else if (adc_sel == 1)
    {
        HW32_REG(0x4000009c) &= ~(3 << 2);
        HW32_REG(0x4000009c) |= (clk_val << 2); // ADC1 CLK select
    }
    else if (adc_sel == 2)
    {
        HW32_REG(0x4000009c) &= ~(3 << 4);
        HW32_REG(0x4000009c) |= (clk_val << 4); // ADC2 CLK select
    }
}

void adc_clk_div(int adc_sel, int clk_div_val)
{
    if (clk_div_val < 0 || clk_div_val > 15)
    {
        return;
    }
    if (adc_sel == 0)
    {
        HW32_REG(0x4000009c) &= ~(0xF << 8);
        HW32_REG(0x4000009c) |= (clk_div_val << 8); // ADC0 CLK div select
    }
    else if (adc_sel == 1)
    {
        HW32_REG(0x4000009c) &= ~(0xF << 24);
        HW32_REG(0x4000009c) |= (clk_div_val << 24); // ADC1 CLK div select
    }
    else if (adc_sel == 2)
    {
        HW32_REG(0x4000009c) &= ~(0xF << 28);
        HW32_REG(0x4000009c) |= (clk_div_val << 28); // ADC2 CLK div select
    }
}

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

static int adc_pinumx_config(struct device *port)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_uartPinMux[2] =
    {
        {PINMUX_GPIO_PIN_0, GPIOC, 0},
        {PINMUX_GPIO_PIN_1, GPIOC, 0},
    };

    pin_cfg.pin = BIT(s_uartPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    ret = pinmux_pin_set(port, s_uartPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_uartPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    ret = pinmux_pin_set(port, s_uartPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}

static Device_Pinmux_T s_pinmux[] =
{
    {PINMUX_GPIO_PIN_0,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_0 pinmux config paramter */
    {PINMUX_GPIO_PIN_1,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_1 pinmux config paramter */
    {PINMUX_GPIO_PIN_2,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_2 pinmux config paramter */
    {PINMUX_GPIO_PIN_7,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_3 pinmux config paramter */
    {PINMUX_GPIO_PIN_8,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_4 pinmux config paramter */
    {PINMUX_GPIO_PIN_9,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_5 pinmux config paramter */
    {PINMUX_GPIO_PIN_10,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_6 pinmux config paramter */
    {PINMUX_GPIO_PIN_11,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_7 pinmux config paramter */
    {PINMUX_GPIO_PIN_14,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_8 pinmux config paramter */
    {PINMUX_GPIO_PIN_15,  GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_9 pinmux config paramter */

    {PINMUX_GPIO_PIN_12, GPIOB, GPIO_NOPULL}, /* ADC_CHANNEL_10 pinmux config paramter */
    {PINMUX_GPIO_PIN_13, GPIOB, GPIO_NOPULL}, /* ADC_CHANNEL_11 pinmux config paramter */
    {PINMUX_GPIO_PIN_14, GPIOB, GPIO_NOPULL}, /* ADC_CHANNEL_12 pinmux config paramter */
    {PINMUX_GPIO_PIN_15, GPIOB, GPIO_NOPULL}, /* ADC_CHANNEL_13 pinmux config paramter */

    {PINMUX_GPIO_PIN_12, GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_14 pinmux config paramter */
    {PINMUX_GPIO_PIN_13, GPIOC, GPIO_NOPULL}, /* ADC_CHANNEL_15 pinmux config paramter */
};

static int ADC_ConfigPinmux(struct device *port)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;

    Device_Pinmux_T *pADCPinmux = &s_pinmux[ADC_CHANNEL_0];

    memset(&pin_cfg, 0, sizeof(pin_cfg));

    pin_cfg.pin = BIT(s_pinmux[ADC_CHANNEL_0].pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = 0;
    ret = pinmux_pin_set(port, s_pinmux[ADC_CHANNEL_0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return ret;
}


/*
1.Configure ADC_MODE = 1 and disable discontinuous mode
2.Sequence length/Channel selection/sample time/data resolution are all random(the min,middle and max value must be covered)
3.Triggre ADC to convert at least 1000 times
4.Check whether ADC can operate properly at following cases:
         a.Sequence length can be 0-16
         b.Channel selection can be 0-31
         c.ADC_ST bit can be 4'b0000 to 4'b1111
         d.Result resolution can be 8/10/12 bit
 The result = convertion counter + channel
*/

static void HAL_ADC_CLOCK_STATIC(int en)
{
    if (en == 0)
    {
        *(volatile unsigned int *)(0x40000080) &= ~(1 << 12); //disabled the ADC clock
    }
    else
    {
        *(volatile unsigned int *)(0x40000080) |= (1 << 12); //enable (release) the ADC clock, ADC1 clock
        *(volatile unsigned int *)(0x40000080) |= (1 << 13);
        *(volatile unsigned int *)(0x40000080) |= (1 << 20);
    }
}

void adc_basic_test(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;

    float adc_out = 0;
    const float vrefh = 3.0035;
    const float vr = 0.018;
    const float vi = 0.02;
    float vin = 0;// (adc_out) * (vrefh-vr) /( (1<<12) - 1)+ vi;
    float adc_out1 = 0;//((1<<12) -1)/ (1/( (adc_out/((1<<12)-1))+vi/(vrefh-vr))+vr/vin);

    udma_clk_enable();
    udma_reset_release();
    gpio_clk_enable();
    gpio_reset_release();
    patch_config();

    //HAL_ADC_CLOCK(0);
    HAL_ADC_CLOCK_STATIC(1);
    HAL_ADC_RESET();

    adc_clk_sel(0, 0);
    adc_clk_sel(0, 0);
    adc_clk_sel(0, 0);

    adc_clk_div(0, 7);
    adc_clk_div(0, 7);
    adc_clk_div(0, 7);

    //adc_clk_sel(1, 0);    //ADC1
    //adc_clk_sel(1, 0);
    //adc_clk_sel(1, 0);

    //adc_clk_div(1, 7);
    //adc_clk_div(1, 7);
    //adc_clk_div(1, 7);

    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);

    adc_pinumx_config(portc);


    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= ADC_CTRL_SINGLE_MODE | ADC_CTRL_EN;
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/

    pADC->RCHN0 = 0x0;    /*Channel selection*/
    pADC->RCHN1 = 0;

    pADC->RCHN2 = 0;

    //pADC1->RCHN2 |= (0x0 << 28); /*Sequence length*/

    pADC1->RCHN0 = 0x1;   /*Channel selection*/
    pADC1->RCHN1 = 0;

    pADC1->RCHN2 = 0;   //ADC1 & ADC2

    pComDev->EXCHEN = 1 | (1 << 1); //ADC1 & ADC2

    static int cnt = 0;
    static float  save_val = 0;
    int  value = 0;

    HAL_ADC_Sample_Time(pADC);
    //HAL_ADC_Sample_Time(pADC1);    //ADC1 & ADC2

    while (1)
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_NONE);
        HAL_ADC_Triger(pADC1, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_NONE);
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            value = pADC->DRST;
            save_val += value;
        }
        // PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, value);
        cnt++;

        if (cnt >= 8)
        {

            adc_out =  save_val / 8;
            vin = (adc_out) * (vrefh - vr) / ((1 << 12) - 1) + vi;


            // PR_DEBUG("adc vin=[%f] \n", vin);
            adc_out1 = ((1 << 12) - 1) / (1 / ((adc_out / ((1 << 12) - 1)) + vi / (vrefh - vr)) + vr / vin);
            PR_DEBUG("adc adc_out=[%f],adc_out1=[%f] \n", adc_out, adc_out1);
            cnt = 0;
            save_val = 0;
        }
        k_sleep(20);



        pADC->ST = ADC_ST_EOCE;
        while (pADC->ST & ADC_ST_EOCE == ADC_ST_EOCE);

        i++;
    }
    PR_DEBUG("ADC_SingleModeNbitTest end\n");

}


void main(void)
{
    int m = 0;

    ADC_Device_T *pDevice0 = ADC_DEV0;
    ADC_Device_T *pDevice = ADC_DEV0;
    ADC_Device_T *pDevice2 = ADC_DEV1;
    ADC_Device_T *pDevice3 = ADC_DEV2;

#if 0//def TEST_CASE_APP
    //adc_demo();
    //adc_basic_test(pDevice0,  pDevice2);     //ADC1 & ADC2
    adc_basic_test(pDevice0);
    while (1);
#endif



    udma_clk_enable();
    udma_reset_release();
    gpio_clk_enable();
    gpio_reset_release();
    patch_config();

    //HAL_ADC_CLOCK(0);
    HAL_ADC_CLOCK(1);
    HAL_ADC_RESET();

    adc_clk_sel(1, 0);
    adc_clk_sel(0, 0);
    adc_clk_sel(2, 0);

    adc_clk_div(0, 0);
    adc_clk_div(1, 0);
    adc_clk_div(2, 0);
    /** Configure pinmux for an available UART device */
    struct device *porte = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (porte != NULL)
    {
        adc_pinumx_config(porte);
    }

    //adc_basic_test(ADC3);
    //while(1);

#if defined(TEST_CASE1)
    ADC_BusyTest(pDevice);
    return 0;
#endif

#if defined(TEST_CASE2)
    ADC_DfifoEmptyTest(pDevice3);
#endif

#if defined(TEST_CASE3)
    ADC_EocIntTest(pDevice);
#endif

#if defined(TEST_CASE4)
    ADC_EosIntTest(pDevice);
#endif

#if defined(TEST_CASE5)
    ADC_JEocIntTest(pDevice);
#endif

#if defined(TEST_CASE6)
    ADC_JEosIntTest(pDevice);
#endif

#if defined(TEST_CASE7)
    ADC_DfifoFullIntTest(pDevice);
#endif

#if defined(TEST_CASE8)
    ADC_DfifoPrefIntTest(pDevice);
#endif

#if defined(TEST_CASE9)
    ADC_DfifoOVFIntTest(pDevice);
#endif

#if defined(TEST_CASE10)
    ADC_CompMatIntTest(pDevice3);
#endif

#if defined(TEST_CASE11)
    ADC_SingleModeNbitTest(pDevice3);
#endif
#if defined(TEST_CASE12)
    ADC_RegularRepeatModeNbitTest(pDevice);
#endif

#if defined(TEST_CASE13)
    ADC_RegularDiscontinuousModeTest(pDevice);
#endif

#if defined(TEST_CASE16)
    ADC_LOWPOWERTest(pDevice);
#endif

#if defined(TEST_CASE17)
    ADC_OverSampleAVRTest(pDevice);
#endif

#if defined(TEST_CASE18)
    ADC_TriggerOnceTest(pDevice);
#endif

#if defined(TEST_CASE19)
    ADC_InjectedSequenceModeTest(pDevice);
#endif

#if defined(TEST_CASE20)
    ADC_InjectedNoSequenceModeTest(pDevice);
#endif

#if defined(TEST_CASE21)
    ADC_InjectedAutoModeTest(pDevice);
#endif

#if defined(TEST_CASE22)
    ADC_InjectedIntReguModeTest(pDevice3);
#endif

#if defined(TEST_CASE23)
    ADC_InjectedPriorityTest(pDevice);
#endif

#if defined(TEST_CASE24)
    ADC_InjectedConvReguTrigModeTest(pDevice);
#endif


#if defined(TEST_CASE25)
    ADC_AutoCompareModeTest(pDevice);
#endif

#if defined(TEST_CASE26)
    ADC_AutoCompareOneChanTest(pDevice);
#endif

#if defined(TEST_CASE27)
    ADC_ChanWiseTest(pDevice);
#endif

#if defined(TEST_CASE28)
    ADC_OutputEventToPESTest(pDevice);
#endif

#if defined(TEST_CASE29)
    ADC_InjectedSimultaneousTest(pDevice);
#endif

#if defined(TEST_CASE30)
    ADC_REGUInjectedSimultaneousTest(pDevice);
#endif

#if defined(TEST_CASE31)
    ADC_InjectedSimultaneousPrioTest(pDevice);
#endif

#if defined(TEST_CASE32)
    ADC3_InjectedSimultaneousTest(pDevice);
#endif

#if defined(TEST_CASE33)
    ADC_RegularSimultaneousTest(pDevice);
#endif

#if defined(TEST_CASE34)
    ADC_RegularSimultaneousPrioTest(pDevice);
#endif

#if defined(TEST_CASE35)
    ADC3_RegularSimultaneousTest(pDevice);
#endif

#if defined(TEST_CASE36)
    ADC_RegularInterleavedRepeatTest(pDevice);
#endif

#if defined(TEST_CASE36_1)
    ADC_RegularInterleavedTest(pDevice);
#endif


#if defined(TEST_CASE37)
    ADC_RegularInterleavedPrioTest(pDevice);
#endif

#if defined(TEST_CASE38)
    ADC3_InInterleavedModeTest(pDevice);
#endif

#if defined(TEST_CASE39)
    ADC_InjectAlternateTest(pDevice);
#endif

#if defined(TEST_CASE40)

    ADC_RegConvInInjectAlternateTrigTest(pDevice); //FAILED
#endif


#if defined(TEST_CASE41)
    ADC_InjectAlternateTrigPrioTest(pDevice);
#endif

#if defined(TEST_CASE42)
    ADC3_InInJectAltTriModeTest(pDevice);
#endif


#if defined(TEST_CASE43)
    ADC_CombReguSimuInjectSimuTest(pDevice);
#endif

#if defined(TEST_CASE44)
    ADC_CombReguSimuInjectSimuPrioTest(pDevice);
#endif

#if defined(TEST_CASE45)
    ADC3_CombReguSimuInjectSimuModeTest(pDevice);
#endif

#if defined(TEST_CASE46)
    ADC_CombReguSimuInjectAltModeTest(pDevice);
#endif


#if defined(TEST_CASE47)
    ADC_CombReguSimuInjectAltSimuPrioModeTest(pDevice);
#endif


#if defined(TEST_CASE48)
    ADC3_CombReguSimuInjectAltModeTest(pDevice);
#endif


#if defined(TEST_CASE49)
    ADC_InjectedSimultaneousTripleTest(pDevice);
#endif



#if defined(TEST_CASE50)
    ADC_REGUInInjectedSimultaneousTripleTest(pDevice);
#endif


#if defined(TEST_CASE51)
    ADC_InjectedSimultaneousPrioTripleTest(pDevice);
#endif


#if defined(TEST_CASE52)

    ADC_RegularSimultaneousTripleTest(pDevice);
#endif

#if defined(TEST_CASE53)
    ADC_RegularSimultaneousPrioTripleTest(pDevice);
#endif

#if defined(TEST_CASE54)
    ADC_RegularInterleavedTripleTest(pDevice);
#endif

#if defined(TEST_CASE55)
    ADC_RegularInterleavedPrioTripleTest(pDevice);
#endif


#if defined(TEST_CASE56)
    ADC_InjectAlternateTripleTest(pDevice);
#endif

#if defined(TEST_CASE57)
    ADC_RegConvInjectAlternateTrigTripleTest(pDevice);
#endif

#if defined(TEST_CASE59)
    ADC_InjectAlternateTrigPrioTripleTest(pDevice);
#endif

#if defined(TEST_CASE60)
    ADC_CombReguSimuInjectSimuTripleTest(pDevice);
#endif

#if defined(TEST_CASE61)
    ADC_CombReguSimuInjectSimuPrioTripleTest(pDevice);
#endif


#if defined(TEST_CASE62)
    ADC_CombReguSimuInjectAltModeTripleTest(pDevice);
#endif

#if defined(TEST_CASE63)
    ADC_CombReguSimuInjectAltSimuPrioTripleModeTest(pDevice0);
#endif



#if 0 //defined(TEST_CASE64)
    ADC_SingleModeNbitDemo(pDevice);
#endif

#ifdef TEST_CASE_68_PES_TO_ADC
    pes_to_adc_demo();
    while (1);
#endif


    //ADC_RegularRepeatModeNbitTest(ADC1);
    //return;

    if (adc_demo() != 0)
    {
        printk("adc test FAILED !!!\r\n");
    }
    else
    {
        printk("adc test PASS !!!\r\n");
    }

    while (1)
    {
        printk("SRAM BringUp:Hello World! %s\n", CONFIG_ARCH);
        k_sleep(1000);
    }
}
