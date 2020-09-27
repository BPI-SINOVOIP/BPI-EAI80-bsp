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
#include <fs.h>
#include <ff.h>
#include <dlmalloc.h>

#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_sqi.h>
#include <gm_hal_gpio.h>
#include "ui.h"

#define BUTTON_PORT  "GPIOB"
#define BUTTON_PIN   13   /*PB13*/

#define PIC_WIDTH  320
#define PIC_HEIGHT 240

#define POINT_COLOR  0x0000F800
#define BACK_COLOR   0xFFFFFFFF

#define UI_PIC_WIDTH  320
#define UI_PIC_HEIGHT 240

#define CAMERA_BUF_BASE ((CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000)

static uint32_t ui_buf[LCD_UI_W * LCD_UI_H];
uint32_t ui_buf_addr = &ui_buf;

struct device *gpio_bl;
struct device *ltdc;
struct device *dcmi;
struct device *camera_sensor;
struct fb_displayinfo info;
static uint8_t lcd_inited = 0;

//#define CORNER_DETECTOR     1
//#define MEDIAN_FILTER          1
//#define GAUSSIAN_FILTER      1
//#define CLAHE_HISTEQ            1
#define LBP_OPERATOR            1
//#define K_MEANS                     1

#ifdef GAUSSIAN_FILTER
const int8_t kernel_gauss_3[3 * 3] =
{
    1, 2, 1,
    2, 4, 2,
    1, 2, 1,
};
#endif

#ifdef LBP_OPERATOR
#define FATFS_MNTP  "/SFLASH0:"
#define LBP_DESC_FILE   FATFS_MNTP"/lbptfile.txt"

#define HIST_SIZE   (59)    //58 uniform hist + 1
#define NUM_REGIONS (7)     //7x7 regions
#define DESC_SIZE   (NUM_REGIONS*NUM_REGIONS*HIST_SIZE)

#define MAX_BASE_CNT  5

struct LBP_desc_mgr
{
    int label;
    uint8_t base_lbp_desc[DESC_SIZE];
};

static struct LBP_desc_mgr lbp_desc_mgr[MAX_BASE_CNT];

static bool save_desc = FALSE;

static struct fs_file_t filep;

/* FatFs work area */
static FATFS fat_fs;

/* mounting info */
static struct fs_mount_t fatfs_mnt =
{
    .type = FS_FATFS,
    .mnt_point = FATFS_MNTP,
    .fs_data = &fat_fs,
};

int check_file_dir_exists(const char *path)
{
    int res;
    struct fs_dirent entry;

    /* Verify fs_stat() */
    res = fs_stat(path, &entry);

    return !res;
}
#endif

#ifdef K_MEANS
float cluster_dist(int cx, int cy, void *kp_in)
{
    float sum = 0.0f;
    kp_t *kp = kp_in;
    sum += (cx - kp->x) * (cx - kp->x);
    sum += (cy - kp->y) * (cy - kp->y);
    return fast_sqrtf(sum);
}
#endif

void lcd_config(uint32_t fb_addr)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpio_bl = device_get_binding(LCD_BL_PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpio_bl, LCD_BL_PIN, GPIO_DIR_OUT);
    gpio_pin_write(gpio_bl, LCD_BL_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_L8;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = fb_addr;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    /*First Layer*/
    info.pic_width =  PIC_WIDTH;
    info.pic_height = PIC_HEIGHT;
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);
}

void set_ui_layer(int x, int y)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_ARGB8888;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = (uint32_t) ui_buf_addr;

    fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = x;
    wininfo.y_start = y;
    wininfo.width   = LCD_UI_W;
    wininfo.height  = LCD_UI_H;

    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
}

/*Called every frame data captured by Camera*/
void camera_isr(struct device *dev, uint32_t line_num)
{
    struct dcmi_buf dis_buf;
    int ret = 1;

    /*Camera can display Real-Time, we initialize LCD to fix  snowflake sreen issue when display the first image frame*/
    if (!lcd_inited)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        if (ret == 0)
        {
            lcd_config(dis_buf.addr);
            set_ui_layer(320, 0);
            lcd_inited = 1;

            /*Release the buffer for next camera capturing*/
            dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

            dcmi_irq_callback_set(dcmi,  NULL);
        }
    }
}

int camera_config(dcmi_frame_irq_callback_t cb)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    enum af_mode auto_foucs;
    int saturation, brightness, contrast, exposure;
    uint32_t onoff = 1;

    camera_sensor = device_get_binding(OV7725_DEV_NAME);

    camera_set_power(camera_sensor, true);
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

    brightness = 2;
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

    exposure = -1;
    ret = camera_ioctl(camera_sensor, CAMERA_CID_EXPOSURE, &exposure);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    dcmi = device_get_binding(CONFIG_VIN_NAME);

    fmt.pix_fmt = VIDEO_FMT_YUV422;
    fmt.width = PIC_WIDTH;
    fmt.height = PIC_HEIGHT;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    //Reqbufs starting address is [reqbufs.memstart] and its size is [reqbufs.length].
    //The vin driver will malloc 4 buffers at most from reqbufs to store every frame.
    //The size of every frame is rect.width*rect.height
    reqbufs.count = 3;
    reqbufs.mem_start = CAMERA_BUF_BASE;
    reqbufs.length = PIC_WIDTH * PIC_HEIGHT * 3 * 3;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);

    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);

    dcmi_irq_callback_set(dcmi, cb);

    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS);
    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

#ifdef LBP_OPERATOR
void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    printf("Button pressed\n");
    save_desc = TRUE;
}

void gpio_interrupt_setting(void)
{
    uint32_t val ;
    struct device *dev;
    static struct gpio_callback gpio_cb;

    dev = device_get_binding(BUTTON_PORT);

    /* Set key pin as interrupt active flalling mode */
    gpio_pin_configure(dev, BUTTON_PIN,  GPIO_PUD_PULL_DOWN | GPIO_MODE_IT_RISING);
    gpio_init_callback(&gpio_cb, button_pressed, BIT(BUTTON_PIN));

    gpio_add_callback(dev, &gpio_cb);
    gpio_pin_enable_callback(dev, BUTTON_PIN);
}
#endif

void main(void)
{
    struct dcmi_buf dis_buf;
    image_t src_img;
    image_t dst_img;
    rectangle_t roi;
    uint8_t *lbp_desc = NULL;
    uint8_t *base_lbp_desc = NULL;
    array_t *kpts = NULL;
    array_t *clusters = NULL;
    int kpts_index = 0;
    uint32_t addr = 0;
    uint8_t *gray_pic = NULL;
    uint8_t *gray_pic_0 = NULL;
    uint8_t *gray_pic_1 = NULL;
    uint8_t *fb_buf = NULL;
    uint32_t sizeOfPixel = PIC_WIDTH * PIC_HEIGHT * 2 + 64;
    int ret = 1;
    uint32_t bytes = 0;
    static int label = 0;
    int distance = 0;
    int i = 0;
    char dist_str[16];

    printf("corner detector demo \n");

#ifdef LBP_OPERATOR
    gpio_interrupt_setting();

    ret = fs_mount(&fatfs_mnt);
    if (ret < 0)
    {
        printf("Error mounting fs [%d]\n", ret);
        return;
    }

    if (check_file_dir_exists(LBP_DESC_FILE))
    {
        printf("Opening existing file %s\n", LBP_DESC_FILE);
    }
    else
    {
        printf("Creating new file %s\n", LBP_DESC_FILE);
    }

    /* Verify fs_open() */
    ret = fs_open(&filep, LBP_DESC_FILE);
    if (ret)
    {
        printf("Failed opening file [%d]\n", ret);
        return;
    }

    base_lbp_desc = dlmem_malloc(DESC_SIZE);
    if (base_lbp_desc != NULL)
    {
        memset(base_lbp_desc, 0, DESC_SIZE);
    }
    else
    {
        printf("malloc lbp desc buffer wrong\n");
        return;
    }

    ret = fs_seek(&filep, 0, FS_SEEK_SET);
    if (ret)
    {
        printf("Failed seek file [%d]\n", ret);
        return;
    }

    for (i = 0; i < MAX_BASE_CNT; i++)
    {
        bytes = fs_read(&filep, &label, sizeof(int));
        if (bytes != sizeof(int))
        {
            printf("read desc fail \n");
        }

        bytes = imlib_lbp_desc_load(&filep, base_lbp_desc);
        if (bytes != DESC_SIZE)
        {
            printf("read desc fail \n");
        }

        lbp_desc_mgr[i].label = label;
        memcpy(lbp_desc_mgr[i].base_lbp_desc, base_lbp_desc, DESC_SIZE);
    }

    ret = fs_seek(&filep, 0, FS_SEEK_SET);
    if (ret)
    {
        printf("Failed seek file [%d]\n", ret);
        return;
    }

    label = 0;
#endif

    ret = camera_config(camera_isr);
    if (ret != 0)
    {
        printf("camera config wrong\n");
        return;
    }

    /* malloc buffer for image process */
    fb_buf = dlmem_malloc(sizeOfPixel);
    if (fb_buf != NULL)
    {
        memset(fb_buf, 0, sizeOfPixel);
    }
    else
    {
        printf("malloc frame buffer wrong\n");
        return;
    }

    gray_pic_0 = ((uint32_t)fb_buf + 15) & 0xfffffff0;
    gray_pic_1 = ((uint32_t)gray_pic_0 + PIC_WIDTH * PIC_HEIGHT + 15) & 0xfffffff0;

    gray_pic = gray_pic_0;

    while (1)
    {
        /* get video data from display queue */
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        if (ret == 0)
        {
            sprintf(dist_str, "ZhuHai Edgeless");
            lcd_draw_text(0, 50, 160, 24, 16, dist_str, RED);
            /* video format convert, RGB888 to GRAY */
            fb_convert_color(ltdc, dis_buf.addr, gray_pic, FMT_RGB888, FMT_L8, PIC_WIDTH, PIC_HEIGHT);
            addr = (uint32_t)gray_pic;

            /* update display buffer address */
            //fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));

            /*Release the buffer for next camera capturing*/
            dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

            src_img.bpp = 1;
            src_img.w = PIC_WIDTH;//info.pic_width;
            src_img.h = PIC_HEIGHT;//info.pic_height;
            src_img.pixels = addr;

#ifdef CORNER_DETECTOR
            /* corner detector */
            kpts = orb_find_keypoints(&src_img, 40, 100);

#ifdef  K_MEANS
            if (kpts != NULL)
            {
                clusters = cluster_kmeans(kpts, 9, cluster_dist);
                if (clusters != NULL)
                {
                    array_free(clusters);
                    clusters = NULL;
                }
            }
#endif

            if (kpts != NULL)
            {
                for (int k = kpts_index; k < array_length(kpts); k++, kpts_index++)
                {
                    // Set keypoint octave/scale
                    kp_t *kpt = array_at(kpts, k);

                    imlib_draw_circle(&src_img, kpt->x, kpt->y, 4, 0x0);
                }

                array_free(kpts);
                kpts_index = 0;
                kpts = NULL;
            }
#endif

#ifdef GAUSSIAN_FILTER
            imlib_gaussian_filter(&src_img, kernel_gauss_3, 1.0f / 16.0f, 0.0f);
#endif

#ifdef MEDIAN_FILTER
            imlib_median_filter(&src_img, 1, 0.5, true, 10, true, NULL);
#endif

#ifdef CLAHE_HISTEQ
            imlib_clahe_histeq(&src_img, 10, NULL);
#endif

#ifdef LBP_OPERATOR
            roi.x = 10;
            roi.y = 10;
            roi.h = 35;
            roi.w = 35;
            lbp_desc = imlib_lbp_desc(&src_img, &roi);

            if (lbp_desc != NULL)
            {
                if (save_desc == TRUE)
                {
                    bytes = fs_write(&filep, &label, sizeof(int));
                    if (bytes != sizeof(int))
                    {
                        printf("write desc fail \n");
                    }

                    bytes = imlib_lbp_desc_save(&filep, lbp_desc);
                    if (bytes != DESC_SIZE)
                    {
                        printf("write desc fail \n");
                    }

                    ret = fs_sync(&filep);
                    if (ret)
                    {
                        printf("Failed sync file [%d]\n", ret);
                        return;
                    }

                    lbp_desc_mgr[label].label = label;
                    memcpy(lbp_desc_mgr[label].base_lbp_desc, lbp_desc, DESC_SIZE);

                    label++;
                    if (label >= MAX_BASE_CNT)
                    {
                        ret = fs_seek(&filep, 0, FS_SEEK_SET);
                        if (ret)
                        {
                            printf("Failed seek file [%d]\n", ret);
                            return;
                        }

                        label = 0;
                    }

                    save_desc = FALSE;
                }

                distance = imlib_lbp_desc_distance(lbp_desc, lbp_desc_mgr[label].base_lbp_desc);

                xfree(lbp_desc);
                lbp_desc = NULL;
            }
#endif

            /* update display buffer address */
            fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));

            gray_pic = (gray_pic == gray_pic_0) ? gray_pic_1 : gray_pic_0;

            k_sleep(10);
        }
    }
}
