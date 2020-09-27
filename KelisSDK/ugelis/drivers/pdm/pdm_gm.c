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
#include <pdm.h>
#include "pdm_gm.h"
#include <gm_hal_pdm.h>
#include <gm_hal_dma.h>


#define PDM_BUF_LEN 16000*2 //equal to main.c whose transfer_length
extern void pdmAReceiveCallback(struct PDM_Handle_T *pPDM, uint8_t event, unsigned long param);
uint16_t tranf_buf[PDM_BUF_LEN];
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
    static int gm_pdm_pm_control(struct device *device, uint32_t command, void *context);
#endif

static int gm_pdm_config(struct device *dev);
static int gm_pdm_exit(struct device *dev);
static int gm_pdm_write(struct device *dev, uint16_t *buf, uint32_t buf_length);

//static  int gm_mmc_set_callback(struct device *dev, mmc_card_on_off_cb_t cb);
//static void gm_mmc_isr(uint32_t param);
//static int gm_mmc_irq_config(struct device *dev);
static int gm_pdm_init(struct device *dev);

//static K_THREAD_STACK_DEFINE(wrokq_tack, WORKQ_STACK_SIZE);

static const struct pdm_driver_api pdm_driver_api_funcs =
{
    .pdm_config             = gm_pdm_config,
    //.mmc_set_callback       = gm_mmc_set_callback,
    .pdm_write              = gm_pdm_write,
    .pdm_exit               = gm_pdm_exit,
};

#ifdef  CONFIG_PDMA
static const struct pdm_gm_config_t pdm_configa =
{
    .base   = (u32_t)PDMA_BASE_ADDRESS
};

static PDM_Handle_T pdm_handle_a;

static  struct pdm_gm_data_t pdm_data_a =
{
    .pdm_handle         = &pdm_handle_a,
    //.cb                 = NULL,
};

/*todo this only remove compiler waring that pm_control not call*/
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(gm_pdm, DEV_PDMA_NAME, gm_pdm_init, gm_pdm_pm_control, \
              &pdm_data_a, &pdm_configa, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  &pdm_driver_api_funcs) ;
#else

DEVICE_DEFINE(gm_pdm, DEV_PDMA_NAME, gm_pdm_init, NULL, \
              &pdm_data_a, &pdm_configa, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  &pdm_driver_api_funcs) ;

#endif
#endif

#ifdef  CONFIG_PDMB
static const struct pdm_gm_config_t pdm_configb =
{
    .base   = (u32_t)PDMB_BASE_ADDRESS
};

static PDM_Handle_T pdm_handle_b;

static  struct pdm_gm_data_t pdm_data_b =
{
    .pdm_handle         = &pdm_handle_b,
    .cb                 = NULL,
};

/*todo this only remove compiler waring that pm_control not call*/
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(gm_pdm, DEV_PDMB_NAME, gm_pdm_init, gm_pdm_pm_control, \
              &pdm_data_b, &pdm_configb, POST_KERNEL, DEVICE_PRIORITY_PDM,  &pdm_driver_api_funcs) ;
#else

DEVICE_DEFINE(gm_pdm, DEV_PDMB_NAME, gm_pdm_init, NULL, \
              &pdm_data_b, &pdm_configb, POST_KERNEL, DEVICE_PRIORITY_PDM,  &pdm_driver_api_funcs) ;

#endif
#endif

static int gm_pdm_init(struct device *dev)
{
    int ret = 0;
    return ret;
}

static int gm_pdm_config(struct device *dev)
{
    int ret = 0;
    PDM_Config_T gm_config_pdm;
    PDM_Device_T *pPDM = DEV_STRUCT(dev);
    struct pdm_gm_data_t *data = DEV_DATA(dev);
    PDM_Handle_T *hpdm  = data->pdm_handle;
    gm_config_pdm = hpdm->config;
    gm_config_pdm.DataSize            = PDM_DATASIZE_32;
    gm_config_pdm.Fifo_Thr            = PDM_FIFO_THRESHOLD_FULL;
    gm_config_pdm.Cid                 = DISABLE;
    gm_config_pdm.CH_FLAG             = PDM_CH_RIGHT;
    gm_config_pdm.Capt                = ENABLE;
    gm_config_pdm.Shift_Level         = PDM_SHIFT_LEVEL_6;
    gm_config_pdm.bckdiv              = PDM_BCK_1M;
    gm_config_pdm.FS                  = PDM_FS_16K;
    gm_config_pdm.Dual_Ch             = PDM_Dual_LR;  //double channel
    gm_config_pdm.NBSlot              = PDM_NB_SLOT_2;
    gm_config_pdm.SlotSize            = PDM_SLOTSZ_32;
    gm_config_pdm.enableDMA           = TRUE;
    gm_config_pdm.pdm_callback        = pdmAReceiveCallback;
    ret = HAL_PDM_Init(hpdm, pPDM, &gm_config_pdm);
    return ret;
}

static int gm_pdm_write(struct device *dev, uint16_t *buf, uint32_t buf_length)
{
    int ret = 0;
    struct pdm_gm_data_t *data = DEV_DATA(dev);
    PDM_Handle_T *hpdm  = data->pdm_handle;
    ret = HAL_PDM_Receive_DMA(hpdm, PDM_MASTER_MODE, tranf_buf, buf_length);

    return ret;
}

static int gm_pdm_exit(struct device *dev)
{
    int ret = 0;
    struct pdm_gm_data_t *data = DEV_DATA(dev);
    PDM_Handle_T *hpdm  = data->pdm_handle;
    HAL_DMA_Abort(hpdm->hDMA);
    ret = HAL_PDM_Exit(hpdm);

    return ret;
}
#if 0
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


#endif

#ifdef CONFIG_DEVICE_POWER_MANAGEMENT

static int gm_pdm_pm_control(struct device *device, uint32_t command, void *context)
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


























