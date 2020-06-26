
/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          device_proi.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2019/11/25
 *
 *
 * @brief Device priority for power mode,0 is highest
  *  Before system enter lower power,OS will suspend device according to the priority of the device.
 *
 *
 * @note
 *    2019/11/25, Mike.Zheng, V1.0.0
 *        Initial version.
 */


#ifndef _DEVICE_PROI_H_
#define _DEVICE_PROI_H_
#include <kernel.h>






/**
 *level:LEVEL_PRE_KERNEL_1
 *init before os is init
 */

#define DEVICE_PRIORITY_SYSTEM_INIT            0
#define DEVICE_PRIORITY_MEMORY_MANAGE          1
#define DEVICE_PRIORITY_PINMUX                 3
#define DEVICE_PRIORITY_BOARD_PINMUX           4
#define DEVICE_PRIORITY_DMA                    5
#define DEVICE_PRIORITY_GPIO                   6
#define DEVICE_PRIORITY_UART                   7
#define DEVICE_PRIORITY_TIM                    50

/*add your device priority accoding your driver init sequence logic*/



/**
 *level:LEVEL_PRE_KERNEL_2
 *init before os is init
 */

#define DEVICE_PRIORITY_PRE_KERNEL2            0
/*add your device priority accoding your driver init sequence logic*/


/**
 *level:LEVEL_POST_KERNEL
 *init after os is init
 */
#define DEVICE_PRIORITY_MMC                    50
#define DEVICE_PRIORITY_RTC                    51
#define DEVICE_PRIORITY_POWER                  60

/*add your device priority accoding your driver init sequence logic*/


/**
 *level:LEVEL_APPLICATION
 *init after os is init
 */
#define DEVICE_PRIORITY_APP                   0
/*add your device priority accoding your driver init sequence logic*/





#endif
