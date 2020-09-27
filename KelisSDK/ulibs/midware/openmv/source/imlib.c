/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image library.
 *
 */
#include <stdlib.h>
#include "array.h"
#include "imlib.h"

/////////////////
// Image Stuff //
/////////////////
// Set pixel (handles boundary check and image type check).
void imlib_set_pixel(image_t *img, int x, int y, int p)
{
    if (IM_X_INSIDE(img, x) && IM_Y_INSIDE(img, y)) {
        if (IM_IS_GS(img)) {
            IM_SET_GS_PIXEL(img, x, y, p);
        } else {
            IM_SET_RGB565_PIXEL(img, x, y, p);
        }
    }
}

void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c)
{
    if (rw<=0 || rh<=0) {
        return;
    }
    for (int i=rx, j=rx+rw, k=ry+rh-1; i<j; i++) {
        imlib_set_pixel(img, i, ry, c);
        imlib_set_pixel(img, i, k, c);
    }
    for (int i=ry+1, j=ry+rh-1, k=rx+rw-1; i<j; i++) {
        imlib_set_pixel(img, rx, i, c);
        imlib_set_pixel(img, k, i, c);
    }
}

void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c)
{
    int x = r, y = 0, radiusError = 1-x;
    while (x>=y) {
        imlib_set_pixel(img,  x + cx,  y + cy, c);
        imlib_set_pixel(img,  y + cx,  x + cy, c);
        imlib_set_pixel(img, -x + cx,  y + cy, c);
        imlib_set_pixel(img, -y + cx,  x + cy, c);
        imlib_set_pixel(img, -x + cx, -y + cy, c);
        imlib_set_pixel(img, -y + cx, -x + cy, c);
        imlib_set_pixel(img,  x + cx, -y + cy, c);
        imlib_set_pixel(img,  y + cx, -x + cy, c);
        y++;
        if (radiusError<0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

bool image_get_mask_pixel(image_t *ptr, int x, int y)
{
    if ((0 <= x) && (x < ptr->w) && (0 <= y) && (y < ptr->h)) {
        switch (ptr->bpp) {
            case IMAGE_BPP_GRAYSCALE: {
                return COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL(ptr, x, y));
            }
            default: {
                return false;
            }
        }
    }

    return false;
}

void imlib_rgb565_to_gray(uint32_t src_addr, char *gray, uint32_t sizeOfPixel)
{
    uint8_t red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        red   = ((*(uint16_t *)src_addr)&0xf800) >> 8;
        green = ((*(uint16_t *)src_addr)&0x07e0) >> 3;
        blue  = ((*(uint16_t *)src_addr)&0x001f) << 3;
        gray[i] = (uint8_t)((red*77 + green*150 + blue*29 + 128) >> 8);
        src_addr += 2;
    }
}

void imlib_rgb565_to_rgb888(uint32_t src_addr, char *rgb, uint32_t sizeOfPixel)
{
    uint8_t red, green, blue;
    int i, j = 0;

    for(i=0, j=0; i<sizeOfPixel; i++)
    {
        red   = ((*(uint16_t *)src_addr)&0xf800) >> 8;
        green = ((*(uint16_t *)src_addr)&0x07e0) >> 3;
        blue  = ((*(uint16_t *)src_addr)&0x001f) << 3;
        rgb[j] = blue;
        rgb[j+1] = green;
        rgb[j+2] = red;
        j += 3;
        src_addr += 2;
    }
}

void imlib_rgb888_to_gray(uint32_t src_addr, char *gray, uint32_t sizeOfPixel)
{
    uint8_t red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        blue  = (*(uint8_t *)src_addr);
        src_addr++;
        green = (*(uint8_t *)src_addr);
        src_addr++;
        red   = (*(uint8_t *)src_addr);
        src_addr++;
        gray[i] = (uint8_t)((red*77 + green*150 + blue*29 + 128) >> 8);
    }
}

void imlib_rgb565_to_argb8888(uint32_t src_addr, uint32_t *argb, uint8_t alpha, uint32_t sizeOfPixel)
{
    uint8_t red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        red   = ((*(uint16_t *)src_addr)&0xf800) >> 8;
        green = ((*(uint16_t *)src_addr)&0x07e0) >> 3;
        blue  = ((*(uint16_t *)src_addr)&0x001f) << 3;
        argb[i] = (uint32_t)((alpha << 24) | (red << 16) | (green << 8) | (blue));
        src_addr += 2;
    }
}

void imlib_argb8888_to_argb1555(uint32_t src_addr, uint16_t *argb1555, uint32_t sizeOfPixel)
{
    uint8_t alpha, red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        blue  = (*(uint8_t *)src_addr);
        src_addr++;
        green = (*(uint8_t *)src_addr);
        src_addr++;
        red   = (*(uint8_t *)src_addr);
        src_addr++;
        alpha   = (*(uint8_t *)src_addr);
        src_addr++;
        argb1555[i] = (uint16_t)((alpha << 15) | ((red >> 3) << 10) | ((green >> 3) << 5) | (blue >> 3));
    }
}

void imlib_argb8888_to_argb4444(uint32_t src_addr, uint16_t *argb4444, uint32_t sizeOfPixel)
{
    uint8_t alpha, red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        blue  = (*(uint8_t *)src_addr);
        src_addr++;
        green = (*(uint8_t *)src_addr);
        src_addr++;
        red   = (*(uint8_t *)src_addr);
        src_addr++;
        alpha   = (*(uint8_t *)src_addr);
        src_addr++;
        argb4444[i] = (uint16_t)(((alpha >> 4) << 12) | ((red >> 4) << 8) | ((green >> 4) << 4) | (blue >> 4));
    }
}

void imlib_argb8888_to_al88(uint32_t src_addr, uint16_t *al88, uint32_t sizeOfPixel)
{
    uint8_t alpha, red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        blue  = (*(uint8_t *)src_addr);
        src_addr++;
        green = (*(uint8_t *)src_addr);
        src_addr++;
        red   = (*(uint8_t *)src_addr);
        src_addr++;
        alpha   = (*(uint8_t *)src_addr);
        src_addr++;
        al88[i] = (uint16_t)((alpha << 8) | ((red*77 + green*150 + blue*29 + 128) >> 8));
    }
}

void imlib_argb8888_to_al44(uint32_t src_addr, uint8_t *al44, uint32_t sizeOfPixel)
{
    uint8_t alpha, red, green, blue;
    int i = 0;

    for(i=0; i<sizeOfPixel; i++)
    {
        blue  = (*(uint8_t *)src_addr);
        src_addr++;
        green = (*(uint8_t *)src_addr);
        src_addr++;
        red   = (*(uint8_t *)src_addr);
        src_addr++;
        alpha   = (*(uint8_t *)src_addr);
        src_addr++;
        al44[i] = (uint8_t)(((alpha >> 4) << 4) | (((red*77 + green*150 + blue*29 + 128) >> 8) >> 4));
    }
}


//////////////////////////////////////////////////////////
/////////////////// map functions ////////////////////////
//////////////////////////////////////////////////////////

// A simple algorithm for correcting lens distortion.
// See http://www.tannerhelland.com/4743/simple-algorithm-correcting-lens-distortion/
void imlib_rectify_map(image_t *img, float strength, float zoom, uint16_t *mapx, uint16_t *mapy)
{
    zoom = 1 / zoom;
    int halfWidth = img->w / 2;
    int halfHeight = img->h / 2;
    float lens_corr_radius = strength / fast_sqrtf((img->w * img->w) + (img->h * img->h));

    for (int y = 0, yy = img->h; y < yy; y++) {
        uint16_t *x_ptr = mapx + y * img->w;
        uint16_t *y_ptr = mapy + y * img->w;
        int newY = y - halfHeight;
        int newY2 = newY * newY;
        float zoomedY = newY * zoom;
        
        for (int x = 0, xx = img->w; x < xx; x++) {
            int newX = x - halfWidth;
            int newX2 = newX * newX;
            float zoomedX = newX * zoom;
            
            float r = lens_corr_radius * fast_sqrtf(newX2 + newY2);
            float theta = (r < 0.0000001f) ? 1.0f : (fast_atanf(r) / r);
            int sourceX = halfWidth + fast_roundf(theta * zoomedX);
            int sourceY = halfHeight + fast_roundf(theta * zoomedY);
            x_ptr[x] = sourceX;
            y_ptr[x] = sourceY;
        }
    }
}

void imlib_remap(image_t *src_img, image_t *dst_img, uint16_t *mapx, uint16_t *mapy)
{
    switch(src_img->bpp) {
        case IMAGE_BPP_GRAYSCALE: {
            for (int y = 0, yy = src_img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(dst_img, y);
                for (int x = 0, xx = src_img->w; x < xx; x++) {
                    int sourceX = *(uint16_t *)(mapx + y * src_img->w + x);
                    int sourceY = *(uint16_t *)(mapy + y * src_img->w + x);

                    if ((0 <= sourceX) && (sourceX < src_img->w) && (0 <= sourceY) && (sourceY < src_img->h)) {
                        uint8_t *ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(src_img, sourceY);
                        int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, sourceX);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, pixel);
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_RGB565: {
            for (int y = 0, yy = src_img->h; y < yy; y++) {
                uint16_t *row_ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(dst_img, y);

                for (int x = 0, xx = src_img->w; x < xx; x++) {
                    int sourceX = *(uint16_t *)(mapx + y * src_img->w + x);
                    int sourceY = *(uint16_t *)(mapy + y * src_img->w + x);

                    if ((0 <= sourceX) && (sourceX < src_img->w) && (0 <= sourceY) && (sourceY < src_img->h)) {
                        uint16_t *ptr = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(src_img, sourceY);
                        int pixel = IMAGE_GET_RGB565_PIXEL_FAST(ptr, sourceX);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, pixel);
                    }
                }
            }
            break;
        }
        case IMAGE_BPP_BAYER: {
            for (int y = 0, yy = src_img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(dst_img, y);

                for (int x = 0, xx = src_img->w; x < xx; x++) {
                    int sourceX = *(uint16_t *)(mapx + y * src_img->w + x);
                    int sourceY = *(uint16_t *)(mapy + y * src_img->w + x);

                    if ((0 <= sourceX) && (sourceX < src_img->w) && (0 <= sourceY) && (sourceY < src_img->h)) {
                        uint8_t *ptr = IMAGE_COMPUTE_RGB888_PIXEL_ROW_PTR(src_img, sourceY);
                        int pixel_b = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, sourceX*3);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x*3, pixel_b);
                        int pixel_g = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, sourceX*3+1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x*3+1, pixel_g);
                        int pixel_r = IMAGE_GET_GRAYSCALE_PIXEL_FAST(ptr, sourceX*3+2);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x*3+2, pixel_r);
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

rectangle_t *rectangle_alloc(int16_t x, int16_t y, int16_t w, int16_t h)
{
    rectangle_t *r = xalloc(sizeof(rectangle_t));
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
    return r;
}

static bool rectangle_intersects(rectangle_t *r1, rectangle_t *r2)
{
    return  ((r1->x < (r2->x+r2->w)) &&
             (r1->y < (r2->y+r2->h)) &&
             ((r1->x+r1->w) > r2->x) &&
             ((r1->y+r1->h) > r2->y));
}

// This isn't for actually combining the rects standardly, but, to instead
// find the average rectangle between a bunch of overlapping rectangles.
static void rectangle_add(rectangle_t *r1, rectangle_t *r2)
{
    r1->x += r2->x;
    r1->y += r2->y;
    r1->w += r2->w;
    r1->h += r2->h;
}

// This isn't for actually combining the rects standardly, but, to instead
// find the average rectangle between a bunch of overlapping rectangles.
static void rectangle_div(rectangle_t *r, int c)
{
    r->x /= c;
    r->y /= c;
    r->w /= c;
    r->h /= c;
}

array_t *rectangle_merge(array_t *rectangles)
{
    array_t *objects; array_alloc(&objects, xfree);
    array_t *overlap; array_alloc(&overlap, xfree);
    /* merge overlaping detections */
    while (array_length(rectangles)) {
        /* check for overlaping detections */
        rectangle_t *rect = (rectangle_t *) array_take(rectangles, 0);
        for (int j=0; j<array_length(rectangles); j++) { // do not cache bound
            if (rectangle_intersects(rect, (rectangle_t *) array_at(rectangles, j))) {
                array_push_back(overlap, array_take(rectangles, j--));
            }
        }
        /* add the overlaping detections */
        int count = array_length(overlap);
        for (int i=0; i<count; i++) {
            rectangle_t *overlap_rect = (rectangle_t *) array_pop_back(overlap);
            rectangle_add(rect, overlap_rect);
            xfree(overlap_rect);
        }
        /* average the overlaping detections */
        rectangle_div(rect, count + 1);
        array_push_back(objects, rect);
    }
    array_free(rectangles);
    array_free(overlap);
    return objects;
}

// Expands a bounding box with a point.
// After adding all points sub x from w and y from h.
void rectangle_expand(rectangle_t *r, int x, int y)
{
    if (x < r->x) {
        r->x = x;
    }
    if (y < r->y) {
        r->y = y;
    }
    if (x > r->w) {
        r->w = x;
    }
    if (y > r->h) {
        r->h = y;
    }
}

