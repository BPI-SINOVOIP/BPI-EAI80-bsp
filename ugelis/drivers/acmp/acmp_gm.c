 /**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        acmp_gm.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       ACMP source file
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
#include "acmp_gm.h"
#include <gm_hal_acmp.h>
#include <gm_hal_gpio.h>

#ifndef DEBUG_PRINTF_ACMP
	#define PR_DEBUG_ACMP(...)		do{}while(0)
#else
	#define PR_DEBUG_ACMP			printf
#endif

#ifndef ERR_PRINTF_ACMP
	#define PR_ERR_ACMP(...) 	   do{}while(0)
#else
	#define PR_ERR_ACMP			   printf
#endif

#ifndef WARNING_PRINTF_ACMP
	#define PR_WARNINR_ACMP(...)    do{}while(0)
#else
	#define PR_WARNINR_ACMP		   printf
#endif


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
static void ACMP_Callback(ACMP_Handle_T *pHandle,ACMP_Event_T event, unsigned long Param);

static int acmp_config_disable_irq(struct device *dev);
static int acmp_config_irq(struct device *dev);

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
static void ACMP_Callback(ACMP_Handle_T *pHandle,ACMP_Event_T event, unsigned long Param)
{
    static int count = 0;
    if (event == ACMP_MATCH_EVENT)
    {
    	count++;
        PR_DEBUG_ACMP("driver negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", count);
    }
}

RET_CODE acmp_init_config(ACMP_Handle_T *pHandle, ACMP_Config_T *acmo_config)
{
    RET_CODE ret = RET_OK;

    ACMP_Config_T *pconfig = &(pHandle->config);
    pconfig->callback = (ACMP_Callback_T)ACMP_Callback;

    pconfig->outputInversionEnable = acmo_config->outputInversionEnable;

    pconfig->filterThr = acmo_config->filterThr;

    pconfig->negativePin = acmo_config->negativePin;
    pconfig->positivePin = acmo_config->positivePin;
    pconfig->outputPinEnable = acmo_config->outputPinEnable;
    pconfig->triggerMode = acmo_config->triggerMode;
	pconfig->blankTimesel = acmo_config->blankTimesel;

    ret = HAL_ACMP_Init(pHandle, ACMP_DEV, pconfig);
    if(ret != RET_OK)
    {
        PR_ERR_ACMP("%s:ACMP_Init Error ret:%d\n",__FUNCTION__,ret);
        goto out;
    }

out:
    return ret;
}

static void acmp_irq_handler(void *arg)
{
    ACMP_Handle_T handle;

    struct device *dev = arg;
    /*const struct acmp_gm_config *config = DEV_CFG(dev);*/
    struct acmp_gm_data *data = DEV_DATA(dev);
    handle = data->handle;

	if (handle.device == NULL)
	{
		PR_ERR_ACMP("acmp device address not configure,warning\n");
		return;
	}
	/*clear irq*/
    ACMP_CLEAR_INTERRUPT(handle.device);

    PR_DEBUG_ACMP("%s:%d\r\n", __FUNCTION__, __LINE__);
    handle.config.callback((void *)&handle, ACMP_MATCH_EVENT, 0);
}

static void acmp_gm_enable(struct device *dev)
{
    ACMP_Device_T *pacmp = ACMP_STRUCT(dev);

    PR_DEBUG_ACMP("pacmp device address = 0x%x\n", (ACMP_Device_T *)pacmp);

    acmp_config_irq(dev);
	ACMP_ENABLE(pacmp);
}

static void acmp_gm_disable(struct device *dev)
{
    ARG_UNUSED(dev);
    acmp_config_disable_irq(dev);
}

static int acmp_gm_read(struct device *dev,
                    struct acmp_config_data *config_data)
{

    PR_DEBUG_ACMP("enter func:%s\r\n", __FUNCTION__);
    struct acmp_gm_data *acmp = DEV_DATA(dev);

    config_data->result  = HAL_ACMP_GetCompareRestult(&(acmp->handle));

    PR_DEBUG_ACMP("config_data->result  = 0x%x\n",config_data->result );
    return 0;
}

static int acmp_gm_configure(struct device *dev, struct acmp_config_data *config_data)
{
    int ret;

    ACMP_Device_T *pacmp = ACMP_STRUCT(dev);
    struct acmp_gm_data *data = DEV_DATA(dev);

    /*PR_DEBUG("%s:device=0x%x, &pacmp = 0x%x\r\n", __FUNCTION__, pacmp, &pacmp);*/

    data->handle.device = pacmp;

    ret = acmp_init_config(&(data->handle), &(config_data->acmp_config));
    if (ret != RET_OK)
    {
        PR_ERR_ACMP("%s:Init Error ret:%d\n",__FUNCTION__,ret);
    }

    return ret;
}

const static  struct acmp_driver_api acmp_gm_api = {
    .enable = acmp_gm_enable,
    .disable = acmp_gm_disable,
    .read = acmp_gm_read,
    .config = acmp_gm_configure
};

static int acmp_gm_init(struct device *dev)
{
    PR_DEBUG_ACMP("enter fun[%s] initialized\n", __FUNCTION__);

    /*struct acmp_gm_data *acmp = dev->driver_data;*/

    dev->driver_api = &acmp_gm_api;

    PR_DEBUG_ACMP("exit fun[%s] initialized\n", __FUNCTION__);

    return 0;
}

#ifdef CONFIG_ACMP_GM
static struct acmp_gm_data acmp_data;

static const struct acmp_gm_config s_acmp_config = {
    .base = ACMP_DEV,
};

DEVICE_INIT(acmp_gm, CONFIG_ACMP_0_NAME,
            acmp_gm_init,
            &acmp_data, &s_acmp_config,
            POST_KERNEL, CONFIG_ACMP_INIT_PRIORITY);

static int acmp_config_irq(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(ACMP_IRQn, 1, acmp_irq_handler, DEVICE_GET(acmp_gm), 0);
    irq_enable(ACMP_IRQn);

    return 0;
}

static int acmp_config_disable_irq(struct device *dev)
{
    ARG_UNUSED(dev);

    irq_disable(ACMP_IRQn);

    return 0;
}

#endif