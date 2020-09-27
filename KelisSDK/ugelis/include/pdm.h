/*
 * Copyright (c) 2017 ARM Ltd
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public APIs for PDM drivers
 */

#ifndef __PDM_H__
#define __PDM_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __USE_KEIL_
#include <errno_ex.h>
#else
#include <errno.h>
#endif
#include <ugelis/types.h>
#include <stddef.h>
#include <device.h>


#define DEV_PDMB_NAME "PDMB"    //should config for dts
#define DEV_PDMA_NAME "PDMA"
typedef int(*pdm_config_t)(struct device *dev);
typedef int(*pdm_write_t)(struct device *dev, uint16_t *buf, uint32_t buf_length);
typedef int(*pdm_exit_t)(struct device *dev);

/** @brief pdm driver API definition. */

struct pdm_driver_api
{
    pdm_config_t        pdm_config;
    //pdm_set_callback_t  pdm_set_callback;
    pdm_write_t         pdm_write;
    pdm_exit_t          pdm_exit;
};


/**
 * @brief pdm init
 *
 * @param dev pdm device structure.
 *
 * @retval 0 If successful.
 * @retval failed Otherwise.
 */
__syscall  int pdm_config(struct device *dev);
static inline int _impl_pdm_config(struct device *dev)
{
    struct pdm_driver_api *api;

    api = (struct pdm_driver_api *)dev->driver_api;

    return api->pdm_config(dev);
}

/**
 * @brief write pdm device block data
 *
 * @param dev pdm device structure.
 * @param buf
 * @param buf_length
 *
 * @retval sector_nr If successful.
 * @retval failed Otherwise.
 */

__syscall int pdm_write(struct device *dev, uint16_t *buf, uint32_t buf_length);
static inline int  _impl_pdm_write(struct device *dev, uint16_t *buf, uint32_t buf_length)
{
    struct pdm_driver_api *api;

    api = (struct pdm_driver_api *)dev->driver_api;

    return api->pdm_write(dev, buf, buf_length);
}

/**
 * @brief exit pdm device
 *
 * @param dev pdm device structure.
 *
 * @retval sector_nr If successful.
 * @retval failed Otherwise.
 */

__syscall int pdm_exit(struct device *dev);
static inline int _impl_pdm_exit(struct device *dev)
{
    struct pdm_driver_api *api;

    api = (struct pdm_driver_api *)dev->driver_api;

    return api->pdm_exit(dev);
}


#ifdef __cplusplus
}
#endif

#include <syscalls/pdm.h>

#endif /* _PDM_H_ */


