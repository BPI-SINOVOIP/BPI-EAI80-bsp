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

#include <dev/camera.h>
#include <inc/config.h>

#include "worker.h"

static void frame_ready(void *buf, void *arg)
{
    worker_t *self = (worker_t *)arg;
    dev_camera_t *dev = (dev_camera_t *)self->data;

    worker_frame_t frame;
    frame.orig = buf;
    do_next_process(self, CMD_LCD_SETFRAME, &frame);
}

static int camera_worker_init(worker_t *self, void *data)
{
    dev_camera_t *dev = kmalloc(sizeof(dev_camera_t));

    dev_camera_arg_t *cam = &(dev->cam);
    worker_cam_param_t *param = (worker_cam_param_t *)data;
    int frame_pixels = 3;

    /* frame settings */
    /* camera settings */
    cam->w       = param->w;
    cam->h       = param->h;
    cam->frames  = 2;
    cam->cb      = frame_ready;
    cam->cb_arg  = (void *)self;
    cam->mem_size    = cam->w * cam->h * cam->frames * (frame_pixels);
    cam->mem_start   = kmalloc(cam->mem_size);

    camera_init(cam);
    self->data = dev;
}

static int camera_worker_run(worker_t *self, int cmd, void *param)
{
    dev_camera_t *dev = (dev_camera_t *)self->data;
    camera_start(&(dev->cam));
}

static int camera_worker_deinit(worker_t *self)
{
    dev_camera_t *dev = (dev_camera_t *)(self->data);
    kfree(dev);
}

worker_t camera_worker =
{
    .run    = camera_worker_run,
    .init   = camera_worker_init,
    .deinit = camera_worker_deinit,
};
