

#include "camera.h"
#include "ai_common.h"
#include "gm_hal_vin.h"

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

struct device *dcmi;
struct device *camera_sensor;

#define YUV422_BPP  2
int camera_config(dcmi_frame_irq_callback_t cb)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct video_format v_fmt;
    enum light_mode awb;
    enum af_mode auto_foucs;
    int saturation, brightness, contrast;
    uint32_t onoff = 1;

    camera_sensor = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor, true);
    //camera_set_fmt(camera_sensor, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
    camera_set_fmt(camera_sensor, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_30);

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

    v_fmt.width = 224;
    v_fmt.height = 224;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_FRAME_SIZE, &v_fmt);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    dcmi = device_get_binding(CONFIG_VIN_NAME);

    fmt.pix_fmt = VIDEO_FMT_YUV422;
    fmt.width = 224;
    fmt.height = 224;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    //Reqbufs starting address is [reqbufs.memstart] and its size is [reqbufs.length].
    //The vin driver will malloc 4 buffers at most from reqbufs to store every frame.
    //The size of every frame is rect.width*rect.height
    reqbufs.count = 3;
    reqbufs.mem_start = CAMERA_BUF_BASE;//0x10020000(flash);//0x20020000(sram);//0xc0000000(sdram);
    reqbufs.length = 224 * 224 * 3 * 3;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);

    dcmi_irq_callback_set(dcmi, cb);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_SNAPSHOT);

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

