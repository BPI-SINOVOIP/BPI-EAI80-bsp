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
#include <display/fb_display.h>
#include "lcd.h"
#include "ai_common.h"


struct device *ltdc;

uint32_t show_buf_addr ;//= (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0x24C00 + 0xC0000000;//0x10424C00 //
static void lcd_set_geometry(dev_lcd_t *dev, dev_lcd_layer_t *info)
{
    struct fb_wininfo wininfo;

    wininfo.x_start = 0;
    wininfo.y_start = 0;
    //wininfo.width   = info->win_w;
    //wininfo.height  = info->win_h;

    wininfo.width = 224;
    wininfo.height = 224;

    fb_ioctl(dev->ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(dev->ltdc, 0, FB_X_SETPOS, &wininfo);
}

void lcd_init_layer(dev_lcd_t *dev, dev_lcd_layer_t *info)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
    struct fb_displayinfo info1;

    fb_get_displayinfo(ltdc, &info1);

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.color = 0X000000;

    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;

    layerinfo.fb_start = info->buf;
    layerinfo.pixformat = info->pixformat;

    fb_ioctl(dev->ltdc, info->layer_idx, FB_X_INITLAYER, &layerinfo);

    lcd_set_geometry(dev, info);

    /*Make the first layer transparent*/
    //memset((void *)(show_buf_addr), 0x00, 224 * 224 * 4);

    //memset((uint8_t *)dev->layer[LCD_LAYER_UI].buf, 0, 224 * 224 * 4);
}

void lcd_clean_ui(dev_lcd_t *dev)
{
    //int size = dev->layer[LCD_LAYER_UI].win_w * dev->layer[1].win_h * ARGB_BYTES;
    //memset((uint8_t *)dev->layer[LCD_LAYER_UI].buf, 0, size);
}
int lcd_set_frame(void *ltdc, void *addr)
{
    fb_ioctl(ltdc, LCD_LAYER_CAM, FB_X_SETADDR, &addr);
    return 0;
}

int lcd_set_frame_layercamera(void *addr)
{
    fb_ioctl(ltdc, LCD_LAYER_CAM, FB_X_SETADDR, &addr);
    return 0;
}


int lcd_init(dev_lcd_t *dev)
{
    struct device *bl_gpio;

    bl_gpio = device_get_binding(LCD_BL_PORT);
    if (!bl_gpio)
    {
        return -1;
    }
    /* Set LED pin as output */
    gpio_pin_configure(bl_gpio, LCD_BL_PIN, GPIO_DIR_OUT);

    gpio_pin_write(bl_gpio, LCD_BL_PIN, 1);

    dev->ltdc = device_get_binding(CONFIG_VOUT_NAME);
    ltdc =  dev->ltdc;

    if (!dev->ltdc)
    {
        return -1;
    }

    dev->layer_cnt = LCD_LAYER_MAX_CNT;

    return 0;
}


void lcd_exit()
{

}
