/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_pes.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2017/05/19
 *
 * @brief         Header file of pes driver.
 *
 * @note
 *    2017/05/19, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#ifndef _GM_HAL_POWER_H_
#define _GM_HAL_POWER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>

typedef enum
{
    POWER_SLEEP_MODE           = 0,
    POWER_STOP1_MODE,
    POWER_STOP2_MODE,
    POWER_STADDBY_MODE,
    POWER_SHUTDOWN_MODE,
} POWER_Mode_T;

typedef enum
{
    POWER_RISING_EDGE      = 0,
    POWER_FALLING_EDGE,

} POWER_PinVaildTpye_T;

#if defined(CHIP_GM6721)
typedef enum
{
    POWER_WK_XPA0 = 0,  /*stop2/standby pin*/
    POWER_WK_XPA7,      /*shutdown wkup pin*/
    POWER_WK_XPA8,      /*shutdown wkup pin*/

} POWER_Pin_T;
#elif defined(CHIP_F6721B)
typedef enum
{
    POWER_WK_XPA0 = 0,  /*stop2/standby pin*/
    POWER_WK_XPB12,      /*shutdown wkup pin*/
    POWER_WK_XPB13,         /*shutdown wkup pin*/

} POWER_Pin_T;
#endif


typedef enum
{
    POWER_WKPIN_STOP2_MODE            = 0,    /*stop2*/
    POWER_WKPIN_STANDBY_MODE,                 /*standby*/
    POWER_WKPIN_SHUTDOWN_MODE,                /*shutdown*/
} POWER_WkPinLowPowerMode_T;


typedef struct
{
    POWER_Pin_T pin;
    POWER_WkPinLowPowerMode_T powerMode;
    POWER_PinVaildTpye_T vaildType;

} POWER_WakeUpPinConfig_T;


void HAL_POWER_Init(void);
void HAL_PMU_IRQHandler(void *param);
RET_CODE HAL_POWER_EnterLowPowerMode(POWER_Mode_T mode);
RET_CODE HAL_POWER_ConfigWakeUpPin(POWER_WakeUpPinConfig_T *powerPinConfig);


#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_POWER_H_*/



















