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

#include <worker/worker.h>
#include <camera/dcmi.h>

typedef int (*camera_cb_func)(void *buffer, void *arg);

typedef struct dev_camera_arg
{
    camera_cb_func cb;
    void *cb_arg;
    struct device *dcmi;
    uint32_t mem_start;
    uint32_t mem_size;
    uint32_t frames;    /* frame counts */
    uint32_t w;
    uint32_t h;
} dev_camera_arg_t;

typedef struct dev_camera
{
    dev_camera_arg_t cam;
    worker_frame_t frame;
    struct k_thread ai_thread;
    worker_t *self;
    int mem_buf;
    int mem_size;
    int running;
    worker_t *lcd;
    int status;
    struct k_sem lock;
} dev_camera_t;

enum
{
    CAM_STATUS_IDLE = 0,
    CAM_STATUS_READY,
};

int camera_start(dev_camera_arg_t *dev);
int camera_stop(dev_camera_arg_t *dev);
int camera_init(dev_camera_arg_t *dev);
void camera_exit(void);

#endif //__DEV_CAMERA_H__

