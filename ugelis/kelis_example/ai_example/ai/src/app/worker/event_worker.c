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
 * @brief         Gpio worker interface.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <kernel.h>
#include <ugelis.h>

#include "event_worker.h"
#include "post_worker.h"
#include "worker.h"

static int event_worker_cb(int cmd, void *arg)
{
    worker_t *self = (worker_t *)arg;
    if (cmd == GPIO_KEY_LONGPRESS)
    {
        do_next_process(self, CMD_EVENT_LONGPRESS, 0);
    }
    else if (cmd == GPIO_KEY_SHORTPRESS)
    {
        do_next_process(self, CMD_EVENT_SHORTPRESS, 0);
    }
    return 0;
}

static int event_worker_init(worker_t *self, void *param)
{
    event_worker_t *event = (event_worker_t *)kmalloc(sizeof(event_worker_t));
    event->gpio_dev.gpio_cb.arg  = self;
    event->gpio_dev.gpio_cb.func = event_worker_cb;
    gpio_init(&event->gpio_dev);

    //event->touch_dev.touch_cb.arg  = self;
    //event->touch_dev.touch_cb.func = event_worker_cb;
    //touchpad_init(&event->touch_dev);
    self->data = event;
    return 0;
}

static int event_worker_deinit(worker_t *self)
{
    kfree(self->data);
    return 0;
}

worker_t event_worker =
{
    .init   = event_worker_init,
    .deinit = event_worker_deinit,
};

