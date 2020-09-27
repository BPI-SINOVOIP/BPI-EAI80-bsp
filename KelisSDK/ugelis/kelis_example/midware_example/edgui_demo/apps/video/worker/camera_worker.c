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
    int i = 0;
    worker_t *self = (worker_t *)arg;
    dev_camera_t *dev = (dev_camera_t *)self->data;
    worker_buf_t set;
    set.orig = buf;
    set.gray = dev->buf_gray;
    do_next_process(self, CMD_LCD_SETFRAME, &set);
}

static int camera_worker_init(worker_t *self, void *param)
{
    dev_camera_t *dev = kmalloc(sizeof(dev_camera_t));

    dev->cam_cb.func = frame_ready;
    dev->cam_cb.arg  = (void *)self;
    dev->buf_w       = CAM_CROP_W;
    dev->buf_h       = CAM_CROP_H;
    dev->mem_start   = 0;
    camera_init(dev);
    dev->buf_gray = kmalloc(CAM_CROP_W * CAM_CROP_H);
    self->data = dev;
}

static int camera_worker_run(worker_t *self, int cmd, void *param)
{
    camera_start((dev_camera_t *)(self->data));
}

static int camera_worker_deinit(worker_t *self)
{
    dev_camera_t *dev = (dev_camera_t *)(self->data);
    kfree(dev->buf_gray);
    kfree(dev);
}

worker_t camera_worker =
{
    .run    = camera_worker_run,
    .init   = camera_worker_init,
    .deinit = camera_worker_deinit,
};
