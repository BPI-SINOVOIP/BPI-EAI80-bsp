/*
* Copyright (c) 2016 Linaro Limited.
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file Header file for GC0308 CMOS Image Sensor.
 */
#ifndef __GC0308_REG_H__
#define __GC0308_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* GC0308 reg list */
#define OUTPUT_FMT                  0x24

#define OUTPUT_FMT_YUV0             0x00
#define OUTPUT_FMT_YUV1             0x01
#define OUTPUT_FMT_YUV2             0x02/*used YUV format*/
#define OUTPUT_FMT_YUV3             0x03
#define OUTPUT_FMT_RGB565           0x06
#define OUTPUT_FMT_RGBx555          0x07
#define OUTPUT_FMT_RGB555x          0x08
#define OUTPUT_FMT_RGBx444          0x09
#define OUTPUT_FMT_RGB444x          0x0a
#define OUTPUT_FMT_RGBBGRG          0x0b
#define OUTPUT_FMT_RGBRGBG          0x0c
#define OUTPUT_FMT_RGBGBGR          0x0d
#define OUTPUT_FMT_RGBGRGB          0x0d
#define OUTPUT_FMT_BAYER            0x17 /*need to config P1:0x53[6:5]*/
#define OUTPUT_SET_FMT(r, x)      ((r&0xe0)|((x&0x1f)<<0))


#ifdef __cplusplus
}
#endif

#endif /* __OV5640_AF_FIRMWARE_H__ */
