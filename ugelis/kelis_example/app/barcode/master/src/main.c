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
#include <gm_hal_imp.h>
#include <gm_irq.h>
#include <gm_hal_gpio.h>

#include <mem_cfg.h>


#include "zbar.h"
#include "image.h"
#include "qrcode.h"
#include "test_pic.h"

#include "ui/ui.h"

#define ZBAR_DEMO_DEBUG printf
//#define ZBAR_DEMO_DEBUG(...) do{}while(0);

//#define UART_PROT_TEST


#ifdef ENABLE_BARCODE_ZOOM
#define CAM_WIDTH       640
#define CAM_HEIGHT      480
#define CAM_CROP_WIDTH  640
#define CAM_CROP_HEIGHT 480
#define DIS_WIDTH       320
#define DIS_HEIGHT      240
#define NORMAL_WIDTH    DIS_WIDTH
#define NORMAL_HEIGHT   DIS_HEIGHT
#else
#define CAM_WIDTH       320
#define CAM_HEIGHT      240
#define CAM_CROP_WIDTH  320
#define CAM_CROP_HEIGHT 240
#define DIS_WIDTH       CAM_CROP_WIDTH
#define DIS_HEIGHT      CAM_CROP_HEIGHT
#define NORMAL_WIDTH    DIS_WIDTH
#define NORMAL_HEIGHT   DIS_HEIGHT
#endif

#ifdef CONFIG_GC0308
#define CAMERA_DEV_NAME GC0308_DEV_NAME
#define CAPTURE_YUV422
#elif defined(CONFIG_BF3007)
#define CAMERA_DEV_NAME BF3007_DEV_NAME
#define CAPTURE_YUV422
#elif defined(CONFIG_OV7725)
#define CAMERA_DEV_NAME OV7725_DEV_NAME
#define CAPTURE_YUV422
#elif defined(CONFIG_OV5640)
#define CAMERA_DEV_NAME OV5640_0_DEV_NAME
#define CAPTURE_RGB565
#endif

#define IMP_DEVICE (IMP_Device_T *)(0x40024000)



#ifdef CONFIG_DISPLAY
#define LCD_DISPLAY

/* Change this if you have an LED connected to a custom port */
struct device *lcd_gpio;
#if defined (CHIP_F6721B)
    /*F6721B*/
    #if defined (F6721B_EVB_QFN80PIN)
        /*QFN80 demo board*/
        #if defined (QFN80_SORTING_BOARD)
            #define LCD_GPIO_PORT     "GPIOC"
            #define LCD_GPIO_PIN   5   /*PC5*/
        #else
            /*QFN80 demo board*/
            #define LCD_GPIO_PORT     "GPIOC"
            #define LCD_GPIO_PIN   3   /*PC3*/
        #endif
    #else
        /*QFN176*/
        #define LCD_GPIO_PORT     "GPIOD"
        #define LCD_GPIO_PIN   12   /*PD12*/
    #endif
#else
    /*GM6721*/
    #define LCD_GPIO_PORT     "GPIOB"
    #define LCD_GPIO_PIN   13   /*PB13*/
#endif
#endif




#ifdef F6721B_RINGSCANER
#define LED_GPIO_PORT           "GPIOH"
#define LED_FILL_GPIO_PIN       4
#define LED_POSITION_GPIO_PIN   5
static struct device *dev_led;

typedef enum
{
    SCAN_START,
    SCAN_STOP,
}ringscaner_status_t;
static ringscaner_status_t ringscaner_status = SCAN_STOP; 
#endif

static struct device *dcmi = NULL;
static struct device *camera_sensor_0;
static struct device *camera_sensor_1;




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

static uint8_t cam_crop_data[CAM_CROP_WIDTH * CAM_CROP_HEIGHT];
#ifdef ENABLE_BARCODE_ZOOM
static uint8_t normal_data[NORMAL_WIDTH * NORMAL_HEIGHT];
extern barcode_detected_info_t barcode_detected_info;
#endif


#define TIME_DEBUG
#ifdef TIME_DEBUG
    static volatile int32_t time_old_frame_end = 0;
    static volatile int32_t time_new_frame_got = 0;
    static volatile int32_t time_gray = 0;
    static volatile int32_t time_zbar_prepare = 0;
    extern volatile int32_t time_zbar_scan_prepare;
    extern volatile int32_t time_zbar_scan_Y;
    extern volatile int32_t time_zbar_scan_X;
    extern volatile int32_t time_zbar_scan_XY;
    extern volatile int32_t time_zbar_scan_decode;
    extern volatile int32_t time_zbar_scan_remainder;
    static volatile int32_t time_zbar_scan = 0;
    static volatile int32_t time_zbar = 0;
    static volatile int32_t time_zbar_last = 0;

    extern volatile int32_t time_zbar_scan_Y_prepare;
    extern volatile int32_t time_zbar_scan_Y_scan_y_0;
    extern volatile int32_t time_zbar_scan_Y_scan_y_1;
#endif

typedef enum
{
    DATA_FROM_PIC,
    DATA_FROM_CAM,
} data_source_t;
data_source_t data_source = DATA_FROM_CAM;//DATA_FROM_PIC;//

static int imp_format(uint32_t src, uint32_t dst, uint32_t input_pixfmt, uint32_t output_pixfmt, \
                          uint32_t input_offset, uint32_t output_offset, uint32_t pic_width, uint32_t pic_height)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;
    float w_scale = 0.0;
    float h_scale = 0.0;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    pixel_num = pic_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

#if 0// defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY)
    w_scale = (float)pixel_num / (DIS_WIDTH - output_offset);
    h_scale = (float)pic_height / DIS_HEIGHT;

    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
    scaleCfg.scaleWidth = (DIS_WIDTH - output_offset);
    scaleCfg.scaleHeight = DIS_HEIGHT;
    scaleCfg.svi = fast_floorf(h_scale);
    scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi) * 256);
    scaleCfg.shi = fast_floorf(w_scale);
    scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi) * 256);
#else
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
#endif

    HAL_IMP_Start(pIMP, (uint32_t)src, (uint32_t)dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    return 0;
}

static int fast_floorf(float x)
{
         int n;
         float r;      
         n = (int) x;
         r = (float) n;
         r = r - (r > x);
         return r;
}
static int fast_roundf(float x)
{
         int tmp;
         float r;      
                               float diff = 0;
                                      tmp = (int) x;
                                      diff = x - tmp;
                                      if(diff >= 0.5)
                                      {
                                                return (tmp+1);
                                      }
                                      return tmp;
}

int imp_scale(uint32_t src, uint32_t dst, uint32_t input_pixfmt, uint32_t output_pixfmt, \
         uint32_t input_offset, uint32_t output_offset, uint32_t pic_width, uint32_t pic_height, \
         uint32_t out_width, uint32_t out_height)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;
    float w_scale = 0.0;
    float h_scale = 0.0;
    
    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
         layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    pixel_num = pic_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    w_scale = (float)pixel_num/(out_width- output_offset);
    h_scale = (float)pic_height/out_height;
         
    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
    scaleCfg.scaleWidth = (out_width - output_offset);
    scaleCfg.scaleHeight = out_height;
    scaleCfg.svi = fast_floorf(h_scale);
    scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi)*256);
    scaleCfg.shi = fast_floorf(w_scale);
    scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi)*256);
    HAL_IMP_Start(pIMP, (uint32_t)src, (uint32_t)dst, &scaleCfg);

    while(__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break; 
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);
         
    return 0;
}


#ifdef LCD_DISPLAY

struct device *ltdc = NULL;
struct fb_displayinfo info;
static char dist_str[1024];
static uint32_t ui_buf[LCD_UI_W * LCD_UI_H];
uint32_t ui_buf_addr = &ui_buf;

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

static void LCD_isr(struct device *dev)
{
    //do nothing
    return;
}

void lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    lcd_gpio = device_get_binding(LCD_GPIO_PORT);

    /* Set LED pin as output */
    gpio_pin_configure(lcd_gpio, LCD_GPIO_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(lcd_gpio, LCD_GPIO_PIN, 0);

    gpio_pin_write(lcd_gpio, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

#if (defined CAPTURE_YUV422)
    pixfmt = FMT_RGB888;
#else
    pixfmt = FMT_RGB565;
#endif
    
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

    fb_irq_line_enable(ltdc, (info.pic_height-10));
    fb_irq_callback_set(ltdc, LCD_isr);
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

#endif

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
    uint32_t one_frame_size = 0;

    camera_sensor_0 = device_get_binding(CAMERA_DEV_NAME);

    camera_set_power(camera_sensor_0, true);

#ifdef ENABLE_BARCODE_ZOOM
    #ifdef CAPTURE_YUV422
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_YUV422, MODE_VGA_640_480, FPS_15);
    #else
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_RGB565, MODE_VGA_640_480, FPS_30);
    #endif
#else
    #ifdef CAPTURE_YUV422
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_30);
    #else
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_RGB565, MODE_QVGA_320_240, FPS_30);
    #endif
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

    brightness = 3;
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

#if 0
    v_fmt.width = CAM_WIDTH;
    v_fmt.height = CAM_HEIGHT;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FRAME_SIZE, &v_fmt);
    if (ret != 0)
    {
        return -ENOTSUP;
    }
#endif

#if 0
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
#endif

    dcmi = device_get_binding(CONFIG_VIN_NAME);

#ifdef CAPTURE_YUV422
    fmt.pix_fmt = PIX_FMT_YUV422;
    fmt.width = CAM_WIDTH;
    fmt.height = CAM_HEIGHT;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * CAM_CROP_WIDTH - 1;
    rect.height = CAM_CROP_HEIGHT - 1;
    //dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    one_frame_size = CAM_CROP_WIDTH * CAM_CROP_HEIGHT * 3;
    reqbufs.count = CAMERA_IMAGE_BUFFER_LENGTH / one_frame_size;
    reqbufs.mem_start = CAMERA_IMAGE_BUFFER_START;
    reqbufs.length =  one_frame_size * reqbufs.count;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#else
    fmt.pix_fmt = PIX_FMT_RGB565;
    fmt.width = CAM_WIDTH;
    fmt.height = CAM_HEIGHT;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    /* crop from center area of the camera buffer */
    rect.top = 0;//(fmt.height-info.pic_height)/2;
    rect.left = 0;//(fmt.width-info.pic_width)/2;
    rect.width = 2 * CAM_CROP_WIDTH - 1;
    rect.height = CAM_CROP_HEIGHT - 1;
    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

    one_frame_size = CAM_CROP_WIDTH * CAM_CROP_HEIGHT * 2;
    reqbufs.count = CAMERA_IMAGE_BUFFER_LENGTH / one_frame_size;
    reqbufs.mem_start = CAMERA_IMAGE_BUFFER_START;
    reqbufs.length =  one_frame_size * reqbufs.count;
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

#ifdef F6721B_RINGSCANER
static int led_init()
{
    dev_led = device_get_binding(LED_GPIO_PORT);

    gpio_pin_configure(dev_led, LED_FILL_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(dev_led, LED_POSITION_GPIO_PIN, GPIO_MODE_OUTPUT);

    /* default off */
    gpio_pin_write(dev_led, LED_FILL_GPIO_PIN, GPIO_PIN_SET);
    gpio_pin_write(dev_led, LED_POSITION_GPIO_PIN, GPIO_PIN_SET);

    return 0;
}

static int led_on(uint8_t on)
{
    static char led_init = 0;
    
    if(!led_init)
    {
        led_init();
        led_init = 1;
    }
    
    if(on)
    {
        gpio_pin_write(dev_led, LED_FILL_GPIO_PIN, GPIO_PIN_RESET);
        gpio_pin_write(dev_led, LED_POSITION_GPIO_PIN, GPIO_PIN_RESET);
    }
    else
    {
        gpio_pin_write(dev_led, LED_FILL_GPIO_PIN, GPIO_PIN_SET);
        gpio_pin_write(dev_led, LED_POSITION_GPIO_PIN, GPIO_PIN_SET);
    }

    return 0;
}

static void send_heartbeat()
{
    static int32_t time_last = 0;
    static int32_t time_cur = 0;

    time_cur = k_uptime_get_32();
    if(time_cur-time_last >= 1000)
    {
        protocol_send_heartbeat(0);
        time_last = time_cur;
    }
    return;   
}

static void reset_bt_board()
{
    return;
}
#endif

int zbar_proc(void *raw, int width, int height)
{
    
    zbar_mem_clear();

    int zbar_found = 0;
    
    zbar_image_scanner_t *scanner = NULL;
    zbar_image_t *image = NULL;
    /* create a reader */
    scanner = zbar_image_scanner_create();
    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);
    /* wrap image data */
    image = zbar_image_create();
    zbar_image_set_format(image, *(int *)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);

#ifdef CONFIG_QR_DUAL_EN
    zbar_image_scanner_t *scanner_slave = NULL;
    zbar_image_t *image_slave = NULL;
    scanner_slave = zbar_image_scanner_create();
    zbar_image_scanner_set_config(scanner_slave, 0, ZBAR_CFG_ENABLE, 1);
    image_slave = zbar_image_create();
    zbar_image_set_format(image_slave, *(int *)"Y800");
    zbar_image_set_size(image_slave, width, height);
    zbar_image_set_data(image_slave, raw, width * height, zbar_image_free_data);
#endif
    
    //time_zbar_prepare = k_uptime_get_32();    

    /* scan the image for barcodes */
#ifdef CONFIG_QR_DUAL_EN
    int n = zbar_scan_image(scanner, scanner_slave, image, image_slave);
#else
    int n = zbar_scan_image(scanner, image);
#endif
    //ZBAR_DEMO_DEBUG("n = %d\r\n", n);
    //time_zbar_scan = k_uptime_get_32();
    //ZBAR_DEMO_DEBUG("scan:%d\n\n", time_zbar_scan-time_zbar_prepare);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    time_zbar = k_uptime_get_32();
    char *data = NULL;
    for (; symbol; symbol = zbar_symbol_next(symbol))
    {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        if((typ == ZBAR_CODE128) || (typ == ZBAR_QRCODE))
        {
            data = zbar_symbol_get_data(symbol);
            if(strlen(data) != zbar_symbol_get_data_length(symbol))
            {
                data = NULL;
            }
            else
            {
                #ifdef F6721B_RINGSCANER
                if(protocol_send_barcode(data, strlen(data)) != RET_OK)
                {
                    reset_bt_board();
                    ZBAR_DEMO_DEBUG("[%s(%d)] error\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    led_on(0);
                    ringscaner_status = SCAN_STOP;
                }
                #endif
                ZBAR_DEMO_DEBUG("\r\ndecoded %s symbol:\r\n%s\r\n",
                       zbar_get_symbol_name(typ), data);
                zbar_found = 1;
            }
        }

#ifdef LCD_DISPLAY
        if (data)
        {
            //lcd_drawlogo(0, 0, 0xffff0000);
            //lcd_clear_area();
            sprintf(dist_str, data);
            lcd_draw_text(0, 0, 480, 24, 24, dist_str, RED);
        }
#endif

        ZBAR_DEMO_DEBUG("cost=%d\n", time_zbar-time_new_frame_got);
        //ZBAR_DEMO_DEBUG("cost2=%d\n", time_zbar-time_zbar_last);
        //time_zbar_last = k_uptime_get_32();

        //ZBAR_DEMO_DEBUG("pre:%d\n", time_zbar_prepare-time_new_frame_got);
        //ZBAR_DEMO_DEBUG("scan:%d\n", time_zbar_scan-time_zbar_prepare);
        //ZBAR_DEMO_DEBUG("    XY:%d(Y:%d, X:%d), decode:%d, remainder:%d\n",
        //    time_zbar_scan_XY-time_zbar_scan_prepare, time_zbar_scan_Y-time_zbar_scan_prepare, time_zbar_scan_X-time_zbar_scan_Y, time_zbar_scan_decode-time_zbar_scan_XY, time_zbar_scan_remainder-time_zbar_scan_decode);
        //ZBAR_DEMO_DEBUG("    XY:%d, decode:%d\n",
        //    time_zbar_scan_XY-time_zbar_scan_prepare, time_zbar_scan_decode-time_zbar_scan_XY);
        //ZBAR_DEMO_DEBUG("extract:%d\n", time_zbar-time_zbar_scan);

        
        ZBAR_DEMO_DEBUG("    Y:%d, X:%d, decode:%d\n",
            time_zbar_scan_Y-time_zbar_scan_Y_prepare, time_zbar_scan_X-time_zbar_scan_Y, time_zbar_scan_decode-time_zbar_scan_X);

        break;//Evan: only need first one of the barcode currently
    }

END:
   /* clean up */
   zbar_image_destroy(image);
   zbar_image_scanner_destroy(scanner);
#ifdef CONFIG_QR_DUAL_EN
    zbar_image_destroy(image_slave);
   zbar_image_scanner_destroy(scanner_slave);
#endif

   // ZBAR_DEMO_DEBUG("\n<<<<<<<<<<<<<<<<<<<<<zbar_proc() out\n");
    return zbar_found;
}

unsigned char *test_data = code128_data_2;//code128_data_1;//qr_data;//

static void video_proc()
{
    struct dcmi_buf dis_buf;
    struct dcmi_buf release_buf;
    int ret = -1;
    uint32_t dis_addr = 0;
    uint8_t found_by_large_pic = 0;
    uint8_t *big_resolution_data = NULL;
    uint8_t *normal_resolution_data = NULL;
    int zbar_found = 0;

    #ifdef TIME_DEBUG
    time_new_frame_got = k_uptime_get_32();
    #endif
    
    if(data_source == DATA_FROM_CAM)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
        if(ret == 0)
        {
            #ifdef CAPTURE_YUV422
            imp_format(dis_buf.addr, (uint32_t)cam_crop_data, IMP_INPUT_RGB888, FMT_L8, 0, 0, CAM_CROP_WIDTH, CAM_CROP_HEIGHT);
            #else
            imp_format(dis_buf.addr, (uint32_t)cam_crop_data, IMP_INPUT_RGB565, FMT_L8, 0, 0, CAM_CROP_WIDTH, CAM_CROP_HEIGHT);
            #endif
            big_resolution_data = cam_crop_data;

            buf_mgr.last_layer0_index = buf_mgr.cur_layer0_index;
            buf_mgr.cur_layer0_index = dis_buf.index;
            if (buf_mgr.last_layer0_index != 0xff)
            {
                release_buf.index = buf_mgr.last_layer0_index;
                dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
            }
        }
      
        #ifdef ENABLE_BARCODE_ZOOM
        imp_scale((uint32_t)big_resolution_data, (uint32_t)normal_data, IMP_INPUT_L8, FMT_L8, 0, 0, CAM_CROP_WIDTH, CAM_CROP_HEIGHT, NORMAL_WIDTH, NORMAL_HEIGHT);
        normal_resolution_data = normal_data;
        #else
        normal_resolution_data = cam_crop_data;
        #endif
    }
    else
    {
        big_resolution_data = test_data;
        normal_resolution_data = test_data;
        ret = 0;
    }

    if(ret != 0)
    {
        return;
    }

#ifdef LCD_DISPLAY
    LCD_set_pixfmt(FMT_L8);
    dis_addr = (uint32_t)normal_resolution_data;
    fb_ioctl(ltdc, 0, FB_X_SETADDR, &dis_addr);
#endif

#ifdef ENABLE_BARCODE_ZOOM
    barcode_detected_info.barcode_detected = 0;
#endif
    zbar_found = zbar_proc(normal_resolution_data, NORMAL_WIDTH, NORMAL_HEIGHT);
    if(zbar_found)
    {
        ZBAR_DEMO_DEBUG("-->in normal resolution mode.\n");
    }
#ifdef ENABLE_BARCODE_ZOOM
    if((zbar_found == 0) && (barcode_detected_info.barcode_detected == 1))
    {
        zbar_found = zbar_proc(big_resolution_data, CAM_CROP_WIDTH, CAM_CROP_HEIGHT);
        if(zbar_found)
        {
            ZBAR_DEMO_DEBUG("-->in big resolution mode.\n");
        }
    }
#endif
    
#ifdef LCD_DISPLAY
    if (zbar_found == 0)
    {
        lcd_clear_area(0, 0, 480, 24);
    }
#endif

    return;
}

int main(void)
{
    int ret = 0;
    
    *(volatile uint32_t *)0x40000090 |= (0x2 << 0);

#ifdef UART_PROT_TEST
    extern int protocol_test(void);
    protocol_test();    
#endif

#ifdef LCD_DISPLAY
    lcd_config();
    set_ui_layer(0, 240);
#endif

    index = 0;
    buf_mgr.last_layer0_index = 0xff;
    buf_mgr.last_layer1_index = 0xff;
    buf_mgr.cur_layer0_index = 0xff;
    buf_mgr.cur_layer1_index = 0xff;

    ret = camera_config();
    if (ret != 0)
    {
        ZBAR_DEMO_DEBUG("there are something wrong\n");
    }

    if(zbar_mem_init() != 0)
    {
        ZBAR_DEMO_DEBUG("zbar_mem_init fail\n");
        return -1;
    }

#ifdef F6721B_RINGSCANER
    protocol_init();
    led_on(1);
    ringscaner_status = SCAN_START;
    while (1)
    {
        if(ringscaner_status == SCAN_START)
        {
            video_proc();
        }
        else
        {
            ksleep(10);
        }
        
        send_heartbeat();
    }
#else
    while (1)
    {
        video_proc();
    }   
#endif

    if(zbar_mem_deinit() != 0)
    {
        ZBAR_DEMO_DEBUG("zbar_mem_init fail\n");
        return -1;
    }

    return 0;
}


