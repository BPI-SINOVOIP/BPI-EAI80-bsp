/*
* Copyright (c) 2016 Linaro Limited.
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file Header file for OV5640 CMOS Image Sensor.
 */
#ifndef __GPIO_SCCB_H
#define __GPIO_SCCB_H

//#include <ugelis/types.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief sccb driver API definition. */
struct sccb_driver_api
{
    int (*start)(struct device *dev);
    int (*stop)(struct device *dev);
    int (*nack)(struct device *dev);
    uint8_t (*sendbyte)(struct device *dev, uint8_t data);
    uint8_t (*readbyte)(struct device *dev);
};


/**
 * @brief Driver sccb start.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int sccb_start(struct device *dev);

static inline int _impl_sccb_start(struct device *dev)
{
    const struct sccb_driver_api *api = dev->driver_api;

    if (!api->start)
    {
        return -ENOTSUP;
    }

    return api->start(dev);
}

/**
 * @brief Driver sccb stop.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int sccb_stop(struct device *dev);

static inline int _impl_sccb_stop(struct device *dev)
{
    const struct sccb_driver_api *api = dev->driver_api;

    if (!api->stop)
    {
        return -ENOTSUP;
    }

    return api->stop(dev);
}

/**
 * @brief Driver sccb nack.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int sccb_nack(struct device *dev);

static inline int _impl_sccb_nack(struct device *dev)
{
    const struct sccb_driver_api *api = dev->driver_api;

    if (!api->nack)
    {
        return -ENOTSUP;
    }

    return api->nack(dev);
}

/**
 * @brief Driver sccb sendbyte.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall uint8_t sccb_sendbyte(struct device *dev, uint8_t data);

static inline uint8_t _impl_sccb_sendbyte(struct device *dev, uint8_t data)
{
    const struct sccb_driver_api *api = dev->driver_api;

    if (!api->sendbyte)
    {
        return -ENOTSUP;
    }

    return api->sendbyte(dev, data);
}

/**
 * @brief Driver sccb start.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall uint8_t sccb_readbyte(struct device *dev);

static inline uint8_t _impl_sccb_readbyte(struct device *dev)
{
    const struct sccb_driver_api *api = dev->driver_api;

    if (!api->readbyte)
    {
        return -ENOTSUP;
    }

    return api->readbyte(dev);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/gpio_sccb.h>

#endif /* __GPIO_SCCB_H */

