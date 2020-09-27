/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Edward.Zhu
 *
 * @version     v1.0.0
 *
 * @date        2019/12/27
 *
 * @brief       btm demo source file
 *
 * @note
 *              2019/12/27, Edward.Zhu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <ztest.h>
#include "gm_hal_test_btm.h"

void test_main(void)
{
    ztest_test_suite(gm_hal_btm_basic_test,
                     ztest_unit_test(gm_hal_test_btm));
    ztest_run_test_suite(gm_hal_btm_basic_test);
}
