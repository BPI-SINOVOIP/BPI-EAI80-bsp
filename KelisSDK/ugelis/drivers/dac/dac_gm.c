/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file        dac_gm.c
*
* @author      Richard.Liu
*
* @version     v1.0.0
*
* @date        2019/01/24
*
* @brief       DAC source file
*
* @note
*              2019/01/24, Richard.Liu, v1.0.0
*                  Initial version.
*
**/

#include <errno.h>
#include <kernel.h>
#include <misc/util.h>
#define SYS_LOG_NO_NEWLINE
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_ADC_LEVEL
#include <logging/sys_log.h>
#include <string.h>
#include <init.h>

#include "dac_gm.h"
#include <gm_hal_dac.h>
#include <gm_hal_gpio.h>
#include "dac.h"


/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
/******************************************************************************
* Local functions
******************************************************************************/
static int dac_config_disable_irq(struct device *dev);
static int dac_config_irq(struct device *dev);

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
static void dac_irq_handler(void *arg)
{
    DAC_Handle_T dac_handle;

    struct device *dev = arg;
    struct dac_gm_data *data = DEV_DATA(dev);
    dac_handle = data->handle;

    /*clear irq*/
    DAC_CLEAR_INTERRUPT(dac_handle.device);

}

static void dac_gm_enable(struct device *dev)
{
    DAC_Device_T *pdac = DAC_STRUCT(dev);

    DAC_ENABLE(pdac);
    dac_config_irq(dev);

    PR_DEBUG("func[%s]:pdac=0x%x \r\n", __FUNCTION__, pdac);
}

static void dac_gm_disable(struct device *dev)
{
    DAC_Device_T *pdac = DAC_STRUCT(dev);
    DAC_DISABLE(pdac);
    DAC_SET_WAVE_TYPE(pdac, 0);
    dac_config_disable_irq(dev);
}

static int dac_gm_read(struct device *dev,
                       struct dac_config_data *data)
{
    DAC_Device_T *pdac = DAC_STRUCT(dev);
    pdac = pdac;

    struct dac_gm_data *dac = DEV_DATA(dev);

    data->dac_data_out = HAL_DAC_GetDataOut(&(dac->handle));

    PR_DEBUG("fun[%s], dac_data_out=[0x%x], dac address=0x%x\r\n", __FUNCTION__, data->dac_data_out, pdac);

    return 0;
}

static int dac_gm_configure(struct device *dev,
                            struct dac_config_data *cfg_data)
{
    RET_CODE ret = RET_OK;

    DAC_Device_T *pdac = DAC_STRUCT(dev);
    struct dac_gm_data *data = DEV_DATA(dev);

    PR_DEBUG("%s\r\n", __FUNCTION__);

#if defined(CHIP_F6721B)
    if ((pdac == DAC0_DEV))
    {
        data->handle.device = pdac;
        ret = HAL_DAC_Init(&(data->handle), pdac, &(cfg_data->config_data));
    }
    else
    {
        ret = RET_ERROR;
    }
#elif defined(CHIP_GM6721)
    if ((pdac == DAC0_DEV) || (pdac == DAC1_DEV))
    {
        data->handle.device = pdac;
        ret = HAL_DAC_Init(&(data->handle), pdac, &(cfg_data->config_data));
    }
    else
    {
        ret = RET_ERROR;
    }
#endif

    data->handle.dma_rst_len = 1;
    data->handle.rst_data[0] = 0xFFF;

    /*if (s_dacHandle.config.dacDMAEn == ENABLE)
        HAL_DAC_DmaConfig(&(data->handle),  &(data->handle.rst_data[0]), data->handle.dma_rst_len);
    */

    return ret;
}

static int dac_gm_triger(struct device *dev,
                         struct dac_config_data *cfg_data)
{
    RET_CODE ret = RET_OK;

    DAC_Device_T *pdac = DAC_STRUCT(dev);
    struct dac_gm_data *data = DEV_DATA(dev);

    PR_DEBUG("%s\r\n", __FUNCTION__);
#if defined(CHIP_GM6721)
    if ((pdac == DAC0_DEV) || (pdac == DAC1_DEV))
    {
        data->handle.device = pdac;
        ret = HAL_DAC_Triger(&(data->handle), &(cfg_data->config_data));
    }
    else
    {
        ret = RET_ERROR;
    }
#endif

#if defined(CHIP_F6721B)
    if ((pdac == DAC0_DEV))
    {
        data->handle.device = pdac;
        ret = HAL_DAC_Triger(&(data->handle), &(cfg_data->config_data));
    }
    else
    {
        ret = RET_ERROR;
    }
#endif
    PR_DEBUG("pdac=0x%x \n", pdac);

    return ret;
}


const static  struct dac_driver_api dac_gm_api =
{
    .enable = dac_gm_enable,
    .disable = dac_gm_disable,
    .read = dac_gm_read,
    .config = dac_gm_configure,
    .triger = dac_gm_triger
};

static int dac_gm_init(struct device *dev)
{
    RET_CODE ret = RET_OK;
    dev->driver_api = &dac_gm_api;

    PR_DEBUG("enter fun[%s] initialized\n", __FUNCTION__);

    return ret;
}

#ifdef CONFIG_DAC_GM
#ifdef CONFIG_DAC_0
static struct dac_gm_data l_dac0_data;

static const struct dac_gm_config l_dac_config =
{
    .base = DAC0_DEV,
};

DEVICE_INIT(DAC0_DEV_NAME, CONFIG_DAC_0_NAME,
            dac_gm_init,
            &l_dac0_data, &l_dac_config,
            POST_KERNEL, CONFIG_DAC_INIT_PRIORITY);
#endif

#ifdef CONFIG_DAC_1
static struct dac_gm_data l_dac1_data;

static const struct dac_gm_config l_dac1_config =
{
#if defined(CHIP_GM6721)
    .base = DAC1_DEV,
#endif
};


DEVICE_INIT(DAC1_DEV_NAME, CONFIG_DAC_1_NAME,
            dac_gm_init,
            &l_dac1_data, &l_dac1_config,
            POST_KERNEL, CONFIG_DAC_INIT_PRIORITY);
#endif

static int dac_config_irq(struct device *dev)
{
    DAC_Device_T *pdac = DAC_STRUCT(dev);

    if (pdac == DAC0_DEV)
    {
        IRQ_CONNECT(DAC0_IRQn, 1, dac_irq_handler, DEVICE_GET(DAC0_DEV_NAME), 0);
        irq_enable(DAC0_IRQn);
    }
#if defined(CHIP_GM6721)
    else if (pdac == DAC1_DEV)
    {
        IRQ_CONNECT(DAC1_IRQn, 1, dac_irq_handler, DEVICE_GET(DAC1_DEV_NAME), 0);
        irq_enable(DAC1_IRQn);
    }
#endif
    else
    {

    }

    return 0;
}

static int dac_config_disable_irq(struct device *dev)
{
    DAC_Device_T *pdac = DAC_STRUCT(dev);

    if (pdac == DAC0_DEV)
    {
        irq_disable(DAC0_IRQn);
    }
#if defined(CHIP_GM6721)
    else if (pdac == DAC1_DEV)
    {
        irq_disable(DAC1_IRQn);
    }
#endif
    else
    {

    }

    return 0;
}
#endif
