/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Driver for  Watchdog (wwdg) for GM67xx MCUs
 *
 * Based on reference manual:
 *   GM6721
 *   advanced ARM ? -based 32-bit MCUs
 *
 * 
 *
 */

#include <watchdog.h>
#include <soc.h>
#include <errno.h>

#include "wwdg_gm.h"
#include <misc/printk.h>


/*gm hal inc file*/
#include <gm_hal_wwdg.h>


/* convenience defines */
#define DEV_CFG(dev)                                                                \
	((const struct wwdg_gm_config * const)(dev)->config->config_info)
#define DEV_DATA(dev)                                                             \
	((struct wwdg_gm_data * const)(dev)->driver_data)
#define WWDG_STRUCT(dev)                                                 \
	((WDOG_Device_T *)(DEV_CFG(dev))->base)
	
/* watchdog reload value in ms */
static unsigned int reload_s  =0;
static unsigned int mode = 0;
static unsigned int workEwiMode = 0;

static void wwdg_gm_isr(void *arg)
{
	/*
	 * Check if the watchdog was the reason of the NMI interrupt
	 * and if not, exit immediately	 */

   struct device *dev = arg;

   ARG_UNUSED(dev);

   HAL_WDOG_IRQHandler();

}


static void wwdg_gm_enable(struct device *dev)
{
	struct wwdg_gm_data *data = DEV_DATA(dev);
	WDOG_Handle_T *pWWDG_Handler = &data->hwwdg;
	
	ARG_UNUSED(dev); 
    HAL_WDOG_Enable(pWWDG_Handler);

}

static void wwdg_gm_disable(struct device *dev)
{
    ARG_UNUSED(dev); 

	struct wwdg_gm_data *data = DEV_DATA(dev);
	WDOG_Handle_T *pWWDG_Handler = &data->hwwdg;
	
	/* watchdog cannot be stopped once started */
	HAL_WDOG_Disable(pWWDG_Handler);

}   

static int wwdg_gm_set_config(struct device *dev,
				struct wdt_config *config)
{

	ARG_UNUSED(dev);

	reload_s = config->timeout;
	mode     = config->mode;

	struct wwdg_gm_data *data = DEV_DATA(dev);
	WDOG_Handle_T *pHWWDG = &data->hwwdg;
	WDOG_Device_T *pWWDG  = WWDG_STRUCT(dev);
	WDOG_Config_T wwdg_config;

	pHWWDG->Instance = WWDG_STRUCT(dev);
	wwdg_config.clkDivFactor = config->clkdiv;
	wwdg_config.workEwiMode  = config->workmode;
	wwdg_config.resetTime    = config->timeout;
	wwdg_config.windowTime   = config->windowtimeout;
	wwdg_config.winLen       = config->winLen;
	wwdg_config.mode         = config->mode;
	
   if((wwdg_config.mode  == WDT_MODE_INTERRUPT_RESET) && \
   	 ((wwdg_config.workEwiMode == WWDG_NORMAL_EWI) || \
   	  (wwdg_config.workEwiMode ==WWDG_WINDOW_EWI)))
   {
        const struct wwdg_gm_config *config_irq = DEV_CFG(dev);
	    wwdg_config.callBack = config->interrupt_fn;
		data->callback = config->interrupt_fn;
        config_irq->irq_config_func(dev);
   }
   else
   {
         wwdg_config.callBack = NULL;
   }

    HAL_WDOG_Init( pWWDG,&wwdg_config,pHWWDG);

	return 0;
}

static void wwdg_gm_get_config(struct device *dev,
				struct wdt_config *config)
{

	struct wwdg_gm_data *data = DEV_DATA(dev);


	ARG_UNUSED(dev);

	config->timeout = reload_s;
	config->mode = mode;
	config->workmode =  workEwiMode;
	
    if((config->mode  == WDT_MODE_INTERRUPT_RESET) && \
		((config->workmode == WWDG_NORMAL_EWI) ||  \
		(config->workmode ==WWDG_WINDOW_EWI)))
    {
	   config->interrupt_fn = data->callback;
    }
	else
	{
	  config->interrupt_fn = NULL;
	}
	
}

static void wwdg_gm_reload(struct device *dev)
{

   struct wwdg_gm_data *data = DEV_DATA(dev);
   WDOG_Handle_T *pWWDG_Handler = &data->hwwdg;

   ARG_UNUSED(dev);
   /*FeedWdog*/
   HAL_WDOG_FeedWdog(pWWDG_Handler);

}


static const struct wdt_driver_api wwdg_gm_api = {
	.enable = wwdg_gm_enable,
	.disable = wwdg_gm_disable,
	.get_config = wwdg_gm_get_config,
	.set_config = wwdg_gm_set_config,
	.reload = wwdg_gm_reload,
};



static inline int __wwdg_gm_workmode(int setting)
{

	int res = 0;

	switch(setting)
	{
		case 0:
			res = WDOG_NORMAL_MODE;
			break;
		case 1:
			res = WDOG_WINDOW_MODE;
			break;
		case 2:
			res = WDOG_NORMAL_EWI;
			break;
		case 3:
			res = WDOG_WINDOW_EWI;
			break;
		default:
			res = WDOG_NORMAL_MODE;
			break;
	}
	return res;

}

static int wwdg_gm_init(struct device *dev)
{
	struct wwdg_gm_data *data = DEV_DATA(dev);
	WDOG_Handle_T *pHWWDG = &data->hwwdg;
	WDOG_Device_T *pWWDG  = WWDG_STRUCT(dev);
    WDOG_Config_T wwdg_config;

	pHWWDG->Instance = WWDG_STRUCT(dev);
    wwdg_config.clkDivFactor= CONFIG_WWDG_GM_DIV_FACTOR;
	wwdg_config.workEwiMode= __wwdg_gm_workmode(CONFIG_WWDG_GM_MODE);
	wwdg_config.resetTime  = CONFIG_WWDG_GM_RELOAD_COUNTER;
	wwdg_config.windowTime = CONFIG_WWDG_GM_WINDOW_VALUE;
	wwdg_config.winLen     = CONFIG_WWDG_GM_WINLEN;
	wwdg_config.mode       = WDT_MODE_RESET;

    HAL_WDOG_Init( pWWDG,&wwdg_config,pHWWDG);
	

#ifdef CONFIG_WWDG_GM_START_AT_BOOT
	wwdg_gm_enable(dev);
#endif

	return 0;
}

static void wwdg_config_func(struct device *dev);

static const struct wwdg_gm_config wwdg_gm_cfg={
	.base = (void *)(CONFIG_WWDG_BASE_ADDRESS),	
	.irq_config_func =  wwdg_config_func,
};

static struct wwdg_gm_data wwdg_gm_data ;


DEVICE_AND_API_INIT(wwdg_gm, CONFIG_WWDG_GM_DEVICE_NAME, wwdg_gm_init,
		    &wwdg_gm_data,&wwdg_gm_cfg,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    &wwdg_gm_api);



static void wwdg_config_func(struct device *dev)
{
	ARG_UNUSED(dev);
	
	IRQ_CONNECT(WDOG_IRQn,0,wwdg_gm_isr, DEVICE_GET(wwdg_gm),0);
	
	irq_enable(WDOG_IRQn);
}




