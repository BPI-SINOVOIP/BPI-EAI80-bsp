/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        ts_core.h
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2020/02/17
 *
 * @brief       The header file of ts_core
 *
 * @note
 *              2020/02/17, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef __TS_CORE_H
#define __TS_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ts.h>

int put_ts_event(struct ts_input_event *event, int timeout);

#ifdef __cplusplus
}
#endif

#endif /*__TS_CORE_H */








