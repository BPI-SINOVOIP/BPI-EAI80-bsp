/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      William.Wang
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       sqi demo source file
 *
 * @note
 *              2019/01/24, William.Wang, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <ztest.h>
#include "gm_test_lvd.h"

void test_main(void)
{
    ztest_test_suite(gm_driver_lvd_basic_test,
                     ztest_unit_test(gm_test_lvd));
    ztest_run_test_suite(gm_driver_lvd_basic_test);
}
