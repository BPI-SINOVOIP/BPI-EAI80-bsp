/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <key.h>
#include <input.h>
#include <key/gpio_key.h>


void main(void)
{

    int ret = 0;
    static uint32_t cnt = 0;
    struct key_input_event event;

    while (1)
    {

        ret = read_key_event(&event, K_FOREVER);
        if (!ret)
        {
            printk("\n\n\n");
            printk("###########Key Action:%d################\n", ++cnt);
            /*code*/
            if (event.code == KEY_UP)
            {
                printk("CODE:KEY_UP \n");
            }
            else if (event.code == KEY_0)
            {
                printk("CODE:KEY_0\n");
            }
            else if (event.code == KEY_1)
            {
                printk("CODE:KEY_1 \n");
            }
            else if (event.code == KEY_2)
            {
                printk("CODE:KEY_2 \n");
            }
            else
            {
                printk("########type code:%d#######\n", event.code);
            }

            /*status*/
            if (event.status == KEY_RELEASE)
            {
                printk("STATUS:KEY_RELEASE\n");
            }
            else
            {
                printk("STATUS:KEY_PRESS\n");
            }
            /*type*/
            if (event.type == EV_KEY)
            {
                printk("TYPE:KEY\n");
            }
            else
            {
                printk("########type error:%d#######\n", event.type);
            }
        }

    }

    test();

}



