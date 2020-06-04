/**
 * @file edgui_color.h
 *
 */

#ifndef EDGUI_COLOR_H
#define EDGUI_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

/*Error checking*/
#if EDGUI_COLOR_DEPTH == 24
#error "EDGUI_COLOR_DEPTH  24 is deprecated. Use EDGUI_COLOR_DEPTH  32 instead (edgui_conf.h)"
#endif

#if EDGUI_COLOR_DEPTH != 32 && EDGUI_COLOR_SCREEN_TRANSP != 0
#error "EDGUI_COLOR_SCREEN_TRANSP requires EDGUI_COLOR_DEPTH == 32. Set it in edgui_conf.h"
#endif

#if EDGUI_COLOR_DEPTH != 16 && EDGUI_COLOR_16_SWAP != 0
#error "EDGUI_COLOR_16_SWAP requires EDGUI_COLOR_DEPTH == 16. Set it in edgui_conf.h"
#endif

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define EDGUI_COLOR_WHITE EDGUI_COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define EDGUI_COLOR_SILVER EDGUI_COLOR_MAKE(0xC0, 0xC0, 0xC0)
#define EDGUI_COLOR_GRAY EDGUI_COLOR_MAKE(0x80, 0x80, 0x80)
#define EDGUI_COLOR_BLACK EDGUI_COLOR_MAKE(0x00, 0x00, 0x00)
#define EDGUI_COLOR_RED EDGUI_COLOR_MAKE(0xFF, 0x00, 0x00)
#define EDGUI_COLOR_MAROON EDGUI_COLOR_MAKE(0x80, 0x00, 0x00)
#define EDGUI_COLOR_YELLOW EDGUI_COLOR_MAKE(0xFF, 0xFF, 0x00)
#define EDGUI_COLOR_OLIVE EDGUI_COLOR_MAKE(0x80, 0x80, 0x00)
#define EDGUI_COLOR_LIME EDGUI_COLOR_MAKE(0x00, 0xFF, 0x00)
#define EDGUI_COLOR_GREEN EDGUI_COLOR_MAKE(0x00, 0x80, 0x00)
#define EDGUI_COLOR_CYAN EDGUI_COLOR_MAKE(0x00, 0xFF, 0xFF)
#define EDGUI_COLOR_AQUA EDGUI_COLOR_CYAN
#define EDGUI_COLOR_TEAL EDGUI_COLOR_MAKE(0x00, 0x80, 0x80)
#define EDGUI_COLOR_BLUE EDGUI_COLOR_MAKE(0x00, 0x00, 0xFF)
#define EDGUI_COLOR_NAVY EDGUI_COLOR_MAKE(0x00, 0x00, 0x80)
#define EDGUI_COLOR_MAGENTA EDGUI_COLOR_MAKE(0xFF, 0x00, 0xFF)
#define EDGUI_COLOR_PURPLE EDGUI_COLOR_MAKE(0x80, 0x00, 0x80)
#define EDGUI_COLOR_ORANGE EDGUI_COLOR_MAKE(0xFF, 0xA5, 0x00)

/**
 * Opacity percentages.
 */
enum {
    EDGUI_OPA_TRANSP = 0,
    EDGUI_OPA_0      = 0,
    EDGUI_OPA_10     = 25,
    EDGUI_OPA_20     = 51,
    EDGUI_OPA_30     = 76,
    EDGUI_OPA_40     = 102,
    EDGUI_OPA_50     = 127,
    EDGUI_OPA_60     = 153,
    EDGUI_OPA_70     = 178,
    EDGUI_OPA_80     = 204,
    EDGUI_OPA_90     = 229,
    EDGUI_OPA_100    = 255,
    EDGUI_OPA_COVER  = 255,
};

#define EDGUI_OPA_MIN 16  /*Opacities below this will be transparent*/
#define EDGUI_OPA_MAX 251 /*Opacities above this will fully cover*/

#if EDGUI_COLOR_DEPTH == 1
#define EDGUI_COLOR_SIZE 8
#elif EDGUI_COLOR_DEPTH == 8
#define EDGUI_COLOR_SIZE 8
#elif EDGUI_COLOR_DEPTH == 16
#define EDGUI_COLOR_SIZE 16
#elif EDGUI_COLOR_DEPTH == 32
#define EDGUI_COLOR_SIZE 32
#else
#error "Invalid EDGUI_COLOR_DEPTH in edgui_conf.h! Set it to 1, 8, 16 or 32!"
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef union
{
    uint8_t blue : 1;
    uint8_t green : 1;
    uint8_t red : 1;
    uint8_t full : 1;
} edgui_color1_t;

typedef union
{
    struct
    {
        uint8_t blue : 2;
        uint8_t green : 3;
        uint8_t red : 3;
    } ch;
    uint8_t full;
} edgui_color8_t;

typedef union
{
    struct
    {
#if EDGUI_COLOR_16_SWAP == 0
        uint16_t blue : 5;
        uint16_t green : 6;
        uint16_t red : 5;
#else
        uint16_t green_h : 3;
        uint16_t red : 5;
        uint16_t blue : 5;
        uint16_t green_l : 3;
#endif
    } ch;
    uint16_t full;
} edgui_color16_t;

typedef union
{
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t full;
} edgui_color32_t;

#if EDGUI_COLOR_DEPTH == 1
typedef uint8_t edgui_color_int_t;
typedef edgui_color1_t edgui_color_t;
#elif EDGUI_COLOR_DEPTH == 8
typedef uint8_t edgui_color_int_t;
typedef edgui_color8_t edgui_color_t;
#elif EDGUI_COLOR_DEPTH == 16
typedef uint16_t edgui_color_int_t;
typedef edgui_color16_t edgui_color_t;
#elif EDGUI_COLOR_DEPTH == 32
typedef uint32_t edgui_color_int_t;
typedef edgui_color32_t edgui_color_t;
#else
#error "Invalid EDGUI_COLOR_DEPTH in edgui_conf.h! Set it to 1, 8, 16 or 32!"
#endif

typedef uint8_t edgui_opa_t;

typedef struct
{
    uint16_t h;
    uint8_t s;
    uint8_t v;
} edgui_color_hsv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*In color conversations:
 * - When converting to bigger color type the LSB weight of 1 LSB is calculated
 *   E.g. 16 bit Red has 5 bits
 *         8 bit Red has 2 bits
 *        ----------------------
 *        8 bit red LSB = (2^5 - 1) / (2^2 - 1) = 31 / 3 = 10
 *
 * - When calculating to smaller color type simply shift out the LSBs
 *   E.g.  8 bit Red has 2 bits
 *        16 bit Red has 5 bits
 *        ----------------------
 *         Shift right with 5 - 3 = 2
 */

static __inline uint8_t edgui_color_to1(edgui_color_t color)
{
#if EDGUI_COLOR_DEPTH == 1
    return color.full;
#elif EDGUI_COLOR_DEPTH == 8
    if((color.ch.red & 0x4) || (color.ch.green & 0x4) || (color.ch.blue & 0x2)) {
        return 1;
    } else {
        return 0;
    }
#elif EDGUI_COLOR_DEPTH == 16
#if EDGUI_COLOR_16_SWAP == 0
    if((color.ch.red & 0x10) || (color.ch.green & 0x20) || (color.ch.blue & 0x10)) {
        return 1;
#else
    if((color.ch.red & 0x10) || (color.ch.green_h & 0x20) || (color.ch.blue & 0x10)) {
        return 1;
#endif
    } else {
        return 0;
    }
#elif EDGUI_COLOR_DEPTH == 32
    if((color.ch.red & 0x80) || (color.ch.green & 0x80) || (color.ch.blue & 0x80)) {
        return 1;
    } else {
        return 0;
    }
#endif
}

static __inline uint8_t edgui_color_to8(edgui_color_t color)
{
#if EDGUI_COLOR_DEPTH == 1
    if(color.full == 0)
        return 0;
    else
        return 0xFF;
#elif EDGUI_COLOR_DEPTH == 8
    return color.full;
#elif EDGUI_COLOR_DEPTH == 16

#if EDGUI_COLOR_16_SWAP == 0
    edgui_color8_t ret;
    ret.ch.red   = color.ch.red >> 2;   /* 5 - 3  = 2*/
    ret.ch.green = color.ch.green >> 3; /* 6 - 3  = 3*/
    ret.ch.blue  = color.ch.blue >> 3;  /* 5 - 2  = 3*/
    return ret.full;
#else
    edgui_color8_t ret;
    ret.ch.red   = color.ch.red >> 2;  /* 5 - 3  = 2*/
    ret.ch.green = color.ch.green_h;   /* 6 - 3  = 3*/
    ret.ch.blue  = color.ch.blue >> 3; /* 5 - 2  = 3*/
    return ret.full;
#endif
#elif EDGUI_COLOR_DEPTH == 32
    edgui_color8_t ret;
    ret.ch.red   = color.ch.red >> 5;   /* 8 - 3  = 5*/
    ret.ch.green = color.ch.green >> 5; /* 8 - 3  = 5*/
    ret.ch.blue  = color.ch.blue >> 6;  /* 8 - 2  = 6*/
    return ret.full;
#endif
}

static __inline uint16_t edgui_color_to16(edgui_color_t color)
{
#if EDGUI_COLOR_DEPTH == 1
    if(color.full == 0)
        return 0;
    else
        return 0xFFFF;
#elif EDGUI_COLOR_DEPTH == 8
    edgui_color16_t ret;
#if EDGUI_COLOR_16_SWAP == 0
    ret.ch.red = color.ch.red * 4;     /*(2^5 - 1)/(2^3 - 1) = 31/7 = 4*/
    ret.ch.green = color.ch.green * 9; /*(2^6 - 1)/(2^3 - 1) = 63/7 = 9*/
    ret.ch.blue = color.ch.blue * 10;  /*(2^5 - 1)/(2^2 - 1) = 31/3 = 10*/
#else
    ret.red        = color.ch.red * 4;
    uint8_t g_tmp  = color.ch.green * 9;
    ret.ch.green_h = (g_tmp & 0x1F) >> 3;
    ret.ch.green_l = g_tmp & 0x07;
    ret.ch.blue    = color.ch.blue * 10;
#endif
    return ret.full;
#elif EDGUI_COLOR_DEPTH == 16
    return color.full;
#elif EDGUI_COLOR_DEPTH == 32
    edgui_color16_t ret;
#if EDGUI_COLOR_16_SWAP == 0
    ret.ch.red   = color.ch.red >> 3;   /* 8 - 5  = 3*/
    ret.ch.green = color.ch.green >> 2; /* 8 - 6  = 2*/
    ret.ch.blue  = color.ch.blue >> 3;  /* 8 - 5  = 3*/
#else
    ret.ch.red     = color.ch.red >> 3;
    ret.ch.green_h = (color.ch.green & 0xE0) >> 5;
    ret.ch.green_l = (color.ch.green & 0x1C) >> 2;
    ret.ch.blue    = color.ch.blue >> 3;
#endif
    return ret.full;
#endif
}

static __inline uint32_t edgui_color_to32(edgui_color_t color)
{
#if EDGUI_COLOR_DEPTH == 1
    if(color.full == 0)
        return 0;
    else
        return 0xFFFFFFFF;
#elif EDGUI_COLOR_DEPTH == 8
    edgui_color32_t ret;
    ret.ch.red = color.ch.red * 36;     /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    ret.ch.green = color.ch.green * 36; /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    ret.ch.blue = color.ch.blue * 85;   /*(2^8 - 1)/(2^2 - 1) = 255/3 = 85*/
    ret.ch.alpha = 0xFF;
    return ret.full;
#elif EDGUI_COLOR_DEPTH == 16
#if EDGUI_COLOR_16_SWAP == 0
    edgui_color32_t ret;
    ret.ch.red   = color.ch.red * 8;   /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.ch.green = color.ch.green * 4; /*(2^8 - 1)/(2^6 - 1) = 255/63 = 4*/
    ret.ch.blue  = color.ch.blue * 8;  /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.ch.alpha = 0xFF;
    return ret.full;
#else
    edgui_color32_t ret;
    ret.ch.red   = color.ch.red * 8;                                 /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.ch.green = ((color.ch.green_h << 3) + color.ch.green_l) * 4; /*(2^8 - 1)/(2^6 - 1) = 255/63 = 4*/
    ret.ch.blue  = color.ch.blue * 8;                                /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.ch.alpha = 0xFF;
    return ret.full;
#endif
#elif EDGUI_COLOR_DEPTH == 32
    return color.full;
#endif
}

static __inline edgui_color_t edgui_color_mix(edgui_color_t c1, edgui_color_t c2, uint8_t mix)
{
    edgui_color_t ret;
#if EDGUI_COLOR_DEPTH != 1
    /*EDGUI_COLOR_DEPTH == 8, 16 or 32*/
    ret.ch.red = (uint16_t)((uint16_t)c1.ch.red * mix + (c2.ch.red * (255 - mix))) >> 8;
#if EDGUI_COLOR_DEPTH == 16 && EDGUI_COLOR_16_SWAP
    /*If swapped Green is in 2 parts*/
    uint16_t g_1   = (c1.ch.green_h << 3) + c1.ch.green_l;
    uint16_t g_2   = (c2.ch.green_h << 3) + c2.ch.green_l;
    uint16_t g_out = (uint16_t)((uint16_t)g_1 * mix + (g_2 * (255 - mix))) >> 8;
    ret.ch.green_h = g_out >> 3;
    ret.ch.green_l = g_out & 0x7;
#else
    ret.ch.green = (uint16_t)((uint16_t)c1.ch.green * mix + (c2.ch.green * (255 - mix))) >> 8;
#endif
    ret.ch.blue = (uint16_t)((uint16_t)c1.ch.blue * mix + (c2.ch.blue * (255 - mix))) >> 8;
#if EDGUI_COLOR_DEPTH == 32
    ret.ch.alpha = 0xFF;
#endif
#else
    /*EDGUI_COLOR_DEPTH == 1*/
    ret.full = mix > EDGUI_OPA_50 ? c1.full : c2.full;
#endif

    return ret;
}

/**
 * Get the brightness of a color
 * @param color a color
 * @return the brightness [0..255]
 */
static __inline uint8_t edgui_color_brightness(edgui_color_t color)
{
    edgui_color32_t c32;
    c32.full        = edgui_color_to32(color);
    uint16_t bright = 3 * c32.ch.red + c32.ch.blue + 4 * c32.ch.green;
    return (uint16_t)bright >> 3;
}

/* The most simple macro to create a color from R,G and B values */
#if EDGUI_COLOR_DEPTH == 1
#define EDGUI_COLOR_MAKE(r8, g8, b8) ((edgui_color_t){(b8 >> 7 | g8 >> 7 | r8 >> 7)})
static __inline edgui_color_t edgui_color_make(int r8, int g8, int b8)
{
    edgui_color_t color;
    color.full = (b8 >> 7 | g8 >> 7 | r8 >> 7);
    return color;
}
#elif EDGUI_COLOR_DEPTH == 8
#define EDGUI_COLOR_MAKE(r8, g8, b8) ((edgui_color_t){{b8 >> 6, g8 >> 5, r8 >> 5}})
static __inline edgui_color_t edgui_color_make(uint8_t r8, int g8, int b8)
{
    edgui_color_t color;
    color.ch.blue = b8 >> 6;
    color.ch.green = g8 >> 5;
    color.ch.red = r8 >> 5;
    return color;
}
#elif EDGUI_COLOR_DEPTH == 16
#if EDGUI_COLOR_16_SWAP == 0
#define EDGUI_COLOR_MAKE(r8, g8, b8) ((edgui_color_t){{b8 >> 3, g8 >> 2, r8 >> 3}})
static __inline edgui_color_t edgui_color_make(uint8_t r8, uint8_t g8, uint8_t b8)
{
    edgui_color_t color;
    color.ch.blue  = (uint16_t)(b8 >> 3);
    color.ch.green = (uint16_t)(g8 >> 2);
    color.ch.red   = (uint16_t)(r8 >> 3);
    return color;
}
#else
#define EDGUI_COLOR_MAKE(r8, g8, b8) ((edgui_color_t){{g8 >> 5, r8 >> 3, b8 >> 3, (g8 >> 2) & 0x7}})
static __inline edgui_color_t edgui_color_make(uint8_t r8, uint8_t g8, uint8_t b8)
{
    edgui_color_t color;
    color.ch.green_h = (uint16_t)(g8 >> 5);
    color.ch.red = (uint16_t)(r8 >> 3);
    color.ch.blue = (uint16_t)(b8 >> 3);
    color.ch.green_l = (uint16_t)((g8 >> 2) & 0x7);
    return color;
}
#endif
#elif EDGUI_COLOR_DEPTH == 32
#define EDGUI_COLOR_MAKE(r8, g8, b8) ((edgui_color_t){{b8, g8, r8, 0xff}}) /*Fix 0xff alpha*/
static __inline edgui_color_t edgui_color_make(uint8_t r8, uint8_t g8, uint8_t b8)
{
    edgui_color_t color;
    color.ch.blue  = b8;
    color.ch.green = g8;
    color.ch.red   = r8;
    color.ch.alpha = 0xff;
    return color;
}
#endif

static __inline edgui_color_t edgui_color_hex(uint32_t c)
{
    return edgui_color_make((uint8_t)((c >> 16) & 0xFF), (uint8_t)((c >> 8) & 0xFF), (uint8_t)(c & 0xFF));
}

static __inline edgui_color_t edgui_color_hex3(uint32_t c)
{
    return edgui_color_make((uint8_t)(((c >> 4) & 0xF0) | ((c >> 8) & 0xF)), (uint8_t)((c & 0xF0) | ((c & 0xF0) >> 4)),
                         (uint8_t)((c & 0xF) | ((c & 0xF) << 4)));
}

/**
 * Convert a HSV color to RGB
 * @param h hue [0..359]
 * @param s saturation [0..100]
 * @param v value [0..100]
 * @return the given RGB color in RGB (with EDGUI_COLOR_DEPTH depth)
 */
edgui_color_t edgui_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * Convert an RGB color to HSV
 * @param r red
 * @param g green
 * @param b blue
 * @return the given RGB color n HSV
 */
edgui_color_hsv_t edgui_color_rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_COLOR*/
