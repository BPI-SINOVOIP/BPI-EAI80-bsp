#ifndef __DRAW_H__
#define __DRAW_H__
#include "ui.h"

#define WHITE            0xFFFFFF
#define BLACK            0x000000
#define BLUE             0x0000FF
#define RED              0x00FF0000


#define POINT_COLOR  0x0000F800
#define BACK_COLOR   0x00000000
#define RECT_COLOR   0xFFFFFFFF
#define WARN_COLOR   0xFFFFFF00

void lcd_clear(void);
void lcd_clear_area(int x, int y, int w, int h);
void lcd_draw_text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color);

#endif
