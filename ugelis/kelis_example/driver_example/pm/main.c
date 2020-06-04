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
 * @brief       Sample code for adc test case.
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <misc/printk.h>
#include <pm.h>

#define PM_DEAD_LOOP()  do{}while(1)

extern void pm_config_rtc_wakeup_time(uint32_t sec);
extern int pm_entry_low_power(uint8_t mode);

void main(void)
{
    int ret = 0;
    while (1)
    {
        printk("SDRAM BringUp:Hello World!\n");
        k_sleep(3000);
        pm_config_rtc_wakeup_time(5);
        ret = pm_entry_low_power(STANDBY_MODE);
        if (ret)
        {
            printk("Err:%d pm_entry_lower_power\n", ret);
            PM_DEAD_LOOP();
        }
        printk("resume back!!\n");

    }

}

