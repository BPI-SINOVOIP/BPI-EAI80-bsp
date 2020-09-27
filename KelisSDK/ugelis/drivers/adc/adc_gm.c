/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		adc_gm.c
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

#include <errno.h>
#include <kernel.h>
#include <misc/util.h>
#include <logging/sys_log.h>
#include <string.h>
#include <init.h>
#include "adc_gm.h"
static int adc_config_disable_irq(struct device *dev);
static int adc_config_irq(struct device *dev);

static inline int adc_gm_sampling(struct device *dev);

/*
static ADC_Callback_T s_adcCallback = NULL;
static void ADC_Callback(ADC_HandleTypeDef *pHander, ADC_Event_T event, unsigned long param);
*/

void adc_irq_handler(void *arg)
{
    struct adc_gm_data *data = DEV_DATA((struct device *)arg);
    ADC_HandleTypeDef *hADC = data->adc_handle;

	HAL_ADC_IRQ_handler(hADC);
}

static inline int adc_gm_sampling(struct device *dev)
{
	int i = 0;
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t convert_value[SEQUECE_LEN_MAX];
	struct adc_gm_data *data = DEV_DATA(dev);
    unsigned char  len = data->adc_handle->Init.NbrOfConversion;

	for (i = 0; i < len; i++)
        convert_value[i] = data->adc_handle->rst_data[i];

	if (ret == HAL_OK) {
		for(i = 0; i < data->seq_table->entries->buffer_length; i++) {
			data->seq_table->entries->buffer[i] = convert_value[i] & 0xFFF;
            /*printk("Channel:%d Convert Success Value:%d\n", i, convert_value[i]);*/
		}
	} else {
		PR_DEBUG("%s:Err:%d ADC_ReadConvertValue\n",__FUNCTION__, ret);
	}

	return ret;
}

static void adc_gm_enable(struct device *dev)
{
	adc_config_irq(dev);
}

static void adc_gm_disable(struct device *dev)
{
	ARG_UNUSED(dev);
    adc_config_disable_irq(dev);
}
 
static int adc_gm_read(struct device *dev,
					struct adc_seq_table *seq_table)
{
	struct adc_gm_data *data = DEV_DATA(dev);
	ADC_HandleTypeDef *adc_handle = data->adc_handle;
	/*const struct adc_gm_config *config = DEV_CFG(dev);*/
    int i = 0;
	unsigned char  len = adc_handle->Init.NbrOfConversion;
	data->seq_table = seq_table;
	HAL_ADC_Start_IT(adc_handle);
	
	/* read convert */
    if (adc_handle->blockStatus == BLOCK)
    {
        while (1)
        {
            /* Exit if adc convert finish */
            if (adc_handle->State == adc_handle->expectFinishStatus){
                break;
            }
			if (i > 1200)
			{
				PR_DEBUG("fun[%s] failed \n", __FUNCTION__);
				break;
			}
			k_sleep(1);
			i++;
        }

        /* store convet value to pConvertData */
        if (adc_handle->expectFinishStatus == HAL_ADC_STATE_CPU_FINISH)
        {
            /*cpu mode*/
            HAL_ADC_FifoConvertValue(adc_handle, adc_handle->Init.Resolution, adc_handle->rst_data, len);
        }
        else
        {
            /* dma mode*/
            HAL_ADC_BufConvertValue(adc_handle, adc_handle->Init.Resolution,  adc_handle->rst_data, len);
        }

        adc_handle->State = HAL_ADC_STATE_IDLE;
    }
    else
    {
     /**
         * noblock mode read
         * 1: after send callback should set hADC->status = IDLE_STATUS
         * 2: after send callback should unlock handlelock ,so not to unlock it here.
         */
        adc_handle->State = HAL_ADC_STATE_BUSY;
        return RET_ONGOING;
    }
	
	adc_gm_sampling(dev);

	return 0;
}

static int adc_gm_write_cfg(struct device *dev,
					struct adc_seq_table *seq_table)
{
	/*PR_DEBUG("enter fun[%s] \n", __FUNCTION__);*/
	int ret = 0;
    int j = 0;
    ADC_ChannelConfTypeDef channel_cfg[32];
	ADC_CompareConfig_T compareConfig ;
    struct adc_seq_entry *entry;
	ADC_HandleTypeDef *adc_handle;
	struct adc_gm_data *data = DEV_DATA(dev);
	data->seq_table = seq_table;
	seq_table->num_entries = 1;
	adc_handle = data->adc_handle;

	if(seq_table->num_entries > 0 ) {
		entry = &seq_table->entries[0];
        adc_handle->Init.Resolution = entry->init.Resolution;
        adc_handle->Init.workMode = entry->init.workMode;
        adc_handle->Init.NbrOfConversion = entry->init.NbrOfConversion;

        for (j = 0; j < entry->init.NbrOfConversion; j++) {
        	channel_cfg[j].SamplingTime = entry->channel_cfg[j].SamplingTime;
			channel_cfg[j].rchn_sel =entry->channel_cfg[j].rchn_sel;
        }

        compareConfig.compareHighValue = entry->compareConfig.compareHighValue;
        compareConfig.compareLowValue = entry->compareConfig.compareLowValue;
        compareConfig.compareMode = entry->compareConfig.compareMode;
        adc_handle->Init.lowpowerEn = entry->init.lowpowerEn;
        adc_handle->Init.discontinuousConvEn = entry->init.discontinuousConvEn;

        adc_handle->Init.trigersel = entry->init.trigersel;
  		adc_handle->Init.dma_en = entry->init.dma_en;
  		adc_handle->Init.trigermode = entry->init.trigermode;
		adc_handle->Init.timeOut = entry->init.timeOut;
    }

	if(adc_handle->Init.timeOut == TIMEOUT_NO_BLOCKING) {
		adc_handle->blockStatus = NOBLOCK;
	}
	else {
		adc_handle->blockStatus = BLOCK;
	}

    
    if(data->adc_handle->Init.dma_en == ENABLE)
   	 	adc_handle->expectFinishStatus = HAL_ADC_STATE_DMA_FINISH;
	else
		adc_handle->expectFinishStatus = HAL_ADC_STATE_CPU_FINISH;	

    ret = HAL_ADC_Init(adc_handle);
    if (ret != RET_OK) {
        PR_ERR("%s:Err ADC_Init\n",__FUNCTION__);

    }

    if (data->adc_handle->Init.compareEn == ENABLE) {
        HAL_ADC_ConfigCompChannel(adc_handle, &compareConfig);
    }

	if (adc_handle->Init.NbrOfConversion == 1)
		HAL_ADC_ConfigChannel(adc_handle, &channel_cfg[0]);

	else if (adc_handle->Init.NbrOfConversion >1)
		HAL_ADC_SequenceConfig(adc_handle, &channel_cfg[0]);

	adc_handle->rst_data[0] = 0;
	adc_handle->dma_rst_len = entry->buffer_length;
    if (adc_handle->Init.dma_en == ENABLE)
    {
		HAL_ADC_DmaConfig(adc_handle, &(adc_handle->rst_data[0]), adc_handle->dma_rst_len);
    }

	return 0;
}

static const struct adc_driver_api adc_gm_api = {
	.enable = adc_gm_enable,
	.disable = adc_gm_disable,
	.read = adc_gm_read,
	.write_cfg = adc_gm_write_cfg,
};

/*
static void ADC_Callback(ADC_HandleTypeDef *pHander, ADC_Event_T event, unsigned long param)
{
	if(event == ADC_EVENT_FINISHED)
	{
		PR_WARNINR("ADC_EVENT_FINISHED\n");
	} else if (event == ADC_EVENT_COMPERE_MATCH) {
		PR_DEBUG("ADC_EVENT_COMPERE_MATCH\n");
	} else if(event == ADC_EVENT_OVERFLOW) {
		PR_WARNINR("ADC_EVENT_OVERFLOW\n");
	} else {
		PR_WARNINR("ADC_EVENT_CONVERT_ERROR\n");
	}
}
*/

static int adc_gm_init(struct device *dev)
{
	PR_DEBUG("enter fun[%s] initialized\n", __FUNCTION__);
    int ret = 0;

	ADC_Device_T *padc = ADC_STRUCT(dev);
	struct adc_gm_data *data = DEV_DATA(dev);

	dev->driver_api = &adc_gm_api;

	if((padc == ADC_DEV0) || (padc == ADC_DEV1) || (padc == ADC_DEV2))
	{
		data->adc_handle->device = padc;
	}

	PR_DEBUG("exit fun[%s] initialized\n", __FUNCTION__);

	return ret;
}


#ifdef CONFIG_ADC_GM

#ifdef CONFIG_ADC_0

static ADC_HandleTypeDef adc_handle0;
static struct adc_gm_data adc0_data = {
	.adc_handle = &adc_handle0,
};

static const struct adc_gm_config adc_config = {
	.base = ADC_DEV0,
};


DEVICE_INIT(ADC0_DEV_NAME, CONFIG_ADC_0_NAME,
			adc_gm_init,
			&adc0_data, &adc_config,
			POST_KERNEL, CONFIG_ADC_INIT_PRIORITY);
#endif

#ifdef CONFIG_ADC_1
static ADC_HandleTypeDef adc_handle1;
static struct adc_gm_data adc1_data = {
	.adc_handle = &adc_handle1,
};

static const struct adc_gm_config adc1_config = {
	.base = ADC_DEV1,
};

DEVICE_INIT(ADC1_DEV_NAME, CONFIG_ADC_1_NAME,
			adc_gm_init,
			&adc1_data, &adc1_config,
			POST_KERNEL, CONFIG_ADC_INIT_PRIORITY);
#endif

#ifdef CONFIG_ADC_2
static ADC_HandleTypeDef adc_handle2;
static struct adc_gm_data adc2_data = {
	.adc_handle = &adc_handle2,
};

static const struct adc_gm_config adc2_config = {
	.base = ADC_DEV2,
};


DEVICE_INIT(ADC2_DEV_NAME, CONFIG_ADC_2_NAME,
			adc_gm_init,
			&adc2_data, &adc2_config,
			POST_KERNEL, CONFIG_ADC_INIT_PRIORITY);
#endif

static int adc_config_irq(struct device *dev)
{
    ADC_Device_T *padc = ADC_STRUCT(dev);

	if(padc == ADC_DEV0)
	{
#ifdef CONFIG_ADC_0	
		IRQ_CONNECT(ADC0_IRQn, 0, adc_irq_handler, DEVICE_GET(ADC0_DEV_NAME), 0);
#endif
		irq_enable(ADC0_IRQn);
	}
	else if(padc == ADC_DEV1)
	{
#ifdef CONFIG_ADC_1	
		IRQ_CONNECT(ADC1_IRQn, 0, adc_irq_handler, DEVICE_GET(ADC1_DEV_NAME), 0);
#endif
		irq_enable(ADC1_IRQn);
	}	
	else if(padc == ADC_DEV2)
	{
#ifdef CONFIG_ADC_2	
		IRQ_CONNECT(ADC2_IRQn, 0, adc_irq_handler, DEVICE_GET(ADC2_DEV_NAME), 0);
#endif
		irq_enable(ADC2_IRQn);
	}

	return 0;
}

static int adc_config_disable_irq(struct device *dev)
{
	ADC_Device_T *padc = ADC_STRUCT(dev);

	if(padc == ADC_DEV0)
	{
		irq_disable(ADC0_IRQn);
	}
	else if(padc == ADC_DEV1)
	{
		irq_disable(ADC1_IRQn);
	}	
	else if(padc == ADC_DEV2)
	{
		irq_disable(ADC2_IRQn);
	}

	return 0;
}

#endif
