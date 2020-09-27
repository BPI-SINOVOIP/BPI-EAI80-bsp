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

#include "lib/image.h"
#include "pre_worker.h"
#include "easynet_worker.h"
#include "worker.h"
#include "inc/config.h"

K_THREAD_STACK_DEFINE(stack_area, 1 * 1024);
static int pre_worker_process(face_detect_t *fd, void *param)
{
    worker_t *self = (worker_t *)param;
    pre_worker_info_t *pre = (pre_worker_info_t *)self->data;
    int orig_x, orig_y, orig_w, orig_h;

    ui_clear();

    /* Crop the detected rect where the face in from the original frame */
    orig_x = ui_rel(fd->x, pre->src_size[0], pre->input_size[0]);
    orig_y = ui_rel(fd->y, pre->src_size[1], pre->input_size[1]);
    orig_w = ui_rel(fd->w, pre->src_size[0], pre->input_size[0]);
    orig_h = ui_rel(fd->h, pre->src_size[1], pre->input_size[1]);
    image_crop(pre->buf.in0, pre->buf.in1, orig_x, orig_y, orig_w, orig_h, pre->src_size[0]);
    //ui_draw_rgb24(pre->buf.in1, 0, 0, orig_w, orig_h);

    /* Zoom the cropped face image, which can be any size, up/down to the pointed size
       that equals to the input of AI model */
    image_zoom(pre->buf.in1, pre->buf.in0, orig_w, orig_h, pre->out_size[0], pre->out_size[1]);
    //ui_draw_rgb24(pre->buf.in0, 10, 20, pre->out_size[0], pre->out_size[1]);

    /* Split the combined RGBRGBRGB data to separated pixels like RRRGGGBBB */
    image_split(pre->buf.in0, pre->buf.in1, pre->out_size[0], pre->out_size[1]);
    //ui_draw_rgb(pre->buf.input, 0, 0, pre->out_size[0], pre->out_size[1]);

    orig_x = ui_rel(fd->x, -1, pre->input_size[0]);
    orig_y = ui_rel(fd->y, -2, pre->input_size[1]);
    orig_w = ui_rel(fd->w, -1, pre->input_size[0]);
    orig_h = ui_rel(fd->h, -2, pre->input_size[1]);
    ui_draw_focus(orig_x, orig_y, orig_x + orig_w - 1, orig_y + orig_h - 1, 0xFFFFFF00);
    do_next_process(self, CMD_EASYNET_DATAREADY, pre->buf.in1);
}

static int pre_worker_init(worker_t *self, void *data)
{
    pre_worker_info_t *pre = kmalloc(sizeof(pre_worker_info_t));
    worker_pre_param_t *param = (worker_pre_param_t *)data;

    int frame_pixels = 3;
    pre->src_size[0] = param->src_w;
    pre->src_size[1] = param->src_h;
    pre->out_size[0] = param->dst_w;
    pre->out_size[1] = param->dst_h;
    pre->input_size[0] = param->input_w;
    pre->input_size[1] = param->input_h;
    pre->buf.in0 = kmalloc(pre->src_size[0] * pre->src_size[1] * frame_pixels);
    pre->buf.in1 = kmalloc(pre->src_size[0] * pre->src_size[1] * frame_pixels);
    pre->fd.exit         = 0;
    pre->fd.frame_ready  = 0;
    pre->fd.detected_buf = pre->buf.in1;
    pre->fd.finish_cb    = pre_worker_process;

    face_detect_init(param->input_w, param->input_h);

    k_thread_create(&pre->task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)face_detect_thread,
                    &pre->fd, self, 0,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    self->data = pre;
    return 0;
}

static int pre_worker_run(worker_t *self, int cmd, void *param)
{
    pre_worker_info_t *pre = (pre_worker_info_t *)self->data;
    if (!pre->fd.frame_ready)
    {
        int frame_pixels = 3;
        worker_frame_t *frame = param;
        memcpy(pre->buf.in0, frame->out0, pre->src_size[0]*pre->src_size[1] * frame_pixels);
        //ui_draw_rgb24(pre->buf.in0, 0, 0, pre->src_size[0], pre->src_size[1]);
        memcpy(pre->buf.in1, frame->out1, pre->input_size[0]*pre->input_size[1]);
        //ui_draw_l8(pre->buf.in1, 0, 0, pre->input_size[0], pre->input_size[1]);
        pre->fd.frame_ready = 1;
        do_next_process(self, CMD_EASYNET_FRAMEREADY, pre->buf.in0);
    }
}

static int pre_worker_deinit(worker_t *self)
{
    pre_worker_info_t *pre = self->data;
    pre->fd.exit = 1;
    kfree(pre->buf.in1);
    kfree(pre->buf.in0);
    kfree(pre);
}

worker_t pre_worker =
{
    .run    = pre_worker_run,
    .init   = pre_worker_init,
    .deinit = pre_worker_deinit,
};
