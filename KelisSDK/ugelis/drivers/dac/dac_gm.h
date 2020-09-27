/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file 	   dac_gm.h
*
* @author	   Richard.Liu
*
* @version	   v1.0.0
*
* @date 	   2019/01/24
*
* @brief	   DAC source file
*
* @note
*			   2019/01/24, Richard.Liu, v1.0.0
*				   Initial version.
*
**/

#ifndef __DAC_GM6XXX_H__
#define __DAC_GM6XXX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <dac.h>
#include <misc/byteorder.h>
#include <gm_hal_dac.h>
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

/*
#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF
*/

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

#define  DAC0_DEV_NAME     dac0_dev
#define  DAC1_DEV_NAME     dac1_dev

struct dac_gm_config {
	const DAC_Device_T *base;
};

struct dac_gm_data {
    DAC_Handle_T handle;
	DAC_Config_T  dac_config;
	DAC_ComConfig_T dac_com_config;
};

#define DEV_CFG(dev) \
	((const struct dac_gm_config *)(dev)->config->config_info)
#define DEV_DATA(dev) \
	((struct dac_gm_data *)(dev)->driver_data)

#define DAC_STRUCT(dev)                                                 \
	((DAC_Device_T *)(DEV_CFG(dev))->base)

#ifdef __cplusplus
}
#endif

#endif /* __DAC_GM6XXX_H__ */
