/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          lcd_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer worker to manage lcd interface.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <device.h>

#include "dev/lcd.h"
#include "inc/config.h"
#include "worker.h"

static int lcd_worker_pixels_size(int pixel_fmt)
{
    switch (pixel_fmt)
    {
        case FMT_ARGB8888:
            return 4;
        case FMT_RGB888:
            return 3;
        case FMT_RGB565:
        case FMT_ARGB4444:
            return 2;
        case FMT_L8:
            return 1;
        default:
            return 0;
    }
    return 0;
}

static int lcd_worker_init(worker_t *self, void *data)
{
    dev_lcd_t *dev = kmalloc(sizeof(dev_lcd_t));
    worker_lcd_param_t *param = (worker_lcd_param_t *)data;
    int i = 0;
    int index = 0;
    int ui_pixels;
    int frame_pixels;
    dev_lcd_layer_t *layer;
    scaler_param_t *video_scaler = &(dev->video_scaler);
    scaler_param_t *output_scaler0 = &(dev->output_scaler0);
    scaler_param_t *output_scaler1 = &(dev->output_scaler1);

    /* config video layer */
    layer = &(dev->layer[LCD_LAYER_CAM]);
    layer->win_x     = param->video.disp_x;
    layer->win_y     = param->video.disp_y;
    layer->win_w     = param->video.disp_w;
    layer->win_h     = param->video.disp_h;
    layer->buf       = 0;
    layer->pixformat = param->video.src_fmt;
    layer->layer_idx = LCD_LAYER_CAM;
    layer->used      = 1;

    video_scaler->use = 0;
    if (param->video.src_w != param->video.disp_w
            || param->video.src_h != param->video.disp_h)
    {
        frame_pixels = lcd_worker_pixels_size(param->video.src_fmt);
        video_scaler->dst = kmalloc(param->video.disp_w * param->video.disp_h * frame_pixels);
        video_scaler->src_w = param->video.src_w;
        video_scaler->src_h = param->video.src_h;
        video_scaler->dst_w = param->video.disp_w;
        video_scaler->dst_h = param->video.disp_h;
        video_scaler->src_fmt = param->video.src_fmt;
        video_scaler->dst_fmt = param->video.src_fmt;
        video_scaler->use = 1;
    }

    output_scaler0->use = 0;
    if (param->video.src_w != param->video.out0_w
            || param->video.src_h != param->video.out0_h
            || param->video.src_fmt != param->video.out0_fmt)
    {
        frame_pixels = lcd_worker_pixels_size(param->video.out0_fmt);
        output_scaler0->dst = kmalloc(param->video.out0_w * param->video.out0_h * frame_pixels);
        output_scaler0->src_w = param->video.src_w;
        output_scaler0->src_h = param->video.src_h;
        output_scaler0->dst_w = param->video.out0_w;
        output_scaler0->dst_h = param->video.out0_h;
        output_scaler0->src_fmt = param->video.src_fmt;
        output_scaler0->dst_fmt = param->video.out0_fmt;
        output_scaler0->use = 1;
    }

    output_scaler1->use = 0;
    if (param->video.src_w != param->video.out1_w
            || param->video.src_h != param->video.out1_h
            || param->video.src_fmt != param->video.out1_fmt)
    {
        frame_pixels = lcd_worker_pixels_size(param->video.out1_fmt);
        output_scaler1->dst = kmalloc(param->video.out1_w * param->video.out1_h * frame_pixels);
        output_scaler1->src_w = param->video.src_w;
        output_scaler1->src_h = param->video.src_h;
        output_scaler1->dst_w = param->video.out1_w;
        output_scaler1->dst_h = param->video.out1_h;
        output_scaler1->src_fmt = param->video.src_fmt;
        output_scaler1->dst_fmt = param->video.out1_fmt;
        output_scaler1->use = 1;
    }

    /* config ui layer */
    layer = &(dev->layer[LCD_LAYER_UI]);
    layer->win_x      = param->ui.disp_x;
    layer->win_y      = param->ui.disp_y;
    layer->win_w      = param->ui.disp_w;
    layer->win_h      = param->ui.disp_h;
    layer->pixformat  = param->ui.src_fmt;
    ui_pixels         = lcd_worker_pixels_size(layer->pixformat);
    layer->buf        = kmalloc(layer->win_w * layer->win_h * ui_pixels);
    layer->layer_idx  = LCD_LAYER_UI;
    layer->used       = 1;

    ui_init(layer->win_w, layer->win_h, layer->buf, ui_pixels);

    /* init all layers */
    lcd_init(dev);
    for (i = 0; i < dev->layer_cnt; i++)
    {
        if (dev->layer[i].used)
        {
            lcd_init_layer(dev, &dev->layer[i]);
        }
    }
    self->data = dev;
}

void *lcd_worker_fetch(struct worker *self, void *param)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);
    return dev->ltdc;
}

static int lcd_worker_run(worker_t *self, int cmd, void *param)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);

    if (CMD_LCD_SETFRAME == cmd)
    {
        worker_frame_t *frame = param;
        void *disp_buf;
        int index = LCD_LAYER_CAM;
        int frame_pixels = 3;   //FMT_RGB888

        /* set video frame */
        disp_buf = frame->orig;
        if (dev->video_scaler.use)
        {
            dev->video_scaler.src = frame->orig;
            ai_imp_scale(&(dev->video_scaler));
            disp_buf = dev->video_scaler.dst;
        }
        lcd_set_frame(dev->ltdc, disp_buf);

        /* get output0 frame */
        frame->out0 = frame->orig;
        if (dev->output_scaler0.use)
        {
            dev->output_scaler0.src = frame->orig;
            //ui_draw_rgb565(frame->out0, 0, 0, dev->output_scaler0.src_w, dev->output_scaler0.src_h);
            ai_imp_scale(&(dev->output_scaler0));
            frame->out0 = dev->output_scaler0.dst;
            //ui_draw_rgb24(frame->out0, 0, 0, dev->output_scaler0.dst_w, dev->output_scaler0.dst_h);
        }

        /* get output1 frame */
        frame->out1 = frame->orig;
        if (dev->output_scaler1.use)
        {
            dev->output_scaler1.src = frame->orig;
            ai_imp_scale(&(dev->output_scaler1));
            frame->out1 = dev->output_scaler1.dst;
            //ui_draw_l8(frame->out1, 0, 0, dev->output_scaler1.dst_w, dev->output_scaler1.dst_h);
        }
    }
}

static int lcd_worker_deinit(worker_t *self)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);
    scaler_param_t *video_scaler = &(dev->video_scaler);
    scaler_param_t *output_scaler1 = &(dev->output_scaler1);

    if (video_scaler->use)
    {
        kfree(video_scaler->dst);
    }

    if (output_scaler1->use)
    {
        kfree(output_scaler1->dst);
    }

    kfree(dev->layer[LCD_LAYER_UI].buf);
    kfree(dev);
}

worker_t lcd_worker =
{
    .run    = lcd_worker_run,
    .init   = lcd_worker_init,
    .deinit = lcd_worker_deinit,
    .fetch  = lcd_worker_fetch,
};
