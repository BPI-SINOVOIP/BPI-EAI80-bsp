/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          pre_worker.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of pre processing worker
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __PRE_WORKER_H__
#define __PRE_WORKER_H__

#include <faceai/face_detect.h>

typedef struct
{
    unsigned char *tmp;
    unsigned char *input;         /* buffer that is used to be stored one cropped face image */
    unsigned char *detected;        /* buffer that is used to be detected face */
} extra_buffer_t;

typedef struct pre_worker_info
{
    face_detect_t fd;
    extra_buffer_t buf;
    uint32_t src_size[2];
    uint32_t dst_size[2];
    struct k_thread task_thread;
} pre_worker_info_t;

#endif //__PRE_WORKER_H__
