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
 * @brief       lcd demo source file
 *
 * @note
 *              2019/12/27, Christal.Xu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <ztest.h>
#include "lcd_test.h"

void test_main(void)
{
    ztest_test_suite(gm_hal_lcd_basic_test,
                     ztest_unit_test(lcd_test));
    ztest_run_test_suite(gm_hal_lcd_basic_test);
}
