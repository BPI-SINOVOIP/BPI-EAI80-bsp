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


#define RESET_TIME    CONFIG_WWDG_GM_RELOAD_COUNTER
#define WINDOW_TIME   CONFIG_WWDG_GM_WINDOW_VALUE

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_example_cb(WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param)
{
    ARG_UNUSED(hWDOG);
    printk("Waiting to return main\n");
}

void main(void)
{

    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    uint32_t  cnt = 0;

    cnt = 5;

    printf("wwdg status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));
    /*clear reset iwdg  status bit*/
    *(volatile uint32_t *)(0x40000004) |= (1 << 1);
    printf("clear wwdg status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));

    printk("Start wwdg test,sysclk:%d\n", SYS_CLOCK);
    k_sleep(1000);

    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        printk("Cann't get GM-WWDG Device\n");
    }

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.windowtimeout = WINDOW_TIME;
    wr_cfg.workmode = CONFIG_WWDG_GM_MODE;
    wr_cfg.clkdiv = CONFIG_WWDG_GM_DIV_FACTOR;

    if (wr_cfg.workmode == WWDG_NORMAL_EWI)
    {
        wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
        wr_cfg.winLen = CONFIG_WWDG_GM_WINLEN;         /*normal_ewi or window_ewi mode,it must set winlen*/
        wr_cfg.interrupt_fn = wdt_example_cb;
    }
    else
    {
        wr_cfg.mode = WDT_MODE_RESET;
        wr_cfg.winLen = CONFIG_WWDG_GM_WINLEN;        /*normal or window mode,it may set winlen*/
        wr_cfg.interrupt_fn = NULL;
    }
    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_enable(wdt_dev);


    do
    {
        printk("waiting for .....!\n");
        k_sleep(1000);
        /*if you already had configed WWDG support EWI interrupt
          in makemenuconfig,the flow cann't run */
        printk("feedwdg\n");
        wdt_reload(wdt_dev);

    } while (cnt--);
    printk("waiting for wwdg reset!\n");
    while (1);
}

