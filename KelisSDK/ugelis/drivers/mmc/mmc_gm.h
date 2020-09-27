#ifndef _MMC_GM_H_
#define _MMC_GM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include<mmc.h>
#include<gm_hal_mmc.h>

 /* convenience defines */
#define DEV_CFG(dev)							\
                   (( struct mmc_gm_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)							\
                   ((struct mmc_gm_data_t *)(dev)->driver_data)
 #define DEV_STRUCT(dev)                        \
                     ((MMC_Device_T *)((DEV_CFG(dev))->base))


enum mmc_detect_mode {
    MMC_GPIO_DETECT,
    MMC_POLL_DETECT
};

struct mmc_input_pin_desc_t{
    char        *gpio_port;
    uint32_t    pin;
    bool        pullup;
    bool        vaild_level;
    char        *desc;
};


 /* device config */
 struct mmc_gm_config_t {
    uint32_t                           base;
    const struct mmc_input_pin_desc_t  *cd_pin;
    MMC_Config_T                       config;

 };

 /* driver data */
  struct mmc_gm_data_t {
    /*tranfer control used*/
    struct k_work_q workq;
    struct k_delayed_work delaywork;
    struct k_mutex mutex;
    /*basic*/
    mmc_card_on_off_cb_t cb;
    MMC_Handle_T *mmc_handle;
    struct device *gpio_dev;
    /*card hotplut detect mode*/
    enum mmc_detect_mode detect_mode;
 };


static inline void mmc_claim_init(struct device *dev)
{
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    k_mutex_init(&data->mutex);
}

static inline void mmc_claim_host(struct device *dev)
{
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    k_mutex_lock(&data->mutex, K_FOREVER);
}

static inline void mmc_release_host(struct device *dev)
{
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    k_mutex_unlock(&data->mutex);
}

#ifdef __cplusplus
}
#endif

#endif /* _MMC_GM_H_ */

