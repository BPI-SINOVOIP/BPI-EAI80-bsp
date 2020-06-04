/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          systick.h
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/07/23
 *
 * @brief         Header file of uart driver.
 *
 * @note
 *    2015/11/19, wen.liu, V1.0.0
 *        Initial version.
 *    2016/07/23, wen.liu, V1.1.0
 *        Update coding style.
 *        Add function about wait timer.
 *        Add function about delay some ticks after expected time or current time.
 */
#ifndef __SYSTICK_H
#define __SYSTICK_H

#include <gm_common.h>

/**
 * @defgroup Delay_Macros
 * Specify macros of delay time function
 * @{
 */
#define DelayTick SysTick_DelayTicks
#define DelayUS   SysTick_DelayUS
#define DelayMS   SysTick_DelayMS
/**
 * @}
 */

/**
 * @defgroup SYSTICK_US_TICKS
 * Specify ticks of one microsecond or millisecond
 * @{
 */
#define SYSTICK_US_TICKS  ((SYS_CLOCK)/1000000)
#define SYSTICK_MS_TICKS  ((SYS_CLOCK)/1000)
/**
 * @}
 */

/**
 * Polling timer handle structure, it can be wakup periodly to save power and cpu performance
 */
typedef struct
{
    uint32_t startTime;       /* Start time by millisecond */
    uint32_t wakeupPeriod;    /* Wakeup interval */
    uint32_t timeout;         /* Expected timeout interval by millisecond */
} WaitTimer_T;

/**
 * @brief Initialize a timer to wait for an event within timeout interval. It can wakeup periodically.
 *
 * @param  pWaitTimer   Pointer to timer handle.
 * @param  timeout      Timeoout interval in millisecond.
 * @param  wakeupPeriod Wakeup period in microsecond.
 * @retval              None.
 */
void WaitTimerInit(WaitTimer_T *pWaitTimer, uint32_t timeout, uint32_t wakeupPeriod);

/**
 * @brief Wakeup periodically or wait till timeout.
 *
 * @param  pWaitTimer   Pointer to timer handle.
 * @retval              Return RET_ONGOING if wakeup periodically, return RET_TIMEOUT if timeout.
 */
RET_CODE WaitTillTimeout(WaitTimer_T *waitTimer);

/**
 * @brief Initialize systick timer.
 */
void SysTick_Init(void);

/**
 * @brief Disable and stop systick timer.
 */
void SysTick_Disable(void);


/**
 * @brief Enable systick timer.
 */
void SysTick_Enable(void);


/**
 * @brief Get current tick value of systick timer.
 *
 * @retval  Return current tick value.
 */
uint32_t SysTick_Get(void);

/**
 * @brief Delay ticks with minimum delay interval.
 *
 * @param ticks Delay ticks of sys clock.
 */
void SysTick_DelayTicks(uint32_t ticks);

/**
 * @brief Calculate interval of two timer value.
 *
 * @retval return absolute tick interval of two time value.
 */
uint32_t SysTick_Interval(uint32_t old_tick, uint32_t cur_tick);

/**
 * @brief Delay some ticks after an interval to the expected tick arrives, no matter
 *        what is current tick. The expected tick after the interval is returned after
 *        delay time is over, and returns immediately if current tick is overdue. The
 *        function is used to assure us-accuracy and must be run with all interrupts
 *        disabled.
 *
 * @param  exp_tick The tick value expected, the new tick value after the interval
 *                       is returned before the funtion is returned.
 * @param  interval The tick value after expected tick which should expires before
 *                       the funtion is returned.
 * @retval  none
 *
 */
uint32_t SysTick_DelayAfterTick(uint32_t ExpTick, uint32_t Interval);

/**
 *
 * @brief Delay some milliseconds.
 *
 * @param  Delay time in milliseconds.
 * @retval     none
 */
void SysTick_DelayMS(uint32_t ms);

/**
 *
 * @brief Delay some microseconds.
 *
 * @param[in]  Delay time in microseconds.
 * @retval     none
 */
void SysTick_DelayUS(uint16_t us);

#endif
