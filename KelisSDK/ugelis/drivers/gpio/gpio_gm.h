/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gpio_gm.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		GPIO source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _GPIO_GM_H_
#define _GPIO_GM_H_

/**
 * @file header for GREE GPIO
 */

#include <gpio.h>

#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF
#endif

/*#define DEBUG_PRINTF*/
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

#define EXCEPTION_GPIOA  GPIOA_IRQn
#define EXCEPTION_GPIOB  GPIOB_IRQn
#define EXCEPTION_GPIOC  GPIOC_IRQn
#define EXCEPTION_GPIOD  GPIOD_IRQn
#define EXCEPTION_GPIOE  GPIOE_IRQn
#define EXCEPTION_GPIOF  GPIOF_IRQn
#define EXCEPTION_GPIOG  GPIOG_IRQn
#define EXCEPTION_GPIOH  GPIOH_IRQn
#define EXCEPTION_GPIOI  GPIOI_IRQn
#define EXCEPTION_GPIOJ  GPIOJ_IRQn
#define EXCEPTION_GPIOK  GPIOK_IRQn

/** GPIO pin to trigger interrupt. */
#define GPIO_INT		(1 << 28)
/** @cond INTERNAL_HIDDEN */
#define GPIO_PUD_POS		8

/**
 * @brief configuration of GPIO device
 */
struct gpio_gm_config {
	/* base address of GPIO port */
	GPIO_Device_T *port_base;
	/* GPIO IRQ number */
	unsigned long irq_num;
};

/**
 * @brief driver data
 */
struct gpio_gm_data {
	/* list of registered callbacks */
	sys_slist_t callbacks;
	/* callback enable pin bitmask */
	uint32_t pin_callback_enables;
};

#define DEV_CFG(dev) \
	((const struct gpio_gm_config *)(dev)->config->config_info)
#define DEV_DATA(dev) \
	((struct gpio_gm_data *)(dev)->driver_data)

#endif /* _GPIO_GM_H_ */
