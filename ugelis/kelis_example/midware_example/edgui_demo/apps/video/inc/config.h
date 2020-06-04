/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          config.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Macros for device init.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "inc/common.h"
#include <camera/camera_sensor.h>

/***************************************************
 *
 * Display Parameters
 *
 ***************************************************/
//LCD Total Pixels
#define DEV_LCD_W           800
#define DEV_LCD_H           480

/***************************************************
 *
 * Camera Parameters
 *
 ***************************************************/

#define CAM_CROP_W          320
#define CAM_CROP_H          240

#define CAM_BUF_PIXEL_SIZE      3   //FMT_RGB888

/***************************************************
 *
 * Memory Parameters
 *
 ***************************************************/
#define MEM_RESERVE_START   (CONFIG_SDRAM_BASE_ADDRESS + (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024)

#ifndef __USE_KEIL_
    #define VIDEO_SECTION __in_section_unique(mem_video_data) __aligned(32)
#else
    #define VIDEO_SECTION __attribute__((section(".mem_video_data"),zero_init)) __aligned(32)
#endif

#endif //__CONFIG_H__
