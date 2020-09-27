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
#include "hal_integration_test.h"

void test_main(void)
{
    //while (1)
    {
        ztest_test_suite(framework_tests,
                         ztest_unit_test(gm_test_wwdg),
                         ztest_unit_test(gm_hal_test_lvd),
                         ztest_unit_test(gm_hal_test_rtc),
                         ztest_unit_test(dac_test),
                         ztest_unit_test(gpio_test),
                         ztest_unit_test(adc_test),
                         ztest_unit_test(acmp_test),
                         ztest_unit_test(gm_hal_test_uart),
                         ztest_unit_test(i2c_test),
                         ztest_unit_test(dma_test),
                         ztest_unit_test(gm_hal_test_tim),
                         ztest_unit_test(gm_hal_test_btm),
                         ztest_unit_test(can_test),
                         ztest_unit_test(mmc_test),
                         ztest_unit_test(pes_test),
                         ztest_unit_test(imp_test),
                         ztest_unit_test(vout_test),
                         ztest_unit_test(sai_test)
                         //ztest_unit_test(gm_test_iwdg)

                        );
        ztest_run_test_suite(framework_tests);
    }


}
