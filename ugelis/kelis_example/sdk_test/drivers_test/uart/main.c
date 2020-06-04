#include <ztest.h>

#include "gm_test_uart.h"

void test_main(void)
{
	ztest_test_suite(gm_driver_uart_test, ztest_unit_test(gm_test_uart));
	ztest_run_test_suite(gm_driver_uart_test);
}
