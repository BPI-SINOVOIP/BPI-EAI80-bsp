/*
 * Copyright (c) 2017 ARM Ltd
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public APIs for TP drivers
 */

#ifndef __TP_H__
#define __TP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <misc/__assert.h>
#include <misc/slist.h>

#include <stdint.h>
#include <stddef.h>
#include <device.h>
#include <input.h>

#define DEV_TS_NAME                    "TOUCH_SCREEN" //devname

#define TS_DIR_THWARTWISE  0 //horizontal screen
#define TS_DIR_ERECT       1 //vertical screen

struct ts_input_event
{
    uint32_t    press_status;
    uint8_t     type ;
    uint32_t    x_abs ;
    uint32_t    y_abs ;
};

int read_ts_event(struct ts_input_event *event, int timeout);

typedef void (*set_ts_dir_t)(struct device *dev, bool dir);
typedef bool (*get_ts_dir_t)(struct device *dev);
typedef uint32_t (*global_ts_cb)(int idx, int32_t x, int32_t y);

/** @brief ts driver API definition. */
struct ts_driver_api
{
    set_ts_dir_t    set_ts_dir;
    get_ts_dir_t    get_ts_dir;
};

static inline void set_ts_dir(struct device *dev, bool dir)
{
    struct ts_driver_api *api;

    api = (struct ts_driver_api *)dev->driver_api;

    api->set_ts_dir(dev, dir);
}

static inline bool get_ts_dir(struct device *dev)
{
    struct ts_driver_api *api;

    api = (struct ts_driver_api *)dev->driver_api;

    return api->get_ts_dir(dev);
}


#ifdef __cplusplus
}
#endif


#endif /* __TP_H__ */


