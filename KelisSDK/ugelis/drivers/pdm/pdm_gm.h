#ifndef _PDM_GM_H_
#define _PDM_GM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include<pdm.h>
#include<gm_hal_pdm.h>

/* convenience defines */
#define DEV_CFG(dev)                            \
    (( struct pdm_gm_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct pdm_gm_data_t *)(dev)->driver_data)
#define DEV_STRUCT(dev)                        \
    ((PDM_Device_T *)((DEV_CFG(dev))->base))


/* device config */
struct pdm_gm_config_t
{
    uint32_t                           base;
    PDM_Config_T                       config;

};
/* driver data */
struct pdm_gm_data_t
{
    /*tranfer control used*/
    struct k_work_q workq;
    struct k_delayed_work delaywork;
    struct k_mutex mutex;
    /*basic*/
    // mmc_card_on_off_cb_t cb;
    PDM_Handle_T *pdm_handle;
    struct device *gpio_dev;
    /*card hotplut detect mode*/
    // enum mmc_detect_mode detect_mode;
};

#ifdef __cplusplus
}
#endif

#endif /* _MMC_GM_H_ */

