/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2020/02/17
 *
 * @brief       The demo of touchpanel usage.
 *
 * @note
 *              2020/02/17, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <ts.h>

int main(void)
{
    struct device *touch = device_get_binding(GT911_DEV_NAME);

    int ret = 0;
    static uint32_t cnt = 0;
    struct ts_input_event event;
    while (1)
    {
        ret = read_ts_event(&event, K_FOREVER);
        if (!ret)
        {
            if (event.type == ABS_XY)
            {
                printk("\n###########Touch Screen Action:%d################\n", ++cnt);
                if (event.press_status == TS_PRESS)
                {
                    /*ts press*/
                    printk("ts prss: x_abs:%d y_abs:%d\n", event.x_abs, event.y_abs);
                }
                else
                {
                    /*ts release*/
                    printk("ts release\n");
                }
            }
        }
    }

    return 0;
}