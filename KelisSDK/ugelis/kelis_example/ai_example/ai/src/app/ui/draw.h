
/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          draw.h
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

#ifndef __DRAW_H__
#define __DRAW_H__


#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40
#define BRRED            0XFC07
#define GRAY             0X8430
#define LGRAY            0XC618

#define POINT_COLOR  0x0000F800
#define BACK_COLOR   0x00000000
#define RECT_COLOR   0xFFFFFFFF
#define WARN_COLOR   0xFFFFFF00

typedef struct rect
{
    int x;
    int y;
    int w;
    int h;
} rect_t;

typedef struct dev_ui
{
    int x;
    int y;
    int w;
    int h;
    int buf;
    int buf_size;
    void *ltdc;
    int pixel_fmt;
    int pixel_size;
} dev_ui_t;

void ui_init(int x, int y, int w, int h, int buf, int pixel_size, void *ltdc);

void ui_draw_point(int dx, int dy, int color);

void ui_draw_rectangle(int x1, int y1, int x2, int y2, int color);
void ui_draw_focus(int x1, int y1, int x2, int y2, int color);

//bbggrr24(h-l) ==> argb32
void ui_draw_rgb(unsigned char *src, int x, int y, int w, int h);

//rgbrgb(h-l) ==> argb32
void ui_draw_rgb24(unsigned char *src, int x, int y, int w, int h);
void ui_draw_pic(int x, int y, int w, int h, unsigned char *pic, char alpha);
void ui_clear(void);
void ui_draw_text(int x, int y, int width, int height, int size, char *p, int color);

int ui_rel(int xy, int src_wh, int wh, int off);
void ui_clear_area(int x, int y, int w, int h);
void ui_draw_chstr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *chstr, uint32_t color);

dev_ui_t *ui_get_handler(void);

#endif //__DRAW_H__
