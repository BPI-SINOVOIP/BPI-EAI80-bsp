/** @file
 *  @brief touch screen interface APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <misc/__assert.h>
#include <misc/slist.h>

#include <stdint.h>
#include <stddef.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TS_MAX_TOUCH  5

typedef enum
{
    DOWN = 0,
    MOVE,
    UP,
} Touch_Sta_T;

typedef struct
{
    uint16_t x;
    uint16_t y;
    Touch_Sta_T touchSta;
    uint16_t valid;         //point whether is valid
} Touch_Point_T;

typedef struct
{
    uint32_t touchID;
    uint8_t (*init)(void);
    uint8_t (*scan)(void);
    Touch_Point_T touchPoints[TS_MAX_TOUCH];    //touch points informations
    uint8_t pointNum;                           //number of touch points
    uint8_t dir;
    uint8_t press;                              //when screen is pressed, set 1
} Touch_Screen_t;

/** @brief touch screen IC driver API definition. */
struct touchscreen_driver_api
{
    int (*scan)(struct device *dev);
};

/**
 * @brief Driver touch screen ic scan the screen.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int touch_scan(struct device *dev);

static inline int _impl_touch_scan(struct device *dev)
{
    const struct touchscreen_driver_api *api = dev->driver_api;

    if (!api->scan)
    {
        return -ENOTSUP;
    }

    return api->scan(dev);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/touch.h>

#endif /* __CAMERA_SENSOR_H */

















