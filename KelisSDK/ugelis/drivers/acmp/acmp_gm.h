/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file 	   acmp_gm.h
*
* @author	   Richard.Liu
*
* @version	   v1.0.0
*
* @date 	   2019/01/24
*
* @brief	   ACMP source file
*
* @note
*			   2019/01/24, Richard.Liu, v1.0.0
*				   Initial version.
*
**/

#ifndef __ACMP_GM_H__
#define __ACMP_GM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <acmp.h>
#include <misc/byteorder.h>
#include <gm_hal_acmp.h>
#include <misc/printk.h>

#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF
#endif


#define DEBUG_PRINTF
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

struct acmp_gm_config {
	const ACMP_Device_T *base;
};

struct acmp_gm_data {
    ACMP_Handle_T handle;
	ACMP_Config_T acmp_config;
};

#define DEV_CFG(dev) \
	((const struct acmp_gm_config *)(dev)->config->config_info)
#define DEV_DATA(dev) \
	((struct acmp_gm_data *)(dev)->driver_data)

#define ACMP_STRUCT(dev)                                                 \
	((ACMP_Device_T *)(DEV_CFG(dev))->base)

#ifdef __cplusplus
}
#endif

#endif /* __ACMP_GM_H__ */
