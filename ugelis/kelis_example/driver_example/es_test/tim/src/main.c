/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          Main.c
 *
 * @author        edward.zhu
 *
 * @version       1.1.0
 *
 * @date          2018/03/26
 *
 * @brief         FPGA test code for TIM module.
 *
 * @note
 *    2018/03/26, edward.zhu, V1.0.0
 *        Initial version.
 *
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <uart.h>
#include <misc/printk.h>
#include <gm_hal_clock.h>
#include <gm_hal_tim.h>
//#include <gm_hal_pes.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
/* TIM update event test */
//#define TEST_CASE_1

/* TIM auto-reload preload test */
//#define TEST_CASE_2

/* TIM compare test */
//#define TEST_CASE_3

/* TIM output test */
#define TEST_CASE_4

/* TIM one-pulse mode test */
//#define TEST_CASE_5

/* TIM dead-time test */
//#define TEST_CASE_6

/* TIM brake test */
//#define TEST_CASE_7

/* TIM input test */
//#define TEST_CASE_8

/* TIM capture test */
//#define TEST_CASE_9

/* TIM prescaler test */
//#define TEST_CASE_10

/* TIM slaver mode test */
//#define TEST_CASE_11

/* TIM commutation event test */
//#define TEST_CASE_12

/* TIM dmaRead test*/
//#define TEST_CASE_13_1

/* TIM dmaWrite test*/
//#define TEST_CASE_13_2

/* TIM quadrature encoder test */
//#define TEST_CASE_14

/* TIM external clock mode 2 test */
//#define TEST_CASE_15

/* TIM external event clear ATM_OCxREF test */
//#define TEST_CASE_16

/* TIM spwm test */
//#define TEST_CASE_17

/* Hall sensor test(TIM3 as interface timer, TIM1 as advanced timer) */
//#define TEST_CASE_18

/* TIM3 used as TIM1 prescaler */
//#define TEST_CASE_19

/* TIM2 used to gate TIM1 */
//#define TEST_CASE_20

/* TIM2 used to enable TIM1 */
//#define TEST_CASE_21

/* TIM1 as slave timer connecting master timer test */
//#define TEST_CASE_22

/* TIM2 as slave timer connecting master timer test */
//#define TEST_CASE_23

/* TIM3 as slave timer connecting master timer test */
//#define TEST_CASE_24

/* TIM4 as slave timer connecting master timer test */
//#define TEST_CASE_25

/* TIM5 as slave timer connecting master timer test */
//#define TEST_CASE_26

/* TIM8 as slave timer connecting master timer test */
//#define TEST_CASE_27

/* TIM9 as slave timer connecting master timer test */
//#define TEST_CASE_28

/* TIM12 as slave timer connecting master timer test */
//#define TEST_CASE_29

//#define TIM_TRG_RESET

/* TIM random test 1 */
//#define TEST_CASE_30

/* TIM random test 2*/
//#define TEST_CASE_31

/* TIM random test 3*/
//#define TEST_CASE_32

/*TIM Trigger PES TEST*/
//#define TEST_CASE_33

/*PES Trigger TIM TEST*/
//#define TEST_CASE_34

/*BaseTimer 1 function test*/
//#define TEST_CASE_35

/*BaseTimer 2 functuon test*/
//#define TEST_CASE_36

/*BaseTimer 2 trigger event to PES*/
//#define TEST_CASE_37

#if (defined TEST_CASE_5 || defined TEST_CASE_17 || defined TEST_CASE_30 ||defined TEST_CASE_31)
    #ifdef TEST_CASE_5
        #define PERIOD_VALUE     60000
    #else
        #define PERIOD_VALUE      7500
    #endif
    #define  PULSE1_VALUE     800
    #define  PULSE2_VALUE     600
    #define  PULSE3_VALUE     300
    #define  PULSE4_VALUE     300
#elif (defined TEST_CASE_14 || defined TEST_CASE_18)
    #define PERIOD_VALUE     1800
    #define PULSE1_VALUE      900
    #define PULSE2_VALUE      900
#elif (defined TEST_CASE_19 || defined TEST_CASE_20 || defined TEST_CASE_21)
    #define PERIOD_VALUE     0xFF
    #define PULSE1_VALUE      0x3F
    #define PULSE2_VALUE      0x3F
    #define PULSE3_VALUE      0x3F
    #define PULSE4_VALUE      0x3F
#else
    #define  PERIOD_VALUE       0xFFFF        /* Period Value  */
    #define  PULSE1_VALUE       0x7FFF        /* Capture Compare 1 Value  */
    #define  PULSE2_VALUE       0x3FFF        /* Capture Compare 2 Value  */
    #define  PULSE3_VALUE       0xFFF         /* Capture Compare 3 Value  */
    #define  PULSE4_VALUE       0x7FF         /* Capture Compare 4 Value  */
#endif

#define USE_FULL_ASSERT    1

/******************************************************************************
* Local types
******************************************************************************/

/*******************************************************************************
* Global variables
*******************************************************************************/
/* TIM handle declaration */
TIM_Handle_T TimeHandle;

/* DMA handle declaration */
DMA_Handle_T dmaHandle;

/* Captured Value */
uint32_t uwIC2Value = 0;   /* TIM_PWMInput */
/* Duty Cycle Value */
uint32_t uwDutyCycle = 0;  /* TIM_PWMInput */
/* Frequency Value */
uint32_t uwFrequency = 0;  /* TIM_PWMInput */

/* Captured Values */
uint32_t uwIC2Value1 = 0;       /* TIM_InputCapture */
uint32_t uwIC2Value2 = 0;       /* TIM_InputCapture */
uint32_t uwDiffCapture = 0;     /* TIM_InputCapture */
/* Capture index */
uint16_t uhCaptureIndex = 0;    /* TIM_InputCapture */

/* DMA Read buffer */
uint32_t arrayRead[] = {0, 0, 0, 0};

/* DMA Write buffer */
uint32_t arrayWrite[] = {0xFFF, 0x7FF, 0x3FF, 0xFF};

uint32_t dma_done ;
int ret;

/*******************************************************************************
* Static variables
*******************************************************************************/
static void utim_hirc_calibrate(void);
static void utim_reset_release(void);
static void ubtm_reset_relaese(void);
static void utim_clk_enable(void);
static void ubtm_clk_enable(void);
void ErrorHandler(void);

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static void utim_hirc_calibrate(void)
{
    *(volatile uint32_t *)(0x1ffff0a8) = (0xfffffbd7);   //LQFP176, ID=21, hirc calibrate.
    *(volatile uint32_t *)(0x1ffff0ac) = (0xff0dffff);
    *(volatile uint32_t *)(0x1ffff0b0) = (0xff3fffff);
}

#if 0
static void utim_reset_release(void)
{
    *(volatile uint32_t *)(0x40000078) |= (0x0fff0000);     //TIM_SW_RSTJ
}

static void utim_clk_enable(void)
{
    *(volatile uint32_t *)(0x40000088) |= (0x0fff0000);     //TIM_CLK_EN
}

static void ubtm_reset_release(void)
{
    *(volatile uint32_t *)(0x40000074) |= (0x00030000);     //BTM_SW_RSTJ
}

static void ubtm_clk_enable(void)
{
    *(volatile uint32_t *)(0x40000084) |= (0x00030000);     //BTM_CLK_EN
}

static RET_CODE PINMUX_Config(void)
{
    /* TIM1 PINMUX Setting */
    /* TIM1_CH1  XPE0 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00000001);
    *(volatile uint32_t *)(0x40000310) |= (0x00000001);

    /* TIM1_CH1N  XPD15 */
    *(volatile uint32_t *)(0x400002D0) |= (0x40000000);
    *(volatile uint32_t *)(0x4000030C) |= (0x10000000);

    /* TIM1_CH2  XPE2 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00000010);
    *(volatile uint32_t *)(0x40000310) |= (0x00000100);

    /* TIM1_CH2N  XPE1 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00000004);
    *(volatile uint32_t *)(0x40000310) |= (0x00000010);

    /* TIM1_CH3  XPE4 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00000100);
    *(volatile uint32_t *)(0x40000310) |= (0x00010000);

    /* TIM1_CH3N  XPE3 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00000040);
    *(volatile uint32_t *)(0x40000310) |= (0x00001000);

    /* TIM1_CH4  XPE5 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00000400);
    *(volatile uint32_t *)(0x40000310) |= (0x00100000);

    /* TIM1_ETR  XPD14 */
    *(volatile uint32_t *)(0x400002D0) |= (0x10000000);
    *(volatile uint32_t *)(0x4000030C) |= (0x01000000);

    /* TIM1_BKIN  XPE6 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00001000);
    *(volatile uint32_t *)(0x40000310) |= (0x01000000);



    /* TIM2 PINMUX Setting */
    /* TIM2_CH1  XPC0 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00000001);
    *(volatile uint32_t *)(0x40000300) |= (0x00000001);

    /* TIM2_CH2  XPC1 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00000004);
    *(volatile uint32_t *)(0x40000300) |= (0x00000010);

    /* TIM2_CH3  XPC2 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00000010);
    *(volatile uint32_t *)(0x40000300) |= (0x00000100);

    /* TIM2_CH4  XPC7 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00004000);
    *(volatile uint32_t *)(0x40000300) |= (0x10000000);

    /* TIM2_ETR  XPC9 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00040000);
    *(volatile uint32_t *)(0x40000304) |= (0x00000020);



    /* TIM3 PINMUX Setting */
    /* TIM3_CH1  XPC10 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00100000);
    *(volatile uint32_t *)(0x40000304) |= (0x00000200);

    /* TIM3_CH2  XPC11 */
    *(volatile uint32_t *)(0x400002CC) |= (0x00400000);
    *(volatile uint32_t *)(0x40000304) |= (0x00400000);

    /* TIM3_CH3  XPC14 */
    *(volatile uint32_t *)(0x400002CC) |= (0x10000000);
    *(volatile uint32_t *)(0x40000304) |= (0x02000000);

    /* TIM3_CH4  XPC15 */
    *(volatile uint32_t *)(0x400002CC) |= (0x40000000);
    *(volatile uint32_t *)(0x40000304) |= (0x40000000);

    /* TIM3_ETR XPI5 */
    *(volatile uint32_t *)(0x400002E4) |= (0x00000400);
    *(volatile uint32_t *)(0x40000330) |= (0x00200000);



    /* TIM4 PINMUX Setting */
    /* TIM4_CH1  XPF9 */
    *(volatile uint32_t *)(0x400002D8) |= (0x00040000);
    *(volatile uint32_t *)(0x4000031C) |= (0x00000020);

    /* TIM4_CH2  XPF10 */
    *(volatile uint32_t *)(0x400002D8) |= (0x00100000);
    *(volatile uint32_t *)(0x4000031C) |= (0x00000200);

    /* TIM4_CH3  XPF11 */
    *(volatile uint32_t *)(0x400002D8) |= (0x00400000);
    *(volatile uint32_t *)(0x4000031C) |= (0x00002000);

    /* TIM4_CH4  XPF12 */
    *(volatile uint32_t *)(0x400002D8) |= (0x01000000);
    *(volatile uint32_t *)(0x4000031C) |= (0x00020000);

    /* TIM4_ETR  XPK3 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00000040);
    *(volatile uint32_t *)(0x40000340) |= (0x00002000);



    /* TIM5 PINMUX Setting */
    /* TIM5_CH1  XPE14 */
    *(volatile uint32_t *)(0x400002D4) |= (0x10000000);
    *(volatile uint32_t *)(0x40000314) |= (0x02000000);

    /* TIM5_CH2  XPE15 */
    *(volatile uint32_t *)(0x400002D4) |= (0x40000000);
    *(volatile uint32_t *)(0x40000314) |= (0x20000000);

    /* TIM5_CH3  XPF0 */
    *(volatile uint32_t *)(0x400002D8) |= (0x00000001);
    *(volatile uint32_t *)(0x40000318) |= (0x00000002);

    /* TIM5_CH4  XPH10 */
    *(volatile uint32_t *)(0x400002E0) |= (0x00100000);
    *(volatile uint32_t *)(0x4000032C) |= (0x00000200);



    /* TIM8 PINMUX Setting */
    /* TIM8_CH1  XPK6 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00001000);
    *(volatile uint32_t *)(0x40000340) |= (0x03000000);

    /* TIM8_CH1N  XPH7 */
    *(volatile uint32_t *)(0x400002E0) |= (0x00004000);
    *(volatile uint32_t *)(0x40000328) |= (0x30000000);

    /* TIM8_CH2  XPK7 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00004000);
    *(volatile uint32_t *)(0x40000340) |= (0x30000000);

    /* TIM8_CH2N  XPH8 */
    *(volatile uint32_t *)(0x400002E0) |= (0x00010000);
    *(volatile uint32_t *)(0x4000032C) |= (0x00000003);

    /* TIM8_CH3  XPK8 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00010000);
    *(volatile uint32_t *)(0x40000344) |= (0x00000003);

    /* TIM8_CH3N  XPH9 */
    *(volatile uint32_t *)(0x400002E0) |= (0x00040000);
    *(volatile uint32_t *)(0x4000032C) |= (0x00000030);

    /* TIM8_CH4  XPH0 */
    *(volatile uint32_t *)(0x400002E0) |= (0x00001000);
    *(volatile uint32_t *)(0x40000328) |= (0x00000003);

    /* TIM8_ETR  XPH13 */
    *(volatile uint32_t *)(0x400002E0) |= (0x04000000);
    *(volatile uint32_t *)(0x4000032C) |= (0x00300000);

    /* TIM8_BKIN  XPK5 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00000400);
    *(volatile uint32_t *)(0x40000340) |= (0x00300000);


    /* TIM9 PINMUX Setting */
    /* TIM9_CH1 XPA3 */
    *(volatile uint32_t *)(0x400002C4) |= (0x00000040);
    *(volatile uint32_t *)(0x400002F0) |= (0x00003000);

    /* TIM9_CH2 XPA4 */
    *(volatile uint32_t *)(0x400002C4) |= (0x00000100);
    *(volatile uint32_t *)(0x400002F0) |= (0x00030000);



    /* TIM10 PINMUX Setting */
    /* TIM10_CH1 XPK1 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00000004);
    *(volatile uint32_t *)(0x40000340) |= (0x00000030);


    /* TIM11 PINMUX Setting */
    /* TIM11_CH1 XPK2 */
    *(volatile uint32_t *)(0x400002EC) |= (0x00000010);
    *(volatile uint32_t *)(0x40000340) |= (0x00000300);


    /* TIM12 PINMUX Setting */
    /* TIM12_CH1 XPE10 */
    *(volatile uint32_t *)(0x400002D4) |= (0x00100000);
    *(volatile uint32_t *)(0x40000314) |= (0x00000700);

    /* TIM12_CH2 XPE13 */
    *(volatile uint32_t *)(0x400002D4) |= (0x04000000);
    *(volatile uint32_t *)(0x40000314) |= (0x00700000);

    /* TIM13 PINMUX Setting */
    /* TIM13_CH1 XPB7 */
    *(volatile uint32_t *)(0x400002C8) |= (0x00004000);
    *(volatile uint32_t *)(0x400002F8) |= (0x70000000);


    /* TIM14 PINMUX Setting */
    /* TIM14_CH1 XPB8 */
    *(volatile uint32_t *)(0x400002C8) |= (0x00010000);
    *(volatile uint32_t *)(0x400002FC) |= (0x00000007);

    return RET_OK;
}
#endif

void HAL_TIM_ICCaptureCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    if (htim->channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if (uhCaptureIndex == 0)
        {
            /* Get the 1st Input Capture value */
            uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIMx, TIM_CHANNEL_2);
            uhCaptureIndex = 1;
        }
        else if (uhCaptureIndex == 1)
        {
            /* Get the 2nd Input Capture value */
            uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIMx, TIM_CHANNEL_2);

            /* Capture computation */
            if (uwIC2Value2 > uwIC2Value1)
            {
                uwDiffCapture = (uwIC2Value2 - uwIC2Value1);
            }
            else if (uwIC2Value2 < uwIC2Value1)
            {
                uwDiffCapture = ((0xFFFF - uwIC2Value1) + uwIC2Value2) + 1;
            }
            else
            {
                uwDiffCapture = 0;
            }

            uwFrequency = SYS_CLOCK / (uwDiffCapture + 1);
            uhCaptureIndex = 0;

        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    static uint16_t dutyVal = 100;
    TIMx->CCR1 = dutyVal;
    dutyVal += 100;

    if (dutyVal >= 7500)
    {
        dutyVal = 200;
    }

    TIMx_SetPWMDuty(TIMx, TRUE, dutyVal);

    TIMx->ARR = dutyVal << 1;

}


void HAL_TIM_CommutationCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
#ifdef TEST_CASE_12
    LL_TIM_Write_32BitsAddr(0xff, 0x4000702c);
    LL_TIM_Write_32BitsAddr(0x3f, 0x40007034);
    LL_TIM_ClearComFlag(TIM1);
#else (defined TEST_CASE_18)
    LL_TIM_CC1E_DISABLE(TIM1);
    LL_TIM_CC2E_DISABLE(TIM1);
    LL_TIM_CC3E_ENABLE(TIM1);
#endif
}


#if (defined TEST_CASE_1 || defined TEST_CASE_2)
static RET_CODE TIM_UpdateAutoReloadFunc(TIM_Device_T *TIMx)
{
#ifdef TEST_CASE_1
    TIM_SlaveConfig_T sSlaveConfig;
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));
#endif

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    TimeHandle.init.repetitionCounter = 0;

    if (HAL_TIM_InitTimeBase(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_1
    sSlaveConfig.slaveMode = TIM_SLAVEMODE_RESET;
    sSlaveConfig.inputTrigger = TIM_TS_TI1FP1;
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIMx, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    /*Enable auto-reload preload mode*/
    LL_TIM_ARPE_ENABLE(TIMx);
#else
    /*Enable auto-reload preload mode*/
    //LL_TIM_ARPE_ENABLE(TIMx);
#endif

    LL_TIM_CNT_ENABLE(TIMx);

    return HAL_OK;
}
#endif

#ifdef TEST_CASE_3
static RET_CODE TIM_CompareFunc(TIM_Device_T *TIMx, uint32_t ocmode)
{
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitOutputCompare(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = ocmode;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_LOW;//TIM_OCPOLARITY_HIGH;
    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE2_VALUE;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE3_VALUE;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE4_VALUE;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_4) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_4) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#if (defined TEST_CASE_4 || defined TEST_CASE_5 || defined TEST_CASE_6 || defined TEST_CASE_8 || defined TEST_CASE_16 \
    || defined TEST_CASE_9 || defined TEST_CASE_10 || defined TEST_CASE_15 || defined TEST_CASE_32 || defined TEST_CASE_33)
static RET_CODE TIM_PWMOutput(TIM_Device_T *TIMx)
{
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

#ifdef TEST_CASE_6
    TIM_BreakDeadTimeConfig_T sBDTConfig;
    memset(&sBDTConfig, 0, sizeof(TIM_BreakDeadTimeConfig_T));
#endif

    TimeHandle.instance = TIMx;
#if defined(TEST_CASE_10)
    TimeHandle.init.prescaler = 3;//5;
#else
    TimeHandle.init.prescaler = 0;
#endif

#ifdef TEST_CASE_32
    TimeHandle.init.period = 1600;
#elif (defined TEST_CASE_16)
    TimeHandle.init.period = 0xFFF;
#else
    TimeHandle.init.period = PERIOD_VALUE;
#endif
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {

        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    //sOCConfig.pulse = 0x80;
#ifdef TEST_CASE_16
    sOCConfig.pulse = 0x7FF;
#else
    sOCConfig.pulse = PULSE1_VALUE;
#endif
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE2_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE3_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE4_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_4) != HAL_OK)
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_6
    /* Set the deadtime value */
    sBDTConfig.deadTime = DT_DELAY_4;
    if (HAL_TIM_ConfigBreakDeadTime(&TimeHandle, TIMx, &sBDTConfig) != HAL_OK)
    {
        ErrorHandler();
    }
#endif

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_4) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#ifdef TEST_CASE_5
static RET_CODE TIM_OnePulse(TIM_Device_T *TIMx)
{
    TIM_OnePulseInit_T sOPMConfig;
    memset(&sOPMConfig, 0, sizeof(TIM_OnePulseInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;
    TimeHandle.init.repetitionCounter = 3;

    if (HAL_TIM_InitOnePulse(&TimeHandle, TIMx, TIM_OPMODE_SINGLE) != HAL_OK)
    {
        ErrorHandler();
    }

    sOPMConfig.ocMode = TIM_OCMODE_PWM1;
    sOPMConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOPMConfig.pulse = 600;
    sOPMConfig.icPolarity = TIM_ICPOLARITY_RISING;
    sOPMConfig.icSelection = TIM_ICSELECTION_DIRECTTI;
    sOPMConfig.icFilter = 20;

    if (HAL_TIM_ConfigOPMChannel(&TimeHandle, TIMx, &sOPMConfig, TIM_CHANNEL_1, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOnePulse(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    LL_TIM_CNT_ENABLE(TIMx);


    return HAL_OK;
}
#endif

#ifdef TEST_CASE_7
static RET_CODE TIM_BrakeFunc(TIM_Device_T *TIMx, uint32_t bkp)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_BreakDeadTimeConfig_T sBDTConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sBDTConfig, 0, sizeof(TIM_BreakDeadTimeConfig_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sBDTConfig.breakState = TIM_BREAK_ENABLE;
    sBDTConfig.breakPolarity = bkp;
    sBDTConfig.automaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;//TIM_AUTOMATICOUTPUT_DISABLE;
    sBDTConfig.lockLevel = TIM_LOCKLEVEL_OFF;
    sBDTConfig.deadTime = 0xd2;

    /* Set the deadtime value */
    if (HAL_TIM_ConfigBreakDeadTime(&TimeHandle, TIMx, &sBDTConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#ifdef TEST_CASE_8
static RET_CODE TIM_InputTest(TIM_Device_T *TIMx)
{
    TIM_InputCaptureInit_T sICConfig;
    memset(&sICConfig, 0, sizeof(TIM_InputCaptureInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.clockDivision = 2;
    if (HAL_TIM_InitInputCapture(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sICConfig.icPolarity  = TIM_ICPOLARITY_RISING;
    sICConfig.icSelection = TIM_ICSELECTION_DIRECTTI;
    sICConfig.icPrescaler = TIM_ICPSC_DIV2;
    sICConfig.icFilter    = 2;
    if (HAL_TIM_ConfigICChannel(&TimeHandle, TIMx, &sICConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartInputCapture(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}

#endif

#if (defined TEST_CASE_9 || defined TEST_CASE_31 || defined TEST_CASE_32 || defined TEST_CASE_34)
static RET_CODE TIM_InputCapture(TIM_Device_T *TIMx)
{
    TIM_InputCaptureInit_T sICConfig;
    memset(&sICConfig, 0, sizeof(TIM_InputCaptureInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitInputCapture(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_34
    //*(volatile uint32_t *)(0x40007058) |= (0x11);
    LL_TIM_Write_32BitsAddr(0x10, 0X40007058);
#endif

#ifdef TEST_CASE_32
    LL_TIM_Write_32BitsAddr(0x200, 0X40007000);
#endif

    sICConfig.icPolarity  = TIM_ICPOLARITY_RISING;
    sICConfig.icSelection = TIM_ICSELECTION_DIRECTTI;
    sICConfig.icPrescaler = TIM_ICPSC_DIV1;

#ifdef TEST_CASE_32
    sICConfig.icFilter = 0x7;
#else
    sICConfig.icFilter = 0;
#endif

    if (HAL_TIM_ConfigICChannel(&TimeHandle, TIMx, &sICConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartInputCapture(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#ifdef TEST_CASE_11
static RET_CODE TIM_SlaveModeFunc(TIM_Device_T *TIMx, uint32_t slavemode)
{
    TIM_SlaveConfig_T sSlaveConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    /* TIM Slave configuration Initialization */
    sSlaveConfig.slaveMode = slavemode;
    sSlaveConfig.inputTrigger = TIM_TS_ITR2;//TIM_TS_ETRF;
    //sSlaveConfig.inputTrigger = TIM_TS_ITR0;//TIM_TS_ITR1;//TIM_TS_ITR2;//TIM_TS_ITR3;
    //sSlaveConfig.inputTrigger = TIM_TS_TI1F_ED;//TIM_TS_TI1FP1;//TIM_TS_TI2FP2;
    sSlaveConfig.triggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
    sSlaveConfig.triggerPolarity = TIM_TRIGGERPOLARITY_INVERTED;//TIM_TRIGGERPOLARITY_NONINVERTED;
    //sSlaveConfig.triggerPolarity = TIM_TRIGGERPOLARITY_RISING;//TIM_TRIGGERPOLARITY_FALLING;//TIM_TRIGGERPOLARITY_BOTHEDGE;
    sSlaveConfig.triggerFilter = 0;//20;

    /* Configures the TIM in Slave mode */
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIMx, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    LL_TIM_CNT_DISABLE(TIMx);

    return HAL_OK;
}
#endif

#if (defined TEST_CASE_11 || defined TEST_CASE_12 || defined TEST_CASE_17 \
    || defined TEST_CASE_30 || defined TEST_CASE_31)
static RET_CODE TIM3_AsMasterTimer(void)
{
    TIM_MasterConfig_T sMasterConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIM3;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM3, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
#ifdef TEST_CASE_11
    sMasterConfig.masterOutputTrigger = TIM_TRGO_ENABLE;
#else
    sMasterConfig.masterOutputTrigger = TIM_TRGO_UPDATE;
#endif
    //sMasterConfig.masterOutputTrigger = TIM_TRGO_OC1REF;
    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIM3, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }
#ifdef TEST_CASE_11
#else
    if (HAL_TIM_StartPWM(&TimeHandle, TIM3, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }
#endif
    return HAL_OK;
}

static RET_CODE TIM_ComSpwmFunc(TIM_Device_T *TIMx)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_SlaveConfig_T sSlaveConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));
#ifdef TEST_CASE_30
    TIM_BreakDeadTimeConfig_T sBDTConfig;
    memset(&sBDTConfig, 0, sizeof(TIM_BreakDeadTimeConfig_T));
#endif

    TimeHandle.instance = TIMx;
#ifdef TEST_CASE_30
    TimeHandle.init.prescaler = 0xFFFF;
#else
    TimeHandle.init.prescaler = 0;
#endif
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    /* Enable auto-reload preload mode */
    LL_TIM_ARPE_ENABLE(TIMx);

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE2_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE3_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_30
    /* Set the deadtime value */
    sBDTConfig.breakState = TIM_BREAK_ENABLE;
    sBDTConfig.breakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBDTConfig.automaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;//TIM_AUTOMATICOUTPUT_DISABLE;
    sBDTConfig.lockLevel = TIM_LOCKLEVEL_OFF;
    sBDTConfig.deadTime = 0xFF;
    if (HAL_TIM_ConfigBreakDeadTime(&TimeHandle, TIMx, &sBDTConfig) != HAL_OK)
    {
        ErrorHandler();
    }
#endif

    sSlaveConfig.slaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.inputTrigger = TIM_TS_ITR2;
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIMx, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_12
    //if(HAL_TIM_ConfigCOMEventInterrupt(&TimeHandle, TIMx, TIM_TS_ITR2, TIM_COMMUTATION_SOFTWARE) != HAL_OK)
#endif
    if (HAL_TIM_ConfigCOMEventInterrupt(&TimeHandle, TIMx, TIM_TS_ITR2, TIM_COMMUTATION_TRGI) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}
#endif

#if (defined TEST_CASE_13_1 || defined TEST_CASE_13_2)
void TIM_DMACallback(DMA_Handle_T *pDMAHandle, uint8_t event, unsigned long param)
{
    TIM_Handle_T *htim = GET_PARENTHDL_FROM_DMAHDL(pDMAHandle, TIM_Handle_T);

    htim->state = HAL_TIM_STATE_DMA_FINISH;
    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        dma_done = 1;
    }

    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        dma_done = -1;
    }

    else
    {

    }

    /* release DMA device */
    HAL_DMA_Abort(pDMAHandle);
}

static RET_CODE TIM_DMAReadWriteFunc(uint32_t *buff1)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_DmaConfig_T sDmaConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sDmaConfig, 0, sizeof(TIM_DmaConfig_T));

    TimeHandle.instance = TIM1;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
#ifdef TEST_CASE_13_1
    sOCConfig.pulse = PULSE1_VALUE;
#endif
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }
#ifdef TEST_CASE_13_1
    sOCConfig.pulse = PULSE2_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE3_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = PULSE4_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_4) != HAL_OK)
    {
        ErrorHandler();
    }
#endif

    LL_TIM_Write_32BitsAddr(0x300, 0x40007054);

#ifdef TEST_CASE_13_1
    if (HAL_TIM_DMAConfig(TIM1, TRUE, TIM_DMA_CC2, &dmaHandle) != HAL_OK)
#else
    if (HAL_TIM_DMAConfig(TIM1, FALSE, TIM_DMA_CC2, &dmaHandle) != HAL_OK)
#endif
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_13_1
    if (HAL_DMA_Start(&dmaHandle, 0x4000704c, buff1, 4) != HAL_OK)
#else
    if (HAL_DMA_Start(&dmaHandle, buff1, 0x4000704c, 4) != HAL_OK)
#endif
    {
        ErrorHandler();
    }

    sDmaConfig.dmaBaseAddress = TIM_DMABASE_CCR1;
    sDmaConfig.dmaBurstLength = TIM_DMABURSTLENGTH_4TRANSFERS;
    if (HAL_TIM_ConfigDmaReg(&TimeHandle, TIM1, &sDmaConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_13_1
    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_4) != HAL_OK)
    {
        ErrorHandler();
    }
#endif
    return HAL_OK;
}
#endif

#if (defined TEST_CASE_14 || defined TEST_CASE_18)
static RET_CODE TIM_QEHSconfig1(TIM_Device_T *TIMx)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_MasterConfig_T sMasterConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM2;
#ifdef TEST_CASE_14
    /* delay 90 angle */
    sOCConfig.pulse = PULSE2_VALUE;
#else
    /* delay 60/120/30 angle */
    sOCConfig.pulse = 300;//600;//150;
#endif
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    sMasterConfig.masterOutputTrigger = TIM_TRGO_OC2REF;
    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIMx, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}

static void TIM_QEHSconfig2(void)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_SlaveConfig_T sSlaveConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));

    TimeHandle.instance = TIM2;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM2, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sSlaveConfig.slaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.inputTrigger = TIM_TS_ITR3;
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIM2, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM2, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}

static void TIM_QEHSconfig3(void)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_SlaveConfig_T sSlaveConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));

    TimeHandle.instance = TIM9;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM9, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sSlaveConfig.slaveMode = TIM_SLAVEMODE_TRIGGER;
    sSlaveConfig.inputTrigger = TIM_TS_ITR0;
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIM9, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM9, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}

static RET_CODE TIM_QEHSconfig4(void)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_MasterConfig_T sMasterConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));

    TimeHandle.instance = TIM2;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM2;
#ifdef TEST_CASE_14
    /* delay 90 angle */
    sOCConfig.pulse = PULSE2_VALUE;
#else
    /* delay 60/120/30 angle */
    sOCConfig.pulse = 300;//600;//150;
#endif
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM2, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    sMasterConfig.masterOutputTrigger = TIM_TRGO_OC2REF;
    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIM2, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM2, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}

static RET_CODE TIM2_OutputToggleModeConfig(void)
{
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIM2;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = 1800;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitOutputCompare(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_TOGGLE;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_LOW;//TIM_OCPOLARITY_HIGH;
    sOCConfig.pulse = 300;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIM2, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = 600;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIM2, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = 900;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIM2, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIM2, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIM2, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIM2, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}
#endif

#ifdef TEST_CASE_14
static RET_CODE TIM_QuadratureEncodeFunc(TIM_Device_T *TIMx, uint32_t qemode)
{
    TIM_EncoderInit_T sQEConfig;
    memset(&sQEConfig, 0, sizeof(TIM_EncoderInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    sQEConfig.encoderMode = qemode;
    sQEConfig.ic1Filter = 0;
    sQEConfig.ic1Polarity = TIM_ICPOLARITY_BOTHEDGE;
    sQEConfig.ic1Prescaler = TIM_ICPSC_DIV1;
    sQEConfig.ic1Selection = TIM_ICSELECTION_DIRECTTI;
    sQEConfig.ic2Filter = 0;
    sQEConfig.ic2Polarity = TIM_ICPOLARITY_BOTHEDGE;
    sQEConfig.ic2Prescaler = TIM_ICPSC_DIV1;
    sQEConfig.ic2Selection = TIM_ICSELECTION_DIRECTTI;

    if (HAL_TIM_InitEncoder(&TimeHandle, TIMx, &sQEConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    /* Starts the TIM Encoder Interface */
    if (HAL_TIM_StartEncoder(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartEncoder(&TimeHandle, TIMx, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#ifdef TEST_CASE_18
static RET_CODE TIM3_MasterSlaveModeConfig(void)
{
    TIM_HallSensorInit_T sHSConfig;
    TIM_OutputCompareInit_T sOCConfig;
    TIM_InputCaptureInit_T sICConfig;
    TIM_MasterConfig_T sMasterConfig;
    memset(&sHSConfig, 0, sizeof(TIM_HallSensorInit_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sICConfig, 0, sizeof(TIM_InputCaptureInit_T));
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));

    TimeHandle.instance = TIM3;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = 0xFFF;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    sICConfig.icPolarity  = TIM_ICPOLARITY_RISING;
    sICConfig.icSelection = TIM_ICSELECTION_DIRECTTI;
    sICConfig.icPrescaler = TIM_ICPSC_DIV1;
    sICConfig.icFilter    = 0;
    sICConfig.icFilter = 0;

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    sMasterConfig.masterOutputTrigger = TIM_TRGO_OC2REF;
    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIM3, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }


    //*(volatile uint32_t *)(0x40007238) = 0xC8;
    sHSConfig.ic1Prescaler = TIM_TRIGGERPRESCALER_DIV1;
    sHSConfig.ic1Polarity = TIM_TRIGGERPOLARITY_NONINVERTED;
    sHSConfig.ic1Filter = 0;
    sHSConfig.commutation_Delay = 0x7FF;
    if (HAL_TIM_InitHallSensor(&TimeHandle, TIM3, &sHSConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartInputCapture(&TimeHandle, TIM3, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIM3, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartHallSensor(&TimeHandle, TIM3) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}

static RET_CODE TIM_HallSensorFunc(void)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_SlaveConfig_T sSlaveConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIM1;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = 0xFF;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;//TIM_COUNTERMODE_CENTERALIGNED3;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = 0x7F;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = 0x3F;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.pulse = 0x1F;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIM1, &sOCConfig, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }

    //sSlaveConfig.slaveMode = TIM_SLAVEMODE_TRIGGER;
    //sSlaveConfig.inputTrigger = TIM_TS_ITR2;
    //if(HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIM1, &sSlaveConfig) != HAL_OK)
    //{
    //ErrorHandler();
    //}

    if (HAL_TIM_ConfigCOMEventInterrupt(&TimeHandle, TIM1, TIM_TS_ITR2, TIM_COMMUTATION_TRGI) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_2) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIM1, TIM_CHANNEL_3) != HAL_OK)
    {
        ErrorHandler();
    }



    return HAL_OK;
}
#endif

#ifdef TEST_CASE_15
static RET_CODE TIM_ExtClk2Func(TIM_Device_T *TIMx)
{
    TIM_ClockConfig_T sClkConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sClkConfig, 0, sizeof(TIM_ClockConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 2;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    /* Clock Configuration Initialization */
    sClkConfig.clockSource = TIM_CLOCKSOURCE_ETRMODE2;
    sClkConfig.clockPolarity = TIM_ETRPOLARITY_NONINVERTED;
    sClkConfig.clockPrescaler = TIM_ETRPRESCALER_DIV1;
    sClkConfig.clockFilter = 0;
    if (HAL_TIM_ConfigClockSource(&TimeHandle, TIMx, &sClkConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#ifdef TEST_CASE_16
static RET_CODE TIM_ExtClearOCref(TIM_Device_T *TIMx)
{
    TIM_OutputCompareInit_T sOCConfig;
    TIM_ClearInputConfig_T sCIConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sCIConfig, 0, sizeof(TIM_ClearInputConfig_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = 0xFF;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitTimeBase(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 0x7F;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    /* Clear Input Configuration Initialization */
    sCIConfig.clearInputState = 1;
    sCIConfig.clearInputSource = TIM_CLEARINPUTSOURCE_ETR;
    sCIConfig.clearInputFilter = 0;
    sCIConfig.clearInputPolarity = TIM_CLEARINPUTPOLARITY_NONINVERTED;//TIM_CLEARINPUTPOLARITY_INVERTED;
    sCIConfig.clearInputPrescaler = TIM_CLEARINPUTPRESCALER_DIV1;

    /* Configures the OCRef clear feature */
    if (HAL_TIM_ConfigOCrefClear(&TimeHandle, TIMx, &sCIConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;
}
#endif

#if (defined TEST_CASE_19 || defined TEST_CASE_20 || defined TEST_CASE_21)
static RET_CODE TIMx_MasterModeConfig(TIM_Device_T *TIMx)
{
    TIM_MasterConfig_T sMasterConfig;
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));
#ifdef TEST_CASE_20
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
#endif

    TimeHandle.instance = TIMx;
#if (defined TEST_CASE_20 || defined TEST_CASE_21)
    TimeHandle.init.prescaler = 3;
#else
    TimeHandle.init.prescaler = 7;
#endif
    TimeHandle.init.period = 0xFF;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

#if (defined TEST_CASE_19 || defined TEST_CASE_21)
    if (HAL_TIM_InitTimeBase(&TimeHandle) != HAL_OK)
#else
    if (HAL_TIM_InitOutputCompare(&TimeHandle) != HAL_OK)
#endif
    {
        ErrorHandler();
    }

#ifdef TEST_CASE_20
    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 0x3F;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }
#endif

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
#if (defined TEST_CASE_19 || defined TEST_CASE_21)
    sMasterConfig.masterOutputTrigger = TIM_TRGO_UPDATE;
#else
    sMasterConfig.masterOutputTrigger = TIM_TRGO_OC2REF;
#endif
    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIMx, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }

#if (defined TEST_CASE_19 || defined TEST_CASE_21)
    LL_TIM_CNT_ENABLE(TIMx);
#else
    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }
#endif

    return HAL_OK;

}

static RET_CODE TIMx_PscEnableGateTIMx(TIM_Device_T *TIMx, uint32_t ts)
{
    TIM_SlaveConfig_T sSlaveConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
#if (defined TEST_CASE_20 || defined TEST_CASE_21)
    TimeHandle.init.prescaler = 3;
#else
    TimeHandle.init.prescaler = 0;
#endif
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = PULSE1_VALUE;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sSlaveConfig.inputTrigger = ts;
#if (defined TEST_CASE_19)
    sSlaveConfig.slaveMode = TIM_SLAVEMODE_EXTERNAL1;
#elif (defined TEST_CASE_20)
    sSlaveConfig.slaveMode = TIM_SLAVEMODE_GATED;
#else
    sSlaveConfig.slaveMode = TIM_SLAVEMODE_TRIGGER;
#endif
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIMx, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}
#endif

#if (defined TEST_CASE_11 || defined TEST_CASE_22 || defined TEST_CASE_23 || defined TEST_CASE_24 || defined TEST_CASE_25 || \
     defined TEST_CASE_26 || defined TEST_CASE_27 || defined TEST_CASE_28 || defined TEST_CASE_29)
static RET_CODE TIMx_AsMasterTimer(TIM_Device_T *TIMx, uint16_t arr)
{
    TIM_MasterConfig_T sMasterConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = arr;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitOutputCompare(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    //LL_TIM_CC1IE_DISABLE(TIMx);

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 0x3F;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    sMasterConfig.masterOutputTrigger = TIM_TRGO_OC1REF;

    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIMx, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}

static RET_CODE TIMx_AsSlaveTimer(TIM_Device_T *TIMx, uint32_t ts)
{
    TIM_SlaveConfig_T sSlaveConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = 0xFF;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 0x3F;
    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sSlaveConfig.slaveMode = TIM_SLAVEMODE_EXTERNAL1;
    sSlaveConfig.inputTrigger = ts;
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIMx, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}
#endif

#ifdef TIM_TRG_RESET
static RET_CODE TIM5_AsMasterTimer(TIM_Device_T *TIMx)
{
    TIM_MasterConfig_T sMasterConfig;
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    TimeHandle.init.repetitionCounter = 0;

    if (HAL_TIM_InitTimeBase(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    TimeHandle.instance = TIM2;
    LL_TIM_CNT_ENABLE(TIM2);

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    sMasterConfig.masterOutputTrigger = TIM_TRGO_RESET;

    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIM2, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    LL_TIM_ClearUgFlag(TIM2);
    HAL_DelayMs(1000);
    LL_TIM_UG_ENABLE(TIM2);

    return HAL_OK;



    TIM_MasterConfig_T sMasterConfig;
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sMasterConfig, 0, sizeof(TIM_MasterConfig_T));
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = arr;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;

    if (HAL_TIM_InitOutputCompare(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    //LL_TIM_CC1IE_DISABLE(TIMx);

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 0x3F;
    if (HAL_TIM_ConfigOCChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    sMasterConfig.masterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    sMasterConfig.masterOutputTrigger = TIM_TRGO_OC1REF;

    if (HAL_TIM_ConfigMasterSyn(&TimeHandle, TIMx, &sMasterConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    if (HAL_TIM_StartOutputCompare(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;


}

static RET_CODE TIM1_AsSlaveTimer(void)
{
    TIM_SlaveConfig_T sSlaveConfig;
    memset(&sSlaveConfig, 0, sizeof(TIM_SlaveConfig_T));

    TimeHandle.instance = TIM1;
    LL_TIM_ClearUgFlag(TIM1);
    //HAL_DelayMs(1000);

    sSlaveConfig.slaveMode = TIM_SLAVEMODE_RESET;
    sSlaveConfig.inputTrigger = TIM_TS_ITR0;
    if (HAL_TIM_ConfigSlaveSyn(&TimeHandle, TIM1, &sSlaveConfig) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}
#endif

#ifdef TEST_CASE_17
void TIMx_SpwmTest(TIM_Device_T *TIMx)
{
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = 7500;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        ErrorHandler();
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 800;

    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    LL_TIM_UIE_ENABLE(TIMx);

    if (HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1) != HAL_OK)
    {
        ErrorHandler();
    }

    return HAL_OK;

}
#endif

#if (defined TEST_CASE_33 || defined TEST_CASE_37)
static RET_CODE PES_OtherIPasMasterCfg(void)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    PES_ChannelIndex_T chIndex;

    //chIndex = IO_PIN_XPA0;
    chIndex = IO_PIN_XPA2;

    chCfg.syncMode = PES_ASYNC;
#ifdef TEST_CASE_33
    chCfg.masterSrcConfig.masterSource = PES_MASTER_TIM8;
    chCfg.masterSrcConfig.masterTrigerOutput.adcOutput = PES_TIM_CH1;
#else
    chCfg.masterSrcConfig.masterSource = PES_MASTER_BTM2;
#endif
    //chCfg.mode.adcOutput = PES_ADC1_EOC;
    chCfg.cascadeConfig.andNextChannelEnable = FALSE;
    chCfg.cascadeConfig.orpreChannelEnable = FALSE;
    chCfg.channelOutputInverted = FALSE;
    chCfg.detectMode = PES_RISING_EDGE;
    HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    HAL_PES_ConfigIEandStatusReg(TRUE, chIndex);

    return ret;
}
#endif

#ifdef TEST_CASE_34
static void PES_TriggerTIM_Config(void)
{
    int i = 0;
    for (i = 0; i < 50000; i++)
    {
        HAL_PES_CupAsMasterGenerateTrigerEvent(PES_HIGH_LEVEL, PES_EVENT_CHANNEL0);
        HAL_DelayMs(500);
        HAL_PES_CupAsMasterGenerateTrigerEvent(PES_LOW_LEVEL, PES_EVENT_CHANNEL0);
        HAL_DelayMs(500);
    }
}
#endif

#ifdef TEST_CASE_35
static void BaseTimer1_Func_Test(void)
{
    HAL_BTM1_InitCommon();
    *(volatile uint32_t *)(0x40036004) = 0xFFFF;
    *(volatile uint32_t *)(0x40036000) |= (0x101);
}
#endif

#if (defined TEST_CASE_36 || defined TEST_CASE_37)
static void BaseTimer2_Func_Test(void)
{
#ifdef TEST_CASE_36
    HAL_BTM2_InitCommon();
#endif
    *(volatile uint32_t *)(0x40036404) = 0xFFFF;
    *(volatile uint32_t *)(0x40036400) |= (0x101);
}
#endif

int main(void)
{
#if (!defined TEST_CASE_35 && !defined TEST_CASE_36 && !defined TEST_CASE_37)
    //utim_clk_enable();
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM1);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM2);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM3);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM4);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM5);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM8);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM9);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM10);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM11);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM12);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM13);
    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM14);
    utim_hirc_calibrate();
    //utim_reset_release();
    HAL_Reset_Module(RESET_TIM1_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM2_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM3_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM4_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM5_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM8_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM9_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM10_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM11_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM12_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM13_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM14_SW_RSTJ);

    //PINMUX_Config();
#endif

#if (defined TEST_CASE_35 || defined TEST_CASE_36 || defined TEST_CASE_37)
    //ubtm_clk_enable();
    HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_BTM1);
    HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_BTM2);
    utim_hirc_calibrate();
    //ubtm_reset_release();
    HAL_Reset_Module(RESET_BTM1_SW_RSTJ);
    HAL_Reset_Module(RESET_BTM2_SW_RSTJ);
#endif

#if (defined TEST_CASE_1 || defined TEST_CASE_2)
    TIM_UpdateAutoReloadFunc(TIM1);
#endif

#ifdef TEST_CASE_3
    TIM_CompareFunc(TIM1, TIM_OCMODE_ACTIVE); //TIM_OCMODE_TIMING;//TIM_OCMODE_INACTIVE;
    TIM_CompareFunc(TIM2, TIM_OCMODE_PWM1); //TIM_OCMODE_PWM2;//TIM_OCMODE_FORCED_ACTIVE
    TIM_CompareFunc(TIM3, TIM_OCMODE_FORCED_INACTIVE);//TIM_OCMODE_TOGGLE;
#endif

#if (defined TEST_CASE_4 || defined TEST_CASE_6 || defined TEST_CASE_10)
    TIM_PWMOutput(TIM1);
#endif

#ifdef TEST_CASE_5
    //TIM_PWMOutput(TIM3);
    TIM_OnePulse(TIM1);
#endif

#ifdef TEST_CASE_7
    //TIM_BrakeFunc(TIM1, TIM_BREAKPOLARITY_HIGH);
    //TIM_BrakeFunc(TIM8, TIM_BREAKPOLARITY_LOW);
    TIM_BrakeFunc(TIM8, TIM_BREAKPOLARITY_HIGH);
#endif

#ifdef TEST_CASE_8
    TIM_PWMOutput(TIM2);
    TIM_InputTest(TIM1);
#endif

#if (defined TEST_CASE_9 || defined TEST_CASE_32)
    //TIM_PWMOutput(TIM2);
    TIM_PWMOutput(TIM2);
    TIM_InputCapture(TIM1);
#endif

#ifdef TEST_CASE_11
    TIM3_AsMasterTimer();
    //TIM_SlaveModeFunc(TIM1, TIM_SLAVEMODE_DISABLE);
    TIM_SlaveModeFunc(TIM1, TIM_SLAVEMODE_RESET);
    //TIM_SlaveModeFunc(TIM1, TIM_SLAVEMODE_GATED);
    TIM_SlaveModeFunc(TIM1, TIM_SLAVEMODE_TRIGGER);
    //TIM_SlaveModeFunc(TIM1, TIM_SLAVEMODE_EXTERNAL1);
#endif

#if (defined TEST_CASE_12 || defined TEST_CASE_30)
    //TIM3_AsMasterTimer();
    //HAL_DelayMs(500);
    //#ifdef TEST_CASE_12
    //LL_TIM_COMG_ENABLE(TIM1);
    //#endif
    //TIM_ComSpwmFunc(TIM1);
#endif

#if defined(TEST_CASE_13_1)
    TIM_DMAReadWriteFunc(arrayRead);
#elif defined(TEST_CASE_13_2)
    TIM_DMAReadWriteFunc(arrayWrite);
#endif

#ifdef TEST_CASE_14
#if 1
    /* TIM4 ahead TIM2 90 angle*/
    TIM_QEHSconfig1(TIM4);
    TIM_QEHSconfig2();
    LL_TIM_CNT_ENABLE(TIM4);
#else
    /* TIM9 behind TIM2 90 angle*/
    TIM_QEHSconfig1(TIM2);
    TIM_QEHSconfig3(void);
    LL_TIM_CNT_ENABLE(TIM2);
#endif
    TIM_QuadratureEncodeFunc(TIM1, TIM_ENCODERMODE_TI12);//TIM_ENCODERMODE_TI1;//TIM_ENCODERMODE_TI2;
#endif

#ifdef TEST_CASE_15
    TIM_PWMOutput(TIM3);
    TIM_ExtClk2Func(TIM1);
#endif

#ifdef TEST_CASE_16
    TIM_PWMOutput(TIM2);
    TIM_ExtClearOCref(TIM1);
#endif

#ifdef TEST_CASE_17
    TIMx_SpwmTest(TIM1);
#endif

#ifdef TEST_CASE_18
    //TIM_QEHSconfig1(TIM4);
    //TIM_QEHSconfig2();
    //LL_TIM_CNT_ENABLE(TIM4);
    //TIM_QEHSconfig1(TIM2);
    //TIM_QEHSconfig4();
    //TIM_QEHSconfig3();
    //LL_TIM_CNT_ENABLE(TIM4);
    //LL_TIM_CNT_ENABLE(TIM2);
    //TIM3_MasterSlaveModeConfig();
    //TIM_HallSensorFunc();
    //*(volatile uint32_t *)(0x4000710C) = 0x0;
    TIM2_OutputToggleModeConfig();
    //*(volatile uint32_t *)(0x4000710C) = 0x0;
    TIM3_MasterSlaveModeConfig();
    TIM_HallSensorFunc();
#endif

#ifdef TEST_CASE_19
    TIMx_MasterModeConfig(TIM3);
    TIMx_PscEnableGateTIMx(TIM1, TIM_TS_ITR2);
#endif

#ifdef TEST_CASE_20
    TIMx_MasterModeConfig(TIM2);
    TIMx_PscEnableGateTIMx(TIM1, TIM_TS_ITR1);
#endif

#ifdef TEST_CASE_21
    TIMx_MasterModeConfig(TIM2);
    TIMx_PscEnableGateTIMx(TIM1, TIM_TS_ITR1);
#endif

#ifdef TEST_CASE_22
    //TIMx_AsMasterTimer(TIM5, 0xfff);
    //TIMx_AsSlaveTimer(TIM1, TIM_TS_ITR0);
    //LL_TIM_ClearTriggerFlag(TIM1);

    //TIMx_AsMasterTimer(TIM2, 0xfff);
    //TIMx_AsSlaveTimer(TIM1, TIM_TS_ITR1);
    //LL_TIM_ClearTriggerFlag(TIM1);

    //TIMx_AsMasterTimer(TIM3, 0xfff);
    //TIMx_AsSlaveTimer(TIM1, TIM_TS_ITR2);
    //LL_TIM_ClearTriggerFlag(TIM1);

    TIMx_AsMasterTimer(TIM4, 0xfff);
    TIMx_AsSlaveTimer(TIM1, TIM_TS_ITR3);
    LL_TIM_ClearTriggerFlag(TIM1);
#endif

#ifdef TEST_CASE_23
    TIMx_AsMasterTimer(TIM1, 0xfff);
    TIMx_AsSlaveTimer(TIM2, TIM_TS_ITR0);
    LL_TIM_ClearTriggerFlag(TIM2);

    //TIMx_AsMasterTimer(TIM8, 0xfff);
    //TIMx_AsSlaveTimer(TIM2, TIM_TS_ITR1);
    //LL_TIM_ClearTriggerFlag(TIM2);

    //TIMx_AsMasterTimer(TIM3, 0xfff);
    //TIMx_AsSlaveTimer(TIM2, TIM_TS_ITR2);
    //LL_TIM_ClearTriggerFlag(TIM2);

    //TIMx_AsMasterTimer(TIM4, 0xfff);
    //TIMx_AsSlaveTimer(TIM2, TIM_TS_ITR3);
    //LL_TIM_ClearTriggerFlag(TIM2);
#endif

#ifdef TEST_CASE_24
    //TIMx_AsMasterTimer(TIM1, 0xfff);
    //TIMx_AsSlaveTimer(TIM3, TIM_TS_ITR0);
    //LL_TIM_ClearTriggerFlag(TIM3);

    //TIMx_AsMasterTimer(TIM2, 0xfff);
    //TIMx_AsSlaveTimer(TIM3, TIM_TS_ITR1);
    //LL_TIM_ClearTriggerFlag(TIM3);

    TIMx_AsMasterTimer(TIM5, 0xfff);
    TIMx_AsSlaveTimer(TIM3, TIM_TS_ITR2);
    LL_TIM_ClearTriggerFlag(TIM3);

    //TIMx_AsMasterTimer(TIM4, 0xfff);
    //TIMx_AsSlaveTimer(TIM3, TIM_TS_ITR3);
    //LL_TIM_ClearTriggerFlag(TIM3);
#endif

#ifdef TEST_CASE_25
    TIMx_AsMasterTimer(TIM1, 0xfff);
    TIMx_AsSlaveTimer(TIM4, TIM_TS_ITR0);
    LL_TIM_ClearTriggerFlag(TIM4);

    //TIMx_AsMasterTimer(TIM2, 0xfff);
    //TIMx_AsSlaveTimer(TIM4, TIM_TS_ITR1);
    //LL_TIM_ClearTriggerFlag(TIM4);

    //TIMx_AsMasterTimer(TIM3, 0xfff);
    //TIMx_AsSlaveTimer(TIM4, TIM_TS_ITR2);
    //LL_TIM_ClearTriggerFlag(TIM4);

    //TIMx_AsMasterTimer(TIM8, 0xfff);
    //TIMx_AsSlaveTimer(TIM4, TIM_TS_ITR3);
    //LL_TIM_ClearTriggerFlag(TIM4);
#endif

#ifdef TEST_CASE_26
    //TIMx_AsMasterTimer(TIM2, 0xfff);
    //TIMx_AsSlaveTimer(TIM5, TIM_TS_ITR0);
    //LL_TIM_ClearTriggerFlag(TIM5);

    TIMx_AsMasterTimer(TIM3, 0xfff);
    TIMx_AsSlaveTimer(TIM5, TIM_TS_ITR1);
    LL_TIM_ClearTriggerFlag(TIM5);

    //TIMx_AsMasterTimer(TIM4, 0xfff);
    //TIMx_AsSlaveTimer(TIM5, TIM_TS_ITR2);
    //LL_TIM_ClearTriggerFlag(TIM5);

    //TIMx_AsMasterTimer(TIM8, 0xfff);
    //TIMx_AsSlaveTimer(TIM5, TIM_TS_ITR3);
    //LL_TIM_ClearTriggerFlag(TIM5);
#endif

#ifdef TEST_CASE_27
    //TIMx_AsMasterTimer(TIM1, 0xfff);
    //TIMx_AsSlaveTimer(TIM8, TIM_TS_ITR0);
    //LL_TIM_ClearTriggerFlag(TIM8);

    //TIMx_AsMasterTimer(TIM2, 0xfff);
    //TIMx_AsSlaveTimer(TIM8, TIM_TS_ITR1);
    //LL_TIM_ClearTriggerFlag(TIM8);

    TIMx_AsMasterTimer(TIM4, 0xfff);
    TIMx_AsSlaveTimer(TIM8, TIM_TS_ITR2);
    LL_TIM_ClearTriggerFlag(TIM8);

    //TIMx_AsMasterTimer(TIM5, 0xfff);
    //TIMx_AsSlaveTimer(TIM8, TIM_TS_ITR3);
    //LL_TIM_ClearTriggerFlag(TIM8);
#endif

#ifdef TEST_CASE_28
    //TIMx_AsMasterTimer(TIM2, 0xfff);
    //TIMx_AsSlaveTimer(TIM9, TIM_TS_ITR0);
    //LL_TIM_ClearTriggerFlag(TIM9);

    //TIMx_AsMasterTimer(TIM3, 0xfff);
    //TIMx_AsSlaveTimer(TIM9, TIM_TS_ITR1);
    //LL_TIM_ClearTriggerFlag(TIM9);

    //TIMx_AsMasterTimer(TIM10, 0xfff);
    //TIMx_AsSlaveTimer(TIM9, TIM_TS_ITR2);
    //LL_TIM_ClearTriggerFlag(TIM9);

    TIMx_AsMasterTimer(TIM11, 0xfff);
    TIMx_AsSlaveTimer(TIM9, TIM_TS_ITR3);
    LL_TIM_ClearTriggerFlag(TIM9);
#endif

#ifdef TEST_CASE_29
    //TIMx_AsMasterTimer(TIM4, 0xfff);
    //TIMx_AsSlaveTimer(TIM12, TIM_TS_ITR0);
    //LL_TIM_ClearTriggerFlag(TIM12);

    TIMx_AsMasterTimer(TIM5, 0xfff);
    TIMx_AsSlaveTimer(TIM12, TIM_TS_ITR1);
    LL_TIM_ClearTriggerFlag(TIM12);

    //TIMx_AsMasterTimer(TIM13, 0xfff);
    //TIMx_AsSlaveTimer(TIM12, TIM_TS_ITR2);
    //LL_TIM_ClearTriggerFlag(TIM12);

    //TIMx_AsMasterTimer(TIM14, 0xfff);
    //TIMx_AsSlaveTimer(TIM12, TIM_TS_ITR3);
    //LL_TIM_ClearTriggerFlag(TIM12);
#endif

#ifdef TEST_CASE_31
    TIM3_AsMasterTimer();
    TIM_ComSpwmFunc(TIM1);
    TIM_InputCapture(TIM2);
#endif

#ifdef TIM_TRG_RESET
    TIM1_AsSlaveTimer();
    TIM5_AsMasterTimer(TIM5);
#endif

#ifdef TEST_CASE_33
    PES_OtherIPasMasterCfg();
    TIM_PWMOutput(TIM8);
#endif

#ifdef TEST_CASE_34
    //PES_TriggerTIM_Config();
    TIM_InputCapture(TIM1);
    PES_TriggerTIM_Config();
#endif

#ifdef TEST_CASE_35
    BaseTimer1_Func_Test();
#endif

#ifdef TEST_CASE_36
    BaseTimer2_Func_Test();
#endif

#ifdef TEST_CASE_37
    PES_OtherIPasMasterCfg();
    BaseTimer2_Func_Test();
#endif

    do
    {
        k_sleep(1);
        printf("test ok\n");
    } while (1);
}

void ErrorHandler(void)
{
    do
    {
        k_sleep(1);
        printf("test fail\n");
    } while (1);
}




