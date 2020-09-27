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

#define YUV422_BPP          2

static dev_camera_arg_t *gCam = NULL;
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

static void camera_isr(struct device *cam_dev, uint32_t line_num)
{
    int ret = 0;
    struct dcmi_buf dis_buf;

    ret = camera_get_frame(cam_dev, &dis_buf);
    if (ret == 0)
    {
        gCam->cb((void *)dis_buf.addr, gCam->cb_arg);
        camera_release_frame(cam_dev, &dis_buf);
    }
}

int camera_start(dev_camera_arg_t *cam)
{
    int ret = 0;

    ret = dcmi_start_streaming(cam->dcmi, VIN_MODE_CONTINUOUS);
    return ret;
}

int camera_stop(dev_camera_arg_t *cam)
{
    int ret = 0;

    ret = dcmi_stop_streaming(cam->dcmi);
    return ret;
}


int camera_init(dev_camera_arg_t *cam)
{
    struct device *camera_sensor;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    //struct dcmi_crop_rect rect;
    enum light_mode awb;
    struct video_format v_fmt;
    //enum af_mode auto_foucs;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;
    int src_fmt = VIDEO_FMT_RGB565;

    int ret = 0;

    gCam = cam;
    camera_sensor = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor, true);

    camera_set_fmt(camera_sensor, (enum camera_output_format)src_fmt, MODE_VGA_640_480, FPS_15);

    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_WHITE_BALANCE, &awb);
    if (ret != 0)
    {
        return -1;
    }

    saturation = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_SATURATION, &saturation);
    if (ret != 0)
    {
        return -1;
    }

    brightness = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_BRIGHTNESS, &brightness);
    if (ret != 0)
    {
        return -1;
    }

    contrast = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_CONTRAST, &contrast);
    if (ret != 0)
    {
        return -1;
    }

    sharpness = 33;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_SHARPNESS, &sharpness);
    if (ret != 0)
    {
        return -1;
    }

    cam->dcmi = device_get_binding(CONFIG_VIN_NAME);

    fmt.pix_fmt = (enum dcmi_pixel_format)VIDEO_FMT_YUV422;
    fmt.width   = cam->w;
    fmt.height  = cam->h;
    dcmi_set_fmt_vid_cap(cam->dcmi, &fmt);


#ifdef CONFIG_OV5640
    v_fmt.width  = cam->w;
    v_fmt.height = cam->h;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_FRAME_SIZE, &v_fmt); //5640
    if (ret != 0)
    {
        return -ENOTSUP;
    }
#else
    //If there is VIDIOC_S_CROP, fmt.width and fmt.height would be replaced
    //by rect.width and rect.height

    rect.top    = 0;
    rect.left   = 0;
    rect.width  = YUV422_BPP * cam->w - 1;
    rect.height = cam->h - 1;
    dcmi_ioctl(cam->dcmi, VIDIOC_S_CROP, &rect);
#endif

    if (src_fmt == VIDEO_FMT_YUV422)
    {
        /* only support YUV422 TO RGB888 */
        dcmi_ioctl(cam->dcmi, VIDIOC_CONV_COLORSPACE, &onoff);
    }

    //Reqbufs starting address is [reqbufs.memstart] and its size is [reqbufs.length].
    //The vin driver will malloc 4 buffers at most from reqbufs to store every frame.
    //The size of every frame is rect.width*rect.height*3(rgb888)*reqbufs.count

    reqbufs.count       = cam->frames;
    reqbufs.mem_start   = cam->mem_start;
    reqbufs.length      = cam->mem_size;
    //    printf("CAM_BUF_FRAME_CNT %d\n", CAM_BUF_FRAME_CNT);
    dcmi_ioctl(cam->dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

    buf.index = 0;
    dcmi_ioctl(cam->dcmi, VIDIOC_QUERYBUF, &buf);

    dcmi_irq_callback_set(cam->dcmi, camera_isr);

    return 0;
}


//TODO
void camera_exit()
{

}
