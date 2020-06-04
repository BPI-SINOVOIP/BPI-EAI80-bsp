/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_utc.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/06/13
 *
 * @brief         Header file of UTC time,  which is used by RTC.
 *
 * @note
 *    2018/06/13, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef _GM_HAL_UTC_H_
#define _GM_HAL_UTC_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef int Time_T;

/**
 * Coordinated Universal Time structure
 */
typedef struct
{
    int sec;        /**< Seconds.       [0-59]                  */
    int min;        /**< Minutes.       [0-59]                  */
    int hour;       /**< Hours.         [0-23]                  */
    int day;        /**< Day.           [1-31]                  */
    int mon;        /**< Month.         [1-12]                  */
    int year;       /**< Year.          [1902-2038]             */
    int wday;       /**< Day of week.   [1-7] , 1-Monday ...    */
    int yday;       /**< Days in year.  [0-365]                 */
    int subsec;     /**< Subseconds.                            */
    int fmt12;      /**< 12 format.     0-24 hour, 1-am/pm      */
    int wflag;      /**< 0 - the DAY is the day of month, 1 - the is the week day */
} UTCTime_T;

/* GMXX wday is 1~7 while the standard utctime wday is 0~6 */
#define HAL_RTC_ConvertToWday0(x)   ((x) = ((x) == 7 ? 0 : (x)))
#define HAL_RTC_ConvertToWday1(x)   ((x) = ((x) == 0 ? 7 : (x)))



/**
* @brief Get the weekday on the day y/m/d
* @param y - year
* @param m - month
* @param d - day
* @return weekday from 1 to 7
* @note
*/
uint32_t HAL_UTC_CalcWeekday(uint32_t y, uint32_t m, uint32_t d);

/* Return the `UTCTime_T' representation of *TIMER in the local timezone.
   Use local time if USE_LOCALTIME is nonzero, UTC otherwise.  */

/**
* @brief Convert the seconds from Jan 1 00:00:00 1970 to UTC time.
* @param t - seconds from Jan 1 00:00:00 1970.
* @return the `UTCTime_T' representation of *t in UTC.
* @note
*/
UTCTime_T *HAL_UTC_Convert(const Time_T *timer, UTCTime_T *tp);

/**
* @brief Convert UTC time to the seconds from Jan 1 00:00:00 1970.
* @param tp - UTC time
* @return the seconds from Jan 1 00:00:00 1970.
* @note
*/
Time_T HAL_UTC_ToTimeT(UTCTime_T *tp);

#ifdef __cplusplus
}
#endif

#endif //_GM_HAL_UTC_H_

