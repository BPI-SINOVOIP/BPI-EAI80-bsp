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
#include <kernel.h>
#include "processor.h"

enum
{
    /* camera worker */
    CMD_CAM_START,
    CMD_CAM_STOP,

    /* lcd_worker */
    CMD_LCD_SETFRAME,
    CMD_LCD_RESETSCALER,
    CMD_LCD_CAPTURE,
    CMD_LCD_DOIMP,

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
    CMD_POST_FACEFRAME,

    /* painter worker */
    CMD_PAINT_CAP,
    CMD_PAINT_PASS,
    CMD_PAINT_CLEAR,
    CMD_PAINT_TEXT,
    CMD_PAINT_BIND,
    CMD_PAINT_RECT,
    CMD_PAINT_SHOWCH,
    CMD_PAINT_INIT,
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
    int out0;
    int out1;
} worker_frame_t;

typedef struct worker_layer
{
    int src_w;
    int src_h;
    int src_fmt;
    int disp_x;
    int disp_y;
    int disp_w;
    int disp_h;
    int out0_w;
    int out0_h;
    int out0_fmt;
    int out1_w;
    int out1_h;
    int out1_fmt;
} worker_layer;

typedef struct worker_lcd_param
{
    worker_layer video;
    worker_layer ui;
} worker_lcd_param_t;

typedef struct worker_cam_param
{
    int w;
    int h;
} worker_cam_param_t;

struct worker;

typedef int (*proc_finish_func)(void *src, struct worker *param);

typedef int (*worker_run_functio)(struct worker *self, int cmd, void *param);
typedef int (*worker_stop_functio)(struct worker *self, int cmd, void *param);
typedef int (*worker_init_functio)(struct worker *self, void *param);
typedef void *(*worker_fetch_function)(struct worker *self, void *param);
typedef int (*worker_deinit_functio)(struct worker *self);

typedef struct worker
{
    worker_run_functio      run;
    worker_stop_functio     stop;
    worker_init_functio     init;
    worker_fetch_function   fetch;
    worker_deinit_functio   deinit;
    struct worker *next[2];
    void *data;
} worker_t;

typedef worker_t *(*worker_creator_func)();

typedef struct worker_registry_entry
{
    const char *name;
    worker_t *worker;
} worker_registry_entry_t;

/* pre processing worker parameters */

typedef struct worker_pre_sizes
{
    int src_w;
    int src_h;
    int ai_w;
    int ai_h;
    int input_w;
    int input_h;
} worker_pre_sizes_t;

typedef struct pre_proc_param
{
    int detected_buf;
    int original_buf;

    worker_pre_sizes_t sizes;
    proc_finish_func process_cb;
    proc_finish_func finish_cb;
    unsigned char *in[2];

    struct k_mutex mutex;
    worker_t *painter;
} pre_proc_param_t;

typedef int (*pre_proc_func)(pre_proc_param_t *param);
typedef void (*pre_proc_do_func)(void *, void *, void *);

typedef struct worker_pre_param
{
    pre_proc_func frame_in;
    pre_proc_func init;
    pre_proc_func exit;
    worker_pre_sizes_t sizes;
    int ai_w;
    int ai_h;
} worker_pre_param_t;

/* easynet worker parameters */
typedef struct easynet_proc_param
{
    int running;
    int ops;
    int processed;
    struct easynet_dev *dev;
    struct k_sem predict_lock;
    proc_finish_func process_cb;
    proc_finish_func finish_cb;
    unsigned char *in;

    void *data;

    worker_t *painter;
} easynet_proc_param_t;

typedef int (*easynet_proc_func)(easynet_proc_param_t *param);
typedef void (*easynet_do_func)(void *, void *, void *);

typedef struct worker_easynet_param
{
    easynet_proc_func init;
    easynet_proc_func exit;
    easynet_proc_func frame_in;
    easynet_do_func process;
    int ops;
    void *data;
} worker_easynet_param_t;

/* post worker parameters */
typedef struct worker_post_param
{
    int base_ofst;
    int base_size;
} worker_post_param_t;

/* painter worker parameters */
typedef struct worker_painter_param
{
} worker_painter_param_t;


worker_t *get_worker(const char *name);
int do_next_process(worker_t *worker, int cmd, void *param);

worker_lcd_param_t *stream_get_lcd_param(void);
uint32_t stream_start_loaddata(worker_io_t *info);

#endif //__WORKER_H__

