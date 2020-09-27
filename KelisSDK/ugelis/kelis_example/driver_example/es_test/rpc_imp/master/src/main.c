/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>

#include <imlib.h>
#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_vout.h>
#include <gm_hal_imp.h>
#include <gm_irq.h>

#include <rpc_core.h>
#include <rpc_plugin_imp.h>


#include "img_data.h"


#define SAMPLE_DEBUG 1

#ifdef SAMPLE_DEBUG
    #define sample_debug       printk
#else
    #define sample_debug(...)  do{}while(0);
#endif



#define LCD_GPIO_PORT    "GPIOB"
#define LCD_GPIO_PIN     13   /*PB13*/


static struct device *ltdc;
struct fb_displayinfo info;

static uint8_t pic_in[320 * 240 * 3];
static uint8_t pic_out[320 * 240 * 3];

static int32_t time_old = 0;
static int32_t time_new = 0;
static int32_t time_diff = 0;




static void LCD_set_pixfmt(uint32_t output_pixfmt)
{
    uint32_t pre_multiply = 0;

#if defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY)
    struct fb_wininfo wininfo;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = SCALE_WIN_WIDTH;
    wininfo.height = SCALE_WIN_HEIGHT;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);
#endif

    HAL_VOUT_SetPixelFormat((VOUT_Device_T *)0x40026000, output_pixfmt, 0);

    if (output_pixfmt == FMT_ARGB8888 || output_pixfmt == FMT_ARGB1555
            || output_pixfmt == FMT_ARGB4444 || output_pixfmt == FMT_AL44
            || output_pixfmt == FMT_AL88)
    {
        pre_multiply = 1;
    }
    else
    {
        pre_multiply = 0;
    }

    fb_ioctl(ltdc, 0, FB_X_SETPREMUL, &pre_multiply);
}

static void LCD_isr(struct device *dev)
{
    //do nothing
    return;
}

void lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
    struct device *gpiob;
    uint8_t pixfmt = FMT_RGB888;

    gpiob = device_get_binding(LCD_GPIO_PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(gpiob, LCD_GPIO_PIN, 0);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

    pixfmt = FMT_RGB565;
    info.pic_width = DATA_WIDTH;//240;
    info.pic_height = DATA_HEIGHT;//160;

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0000000;
    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);


    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;
    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);
    fb_irq_line_enable(ltdc, (info.pic_height - 10));
    fb_irq_callback_set(ltdc, LCD_isr);

}

int main(void)
{
    RPC_Package_T pack;
    IMP_Data_T imp_data;
    RET_CODE ret = RET_OK;
    uint32_t image_width;
    uint32_t image_height;
    uint32_t addr = 0;

    *(volatile uint32_t *)0x40000090 |= (0x2 << 0);
    lcd_config();

    LCD_set_pixfmt(FMT_L8);

    /* prepare buffer */
    memset(pic_out, 0, sizeof(pic_out));
    memcpy(pic_in, img_gray_lena, sizeof(img_gray_lena));
    image_width = DATA_WIDTH;
    image_height = DATA_HEIGHT;

    while (1)
    {
        //k_sleep(1000);
        //memset(pic_out, 0, sizeof(pic_out));/* clear pic_out to check result */

        time_old = k_uptime_get_32();

#if 1
        /* prepare private data */
        imp_data.impCmd = IMP_CMD_SOBEL;
        imp_data.pSrc = pic_in;
        imp_data.pDst = pic_out;
        imp_data.width = image_width;
        imp_data.height = image_height / 2;
        /* prepare rpc pack */
        pack.cmd = RPC_CMD_IMP;
        pack.privData = &imp_data;
        ret = RPC_MasterSendPack(&pack);
        if (ret != RET_OK)
        {
            sample_debug("!!! RPC_MasterSendPack wrong \n");
            continue;
        }

        imlib_sobel(pic_in + (image_height / 2)*image_width, image_height / 2, image_width, pic_out + (image_height / 2)*image_width);

        //if(RPC_MasterWaitResp(&pack, TIMEOUT_WAIT_FOREVER) != RET_OK)
        if ((RPC_MasterWaitResp(&pack, 2000) != RET_OK) || (pack.result != RET_OK))
        {
            sample_debug("!!! sobel_slave wrong \n");
            continue;
        }
#else
        imlib_sobel(pic_in, image_height, image_width, pic_out);
#endif

        time_new = k_uptime_get_32();
        time_diff = time_new - time_old;
        sample_debug("cost: %d ms\n", time_diff);

        addr = (uint32_t)pic_out;
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));
    }

    return 0;
}

