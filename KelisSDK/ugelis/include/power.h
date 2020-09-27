/*
 * Copyright (c) 2017 ARM Ltd
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public APIs for POWER drivers
 */

#ifndef __POWER_H__
#define __POWER_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <errno.h>
#include <stddef.h>
#include <device.h>


#define DEV_POWER_NAME "POWER_DEV"  //should config for dts


enum pm_mode
{
    PM_SLEEP_MODE           			= 0,
    PM_STOP1_MODE              			   ,
    PM_STOP2_MODE              			   ,
    PM_STADDBY_MODE            			   ,
    PM_SHUTDOWN_MODE          			   ,
} ;

enum pm_pin_wakup_low_mode
{

    PM_PIN_WAKEUP_STOP2_MODE           =0   ,
    PM_PIN_WAKEUP_STADDBY_MODE            ,
    PM_PIN_WAKEUP_SHUTDOWN_MODE           ,
} ;


typedef int (*enter_low_power_mode_t)(struct device *dev, enum pm_mode mode);
typedef int (*config_wakeup_pin_t)(struct device *dev, enum pm_pin_wakup_low_mode mode);

/** @brief pm driver API definition. */
struct pm_driver_api  {
    enter_low_power_mode_t       pm_enter_low_power;
    config_wakeup_pin_t          pm_config_wakeup_pin;
};



/**
 * @brief soc enter low power mode.
 *
 *
 * @param dev power device structure.
 * @param mode enter low power mode.

 * @retval 0 If successful.
 * @retval failed Otherwise.
 */
__syscall  int pm_enter_low_power(struct device *dev, enum pm_mode mode);
static inline s32_t _impl_pm_enter_low_power(struct device *dev, enum pm_mode mode)
{
	struct pm_driver_api *api;

	api = (struct pm_driver_api *)dev->driver_api;

	return api->pm_enter_low_power(dev, mode);
}



/**
 * @brief check whether the card is in the slot.
 *
 *
 * @param dev mmc device structure.
 *
 * @retval ture If the card is in the slot.
 * @retval false Otherwise.
 */
__syscall  bool pm_config_wakeup_pin(struct device *dev, enum pm_pin_wakup_low_mode mode);

static inline bool _impl_pm_config_wakeup_pin(struct device *dev, enum pm_pin_wakup_low_mode mode)
{
	struct pm_driver_api *api;

	api = (struct pm_driver_api *)dev->driver_api;

	return api->pm_config_wakeup_pin(dev,mode);
}




#ifdef __cplusplus
}
#endif

#include <syscalls/power.h>

#endif /* _POWER_H_ */


