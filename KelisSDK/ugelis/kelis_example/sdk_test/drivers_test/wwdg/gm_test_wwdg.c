/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <device.h>
#include <watchdog.h>
#include "board.h"
#include <ztest.h>
#include "gm_test_wwdg.h"


#define WWDT_TEST_CB0_TEST_VALUE    0x0CB0

#define RESET_TIME     CONFIG_WWDG_GM_RELOAD_COUNTER
#define WINDOW_TIME    CONFIG_WWDG_GM_WINDOW_VALUE


/* m_testcase_index is incremented after each test to make test possible
 * switch to next testcase.
 */
volatile static uint32_t m_testcase_index = 0;

/* m_testvalue contains value set in interrupt callback to point whether
 * first or second interrupt was fired.
 */
volatile static uint32_t m_testvalue  = 0;

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_example_cb0(WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param)
{
    ARG_UNUSED(hWDOG);
    m_testvalue = WWDT_TEST_CB0_TEST_VALUE;
    TC_PRINT("Waiting to return main\n");

}
static int test_wwdg_normal(void)
{
    int err;
    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    static unsigned int test_cnt = 5;

    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        TC_PRINT("Cann't get GM-WWDG Device\n");
        return TC_FAIL;
    }
    TC_PRINT("Testcase: %s\n", __func__);
    if ((*(volatile uint32_t *)(0x40000004) & 0x02) == 0x02)
    {
        TC_PRINT("WWDG status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));
        *(volatile uint32_t *)(0x40000004) |= (1 << 1);
        wdt_disable(wdt_dev);
        m_testcase_index++;
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    *(volatile uint32_t *)(0x40000004) |= (1 << 1);
    TC_PRINT("WWDG status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.windowtimeout = WINDOW_TIME;
    wr_cfg.mode = WDT_MODE_RESET;
    wr_cfg.workmode = WWDG_NORMAL_MODE;
    wr_cfg.clkdiv = CONFIG_WWDG_GM_DIV_FACTOR;
    wr_cfg.winLen = CONFIG_WWDG_GM_WINLEN;
    wr_cfg.interrupt_fn = NULL;
    err = wdt_set_config(wdt_dev, &wr_cfg);
    if (err < 0)
    {
        TC_PRINT("WWDG install error\n");
    }
    wdt_enable(wdt_dev);
    do
    {
        TC_PRINT("waiting for .....!\n");
        k_sleep(800);
        wdt_reload(wdt_dev);
    } while (test_cnt--);
    TC_PRINT("Waiting to restart MCU\n");
    while (1)
    {
        k_yield();
    }
}
static int test_wwdg_normal_ewi(void)
{
    int err;
    struct wdt_config wr_cfg;
    struct device *wdt_dev;

    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        TC_PRINT("Cann't get GM-WWDG Device\n");
        return TC_FAIL;
    }
    TC_PRINT("Testcase: %s\n", __func__);

    *(volatile uint32_t *)(0x40000004) |= (1 << 1);
    TC_PRINT("wwdg status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.windowtimeout = WINDOW_TIME;
    wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
    wr_cfg.workmode = WWDG_NORMAL_EWI;
    wr_cfg.clkdiv = CONFIG_WWDG_GM_DIV_FACTOR;
    wr_cfg.winLen = CONFIG_WWDG_GM_WINLEN;
    wr_cfg.interrupt_fn = wdt_example_cb0;
    err = wdt_set_config(wdt_dev, &wr_cfg);
    if (err < 0)
    {
        TC_PRINT("Watchdog install error\n");
        return TC_FAIL;
    }
    wdt_enable(wdt_dev);

    TC_PRINT("Waiting to enter into wwdg interrupt\n");
    m_testvalue = 0;
    do
    {
        k_yield();
    } while (m_testvalue != WWDT_TEST_CB0_TEST_VALUE);
    if (m_testvalue == WWDT_TEST_CB0_TEST_VALUE)
    {
        wdt_disable(wdt_dev);
        m_testcase_index++;
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    wdt_disable(wdt_dev);
    return TC_FAIL;
}
void gm_test_wwdg_normal(void)
{

    if (m_testcase_index == 0)
    {
        zassert_true(test_wwdg_normal() == TC_PASS, NULL);
    }
    if (m_testcase_index == 1)
    {
        zassert_true(test_wwdg_normal_ewi() == TC_PASS, NULL);
    }
    if (m_testcase_index > 1)
    {
        m_testcase_index = 0;
        m_testvalue = 0;
    }

}
