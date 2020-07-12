/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lcd_demo.c
 *
 * @author        Christal.Xu
 *
 * @version       1.0.0
 *
 * @date          2019/04/20
 *
 * @brief         LCD Demo Calls LCD Driver
 *
 * @note
 *    2019/04/20, Christal.Xu, V1.0.0
 *        Initial version.
 */
#include <gm_hal_vout.h>
#include <gm_hal_gpio.h>
#include "font.h"

#ifdef RABBIT_NO_OS_SYSTEM
    #include <gm_hal_reset.h>
    #include <gm_hal_clock.h>
    #include <pinmux.h>
#endif

#ifndef RABBIT_NO_OS_SYSTEM
    #include <kernel.h>
#endif



/*some basic config options*/
#define BLACK           0xFF000000
#define WHITE           0xFFFFFFFF
#define RED             0xFFFF0000
#define GREEN           0xFF00FF00
#define BLUE            0xFF0000FF
#define BACK_COLOR      0x00000000


#define WORD_COLOR      0xFFFF0000
#define LOGO_BG_COLOR   0xFFFF0000
#define LAYER_COLOR     0xFFFFFF
#ifdef BPI
#define PIC_WIDTH       480
#define PIC_HEIGHT      266
#else
#define PIC_WIDTH       800
#define PIC_HEIGHT      480
#endif

uint32_t addr   = 0xc0a00000;

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

/*GREE logo size 32*32*/
#define LOGO_W 32
#define LOGO_H 32
#define LOGOSIZE (LOGO_W*LOGO_H*4)
const unsigned char logo[LOGOSIZE] =   /* 0X00,0X20,0X10,0X00,0X10,0X00,0X00,0X1B, */
{
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XA7, 0X6E, 0X1C, 0X00,
    0XC1, 0X79, 0X11, 0X00, 0XC1, 0X79, 0X11, 0X00, 0XC1, 0X79, 0X11, 0X00, 0XC1, 0X79, 0X11, 0X00,
    0XC1, 0X79, 0X11, 0X00, 0XC1, 0X79, 0X11, 0X00, 0XC1, 0X79, 0X11, 0X00, 0XC0, 0X78, 0X11, 0X00,
    0XA2, 0X6C, 0X1F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XBE, 0X78, 0X13, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XBB, 0X75, 0X13, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X10, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC1, 0X7A, 0X13, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC2, 0X78, 0X10, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X78, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X78, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X78, 0X0F, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC1, 0X78, 0X0F, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC1, 0X78, 0X0E, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC1, 0X77, 0X0E, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC4, 0X7D, 0X18, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X84, 0X5E, 0X2A, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X2E, 0X22, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC1, 0X7D, 0X1B, 0X00, 0XC2, 0X7D, 0X1A, 0X00,
    0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00,
    0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00,
    0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00,
    0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X7D, 0X1A, 0X00, 0XC4, 0X7C, 0X17, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XA8, 0X6D, 0X1A, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X9F, 0X6E, 0X27, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0X79, 0X58, 0X27, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XE5, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XA7, 0X6F, 0X21, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0XB3, 0X71, 0X13, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X19, 0X11, 0XD2, 0X00,
    0X2F, 0X24, 0X15, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X06, 0X05, 0X06, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X43, 0X33, 0X1D, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XBB, 0X76, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X0E, 0X0C, 0X09, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XCA, 0X8E, 0X3A, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X1D, 0X16, 0XD8, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X92, 0X66, 0X26, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0XBE, 0X76, 0X0F, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0X86, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XAE, 0X72, 0X1E, 0X00,
    0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X13, 0X0E, 0X3A, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X16, 0X12, 0XBB, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0XC2, 0X7D, 0X1A, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00, 0XC2, 0X79, 0X11, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X0A, 0X07, 0XF6, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0XBE, 0X7A, 0X19, 0X00, 0XC2, 0X79, 0X11, 0X00, 0X7A, 0X57, 0X25, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X0D, 0X0A, 0XF0, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XA9, 0X6C, 0X15, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X1F, 0X17, 0XCE, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XC7, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XD7, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X13, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00, 0X00, 0X00, 0XFF, 0X00,
    0X00, 0X00, 0XFF, 0X00, 0X05, 0X03, 0XF2, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X24, 0X1A, 0XB4, 0X00, 0X00, 0X00, 0XFA, 0X00, 0X01, 0X01, 0XFC, 0X00, 0X00, 0X00, 0XFE, 0X00,
    0X00, 0X00, 0XFE, 0X00, 0X01, 0X01, 0XFC, 0X00, 0X00, 0X00, 0XFA, 0X00, 0X26, 0X1D, 0XAA, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,

};

static void LCD_draw_point(uint16_t dx, uint16_t dy, uint32_t color)
{
    *(uint32_t *)((uint32_t)addr + ((PIC_WIDTH * dy + dx) << 2)) = color;
}

void LCD_drawlogo(int x, int y, unsigned int color)
{
    int j = 0;
    int i = 0;
    int index = 0;
    int w = 32;
    int h = 32;
    unsigned int *addr = (unsigned int *)logo;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            LCD_draw_point(x + i, y + j, addr[index]);
            index += 1;
        }
    }
}

static void LCD_showchar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode, uint32_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    num = num - ' ';
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
        {
            temp = asc2_1206[num][t];
        }
        else if (size == 16)
        {
            temp = asc2_1608[num][t];
        }
        else if (size == 24)
        {
            temp = asc2_2412[num][t];
        }
        else if (size == 32)
        {
            temp = asc2_3216[num][t];
        }
        else
        {
            return;
        }
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                LCD_draw_point(x, y, color);
            }
            else if (mode == 0)
            {
                LCD_draw_point(x, y, BACK_COLOR);
            }
            temp <<= 1;
            y++;
            if (y >= PIC_HEIGHT)
            {
                return;
            }
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= PIC_WIDTH)
                {
                    return;
                }
                break;
            }
        }
    }
}

void LCD_showstring(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint32_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' '))
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)
        {
            break;
        }
        LCD_showchar(x, y, *p, size, 1, color);
        x += size / 2;
        p++;
    }
}

#ifdef RABBIT_NO_OS_SYSTEM
int vout_set_clock()
{
    Peripheral_Config_T peripheClockSet;
    /* VOUT clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_VOUT);
    HAL_Reset_Module(RESET_VOUT_SW_RSTJ);
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, ENABLE);

    /** lcd pixel clock = 2*(pllsain+128)/pllsair/pllsaidivr **/
    //HAL_CLOCK_PeripheralClkSetConfig();
    HAL_CLOCK_AdjustPLLSAIDefaultDiv(0, 8, 16, 0);
    peripheClockSet.peripheMask = PERIPHERAL_VOUT_MASK;
    peripheClockSet.peripheClkConfig.voutClkConfig = CLOCK_VOUT_DIV_5;
    HAL_CLOCK_PeripheralClkSetConfig(&peripheClockSet);


    /* IMP clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_IMP);
    HAL_CLOCK_PeripheralClockEnable0(RESET_IMP_SW_RSTJ);

    return 0;
}

static int vout_init()
{
    VOUT_Config_T data1;

    vout_set_clock();

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
    data1.backColor.red = 0;
    data1.backColor.green = 0;
    data1.backColor.blue = 0;

    HAL_VOUT_Init(VOUT, &data1);

    return 0;
}
#endif

void init_layer(uint8_t layer_index, struct fb_layerinfo *layerinfo)
{
    VOUT_LayerConfig_T pLayerCfg;

    pLayerCfg.windowX0 = 0;
    pLayerCfg.windowY0 = 0;
    pLayerCfg.windowX1 = PIC_WIDTH;
    pLayerCfg.windowY1 = PIC_HEIGHT;
    pLayerCfg.pixelFormat = layerinfo->pixformat;
    pLayerCfg.alpha = layerinfo->alpha;
    pLayerCfg.alpha0 = layerinfo->alpha0;
    pLayerCfg.FBStartAdress = layerinfo->fb_start;
    pLayerCfg.imageWidth = PIC_WIDTH;
    pLayerCfg.imageHeight = PIC_HEIGHT;
    pLayerCfg.backColor.red = (uint8_t)((layerinfo->color & 0X00FF0000) >> 16);
    pLayerCfg.backColor.green = (uint8_t)((layerinfo->color & 0X0000FF00) >> 8);
    pLayerCfg.backColor.blue = (uint8_t)(layerinfo->color & 0X000000FF);

    HAL_VOUT_ConfigLayer(VOUT, &pLayerCfg, layer_index);
}

void set_window_size(uint8_t layer_index, void *arg)
{
    struct fb_wininfo *wininfo;
    wininfo = (struct fb_wininfo *)arg;
    HAL_VOUT_SetWindowSize(VOUT, wininfo->width, wininfo->height, layer_index);
}

void set_pos(uint8_t layer_index, void *arg)
{
    struct fb_wininfo *wininfo;
    wininfo = (struct fb_wininfo *)arg;
    HAL_VOUT_SetWindowPosition(VOUT, wininfo->x_start, wininfo->y_start, layer_index);
}

void set_address(uint8_t layer_index, void *arg)
{
    uint32_t fb_start;
    fb_start = *(uint32_t *)arg;
    HAL_VOUT_SetAddress(VOUT, fb_start, layer_index);
}

void Creat_pic_bg(uint16_t x, uint16_t y, uint32_t addr, uint32_t color)
{
    uint16_t i, j;
    for (i = 0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            *(volatile uint32_t *)addr = color;
            addr += 4;
        }
    }
}

void lcd_config(void)
{
    struct fb_displayinfo info;
    struct fb_layerinfo layerinfo;
    struct fb_wininfo wininfo;

    uint8_t pin_num;

    /* Set LED pin as output */
    GPIO_PinConfig_T pConfig;
    pConfig.pin = 1 << LCD_BL_PIN;
    pConfig.mode = GPIO_MODE_OUTPUT;
    pConfig.pull = GPIO_NOPULL;
    pConfig.alternate = 0;

    pin_num = LCD_BL_PIN + 16;//set gpiob pin number
#ifdef RABBIT_NO_OS_SYSTEM
    LCD_ConfigPinmux();
    vout_init();
#endif
    HAL_GPIO_Init(GPIOB, &pConfig);
    HAL_GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_SET);

    info.pic_width = PIC_WIDTH;
    info.pic_height = PIC_HEIGHT;

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_ARGB8888;//pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = LAYER_COLOR;
    layerinfo.fb_start = addr;

    init_layer(0, &layerinfo);

    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    set_window_size(0, &wininfo);
    set_pos(0, &wininfo);
}


//#include <ugelis.h>
#include <ts.h>

int ts_main(void)
{
    struct device *touch = device_get_binding(GT911_DEV_NAME);
    int ret = 0;
    static uint32_t cnt = 0;
    struct ts_input_event event;
    printk("\n###########Touch Screen DEMO################\n");
    while (1)
    {
        ret = read_ts_event(&event, K_FOREVER);
        if (!ret)
        {
            if (event.type == ABS_XY)
            {
                printk("\n###########Touch Screen Action:%d################\n", ++cnt);
                if (event.press_status == TS_PRESS)
                {
                    /*ts press*/
                    printk("ts prss: x_abs:%d y_abs:%d\n", event.x_abs, event.y_abs);
                }
                else
                {
                    /*ts release*/
                    printk("ts release\n");
		    break;
                }
            }
        }
    }

    return 0;
}



int main(void)
{
    struct device *touch = device_get_binding(GT911_DEV_NAME);
    int ret = 0;
    static uint32_t cnt = 0;
    struct ts_input_event event;

    char dist_str[16];
    uint8_t x = 0;
    printk("\n###########Touch Screen DEMO################\n");
    sprintf(dist_str, "ZhuHai Edgeless");
    printk("ZhuHai Edgeless - Banana Pi \n");
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
#endif
    lcd_config();
    while (1)
    {
        switch (x)
        {
            case 0:
                Creat_pic_bg(PIC_WIDTH, PIC_HEIGHT, addr, WHITE);
                break;
            case 1:
                Creat_pic_bg(PIC_WIDTH, PIC_HEIGHT, addr, RED);
                break;
            case 2:
                Creat_pic_bg(PIC_WIDTH, PIC_HEIGHT, addr, GREEN);
                break;
            case 3:
                Creat_pic_bg(PIC_WIDTH, PIC_HEIGHT, addr, BLUE);
                break;
            case 4:
                Creat_pic_bg(PIC_WIDTH, PIC_HEIGHT, addr, BLACK);
                break;
            case 100:
                ts_main();
                break;				
        }
        LCD_drawlogo(PIC_WIDTH - LOGO_W - 1, 1, LOGO_BG_COLOR);
        LCD_showstring(10, 40, 800, 32, 32, "GREE ELECTRIC APPLIANCES - Banana Pi", WORD_COLOR);
        LCD_showstring(10, 80, 240, 24, 24, dist_str, WORD_COLOR);
        LCD_showstring(10, 110, 240, 16, 16, "BPI: VOUT LCD TEST", WORD_COLOR);
        //LCD_showstring(10, 130, 240, 12, 12, "2019/4/30", WORD_COLOR);
        LCD_showstring(10, 130, 240, 12, 12, "2020/07/08", WORD_COLOR);
        LCD_showstring(10, 200, 600, 32, 32, "Banana Pi BPI-EAI80", WORD_COLOR);
	//ts_main();
        while (1) {
        ret = read_ts_event(&event, K_FOREVER);
        if (!ret) {
            if (event.type == ABS_XY) {
                printk("\n###########Touch Screen Action:%d################\n", ++cnt);
                if (event.press_status == TS_PRESS) {
                    /*ts press*/
                    printk("ts prss: x_abs:%d y_abs:%d\n", event.x_abs, event.y_abs);
                }
                else {
                    /*ts release*/
                    printk("ts release\n");
		    break;
                }
            }
        }
        }
        x++;
        if (x == 5)
        {
            x = 0;
        }
#ifdef RABBIT_NO_OS_SYSTEM
        HAL_DelayMs(1000);
#endif
#ifndef RABBIT_NO_OS_SYSTEM
        k_sleep(1000);
#endif
    }
}

