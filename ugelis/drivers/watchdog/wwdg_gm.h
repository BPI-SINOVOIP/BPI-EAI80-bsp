/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _WWDG_GM_H_
#define _WWDG_GM_H_

#include <stdint.h>
#include <watchdog.h>
#include <gm_hal_wwdg.h>

/**
 * @brief Driver for Window Watchdog (WWDG) for STM32 MCUs
 *
 * Based on reference manual:
 *   STM32F429xx 
 *   advanced ARM ? -based 32-bit MCUs
 *
 * Chapter 22: Window watchdog (WWDG)
 *
 */

typedef void (*irq_config_func_t)(struct device *dev);

/**
 * @typedef wwdg_irq_callback_t
 * @brief Define the application callback function signature for WWDG.
 *
 * @param dev Device struct for the WWDG device.
 */
typedef void (*wwdg_irq_callback_t)(WDOG_Handle_T *hWDOG, WDOG_Event_T event, unsigned long Param);



/*device config*/
struct wwdg_gm_config{
	WDOG_Device_T *base;
	irq_config_func_t irq_config_func;
};

/* driver data */
struct wwdg_gm_data{

	/*wwdg peripheral handler*/
	WDOG_Handle_T hwwdg;
	/* clock device */
	struct device *clock;
	wwdg_irq_callback_t callback;
};

#endif	/* _WWDG_GM_H_ */
