/*
 * Copyright (c) 2018 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SRC_CAMERA_INTERFACE_COMMON_C
#define __SRC_CAMERA_INTERFACE_COMMON_C
#ifdef __cplusplus
extern "C" {
#endif
#include <config.h>
#include <feature/gm6721x_driver_feature.h>

#if defined(CAMERA_OV7725)||defined(CAMERA_GC0308)||defined(CAMERA_GC2145)
/*sccb protocol*/
#include "sccb/gpio_sccb.c"
#if defined(CAMERA_OV7725)
#include "ov7725/ov7725.c"
#elif defined(CAMERA_GC0308)
#include "gc0308/gc0308.c"
#elif defined(CAMERA_GC2145)
#include "gc2145/gc2145.c"
#endif
#elif defined(CAMERA_OV5640)||defined(CAMERA_BF3007)
/*hardware i2c protocol*/
#include "../i2c/i2c_gm.c"
#if defined(CAMERA_OV5640)
#include "ov5640/ov5640.c"
#elif defined(CAMERA_BF3007)
#include "bf3007/bf3007.c"
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif /* __SRC_CAMERA_INTERFACE_COMMON_C */
