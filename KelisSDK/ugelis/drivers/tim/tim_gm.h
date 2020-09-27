/*
 * Copyright (c) 2016, Gree Instruments Incorporated.
 *
 */

#ifndef __TIM_GM_H__
#define __TIM_GM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include<tim.h>
#include<gm_hal_tim.h>


/* convenience defines */
#define DEV_CFG(dev)                            \
    (( struct tim_gm_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct tim_gm_data_t *)(dev)->driver_data)
#define DEV_STRUCT(dev)                        \
    ((TIM_Device_T *)((DEV_CFG(dev))->base))

/** Configuration data */
struct tim_gm_config_t
{
    uint32_t base;
};

/** Runtime driver data */
struct tim_gm_data_t
{
    /* TIM peripheral handler */
    TIM_Handle_T *tim_handle;
    struct device *tim_dev;
    struct k_mutex mutex;
};

#ifdef __cplusplus
}
#endif

#endif /* __TIM_GM_H__ */
