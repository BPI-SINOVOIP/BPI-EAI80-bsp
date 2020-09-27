#include <ugelis.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <kernel.h>
#include <misc/printk.h>
#include <device.h>
#include <gm_common.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#define bool int
#define false 0
#define true 1
#define uint8_t  unsigned char
#define uint32_t  unsigned int
#define WIDTH    320
#define HEGIHT   240

typedef enum  jpeg_subsample
{
    JPEG_SUBSAMPLE_1x1 = 0x11,  // 1x1 chroma subsampling (No subsampling)
    JPEG_SUBSAMPLE_2x1 = 0x21,  // 2x2 chroma subsampling
    JPEG_SUBSAMPLE_2x2 = 0x22,  // 2x2 chroma subsampling
} jpeg_subsample_t;

typedef struct
{
    int idx;
    int length;
    uint8_t *buf;
    int bitc, bitb;
    bool realloc;
    bool overflow;
} jpeg_buf_t;

typedef struct image
{
    int w;
    int h;
    int bpp;
    union
    {
        uint8_t *pixels;
        uint8_t *data;
    };
} image_t;

void jpeg_encode(image_t *img, uint8_t *buffer, int quality);
bool openmv_jpeg_compress(image_t *src, image_t *dst, int quality, bool realloc);
