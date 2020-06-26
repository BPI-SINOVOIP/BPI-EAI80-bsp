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

void faceai_start(void);
void faceai_exit(void);
int faceai_cmd_exit(int argc, char *argv[], void *data);
int faceai_cmd_start(int argc, char *argv[], void *data);
int faceai_record(int argc, char *argv[], void *data);
int faceai_clear(int argc, char *argv[], void *data);

#endif //__FACEAI_H__
