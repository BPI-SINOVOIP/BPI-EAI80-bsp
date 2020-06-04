/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <ztest.h>

/* The only point to CONFIG_MULTITHREADING=n is to use uGelis's
 * multiplatform toolchain, linkage and boostrap integration to arrive
 * here so the user can run C code unimpeded.  In general, we don't
 * promise that *any* uGelis APIs are going to work properly, so don't
 * try to test any.  That means we can't even use the ztest suite
 * framework (which spawns threads internally).
 */
void test_main(void)
{
	TC_PRINT("It works\n");
	TC_END_REPORT(TC_PASS);
}
