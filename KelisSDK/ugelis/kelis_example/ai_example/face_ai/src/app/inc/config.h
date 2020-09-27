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
//#include <display/fb_display.h>

#if CONFIG_SDRAM_SIZE <= 8192
    #define MEM_LITE_VERSION
#endif

/***************************************************
 *
 * FaceDetect Parameters
 *
 ***************************************************/
#define OUT_BUF_W               160
#define OUT_BUF_H               120
#define AI_BUF_W                96
#define AI_BUF_H                96
/***************************************************
 *
 * Display Parameters
 *
 ***************************************************/
#define DEV_LCD_W               (CONFIG_VOUT_DEFAULT_WIDTH)
#define DEV_LCD_H               (CONFIG_VOUT_DEFAULT_HEIGHT)
#define DEV_LCD_X               (CONFIG_VOUT_DEFAULT_WIDTH  - DEV_LCD_W)
#define DEV_LCD_Y               (CONFIG_VOUT_DEFAULT_HEIGHT - DEV_LCD_H)

/***************************************************
 *
 * Camera Parameters
 *
 ***************************************************/
#ifdef MEM_LITE_VERSION
    #define CAM_BUF_W          OUT_BUF_W
    #define CAM_BUF_H          OUT_BUF_H
#else
    #if (CONFIG_VOUT_DEFAULT_HEIGHT > 360)
        /* rgb565 to l8, an issue here if width >= 484 */
        #define CAM_BUF_W          480//DEV_LCD_W
        #define CAM_BUF_H          360//DEV_LCD_H
    #else
        #define CAM_BUF_W          480//DEV_LCD_W
        #define CAM_BUF_H          272//DEV_LCD_H
    #endif

#endif

/***************************************************
 *
 * Memory Parameters
 *
 ***************************************************/
#define MEM_RESERVE_START   (CONFIG_SDRAM_BASE_ADDRESS + (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024)

/***************************************************
 *
 * Flash Parameters
 *
 ***************************************************/
#define OPS_BIN_FLASH_SIZE      (0x5a49c0)
#define BASE_BIN_FLASH_SIZE     (0x25680)

#define AI_FLASH_START_OFFSET   (0x00110000)
#define OPS_BIN_FLASH_START     (AI_FLASH_START_OFFSET)
#define BASE_BIN_FLASH_START    (OPS_BIN_FLASH_START+0x5B0000)

#endif //__CONFIG_H__
