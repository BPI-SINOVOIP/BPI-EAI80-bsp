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

//#include "lib/image.h"
#include "pre_worker.h"
#include "easynet_worker.h"
#include "worker.h"
#include "inc/config.h"

extern uint32_t show_buf_addr;

#define CAM_PIC_SIZE  (INPUT_HEIGHT * INPUT_HEIGHT * 3)

K_THREAD_STACK_DEFINE(stack_area, 1 * 1024);

static inline int _h(int x)
{
    return x * DEV_LCD_W / FD_BUF_W;
}

static inline int _v(int x)
{
    return x * DEV_LCD_H / FD_BUF_H;
}

static int pre_worker_process(face_detect_t *fd, void *param)
{
    worker_t *self = (worker_t *)param;
    pre_worker_info_t *pre = (pre_worker_info_t *)self->data;
#if 0
    ui_clear();

    //ui_draw_rgb24(pre->buf.tmp, 30, 40, pre->src_size[0], pre->src_size[1]);
    image_crop(pre->buf.tmp, pre->buf.detected, fd->x, fd->y, fd->w, fd->h, pre->src_size[0]);
    //ui_draw_rgb24(pre->buf.detected, 10, 20, fd->w, fd->h);
    //ai_imp_scaleup(pre->buf.input, pre->buf.tmp, fd->w, fd->h, pre->dst_size[0], pre->dst_size[1]);
    image_zoom(pre->buf.detected, pre->buf.tmp, fd->w, fd->h, pre->dst_size[0], pre->dst_size[1]);
    //ui_draw_rgb24(pre->buf.tmp, 10, 20, pre->dst_size[0], pre->dst_size[1]);
    image_split(pre->buf.tmp, pre->buf.detected, pre->dst_size[0], pre->dst_size[1]);
    //ui_draw_rgb(pre->buf.input, 0, 0, pre->dst_size[0], pre->dst_size[1]);
    //ui_draw_rectangle(fd->x, fd->y, fd->x+fd->w-1, fd->y+fd->h-1, 0xFFFFFF00);
    ui_draw_focus(_h(fd->x), _v(fd->y), _h(fd->x + fd->w - 1), _v(fd->y + fd->h - 1), 0xFFFFFF00);
    do_next_process(self, CMD_EASYNET_DATAREADY, pre->buf.detected);
#else
    do_next_process(self, CMD_EASYNET_DATAREADY, pre->buf.tmp);
#endif
}

static int pre_worker_init(worker_t *self, void *param)
{
    pre_worker_info_t *pre = kmalloc(sizeof(pre_worker_info_t));
    pre->src_size[0] = INPUT_WIDTH;
    pre->src_size[1] = INPUT_HEIGHT;
    pre->dst_size[0] = INPUT_WIDTH;
    pre->dst_size[1] = INPUT_HEIGHT;
    pre->buf.detected = kmalloc(pre->src_size[0] * pre->src_size[1] * 3);
    pre->buf.tmp      = kmalloc(pre->src_size[0] * pre->src_size[1] * 3);
    pre->fd.exit         = 0;
    pre->fd.frame_ready  = 0;
    pre->fd.detected_buf = pre->buf.detected;
    pre->fd.finish_cb    = pre_worker_process;

    face_detect_init(FD_BUF_W, FD_BUF_H);

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
    worker_buf_t *set = param;
    if (!pre->fd.frame_ready)
    {
#if 1
        //memcpy(pre->buf.detected, set->gray, pre->src_size[0]*pre->src_size[1]);
        //memcpy(pre->buf.tmp, set->orig, pre->src_size[0]*pre->src_size[1] * 3);
        //ui_draw_rgb24(pre->buf.detected, 10, 20, pre->src_size[0], pre->src_size[1]);
        pre->fd.frame_ready = 1;
        //do_next_process(self, CMD_EASYNET_FRAMEREADY, pre->buf.tmp);
        unsigned char *buf = set->orig;
        /*Captured Picture data*/
        memcpy(pre->buf.detected, (unsigned char *)(buf), CAM_PIC_SIZE);

        show_buf_addr = buf;

#endif
    }


}

static int pre_worker_deinit(worker_t *self)
{
    pre_worker_info_t *pre = self->data;
    pre->fd.exit = 1;
    kfree(pre->buf.detected);
    kfree(pre->buf.tmp);
    kfree(pre);
}

worker_t pre_worker =
{
    .run    = pre_worker_run,
    .init   = pre_worker_init,
    .deinit = pre_worker_deinit,
};
