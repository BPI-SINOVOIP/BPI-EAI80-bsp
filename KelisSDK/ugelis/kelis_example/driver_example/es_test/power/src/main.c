/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_power.h>
#include <gm_hal_rtc.h>
#include <gm_hal_reset.h>

#include <gm_hal_gpio.h>
#include <pinmux.h>
#include <device.h>
#include <gpio.h>


#define DBG_PRINTF           1
#define ERR_PRINTF           1


#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef DBG_PRINTF
    #define PR_ERR(...)       do {} while (0)
#else
    #define PR_ERR            printk
#endif


#define RTC_AS_WAKE_SOURCE 1
//#define WKPIN_AS_WAKE_SOURCE 1


static void SysTick_Disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

static void SysTick_Enable(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}


#ifdef RTC_AS_WAKE_SOURCE
static RET_CODE pm_config_rtc_wakeup_as_source()
{
    RET_CODE ret = RET_OK;

    UTCTime_T utcTime;
    HAL_RTC_Config_T rtcConfig;

#define YEAR    2017
#define MON     5
#define DAY     11
#define HOUR    14
#define MINUTE  7
#define SEC     0

    rtcConfig.alarmCallback = NULL;
    rtcConfig.clock         = HAL_RTC_CLOCK_LIRC;

    HAL_RTC_Init(&rtcConfig);

    /* RTC works now */
    HAL_RTC_Start();

    HAL_RTC_SetDateTime(YEAR, MON, DAY, HOUR, MINUTE, SEC);

    memset(&utcTime, 0, sizeof(UTCTime_T));
    utcTime.year = YEAR;
    utcTime.mon  = MON;
    utcTime.day  = DAY;
    utcTime.hour = HOUR;
    utcTime.min  = MINUTE;
    utcTime.sec  = SEC + 3; /* ensure sec < 60 */

    HAL_RTC_SetAlarm(&utcTime);


    return ret;

}
#endif

#ifdef WKPIN_AS_WAKE_SOURCE
static RET_CODE pm_config_wkpin_wakeup_as_source()
{
    RET_CODE ret = RET_OK;

    POWER_WakeUpPinConfig_T pinconfig;

    HAL_RTC_Config_T rtcConfig;
    rtcConfig.alarmCallback = NULL;
    rtcConfig.clock         = HAL_RTC_CLOCK_LIRC;

    HAL_RTC_Init(&rtcConfig);

    pinconfig.pin = POWER_WK_XPA8;
    pinconfig.powerMode = POWER_WKPIN_SHUTDOWN_MODE;
    pinconfig.vaildType = POWER_FALLING_EDGE;

    ret =  HAL_POWER_ConfigWakeUpPin(&pinconfig);

    return ret;

}

#endif


int main()
{
    RET_CODE ret = RET_OK;

    if (HAL_Reset_CheckRestCause(RESET_STATUS_STANDBY_MODE) == false)
    {
        /*noramal boot*/
        HAL_POWER_Init();
#ifdef RTC_AS_WAKE_SOURCE
        ret = pm_config_rtc_wakeup_as_source();
#endif
#ifdef WKPIN_AS_WAKE_SOURCE
        ret = pm_config_wkpin_wakeup_as_source();
#endif
        if (ret)
        {
            PR_ERR("ERR ret:%d wkup config\n", ret);
            return ret;
        }

        PR_DEBUG("Enter RESET_STATUS_STANDBY_MODE\n");

        k_sleep(1000);
        SysTick_Disable();

        ret = HAL_POWER_EnterLowPowerMode(POWER_STADDBY_MODE);
        if (ret)
        {
            PR_ERR("!!!ERR:Enter HAL_POWER_EnterLowPowerMode\n");
            return ret;
        }

        SysTick_Enable();
    }
    else
    {

        PR_DEBUG("standby resume Status:0x%08x\n", SYS_DEV->SRS);
        HAL_Reset_ClearRestCause(RESET_STATUS_STOP2_MODE);
        PR_DEBUG("clear Status:0x%08x\n", SYS_DEV->SRS);

    }

    do
    {
        k_sleep(1);
    } while (1);
}




