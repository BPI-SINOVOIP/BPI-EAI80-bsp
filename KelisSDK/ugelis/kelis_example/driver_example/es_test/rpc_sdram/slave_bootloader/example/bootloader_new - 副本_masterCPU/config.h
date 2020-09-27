/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file        config.h
 *
 * @author      wen.liu
 *
 * @version     1.1.0
 *
 * @date        2016/06/13
 *
 * @brief       Configuration file of this project.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CHIP_GM6601
#undef CHIP_GM6601
#endif

#ifdef CHIP_GM6601B
#undef CHIP_GM6601B
#endif

#ifdef CHIP_GM6611
#undef CHIP_GM6611
#endif

#ifdef CHIP_GM6621
#undef CHIP_GM6621
#endif

/**
 * Chipset select
 */
#define CHIP_GM6611

#define RABBIT_NO_OS_SYSTEM
#define printk  printf

#define CONFIG_SDRAM

/**
 * System clock configure
 */
#define SYS_CLOCK    48000000

#define DEBUG_PRINTF

#define USE_UART_HAL_TEST

/**
 * Add your project's Macro here
 */



#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
