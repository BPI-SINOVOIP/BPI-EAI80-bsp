/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          app.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief        Common interface for app
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */
#include <display/fb_display.h>

#include <inc/config.h>
#include <worker/worker.h>
#include <worker/loader_worker.h>
#include <dev/gpio.h>

static worker_lcd_param_t lcd_param =
{
    .video =
    {
        .src_w  = CAM_BUF_W,
        .src_h  = CAM_BUF_H,
        .src_fmt = FMT_RGB565,
        .disp_x = CAM_DISP_X + 160,
        .disp_y = CAM_DISP_Y + 104,
        .disp_w = CAM_DISP_W,
        .disp_h = CAM_DISP_H,

        .out0_w   = CAM_BUF_W,
        .out0_h   = CAM_BUF_H,
        .out0_fmt = FMT_RGB565,

        .out1_w   = CAM_BUF_W,
        .out1_h   = CAM_BUF_H,
        .out1_fmt = FMT_RGB565,
    },
    .ui =
    {
        .src_fmt = FMT_ARGB4444,
        .disp_x = 160,
        .disp_y = 104,
        .disp_w = DEV_LCD_W,
        .disp_h = DEV_LCD_H,
    },
};


static worker_cam_param_t cam_param =
{
    .w = CAM_BUF_W,
    .h = CAM_BUF_H,
};

worker_lcd_param_t *stream_get_lcd_param(void)
{
    return &lcd_param;
}

void stream_init()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");
    worker_t *loader = get_worker("loader");

    cam->init(cam, &cam_param);
    lcd->init(lcd, &lcd_param);

    loader->init(loader, 0);
}

void stream_deinit()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");
    worker_t *loader = get_worker("loader");

    loader->deinit(loader);
    lcd->deinit(lcd);
    cam->deinit(cam);
}

int stream_start_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    cam->next[0] = lcd;
    cam->run(cam, CMD_CAM_START, 0);

    return 0;
}

int stream_stop_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    cam->next[0] = 0;
    cam->run(cam, CMD_CAM_STOP, 0);

    return 0;
}

uint32_t stream_start_loaddata(worker_io_t *info)
{
    worker_t *loader = get_worker("loader");
    loader->run(loader, CMD_LOADER_READ, info);
    return info->dst;
}

void ui_bind_event(void *cb)
{
    worker_t *painter = get_worker("painter");
    painter->run(painter, CMD_PAINT_BIND, cb);
}

void gui_init(void *cb)
{
    worker_painter_param_t painter_param;
    worker_t *painter   = get_worker("painter");
    painter->init(painter, &painter_param);

    ui_bind_event(cb);
}

void gui_exit()
{
    worker_t *painter   = get_worker("painter");
    painter->deinit(painter);

    ui_bind_event(0);
}

void app_capture_screen()
{
    worker_t *lcd   = get_worker("lcd");
    lcd->run(lcd, CMD_LCD_CAPTURE, 0);
}

