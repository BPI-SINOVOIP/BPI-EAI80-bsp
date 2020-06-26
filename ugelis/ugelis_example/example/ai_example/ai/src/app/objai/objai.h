/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          objai.h
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
#ifndef __OBJAI_H__
#define __OBJAI_H__

void bodyai_start(void);
void bodyai_exit(void);
void gesai_start(void);
void gesai_exit(void);
int bodyai_cmd_exit(int argc, char *argv[], void *data);
int bodyai_cmd_start(int argc, char *argv[], void *data);
int gesai_cmd_exit(int argc, char *argv[], void *data);
int gesai_cmd_start(int argc, char *argv[], void *data);

#endif //__OBJAI_H__
