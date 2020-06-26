#include <string.h>
#include <errno.h>
#include <misc/__assert.h>
#include <misc/printk.h>
#include <kernel.h>
#include <device.h>
#include <string.h>
#include <flash.h>
#include <init.h>
#include <soc.h>
#include <board.h>
#include <arch/cpu.h>
#include "flash_gm.h"




static int gm_flash_init(struct device *dev);
static int flash_gm_read(struct device *dev, off_t offset, void *data, size_t len);
static int flash_gm_erase(struct device *dev, off_t offset, size_t len);
static int flash_gm_write(struct device *dev, off_t offset, const void *data, size_t len);
static int flash_gm_write_protection(struct device *dev, bool enable);





static SQI_Handle_T   sflash_handle;





static const struct flash_driver_api sflash_driver_api_funcs =
{
    .write_protection = flash_gm_write_protection,
    .erase = flash_gm_erase,
    .write = flash_gm_write,
    .read = flash_gm_read,
#ifdef CONFIG_FLASH_PAGE_LAYOUT
    .page_layout = flash_gm_page_layout,
#endif
};


static const struct flash_gm_config_t sflash_config =
{
    .base              =   CONFIG_FLASH_BASE_ADDRESS,
    .hConfig.read_mode =   CONFIG_SFLASH_READMODE,
    .hConfig.bit_mode  =   CONFIG_SFLASH_BITMODE,
    .hConfig.clk_div   =   CONFIG_SFLASH_CLKDIV,
    .pSQIDev           = (SQI_Device_T *)EFLASH_CTRL_BASE,

};

static  struct falsh_gm_data_t sflash_data =
{
    .hSQI         = &sflash_handle,
};


#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(gm_sflash, DEV_FLASH_NAME, gm_flash_init, NULL, \
              &sflash_data, &sflash_config, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  &sflash_driver_api_funcs) ;

#else
DEVICE_DEFINE(gm_sflash, DEV_FLASH_NAME, gm_flash_init, NULL, \
              &sflash_data, &sflash_config, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  &sflash_driver_api_funcs) ;

#endif

/*
 * This is named flash_gm_sem_take instead of flash_gm_lock (and
 * similarly for flash_gm_sem_give) to avoid confusion with locking
 * actual flash pages.
 */

static inline void flash_gm_sem_take(struct device *dev)
{
    k_sem_take(DEV_DATA(dev)->sem, K_FOREVER);
}

static inline void flash_gm_sem_give(struct device *dev)
{
    k_sem_give(DEV_DATA(dev)->sem);
}

static int flash_gm_read(struct device *dev, off_t offset, void *buff,
                         size_t len)
{
    RET_CODE    ret;
    struct falsh_gm_data_t *hdata = DEV_DATA(dev);
    if ((NULL == dev) || ((len + offset) > hdata->hSQI->flashsize))
    {
        return -EINVAL;
    }

    if (!len)
    {
        return 0;
    }

    ret  = HAL_SQI_Read(hdata->hSQI, offset, buff, len);
    if (ret != RET_OK)
    {
        return (-EINVAL);
    }

    return 0;
}

static int flash_gm_erase(struct device *dev, off_t offset, size_t len)
{
    RET_CODE    ret;
    struct falsh_gm_data_t *data = DEV_DATA(dev);
    if ((NULL == dev) || ((len + offset) > data->hSQI->flashsize))
    {
        return -EINVAL;
    }

    if (!len)
    {
        return 0;
    }


    flash_gm_sem_take(dev);

    ret  = HAL_SQI_Erase(data->hSQI, offset, len);
    if (ret != RET_OK)
    {
        flash_gm_sem_give(dev);

        return (-1);
    }

    flash_gm_sem_give(dev);

    return 0;
}

static int flash_gm_write(struct device *dev, off_t offset,
                          const void *buff, size_t len)
{
    RET_CODE    ret;
    struct falsh_gm_data_t *data = DEV_DATA(dev);
    if (NULL == dev)
    {
        return -EINVAL;
    }

    if (!len)
    {
        return 0;
    }

    flash_gm_sem_take(dev);

    ret  = HAL_SQI_Write(data->hSQI, offset, buff, len);
    if (ret != RET_OK)
    {
        flash_gm_sem_give(dev);

        return (-1);
    }

    flash_gm_sem_give(dev);

    return 0;
}

static int flash_gm_write_protection(struct device *dev, bool enable)
{
    return 0;
}

static int gm_flash_init(struct device *dev)
{
    RET_CODE    ret;
    struct falsh_gm_data_t *data = DEV_DATA(dev);
    struct flash_gm_config_t *pconfig = DEV_CFG(dev);
    SQI_Handle_T *hSQI  = data->hSQI;
    printf("********SFlash es Test Begin*******\n");
    ret  = HAL_SQI_Init(hSQI, pconfig->pSQIDev, &(pconfig->hConfig));
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return (-1);
    }

    ret = HAL_SQI_Print_Device(hSQI);
    if (ret != RET_OK)
    {
        printf("********SFlash print Device Error*******\n");

        return (-1);
    }

    k_sem_init(data->sem, 1, 1);

    return 0;
}

