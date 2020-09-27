/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        app_processor.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/10/15
 *
 * @brief       app_processor source file
 *
 * @note
 *              2019/10/15, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include "app_processor.h"
#include <imlib.h>

/**
*
* This function run average pooling
*
* @param a array
*
* @return
*
*
* @note     None.
*
******************************************************************************/

/*Feed rrggbb format to KDP310 for further training */
void rgb_split2rrggbb(uint8_t *src, uint8_t *dst, int w, int h)
{
    uint8_t *dst_b = &(dst[0]);
    uint8_t *dst_g = &(dst[w * h]);
    uint8_t *dst_r = &(dst[w * h * 2]);

    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = 0;
    for (l = 0; l < h; l++)
    {
        for (c = 0; c < w; c++)
        {
            src_rgb = src + (l * w + c) * 3;
            *(dst_b + i) = *(src_rgb + 2);    //b
            *(dst_g + i) = *(src_rgb + 1);    //g
            *(dst_r + i) = *(src_rgb);        //r
            i++;
        }
    }
}

static float gm_exp(float x)
{
    x = 1.0 + x / 256.0;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    return x;
}

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h)
{
    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = src;
    uint8_t *dst_rgb = dst;

    for (i = 0; i < w * h; i ++)
    {
        *dst_rgb = *(src_rgb +  w * h * 2 + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + w * h + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + i);
        dst_rgb ++;
    }
}

int imlib_image_mean(image_t *src, int threshold, int *mean, int *mean_dev)
{
    int w = src->w;
    int h = src->h;
    int n = w * h;
    uint8_t *data = src->pixels;

    uint32_t s = 0;
    int sum = 0;

    for (int i = 0; i < n; i += 2)
    {
        s += data[i + 0] + data[i + 1];
    }

    if (n % 2)
    {
        s += data[n - 1];
    }

    /* mean */
    *mean = s / n - threshold;

    for (int j = 0; j < n; j++)
    {
        int diff = data[j] - 128;
        int abs_tmp = abs(diff - (*mean));
        sum += abs_tmp;
    }

    *mean_dev = sum / n;
}
