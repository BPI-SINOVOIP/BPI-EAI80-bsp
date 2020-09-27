/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/02/25, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_HAL_CPU_H_
#define _GM_HAL_CPU_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

#include"gm_hal_cpu_dev.h"
#include"gm_hal_bus_map.h"

#ifdef CONFIG_GM_HAL_CACHE
#include"gm_hal_cache.h"
#endif

#ifdef CONFIG_GM_ARM_HAL_MPU
#include"arm_hal_mpu.h"
#endif

#ifdef CONFIG_GM_ARM_HAL_FLASH_PATCH
#include"arm_hal_flash_patch.h"
#endif






#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_CPU_H_ */
