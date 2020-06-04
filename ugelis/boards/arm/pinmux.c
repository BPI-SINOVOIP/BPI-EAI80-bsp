/*
 * Copyright (c) 2018 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SRC_PINMUX_COMMON_C
#define __SRC_PINMUX_COMMON_C
#ifdef __cplusplus
extern "C" {
#endif
#include <config.h>

#if defined(F6721B_EVB_144PIN)
	#include "f6721b_evb_144pin/pinmux.c"
#else
	#if defined(F6721B_EVB_QFN40PIN) || defined(F6721B_RINGSCANER)
		#include "f6721b_evb_qfn40Pin/pinmux.c"
	#else
		#if defined(F6721B_EVB_QFN80PIN)
			#include "f6721b_evb_qfn80Pin/pinmux.c"
		#else
			#if defined(F6721B_EVB_176PIN)
				#include "f6721b_evb_176pin/pinmux.c"
			#else
				#include "f6721b_evb_176pin/pinmux.c"
			#endif
		#endif
	#endif
#endif
#ifdef __cplusplus
}
#endif
#endif /* __SRC_PINMUX_COMMON_C */
