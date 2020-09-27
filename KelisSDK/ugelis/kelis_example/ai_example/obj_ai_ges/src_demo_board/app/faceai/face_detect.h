/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          face_detect.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of face detector
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef _FACE_DETECT_H__
#define _FACE_DETECT_H__
#include <stdint.h>

typedef int (*face_detect_cb)(struct face_detect *fd, void *param);

typedef struct face_detect
{
    uint32_t exit;
    uint32_t frame_ready;
    uint32_t detected_buf;
    face_detect_cb finish_cb;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
} face_detect_t;

void face_detect_thread(void *data, void *param, void *p3);

#endif //_FACE_DETECT_H__
