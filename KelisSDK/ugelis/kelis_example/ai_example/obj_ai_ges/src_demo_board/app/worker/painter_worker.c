/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          painter_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer worker to paint ui
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */


#include "worker.h"
#include "painter_worker.h"
#include "ui/picture_raw.h"
#include "ui/draw.h"

static int painter_worker_init(worker_t *self, void *param)
{

}

static int painter_worker_run(worker_t *self, int cmd, void *param)
{
    int label = (int)param;
    rect_t pic_rt;
    pic_rt.x = 0;
    pic_rt.y = 222;
    pic_rt.w = 480;
    pic_rt.h = 50;

    if (cmd == CMD_PAINT_CAP)
    {
        //???????? todo
        paint_info_t *info = param;
        ui_draw_pic(&pic_rt, gImage_rawimage_cap, 128);
        ui_draw_text(info->x, info->y, strlen(info->str) * 12, 12, 12, info->str, info->color);
    }
    else if (cmd == CMD_PAINT_PASS)
    {
        paint_info_t *info = param;
        ui_draw_pic(&pic_rt, gImage_rawimage_pass, 128);
        ui_draw_text(info->x, info->y, strlen(info->str) * 12, 12, 12, info->str, info->color);
    }
    else if (cmd == CMD_PAINT_CLEAR)
    {
        ui_clear();
    }
    else if (cmd == CMD_PAINT_TEXT)
    {
        paint_info_t *info = param;
        ui_draw_text(info->x, info->y, strlen(info->str) * 12, 12, 12, info->str, info->color);
    }
}

static int painter_worker_deinit(worker_t *self)
{
    kfree(self->data);
}

worker_t painter_worker =
{
    .run    = painter_worker_run,
    .init   = painter_worker_init,
    .deinit = painter_worker_deinit,
};
