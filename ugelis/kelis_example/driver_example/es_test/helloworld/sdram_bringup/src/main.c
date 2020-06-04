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
        printk("SDRAM BringUp:Hello World! %s\n", CONFIG_ARCH);
        k_sleep(1000);
    }

}
