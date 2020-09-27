#ifndef __LCD_H__
#define __LCD_H__

/*--------------------------------------------
 *lcd camera
--------------------------------------------*/
struct device *ai_init_lcd(void);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void lcd_draw_rgb24(unsigned char *src, int x, int y, int w, int h);
void set_ui_layer(int x, int y);

#endif//__LCD_H__
