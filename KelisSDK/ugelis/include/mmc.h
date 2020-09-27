/*
 * Copyright (c) 2017 ARM Ltd
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public APIs for MMC drivers
 */

#ifndef __MMC_H__
#define __MMC_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __USE_KEIL_
#include <errno_ex.h>
#else
#include <errno.h>
#endif
#include <stddef.h>
#include <device.h>


#define DEV_MMC0_NAME "MMC0"//should config for dts


enum mmc_slot_status  {
    MMC_REMOVE_SLOT = 0,
    MMC_INSERD_SLOT    ,
};


typedef bool (*mmc_in_slot_t)(struct device *dev);
typedef int (*mmc_card_on_off_cb_t)(enum mmc_slot_status  status);
typedef int (*mmc_set_callback_t)(struct device *dev, mmc_card_on_off_cb_t cb);
typedef uint32_t (*mmc_get_capacity_t)(struct device *dev);
typedef int (*mmc_read_t)(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, uint8_t *buf);
typedef int (*mmc_write_t)(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, const uint8_t *buf);

/** @brief mmc driver API definition. */
struct mmc_driver_api  {
    mmc_in_slot_t       mmc_in_slot;
    mmc_set_callback_t  mmc_set_callback;
    mmc_read_t          mmc_read;
	mmc_write_t         mmc_write;
    mmc_get_capacity_t  mmc_get_capacity;
};



/**
 * @brief check whether the card is in the slot.
 *
 *
 * @param dev mmc device structure.
 *
 * @retval ture If the card is in the slot.
 * @retval false Otherwise.
 */
__syscall  bool mmc_in_slot(struct device *dev);

static inline bool _impl_mmc_in_slot(struct device *dev)
{
	struct mmc_driver_api *api;

	api = (struct mmc_driver_api *)dev->driver_api;

	return api->mmc_in_slot(dev);
}

/**
 * @brief set callback to notice user the card is remove or inserd
 *
 * @param dev mmc device structure.
 * @param cb callback function.
 *
 * @retval 0 If successful.
 * @retval failed Otherwise.
 */
__syscall  int mmc_set_callback(struct device *dev, mmc_card_on_off_cb_t cb);
static inline int _impl_mmc_set_callback(struct device *dev, mmc_card_on_off_cb_t cb)
{
	struct mmc_driver_api *api;

	api = (struct mmc_driver_api *)dev->driver_api;

	return api->mmc_set_callback(dev,cb);
}



/**
 * @brief read mmc device block data
 *
 * @param dev mmc device structure.
 * @param sector_addr start read sector
 * @param sector_nr read the number of sector
 * @buf the buf use to load read mmc data
 *
 * @retval sector_nr If successful.
 * @retval failed Otherwise.
 */
__syscall int mmc_read(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, uint8_t *buf);

static inline int _impl_mmc_read(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, uint8_t *buf)
{
	struct mmc_driver_api *api;

	api = (struct mmc_driver_api *)dev->driver_api;
	return api->mmc_read(dev, sector_addr,sector_nr, buf);
}

/**
 * @brief write mmc device block data
 *
 * @param dev mmc device structure.
 * @param sector_addr start read sector
 * @param sector_nr write the number of sector
 * @buf the buf use to store read mmc data
 *
 * @retval sector_nr If successful.
 * @retval failed Otherwise.
 */

__syscall int mmc_write(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, const uint8_t *buf);
static inline int  _impl_mmc_write(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, const uint8_t *buf)
{
	struct mmc_driver_api *api;

	api = (struct mmc_driver_api *)dev->driver_api;

	return api->mmc_write(dev, sector_addr,sector_nr, buf);
}

/**
 * @brief get mmc device block caoacity
 *
 * @param dev mmc device structure.
 *
 * @retval mmc device block caoacity (uint Mbyte)
 */

__syscall uint32_t mmc_get_capacity(struct device *dev);
static inline uint32_t _impl_mmc_get_capacity(struct device *dev)
{
	struct mmc_driver_api *api;

	api = (struct mmc_driver_api *)dev->driver_api;

	return api->mmc_get_capacity(dev);
}



#ifdef __cplusplus
}
#endif

#include <syscalls/mmc.h>

#endif /* _MMC_H_ */


