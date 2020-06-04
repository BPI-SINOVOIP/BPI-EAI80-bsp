#include <display/fb_display.h>
#include "font.h"

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

#define POINT_COLOR      0x0000F800
#define BACK_COLOR       0x00000000
#define RECT_COLOR       0xFFFF0000


#define PIC_WIDTH  96
#define PIC_HEIGHT 96

extern uint32_t show_buf_addr;
extern struct device *ltdc;

//ARGB8888
void LCD_clear(void)
{
    uint32_t layer_onoff = 0;

    /* close layer 1 */
    fb_ioctl(ltdc, 1, FB_X_SETVIS, &layer_onoff);

    /*Layer 1*/
    memset((unsigned char *)show_buf_addr, 0x000000,  224 * 224 * 4);
}

static void LCD_draw_point(uint16_t dx, uint16_t dy, uint32_t color)
{
    *(uint32_t *)((uint32_t)show_buf_addr + (224 * dy +  dx) * 3) = color; //PIC_WIDTH
    //*(uint32_t*)((uint32_t)show_buf_addr + (224 *dy +  dx) * 4) = color;
}


static void LCD_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
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
        LCD_draw_point(uRow, uCol, color); //画点  //RECT_COLOR

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

static void LCD_draw_point2orgpic(uint8_t *src, uint16_t dx, uint16_t dy, uint32_t color)
{
    *(uint8_t *)((uint32_t)src + (224 * dy +  dx) * 3) = color & 0xFF;
    *(uint8_t *)((uint32_t)src + (224 * dy +  dx) * 3 + 1) = (color >> 8) & 0xFF;
    *(uint8_t *)((uint32_t)src + (224 * dy +  dx) * 3 + 2) = (color >> 16) & 0xFF;
}

static void LCD_draw_line2orgpic(uint8_t *src, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
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
        LCD_draw_point2orgpic(src, uRow, uCol, color); //画点  //RECT_COLOR

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


/*
 * x , cow  --> width
 * y , line --> height
 */
void LCD_draw_detobjects(uint8_t *src, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    int l = 0, c = 0;
    uint8_t r, g, b;
    uint32_t rgb = 0;

    for (l = 0; l < h; l ++)
    {
        for (c = 0; c < w; c ++)
        {
            r = *(src + ((y + l) * 224 + x + c) * 3)  ;      //224 is width
            g = *(src + ((y + l) * 224 + x + c) * 3 + 1);
            b = *(src + ((y + l) * 224 + x + c) * 3 + 2);
            rgb = b << 16 | g << 8 | r;
            *(uint32_t *)((uint32_t)show_buf_addr + ((y + l) * 224 + x + c) * 4) = 0xFF << 24 | rgb;
        }
    }
}

void LCD_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    LCD_draw_line(x1, y1, x2, y1, color);
    LCD_draw_line(x1, y1, x1, y2, color);
    LCD_draw_line(x1, y2, x2, y2, color);
    LCD_draw_line(x2, y1, x2, y2, color);
}

/*Draw a rectangle on orginal picture*/
void LCD_draw_rectangle2orgpic(uint8_t *src, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    LCD_draw_line2orgpic(src, x1, y1, x2, y1, color);
    LCD_draw_line2orgpic(src, x1, y1, x1, y2, color);
    LCD_draw_line2orgpic(src, x1, y2, x2, y2, color);
    LCD_draw_line2orgpic(src, x2, y1, x2, y2, color);
}

//rrggbb24 ==> argb32
void LCD_draw_rgb24(unsigned char *src, int x, int y, int w, int h)
{
    int i, j, cnt = 0;
    unsigned int color = 0xff000000;
    unsigned char *b = src;
    unsigned char *g = src + w * h;
    unsigned char *r = src + w * h * 2;

    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            color = 0xff000000;
            color |= *(b + cnt);//b
            color |= *(g + cnt) << 8; //g
            color |= *(r + cnt) << 16; //r
            LCD_draw_point(x + i, y + j, color);
            cnt++;
        }
    }
}

static void LCD_showchar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode, uint32_t color)
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
        else if (size == 16)
        {
            temp = asc2_1608[num][t];
        }
        else if (size == 24)
        {
            temp = asc2_2412[num][t];
        }
        else if (size == 32)
        {
            temp = asc2_3216[num][t];
        }
        else
        {
            return;
        }
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                LCD_draw_point(x, y, color);
            }
            else if (mode == 0)
            {
                LCD_draw_point(x, y, BACK_COLOR);
            }
            temp <<= 1;
            y++;
            if (y >= PIC_HEIGHT)
            {
                return;
            }
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= PIC_WIDTH)
                {
                    return;
                }
                break;
            }
        }
    }
}

void LCD_showstring(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color)
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
        LCD_showchar(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}
