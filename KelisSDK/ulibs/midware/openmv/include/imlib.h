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
#include <fs.h>

#define IM_LOG2_2(x)    (((x) &                0x2ULL) ? ( 2                        ) :             1) // NO ({ ... }) !
#define IM_LOG2_4(x)    (((x) &                0xCULL) ? ( 2 +  IM_LOG2_2((x) >>  2)) :  IM_LOG2_2(x)) // NO ({ ... }) !
#define IM_LOG2_8(x)    (((x) &               0xF0ULL) ? ( 4 +  IM_LOG2_4((x) >>  4)) :  IM_LOG2_4(x)) // NO ({ ... }) !
#define IM_LOG2_16(x)   (((x) &             0xFF00ULL) ? ( 8 +  IM_LOG2_8((x) >>  8)) :  IM_LOG2_8(x)) // NO ({ ... }) !
#define IM_LOG2_32(x)   (((x) &         0xFFFF0000ULL) ? (16 + IM_LOG2_16((x) >> 16)) : IM_LOG2_16(x)) // NO ({ ... }) !
#define IM_LOG2(x)      (((x) & 0xFFFFFFFF00000000ULL) ? (32 + IM_LOG2_32((x) >> 32)) : IM_LOG2_32(x)) // NO ({ ... }) !

#define IM_MAX(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

// Grayscale maxes
#define IM_MAX_GS (255)

#define IM_X_INSIDE(img, x) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       (0<=_x)&&(_x<_img->w); })

#define IM_Y_INSIDE(img, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (y) _y = (y); \
       (0<=_y)&&(_y<_img->h); })

#define IM_IS_GS(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp == 1; })

#define IM_IS_RGB565(img) \
    ({ __typeof__ (img) _img = (img); \
       _img->bpp == 2; })

#define IM_SET_GS_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_GET_GS_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })
       
#define IM_SET_RGB565_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

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

#define COLOR_GRAYSCALE_BINARY_MIN 0x00
#define COLOR_GRAYSCALE_BINARY_MAX 0xFF
#define COLOR_GRAYSCALE_MIN 0
#define COLOR_GRAYSCALE_MAX 255

#define COLOR_GRAYSCALE_TO_BINARY(pixel) ((pixel) > (((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) / 2) + COLOR_GRAYSCALE_MIN))

#define IMAGE_GRAYSCALE_LINE_LEN(image) ((image)->w)
#define IMAGE_GRAYSCALE_LINE_LEN_BYTES(image) (IMAGE_GRAYSCALE_LINE_LEN(image) * sizeof(uint8_t))

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

/////////////////
// Point Stuff //
/////////////////
typedef struct point {
    int16_t x;
    int16_t y;
} point_t;

/////////////////////
// Rectangle Stuff //
/////////////////////
typedef struct rectangle {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} rectangle_t;

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
    uint16_t angle;
    uint16_t matched;
    uint8_t desc[32];
} kp_t;

bool image_get_mask_pixel(image_t *ptr, int x, int y);

/* Basic image functions */
void imlib_set_pixel(image_t *img, int x, int y, int p);

/* Rectangle functions */
rectangle_t *rectangle_alloc(int16_t x, int16_t y, int16_t w, int16_t h);
void rectangle_expand(rectangle_t *r, int x, int y);
array_t *rectangle_merge(array_t *rectangles);

/* Drawing functions */
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c);
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c);

/* Color space functions */
void imlib_rgb565_to_gray(uint32_t src_addr, char *gray, uint32_t sizeOfPixel);
void imlib_rgb888_to_gray(uint32_t src_addr, char *gray, uint32_t sizeOfPixel);
void imlib_rgb565_to_argb8888(uint32_t src_addr, uint32_t *argb, uint8_t alpha, uint32_t sizeOfPixel);
void imlib_argb8888_to_argb1555(uint32_t src_addr, uint16_t *argb1555, uint32_t sizeOfPixel);
void imlib_argb8888_to_argb4444(uint32_t src_addr, uint16_t *argb4444, uint32_t sizeOfPixel);
void imlib_argb8888_to_al88(uint32_t src_addr, uint16_t *al88, uint32_t sizeOfPixel);
void imlib_argb8888_to_al44(uint32_t src_addr, uint8_t *al44, uint32_t sizeOfPixel);
void imlib_rgb565_to_rgb888(uint32_t src_addr, char *rgb, uint32_t sizeOfPixel);

/* Corner detectors */
void fast_detect(image_t *img, array_t *keypoints, int threshold, int max_keypoints, bool nonmax_suppression);

/* ORB descriptor */
array_t *orb_find_keypoints(image_t *img, int threshold, int max_keypoints);
int orb_match_keypoints(array_t *kpts1, array_t *kpts2, int *match, int threshold, rectangle_t *r, point_t *c, int *angle);
int orb_filter_keypoints(array_t *kpts, rectangle_t *r, point_t *c);
float orb_cluster_dist(int cx, int cy, void *kp);

// Integral moving window
array_t *cluster_kmeans(array_t *points, int k, cluster_dist_t dist_func);

/* LBP Operator */
uint8_t *imlib_lbp_desc(image_t *image, rectangle_t *roi);
int imlib_lbp_desc_distance(uint8_t *d0, uint8_t *d1);
uint32_t imlib_lbp_desc_save(struct fs_file_t *zfp, uint8_t *desc);
uint32_t imlib_lbp_desc_load(struct fs_file_t *zfp, uint8_t *desc);

// Filtering Functions
void imlib_clahe_histeq(image_t *img, float clip_limit, image_t *mask);
void imlib_median_filter(image_t *img, const int ksize, float percentile, bool threshold, int offset, bool invert, image_t *mask);
void imlib_gaussian_filter(image_t *img, const int8_t *krn, const float m, const int b);
// Lens Correction
void imlib_rectify_map(image_t *img, float strength, float zoom, uint16_t *mapx, uint16_t *mapy);
void imlib_remap(image_t *src_img, image_t *dst_img, uint16_t *mapx, uint16_t *mapy);

#endif //__IMLIB_H__
