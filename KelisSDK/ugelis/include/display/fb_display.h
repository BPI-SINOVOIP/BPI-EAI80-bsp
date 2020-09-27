/** @file
 *  @brief LCD display APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __FB_DISPLAY_H
#define __FB_DISPLAY_H

/**
 * @brief LCD display APIs
 * @defgroup fb_display LCD display APIs
 * @{
 */

#include <misc/__assert.h>
#include <misc/slist.h>

#include <stdint.h>
#include <stddef.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

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

struct fb_fillrect
{
    uint32_t xsize;          /* pixel number of a line */
    uint32_t ysize;          /* line number */
    uint32_t pixfmt;         /* pixel format */
    uint32_t offline;        /* destination buffer offset address */
    uint32_t dst_addr;       /* destination buffer address */
    uint32_t color;          /* color value */
};

struct fb_copyarea
{
    uint32_t xsize;          /* pixel number of a line */
    uint32_t ysize;          /* line number */
    uint32_t pixfmt;         /* pixel format */
    uint32_t offline_src;    /* source buffer offset address */
    uint32_t offline_dst;    /* destination buffer offset address */
    uint32_t src_addr;       /* source buffer address */
    uint32_t dst_addr;       /* destination buffer address */
};

struct fb_mixcolorbulk
{
    uint32_t fg_addr;       /* foreground address */
    uint32_t bg_addr;       /* background address */
    uint32_t dst_addr;      /* destination address */
    uint32_t fg_pixfmt;     /* foreground pixel format */
    uint32_t bg_pixfmt;     /* background pixel format */
    uint32_t dst_pixfmt;    /* destination pixel format */
    uint32_t xsize;         /* pixel width */
    uint32_t ysize;         /* pixel height */
    uint32_t alpha;         /* blending alpha value */
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

struct fb_clutinfo
{
    uint32_t *clut_addr;
    uint32_t clut_size;
    bool enable;
};

struct fb_displayinfo
{
    uint32_t pic_width;
    uint32_t pic_height;
    uint32_t pixclock;
};

struct fb_scalerinfo
{
    uint32_t src;
    uint32_t dst;
    uint32_t src_w;
    uint32_t src_h;
    uint32_t dst_w;
    uint32_t dst_h;
    uint32_t src_fmt;
    uint32_t dst_fmt;
};

/**
 * @typedef fb_irq_callback_t
 * @brief Define the application callback function signature for LTDC.
 *
 * @param port Device struct for the LTDC device.
 */
typedef void (*fb_irq_callback_t)(struct device *port);


/** @brief DISPLAY driver API definition. */
struct fb_driver_api
{
    int (*get_displayinfo)(struct device *dev, struct fb_displayinfo *info);

    int (*ioctl)(struct device *dev, uint8_t layer_index, uint32_t cmd, void *arg);

    int (*fill_rect)(struct device *dev, const struct fb_fillrect *rect);

    int (*copy_area)(struct device *dev, const struct fb_copyarea *region);

    int (*load_clut)(struct device *dev, uint32_t *color, uint32_t clut_size);

    int (*alpha_blendingbulk)(struct device *dev, uint32_t *fg_addr, uint32_t *bg_addr, uint32_t *dst, uint32_t pixel_num);

    int (*mix_colors)(struct device *dev, uint32_t fg_color, uint32_t bg_color, uint8_t alpha, uint32_t *output_color);

    int (*convert_color)(struct device *dev, void *src, void *dst, uint32_t src_pixfmt, uint32_t dst_pixfmt, uint32_t xsize, uint32_t ysize);

    int (*mix_colorsbulk)(struct device *dev, const struct fb_mixcolorbulk *mixcolorbulk);

    int (*draw_bitmapl8)(struct device *dev, void *src, void *dst, uint32_t offline_src, uint32_t offline_dst, uint32_t pixfmt, uint32_t xsize, uint32_t ysize);

    int (*scaler)(struct device *dev, const struct fb_scalerinfo *param);

    /** Line interrupt enable function */
    void (*irq_line_enable)(struct device *dev, uint32_t line_num);

    /** Line interrupt disable function */
    void (*irq_line_disable)(struct device *dev);

    /** Set the callback function */
    void (*irq_callback_set)(struct device *dev, fb_irq_callback_t cb);

};

/**
 * @brief Getting display information.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param info.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int fb_get_displayinfo(struct device *dev, struct fb_displayinfo *info);

static inline int _impl_fb_get_displayinfo(struct device *dev, struct fb_displayinfo *info)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->get_displayinfo)
    {
        return -ENOTSUP;
    }

    return api->get_displayinfo(dev, info);
}

/**
 * @brief FB io command operation.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param layer_index  display layer index.
 * @param cmd  io command.
 * @param arg  .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int fb_ioctl(struct device *dev, uint8_t layer_index, uint32_t cmd, void *arg);

static inline int _impl_fb_ioctl(struct device *dev, uint8_t layer_index, uint32_t cmd, void *arg)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->ioctl)
    {
        return -ENOTSUP;
    }

    return api->ioctl(dev, layer_index, cmd, arg);
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
__syscall int fb_fill_rect(struct device *dev, const struct fb_fillrect *rect);

static inline int _impl_fb_fill_rect(struct device *dev, const struct fb_fillrect *rect)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->fill_rect)
    {
        return -ENOTSUP;
    }

    return api->fill_rect(dev, rect);
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
__syscall int fb_copy_area(struct device *dev, const struct fb_copyarea *region);

static inline int _impl_fb_copy_area(struct device *dev, const struct fb_copyarea *region)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->copy_area)
    {
        return -ENOTSUP;
    }

    return api->copy_area(dev, region);
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
__syscall int fb_load_clut(struct device *dev, uint32_t *color, uint32_t clut_size);

static inline int _impl_fb_load_clut(struct device *dev, uint32_t *color, uint32_t clut_size)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->load_clut)
    {
        return -ENOTSUP;
    }

    return api->load_clut(dev, color, clut_size);
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
__syscall int fb_alpha_blendingbulk(struct device *dev, uint32_t *fg_addr, uint32_t *bg_addr, uint32_t *dst, uint32_t pixel_num);

static inline int _impl_fb_alpha_blendingbulk(struct device *dev, uint32_t *fg_addr, uint32_t *bg_addr, uint32_t *dst, uint32_t pixel_num)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->alpha_blendingbulk)
    {
        return -ENOTSUP;
    }

    return api->alpha_blendingbulk(dev, fg_addr, bg_addr, dst, pixel_num);
}

/**
 * @brief Alpha blending foreground and background color.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fg_color       foreground color.
 * @param bg_color       background color.
 * @param alpha          alpha value.
 * @param output_color   color after alpha blending.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int fb_mix_colors(struct device *dev, uint32_t fg_color, uint32_t bg_color, uint8_t alpha, uint32_t *output_color);

static inline int _impl_fb_mix_colors(struct device *dev, uint32_t fg_color, uint32_t bg_color, uint8_t alpha, uint32_t *output_color)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->mix_colors)
    {
        return -ENOTSUP;
    }

    return api->mix_colors(dev, fg_color, bg_color, alpha, output_color);
}

/**
 * @brief Pixel format conversion.
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
__syscall int fb_convert_color(struct device *dev, void *src, void *dst, uint32_t src_pixfmt, uint32_t dst_pixfmt, uint32_t xsize, uint32_t ysize);

static inline int _impl_fb_convert_color(struct device *dev, void *src, void *dst, uint32_t src_pixfmt, uint32_t dst_pixfmt, uint32_t xsize, uint32_t ysize)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->convert_color)
    {
        return -ENOTSUP;
    }

    return api->convert_color(dev, src, dst, src_pixfmt, dst_pixfmt, xsize, ysize);
}

/**
 * @brief Alpha blending foreground and background.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fg_color  foreground memory address.
 * @param bg_color  background memory address.
 * @param dst       destination buffer address.
 * @param alpha     alpha value.
 * @param pixel_num pixel number of a line.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int fb_mix_colorsbulk(struct device *dev, const struct fb_mixcolorbulk *mixcolorbulk);

static inline int _impl_fb_mix_colorsbulk(struct device *dev, const struct fb_mixcolorbulk *mixcolorbulk)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->mix_colorsbulk)
    {
        return -ENOTSUP;
    }

    return api->mix_colorsbulk(dev, mixcolorbulk);
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
__syscall int fb_draw_bitmapl8(struct device *dev, void *src, void *dst, uint32_t offline_src, uint32_t offline_dst, uint32_t pixfmt, uint32_t xsize, uint32_t ysize);

static inline int _impl_fb_draw_bitmapl8(struct device *dev, void *src, void *dst, uint32_t offline_src, uint32_t offline_dst, uint32_t pixfmt, uint32_t xsize, uint32_t ysize)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->draw_bitmapl8)
    {
        return -ENOTSUP;
    }

    return api->draw_bitmapl8(dev, src, dst, offline_src, offline_dst, pixfmt, xsize, ysize);
}

/**
 * @brief Scaler image
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fb_scalerinfo the address, rect of source image and dest image.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int fb_scaler(struct device *dev, const struct fb_scalerinfo *param);

static inline int _impl_fb_scaler(struct device *dev, const struct fb_scalerinfo *param)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (!api->scaler)
    {
        return -ENOTSUP;
    }

    return api->scaler(dev, param);
}

/**
 * @brief Enable line interrupt.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
static inline void fb_irq_line_enable(struct device *dev, uint32_t line_num)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (api->irq_line_enable)
    {
        api->irq_line_enable(dev, line_num);
    }
}

/**
 * @brief Disable line interrupt.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 1 If an IRQ is ready.
 * @retval 0 Otherwise.
 */
static inline void fb_irq_line_disable(struct device *dev)
{
    const struct fb_driver_api *api = dev->driver_api;

    if (api->irq_line_disable)
    {
        api->irq_line_disable(dev);
    }
}

/**
 * @brief Set the IRQ callback function pointer.
 *
 * This sets up the callback for IRQ. When an IRQ is triggered,
 * the specified function will be called.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cb Pointer to the callback function.
 *
 * @return N/A
 */
static inline void fb_irq_callback_set(struct device *dev,
                                       fb_irq_callback_t cb)
{
    const struct fb_driver_api *api = dev->driver_api;

    if ((api != NULL) && (api->irq_callback_set != NULL))
    {
        api->irq_callback_set(dev, cb);
    }
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#include <syscalls/fb_display.h>

#endif /* __FB_DISPLAY_H */
