/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file 	   acmp.h
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

#ifndef __ACMP_H__
#define __ACMP_H__

#include <stdint.h>
#include <device.h>
#include <gm_hal_acmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief acmp driver APIs
 * @defgroup acmp_interface acmp driver APIs
 * @ingroup io_interfaces
 * @{
 */


/**
 * @brief acmp driver Sequence table
 *
 * This structure defines a list of sequence entries
 * used to execute a sequence of samplings.
 */
struct acmp_config_data {
	ACMP_Config_T   acmp_config;
    ACMP_CompareResult_T result;
	unsigned int dac_convert_val;
    ACMP_Callback_T             callback;  
};

/**
 * @brief acmp driver API
 *
 * This structure holds all API function pointers.
 */
struct acmp_driver_api {
	/** Pointer to the enable routine. */
	void (*enable)(struct device *dev);

	/** Pointer to the disable routine. */
	void (*disable)(struct device *dev);

	/** Pointer to the read routine. */
	int (*read)(struct device *dev, struct acmp_config_data *data);
    int (*config)(struct device *dev,  struct acmp_config_data *data);
};

/**
 * @brief Enable acmp hardware
 *
 * This routine enables the acmp hardware block for data sampling for the
 * specified device.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
static inline void acmp_enable(struct device *dev)
{
	const struct acmp_driver_api *api = dev->driver_api;

	api->enable(dev);
}

/**
 * @brief Disable acmp hardware
 *
 * This routine disables the acmp hardware block for data sampling for the
 * specified device.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
static inline void acmp_disable(struct device *dev)
{
	const struct acmp_driver_api *api = dev->driver_api;

	api->disable(dev);
}

/**
 * @brief Set a read request.
 *
 * This routine sends a read or sampling request to the acmp hardware block.
 * A sequence table describes the read request.
 * The routine returns once the acmp completes the read sequence.
 * The sample data can be retrieved from the memory buffers in
 * the sequence table structure.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param data Pointer to the structure representing the sequence table.
 *
 * @retval 0 On success
 * @retval else Otherwise.
 */
static inline int acmp_read(struct device *dev, struct acmp_config_data *data)
{
	const struct acmp_driver_api *api = dev->driver_api;

	return api->read(dev, data);
}

static inline int acmp_config(struct device *dev, struct acmp_config_data *data)
{
	const struct acmp_driver_api *api = dev->driver_api;

	return api->config(dev, data);
}


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  /* __ACMP_H__ */
