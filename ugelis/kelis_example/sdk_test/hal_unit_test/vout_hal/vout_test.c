/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Christal.Xu
 *
 * @version     v1.0.0
 *
 * @date        2019/12/27
 *
 * @brief       pes test demo source file
 *
 * @note
 *              2019/12/27, Christal.Xu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_vout.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#include <pinmux.c>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif


/* change this to enable pull-up/pull-down */


/* Change this if you have an LED connected to a custom port */

/*some basic config options*/
#define BLACK           0x0000
#define BLUE            0x001F
#define BACK_COLOR      0x00000000
#define PIC_WIDTH       240
#define PIC_HEIGHT      160

/*some basic config options*/
#define FB_X_INITLAYER      0x01 /* Initializing the display layer */
#define FB_X_SETADDR        0x02 /* Setting the frame buffer address */
#define FB_X_SETLUTENTRY    0x03 /* Setting an entry of the LUT */
#define FB_X_ON             0x04 /* Switching the display on */
#define FB_X_OFF            0x05 /* Switching the display off */
#define FB_X_SETSIZE        0x06 /* Setting the layer size */
#define FB_X_SETPOS         0x07 /* Setting the layer position */
#define FB_X_SETVIS         0x08 /* Setting the visibility of a layer */
#define FB_X_SETALPHA       0x09 /* Setting the alpha value of the layer */
#define FB_X_SETCHROMAMODE  0x0A /* Setting the chroma blending mode */
#define FB_X_SETCHROMA      0x0B /* Setting the chroma values */
#define FB_X_SETPREMUL      0x0C /* Setting the layer input pixel data has been multiply by alpha */

#define DEV_CFG(dev)                            \
    ((const struct vout_gm6721x_config * const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct vout_gm6721x_data * const)(dev)->driver_data)
#define VOUT_STRUCT(dev)                    \
    ((VOUT_Device_T *)(DEV_CFG(dev))->vout_base)
#define IMP_STRUCT(dev)                 \
    ((IMP_Device_T *)(DEV_CFG(dev))->imp_base)

struct device *gpiob;
struct device *ltdc;

uint32_t addr0      = 0xc0a00000;

struct fb_displayinfo
{
    uint32_t pic_width;
    uint32_t pic_height;
    uint32_t pixclock;
};

struct fb_wininfo
{
    uint16_t x_start;       /* Where to place image */
    uint16_t y_start;
    uint16_t width;         /* Size of image */
    uint16_t height;
};

struct fb_layerinfo
{
    uint32_t pixformat;
    uint32_t alpha;
    uint32_t alpha0;
    uint32_t blending_factor1;
    uint32_t blending_factor2;
    uint32_t fb_start;
    uint32_t color;
};

enum fb_pixel_format
{
    FMT_ARGB8888,
    FMT_RGB888,
    FMT_RGB565,
    FMT_ARGB1555,
    FMT_ARGB4444,
    FMT_L8,
    FMT_AL44,
    FMT_AL88,
};

struct vout_gm6721x_config
{
    uint32_t vout_base;
    uint32_t imp_base;
    uint32_t pic_width;
    uint32_t pic_height;
    uint16_t hsw;
    uint16_t vsw;
    uint16_t hbp;
    uint16_t vbp;
    uint16_t hfp;
    uint16_t vfp;
    uint32_t pllsain;
    uint32_t pllsair;
    uint32_t pllsaidivr;
    void (*irq_config_func)(struct device *dev);
};

static int vout_set_clock()
{
    Peripheral_Config_T peripheClockSet;

    /* VOUT clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_VOUT);
    HAL_Reset_Module(RESET_VOUT_SW_RSTJ);
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, ENABLE);

    /** lcd pixel clock = 2*(pllsain+128)/pllsair/pllsaidivr **/
    HAL_CLOCK_AdjustPLLSAIDefaultDiv(0, 8, 16, 0);
    peripheClockSet.peripheMask = PERIPHERAL_VOUT_MASK;
    peripheClockSet.peripheClkConfig.voutClkConfig = CLOCK_VOUT_DIV_5;
    HAL_CLOCK_PeripheralClkSetConfig(&peripheClockSet);

    /* IMP clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_IMP);
    HAL_CLOCK_PeripheralClockEnable0(RESET_IMP_SW_RSTJ);

    return 0;
}

static int init_layer(uint8_t layer_index, void *arg)
{
    RET_CODE ret = RET_OK;

    const struct vout_gm6721x_config *cfg;
    VOUT_LayerConfig_T pLayerCfg;
    struct fb_layerinfo *layerinfo;

    layerinfo = (struct fb_layerinfo *)arg;
    pLayerCfg.windowX0 = 0;
    pLayerCfg.windowY0 = 0;
    //pLayerCfg.windowX1 = cfg->pic_width;
    //pLayerCfg.windowY1 = cfg->pic_height;
    pLayerCfg.pixelFormat = layerinfo->pixformat;
    pLayerCfg.alpha = layerinfo->alpha;
    pLayerCfg.alpha0 = layerinfo->alpha0;
    pLayerCfg.FBStartAdress = layerinfo->fb_start;
    pLayerCfg.imageWidth = cfg->pic_width;
    pLayerCfg.imageHeight = cfg->pic_height;
    pLayerCfg.backColor.red = (uint8_t)((layerinfo->color & 0X00FF0000) >> 16);
    pLayerCfg.backColor.green = (uint8_t)((layerinfo->color & 0X0000FF00) >> 8);
    pLayerCfg.backColor.blue = (uint8_t)(layerinfo->color & 0X000000FF);
    ret = HAL_VOUT_ConfigLayer(VOUT, &pLayerCfg, layer_index);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    return ret;
}

static void Creat_pic_bg(uint16_t x, uint16_t y, uint32_t addr, uint32_t color)
{
    uint16_t i, j;
    for (i = 0; i < x * 4; i++)
    {
        for (j = 0; j < y; j++)
        {
            *(volatile uint32_t *)addr = color;
            addr++;
        }
    }
}

static int test_vout(void)
{
    RET_CODE ret = RET_OK;

    struct fb_displayinfo displayinfo0, displayinfo1;
    struct fb_layerinfo layerinfo0, layerinfo1;
    struct fb_wininfo wininfo0, wininfo1;
    VOUT_Config_T data1;

    //memset(displayinfo, 0, sizeof(displayinfo));

    /* LCD pin config */
    uint8_t pin_num;
    GPIO_PinConfig_T pConfig;
    pConfig.pin = GPIO_PIN_12;
    pConfig.mode = GPIO_MODE_OUTPUT;
    pConfig.pull = GPIO_NOPULL;
    pConfig.alternate = 0;
    pin_num = 12 + 48;
    lcd_pinmux_config();
    HAL_GPIO_Init(GPIOD, &pConfig);
    HAL_GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_SET);

    /* VOUT clock config */
    vout_set_clock();

    /* VOUT init*/
    data1.HSPolarity = VOUT_HSPOLARITY_AL;
    data1.VSPolarity = VOUT_VSPOLARITY_AL;
    data1.DEPolarity = VOUT_DEPOLARITY_AL;
    data1.PCPolarity = VOUT_PCPOLARITY_IPC;
    data1.horizontalSync = 0; // 1
    data1.verticalSync = 0;   // 1
    data1.accumulatedHBP = 40; //hbp=40
    data1.accumulatedVBP = 8; //vbp=8
    data1.accumulatedActiveW = 520;//width=480
    data1.accumulatedActiveH = 280;//height=272
    data1.totalWidth = 525;   //hfp=5
    data1.totalHeigh = 288;  //vfp=8
    data1.backColor.red = 0xFF;
    data1.backColor.green = 0x00;
    data1.backColor.blue = 0x00;
    HAL_VOUT_Init(VOUT, &data1);

    /* VOUT layer init*/
    layerinfo0.alpha = 255;
    layerinfo0.alpha0 = 0;
    layerinfo0.pixformat = FMT_RGB888;//pixfmt;
    layerinfo0.blending_factor1 = 6;
    layerinfo0.blending_factor2 = 7;
    layerinfo0.color = 0x00FF00;
    layerinfo0.fb_start = 0xc0a00000;
    init_layer(0, &layerinfo0);

    layerinfo1.alpha = 255;
    layerinfo1.alpha0 = 0;
    layerinfo1.pixformat = FMT_RGB888;//pixfmt;
    layerinfo1.blending_factor1 = 6;
    layerinfo1.blending_factor2 = 7;
    layerinfo1.color = 0x0000FF;
    layerinfo1.fb_start = 0xc0a1c200;
    init_layer(1, &layerinfo1);

    Creat_pic_bg(240, 160, 0xc0a00000, 0xFF);
    Creat_pic_bg(240, 160, 0xc0a1c200, 0x88);

    /* VOUT window size set*/
    displayinfo0.pic_width = 240;
    displayinfo0.pic_height = 160;
    wininfo0.x_start = 0;
    wininfo0.y_start = 0;
    wininfo0.width = displayinfo0.pic_width;
    wininfo0.height = displayinfo0.pic_height;
    ret = HAL_VOUT_SetWindowSize(VOUT, wininfo0.width, wininfo0.height, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    displayinfo1.pic_width = 240;
    displayinfo1.pic_height = 160;
    wininfo1.x_start = 240;
    wininfo1.y_start = 160;
    wininfo1.width = displayinfo1.pic_width;
    wininfo1.height = displayinfo1.pic_height;
    ret = HAL_VOUT_SetWindowSize(VOUT, wininfo1.width, wininfo1.height, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }


    /* VOUT window position set*/
    ret = HAL_VOUT_SetWindowPosition(VOUT, wininfo0.x_start, wininfo0.y_start, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_SetWindowPosition(VOUT, wininfo1.x_start, wininfo1.y_start, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT color key config*/
    ret = HAL_VOUT_EnableColorKeying(VOUT, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_ConfigColorKeying(VOUT, 0x00FF00, 0);//replace specify color
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_DisableColorKeying(VOUT, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_VOUT_EnableColorKeying(VOUT, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_ConfigColorKeying(VOUT, 0x0000FF, 1);//replace specify color
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_DisableColorKeying(VOUT, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT clut config*/
    ret = HAL_VOUT_EnableCLUT(VOUT, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    //ret = HAL_VOUT_ConfigCLUT(VOUT, uint32_t *pCLUT, uint32_t CLUTSize, 0);//replace specify color
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_DisableCLUT(VOUT, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_VOUT_EnableCLUT(VOUT, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    //ret = HAL_VOUT_ConfigCLUT(VOUT, uint32_t *pCLUT, uint32_t CLUTSize, 1);//replace specify color
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_DisableCLUT(VOUT, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT premultiply config(with alpha value)*/
    ret = HAL_VOUT_EnablePreMultiply(VOUT, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_DisablePreMultiply(VOUT, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_VOUT_EnablePreMultiply(VOUT, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_DisablePreMultiply(VOUT, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT dither enable*/
    ret = HAL_VOUT_EnableDither(VOUT);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_EnableDither(VOUT);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT alpha value set */
    ret = HAL_VOUT_SetAlpha(VOUT, 0x00FFFFFF, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_SetAlpha(VOUT, 0x0000FFFF, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }


    /* VOUT color frame buffer length config */
    ret = HAL_VOUT_SetPitch(VOUT, 0x38400, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_SetPitch(VOUT, 0x38400, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT interrupt enable config */
    HAL_VOUT_EnableInterrupts(VOUT, 0x03);

    HAL_VOUT_DisableInterrupts(VOUT, 0x03);


    /* VOUT work enable config */
    HAL_VOUT_OnOff(VOUT, OFF);
    HAL_VOUT_OnOff(VOUT, ON);

    /* VOUT layer work enable config */
    ret = HAL_VOUT_layerOnOff(VOUT, 0, OFF);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_layerOnOff(VOUT, 0, ON);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_VOUT_layerOnOff(VOUT, 1, OFF);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    ret = HAL_VOUT_layerOnOff(VOUT, 1, ON);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    return TC_PASS;
}

void vout_test(void)
{
    TC_PRINT("================VOUT Test Start=================\n\n\n");
    TC_PRINT("TEST1\n");
    zassert_true(test_vout() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================VOUT Test End==================\n\n\n");
}
