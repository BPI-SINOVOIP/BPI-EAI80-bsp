/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          painter_worker.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of painter worker
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __PAINTER_WORKER_H__
#define __PAINTER_WORKER_H__

#include "ui/draw.h"
#include "worker.h"

typedef struct paint_info
{
    int x;
    int y;
    int w;
    int h;
    char *str;
    char *pic;
    int color;
    int size;
} paint_info_t;

typedef struct paint_rect
{
    int x0;
    int y0;
    int x1;
    int y1;
    char *str;
    int color;
    int wh_flag;
    int rel;
    int rel_w;
    int rel_h;
    int update;
    int use;
} paint_rect_t;

typedef void (*gui_func)(void *param);
typedef struct gui_info
{
    void *data;
    gui_func show_pic;
    gui_func show_text;
    gui_func show_char;
    gui_func fill_frame;
    gui_func show_rect;
    gui_func exit;
    gui_func set_event_cb;
} gui_info_t;

typedef struct painter_worker_info
{
    gui_info_t *gui;
} painter_worker_info_t;

typedef int (*painter_event_cb)(int index);

gui_info_t *ugui_start(void);

#endif //__PAINTER_WORKER_H__
