/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		dac.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		DAC source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

/**
 * @file
 * @brief Public APIs for GPIO drivers
 */

#ifndef __INCLUDE_DAC_H__
#define __INCLUDE_DAC_H__

#include <stdint.h>
#include <device.h>
#include <gm_hal_dac.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dac driver APIs
 * @defgroup dac_interface dac driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief dac driver Sequence table
 *
 * This structure defines a list of sequence entries
 * used to execute a sequence of samplings.
 */
struct dac_config_data {
	DAC_Config_T config_data;
	unsigned int dac_data_out;
};

/**
 * @brief dac driver API
 *
 * This structure holds all API function pointers.
 */
struct dac_driver_api {
	/** Pointer to the enable routine. */
	void (*enable)(struct device *dev);

	/** Pointer to the disable routine. */
	void (*disable)(struct device *dev);

	/** Pointer to the read routine. */
	int (*read)(struct device *dev, struct dac_config_data *data);
    int (*config)(struct device *dev,  struct dac_config_data *data);
	int (*triger)(struct device *dev,  struct dac_config_data *data);
};

/**
 * @brief Enable dac hardware
 *
 * This routine enables the dac hardware block for data sampling for the
 * specified device.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
static inline void dac_enable(struct device *dev)
{
	const struct dac_driver_api *api = dev->driver_api;

	api->enable(dev);
}

/**
 * @brief Disable dac hardware
 *
 * This routine disables the dac hardware block for data sampling for the
 * specified device.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
static inline void dac_disable(struct device *dev)
{
	const struct dac_driver_api *api = dev->driver_api;

	api->disable(dev);
}

/**
 * @brief Set a read request.
 *
 * This routine sends a read or sampling request to the dac hardware block.
 * A sequence table describes the read request.
 * The routine returns once the dac completes the read sequence.
 * The sample data can be retrieved from the memory buffers in
 * the sequence table structure.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param data Pointer to the structure representing the sequence table.
 *
 * @retval 0 On success
 * @retval else Otherwise.
 */
static inline int dac_read(struct device *dev, struct dac_config_data *data)
{
	const struct dac_driver_api *api = dev->driver_api;

	return api->read(dev, data);
}

static inline int dac_config(struct device *dev, struct dac_config_data *data)
{
	const struct dac_driver_api *api = dev->driver_api;

	return api->config(dev, data);
}

static inline int dac_triger(struct device *dev, struct dac_config_data *data)
{
	const struct dac_driver_api *api = dev->driver_api;

	return api->triger(dev, data);
}



/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  /* __INCLUDE_DAC_H__ */
