/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @addtogroup t_fat_fs
 * @{
 * @defgroup t_fat_fs_basic test_fat_fs_basic_operations
 * @}
 */

#include "test_fat.h"

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffba2;
    HW32_REG(0x1ffff0ac) = 0xff00ffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}


void test_main(void)
{
	/*patch_config();*/

	ztest_test_suite(fat_fs_basic_test,
			 ztest_unit_test(test_fat_mount),
			 ztest_unit_test(test_fat_file),
			 ztest_unit_test(test_fat_dir),
			 ztest_unit_test(test_fat_fs));
	ztest_run_test_suite(fat_fs_basic_test);
}
