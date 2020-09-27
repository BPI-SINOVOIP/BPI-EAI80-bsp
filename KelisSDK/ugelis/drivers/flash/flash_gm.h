/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2017 BayLibre, SAS.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FLASH_GM_H_
#define _FLASH_GM_H_
#ifdef __cplusplus
extern "C" {
#endif


#include <gm_soc.h>
#include <gm_hal_sqi.h>



/* driver config */

struct flash_gm_config_t
{

    uint32_t     base;
    SQI_Config_T hConfig;
    SQI_Device_T *pSQIDev;
};


/* driver data */
struct falsh_gm_data_t
{

    /*basic*/
    SQI_Handle_T *hSQI;
    struct k_sem *sem;
};


/* convenience defines */
#define DEV_CFG(dev)                            \
    (( struct flash_gm_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct falsh_gm_data_t *)(dev)->driver_data)
#define DEV_STRUCT(dev)                        \
    ((SQI_Device_T *)((DEV_CFG(dev))->base))



#ifdef __cplusplus
}
#endif

#endif /* _FLASH_GM_H_ */

