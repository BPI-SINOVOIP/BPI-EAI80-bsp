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
#include <imlib.h>

#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_vout.h>

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOB"

#define BLACK            0x0000
#define BLUE             0x001F

#define LCD_GPIO_PIN   13   /*PB13*/

//#define COLOR_KEY_CASE 1
//#define CLUT_CASE 1

struct device *gpiob;
struct device *ltdc;
struct device *dcmi;
struct device *camera_sensor;

struct fb_displayinfo info;

uint8_t pixfmt = FMT_RGB565;//FMT_ARGB1555;//FMT_L8;//FMT_ARGB4444;//FMT_AL88;//FMT_AL44;//FMT_ARGB8888;
uint32_t lcd_buf0 = 0xc0000000;

static void LCD_isr(struct device *dev)
{
    fb_irq_line_disable(ltdc);
}

#ifdef CLUT_CASE
uint32_t clut[256] =
{
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
    0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000, 0xff0000,
};
#endif

#ifdef COLOR_KEY_CASE
void imlib_rgb888_to_background(uint32_t src_addr, uint32_t sizeOfPixel)
{
    uint8_t red, green, blue;
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
#endif

void lcd_config(void)
{
    struct fb_fillrect rect;
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
    struct fb_clutinfo clut_info;
    uint32_t onoff = 1;
    uint32_t color = 0xff0000;
    uint32_t pre_multiply = 1;

    gpiob = device_get_binding(PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

#if (defined COLOR_KEY_CASE)
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_RGB888;//pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    fb_ioctl(ltdc, 0, FB_X_SETCHROMAMODE, &onoff);
    fb_ioctl(ltdc, 0, FB_X_SETCHROMA, &color);
#elif (defined CLUT_CASE)
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_AL44;//FMT_L8;//pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);
    fb_ioctl(ltdc, 0, FB_X_SETPREMUL, &pre_multiply);

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    clut_info.enable = true;
    clut_info.clut_size = 256;
    clut_info.clut_addr = clut;

    fb_ioctl(ltdc, 0, FB_X_SETLUTENTRY, &clut_info);
#else
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_AL44;//pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);
    fb_ioctl(ltdc, 0, FB_X_SETPREMUL, &pre_multiply);

    layerinfo.alpha = 255;//0x7f;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 200;
    wininfo.y_start = 100;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);

    fb_irq_line_enable(ltdc, 239);
    fb_irq_callback_set(ltdc, LCD_isr);

    fb_ioctl(ltdc, 1, FB_X_SETCHROMAMODE, &onoff);
    fb_ioctl(ltdc, 1, FB_X_SETCHROMA, &color);
#endif
}

int camera_config(void)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    enum af_mode auto_foucs;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;

    camera_sensor = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor, true);

#ifdef COLOR_KEY_CASE
    camera_set_fmt(camera_sensor, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
#else
    camera_set_fmt(camera_sensor, VIDEO_FMT_RGB565, MODE_QVGA_320_240, FPS_15);
#endif

    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_WHITE_BALANCE, &awb);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    saturation = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_SATURATION, &saturation);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    brightness = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_BRIGHTNESS, &brightness);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    contrast = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_CONTRAST, &contrast);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    sharpness = 33;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_SHARPNESS, &sharpness);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    dcmi = device_get_binding(CONFIG_VIN_NAME);

#ifdef COLOR_KEY_CASE
    fmt.pix_fmt = PIX_FMT_YUV422;
#else
    fmt.pix_fmt = PIX_FMT_RGB565;
#endif

    fmt.width = 320;
    fmt.height = 240;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * info.pic_width - 1;
    rect.height = info.pic_height - 1;

    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

#ifdef COLOR_KEY_CASE
    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);
#endif

    reqbufs.count = 3;
    reqbufs.mem_start = 0xc0a00000;//0x10020000;//0x20020000;//0xc0000000;

#ifdef COLOR_KEY_CASE
    reqbufs.length = info.pic_width * info.pic_height * 3 * 3;
#else
    reqbufs.length = info.pic_width * info.pic_height * 2 * 3;
#endif

    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

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
    struct dcmi_buf dis_buf;
    uint32_t addr = 0;
    uint32_t sizeOfPixel = 0;//320 * 240;
    uint8_t *gray_pic;
    uint8_t *gray_pic_0 = 0xc0a6B000;
    uint8_t *gray_pic_1 = 0xc0a7DC00;
    uint32_t *argb8888 = 0xc0a45800;
    int ret = 0;
    bool flag = true;


    lcd_config();
    ret = camera_config();
    if (ret != 0)
    {
        printk("there are something wrong\n");
    }

    sizeOfPixel = info.pic_width * info.pic_height;

    gray_pic = gray_pic_0;

    while (1)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        if (ret == 0)
        {
            if (/*flag*/1)
            {
#if (defined COLOR_KEY_CASE)
                imlib_rgb888_to_background(dis_buf.addr, sizeOfPixel);

                fb_ioctl(ltdc, 0, FB_X_SETADDR, &(dis_buf.addr));
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
#elif (defined CLUT_CASE)
                //imlib_rgb565_to_gray(dis_buf.addr, gray_pic, sizeOfPixel);
                imlib_rgb565_to_argb8888(dis_buf.addr, argb8888, 0x80, sizeOfPixel);
                imlib_argb8888_to_al44((uint32_t)argb8888, gray_pic, sizeOfPixel);
                addr = (uint32_t)gray_pic;

                fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

                gray_pic = (gray_pic == gray_pic_0) ? gray_pic_1 : gray_pic_0;
#else
                //imlib_rgb565_to_gray(dis_buf.addr, gray_pic, sizeOfPixel);
                imlib_rgb565_to_argb8888(dis_buf.addr, argb8888, 0xff, sizeOfPixel);
                imlib_argb8888_to_al44((uint32_t)argb8888, gray_pic, sizeOfPixel);
                addr = (uint32_t)gray_pic;
                //addr = (uint32_t)argb8888;
#if 0
                if (gray_pic == gray_pic_0)
                {
                    HAL_VOUT_SetPixelFormat((VOUT_Device_T *)0x40026000, FMT_RGB565, 1);
                    fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
                }
                else
                {
                    HAL_VOUT_SetPixelFormat((VOUT_Device_T *)0x40026000, FMT_AL44, 1);
                    fb_ioctl(ltdc, 1, FB_X_SETADDR, &(addr));
                }
#else
                fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
                fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));
#endif
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

                gray_pic = (gray_pic == gray_pic_0) ? gray_pic_1 : gray_pic_0;
#endif
                flag = false;
            }
            else
            {
                fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
            }
        }
    }
}
