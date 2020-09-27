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
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>
#include <gm_hal_dac.h>
#include <gm_hal_gpio.h>
#include "dac_test.h"

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

#define CNT     1

DAC_Handle_T dac_handle;

static RET_CODE DAC_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorder to not affect analog input*/
    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;

    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_ERR("Err:dac_ConfigPinmux\n");
    }

    return ret;
}

RET_CODE dac_demo(DAC_Handle_T *pHandle, DAC_Config_T *dac_config)
{
    RET_CODE ret = RET_OK;
    DAC_Device_T *pDAC;
    unsigned int dac_out = 0;
    int cnt = 0;

    PR_DEBUG("=Enter func[%s] Test=dac%d\n", __FUNCTION__,  dac_config->dacSel);

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

    ret = DAC_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:%d DAC_ConfigPinmux\n", __FUNCTION__, ret);
        goto out;
    }

    if (pconfig->dacSel == DAC0_SEL)
    {
        pDAC = DAC0_DEV;
        ret = HAL_DAC_Init(pHandle, pDAC, pconfig);
        if (ret != RET_OK)
        {
            PR_ERR("%s:HAL_DAC_Init Error ret:%d\n", __FUNCTION__, ret);
            goto out;
        }

        while (cnt++ < CNT)
        {
            HAL_DAC_Triger(pHandle, pconfig);
            dac_out = HAL_DAC_GetDataOut(pHandle);
            PR_DEBUG("Func[%s]: dacout = [0x%x], dacSel = %d\n", __FUNCTION__, dac_out, pconfig->dacSel);
            k_sleep(500);
        }

    }

    PR_DEBUG("Exit func[%s] Test==dac%d\n", __FUNCTION__,  pconfig->dacSel);

out:
    return ret;
}

static int demo(int dac_select)
{
    DAC_Config_T dac_config;

    if (dac_select == 0)
    {
        dac_config.dacSel = DAC0_SEL;
        dac_config.dacEn = ENABLE;
        dac_config.samholdEn = DISABLE;
        dac_config.waveType = DAC_WAVE_DISABLE;
        dac_config.msAM = 0;
        dac_config.dacDMAEn = DISABLE;
        dac_config.resolution = DAC_DATA_MODE_12BIT;
        dac_config.dacSoftTrigleEn = ENABLE;
        dac_config.dacComConfig.dacHoldData = 0xFFF / 2;
        dac_config.dacComConfig.dacHoldSampTime = 0;
        dac_config.dacComConfig.dacHoldTime = 0;
        dac_config.dacComConfig.dacHoldRefTime = 0;
        dac_config.dacComConfig.dacDAC1DualHoldData = 0;
        dac_config.dacComConfig.dacDAC0DualHoldData = 0;
        dac_config.triggerMode = DAC_SINGLE_SOFT_TRIG;
        dac_demo(&dac_handle, &dac_config);
    }

    return TC_PASS;
}

void dac_test(void)
{
    zassert_true(demo(0) == TC_PASS, NULL);
}
