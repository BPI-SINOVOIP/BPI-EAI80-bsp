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
#include "board.h"
#include <ztest.h>
#include <gm_hal_gpio.h>
#include "adc_test.h"
#include <gm_hal_adc.h>
#include <gm_common.h>

#define DEBUG_PRINTF
#define ERR_PRINTF
#define WARNING_PRINTF

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

#ifndef CONFIG_SDRAM_BOOT
    #define USER_SEQUECE_LEN  16
    #define USER_SINGLE_CHANNEL_LEN   1
    #define USER_INJECT_SEQUECE_LEN  2
#else

    #define USER_SEQUECE_LEN  2
    #define USER_SINGLE_CHANNEL_LEN   1
    #define USER_INJECT_SEQUECE_LEN  2
#endif

#define CNT     10

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

static ADC_HandleTypeDef g_ADCHandler;


RET_CODE HAL_ADC_ReadConvertValue(ADC_HandleTypeDef *hADC, uint32_t *pConvertData, uint32_t timeout)
{
    RET_CODE ret = RET_OK;
    int time_cnt = 0;

    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        /* noblock */
        hADC->blockStatus = NOBLOCK;
    }
    else
    {
        /* block*/
        hADC->blockStatus = BLOCK;
        hADC->expectFinishStatus = HAL_ADC_STATE_CPU_FINISH;
    }

    HAL_ADC_Start_IT(hADC);

    /* read convert */
    if (hADC->blockStatus == BLOCK)
    {
        while (1)
        {
            /* Exit if adc convert finish */
            if (hADC->State == hADC->expectFinishStatus)
            {
                break;
            }

            if (time_cnt++ > 5000)
            {
                ret = RET_TIMEOUT;
                goto out;

            }
            k_sleep(1);
        }

        /* store convet value to pConvertData */
        if (hADC->expectFinishStatus == HAL_ADC_STATE_CPU_FINISH)
        {
            /*cpu mode*/
            HAL_ADC_FifoConvertValue(hADC, hADC->Init.Resolution,  pConvertData, hADC->Init.NbrOfConversion);
        }

    }

out:
    return ret;

}

RET_CODE HAL_ADC_ReadInJectConvertValue(ADC_HandleTypeDef *hADC, uint32_t *pConvertData, uint32_t timeout)
{
    RET_CODE ret = RET_OK;
    int time_cnt = 0;

    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        /* noblock */
        hADC->blockStatus = NOBLOCK;
    }
    else
    {
        /* block*/
        hADC->blockStatus = BLOCK;
        hADC->expectFinishStatus = HAL_ADC_STATE_CPU_FINISH;
    }

    HAL_ADCEx_InjectedStart_IT(hADC);
    /* read convert */
    if (hADC->blockStatus == BLOCK)
    {
        while (1)
        {
            /* Exit if adc convert finish */
            if (hADC->State == hADC->expectFinishStatus)
            {
                break;
            }

            if (time_cnt++ > 50000)
            {
                ret = RET_TIMEOUT;
                goto out;

            }
            k_sleep(1);
        }

        /* store convet value to pConvertData */
        if (hADC->expectFinishStatus == HAL_ADC_STATE_CPU_FINISH)
        {
            /*cpu mode*/
            HAL_ADC_InjectConvertValue(hADC, hADC->Init.Resolution,  pConvertData, hADC->Init.InjectedNbrOfConversion);
        }

    }

out:
    return ret;

}

int adc_channel_sync_demo(int adc_select)
{
    ADC_ChannelConfTypeDef config;
    GPIO_PinConfig_T GPIO_InitStruct;
    int ConvertData[16] = {0};
    int ret = 0;
    int cnt = 0;

    g_ADCHandler.device = ADC_DEV0;
    g_ADCHandler.Init.Resolution = ADC_RESOLUTION_12BIT;
    g_ADCHandler.Init.NbrOfConversion = USER_SINGLE_CHANNEL_LEN;
    g_ADCHandler.Init.workMode = ADC_SINGLE_MODE;
    g_ADCHandler.Init.dma_en = DISABLE;
    g_ADCHandler.dma_rst_len = USER_SINGLE_CHANNEL_LEN;
    g_ADCHandler.Init.timeOut = TIMEOUT_WAIT_FOREVER;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1; /*GPIOA0,GPIOA1*/
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = 0;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);

    PR_DEBUG("Enter func[%s] Test\n", __FUNCTION__);

    HAL_ADC_Init(&g_ADCHandler);

    config.rchn_sel = ADC_CHANNEL_0;

    //ADC_ConfigPinmux(config.rchn_sel);

    HAL_ADC_ConfigChannel(&g_ADCHandler, &config);

    while (cnt++ < CNT)
    {
        ret = HAL_ADC_ReadConvertValue(&g_ADCHandler, &ConvertData[0], TIMEOUT_WAIT_FOREVER);

        PR_DEBUG("channel:%d Convert Success Value:%d\n", config.rchn_sel, ConvertData[0]);
        if (ret == RET_TIMEOUT)
        {
            PR_ERR("error:%d ADC_ReadConvertValue NoBlock Mode\n", ret);
            goto out;
        }

        k_sleep(500);
    }
    PR_DEBUG("Exit func[%s] Test\n", __FUNCTION__);
    return TC_PASS;

out:
    return ret;
}

int adc_Injectedchannel_sync_demo(int adc_select)
{
    ADC_InjectionConfTypeDef config;
    GPIO_PinConfig_T GPIO_InitStruct;
    int ConvertData[16] = {0};
    int ret = 0;
    int cnt = 0;

    g_ADCHandler.device = ADC_DEV0;
    g_ADCHandler.Init.Resolution = ADC_RESOLUTION_12BIT;
    g_ADCHandler.Init.InjectedNbrOfConversion = USER_INJECT_SEQUECE_LEN;
    g_ADCHandler.Init.workMode = ADC_SINGLE_MODE;
    config.InjectedNbrOfConversion = g_ADCHandler.Init.InjectedNbrOfConversion;
    g_ADCHandler.Init.dma_en = DISABLE;
    g_ADCHandler.dma_rst_len = g_ADCHandler.Init.InjectedNbrOfConversion;
    g_ADCHandler.Init.timeOut = TIMEOUT_WAIT_FOREVER;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1; /*GPIOA0,GPIOA1*/
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = 0;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);

    PR_DEBUG("Enter func[%s] Test\n", __FUNCTION__);

    config.injectchn_sel[0] = ADC_CHANNEL_0;

    //ADC_ConfigPinmux(config.injectchn_sel[0]);
    HAL_ADC_Init(&g_ADCHandler);

    HAL_ADCEx_InjectedConfigChannel(&g_ADCHandler, &config);

    while (cnt++ < CNT)
    {
        ret = HAL_ADC_ReadInJectConvertValue(&g_ADCHandler, &ConvertData[0], TIMEOUT_WAIT_FOREVER);

        PR_DEBUG("InjectChannel:%d Convert Success Value:%d\n", config.injectchn_sel[0], ConvertData[0]);
        if (ret == RET_TIMEOUT)
        {
            PR_ERR("Err:%d ADC_ReadConvertValue NoBlock Mode\n", ret);
            goto out;
        }

        k_sleep(50);
    }

    PR_DEBUG("Exit func[%s] Test\n", __FUNCTION__);
    return TC_PASS;

out:

    return ret;
}

int adc_InjectedSequence_async_demo(int adc_select)
{
    ADC_InjectionConfTypeDef config;
    GPIO_PinConfig_T GPIO_InitStruct;
    int ConvertData[16] = {0};
    int ret = 0;
    int i = 0;
    int cnt = 0;

    g_ADCHandler.device = ADC_DEV0;
    g_ADCHandler.Init.Resolution = ADC_RESOLUTION_12BIT;
    g_ADCHandler.Init.InjectedNbrOfConversion = USER_INJECT_SEQUECE_LEN;
    g_ADCHandler.Init.workMode = ADC_SINGLE_MODE;

    g_ADCHandler.Init.dma_en = DISABLE;
    g_ADCHandler.dma_rst_len = g_ADCHandler.Init.InjectedNbrOfConversion;
    g_ADCHandler.Init.timeOut = TIMEOUT_WAIT_FOREVER;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1; /*GPIOA0,GPIOA1*/
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = 0;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);

    PR_DEBUG("Enter func[%s] Test\n", __FUNCTION__);

    config.InjectedNbrOfConversion = g_ADCHandler.Init.InjectedNbrOfConversion;

    config.injectchn_sel[0] = ADC_CHANNEL_0;
    config.injectchn_sel[1] = ADC_CHANNEL_1;
    config.injectchn_sel[2] = ADC_CHANNEL_2;
    config.injectchn_sel[3] = ADC_CHANNEL_3;
    config.InjectedSamplingTime = 0x1;

    //ADC_ConfigAllPinmux();
    HAL_ADC_Init(&g_ADCHandler);

    HAL_ADCEx_InjectedSequenceChannel(&g_ADCHandler, &config);

    while (cnt++ < CNT)
    {
        ret = HAL_ADC_ReadInJectConvertValue(&g_ADCHandler, &ConvertData[0], TIMEOUT_WAIT_FOREVER);
        if (ret == RET_TIMEOUT)
        {
            PR_ERR("Err:%d ADC_ReadConvertValue BLOCK MODE\n", ret);
            goto out;
        }
        for (i = 0; i < g_ADCHandler.Init.InjectedNbrOfConversion; i++)
        {
            PR_DEBUG("InjectChannel:%d Convert Success Value:%d\n", i, ConvertData[i]);
        }

        k_sleep(500);
    }
    PR_DEBUG("Exit func[%s] Test\n", __FUNCTION__);
    return TC_PASS;

out:
    return ret;
}

int adc_sequence_async_demo(int adc_select)
{
    ADC_ChannelConfTypeDef config;
    GPIO_PinConfig_T GPIO_InitStruct;
    int ConvertData[16] = {0};
    int ret = 0;
    int i = 0;
    int cnt = 0;

    g_ADCHandler.device = ADC_DEV0;
    g_ADCHandler.Init.Resolution = ADC_RESOLUTION_12BIT;
    g_ADCHandler.Init.NbrOfConversion = USER_SEQUECE_LEN;
    g_ADCHandler.Init.workMode = ADC_SINGLE_MODE;
    g_ADCHandler.Init.dma_en = DISABLE;
    g_ADCHandler.dma_rst_len = USER_SEQUECE_LEN;
    g_ADCHandler.Init.timeOut = TIMEOUT_WAIT_FOREVER;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1; /*GPIOA0,GPIOA1*/
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = 0;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);

    PR_DEBUG("Enter func[%s] Test\n", __FUNCTION__);

    HAL_ADC_Init(&g_ADCHandler);

    config.rchn_sel = ADC_CHANNEL_1;

    //ADC_ConfigAllPinmux();
    HAL_ADC_SequenceConfig(&g_ADCHandler, &config);

    while (cnt++ < CNT)
    {
        ret = HAL_ADC_ReadConvertValue(&g_ADCHandler, &ConvertData[0], TIMEOUT_WAIT_FOREVER);

        for (i = 0; i < USER_SEQUECE_LEN; i++)
        {
            PR_DEBUG("Channel:%d Convert Success Value:%d\n", i, ConvertData[i]);
        }
        if (ret == RET_TIMEOUT)
        {
            PR_ERR("error:%d ADC_ReadConvertValue TIMEOUT_WAIT_FOREVER Mode\n", ret);
            goto out;
        }

        k_sleep(500);
    }
    ADC_DeInit(ADC_DEV0, &g_ADCHandler);
    PR_DEBUG("Exit func[%s] Test\n", __FUNCTION__);
    return TC_PASS;

out:
    return ret;
}

void adc_test(void)
{
    while (1)
    {
        //  zassert_true(adc_InjectedSequence_async_demo(0) == TC_PASS, NULL);
        //  zassert_true(adc_channel_sync_demo(0) == TC_PASS, NULL);
        //  zassert_true(adc_sequence_async_demo(0) == TC_PASS, NULL);
        zassert_true(adc_Injectedchannel_sync_demo(0) == TC_PASS, NULL);
    }
}
