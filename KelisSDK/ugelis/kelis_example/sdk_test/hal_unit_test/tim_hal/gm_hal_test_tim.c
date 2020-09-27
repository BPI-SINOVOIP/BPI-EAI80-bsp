/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_test_tim.c
 *
 * @author      Edward.Zhu
 *
 * @version     v1.0.0
 *
 * @date        2019/11/26
 *
 * @brief       tim demo source file
 *
 * @note
 *              2019/11/26, Edward.Zhu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <board.h>
#include <ztest.h>

#include "gm_hal_test_tim.h"
#include <gm_common.h>
#include <gm_hal_tim.h>
#include <gm_hal_gpio.h>

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

#define PERIOD_VALUE  0xFFFF
#define PULSE1_VALUE  0x7FFF
#define PULSE2_VALUE  0x3FFF
#define PULSE3_VALUE  0xFFF
#define PULSE4_VALUE  0x7FF

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

TIM_Handle_T TimeHandle;

uint32_t uwFrequency = 0;  /* TIM_PWMInput */

/* Captured Values */
uint32_t uwIC2Value1 = 0;       /* TIM_InputCapture */
uint32_t uwIC2Value2 = 0;       /* TIM_InputCapture */
uint32_t uwDiffCapture = 0;     /* TIM_InputCapture */
/* Capture index */
uint16_t uhCaptureIndex = 0;    /* TIM_InputCapture */

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

static int pwm_init(TIM_Device_T *TIMx)
{
    RET_CODE ret = RET_OK;

    TIM_OutputCompareInit_T sOCConfig;
    TIM_BreakDeadTimeConfig_T sBDTConfig;

    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));
    memset(&sBDTConfig, 0, sizeof(TIM_BreakDeadTimeConfig_T));

    TC_PRINT("Testcase: %s\n", __func__);

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 7;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_CENTERALIGNED1;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;

    ret = HAL_TIM_InitPWM(&TimeHandle);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;

    sOCConfig.pulse = PULSE1_VALUE;
    ret = HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    sBDTConfig.breakState = TIM_BREAK_ENABLE;
    sBDTConfig.breakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBDTConfig.automaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;//TIM_AUTOMATICOUTPUT_DISABLE;
    sBDTConfig.lockLevel = TIM_LOCKLEVEL_OFF;
    sBDTConfig.deadTime = 0xd2;
    ret = HAL_TIM_ConfigBreakDeadTime(&TimeHandle, TIMx, &sBDTConfig);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    ret = HAL_TIM_StartPWM(&TimeHandle, TIMx, TIM_CHANNEL_1);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    TC_PRINT("tim output test ok!\n");

    return TC_PASS;

}

static int pwm_capture(TIM_Device_T *TIMx)
{
    RET_CODE ret = RET_OK;

    TIM_InputCaptureInit_T sICConfig;
    memset(&sICConfig, 0, sizeof(TIM_InputCaptureInit_T));

    TC_PRINT("Testcase: %s\n", __func__);

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    ret = HAL_TIM_InitInputCapture(&TimeHandle);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    sICConfig.icPolarity  = TIM_ICPOLARITY_RISING;
    sICConfig.icSelection = TIM_ICSELECTION_DIRECTTI;
    sICConfig.icPrescaler = TIM_ICPSC_DIV1;
    sICConfig.icFilter = 0;

    ret = HAL_TIM_ConfigICChannel(&TimeHandle, TIMx, &sICConfig, TIM_CHANNEL_2);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    ret = HAL_TIM_StartInputCapture(&TimeHandle, TIMx, TIM_CHANNEL_2);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    TC_PRINT("tim capture test ok!\n");

    return TC_PASS;

}

static int opm_test(TIM_Device_T *TIMx)
{
    RET_CODE ret = RET_OK;

    TIM_OnePulseInit_T sOPMConfig;
    memset(&sOPMConfig, 0, sizeof(TIM_OnePulseInit_T));

    TC_PRINT("Testcase: %s\n", __func__);

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 0;
    TimeHandle.init.period = PERIOD_VALUE;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;
    TimeHandle.init.repetitionCounter = 3;

    ret = HAL_TIM_InitOnePulse(&TimeHandle, TIMx, TIM_OPMODE_SINGLE);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    sOPMConfig.ocMode = TIM_OCMODE_PWM1;
    sOPMConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOPMConfig.pulse = 600;
    sOPMConfig.icPolarity = TIM_ICPOLARITY_RISING;
    sOPMConfig.icSelection = TIM_ICSELECTION_DIRECTTI;
    sOPMConfig.icFilter = 20;

    ret = HAL_TIM_ConfigOPMChannel(&TimeHandle, TIMx, &sOPMConfig, TIM_CHANNEL_1, TIM_CHANNEL_2);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    ret = HAL_TIM_StartOnePulse(&TimeHandle, TIMx, TIM_CHANNEL_1);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    LL_TIM_CNT_ENABLE(TIMx);

    TC_PRINT("tim one pulse mode test ok!\n");

    return TC_PASS;
}

static int test_tim_output(void)
{
    RET_CODE ret = RET_OK;

    ret = pwm_init(TIM2);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_tim_input(void)
{
    RET_CODE ret = RET_OK;

    ret = pwm_init(TIM4);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    ret = pwm_capture(TIM3);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    return TC_PASS;
}

static int test_tim_opm(void)
{
    RET_CODE ret = RET_OK;

    ret = opm_test(TIM1);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    return TC_PASS;
}

void gm_hal_test_tim(void)
{
    TC_PRINT("================TIM Test Start=================\n\n\n");

    TC_PRINT("TEST1\n");
    zassert_true(test_tim_output() == TC_PASS, NULL);
    TC_PRINT("TEST2\n");
    zassert_true(test_tim_input() == TC_PASS, NULL);
    zassert_true(test_tim_opm() == TC_PASS, NULL);
    TC_PRINT("TEST3\n");

    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================TIM Test End==================\n\n\n");
}
