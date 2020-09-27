/* This file is part of the OpenMV project.
 * Copyright (c) 2013-2018 Ibrahim Abdelkader <iabdalkader@openmv.io> & Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 */

#include "fsort.h"
#include "imlib.h"

 static inline uint32_t __SMLAD(
  uint32_t x,
  uint32_t y,
  uint32_t sum)
  {
    return ((uint32_t)(((((int32_t)x << 16) >> 16) * (((int32_t)y << 16) >> 16)) +
                       ((((int32_t)x      ) >> 16) * (((int32_t)y      ) >> 16)) +
                       ( ((int32_t)sum    )                                  )   ));
  }

void imlib_median_filter(image_t *img, const int ksize, float percentile, bool threshold, int offset, bool invert, image_t *mask)
{
    int brows = ksize + 1;
    image_t buf;
    buf.w = img->w;
    buf.h = brows;
    buf.bpp = img->bpp;

    int n = ((ksize*2)+1)*((ksize*2)+1);
    int int_percentile = fast_roundf(percentile * (n - 1));

    switch(img->bpp) {
        case IMAGE_BPP_GRAYSCALE: {
            buf.data = (uint8_t *)xalloc0(IMAGE_GRAYSCALE_LINE_LEN_BYTES(img) * brows );
            if(buf.data == NULL)
            {
                return;
            }
			
            int *data = (int *)xalloc0(n*sizeof(int));
            if(data == NULL)
            {
                xfree(buf.data);
                buf.data = NULL;
                return;
            }

            for (int y = 0, yy = img->h; y < yy; y++) {
                uint8_t *row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y);
                uint8_t *buf_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows));

                for (int x = 0, xx = img->w; x < xx; x++) {
                    if (mask && (!image_get_mask_pixel(mask, x, y))) {
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x));
                        continue; // Short circuit.
                    }

                    int *data_ptr = data;

                    for (int j = -ksize; j <= ksize; j++) {
                        uint8_t *k_row_ptr = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img,
                            IM_MIN(IM_MAX(y + j, 0), (img->h - 1)));

                        for (int k = -ksize; k <= ksize; k++) {
                            *data_ptr++ = IMAGE_GET_GRAYSCALE_PIXEL_FAST(k_row_ptr,
                                IM_MIN(IM_MAX(x + k, 0), (img->w - 1)));
                        }
                    }

                    fsort(data, n);

                    int pixel = data[int_percentile];

                    if (threshold) {
                        if (((pixel - offset) < IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x)) ^ invert) {
                            pixel = COLOR_GRAYSCALE_BINARY_MAX;
                        } else {
                            pixel = COLOR_GRAYSCALE_BINARY_MIN;
                        }
                    }

                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(buf_row_ptr, x, (uint8_t)pixel);
                }

                if (y >= ksize) { // Transfer buffer lines...
                    memcpy(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, (y - ksize)),
                           IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, ((y - ksize) % brows)),
                           IMAGE_GRAYSCALE_LINE_LEN_BYTES(img));
                }
            }

            // Copy any remaining lines from the buffer image...
            for (int y = img->h - ksize, yy = img->h; y < yy; y++) {
                memcpy(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, y),
                       IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&buf, (y % brows)),
                       IMAGE_GRAYSCALE_LINE_LEN_BYTES(img));
            }

            xfree(buf.data);
            buf.data = NULL;
            xfree(data);
            data = NULL;
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_gaussian_filter(image_t *img, const int8_t *krn, const float m, const int b)
{
    int ksize = 3;
    // TODO: Support RGB
    int *buffer = (int *)xalloc0(img->w * 2 * sizeof(*buffer));
    if(buffer == NULL)
    {
        return;
    }

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
    xfree(buffer);
    buffer = NULL;

    return;
}

