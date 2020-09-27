/*
 * Copyright (c) 2018 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SRC_CAMERA_COMMON_C
#define __SRC_CAMERA_COMMON_C
#ifdef __cplusplus
extern "C" {
#endif
#include <config.h>

#if defined(CONFIG_OV5640)
#include "ov5640/ov5640.c"
#else
#if defined(CONFIG_OV7725)
#include "ov7725/ov7725.c"
#else
#if defined(CONFIG_GC0308)
#include "gc0308/gc0308.c"
#endif
#endif
#endif
#ifdef __cplusplus
}
#endif
#endif /* __SRC_CAMERA_COMMON_C */
