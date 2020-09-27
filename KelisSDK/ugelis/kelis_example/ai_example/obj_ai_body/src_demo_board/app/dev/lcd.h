/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          lcd.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of Lcd management
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __DEV_LCD_H__
#define __DEV_LCD_H__
#include <display/fb_display.h>
/*--------------------------------------------*/
//void ui_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
#define ARGB_BYTES 4
#define LCD_LAYER_MAX_CNT 2

#define LCD_LAYER_CAM 0
#define LCD_LAYER_UI  1


typedef struct dev_lcd_layer
{
    uint32_t layer_idx;
    uint32_t buf;
    uint32_t pixformat;
    uint32_t win_w;
    uint32_t win_h;
    uint32_t used;
} dev_lcd_layer_t;

typedef struct dev_lcd
{
    struct device *ltdc;
    dev_lcd_layer_t layer[LCD_LAYER_MAX_CNT];
    uint32_t layer_cnt;
} dev_lcd_t;

void lcd_init_layer(dev_lcd_t *dev, dev_lcd_layer_t *info);
void lcd_clean_ui(dev_lcd_t *dev);
int lcd_set_frame(void *ltdc, void *addr);
int lcd_init(dev_lcd_t *dev);
int lcd_set_frame_layercamera(void *addr);

#endif //__DEV_LCD_H__
