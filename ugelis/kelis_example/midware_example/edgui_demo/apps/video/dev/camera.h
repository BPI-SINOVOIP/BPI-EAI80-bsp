/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          camera.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of camera API
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __DEV_CAMERA_H__
#define __DEV_CAMERA_H__

#include <camera/dcmi.h>

typedef int (*camera_cb_func)(void *buffer, void *arg);

typedef struct dev_camera_cb
{
    camera_cb_func func;
    void *arg;
} dev_camera_cb_t;

typedef struct dev_camera
{
    struct device *dcmi;
    dev_camera_cb_t cam_cb;
    uint32_t mem_start;
    uint32_t buf_w;
    uint32_t buf_h;
    uint32_t buf_gray;
} dev_camera_t;

int camera_start(dev_camera_t *dev);
int camera_init(dev_camera_t *dev);
void camera_exit();

#endif //__DEV_CAMERA_H__

