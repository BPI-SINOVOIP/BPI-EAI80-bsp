/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_test_btm.c
 *
 * @author      Edward.Zhu
 *
 * @version     v1.0.0
 *
 * @date        2019/11/27
 *
 * @brief       btm demo source file
 *
 * @note
 *              2019/11/27, Edward.Zhu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <board.h>
#include <ztest.h>

#include "gm_hal_test_btm.h"
#include <gm_common.h>
#include <gm_hal_btm.h>

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

static int btm_counter_prescaler(void)
{
    RET_CODE ret = RET_OK;
    int counter_id = 1;
    uint32_t cnt_before_decrease = 0;
    uint32_t current_cnt = 0;
    uint32_t cnt_before_increase = 0;
    float delta_time = 0;

    BTM_Config_T btmConfig;

    TC_PRINT("Testcase: %s\n", __func__);

    btmConfig.counterCallback = NULL;

    HAL_BTM_Init(&btmConfig);

    ret = HAL_BTM_SetCounter(counter_id, 0xFFFFFFFF);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    cnt_before_decrease = HAL_BTM_GetCurrentCount(counter_id);

    ret = HAL_BTM_StartCounter(counter_id);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    k_sleep(275);
    current_cnt = HAL_BTM_GetCurrentCount(counter_id);
    delta_time = 1.0 * (cnt_before_decrease - current_cnt) / HAL_BTM_GetFrequence(counter_id);
    TC_PRINT("interval 1 time %.3fs\n", delta_time);

    ret = HAL_BTM_StopCounter(counter_id);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    counter_id = 0;

    ret = HAL_BTM_SetCounter(counter_id, 0xFFFFFFFF);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    ret = HAL_BTM_SetCounterPrescaler(counter_id, BTM1_CLK_DIV_16);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    cnt_before_increase = HAL_BTM_GetCurrentCount(counter_id);

    ret = HAL_BTM_StartCounter(counter_id);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    k_sleep(325);
    current_cnt = HAL_BTM_GetCurrentCount(counter_id);
    delta_time = 1.0 * (current_cnt - cnt_before_increase) / HAL_BTM_GetFrequence(counter_id);
    TC_PRINT("interval 2 time %.3fs\n", delta_time);

    ret = HAL_BTM_StopCounter(counter_id);
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    TC_PRINT("btm counter and prescaler function test ok!");

    return TC_PASS;

}

static int test_btm_counter_prescaler(void)
{
    RET_CODE ret = RET_OK;

    ret = btm_counter_prescaler();
    if (ret != RET_OK)
    {
        TC_PRINT("Line = %d, Func:%s \n", __LINE__, __func__);
        return TC_FAIL;
    }

    return TC_PASS;
}

void gm_hal_test_btm(void)
{
    TC_PRINT("================BTM Test Start=================\n\n\n");

    zassert_true(test_btm_counter_prescaler() == TC_PASS, NULL);

    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================BTM Test  End==================\n\n\n");
}
