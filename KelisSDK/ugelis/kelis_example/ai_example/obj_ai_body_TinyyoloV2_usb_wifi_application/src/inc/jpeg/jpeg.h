/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _JPGE_H
#define _JPEG_H

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

void jpeg_init(int quality);
void jpeg_write(image_t *img, uint32_t dst,  int quality);

#endif /* _JPEG_H */
