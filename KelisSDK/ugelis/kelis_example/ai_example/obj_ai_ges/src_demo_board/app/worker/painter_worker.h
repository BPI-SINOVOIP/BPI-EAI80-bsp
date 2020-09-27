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

typedef struct paint_info
{
    int x;
    int y;
    char *str;
    int color;
} paint_info_t;

#endif //__PAINTER_WORKER_H__
