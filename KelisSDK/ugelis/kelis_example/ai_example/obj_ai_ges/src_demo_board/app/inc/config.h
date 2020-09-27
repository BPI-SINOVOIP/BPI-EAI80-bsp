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


#if CONFIG_SDRAM_SIZE > 8192
    #define DIAMOND_VERSION
#endif

#define INPUT_WIDTH    224
#define INPUT_HEIGHT   224


/***************************************************
 *
 * FaceDetect Parameters
 *
 ***************************************************/
#define FD_BUF_W            160
#define FD_BUF_H            120
/***************************************************
 *
 * Display Parameters
 *
 ***************************************************/
//LCD Total Pixels
#define DEV_LCD_W           480
#define DEV_LCD_H           272

/***************************************************
 *
 * Camera Parameters
 *
 ***************************************************/

#ifdef DIAMOND_VERSION
    #define CAM_CROP_W          480
    #define CAM_CROP_H          272
#else
    #define CAM_CROP_W          FD_BUF_W
    #define CAM_CROP_H          FD_BUF_H
#endif

#define CAM_BUF_PIXEL_SIZE      3   //FMT_RGB888

#define LCD_UI_W                DEV_LCD_W
#define LCD_UI_H                DEV_LCD_H

#define LCD_BUF_PIXEL_SIZE      4   //FMT_ARGB8888
#define MEM_UI_SIZE             (LCD_UI_W*LCD_UI_H*LCD_BUF_PIXEL_SIZE)

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


//#define IMAGE_BIN_FLASH_START (OPS_BIN_FLASH_START+OPS_BIN_SIZE)


#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE        (5*SIZE_1M)

#define OPS_BIN_FLASH_SIZE      (0x500000)
#define BASE_BIN_FLASH_SIZE     (224*224*3+2)

#define AI_FLASH_START_OFFSET   (0x00050000)
#define OPS_BIN_FLASH_START     (AI_FLASH_START_OFFSET)
#define OPS_BIN_SIZE            (5*SIZE_1M)
#define BASE_BIN_FLASH_START    (OPS_BIN_FLASH_START+OPS_BIN_SIZE)



#ifndef __USE_KEIL_
    #define VIDEO_SECTION __in_section_unique(mem_video_data) __aligned(32)
#else
    #define VIDEO_SECTION __attribute__((section(".mem_video_data"),zero_init)) __aligned(32)
#endif

#endif //__CONFIG_H__
