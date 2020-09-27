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

#include <string.h>

#include "worker.h"
#include "painter_worker.h"
#include "ui/draw.h"
#include "inc/config.h"

extern const unsigned char gImage_rawimage_cap[];
extern const unsigned char gImage_rawimage_pass[];

static int painter_worker_init(struct worker *self, void *param)
{
    painter_worker_info_t *painter = (painter_worker_info_t *)kmalloc(sizeof(painter_worker_info_t));

    memset(painter, 0, sizeof(painter_worker_info_t));
    painter->gui = ugui_start();
    self->data = painter;
    return 0;
}

static int painter_worker_run(worker_t *self, int cmd, void *param)
{

    painter_worker_info_t *painter = (painter_worker_info_t *)self->data;
    gui_info_t *gui = painter->gui;

    if (cmd == CMD_PAINT_CAP)
    {
        paint_info_t *info = param;
        info->pic = (char *)gImage_rawimage_cap;

        gui->show_pic(info);
        gui->show_text(info);
    }
    else if (cmd == CMD_PAINT_PASS)
    {
        paint_info_t *info = param;
        info->pic = (char *)gImage_rawimage_pass;

        gui->show_pic(info);
        gui->show_text(info);
    }
    else if (cmd == CMD_PAINT_TEXT)
    {
        paint_info_t *info = param;
        gui->show_text(info);
    }
    else if (cmd == CMD_PAINT_BIND)
    {
        gui->set_event_cb(param);
    }
    else if (cmd == CMD_PAINT_RECT)
    {
        gui->show_rect(param);
    }
    else if (cmd == CMD_PAINT_SHOWCH)
    {
        paint_info_t *info = param;
        info->size = 16 * DESIGN_W;
        gui->show_char(info);
    }
    else if (cmd == CMD_PAINT_INIT)
    {
        if (0 == param)
        {
            paint_info_t info_init;
            info_init.x = CAM_DISP_X;
            info_init.y = 0;
            info_init.w = DEV_LCD_W - CAM_DISP_X;
            info_init.h = DEV_LCD_H;
            info_init.color = 0x0;
            gui->fill_frame(&info_init);
        }
        else
        {
            gui->fill_frame(param);
        }
    }
    return 0;
}

static int painter_worker_deinit(worker_t *self)
{
    painter_worker_info_t *painter = (painter_worker_info_t *)self->data;
    painter->gui->exit(painter->gui);
    kfree(painter);
    return 0;
}

worker_t painter_worker =
{
    .run    = painter_worker_run,
    .init   = painter_worker_init,
    .deinit = painter_worker_deinit,
};
