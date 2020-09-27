#ifndef __DRAW_H__
#define __DRAW_H__

void LCD_clear(void);
void LCD_showstring(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color);
void LCD_draw_detobjects(uint8_t *src, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif
