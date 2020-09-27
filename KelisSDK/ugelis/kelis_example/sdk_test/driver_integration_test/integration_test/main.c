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
#include "integration_test.h"

void test_main(void)
{
    //while (1)
    {
        ztest_test_suite(framework_tests,
                         ztest_unit_test(gm_test_wwdg_normal),
                         ztest_unit_test(gm_test_lvd),
                         ztest_unit_test(gm_test_rtc),
                         ztest_unit_test(dac_test),
                         ztest_unit_test(gpio_test),
                         ztest_unit_test(gm_test_tim),
                         ztest_unit_test(mmc_test),
                         ztest_unit_test(gm_test_uart),
                         ztest_unit_test(lcd_test),
                         ztest_unit_test(camera_test),
                         ztest_unit_test(can_test)
                         //ztest_unit_test(gm_test_iwdg)

                        );
        ztest_run_test_suite(framework_tests);
    }
}
