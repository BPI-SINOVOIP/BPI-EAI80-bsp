 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          arm_hal_flash_patch.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/04/21
 *
 * @brief         Header file of flash pathc driver.
 *
 * @note
 *     2018/04/21, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#ifndef _ARM_HAL_FLASH_PATCH_H_
#define _ARM_HAL_FLASH_PATCH_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>


extern void HAL_FlashPatchConfig( void* oldAddress, void* newAddress);

#ifdef __cplusplus
}
#endif

#endif /*_ARM_HAL_FLASH_PATCH_H_*/
