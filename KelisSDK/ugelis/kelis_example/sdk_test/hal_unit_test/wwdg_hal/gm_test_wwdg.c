/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <device.h>
#include <gm_hal_wwdg.h>
#include "board.h"
#include <ztest.h>
#include "gm_test_wwdg.h"


#define WWDT_TEST_CB0_TEST_VALUE    0x0CB0

#define RESET_TIME       5000
#define WINDOW_TIME    3000

WDOG_Handle_T pHWWDG;
/* m_testcase_index is incremented after each test to make test possible
 * switch to next testcase.
 */
volatile static uint32_t m_testcase_index = 0;

/* m_testvalue contains value set in interrupt callback to point whether
 * first or second interrupt was fired.
 */
volatile static uint32_t m_testvalue  = 0;

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_example_cb(WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param)
{
    ARG_UNUSED(hWDOG);
    m_testvalue = WWDT_TEST_CB0_TEST_VALUE;
    TC_PRINT("Waiting to return main\n");

}

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_example_win_cb(WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param)
{
    uint32_t nu = 1;
    WDOG_Device_T *pWWDG = NULL;
    ARG_UNUSED(hWDOG);
    m_testvalue = WWDT_TEST_CB0_TEST_VALUE;

    pWWDG = hWDOG->Instance;
    TC_PRINT("WIN_EMI,during wdog int,repeat feedwdog as 3 times\n");

    while (HAL_WDOG_GetCntOutWinFlag(hWDOG) == 0x04);
    do
    {
        if ((HAL_WDOG_GetCountVal(hWDOG) < pWWDG->WL)
                && (pWWDG->WL < pWWDG->LD))
        {
            TC_PRINT("\r FeedWdog cnt:%d \n", nu++);
            HAL_WDOG_FeedWdog(hWDOG);
        }

        if (nu > 3)
        {
        }
    } while (nu <= 3);
    TC_PRINT("Waiting to return main\n");

}

static int test_wwdg_normal(void)
{
    int err;
    WDOG_Device_T *pWWDG  = WWDG;
    WDOG_Config_T wwdg_config;
    uint32_t  cnt = 5;

    if ((*(volatile uint32_t *)(0x40000004) & 0x02) == 0x02)
    {
        TC_PRINT("WWDG status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));
        *(volatile uint32_t *)(0x40000004) |= (1 << 1);
        HAL_WDOG_Disable(&pHWWDG);
        m_testcase_index++;
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    TC_PRINT("Testcase: %s,date:%s,time:%s\n", __func__, __DATE__, __TIME__);
    *(volatile uint32_t *)(0x40000004) |= (1 << 1);
    TC_PRINT("after clear,WWDG status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));

    wwdg_config.resetTime = RESET_TIME;
    wwdg_config.windowTime = WINDOW_TIME;
    wwdg_config.workEwiMode = WDOG_NORMAL_MODE;
    wwdg_config.clkDivFactor = HAL_WWDG_CLKDIV_FACTOR_32;
    wwdg_config.winLen = HAL_WWDG_WINLIN_15;
    if ((wwdg_config.workEwiMode & WDOG_NORMAL_EWI) == WDOG_NORMAL_EWI)
    {
        wwdg_config.callBack = wdt_example_cb;
        HAL_IRQ_ENABLE(WDOG_IRQn);
    }
    else
    {
        wwdg_config.callBack = NULL;
    }
    err = HAL_WDOG_Init(pWWDG, &wwdg_config, &pHWWDG);
    if (err < 0)
    {
        TC_PRINT("WWDG install error\n");
        return TC_FAIL;
    }
    HAL_WDOG_Enable(&pHWWDG);

    do
    {
        TC_PRINT("waiting for .....!\n");
        HAL_WDOG_FeedWdog(&pHWWDG);
    } while (cnt--);
    TC_PRINT("Waiting to restart MCU\n");
    while (1)
    {
    }
}
static int test_wwdg_normal_ewi(void)
{
    int err;
    WDOG_Device_T *pWWDG  = WWDG;
    WDOG_Config_T wwdg_config;
    uint32_t  cnt = 5;

    TC_PRINT("Testcase: %s\n", __func__);

    wwdg_config.resetTime = RESET_TIME;
    wwdg_config.windowTime = WINDOW_TIME;
    wwdg_config.workEwiMode = WDOG_NORMAL_EWI;
    wwdg_config.clkDivFactor = HAL_WWDG_CLKDIV_FACTOR_32;
    wwdg_config.winLen = HAL_WWDG_WINLIN_15;
    if ((wwdg_config.workEwiMode & WDOG_NORMAL_EWI) == WDOG_NORMAL_EWI)
    {
        wwdg_config.callBack = wdt_example_cb;
    }
    else
    {
        wwdg_config.callBack = NULL;
    }
    err = HAL_WDOG_Init(pWWDG, &wwdg_config, &pHWWDG);
    if (err > 0)
    {
        TC_PRINT("WWDG install error\n");
        return TC_FAIL;
    }
    HAL_WDOG_Enable(&pHWWDG);

    m_testvalue = 0;
    TC_PRINT("Waiting to enter into wwdg interrupt\n");
    do
    {

    } while (m_testvalue != WWDT_TEST_CB0_TEST_VALUE);
    if (m_testvalue == WWDT_TEST_CB0_TEST_VALUE)
    {
        HAL_WDOG_Disable(&pHWWDG);
        m_testcase_index++;
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    HAL_WDOG_Disable(&pHWWDG);
    TC_PRINT("wwdg interrupt failed\n");
    return TC_FAIL;
}

static int test_wwdg_win_ewi(void)
{
    int err;
    WDOG_Device_T *pWWDG  = WWDG;
    WDOG_Config_T wwdg_config;

    TC_PRINT("Testcase: %s\n", __func__);

    wwdg_config.resetTime = RESET_TIME;
    wwdg_config.windowTime = WINDOW_TIME;
    wwdg_config.workEwiMode = WDOG_WINDOW_EWI;
    wwdg_config.clkDivFactor = HAL_WWDG_CLKDIV_FACTOR_32;
    wwdg_config.winLen = HAL_WWDG_WINLIN_15;
    if ((wwdg_config.workEwiMode & WDOG_WINDOW_EWI) == WDOG_WINDOW_EWI)
    {
        wwdg_config.callBack = wdt_example_win_cb;
    }
    else
    {
        wwdg_config.callBack = NULL;
    }
    err = HAL_WDOG_Init(pWWDG, &wwdg_config, &pHWWDG);
    if (err > 0)
    {
        TC_PRINT("WWDG install error\n");
        return TC_FAIL;
    }
    HAL_WDOG_Enable(&pHWWDG);


    while (HAL_WDOG_GetCntOutWinFlag(&pHWWDG) == 0x04);
    if ((HAL_WDOG_GetCountVal(&pHWWDG) < pWWDG->WL) && (pWWDG->WL < pWWDG->LD))
    {
        TC_PRINT("FeedWdog in open aera\n");
        HAL_WDOG_FeedWdog(&pHWWDG);
    }
    m_testvalue = 0;
    TC_PRINT("Waiting to enter into wwdg interrupt\n");
    do
    {

    } while (m_testvalue != WWDT_TEST_CB0_TEST_VALUE);
    if (m_testvalue == WWDT_TEST_CB0_TEST_VALUE)
    {
        HAL_WDOG_Disable(&pHWWDG);
        m_testcase_index++;
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    HAL_WDOG_Disable(&pHWWDG);
    TC_PRINT("wwdg interrupt failed\n");
    return TC_FAIL;
}

void gm_test_wwdg(void)
{

    if (m_testcase_index == 0)
    {
        zassert_true(test_wwdg_normal() == TC_PASS, NULL);
    }
    if (m_testcase_index == 1)
    {
        zassert_true(test_wwdg_normal_ewi() == TC_PASS, NULL);
    }
    if (m_testcase_index == 2)
    {
        zassert_true(test_wwdg_win_ewi() == TC_PASS, NULL);
    }

    if (m_testcase_index > 1)
    {
        m_testcase_index = 0;
        m_testvalue = 0;
    }

}
