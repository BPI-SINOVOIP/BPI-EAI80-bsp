/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file        adc.h
*
* @author      Richard.Liu
*
* @version     v1.0.0
*
* @date        2019/01/25
*
* @brief       DAC source file
*
* @note
*              2019/01/25, Richard.Liu, v1.0.0
*                  Initial version.
*
**/

#ifndef __INCLUDE_ADC_H__
#define __INCLUDE_ADC_H__

#include <ugelis/types.h>
#include <device.h>
#if defined(CHIP_GM6721) || defined(CONFIG_ADC_GM)
    #include <gm_hal_adc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define gm_adc_enable _impl_adc_enable
#define gm_adc_read _impl_adc_read
#define gm_adc_write_cfg adc_write_cfg

/**
 * @brief ADC driver APIs
 * @defgroup adc_interface ADC driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief ADC driver Sequence entry
 *
 * This structure defines a sequence entry used
 * to define a sample from a specific channel.
 */
struct adc_seq_entry
{
#if !(defined(CHIP_GM6721) || defined(CHIP_F6721B))
    /** Clock ticks delay before sampling the ADC. */
    int32_t sampling_delay;

    /** Buffer pointer where the sample is written.*/
    uint8_t *buffer;
    /** Length of the sampling buffer.*/
    uint32_t buffer_length;
    /** Channel ID that should be sampled from the ADC */
    uint8_t channel_id;

    uint8_t stride[3];
#endif

#if defined(CHIP_GM6721) || defined(CONFIG_ADC_GM) || defined(CHIP_F6721B)
    /** Buffer pointer where the sample is written.*/
    uint32_t *buffer;
    /** Length of the sampling buffer.*/
    uint32_t buffer_length;
    ADC_InitTypeDef              init;

    ADC_ChannelConfTypeDef channel_cfg[32];
    ADC_CompareConfig_T compareConfig;
#endif
};

/**
 * @brief ADC driver Sequence table
 *
 * This structure defines a list of sequence entries
 * used to execute a sequence of samplings.
 */
struct adc_seq_table
{
    /* Pointer to a sequence entry array. */
    struct adc_seq_entry *entries;

    /* Number of entries in the sequence entry array. */
    uint8_t num_entries;
    uint8_t stride[3];
};

/**
 * @brief ADC driver API
 *
 * This structure holds all API function pointers.
 */
struct adc_driver_api
{
    /** Pointer to the enable routine. */
    void (*enable)(struct device *dev);

    /** Pointer to the disable routine. */
    void (*disable)(struct device *dev);

    /** Pointer to the read routine. */
    int (*read)(struct device *dev, struct adc_seq_table *seq_table);
#if defined(CHIP_GM6721) || defined(CONFIG_ADC_GM)
    int (*write_cfg)(struct device *dev, struct adc_seq_table *seq_table);
#endif
};

/**
 * @brief Enable ADC hardware
 *
 * This routine enables the ADC hardware block for data sampling for the
 * specified device.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
//__syscall void adc_enable(struct device *dev);

static inline void _impl_adc_enable(struct device *dev)
{
    const struct adc_driver_api *api = dev->driver_api;

    api->enable(dev);
}

/**
 * @brief Disable ADC hardware
 *
 * This routine disables the ADC hardware block for data sampling for the
 * specified device.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @return N/A
 */
//__syscall void adc_disable(struct device *dev);

static inline void _impl_adc_disable(struct device *dev)
{
    const struct adc_driver_api *api = dev->driver_api;

    api->disable(dev);
}

/**
 * @brief Set a read request.
 *
 * This routine sends a read or sampling request to the ADC hardware block.
 * A sequence table describes the read request.
 * The routine returns once the ADC completes the read sequence.
 * The sample data can be retrieved from the memory buffers in
 * the sequence table structure.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param seq_table Pointer to the structure representing the sequence table.
 *
 * @retval 0 On success
 * @retval else Otherwise.
 */
//__syscall int adc_read(struct device *dev, struct adc_seq_table *seq_table);

static inline int _impl_adc_read(struct device *dev,
                                 struct adc_seq_table *seq_table)
{
    const struct adc_driver_api *api = dev->driver_api;

    return api->read(dev, seq_table);
}

/**
 * @brief Set a write request.
 *
 * This routine sends a read or sampling request to the ADC hardware block.
 * A sequence table describes the read request.
 * The routine returns once the ADC completes the read sequence.
 * The sample data can be retrieved from the memory buffers in
 * the sequence table structure.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param seq_table Pointer to the structure representing the sequence table.
 *
 * @retval 0 On success
 * @retval else Otherwise.
 */
#if defined(CONFIG_ADC_GM)
static inline int adc_write_cfg(struct device *dev, struct adc_seq_table *seq_table)
{
    const struct adc_driver_api *api = dev->driver_api;

    return api->write_cfg(dev, seq_table);
}
#endif


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

//#include <syscalls/adc.h>

#endif  /* __INCLUDE_ADC_H__ */
