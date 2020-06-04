/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Christal.Xu
 *
 * @version     v1.0.0
 *
 * @date        2019/12/27
 *
 * @brief       imp test demo source file
 *
 * @note
 *              2019/12/27, Christal.Xu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <ztest.h>
#include "imp_test.h"

void test_main(void)
{
    ztest_test_suite(gm_hal_imp_basic_test,
                     ztest_unit_test(imp_test));
    ztest_run_test_suite(gm_hal_imp_basic_test);
}
