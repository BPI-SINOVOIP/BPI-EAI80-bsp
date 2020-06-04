/*
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 */

#include <logging/sys_log.h>
#include <errno.h>
#include <device.h>
#include <init.h>
#include <kernel.h>
#include <rtc.h>
#include <power.h>
#include <soc.h>
#include <misc/util.h>
#include <gm_hal_rtc.h>

struct gm_rtc_config
{
    void (*irq_config_func)(struct device *dev);
};

struct gm_rtc_data
{
    struct rtc_cfg config;
};

#define DEV_CFG(dev)                                                                \
    ((const struct gm_rtc_config * const)(dev)->config->config_info)

static int gm_rtc_set_datetime(struct device *dev, rtc_datetime *rdt)
{
    HAL_RTC_SetDateTime(rdt->year, rdt->mon, rdt->day, rdt->hour, rdt->min, rdt->sec);
    return 0;
}

static rtc_datetime *gm_rtc_get_time(struct device *dev)
{
    return (rtc_datetime *)HAL_RTC_GetTime();
}

static void gm_rtc_stop_timer(struct device *dev)
{
    HAL_RTC_StopTimer();
}

static void gm_rtc_cancel_alarm(struct device *dev)
{
    HAL_RTC_CancelAlarm();
}

static int gm_rtc_start_timer(struct device *dev, const rtc_timer *rt)
{
    HAL_RTC_StartTimer((Timer_T *)rt);
    return 0;
}

static int gm_rtc_next_sec(struct device *dev, rtc_datetime *rdt, int sec)
{
    return HAL_RTC_NextSec((UTCTime_T *)rdt, sec);
}

static void gm_rtc_enable(struct device *dev)
{
    HAL_RTC_Start();
}

static void gm_rtc_disable(struct device *dev)
{
    HAL_RTC_Stop();
}

static int gm_rtc_set_alarm(struct device *dev, const rtc_datetime *rdt)
{
    HAL_RTC_SetAlarm((UTCTime_T *)rdt);
    return 0;
}

static int gm_rtc_set_config(struct device *dev, struct rtc_cfg cfg)
{
    HAL_RTC_SetCallback((Timer_Callback)cfg.callback);

    return 0;
}

static int gm_rtc_read(struct device *dev)
{
    return HAL_RTC_GetTickSecond();
}

static u32_t gm_rtc_get_pending_int(struct device *dev)
{
    return 0;
}

static int gm_rtc_init(struct device *dev)
{
    HAL_RTC_Config_T hal_config;
    hal_config.alarmCallback = NULL;
    hal_config.timerCallback = NULL;
    hal_config.clock         = (HAL_RTC_Clock_T)RTC_CLK_SRC;

    HAL_RTC_Init(&hal_config);

    const struct gm_rtc_config *config_irq = DEV_CFG(dev);

    config_irq->irq_config_func(dev);

    return 0;
}

static void gm_rtc_reinit(struct device *dev)
{
    gm_rtc_init(dev);
}

static const struct rtc_driver_api gm_rtc_driver_api =
{
    .enable = gm_rtc_enable,
    .disable = gm_rtc_disable,
    .reinit = gm_rtc_reinit,
    .read = gm_rtc_read,
    .set_config = gm_rtc_set_config,
    .set_alarm = gm_rtc_set_alarm,
    .get_pending_int = gm_rtc_get_pending_int,
    .set_datetime = gm_rtc_set_datetime,
    .get_time = gm_rtc_get_time,
    .stop_timer = gm_rtc_stop_timer,
    .cancel_alarm = gm_rtc_cancel_alarm,
    .start_timer = gm_rtc_start_timer,
    .next_sec = gm_rtc_next_sec,
};

static void gm_rtc_isr(void *arg)
{
    ARG_UNUSED(arg);

    HAL_RTC_IRQHandler();
}


static struct gm_rtc_data rtc_gm_data;

static void rtc_gm_irq_config(struct device *dev);

static struct gm_rtc_config rtc_gm_config =
{
    .irq_config_func = rtc_gm_irq_config,
};


#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
static int gm_rtc_pm_control(struct device *device, uint32_t command, void *context)
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

/*todo this only remove compiler waring that pm_control not call*/
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(rtc, CONFIG_RTC_NAME,
              &gm_rtc_init, gm_rtc_pm_control, &rtc_gm_data, &rtc_gm_config,
              POST_KERNEL, DEVICE_PRIORITY_RTC,
              &gm_rtc_driver_api);

#else
DEVICE_DEFINE(rtc, CONFIG_RTC_NAME,
              NULL, gm_rtc_pm_control, &rtc_gm_data, &rtc_gm_config,
              POST_KERNEL, DEVICE_PRIORITY_RTC,
              &gm_rtc_driver_api);

#endif









static void rtc_gm_irq_config(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(RTC_IRQn, 0, gm_rtc_isr, DEVICE_GET(rtc), 0);
    irq_enable(RTC_IRQn);
}
