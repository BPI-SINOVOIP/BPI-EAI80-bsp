/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          camera.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Interface to capture frames from camera module
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <camera/camera_sensor.h>
#include <camera/dcmi.h>
#include <gm_hal_vin.h>

#include "camera.h"
#include "inc/config.h"

#define CAPTURE_YUV422

#define YUV422_BPP          2
#define CAM_BUF_FRAME_CNT   2
#define CAM_BUF_SIZE        ((CAM_CROP_W)*(CAM_CROP_H)*(CAM_BUF_PIXEL_SIZE)*(CAM_BUF_FRAME_CNT))
#define MEM_CAM_SIZE        (CAM_BUF_SIZE)

unsigned char VIDEO_SEC_CAM[CAM_BUF_SIZE] VIDEO_SECTION;

dev_camera_cb_t *gCamCb = NULL;
static inline dev_camera_cb_t *camera_get_cb()
{
    return gCamCb;
}

static void camera_set_cb(dev_camera_cb_t *cam_cb)
{
    gCamCb = cam_cb;
}

static int camera_get_frame(void *cam_dev, void *dis_buf)
{
    int ret = 0;
    ret = dcmi_ioctl((struct device *)cam_dev, VIDIOC_DQBUF, (struct dcmi_buf *)dis_buf);
    return ret;
}

static int camera_release_frame(void *cam_dev, void *dis_buf)
{
    dcmi_ioctl((struct device *)cam_dev, VIDIOC_QBUF, (struct dcmi_buf *)dis_buf);

    return 0;
}

static void camera_isr(void *cam_dev, uint32_t line_num)
{
    int ret = 0;
    struct dcmi_buf dis_buf;
    dev_camera_cb_t *cb;

    ret = camera_get_frame(cam_dev, &dis_buf);
    if (ret == 0)
    {
        cb = camera_get_cb();
        cb->func((void *)dis_buf.addr, cb->arg);
        camera_release_frame(cam_dev, &dis_buf);
    }
}

int camera_start(dev_camera_t *dev)
{
    int ret = 0;

    ret = dcmi_start_streaming(dev->dcmi, VIN_MODE_CONTINUOUS);
    return ret;
}

int camera_init(dev_camera_t *dev)
{
    struct device *camera_sensor;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    struct video_format v_fmt;
    //enum af_mode auto_foucs;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;

    int ret = 0;

    camera_set_cb(&dev->cam_cb);

    camera_sensor = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor, true);

    //VIDEO_FMT_RGB565
    camera_set_fmt(camera_sensor, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);//MODE_720P_1280_720,MODE_VGA_640_480,MODE_QVGA_320_240


#ifdef CONFIG_OV5640
    v_fmt.width  = CAM_CROP_W;
    v_fmt.height = CAM_CROP_H;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_FRAME_SIZE, &v_fmt); //5640
    if (ret != 0)
    {
        return -ENOTSUP;
    }
    dev->dcmi = device_get_binding(CONFIG_VIN_NAME);
#endif

    fmt.pix_fmt = VIDEO_FMT_YUV422;
    fmt.width   = CAM_CROP_W;
    fmt.height  = CAM_CROP_H;
    dcmi_set_fmt_vid_cap(dev->dcmi, &fmt);

#ifndef CONFIG_OV5640
    //If there is VIDIOC_S_CROP, fmt.width and fmt.height would be replaced
    //by rect.width and rect.height
    //rect = dev->rect;
    rect.top    = 0;
    rect.left   = 0;
    rect.width  = YUV422_BPP * dev->buf_w - 1;
    rect.height = dev->buf_h - 1;
    dcmi_ioctl(dev->dcmi, VIDIOC_S_CROP, &rect);
#endif
    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dev->dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    //Reqbufs starting address is [reqbufs.memstart] and its size is [reqbufs.length].
    //The vin driver will malloc 4 buffers at most from reqbufs to store every frame.
    //The size of every frame is rect.width*rect.height*3(rgb888)*reqbufs.count

    reqbufs.count       = CAM_BUF_FRAME_CNT;
    reqbufs.mem_start   = VIDEO_SEC_CAM;
    reqbufs.length      = CAM_BUF_SIZE;
    //    printf("CAM_BUF_FRAME_CNT %d\n", CAM_BUF_FRAME_CNT);
    dcmi_ioctl(dev->dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

    buf.index = 0;
    dcmi_ioctl(dev->dcmi, VIDIOC_QUERYBUF, &buf);

    dcmi_irq_callback_set(dev->dcmi, camera_isr);

    return 0;
}


//TODO
void camera_exit()
{

}
