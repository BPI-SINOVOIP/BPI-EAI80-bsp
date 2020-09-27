/*
 * Copyright (c) 2016 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Header file for the GM6721X DISPLAY driver.
 */

#ifndef __LCD_GM6721X_H__
#define __LCD_GM6721X_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_hal_vout.h>

/** Configuration data */
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
    uint32_t pllsaiq;
    uint32_t pllsaidivr;
    void (*irq_config_func)(struct device *dev);
};

/** Runtime driver data */
struct vout_gm6721x_data
{
    VOUT_Config_T pConfig;
    struct k_mutex mutex;

    fb_irq_callback_t user_cb;
};

#ifdef __cplusplus
}
#endif

#endif /* __PWM_STM32_H__ */
