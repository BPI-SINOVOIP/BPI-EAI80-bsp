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
 * @brief         Interface to manage lcd.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <gpio.h>
#include <string.h>
#include <display/fb_display.h>

#include "lcd.h"

static void lcd_set_geometry(dev_lcd_t *dev, dev_lcd_layer_t *info)
{
    struct fb_wininfo wininfo;

    wininfo.x_start = info->win_x;
    wininfo.y_start = info->win_y;
    wininfo.width   = info->win_w;
    wininfo.height  = info->win_h;

    fb_ioctl(dev->ltdc, info->layer_idx, FB_X_SETSIZE, &wininfo);
    fb_ioctl(dev->ltdc, info->layer_idx, FB_X_SETPOS, &wininfo);
}

void lcd_init_layer(dev_lcd_t *dev, dev_lcd_layer_t *info)
{
    struct fb_layerinfo layerinfo;

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.color = 0X000000;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.fb_start = info->buf;
    layerinfo.pixformat = info->pixformat;

    fb_ioctl(dev->ltdc, info->layer_idx, FB_X_INITLAYER, &layerinfo);
    int premultiply_onoff = 1;
    fb_ioctl(dev->ltdc, info->layer_idx, FB_X_SETPREMUL, &premultiply_onoff);

    lcd_set_geometry(dev, info);
}

void lcd_clean_ui(dev_lcd_t *dev)
{
    int size = dev->layer[LCD_LAYER_UI].win_w * dev->layer[1].win_h * ARGB_BYTES;
    memset((uint8_t *)dev->layer[LCD_LAYER_UI].buf, 0, size);
}

int lcd_set_frame(void *ltdc, void *addr)
{
    fb_ioctl(ltdc, LCD_LAYER_CAM, FB_X_SETADDR, &addr);
    return 0;
}

void *lcd_init(dev_lcd_t *dev)
{
    struct device *gpiob;

    gpiob = device_get_binding(LCD_BL_PORT);
    if (!gpiob)
    {
        return 0;
    }
    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_BL_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(gpiob, LCD_GPIO_PIN, 0);

    gpio_pin_write(gpiob, LCD_BL_PIN, 1);

    dev->ltdc = device_get_binding(CONFIG_VOUT_NAME);
    if (!dev->ltdc)
    {
        return 0;
    }

    dev->layer_cnt = LCD_LAYER_MAX_CNT;

    return dev->ltdc;
}

void lcd_exit()
{

}
