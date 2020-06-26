/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          event_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of Gpio worker.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __EVENT_WORKER_H__
#define __EVENT_WORKER_H__

#include <dev/gpio.h>

typedef struct event_worker
{
    dev_gpio_t gpio_dev;
} event_worker_t;

#endif //__EVENT_WORKER_H__
