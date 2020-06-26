/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          pre_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer worker to do pre processing of the layer input
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <kernel.h>
#include <ugelis.h>
#include <string.h>

#include "lib/image.h"
#include "pre_worker.h"
#include "easynet_worker.h"
#include "worker.h"
#include "inc/config.h"

K_THREAD_STACK_DEFINE(stack_area, 4 * 1024);

static int pre_worker_process(void *src, void *param)
{
    worker_t *self = (worker_t *)param;
    pre_worker_info_t *pre = (pre_worker_info_t *)self->data;
    /* Split the combined RGBRGBRGB data to separated pixels like RRRGGGBBB */
    if (pre->dst)
    {
        image_split(src, pre->dst, pre->ai_w, pre->ai_h);
        //ui_draw_rgb(pre->dst, 0, 0, pre->ai_w, pre->ai_h);

        do_next_process(self, CMD_EASYNET_DATAREADY, pre->dst);
    }
    return 0;
}

static int pre_worker_init(worker_t *self, void *data)
{
    pre_worker_info_t *pre = (pre_worker_info_t *)kmalloc(sizeof(pre_worker_info_t));
    worker_pre_param_t *param = (worker_pre_param_t *)data;
    int frame_pixels = 3;

    memset(pre, 0, sizeof(pre_worker_info_t));
    pre->ai_w  = param->ai_w;
    pre->ai_h  = param->ai_h;
    pre->dst   = (uint8_t *)kmalloc(pre->ai_w * pre->ai_h * frame_pixels);

    pre->pp.sizes      = param->sizes;
    pre->pp.sizes.ai_w = param->ai_w;
    pre->pp.sizes.ai_h = param->ai_h;

    pre->frame_in = param->frame_in;
    pre->exit     = param->exit;
    if (param->init)
    {
        param->init(&pre->pp);
    }
    self->data = pre;
    return 0;
}

static int pre_worker_run(worker_t *self, int cmd, void *param)
{
    pre_worker_info_t *pre = (pre_worker_info_t *)self->data;
    worker_frame_t *frame = param;
    int result = 0;
    if (cmd == CMD_LCD_DOIMP)
    {
        if (pre->frame_in)
        {
            pre->pp.in[0] = (unsigned char *)frame->out0;
            pre->pp.in[1] = (unsigned char *)frame->out1;
            result = pre->frame_in(&pre->pp);

        }
        else
        {
            result = frame->out0;
        }
        if (result)
        {
            pre_worker_process((void *)result, self);
        }
    }
    return 0;
}

static int pre_worker_deinit(worker_t *self)
{
    pre_worker_info_t *pre = self->data;

    if (pre->exit)
    {
        pre->exit(&pre->pp);
    }
    kfree(pre->dst);
    kfree(pre);
    return 0;
}

worker_t pre_worker =
{
    .run    = pre_worker_run,
    .init   = pre_worker_init,
    .deinit = pre_worker_deinit,
};
