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


#define CONFIG_WWDG_GM_FPGA   1

#if CONFIG_WWDG_GM_FPGA
extern void wwdg_test();

#else

#define RESET_TIME     5000
#define WINDOW_TIME    3000

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_example_cb(WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param)
{
    printk("Waiting to return main\n");
}
#endif

void main(void)
{

#if CONFIG_WWDG_GM_FPGA
    wwdg_test();
#else
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
    wr_cfg.workmode = WWDG_NORMAL_EWI;//WWDG_NORMAL_MODE; //WWDG_NORMAL_EWI;
    wr_cfg.clkdiv = WWDG_CLKDIV_FACTOR_32;

    if ((wr_cfg.workmode == WWDG_NORMAL_EWI) || (wr_cfg.workmode == WWDG_WINDOW_EWI))
    {
        wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
        wr_cfg.winLen = 15;             /*normal_ewi or window_ewi mode,it must set winlen*/
        wr_cfg.interrupt_fn = wdt_example_cb;
    }
    else
    {
        wr_cfg.mode = WDT_MODE_RESET;
        wr_cfg.winLen = 0;              /*normal or window mode,it may set winlen*/
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
#endif /*CONFIG_WWDG_GM_FPGA*/

}

