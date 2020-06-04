/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        ts_core.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2020/02/17
 *
 * @brief       Common touchscreen api called from user space.
 *
 * @note
 *              2020/02/17, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#include <errno.h>
#include <kernel.h>
#include <ugelis.h>
#include <arch/cpu.h>
#include <misc/__assert.h>
#include <misc/printk.h>
#include <init.h>
#include <string.h>
#include <ts.h>

#define CONFIG_TS_EVENT_BUFF_LENGTH 20

K_MSGQ_DEFINE(ts_event_msgq, sizeof(struct ts_input_event), CONFIG_TS_EVENT_BUFF_LENGTH, 4);

int read_ts_event(struct ts_input_event *event, int timeout)
{
    int ret;

    ret = k_msgq_get(&ts_event_msgq, (struct ts_input_event *)event, timeout);

    return ret;

}

int put_ts_event(struct ts_input_event *event, int timeout)
{
    int ret = 0;

    ret = k_msgq_put(&ts_event_msgq, (struct ts_input_event *)event, timeout);


    return ret;
}





