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
 * @brief       gpio demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include "board.h"
#include <ztest.h>
#include <gm_hal_gpio.h>
#include <gm_hal_acmp.h>
#include <gm_hal_dac.h>
#include "acmp_test.h"

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printf
#endif


ACMP_Handle_T acmp_hanle;
#define CNT         2

//static RET_CODE DAC_ConfigPinmux(void)
//{
//  RET_CODE ret = RET_OK;
//  GPIO_PinConfig_T GPIO_InitStruct;
//    GPIO_Device_T *pGPIO = NULL;

//  memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

//    /*set digtal pin as input , inorder to not affect analog input*/
//    pGPIO = GPIOC;
//  GPIO_InitStruct.pin  = GPIO_PIN_8; /*GPIO_PIN_9*/
//  GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
//  GPIO_InitStruct.pull = GPIO_NOPULL;

//  ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
//  if (ret != RET_OK)
//  {
//      PR_ERR("Err:dac_ConfigPinmux\n");
//  }

//  return ret;
//}
//static RET_CODE acmp_configoutput_pinmux(void)
//{
//    RET_CODE ret = RET_OK;
//    GPIO_PinConfig_T GPIO_InitStruct;
//    GPIO_Device_T *pGPIO = NULL;

//    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

//    /* config pinmux as acmp output pin */

//    pGPIO = GPIOC;
//    GPIO_InitStruct.pin = GPIO_PIN_14;   /*XPC14 ,XPE7*/
//    GPIO_InitStruct.mode = GPIO_MODE_AF;
//    GPIO_InitStruct.pull = GPIO_PULLUP;
//    GPIO_InitStruct.alternate = GPIOC14_AF10_ACMP_OUT;

//    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
//    if (ret != RET_OK)
//    {
//        goto out;
//    }

//out:
//    return ret;
//}

/*
    ACMP3(GPIO12)  ACMP0(GPIOC13) ACMP1(GPIOC14) ACMP2(GPIO15)
*/
static RET_CODE ACMP_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorde to not affect analog input*/
    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_12 | GPIO_PIN_14; /* GPIO_PIN_13 | GPIO_PIN_14 |GPIO_PIN_15 |*/
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
}

static void ACMP_Callback(ACMP_Handle_T *pHandle, ACMP_Event_T event, unsigned long Param)
{
    static int cnt = 0;
    if (event == ACMP_MATCH_EVENT)
    {
        PR_DEBUG("negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", cnt++);
    }
}

RET_CODE acmp_demo(ACMP_Handle_T *pHandle, ACMP_Config_T *acmo_config)
{
    RET_CODE ret = RET_OK;
    ACMP_CompareResult_T result;
    int cnt = 0;

    ACMP_Config_T *pconfig = &(pHandle->config);
    pconfig->callback = (ACMP_Callback_T)ACMP_Callback;

    pconfig->outputInversionEnable = acmo_config->outputInversionEnable;

    pconfig->filterThr = acmo_config->filterThr;

    pconfig->negativePin = acmo_config->negativePin;
    pconfig->positivePin = acmo_config->positivePin;
    pconfig->outputPinEnable = acmo_config->outputPinEnable;
    pconfig->triggerMode = acmo_config->triggerMode;
    pconfig->blankTimesel = acmo_config->blankTimesel;

    //ret = ACMP_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:%d ACMP_ConfigPinmux\n", __FUNCTION__, ret);
        goto out;
    }

    /* if use outputpin as output , must config pinmux first */
    if (pconfig->outputPinEnable == ENABLE)
    {
        //ret = acmp_configoutput_pinmux();
        if (ret != RET_OK)
        {
            PR_ERR("%s:ACMP Config Pinmux Error ret:%d\n", __FUNCTION__, ret);
            goto out;
        }
    }

    ret = HAL_ACMP_Init(pHandle, ACMP_DEV, pconfig);
    if (ret != RET_OK)
    {
        PR_ERR("%s:ACMP_Init Error ret:%d\n", __FUNCTION__, ret);
        goto out;
    }

    while (cnt++ < CNT)
    {
        result = HAL_ACMP_GetCompareRestult(pHandle);
        PR_DEBUG("acmp result  = 0x%x\n", result);
        k_sleep(100);
    }

out:
    return ret;
}

RET_CODE dac_cfg(DAC_Handle_T *pHandle, DAC_Config_T *dac_config)
{
    RET_CODE ret = RET_OK;
    DAC_Device_T *pDAC = DAC0_DEV;
    unsigned int dac_out = 0;

    DAC_Config_T *pconfig = &(pHandle->config);
    pconfig->dacEn = dac_config->dacEn;
    pconfig->samholdEn = dac_config->samholdEn;
    pconfig->waveType = dac_config->waveType;
    pconfig->msAM = dac_config->msAM;
    pconfig->dacDMAEn = dac_config->dacDMAEn;
    pconfig->resolution = dac_config->resolution;
    pconfig->dacSoftTrigleEn = dac_config->dacSoftTrigleEn;
    pconfig->dacComConfig.dacHoldData = dac_config->dacComConfig.dacHoldData;
    pconfig->dacComConfig.dacHoldSampTime = dac_config->dacComConfig.dacHoldSampTime;
    pconfig->dacComConfig.dacHoldTime = dac_config->dacComConfig.dacHoldTime;
    pconfig->dacComConfig.dacHoldRefTime = dac_config->dacComConfig.dacHoldRefTime;
    pconfig->dacComConfig.dacDAC1DualHoldData = dac_config->dacComConfig.dacDAC1DualHoldData;
    pconfig->dacComConfig.dacDAC0DualHoldData = dac_config->dacComConfig.dacDAC0DualHoldData;
    pconfig->triggerMode = dac_config->triggerMode;
    pconfig->dacSel = dac_config->dacSel;

    //ret = DAC_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:%d DAC_ConfigPinmux\n", __FUNCTION__, ret);
        goto out;
    }
    pDAC = DAC0_DEV;

    ret = HAL_DAC_Init(pHandle, pDAC, pconfig);
    if (ret != RET_OK)
    {
        PR_ERR("%s:HAL_DAC_Init Error ret:%d\n", __FUNCTION__, ret);
        goto out;
    }


    HAL_DAC_Triger(pHandle, pconfig);
    dac_out = HAL_DAC_GetDataOut(pHandle);
    PR_DEBUG("Func[%s]: dacout = [0x%x], dacSel = %d\n", __FUNCTION__, dac_out, pconfig->dacSel);

out:
    return ret;
}

static int demo(int demo_select)
{
    ACMP_Config_T acmp_config;
    GPIO_PinConfig_T GPIO_InitStruct;
    DAC_Config_T dac_config;
    DAC_Handle_T dac_handle;

    PR_DEBUG("================Enter func[ACMP_%s%d] Test=================\n", __FUNCTION__, demo_select);

    memset(&acmp_config, 0, sizeof(acmp_config));
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;

    memset(&dac_config, 0, sizeof(dac_config));

    if (demo_select == 1)
    {
        dac_config.dacSel = DAC0_SEL;
        dac_config.dacEn = ENABLE;
        dac_config.samholdEn = DISABLE;
        dac_config.waveType = DAC_WAVE_DISABLE;
        dac_config.msAM = 0;
        dac_config.dacDMAEn = DISABLE;
        dac_config.resolution = DAC_DATA_MODE_12BIT;
        dac_config.dacSoftTrigleEn = ENABLE;
        dac_config.dacComConfig.dacHoldData = 0xFFF;
        dac_config.dacComConfig.dacHoldSampTime = 0;
        dac_config.dacComConfig.dacHoldTime = 0;
        dac_config.dacComConfig.dacHoldRefTime = 0;
        dac_config.dacComConfig.dacDAC1DualHoldData = 0;
        dac_config.dacComConfig.dacDAC0DualHoldData = 0;
        dac_config.triggerMode = DAC_SINGLE_SOFT_TRIG;
        dac_cfg(&dac_handle, &dac_config);
    }

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);

    if (demo_select == 0)
    {
        acmp_config.achyst_sel = 0;
        acmp_config.blankTimesel = NO_BLANKING;
        acmp_config.triggerMode = ANALOG_MATCH_RISING;
        acmp_config.negativePin = ANALOG_COMPARE_PIN_ACMP3;
        acmp_config.positivePin = ANALOG_COMPARE_PIN_ACMP1;
        acmp_config.outputInversionEnable = ENABLE;
        acmp_config.outputPinEnable = DISABLE;
        acmp_config.filterThr = 255;
        acmp_demo(&acmp_hanle, &acmp_config);
    }
    else if (demo_select == 1)
    {
        acmp_config.achyst_sel = 0;
        acmp_config.blankTimesel = NO_BLANKING;
        acmp_config.triggerMode = ANALOG_MATCH_RISING;
        acmp_config.negativePin = ANALOG_COMPARE_DAC1_OUT;
        acmp_config.positivePin = ANALOG_COMPARE_PIN_ACMP1;
        acmp_config.outputInversionEnable = ENABLE;
        acmp_config.outputPinEnable = DISABLE;
        acmp_config.filterThr = 255;
        acmp_demo(&acmp_hanle, &acmp_config);
    }

    PR_DEBUG("================Exit func[ACMP_%s%d] Test=================\n", __FUNCTION__, demo_select);

    return TC_PASS;
}

void acmp_test(void)
{
    zassert_true(demo(0) == TC_PASS, NULL);
    zassert_true(demo(1) == TC_PASS, NULL);
}
