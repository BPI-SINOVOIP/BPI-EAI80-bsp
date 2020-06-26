/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          gpio.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Gpio Reader Interface
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <gm_hal_gpio.h>
#include <gpio.h>

#include "dev/gpio.h"

#define PORT_B    "GPIOB"

#if defined(LQFP176_DEMO_BOARD)
    #define SW0_INPUT_GPIO_PIN   8
#elif defined(F6721B_EVB_QFN80PIN)
    #define SW0_INPUT_GPIO_PIN   13
#else
    #define SW0_INPUT_GPIO_PIN   -1
#endif

#define CNT 2
dev_gpio_cb_t *gGpioCb = NULL;
static inline dev_gpio_cb_t *gpio_get_cb()
{
    return gGpioCb;
}

static void gpio_set_cb(dev_gpio_cb_t *gpio_cb)
{
    gGpioCb = gpio_cb;
}

static void gpio_timer_handler(struct k_timer *timer)
{
    dev_gpio_cb_t *user_cb = gpio_get_cb();
    printf("gpio_timer_handler\n");
    user_cb->long_press = 1;
}

static void button_pressed(struct device *dev, struct gpio_callback *cb,
                           uint32_t pins)
{
    static int32_t last_val = 1;
    int32_t val = 0;
    dev_gpio_cb_t *user_cb = gpio_get_cb();
    //printf("Button pressed\n");
    gpio_pin_read(dev, SW0_INPUT_GPIO_PIN, (u32_t *)&val);
    //printf("key value = [%d]\n", val);

    if ((val - last_val) == -1)//0:pressed
    {
        printf("press_start\n");
        user_cb->long_press = 0;
        k_timer_start(&user_cb->timer, 300, 0);
    }
    else if ((val - last_val) == 1)
    {
        k_timer_stop(&user_cb->timer);
        if (user_cb->long_press)
        {
            user_cb->func(GPIO_KEY_LONGPRESS, user_cb->arg);
        }
        else
        {
            user_cb->func(GPIO_KEY_SHORTPRESS, user_cb->arg);
        }
    }
    else if (val - last_val == 0)
    {
        //printf("invalid press\b");
    }
    last_val = val;
}

int gpio_init(dev_gpio_t *gpio_dev)
{
    struct device *dev;
    static struct gpio_callback gpio_cb;

    if (SW0_INPUT_GPIO_PIN < 0)
    {
        printf("unsupported gpio key on this platform\n");
        return -1;
    }

    gpio_set_cb(&gpio_dev->gpio_cb);
    dev = device_get_binding(PORT_B);
    if (dev == NULL)
    {
        printf("gpio init failed\n");
        return -1;
    }
    k_timer_init(&gpio_dev->gpio_cb.timer, gpio_timer_handler, NULL);

    /* Set key pin as interrupt active flalling mode */
    gpio_pin_configure(dev, SW0_INPUT_GPIO_PIN, GPIO_INT | GPIO_PUD_PULL_DOWN | GPIO_MODE_IT_LOW);
    gpio_init_callback(&gpio_cb, button_pressed, BIT(SW0_INPUT_GPIO_PIN));

    gpio_add_callback(dev, &gpio_cb);
    gpio_pin_enable_callback(dev, SW0_INPUT_GPIO_PIN);

    return 0;
}
