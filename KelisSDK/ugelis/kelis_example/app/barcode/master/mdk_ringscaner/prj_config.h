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
//#define  F6721B_EVB_QFN80PIN 1
#define  F6721B_RINGSCANER 1

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
#define CONFIG_GC0308
//#define CONFIG_OV5640
//#define CONFIG_OV7725
//#define CONFIG_BF3007
//#define CONFIG_DISPLAY

// rpc
#define CONFIG_GM_HAL_RPC
#define CONFIG_GM_HAL_RPC_MASTER
#define CONFIG_USE_HAL_TEST
#define CONFIG_RPC_SUBSYS
#define CONFIG_SDRAM_BOOT

/* memory config */
//#define CONFIG_KERNEL_MEM_RESVER_SIZE 3072
#define CONFIG_IMAGE_OFFSET_SIZE 2048//64
#define CONFIG_KERNEL_MEM_RESVER_SIZE 3072//5120//6144
#define CONFIG_SLAVE_MEM_RESVER_SIZE 0//1024
#define CAMERA_IMAGE_BUFFER_LENGTH (3072*1024)//(4096*1024)//in Byte

#define CONFIG_BARCODE
/* select barcode type(multiple choise) */
#ifdef CONFIG_BARCODE
#define ENABLE_CODE128 1
#define ENABLE_QRCODE  1
#endif

#define CONFIG_MAIN_STACK_SIZE 4096

#define CONFIG_JERRY_LIBM
#define CONFIG_MINIMAL_LIBC_EXT
#define CONFIG_OPENMV
#define CONFIG_GM_HAL_VOUT
#define CONFIG_GM_HAL_IMP
#define CONFIG_CONSOLE
#ifdef CONFIG_OV5640
#define CONFIG_OV5640_0
#endif
#ifdef CONFIG_DISPLAY
#define CONFIG_DISPLAY_GM6721X
#endif

#define CONFIG_SFLASH     1
#define CONFIG_SDRAM_BOOT   1


// fix compile issue
//#define MEM_RESVER_START 0xC0D00000
	
#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
