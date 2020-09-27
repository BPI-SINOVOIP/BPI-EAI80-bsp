/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          worker.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         worker interface definitions
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __WORKER_H__
#define __WORKER_H__

#include <inc/common.h>

enum
{
    /* event_worker */
    CMD_EVENT_SHORTPRESS,
    CMD_EVENT_LONGPRESS,

    /* loader_worker */
    CMD_LOADER_READ,
    CMD_LOADER_WRITE,

    /* easynet worker */
    CMD_EASYNET_DATAREADY,
    CMD_EASYNET_FRAMEREADY,

    /* post worker */
    CMD_POST_CAP,
    CMD_POST_NORMALFRAME,
    CMD_POST_FACEFRAME,

    /* painter worker */
    CMD_PAINT_CAP,
    CMD_PAINT_PASS,
    CMD_PAINT_CLEAR,
    CMD_PAINT_TEXT,
};

typedef struct worker_io
{
    int size;
    int src;
    int dst;
} worker_io_t;

typedef struct worker_buf
{
    int orig;
    int gray;
    int fd;
} worker_buf_t;

typedef int (*worker_run_functio)(struct worker *self, int cmd, void *param);
typedef int (*worker_stop_functio)(struct worker *self, int cmd, void *param);
typedef int (*worker_init_functio)(struct worker *self, void *param);
typedef void *(*worker_fetch_function)(struct worker *self, void *param);
typedef int (*worker_deinit_functio)(struct worker *self, void *param);

typedef struct worker
{
    worker_run_functio      run;
    worker_stop_functio     stop;
    worker_init_functio     init;
    worker_fetch_function   fetch;
    worker_init_functio     deinit;
    struct worker *next[2];
    void *data;
} worker_t;

typedef worker_t *(*worker_creator_func)();

typedef struct worker_registry_entry
{
    const char *name;
    worker_t *worker;
} worker_registry_entry_t;

#endif //__WORKER_H__
