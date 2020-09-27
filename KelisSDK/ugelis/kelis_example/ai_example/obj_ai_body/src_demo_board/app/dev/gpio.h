/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          gpio.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of Gpio reader
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __DEV_GPIO_H__
#define __DEV_GPIO_H__

enum
{
    GPIO_KEY_SHORTPRESS = 0,
    GPIO_KEY_LONGPRESS,
};

typedef int (*gpio_cb_func)(int cmd, void *arg);

typedef struct dev_gpio_cb
{
    struct k_timer timer;
    int long_press;
    gpio_cb_func func;
    void *arg;
} dev_gpio_cb_t;

typedef struct dev_gpio
{
    dev_gpio_cb_t gpio_cb;
} dev_gpio_t;

int gpio_init(dev_gpio_t *gpio_dev);

#endif //__DEV_GPIO_H__
