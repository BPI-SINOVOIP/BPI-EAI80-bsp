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

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#if defined (LQFP176_DEMO_BOARD)
    #define PORT     "GPIOG"
    #define LCD_GPIO_PIN   13   /*PB13*/
#else
    #define PORT     "GPIOB"
    #define LCD_GPIO_PIN   13   /*PB13*/
#endif

#define BLACK            0x0000
#define BLUE             0x001F

#define WIN_WIDTH 240
#define WIN_HEIGHT 160

#define IMP_DEVICE (IMP_Device_T *)(0x40024000)

struct device *gpiob;
struct device *ltdc;
struct device *dcmi;
struct device *camera_sensor;

struct fb_displayinfo info;

/** IMP memory to memory transfer mode **/
//#define TEST_CASE_MODE0_ARGB8888    1
//#define TEST_CASE_MODE0_RGB888        1
//#define TEST_CASE_MODE0_RGB565        1
//#define TEST_CASE_MODE0_ARGB1555    1
//#define TEST_CASE_MODE0_ARGB4444    1      //fail
#define TEST_CASE_MODE0_L8_ALIGN_0  1  // richard
//#define TEST_CASE_MODE0_L8_ALIGN_1  1
//#define TEST_CASE_MODE0_L8_ALIGN_2  1
//#define TEST_CASE_MODE0_L8_ALIGN_3  1
//#define TEST_CASE_MODE0_AL44            1
//#define TEST_CASE_MODE0_AL88            1

/** IMP memory to memory with pixel format conversion transfer mode  **/
//#define TEST_CASE_MODE1_ARGB8888_RGB888        1
//#define TEST_CASE_MODE1_RGB888_RGB565            1
//#define TEST_CASE_MODE1_RGB565_L8            1
//#define TEST_CASE_MODE1_RGB888_L8            1
//#define TEST_CASE_MODE1_RGB565_ARGB1555        1
//#define TEST_CASE_MODE1_ARGB1555_ARGB4444    1
//#define TEST_CASE_MODE1_ARGB4444_ARGB8888    1
//#define TEST_CASE_MODE1_L8_ARGB8888                1
//#define TEST_CASE_MODE1_AL44_RGB565                1
//#define TEST_CASE_MODE1_AL88_RGB888                1
//#define TEST_CASE_MODE1_L8_ARGB8888_CLUT       1
//#define TEST_CASE_MODE1_AL44_RGB565_CLUT       1
//#define TEST_CASE_MODE1_AL88_RGB888_CLUT       1
//#define TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW        1
//#define TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY        1


/**IMP memory to memory with blending transfer mode **/
//#define TEST_CASE_MODE2_RGB888_ARGB8888                                1
//#define TEST_CASE_MODE2_RGB565_ARGB8888                     1
//#define TEST_CASE_MODE2_ARGB1555_ARGB8888                            1
//#define TEST_CASE_MODE2_ARGB4444_ARGB8888                            1
//#define TEST_CASE_MODE2_L8_ARGB8888                                        1
//#define TEST_CASE_MODE2_AL44_ARGB8888                                    1   //fail, AL44->ARGB8888 ?
//#define TEST_CASE_MODE2_AL88_ARGB8888                                    1   //fail, AL88->ARGB8888 ?
//#define TEST_CASE_MODE2_ARGB8888_RGB565                                1
//#define TEST_CASE_MODE2_ARGB8888_L8                                1    //fail, ARGB8888->L8
//#define TEST_CASE_MODE2_ARGB1555_L8                            1        //fail, ARG1555->L8
//#define TEST_CASE_MODE2_ARGB4444_L8                            1        //fail, ARGB4444->L8 ?
//#define TEST_CASE_MODE2_ARGB8888_RGB565_ALIGN_2                  1
//#define TEST_CASE_MODE2_ARGB8888_ARGB8888_OFFSET_ODD       1
//#define TEST_CASE_MODE2_ARGB8888_ARGB8888_OFFSET_EVEN      1
//#define TEST_CASE_MODE2_L8_RGB565_ALIGN_OFFSET_ODD            1
//#define TEST_CASE_MODE2_L8_RGB565_ALIGN_OFFSET_EVEN           1
//#define TEST_CASE_MODE2_A4_RGB888                    1
//#define TEST_CASE_MODE2_A8_RGB565                    1
//#define TEST_CASE_MODE2_RGB565_RGB888_ARGB8888                1
//#define TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW        1
//#define TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY        1


/** IMP register to memory transfer mode **/
//#define TEST_CASE_MODE3_ARGB8888    1
//#define TEST_CASE_MODE3_RGB888        1
//#define TEST_CASE_MODE3_RGB565        1
//#define TEST_CASE_MODE3_ARGB1555    1
//#define TEST_CASE_MODE3_ARGB4444    1

//#define TEST_CASE_IRQ  1
//#define TEST_CASE_ABORT 1
//#define TEST_CASE_SUSPEND 1
//#define TEST_CASE_16RD_32WR  1
//#define TEST_CASE_32RD_16WR  1

#if defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW)
    #define SCALE_WIN_WIDTH  200
    #define SCALE_WIN_HEIGHT 100
#else
    #define SCALE_WIN_WIDTH  320
    #define SCALE_WIN_HEIGHT 100//240
#endif

uint32_t clut_rgb888[256] =
{
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
    0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000,
};

uint32_t clut_argb8888[256] =
{
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
    0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff, 0xff0000ff,
};

int IMP_load_clut(uint32_t *color,  uint32_t colorMode, uint32_t clut_size, uint32_t layerIdx)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_CLUTCfg_T CLUTCfg;

    memset(&CLUTCfg, 0, sizeof(IMP_CLUTCfg_T));
    CLUTCfg.enable = 1;
    CLUTCfg.pCLUT = color;
    CLUTCfg.CLUTColorMode = colorMode;//IMP_CCM_ARGB8888;//IMP_CCM_RGB888;
    CLUTCfg.size = 256;

    HAL_IMP_CLUTLoad(pIMP, &CLUTCfg, layerIdx);

    return 0;
}


void IMP_IRQHandler(void *param)
{
    IMP_Device_T *pIMP  = IMP_DEVICE;

    if (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_CE))
    {
        __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_CE);
        printk("Configuration Error Interrupt Flag \n");
    }

    if (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TW))
    {
        __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TW);
        printk("Transfer Watermark Interrupt Flag \n");
    }

    if (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC))
    {
        //__HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);
        printk("Transfer Complete Interrupt Flag \n");
    }

    if (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TE))
    {
        __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TE);
        printk("Transfer Error Interrupt Flag \n");
    }

}

void IMP_set_irq(void)
{
    IMP_Device_T *pIMP = IMP_DEVICE;

    HAL_RequestIrq(IMP_IRQn, IMP_IRQ_PRIORITY, IMP_IRQHandler, NULL);
    HAL_IRQ_ENABLE(IMP_IRQn);

    HAL_IMP_ProgramLineEvent(pIMP, 1);
    __HAL_IMP_ENABLE_IT(pIMP, IMP_IT_CE | IMP_IT_TC | IMP_IT_TE);
}

void imlib_gray_to_background(uint32_t src_addr, uint32_t sizeOfPixel)
{
    int i = 0;

    for (i = 0; i < sizeOfPixel; i++)
    {
        if (i < (sizeOfPixel >> 1))
        {
#if (defined TEST_CASE_MODE2_A4_RGB888) || (defined TEST_CASE_MODE2_A8_RGB565)
            (*(uint8_t *)src_addr) = 0xff;
#else
            (*(uint8_t *)src_addr) = 0x1f;
#endif
            src_addr++;
        }
        else
        {
#if (defined TEST_CASE_MODE2_A4_RGB888) || (defined TEST_CASE_MODE2_A8_RGB565)
            (*(uint8_t *)src_addr) = 0xff;
#else
            (*(uint8_t *)src_addr) = 0x2f;
#endif
            src_addr++;
        }
    }
}

void LCD_set_pixfmt(uint32_t output_pixfmt)
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


int IMP_test_mode0(uint32_t fg_addr, uint32_t dst, uint32_t input_pixfmt, uint32_t output_pixfmt, \
                   uint32_t input_offset, uint32_t output_offset, uint32_t pic_width, uint32_t pic_height)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    pixel_num = pic_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;

    HAL_IMP_HarrisStart(pIMP, fg_addr, dst, &scaleCfg, ENABLE, 2500);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    return 0;
}

int IMP_test_mode1(uint32_t src, uint32_t dst, uint32_t input_pixfmt, uint32_t output_pixfmt, \
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

#if defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY)
    w_scale = (float)pixel_num / (SCALE_WIN_WIDTH - output_offset);
    h_scale = (float)pic_height / SCALE_WIN_HEIGHT;

    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
    scaleCfg.scaleWidth = (SCALE_WIN_WIDTH - output_offset);
    scaleCfg.scaleHeight = SCALE_WIN_HEIGHT;
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

#if (defined TEST_CASE_MODE2_RGB565_RGB888_ARGB8888)
/** if fg/bg frame size is pic_width*pic_height, dst frame size is WIN_WIDTH*WIN_HEIGHT, must make sure pic_width-input_offset = WIN_WIDTH-output_offset **/
int IMP_test_mode2(uint32_t fg_addr, uint32_t bg_addr, uint32_t dst, uint32_t pic_width, uint32_t pic_height)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_BLEND;
    config.colorMode = FMT_ARGB8888;
    config.outputOffset = 0;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = IMP_INPUT_RGB888;
    layerCfg.inputOffset = 0;
    layerCfg.alphaMode = IMP_COMBINE_ALPHA;
    layerCfg.inputAlpha = 0x9f;//0x44;

    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);

    layerCfg.inputColorMode = IMP_INPUT_RGB565;
    layerCfg.inputOffset = 0;
    layerCfg.alphaMode = IMP_COMBINE_ALPHA;
    layerCfg.inputAlpha = 0x7f;//0x38;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pic_width;
    scaleCfg.oriHeight = pic_height;

    HAL_IMP_BlendingStart(pIMP, fg_addr, bg_addr, dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    return 0;
}
#else
/** if fg/bg frame size is pic_width*pic_height, dst frame size is WIN_WIDTH*WIN_HEIGHT, must make sure pic_width-input_offset = WIN_WIDTH-output_offset **/
int IMP_test_mode2(uint32_t fg_addr, uint32_t bg_addr, uint32_t dst, uint32_t input_pixfmt, uint32_t output_pixfmt, \
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
    config.mode = IMP_M2M_BLEND;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_COMBINE_ALPHA;
    layerCfg.inputAlpha = 0x9f;//0x44;

    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);

#if defined(TEST_CASE_MODE2_AL44_ARGB8888) || defined(TEST_CASE_MODE2_AL88_ARGB8888)
    layerCfg.inputColorMode = IMP_INPUT_ARGB8888;
#else
    layerCfg.inputColorMode = input_pixfmt;
#endif

    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_COMBINE_ALPHA;
    layerCfg.inputAlpha = 0x7f;//0x38;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    pixel_num = pic_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

#if defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY)
    w_scale = (float)pixel_num / (SCALE_WIN_WIDTH - output_offset);
    h_scale = (float)pic_height / SCALE_WIN_HEIGHT;

    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
    scaleCfg.scaleWidth = (SCALE_WIN_WIDTH - output_offset);
    scaleCfg.scaleHeight = SCALE_WIN_HEIGHT;
    scaleCfg.svi = fast_floorf(h_scale);
    scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi) * 256);
    scaleCfg.shi = fast_floorf(w_scale);
    scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi) * 256);
#else
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
#endif
    HAL_IMP_BlendingStart(pIMP, fg_addr, bg_addr, dst, &scaleCfg);

#if defined(TEST_CASE_ABORT)
    HAL_IMP_Abort(pIMP);
#else
#if defined(TEST_CASE_SUSPEND)
    HAL_IMP_Suspend(pIMP);
    k_busy_wait(50);
    HAL_IMP_Resume(pIMP);
#endif

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);
#endif

    return 0;
}
#endif

int IMP_test_mode3(uint32_t color, uint32_t dst, uint32_t output_pixfmt, \
                   uint32_t output_offset, uint32_t pic_width, uint32_t pic_height)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_R2M;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    pixel_num = WIN_WIDTH - output_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
    HAL_IMP_Start(pIMP, color, dst, &scaleCfg);

#if defined(TEST_CASE_ABORT)
    HAL_IMP_Abort(pIMP);
#else
    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);
#endif

    return 0;
}

static int imp_trans(int src_addr, int src_offset, int dst_addr, int dst_offset, int image_w, int image_h, int rd16_wr32_en, int rd32_wr16_en)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;

    if (rd16_wr32_en)
    {
        config.colorMode = FMT_ARGB8888;
    }
    else if (rd32_wr16_en)
    {
        config.colorMode = FMT_RGB565;
    }

    config.outputOffset = dst_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));

    if (rd16_wr32_en)
    {
        layerCfg.inputColorMode = IMP_INPUT_RGB565;
    }
    else if (rd32_wr16_en)
    {
        layerCfg.inputColorMode = IMP_INPUT_ARGB8888;
    }

    layerCfg.inputOffset = src_offset;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);


    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = image_w;
    scaleCfg.oriHeight = image_h;

    *(volatile uint32_t *)0x40024000 |= (rd16_wr32_en << 20) | (rd32_wr16_en << 21);

    HAL_IMP_Start(pIMP, (uint32_t)src_addr, (uint32_t)dst_addr, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    return 0;
}

void lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpiob = device_get_binding(PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

#if defined(TEST_CASE_MODE0_RGB888) ||  defined(TEST_CASE_MODE1_RGB888_RGB565) || defined(TEST_CASE_MODE1_RGB888_L8)  \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE3_RGB888)
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_RGB888;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    layerinfo.alpha = 255;//0x7f;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_RGB888;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    //fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = WIN_WIDTH;
    wininfo.height = WIN_HEIGHT;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 200;
    wininfo.y_start = 100;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    //fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    //fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
#else
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_RGB565;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    layerinfo.alpha = 255;//0x7f;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_RGB565;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    //fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = WIN_WIDTH;
    wininfo.height = WIN_HEIGHT;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    info.pic_width = 240;
    info.pic_height = 160;
    wininfo.x_start = 200;
    wininfo.y_start = 100;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    //fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    //fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
#endif
}

#define GREE_CAMERA_OV5640_0_LABEL             "CAMERA_OV5640_0"
#define OV5640_0_DEV_NAME                            GREE_CAMERA_OV5640_0_LABEL

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

#if defined(TEST_CASE_MODE0_RGB888) || defined(TEST_CASE_MODE1_RGB888_RGB565) ||  defined(TEST_CASE_MODE1_RGB888_L8) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE3_RGB888)
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

#if defined(TEST_CASE_MODE0_RGB888) || defined(TEST_CASE_MODE1_RGB888_RGB565) || defined(TEST_CASE_MODE1_RGB888_L8) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE3_RGB888)
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

#if defined(TEST_CASE_MODE0_RGB888) || defined(TEST_CASE_MODE1_RGB888_RGB565) || defined(TEST_CASE_MODE1_RGB888_L8) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE3_RGB888)
    /* only support YUV422 TO RGB888 */
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);
#endif

    reqbufs.count = 2;
    reqbufs.mem_start = 0xc0a00000;//0x1006B000;//0x20020000;//0x10020000;//0xc0000000;

#if defined(TEST_CASE_MODE0_RGB888) || defined(TEST_CASE_MODE1_RGB888_RGB565) ||  defined(TEST_CASE_MODE1_RGB888_L8) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE3_RGB888)
    reqbufs.length = info.pic_width * info.pic_height * 3 * 2;
#else
    reqbufs.length = info.pic_width * info.pic_height * 2 * 2;
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

extern unsigned char img_data_new[];

void make_dstfile(unsigned char *buf, unsigned int size)
{

    int i, j, k, n;

    char pbuf[10] = { 0 };

    k = 0;


    printf("unsigned char img_new_data[] = {\n");

    for (i = 0; i < size; i++)
    {
        k++;
        //sprintf(pbuf, "0x%02x", buf[i]);



        //fwrite(pbuf, strlen(pbuf), 1, infile);
        if (k != 16)
        {
            printf("0x%02x, ", buf[i]);
        }
        //else
        //  printf(",");

        if (k == 16)
        {
            k = 0;
            //fwrite("\n", strlen("\n"), 1, infile);
            printf("\n");

        }

    }

    //if (k == 0)
    printf("};\n");
    //else
    //  fwrite("\n};", strlen("\n};"), 1, infile);

}

int cnt_r = 0;

void main(void)
{
    struct dcmi_buf dis_buf;
    uint32_t addr = 0;
    uint32_t sizeOfPixel = 0;//320 * 240;
    uint32_t *argb8888;
    char *gray_pic;

    char *test_m;

    test_m =  mem_malloc_ext(0, 100);

    *test_m = 3;
    printf("testm = %d", *test_m);

    *(volatile uint32_t *)0x40000090 |= (0x2 << 0);

#if defined(TEST_CASE_MODE2_ARGB8888_RGB565_ALIGN_2)
    uint32_t *argb8888_pic_0 = 0xc0a38404;
    uint32_t *argb8888_pic_1 = 0xc0a5dc04;
    uint32_t *argb8888_pic_2 = 0xc0a83404;
    uint32_t *argb8888_pic_3 = 0xc0aa8c00;
    uint32_t *argb8888_pic_4 = 0xc0ace400;
#elif defined(TEST_CASE_MODE2_L8_RGB565_ALIGN_OFFSET_ODD) || defined(TEST_CASE_MODE2_L8_RGB565_ALIGN_OFFSET_EVEN)
    uint32_t *argb8888_pic_0 = 0xc0a38402;
    uint32_t *argb8888_pic_1 = 0xc0a5dc02;
    uint32_t *argb8888_pic_2 = 0xc0a83402;
    uint32_t *argb8888_pic_3 = 0xc0aa8c00;
    uint32_t *argb8888_pic_4 = 0xc0ace400;
#else
    uint32_t *argb8888_pic_0 = 0xc0a38400;
    uint32_t *argb8888_pic_1 = 0xc0a5dc00;
    uint32_t *argb8888_pic_2 = 0xc0a83400;
    uint32_t *argb8888_pic_3 = 0xc0aa8c00;
    uint32_t *argb8888_pic_4 = 0xc0ace400;
#endif

#if (defined TEST_CASE_MODE0_L8_ALIGN_0) || defined(TEST_CASE_MODE1_L8_ARGB8888) || defined(TEST_CASE_MODE1_L8_ARGB8888_CLUT) || defined(TEST_CASE_MODE2_L8_ARGB8888)
    uint8_t *gray_pic_0 = 0xc0a40000;
    uint8_t *gray_pic_1 = 0xc0a50000;
    uint8_t *gray_pic_2 = 0xc0a60000;
    uint8_t *gray_pic_3 = 0xc0a70000;
#elif (defined TEST_CASE_MODE0_L8_ALIGN_1)
    uint8_t *gray_pic_0 = 0xc0a40001;
    uint8_t *gray_pic_1 = 0xc0a50001;
    uint8_t *gray_pic_2 = 0xc0a60000;
    uint8_t *gray_pic_3 = 0xc0a70000;
#elif (defined TEST_CASE_MODE0_L8_ALIGN_2)
    uint8_t *gray_pic_0 = 0xc0a40002;
    uint8_t *gray_pic_1 = 0xc0a50002;
    uint8_t *gray_pic_2 = 0xc0a60000;
    uint8_t *gray_pic_3 = 0xc0a70000;
#elif (defined TEST_CASE_MODE0_L8_ALIGN_3)
    uint8_t *gray_pic_0 = 0xc0a40003;
    uint8_t *gray_pic_1 = 0xc0a50003;
    uint8_t *gray_pic_2 = 0xc0a60000;
    uint8_t *gray_pic_3 = 0xc0a70000;
#else
    uint8_t *gray_pic_0 = 0xc0a40000;
    uint8_t *gray_pic_1 = 0xc0a50000;
    uint8_t *gray_pic_2 = 0xc0a60000;
    uint8_t *gray_pic_3 = 0xc0a70000;
#endif

    image_t img;
    unsigned char *img_p = (uint32_t)gray_pic_0;
    unsigned char *img_corner = (uint32_t)gray_pic_1;
    int ret = 0;

    lcd_config();
    ret = camera_config();
    if (ret != 0)
    {
        printk("there are something wrong\n");
    }

    sizeOfPixel = info.pic_width * info.pic_height;

    argb8888 = argb8888_pic_3;
    gray_pic = gray_pic_2;

#if defined (TEST_CASE_MODE1_L8_ARGB8888_CLUT) || defined(TEST_CASE_MODE1_AL88_RGB888_CLUT)
    IMP_load_clut(clut_argb8888, IMP_CCM_ARGB8888, 256, 1);
#elif defined(TEST_CASE_MODE1_AL44_RGB565_CLUT)
    IMP_load_clut(clut_rgb888, IMP_CCM_RGB888, 256, 1);
#endif

#if defined(TEST_CASE_IRQ)
    IMP_set_irq();
#endif

#define cxDIB      (240)
#define I(ROW,COL) img_corner[cxDIB*(ROW)+(COL)]
    while (1)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        if (ret == 0)
        {

#if defined(TEST_CASE_MODE0_L8_ALIGN_0)
            imlib_rgb565_to_gray(dis_buf.addr, gray_pic_0, sizeOfPixel);
            //IMP_test_mode0((uint32_t)gray_pic_0, (uint32_t)gray_pic, IMP_INPUT_L8, FMT_L8, 0, 0, info.pic_width, info.pic_height);
            //addr = (uint32_t)gray_pic;

            IMP_test_mode0((uint32_t)gray_pic_0, (uint32_t)gray_pic_1, IMP_INPUT_L8, FMT_L8, 0, 0, info.pic_width, info.pic_height);
            addr = (uint32_t)gray_pic_0;

            LCD_set_pixfmt(FMT_L8);
#endif

            img.bpp = 1;
            img.w = 240;//info.pic_width;
            img.h = 160;//info.pic_height;
            img.pixels = addr;
            int x = 200;
            int y = 40;
            fb_ioctl(ltdc, 1, FB_X_SETADDR, &(dis_buf.addr));
            int k  =  0;
            fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));
            for (int i = 0; i < 160; i++)
            {
                for (int j = 0; j < 240; j++)
                {

                    if ((I(i, j) == 0xFF) || (I(i, j) == 0x0))
                    {
                        continue;
                    }
                    if ((k != 0) && (k % 16) == 0)
                    {
                        //printf("\r\n");
                        k = 0;
                    }
                    k++;

                    if (I(i, j) == 1)
                    {
                        imlib_draw_circle(&img, j, i, 4, 0x0);
                        //printf("(%d,%d)=0x%x ",i,j,I(i,j));
                    }
                }
            }

            dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);

            argb8888 = (argb8888 == argb8888_pic_3) ? argb8888_pic_4 : argb8888_pic_3;
            //gray_pic = (gray_pic == gray_pic_2)? gray_pic_3: gray_pic_2;
        }
    }
}
