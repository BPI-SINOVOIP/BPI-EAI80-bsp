/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>

#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>

#include "ui.h"
#include "draw.h"

#define PORT     "GPIOC"
#define LCD_GPIO_PIN   3

struct device *gpiob;
struct device *ltdc;

uint32_t ui_buf_addr = 0xc0600000;

void set_ui_layer(int x, int y)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpiob = device_get_binding(PORT);
    if (!gpiob)
    {
        return NULL;
    }
    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);
    if (!ltdc)
    {
        return NULL;
    }

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_ARGB8888;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = (uint32_t) ui_buf_addr;

    fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = x;
    wininfo.y_start = y;
    wininfo.width   = LCD_UI_W;
    wininfo.height  = LCD_UI_H;

    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
}

void Creat_pic_bg(uint16_t x, uint16_t y, uint32_t addr, uint32_t color)
{
    uint16_t i, j;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            *(volatile uint32_t *)addr = color;
            addr += 4;
        }
    }
}

int main(void)
{
    char dist_str[16];
    char chstr0[] = {"格力芯片大卖"};
    char chstr1[] = {"格力金贝"};
    char chstr2[] = {"打开空调"};
    char chstr3[] = {"升高温度"};
    char chstr4[] = {"语音关闭"};
    char chstr5[] = {"十九度"};

    uint8_t loop = 0;
    sprintf(dist_str, "ZhuHai Edgeless");
    set_ui_layer(0, 0);
    while (1)
    {
        switch (loop)
        {
            case 0:
                Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, WHITE);
                break;
            case 1:
                Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, RED);
                break;
            case 2:
                Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, GREEN);
                break;
            case 3:
                Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, BLUE);
                break;
            case 4:
                Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, BLACK);
                break;
        }
        /*ASCII*/
        lcd_drawlogo(LCD_UI_W - LOGO_W - 1, 1, LOGO_BG_COLOR);
        lcd_draw_text(10, 10, 480, 32, 32, "GREE ELECTRIC APPLIANCES", WORD_COLOR);
        lcd_draw_text(10, 50, 480, 24, 24, dist_str, WORD_COLOR);
        lcd_draw_text(10, 80, 480, 16, 16, "VOUT LCD TEST", WORD_COLOR);
        lcd_draw_text(10, 100, 480, 12, 12, "2019/4/30", WORD_COLOR);

        /*中文*/
        lcd_draw_chstr(10, 120, 480, 32, 32, chstr0, WORD_COLOR);
        lcd_draw_chstr(10, 160, 480, 16, 16, chstr1, WORD_COLOR);
        lcd_draw_chstr(10, 180, 480, 16, 16, chstr2, WORD_COLOR);
        lcd_draw_chstr(10, 200, 480, 16, 16, chstr3, WORD_COLOR);
        lcd_draw_chstr(10, 220, 480, 16, 16, chstr4, WORD_COLOR);
        lcd_draw_chstr(10, 240, 480, 16, 16, chstr5, WORD_COLOR);
        loop++;
        if (loop == 5)
        {
            loop = 0;
        }
        k_sleep(1000);
    }
}

