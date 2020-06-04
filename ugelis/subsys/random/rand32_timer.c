/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief        
 *
 * @note
 *    2018/03/24, Mike.Zheng, V1.0.0
 *        Initial version.
 */

/**
 * @file
 * @brief Non-random number generator based on system timer
 *
 * This module provides a non-random implementation of sys_rand32_get(), which
 * is not meant to be used in a final product as a truly random number
 * generator. It was provided to allow testing on a platform that does not (yet)
 * provide a random number generator.
 */

#include <random/rand32.h>
#include <drivers/system_timer.h>
#include <kernel.h>
#include <atomic.h>

#if defined(__GNUC__)

/*
 * Symbols used to ensure a rapid series of calls to random number generator
 * return different values.
 */
static atomic_val_t _rand32_counter;

#define _RAND32_INC 1000000013

/**
 *
 * @brief Get a 32 bit random number
 *
 * The non-random number generator returns values that are based off the
 * target's clock counter, which means that successive calls will return
 * different values.
 *
 * @return a 32-bit number
 */

u32_t sys_rand32_get(void)
{
	return k_cycle_get_32() + atomic_add(&_rand32_counter, _RAND32_INC);
}

#endif /* __GNUC__ */
