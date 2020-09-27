/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file 	   config.h
*
* @author	   Richard.Liu
*
* @version	   v1.0.0
*
* @date 	   2019/01/25
*
* @brief	   config source file
*
* @note
*			   2019/01/25, Richard.Liu, v1.0.0
*				   Initial version.
*
**/
#ifndef _PRJ_CONFIG_H_
#define _PRJ_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif



/**
 *platform select
 *#########################################
 *!!!you must be sure only open one
 *!!!!plafrom marco and close the others
 *########################################
 */
//#define  F6721B_EVB_144PIN 1
//#define  F6721B_EVB_176PIN 1
//#define  F6721B_EVB_QFN40PIN 1
#define  F6721B_EVB_QFN80PIN 1

//#define  GM6721_EVB_QFN40PIN
//#define  GM6721_EVB_QFN80PIN
//#define  GM6721_EVB_144PIN
//#define  GM6721_EVB_176PIN

#define  CHIP_F6721B
#ifdef CHIP_F6721B
//#define QFN80_SORTING_BOARD //open when use sorting board, default is demo board
#endif

/**
*feature for your project
*/

#define CONFIG_CONSOLE


#define CONFIG_GM_HAL_RPC
#define CONFIG_GM_HAL_RPC_SLAVE
#define CONFIG_USE_HAL_TEST
#define CONFIG_RPC_SUBSYS
#define CONFIG_SDRAM_BOOT
#define CONFIG_IMAGE_OFFSET_SIZE 64//6144//4096
#define CONFIG_KERNEL_MEM_RESVER_SIZE 6144//0//2048
#define CAMERA_IMAGE_BUFFER_LENGTH (0)//in Byte

#define CONFIG_BARCODE
/* select barcode type(multiple choise) */
#ifdef CONFIG_BARCODE
#define ENABLE_CODE128 1
#define ENABLE_QRCODE  1
#endif


#define CONFIG_JERRY_LIBM
#define CONFIG_MINIMAL_LIBC_EXT
#define CONFIG_OPENMV

	
#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
