/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          camera_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer interface to call camera device API
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <string.h>
#include <stdio.h>

#include <dev/camera.h>
#include <inc/config.h>
#include "worker.h"

K_THREAD_STACK_DEFINE(camera_stack_area1, 4 * 1024);

static int frame_ready(void *buf, void *arg)
{
    worker_t *self    = (worker_t *)arg;
    dev_camera_t *dev = (dev_camera_t *)self->data;

    if (dev->status == CAM_STATUS_IDLE)
    {
        memcpy((void *)dev->mem_buf, buf, dev->mem_size);
        dev->status = CAM_STATUS_READY;
    }

    dev->lcd->run(dev->lcd, CMD_LCD_SETFRAME, buf);

    return 0;
}

void camera_ai_thread(void *data, void *param, void *not_used)
{
    worker_t *self = (worker_t *)param;
    dev_camera_t *dev = (dev_camera_t *)self->data;
    worker_frame_t frame;
    while (dev->running)
    {
        if (dev->status == CAM_STATUS_READY)
        {
            frame.orig = dev->mem_buf;
            do_next_process(dev->self, CMD_LCD_DOIMP, &frame);
            dev->status = CAM_STATUS_IDLE;
        }
        k_yield();
    }
    k_sem_give(&dev->lock);
}

static int camera_worker_init(worker_t *self, void *data)
{
    dev_camera_t *dev = (dev_camera_t *)kmalloc(sizeof(dev_camera_t));

    dev_camera_arg_t *cam = &(dev->cam);
    worker_cam_param_t *param = (worker_cam_param_t *)data;
    int frame_pixels = 2;

    /* camera settings */
    cam->w       = param->w;
    cam->h       = param->h;
    cam->frames  = 2;
    cam->cb      = frame_ready;
    cam->cb_arg  = (void *)self;
    cam->mem_size    = cam->w * cam->h * cam->frames * (frame_pixels);
    cam->mem_start   = (uint32_t)kmalloc(cam->mem_size);
    camera_init(cam);

    dev->status   = CAM_STATUS_IDLE;
    dev->mem_size = cam->w * cam->h * (frame_pixels);
    dev->mem_buf  = (int)kmalloc(dev->mem_size);
    dev->running  = 1;
    dev->self = self;
    dev->lcd  = get_worker("lcd");
    self->data = dev;
    k_sem_init(&dev->lock, 0, 1);
    k_thread_create(&dev->ai_thread, camera_stack_area1,
                    K_THREAD_STACK_SIZEOF(camera_stack_area1),
                    (k_thread_entry_t)camera_ai_thread,
                    0, self, 0,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    return 0;
}

static int camera_worker_run(worker_t *self, int cmd, void *param)
{
    dev_camera_t *dev = (dev_camera_t *)self->data;
    if (cmd == CMD_CAM_STOP)
    {
        camera_stop(&dev->cam);
    }
    else if (cmd == CMD_CAM_START)
    {
        camera_start(&dev->cam);
    }
    return 0;
}

static int camera_worker_deinit(worker_t *self)
{
    dev_camera_t *dev = (dev_camera_t *)(self->data);
    dev_camera_arg_t *cam = &(dev->cam);
    kfree((void *)cam->mem_start);
    dev->running = 0;
    k_sem_take(&dev->lock, K_FOREVER);
    kfree((void *)dev->mem_buf);
    kfree(dev);
    return 0;
}

worker_t camera_worker =
{
    .run    = camera_worker_run,
    .init   = camera_worker_init,
    .deinit = camera_worker_deinit,
};
