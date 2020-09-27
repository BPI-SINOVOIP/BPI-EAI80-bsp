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

//#include <easynet/easynet.h>
#include <worker/worker.h>

typedef struct easynet_worker
{
    struct k_thread task_thread;
    k_tid_t task_id;

    easynet_proc_param_t ep;
    easynet_proc_func frame_in;
    easynet_proc_func exit;
    struct k_sem lock;
} easynet_worker_t;

#endif //__EASYNET_WORKER_H__