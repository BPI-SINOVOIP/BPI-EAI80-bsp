/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          easynet_worker.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of easynet_worker
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __EASYNET_WORKER_H__
#define __EASYNET_WORKER_H__

#include "kernel.h"
//#include <easynet/easynet.h>

#include "processor.h"

typedef struct easynet_worker
{
    int exit;
    char *ops;
    struct easynet_dev *dev;
    struct k_sem predict_lock;
    struct k_thread task_thread;
} easynet_worker_t;

#endif //__EASYNET_WORKER_H__