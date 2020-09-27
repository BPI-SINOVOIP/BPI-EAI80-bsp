/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          draw.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Common UI interface.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <inc/config.h>
#include "draw.h"
#include "font.h"

static uint32_t ui_buf_addr = 0;

void ui_set_addr(uint32_t addr)
{
    ui_buf_addr = addr;
}

static void ui_draw_point(uint16_t dx, uint16_t dy, uint32_t color)
{
    *(uint32_t *)((uint32_t)ui_buf_addr + (224 * dy +  dx) * 3) = color;
}

static void ui_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
    {
        incx = 1;    //设置单步方向
    }
    else if (delta_x == 0)
    {
        incx = 0;    //垂直线
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;    //水平线
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
    {
        distance = delta_x;    //选取基本增量坐标轴
    }
    else
    {
        distance = delta_y;
    }
    for (t = 0; t <= distance + 1; t++) //画线输出
    {
        ui_draw_point(uRow, uCol, color); //画点  //RECT_COLOR

        xerr += delta_x ;
        yerr += delta_y ;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

static void ui_draw_cross(uint16_t x1, uint16_t y1, int pos, uint32_t color)
{
    int len = 10;
    if (pos == 0)   //left top
    {
        ui_draw_line(x1, y1, x1 + len, y1, color);
        ui_draw_line(x1, y1, x1, y1 + len, color);
    }
    else if (pos == 1) //right top
    {
        ui_draw_line(x1 - len, y1, x1, y1, color);
        ui_draw_line(x1, y1, x1, y1 + len, color);
    }
    else if (pos == 2) //left bottom
    {
        ui_draw_line(x1, y1 - len, x1, y1, color);
        ui_draw_line(x1, y1, x1 + len, y1, color);
    }
    else if (pos == 3) //right bottom
    {
        ui_draw_line(x1 - len, y1, x1, y1, color);
        ui_draw_line(x1, y1 - len, x1, y1, color);
    }
}

void ui_draw_rectangle(int x1, int y1, int x2, int y2, int color)
{
    ui_draw_line(x1, y1, x2, y1, color);
    ui_draw_line(x1, y1, x1, y2, color);
    ui_draw_line(x1, y2, x2, y2, color);
    ui_draw_line(x2, y1, x2, y2, color);
}

void ui_draw_focus(int x1, int y1, int x2, int y2, int color)
{
    ui_draw_cross(x1, y1, 0, color);
    ui_draw_cross(x2, y1, 1, color);
    ui_draw_cross(x1, y2, 2, color);
    ui_draw_cross(x2, y2, 3, color);
}


//bbggrr24(h-l) ==> argb32
void ui_draw_rgb(unsigned char *src, int x, int y, int w, int h)
{
    int i, j, cnt = 0;
    unsigned int color = 0xff000000;
    unsigned char *b = src;
    unsigned char *g = src + w * h;
    unsigned char *r = src + w * h * 2;
    ui_clear();
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            color = 0xff000000;
            color |= *(r + cnt);//r
            color |= *(g + cnt) << 8; //g
            color |= *(b + cnt) << 16; //b
            ui_draw_point(x + i, y + j, color);
            cnt++;
        }
    }
}

//rgbrgb(h-l) ==> argb32
void ui_draw_rgb24(unsigned char *src, int x, int y, int w, int h)
{
    int i, j, cnt = 0;
    unsigned int color = 0xff000000;
    unsigned char *rgb = src;
    ui_clear();
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            color = 0xff000000;
            color |= ((rgb[0]) | (rgb[1] << 8) | (rgb[2] << 16));
            rgb += 3;
            ui_draw_point(x + i, y + j, color);
        }
    }
}

//rgbrgb(h-l) ==> argb32
void ui_draw_l8(unsigned char *src, int x, int y, int w, int h)
{
    int i, j, cnt = 0;
    unsigned int color = 0xff000000;
    unsigned char *rgb = src;
    ui_clear();
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            color = 0xff000000;
            color |= ((0) | (rgb[0] << 8) | (0 << 16));
            rgb += 1;
            ui_draw_point(x + i, y + j, color);
        }
        cnt = 0;
    }
}

//@Todo: Alpha blending problems
void ui_draw_pic(rect_t *rt, unsigned char *pic, char alpha)
{
    int j = 0;
    int i = 0;
    int index = 0;
    int x = rt->x;
    int y = rt->y;
    int w = rt->w;
    int h = rt->h;
    unsigned int *addr = (unsigned int *)pic;
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            if (alpha != 0)
            {
                addr[index] &= (0x00FFFFFF);
                addr[index] |= (alpha << 24);
            }
            ui_draw_point(x + i, y + j, addr[index]);
            index += 1;
        }
    }
}

void ui_clear(void)
{
    memset((unsigned char *)ui_buf_addr, 0, LCD_UI_W * LCD_UI_H * 4);
}

static void ui_draw_char(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode, uint32_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    num = num - ' ';
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
        {
            temp = asc2_1206[num][t];
        }
        else
        {
            return;
        }
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                ui_draw_point(x, y, color);
            }
            else if (mode == 0)
            {
                ui_draw_point(x, y, BACK_COLOR);
            }
            temp <<= 1;
            y++;
            if (y >= LCD_UI_H)
            {
                return;
            }
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= LCD_UI_W)
                {
                    return;
                }
                break;
            }
        }
    }
}


void ui_draw_text(int x, int y, int width, int height, int size, char *p, int color)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' '))
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)
        {
            break;
        }
        ui_draw_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

