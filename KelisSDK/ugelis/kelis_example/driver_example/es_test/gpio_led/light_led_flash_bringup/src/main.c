/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>

/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOC"

#define SW0_GPIO_PIN   8    /*PC8*/

static int cnt = 0;

struct device *dev;
static struct gpio_callback gpio_cb;


void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    printk("Button pressed\n");
    /*gpio_pin_write(dev, LED_GPIO_PIN, cnt % 2);*/
}


void main(void)
{
    struct device *gpiob;
    int i = 0;
    uint32_t val = 0;

    printk("\n led and key demo \n");
    dev = device_get_binding(PORT);
    gpiob = dev;

    printk("key dev address =0x%x\n", dev);

    /* Set key pin as interrupt active flalling mode */
    gpio_pin_configure(dev, SW0_GPIO_PIN, GPIO_INT | PULL_UP | GPIO_INT_EDGE | GPIO_INT_ACTIVE_FALLING);
    gpio_init_callback(&gpio_cb, button_pressed, BIT(SW0_GPIO_PIN));

    gpio_add_callback(gpiob, &gpio_cb);
    gpio_pin_enable_callback(gpiob, SW0_GPIO_PIN);

    while (1)
    {
        gpio_pin_read(dev, SW0_GPIO_PIN, &val);
        printk("key vale = [%d]\n", val);
        k_sleep(1000);
    }
}
