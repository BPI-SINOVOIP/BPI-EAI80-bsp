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
#include <misc/printk.h>
#include <gm_hal_cpu.h>
#include <stdint.h>



extern int coremark();


/* Icache -> disable Dcache -> disable */

//#define CACHE_TEST_0    1

/* Icache -> enable Dcache -> disable */
//#define CACHE_TEST_1  1

/* Icache -> disable Dcache -> enable */
//#define CACHE_TEST_2  1

/* Icache -> enable Dcache -> enable */
#define CACHE_TEST_3  1


/* Icache -> disable Dcache -> disable */
#ifdef CACHE_TEST_0
/*mark:*/
void test_cache_config()
{
    printk("HAL_ICACHE_Disable  HAL_DCACHE_Disable\n");
    HAL_ICACHE_Disable();
    HAL_DCACHE_Disable();
}
#endif

/* Icache -> enable Dcache -> disable */
#ifdef CACHE_TEST_1
void test_cache_config()
{
    printk("HAL_ICACHE_Enable  HAL_DCACHE_Disable\n");

    HAL_ICACHE_Enable();
    HAL_DCACHE_Disable();
}
#endif

/* Icache -> disable Dcache -> enable */

#ifdef CACHE_TEST_2
void test_cache_config()
{

    printk("HAL_ICACHE_Disable  HAL_DCACHE_Enable\n");

    HAL_ICACHE_Disable();
    HAL_DCACHE_Enable();
}
#endif

/* Icache -> enableDcache -> enable */

#ifdef CACHE_TEST_3
void test_cache_config()
{

    printk("HAL_DCACHE_Enable  HAL_ICACHE_Enable\n");

    HAL_DCACHE_Enable();
    HAL_ICACHE_Enable();
}
#endif


void main(void)
{

    test_cache_config();
    coremark();
}
