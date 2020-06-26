/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>

#include <device.h>
#include <watchdog.h>
#include "board.h"
#include <misc/printk.h>

#define   reset_time  1000

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
    printk("\rDBGCTRL:0X%X\n", SYS_DB_CTRL);
    if (SetVal != 0)
    {
        SYS_DB_CTRL |= SetVal;
        printk("\rSet DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
    else
    {
        SYS_DB_CTRL &= ~1;
        printk("\rClear DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
}

void main(void)
{
    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    unsigned int i = 5;

    //  k_sleep(2000);

    printf("iwdg status bit5:0x%x\n", (*(volatile uint32_t *)(0x40000004) & 0xFF));
    /*clear reset iwdg  status bit*/
    *(volatile uint32_t *)(0x40000004) |= (1 << 5);
    printf("iwdg status bit5:0x%x\n", (*(volatile uint32_t *)(0x40000004) & 0xFF));

    SysDebugCtlReg(IWDG_DB_CTRL);
    printk("Start iwdg test,sysclk:%d,date:%s\n", SYS_CLOCK, __DATE__);
    wdt_dev = device_get_binding("IWDG");
    if (!wdt_dev)
    {
        printk("Cann't get GM-IWDG Device\n");
    }

    wr_cfg.timeout = reset_time;
    wr_cfg.mode = WDT_MODE_RESET;
    wr_cfg.clkdiv = IWDG_CLKDIV_FACTOR_256;
    wr_cfg.interrupt_fn = NULL;

    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_get_config(wdt_dev, &wr_cfg);

    wdt_enable(wdt_dev);

    while (1)
    {
        if (i--)
        {
            wdt_reload(wdt_dev);
            k_sleep(800);
            printf("feediwdg\n");
        }
        if (i == 0)
        {
            printk("waiting for iwdg reset!\r\n");
            while (1);
        }
    }

}
