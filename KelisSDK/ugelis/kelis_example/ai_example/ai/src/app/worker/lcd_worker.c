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
#include <string.h>
#include "dev/lcd.h"
#include "inc/config.h"
#include "worker.h"
#include "ui/draw.h"

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
}

static int lcd_worker_reset_scaler(dev_lcd_t *dev, worker_lcd_param_t *param)
{
    int frame_pixels;

    scaler_param_t *output_scaler0 = &(dev->output_scaler0);
    scaler_param_t *output_scaler1 = &(dev->output_scaler1);
    //k_mutex_lock(&dev->mutex, K_FOREVER);
    if (param->video.src_w != param->video.out0_w
            || param->video.src_h != param->video.out0_h
            || param->video.src_fmt != param->video.out0_fmt)
    {
        frame_pixels = lcd_worker_pixels_size(param->video.out0_fmt);
        if (output_scaler0->use && output_scaler0->dst)
        {
            kfree((void *)output_scaler0->dst);
            output_scaler0->dst = 0;
        }
        output_scaler0->dst = (unsigned int)kmalloc(param->video.out0_w * param->video.out0_h * frame_pixels);
        output_scaler0->src_w = param->video.src_w;
        output_scaler0->src_h = param->video.src_h;
        output_scaler0->dst_w = param->video.out0_w;
        output_scaler0->dst_h = param->video.out0_h;
        output_scaler0->src_fmt = param->video.src_fmt;
        output_scaler0->dst_fmt = param->video.out0_fmt;
        output_scaler0->use = 1;
    }

    if (param->video.src_w != param->video.out1_w
            || param->video.src_h != param->video.out1_h
            || param->video.src_fmt != param->video.out1_fmt)
    {
        frame_pixels = lcd_worker_pixels_size(param->video.out1_fmt);
        if (output_scaler1->use && output_scaler1->dst)
        {
            kfree((void *)output_scaler1->dst);
            output_scaler1->dst = 0;
        }
        output_scaler1->dst = (unsigned int)kmalloc(param->video.out1_w * param->video.out1_h * frame_pixels);
        output_scaler1->src_w = param->video.src_w;
        output_scaler1->src_h = param->video.src_h;
        output_scaler1->dst_w = param->video.out1_w;
        output_scaler1->dst_h = param->video.out1_h;
        output_scaler1->src_fmt = param->video.src_fmt;
        output_scaler1->dst_fmt = param->video.out1_fmt;
        output_scaler1->use = 1;
    }
    //k_mutex_unlock(&dev->mutex);
    return 0;
}

static int lcd_worker_init(worker_t *self, void *data)
{
    dev_lcd_t *dev = (dev_lcd_t *)kmalloc(sizeof(dev_lcd_t));
    worker_lcd_param_t *param = (worker_lcd_param_t *)data;
    int i = 0;
    int ui_pixels;
    int frame_pixels;
    dev_lcd_layer_t *layer;
    memset((void *)dev, 0, sizeof(dev_lcd_t));

    scaler_param_t *video_scaler = &(dev->video_scaler);

    k_mutex_init(&dev->mutex);

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
    frame_pixels = lcd_worker_pixels_size(param->video.src_fmt);
    dev->disp_buf_size = layer->win_w * layer->win_h * frame_pixels;

    if (param->video.src_w != param->video.disp_w
            || param->video.src_h != param->video.disp_h)
    {
        video_scaler->dst = (unsigned int)kmalloc(param->video.disp_w * param->video.disp_h * frame_pixels);
        video_scaler->src_w = param->video.src_w;
        video_scaler->src_h = param->video.src_h;
        video_scaler->dst_w = param->video.disp_w;
        video_scaler->dst_h = param->video.disp_h;
        video_scaler->src_fmt = param->video.src_fmt;
        video_scaler->dst_fmt = param->video.src_fmt;
        video_scaler->use = 1;
    }
    else
    {
        video_scaler->dst = (unsigned int)kmalloc(param->video.disp_w * param->video.disp_h * frame_pixels);
    }

    lcd_worker_reset_scaler(dev, param);

    /* config ui layer */
    layer = &(dev->layer[LCD_LAYER_UI]);
    layer->win_x      = param->ui.disp_x;
    layer->win_y      = param->ui.disp_y;
    layer->win_w      = param->ui.disp_w;
    layer->win_h      = param->ui.disp_h;
    layer->pixformat  = param->ui.src_fmt;
    ui_pixels         = lcd_worker_pixels_size(layer->pixformat);
    layer->buf        = (uint32_t)kmalloc(layer->win_w * layer->win_h * ui_pixels);
    layer->layer_idx  = LCD_LAYER_UI;
    layer->used       = 1;

    void *ltdc = lcd_init(dev);

    ui_init(layer->win_x, layer->win_y, layer->win_w, layer->win_h, layer->buf, ui_pixels, ltdc);

    /* init all layers */

    for (i = 0; i < dev->layer_cnt; i++)
    {
        if (dev->layer[i].used)
        {
            lcd_init_layer(dev, &dev->layer[i]);
        }
    }
    self->data = dev;
    return 0;
}

void *lcd_worker_fetch(struct worker *self, void *param)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);
    return dev->ltdc;
}

static int lcd_worker_run(worker_t *self, int cmd, void *param)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);

    /* set video frame */
    if (CMD_LCD_SETFRAME == cmd)
    {
        void *disp_buf;
        disp_buf = param;
        if (dev->video_scaler.use)
        {
            dev->video_scaler.src = (unsigned int)param;
            ai_imp_scale(dev->ltdc, &(dev->video_scaler));
            disp_buf = (void *)dev->video_scaler.dst;
        }
        else
        {
            memcpy(dev->video_scaler.dst, param, dev->disp_buf_size);
            disp_buf = (void *)dev->video_scaler.dst;
        }
        lcd_set_frame(dev->ltdc, disp_buf);
        dev->layer[LCD_LAYER_CAM].buf = (uint32_t)disp_buf;
    }
    else if (CMD_LCD_DOIMP == cmd)
    {
        worker_frame_t *frame = param;
        /* get output0 frame */
        frame->out0 = frame->orig;
        if (dev->output_scaler0.use)
        {
            dev->output_scaler0.src = frame->orig;
            //ui_draw_rgb565(frame->out0, 0, 0, dev->output_scaler0.src_w, dev->output_scaler0.src_h);
            int key = irq_lock();
            ai_imp_scale(dev->ltdc, &(dev->output_scaler0));
            irq_unlock(key);
            frame->out0 = dev->output_scaler0.dst;
            //ui_draw_rgb24(frame->out0, 0, 0, dev->output_scaler0.dst_w, dev->output_scaler0.dst_h);
        }

        /* get output1 frame */
        frame->out1 = frame->orig;
        if (dev->output_scaler1.use)
        {
            dev->output_scaler1.src = frame->orig;
            int key = irq_lock();
            ai_imp_scale(dev->ltdc, &(dev->output_scaler1));
            irq_unlock(key);
            frame->out1 = dev->output_scaler1.dst;
            //ui_draw_l8(frame->out1, 0, 0, dev->output_scaler1.dst_w, dev->output_scaler1.dst_h);
        }
    }
    else if (CMD_LCD_RESETSCALER == cmd)
    {
        lcd_worker_reset_scaler(dev, param);
    }
#ifdef DEBUG_CAP
    else if (CMD_LCD_CAPTURE == cmd)
    {
        dev_lcd_layer_t *ui    = &(dev->layer[LCD_LAYER_UI]);
        dev_lcd_layer_t *video = &(dev->layer[LCD_LAYER_CAM]);
        printf("Commands:\n");
        printf("speed 4000\n");
        printf("savebin C:\\bg.bin 0x%x 0x%x\n", video->buf,
               video->win_w * video->win_h * lcd_worker_pixels_size(video->pixformat)
              );
        printf("savebin C:\\fg.bin 0x%x 0x%x\n", ui->buf,
               ui->win_w * ui->win_h * lcd_worker_pixels_size(ui->pixformat));
        printf("./blending_bin %d %d bg.bin RGB565 %d %d %d %d fg.bin ARGB4444 %d %d %d %d output.bmp",
               DEV_LCD_W, DEV_LCD_H, video->win_x, video->win_y, video->win_w, video->win_h,
               ui->win_x, ui->win_x, ui->win_y, ui->win_w, ui->win_h);
        printf("\n");
    }
#endif
    return 0;
}

static int lcd_worker_deinit(worker_t *self)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);
    scaler_param_t *video_scaler = &(dev->video_scaler);
    scaler_param_t *output_scaler0 = &(dev->output_scaler0);
    scaler_param_t *output_scaler1 = &(dev->output_scaler1);

    //if (video_scaler->dst)
    {
        kfree((void *)video_scaler->dst);
    }

    if (output_scaler0->dst)
    {
        kfree((void *)output_scaler0->dst);
    }

    if (output_scaler1->dst)
    {
        kfree((void *)output_scaler1->dst);
    }

    kfree((void *)dev->layer[LCD_LAYER_UI].buf);
    kfree(dev);
    return 0;
}

worker_t lcd_worker =
{
    .run    = lcd_worker_run,
    .init   = lcd_worker_init,
    .deinit = lcd_worker_deinit,
    .fetch  = lcd_worker_fetch,
};
