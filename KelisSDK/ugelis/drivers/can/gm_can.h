#ifndef _CAN_GM_H_
#define _CAN_GM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include<can.h>
#include<gm_hal_can.h>
#include <gm_soc.h>

/* convenience defines */
#define DEV_CFG(dev)                            \
    (( struct can_gm_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct can_gm_data_t *)(dev)->driver_data)
#define DEV_STRUCT(dev)                        \
    ((CAN_Device_T *)((DEV_CFG(dev))->base))



/* device config */
struct can_gm_config_t
{
    uint32_t                           base;
    CAN_Config_T                       config;
};

/* driver data */
struct can_gm_data_t
{
    /*tranfer control used*/
    struct k_work_q workq;
    struct k_delayed_work delaywork;
    struct k_mutex mutex;
    /*basic*/
    can_rx_tx_cb_t cb;
    CAN_Handle_T *can_handle;
    struct device *gpio_dev;
};


static inline void can_claim_init(struct device *dev)
{
    struct can_gm_data_t *data = DEV_DATA(dev);
    k_mutex_init(&data->mutex);
}

static inline void can_claim_host(struct device *dev)
{
    struct can_gm_data_t *data = DEV_DATA(dev);
    k_mutex_lock(&data->mutex, K_FOREVER);
}

static inline void can_release_host(struct device *dev)
{
    struct can_gm_data_t *data = DEV_DATA(dev);
    k_mutex_unlock(&data->mutex);
}

#ifdef __cplusplus
}
#endif

#endif /* _MMC_GM_H_ */

