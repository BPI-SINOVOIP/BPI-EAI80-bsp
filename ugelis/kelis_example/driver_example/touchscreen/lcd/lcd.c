#include <display/fb_display.h>
#include <gpio.h>
#include "lcd.h"

struct device *gpiob;
struct device *ltdc;

struct fb_displayinfo info;
uint8_t pixfmt = FMT_RGB888;

extern uint32_t ui_buf_addr;

void set_touchscreen_layer(int x, int y)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpiob = device_get_binding(LCD_BL_PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_BL_PIN, GPIO_DIR_OUT);
    gpio_pin_write(gpiob, LCD_BL_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_ARGB8888;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = (uint32_t) ui_buf_addr;
    fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = x;
    wininfo.y_start = y;
    wininfo.width   = 480;//LCD_UI_W;
    wininfo.height  = 272;//LCD_UI_H;
    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
}

