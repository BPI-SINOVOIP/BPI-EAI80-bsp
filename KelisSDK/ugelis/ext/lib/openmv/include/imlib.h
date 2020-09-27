/* This file is part of the OpenMV project.
 * Copyright (c) 2013-2017 Ibrahim Abdelkader <iabdalkader@openmv.io> & Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 */

#ifndef __IMLIB_H__
#define __IMLIB_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include "xalloc.h"
#include "array.h"
#include "fmath.h"
#include "collections.h"
//#include "stats.h"
#include <arch/arm/cortex_m/cmsis.h>


#define IM_LOG2_2(x)    (((x) &                0x2ULL) ? ( 2                        ) :             1) // NO ({ ... }) !
#define IM_LOG2_4(x)    (((x) &                0xCULL) ? ( 2 +  IM_LOG2_2((x) >>  2)) :  IM_LOG2_2(x)) // NO ({ ... }) !
#define IM_LOG2_8(x)    (((x) &               0xF0ULL) ? ( 4 +  IM_LOG2_4((x) >>  4)) :  IM_LOG2_4(x)) // NO ({ ... }) !
#define IM_LOG2_16(x)   (((x) &             0xFF00ULL) ? ( 8 +  IM_LOG2_8((x) >>  8)) :  IM_LOG2_8(x)) // NO ({ ... }) !
#define IM_LOG2_32(x)   (((x) &         0xFFFF0000ULL) ? (16 + IM_LOG2_16((x) >> 16)) : IM_LOG2_16(x)) // NO ({ ... }) !
#define IM_LOG2(x)      (((x) & 0xFFFFFFFF00000000ULL) ? (32 + IM_LOG2_32((x) >> 32)) : IM_LOG2_32(x)) // NO ({ ... }) !

#define IM_MAX(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define IM_DIV(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a / _b) : 0; })
#define IM_MOD(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _b ? (_a % _b) : 0; })

#define INT8_T_BITS     (sizeof(int8_t) * 8)
#define INT8_T_MASK     (INT8_T_BITS - 1)
#define INT8_T_SHIFT    IM_LOG2(INT8_T_MASK)

#define INT16_T_BITS    (sizeof(int16_t) * 8)
#define INT16_T_MASK    (INT16_T_BITS - 1)
#define INT16_T_SHIFT   IM_LOG2(INT16_T_MASK)

#define INT32_T_BITS    (sizeof(int32_t) * 8)
#define INT32_T_MASK    (INT32_T_BITS - 1)
#define INT32_T_SHIFT   IM_LOG2(INT32_T_MASK)

#define INT64_T_BITS    (sizeof(int64_t) * 8)
#define INT64_T_MASK    (INT64_T_BITS - 1)
#define INT64_T_SHIFT   IM_LOG2(INT64_T_MASK)

#define UINT8_T_BITS    (sizeof(uint8_t) * 8)
#define UINT8_T_MASK    (UINT8_T_BITS - 1)
#define UINT8_T_SHIFT   IM_LOG2(UINT8_T_MASK)

#define UINT16_T_BITS   (sizeof(uint16_t) * 8)
#define UINT16_T_MASK   (UINT16_T_BITS - 1)
#define UINT16_T_SHIFT  IM_LOG2(UINT16_T_MASK)

#define UINT32_T_BITS   (sizeof(uint32_t) * 8)
#define UINT32_T_MASK   (UINT32_T_BITS - 1)
#define UINT32_T_SHIFT  IM_LOG2(UINT32_T_MASK)

#define UINT64_T_BITS   (sizeof(uint64_t) * 8)
#define UINT64_T_MASK   (UINT64_T_BITS - 1)
#define UINT64_T_SHIFT  IM_LOG2(UINT64_T_MASK)

#define IM_DEG2RAD(x)   (((x)*M_PI)/180)
#define IM_RAD2DEG(x)   (((x)*180)/M_PI)

/////////////////
// Point Stuff //
/////////////////

#ifndef POINT_T
#define POINT_T
typedef struct point {
    int16_t x;
    int16_t y;
} point_t;
#endif

void point_init(point_t *ptr, int x, int y);
void point_copy(point_t *dst, point_t *src);
bool point_equal_fast(point_t *ptr0, point_t *ptr1);
int point_quadrance(point_t *ptr0, point_t *ptr1);
void point_rotate(int x, int y, float r, int center_x, int center_y, int16_t *new_x, int16_t *new_y);
void point_min_area_rectangle(point_t *corners, point_t *new_corners, int corners_len);

////////////////
// Line Stuff //
////////////////

typedef struct line {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} line_t;

bool lb_clip_line(line_t *l, int x, int y, int w, int h);

/////////////////////
// Rectangle Stuff //
/////////////////////

typedef struct rectangle {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} rectangle_t;

void rectangle_init(rectangle_t *ptr, int x, int y, int w, int h);
void rectangle_copy(rectangle_t *dst, rectangle_t *src);
bool rectangle_equal_fast(rectangle_t *ptr0, rectangle_t *ptr1);
bool rectangle_overlap(rectangle_t *ptr0, rectangle_t *ptr1);
void rectangle_intersected(rectangle_t *dst, rectangle_t *src);
void rectangle_united(rectangle_t *dst, rectangle_t *src);

/////////////////
// Color Stuff //
/////////////////

typedef struct color_thresholds_list_lnk_data
{
    uint8_t LMin, LMax; // or grayscale
    int8_t AMin, AMax;
    int8_t BMin, BMax;
}
color_thresholds_list_lnk_data_t;

#define COLOR_THRESHOLD_BINARY(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    ((_threshold->LMin <= _pixel) && (_pixel <= _threshold->LMax)) ^ _invert; \
})

#define COLOR_THRESHOLD_GRAYSCALE(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    ((_threshold->LMin <= _pixel) && (_pixel <= _threshold->LMax)) ^ _invert; \
})

#define COLOR_THRESHOLD_RGB565(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    uint8_t _l = COLOR_RGB565_TO_L(_pixel); \
    int8_t _a = COLOR_RGB565_TO_A(_pixel); \
    int8_t _b = COLOR_RGB565_TO_B(_pixel); \
    ((_threshold->LMin <= _l) && (_l <= _threshold->LMax) && \
    (_threshold->AMin <= _a) && (_a <= _threshold->AMax) && \
    (_threshold->BMin <= _b) && (_b <= _threshold->BMax)) ^ _invert; \
})

#define COLOR_BOUND_BINARY(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(_pixel0 - _pixel1) <= _threshold); \
})

#define COLOR_BOUND_GRAYSCALE(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(_pixel0 - _pixel1) <= _threshold); \
})

#define COLOR_BOUND_RGB565(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(COLOR_RGB565_TO_R5(_pixel0) - COLOR_RGB565_TO_R5(_pixel1)) <= COLOR_RGB565_TO_R5(_threshold)) && \
    (abs(COLOR_RGB565_TO_G6(_pixel0) - COLOR_RGB565_TO_G6(_pixel1)) <= COLOR_RGB565_TO_G6(_threshold)) && \
    (abs(COLOR_RGB565_TO_B5(_pixel0) - COLOR_RGB565_TO_B5(_pixel1)) <= COLOR_RGB565_TO_B5(_threshold)); \
})

#define COLOR_BINARY_MIN 0
#define COLOR_BINARY_MAX 1
#define COLOR_GRAYSCALE_BINARY_MIN 0x00
#define COLOR_GRAYSCALE_BINARY_MAX 0xFF
#define COLOR_RGB565_BINARY_MIN 0x0000
#define COLOR_RGB565_BINARY_MAX 0xFFFF

#define COLOR_GRAYSCALE_MIN 0
#define COLOR_GRAYSCALE_MAX 255

#define COLOR_R5_MIN 0
#define COLOR_R5_MAX 31
#define COLOR_G6_MIN 0
#define COLOR_G6_MAX 63
#define COLOR_B5_MIN 0
#define COLOR_B5_MAX 31

#define COLOR_R8_MIN 0
#define COLOR_R8_MAX 255
#define COLOR_G8_MIN 0
#define COLOR_G8_MAX 255
#define COLOR_B8_MIN 0
#define COLOR_B8_MAX 255

#define COLOR_L_MIN 0
#define COLOR_L_MAX 100
#define COLOR_A_MIN -128
#define COLOR_A_MAX 127
#define COLOR_B_MIN -128
#define COLOR_B_MAX 127

#define COLOR_Y_MIN -128
#define COLOR_Y_MAX 127
#define COLOR_U_MIN -128
#define COLOR_U_MAX 127
#define COLOR_V_MIN -128
#define COLOR_V_MAX 127

extern const uint8_t rb528_table[32];
extern const uint8_t g628_table[64];

#define COLOR_R5_TO_R8(color) rb528_table[color]
#define COLOR_G6_TO_G8(color) g628_table[color]
#define COLOR_B5_TO_B8(color) rb528_table[color]

extern const uint8_t rb825_table[256];
extern const uint8_t g826_table[256];

#define COLOR_R8_TO_R5(color) rb825_table[color]
#define COLOR_G8_TO_G6(color) g826_table[color]
#define COLOR_B8_TO_B5(color) rb825_table[color]

// RGB565 Stuff //

#define COLOR_RGB565_TO_R5(pixel) (((pixel) >> 3) & 0x1F)
#define COLOR_RGB565_TO_G6(pixel) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    ((_pixel & 0x07) << 3) | (_pixel >> 13); \
})
#define COLOR_RGB565_TO_B5(pixel) (((pixel) >> 8) & 0x1F)
#define COLOR_RGB565_TO_R8(pixel) COLOR_R5_TO_R8(COLOR_RGB565_TO_R5(pixel))
#define COLOR_RGB565_TO_G8(pixel) COLOR_G6_TO_G8(COLOR_RGB565_TO_G6(pixel))
#define COLOR_RGB565_TO_B8(pixel) COLOR_B5_TO_B8(COLOR_RGB565_TO_B5(pixel))

#define COLOR_R5_G6_B5_TO_RGB565(r5, g6, b5) \
({ \
    __typeof__ (r5) _r5 = (r5); \
    __typeof__ (g6) _g6 = (g6); \
    __typeof__ (b5) _b5 = (b5); \
    (_r5 << 3) | (_g6 >> 3) | ((_g6 & 0x7) << 13) | (_b5 << 8); \
})

#define COLOR_R8_G8_B8_TO_RGB565(r8, g8, b8) COLOR_R5_G6_B5_TO_RGB565(COLOR_R8_TO_R5(r8), COLOR_G8_TO_G6(g8), COLOR_B8_TO_B5(b8))

extern const int8_t lab_table[196608];
extern const int8_t yuv_table[196608];

#ifdef IMLIB_ENABLE_LAB_LUT
#define COLOR_RGB565_TO_L(pixel) lab_table[(pixel) * 3]
#define COLOR_RGB565_TO_A(pixel) lab_table[((pixel) * 3) + 1]
#define COLOR_RGB565_TO_B(pixel) lab_table[((pixel) * 3) + 2]
#else
#define COLOR_RGB565_TO_L(pixel) imlib_rgb565_to_l(pixel)
#define COLOR_RGB565_TO_A(pixel) imlib_rgb565_to_a(pixel)
#define COLOR_RGB565_TO_B(pixel) imlib_rgb565_to_b(pixel)
#endif

#ifdef IMLIB_ENABLE_YUV_LUT
#define COLOR_RGB565_TO_Y(pixel) yuv_table[(pixel) * 3]
#define COLOR_RGB565_TO_U(pixel) yuv_table[((pixel) * 3) + 1]
#define COLOR_RGB565_TO_V(pixel) yuv_table[((pixel) * 3) + 2]
#else
#define COLOR_RGB565_TO_Y(pixel) imlib_rgb565_to_y(pixel)
#define COLOR_RGB565_TO_U(pixel) imlib_rgb565_to_u(pixel)
#define COLOR_RGB565_TO_V(pixel) imlib_rgb565_to_v(pixel)
#endif

#define COLOR_LAB_TO_RGB565(l, a, b) imlib_lab_to_rgb(l, a, b)
#define COLOR_YUV_TO_RGB565(y, u, v) imlib_yuv_to_rgb((y) + 128, u, v)

#define COLOR_BAYER_TO_RGB565(img, x, y, r, g, b)            \
({                                                           \
    __typeof__ (x) __x = (x);                                \
    __typeof__ (y) __y = (y);                                \
    if ((__y % 2) == 0) {                                    \
        if ((__x % 2) == 0) {                                \
            r = (IM_GET_RAW_PIXEL(img, __x-1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y+1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y+1)) >> 2;  \
                                                             \
            g = (IM_GET_RAW_PIXEL(img, __x,   __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x,   __y+1)  +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y)    +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y))   >> 2;  \
                                                             \
            b = IM_GET_RAW_PIXEL(img,  __x, __y);            \
        } else {                                             \
            r = (IM_GET_RAW_PIXEL(img, __x, __y-1)  +        \
                 IM_GET_RAW_PIXEL(img, __x, __y+1)) >> 1;    \
                                                             \
            b = (IM_GET_RAW_PIXEL(img, __x-1, __y)  +        \
                 IM_GET_RAW_PIXEL(img, __x+1, __y)) >> 1;    \
                                                             \
            g =  IM_GET_RAW_PIXEL(img, __x, __y);            \
        }                                                    \
    } else {                                                 \
        if ((__x % 2) == 0) {                                \
            r = (IM_GET_RAW_PIXEL(img, __x-1, __y)  +        \
                 IM_GET_RAW_PIXEL(img, __x+1, __y)) >> 1;    \
                                                             \
            g =  IM_GET_RAW_PIXEL(img, __x, __y);            \
                                                             \
            b = (IM_GET_RAW_PIXEL(img, __x, __y-1)  +        \
                 IM_GET_RAW_PIXEL(img, __x, __y+1)) >> 1;    \
        } else {                                             \
            r = IM_GET_RAW_PIXEL(img,  __x, __y);            \
                                                             \
            g = (IM_GET_RAW_PIXEL(img, __x, __y-1)    +      \
                 IM_GET_RAW_PIXEL(img, __x, __y+1)    +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y)    +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y))   >> 2;  \
                                                             \
            b = (IM_GET_RAW_PIXEL(img, __x-1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y+1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y+1)) >> 2;  \
        }                                                    \
    }                                                        \
    r  = IM_R825(r);                                         \
    g  = IM_G826(g);                                         \
    b  = IM_B825(b);                                         \
})

#define COLOR_BINARY_TO_GRAYSCALE(pixel) ((pixel) * COLOR_GRAYSCALE_MAX)
#define COLOR_BINARY_TO_RGB565(pixel) COLOR_YUV_TO_RGB565(((pixel) ? 127 : -128), 0, 0)
#define COLOR_RGB565_TO_BINARY(pixel) (COLOR_RGB565_TO_Y(pixel) > (((COLOR_Y_MAX - COLOR_Y_MIN) / 2) + COLOR_Y_MIN))
#define COLOR_RGB565_TO_GRAYSCALE(pixel) (COLOR_RGB565_TO_Y(pixel) + 128)
#define COLOR_GRAYSCALE_TO_BINARY(pixel) ((pixel) > (((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) / 2) + COLOR_GRAYSCALE_MIN))
#define COLOR_GRAYSCALE_TO_RGB565(pixel) COLOR_YUV_TO_RGB565(((pixel) - 128), 0, 0)

typedef enum {
    COLOR_PALETTE_RAINBOW,
    COLOR_PALETTE_IRONBOW
} color_palette_t;

// Color palette LUTs
extern const uint16_t rainbow_table[256];
extern const uint16_t ironbow_table[256];

/////////////////
// Image Stuff //
/////////////////

typedef enum image_bpp
{
    IMAGE_BPP_BINARY,       // BPP = 0
    IMAGE_BPP_GRAYSCALE,    // BPP = 1
    IMAGE_BPP_RGB565,       // BPP = 2
    IMAGE_BPP_BAYER,        // BPP = 3
    IMAGE_BPP_JPEG          // BPP > 3
}
image_bpp_t;

typedef struct image {
    int w;
    int h;
    int bpp;
    union {
        uint8_t *pixels;
        uint8_t *data;
    };
} image_t;

void image_init(image_t *ptr, int w, int h, int bpp, void *data);
void image_copy(image_t *dst, image_t *src);
size_t image_size(image_t *ptr);
bool image_get_mask_pixel(image_t *ptr, int x, int y);

#define IMAGE_IS_MUTABLE(image) \
({ \
    __typeof__ (image) _image = (image); \
    (_image->bpp == IMAGE_BPP_BINARY) || \
    (_image->bpp == IMAGE_BPP_GRAYSCALE) || \
    (_image->bpp == IMAGE_BPP_RGB565); \
})

#define IMAGE_IS_MUTABLE_BAYER(image) \
({ \
    __typeof__ (image) _image = (image); \
    (_image->bpp == IMAGE_BPP_BINARY) || \
    (_image->bpp == IMAGE_BPP_GRAYSCALE) || \
    (_image->bpp == IMAGE_BPP_RGB565) || \
    (_image->bpp == IMAGE_BPP_BAYER); \
})

#define IMAGE_BINARY_LINE_LEN(image) (((image)->w + UINT32_T_MASK) >> UINT32_T_SHIFT)
#define IMAGE_BINARY_LINE_LEN_BYTES(image) (IMAGE_BINARY_LINE_LEN(image) * sizeof(uint32_t))

#define IMAGE_GRAYSCALE_LINE_LEN(image) ((image)->w)
#define IMAGE_GRAYSCALE_LINE_LEN_BYTES(image) (IMAGE_GRAYSCALE_LINE_LEN(image) * sizeof(uint8_t))

#define IMAGE_RGB565_LINE_LEN(image) ((image)->w)
#define IMAGE_RGB565_LINE_LEN_BYTES(image) (IMAGE_RGB565_LINE_LEN(image) * sizeof(uint16_t))

#define IMAGE_GET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    (((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    size_t _i = (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT); \
    size_t _j = _x & UINT32_T_MASK; \
    ((uint32_t *) _image->data)[_i] = (((uint32_t *) _image->data)[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_GET_GRAYSCALE_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_RGB565_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_RGB565_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#ifdef __arm__
    #define IMAGE_REV_RGB565_PIXEL(pixel) \
    ({ \
        __typeof__ (pixel) _pixel = (pixel); \
        __REV16(_pixel); \
    })
#else
    #define IMAGE_REV_RGB565_PIXEL(pixel) \
    ({ \
        __typeof__ (pixel) _pixel = (pixel); \
        ((_pixel >> 8) | (_pixel << 8)) & 0xFFFF; \
    })
#endif

#define IMAGE_COMPUTE_TARGET_SIZE_SCALE_FACTOR(target_size, source_rect) \
__typeof__ (target_size) _target_size = (target_size); \
__typeof__ (source_rect) _source_rect = (source_rect); \
int IMAGE_X_SOURCE_OFFSET = _source_rect->p.x; \
int IMAGE_Y_SOURCE_OFFSET = _source_rect->p.y; \
int IMAGE_X_TARGET_OFFSET = 0; \
int IMAGE_Y_TARGET_OFFSET = 0; \
float IMAGE_X_RATIO = ((float) _source_rect->s.w) / ((float) _target_size->w); \
float IMAGE_Y_RATIO = ((float) _source_rect->s.h) / ((float) _target_size->h); \
({ 0; })

#define IMAGE_COMPUTE_TARGET_RECT_SCALE_FACTOR(target_rect, source_rect) \
__typeof__ (target_rect) _target_rect = (target_rect); \
__typeof__ (source_rect) _source_rect = (source_rect); \
int IMAGE_X_SOURCE_OFFSET = _source_rect->p.x; \
int IMAGE_Y_SOURCE_OFFSET = _source_rect->p.y; \
int IMAGE_X_TARGET_OFFSET = _target_rect->p.x; \
int IMAGE_Y_TARGET_OFFSET = _target_rect->p.y; \
float IMAGE_X_RATIO = ((float) _source_rect->s.w) / ((float) _target_rect->s.w); \
float IMAGE_Y_RATIO = ((float) _source_rect->s.h) / ((float) _target_rect->s.h); \
({ 0; })

#define IMAGE_GET_SCALED_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    IMAGE_GET_BINARY_PIXEL(_image, ((size_t) ((IMAGE_X_RATIO * (_x - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET, ((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET); \
})

#define IMAGE_GET_SCALED_GRAYSCALE_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    IMAGE_GET_GRAYSCALE_PIXEL(_image, ((size_t) ((IMAGE_X_RATIO * (_x - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET, ((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET); \
})

#define IMAGE_GET_SCALED_RGB565_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    IMAGE_GET_RGB565_PIXEL(_image, ((size_t) ((IMAGE_X_RATIO * (_x - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET, ((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET); \
})

// Fast Stuff //

#define IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data) + (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y); \
})

#define IMAGE_INC_BINARY_PIXEL_ROW_PTR(row_ptr, image) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (image) _image = (image); \
    _row_ptr + ((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT); \
})

#define IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    (_row_ptr[_x >> UINT32_T_SHIFT] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    size_t _i = _x >> UINT32_T_SHIFT; \
    size_t _j = _x & UINT32_T_MASK; \
    _row_ptr[_i] = (_row_ptr[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_INC_GRAYSCALE_PIXEL_ROW_PTR(row_ptr, image) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (image) _image = (image); \
    row_ptr + _image->w; \
})

#define IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_INC_RGB565_PIXEL_ROW_PTR(row_ptr, image) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (image) _image = (image); \
    row_ptr + _image->w; \
})

#define IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * 3 * _y); \
})

#define IMAGE_COMPUTE_SCALED_BINARY_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data) + (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * (((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET)); \
})

#define IMAGE_GET_SCALED_BINARY_PIXEL_FAST(row_ptr, x) IMAGE_GET_BINARY_PIXEL_FAST((row_ptr), ((size_t) ((IMAGE_X_RATIO * ((x) - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET)

#define IMAGE_COMPUTE_SCALED_GRAYSCALE_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * (((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET)); \
})

#define IMAGE_GET_SCALED_GRAYSCALE_PIXEL_FAST(row_ptr, x) IMAGE_GET_GRAYSCALE_PIXEL_FAST((row_ptr), ((size_t) ((IMAGE_X_RATIO * ((x) - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET)

#define IMAGE_COMPUTE_SCALED_RGB565_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * (((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET)); \
})

#define IMAGE_GET_SCALED_RGB565_PIXEL_FAST(row_ptr, x) IMAGE_GET_RGB565_PIXEL_FAST((row_ptr), ((size_t) ((IMAGE_X_RATIO * ((x) - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET)

// Old Image Macros - Will be refactor and removed. But, only after making sure through testing new macros work.

#define IM_SWAP16(x) __REV16(x) // Swap bottom two chars in short.
#define IM_SWAP32(x) __REV32(x) // Swap bottom two shorts in long.

// RGB565 to RGB888 conversion.
extern const uint8_t rb528_table[32];
extern const uint8_t g628_table[64];

#define IM_R528(p) \
    ({ __typeof__ (p) _p = (p); \
       rb528_table[_p]; })

#define IM_G628(p) \
    ({ __typeof__ (p) _p = (p); \
       g628_table[_p]; })

#define IM_B528(p) \
    ({ __typeof__ (p) _p = (p); \
       rb528_table[_p]; })

// RGB888 to RGB565 conversion.
extern const uint8_t rb825_table[256];
extern const uint8_t g826_table[256];

#define IM_R825(p) \
    ({ __typeof__ (p) _p = (p); \
       rb825_table[_p]; })

#define IM_G826(p) \
    ({ __typeof__ (p) _p = (p); \
       g826_table[_p]; })

#define IM_B825(p) \
    ({ __typeof__ (p) _p = (p); \
       rb825_table[_p]; })

// Split RGB565 values (note the RGB565 value is byte reversed).

#define IM_R565(p) \
    ({ __typeof__ (p) _p = (p); \
       ((_p)>>3)&0x1F; })

#define IM_G565(p) \
    ({ __typeof__ (p) _p = (p); \
       (((_p)&0x07)<<3)|((_p)>>13); })

#define IM_B565(p) \
    ({ __typeof__ (p) _p = (p); \
       ((_p)>>8)&0x1F; })

// Merge RGB565 values (note the RGB565 value is byte reversed).

#define IM_RGB565(r, g, b) \
    ({ __typeof__ (r) _r = (r); \
       __typeof__ (g) _g = (g); \
       __typeof__ (b) _b = (b); \
       ((_r)<<3)|((_g)>>3)|((_g)<<13)|((_b)<<8); })

// Image kernels
extern const int8_t kernel_gauss_3[9];
extern const int8_t kernel_gauss_5[25];
extern const int kernel_laplacian_3[9];
extern const int kernel_high_pass_3[9];

#define IM_RGB5652L(p) \
    ({ __typeof__ (p) _p = (p); \
       lab_table[_p * 3]; })

#define IM_RGB5652A(p) \
    ({ __typeof__ (p) _p = (p); \
       lab_table[(_p * 3) + 1]; })

#define IM_RGB5652B(p) \
    ({ __typeof__ (p) _p = (p); \
       lab_table[(_p * 3) + 2]; })

// Grayscale maxes
#define IM_MAX_GS (255)

// RGB565 maxes
#define IM_MAX_R5 (31)
#define IM_MAX_G6 (63)
#define IM_MAX_B5 (31)

// Grayscale histogram
#define IM_G_HIST_SIZE (256)
#define IM_G_HIST_OFFSET (0)

// LAB histogram
#define IM_L_HIST_SIZE (256)
#define IM_L_HIST_OFFSET (0)
#define IM_A_HIST_SIZE (256)
#define IM_A_HIST_OFFSET (256)
#define IM_B_HIST_SIZE (256)
#define IM_B_HIST_OFFSET (512)

#define IM_IS_BINARY(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp == 0; })

#define IM_IS_GS(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp == 1; })

#define IM_IS_RGB565(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp == 2; })

#define IM_IS_BAYER(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp == 3; })

#define IM_IS_JPEG(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp >= 4; })

#define IM_IS_MUTABLE(img) \
    ({ __typeof__ (img) _img = (img); \
       (_img->bpp == 1 || _img->bpp == 2); })

#define IM_X_INSIDE(img, x) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       (0<=_x)&&(_x<_img->w); })

#define IM_Y_INSIDE(img, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (y) _y = (y); \
       (0<=_y)&&(_y<_img->h); })

#define IM_GET_GS_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _x = (_x < 0) ? 0 : (_x >= img->w) ? (img->w -1): _x; \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _y = (_y < 0) ? 0 : (_y >= img->h) ? (img->h -1): _y; \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _x = (_x < 0) ? 0 : (_x >= img->w) ? (img->w -1): _x; \
       _y = (_y < 0) ? 0 : (_y >= img->h) ? (img->h -1): _y; \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RGB565_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_SET_GS_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_SET_RGB565_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_EQUAL(img0, img1) \
    ({ __typeof__ (img0) _img0 = (img0); \
       __typeof__ (img1) _img1 = (img1); \
       (_img0->w==_img1->w)&&(_img0->h==_img1->h)&&(_img0->bpp==_img1->bpp); })

#define IM_TO_GS_PIXEL(img, x, y)    \
    (img->bpp == 1 ? img->pixels[((y)*img->w)+(x)] : (COLOR_RGB565_TO_Y(((uint16_t*)img->pixels)[((y)*img->w)+(x)]) + 128))

typedef struct simple_color {
    uint8_t G;          // Gray
    union {
        int8_t L;       // LAB L
        uint8_t red;    // RGB888 Red
    };
    union {
        int8_t A;       // LAB A
        uint8_t green;  // RGB888 Green
    };
    union {
        int8_t B;       // LAB B
        uint8_t blue;   // RGB888 Blue
    };
} simple_color_t;

typedef struct integral_image {
    int w;
    int h;
    uint32_t *data;
} i_image_t;

typedef struct {
    int w;
    int h;
    int y_offs;
    int x_ratio;
    int y_ratio;
    uint32_t **data;
    uint32_t **swap;
} mw_image_t;

typedef struct _vector {
    float x;
    float y;
    float m;
    uint16_t cx,cy;
} vec_t;

typedef struct cluster {
    int x, y, w, h;
    array_t *points;
} cluster_t;

// Return the distance between a cluster centroid and some object.
typedef float (*cluster_dist_t)(int cx, int cy, void *obj);

/* Keypoint */
typedef struct kp {
    uint16_t x;
    uint16_t y;
    uint16_t score;
    uint16_t octave;
    uint16_t angle;
    uint16_t matched;
    uint8_t desc[32];
} kp_t;

typedef struct size {
    int w;
    int h;
} wsize_t;

/* Haar cascade struct */
typedef struct cascade {
    int std;                        // Image standard deviation.
    int step;                       // Image scanning factor.
    float threshold;                // Detection threshold.
    float scale_factor;             // Image scaling factor.
    int n_stages;                   // Number of stages in the cascade.
    int n_features;                 // Number of features in the cascade.
    int n_rectangles;               // Number of rectangles in the cascade.
    struct size window;             // Detection window size.
    struct image *img;              // Grayscale image.
    mw_image_t *sum;                // Integral image.
    mw_image_t *ssq;                // Squared integral image.
    uint8_t *stages_array;          // Number of features per stage.
    int16_t *stages_thresh_array;   // Stages thresholds.
    int16_t *tree_thresh_array;     // Features threshold (1 per feature).
    int16_t *alpha1_array;          // Alpha1 array (1 per feature).
    int16_t *alpha2_array;          // Alpha2 array (1 per feature).
    int8_t *num_rectangles_array;   // Number of rectangles per features (1 per feature).
    int8_t *weights_array;          // Rectangles weights (1 per rectangle).
    int8_t *rectangles_array;       // Rectangles array.
} cascade_t;

typedef struct bmp_read_settings {
    int32_t bmp_w;
    int32_t bmp_h;
    uint16_t bmp_bpp;
    uint32_t bmp_fmt;
    uint32_t bmp_row_bytes;
} bmp_read_settings_t;

typedef struct ppm_read_settings {
    uint8_t read_int_c;
    bool read_int_c_valid;
    uint8_t ppm_fmt;
} ppm_read_settings_t;

typedef enum save_image_format {
    FORMAT_DONT_CARE,
    FORMAT_BMP,
    FORMAT_PNM,
    FORMAT_JPG,
    FORMAT_RAW,
} save_image_format_t;

typedef struct img_read_settings {
    union
    {
        bmp_read_settings_t bmp_rs;
        ppm_read_settings_t ppm_rs;
    };
    save_image_format_t format;
} img_read_settings_t;

typedef void (*line_op_t)(image_t*, int, void*, void*, bool);
typedef void (*flood_fill_call_back_t)(image_t *, int, int, int, void *);

typedef enum descriptor_type {
    DESC_LBP,
    DESC_ORB,
} descriptor_t;

typedef enum edge_detector_type {
   EDGE_CANNY,
   EDGE_SIMPLE,
} edge_detector_t;

typedef enum template_match {
    SEARCH_EX,  // Exhaustive search
    SEARCH_DS,  // Diamond search
} template_match_t;

typedef enum  jpeg_subsample {
    JPEG_SUBSAMPLE_1x1 = 0x11,  // 1x1 chroma subsampling (No subsampling)
    JPEG_SUBSAMPLE_2x1 = 0x21,  // 2x2 chroma subsampling
    JPEG_SUBSAMPLE_2x2 = 0x22,  // 2x2 chroma subsampling
} jpeg_subsample_t;

typedef enum corner_detector_type {
    CORNER_FAST,
    CORNER_AGAST
} corner_detector_t;

typedef struct histogram {
    int LBinCount;
    float *LBins;
    int ABinCount;
    float *ABins;
    int BBinCount;
    float *BBins;
} histogram_t;

typedef struct percentile {
    uint8_t LValue;
    int8_t AValue;
    int8_t BValue;
} percentile_t;

typedef struct threshold {
    uint8_t LValue;
    int8_t AValue;
    int8_t BValue;
} threshold_t;

typedef struct statistics {
    uint8_t LMean, LMedian, LMode, LSTDev, LMin, LMax, LLQ, LUQ;
    int8_t AMean, AMedian, AMode, ASTDev, AMin, AMax, ALQ, AUQ;
    int8_t BMean, BMedian, BMode, BSTDev, BMin, BMax, BLQ, BUQ;
} statistics_t;

#define FIND_BLOBS_CORNERS_RESOLUTION 20 // multiple of 4
#define FIND_BLOBS_ANGLE_RESOLUTION (360 / FIND_BLOBS_CORNERS_RESOLUTION)

typedef struct find_blobs_list_lnk_data {
    point_t corners[FIND_BLOBS_CORNERS_RESOLUTION];
    rectangle_t rect;
    uint32_t pixels, perimeter, code, count;
    float centroid_x, centroid_y, rotation, roundness;
    uint16_t x_hist_bins_count, y_hist_bins_count, *x_hist_bins, *y_hist_bins;
    float centroid_x_acc, centroid_y_acc, rotation_acc_x, rotation_acc_y, roundness_acc;
} find_blobs_list_lnk_data_t;

typedef struct find_lines_list_lnk_data {
    line_t line;
    uint32_t magnitude;
    int16_t theta, rho;
} find_lines_list_lnk_data_t;

typedef struct find_circles_list_lnk_data {
    point_t p;
    uint32_t r, magnitude;
} find_circles_list_lnk_data_t;

typedef struct find_rects_list_lnk_data {
    point_t corners[4];
    rectangle_t rect;
    uint32_t magnitude;
} find_rects_list_lnk_data_t;

typedef struct find_qrcodes_list_lnk_data {
    point_t corners[4];
    rectangle_t rect;
    size_t payload_len;
    char *payload;
    uint8_t version, ecc_level, mask, data_type;
    uint32_t eci;
} find_qrcodes_list_lnk_data_t;

typedef enum apriltag_families {
    TAG16H5 = 1,
    TAG25H7 = 2,
    TAG25H9 = 4,
    TAG36H10 = 8,
    TAG36H11 = 16,
    ARTOOLKIT = 32
} apriltag_families_t;

typedef struct find_apriltags_list_lnk_data {
    point_t corners[4];
    rectangle_t rect;
    uint16_t id;
    uint8_t family, hamming;
    point_t centroid;
    float goodness, decision_margin;
    float x_translation, y_translation, z_translation;
    float x_rotation, y_rotation, z_rotation;
} find_apriltags_list_lnk_data_t;

typedef struct find_datamatrices_list_lnk_data {
    point_t corners[4];
    rectangle_t rect;
    size_t payload_len;
    char *payload;
    uint16_t rotation;
    uint8_t rows, columns;
    uint16_t capacity, padding;
} find_datamatrices_list_lnk_data_t;

typedef enum barcodes {
    BARCODE_EAN2,
    BARCODE_EAN5,
    BARCODE_EAN8,
    BARCODE_UPCE,
    BARCODE_ISBN10,
    BARCODE_UPCA,
    BARCODE_EAN13,
    BARCODE_ISBN13,
    BARCODE_I25,
    BARCODE_DATABAR,
    BARCODE_DATABAR_EXP,
    BARCODE_CODABAR,
    BARCODE_CODE39,
    BARCODE_PDF417,
    BARCODE_CODE93,
    BARCODE_CODE128
} barcodes_t;

typedef struct find_barcodes_list_lnk_data {
    point_t corners[4];
    rectangle_t rect;
    size_t payload_len;
    char *payload;
    uint16_t type, rotation;
    int quality;
} find_barcodes_list_lnk_data_t;

/* Color space functions */
int8_t imlib_rgb565_to_l(uint16_t pixel);
int8_t imlib_rgb565_to_a(uint16_t pixel);
int8_t imlib_rgb565_to_b(uint16_t pixel);
int8_t imlib_rgb565_to_y(uint16_t pixel);
int8_t imlib_rgb565_to_u(uint16_t pixel);
int8_t imlib_rgb565_to_v(uint16_t pixel);
uint16_t imlib_lab_to_rgb(uint8_t l, int8_t a, int8_t b);
uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v);
void imlib_bayer_to_rgb565(image_t *img, int w, int h, int xoffs, int yoffs, uint16_t *rgbbuf);

/* Image file functions */
void imlib_image_operation(image_t *img, const char *path, image_t *other, int scalar, line_op_t op, void *data);


/* Color space functions 2 */
void imlib_rgb565_to_gray(uint32_t src_addr, char *gray, uint32_t sizeOfPixel);
void imlib_rgb888_to_gray(uint32_t src_addr, char *gray, uint32_t sizeOfPixel);
void imlib_rgb565_to_argb8888(uint32_t src_addr, uint32_t *argb, uint8_t alpha, uint32_t sizeOfPixel);
void imlib_argb8888_to_argb1555(uint32_t src_addr, uint16_t *argb1555, uint32_t sizeOfPixel);
void imlib_argb8888_to_argb4444(uint32_t src_addr, uint16_t *argb4444, uint32_t sizeOfPixel);
void imlib_argb8888_to_al88(uint32_t src_addr, uint16_t *al88, uint32_t sizeOfPixel);
void imlib_argb8888_to_al44(uint32_t src_addr, uint8_t *al44, uint32_t sizeOfPixel);
void imlib_rgb565_to_rgb888(uint32_t src_addr, char *rgb, uint32_t sizeOfPixel);




/* Rectangle functions */
rectangle_t *rectangle_alloc(int16_t x, int16_t y, int16_t w, int16_t h);
void rectangle_expand(rectangle_t *r, int x, int y);
array_t *rectangle_merge(array_t *rectangles);


/* Separable 2D convolution */
void imlib_sepconv3(image_t *img, const int8_t *krn, const float m, const int b);







// Integral moving window
void imlib_integral_mw_alloc(mw_image_t *sum, int w, int h);
void imlib_integral_mw_free(mw_image_t *sum);
void imlib_integral_mw_scale(rectangle_t *roi, mw_image_t *sum, int w, int h);
void imlib_integral_mw(image_t *src, mw_image_t *sum);
void imlib_integral_mw_sq(image_t *src, mw_image_t *sum);
void imlib_integral_mw_shift(image_t *src, mw_image_t *sum, int n);
void imlib_integral_mw_shift_sq(image_t *src, mw_image_t *sum, int n);
void imlib_integral_mw_ss(image_t *src, mw_image_t *sum, mw_image_t *ssq, rectangle_t *roi);
void imlib_integral_mw_shift_ss(image_t *src, mw_image_t *sum, mw_image_t *ssq, rectangle_t *roi, int n);
long imlib_integral_mw_lookup(mw_image_t *sum, int x, int y, int w, int h);

/* Haar */
void imlib_load_cascade(cascade_t *cascade, float threshold, float scale_factor);
array_t *imlib_detect_objects(struct image *image, struct cascade *cascade, struct rectangle *roi);

/* Corner detectors */
void fast_detect(image_t *img, array_t *keypoints, int threshold, int max_keypoints, bool nonmax_suppression);

/* ORB descriptor */
array_t *orb_find_keypoints(image_t *img, int threshold, int max_keypoints);
int orb_match_keypoints(array_t *kpts1, array_t *kpts2, int *match, int threshold, rectangle_t *r, point_t *c, int *angle);
int orb_filter_keypoints(array_t *kpts, rectangle_t *r, point_t *c);
float orb_cluster_dist(int cx, int cy, void *kp);
// Edge detection
//void imlib_edge_simple(image_t *src, rectangle_t *roi, int low_thresh, int high_thresh);
void imlib_edge_canny(image_t *src, rectangle_t *roi, int low_thresh, int high_thresh);

/* Drawing functions */
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c);
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c);
// Filtering Functions
void imlib_histeq(image_t *img, image_t *mask);
void imlib_clahe_histeq(image_t *img, float clip_limit, image_t *mask);
void imlib_mean_filter(image_t *img, const int ksize, bool threshold, int offset, bool invert, image_t *mask);
void imlib_median_filter(image_t *img, const int ksize, float percentile, bool threshold, int offset, bool invert, image_t *mask);
void imlib_mode_filter(image_t *img, const int ksize, bool threshold, int offset, bool invert, image_t *mask);
void imlib_midpoint_filter(image_t *img, const int ksize, float bias, bool threshold, int offset, bool invert, image_t *mask);
void imlib_morph(image_t *img, const int ksize, const int *krn, const float m, const int b, bool threshold, int offset, bool invert, image_t *mask);
void imlib_bilateral_filter(image_t *img, const int ksize, float color_sigma, float space_sigma, bool threshold, int offset, bool invert, image_t *mask);
void imlib_cartoon_filter(image_t *img, float seed_threshold, float floating_threshold, image_t *mask);
// Lens Correction
void imlib_rectify_map(image_t *img, float strength, float zoom, uint16_t *mapx, uint16_t *mapy);
void imlib_remap(image_t *src_img, image_t *dst_img, uint16_t *mapx, uint16_t *mapy);

int imlib_image_mean(image_t *src, int threshold, int *mean, int *mean_dev);


#endif //__IMLIB_H__
