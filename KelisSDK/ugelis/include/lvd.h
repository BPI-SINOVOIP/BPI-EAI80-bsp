/*
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LVD_H_
#define _LVD_H_
#include <ugelis/types.h>
#include <device.h>
#include <misc/util.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LVD_VD1_Level_0        0x0    /* 1.70+0.05V/-0.05V  */ 
#define LVD_VD1_Level_1        0x1    /* 1.80+0.05V/-0.05V  */ 
#define LVD_VD1_Level_2        0x2    /* 2.00+0.05V/-0.05V  */ 
#define LVD_VD1_Level_3        0x3    /* 2.20+0.05V/-0.05V  */ 
#define LVD_VD1_Level_4        0x4    /* 2.40+0.05V/-0.05V  */ 
#define LVD_VD1_Level_5        0x5    /* 2.60+0.05V/-0.05V  */ 
#define LVD_VD1_Level_6        0x6    /* 2.80+0.05V/-0.05V  */ 
#define LVD_VD1_Level_7        0x7    /* 3.00+0.05V/-0.05V  */ //POR Reset at level 2.4v
                                                                  
#define LVD_VD2_Level_0        0x0    /* 1.80+0.05V/-0.05V  */ 
#define LVD_VD2_Level_1        0x1    /* 1.90+0.05V/-0.05V  */ 
#define LVD_VD2_Level_2        0x2    /* 2.00+0.05V/-0.05V  */ 
#define LVD_VD2_Level_3        0x3    /* 2.20+0.05V/-0.05V  */ 
#define LVD_VD2_Level_4        0x4    /* 2.40+0.05V/-0.05V  */ 
#define LVD_VD2_Level_5        0x5    /* 2.60+0.05V/-0.05V  */ 
#define LVD_VD2_Level_6        0x6    /* 2.80+0.05V/-0.05V  */ 
#define LVD_VD2_Level_7        0x7    /* 3.00+0.05V/-0.05V  */ 
                                                                  
#define LVD_VD3_Level_0        0x0    /* 0.90+0.03V/-0.03V  */ 
#define LVD_VD3_Level_1        0x1    /* 0.95+0.03V/-0.03V  */ 
#define LVD_VD3_Level_2        0x2    /* 1.00+0.03V/-0.03V  */ 
#define LVD_VD3_Level_3        0x3    /* 1.05+0.03V/-0.03V  */ 

typedef void (*LVD_Int_Callback) (int);

typedef struct
{
    uint8_t threshold;         /**< Configures timing threshold for filter the glitch of voltage detector */
    uint8_t detectionLevel;    /**< Configures the voltage detection level */
    uint8_t reset;             /**< reset chip when VDD is less than detectionLevel */
    uint8_t enable;            /**< enable the operation of voltage detection */
	LVD_Int_Callback int_call;
} LVD_Config;

typedef struct {
	LVD_Config lvd1;
	LVD_Config lvd2;
	LVD_Config lvd3;
}lvd_cfg;

typedef enum
{
    LVD_1 = 1U,		/**< Voltage detection 1 */
    LVD_2 = 2U,		/**< Voltage detection 2*/
    LVD_3 = 3U		/**< Voltage detection 3*/
}
LVD_VoltageDetection;

typedef int (*lvd_api_enable_interrupt)(struct device *dev, LVD_VoltageDetection voltage);
typedef int (*lvd_api_disable_interrupt)(struct device *dev, LVD_VoltageDetection voltage);
typedef int (*lvd_api_clear_interrupt_flag)(struct device *dev, LVD_VoltageDetection voltage);
typedef int (*lvd_api_lock)(struct device *dev, LVD_VoltageDetection voltage);
typedef int (*lvd_api_reset_level)(struct device *dev, LVD_VoltageDetection voltage, uint32_t level);
typedef int (*lvd_api_check_reset_status)(struct device *dev);
typedef int (*lvd_api_clear_reset_status)(struct device *dev);
typedef int (*lvd_api_set_config)(struct device *dev, lvd_cfg cfg);
typedef int (*lvd_api_enable)(struct device *dev, lvd_cfg *data);
typedef int (*lvd_api_get_VDX_flag)(struct device *dev, int source);




struct lvd_driver_api {
	lvd_api_enable_interrupt enable_interrupt;
	lvd_api_disable_interrupt disable_interrupt;
	lvd_api_clear_interrupt_flag clear_interrupt_flag;
	lvd_api_lock lock;
	lvd_api_reset_level reset_level;
	lvd_api_check_reset_status check_reset_status;
	lvd_api_clear_reset_status clear_reset_status;
	lvd_api_set_config set_config;
	lvd_api_enable enable;
	lvd_api_get_VDX_flag get_VDX_flag;
};

__syscall int lvd_enable_interrupt(struct device *dev, LVD_VoltageDetection voltage);

static inline int _impl_lvd_enable_interrupt(struct device *dev, LVD_VoltageDetection voltage)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->enable_interrupt(dev, voltage);
}

__syscall int lvd_disable_interrupt(struct device *dev, LVD_VoltageDetection voltage);

static inline int _impl_lvd_disable_interrupt(struct device *dev, LVD_VoltageDetection voltage)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->disable_interrupt(dev, voltage);
}

__syscall int lvd_clear_interrupt_flag(struct device *dev, LVD_VoltageDetection voltage);

static inline int _impl_lvd_clear_interrupt_flag(struct device *dev, LVD_VoltageDetection voltage)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->clear_interrupt_flag(dev, voltage);
}

__syscall int lvd_lock(struct device *dev, LVD_VoltageDetection voltage);

static inline int _impl_lvd_lock(struct device *dev, LVD_VoltageDetection voltage)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->lock(dev, voltage);
}

__syscall int lvd_reset_level(struct device *dev, LVD_VoltageDetection voltage, uint32_t level);

static inline int _impl_reset_level(struct device *dev, LVD_VoltageDetection voltage, uint32_t level)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->reset_level(dev, voltage, level);
}

__syscall int lvd_check_reset_status(struct device *dev);

static inline int _impl_lvd_check_reset_status(struct device *dev)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->check_reset_status(dev);
}

__syscall int lvd_clear_reset_status(struct device *dev);

static inline int _impl_lvd_clear_reset_status(struct device *dev)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->clear_reset_status(dev);
}

__syscall int lvd_set_config(struct device *dev, lvd_cfg cfg);

static inline int _impl_lvd_set_config(struct device *dev, lvd_cfg cfg)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->set_config(dev, cfg);
}

__syscall int lvd_enable(struct device *dev, lvd_cfg *data);

static inline int _impl_lvd_enable(struct device *dev, lvd_cfg *data)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->enable(dev, data);
}

__syscall int lvd_get_VDX_flag(struct device *dev, int source);

static inline int _impl_lvd_get_VDX_flag(struct device *dev, int source)
{
	const struct lvd_driver_api *api = dev->driver_api;

	return api->get_VDX_flag(dev, source);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/lvd.h>

#endif
