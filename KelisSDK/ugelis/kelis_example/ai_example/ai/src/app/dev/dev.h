/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          dev.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of dev
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __DEV_H__
#define __DEV_H__

typedef struct scaler_param
{
    unsigned int src;
    unsigned int dst;
    unsigned int src_w;
    unsigned int src_h;
    unsigned int dst_w;
    unsigned int dst_h;
    unsigned int src_fmt;
    unsigned int dst_fmt;
    unsigned int use;
} scaler_param_t;

int dev_init(void);
int dev_exit(void);
int ai_imp_scale(void *ltdc, scaler_param_t *param);
int dev_available_size(void);

#endif //__DEV_H__
