/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_rtc.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/06/13
 *
 * @brief         Header file of GM6721 RTC Driver
 *
 * @note
 *    2018/06/13, Angrad.Yang, V1.0.0
 *        Initial version.
 */


#ifndef _GM_HAL_RTC_H_
#define _GM_HAL_RTC_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_utc.h>

////typedef unsigned int uint32_t;

#define HAL_RTC_STATUS_OK      0
#define HAL_RTC_STATUS_FAILED -1

/**
 * TimeValue structure for Timer_T.
 */
typedef struct
{
    Time_T tv_sec;      /**< Seconds.      */
    Time_T tv_msec;     /**< MilliSeconds. */
} TimeValue_T;

/**
 * Timer_T structure. used by the timer or alarm.
 */
typedef struct
{
    TimeValue_T cycle;  /**< Time to call the timer/alarm handler periodly */
} Timer_T;

/**
 * @defgroup Callback_Macros
 * Specify macros of Callback
 * @{
 */
typedef void (*Timer_Callback)(int);

typedef enum
{
    HAL_RTC_ALARM = 0,
    HAL_RTC_TIMER = 1,
} Rtc_Callback_T;

/**
 * @}
 */

/**
 * RTC Clock Source
 */
typedef enum
{
    HAL_RTC_CLOCK_LIRC              = 0, /**< RTC clock is sourced by LIRC */
    HAL_RTC_CLOCK_LOSC,                  /**< RTC clock is sourced by LOSC */
    HAL_RTC_CLOCK_HOSC,                  /**< RTC clock is sourced by HOSC/HOSC_DIV_RTC */
} HAL_RTC_Clock_T;

/**
 * RTC configuration structure
 *
 * This structure holds the configuration settings for RTC initialization.
 *
 */
typedef struct
{
    Timer_Callback  alarmCallback;
    Timer_Callback  timerCallback;
    HAL_RTC_Clock_T     clock;
} HAL_RTC_Config_T;



/**
 * RTC about power pin
 *
 */
#if defined(CHIP_GM6721)
typedef enum
{
    RTC_WAKEUP_PIN_XPA7              = 0,
    RTC_WAKEUP_PIN_XPA8,
} RTC_PowerPin_T;
#elif defined(CHIP_F6721B)
typedef enum
{
    RTC_WAKEUP_PIN_XPB12              = 0,
    RTC_WAKEUP_PIN_XPB13,
} RTC_PowerPin_T;

#endif


typedef enum
{
    RTC_WKPIN_RISING_EDGE      = 0,
    RTC_WKPIN_FALLING_EDGE,
} RTC_PowerPinVaildTpye_T;


typedef struct
{
    RTC_PowerPin_T pin;
    RTC_PowerPinVaildTpye_T vaildType;

} RTC_WakeUpPinConfig_T;




/**
 * @brief Initialize the RTC default clock OSCLS and enable timer0.
 *        Callbacks ought to be set to NULL even if not used.
 * @param  pConfig  The initial configuration of RTC.
 * @retval          None.
 */
void HAL_RTC_Init(HAL_RTC_Config_T *pConfig);

/**
 * @brief Do Nothing. (Deprecated API)
 *
 */
void HAL_RTC_DeInit(void);

/**
 * @brief The RTC time elapses, with counter tick at the frequency that
 */
void HAL_RTC_Start(void);

/**
 * @brief Stop counter tick, that the time will be paused.
 */
void HAL_RTC_Stop(void);

/**
 * @brief Rest time to 1970/01/01 00:00:00.
 */
void HAL_RTC_Reset(void);

/**
 * @brief Get RTC seconds from 1970/01/01 00:00:00.
 *
 * @param  pTime    Tick seconds from Jan 1, 1970. 00:00:00.
 * @retval          Tick seconds from Jan 1, 1970. 00:00:00.
 */
Time_T HAL_RTC_GetTickSecond(void);

/**
 * @brief Store RTC seconds from 1970/01/01 00:00:00 to RTC module.
 *
 * @param  pTime    Tick seconds from Jan 1, 1970. 00:00:00.
 * @retval          None.
 */
void HAL_RTC_SetTickSecond(const Time_T *pTime);

/**
 * @brief Convert Time_T seconds to UTC time.
 *
 * @param   pTime   the seconds which are needed to be converted.
 * @retval  Return the corresponding utc time structure.
 */
UTCTime_T *HAL_RTC_ConvertToUtcTime(const Time_T *pTime);

/**
 * @brief Convert UTC time to Time_T seconds.
 *
 * @param   pUtcTime    The utc time which is needed to be converted.
 * @retval              Return the corresponding seconds.
 */
Time_T HAL_RTC_ConvertToTickSecond(UTCTime_T *pUtcTime);

/**
 * @brief Start the iterative timer.
 *
 * #GM6621 NOTICE Limitation#
 * Only 1s or max_msec xxxms are allowed,
 * the max_msec = MAX_WAKEUP_CNT * 1000 * 2 * (PRESCALE+1) / RTCCLK
 * eg. MAX_WAKEUP_CNT=65535, PRESCALE=121, RTCCLK=8000000, max_msec=1998ms
 *
 * @param   pTimer      Indication the cycle of the timer.
 * @retval              status
 */
RET_CODE HAL_RTC_StartTimer(const Timer_T *pTimer);

/**
 * @brief Stop the timer.
 */
void HAL_RTC_StopTimer(void);

/**
 * @brief Set the future time alarm.
 *  #NOTICE# Both of the alarm and the timer can not be effective at the same time.
 *
 * @param   pUtcTime    A future utc time of the alarm.
 * @retval              None.
 */
void HAL_RTC_SetAlarm(UTCTime_T *pUtcTime);

/**
 * @brief Cancel the alarm.
 */
void HAL_RTC_CancelAlarm(void);

/**
 * @brief Set UTC time directly.
 *
 * @param   hour    The hour 0~23.
 * @param   min     The minute 0~59.
 * @param   sec     The second 0~60.
 * @retval          None.
 */
void HAL_RTC_SetTime(uint32_t hour, uint32_t min, uint32_t sec);

/**
 * @brief Set UTC date directly.
 *
 * @param   year    The year 1902~2038.
 * @param   mon     The month 1~12.
 * @param   day     The day 1~31.
 * @retval          None.
 */
void HAL_RTC_SetDate(uint32_t year, uint32_t mon, uint32_t day);

/**
 * @brief Set UTC date time directly.
 *
 * @param   year    The year 1902~2038.
 * @param   mon     The month 1~12.
 * @param   day     The day 1~31.
 * @param   hour    The hour 0~23.
 * @param   min     The minute 0~59.
 * @param   sec     The second 0~60.
 * @retval          None.
 */
RET_CODE HAL_RTC_SetDateTime(uint32_t year, uint32_t mon, uint32_t day,
                             uint32_t hour, uint32_t min, uint32_t sec);

/**
 * @brief Get UTC time.
 *
 * @param               None.
 * @retval  UTCTime_T*  Current time with UTC format.
 */
UTCTime_T *HAL_RTC_GetTime(void);

/**
 * @brief Enable the trigger event from RTC to the ACMP module.
 *
 * @param               None.
 * @retval              None.
 */
void HAL_RTC_TriggerACMPEnable(void);


/**
 * RTC about power pin
 *
 */
RET_CODE HAL_RTC_ConfigWakeUpEn(RTC_PowerPin_T pin, uint8_t en);
RET_CODE HAL_RTC_ConfigWakeUpPinVaildEdge(RTC_WakeUpPinConfig_T *pinCfg);
RET_CODE HAL_RTC_ClearWakePendingStatus(RTC_PowerPin_T pin);
uint8_t HAL_RTC_CheckWakePinPending(RTC_PowerPin_T pin);


/**
 * @brief Default RTC Irq Handler
 *
 * @param               None.
 * @retval              None.
 */
void HAL_RTC_IRQHandler(void);

/**
 * @brief Set user callback in RTC ISR
 *
 * @param   handler - callback of alarm and timer.
 * @retval              None.
 */
void HAL_RTC_SetCallback(Timer_Callback handler);


/**
 * @brief Get the next time after x sec
 *
 * @param   UTCTime_T - basic utc time
 *          sec       - the interval time.
 * @retval  0 - OK, -1 - failed
 */
RET_CODE HAL_RTC_NextSec(UTCTime_T *pUtcTime, int sec);

#ifdef __cplusplus
}
#endif

#endif //_GM_HAL_RTC_H_
