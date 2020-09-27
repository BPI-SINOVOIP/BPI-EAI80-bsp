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

void lcd_clear(void);
void lcd_clear_area(int x, int y, int w, int h);
void lcd_draw_text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint32_t color);

#endif
