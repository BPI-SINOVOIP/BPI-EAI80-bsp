/*
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _WDT_H_
#define _WDT_H_

#include <ugelis/types.h>
#include <device.h>
#include <misc/util.h>
/*gm hal inc file*/
#include <gm_hal_wwdg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WDT_MODE		(BIT(1))
#define WDT_MODE_OFFSET         (1)
#define WDT_TIMEOUT_MASK        (0xF)

enum wdt_mode {
	WDT_MODE_RESET = 0,
	WDT_MODE_INTERRUPT_RESET
};

/**
 * WDT clock cycles for timeout type.
 */
enum wdt_clock_timeout_cycles {
	WDT_2_16_CYCLES,
	WDT_2_17_CYCLES,
	WDT_2_18_CYCLES,
	WDT_2_19_CYCLES,
	WDT_2_20_CYCLES,
	WDT_2_21_CYCLES,
	WDT_2_22_CYCLES,
	WDT_2_23_CYCLES,
	WDT_2_24_CYCLES,
	WDT_2_25_CYCLES,
	WDT_2_26_CYCLES,
	WDT_2_27_CYCLES,
	WDT_2_28_CYCLES,
	WDT_2_29_CYCLES,
	WDT_2_30_CYCLES,
	WDT_2_31_CYCLES
};


/**
 * WDOG_work_mode
 * author:cmt add
 */
typedef enum
{
	WWDG_NORMAL_MODE = 0,                           /**<  WDOG work mode: normal */
	WWDG_WINDOW_MODE    ,                           /**<  WDOG work mode: windows */
	WWDG_NORMAL_EWI     ,                           /**<  WDOG work mode: normal and early warning interrupt */
	WWDG_WINDOW_EWI                                 /**<  WDOG work mode: windows and early warning interrupt */   
}wdt_workmode;
/**
 * @}
 */

/*
 * WWDG CLK division factor
 * WWDG_CLK = system clock/(WWDG_CLK_DIV+1);
 */
typedef enum
{
	WWDG_CLKDIV_FACTOR_1   = 0,                      /**< Select SYSCLK/(0+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_2   = 1,                      /**< Select SYSCLK/(1+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_4   = 3,                      /**< Select SYSCLK/(3+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_8   = 7,                      /**< Select SYSCLK/(7+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_16  = 15,					 /**< Select SYSCLK/(15+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_32  = 31,					 /**< Select SYSCLK/(31+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_64  = 63,					 /**< Select SYSCLK/(63+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_128 = 127,					 /**< Select SYSCLK/(127+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_256 = 255,					 /**< Select SYSCLK/(255+1) As WDOG Clksrc () */
	WWDG_CLKDIV_FACTOR_511 = 511					 /**< Select SYSCLK/(512+1) As WDOG Clksrc () */

}wdt_clkdiv;


/*
 * IWDG CLK division factor
 * IWDG = 32K/(IWDG_CLK_DIV);
 */
typedef enum
{
    IWDG_CLKDIV_FACTOR_4   ,                        
	IWDG_CLKDIV_FACTOR_8   ,                       
	IWDG_CLKDIV_FACTOR_16  ,                        
	IWDG_CLKDIV_FACTOR_32  ,					   
	IWDG_CLKDIV_FACTOR_64  ,					   
	IWDG_CLKDIV_FACTOR_128 ,					   
	IWDG_CLKDIV_FACTOR_256 ,
	IWDG_CLKDIV_FACTOR256 	
}iwdg_clkdiv;


/**
 * WDT configuration struct.
 */
struct wdt_config {
	uint32_t timeout;
	enum wdt_mode mode;
/*cmt add*/	
	uint32_t     windowtimeout;
    uint32_t     winLen;    
	wdt_clkdiv   clkdiv;
	wdt_workmode workmode;
/*end cmt*/
	void (*interrupt_fn)(WDOG_Handle_T *hWDOG, WDOG_Event_T event, unsigned long Param);

};

typedef void (*wdt_api_disable)(struct device *dev);

typedef void (*wdt_api_enable)(struct device *dev);

typedef void (*wdt_api_get_config)(struct device *dev,
				   struct wdt_config *config);

typedef int (*wdt_api_set_config)(struct device *dev,
				  struct wdt_config *config);

typedef void (*wdt_api_reload)(struct device *dev);

struct wdt_driver_api {
	wdt_api_enable enable;
	wdt_api_disable disable;
	wdt_api_get_config get_config;
	wdt_api_set_config set_config;
	wdt_api_reload reload;
};

/*
struct wdt_driver_api {
	// Callback API for disabling watchdog instance.
	void (*disable)(struct device *dev);

	// Callback API for enabling watchdog.
	void (*enable)(struct device *dev);

	// API for getting current watchdog configuration.
	void (*get_config)(struct device *dev,struct wdt_config *config);

	//Callback API for setting current watchdog configuration.
	int (*set_config)(struct device *dev,struct wdt_config *config);

	//Callback API for reloading watchdog.
	void (*reload)(struct device *dev);
};
*/
/**
 * @typedef wdt_enable
 * @brief Callback API for enabling watchdog.
 * See wdt_enable() for argument descriptions. Please note that this function
 * is deprecated.
 */
__syscall void wdt_enable(struct device *dev);

static inline void  _impl_wdt_enable(struct device *dev)
{
	const struct wdt_driver_api *api = dev->driver_api;

	api->enable(dev);
}

/**
 * @typedef wdt_disable
 * @brief Callback API for disabling watchdog instance.
 * See wdt_disable() for argument descriptions
 */
__syscall void wdt_disable(struct device *dev);

static inline void _impl_wdt_disable(struct device *dev)
{
	const struct wdt_driver_api *api = dev->driver_api;

	api->disable(dev);
}
/**
 * @typedef wdt_get_config
 * @brief Callback API for getting current watchdog configuration.
 * See wdt_get_config() for argument descriptions. Please note that this
 * function is deprecated.
 */
__syscall void wdt_get_config(struct device *dev,
				   struct wdt_config *config);

static inline void  _impl_wdt_get_config(struct device *dev,
				  struct wdt_config *config)
{
	const struct wdt_driver_api *api = dev->driver_api;

	api->get_config(dev, config);
}

/**
 * @typedef wdt_set_config
 * @brief Callback API for setting current watchdog configuration.
 * See wdt_set_config() for argument descriptions. Please note that this
 * function is deprecated.
 */
__syscall int wdt_set_config(struct device *dev,
				  struct wdt_config *config);

static inline int  _impl_wdt_set_config(struct device *dev,
				 struct wdt_config *config)
{
	const struct wdt_driver_api *api = dev->driver_api;

	return api->set_config(dev, config);
}
/**
 * @typedef wdt_reload
 * @brief Callback API for reloading watchdog.
 * See wdt_reload() for argument descriptions. Please note that this function
 * is deprecated.
 */
__syscall void wdt_reload(struct device *dev);

static inline void  _impl_wdt_reload(struct device *dev)
{
	const struct wdt_driver_api *api = dev->driver_api;

	api->reload(dev);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#include <syscalls/watchdog.h>

#endif
