/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <kernel_structs.h>

#include <misc/printk.h>
#include <pm.h>


#define PM_DEAD_LOOP()  do{}while(1)


void main(void)
{
    int ret = 0;

    while (1)
    {

        printk("###sys entr low power mode###\n");
        k_sleep(3000);
        pm_config_rtc_wakeup_time(0);
        ret = pm_entry_low_power(SHUTDOWN_MODE);
        if (ret)
        {
            printk("Err:%d pm_entry_lower_power\n", ret);
            PM_DEAD_LOOP();
        }
        printk("resume back!!\n");
        k_sleep(3000);

    }



}
