/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>


int main(void)
{
    printk("Enter Slave...n");
    while (1)
    {
        k_sleep(1);
    }

    return 0;
}

