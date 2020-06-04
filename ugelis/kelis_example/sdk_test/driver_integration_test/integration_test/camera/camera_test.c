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

#include "camera_test.h"
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

struct buf_info
{
    uint8_t last_layer0_index;
    uint8_t last_layer1_index;
    uint8_t cur_layer0_index;
    uint8_t cur_layer1_index;
};

static struct buf_info buf_mgr;

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
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &(dis_buf.addr));
        buf_mgr.last_layer0_index = buf_mgr.cur_layer0_index;
        buf_mgr.cur_layer0_index = dis_buf.index;

        if (buf_mgr.last_layer0_index != 0xff)
        {
            release_buf.index = buf_mgr.last_layer0_index;
            dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
        }
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

    }
}

static int lcd_config(enum fb_pixel_format format0)
{
    RET_CODE ret = RET_OK;
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
    uint32_t onoff = 0;
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

    fb_irq_line_enable(ltdc, (info.pic_height - 10));
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
    reqbufs.count = 4;
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

static int test_camera(void)
{
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("camera test start");
    TC_PRINT("----------------------------------\n");

    RET_CODE ret = 0;
    enum af_mode auto_foucs;
    uint64_t time1, time2;
    ret = lcd_config(FMT_RGB565);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    buf_mgr.last_layer0_index = 0xff;
    buf_mgr.last_layer1_index = 0xff;
    buf_mgr.cur_layer0_index = 0xff;
    buf_mgr.cur_layer1_index = 0xff;

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

    TC_PRINT("Now the camera format is RGB565!\n");
    TC_PRINT("Waiting for 5s switching to YUV422 !\n");

    k_sleep(5000);

    dcmi_stop_streaming(dcmi);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = lcd_config(FMT_RGB888);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = camera_config(PIX_FMT_YUV422);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    TC_PRINT("Now the camera format is YUV422!\n");

    TC_PRINT("Testcase %s passed!\n", __func__);
    return TC_PASS;

    while (1)
    {
    }
}

void camera_test(void)
{
    TC_PRINT("================CAMERA Test Start=================\n\n\n");
    TC_PRINT("TEST1\n");
    zassert_true(test_camera() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);

    TC_PRINT("================CAMERA Test  End==================\n\n\n");
}
