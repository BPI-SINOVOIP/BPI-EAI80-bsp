#include <display/fb_display.h>
#include <gpio.h>

#include "ai_common.h"

struct device *bl_gpio;
struct device *ltdc;

struct fb_displayinfo info;
uint8_t pixfmt = FMT_RGB888;

uint32_t ui_buf_addr   = 0x20017000;

uint32_t show_buf_addr = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0x24C00 + 0xC0000000;//0x10424C00 //

void lcd_config(uint32_t fb_addr)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    bl_gpio = device_get_binding(LCD_BL_PORT);

    /* Set LED pin as output */
    gpio_pin_configure(bl_gpio, LCD_BL_PIN, GPIO_DIR_OUT);
    gpio_pin_write(bl_gpio, LCD_BL_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = fb_addr;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    /*First Layer*/
    info.pic_width =  224;
    info.pic_height = 224;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    /*Make the first layer transparent*/
    memset((void *)(show_buf_addr), 0x00, 224 * 224 * 4);
}
