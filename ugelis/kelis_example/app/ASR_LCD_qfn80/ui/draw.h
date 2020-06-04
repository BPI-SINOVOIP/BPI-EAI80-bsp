#ifndef __DRAW_H__
#define __DRAW_H__
#include "ui.h"

#define POINT_COLOR     0x0000F800
#define BACK_COLOR      0x00000000
#define RECT_COLOR      0xFFFFFFFF
#define WARN_COLOR      0xFFFFFF00
#define WORD_COLOR      0xFFFF0000
#define LOGO_BG_COLOR   0xFFFF0000
#define LAYER_COLOR     0xFFFFFF
#define BLACK           0xFF000000
#define WHITE           0xFFFFFFFF
#define RED             0xFFFF0000
#define GREEN           0xFF00FF00
#define BLUE            0xFF0000FF

#define LOGO_W 32
#define LOGO_H 32
#define LOGOSIZE (LOGO_W*LOGO_H*4)

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void lcd_clear(void);
void lcd_clear_area(int x, int y, int w, int h);
void lcd_draw_text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color);
void lcd_drawlogo(int x, int y, unsigned int color);
void lcd_draw_chstr(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *chstr, uint32_t color);
#endif
