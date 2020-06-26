/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          face_detect.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of face detector
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef _FACE_DETECT_H__
#define _FACE_DETECT_H__

#include <worker/worker.h>

int face_detect_init(pre_proc_param_t *param);
int face_detect_exit(pre_proc_param_t *param);
int face_detect_frame_in(pre_proc_param_t *param);


#endif //_FACE_DETECT_H__
