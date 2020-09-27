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
 * @brief       imp test demo source file
 *
 * @note
 *              2019/12/27, Christal.Xu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_imp.h>
#include <gm_hal_vout.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>
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

#define IMP_DEVICE (IMP_Device_T *)(0x40024000)

#define output_pixfmt   FMT_RGB888
#define output_offset   0
#define input_pixfmt    FMT_RGB888
#define input_offset    0

#define pix_width       240
#define pix_height      160
#define SCALE_WIN_WIDTH  200
#define SCALE_WIN_HEIGHT 100
#define clut_size       SCALE_WIN_WIDTH*SCALE_WIN_HEIGHT


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
    HAL_Reset_Module(RESET_IMP_SW_RSTJ);

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
    for (i = 0; i < x * 3; i++)
    {
        for (j = 0; j < y; j++)
        {
            *(volatile uint32_t *)addr = color;
            addr++;
        }
    }
}

static int lcd_config(void)
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
    ret = vout_set_clock();
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

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
    ret = HAL_VOUT_Init(VOUT, &data1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* VOUT layer init*/
    layerinfo0.alpha = 255;
    layerinfo0.alpha0 = 0;
    layerinfo0.pixformat = input_pixfmt;//pixfmt;
    layerinfo0.blending_factor1 = 6;
    layerinfo0.blending_factor2 = 7;
    layerinfo0.color = 0x00FF00;
    layerinfo0.fb_start = 0xc0a00000;
    ret = init_layer(0, &layerinfo0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    layerinfo1.alpha = 255;
    layerinfo1.alpha0 = 0;
    layerinfo1.pixformat = output_pixfmt;//pixfmt;
    layerinfo1.blending_factor1 = 6;
    layerinfo1.blending_factor2 = 7;
    layerinfo1.color = 0x0000FF;
    layerinfo1.fb_start = 0xc0a1c200;
    ret = init_layer(1, &layerinfo1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

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

}

static int test_imp(void)
{
    RET_CODE ret = RET_OK;
    IMP_Device_T *pIMP = IMP_DEVICE;
    uint32_t rgb888_pic_0 = 0xc0a00000;
    uint32_t rgb888_pic_1 = 0xc0a1c200;
    uint32_t rgb888_pic_2 = 0xc0a83404;
    uint32_t *rgb888_pic_3 = 0xc0aa8c00;
    uint32_t *rgb888_pic_4 = 0xc0ace400;


    lcd_config();

    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    IMP_CLUTCfg_T CLUTCfg;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;
    float w_scale = 0.0;
    float h_scale = 0.0;
    /* IMP data tranport*/
    /* IMP init*/
    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M;
    config.colorMode = input_pixfmt;
    config.outputOffset = output_offset;
    ret = HAL_IMP_Init(pIMP, &config);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* IMP layer config*/
    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
    //layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    ret = HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* IMP scale config*/
    pixel_num = pix_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    w_scale = (float)pixel_num / (SCALE_WIN_WIDTH - output_offset);
    h_scale = (float)pix_height / SCALE_WIN_HEIGHT;

    //scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pix_height;
    scaleCfg.scaleWidth = (SCALE_WIN_WIDTH - output_offset);
    scaleCfg.scaleHeight = SCALE_WIN_HEIGHT;
    //scaleCfg.svi = fast_floorf(h_scale);
    //scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi)*256);
    //scaleCfg.shi = fast_floorf(w_scale);
    //scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi)*256);

    scaleCfg.enable = FALSE;

    ret = HAL_IMP_Start(pIMP, rgb888_pic_0, rgb888_pic_1, &scaleCfg);//mode memory to memory,the 2nd para is source address
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0) //transfer complete flag
    {
        timeout++;
        if (timeout > 0X1FFFFF)
        {
            break;
        }
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    /* IMP harris corner detect*/

    ret = HAL_IMP_HarrisStart(pIMP, rgb888_pic_0, rgb888_pic_1, &scaleCfg, ENABLE, 128);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }


    /* IMP data blending*/
    /* IMP init*/
    config.mode = IMP_M2M_BLEND;
    config.colorMode = FMT_RGB888;
    config.outputOffset = 0;
    ret = HAL_IMP_Init(pIMP, &config);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* IMP layer config*/
    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = 0;
    //layerCfg.alphaMode = IMP_COMBINE_ALPHA;
    //layerCfg.inputAlpha = 0x9f;//0x44;
    ret = HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* IMP scale config*/
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = 0;
    //layerCfg.alphaMode = IMP_COMBINE_ALPHA;
    //layerCfg.inputAlpha = 0x7f;//0x38;
    ret = HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pix_width;
    scaleCfg.oriHeight = pix_height;

    Creat_pic_bg(240, 160, 0xc0a00000, 0xFF);
    Creat_pic_bg(240, 160, 0xc0a1c200, 0x20);
    Creat_pic_bg(240, 160, 0xc0a38400, 0x00);

    ret = HAL_IMP_BlendingStart(pIMP, 0xc0a00000, 0xc0a38400, 0xc0a1c200, &scaleCfg);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    /* IMP CLUT function*/
    Creat_pic_bg(240, 160, 0xc0a00000, 0x00);
    memset(&CLUTCfg, 0, sizeof(IMP_CLUTCfg_T));
    CLUTCfg.enable = 1;
    CLUTCfg.pCLUT = &clut_rgb888;
    CLUTCfg.CLUTColorMode = IMP_CCM_RGB888;
    CLUTCfg.size = 256;
    ret = HAL_IMP_CLUTLoad(pIMP, &CLUTCfg, 0);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_IMP_ProgramLineEvent(pIMP, 0x64);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }
    if (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TW) == 0) //watermask line interrupt
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_IMP_Suspend(pIMP);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_IMP_Resume(pIMP);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    ret = HAL_IMP_Abort(pIMP);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return TC_FAIL;
    }

    return TC_PASS;
}

void gm_test_imp(void)
{
    zassert_true(test_imp() == TC_PASS, NULL);
}

void imp_test(void)
{
    TC_PRINT("================IMP Test Start=================\n\n\n");
    TC_PRINT("TEST1\n");
    zassert_true(test_imp() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================IMP Test End==================\n\n\n");
    //wait 5 sec for display
    k_sleep(5000);
}
