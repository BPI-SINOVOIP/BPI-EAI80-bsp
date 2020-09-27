/*
 * Copyright (c) 2016 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Header file for the GM6721X VIN driver.
 */

#ifndef __VIN_GM6721X_H__
#define __VIN_GM6721X_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_hal_vin.h>
#include "vb_queue.h"

#define MAX_FRAME_SIZE  4

enum vb_state {
    VB_MEMORY_FREE,
    VB_MEMORY_BUSY,
};

struct vb_info {
    uint32_t fb_start;
    uint32_t buf_size;
	uint32_t timestamp;
    enum vb_state state;
};

struct vb_manager
{
    uint8_t  active_idx;
    uint8_t  frm_number;
    struct vb_info frm_info[MAX_FRAME_SIZE];
};

/** Configuration data */
struct vin_gm6721x_config {
    uint32_t base;
    uint8_t  pclk_polarity;			
	uint8_t  hsync_polarity;			
	uint8_t  vsync_polarity;			
    void (*irq_config_func)(struct device *dev);
};

/** Runtime driver data */
struct vin_gm6721x_data {
	/* DCMI peripheral handler */
	VIN_Config_T  pConfig;
    struct dcmi_queue q;
    struct vb_manager vin_frm_mgr;
    struct k_mutex mutex;
    
    uint32_t cur_line;
    bool     csc_en;
    uint16_t width;
    uint16_t height;
    enum dcmi_pixel_format pix_fmt;
    uint8_t  bpp;
    dcmi_frame_irq_callback_t user_cb;
};

#ifdef __cplusplus
}
#endif

#endif /* __VIN_GM6721X_H__ */
