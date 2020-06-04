/*
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 */

#include <logging/sys_log.h>
#include <errno.h>
#include <device.h>
#include <init.h>
#include <kernel.h>
#include <lvd.h>
#include <power.h>
#include <soc.h>
#include <misc/util.h>
#include <gm_hal_lvd.h>

struct gm_lvd_config
{
    void (*irq_config_func)(struct device *dev);
};

struct gm_lvd_data
{
    lvd_cfg config;
};

#define DEV_CFG(dev)                                                                \
    ((const struct gm_lvd_config * const)(dev)->config->config_info)


static int gm_lvd_enable_interrupt(struct device *dev, LVD_VoltageDetection voltage)
{
    HAL_LVD_EnableInterrupt(voltage);
    return 0;
}

static int gm_lvd_disable_interrupt(struct device *dev, LVD_VoltageDetection voltage)
{
    HAL_LVD_DisableInterrupt(voltage);
    return 0;
}

static int gm_lvd_clear_interrupt_flag(struct device *dev, LVD_VoltageDetection voltage)
{
    HAL_LVD_ClearInterruptFlag(voltage);
    return 0;
}

static int gm_lvd_lock(struct device *dev, LVD_VoltageDetection voltage)
{
    HAL_LVD_Lock(voltage);
    return 0;
}

static int gm_lvd_reset_level(struct device *dev, LVD_VoltageDetection voltage, uint32_t level)
{
    HAL_LVD_ResetLevel(voltage, level);
    return 0;
}

static int gm_lvd_check_reset_status(struct device *dev)
{
    RET_CODE ret = RET_OK;
    ret = HAL_LVD_CheckResetStatus();
    return ret;
}

static int gm_lvd_clear_reset_status(struct device *dev)
{
    HAL_LVD_ClearResetStatus();
    return 0;
}

static int gm_lvd_enable(struct device *dev, lvd_cfg *data)
{
    //struct gm_lvd_data *data = dev->driver_data;
    LVD_Config_T hal_config1, hal_config2, hal_config3;
    hal_config1.threshold = data->lvd1.threshold;
    hal_config1.detectionLevel = data->lvd1.detectionLevel;
    hal_config1.reset = data->lvd1.reset;
    hal_config1.enable = data->lvd1.enable;
    hal_config1.int_call = data->lvd1.int_call;

    hal_config2.threshold = data->lvd2.threshold;
    hal_config2.detectionLevel = data->lvd2.detectionLevel;
    hal_config2.reset = data->lvd2.reset;
    hal_config2.enable = data->lvd2.enable;
    hal_config2.int_call = (LVD_Int_Callback)data->lvd2.int_call;

    hal_config3.threshold = data->lvd3.threshold;
    hal_config3.detectionLevel = data->lvd3.detectionLevel;
    hal_config3.reset = data->lvd3.reset;
    hal_config3.enable = data->lvd3.enable;
    hal_config3.int_call = (LVD_Int_Callback)data->lvd3.int_call;

    HAL_LVD_Init(&hal_config1, &hal_config2, &hal_config3);

    const struct gm_lvd_config *config_irq = DEV_CFG(dev);
    config_irq->irq_config_func(dev);

    return 0;
}

static int gm_lvd_set_config(struct device *dev, lvd_cfg cfg)
{

    return 0;
}

static int gm_lvd_get_VDX_flag(struct device *dev, int source)
{
    RET_CODE ret = RET_OK;
    ret = HAL_LVD_GetVDXFlag(source);
    return ret;
}

static int gm_lvd_init(struct device *dev)
{
    return 0;
}

static const struct lvd_driver_api gm_lvd_driver_api =
{
    .enable_interrupt = gm_lvd_enable_interrupt,
    .disable_interrupt = gm_lvd_disable_interrupt,
    .clear_interrupt_flag = gm_lvd_clear_interrupt_flag,
    .lock = gm_lvd_lock,
    .reset_level = gm_lvd_reset_level,
    .check_reset_status = gm_lvd_check_reset_status,
    .clear_reset_status = gm_lvd_clear_reset_status,
    .set_config = gm_lvd_set_config,
    .enable = gm_lvd_enable,
    .get_VDX_flag = gm_lvd_get_VDX_flag,
};


static void gm_lvd_isr(void *arg)
{
    ARG_UNUSED(arg);

    HAL_LVD_IRQHandler();
}


static struct gm_lvd_data lvd_gm_data_0;

static void lvd_gm_irq_config(struct device *dev);

static struct gm_lvd_config lvd_gm_config =
{
    .irq_config_func = lvd_gm_irq_config,
};

DEVICE_DEFINE(lvd, CONFIG_LVD_NAME,
              &gm_lvd_init, NULL, &lvd_gm_data_0, &lvd_gm_config,
              POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
              &gm_lvd_driver_api);

static void lvd_gm_irq_config(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(LVD_IRQn, 0, gm_lvd_isr, DEVICE_GET(lvd), 0);
    irq_enable(LVD_IRQn);
}
