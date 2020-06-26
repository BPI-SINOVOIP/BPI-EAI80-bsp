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
#include <mem_cfg.h>

#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>

struct device *ltdc = NULL;
struct device *dcmi = NULL;
struct device *camera_sensor_0;

struct fb_displayinfo info;

uint8_t pixfmt = FMT_RGB888;

struct buf_info
{
    uint8_t last_layer0_index;
    uint8_t cur_layer0_index;
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

void lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
    struct device *bl_gpio;
    bl_gpio = device_get_binding(LCD_BL_PORT);
    /* Set LED pin as output */
    gpio_pin_configure(bl_gpio, LCD_BL_PIN, GPIO_DIR_OUT);
    gpio_pin_write(bl_gpio, LCD_BL_PIN, 1);
    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

    pixfmt = FMT_RGB888;
    info.pic_width = 320;
    info.pic_height = 240;

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = CAMERA_IMAGE_BUFFER_START;//0xc0600000,changed by reserved sdram space

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

int camera_config(void)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    struct video_format v_fmt;
    enum light_mode awb;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;

    camera_sensor_0 = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor_0, true);

    camera_set_fmt(camera_sensor_0, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);

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

    /*only support ov5640 for scaling image to defined size(not crop)
    and support all camera to set frame size, maybe not scaling
    /*
    v_fmt.width = 240;
    v_fmt.height = 160;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FRAME_SIZE, &v_fmt);
    if (ret != 0)
    {
        return -ENOTSUP;
    }
    */
    dcmi = device_get_binding(CONFIG_VIN_NAME);

    fmt.pix_fmt = PIX_FMT_YUV422;
    fmt.width = 320;
    fmt.height = 240;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * info.pic_width - 1;
    rect.height = info.pic_height - 1;
    //dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    reqbufs.count = 4;
    reqbufs.mem_start = CAMERA_IMAGE_BUFFER_START;//0xc0600000,changed by reserved sdram space
    reqbufs.length =  info.pic_width * info.pic_height * 3 * 4;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);

    //dcmi_irq_callback_set(dcmi, camera_isr);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_SNAPSHOT);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

int main(void)
{
    int ret = 0;
    enum af_mode auto_foucs;
    int j = 0;
    lcd_config();
    buf_mgr.last_layer0_index = 0xff;
    buf_mgr.cur_layer0_index = 0xff;

    ret = camera_config();
    if (ret != 0)
    {
        printk("there are something wrong\n");
    }

    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != 0)
    {
        printk("auto focus fail \n");
    }
    while (1)
    {
        ;
    }
    return 0;
}
