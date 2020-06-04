/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <kernel.h>
#ifdef __USE_KEIL_
#include <errno_ex.h>
#else
#include <errno.h>
#endif
#include <posix/time.h>
#include <posix/sys/types.h>

/**
 * @brief Get clock time specified by clock_id.
 *
 * See IEEE 1003.1
 */
int clock_gettime(clockid_t clock_id, struct timespec *ts)
{
	u64_t elapsed_msecs;

	if (clock_id != CLOCK_MONOTONIC) {
		errno = EINVAL;
		return -1;
	}

	elapsed_msecs = k_uptime_get();
	ts->tv_sec = (s32_t) (elapsed_msecs / MSEC_PER_SEC);
	ts->tv_nsec = (s32_t) ((elapsed_msecs % MSEC_PER_SEC) *
					USEC_PER_MSEC * NSEC_PER_USEC);

	return 0;
}
