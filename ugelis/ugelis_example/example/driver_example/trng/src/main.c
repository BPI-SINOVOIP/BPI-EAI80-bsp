/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file            main.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 *
 * @date           2019/03/18
 *
 * @brief           Sample code of trng Driver.
 *
 * @note
 *    2019/03/18, terry.wu, V1.0.0
 *        Initial version.
 *
 */

#include <ugelis.h>
#include <entropy.h>
#include <stdio.h>

/* TRNG of GM6721 series genetate 192bit each time */
#define BUFFER_LENGTH 5

void main(void)
{
    int i = 0;
    int cnt = 0;
    struct device *dev;
    u32_t buffer[BUFFER_LENGTH] = {0};
    int ret;

    printk("Entropy Example start! %s/%s\n", CONFIG_ARCH, CONFIG_SOC);
    dev = device_get_binding(CONFIG_ENTROPY_NAME);
    if (!dev)
    {
        printk("error: no entropy device\n");
        return;
    }

    printk("entropy device is %p, name is %s\n",
           dev, dev->config->name);

    while (cnt < 10)
    {
        ret = entropy_get_entropy(dev, buffer, BUFFER_LENGTH * sizeof(u32_t));
        if (ret)
        {
            printk("entropy_get_entropy failed: %d\n", ret);
            break;
        }

        for (i = 0; i < BUFFER_LENGTH; i++)
        {
            printk("0x%x  ", buffer[i]);
        }
        printk("\n");

        cnt++;
        k_sleep(500);
    }

    printk("Entropy Example end!");
}



