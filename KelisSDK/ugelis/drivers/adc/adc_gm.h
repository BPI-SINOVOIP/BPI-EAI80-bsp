/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		adc_gm.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		adc source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef __ADC_GM_H__
#define __ADC_GM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <adc.h>
#include <misc/byteorder.h>
#include <gm_hal_adc.h>
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

#define ADC_SAMPLING_BUFFER_SIZE		8
#define ADC_CHANNELS			16
#define ADC_CHANNELS_SIZE		(ADC_CHANNELS * sizeof(uint32_t))

struct adc_gm_config {
	const ADC_Device_T *base;
};

struct adc_gm_data {

	ADC_HandleTypeDef *adc_handle;
	ADC_InitTypeDef               Init;                        /*!< ADC required parameters */
	__IO uint32_t                 NbrOfCurrentConversionRank;  /*!< ADC number of current conversion rank */

   //DMA_HandleTypeDef             *DMA_Handle;                 /*!< Pointer DMA Handler */

	uint32_t sampling_buffer[ADC_SAMPLING_BUFFER_SIZE];
	struct adc_seq_table *seq_table;
    int               Lock;                        /*!< ADC locking object */

    __IO HAL_ADC_StateTypeDef     State;                       /*!< ADC communication state */

    __IO uint32_t                 ErrorCode;                   /*!< ADC Error code */
	uint8_t cmd_buf_len;
	uint8_t sampling_buf_len;
	uint8_t stride[2];
};

#define DEV_CFG(dev) \
	((const struct adc_gm_config *)(dev)->config->config_info)

#define DEV_DATA(dev) \
	((struct adc_gm_data *)(dev)->driver_data)

#define ADC_STRUCT(dev)                                                 \
	((ADC_Device_T *)(DEV_CFG(dev))->base)

#define  ADC0_DEV_NAME     adc0_dev
#define  ADC1_DEV_NAME     adc1_dev
#define  ADC2_DEV_NAME     adc2_dev


#ifdef __cplusplus
}
#endif

#endif /* __ADC_GM_H__ */
