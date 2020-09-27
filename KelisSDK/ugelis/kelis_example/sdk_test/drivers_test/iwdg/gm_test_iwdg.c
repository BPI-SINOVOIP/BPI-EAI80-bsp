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
#include "gm_test_iwdg.h"

#define   RESET_TIME  1000

#define SYS_DB_CTRL                         (*(volatile uint32_t *)(0x40000058))
#define IWDG_DB_CTRL                        1
#define IWDG_STOP_DB_CTRL                   0


/* Debug Ctrl Reg about wdog stoped and runned when cpu halted and running*/
static void SysDebugCtlReg(uint32_t SetVal)
{
    /*
        In case 1 CPU halt,
        SYS_TICK will not active and WDG should stop counting and continue
        while recover from halt condition;
        ==>DBG_WDOG_STOP == 1'b0, WDT continue running;
        ==>DBG_WDOG_STOP == 1'b1, Debug mode ,WDT stoped, you can debug software;
        In case 2 CPU runing,
        SYS_TICK clock will active and WDG counter should continue running;
    */
    TC_PRINT("\rDBGCTRL:0X%X\n", SYS_DB_CTRL);
    if (SetVal != 0)
    {
        SYS_DB_CTRL |= SetVal;
        TC_PRINT("\rSet DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
    else
    {
        SYS_DB_CTRL &= ~1;
        TC_PRINT("\rClear DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
}

static int test_iwdg(void)
{
    int err;
    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    unsigned int i = 5;

    SysDebugCtlReg(IWDG_DB_CTRL);
    wdt_dev = device_get_binding("IWDG");
    if (!wdt_dev)
    {
        TC_PRINT("Cann't get GM-IWDG Device\n");
        return TC_FAIL;
    }
    TC_PRINT("Testcase: %s,sysclk:%d,date:%s\n", __func__, SYS_CLOCK, __DATE__);
    if ((*(volatile uint32_t *)(0x40000004) & (1 << 5)) == (1 << 5))
    {
        *(volatile uint32_t *)(0x40000004) |= (1 << 5);
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }

    /*clear reset iwdg  status bit*/
    *(volatile uint32_t *)(0x40000004) |= (1 << 5);
    TC_PRINT("IWDG status bit5:0x%x\n", (*(volatile uint32_t *)(0x40000004) & 0xFF));

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.mode = WDT_MODE_RESET;
    wr_cfg.clkdiv = IWDG_CLKDIV_FACTOR_256;
    wr_cfg.interrupt_fn = NULL;

    err = wdt_set_config(wdt_dev, &wr_cfg);
    if (err < 0)
    {
        TC_PRINT("IWDG install error\n");
    }
    wdt_get_config(wdt_dev, &wr_cfg);

    wdt_enable(wdt_dev);

    do
    {
        wdt_reload(wdt_dev);
        k_sleep(800);
        TC_PRINT("feediwdg\n");
    } while (i--);
    TC_PRINT("Waiting to restart MCU\n");
    while (1)
    {
        k_yield();
    }


}

void gm_test_iwdg(void)
{
    zassert_true(test_iwdg() == TC_PASS, NULL);
}
