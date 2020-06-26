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

typedef struct pre_worker_info
{
    uint32_t ai_w;
    uint32_t ai_h;
    pre_proc_param_t pp;
    uint8_t *src;
    uint8_t *dst;
    pre_proc_func frame_in;
    pre_proc_func exit;
} pre_worker_info_t;


#endif //__PRE_WORKER_H__
