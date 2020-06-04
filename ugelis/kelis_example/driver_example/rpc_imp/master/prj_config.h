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

#define CONFIG_CONSOLE

// rpc
#define CONFIG_GM_HAL_RPC
#define CONFIG_GM_HAL_RPC_MASTER
#define CONFIG_USE_HAL_TEST
#define CONFIG_RPC_SUBSYS
#define CONFIG_SDRAM_BOOT
#define CONFIG_KERNEL_MEM_RESVER_SIZE 3072


// image_proc
#define CONFIG_JERRY_LIBM
#define CONFIG_MINIMAL_LIBC_EXT
#define CONFIG_OPENMV
#define CONFIG_DISPLAY
#define CONFIG_DISPLAY_GM6721X
#define CONFIG_GM_HAL_VOUT
#define CONFIG_GM_HAL_IMP

// fix compile issue
//#define MEM_RESVER_START 0xC0D00000
	
#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
