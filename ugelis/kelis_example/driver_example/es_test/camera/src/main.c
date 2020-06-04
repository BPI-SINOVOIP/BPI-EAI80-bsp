/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>

#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOB"

#define LCD_GPIO_PIN   13   /*PB13*/

//#define CAPTURE_JPEG
#define CAPTURE_YUV422
//#define CAPTURE_RGB565
#define LCD_DISPLAY

struct device *gpiob;
struct device *ltdc = NULL;
struct device *dcmi = NULL;
struct device *camera_sensor_0;
struct device *camera_sensor_1;

struct fb_displayinfo info;

uint8_t pixfmt = FMT_RGB888;
static int index = 0;

struct buf_info
{
    uint8_t last_layer0_index;
    uint8_t last_layer1_index;
    uint8_t cur_layer0_index;
    uint8_t cur_layer1_index;
};

static struct buf_info buf_mgr;

#ifdef LCD_DISPLAY
static void LCD_isr(struct device *dev)
{
    struct dcmi_buf dis_buf;
    struct dcmi_buf release_buf;
    int ret = 1;

#ifdef CAPTURE_JPEG
    //dcmi_stop_streaming(dcmi);
#endif

    if (dcmi != NULL)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
    }

    if (ret == 0)
    {
#ifdef LCD_DISPLAY
        if (index == 0)
        {
#ifdef CONFIG_OV5640_0
            fb_ioctl(ltdc, 0, FB_X_SETADDR, &(dis_buf.addr));
            buf_mgr.last_layer0_index = buf_mgr.cur_layer0_index;
            buf_mgr.cur_layer0_index = dis_buf.index;

            if (buf_mgr.last_layer0_index != 0xff)
            {
                release_buf.index = buf_mgr.last_layer0_index;
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
            }
#endif
        }
        else if (index == 1)
        {
#ifdef CONFIG_OV5640_1
            fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
            buf_mgr.last_layer1_index = buf_mgr.cur_layer1_index;
            buf_mgr.cur_layer1_index = dis_buf.index;

            if (buf_mgr.last_layer1_index != 0xff)
            {
                release_buf.index = buf_mgr.last_layer1_index;
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
            }
#endif
        }
#endif

#if defined(CONFIG_OV5640_0) &&\
        defined(CONFIG_OV5640_1)
        index = (index == 0) ? 1 : 0;

        dcmi_ioctl(dcmi, VIDIOC_SWITCH_PATH, &index);
#endif

        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

    }
}

void lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpiob = device_get_binding(PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(gpiob, LCD_GPIO_PIN, 0);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

#if (defined CAPTURE_YUV422)
    pixfmt = FMT_RGB888;
    info.pic_width = 240;
    info.pic_height = 160;
#else
    pixfmt = FMT_RGB565;
    info.pic_width = 240;
    info.pic_height = 160;
#endif

#ifdef CONFIG_OV5640_0
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);
#endif

#ifdef CONFIG_OV5640_1
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);
#endif

#ifdef CONFIG_OV5640_0
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    fb_irq_line_enable(ltdc, (info.pic_height - 10));
    fb_irq_callback_set(ltdc, LCD_isr);
#endif

#ifdef CONFIG_OV5640_1
    wininfo.x_start = 200;
    wininfo.y_start = 100;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
#endif
}
#endif

int camera_config(void)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;

#ifdef CONFIG_OV5640_0
    camera_sensor_0 = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor_0, true);

#if (defined CAPTURE_JPEG)
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_JPEG, MODE_QSXGA_2592_1944, FPS_15);
#elif (defined CAPTURE_YUV422)
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
#else
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_RGB565, MODE_QVGA_320_240, FPS_15);
#endif

    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_WHITE_BALANCE, &awb);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    saturation = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_SATURATION, &saturation);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    brightness = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_BRIGHTNESS, &brightness);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    contrast = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_CONTRAST, &contrast);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    sharpness = 33;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_SHARPNESS, &sharpness);
    if (ret != 0)
    {
        return -ENOTSUP;
    }
#endif

#ifdef CONFIG_OV5640_1
    camera_sensor_1 = device_get_binding(OV5640_1_DEV_NAME);

    camera_set_power(camera_sensor_1, true);

#if (defined CAPTURE_JPEG)
    camera_set_fmt(camera_sensor_1, VIDEO_FMT_JPEG, MODE_QSXGA_2592_1944, FPS_15);
#elif (defined CAPTURE_YUV422)
    camera_set_fmt(camera_sensor_1, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
#else
    camera_set_fmt(camera_sensor_1, VIDEO_FMT_RGB565, MODE_QVGA_320_240, FPS_15);
#endif

    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_WHITE_BALANCE, &awb);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    saturation = 0;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_SATURATION, &saturation);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    brightness = 0;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_BRIGHTNESS, &brightness);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    contrast = 0;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_CONTRAST, &contrast);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    sharpness = 33;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_SHARPNESS, &sharpness);
    if (ret != 0)
    {
        return -ENOTSUP;
    }
#endif

    dcmi = device_get_binding(CONFIG_VIN_NAME);

#if (defined CAPTURE_JPEG)
    fmt.pix_fmt = PIX_FMT_JPEG;
    fmt.width = 2592;
    fmt.height = 1944;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    reqbufs.count = 1;
    reqbufs.mem_start = 0xc0a00000;
    reqbufs.length = 6 * 1024 * 1024;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#elif(defined CAPTURE_YUV422)
    fmt.pix_fmt = PIX_FMT_YUV422;
    fmt.width = 320;
    fmt.height = 240;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * info.pic_width - 1;
    rect.height = info.pic_height - 1;

    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    reqbufs.count = 4;
    reqbufs.mem_start = 0xc0a00000;//0x20020000;//0xc0000000;
    reqbufs.length =  info.pic_width * info.pic_height * 3 * 4;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#else
    fmt.pix_fmt = PIX_FMT_RGB565;
    fmt.width = 320;
    fmt.height = 240;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * info.pic_width - 1;
    rect.height = info.pic_height - 1;

    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);


    reqbufs.count = 4;
    reqbufs.mem_start = 0xc0a00000;//0x20020000;//0xc0000000;
    reqbufs.length =  info.pic_width * info.pic_height * 2 * 4;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#endif

    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);

    //dcmi_irq_callback_set(dcmi, camera_isr);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

void main(void)
{
    int ret = 0;
    enum af_mode auto_foucs;

#ifdef LCD_DISPLAY
    lcd_config();
#endif

    index = 0;
    buf_mgr.last_layer0_index = 0xff;
    buf_mgr.last_layer1_index = 0xff;
    buf_mgr.cur_layer0_index = 0xff;
    buf_mgr.cur_layer1_index = 0xff;

    ret = camera_config();
    if (ret != 0)
    {
        printk("there are something wrong\n");
    }

#ifdef CONFIG_OV5640_0
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != 0)
    {
        printk("auto focus fail \n");
    }
#endif

#ifdef CONFIG_OV5640_1
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != 0)
    {
        printk("auto focus fail \n");
    }
#endif

    while (1)
    {
        ;
    }
}
