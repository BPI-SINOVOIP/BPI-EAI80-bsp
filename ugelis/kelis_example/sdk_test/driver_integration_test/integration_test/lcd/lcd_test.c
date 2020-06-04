/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_vin.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>
#include <imlib.h>
#include "lcd_test.h"
#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOD"

/* Change this to set different pix format */
#define PIXFORMAT0   FMT_RGB888
#define PIXFORMAT1   FMT_AL44

#define LCD_GPIO_PIN   12   /*PB13*/

static struct device *gpiod;
static struct device *ltdc = NULL;
static struct device *dcmi = NULL;
static struct device *camera_sensor_0;
static struct device *camera_sensor_1;

static struct fb_displayinfo info;

static uint8_t pixfmt = FMT_RGB888;
static int index = 0;

struct buf_info
{
    uint8_t last_layer0_index;
    uint8_t last_layer1_index;
    uint8_t cur_layer0_index;
    uint8_t cur_layer1_index;
};

static struct buf_info buf_mgr;

uint32_t clut[256] =
{
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
    0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff,
};

static void imlib_rgb888_to_background(uint32_t src_addr, uint32_t sizeOfPixel)
{
    int i = 0;

    for (i = 0; i < sizeOfPixel; i++)
    {
        if (i < (sizeOfPixel >> 1))
        {
            (*(uint8_t *)src_addr) = 0x0;
            src_addr++;
            (*(uint8_t *)src_addr) = 0x0;
            src_addr++;
            (*(uint8_t *)src_addr) = 0xff;
            src_addr++;
        }
        else
        {
            (*(uint8_t *)src_addr) = 0x0;
            src_addr++;
            (*(uint8_t *)src_addr) = 0xff;
            src_addr++;
            (*(uint8_t *)src_addr) = 0x0;
            src_addr++;
        }
    }
}

static void LCD_isr(struct device *dev)
{
    struct dcmi_buf dis_buf;
    struct dcmi_buf release_buf;
    int ret = 1;
    if (dcmi != NULL)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
    }

    if (ret == 0)
    {
        if (index == 0)
        {
            fb_ioctl(ltdc, 0, FB_X_SETADDR, &(dis_buf.addr));
            buf_mgr.last_layer0_index = buf_mgr.cur_layer0_index;
            buf_mgr.cur_layer0_index = dis_buf.index;

            if (buf_mgr.last_layer0_index != 0xff)
            {
                release_buf.index = buf_mgr.last_layer0_index;
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
            }
        }
        else if (index == 1)
        {
            fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
            buf_mgr.last_layer1_index = buf_mgr.cur_layer1_index;
            buf_mgr.cur_layer1_index = dis_buf.index;

            if (buf_mgr.last_layer1_index != 0xff)
            {
                release_buf.index = buf_mgr.last_layer1_index;
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
            }
        }
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

    }
}

static int lcd_config(enum fb_pixel_format format0, enum fb_pixel_format format1)
{
    RET_CODE ret = RET_OK;
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
    uint32_t onoff = 1;
    uint32_t color = 0xff0000;

    gpiod = device_get_binding(PORT);
    if (!gpiod)
    {
        TC_PRINT("Cann't get GM-GPIOB Device\n");
        return TC_FAIL;
    }

    /* Set LED pin as output */
    gpio_pin_configure(gpiod, LCD_GPIO_PIN, GPIO_DIR_OUT);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    gpio_pin_write(gpiod, LCD_GPIO_PIN, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ltdc = device_get_binding(CONFIG_VOUT_NAME);
    if (!ltdc)
    {
        TC_PRINT("Cann't get GM-LCD Device\n");
        return TC_FAIL;
    }

    ret = fb_get_displayinfo(ltdc, &info);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    /*config layer 0 and 1*/
    /*layer 0*/
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = format0;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    ret = fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    ret = fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = fb_ioctl(ltdc, 0, FB_X_SETCHROMAMODE, &onoff);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = fb_ioctl(ltdc, 0, FB_X_SETCHROMA, &color);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /*layer 1*/
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = format1;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    ret = fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 200;
    wininfo.y_start = 100;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    ret = fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = fb_ioctl(ltdc, 1, FB_X_SETCHROMAMODE, &onoff);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = fb_ioctl(ltdc, 1, FB_X_SETCHROMA, &color);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    fb_irq_line_enable(ltdc, 239);
    fb_irq_callback_set(ltdc, LCD_isr);

    return ret;
}

static int camera_config(enum dcmi_pixel_format format)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;

    camera_sensor_0 = device_get_binding(OV5640_0_DEV_NAME);
    if (!camera_sensor_0)
    {
        TC_PRINT("Cann't get GM-CAMERA Device\n");
        return TC_FAIL;
    }

    ret = camera_set_power(camera_sensor_0, true);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = camera_set_fmt(camera_sensor_0, format, MODE_QVGA_320_240, FPS_15);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_WHITE_BALANCE, &awb);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    saturation = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_SATURATION, &saturation);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    brightness = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_BRIGHTNESS, &brightness);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    contrast = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_CONTRAST, &contrast);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    sharpness = 33;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_SHARPNESS, &sharpness);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    dcmi = device_get_binding(CONFIG_VIN_NAME);
    if (!dcmi)
    {
        TC_PRINT("Cann't get GM-VIN Device\n");
        return TC_FAIL;
    }


    fmt.pix_fmt = format;
    fmt.width = 320;
    fmt.height = 240;
    ret = dcmi_set_fmt_vid_cap(dcmi, &fmt);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * info.pic_width - 1;
    rect.height = info.pic_height - 1;

    ret = dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    int bytePerPix;
    switch (format)
    {
        case PIX_FMT_RGB565:
            bytePerPix = 2;
            break;
        case PIX_FMT_YUV422:
            bytePerPix = 3;
            ret = dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);
            if (ret != RET_OK)
            {
                TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
                return TC_FAIL;
            }
            break;
    }
    reqbufs.count = 3;
    reqbufs.mem_start = 0xc0a00000;//0x20020000;//0xc0000000;
    reqbufs.length =  info.pic_width * info.pic_height * bytePerPix * 3;
    ret = dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    buf.index = 0;
    ret = dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    //dcmi_irq_callback_set(dcmi, camera_isr);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    return 0;
}

static int test_lcd_rgb5652al44(void)
{
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Lcd rgb565 convert to al44 test start");
    TC_PRINT("----------------------------------\n");

    RET_CODE ret = 0;
    struct dcmi_buf dis_buf;
    enum af_mode auto_foucs;
    uint32_t addr = 0;
    uint32_t sizeOfPixel = 0;//320 * 240;
    uint8_t *gray_pic;
    uint8_t *gray_pic_0 = 0xc0a6B000;
    uint8_t *gray_pic_1 = 0xc0a7DC00;
    uint32_t *argb8888 = 0xc0a45800;
    bool flag = true;

    gray_pic = gray_pic_0;
    sizeOfPixel = 240 * 160;
    ret = lcd_config(FMT_AL44, FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = camera_config(PIX_FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    uint32_t time = 100000;

    while (time--)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
        if (ret == 0)
        {
            if (/*flag*/1)
            {
                imlib_rgb565_to_argb8888(dis_buf.addr, argb8888, 0xff, sizeOfPixel);
                imlib_argb8888_to_al44((uint32_t)argb8888, gray_pic, sizeOfPixel);
                addr = (uint32_t)gray_pic;

                fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
                fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));

                dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

                gray_pic = (gray_pic == gray_pic_0) ? gray_pic_1 : gray_pic_0;
                flag = false;
            }
        }
    }
    TC_PRINT("Testcase %s passed!\n", __func__);
    TC_PRINT("Now the top left show a grayscale photo \n");

    dcmi_stop_streaming(dcmi);
    return TC_PASS;
}

static int test_lcd_color_key(void)
{
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Lcd color key test start");
    TC_PRINT("----------------------------------\n");

    RET_CODE ret = 0;
    struct dcmi_buf dis_buf;
    enum af_mode auto_foucs;
    uint32_t addr = 0;
    uint32_t sizeOfPixel = 0;
    uint8_t *gray_pic;
    uint8_t *gray_pic_0 = 0xc0a6B000;
    uint8_t *gray_pic_1 = 0xc0a7DC00;
    uint32_t *argb8888 = 0xc0a45800;
    bool flag = true;
    uint32_t color = 0x00ff00;

    gray_pic = gray_pic_0;
    sizeOfPixel = 240 * 160;
    ret = lcd_config(FMT_RGB888, FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = camera_config(PIX_FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    //mi_stop_streaming(dcmi);

    uint32_t time = 100000;
    while (time--)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
        if (ret == 0)
        {
            imlib_rgb888_to_background(gray_pic_0, sizeOfPixel);
            fb_ioctl(ltdc, 0, FB_X_SETADDR, &(gray_pic_0));
            fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
            dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
        }
        flag = false;
    }

    dcmi_stop_streaming(dcmi);
    TC_PRINT("Testcase %s passed!\n", __func__);
    TC_PRINT("Now the top left show a half green photo \n");

    return TC_PASS;
}

static int test_lcd_clut(void)
{
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Lcd clut test start");
    TC_PRINT("----------------------------------\n");

    RET_CODE ret = 0;
    struct dcmi_buf dis_buf;
    struct fb_clutinfo clut_info;

    enum af_mode auto_foucs;
    uint32_t addr = 0;
    uint32_t sizeOfPixel = 0;
    uint8_t *gray_pic;
    uint8_t *gray_pic_0 = 0xc0a6B000;
    uint8_t *gray_pic_1 = 0xc0a7DC00;
    uint32_t *argb8888 = 0xc0a45800;
    bool flag = true;
    uint32_t pre_multiply = 1;

    gray_pic = gray_pic_0;
    sizeOfPixel = 240 * 160;
    ret = lcd_config(FMT_AL44, FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    fb_ioctl(ltdc, 0, FB_X_SETPREMUL, &pre_multiply);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    clut_info.enable = true;
    clut_info.clut_size = 256;
    clut_info.clut_addr = clut;

    fb_ioctl(ltdc, 0, FB_X_SETLUTENTRY, &clut_info);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = camera_config(PIX_FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    uint32_t time = 100000;
    while (time--)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        if (ret == 0)
        {
            if (/*flag*/1)
            {
                imlib_rgb565_to_argb8888(dis_buf.addr, argb8888, 0x80, sizeOfPixel);
                imlib_argb8888_to_al44((uint32_t)argb8888, gray_pic, sizeOfPixel);
                addr = (uint32_t)gray_pic;

                fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));
                fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

                gray_pic = (gray_pic == gray_pic_0) ? gray_pic_1 : gray_pic_0;
            }
        }
    }
    dcmi_stop_streaming(dcmi);
    TC_PRINT("Testcase %s passed!\n", __func__);
    TC_PRINT("Now the top left show a pure gray photo \n");

    return TC_PASS;
}

void lcd_test(void)
{
    TC_PRINT("================LCD Test Start=================\n\n\n");
    TC_PRINT("TEST1\n");
    zassert_true(test_lcd_rgb5652al44() == TC_PASS, NULL);
    TC_PRINT("TEST2\n");
    zassert_true(test_lcd_color_key() == TC_PASS, NULL);
    TC_PRINT("TEST3\n");
    zassert_true(test_lcd_clut() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);

    TC_PRINT("================LCD Test  End==================\n\n\n");
}
