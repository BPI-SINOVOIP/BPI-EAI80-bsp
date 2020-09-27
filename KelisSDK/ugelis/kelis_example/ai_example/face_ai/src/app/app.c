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

int stream_init()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");
    worker_t *loader = get_worker("loader");

    worker_lcd_param_t lcd_param;
    worker_cam_param_t cam_param;

    cam_param.w      = CAM_BUF_W;
    cam_param.h      = CAM_BUF_H;
    cam->init(cam, &cam_param);

    lcd_param.video.src_w  = CAM_BUF_W;
    lcd_param.video.src_h  = CAM_BUF_H;
    lcd_param.video.src_fmt = FMT_RGB565;

    lcd_param.video.disp_x = DEV_LCD_X;
    lcd_param.video.disp_y = DEV_LCD_Y;
    lcd_param.video.disp_w = DEV_LCD_W;
    lcd_param.video.disp_h = DEV_LCD_H;

    lcd_param.video.out0_w  = lcd_param.video.src_w;
    lcd_param.video.out0_h  = lcd_param.video.src_h;
    lcd_param.video.out0_fmt = FMT_RGB888;

    lcd_param.video.out1_w  = OUT_BUF_W;
    lcd_param.video.out1_h  = OUT_BUF_H;
    lcd_param.video.out1_fmt = FMT_L8;

    lcd_param.ui.src_fmt = FMT_ARGB4444;
    lcd_param.ui.disp_x = DEV_LCD_X;
    lcd_param.ui.disp_y = DEV_LCD_Y;
    lcd_param.ui.disp_w = DEV_LCD_W;
    lcd_param.ui.disp_h = DEV_LCD_H;

    lcd->init(lcd, &lcd_param);

    loader->init(loader, 0);
}

int stream_deinit()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");
    worker_t *loader = get_worker("loader");

    loader->deinit(loader, 0);
    lcd->deinit(lcd, 0);
    cam->deinit(cam, 0);
}

int stream_start_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    cam->next[0] = lcd;
    cam->run(cam, 0, 0);

    return 0;
}

int stream_stop_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    cam->next[0] = 0;
    cam->stop(cam, 0, 0);

    return 0;
}

uint32_t stream_start_loaddata(worker_io_t *info)
{
    worker_t *loader = get_worker("loader");
    loader->run(loader, CMD_LOADER_READ, info);
    return info->dst;
}
