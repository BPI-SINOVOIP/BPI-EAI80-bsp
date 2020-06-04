#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <board.h>
#include <gm_hal_vin.h>
#include <gpio.h>
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <gm_hal_tim.h>
#include <pinmux.h>
#include <gm_hal_vout.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_imp.h>
#include <math.h>

#include <image.h>
#include "mjpeg.h"
#include "ugelis.h"

#include "qipan_rgb_320x240.h"
#include "lena_rgb_320x240.h"



#define CAM_WIDTH       320
#define CAM_HEIGHT      240
#define CAM_CROP_WIDTH  320
#define CAM_CROP_HEIGHT 240
#define DIS_WIDTH       CAM_CROP_WIDTH
#define DIS_HEIGHT      CAM_CROP_HEIGHT

#define INPUT_WIDTH    CAM_CROP_WIDTH
#define INPUT_HEIGHT   CAM_CROP_HEIGHT


#ifdef CONFIG_GC0308
    #define CAMERA_DEV_NAME GC0308_DEV_NAME
#elif defined(CONFIG_BF3007)
    #define CAMERA_DEV_NAME BF3007_DEV_NAME
#elif defined(CONFIG_OV7725)
    #define CAMERA_DEV_NAME OV7725_DEV_NAME
#elif defined(CONFIG_OV5640)
    #define CAMERA_DEV_NAME OV5640_0_DEV_NAME
#endif

/* Change this if you have an LED connected to a custom port */
struct device *lcd_gpio;
#if defined (CHIP_F6721B)
    /*F6721B*/
    #if defined (F6721B_EVB_QFN80PIN)
        /*QFN80 demo board*/
        #if defined (QFN80_SORTING_BOARD)
            #define PORT     "GPIOC"
            #define LCD_GPIO_PIN   5   /*PC5*/
        #else
            /*QFN80 demo board*/
            #define PORT     "GPIOC"
            #define LCD_GPIO_PIN   3   /*PC3*/
        #endif
    #else
        /*QFN176*/
        #define PORT     "GPIOD"
        #define LCD_GPIO_PIN   12   /*PD12*/
    #endif
#else
    /*GM6721*/
    #define PORT     "GPIOB"
    #define LCD_GPIO_PIN   13   /*PB13*/
#endif

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

static struct device *dcmi = NULL;
static struct device *camera_sensor_0;
static struct device *camera_sensor_1;

struct device *ltdc = NULL;
struct fb_displayinfo info;

struct buf_info
{
    uint8_t last_layer0_index;
    uint8_t last_layer1_index;
    uint8_t cur_layer0_index;
    uint8_t cur_layer1_index;
};
static struct buf_info buf_mgr;

static unsigned char pred_buf_using = 0;
static unsigned char pred_buf_in[INPUT_WIDTH * INPUT_HEIGHT * 3] = {0};
static unsigned char pred_buf_out[INPUT_WIDTH * INPUT_HEIGHT] = {0};

static uint8_t show_demo_pic = 1;

static uint8_t pixfmt = FMT_RGB888;
static int index = 0;

static uint8_t *ConvBuffer = pred_buf_out;

extern int aimodule_init_device(void);

extern jpeg_compress(image_t *src, image_t *dst, int quality);
extern void jpeg_encode(image_t *img, image_t *dst, int quality);


static void LCD_set_pixfmt(uint32_t output_pixfmt)
{
    struct fb_wininfo wininfo;
    uint32_t pre_multiply = 0;

#if defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY)
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

int g_data_src_flag = 0;

static void camera_isr(struct device *dev)
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
        if (g_data_src_flag == 0)
        {
            memcpy(&buf_rgb888_lena[0], (dis_buf.addr), 320 * 240 * 3);
            g_data_src_flag = 1;
        }

        buf_mgr.last_layer0_index = buf_mgr.cur_layer0_index;
        buf_mgr.cur_layer0_index = dis_buf.index;

        if (buf_mgr.last_layer0_index != 0xff)
        {
            release_buf.index = buf_mgr.last_layer0_index;
            dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
        }
        //HAL_VIN_Enable((VIN_Device_T *)0x40025000);

    }
}


void lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    lcd_gpio = device_get_binding(PORT);

    /* Set LED pin as output */
    gpio_pin_configure(lcd_gpio, LCD_GPIO_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(lcd_gpio, LCD_GPIO_PIN, 0);

    gpio_pin_write(lcd_gpio, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

    pixfmt = FMT_RGB888;

    info.pic_width = DIS_WIDTH;
    info.pic_height = DIS_HEIGHT;

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = CAMERA_IMAGE_BUFFER_START;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    fb_irq_line_enable(ltdc, (info.pic_height - 10));
    //    fb_irq_callback_set(ltdc, LCD_isr);
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
    struct video_format v_fmt;

    camera_sensor_0 = device_get_binding(CAMERA_DEV_NAME);

    camera_set_power(camera_sensor_0, true);

    camera_set_fmt(camera_sensor_0, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_30);

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

    v_fmt.width = CAM_WIDTH;
    v_fmt.height = CAM_HEIGHT;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FRAME_SIZE, &v_fmt);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

#if 0
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if (ret != 0)
    {
        return -ENOTSUP;
    }
#endif

    dcmi = device_get_binding(CONFIG_VIN_NAME);

    fmt.pix_fmt = PIX_FMT_YUV422;
    fmt.width = CAM_WIDTH;
    fmt.height = CAM_HEIGHT;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    reqbufs.count = 3;
    reqbufs.mem_start = CAMERA_IMAGE_BUFFER_START;
    reqbufs.length =  CAM_CROP_WIDTH * CAM_CROP_HEIGHT * 3 * reqbufs.count;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);

    dcmi_irq_callback_set(dcmi, camera_isr);

    //ret = dcmi_start_streaming(dcmi, VIN_MODE_SNAPSHOT);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}



int device_init(void)
{
    int i = 0;
#if 0
    /*enable a stable CPU clock*/
    *(volatile uint32_t *)(0x40000090) |= 2 << 0;

    /*power on kdp310 and reset*/
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);
    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;
    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;
    *(volatile uint32_t *)(0x4000018C) |= 3;
    *(volatile uint32_t *)(0x4000009c) |= 1 << 16;
#else
    aimodule_init_device();
    //*(volatile uint32_t *)(0x4003928C) = 0x00666606;
    *(volatile uint32_t *)(0x4003928C) &= ~0xF0;
#endif

    //memset(inputImage, 0x00, BUFFERSIZE);
    //memset(OutputImage, 0x00, BUFFERSIZE);

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(0x20020000 + i) = 0x00000000;
    }

    lcd_config();
    if (camera_config() != 0)
    {
        printk("there are something wrong\n");
    }
    //ai_init_storage();

    return 0;
}

void jpeg_process(void)
{
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;
    RET_CODE ret = RET_OK;
    image_t src;
    image_t dst;

    while (1)
    {
        t_last = k_uptime_get_32();

        //HAL_CNN_Bram_hfwd_rw_en(ENABLE);

        src.w = 320;
        src.h = 240;
        src.bpp = 2;
        src.u.pixels = (uint8_t *)&buf_rgb888_lena[0]; //(uint8_t *)&buf_rgb888[0];//

        dst.w = 320;
        dst.h = 240;
        dst.bpp = 0;
        dst.u.pixels = (uint8_t *)((uint32_t)ConvBuffer | CONFIG_SDRAM_BASE_ADDRESS);

#if 1//hw decoder
        jpeg_hal_init();
        jpeg_hal_compress(&src, &dst, 90);
#else//sw decoder
        jpeg_compress(&src, &dst, 90);
#endif
        t_now = k_uptime_get_32();
        printf("jpeg cost %d ms, data=0x%x, size=0x%x\n", t_now - t_last, dst.u.data, dst.bpp);
        g_data_src_flag = 0;
        //HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    }

}

#define WIDTH    320
#define HEGIHT   240

imagedatabuf_t *g_buf;

#define  JPEG_NFRAMES       10

int demo(void)
{
    image_t img;
    img.w = WIDTH;
    img.h = HEGIHT;
    img.bpp = 2;

    py_mjpeg_obj_t *pmjpeg = NULL;

    g_buf = (imagedatabuf_t *)malloc_ext(0, sizeof(imagedatabuf_t));

    g_buf->buf = (unsigned char *)malloc_ext(0, 1024 * 1024);

    printf("g_buf->buf=0x%x\n", g_buf->buf);

    g_buf->beginbuf = g_buf->buf;
    g_buf->seek_flag = 0;

    pmjpeg = py_mjpeg_open("out.mjpeg", WIDTH, HEGIHT);

    img.u.data = &buf_rgb888_lena[0]; // 320*240

    printf("pmjpeg frames=%d,bytes=%d\n", pmjpeg->frames, pmjpeg->bytes);

    int t = 0;
    while (t < JPEG_NFRAMES)
    {
        //if( t % 2 == 0)
        img.u.data = &buf_rgb888_lena[0];
        //else
        //  img.u.data = &buf_rgb888[0];
        while (g_data_src_flag == 0);
        mjpeg_add_frame(pmjpeg->fp, &(pmjpeg->frames), &(pmjpeg->bytes), &img, 90);
        g_data_src_flag = 0;

        t++;
    }


    printf("pmjpeg frames=%d,bytes=%d\n", pmjpeg->frames, pmjpeg->bytes);

    mjpeg_close(pmjpeg->fp, &(pmjpeg->frames), &(pmjpeg->bytes), 1);

    if (pmjpeg->fp != NULL)
    {
        //fwrite(g_buf->beginbuf, sizeof(unsigned char), pmjpeg->bytes + 224 + 2*8, pmjpeg->fp);
        //fclose( pmjpeg->fp);
    }

    g_buf->mjpegencodelen =  224 + 8 * JPEG_NFRAMES +  pmjpeg->bytes;

    printf("beginbuf=0x%x, mjpegencodelen=0x%x\n", g_buf->beginbuf, g_buf->mjpegencodelen);


    /*224 + 8* nframe+ pmjpeg->bytes*/
    // savebin C:\Users\550269\Desktop\angrad\layer_cross\2.mjpeg 0xC06EFF80     0xE410

    free_ext(0, pmjpeg);

    printf("end\n");

    return 0;
}


int main(void)
{
    RET_CODE ret = RET_OK;

    printf("jpeg demo begin\n");
    device_init();
    //jpeg_process();
    demo();

    while (1);
    printf("jpeg demo exit\n");

    return ret;
}
