/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file            entropy_gm.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 *
 * @date           2019/03/18
 *
 * @brief           Implementation of trng Driver.
 *
 * @note
 *    2019/03/18, terry.wu, V1.0.0
 *        Initial version.
 *
 */


#include <kernel.h>
#include <entropy.h>
#include <gm_hal_trng.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#include <misc/__assert.h>
#include <misc/util.h>
#include <pm.h>

static TRNG_Handle_T hTrng;

struct entropy_gm_rng_dev_data
{
    TRNG_Handle_T *handle;
};


#define RANDOM_SIZE      6      // 192bit, 6 DWORD
#define DEV_DATA(dev) \
    ((struct entropy_gm_rng_dev_data *)((dev)->driver_data))

#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
static uint32_t TRNG_REG[5] = {0};
#endif


static int entropy_gm_get_rng(struct device *dev, u8_t *buffer, u16_t len)
{
    u16_t i = 0;
    u8_t ret = 0;
    u16_t cnt = len / (RANDOM_SIZE * 4);
    u32_t *tmp = (u32_t *)buffer;
    u32_t random[RANDOM_SIZE];
    struct entropy_gm_rng_dev_data *dev_data;

    __ASSERT_NO_MSG(dev);
    __ASSERT_NO_MSG(buffer);

    dev_data = DEV_DATA(dev);
    for (i = 0; i < cnt; i++)
    {
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
        while (pm_device_suspend_state_check(dev))
        {
            k_sleep(1);
        }
#endif
        ret = HAL_TRNG_GenerateRandomNumber(dev_data->handle, tmp, RANDOM_SIZE);
        if (ret != 0)
        {
            return -1;
        }

        tmp += RANDOM_SIZE;
    }

    if (len % (RANDOM_SIZE * 4))
    {
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
        while (pm_device_suspend_state_check(dev))
        {
            k_sleep(1);
        }
#endif
        ret = HAL_TRNG_GenerateRandomNumber(dev_data->handle, random, RANDOM_SIZE);
        if (ret != 0)
        {
            return -1;
        }
        memcpy(tmp, (u8_t *)random, len % (RANDOM_SIZE * 4));
    }

    return 0;
}


static int entropy_gm_rng_init(struct device *dev)
{
    struct entropy_gm_rng_dev_data *dev_data;

    __ASSERT_NO_MSG(dev);

    dev_data = DEV_DATA(dev);
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
    pm_device_suspend_state_clear(dev);
#endif
    if (HAL_TRNG_Init(dev_data->handle) != 0)
    {
        return -1;
    }

    return 0;
}


static const struct entropy_driver_api entropy_gm_rng_api =
{
    .get_entropy = entropy_gm_get_rng
};


static struct entropy_gm_rng_dev_data entropy_gm_rng_data =
{
    .handle = &hTrng
};

#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
static int entropy_gm_rng_pm_control(struct device *device, uint32_t command, void *context)
{
    int ret = 0;
    uint32_t set_stat;
    static uint32_t pm_dev_status = DEVICE_PM_ACTIVE_STATE;
    struct entropy_gm_rng_dev_data *dev_data;

    dev_data = DEV_DATA(device);
    if (command == DEVICE_PM_SET_POWER_STATE)
    {
        set_stat = *((int *)context);

        if (set_stat == DEVICE_PM_ACTIVE_STATE)
        {

            HAL_Reset_Module(RESET_TRNG_SW_RSTJ);
            HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_TRNG);
            dev_data->handle->Instance->RST = TRNG_REG[0];
            dev_data->handle->Instance->SAMCNT = TRNG_REG[1];
            dev_data->handle->Instance->CFG = TRNG_REG[2];
            dev_data->handle->Instance->DBG = TRNG_REG[3];
            dev_data->handle->Instance->SRCEN = TRNG_REG[4];
            pm_device_suspend_state_clear(device);
            HAL_UnLock(dev_data->handle->lock);
        }
        else if (set_stat == DEVICE_PM_SUSPEND_STATE)
        {
            pm_device_suspend_state_set(device);
            while (HAL_TryLock(dev_data->handle->lock) == RET_BUSY)
            {
                k_sleep(1);
            }
            TRNG_REG[0] = dev_data->handle->Instance->RST;
            TRNG_REG[1] = dev_data->handle->Instance->SAMCNT;
            TRNG_REG[2] = dev_data->handle->Instance->CFG;
            TRNG_REG[3] = dev_data->handle->Instance->DBG;
            TRNG_REG[4] = dev_data->handle->Instance->SRCEN;
        }
        else if (set_stat == DEVICE_PM_OFF_STATE)
        {
            while (HAL_TryLock(dev_data->handle->lock) == RET_BUSY)
            {
                k_sleep(1);
            }
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




#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(entropy_gm_rng, CONFIG_ENTROPY_NAME,
              entropy_gm_rng_init,  entropy_gm_rng_pm_control,
              &entropy_gm_rng_data, NULL,
              PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
              &entropy_gm_rng_api);
#else
DEVICE_DEFINE(entropy_gm_rng, CONFIG_ENTROPY_NAME,
              entropy_gm_rng_init,  NULL,
              &entropy_gm_rng_data, NULL,
              PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
              &entropy_gm_rng_api);
#endif

