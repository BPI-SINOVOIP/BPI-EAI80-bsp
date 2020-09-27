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

#ifndef __DEV_TOUCHPAD_H__
#define __DEV_TOUCHPAD_H__

#include <display/touch.h>

typedef int (*touch_cb_func)(int cmd, void *arg);

typedef struct dev_touch_cb
{
    struct k_timer timer;
    int long_press;
    touch_cb_func func;
    void *arg;
} dev_touch_cb_t;

typedef struct dev_touch
{
    struct device *gt9147;
    dev_touch_cb_t touch_cb;
    struct k_thread task_thread;
} dev_touch_t;

int touchpad_init(dev_touch_t *gpio_dev);

#endif //__DEV_TOUCHPAD_H__