#include <string.h>
#include <errno.h>
#include <board.h>
#include <kernel.h>
#include <arch/cpu.h>
#include <misc/__assert.h>
#include <misc/printk.h>
#include <board.h>
#include <init.h>
#include <uart.h>
#include <power.h>
#include <gpio.h>
#include <mmc.h>
#include "mmc_gm.h"




#define DEBUG()  __asm("BKPT")

#define WORKQ_STACK_SIZE            1024

#define MMC_SCAN_PERIOD             800  // 800ms
#define ERR_RETER_CNT               3     //retry 3 time for io error

//#define MMC_DEBUG 1
#define MMC_ERR   1


#define SECTORS_TO_MB(sectors) ((sectors)/(2048))

#ifndef MMC_DEBUG
    #define mmc_debug(...)      do {} while (0)
#else
    #define mmc_debug           printk
#endif

#ifndef MMC_ERR
    #define mmc_err(...)        do {} while (0)
#else
    #define mmc_err             printk
#endif



#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
    static int gm_mmc_pm_control(struct device *device, uint32_t command, void *context);
#endif

static int gm_mmc_init(struct device *dev);
static int gm_mmc_hotplug_detect_config(struct device *dev);
static int gm_mmc_hal_init(struct device *dev);
static uint32_t gm_mmc_get_capacity(struct device *dev);
static  int gm_mmc_read(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, uint8_t *buf);
static int gm_mmc_write(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, const uint8_t *buf);

#ifdef HOTPLUG_GPIO_DETECT
    static int gm_mmc_cd_gpio_config(struct device *dev);
    static void gm_cd_gpio_hotplug_callback(struct device *gpio_dev, struct gpio_callback *ca, uint32_t pins);
    static void mmc_gpio_detect_hotplug(struct k_work *work);
#endif

#ifdef CONFIG_HOTPLUG_POLL_DETECT
    static void mmc_poll_detect_hotplug(struct k_work *work);
#endif


static void mmc_detect_hotplug(struct k_work *work);
static bool gm_mmc_in_slot(struct device *dev);
static  int gm_mmc_set_callback(struct device *dev, mmc_card_on_off_cb_t cb);
static void gm_mmc_isr(uint32_t param);
static int gm_mmc_irq_config(struct device *dev);


static K_THREAD_STACK_DEFINE(wrokq_tack, WORKQ_STACK_SIZE);

static const struct mmc_driver_api mmc_driver_api_funcs =
{
    .mmc_in_slot            = gm_mmc_in_slot,
    .mmc_set_callback       = gm_mmc_set_callback,
    .mmc_read               = gm_mmc_read,
    .mmc_write              = gm_mmc_write,
    .mmc_get_capacity       = gm_mmc_get_capacity,
};

#ifdef  MMC0_LABEL

static MMC_Handle_T mmc_handle0;

static  struct mmc_gm_data_t mmc_data0 =
{
    .mmc_handle         = &mmc_handle0,
    .cb                 = NULL,
};

/* cd pin */
static const struct mmc_input_pin_desc_t mmc_cd_pin0 =
{
    .gpio_port      = "GPIOC",
    .pin            = 8,
    .pullup         = true,
    .vaild_level    = 0,
    .desc           = "cd",
};

static const struct mmc_gm_config_t mmc_config0 =
{
    .cd_pin             = &mmc_cd_pin0,
    .base               = (u32_t)MMC0_BASE_ADDRESS,
    .config.busWitch    = CONFIG_MMC0_BUSWITCH,
    .config.type        = CONFIG_MMC0_CARD_TYPE,
    .config.removable   = CONFIG_MMC0_CARD_REMOVABLE,
};


/*todo this only remove compiler waring that pm_control not call*/
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(gm_mmc, DEV_MMC0_NAME, gm_mmc_init, gm_mmc_pm_control, \
              &mmc_data0, &mmc_config0, POST_KERNEL, DEVICE_PRIORITY_MMC,  &mmc_driver_api_funcs) ;
#else

DEVICE_DEFINE(gm_mmc, DEV_MMC0_NAME, gm_mmc_init, NULL, \
              &mmc_data0, &mmc_config0, POST_KERNEL, DEVICE_PRIORITY_MMC,  &mmc_driver_api_funcs) ;

#endif

#endif

static int gm_mmc_init(struct device *dev)
{
    int ret = 0;
    /* mutex for mul task acess hardware*/
    mmc_claim_init(dev);
    ret = gm_mmc_irq_config(dev);
    if (ret)
    {
        mmc_err("%s:gm_mmc_irq_config err:%d\n", __FUNCTION__, ret);
        goto out;
    }

    ret = gm_mmc_hal_init(dev);
    if (!ret)
    {
        mmc_debug("mmc init ok\n");
    }
    else
    {
        mmc_debug("mmc init fail\n");
    }


    ret = gm_mmc_hotplug_detect_config(dev);
    if (ret)
    {
        mmc_err("%s:gm_mmc_hotplug_detect_config err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}


static int gm_mmc_hotplug_detect_config(struct device *dev)
{

    int ret = 0;
    struct mmc_gm_data_t *data = DEV_DATA(dev);

#ifdef CONFIG_HOTPLUG_POLL_DETECT
    data->detect_mode = MMC_POLL_DETECT;
#else
    data->detect_mode = MMC_GPIO_DETECT;
#endif

    /*init mmc detect work*/
    k_delayed_work_init(&data->delaywork, mmc_detect_hotplug);
    k_work_q_start(&data->workq, wrokq_tack, WORKQ_STACK_SIZE, -1);
    if (data->detect_mode == MMC_GPIO_DETECT)
    {
#ifdef  HOTPLUG_GPIO_DETECT
        ret = gm_mmc_cd_gpio_config(dev);
        if (ret)
        {
            mmc_err("ERR:%s:gm_mmc_cd_gpio_config\n", __FUNCTION__);
            return ret;
        }
#endif
    }
    else
    {
        k_delayed_work_submit_to_queue(&data->workq, &data->delaywork, 0);
    }

    return ret;
}

static int gm_mmc_hal_init(struct device *dev)
{
    int ret = 0;
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC = data->mmc_handle;

    ret = HAL_MMC_Init(hMMC, (MMC_Device_T *)(DEV_STRUCT(dev)), &(DEV_CFG(dev)->config));
    if (ret)
    {
        mmc_err("Err:%d HAL_SD_Init\n", ret);
        goto out;
    }

    mmc_debug("SD Card Init OK\n");

out:

    return ret;
}

static uint32_t gm_mmc_get_capacity(struct device *dev)
{
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC  = data->mmc_handle;
    uint64_t sectors;

    sectors = HAL_MMC_GetCapcity(hMMC);
    return SECTORS_TO_MB(sectors);
}

static  int gm_mmc_read(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, uint8_t *buf)
{
    uint8_t err_cnt = 0;
    uint32_t blk_cnt;
    int ret = 0;

    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC  = data->mmc_handle;

    mmc_claim_host(dev);

retry:
    blk_cnt = HAL_MMC_Read(hMMC, sector_addr, sector_nr, buf);
    if (blk_cnt != sector_nr)
    {
        if ((err_cnt++ > ERR_RETER_CNT) && (HAL_MMC_GetCd(hMMC) == MMC_IN_SLOT))
        {
            mmc_err("%s:retry %d time\n", __FUNCTION__, err_cnt);
            goto retry;
        }
    }

    mmc_release_host(dev);

    if (blk_cnt != sector_nr)
    {
        ret = -EIO;
    }

    return ret;
}

static int gm_mmc_write(struct device *dev, uint32_t sector_addr, uint32_t sector_nr, const uint8_t *buf)
{
    uint8_t err_cnt = 0;
    uint32_t blk_cnt;
    int ret = 0;

    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC  = data->mmc_handle;

    mmc_claim_host(dev);

retry:

    blk_cnt = HAL_MMC_Write(hMMC, sector_addr, sector_nr, buf);
    if (blk_cnt != sector_nr)
    {
        if ((err_cnt++ > ERR_RETER_CNT) && (HAL_MMC_GetCd(hMMC) == MMC_IN_SLOT))
        {
            mmc_err("%s:retry %d time\n", __FUNCTION__, err_cnt);
            goto retry;
        }
    }
    mmc_release_host(dev);

    if (blk_cnt != sector_nr)
    {
        ret = -EIO;
    }

    return ret;
}


#ifdef HOTPLUG_GPIO_DETECT
static int gm_mmc_cd_gpio_config(struct device *dev)
{
    int ret = 0;

    uint32_t pull = 0;
    const struct mmc_input_pin_desc_t *cd_pin;
    struct device *dev_gpio;
    struct gpio_callback *gpio_callback_config = NULL;

    cd_pin = DEV_CFG(dev)->cd_pin;

    gpio_callback_config = k_malloc(sizeof(struct gpio_callback));

    if (!gpio_callback_config)
    {
        mmc_err("Err:%d %s : k_malloc\n", ret, __FUNCTION__);
        ret = -EADDRNOTAVAIL;
        goto out;
    }

    /*config callback*/

    dev_gpio = device_get_binding(cd_pin->gpio_port);
    if (!dev_gpio)
    {
        ret = -EINVAL;
        goto out;
    }
    /*config pin for pullup or pulldown*/
    if (cd_pin->pullup == true)
    {
        pull = GPIO_PUD_PULL_UP;
    }
    else
    {
        pull = GPIO_PUD_PULL_DOWN;
    }

    /*both elge will generate gpio irq*/
    gpio_pin_configure(dev_gpio, cd_pin->pin,
                       GPIO_INT | pull | (GPIO_INT_EDGE | GPIO_INT_DOUBLE_EDGE));
    gpio_init_callback(gpio_callback_config, gm_cd_gpio_hotplug_callback, BIT(cd_pin->pin));
    gpio_add_callback(dev_gpio, gpio_callback_config);
    gpio_pin_enable_callback(dev_gpio, cd_pin->pin);

    return ret;

out:
    k_free(gpio_callback_config);

    return ret;
}

static bool gm_cd_gpio_is_inserd_level(struct device *dev)
{
    uint32_t val = TRUE;

    struct mmc_gm_data_t *data = DEV_DATA(dev);
    const struct mmc_input_pin_desc_t *cd_pin = DEV_CFG(dev)->cd_pin;

    gpio_pin_read(data->gpio_dev, cd_pin->pin, &val);

    return val;

}

static void gm_cd_gpio_hotplug_callback(struct device *gpio_dev, struct gpio_callback *ca, uint32_t pins)
{

    struct mmc_gm_data_t *data = DEV_DATA(DEVICE_GET(gm_mmc));

    UNUSED(ca);
    UNUSED(pins);
    data->gpio_dev = gpio_dev;
    k_delayed_work_submit_to_queue(&data->workq, &data->delaywork, MMC_SCAN_PERIOD);

}

static void mmc_gpio_detect_hotplug(struct k_work *work)
{

    struct device *dev = DEVICE_GET(gm_mmc);
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC  = data->mmc_handle;
    enum mmc_slot_status slot_status;

    if (gm_cd_gpio_is_inserd_level(dev) == TRUE)
    {
        /**
         *card is insert
         *sometimes there is sharkdouble so we must
         *check card is still inslot  or status from off->in
         */
        mmc_claim_host(dev);
        ret = HAL_MMC_IsActiveStatus(hMMC);

        if (ret == FALSE)
        {
            /*card is off->ininit card again*/
            ret = HAL_MMC_Indentifier(hMMC);
            mmc_release_host(dev);
            if (!ret)
            {
                slot_status = MMC_INSERD_SLOT;

                if (data->cb)
                {
                    /*then notice usr card is insert*/
                    data->cb(slot_status);
                }
            }
        }
        else
        {
            mmc_release_host(dev);
            return;
        }

    }
    else
    {
        /**
          * card is remove
          * only card status from on->off notice usr
          */
        if (HAL_MMC_GetCd(hMMC) == MMC_IN_SLOT)
        {
            /*set card status is remved*/
            HAL_MMC_SetCd(hMMC, MMC_OFF_SLOT);
            slot_status = MMC_REMOVE_SLOT;
            if (data->cb)
            {
                /*then notice usr card is insert*/
                data->cb(slot_status);
            }
        }
    }

}

#endif

#ifdef CONFIG_HOTPLUG_POLL_DETECT

static void mmc_poll_detect_hotplug(struct k_work *work)
{
    struct device *dev = DEVICE_GET(gm_mmc);
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC  = data->mmc_handle;
    enum mmc_slot_status slot_status;
    int ret = 0;

    mmc_claim_host(dev);

    if (HAL_MMC_GetCd(hMMC) == MMC_IN_SLOT)
    {
        ret = HAL_MMC_IsActiveStatus(hMMC);
        mmc_release_host(dev);
        if (ret == TRUE)
        {
            /*sd card is still in slot, detect stop here*/
            mmc_debug("mmc is alive\n");

            goto out;
        }
        else
        {
            /*card remvoe:on-->off*/
            slot_status = MMC_OFF_SLOT;
            HAL_MMC_SetCd(hMMC, MMC_OFF_SLOT);
            mmc_debug("####################################mmc remove : ON--->OFF########################\n");
            if (data->cb)
            {
                /*then notice usr card is insert*/
                data->cb(slot_status);
            }

        }
    }
    else
    {
        /*card is not in slot ,so we must check whether it is insert sometime*/
        ret = HAL_MMC_Indentifier(hMMC);
        mmc_release_host(dev);
        if (!ret)
        {
            slot_status = MMC_INSERD_SLOT;
            mmc_debug("####################################mmc inserd : OFF--->ON########################\n");

            if (data->cb)
            {
                /*then notice usr card is insert*/
                data->cb(slot_status);
            }
        }
    }

out:

    k_delayed_work_submit_to_queue(&data->workq, &data->delaywork, MMC_SCAN_PERIOD);

}

#endif


static void mmc_detect_hotplug(struct k_work *work)
{
    struct device *dev = DEVICE_GET(gm_mmc);
    struct mmc_gm_data_t *data = DEV_DATA(dev);

    if (data->detect_mode == MMC_GPIO_DETECT)
    {
#ifdef  HOTPLUG_GPIO_DETECT
        mmc_gpio_detect_hotplug(work);
#endif
    }
    else
    {
#ifdef CONFIG_HOTPLUG_POLL_DETECT
        mmc_poll_detect_hotplug(work);
#endif
    }

}


static bool gm_mmc_in_slot(struct device *dev)
{
    struct mmc_gm_data_t *data = DEV_DATA(dev);
    MMC_Handle_T *hMMC  = data->mmc_handle;
    bool ret;

    mmc_claim_host(dev);

    if (HAL_MMC_GetCd(hMMC) == MMC_IN_SLOT)
    {

        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }

    mmc_release_host(dev);

    return ret;
}


/*notice user card is in or nor*/
static  int gm_mmc_set_callback(struct device *dev, mmc_card_on_off_cb_t cb)
{
    struct mmc_gm_data_t *const dev_data = DEV_DATA(dev);

    dev_data->cb = cb;

    return 0;
}


static void gm_mmc_isr(uint32_t param)
{
    uint8_t  index = 0;
    struct device *dev = (struct device *)param;
    if (DEV_STRUCT(dev) == MMC0)
    {
        index = 0;
    }

    HAL_MMC_IRQHandler(index);
}

static int gm_mmc_irq_config(struct device *dev)
{
    int ret = 0;

    if (DEV_STRUCT(dev) == MMC0)
    {
        IRQ_CONNECT(MMC0_IRQn, MMC0_IRQ_PRIORITY, gm_mmc_isr, DEVICE_GET(gm_mmc), 0);
        irq_enable(MMC0_IRQn);
    }
    else
    {
        ret = - ENXIO;
    }
    return ret;
}




#ifdef CONFIG_DEVICE_POWER_MANAGEMENT

static int gm_mmc_pm_control(struct device *device, uint32_t command, void *context)
{
    int ret = 0;
    uint32_t set_stat ;
    static uint32_t  pm_dev_status = DEVICE_PM_ACTIVE_STATE;

    if (command == DEVICE_PM_SET_POWER_STATE)
    {
        /*todo:get set dev pm status*/
        set_stat = *((int *)context);
        /*bakup dev pm status*/
        pm_dev_status = set_stat;

        if (set_stat == DEVICE_PM_ACTIVE_STATE)
        {
            /*todo:do resume action*/
        }
        else if (set_stat == DEVICE_PM_LOW_POWER_STATE)
        {
            /*todo:only stop clk*/
        }
        else if (set_stat == DEVICE_PM_SUSPEND_STATE)
        {
            /*todo:pm reg lose,should bakup to resume
             *when resume should int pm modules to
             *work mode that enter standby place
             *should enable IRQ
            */
            irq_enable(MMC0_IRQn);
        }
        else if (set_stat == DEVICE_PM_OFF_STATE)
        {
            /*todo:close pm module power,and lose all the thing*/
        }
        else
        {
            return -EINVAL;
        }
    }
    else if (command == DEVICE_PM_GET_POWER_STATE)
    {
        *((uint32_t *)context) = pm_dev_status;
        return pm_dev_status;
    }

    return ret;
}
#endif


























