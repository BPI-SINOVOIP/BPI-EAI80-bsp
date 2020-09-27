/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Steven.lee
 *
 * @version     v1.0.0
 *
 * @date        2019/12/26
 *
 * @brief       sqi demo source file
 *
 * @note
 *              2019/12/26, Steven.lee, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <ztest.h>
#include "mmc_test.h"

void test_main(void)
{
    ztest_test_suite(gm_hal_mmc_basic_test,
                     ztest_unit_test(mmc_test));
    ztest_run_test_suite(gm_hal_mmc_basic_test);
}
