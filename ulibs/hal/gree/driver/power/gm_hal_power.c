#include <gm_soc.h>
#include <gm_hal_reset.h>
#include <gm_hal_gpio.h>
#include <gm_hal_rtc.h>
#include <gm_hal_power.h>
#include "gm_ll_power.h"
#include <gm_irq.h>

typedef struct
{
    GPIO_Device_T *device;
    uint32_t pin;
    uint32_t mode;
    uint32_t pull;
    uint32_t alternate;

} POWER_PinConfig_T;

#if defined(CHIP_GM6721)
POWER_PinConfig_T s_pinCfgTable[] =
{
    {GPIOA, GPIO_PIN_0,  GPIO_MODE_INPUT, GPIO_PULLUP, 0},
    {GPIOA, GPIO_PIN_7,  GPIO_MODE_INPUT, GPIO_PULLUP, 0},
    {GPIOA, GPIO_PIN_8,  GPIO_MODE_INPUT, GPIO_PULLUP, 0},
};
#elif defined(CHIP_F6721B)
POWER_PinConfig_T s_pinCfgTable[] =
{
    {GPIOA, GPIO_PIN_0,  GPIO_MODE_INPUT, GPIO_PULLUP, 0},
    {GPIOB, GPIO_PIN_12,  GPIO_MODE_INPUT, GPIO_PULLUP, 0},
    {GPIOB, GPIO_PIN_13,  GPIO_MODE_INPUT, GPIO_PULLUP, 0},
};
#endif

static void HAL_POWER_IrqConfig(void);
static RET_CODE HAL_POWER_ConfigWakeUpPinAsGpioInputMode(POWER_Pin_T pin);


static void HAL_POWER_IrqConfig()
{
    HAL_RequestIrq(PMU_IRQn, PMU_IRQ_PRIORITY, HAL_PMU_IRQHandler, 0);
    HAL_IRQ_ENABLE(PMU_IRQn);
}

void HAL_PMU_IRQHandler(void *param)
{
    UNUSED(param);

#if defined(CHIP_GM6721)
    if (HAL_Reset_CheckRestCause(RESET_STATUS_IO_XPA0_PENDING) == TRUE)
    {
        HAL_Reset_ClearRestCause(RESET_STATUS_IO_XPA0_PENDING);
    }
    else if (HAL_Reset_CheckRestCause(RESET_STATUS_PES_WK_PENDING) == TRUE)
    {
        HAL_Reset_ClearRestCause(RESET_STATUS_PES_WK_PENDING);
    }
    else if (HAL_RTC_CheckWakePinPending(RTC_WAKEUP_PIN_XPA7) == TRUE)
    {
        HAL_RTC_ClearWakePendingStatus(RTC_WAKEUP_PIN_XPA7);
    }
    else if (HAL_RTC_ClearWakePendingStatus(RTC_WAKEUP_PIN_XPA8) == TRUE)
    {
        HAL_RTC_ClearWakePendingStatus(RTC_WAKEUP_PIN_XPA8);
    }
#elif defined(CHIP_F6721B)
    if (HAL_Reset_CheckRestCause(RESET_STATUS_IO_XPA0_PENDING) == TRUE)
    {
        HAL_Reset_ClearRestCause(RESET_STATUS_IO_XPA0_PENDING);
    }
    else if (HAL_Reset_CheckRestCause(RESET_STATUS_PES_WK_PENDING) == TRUE)
    {
        HAL_Reset_ClearRestCause(RESET_STATUS_PES_WK_PENDING);
    }
    else if (HAL_RTC_CheckWakePinPending(RTC_WAKEUP_PIN_XPB12) == TRUE)
    {
        HAL_RTC_ClearWakePendingStatus(RTC_WAKEUP_PIN_XPB12);
    }
    else if (HAL_RTC_ClearWakePendingStatus(RTC_WAKEUP_PIN_XPB13) == TRUE)
    {
        HAL_RTC_ClearWakePendingStatus(RTC_WAKEUP_PIN_XPB13);
    }
#endif
}

void HAL_POWER_Init()
{
    HAL_POWER_IrqConfig();
}


RET_CODE HAL_POWER_EnterLowPowerMode(POWER_Mode_T mode)
{
    RET_CODE ret = RET_OK;
    Sys_Device_T *pDev = SYS_DEV;

    switch (mode)
    {
        case POWER_SLEEP_MODE:
            LL_CPU_ENTER_DEEP_SLEEP(FALSE);
            LL_POWER_CONFIG_POWER_PMUENABLE(pDev, FALSE);
            LL_CPU_WFI();
            break;

        case POWER_STOP1_MODE:
            LL_POWER_PMU_ENTER_STOP1_MODE(pDev);
            LL_POWER_CONFIG_POWER_PMUENABLE(pDev, FALSE);
            LL_CPU_ENTER_DEEP_SLEEP(TRUE);
            LL_CPU_WFI();
            break;

        case POWER_STOP2_MODE:
            LL_POWER_PMU_ENTER_STOP2_MODE(pDev);
            LL_POWER_CONFIG_POWER_PMUENABLE(pDev, TRUE);
            LL_CPU_ENTER_DEEP_SLEEP(TRUE);
            LL_CPU_WFI();
            break;

        case POWER_STADDBY_MODE:
            LL_POWER_PMU_ENTER_STANDBY_MODE(pDev);
            LL_POWER_CONFIG_POWER_PMUENABLE(pDev, TRUE);
            LL_CPU_ENTER_DEEP_SLEEP(TRUE);
            LL_CPU_WFI();
            break;

        case POWER_SHUTDOWN_MODE:
            LL_POWER_PMU_ENTER_SHUTDOWN_MODE(pDev);
            LL_POWER_CONFIG_POWER_PMUENABLE(pDev, TRUE);
            LL_CPU_ENTER_DEEP_SLEEP(TRUE);
            LL_CPU_WFI();
            break;

        default:
            ret = RET_INVPARAM ;
    }

    return ret;
}

RET_CODE HAL_POWER_ConfigWakeUpPinVaildEdge(POWER_WakeUpPinConfig_T *powerPinConfig)
{
    RET_CODE ret = RET_OK;
    Sys_Device_T *pDev = SYS_DEV;
    POWER_Pin_T powerPin = powerPinConfig->pin;
    POWER_PinVaildTpye_T vaildType = powerPinConfig->vaildType;
    RTC_WakeUpPinConfig_T rtcPinCfg;

    if (powerPin == POWER_WK_XPA0)
    {
        LL_POWER_CONFIG_XPA0_WKEDG(pDev, vaildType);
    }
    else
    {
        /*config for rtc power pin edge*/
        if (vaildType == POWER_FALLING_EDGE)
        {
            rtcPinCfg.vaildType = RTC_WKPIN_FALLING_EDGE;
        }
        else if (vaildType == POWER_RISING_EDGE)
        {
            rtcPinCfg.vaildType = RTC_WKPIN_RISING_EDGE;
        }
        else
        {
            return RET_INVPARAM;
        }
#if defined(CHIP_GM6721)
        /*config for rtc power pin */
        if (powerPin == POWER_WK_XPA7)
        {
            rtcPinCfg.pin = RTC_WAKEUP_PIN_XPA7;
        }
        else if (powerPin == POWER_WK_XPA8)
        {
            rtcPinCfg.pin = RTC_WAKEUP_PIN_XPA8;
        }
        else
        {
            return RET_INVPARAM;
        }
#elif defined(CHIP_F6721B)
        /*config for rtc power pin */
        if (powerPin == POWER_WK_XPB12)
        {
            rtcPinCfg.pin = RTC_WAKEUP_PIN_XPB12;
        }
        else if (powerPin == POWER_WK_XPB13)
        {
            rtcPinCfg.pin = RTC_WAKEUP_PIN_XPB13;
        }
        else
        {
            return RET_INVPARAM;
        }
#endif
        ret = HAL_RTC_ConfigWakeUpPinVaildEdge(&rtcPinCfg);

    }

    return ret;
}


static RET_CODE HAL_POWER_ConfigWakeUpPinAsGpioInputMode(POWER_Pin_T pin)
{
    RET_CODE ret = RET_OK;

    GPIO_PinConfig_T pinInpuCfg;

    memset(&pinInpuCfg, 0, sizeof(GPIO_PinConfig_T));

    pinInpuCfg.pin          = s_pinCfgTable[pin].pin ;
    pinInpuCfg.mode         = s_pinCfgTable[pin].mode;
    pinInpuCfg.pull         = s_pinCfgTable[pin].pull;
    pinInpuCfg.alternate    = s_pinCfgTable[pin].alternate;

    ret = HAL_GPIO_Init(s_pinCfgTable[pin].device, &pinInpuCfg);


    return ret;

}


RET_CODE HAL_POWER_ConfigWakeUpPinEn(POWER_Pin_T pin, uint8_t en)
{
    RET_CODE ret = RET_OK;
    Sys_Device_T *pDev = SYS_DEV;
    RTC_PowerPin_T rtcPowerPin;


    if (pin == POWER_WK_XPA0)
    {
        LL_POWER_CONFIG_XPA0_WKEN(pDev, en);
    }
    else
    {
#if defined(CHIP_GM6721)
        if (pin == POWER_WK_XPA7)
        {
            rtcPowerPin = RTC_WAKEUP_PIN_XPA7;
        }
        else if (pin == POWER_WK_XPA8)
        {
            rtcPowerPin = RTC_WAKEUP_PIN_XPA8;
        }
        else
        {
            return  RET_INVPARAM ;
        }
#elif defined(CHIP_F6721B)
        if (pin == POWER_WK_XPB12)
        {
            rtcPowerPin = RTC_WAKEUP_PIN_XPB12;
        }
        else if (pin == POWER_WK_XPB13)
        {
            rtcPowerPin = RTC_WAKEUP_PIN_XPB13;
        }
        else
        {
            return  RET_INVPARAM ;
        }
#endif
        ret =  HAL_RTC_ConfigWakeUpEn(rtcPowerPin, en);
        if (ret)
        {
            goto out;
        }
    }

    ret = HAL_POWER_ConfigWakeUpPinAsGpioInputMode(pin);
    if (ret)
    {
        goto out;
    }

out:
    return ret;
}


RET_CODE HAL_POWER_ConfigWakeUpPin(POWER_WakeUpPinConfig_T *powerPinConfig)
{
    Sys_Device_T *pDev = SYS_DEV;
    RET_CODE ret = RET_OK;

    if (!powerPinConfig)
    {
        return RET_INVPARAM;
    }

    if (!((powerPinConfig->powerMode == POWER_WKPIN_STOP2_MODE)     || \
            (powerPinConfig->powerMode == POWER_WKPIN_STANDBY_MODE)     || \
            (powerPinConfig->powerMode == POWER_WKPIN_SHUTDOWN_MODE)))
    {
        return RET_INVPARAM;
    }

    if (((powerPinConfig->powerMode == POWER_WKPIN_STANDBY_MODE) || (powerPinConfig->powerMode == POWER_WKPIN_STOP2_MODE))
            && (powerPinConfig->pin != POWER_WK_XPA0))
    {
        return RET_INVPARAM;
    }

#if defined(CHIP_GM6721)
    if ((powerPinConfig->powerMode == POWER_WKPIN_SHUTDOWN_MODE) &&
            ((powerPinConfig->pin != POWER_WK_XPA7) && (powerPinConfig->pin != POWER_WK_XPA8)))
    {
        return RET_INVPARAM;
    }
#elif defined(CHIP_F6721B)
    if ((powerPinConfig->powerMode == POWER_WKPIN_SHUTDOWN_MODE) &&
            ((powerPinConfig->pin != POWER_WK_XPB12) && (powerPinConfig->pin != POWER_WK_XPB13)))
    {
        return RET_INVPARAM;
    }
#endif
    LL_POWER_BACKUP_REG_UNLOCK(pDev);

    /*config gpio as input*/
    ret = HAL_POWER_ConfigWakeUpPinAsGpioInputMode(powerPinConfig->pin);
    if (ret)
    {
        goto out;
    }

    ret = HAL_POWER_ConfigWakeUpPinEn(powerPinConfig->pin, TRUE);
    if (ret)
    {
        goto out;
    }

    ret = HAL_POWER_ConfigWakeUpPinVaildEdge(powerPinConfig);
    if (ret)
    {
        goto out;
    }

    LL_POWER_BACKUP_REG_LOCK(pDev);

out:
    return ret;
}


