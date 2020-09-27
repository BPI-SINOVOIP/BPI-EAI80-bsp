#include <display/fb_display.h>
#include <gpio.h>

#include "ai_common.h"
#if defined(F6721B_EVB_176PIN)
    #define PORT     "GPIOD"
    #define LCD_GPIO_PIN   12
#elif defined(F6721B_EVB_QFN80PIN )
    #define PORT     "GPIOC"
    #define LCD_GPIO_PIN   3
#endif
struct device *gpiob;
struct device *ltdc;

struct fb_displayinfo info;
uint8_t pixfmt = FMT_RGB888;

uint32_t ui_buf_addr = MEM_UI_BASE;

//uint32_t pingpongbuf0 = 0x10080000;
//uint32_t pingpongbuf1 = 0x100C0000;

void set_ui_layer(int x, int y)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

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
    wininfo.width   = LCD_UI_W;
    wininfo.height  = LCD_UI_H;

    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
}


struct device *ai_init_lcd(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpiob = device_get_binding(PORT);
    if (!gpiob)
    {
        return NULL;
    }
    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(gpiob, LCD_GPIO_PIN, 0);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);
    if (!ltdc)
    {
        return NULL;
    }

    fb_get_displayinfo(ltdc, &info);

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = MEM_CAM_BASE;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    info.pic_width = CAM_DISP_W;
    info.pic_height = CAM_DISP_H;
    wininfo.x_start = 1;
    wininfo.y_start = 2;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    set_ui_layer(0, 0);

    return ltdc;
}

