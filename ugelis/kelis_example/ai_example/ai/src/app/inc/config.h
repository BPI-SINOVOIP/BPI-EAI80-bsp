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

enum
{
    APP_FACE = 0,
    APP_BODY,
    APP_GEST,
    APP_SPEECH,
    APP_END,
};

#ifdef F6721B_EVB_QFN80PIN
    #define START_APP       APP_BODY
    #define WEI_BIN_SIZE    (0x47CFA0)
#else
    #define START_APP       APP_FACE
    #define WEI_BIN_SIZE    (0x5A49C0)
    #define FACEAI          1
#endif

//#define DEBUG_CAP

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
 * ObjectRecognize Parameters
 *
 ***************************************************/
#define OUT_BUF_W               160
#define OUT_BUF_H               120
#define OBJ_AI_BUF_W            224
#define OBJ_AI_BUF_H            224

/***************************************************
 *
 * Display Parameters
 *
 ***************************************************/
#define UI_FOCUS_COLOR           0xFFFF0000
#define UI_FOCUS_COLOR2          0xFF0000FF

/***************************************************
 *
 * Camera Parameters
 *
 ***************************************************/
#define DEV_LCD_W               (CONFIG_VOUT_DEFAULT_WIDTH)
#define DEV_LCD_H               (CONFIG_VOUT_DEFAULT_HEIGHT)

#if (CONFIG_VOUT_DEFAULT_HEIGHT > 360)
    /* rgb565 to l8, an issue here if width >= 484 */
    #define CAM_DISP_W          (640)
    #define CAM_DISP_H          (480)
    #define CAM_DISP_X          (CONFIG_VOUT_DEFAULT_WIDTH  - CAM_DISP_W)
    #define CAM_DISP_Y          (CONFIG_VOUT_DEFAULT_HEIGHT - CAM_DISP_H)
    #define CAM_BUF_W           320
    #define CAM_BUF_H           240
    #define DESIGN_W            2
    #define UI_BKGRD_COLOR      0xFF5F90B0
#else
    #define CAM_DISP_W          (362)
    #define CAM_DISP_H          (272)
    #define CAM_DISP_X          (CONFIG_VOUT_DEFAULT_WIDTH  - CAM_DISP_W)
    #define CAM_DISP_Y          (CONFIG_VOUT_DEFAULT_HEIGHT - CAM_DISP_H)
    #define CAM_BUF_W           362
    #define CAM_BUF_H           272
    #define DESIGN_W            1
    #define UI_BKGRD_COLOR      0xFF5080A0
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
/* binary offset in flash */
#define FACE_OPS_FLASH_OFFSET     (0x0210000)
#define OBJ_OPS_FLASH_OFFSET      (0x07c0000)
#define GES_OPS_FLASH_OFFSET      (0x0c40000)
#define FACE_BASE_FLASH_OFFSET    (0x10c0000)

#define FACE_OPS_SIZE             (0x5a49c0)
#define OBJ_OPS_SIZE              (0x45fee0)
#define GES_OPS_SIZE              (0x47cf98)
#define FACE_BASE_SIZE            (0x80000)

#endif //__CONFIG_H__
