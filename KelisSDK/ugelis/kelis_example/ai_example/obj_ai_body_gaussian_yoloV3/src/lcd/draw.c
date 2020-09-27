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

static void LCD_draw_point(uint16_t dx, uint16_t dy, uint32_t color)
{
    *(uint32_t *)((uint32_t)show_buf_addr + (288 * dy +  dx) * 3) = color; //PIC_WIDTH
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

void LCD_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    LCD_draw_line(x1, y1, x2, y1, color);
    LCD_draw_line(x1, y1, x1, y2, color);
    LCD_draw_line(x1, y2, x2, y2, color);
    LCD_draw_line(x2, y1, x2, y2, color);
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
