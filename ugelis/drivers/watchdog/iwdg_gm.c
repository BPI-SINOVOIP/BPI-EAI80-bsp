/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/**
 * @brief Driver for Independent Watchdog (IWDG) for GM MCUs
 *
 * Based on reference manual:
 *  GM6721
 *   advanced ARM ® -based 32-bit MCUs
 *
 *
 */

#include <watchdog.h>
#include <soc.h>
#include <errno.h>

#include "iwdg_gm.h"


#define AS_IWDG(__base_addr) \
	(struct iwdg_gm *)(__base_addr)

#define DEV_CFG(dev)                                                                \
	((const struct iwdg_gm_config * const)(dev)->config->config_info)
#define DEV_DATA(dev)                                                             \
	((struct iwdg_gm_data * const)(dev)->driver_data)
#define IWDG_STRUCT(dev)                                                 \
	((IWDG_TypeDef *)(DEV_CFG(dev))->base)



/* watchdog reload value in ms */
static unsigned int resettime_b =0;

static void iwdg_gm_enable(struct device *dev)
{
	struct iwdg_gm_data *data = DEV_DATA(dev);
	IWDG_HandleTypeDef *pHIWDG = &data->hiwdg;
	
	pHIWDG->Instance =  IWDG_STRUCT(dev);
    HAL_IWDG_START(pHIWDG);
}

static void iwdg_gm_disable(struct device *dev)
{
	/* watchdog cannot be stopped once started */
	ARG_UNUSED(dev);
}
/*
accord to divider value ,confirm to prescaler value
*/
static int iwdg_gm_get_div_value(uint32_t prescaler)
{
    uint32_t ret  = 0;

	switch(prescaler)
	{
		case IWDG_CLKDIV_FACTOR_4:
			ret = 4;
			break;
		case IWDG_CLKDIV_FACTOR_8:
			ret = 8;
			break;
		case IWDG_CLKDIV_FACTOR_16:
			ret = 16;
			break;
		case IWDG_CLKDIV_FACTOR_32:
			ret = 32;
			break;
		case IWDG_CLKDIV_FACTOR_64:
			ret = 64;
			break;
		case IWDG_CLKDIV_FACTOR_128:
			ret = 128;
			break;
		case IWDG_CLKDIV_FACTOR_256:
			ret = 256;
			break;
		case IWDG_CLKDIV_FACTOR256:
			ret = 256;
			break;
        default:
			ret = 128;
			break;

   }
   return ret;
}
	
static int iwdg_gm_set_config(struct device *dev,
				struct wdt_config *config)
{

	struct iwdg_gm_data *data = DEV_DATA(dev);
	IWDG_HandleTypeDef *pHIWDG = &data->hiwdg;
	pHIWDG->Instance =	IWDG_STRUCT(dev);

	resettime_b = config->timeout;

	pHIWDG->Init.Prescaler= config->clkdiv;
	pHIWDG->Init.Reload= (((HAL_IWDG_CLKSRC/1000)* config->timeout)/iwdg_gm_get_div_value(pHIWDG->Init.Prescaler));

	HAL_IWDG_Init(pHIWDG);
	
	return 0;

}

static void iwdg_gm_get_config(struct device *dev,
				struct wdt_config *config)
{
	struct iwdg_gm_data *data = DEV_DATA(dev);
	IWDG_HandleTypeDef *pHIWDG = &data->hiwdg;
	pHIWDG->Instance =	IWDG_STRUCT(dev);

	config->clkdiv = HAL_IWDG_GetPrescaler(pHIWDG);
	config->timeout= resettime_b;
}

static void iwdg_gm_reload(struct device *dev)
{
	struct iwdg_gm_data *data = DEV_DATA(dev);
	IWDG_HandleTypeDef *pHIWDG = &data->hiwdg;

    HAL_IWDG_RELOAD_COUNTER(pHIWDG);
}

static const struct wdt_driver_api iwdg_gm_api = {
	.enable = iwdg_gm_enable,
	.disable = iwdg_gm_disable,
	.get_config = iwdg_gm_get_config,
	.set_config = iwdg_gm_set_config,
	.reload = iwdg_gm_reload,
};

static inline int __iwdg_gm_prescaler(int setting)
{
	int v;
	int i = 0;

	/* prescaler range 4 - 256 */
	for (v = 4; v < 256; v *= 2, i++) {
		if (v == setting)
			return i;
	}
	return i;
}

static int iwdg_gm_init(struct device *dev)
{
//	volatile struct iwdg_gm *iwdg = AS_IWDG(IWDG_BASE);

	/* if you didn't Reset bakcup domain,you would W/R iwdg regs*/
	HAL_IWDG_BKD_SW_Reset();

    /* clock setup is not required, once the watchdog is enabled
	 * LSI oscillator will be forced on and fed to IWD after
	 * stabilization period
	 */

	struct iwdg_gm_data *data = DEV_DATA(dev);
	IWDG_HandleTypeDef *pHIWDG = &data->hiwdg;
	
    pHIWDG->Instance =  IWDG_STRUCT(dev);
	pHIWDG->Init.Prescaler= __iwdg_gm_prescaler(CONFIG_IWDG_GM_PRESCALER);
	pHIWDG->Init.Reload= CONFIG_IWDG_GM_RELOAD_COUNTER;
	
    HAL_IWDG_Init(pHIWDG);

#ifdef CONFIG_IWDG_GM_START_AT_BOOT
	iwdg_gm_enable(dev);
#endif

	return 0;
}

static const struct iwdg_gm_config iwdg_gm_cfg={
	.base = (void *)(CONFIG_IWDG_BASE_ADDRESS),

};


static struct iwdg_gm_data iwdg_gm_dat ;

DEVICE_AND_API_INIT(iwdg_gm, CONFIG_IWDG_GM_DEVICE_NAME, iwdg_gm_init,
					&iwdg_gm_dat, &iwdg_gm_cfg,
					PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
					&iwdg_gm_api);

