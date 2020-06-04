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
#include <imlib.h>

/////////////////
// Image Stuff //
/////////////////

void image_init(image_t *ptr, int w, int h, int bpp, void *data)
{
    ptr->w = w;
    ptr->h = h;
    ptr->bpp = bpp;
    ptr->data = data;
}

void image_copy(image_t *dst, image_t *src)
{
    memcpy(dst, src, sizeof(image_t));
}

size_t image_size(image_t *ptr)
{
    switch (ptr->bpp) {
        case IMAGE_BPP_BINARY: {
            return IMAGE_BINARY_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case IMAGE_BPP_GRAYSCALE: {
            return IMAGE_GRAYSCALE_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case IMAGE_BPP_RGB565: {
            return IMAGE_RGB565_LINE_LEN_BYTES(ptr) * ptr->h;
        }
        case IMAGE_BPP_BAYER: {
            return ptr->w * ptr->h;
        }
        default: { // JPEG
            return ptr->bpp;
        }
    }
}

bool image_get_mask_pixel(image_t *ptr, int x, int y)
{
    if ((0 <= x) && (x < ptr->w) && (0 <= y) && (y < ptr->h)) {
        switch (ptr->bpp) {
            case IMAGE_BPP_BINARY: {
                return IMAGE_GET_BINARY_PIXEL(ptr, x, y);
            }
            case IMAGE_BPP_GRAYSCALE: {
                return COLOR_GRAYSCALE_TO_BINARY(IMAGE_GET_GRAYSCALE_PIXEL(ptr, x, y));
            }
            case IMAGE_BPP_RGB565: {
                return COLOR_RGB565_TO_BINARY(IMAGE_GET_RGB565_PIXEL(ptr, x, y));
            }
            default: {
                return false;
            }
        }
    }

    return false;
}

// Gamma uncompress
extern const float xyz_table[256];

const int8_t kernel_gauss_3[3*3] = {
     1, 2, 1,
     2, 4, 2,
     1, 2, 1,
};

const int8_t kernel_gauss_5[5*5] = {
    1,  4,  6,  4, 1,
    4, 16, 24, 16, 4,
    6, 24, 36, 24, 6,
    4, 16, 24, 16, 4,
    1,  4,  6,  4, 1
};

const int kernel_laplacian_3[3*3] = {
     -1, -1, -1,
     -1,  8, -1,
     -1, -1, -1
};

const int kernel_high_pass_3[3*3] = {
    -1, -1, -1,
    -1, +8, -1,
    -1, -1, -1
};

int8_t imlib_rgb565_to_l(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return fast_floorf(116 * y) - 16;
}

int8_t imlib_rgb565_to_a(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float x = ((r_lin * 0.4124f) + (g_lin * 0.3576f) + (b_lin * 0.1805f)) * (1.0f / 095.047f);
    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    x = (x>0.008856f) ? fast_cbrtf(x) : ((x * 7.787037f) + 0.137931f);
    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return fast_floorf(500 * (x-y));
}

int8_t imlib_rgb565_to_b(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);
    float z = ((r_lin * 0.0193f) + (g_lin * 0.1192f) + (b_lin * 0.9505f)) * (1.0f / 108.883f);

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);
    z = (z>0.008856f) ? fast_cbrtf(z) : ((z * 7.787037f) + 0.137931f);

    return fast_floorf(200 * (y-z));
}

int8_t imlib_rgb565_to_y(uint16_t pixel)
{
    int r = COLOR_RGB565_TO_R8(pixel);
    int g = COLOR_RGB565_TO_G8(pixel);
    int b = COLOR_RGB565_TO_B8(pixel);

    return fast_floorf((r * +0.299000f) + (g * +0.587000f) + (b * +0.114000f)) - 128;
}

int8_t imlib_rgb565_to_u(uint16_t pixel)
{
    int r = COLOR_RGB565_TO_R8(pixel);
    int g = COLOR_RGB565_TO_G8(pixel);
    int b = COLOR_RGB565_TO_B8(pixel);

    return fast_floorf((r * -0.168736f) + (g * -0.331264f) + (b * +0.500000f));
}

int8_t imlib_rgb565_to_v(uint16_t pixel)
{
    int r = COLOR_RGB565_TO_R8(pixel);
    int g = COLOR_RGB565_TO_G8(pixel);
    int b = COLOR_RGB565_TO_B8(pixel);

    return fast_floorf((r * +0.500000f) + (g * -0.418688f) + (b * -0.081312f));
}

// https://en.wikipedia.org/wiki/Lab_color_space -> CIELAB-CIEXYZ conversions
// https://en.wikipedia.org/wiki/SRGB -> Specification of the transformation
uint16_t imlib_lab_to_rgb(uint8_t l, int8_t a, int8_t b)
{
    float x = ((l + 16) * 0.008621f) + (a * 0.002f);
    float y = ((l + 16) * 0.008621f);
    float z = ((l + 16) * 0.008621f) - (b * 0.005f);

    x = ((x > 0.206897f) ? (x*x*x) : ((0.128419f * x) - 0.017713f)) * 095.047f;
    y = ((y > 0.206897f) ? (y*y*y) : ((0.128419f * y) - 0.017713f)) * 100.000f;
    z = ((z > 0.206897f) ? (z*z*z) : ((0.128419f * z) - 0.017713f)) * 108.883f;

    float r_lin = ((x * +3.2406f) + (y * -1.5372f) + (z * -0.4986f)) / 100.0f;
    float g_lin = ((x * -0.9689f) + (y * +1.8758f) + (z * +0.0415f)) / 100.0f;
    float b_lin = ((x * +0.0557f) + (y * -0.2040f) + (z * +1.0570f)) / 100.0f;

    r_lin = (r_lin>0.0031308f) ? ((1.055f*powf(r_lin, 0.416666f))-0.055f) : (r_lin*12.92f);
    g_lin = (g_lin>0.0031308f) ? ((1.055f*powf(g_lin, 0.416666f))-0.055f) : (g_lin*12.92f);
    b_lin = (b_lin>0.0031308f) ? ((1.055f*powf(b_lin, 0.416666f))-0.055f) : (b_lin*12.92f);

    uint32_t red   = IM_MAX(IM_MIN(fast_floorf(r_lin * COLOR_R8_MAX), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t green = IM_MAX(IM_MIN(fast_floorf(g_lin * COLOR_G8_MAX), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t blue  = IM_MAX(IM_MIN(fast_floorf(b_lin * COLOR_B8_MAX), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(red, green, blue);
}

// https://en.wikipedia.org/wiki/YCbCr -> JPEG Conversion
uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v)
{
    uint32_t r = IM_MAX(IM_MIN(y + ((91881 * v) >> 16), COLOR_R8_MAX), COLOR_R8_MIN);
    uint32_t g = IM_MAX(IM_MIN(y - (((22554 * u) + (46802 * v)) >> 16), COLOR_G8_MAX), COLOR_G8_MIN);
    uint32_t b = IM_MAX(IM_MIN(y + ((116130 * u) >> 16), COLOR_B8_MAX), COLOR_B8_MIN);

    return COLOR_R8_G8_B8_TO_RGB565(r, g, b);
}

void imlib_bayer_to_rgb565(image_t *img, int w, int h, int xoffs, int yoffs, uint16_t *rgbbuf)
{
    int r, g, b;
    for (int y=yoffs; y<yoffs+h; y++) {
        for (int x=xoffs; x<xoffs+w; x++) {
            if ((y % 2) == 0) { // Even row
                if ((x % 2) == 0) { // Even col
                    r = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y+1)) >> 2;

                    g = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 2;

                    b = IM_GET_RAW_PIXEL(img,  x, y);
                } else { // Odd col
                    r = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)) >> 1;

                    b = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 1;

                    g =  IM_GET_RAW_PIXEL(img, x, y);
                }
            } else { // Odd row
                if ((x % 2) == 0) { // Even Col
                    r = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 1;

                    g =  IM_GET_RAW_PIXEL(img, x, y);

                    b = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)) >> 1;
                } else { // Odd col
                    r = IM_GET_RAW_PIXEL(img,  x, y);

                    g = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 2;

                    b = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y+1)) >> 2;
                }

            }
            r = IM_R825(r);
            g = IM_G826(g);
            b = IM_B825(b);
            *rgbbuf++ = IM_RGB565(r, g, b);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////



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

void imlib_image_operation(image_t *img, const char *path, image_t *other, int scalar, line_op_t op, void *data)
{
    printf("!! imlib_image_operation() not implement.\n");
}


void imlib_sepconv3(image_t *img, const int8_t *krn, const float m, const int b)
{
    int ksize = 3;
    // TODO: Support RGB
    int *buffer = (int *)malloc(img->w * 2 * sizeof(*buffer));
    memset(buffer, 0, img->w * 2 * sizeof(*buffer));

    // NOTE: This doesn't deal with borders right now. Adding if
    // statements in the inner loop will slow it down significantly.
    for (int y=0; y<img->h-ksize; y++) {
        for (int x=0; x<img->w-ksize; x+=ksize) {
            for (int k=0; k<ksize; k++) {
                int acc=0;
                //if (IM_X_INSIDE(img, x+k) && IM_Y_INSIDE(img, y+j))
                acc = __SMLAD(krn[0], IM_GET_GS_PIXEL(img, x+k, y+0), acc);
                acc = __SMLAD(krn[1], IM_GET_GS_PIXEL(img, x+k, y+1), acc);
                acc = __SMLAD(krn[2], IM_GET_GS_PIXEL(img, x+k, y+2), acc);
                buffer[((y%2)*img->w) + x+k] = acc;
            }
        }
        if (y > 0) {
            // flush buffer
            for (int x=0; x<img->w-ksize; x++) {
                int acc = 0;
                acc = __SMLAD(krn[0], buffer[((y-1)%2) * img->w + x + 0], acc);
                acc = __SMLAD(krn[1], buffer[((y-1)%2) * img->w + x + 1], acc);
                acc = __SMLAD(krn[2], buffer[((y-1)%2) * img->w + x + 2], acc);
                acc = (acc * m) + b; // scale, offset, and clamp
                acc = IM_MAX(IM_MIN(acc, IM_MAX_GS), 0);
                IM_SET_GS_PIXEL(img, (x+1), (y), acc);
            }
        }
    }
    free(buffer);
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





//////////////////////////////////////////////////////////
//////////////// rectangle functions /////////////////////
//////////////////////////////////////////////////////////

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



//////////////////////////////////////////////////////////
///////////////// draw functions /////////////////////////
//////////////////////////////////////////////////////////

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
//////////////////////////////////////////////////////////

int imlib_image_mean(image_t *src, int threshold, int *mean, int *mean_dev)
{
    int w = src->w;
    int h = src->h;
    int n = w*h;
    uint8_t *data = src->pixels;
    
    uint32_t s = 0;
    int sum = 0;
    
    for (int i=0; i<n; i+=2) {
        s += data[i+0]+data[i+1];
    }
    
    if (n%2) {
        s += data[n-1];
    }
    
    /* mean */
    *mean = s/n - threshold;

    for (int j=0; j<n; j++)
    {
        int diff = data[j] - 128;
        int abs_tmp = abs(diff - (*mean));
        sum += abs_tmp;
    }

    *mean_dev = sum/n;
}
