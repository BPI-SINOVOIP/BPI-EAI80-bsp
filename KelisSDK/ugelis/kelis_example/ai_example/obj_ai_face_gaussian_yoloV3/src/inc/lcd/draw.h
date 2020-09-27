#ifndef __DRAW_H__
#define __DRAW_H__


typedef struct rect
{
    int x_min;
    int x_max;
    int y_min;
    int y_max;
} rect_t;

typedef struct img_rect
{
    int x_center;
    int y_center;
    int w;
    int h;
} img_rect_t;
void ui_draw_rectpic(img_rect_t *rt, rect_t *face_rt, unsigned char *pic);

void LCD_showstring(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color);
void LCD_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);

#endif
