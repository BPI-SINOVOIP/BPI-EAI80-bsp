/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          faceai.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of faceai app.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __FACEAI_H__
#define __FACEAI_H__

#include <stdio.h>

int faceai_enter(void);
int faceai_exit(void);
int faceai_cmd_exit(int argc, char *argv[], void *data);
int faceai_cmd_enter(int argc, char *argv[], void *data);

int objectai_enter(void);
int objectai_exit(void);

#endif //__FACEAI_H__
