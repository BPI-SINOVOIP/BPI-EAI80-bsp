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
#include <imlib.h>
#include <gm_hal_vout.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_imp.h>
#include <math.h>

#include "app_processor.h"
#include "processor.h"


/* Enable one of the following macro */
//#define INPUT_RGB  1
#define INPUT_GRAY 1


#define CAM_WIDTH       320
#define CAM_HEIGHT      240
#define CAM_CROP_WIDTH  320
#define CAM_CROP_HEIGHT 240
#define DIS_WIDTH       CAM_CROP_WIDTH
#define DIS_HEIGHT      CAM_CROP_HEIGHT

#define INPUT_WIDTH    CAM_CROP_WIDTH
#define INPUT_HEIGHT   CAM_CROP_HEIGHT


#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE        (5*SIZE_1M)
#define OPS_BIN_SIZE_OPTIMIZE (4*SIZE_1M)

#define AI_FLASH_START_OFFSET  (0x00050000)

#define OPS_BIN_FLASH_START (AI_FLASH_START_OFFSET)
#define IMAGE_BIN_FLASH_START (OPS_BIN_FLASH_START+OPS_BIN_SIZE)
#define IMAGE_BUFFER_SIZE   (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2 /*input.bin*/

#define BUFFERSIZE (INPUT_WIDTH * INPUT_HEIGHT * 4) /*INPUT_WIDTH * INPUT_HEIGHT * 4*/
#define BLACK         0x000000
#define BLUE          0x0000FF




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
#define IMP_DEVICE (IMP_Device_T *)(0x40024000)


#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE_OPTIMIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/


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
static unsigned char pred_buf_in[INPUT_WIDTH*INPUT_HEIGHT*3] = {0};
static unsigned char pred_buf_out[INPUT_WIDTH*INPUT_HEIGHT] = {0};

static uint8_t show_demo_pic = 1;

static uint8_t pixfmt = FMT_RGB888;
static int index = 0;


extern int aimodule_init_device(void);

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

    fb_irq_line_enable(ltdc, (info.pic_height-10));
    fb_irq_callback_set(ltdc, LCD_isr);
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
    if(ret != 0)
    {
        return -ENOTSUP;
    }
#endif

    dcmi = device_get_binding(CONFIG_VIN_NAME);

    fmt.pix_fmt = PIX_FMT_YUV422;
    fmt.width = CAM_WIDTH;
    fmt.height = CAM_HEIGHT;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

#if 0
    rect.top = 0;
    rect.left = 0;
    rect.width = 2 * CAM_CROP_WIDTH - 1;
    rect.height = CAM_CROP_HEIGHT - 1;
    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);
#endif

    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);

    reqbufs.count = 3;
    reqbufs.mem_start = CAMERA_IMAGE_BUFFER_START;
    reqbufs.length =  CAM_CROP_WIDTH * CAM_CROP_HEIGHT * 3 * reqbufs.count;
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

    memset(inputImage, 0x00, BUFFERSIZE);
    memset(OutputImage, 0x00, BUFFERSIZE);

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
    ai_init_storage();

    return 0;
}

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


//#define FILTER_CPU_TEST /* this test can be enble only if defined INPUT_GRAY */
#ifdef FILTER_CPU_TEST
static void conv2(uint8_t *img, int w[3][3], int rows, int cols, uint8_t *result) 
{
	int bm = 3; int bn = 3;
	int n1 = 0;//rows + bm - 1;
	int n2 = 0;// cols + bn - 1;
	int sum_ = 0;
	int i = 0, j = 0, m = 0, n = 0;
	int rm = 0, rn = 0;

	n1 = rows;//rows + bm - 1;
	n2 = cols;// cols + bn - 1;

	for (i = 0; i < n1; i++) 
	{
		for (j = 0; j < n2; j++)
		{
			if (j % 60 == 0)
			{
				//printf("check\n");
			}

			sum_ = 0;
			for (m = 0; m < bm; m++)
			{
				for (n = 0; n < bn; n++)
				{
					rm = i - m;
					rn = j - n;
					/*zero padding*/
					if (rm >= 0 && rm < rows&&rn >= 0 && rn < cols)
						sum_ += img[rm*cols + rn] * w[m][n];
				}
			}
			sum_ = abs(sum_);
			result[i*cols + j] = sum_;
			if (sum_ > 255)
			{
				result[i*cols + j] = 255;
			}
		}
	}
	return;
}

static int filter_cpu(uint8_t *data, int height, int width) 
{
    int sx[3][3] = {
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 }};

    conv2(data, sx, height, width, pred_buf_out);
}
#endif

void predict(struct easynet_dev *dev, unsigned char *ops_data)
{
    int ret = 0;
    short *poutbuf = NULL;
    int i = 0;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                poutbuf =  dev->predict_out;
				#if 1
                for(i=0; i<INPUT_WIDTH*INPUT_HEIGHT; i++)
                {
                    if(poutbuf[i] > 255)
                    {
                        pred_buf_out[i] = 255;
                    }
                    else if(poutbuf[i] < 0)
                    {
                        pred_buf_out[i] = 0;
                    }
                    else
                    {
                        pred_buf_out[i] = (unsigned char)(poutbuf[i]);
                    }
                }
				#endif
                dev = easynet_process_update(dev->cfg, ops_data);
                dev->predict_out = pred_buf_out;
            }
            break;
        }
    } while (1);

    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

void filter_process(void)
{
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;
    struct easynet_dev *dev;
    RET_CODE ret = RET_OK;
    extern uint32_t t_Hw, t_BankGeneral, t_CrossPut, t_dwcon ;   
    struct easynet_dev_cfg dev_cfg;
    uint32_t dis_addr = 0;
    struct dcmi_buf dis_buf;
    struct dcmi_buf release_buf;

    #define OPTIMIZATION_LEVEL_1  1
    #define LAYER_BUFFER_NUM    6
    #define OPTIMIZATION_LEVEL_2  2

    dev_cfg.bn_epsilon = 0.0; /* Used default epsilon */
    dev_cfg.optimization = OPTIMIZATION_LEVEL_2;
    dev_cfg.layer_buffer_num = LAYER_BUFFER_NUM;
    dev_cfg.layer_buffer[0] = &inputImage;
    dev_cfg.layer_buffer[1] = &OutputImage;   

    dev = easynet_process_init(&dev_cfg, OPS_WEI_BASE);    

	show_demo_pic = 1;
    //k_sleep(1000);
    while (1)
    {
        if (show_demo_pic)
        {
            dev->input = RecvBuffer;
        }
        else
        {
            ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
            if(ret == 0)
            {
                #ifdef INPUT_RGB
                rgb_split2rrggbb((unsigned char*)dis_buf.addr, pred_buf_in, INPUT_WIDTH, INPUT_HEIGHT);
                #else//INPUT_GRAY
                imp_format(dis_buf.addr, (uint32_t)pred_buf_in, IMP_INPUT_RGB888, FMT_L8, 0, 0, INPUT_WIDTH, INPUT_HEIGHT);
                #endif
				dev->input = pred_buf_in;
                
                buf_mgr.last_layer0_index = buf_mgr.cur_layer0_index;
                buf_mgr.cur_layer0_index = dis_buf.index;
                if (buf_mgr.last_layer0_index != 0xff)
                {
                    release_buf.index = buf_mgr.last_layer0_index;
                    dcmi_ioctl(dcmi, VIDIOC_QBUF, &release_buf);
                }
            }
            else
            {
				//k_sleep(10);
                continue;
            }
        }
        t_last = k_uptime_get_32();
        #ifdef FILTER_CPU_TEST
        filter_cpu(dev->input, INPUT_HEIGHT, INPUT_WIDTH);
        #else
        predict(dev, OPS_WEI_BASE);
        #endif
        t_now = k_uptime_get_32();
        printf("cost %d ms\n", t_now - t_last);

        LCD_set_pixfmt(FMT_L8);
        dis_addr = (uint32_t)pred_buf_out;
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &dis_addr);
        //HAL_VIN_Enable((VIN_Device_T *)0x40025000);
    }    
}


int main(void)
{
    RET_CODE ret = RET_OK;

    OBJ_DBG_PRINTF("filter demo begin\n");
    device_init();
    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE_OPTIMIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    filter_process();
    OBJ_DBG_PRINTF("filter demo exit\n");

    return ret;
}


