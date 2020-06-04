/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_rtc.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/06/13
 *
 * @brief         RTC Driver For GM6721.
 *
 * @note
 *    2018/06/13, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include "gm_ll_rtc.h"
#include <gm_hal_rtc.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>

/********************************************************************************
* Constants and macros
*******************************************************************************/

#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))

#define DEBUG_MODE 0

#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef bool
    typedef unsigned char bool;
#endif

#define True    1
#define False   0

typedef void (*TimerOper)();
typedef void (*TimerOperWithArg)(uint32_t value);
typedef uint32_t (*TimerOperWithReturn)(void);

typedef struct
{
    bool alarm_on;
    /* Indicate the cycle time of iterative expirations */
    Timer_T cycle_timer;
} HAL_RTC_Timer_T;

typedef struct
{
    uint32_t        rtc_clk_freq;
    uint32_t        prescaler;
    uint32_t        prescaler_calendar;
    uint32_t        ticks_per_1s;
    uint32_t        ticks_per_1s_calendar;
    UTCTime_T       utc_time;
    HAL_RTC_Timer_T     rtc_timer;
    /*--------------------------------------------------------------
     * 1 means date and time are stored in the registers in Bin mode
     * 0 means date and time are stored in the registers in BCD mode
     *--------------------------------------------------------------*/
    bool bin_mode;
    uint32_t wakeup_cycle[5];/* the unit: ms*/
    int current_wakeup_index;
    uint8_t tdivsel;
    bool locked;
    IRQn_Type calendar_irq;
    Timer_Callback alarm_handler;
    Timer_Callback timer_handler;
    uint8_t hosc_div;
} HAL_RTC_Driver_T;

/*******************************************************************************
* Global variables
*******************************************************************************/
/*******************************************************************************
* Static variables
*******************************************************************************/
static HAL_RTC_Driver_T sRtcDriverT;
/*******************************************************************************
* Local function prototypes
*******************************************************************************/
#if DEBUG_MODE
    static uint32_t HAL_RTC_GetVersion(void);
#endif
static void HAL_RTC_GlobalInit(HAL_RTC_Config_T *pConfig);

/*******************************************************************************
* Codes
*******************************************************************************/


#if (DEBUG_MODE)
/* Print some arguments to help to analysis */
static void HAL_RTC_PrintArgs(void)
{
    PR_DEBUG("--------------------------------\n");
    PR_DEBUG("-        HAL_RTC_PrintArgs         -\n");
    PR_DEBUG("--------------------------------\n");
    PR_DEBUG("Platform:%s\n", "CHIP_GM6721");
    PR_DEBUG("RTC Version %d\n", HAL_RTC_GetVersion());
    PR_DEBUG("RTC clock:%dHz\n", sRtcDriverT.rtc_clk_freq);
    PR_DEBUG("prescaler:%d\n", sRtcDriverT.prescaler);
    PR_DEBUG("ticks_per_second:%d\n", sRtcDriverT.ticks_per_1s);
    PR_DEBUG("--------------------------------\n\n");
}
#endif /* #if (DEBUG_MODE) */

void HAL_RTC_WaitReady(void);


/* Ought to unlock the register firstly before almost all writing about rtc registers.
    Use the variable `sRtcDriverT.locked` to avoid multiple call. */

void HAL_RTC_Lock(void)
{
    if (!sRtcDriverT.locked)
    {
        IO_BITMASK_SET(RTC->PROTECT, LL_RTC_MASK_ALL, LL_RTC_PROTECT_LOCK);
        sRtcDriverT.locked = True;
    }
}

void HAL_RTC_Unlock(void)
{
    if (sRtcDriverT.locked)
    {
        IO_BITMASK_SET(RTC->PROTECT, LL_RTC_MASK_ALL, LL_RTC_PROTECT_UNLOCK);
        sRtcDriverT.locked = False;
    }
}

/* RTC trigger acmp use wakeup4 in default */
void HAL_RTC_TriggerACMPEnable(void)
{
    HAL_RTC_Unlock();

    IO_BIT_CLEAR(RTC->CTRL, LL_RTC_ACMP_TRIEN);
    while (!(IO_BIT_GET(RTC->SPECIAL, LL_RTC_ACMPTRI_WEF)));
    IO_BITMASK_SET(RTC->CTRL, LL_RTC_ACMP_SEL_MASK, LL_RTC_ACMP_SEL_WAKEUP4);
    IO_BIT_SET(RTC->CTRL, LL_RTC_ACMP_TRIEN);

    HAL_RTC_Lock();
}

void HAL_RTC_TriggerACMPDisable(void)
{
    HAL_RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, LL_RTC_ACMP_TRIEN);
    HAL_RTC_Lock();
}

/* RTC trigger adc use alarm pes source in default */
void HAL_RTC_AlarmPESEnable(void)
{
    HAL_RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, LL_RTC_EVSEL_EN);
    while (!(IO_BIT_GET(RTC->SPECIAL, LL_RTC_EVSEL_WEF)));
    IO_BITMASK_SET(RTC->CTRL, LL_RTC_EV_SEL_MASK, LL_RTC_EV_SEL_ALARM);
    IO_BIT_SET(RTC->CTRL, LL_RTC_EVSEL_EN);

    HAL_RTC_Lock();
}

/**
* @brief Enable Tamper irq
* @param
* @return
* @note
*/
void HAL_RTC_IrqTamper_Enable(void)
{

    //NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, LL_RTC_MASK_ALL, LL_RTC_IRQ_TAMPER1_IE | LL_RTC_IRQ_TAMPER2_IE);
    HAL_RTC_Lock();

    //HAL_IRQ_ENABLE(RTC_IRQn);
}


/**
* @brief Disable Tamper irq
* @param
* @return
* @note
*/
void HAL_RTC_IrqTamper_Disable(void)
{

    //NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, LL_RTC_IRQ_TAMPER1_IE | LL_RTC_IRQ_TAMPER2_IE);
    HAL_RTC_Lock();

    //HAL_IRQ_DISABLE(RTC_IRQn);
}


/**
* @brief Two tampers test with different samples
* @param
* @return 0
* @note
*/
int HAL_RTC_Tamper_Enable()
{
    HAL_RTC_IrqTamper_Enable();

    HAL_RTC_Unlock();

    /* tampflt :  2 ^ 0 consecutive samples,  more sensitive */
    /* tampfre :  input sample RTCLK/(2^1) */
    IO_BITMASK_SET(RTC->TAMPER1, LL_RTC_MASK_ALL, ((0 << LL_RTC_TAMPPUDIS_POS) | (3 << LL_RTC_TAMPPRCH_POS) | (3 << LL_RTC_TAMPFLT_POS) |
                   (1 << LL_RTC_TAMPFRE_POS) | (1 << LL_RTC_TAMPTS) | (LL_RTC_TAMPER_TRIGGER_LEVEL << LL_RTC_TAMPTRG) | (1 << LL_RTC_TAMPE)));
    /* tampflt :  2 ^ 3 consecutive samples */
    /* tampfre :  input sample RTCLK/(2^1) */
    IO_BITMASK_SET(RTC->TAMPER2, LL_RTC_MASK_ALL, ((0 << LL_RTC_TAMPPUDIS_POS) | (3 << LL_RTC_TAMPPRCH_POS) | (0 << LL_RTC_TAMPFLT_POS) |
                   (1 << LL_RTC_TAMPFRE_POS) | (1 << LL_RTC_TAMPTS) | (LL_RTC_TAMPER_TRIGGER_LEVEL << LL_RTC_TAMPTRG) | (1 << LL_RTC_TAMPE)));
    HAL_RTC_Lock();
    //HAL_RTC_IrqTamper_Disable();
    return 0;
}

/* RTC trigger adc use tamper pes source in default */
void HAL_RTC_TamperPESEnable(int tamper_index)
{
    HAL_RTC_Tamper_Enable();
    HAL_RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, LL_RTC_EVSEL_EN);
    while (!(IO_BIT_GET(RTC->SPECIAL, LL_RTC_EVSEL_WEF)));
    if (tamper_index == 1)
    {
        IO_BITMASK_SET(RTC->CTRL, LL_RTC_EV_SEL_MASK, LL_RTC_EV_SEL_TAMPER1);
    }
    else
    {
        IO_BITMASK_SET(RTC->CTRL, LL_RTC_EV_SEL_MASK, LL_RTC_EV_SEL_TAMPER2);
    }
    IO_BIT_SET(RTC->CTRL, LL_RTC_EVSEL_EN);

    HAL_RTC_Lock();
}


/* WRDY
 * 1:  means the RTCTM0/ RTCTM1/ RTCSUBSEC/RTCINTEN/RTCSPECIAL register
 *   have been ready to be wrote
 * 0:  means the RTCTM0/ RTCTM1/ RTCSUBSEC/ RTCINTEN/ RTCSPECIAL[3:0] register
 *   have been not ready to be wrote yet, and you can't write to these register
 */
uint32_t HAL_RTC_ReadyToWrite(void)
{
    return IO_BIT_GET(RTC->CTRL, LL_RTC_WRDY);
}


/* When the RTCCLK is 32.768K,
 * you can read the current time register(RTCTM0/ RTCTM1) safely
 * if the SAFE flag is '1'
 */
uint32_t HAL_RTC_CanReadSafely(void)
{
    return IO_BIT_GET(RTC->SPECIAL, LL_RTC_SAFE);
}

/* the RTCTM0 will be captured into the RTCTIMSTAMP0 register and
 * the { RTCSUBSEC[15:0], RTCTM1[15:0], not include year} will be captured into the RTCTIMSTAMP1 register
 * Note1 : When read this bit ,the value '0' will be returned
 * Note2: You can use this function to get the current time safely
 */
void HAL_RTC_DumpTimeSafely(void)
{
    int flag = (IO_BIT_GET(RTC->CTRL, LL_RTC_LOCK) != 0);
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_SET(RTC->SPECIAL, LL_RTC_SOFT_TST);
    if (flag)
    {
        HAL_RTC_Lock();
    }
    HAL_RTC_WaitReady();
}

void HAL_RTC_WaitReady(void)
{
    uint32_t val = HAL_RTC_ReadyToWrite();
    while ((val & LL_RTC_WRDY) == 0)
    {
        val = HAL_RTC_ReadyToWrite();
        ;
    }
}

void HAL_RTC_WaitClockReady(void)

{
    HAL_RTC_Unlock();
    IO_BIT_SET(RTC->CTRL, LL_RTC_HOLD);
    while (!IO_BIT_GET(RTC->SPECIAL, LL_RTC_TICK_WEF));
    IO_BIT_CLEAR(RTC->CTRL, LL_RTC_HOLD);
    HAL_RTC_Lock();
}

#if (DEBUG_MODE)
/* Get RTC module version */
static uint32_t HAL_RTC_GetVersion(void)
{
    return IO_BITS_GET(RTC->VER, LL_RTC_MASK_ALL);
}
#endif

/**
* @brief Set wakeup initial cnt,  WAKEUPCNT and WAKEUPINI
* @param
* @return
* @note The 16bit wakeup timer counter, when the WTR1-WAKEUPINI isn't 0 and
*   WTR0-WAKEUPCNT isn't 0, the counter will decrease 1 at the clock of HAL_RTC_CLK/2,
*   until it decrease to 0, then the wakeup interrupt will happen and
*   the WAKEUPINI will reload to the WAKEUPCNT, and repeat to decreasin.
*   Decrease frequency = HAL_RTC_CLK/2/(PRESCALE+1)/(2^(13-TDIVSEL)
*   RTCCTRL[10:8]
*   0 -> 2^13
*   1 -> 2^12
*   2 -> 2^11
*   ...
*   7 -> 2^6
*/
static void HAL_RTC_StartWakeup(uint32_t wakeup_cnt)
{
    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->WAKEUPT0, 0xFFFF, wakeup_cnt);
    IO_BITMASK_SET(RTC->WAKEUPT1, 0xFFFF, wakeup_cnt);
    HAL_RTC_Lock();
}

static void HAL_RTC_StopWakeup(void)
{
    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->WAKEUPT0, 0xFFFF, 0);
    IO_BITMASK_SET(RTC->WAKEUPT1, 0xFFFF, 0);
    HAL_RTC_Lock();
}

/* Disable the specific wakeup irq */
void HAL_RTC_DisableWakeupIrq(uint8_t wakeupIndex)
{

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    /* LL_RTC_IRQ_WAKEUP_WP4F_EN|LL_RTC_IRQ_WAKEUP_WP3F_EN|LL_RTC_IRQ_WAKEUP_WP2F_EN|LL_RTC_IRQ_WAKEUP_WP1F_EN|LL_RTC_IRQ_WAKEUP_WP0F_EN */
    IO_BIT_CLEAR(RTC->INTEN, 1 << (6 + wakeupIndex));
    HAL_RTC_Lock();

    //HAL_IRQ_DISABLE(sRtcDriverT.calendar_irq);
}

/* Enable the specific wakeup irq */
void HAL_RTC_EnableWakeupIrq(uint8_t wakeupIndex)
{

    //NVIC_ClearPendingIRQ(sRtcDriverT.calendar_irq);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    /* LL_RTC_IRQ_WAKEUP_WP4F_EN|LL_RTC_IRQ_WAKEUP_WP3F_EN|LL_RTC_IRQ_WAKEUP_WP2F_EN|LL_RTC_IRQ_WAKEUP_WP1F_EN|LL_RTC_IRQ_WAKEUP_WP0F_EN */
    IO_BITMASK_SET(RTC->INTEN, LL_RTC_MASK_ALL, 1 << (6 + wakeupIndex));
    HAL_RTC_Lock();

    //HAL_IRQ_ENABLE(sRtcDriverT.calendar_irq);
}

/* Disable second irq */
void HAL_RTC_DisableSecondIrq()
{
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, LL_RTC_IRQ_SECOND_EN);
    HAL_RTC_Lock();

    //HAL_IRQ_DISABLE(sRtcDriverT.calendar_irq);
}

/* Enable second irq */
void HAL_RTC_EnableSecondIrq()
{

    //NVIC_ClearPendingIRQ(sRtcDriverT.calendar_irq);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, LL_RTC_MASK_ALL, LL_RTC_IRQ_SECOND_EN);
    HAL_RTC_Lock();

    //HAL_IRQ_ENABLE(sRtcDriverT.calendar_irq);
}

/**
* @brief Set alarm with the format day(wd), hour:minute:second, pm, format12
* @param pm: 1-pm, 0-am or 24-hours format
*        format12: 1 - 12-hours format, 0 - 24-hours format
*        wd:  0 - the DAY is the day of month, 1 - the DAY[3:0] is the week day
* @return
* @note
*/
static void HAL_RTC_ConfigAlarm(uint8_t wd, uint32_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t pm, uint8_t format12)
{
    if (sRtcDriverT.bin_mode)
    {
        /* 24-hour format */
        if (!format12)
        {
            pm = 0;
        }
        IO_BITMASK_SET(RTC->ALARM, LL_RTC_MASK_ALL, LL_RTC_ALARM_BIN_TIME(wd, day, hour, minute, second, pm));
    }
    else
    {
        IO_BITMASK_SET(RTC->ALARM, LL_RTC_MASK_ALL, LL_RTC_ALARM_BCD_TIME(LL_RTC_BIN2BCD(wd), LL_RTC_BIN2BCD(day), LL_RTC_BIN2BCD(hour), LL_RTC_BIN2BCD(minute),
                       LL_RTC_BIN2BCD(second), pm));
    }
    IO_BITMASK_SET(RTC->CTRL,  LL_RTC_BIN_FORMAT12_MASK, format12 << LL_RTC_BIN_FORMAT12_POS);
}

/* Enable alarm irq */
void HAL_RTC_EnableAlarmIrq(void)
{

    //NVIC_ClearPendingIRQ(sRtcDriverT.calendar_irq);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_SET(RTC->INTEN, LL_RTC_IRQ_ALARM_IE);
    HAL_RTC_Lock();

    //HAL_IRQ_ENABLE(sRtcDriverT.calendar_irq);
}

/* Disable alarm irq */
void HAL_RTC_DisableAlarmIrq(void)
{
    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, LL_RTC_IRQ_ALARM_IE);
    HAL_RTC_Lock();

    //HAL_IRQ_DISABLE(sRtcDriverT.calendar_irq);
}

/* Enbale the alarm */
static void HAL_RTC_EnableAlarm(void)
{
    HAL_RTC_Unlock();
    IO_BIT_SET(RTC->CTRL, LL_RTC_ALARM_EN);
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, ((1 << LL_RTC_ALARM_DAY_AE_POS) | (1 << LL_RTC_ALARM_HOUR_AE_POS) | (1 << LL_RTC_ALARM_MIN_AE_POS) | (1 << LL_RTC_ALARM_SEC_AE_POS)));
    HAL_RTC_Lock();

    HAL_RTC_EnableAlarmIrq();
}

/* Disable the alarm */
static void HAL_RTC_DisableAlarm(void)
{
    HAL_RTC_DisableAlarmIrq();
    HAL_RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, LL_RTC_ALARM_EN);
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, 0);
    HAL_RTC_Lock();
}

/**
* @brief Hold the timer.
* @param hold 1 - pause the tick
*             0 - resume the tick
* @return negative value: failed
* @note
*/
static void HAL_RTC_HoldTime(uint8_t hold)
{
    HAL_RTC_Unlock();
    if (hold)
    {
        IO_BIT_SET(RTC->CTRL, RTC_LL_TIMER_HOLD_MASK);
    }
    else
    {
        IO_BIT_CLEAR(RTC->CTRL, RTC_LL_TIMER_HOLD_MASK);
    }
    HAL_RTC_Lock();
}

/**
* @brief Set global storage mode
* @param 1: bin mode; 0: bcd mode
* @return
* @note
*/
void HAL_RTC_SetBinMode(bool bin_mode)
{

    HAL_RTC_Unlock();
    if (bin_mode == True)
    {
        IO_BIT_CLEAR(RTC->CTRL, LL_RTC_BIN_BCD_MODE);
    }
    else
    {
        IO_BIT_SET(RTC->CTRL, LL_RTC_BIN_BCD_MODE);
    }
    HAL_RTC_Lock();

    sRtcDriverT.bin_mode = bin_mode;
}

/**
* @brief Set RTC time in bin mode or bcd mode
* @param pm: 1 - pm,  0 - am or 24 hours
*        format12: 0 - 24-hours format, 1 - 12-hours format
* @return
* @note
*/
void HAL_RTC_SetTimeEx(uint8_t hour, uint8_t minute, uint8_t second, uint8_t pm, uint8_t format12)
{
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    if (sRtcDriverT.bin_mode)
    {
        /* 24-hour format */
        if (!format12)
        {
            pm = 0;
        }
        IO_BITMASK_SET(RTC->TM0, 0xFFFFFFFF, LL_RTC_BIN_TIME(hour, minute, second, pm));
    }
    else
    {
        IO_BITMASK_SET(RTC->TM0, 0xFFFFFFFF, LL_RTC_BCD_TIME(LL_RTC_BIN2BCD(hour), LL_RTC_BIN2BCD(minute),
                       LL_RTC_BIN2BCD(second), pm));
    }
    IO_BITMASK_SET(RTC->CTRL,   LL_RTC_BIN_FORMAT12_MASK,  format12 << LL_RTC_BIN_FORMAT12_POS);
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->SUBSEC, LL_RTC_BIN_SUBSECOND_MASK, (sRtcDriverT.ticks_per_1s_calendar - 1) << LL_RTC_BIN_SUBSECOND_POS);
    HAL_RTC_WaitReady();
    HAL_RTC_Lock();
}


/**
* @brief Set RTC date in bin mode or bcd mode
* @param
* @return
* @note
*/
void HAL_RTC_SetDateEx(uint32_t year, uint32_t month, uint32_t day, uint32_t week)
{

    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    if (sRtcDriverT.bin_mode)
    {
        IO_BITMASK_SET(RTC->TM1, LL_RTC_MASK_ALL, LL_RTC_BIN_YEAR(year, month, day, week));
    }
    else
    {
        uint32_t bcd_year_high = 0;
        uint32_t bcd_year_low  = 0;
        uint32_t bcd_month = 0;
        uint32_t bcd_day = 0;
        uint32_t bcd_week = 0;

        bcd_year_high = LL_RTC_BIN2BCD(year / 100);
        bcd_year_low  = LL_RTC_BIN2BCD(year % 100);
        bcd_month = LL_RTC_BIN2BCD(month);
        bcd_day = LL_RTC_BIN2BCD(day);
        bcd_week = LL_RTC_BIN2BCD(week);

        IO_BITMASK_SET(RTC->TM1, LL_RTC_MASK_ALL, LL_RTC_BCD_DATE(LL_RTC_BCD_YEAR(bcd_year_high, bcd_year_low), LL_RTC_BCD_MONTH(bcd_month),
                       LL_RTC_BCD_MONTH(bcd_day), LL_RTC_BCD_WEEK(bcd_week)));
    }
    HAL_RTC_Lock();
}

/**
* @brief Get RTC date in bin mode or bcd mode
* @param date_time_type: DATE_TIME_TIMESTAMP/DATE_TIME_RTCTIME
* @return
* @note
*/
static void HAL_RTC_GetBasicDate(UTCTime_T *pTime, DATE_TIME_TYPE date_time_type)
{
    uint32_t date_value = 0;
    if (date_time_type == DATE_TIME_TIMESTAMP)
    {
        date_value = IO_BITS_GET(RTC->TIMSTAMP1, LL_RTC_MASK_ALL);
    }
    else
    {
        date_value = IO_BITS_GET(RTC->TM1, LL_RTC_MASK_ALL);
    }
    if (sRtcDriverT.bin_mode)
    {
        pTime->year     = IO_BITS_GET(date_value, LL_RTC_BIN_YEAR_MASK) >> LL_RTC_BIN_YEAR_POS;
        pTime->mon      = IO_BITS_GET(date_value, LL_RTC_BIN_MONTH_MASK) >> LL_RTC_BIN_MONTH_POS;
        pTime->day      = IO_BITS_GET(date_value, LL_RTC_BIN_DAY_MASK) >> LL_RTC_BIN_DAY_POS;
        pTime->wday     = IO_BITS_GET(date_value, LL_RTC_BIN_WEEK_MASK) >> LL_RTC_BIN_WEEK_POS;
    }
    else
    {
        pTime->year     = LL_RTC_BCD2BIN(LL_RTC_BCD_YEAR_HIGH_GET_VALUE(date_value)) * 100 +
                          LL_RTC_BCD2BIN(LL_RTC_BCD_YEAR_LOW_GET_VALUE(date_value));
        pTime->mon      = LL_RTC_BCD2BIN(LL_RTC_BCD_MONTH_GET_VALUE(date_value));
        pTime->day      = LL_RTC_BCD2BIN(LL_RTC_BCD_DAY_GET_VALUE(date_value));
        pTime->wday     = LL_RTC_BCD2BIN(LL_RTC_BCD_WEEK_GET_VALUE(date_value));
    }
    if (date_time_type == DATE_TIME_TIMESTAMP)      /* timestamp has no year bits */
    {
        pTime->year = 0;
    }
    pTime->fmt12        = IO_BITS_GET(date_value, LL_RTC_BIN_FORMAT12_MASK) >> LL_RTC_BIN_FORMAT12_POS;
}


/**
* @brief Get RTC time in bin mode or bcd mode
* @param date_time_type: DATE_TIME_TIMESTAMP/DATE_TIME_RTCTIME
* @return
* @note
*/
static void HAL_RTC_GetBasicTime(UTCTime_T *pTime, DATE_TIME_TYPE date_time_type)
{
    uint32_t time_value = 0;

    if (date_time_type == DATE_TIME_TIMESTAMP)
    {
        time_value = IO_BITS_GET(RTC->TIMSTAMP0, LL_RTC_MASK_ALL);
    }
    else
    {
        time_value = IO_BITS_GET(RTC->TM0, LL_RTC_MASK_ALL);
    }
    if (sRtcDriverT.bin_mode)
    {
        pTime->hour     = IO_BITS_GET(time_value, LL_RTC_BIN_HOUR_MASK) >> LL_RTC_BIN_HOUR_POS;
        pTime->min      = IO_BITS_GET(time_value, LL_RTC_BIN_MINUTE_MASK) >> LL_RTC_BIN_MINUTE_POS;
        pTime->sec      = IO_BITS_GET(time_value, LL_RTC_BIN_SECOND_MASK) >> LL_RTC_BIN_SECOND_POS;
    }
    else
    {
        pTime->hour     = LL_RTC_BCD2BIN(LL_RTC_BCD_HOUR_GET_VALUE(time_value));
        pTime->min      = LL_RTC_BCD2BIN(LL_RTC_BCD_MINUTE_GET_VALUE(time_value));
        pTime->sec      = LL_RTC_BCD2BIN(LL_RTC_BCD_SECOND_GET_VALUE(time_value));
    }
    pTime->subsec   = IO_BITS_GET(RTC->SUBSEC, LL_RTC_BIN_SUBSECOND_MASK) >> LL_RTC_BIN_SUBSECOND_POS;
}

/**
* @brief Get the timestamp about some concerned event
* @param
* @return
* @note
*/
void HAL_RTC_GetTimeStamp(UTCTime_T *pTime)
{
    HAL_RTC_GetBasicDate(pTime, DATE_TIME_TIMESTAMP);
    HAL_RTC_GetBasicTime(pTime, DATE_TIME_TIMESTAMP);
}

/**
* @brief Get RTC year in bin mode or bcd mode
* @param
* @return the year from 1 to 4000
* @note
*/
static uint32_t HAL_RTC_GetYear(void)
{
    if (sRtcDriverT.bin_mode)
    {
        return IO_BITS_GET(RTC->TM1, LL_RTC_BIN_YEAR_MASK) >> LL_RTC_BIN_YEAR_POS;
    }
    else
    {
        return (LL_RTC_BCD2BIN(LL_RTC_BCD_YEAR_HIGH(RTC->TM1)) * 100 + LL_RTC_BCD2BIN(LL_RTC_BCD_YEAR_LOW(RTC->TM1)));
    }
}


/* Get RTC time */
void HAL_RTC_GetTimeEx(UTCTime_T *pTime)
{
    if ((HAL_RTC_CanReadSafely() & LL_RTC_SAFE) != 0)
    {
        HAL_RTC_GetBasicDate(pTime, DATE_TIME_RTCTIME);
        HAL_RTC_GetBasicTime(pTime, DATE_TIME_RTCTIME);
    }
    else
    {
        HAL_RTC_DumpTimeSafely();
        HAL_RTC_GetTimeStamp(pTime);
        pTime->year = HAL_RTC_GetYear();
    }
}

/* Get the real frequency of Wakeup[x]  */
static void HAL_RTC_InitWakeupFreq(void)
{
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        sRtcDriverT.wakeup_cycle[i] = 1000 * 2 * (sRtcDriverT.prescaler_calendar + 1) * (1 << (13 - i - sRtcDriverT.tdivsel)) / sRtcDriverT.rtc_clk_freq;
#if (DEBUG_MODE)
        PR_DEBUG("wakeup[%d] %dms\n", i, sRtcDriverT.wakeup_cycle[i]);
#endif
    }
    sRtcDriverT.wakeup_cycle[i] = 1000 * 2 * ((LL_RTC_MAX_WAKEUP_CNT) + 1) / sRtcDriverT.rtc_clk_freq;
#if (DEBUG_MODE)
    PR_DEBUG("wakeup[%d] %dms\n", i, sRtcDriverT.wakeup_cycle[i]);
#endif
}

/**
* @brief Set tickval and prescale
* @param max value of prescale is 0x7F
* @param max value of tickvalue is 0xFFFF
* @return
* @note 1s can be generated by
*       RTCCLK / 2 / (PRE_SCALE+1) / (TICKVAL + 1),
*/
static void HAL_RTC_SetTickValue(uint32_t prescale, uint32_t tickvalue)
{
    IO_BITMASK_SET(RTC->TICKVAL, LL_RTC_PRESCALE_MASK,  prescale << LL_RTC_PRESCALE_POS);
    IO_BITMASK_SET(RTC->TICKVAL, LL_RTC_TICKVALUE_MASK, tickvalue << LL_RTC_TICKVAULE_POS);
}

/**
* @brief Start iterative callback every `cycle` seconds.
* @param
* @return status: HAL_RTC_STATUS_OK - 0
* @note
* the valid two-types argument description on GM6621 as follows,
* the max msec = MAX_WAKEUP_CNT*1000*2*(PRESCALE+1)/RTCCLK
* (MAX_WAKEUP_CNT=65535, PRESCALE=121, RTCCLK=8000000)
* 1. pTimer->cycle.tv_sec = 1;
* 2. pTimer->cycle.tv_msec = xxx;
*/
RET_CODE HAL_RTC_StartTimer(const Timer_T *pTimer)
{
    HAL_RTC_Timer_T *timer = &sRtcDriverT.rtc_timer;

    uint32_t cnt = 0;
    timer->cycle_timer.cycle  = pTimer->cycle;

    sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec = sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_sec * 1000 + sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec;

    cnt = (sRtcDriverT.ticks_per_1s_calendar) * sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec / 1000;
    if (cnt > 0 && cnt <= LL_RTC_MAX_WAKEUP_CNT)
    {
        sRtcDriverT.current_wakeup_index = 4;
        HAL_RTC_StartWakeup(cnt);
        HAL_RTC_EnableWakeupIrq(sRtcDriverT.current_wakeup_index);
    }
    else
    {
        return RET_INVPARAM;
    }
    return RET_OK;
}

/* Clear timer's time to zero. */
void HAL_RTC_StopTimer(void)
{
    HAL_RTC_Timer_T *timer = &sRtcDriverT.rtc_timer;
    HAL_RTC_StopWakeup();
    HAL_RTC_DisableWakeupIrq(sRtcDriverT.current_wakeup_index);
    sRtcDriverT.current_wakeup_index = -1;
    timer->cycle_timer.cycle.tv_sec = 0;
    timer->cycle_timer.cycle.tv_msec = 0;
}

/* Set the alarm to a future time. */
void HAL_RTC_SetAlarm(UTCTime_T *pTime)
{
    sRtcDriverT.rtc_timer.alarm_on = True;
    HAL_RTC_EnableAlarm();
    HAL_RTC_Unlock();
    /* alarm on the day */
    if (pTime->wday == 0)
    {
        HAL_RTC_ConfigAlarm(0, pTime->day, pTime->hour, pTime->min, pTime->sec, 0, 0);
    }
    /* alarm on the weekday */
    else
    {
        HAL_RTC_ConfigAlarm(1, pTime->wday, pTime->hour, pTime->min, pTime->sec, 0, 0);
    }
    HAL_RTC_Lock();
}

/* Cancel the alarm. */
void HAL_RTC_CancelAlarm(void)
{
    sRtcDriverT.rtc_timer.alarm_on = False;
}

#if 0
/* Register a timer/alarm callback. */
void HAL_RTC_SetCallback(uint32_t callType, Timer_Callback callback)
{
    if (callType < MAX_TIMERTYPE_CNT)
    {
        sRtcDriverT.cycle_timerHandler[callType] = callback;
    }
}
#endif

/**
* @brief to check the expected time of th timer/alarm.
*       if the seconds are matched, call the registered handler
*       to notify the observer.
* @param
* @return
* @note
*/
void HAL_RTC_IRQHandler(void)
{
    if (!sRtcDriverT.rtc_timer.alarm_on)
    {
        HAL_RTC_Unlock();
        if ((sRtcDriverT.current_wakeup_index == 4) &&
                IO_BIT_GET(RTC->FLAG, LL_RTC_IRQ_WAKEUP_WP4F_EN))
        {
            IO_BIT_SET(RTC->INTCLR, LL_RTC_IRQ_WAKEUP_WP4F_EN);
            if (sRtcDriverT.timer_handler)
            {
                sRtcDriverT.timer_handler(HAL_RTC_TIMER);
            }
        }
    }
    else
    {
        if (IO_BIT_GET(RTC->FLAG, LL_RTC_IRQ_ALARM_IE))
        {
            IO_BIT_SET(RTC->INTCLR, LL_RTC_IRQ_ALARM_IE);
            HAL_RTC_DisableAlarm();
            if (sRtcDriverT.alarm_handler)
            {
                sRtcDriverT.alarm_handler(HAL_RTC_ALARM);
            }
        }
        HAL_RTC_Lock();
    }
}


#if 0
/* Get info if timer interrupt occurs. */
static uint32_t HAL_RTC_InterruptStatus_Get(void)
{
    return IO_BITS_GET(RTC->TM_INTST, INTST_MASK);
}
#endif

/**
* @brief Return the current time as a `Time_T` and also put it in *t
* if t is not NULL. Time is represented as seconds from Jan 1 00:00:00 1970.
* @param t - seconds from Jan 1 00:00:00 1970.
* @return Time_T - (32 bits) seconds from Jan 1 00:00:00 1970.
* @note
*/
Time_T HAL_RTC_GetTickSecond(void)
{
    Time_T res = 0;

    UTCTime_T time;

    HAL_RTC_GetTimeEx(&time);
    time.year -= 1990;
    time.mon -= 1;
    HAL_RTC_ConvertToWday0(time.wday);
    res = HAL_RTC_ConvertToTickSecond(&time);

    return res;
}

/**
* @brief Set Time_T to RTC counter.
* @param when - seconds from Jan 1 00:00:00 1970.
* @return
* @note
*/
void HAL_RTC_SetTickSecond(const Time_T *pTime)
{
    UTCTime_T *pUtcTime = 0;
    pUtcTime = HAL_RTC_ConvertToUtcTime(pTime);
    pUtcTime->year += 1990;
    pUtcTime->mon += 1;
    HAL_RTC_SetDateTime(pUtcTime->year, pUtcTime->mon, pUtcTime->day,
                        pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
}

/**
* @brief Convert the seconds from Jan 1 00:00:00 1970 to UTC time.
* @param t - seconds from Jan 1 00:00:00 1970.
* @return the `UTCTime_T' representation of *t in UTC.
* @note
*/
UTCTime_T *HAL_RTC_ConvertToUtcTime(const Time_T *t)
{
    return HAL_UTC_Convert(t, &sRtcDriverT.utc_time);
}

/**
* @brief Convert UTC time to the seconds from Jan 1 00:00:00 1970.
* @param tp - UTC time
* @return the seconds from Jan 1 00:00:00 1970.
* @note
*/
Time_T HAL_RTC_ConvertToTickSecond(UTCTime_T *tp)
{
    return HAL_UTC_ToTimeT(tp);
}

/**
* @brief Set time
* @param hour:min:sec
* @return
* @note this operation will pause the RTC tick momently.
*/
void HAL_RTC_SetTime(uint32_t hour, uint32_t min, uint32_t sec)
{
    HAL_RTC_Unlock();
    HAL_RTC_SetTimeEx(hour, min, sec, 0, 0);
    HAL_RTC_Lock();
}

/**
* @brief Set date
* @param year/mon/day
* @return
* @note this operation will pause the RTC tick momently.
*/
void HAL_RTC_SetDate(uint32_t year, uint32_t mon, uint32_t day)
{
    HAL_RTC_Unlock();
    HAL_RTC_SetDateEx(year, mon, day,  HAL_UTC_CalcWeekday(year, mon, day));
    HAL_RTC_Lock();
}

/**
* @brief Set date and time
* @param year/mon/day hour:min:sec
* @return
* @note this operation will pause the RTC tick momently.
*/
RET_CODE HAL_RTC_SetDateTime(uint32_t year, uint32_t mon, uint32_t day,
                             uint32_t hour, uint32_t min, uint32_t sec)
{
    if (year > 3999 || mon < 1 || mon > 12 || day == 0 || day > 31
            || hour > 23 || min > 59 || sec > 59)
    {
        return RET_INVPARAM;
    }

    HAL_RTC_Unlock();
    HAL_RTC_SetDateEx(year, mon, day, HAL_UTC_CalcWeekday(year, mon, day));
    HAL_RTC_SetTimeEx(hour, min, sec, 0, 0);
    HAL_RTC_Lock();

    return RET_OK;
}

/**
* @brief Get date and time
* @param none
* @return UtcTime pointer
* @note
*/
UTCTime_T *HAL_RTC_GetTime(void)
{
    HAL_RTC_GetTimeEx(&sRtcDriverT.utc_time);
    return &sRtcDriverT.utc_time;
}

void HAL_RTC_HwReset(void)
{
    HAL_Reset_Module(RESET_BKD_SW_RSTJ);
}

static RET_CODE HAL_RTC_SetClkSrc(HAL_RTC_Config_T *config)
{
    RET_CODE ret = RET_OK;
    Peripheral_Config_T peripheClockSet;

    peripheClockSet.peripheMask = PERIPHERAL_RTC_MASK;
    peripheClockSet.peripheClkConfig.rtcClkConfig.rtcHOSCDiv   = (PeripheClk_RTC_hoscdiv_T)sRtcDriverT.hosc_div;
    peripheClockSet.peripheClkConfig.rtcClkConfig.rtcSourceClk = (PeripheClk_RTC_sel_T)config->clock;

    HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK0_RTC);
    HAL_CLOCK_PeripheralClkSetConfig(&peripheClockSet);

    return ret;
}

void HAL_RTC_SetCallback(Timer_Callback handler)
{
    sRtcDriverT.alarm_handler = handler;
    sRtcDriverT.timer_handler = handler;
}

void HAL_RTC_Set_Subsec(int val)
{
    LL_RTC_SUBSEC_INIT(RTC, val);
}

void HAL_RTC_ClearIrqFlag()
{
    LL_RTC_CLEAR_IRQ_FLAG(RTC);
}

void HAL_RTC_EnableIrq()
{
    HAL_RequestIrq(RTC_IRQn, RTC_IRQ_PRIORITY, HAL_RTC_IRQHandler, 0);
    HAL_IRQ_ENABLE(RTC_IRQn);
}

/**
* @brief Set Rtc clock being derived from oscillator
            OSC_LS(32.768K) (RTC0_CLOCK_OSCLS)
            or
            OSC_HS(8M) (RTC0_CLOCK_PLLSRC + OSC_EXTERNAL_CLOCK)
*           and enable interrupt.
* @param
* @return
* @note
*/
void HAL_RTC_Init(HAL_RTC_Config_T *pConfig)
{
    HAL_RTC_HwReset();

    /* Init sRtcDriverT */
    HAL_RTC_GlobalInit(pConfig);

    HAL_RTC_SetClkSrc(pConfig);

    HAL_RTC_WaitReady();

    /* Enable access to the backup domain */
    BKP_WR_UNLOCK();

    HAL_RTC_SetBinMode(True);

    HAL_RTC_Unlock();
    HAL_RTC_Set_Subsec(0);
    /* 1sec = 31249*subsecond (8000K / 2 / (PRESCALE+1) / (TICKS_PER_1S+1)) */
    HAL_RTC_SetTickValue(sRtcDriverT.prescaler_calendar, sRtcDriverT.ticks_per_1s_calendar);
    HAL_RTC_WaitClockReady();
    HAL_RTC_Lock();

    HAL_RTC_Stop();

    HAL_RTC_EnableIrq();

    /* no need to unlock RTCCLR */
    /* clear all interrupt pending flag */
    HAL_RTC_ClearIrqFlag();

    HAL_RTC_InitWakeupFreq();

    HAL_RTC_SetCallback(pConfig->alarmCallback);

#if (DEBUG_MODE)
    HAL_RTC_PrintArgs();
#endif
}

void HAL_RTC_DeInit(void)
{
    /* do nothing */
}

/* Start Rtc counter. */
void HAL_RTC_Start(void)
{
    HAL_RTC_HoldTime(0);
}

/* Stop Rtc counter.*/
void HAL_RTC_Stop(void)
{
    HAL_RTC_HoldTime(1);
}

/**
* @brief Reset the time to Jan 1 00:00:00 1970.
* @param
* @return
* @note
*/
void HAL_RTC_Reset(void)
{
    HAL_RTC_Unlock();
    HAL_RTC_SetDateEx(1970, 1, 1, 4);
    HAL_RTC_SetTimeEx(0, 0, 0, 0, 0);
    HAL_RTC_Lock();
}

static void HAL_RTC_CalcTicks(void)
{
    uint32_t freq = sRtcDriverT.rtc_clk_freq;
    sRtcDriverT.prescaler             = 121;
    sRtcDriverT.prescaler_calendar    = 121;
    sRtcDriverT.ticks_per_1s          = freq / 2 / (sRtcDriverT.prescaler + 1) - 1;
    sRtcDriverT.ticks_per_1s_calendar = sRtcDriverT.ticks_per_1s;
}

static int HAL_RTC_GetFreq(HAL_RTC_Config_T *pConfig)
{
    int freq = LL_RTC_CLK_FREQ_32K;

#if defined LL_RTC_FPGA_CLK
    PR_DEBUG("(RTC clock is %d Hz now on FPGA.)\n", LL_RTC_FPGA_CLK);
    return LL_RTC_FPGA_CLK;
#endif
    switch (pConfig->clock)
    {
        case HAL_RTC_CLOCK_LIRC:
            freq = LL_RTC_CLK_FREQ_32K;
            break;
        case HAL_RTC_CLOCK_LOSC:
            freq = LL_RTC_CLK_FREQ_32K;
            break;
        case HAL_RTC_CLOCK_HOSC:
            freq = LL_RTC_CLK_FREQ_8M / (sRtcDriverT.hosc_div + 1);
            break;
    }

    return freq;

}

static void HAL_RTC_GlobalInit(HAL_RTC_Config_T *pConfig)
{
    //tick = RTCCLK/2/(PRE_SCALE+1) - 1 < 0xffff;
    //==> RTCCLK < 15990784
    //==> HOSC_CLK / HOSC_DIV < 15990784
    //==> hosc_div > 8000000 / 15990784
    sRtcDriverT.hosc_div             = 0;
    sRtcDriverT.rtc_clk_freq = HAL_RTC_GetFreq(pConfig);
    HAL_RTC_CalcTicks();

    memset(&sRtcDriverT.utc_time, 0, sizeof(UTCTime_T));
    sRtcDriverT.bin_mode             = True;
    sRtcDriverT.current_wakeup_index = -1;
    sRtcDriverT.tdivsel              = 0;
    sRtcDriverT.locked               = True;
    memset(&sRtcDriverT.wakeup_cycle, 0, sizeof(uint32_t) * 5);
    sRtcDriverT.calendar_irq         = RTC_IRQn;
    sRtcDriverT.alarm_handler        = NULL;
    sRtcDriverT.timer_handler        = NULL;
}

uint32_t HAL_RTC_Get1sTicks()
{
    return sRtcDriverT.ticks_per_1s_calendar;
}

uint32_t HAL_RTC_GetPrescaler()
{
    return sRtcDriverT.prescaler;
}

uint32_t HAL_RTC_GetClockFreq()
{
    return sRtcDriverT.rtc_clk_freq;
}

static int HAL_RTC_DayOfMon(int year, int mon)
{
    int maxday = 30;

    if (mon == 2)
    {
        if (((year % 4 == 0) && (year % 100) != 0) || (year % 400 == 0))
        {
            maxday = 29;
        }
        else
        {
            maxday = 28;
        }
    }
    else if (1 == mon || 3 == mon || 5 == mon || 7 == mon || 8 == mon || 10 == mon || 12 == mon)
    {
        maxday = 31;
    }
    else if (2 == mon || 4 == mon || 6 == mon || 9 == mon || 11 == mon)
    {
        maxday = 30;
    }
    return maxday;
}

RET_CODE HAL_RTC_NextSec(UTCTime_T *pUtcTime, int sec)
{
    int maxday, added_hour, added_min, added_sec;
    if (sec > 24 * 3600)
    {
        return RET_INVPARAM;
    }
    added_hour = sec / 3600;
    added_min  = (sec - added_hour * 3600) / 60;
    added_sec  = sec - added_hour * 3600 - added_min * 60;

    pUtcTime->sec += added_sec;
    pUtcTime->min += added_min;
    pUtcTime->hour += added_hour;

    if (pUtcTime->sec >= 60)
    {
        pUtcTime->min += (pUtcTime->sec / 60);
        pUtcTime->sec = pUtcTime->sec % 60;
    }
    if (pUtcTime->min >= 60)
    {
        pUtcTime->hour += (pUtcTime->min / 60);
        pUtcTime->min = pUtcTime->min % 60;
    }
    if (pUtcTime->hour >= 24)
    {
        pUtcTime->day += (pUtcTime->hour / 24);
        pUtcTime->wday += (pUtcTime->hour / 24);
        pUtcTime->wday = pUtcTime->wday % 7;
        pUtcTime->hour = pUtcTime->hour % 24;
    }
    maxday = HAL_RTC_DayOfMon(pUtcTime->year, pUtcTime->mon);
    if (pUtcTime->day > maxday)
    {
        pUtcTime->day = 1;
        pUtcTime->mon += 1;
    }
    if (pUtcTime->mon > 12)
    {
        pUtcTime->mon = 1;
        pUtcTime->year += 1;
    }
    if (pUtcTime->year >= 4000)
    {
        return RET_INVPARAM;
    }
    return RET_OK;
}

/*rtc about wkup pin*/
RET_CODE HAL_RTC_ConfigWakeUpEn(RTC_PowerPin_T pin, uint8_t en)
{
    RET_CODE ret = RET_OK;

    HAL_RTC_Unlock();
#if defined(CHIP_GM6721)
    if (pin == RTC_WAKEUP_PIN_XPA7)
    {
        LL_RTC_XPA7_WAKEUP_EN(RTC, en);
    }
    else if (pin == RTC_WAKEUP_PIN_XPA8)
    {
        LL_RTC_XPA8_WAKEUP_EN(RTC, en);
    }
    else
    {
        ret = RET_INVPARAM;
    }
#elif defined(CHIP_F6721B)
    if (pin == RTC_WAKEUP_PIN_XPB12)
    {
        LL_RTC_XPB12_WAKEUP_EN(RTC, en);
    }
    else if (pin == RTC_WAKEUP_PIN_XPB13)
    {
        LL_RTC_XPB13_WAKEUP_EN(RTC, en);
    }
    else
    {
        ret = RET_INVPARAM;
    }

#endif
    HAL_RTC_Lock();


    return ret;
}


RET_CODE HAL_RTC_ConfigWakeUpPinVaildEdge(RTC_WakeUpPinConfig_T *pinCfg)
{
    RET_CODE ret = RET_OK;
    RTC_PowerPin_T pin;
    RTC_PowerPinVaildTpye_T vaildType;

    if (pinCfg)
    {
        pin = pinCfg->pin;
        vaildType = pinCfg->vaildType;
    }
    else
    {
        return RET_INVPARAM;
    }

#if defined(CHIP_GM6721)
    if (((pin !=  RTC_WAKEUP_PIN_XPA7) && (pin !=  RTC_WAKEUP_PIN_XPA8)) || \
            ((vaildType !=  RTC_WKPIN_RISING_EDGE) && (vaildType !=  RTC_WKPIN_FALLING_EDGE)))
    {
        return RET_INVPARAM;
    }

    HAL_RTC_Unlock();
    if (pin == RTC_WAKEUP_PIN_XPA7)
    {
        LL_RTC_XPA7_SEL_EDGE_VAILD(RTC, vaildType);
    }
    else
    {
        LL_RTC_XPA8_SEL_EDGE_VAILD(RTC, vaildType);
    }
    HAL_RTC_Lock();

#elif defined(CHIP_F6721B)
    if (((pin !=  RTC_WAKEUP_PIN_XPB12) && (pin !=  RTC_WAKEUP_PIN_XPB13)) || \
            ((vaildType !=  RTC_WKPIN_RISING_EDGE) && (vaildType !=  RTC_WKPIN_FALLING_EDGE)))
    {
        return RET_INVPARAM;
    }

    HAL_RTC_Unlock();
    if (pin == RTC_WAKEUP_PIN_XPB12)
    {
        LL_RTC_XPB12_SEL_EDGE_VAILD(RTC, vaildType);
    }
    else
    {
        LL_RTC_XPB13_SEL_EDGE_VAILD(RTC, vaildType);
    }
    HAL_RTC_Lock();

#endif
    return ret;

}


RET_CODE HAL_RTC_ClearWakePendingStatus(RTC_PowerPin_T pin)
{
    RET_CODE ret = RET_OK;

    HAL_RTC_Unlock();

#if defined(CHIP_GM6721)
    if (pin == RTC_WAKEUP_PIN_XPA7)
    {
        LL_RTC_XPA7_CLEAR_WAKEUP_PENDING(RTC);
    }
    else if (pin == RTC_WAKEUP_PIN_XPA8)
    {
        LL_RTC_XPA8_CLEAR_WAKEUP_PENDING(RTC);
    }
    else
    {
        ret = RET_INVPARAM;
    }
#elif defined(CHIP_F6721B)
    if (pin == RTC_WAKEUP_PIN_XPB12)
    {
        LL_RTC_XPB12_CLEAR_WAKEUP_PENDING(RTC);
    }
    else if (pin == RTC_WAKEUP_PIN_XPB13)
    {
        LL_RTC_XPB13_CLEAR_WAKEUP_PENDING(RTC);
    }
    else
    {
        ret = RET_INVPARAM;
    }

#endif
    HAL_RTC_Lock();

    return ret;

}


uint8_t HAL_RTC_CheckWakePinPending(RTC_PowerPin_T pin)
{
    uint8_t status = FALSE;

    HAL_RTC_Unlock();

#if defined(CHIP_GM6721)
    if (pin == RTC_WAKEUP_PIN_XPA7)
    {
        status = LL_RTC_IS_XPA7_WAKEUP_PENGING(RTC);
    }
    else if (pin == RTC_WAKEUP_PIN_XPA8)
    {
        status = LL_RTC_IS_XPA8_WAKEUP_PENGING(RTC);
    }
#elif defined(CHIP_F6721B)
    if (pin == RTC_WAKEUP_PIN_XPB12)
    {
        status = LL_RTC_IS_XPB12_WAKEUP_PENGING(RTC);
    }
    else if (pin == RTC_WAKEUP_PIN_XPB13)
    {
        status = LL_RTC_IS_XPB13_WAKEUP_PENGING(RTC);
    }
#endif
    HAL_RTC_Lock();

    return status;
}






