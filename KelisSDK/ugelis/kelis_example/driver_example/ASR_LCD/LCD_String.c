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

#include "ui/ui.h"
#include "ui/draw.h"

#define PORT     "GPIOD"
#define LCD_GPIO_PIN   12

struct device *gpiob;
struct device *ltdc;

uint32_t ui_buf_addr = 0xc1900000;//0xc0600000;

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

#define MAX_CHSTR_LEN 30
char chstr_1[][MAX_CHSTR_LEN] =
{
   {"请说格力金贝"},
};

char chstr_trig[][MAX_CHSTR_LEN] =
{
    {"格力金贝"},
};
char chstr_cmd[][MAX_CHSTR_LEN] =
{
    {"打开空调"},
    {"关闭空调"},
    {"十六度"},
    {"十七度"},
    {"十八度"},
    {"十九度"},
    {"二十度"},
    {"二十一度"},
    {"二十二度"},
    {"二十三度"},
    {"二十四度"},
    {"二十五度"},
    {"二十六度"},
    {"二十七度"},
    {"二十八度"},
    {"二十九度"},
    {"三十度"},
    {"升高温度"},
    {"降低温度"},
    {"自动模式"},
    {"制冷模式"},
    {"除湿模式"},
    {"送风模式"},
    {"加热模式"},
    {"最大风"},
    {"最小风"},
    {"增大风速"},
    {"减小风速"},
    {"自动风"},
    {"增大音量"},
    {"减小音量"},
    {"打开左右扫风"},
    {"打开上下扫风"},
    {"关闭左右扫风"},
    {"关闭上下扫风"},    
    {"语音关闭"},
    {"关闭语音"},    
};

int lcd_showString(int cmd_trig, int index)
{
    //set_ui_layer(0, 0);
   //while (1)
    {
        if(cmd_trig == 0)//RECOGNITION_MODE_TRIGGER, trig mode
        {
            
            Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, WHITE);         
            /*ASCII*/
            lcd_drawlogo(LCD_UI_W - LOGO_W - 1, 1, LOGO_BG_COLOR);
            lcd_draw_text(10, 10, 480, 32, 32, "GREE ELECTRIC APPLIANCES", WORD_COLOR);

            /*中文*/
            lcd_draw_chstr(10, 120, 480, 32, 32, chstr_trig[index], WORD_COLOR);
        }
        else if(cmd_trig == 1)//RECOGNITION_MODE_COMMAND, cmd mode
        {
            Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, WHITE);         
            /*ASCII*/
            lcd_drawlogo(LCD_UI_W - LOGO_W - 1, 1, LOGO_BG_COLOR);
            lcd_draw_text(10, 10, 480, 32, 32, "GREE ELECTRIC APPLIANCES", WORD_COLOR);

            /*中文*/
            lcd_draw_chstr(10, 120, 480, 32, 32, chstr_cmd[index], WORD_COLOR);
        }
        else if(cmd_trig == 2)//RECOGNITION_MODE_COMMAND, cmd mode
        {
            Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, WHITE);         
            /*ASCII*/
            lcd_drawlogo(LCD_UI_W - LOGO_W - 1, 1, LOGO_BG_COLOR);
            lcd_draw_text(10, 10, 480, 32, 32, "GREE ELECTRIC APPLIANCES", WORD_COLOR);

            /*中文*/
            lcd_draw_chstr(10, 120, 480, 32, 32, chstr_1[index], WORD_COLOR);
        }

        k_sleep(1);
    }
}

