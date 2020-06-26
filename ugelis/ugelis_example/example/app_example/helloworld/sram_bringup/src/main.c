/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>

void main(void)
{
    while (1)
    {
        printk("SRAM BringUp:Hello World! ARCH:%s BOARD:%s \n", CONFIG_ARCH, CONFIG_BOARD);
        k_sleep(1000);
    }

}
