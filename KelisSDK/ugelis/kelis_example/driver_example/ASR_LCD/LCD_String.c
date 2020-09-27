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
   {"��˵������"},
};

char chstr_trig[][MAX_CHSTR_LEN] =
{
    {"������"},
};
char chstr_cmd[][MAX_CHSTR_LEN] =
{
    {"�򿪿յ�"},
    {"�رտյ�"},
    {"ʮ����"},
    {"ʮ�߶�"},
    {"ʮ�˶�"},
    {"ʮ�Ŷ�"},
    {"��ʮ��"},
    {"��ʮһ��"},
    {"��ʮ����"},
    {"��ʮ����"},
    {"��ʮ�Ķ�"},
    {"��ʮ���"},
    {"��ʮ����"},
    {"��ʮ�߶�"},
    {"��ʮ�˶�"},
    {"��ʮ�Ŷ�"},
    {"��ʮ��"},
    {"�����¶�"},
    {"�����¶�"},
    {"�Զ�ģʽ"},
    {"����ģʽ"},
    {"��ʪģʽ"},
    {"�ͷ�ģʽ"},
    {"����ģʽ"},
    {"����"},
    {"��С��"},
    {"�������"},
    {"��С����"},
    {"�Զ���"},
    {"��������"},
    {"��С����"},
    {"������ɨ��"},
    {"������ɨ��"},
    {"�ر�����ɨ��"},
    {"�ر�����ɨ��"},    
    {"�����ر�"},
    {"�ر�����"},    
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

            /*����*/
            lcd_draw_chstr(10, 120, 480, 32, 32, chstr_trig[index], WORD_COLOR);
        }
        else if(cmd_trig == 1)//RECOGNITION_MODE_COMMAND, cmd mode
        {
            Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, WHITE);         
            /*ASCII*/
            lcd_drawlogo(LCD_UI_W - LOGO_W - 1, 1, LOGO_BG_COLOR);
            lcd_draw_text(10, 10, 480, 32, 32, "GREE ELECTRIC APPLIANCES", WORD_COLOR);

            /*����*/
            lcd_draw_chstr(10, 120, 480, 32, 32, chstr_cmd[index], WORD_COLOR);
        }
        else if(cmd_trig == 2)//RECOGNITION_MODE_COMMAND, cmd mode
        {
            Creat_pic_bg(LCD_UI_W, LCD_UI_H, ui_buf_addr, WHITE);         
            /*ASCII*/
            lcd_drawlogo(LCD_UI_W - LOGO_W - 1, 1, LOGO_BG_COLOR);
            lcd_draw_text(10, 10, 480, 32, 32, "GREE ELECTRIC APPLIANCES", WORD_COLOR);

            /*����*/
            lcd_draw_chstr(10, 120, 480, 32, 32, chstr_1[index], WORD_COLOR);
        }

        k_sleep(1);
    }
}

