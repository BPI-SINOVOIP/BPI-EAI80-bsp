/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          speechai.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/03/11
 *
 * @brief         Header file of speechai app.
 *
 * @note
 *    2020/03/11, Angrad.Yang, V1.0.0
 *        Initial version.
 */
#ifndef __SPEECHAI_H__
#define __SPEECHAI_H__


void speechai_start(void);
void speechai_exit(void);
int speechai_cmd_exit(int argc, char *argv[], void *data);
int speechai_cmd_start(int argc, char *argv[], void *data);
#endif //__SPEECHAI_H__
