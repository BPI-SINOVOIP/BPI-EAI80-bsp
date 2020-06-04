/***********************************************************************
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @filename systick.h
 *
 * @author wen
 *
 * @version 1.0.0
 *
 * @date 2015/11/19
 *
 * @description Systick utility routines.
 *
 * @history
 *  Date                   By            Comments                 Ver.
 *  =========       =========        =========           =========
 *  2015/11/19      Wen Liu          Create                  1.0.0
 **********************************************************************/
#include <gm_common.h>
#include <systick.h>
#ifdef OS_ENABLE
    #include <uCOS_II.H>
#endif

#define SYSTICK_RELOAD_VALUE SYSTICK_MS_TICKS

/**
 * becasuse sdk driver depent on s_sysTime
 * so it must keep SYSTICK_RELOAD_VALUE as min irq uint,
 * when OS_ENABLE is  define , OS_TICKS_PER_SEC may be
 * config to less than 1000(sdk 1ms uint), so when systick irq cnt
 * get to the integial multile of OS_SYSTICK_IRQ_REAL_CNT will
 * real to process os systick irq, but it will wast cpu load to enter
 * invaild irq for os systick.
*/
#ifdef  OS_ENABLE
    #define REAL_OS_SYSTICK_PROCESS_CNT   (SYS_CLOCK/OS_TICKS_PER_SEC)/(SYSTICK_RELOAD_VALUE)
#endif




static volatile uint32_t s_sysTime = 0;

/*****************************************************************************
*
* @brief Init standard cortex-m0 systick with max reload value with systick
*        interrupt disabled.
*
* @param     none
* @retval    none
*
******************************************************************************/

void SysTick_Init(void)
{
    /*Disable systick*/
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    s_sysTime = 0;
    SysTick->VAL  = 0x0;       /* Clear current timer value */
    SysTick->LOAD = SYSTICK_RELOAD_VALUE;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

/*****************************************************************************
*
* @brief Stop standard cortex systick.
*
* @param     none
* @retval    none
*
******************************************************************************/
void SysTick_Disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*****************************************************************************
*
* @brief start standard cortex systick.
*
* @param     none
* @retval    none
*
******************************************************************************/
void SysTick_Enable(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}







/*****************************************************************************
*
* @brief Get standard cortex-m0 systick.
*
* @param     none
* @retval    Current value of sys timer.
*
******************************************************************************/
uint32_t SysTick_Get(void)
{
    return SysTick->VAL;
}

uint32_t SysTick_Interval(uint32_t old_tick, uint32_t cur_tick)
{
    uint32_t distance = (cur_tick <= old_tick) ? (old_tick - cur_tick) : (((SYSTICK_RELOAD_VALUE + 1) - cur_tick) + old_tick);
    return distance;
}

/*****************************************************************************
*
* @brief Delay some ticks after an interval to the expected tick arrives, no matter
*        what is current tick. The expected tick after the interval is returned after
*        delay time is over, and returns immediately if current tick is overdue. The
*        function is used to assure us-accuracy and must be run with all interrupts
*        disabled.
*
* @param[in]  exp_tick: The tick value expected, the new tick value after the interval
*                       is returned before the funtion is returned.
* @param[in]  interval: The tick value after expected tick which should expires before
*                       the funtion is returned.
*
* @retval     none
*
******************************************************************************/
uint32_t SysTick_DelayAfterTick(uint32_t ExpTick, uint32_t Interval)
{
    uint32_t exp_tick = ExpTick;
    uint32_t cur_tick, distance;

    exp_tick = (exp_tick >= Interval) ? (exp_tick - Interval) : (((SYSTICK_RELOAD_VALUE + 1) + exp_tick) - Interval);

    do
    {
        cur_tick = (uint32_t)(SysTick->VAL);
        distance = (exp_tick <= cur_tick) ? (cur_tick - exp_tick) : (((SYSTICK_RELOAD_VALUE + 1) - exp_tick) + cur_tick);
        if (distance > (SYSTICK_RELOAD_VALUE / 2) || distance == 0) // Long distance means that current time is overdue.
        {
            break;
        }
    } while (1);

    return exp_tick;

}

/*****************************************************************************
*
* @brief Delay ticks with minimum delay interval.
*
* @param[in]  Delay ticks of sys clock
* @retval     none
*
******************************************************************************/
void SysTick_DelayTicks(uint32_t ticks)
{
    uint32_t distance, cur_tick;
    uint32_t tick = SysTick->VAL;

    do
    {
        cur_tick = SysTick->VAL;
        distance = (cur_tick <= tick) ? (tick - cur_tick) : (((SYSTICK_RELOAD_VALUE + 1) - cur_tick) + tick);
        if (distance >= ticks)
        {
            break;
        }
    } while (1);//((tick - SysTick->VAL) & 0x00FFFFFF) < ticks);
}

/*****************************************************************************
*
* @brief Delay some milliseconds.
*
* @param[in]  Delay interval by ms
* @retval     none
*
******************************************************************************/
void SysTick_DelayMS(uint32_t ms)
{
    int i;

    for (i = 0; i < ms; i++)
    {
        SysTick_DelayUS(1000);
    }
}

/*****************************************************************************
*
* @brief Delay some microseconds.
*
* @param[in]  Delay interval by us
* @retval     none
*
******************************************************************************/
void SysTick_DelayUS(uint16_t us)
{
    uint32_t distance, cur_tick;
    uint32_t tick = SysTick->VAL;
    uint32_t ms = s_sysTime;
    /* Wait ticks = (ticks not waited at the beginning) + (left ticks)  */
    uint32_t ticks = tick + SYSTICK_US_TICKS * us;

    if (us <= 500)
    {
        do
        {
            cur_tick = SysTick->VAL;
            distance = (cur_tick <= tick) ? (tick - cur_tick) : (((SYSTICK_RELOAD_VALUE + 1) - cur_tick) + tick);
            if (distance >= SYSTICK_US_TICKS * us)
            {
                break;
            }
        } while (1);
        return;
    }

    /* Wait for some miniseconds */
    while (ticks >= SYSTICK_MS_TICKS)
    {
        while (s_sysTime == ms) {};
        ms = s_sysTime;
        ticks -= SYSTICK_MS_TICKS;
    }
    do
    {
        if (SysTick->VAL > ticks)
        {
            break;
        }
        /* If ticks is almost equal to SYSTICK_MS_TICKS, it is not sure by only polling "SysTick->VAL" */
        if (s_sysTime != ms)
        {
            break;
        }
    } while (1);/* ((tick - SysTick->VAL) & 0x00FFFFFF) < ticks); */
}

void WaitTimerInit(WaitTimer_T *waitTimer, uint32_t timeout, uint32_t wakeupPeriod)
{
    waitTimer->startTime = s_sysTime;
    waitTimer->wakeupPeriod = wakeupPeriod;
    waitTimer->timeout = timeout;
}

RET_CODE WaitTillTimeout(WaitTimer_T *waitTimer)
{
    uint32_t distance;
    uint32_t cur_tick = SysTick->VAL;

    if (waitTimer->timeout == 0)
    {
        return RET_OK;
    }

    distance = s_sysTime - waitTimer->startTime;

    if (distance > waitTimer->timeout)
    {
        return RET_TIMEOUT;
    }

    SysTick_DelayUS(waitTimer->wakeupPeriod);

    return RET_ONGOING;
}


void Systick_InterruptHandler(void)
{

#ifdef OS_ENABLE
    static unsigned int i = 0;
    OS_CPU_SR  cpu_sr;
    s_sysTime++;                                     /* for sdk driver used for delay*/
    if (++i == REAL_OS_SYSTICK_PROCESS_CNT)
    {
        i = 0;
        OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR */
        OSIntNesting++;
        OS_EXIT_CRITICAL();
        OSTimeTick();                                /* Call uC/OS-II's OSTimeTick() */
        OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR */
    }

#else
    s_sysTime++;
#endif



}
