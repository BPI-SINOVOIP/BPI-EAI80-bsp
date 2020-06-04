/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          image.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of Imgae preprocessor
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */


#ifndef __IMAGE_H__
#define __IMAGE_H__

void image_zoom(unsigned char *pSrcImg, unsigned char *pDstImg, int nSrcWidth, int nSrcHeight, int dst_w, int dst_h);
void image_crop(unsigned char *src, unsigned char *dst_addr, int x, int y, int w, int h, unsigned int src_w);
void image_split(unsigned char *src, unsigned char *dst, int w, int h);

#endif //__IMAGE_H__
