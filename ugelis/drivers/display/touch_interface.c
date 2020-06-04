/*
 * Copyright (c) 2018 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SRC_TOUCH_INTERFACE_COMMON_C
#define __SRC_TOUCH_INTERFACE_COMMON_C
#ifdef __cplusplus
extern "C" {
#endif
#include <config.h>
#include <feature/gm6721x_driver_feature.h>

/*i2c select*/
#if defined(F6721B_EVB_QFN80PIN)//all demoQFN80 board only support gpio i2c
/*software gpio i2c protocol*/
#include "../i2c/i2c_gpio.c"
#include "../i2c/i2c_bitbang.c"
#elif defined(LQFP176_DEMO_BOARD)||defined(BANPIBOARD_QFN80PIN)//176 demoboard and bipaiboard support hardware i2c
/*hardware i2c protocol*/
#if defined(CAMERA_OV5640)||defined(CAMERA_BF3007)//fix compile error when use camera and touchscreen hardware i2c
#else
#include "../i2c/i2c_gm.c"
#endif
#endif

/*touch ic select*/
#if defined(CONFIG_TOUCHSCREEN)
#include "../input/touchscreen/core/ts_core.c"
#if defined(CONFIG_GT9147)
#include "../input/touchscreen/gt9147/gt9147.c"
#elif defined(CONFIG_FT5426)
#include "../input/touchscreen/ft5426/ft5426.c"
#elif defined(CONFIG_GT911)
#include "../input/touchscreen/gt911/gt911.c"
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif /* __SRC_TOUCH_INTERFACE_COMMON_C */
