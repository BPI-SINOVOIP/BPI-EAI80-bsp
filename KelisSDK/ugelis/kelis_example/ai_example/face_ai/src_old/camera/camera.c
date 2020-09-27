#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include "camera.h"
#include "ai_common.h"
#include "gm_hal_vin.h"

//#define CAPTURE_JPEG
//#define CAPTURE_YUV422
#define CAPTURE_RGB565
struct device *dcmi;
struct device *camera_sensor;

#define YUV422_BPP  2
struct device *ai_init_camera(dcmi_frame_irq_callback_t cb)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    //enum af_mode auto_foucs;
    int saturation, brightness, contrast, sharpness;
    uint32_t onoff = 1;

    camera_sensor = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor, true);

#ifdef CAPTURE_JPEG
    camera_set_fmt(camera_sensor, VIDEO_FMT_JPEG, MODE_QSXGA_2592_1944, FPS_15);
#else
    camera_set_fmt(camera_sensor, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
#endif

    /* unknown reason that why AF is not useful */
#if 0
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != 0)
    {
        return -ENOTSUP;
    }
#endif

    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_WHITE_BALANCE, &awb);
    if (ret != 0)
    {
        return NULL;
    }

    saturation = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_SATURATION, &saturation);
    if (ret != 0)
    {
        return NULL;
    }

    brightness = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_BRIGHTNESS, &brightness);
    if (ret != 0)
    {
        return NULL;
    }

    contrast = 0;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_CONTRAST, &contrast);
    if (ret != 0)
    {
        return NULL;
    }

    sharpness = 33;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_SHARPNESS, &sharpness);
    if (ret != 0)
    {
        return NULL;
    }

    dcmi = device_get_binding(CONFIG_VIN_NAME);

#ifdef CAPTURE_JPEG
    fmt.pix_fmt = PIX_FMT_JPEG;
    fmt.width = 2592;
    fmt.height = 1944;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    reqbufs.count = 2;
    reqbufs.mem_start = 0xc0000000;
    reqbufs.length = 4 * 1024 * 1024;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#else
    fmt.pix_fmt = VIDEO_FMT_YUV422;
    fmt.width = CAM_BUF_WIDTH;
    fmt.height = CAM_BUF_HEIGHT;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top    = 0;//CAM_CROP_H / 2;
    rect.left   = CAM_CROP_W / 2;
    rect.width  = CAM_CROP_W * YUV422_BPP - 1;
    rect.height = CAM_CROP_H - 1;

    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    //Reqbufs starting address is [reqbufs.memstart] and its size is [reqbufs.length].
    //The vin driver will malloc 4 buffers at most from reqbufs to store every frame.
    //The size of every frame is rect.width*rect.height*3
    reqbufs.count = 4;

    reqbufs.mem_start = MEM_CAM_BASE;//0x10020000(flash);//0x20020000(sram);//0xc0000000(sdram);
    reqbufs.length = CAM_BUF_SIZE; // 2 frames
    //    printf("CAM_BUF_FRAME_CNT %d\n", CAM_BUF_FRAME_CNT);
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#endif

    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);

    dcmi_irq_callback_set(dcmi, cb);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS);
    if (ret != 0)
    {
        return NULL;
    }

    return dcmi;
}

