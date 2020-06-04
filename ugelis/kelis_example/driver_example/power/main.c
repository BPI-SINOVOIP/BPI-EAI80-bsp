/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>
#include <power.h>
#include <gm_hal_reset.h>


#define DEAD_LOOP() do{                 \
                        k_sleep(1000);  \
                      }while(1)

static void SysTick_Disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

static void SysTick_Enable(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

int main()
{
    int ret = 0;
    struct device * dev;

    dev = device_get_binding(DEV_POWER_NAME);
    if(!dev){
        printk("Main Err: device_get_binding\n");
        return ret;
    }

    if(HAL_Reset_CheckRestCause(RESET_STATUS_STANDBY_MODE) == false){

        ret = pm_config_wakeup_pin(dev, PM_PIN_WAKEUP_STADDBY_MODE);
        if(ret){
            printk("Err:%d pm_config_wakeup_pin\n",ret);
            goto out;
        }
        SysTick_Disable();

        printk("sys enter lower power now\n");
        ret = pm_enter_low_power(dev, PM_STADDBY_MODE);
        if(ret){
            printk("Err:%d pm_config_wakeup_pin\n",ret);
            goto out;
        }
        SysTick_Enable();
        printk("resume from not reset mode\n");
    }else{
        printk("resume from low power reset mode\n");

    }

    DEAD_LOOP();



out:
    return ret;
}





