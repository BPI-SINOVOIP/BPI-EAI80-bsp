#ifndef __DRAW_H__
#define __DRAW_H__

#define WHITE            0xFFFFFFFF
#define BLACK            0xFF000000
#define BLUE             0xFF0000FF
#define RED              0xFFFF0000
#define GREEN            0xFF00FF00
#define GRED             0xFFCCCCCC
#define BROWN            0xFF802A2A

#define TEXT_BACK_COLOR   0x00000000
#define RECT_COLOR   0xFFFFFFFF
#define WARN_COLOR   0xFFFFFF00

#define LCD_UI_W    480
#define LCD_UI_H    272

void lcd_clear(void);
void lcd_clear_area(int x, int y, int w, int h);
void lcd_draw_text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint32_t color);
void Load_Drow_Dialog(void);
#endif
