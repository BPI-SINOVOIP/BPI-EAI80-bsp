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

unsigned char VIDEO_SEC_CAM_DISP[DEV_LCD_W * DEV_LCD_H * CAM_BUF_PIXEL_SIZE] VIDEO_SECTION;

static int lcd_worker_init(worker_t *self, void *param)
{
    dev_lcd_t *lcd_dev = kmalloc(sizeof(dev_lcd_t));
    int i = 0;
    int buf_addr = (int)param;
    int index = LCD_LAYER_CAM;

    lcd_dev->layer[index].win_x     = 0;
    lcd_dev->layer[index].win_y     = 0;
    lcd_dev->layer[index].win_w     = CAM_CROP_W;
    lcd_dev->layer[index].win_h     = CAM_CROP_H;
    lcd_dev->layer[index].buf       = 0;
    lcd_dev->layer[index].pixformat = FMT_RGB888;
    lcd_dev->layer[index].layer_idx = 1;
    lcd_dev->layer[index].used      = 1;

    lcd_init(lcd_dev);

    if (lcd_dev->layer[index].used)
    {
        lcd_init_layer(lcd_dev, &lcd_dev->layer[index]);
    }

    self->data = lcd_dev;
}

void *lcd_worker_fetch(struct worker *self, void *param)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);
    return dev->ltdc;
}

static int lcd_worker_run(worker_t *self, int cmd, void *param)
{
    dev_lcd_t *dev = (dev_lcd_t *)(self->data);
    if (cmd == CMD_LCD_SETFRAME)
    {
        worker_buf_t *set = param;
        void *disp_buf, *fd_buf;
        disp_buf = set->orig;
        set->fd  = VIDEO_SEC_CAM_DISP;
        //ai_imp_scaleup(set->orig, VIDEO_SEC_CAM_DISP, CAM_CROP_W, CAM_CROP_H,
        //dev->layer[LCD_LAYER_CAM].win_w, dev->layer[LCD_LAYER_CAM].win_h);
        //disp_buf = VIDEO_SEC_CAM_DISP;
        lcd_set_frame(dev->ltdc, disp_buf);

        // 480x272==>160x120
        //ai_imp_scaleup(set->orig, set->fd, CAM_CROP_W, CAM_CROP_H, FD_BUF_W, FD_BUF_H);
        //ai_imp_rgb2l(set->fd, set->gray, CAM_CROP_W, CAM_CROP_H, dev->ltdc);
    }
    else if (cmd == CMD_LCD_SETPOS)
    {
        worker_layer_rect_t *rect = param;
        dev->layer[LCD_LAYER_CAM].win_x = rect->x;
        dev->layer[LCD_LAYER_CAM].win_y = rect->y;
        lcd_set_geometry(dev, &dev->layer[LCD_LAYER_CAM]);
    }
    else if (cmd == CMD_LCD_GETPOS)
    {
        worker_layer_rect_t *rect = param;
        rect->x = dev->layer[LCD_LAYER_CAM].win_x;
        rect->y = dev->layer[LCD_LAYER_CAM].win_y;
        rect->w = dev->layer[LCD_LAYER_CAM].win_w;
        rect->h = dev->layer[LCD_LAYER_CAM].win_h;
    }
}

static int lcd_worker_deinit(worker_t *self)
{
    dev_lcd_t *lcd_dev = (dev_lcd_t *)(self->data);
    kfree(lcd_dev);
}

worker_t lcd_worker =
{
    .run    = lcd_worker_run,
    .init   = lcd_worker_init,
    .deinit = lcd_worker_deinit,
    .fetch  = lcd_worker_fetch,
};
