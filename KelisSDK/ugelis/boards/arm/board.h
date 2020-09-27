/*
 * Copyright (c) 2018 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __INC_BOARD_COMMON_H
#define __INC_BOARD_COMMON_H
#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

#if defined(F6721B_EVB_144PIN)
#include "f6721b_evb_144pin/board.h"
#include "f6721b_evb_144pin/mem_cfg.h"
#else
#if defined(F6721B_EVB_QFN40PIN) || defined(F6721B_RINGSCANER)
#include "f6721b_evb_qfn40Pin/board.h"
#include "f6721b_evb_qfn40Pin/mem_cfg.h"
#else
#if defined(F6721B_EVB_QFN80PIN)
#include "f6721b_evb_qfn80Pin/board.h"
#include "f6721b_evb_qfn80Pin/mem_cfg.h"
#else
#if defined(F6721B_EVB_176PIN)
#include "f6721b_evb_176pin/board.h"
#include "f6721b_evb_176pin/mem_cfg.h"
#else
#if defined(BANPIBOARD_QFN80PIN)
#include "banpiboard_qfn80pin/board.h"
#include "banpiboard_qfn80pin/mem_cfg.h"
#else
#if defined(EAIBOARD_176PIN)
#include "eaiboard_176pin/board.h"
#include "eaiboard_176pin/mem_cfg.h"
#else
#include "f6721b_evb_176pin/board.h"
#include "f6721b_evb_176pin/mem_cfg.h"
#endif
#endif
#endif
#endif
#endif
#endif
#ifdef __cplusplus
}
#endif
#endif /* __INC_BOARD_H */
