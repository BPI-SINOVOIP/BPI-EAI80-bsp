/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * References:
 *
 * https://www.microbit.co.uk/device/screen
 * https://lancaster-university.github.io/microbit-docs/ubit/display/
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <board.h>
#include <init.h>
#include <clock_control.h>

#include <display/fb_display.h>
#include <gm_hal_imp.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#include "display_gm6721x.h"



/* convenience defines */
#define DEV_CFG(dev)                            \
    ((const struct vout_gm6721x_config * const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct vout_gm6721x_data * const)(dev)->driver_data)
#define VOUT_STRUCT(dev)                    \
    ((VOUT_Device_T *)(DEV_CFG(dev))->vout_base)
#define IMP_STRUCT(dev)                 \
    ((IMP_Device_T *)(DEV_CFG(dev))->imp_base)

/**
 * @brief Getting display information.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param info.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_get_displayinfo(struct device *dev, struct fb_displayinfo *info)
{
    const struct vout_gm6721x_config *cfg = DEV_CFG(dev);
    struct vout_gm6721x_data *data = DEV_DATA(dev);

    k_mutex_lock(&data->mutex, K_FOREVER);

    info->pic_width = cfg->pic_width;
    info->pic_height = cfg->pic_height;

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Draws a rectangle.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param rect .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_fill_rect(struct device *dev, const struct fb_fillrect *rect)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_R2M;
    config.colorMode = rect->pixfmt;
    config.outputOffset = rect->offline;
    HAL_IMP_Init(pIMP, &config);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = rect->xsize;
    scaleCfg.oriHeight = rect->ysize;

    HAL_IMP_Start(pIMP, rect->color, rect->dst_addr, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Copy data from area to another.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param region .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_copy_area(struct device *dev, const struct fb_copyarea *region)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M;
    config.colorMode = region->pixfmt;
    config.outputOffset = region->offline_dst;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = region->pixfmt;
    layerCfg.inputOffset = region->offline_src;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = region->xsize;
    scaleCfg.oriHeight = region->ysize;
    HAL_IMP_Start(pIMP, region->src_addr, region->dst_addr, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Automatic CLUT loading.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param color     foreground CLUT memory address.
 * @param clut_size clut size.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_load_clut(struct device *dev, uint32_t *color, uint32_t clut_size)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_CLUTCfg_T CLUTCfg;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&CLUTCfg, 0, sizeof(IMP_CLUTCfg_T));
    CLUTCfg.enable = 1;
    CLUTCfg.pCLUT = color;
    CLUTCfg.CLUTColorMode = IMP_CCM_RGB888;
    CLUTCfg.size = ((clut_size - 1) & 0xFF);

    HAL_IMP_CLUTLoad(pIMP, &CLUTCfg, 1);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief blending foreground and background.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fg_addr  foreground memory address.
 * @param bg_addr  background memory address.
 * @param dst       destination buffer address.
 * @param pixel_num pixel number of a line.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_alpha_blendingbulk(struct device *dev, uint32_t *fg_addr, uint32_t *bg_addr, uint32_t *dst, uint32_t pixel_num)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_BLEND;
    config.colorMode = IMP_OUTPUT_ARGB8888;
    config.outputOffset = 0;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = IMP_INPUT_ARGB8888;
    layerCfg.inputOffset = 0;

    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = 1;

    HAL_IMP_BlendingStart(pIMP, (uint32_t)fg_addr, (uint32_t)bg_addr, (uint32_t)dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Alpha blending foreground and background color.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param region .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_mix_colors(struct device *dev, uint32_t fg_color, uint32_t bg_color, uint8_t alpha, uint32_t *output_color)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t fg_format, bg_format, dst;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    if ((bg_color & 0xFF000000) == 0xFF000000)
    {
        *output_color = fg_color;
        k_mutex_unlock(&data->mutex);
        return 0;
    }

    fg_format = fg_color ^ 0xFF000000;
    bg_format = bg_color ^ 0xFF000000;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_BLEND;
    config.colorMode = IMP_OUTPUT_ARGB8888;
    config.outputOffset = 0;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = IMP_INPUT_ARGB8888;
    layerCfg.inputOffset = 0;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    layerCfg.inputAlpha = ((uint32_t)(255 - alpha) << 24);

    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);

    layerCfg.inputColorMode = IMP_INPUT_ARGB8888;
    layerCfg.inputOffset = 0;
    layerCfg.alphaMode = IMP_REPLACE_ALPHA;
    layerCfg.inputAlpha = ((uint32_t)alpha << 24);
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = 1;
    scaleCfg.oriHeight = 1;

    HAL_IMP_BlendingStart(pIMP, (uint32_t)&fg_format, (uint32_t)&bg_format, (uint32_t)&dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    *output_color = (dst ^ 0xFF000000);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief pixel format conversion.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param src        source buffer address.
 * @param dst        destination buffer address.
 * @param src_pixfmt input pixel format.
 * @param dst_pixfmt output pixel format.
 * @param xsize       pixel number of a line.
 * @param ysize       line number.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_convert_color(struct device *dev, void *src, void *dst, uint32_t src_pixfmt, uint32_t dst_pixfmt, uint32_t xsize, uint32_t ysize)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = dst_pixfmt;
    config.outputOffset = 0;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = src_pixfmt;
    layerCfg.inputOffset = 0;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = xsize;
    scaleCfg.oriHeight = ysize;
    HAL_IMP_Start(pIMP, (uint32_t)src, (uint32_t)dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Alpha blending foreground and background.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fg_addr   foreground memory address.
 * @param bg_addr   background memory address.
 * @param dst       destination buffer address.
 * @param alpha     alpha value.
 * @param pixel_num pixel number of a line.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_mix_colorsbulk(struct device *dev, const struct fb_mixcolorbulk *mixcolorbulk)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_BLEND;
    config.colorMode = mixcolorbulk->dst_pixfmt;
    config.outputOffset = 0;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = mixcolorbulk->fg_pixfmt;
    layerCfg.inputOffset = 0;
    layerCfg.alphaMode = IMP_REPLACE_ALPHA;
    layerCfg.inputAlpha = 255 - mixcolorbulk->alpha;

    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 0);

    layerCfg.inputColorMode = mixcolorbulk->bg_pixfmt;
    layerCfg.inputOffset = 0;
    layerCfg.alphaMode = IMP_REPLACE_ALPHA;
    layerCfg.inputAlpha = mixcolorbulk->alpha;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = mixcolorbulk->xsize;
    scaleCfg.oriHeight = mixcolorbulk->ysize;
    HAL_IMP_BlendingStart(pIMP, (uint32_t)mixcolorbulk->fg_addr, (uint32_t)mixcolorbulk->bg_addr, (uint32_t)mixcolorbulk->dst_addr, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Draw L8 bitmap.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param src         source buffer address.
 * @param dst         destination buffer address.
 * @param offline_src source line offset.
 * @param offline_dst destination line offset.
 * @param pixfmt      pixel format.
 * @param xsize       pixel number of a line.
 * @param ysize       line number.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vout_gm6721x_draw_bitmapl8(struct device *dev, void *src, void *dst, uint32_t offline_src, uint32_t offline_dst, uint32_t pixfmt, uint32_t xsize, uint32_t ysize)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t timeout = 0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = pixfmt;
    config.outputOffset = offline_dst;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = IMP_INPUT_L8;
    layerCfg.inputOffset = offline_src;

    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = xsize;
    scaleCfg.oriHeight = ysize;
    HAL_IMP_BlendingStart(pIMP, (uint32_t)src, (uint32_t)dst, (uint32_t)dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

#ifdef __USE_KEIL_
static int asm fast_floorf_in(float x)
{
    VCVT.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

static int fast_floorf(float x)
{
    return fast_floorf_in(x);
}

static int asm fast_roundf_in(float x)
{
    VCVTR.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

static int fast_roundf(float x)
{
    return fast_roundf_in(x);
}
#else
static int inline fast_floorf_in(float x)
{
    int i;

    __asm__ volatile(
                    "vcvt.S32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));

    return i;
}

static int fast_floorf(float x)
{
    return fast_floorf_in(x);
}

static int inline fast_roundf_in(float x)
{
    int i;

    __asm__ volatile(
                    "vcvtr.s32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));

    return i;
}

static int fast_roundf(float x)
{
    return fast_roundf_in(x);
}
#endif

static int vout_gm6721x_scaler(struct device *dev, const struct fb_scalerinfo *param)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    IMP_Device_T *pIMP = IMP_STRUCT(dev);
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t input_offset = 0;
    uint32_t output_offset = 0;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;
    float w_scale = 0.0;
    float h_scale = 0.0;

    k_mutex_lock(&data->mutex, K_FOREVER);

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = param->dst_fmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = param->src_fmt;
    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    pixel_num = param->src_w - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    w_scale = (float)pixel_num / (param->dst_w - output_offset);
    h_scale = (float)param->src_h / param->dst_h;

    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = param->src_h;
    scaleCfg.scaleWidth = (param->dst_w - output_offset);
    scaleCfg.scaleHeight = param->dst_h;
    scaleCfg.svi = fast_floorf(h_scale);
    scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi) * 256);
    scaleCfg.shi = fast_floorf(w_scale);
    scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi) * 256);

    HAL_IMP_Start(pIMP, param->src, param->dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)
        //break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Setting layer information.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param layerinfo .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static int vout_gm6721x_ioctl(struct device *dev, uint8_t layer_index, uint32_t cmd, void *arg)
{
    const struct vout_gm6721x_config *cfg = DEV_CFG(dev);
    VOUT_Device_T *pVOUT = VOUT_STRUCT(dev);
    VOUT_LayerConfig_T pLayerCfg;
    struct fb_layerinfo *layerinfo;
    struct fb_wininfo *wininfo;
    struct fb_clutinfo *clutinfo;
    uint32_t fb_start;
    uint32_t layer_onoff;
    uint32_t chroma_mode;
    uint32_t color;
    uint32_t alpha;
    uint32_t premultiply_onoff;

    switch (cmd)
    {
        case FB_X_INITLAYER:
            layerinfo = (struct fb_layerinfo *)arg;
            pLayerCfg.windowX0 = 0;
            pLayerCfg.windowY0 = 0;
            pLayerCfg.windowX1 = cfg->pic_width;
            pLayerCfg.windowY1 = cfg->pic_height;
            pLayerCfg.pixelFormat = layerinfo->pixformat;
            pLayerCfg.alpha = layerinfo->alpha;
            pLayerCfg.alpha0 = layerinfo->alpha0;
            pLayerCfg.FBStartAdress = layerinfo->fb_start;
            pLayerCfg.imageWidth = cfg->pic_width;
            pLayerCfg.imageHeight = cfg->pic_height;
            pLayerCfg.backColor.red = (uint8_t)((layerinfo->color & 0X00FF0000) >> 16);
            pLayerCfg.backColor.green = (uint8_t)((layerinfo->color & 0X0000FF00) >> 8);
            pLayerCfg.backColor.blue = (uint8_t)(layerinfo->color & 0X000000FF);
            HAL_VOUT_ConfigLayer(pVOUT, &pLayerCfg, layer_index);
            break;
        case FB_X_SETADDR:
            fb_start = *(uint32_t *)arg;
            HAL_VOUT_SetAddress(pVOUT, fb_start, layer_index);
            break;
        case FB_X_SETLUTENTRY:
            clutinfo = (struct fb_clutinfo *)arg;
            if (clutinfo->enable == true)
            {
                HAL_VOUT_EnableCLUT(pVOUT, layer_index);
                HAL_VOUT_ConfigCLUT(pVOUT, clutinfo->clut_addr, clutinfo->clut_size, layer_index);
            }
            else
            {
                HAL_VOUT_DisableCLUT(pVOUT, layer_index);
            }
            break;
        case FB_X_ON:
            HAL_VOUT_OnOff(pVOUT, 1);
            break;
        case FB_X_OFF:
            HAL_VOUT_OnOff(pVOUT, 0);
            break;
        case FB_X_SETSIZE:
            wininfo = (struct fb_wininfo *)arg;
            HAL_VOUT_SetWindowSize(pVOUT, wininfo->width, wininfo->height, layer_index);
            break;
        case FB_X_SETPOS:
            wininfo = (struct fb_wininfo *)arg;
            HAL_VOUT_SetWindowPosition(pVOUT, wininfo->x_start, wininfo->y_start, layer_index);
            break;
        case FB_X_SETVIS:
            layer_onoff = *(uint32_t *)arg;
            HAL_VOUT_layerOnOff(pVOUT, layer_index, layer_onoff);
            break;
        case FB_X_SETALPHA:
            alpha = *(uint32_t *)arg;
            HAL_VOUT_SetAlpha(pVOUT, alpha, layer_index);
            break;
        case FB_X_SETCHROMAMODE:
            chroma_mode = *(uint32_t *)arg;
            if (chroma_mode != 0)
            {
                HAL_VOUT_EnableColorKeying(pVOUT, layer_index);
            }
            else
            {
                HAL_VOUT_DisableColorKeying(pVOUT, layer_index);
            }
            break;
        case FB_X_SETCHROMA:
            color = *(uint32_t *)arg;
            HAL_VOUT_ConfigColorKeying(pVOUT, color, layer_index);
            break;
        case FB_X_SETPREMUL:
            premultiply_onoff = *(uint32_t *)arg;
            if (premultiply_onoff != 0)
            {
                HAL_VOUT_EnablePreMultiply(pVOUT, layer_index);
            }
            else
            {
                HAL_VOUT_DisablePreMultiply(pVOUT, layer_index);
            }
            break;
        default:
            break;
    }

    return 0;
}

static void vout_gm6721x_irq_line_enable(struct device *dev, uint32_t line_num)
{
    VOUT_Device_T *pVOUT = VOUT_STRUCT(dev);

    /* Disable the Line interrupt */
    HAL_VOUT_DisableInterrupts(pVOUT, VOUT_IT_LI);

    /* Set the Line Interrupt position */
    pVOUT->LIPCR = (uint32_t)line_num;

    HAL_VOUT_EnableInterrupts(pVOUT, VOUT_IT_LI);
}

static void vout_gm6721x_irq_line_disable(struct device *dev)
{
    VOUT_Device_T *pVOUT = VOUT_STRUCT(dev);

    /* Disable the Line interrupt */
    HAL_VOUT_DisableInterrupts(pVOUT, VOUT_IT_LI);
}


static void vout_gm6721x_irq_callback_set(struct device *dev,
        fb_irq_callback_t cb)
{
    struct vout_gm6721x_data *data = DEV_DATA(dev);

    data->user_cb = cb;
}


static void vout_gm6721x_isr(void *arg)
{
    struct device *dev = (struct device *)arg;
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    VOUT_Device_T *pVOUT = VOUT_STRUCT(dev);

    if (__HAL_VOUT_GET_IT_SOURCE(pVOUT, VOUT_IT_LI) && __HAL_VOUT_GET_FLAG(pVOUT, VOUT_FLAG_LI))
    {
        /* Clear the Line interrupt flag */
        __HAL_VOUT_CLEAR_FLAG(pVOUT, VOUT_FLAG_LI);

        if (data->user_cb)
        {
            data->user_cb(dev);
        }
    }
}

static const struct fb_driver_api vout_gm6721x_drv_api_funcs =
{
    .get_displayinfo = vout_gm6721x_get_displayinfo,
    .ioctl = vout_gm6721x_ioctl,
    .fill_rect  = vout_gm6721x_fill_rect,
    .copy_area  = vout_gm6721x_copy_area,
    .load_clut = vout_gm6721x_load_clut,
    .alpha_blendingbulk = vout_gm6721x_alpha_blendingbulk,
    .mix_colors = vout_gm6721x_mix_colors,
    .convert_color = vout_gm6721x_convert_color,
    .mix_colorsbulk = vout_gm6721x_mix_colorsbulk,
    .draw_bitmapl8 = vout_gm6721x_draw_bitmapl8,
    .scaler        = vout_gm6721x_scaler,
    .irq_line_enable = vout_gm6721x_irq_line_enable,
    .irq_line_disable = vout_gm6721x_irq_line_disable,
    .irq_callback_set = vout_gm6721x_irq_callback_set,
};

static inline int vout_gm6721x_set_clock(uint32_t pllsain, uint32_t pllsaiq, uint32_t pllsaidivr)
{
    Peripheral_Config_T peripheClockSet;

    /* VOUT clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_VOUT);
    HAL_Reset_Module(RESET_VOUT_SW_RSTJ);
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, ENABLE);

    /** GM6721:lcd pixel clock = 2*(pllsain+128)/pllsair/pllsaidivr **/
    /** F6721B:lcd pixel clock = 2*(pllsain)/pllsaiq/pllsaidiv, default n is 172 when set n as 0, out put is 8.6M**/
    HAL_CLOCK_AdjustPLLSAIDefaultDiv(0, pllsaiq, 16, pllsain);
    peripheClockSet.peripheMask = PERIPHERAL_VOUT_MASK;
    peripheClockSet.peripheClkConfig.voutClkConfig = pllsaidivr;
    HAL_CLOCK_PeripheralClkSetConfig(&peripheClockSet);

    /* IMP clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_IMP);
    HAL_Reset_Module(RESET_IMP_SW_RSTJ);

    return 0;

}

static int vout_gm6721x_init(struct device *dev)
{
    const struct vout_gm6721x_config *config = DEV_CFG(dev);
    struct vout_gm6721x_data *data = DEV_DATA(dev);
    VOUT_Device_T *pVOUT = VOUT_STRUCT(dev);

    /* mutex for mul task acess hardware*/
    k_mutex_init(&data->mutex);

    vout_gm6721x_set_clock(config->pllsain, config->pllsaiq, config->pllsaidivr);

    data->pConfig.HSPolarity = VOUT_HSPOLARITY_AL;
    data->pConfig.VSPolarity = VOUT_VSPOLARITY_AL;
    data->pConfig.DEPolarity = VOUT_DEPOLARITY_AL;
    data->pConfig.PCPolarity = VOUT_PCPOLARITY_IPC;
    data->pConfig.horizontalSync = config->hsw - 1;
    data->pConfig.verticalSync = config->vsw - 1;
    data->pConfig.accumulatedHBP = config->hsw + config->hbp - 1;
    data->pConfig.accumulatedVBP = config->vsw + config->vbp - 1;
    data->pConfig.accumulatedActiveW = config->hsw + config->hbp + config->pic_width - 1;
    data->pConfig.accumulatedActiveH = config->vsw + config->vbp + config->pic_height - 1;
    data->pConfig.totalWidth = config->hsw + config->hbp + config->pic_width + config->hfp - 1;
    data->pConfig.totalHeigh = config->vsw + config->vbp + config->pic_height + config->vfp - 1;

    data->pConfig.backColor.red = 0;
    data->pConfig.backColor.green = 0;
    data->pConfig.backColor.blue = 0;

    HAL_VOUT_Init(pVOUT, &data->pConfig);

    config->irq_config_func(dev);

    return 0;
}

static struct vout_gm6721x_data vout_gm6721x_dev_data =
{
    .pConfig = {
        .HSPolarity = VOUT_HSPOLARITY_AL,
        .VSPolarity = VOUT_VSPOLARITY_AL
    },
};

static void vout_gm6721x_config_func(struct device *dev);

static const struct vout_gm6721x_config vout_gm6721x_dev_cfg =
{
    .vout_base = CONFIG_VOUT_BASE_ADDRESS,
    .imp_base = CONFIG_IMP_BASE_ADDRESS,
    .pic_width = CONFIG_VOUT_DEFAULT_WIDTH,
    .pic_height = CONFIG_VOUT_DEFAULT_HEIGHT,
    .hsw = CONFIG_VOUT_DEFAULT_HSW,
    .vsw = CONFIG_VOUT_DEFAULT_VSW,
    .hbp = CONFIG_VOUT_DEFAULT_HBP,
    .vbp = CONFIG_VOUT_DEFAULT_VBP,
    .hfp = CONFIG_VOUT_DEFAULT_HFP,
    .vfp = CONFIG_VOUT_DEFAULT_VFP,
    .pllsain = CONFIG_VOUT_DEFAULT_PLLSAIN,
    .pllsaiq = CONFIG_VOUT_DEFAULT_PLLSAIQ,
    .pllsaidivr = CONFIG_VOUT_DEFAULT_PLLSAIDIVR,
    .irq_config_func = vout_gm6721x_config_func,
};

DEVICE_AND_API_INIT(display_gm6721x, CONFIG_VOUT_NAME,
                    vout_gm6721x_init,
                    &vout_gm6721x_dev_data, &vout_gm6721x_dev_cfg,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &vout_gm6721x_drv_api_funcs);

static void vout_gm6721x_config_func(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(VOUT_IRQ, CONFIG_VOUT_IRQ_PRI,
                vout_gm6721x_isr, DEVICE_GET(display_gm6721x), 0);

    irq_enable(VOUT_IRQ);
}

