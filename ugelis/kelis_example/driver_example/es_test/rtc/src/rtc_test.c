/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          rtc_test.c
 *
 * @author        yangy
 *
 * @version       1.0.0
 *
 * @date          2017/04/06
 *
 * @brief         RTC functionality test for GM6621.
 *
 * @note
 *    2017/04/06, yangy, V1.0.0
 *        Initial version.
 *    2017/06/06, yangy, V1.1.0
 *        Add some security operations.(GM6621_20170605_1219_K7_RTC.bin)
 *    2019/03/23, yangy, V1.2.0
 *        Porting to ugelis OS on GM6721
 */
#include <ugelis.h>
#include <gm_hal_rtc.h>
#include <gm_hal_pes.h>
#include "rtc_test.h"
#include "gm_ll_rtc.h"
#include "gm_ll_pes.h"

/*-----------------------
 * Print macros
 *-----------------------*/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do {} while (0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{} while (0)
#else
    #define PR_WARNINR         printf
#endif

#define small_delay  __ISB

/* the count of all the RTC registers */
#define RTC_REGISTER_LENGTH 22

/* control the display of RTC calendar */
static uint8_t s_HardwareTimerDisplay = 0;
static UTCTime_T s_Hardtime;

/* control the display of software calendar */
static uint8_t s_SoftwareTimerDisplay = 0;
static UTCTime_T s_Softtime;

/*------------------------------------------------------------------
 *  the mask to control 5 differents wakeup freqency as followling:
 *  WP0_F freq = RTCCLK/2/(PRESCALE+1)/(2^(14-TDIVSEL))
 *  WP1_F freq = RTCCLK/2/(PRESCALE+1)/(2^(13-TDIVSEL))
 *  WP2_F freq = RTCCLK/2/(PRESCALE+1)/(2^(12-TDIVSEL))
 *  WP3_F freq = RTCCLK/2/(PRESCALE+1)/(2^(11-TDIVSEL))
 *  WP4_F freq = RTCCLK/2/(PRESCALE+1)/(WAKEUPINI+1)
 *------------------------------------------------------------------*/
static uint32_t s_WakeUpMaskDisplay = 0;
/* set wakeup to next frequency when $s_WakeUpCnt is matched with current frequency */
static uint32_t s_WakeUpCnt = 0;

/*--------------------------------------------------------------
 * 1 means date and time are stored in the registers in Bin mode
 * 0 means date and time are stored in the registers in BCD mode
 *--------------------------------------------------------------*/
static uint8_t sBinMode = 1;

/* global temporary rtc time struct */
static UTCTime_T s_rtc_time;

/* synchronize alarm test with ISR */
static uint8_t s_AlarmStep = 0;
/* $s_AlarmFailed will be set to a negtive number when alarm test failed */
static int s_AlarmFailed = 0;
/* alarm on after $CHECK_SECOND_COUNTER secs */
#define CHECK_SECOND_COUNTER 5
/* alarm is timeout when $SecondCounter reached 2*CHECK_SECOND_COUNTER */
static uint8_t s_SecondCounter = 0;

/* use the counter to avoid to many print */
#define BASETIMER_PES_TRIGGER_COUNT 10
static uint8_t s_basetimer_pes_trigger_counter = 0;

/* synchronize time irq */
static uint8_t s_TimeIrqStep = 0;
static uint32_t s_TimeIrqDisplay = 0;

static uint8_t *s_WeekName[7] =
{
    "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."
};

/*-------------------------------------------------------
 * the length indicates that how many test values will
 * be written to the registers.
 -------------------------------------------------------*/
#define TEST_VALUE_LENGTH 11
/* those value used for the test of writting register */
static uint32_t s_testValue[TEST_VALUE_LENGTH] =
{
    0x00000001,
    0x00000002,
    0x00000004,
    0x00000008,
    0xAAAA5555,
    0x5555AAAA,
    0xFFFF0000,
    0x0000FFFF,
    0xFFFFFFFF,
    0x1ACCE551,
    0x00010000,
};

/*   offset,        name,       rwType,     resetValue,   writeMask */
static RTC_Reg_T s_RTCRegs[RTC_REGISTER_LENGTH] =
{
    {RTC_TM0,       RTC_TM0_NAME,       RTC_RW_RW,  0x00000000, 0x007F7F7F},
    {RTC_TM1,       RTC_TM1_NAME,       RTC_RW_RW,  0x00002101, 0x3FFFFF3F},
    {RTC_CTRL,      RTC_CTRL_NAME,      RTC_RW_RW,  0x47400980, 0x000FE7F3},
    {RTC_ALRM,      RTC_ALARM_NAME,     RTC_RW_RW,  0x00000080, 0x7F7F7F7F},
    {RTC_ALRMASK,   RTC_ALRMASK_NAME,   RTC_RW_RW,  0x00000000, 0x000000FF},
    {RTC_TICKVAL,   RTC_TICKVAL_NAME,   RTC_RW_SKIP, 0x00003FFF, 0x007FFFFF},
    {RTC_SUBSEC,    RTC_SUBSEC_NAME,    RTC_RW_RW,  0x00000000, 0x00000000},
    {RTC_TIMSTAMP0, RTC_TIMSTAMP0_NAME, RTC_RW_RO,  0x00000000, 0x007F7F7F},
    {RTC_TIMSTAMP1, RTC_TIMSTAMP1_NAME, RTC_RW_RO,  0x00000000, 0xFFFFFF3F},
    {RTC_DCALCTL,   RTC_DCALCTL_NAME,   RTC_RW_SKIP, 0x00000000, 0x00001FFF},
    {RTC_FLAG,      RTC_FLAG_NAME,      RTC_RW_SKIP, 0x00000000, 0x00000000},
    {RTC_INTEN,     RTC_INTEN_NAME,     RTC_RW_RW,  0x00000000, 0x00001FFF},
    {RTC_INTCLR,    RTC_INTCLR_NAME,    RTC_RW_WO,  0x00000000, 0x00000000},
    {RTC_TAMPER1,   RTC_TAMPER1_NAME,   RTC_RW_RW,  0x00000000, 0x000007FF},
    {RTC_TAMPER2,   RTC_TAMPER2_NAME,   RTC_RW_RW,  0x00000000, 0x000007FF},
    /* {RTC_TAMPER3 ,RTC_TAMPER3_NAME,  RTC_RW_RW,  0x00000000, 0x000007FF}, */
    {RTC_PROTECT,   RTC_PROTECT_NAME,   RTC_RW_WO,  0x1ACCE551, 0x00000000},
    {RTC_ALASSEC,   RTC_ALASSEC_NAME,   RTC_RW_RW,  0x00000000, 0x0000FFFF},
    {RTC_WAKEUPT0,  RTC_WAKEUPT0_NAME,  RTC_RW_SKIP, 0x00000000, 0x0000FFFF},
    {RTC_WAKEUPT1,  RTC_WAKEUPT1_NAME,  RTC_RW_SKIP, 0x00000000, 0x0000FFFF},
    {RTC_PES,       RTC_PES_NAME,       RTC_RW_RW,  0x00000000, 0x000000F1},
    {RTC_SPECIAL,   RTC_SPECIAL_NAME,   RTC_RW_SKIP, 0x000C7B00, 0x0000000F},
    {RTC_VER,       RTC_VER_NAME,       RTC_RW_RO,  0x04050200, 0x00000000},
};

void RTCIrq_Wakeup_Enable(void);
void RTCIrq_Wakeup_Disable(void);
void RTCIrq_Tamper_Disable(void);
void RTCIrq_Alarm_Enable(void);
void RTCIrq_Second_Disable(void);

void HAL_RTC_SetBinMode(bool bin_mode);
void HAL_RTC_WaitReady(void);
void HAL_RTC_SetTimeEx(uint8_t hour, uint8_t minute, uint8_t second, uint8_t pm, uint8_t format12);
void HAL_RTC_GetTimeEx(UTCTime_T *pTime);
uint32_t HAL_RTC_Get1sTicks();
uint32_t HAL_RTC_GetPrescaler();
void HAL_RTC_IrqTamper_Enable(void);
uint32_t HAL_RTC_GetClockFreq();
void HAL_RTC_GetTimeStamp(UTCTime_T *pTime);

void RTC_WaitSecond(uint32_t sec) /* @unused */
{
    while (sec--)
    {
        while (IO_BIT_GET(RTC->FLAG, IRQ_SECOND_EN) == 0);
        IO_BIT_SET(RTC->INTCLR, IRQ_SECOND_EN);
    }
}

/* CK_CHG
 * 1:the rising edge of RTCCLK have been dectected
 * 0: the rising edge of RTCCLK havenot been dectected yet
 * Write '0' to this bit will clear it to '0'
 * Note: this flag is working when the RTCCLK is 32.768K,
 * and the system clock is working at more than 4MHz
 */
uint32_t RTC_RisingEdgeOn(void) /* @unused */
{
    return IO_BIT_GET(RTC->CTRL, CK_CHG);
}

/* WRDY_ERR
 * When the WRDY flag is '0',and you write the RTCTM0/ RTCTM1/ RTCSUBSEC/RTCINTEN/RTCSPECIAL register this moment, the WRDY_ERR
 * Will be set to '1'.
 * Write '0' to this bit will clear it to '0'
 */
uint32_t RTC_WriteReadyError(void) /* @unused */
{
    return IO_BIT_GET(RTC->CTRL, WRDY_ERR);
}


void RTC_TriggerPESEnable(void)
{
    HAL_RTC_Unlock();

    IO_BIT_CLEAR(PES_DEV->CHNCFG[0], (0xf << 0)); /* clear [3:0] */
    IO_BIT_SET(PES_DEV->CHNCFG[0], (7 << 0));   /* select channel 7 RTCC */

    IO_BIT_CLEAR(RTC->CTRL, EVSEL_EN);
    while (!(IO_BIT_GET(RTC->SPECIAL, EVSEL_WEF)));
    IO_BITMASK_SET(RTC->CTRL, EV_SEL_MASK, EV_SEL_WAKEUP4);
    IO_BIT_SET(RTC->CTRL, EVSEL_EN);

    HAL_RTC_Lock();
}

void RTC_TriggerPESDisable(void)
{
    HAL_RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, EVSEL_EN);
    HAL_RTC_Lock();
}

void RTC_WakeUpEnable(void) /* @unused */
{
    IO_BIT_SET(RTC->CTRL, WAKEUP_EN);
}

void RTC_WakeUpDisable(void) /* @unused */
{
    IO_BIT_CLEAR(RTC->CTRL, WAKEUP_EN);
}

/* WEF
 *
 * When the RTCCTRL[XXX_EN] bit to be ' 0',then after about two RTCCLK,
 * the bit RTCSPECIAL[XXX_WEF] will be change to '1',
 * And when the RTCCTRL[EVSEL_EN] bit to be '1', then after about two RTCCLK,
 * the bit RTCSPECIAL[XXX_WEF] will be change to '0'.
 */

/* 1: can write the RTCCTRL[EV_SEL] field safely */
uint32_t RTC_CanWriteEVSEL(void)
{
    return IO_BIT_GET(RTC->SPECIAL, EVSEL_WEF);
}

/* 1: can write the RTCCTRL[ACMP_SEL] field safely */
uint32_t RTC_CanWriteACMPSEL(void) /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, ACMPTRI_WEF);
}

/* 1: can write the RTCWAKEUPT0/ RTCWAKEUPT1 register safely */
uint32_t RTC_CanWriteWAKEUP(void) /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, WAKEUP_WEF);
}

uint32_t RTC_CanWriteALARM() /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, ALARM_WEF);
}

uint32_t RTC_CanWriteTAMP1(void) /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, TAMP1_WEF);
}

uint32_t RTC_CanWriteTAMP2(void) /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, TAMP2_WEF);
}

/* 1: can write the RTCPES[RTCEV_SEL] field safely */
uint32_t RTC_CanWritePESEVSEL(void) /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, PES_WEF);
}

/*
 * When the RTCCTRL[HOLD] bit to be ' 0',then after about two RTCCLK, this bit will be change to '0',.
 * And when the RTCCTRL[HOLD] bit to be '1',then after about two RTCCLK, this bit will be change to '1'
 */
uint32_t RTC_CanWriteTICKVAL(void) /* @unused */
{
    return IO_BIT_GET(RTC->SPECIAL, TICK_WEF);
}

/**
* @brief Get the wdayday on the day y/m/d
* @param y - year
* @param m - mon
* @param d - day
* @return wdayday from 1 to 7
* @note
*/
uint32_t WeekDay_Calculate(uint32_t y, uint32_t m, uint32_t d)
{
    uint32_t wday = 0;

    if (m == 1 || m == 2)
    {
        m += 12;
        y--;
    }

    wday = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

    return (wday + 1); /* from 1 to 7 */
}


/**
* @brief Set global storage mode
* @param 1: bin mode; 0: bcd mode
* @return
* @note
*/
void BinMode_Set(uint8_t bin_mode)
{

    PR_DEBUG("\n###########\n");
    if (bin_mode == 1)
    {
        PR_DEBUG("Bin mode test\n");
    }
    else
    {
        PR_DEBUG("Bcd mode test\n");
    }
    PR_DEBUG("###########\n\n");
    HAL_RTC_SetBinMode(bin_mode);
    sBinMode = bin_mode;
}

int BinMode_Get(void)
{
    if (IO_BIT_GET(RTC->CTRL, BIN_BCD_MODE))
    {
        return 0; //bcd mode
    }
    return 1; //bin mode
}

/**
* @brief Get the days' count on the mon of
*           one common year or leap year
* @param 1-leap year;0-common year
* @return the day from 28 to 31
* @note
*/
uint8_t DaysCount_Get(uint8_t leapyear, uint8_t mon)
{
    if (mon > 7)
    {
        return (mon % 2 == 0) ? 31 : 30;
    }
    else if (mon != 2)
    {
        return (mon % 2 == 0) ? 30 : 31;
    }
    else
    {
        return leapyear ? 29 : 28;
    }
}


/**
* @brief Set tickval and prescale
* @param max value of prescale is 0x7F
* @param max value of tickvalue is 0xFFFF
* @return
* @note 1s can be generated by
*       RTCCLK / 2 / (PRE_SCALE+1) / (TICKVAL + 1),
*/
void TickValue_Set(uint32_t prescale, uint32_t tickvalue)
{
    IO_BITMASK_SET(RTC->TICKVAL, PRESCALE_MASK,  prescale << PRESCALE_POS);
    IO_BITMASK_SET(RTC->TICKVAL, TICKVALUE_MASK, tickvalue << TICKVAULE_POS);
}

/**
* @brief Get RTC hour in bin mode or bcd mode
* @param
* @return the hour
* @note
*/
uint32_t Hour_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM0, BIN_HOUR_MASK) >> BIN_HOUR_POS;
    }
    else
    {
        return (BCD2BIN(BCD_HOUR_GET()));
    }
}

/**
* @brief Get RTC min in bin mode or bcd mode
* @param
* @return the min
* @note
*/
uint32_t Minute_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM0, BIN_MINUTE_MASK) >> BIN_MINUTE_POS;
    }
    else
    {
        return (BCD2BIN(BCD_MINUTE_GET()));
    }
}

/**
* @brief Get RTC sec in bin mode or bcd mode
* @param
* @return the sec
* @note
*/
uint32_t Second_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM0, BIN_SECOND_MASK) >> BIN_SECOND_POS;
    }
    else
    {
        return (BCD2BIN(BCD_SECOND_GET()));
    }
}

/**
* @brief Get RTC am/pm mode in bin mode or bcd mode
* @param
* @return 0 - am or 24-hours format, 1 - pm
* @note
*/
uint32_t AMPM_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM0, BIN_AMPM_MASK) >> BIN_AMPM_POS;
    }
    else
    {
        return (BCD2BIN(BCD_AMPM_GET()));
    }
}

/**
* @brief Get RTC wdayday in bin mode or bcd mode
* @param
* @return wdayday from 1 to 7
* @note
*/
uint32_t Weekday_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM1, BIN_WEEK_MASK) >> BIN_WEEK_POS;
    }
    else
    {
        return (BCD2BIN(BCD_WEEK_GET()));
    }
}

/**
* @brief Set RTC wdayday in bin mode or bcd mode
* @param wdayday from 1 to 7
* @return
* @note
*/
void Weekday_Set(uint32_t wday)
{
    uint32_t bcd_wday = 0;
    HAL_RTC_WaitReady();
    if (sBinMode)
    {
        IO_BITMASK_SET(RTC->TM1, BIN_WEEK_MASK, wday << BIN_WEEK_POS);
    }
    else
    {
        bcd_wday = BIN2BCD(wday);
        IO_BITMASK_SET(RTC->TM1, BIN_WEEK_MASK, BCD_WEEK(bcd_wday) << BIN_WEEK_POS);
    }
}

/**
* @brief Get RTC subsec
* @param
* @return
* @note
*/
uint32_t SubSecond_Get(void)
{
    return IO_BITS_GET(RTC->SUBSEC, BIN_SUBSECOND_MASK) >> BIN_SUBSECOND_POS;
}

/**
* @brief Set RTC year in bin mode or bcd mode
* @param year from 1 to 4000
* @return
* @note
*/
void Year_Set(uint32_t year)
{
    uint32_t bcd_year_high = 0;
    uint32_t bcd_year_low  = 0;
    HAL_RTC_WaitReady();
    if (sBinMode)
    {
        IO_BITMASK_SET(RTC->TM1, BIN_YEAR_MASK, year << BIN_YEAR_POS);
    }
    else
    {
        bcd_year_high = BIN2BCD(year / 100);
        bcd_year_low  = BIN2BCD(year % 100);
        IO_BITMASK_SET(RTC->TM1, BIN_YEAR_MASK, BCD_YEAR(bcd_year_high, bcd_year_low) << BIN_YEAR_POS);
    }
}

/**
* @brief Get RTC year in bin mode or bcd mode
* @param
* @return the year from 1 to 4000
* @note
*/
uint32_t RTCYear_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM1, BIN_YEAR_MASK) >> BIN_YEAR_POS;
    }
    else
    {
        return (BCD2BIN(BCD_YEAR_HIGH_GET()) * 100 + BCD2BIN(BCD_YEAR_LOW_GET()));
    }
}

/**
* @brief Set RTC mon in bin mode or bcd mode
* @param mon from 1 to 12
* @return
* @note
*/
void Month_Set(uint32_t mon)
{
    uint32_t bcd_mon = 0;
    HAL_RTC_WaitReady();
    if (sBinMode)
    {
        IO_BITMASK_SET(RTC->TM1, BIN_MONTH_MASK, mon << BIN_MONTH_POS);
    }
    else
    {
        bcd_mon = BIN2BCD(mon);
        printf("%x %x %x\n", RTC->TM1,  BCD_MONTH(bcd_mon), BCD_MONTH(bcd_mon) << BIN_MONTH_POS);
        IO_BITMASK_SET(RTC->TM1, BIN_MONTH_MASK, BCD_MONTH(bcd_mon) << BIN_MONTH_POS);
        IO_BITMASK_SET(RTC->TM1, (0x1f << 8), 0x12);
    }
}

/**
* @brief Get RTC mon in bin mode or bcd mode
* @param
* @return the mon from 1 to 12
* @note
*/
uint32_t Month_Get(void)
{
    HAL_RTC_WaitReady();
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM1, BIN_MONTH_MASK) >> BIN_MONTH_POS;
    }
    else
    {
        return (BCD2BIN(BCD_MONTH_GET()));
    }
}

/**
* @brief Set RTC day in bin mode or bcd mode
* @param mon from 1 to 31
* @return
* @note
*/
void Day_Set(uint32_t day)
{
    uint32_t bcd_day = 0;
    HAL_RTC_WaitReady();
    if (sBinMode)
    {
        IO_BITMASK_SET(RTC->TM1, BIN_DAY_MASK, day << BIN_DAY_POS);
    }
    else
    {
        bcd_day = BIN2BCD(day);
        IO_BITMASK_SET(RTC->TM1, BIN_DAY_MASK, BCD_MONTH(bcd_day) << BIN_DAY_POS);
    }
}

/**
* @brief Get RTC day in bin mode or bcd mode
* @param
* @return the day from 1 to 31
* @note
*/
uint32_t Day_Get(void)
{
    if (sBinMode)
    {
        return IO_BITS_GET(RTC->TM1, BIN_DAY_MASK) >> BIN_DAY_POS;
    }
    else
    {
        return (BCD2BIN(BCD_DAY_GET()));
    }
}

/**
* @brief Get RTC date in bin mode or bcd mode
* @param date_time_type: DATE_TIME_TIMESTAMP/DATE_TIME_RTCTIME
* @return
* @note
*/
void RTC_Date_All_Get(UTCTime_T *time, DATE_TIME_TYPE date_time_type)
{
    uint32_t date_value = 0;
    if (date_time_type == DATE_TIME_TIMESTAMP)
    {
        date_value = IO_BITS_GET(RTC->TIMSTAMP1, MASK_ALL);
    }
    else
    {
        date_value = IO_BITS_GET(RTC->TM1, MASK_ALL);
    }
    if (sBinMode)
    {
        time->year      = IO_BITS_GET(date_value, BIN_YEAR_MASK) >> BIN_YEAR_POS;
        time->mon     = IO_BITS_GET(date_value, BIN_MONTH_MASK) >> BIN_MONTH_POS;
        time->day       = IO_BITS_GET(date_value, BIN_DAY_MASK) >> BIN_DAY_POS;
        time->wday      = IO_BITS_GET(date_value, BIN_WEEK_MASK) >> BIN_WEEK_POS;
    }
    else
    {
        time->year      = BCD2BIN(BCD_YEAR_HIGH_GET_VALUE(date_value)) * 100 +
                          BCD2BIN(BCD_YEAR_LOW_GET_VALUE(date_value));
        time->mon     = BCD2BIN(BCD_MONTH_GET_VALUE(date_value));
        time->day       = BCD2BIN(BCD_DAY_GET_VALUE(date_value));
        time->wday      = BCD2BIN(BCD_WEEK_GET_VALUE(date_value));
    }
    if (date_time_type == DATE_TIME_TIMESTAMP)      /* timestamp has no year bits */
    {
        time->year = 0;
    }
}

/**
* @brief Set alarm with the format day(wd), hour:min:sec, pm, fmt12
* @param pm: 1-pm, 0-am or 24-hours format
*        fmt12: 1 - 12-hours format, 0 - 24-hours format
*        wd:  0 - the DAY is the day of mon, 1 - the DAY[3:0] is the wday day
* @return
* @note
*/
void Alarm_Set(uint8_t wd, uint32_t day, uint8_t hour, uint8_t min, uint8_t sec, uint8_t pm, uint8_t fmt12)
{
    if (sBinMode)
    {
        if (!fmt12)   /* 24-hour format */
        {
            pm = 0;
        }
        IO_BITMASK_SET(RTC->ALARM, MASK_ALL, ALARM_BIN_TIME(wd, day, hour, min, sec, pm));
    }
    else
    {
        IO_BITMASK_SET(RTC->ALARM, MASK_ALL, ALARM_BCD_TIME(BIN2BCD(wd), BIN2BCD(day), BIN2BCD(hour), BIN2BCD(min),
                       BIN2BCD(sec), pm));
    }
    IO_BITMASK_SET(RTC->CTRL,  BIN_FORMAT12_MASK, fmt12 << BIN_FORMAT12_POS);
}

/**
* @brief check if the leapyear flag is on
* @param
* @return 1 - the year of RTC is leapyear
*         0 - the year of RTC is common year
* @note
*/
uint8_t LeapYear_Flag_On(void)
{
    HAL_RTC_WaitReady();
    return ((IO_BIT_GET(RTC->SPECIAL, LEAP_FLAG)&LEAP_FLAG) != 0);
}

/**
* @brief print UTCTime_T in format
*       year-mon-day wdayday hour:min:sec:subsec am(pm)
* @param
* @return
* @note
*/
void Time_Print(UTCTime_T *rtc_time)
{
    PR_DEBUG("%04d-%02d-%02d %s\t%02d:%02d:%02d:%05d %s\n", rtc_time->year, rtc_time->mon,
             rtc_time->day, s_WeekName[rtc_time->wday - 1], rtc_time->hour, rtc_time->min,
             rtc_time->sec, rtc_time->subsec, rtc_time->fmt12 ? "pm" : "am");
}


/**
* @brief do the test if the RTC mode can judge one year is leapyear correctly.
*       step 1, test the year from TEST_YEAR_START to TEST_YEAR_END is leapyear.
*       step 2, test the days' count on every mon of the year is correct.
*       for example, calculate the days'count on February in 2017.
*       the time is set to 2017:2:28 23:59:59 24-hours format.
*       after a few cycles the time should be changed to
*       2017:3:1 00:00:00 24-hours format if errorless.
* @param
* @return negative value - failed to test leapyear
* @note
*/
int LeapYear_Test(void)
{
    int ret = 0;

    uint32_t i = 0;
    uint32_t j = 0;

    uint8_t bRegLeapYearFlag = 0;
    uint8_t bLeapYear = 0;

    uint32_t new_day = 0;
    uint32_t new_year = 0;
    uint32_t new_mon = 0;
    uint32_t day = 0;
    uint8_t fmt12 = 0;

    UTCTime_T rtc_time;

#define TEST_YEAR_START         0
#define TEST_YEAR_END           4000

    /*
     * check days' count of one year from
     * CHECK_LEAPYEAR_CEIL to CHECK_LEAPYEAR_FLOOR
     * ignore the other years beyond the scope.
     */
#define CHECK_LEAPYEAR_CEIL     3000
#define CHECK_LEAPYEAR_FLOOR    2000

    PR_DEBUG("test year from %d to %d, please wait for a sec.\n", TEST_YEAR_START, TEST_YEAR_END);
    HAL_RTC_Unlock();

    for (i = TEST_YEAR_START; i < TEST_YEAR_END; i++)
    {
        HAL_RTC_SetDate(i, 1, 1);

        bLeapYear = ((!(i % 4) && (i % 100)) || !(i % 400));
        bRegLeapYearFlag = LeapYear_Flag_On();
        if (bRegLeapYearFlag != bLeapYear)
        {
            PR_DEBUG("year %d is %s leap year, but reg CR's flag is %d\n", i,
                     bLeapYear ? "" : "not", !bLeapYear);
            ret = ERR_DATE;
        }

        if (i < CHECK_LEAPYEAR_FLOOR || i > CHECK_LEAPYEAR_CEIL)
        {
            continue;
        }
        printf("i = %d\n", i);
        for (j = 1; j < 13; j++)
        {
            HAL_RTC_SetDate(i, j, 1);
            day = DaysCount_Get(bLeapYear, j);
            HAL_RTC_SetDate(i, j, day);
            //Weekday_Set(WeekDay_Calculate(i, j, day));
            fmt12 = 0;

            /* test in fmt12 or format24 mode alternately */
            if (j % 2 == 0)
            {
                fmt12 = 1;
            }

            /*
             * set the time in the format as follows to pass one day very soon
             * 11:59:59:TICKVALUE-1 pm 12-hours format
             * 23:59:59:TICKVALUE-1 am 24-hours format
             */
            HAL_RTC_Unlock();
            HAL_RTC_SetTimeEx(11 + 12 * (1 - fmt12), 59, 59, fmt12, fmt12);
            HAL_RTC_Lock();

            HAL_RTC_GetTimeEx(&rtc_time);

            while (SubSecond_Get() >= (HAL_RTC_Get1sTicks() - 1))
            {
                small_delay();
            }

            HAL_RTC_WaitReady();

            /* one day after */
            HAL_RTC_GetTimeEx(&rtc_time);

            new_mon = rtc_time.mon;

            if (((j + 12) % 12 + 1) != new_mon)
            {
                PR_DEBUG("mon error!");
                Time_Print(&rtc_time);
                PR_DEBUG("\n");
                ret = ERR_DATE;
            }
            else
            {
                new_year = rtc_time.year;
                /* what the year will be if larger than 4000? */
                if ((j == 12) && (((i + TEST_YEAR_END) % TEST_YEAR_END + 1) != new_year))
                {
                    PR_DEBUG("year error!\n");
                    ret = ERR_DATE;
                }
                else
                {
                    new_day = rtc_time.day;
                    if (1 != new_day)
                    {
                        PR_DEBUG("day error!\n");
                        ret = ERR_DATE;
                    }
                }
            }
        }
    }
    HAL_RTC_Lock();
    return ret;
}

/**
* @brief leapyear test with bin mode and bcd mode
* @param
* @return negative value - failed to test leapyear
* @note
*/
int RTCLeapYear_Test(void)
{
    int ret = 0;
    PR_DEBUG("\n\n***Leap Year Test Start***\n");
    PR_DEBUG("__________________________________________________\n");

    BinMode_Set(0);
    ret = LeapYear_Test();

    BinMode_Set(1);
    ret = LeapYear_Test();

    if (ret == 0)
    {
        PR_DEBUG("Leap Year Test passed !!!\n");
    }
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Leap Year Test End***\n");

    return ret;
}

/**
* @brief add or substract one hour of the RTC time immediately.
*       always be use to summer time or winter time adjust.
* @param
* @return negative value - failed.
* @note
*/
int RTCAddAndSub_Test(void)
{
    int ret = 0;
    int i = 0;
    UTCTime_T rtc_time;

    uint32_t origHour = 0;
    int hours_cnt = 0;

    PR_DEBUG("\n\n***Add And Sub One Hour Test Start***\n");
    PR_DEBUG("__________________________________________________\n");
    HAL_RTC_GetTimeEx(&rtc_time);
    Time_Print(&rtc_time);

    hours_cnt = (2 - rtc_time.fmt12) * 12;
    origHour  = rtc_time.hour;

    HAL_RTC_Unlock();

    PR_DEBUG("Substract 1 hour for winter time.\n");
    while (i++ < hours_cnt + 1)
    {
        HAL_RTC_WaitReady();
        IO_BIT_SET(RTC->SPECIAL, SUB_1HOUR);
        HAL_RTC_GetTimeEx(&rtc_time);
        Time_Print(&rtc_time);
        if ((((origHour - 1 + hours_cnt) % hours_cnt) != rtc_time.hour) &&
                (rtc_time.fmt12 && rtc_time.hour != 12 && origHour != 1))
        {
            ret = ERR_ADDSUB;
            PR_DEBUG("Substract 1 hour failed.\n\n");
        }
        origHour = rtc_time.hour;
    }
    PR_DEBUG("After substract 1 hour for winter time.\n\n");

    origHour  = rtc_time.hour;
    PR_DEBUG("Add 1 hour for summer time.\n");
    i = 0;
    while (i++ < hours_cnt + 1)
    {
        HAL_RTC_WaitReady();
        IO_BIT_SET(RTC->SPECIAL, ADD_1HOUR);
        HAL_RTC_GetTimeEx(&rtc_time);
        Time_Print(&rtc_time);
        if ((((rtc_time.hour - 1 + hours_cnt) % hours_cnt) != origHour) &&
                (rtc_time.fmt12 && rtc_time.hour != 1 && origHour != 12))
        {
            ret = ERR_ADDSUB;
            PR_DEBUG("After 1 hour failed.\n\n");
        }
        origHour = rtc_time.hour;
    }
    PR_DEBUG("After add 1 hour for summer time.\n");

    HAL_RTC_Lock();

    if (ret == 0)
    {
        PR_DEBUG("Add And Sub One Hour Test  passed.\n");
    }
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Add And Sub One Hour Test End***\n");
    return ret;
}


/* ---------------------------------------------------------------
 * Wake up timer repeat to generate the flag and rtc event output
 * --------------------------------------------------------------- */

/**
* @brief Set wakeup initial cnt,  WAKEUPCNT and WAKEUPINI
* @param
* @return
* @note The 16bit wakeup timer counter, when the WTR1-WAKEUPINI isn't 0 and
*   WTR0-WAKEUPCNT isn't 0, the counter will decrease 1 at the clock of RTCCLK/2,
*   until it decrease to 0, then the wakeup interrupt will happen and
*   the WAKEUPINI will reload to the WAKEUPCNT, and repeat to decreasin.
*   Decrease frequency = RTCCLK/2/(PRESCALE+1)/(2^(14-TDIVSEL)
*   RTCCTRL[10:8]
*   0: 2^14
*   1: 2^13
*   2: 2^12
*   ...
*   7: 2^7
*/
void WakeUp_Start(uint32_t wakeup_cnt)
{
    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->WAKEUPT0, 0xFFFF, wakeup_cnt);
    IO_BITMASK_SET(RTC->WAKEUPT1, 0xFFFF, wakeup_cnt);
    HAL_RTC_Lock();
}

/**
* @brief Clear wakeup cnt
* @param
* @return
* @note
*/
void WakeUp_End(void)
{
    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->WAKEUPT0, 0xFFFF, 0);
    IO_BITMASK_SET(RTC->WAKEUPT1, 0xFFFF, 0);
    HAL_RTC_Lock();
}

/**
* @brief Start wakeup event test
* @param
* @return negative value: failed
* @note
*   WP0_F freq = RTCCLK/2/(PRESCALE+1)/(2^(14-TDIVSEL))
*   WP1_F freq = RTCCLK/2/(PRESCALE+1)/(2^(13-TDIVSEL))
*   WP2_F freq = RTCCLK/2/(PRESCALE+1)/(2^(12-TDIVSEL))
*   WP3_F freq = RTCCLK/2/(PRESCALE+1)/(2^(11-TDIVSEL))
*   WP4_F freq = RTCCLK/2/(PRESCALE+1)/(WAKEUPINI+1)
*/
int RTCWakeup_Test(void)
{
    PR_DEBUG("\n\n***Wakeup Test Start ***\n");
    PR_DEBUG("__________________________________________________\n");

    s_WakeUpMaskDisplay = IRQ_WAKEUP_WP0F_EN;
    s_WakeUpCnt = 0;
    WakeUp_Start(HAL_RTC_Get1sTicks());
    RTCIrq_Wakeup_Enable();

    while (s_WakeUpMaskDisplay)
    {
        small_delay();
    }
    WakeUp_End();
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Wakeup Test End***\n");
    return 0;
}

/**
* @brief Hold the timer.
* @param hold 1 - hold
*             0 - release
* @return negative value: failed
* @note
*/
void Time_Hold(uint8_t hold)
{
    HAL_RTC_Unlock();
    if (hold)
    {
        IO_BIT_SET(RTC->CTRL, TIMER_HOLD_MASK);
    }
    else
    {
        IO_BIT_CLEAR(RTC->CTRL, TIMER_HOLD_MASK);
    }
    HAL_RTC_Lock();
}


/**
* @brief Hold the timer for several secs, that RTC time will not run.
* @param
* @return negative value: failed
* @note
*/
int RTCTimerHold_Test(void)
{
    int ret = 0;
    int cnt = 20;
    UTCTime_T time;
    UTCTime_T origTime;

    PR_DEBUG("\n\n***Timer Hold Start ***\n");
    PR_DEBUG("__________________________________________________\n");
    Time_Hold(1);
    PR_DEBUG("\n  hold the timer\n");
    HAL_RTC_GetTimeEx(&origTime);
    while (cnt--)
    {
        HAL_RTC_GetTimeEx(&time);
        Time_Print(&time);
        if (!(origTime.year == time.year) &&
                (origTime.mon == time.mon) &&
                (origTime.day == time.day) &&
                (origTime.hour == time.hour) &&
                (origTime.min == time.min) &&
                (origTime.sec == time.sec) &&
                (origTime.subsec == time.subsec))
        {
            PR_DEBUG("failed to hold timer\n");
            ret = ERR_HOLD;
        }
        small_delay();
    }
    PR_DEBUG("\n  run the timer \n");
    HAL_RTC_GetTimeEx(&origTime);
    Time_Hold(0);
    cnt = 20;
    while (cnt--)
    {
        HAL_RTC_GetTimeEx(&time);
        Time_Print(&time);
        if (cnt == 10)
        {
            if (((time.sec == origTime.sec) && (time.subsec <= origTime.subsec)) ||
                    ((time.sec < origTime.sec) && (time.sec != 0) && (origTime.sec != 59)))
            {
                PR_DEBUG("failed to run timer\n");
                ret = ERR_HOLD;
            }
        }
        small_delay();
    }
    if (ret == 0)
    {
        PR_DEBUG("Timer hold passed\n");
    }
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Timer Hold End***\n");
    return ret;
}

/**
* @brief Prescale the timer in factor 1,2,4,8,...,128.
* @param
* @return negative value: failed
* @note
*/
int RTCPreScale_Test(void)
{
    int cnt = 10;
    int interval_subsec = 0;
    int interval_sec = 0;
    int last_subsec = 0;
    int last_sec = 0;
    UTCTime_T time;

    int i = 0;
    int prescale = 1;

    PR_DEBUG("\n\n***Prescale Test Start ***\n");
    PR_DEBUG("__________________________________________________\n");

    HAL_RTC_Unlock();

    /* 1sec = 31249*subsec (8000K / 2 / (PRESCALE+1) / (TICKS_PER_1S+1)) */
    do
    {
        TickValue_Set(((prescale << i) - 1), HAL_RTC_Get1sTicks());
        PR_DEBUG("\n###prescale ----- %d###\n", (prescale << i));
        HAL_RTC_GetTimeEx(&time);
        last_subsec = time.subsec;
        last_sec    = time.sec;
        while (cnt--)
        {
            HAL_RTC_GetTimeEx(&time);
            PR_DEBUG("\t");
            Time_Print(&time);
            interval_subsec = ((time.subsec - last_subsec + HAL_RTC_Get1sTicks()) % HAL_RTC_Get1sTicks());
            interval_sec    = ((time.sec - last_sec + 60) % 60);

            last_subsec = time.subsec;
            last_sec    = time.sec;
            PR_DEBUG("%02d:%05d\n", interval_sec, interval_subsec);
        }
        cnt = 10;
    } while (++i < 8);
    TickValue_Set(HAL_RTC_GetPrescaler(), HAL_RTC_Get1sTicks());
    HAL_RTC_Lock();

    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Prescale Test End ***\n");


    return 0;
}

/**
* @brief Two tampers test with different samples
* @param
* @return 0
* @note
*/

int RTCTamper_Test(void)
{
    HAL_RTC_IrqTamper_Enable();

    HAL_RTC_Unlock();

    //TAMPER1 0x400002C4[13:12] = 0x1 0x400002F0[27:24] = 0x0
    //TAMPER2 0x400002C4[11:10] = 0x1 0x400002F0[23:20] = 0x0
    //XPA6
    IO_BITMASK_SET(TAMPER_XP_MOD_SEL, 0x3 << 12, 0x1 << 12);
    IO_BITMASK_SET(TAMPER_XP_AF_SEL,  0xf << 24, 0x0 << 24);
    //XPA5
    IO_BITMASK_SET(TAMPER_XP_MOD_SEL, 0x3 << 10, 0x1 << 10);
    IO_BITMASK_SET(TAMPER_XP_AF_SEL,  0xf << 20, 0x0 << 20);

    /* tampflt :  2 ^ 0 consecutive samples,  more sensitive */
    /* tampfre :  input sample RTCLK/(2^1) */
    IO_BITMASK_SET(RTC->TAMPER1, MASK_ALL, ((0 << TAMPPUDIS_POS) | (3 << TAMPPRCH_POS) | (0 << TAMPFLT_POS) |
                                            (7 << TAMPFRE_POS) | (1 << TAMPTS) | (TAMPER_TRIGGER_LEVEL << TAMPTRG) | (1 << TAMPE)));
    /* tampflt :  2 ^ 3 consecutive samples */
    /* tampfre :  input sample RTCLK/(2^1) */
    IO_BITMASK_SET(RTC->TAMPER2, MASK_ALL, ((0 << TAMPPUDIS_POS) | (3 << TAMPPRCH_POS) | (3 << TAMPFLT_POS) |
                                            (7 << TAMPFRE_POS) | (1 << TAMPTS) | (TAMPER_TRIGGER_LEVEL << TAMPTRG) | (1 << TAMPE)));
    HAL_RTC_Lock();

    return 0;
}

int RTCPES_Test(void)
{
    return 0;
}

/**
* @brief Alarm test, which is concerned about
*       hour, min, sec, day, wdayday and subsec
* @param
* @return
* @note
*/
void Alarm_Test(void)
{
    uint32_t year   = 0;
    uint32_t mon    = 0;
    uint32_t day    = 0;
    uint32_t wday   = 0;
    uint8_t hour    = 0;
    uint8_t min     = 0;
    uint8_t sec     = 0;
    uint8_t fmt12   = 0;
    uint8_t step    = 0;
    uint8_t j       = 0;
    UTCTime_T alarm;
    s_HardwareTimerDisplay = 1;

    s_AlarmStep = step;

    RTCIrq_Alarm_Enable();

    /* set time 2017/04/12 (Wed) 14:23:25 */
    year    = 2017;
    mon     = 4;
    day     = 12;
    wday    = 3;
    hour    = 14;
    min     = 23;
    sec     = 20;
    fmt12   = 0;

    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    alarm.year  = year;
    alarm.mon   = mon;
    alarm.day   = day;
    alarm.wday  = wday;
    alarm.hour  = hour;
    alarm.min   = min;
    alarm.sec   = sec + CHECK_SECOND_COUNTER;
    alarm.fmt12 = fmt12;

    /* concern about sec     [* / * / * (*) * : * : sec] */
    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, (1 << ALARM_SEC_AE_POS));
    HAL_RTC_Lock();

    s_SecondCounter = 0;
    s_AlarmStep = ++step;
    PR_DEBUG("Set Alarm[%d]: concern about sec   */*/* (*) *:*:%d\n", s_AlarmStep, alarm.sec);

    step++;
    while (s_AlarmStep < step)
    {
        small_delay();
    }

    /* concern about min     [* / * / * (*) * : min : *] */
    hour = 14;
    min  = 14;
    sec  = 60 - CHECK_SECOND_COUNTER;

    alarm.hour  = hour;
    alarm.min   = min + 1;
    alarm.sec   = sec;

    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);
    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, (1 << ALARM_MIN_AE_POS));
    HAL_RTC_Lock();

    s_SecondCounter = 0;
    PR_DEBUG("Set Alarm[%d]: concern about min   */*/* (*) *:%d:*\n", s_AlarmStep, alarm.min);

    step++;
    while (s_AlarmStep < step)
    {
        small_delay();
    }

    /* concern about hour       [* / * / * (*) hour : * : *] */
    hour = 7;
    min  = 59;
    sec  = 60 - CHECK_SECOND_COUNTER;

    alarm.hour  = hour + 1;
    alarm.min   = min;
    alarm.sec   = sec;

    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);
    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, (1 << ALARM_HOUR_AE_POS));
    HAL_RTC_Lock();

    s_SecondCounter = 0;
    PR_DEBUG("Set Alarm[%d]: concern about hour     */*/* (*) %d:*:*\n", s_AlarmStep, alarm.hour);

    step++;
    while (s_AlarmStep < step)
    {
        small_delay();
    }

    /* concern about day        [* / * / day (*) * : * : *] */
    year    = 2017;
    mon     = 4;
    day     = 14;
    wday    = 5;
    hour    = 23;
    min     = 59;
    sec     = 60 - CHECK_SECOND_COUNTER;

    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    alarm.year  = year;
    alarm.mon   = mon;
    alarm.day   = day + 1;
    alarm.wday  = 0;
    alarm.hour  = hour;
    alarm.min   = min;
    alarm.sec   = sec;
    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, (1 << ALARM_DAY_AE_POS));
    HAL_RTC_Lock();

    s_SecondCounter = 0;
    PR_DEBUG("Set Alarm[%d]: concern about day      */*/%d (*) *:*:*\n", s_AlarmStep, alarm.day);

    step++;
    while (s_AlarmStep < step)
    {
        small_delay();
    }

    /* concern about wdayday    [* / * / * (wdayday) * : * : *] */
    year    = 2017;
    mon     = 4;
    day     = 11;
    wday    = 2;
    hour    = 23;
    min     = 59;
    sec     = 60 - CHECK_SECOND_COUNTER;
    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    alarm.year  = year;
    alarm.mon   = mon;
    alarm.day   = day;
    alarm.wday  = wday + 1;
    alarm.hour  = hour;
    alarm.min   = min;
    alarm.sec   = sec;
    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, (1 << ALARM_DAY_AE_POS));
    HAL_RTC_Lock();

    s_SecondCounter = 0;
    PR_DEBUG("Set Alarm[%d]: concern about wdayday      */*/* (%s) *:*:*\n", s_AlarmStep, s_WeekName[alarm.wday]);

    step++;
    while (s_AlarmStep < step)
    {
        small_delay();
    }

    /* concern about all        [* / * / day (*) hour : min : sec] */
    year    = 2017;
    mon     = 4;
    day     = 11;
    wday    = 0;
    hour    = 23;
    min     = 59;
    sec     = 60 - CHECK_SECOND_COUNTER;
    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    alarm.year  = year;
    alarm.mon   = mon;
    alarm.day   = day + 1;
    alarm.wday  = 0;
    alarm.hour  = 0;
    alarm.min   = 0;
    alarm.sec   = 3;
    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, ((1 << ALARM_DAY_AE_POS) | (1 << ALARM_HOUR_AE_POS) |
                                        (1 << ALARM_MIN_AE_POS) | (1 << ALARM_SEC_AE_POS)));
    HAL_RTC_Lock();

    s_SecondCounter = 0;
    PR_DEBUG("Set Alarm[%d]: concern about all      */*/%d (*) %d:%d:%d\n", s_AlarmStep, day + 1, alarm.hour, alarm.min, alarm.sec);

    step++;
    while (s_AlarmStep < step)
    {
        small_delay();
    }

    HAL_RTC_SetAlarm(&alarm);

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALASSEC, 0xFFFF, HAL_RTC_Get1sTicks());
    HAL_RTC_Lock();

    j = 0;
    do
    {
        HAL_RTC_Unlock();
        IO_BITMASK_SET(RTC->ALRMASK, 0xFF, (ALARM_SUBSEC_ALLBIT_CHECK - j) << ALARM_SUBSEC_AE_POS);
        HAL_RTC_Lock();

        PR_DEBUG("Set Alarm[%d]: concern about subsec      */*/* (*) *:*:* 0x%x\n", s_AlarmStep,
                 (HAL_RTC_Get1sTicks()) & ((uint32_t)(1 << (ALARM_SUBSEC_ALLBIT_CHECK - j)) - 1));
        step++;
        while (s_AlarmStep < step)
        {
            small_delay();
        }
    } while (j++ < 3); /* ignore too detailed subsecs */

    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, 0);
    HAL_RTC_Lock();

    RTCIrq_Second_Disable();

    s_AlarmStep = 0;
}

/**
* @brief Set RTC_OUT output
* @param
* @return negative value: failed
* @note
* RtcOut_Mode_DCALCTL_512Hz:
*           = RTCCLK/2/(PRESCALE+1)/(2^5)
*   (8Mhz Clk) freq         = 8000000/2/(127+1)/(2^5) = 976Hz
*   (32.768Khz Clk) freq    = 32768/2/(0+1)/(2^5) = 1Hz
* RtcOut_Mode_DCALCTL_1Hz:
*           =  RTCCLK/2/(PRESCALE+1)/(2^14)
*   (8Mhz Clk) freq         =  8000000/2/(127+1)/(2^14) = 1.90Hz
*   (32.768Khz Clk) freq    =  32768/2/(0+1)/(2^14) = 1Hz
*/
void RTCOut_Set(RTC_RtcOutMode_T rtcout_mode)
{
    HAL_RTC_Unlock();
    /* 0x4000015C[3:0]=5; */
    IO_BITMASK_SET(RTCOUT1, 0xf << 0, 0x5);
    /* 0x40000144[17:16]=1; */
    IO_BITMASK_SET(RTCOUT2, 0x3 << 16, 0x1 << 16);


    if (rtcout_mode == RtcOut_Mode_DCALCTL_512Hz)       /* cosel=1, coe=1 */
    {
        IO_BIT_SET(RTC->CTRL, RTCOUT_COE_EN);
        IO_BIT_SET(RTC->CTRL, RTCOUT_COSEL_EN);
    }
    else if (rtcout_mode == RtcOut_Mode_DCALCTL_1Hz)    /* cosel=0, coe=1 */
    {
        IO_BIT_SET(RTC->CTRL, RTCOUT_COE_EN);
        IO_BIT_CLEAR(RTC->CTRL, RTCOUT_COSEL_EN);
    }
    else if (rtcout_mode == RtcOut_Mode_ALARMOUT)       /* coe=0 */
    {
        IO_BIT_CLEAR(RTC->CTRL, RTCOUT_COE_EN);
    }
    HAL_RTC_Lock();
}

/**
* @brief Alarm test in bcd mode or bin mode.
* @param
* @return negative value: failed
* @note
*/
int RTCAlarm_Test(void)
{
    PR_DEBUG("\n\n***Alarm Test Start***\n");
    PR_DEBUG("__________________________________________________\n");

    RTCOut_Set(RtcOut_Mode_ALARMOUT);

    BinMode_Set(0);
    Alarm_Test();

    BinMode_Set(1);
    Alarm_Test();

    if (s_AlarmFailed == 0)
    {
        PR_DEBUG("Alarm test passed\n");
    }

    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Alarm Test End***\n");

    return s_AlarmFailed;
}

/**
* @brief Just set RTC_OUT to Calibration 512Hz(32.768Khz)
* @param
* @return negative value: failed
* @note
*/


/* tickval freq */
int RTCCalibration_Test(void)
{
    UTCTime_T rtc_time;
    UTCTime_T rtc_time_orig;
    uint32_t check_time = 45;

    //XPA6 output
    IO_BITMASK_SET(TAMPER_XP_MOD_SEL, 0x3 << 12, 0x1 << 12);
    IO_BITMASK_SET(TAMPER_XP_AF_SEL,  0xf << 24, 0x1 << 24);

    //PR_DEBUG("\n\n***Calibration Test Start***\n");
    //PR_DEBUG("__________________________________________________\n");

    RTCOut_Set(RtcOut_Mode_DCALCTL_512Hz);
    HAL_RTC_Unlock();
    IO_BITMASK_SET(RTC->DCALCTL, (0x7 << 9), (0x2 << 9));
    IO_BITMASK_SET(RTC->DCALCTL, (0x1 << 12), (0x1 << 12));
    IO_BITMASK_SET(RTC->DCALCTL, (0x1FF << 0), (128 << 0));
    HAL_RTC_Lock();
    HAL_RTC_GetTimeEx(&rtc_time_orig);
    //PR_DEBUG("Calibration will be done after %d secs.\n", check_time);
    do
    {
        while (!IO_BIT_GET(RTC->FLAG, IRQ_SECOND_EN))
        {
            ;
        }
        IO_BIT_SET(RTC->INTCLR, IRQ_SECOND_EN);
        HAL_RTC_GetTimeEx(&rtc_time);
    } while (((rtc_time.sec + 60 - rtc_time_orig.sec) % 60) < check_time);

    //PR_DEBUG("__________________________________________________\n");
    //PR_DEBUG("***Calibration Test End***\n");

    return 0;
}

/**
* @brief Test the protect of register
* @param
* @return negative value: failed
* @note
*/
int RTCRegisterProtect_Test(void)
{

    uint32_t new_value = 0;
    uint32_t orig_value = 0;
    uint32_t add_value = 0xF;
    int i = 0;
    int ret = 0;

    RTC_Reg_T *pRTCReg = NULL;
    __IO uint32_t *addr = NULL;

    PR_DEBUG("\n\n***Lock And Unlock Register Test Start***\n");
    PR_DEBUG("__________________________________________________\n");


    for (i = 0; i < RTC_REGISTER_LENGTH; i++)
    {
        add_value = 0x1;
        pRTCReg = &(s_RTCRegs[i]);

        if (pRTCReg->rwType == RTC_RW_SKIP)
        {
            continue;
        }

        addr = pRTCReg->addr;
        /* Ensure that register is locked */
        HAL_RTC_Lock();

        orig_value = ADDRESS_READ_UINT32(addr);
        HAL_RTC_WaitReady();
        ADDRESS_WRITE_UINT32(addr, orig_value + add_value);
        HAL_RTC_WaitReady();
        new_value = ADDRESS_READ_UINT32(addr);

        if (new_value != orig_value)
        {
            PR_DEBUG("failed to lock register %s (0x%x!=0x%x)\n", pRTCReg->name, new_value, orig_value);
            ret |= ERR_LOCK;
        }
        if (pRTCReg->writeMask == 0)
        {
            continue;
        }
        HAL_RTC_Unlock();
        orig_value = ADDRESS_READ_UINT32(addr);
        HAL_RTC_WaitReady();
        ADDRESS_WRITE_UINT32(addr, orig_value + add_value);
        HAL_RTC_WaitReady();
        new_value = ADDRESS_READ_UINT32(addr);

        if (new_value != (orig_value + add_value))
        {
            PR_DEBUG("failed to unlock register %s (0x%x!=0x%x)\n", pRTCReg->name, new_value, (orig_value + add_value));
            ret |= ERR_UNLOCK;
        }
    }
    HAL_RTC_Lock();

    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Lock And Unlock Register Test End***\n");

    return 0;
}

/**
* @brief Register Read/Write test
        1. check the resetValue.
        2. write a value to RW register, than check the feedback value
        3. write a value to RO register, than check the feedback value
* @param
* @return negative value: failed
* @note
*/
int RTCRegisterRW_Test(void)
{
    int ret = 0;
    int i = 0;
    int j = 0;
    int successful_written_cnt = 0;
    int incorrect_cnt = 0;
    uint32_t tmp_value = 0;
    uint32_t write_value = 0;
    RTC_Reg_T *pRTCReg = NULL;
    PR_DEBUG("\n\n***Read all RTC registers' ResetValue Start***\n");
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("Reg\tAddress\t\tCurrentValue\tResetValue\n");
    for (i = 0; i < RTC_REGISTER_LENGTH ; i++)
    {
        pRTCReg = &(s_RTCRegs[i]);

        /* Check resetValue */
        if (pRTCReg->resetValue != *(pRTCReg->addr))
        {
            incorrect_cnt++;
            PR_DEBUG("%s\t0x%x\t0x%08x\t0x%08x\n",
                     pRTCReg->name, (uint32_t)pRTCReg->addr, *(pRTCReg->addr), pRTCReg->resetValue);
        }
    }
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("There are %d incorrect items.\n", incorrect_cnt);
    PR_DEBUG("***Read all RTC registers End***\n\n");


    /* unlock the write asscess of register */
    HAL_RTC_Unlock();

    PR_DEBUG("\n\n***Write all RTC registers' ResetValue Start***\n");
    PR_DEBUG("__________________________________________________\n");
    for (i = 0; i < RTC_REGISTER_LENGTH ; i++)
    {
        pRTCReg = &(s_RTCRegs[i]);
        if (pRTCReg->rwType == RTC_RW_SKIP || pRTCReg->writeMask == 0)
        {
            continue;
        }
        /* register can be written. */
        if (pRTCReg->rwType == RTC_RW_RW)
        {
            for (j = 0; j < TEST_VALUE_LENGTH; j++)
            {
                write_value = s_testValue[j] & pRTCReg->writeMask;
                HAL_RTC_WaitReady();
                *(pRTCReg->addr) = write_value;
                HAL_RTC_WaitReady();
                tmp_value = *(pRTCReg->addr);
                if ((tmp_value & pRTCReg->writeMask) != write_value)
                {
                    PR_DEBUG("write 0x%08x to %s error, current value is 0x%08x.\n",
                             write_value, pRTCReg->name, tmp_value);
                    ret |= ERR_WRITE;
                }
            }
        }
        else if (pRTCReg->rwType == RTC_RW_RO)    /* read only */
        {
            successful_written_cnt = 0;
            for (j = 0; j < TEST_VALUE_LENGTH; j++)
            {
                write_value = s_testValue[j] & pRTCReg->writeMask;
                HAL_RTC_WaitReady();
                *(pRTCReg->addr) = write_value;
                tmp_value = *(pRTCReg->addr);
                if (tmp_value == write_value)
                {
                    successful_written_cnt++;
                }
            }
            if (successful_written_cnt == TEST_VALUE_LENGTH)
            {
                PR_DEBUG("error: readonly register %s can be written\n",
                         pRTCReg->name);
                ret |= ERR_WRITE;
            }
        }
    }
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("***Write all RTC registers End***\n\n");

    HAL_RTC_Lock();
    return ret;
}

/**
* @brief increase sec of software calendar in sec ISR
* @param
* @return
* @note
*/
void SoftCalendarSecond_Increase(void)
{
    uint8_t day_added_flag = 0;
    uint8_t bLeapYear = 0;
    s_Softtime.sec++;

    if (s_Softtime.sec == 60)
    {
        s_Softtime.sec = 0;
        s_Softtime.min++;
        if (s_Softtime.min == 60)
        {
            s_Softtime.min = 0;
            s_Softtime.hour++;
            if (s_Softtime.hour == 24)
            {
                s_Softtime.hour = 0;
                day_added_flag = 1;
            }
        }
    }
    if (day_added_flag == 1)
    {
        bLeapYear = ((!(s_Softtime.year % 4) && (s_Softtime.year % 100)) || !(s_Softtime.year % 400));
        s_Softtime.day++;
        s_Softtime.wday++;
        s_Softtime.wday = (s_Softtime.wday + 7) % 7;
        if (s_Softtime.day == (DaysCount_Get(bLeapYear, s_Softtime.mon) + 1))
        {
            s_Softtime.day = 1;
            s_Softtime.mon++;
            if (s_Softtime.mon == 13)
            {
                s_Softtime.mon = 0;
                s_Softtime.year++;
            }
        }
    }
}

uint32_t RTC_Get_Wakeup_Freq(int index)
{
    if (index >= 0)
    {
        if (index < 4)
        {
            return HAL_RTC_GetClockFreq() / 2 / (HAL_RTC_GetPrescaler() + 1) / (1 << (14 - index - TDIVSEL));
        }
        else if (index == 4)
        {
            return HAL_RTC_GetClockFreq() / 2 / (HAL_RTC_GetPrescaler() + 1) / (HAL_RTC_Get1sTicks() + 1);
        }
    }
    return 0;
}
/**
* @brief RTC Interrupt Service Routine
*       hanlder:
*       sec
*       wakeup_wp4
*       wakeup_wp3
*       wakeup_wp2
*       wakeup_wp1
*       wakeup_wp0
*       tamper1
*       tamper2
*       alarm
* @param
* @return
* @note
*/
void RTC_InterruptHandler(void)
{
    if (IO_BIT_GET(RTC->FLAG, IRQ_SECOND_EN))
    {
        SoftCalendarSecond_Increase();

        if (s_AlarmStep > 0)
        {
            if (s_SecondCounter++ > (CHECK_SECOND_COUNTER << 1))
            {
                PR_DEBUG("failed on alarm[%d]\n", s_AlarmStep);
                s_AlarmFailed = -1;
                s_AlarmStep++;
            }
        }

        if (s_HardwareTimerDisplay)
        {
            HAL_RTC_GetTimeEx(&s_Hardtime);
            Time_Print(&s_Hardtime);
        }
        if (s_SoftwareTimerDisplay)
        {
            Time_Print(&s_Softtime);
        }
        IO_BIT_SET(RTC->INTCLR, IRQ_SECOND_EN);
    }
    if (s_TimeIrqDisplay)
    {
        if (IO_BIT_GET(RTC->FLAG, IRQ_MINUTE_EN))
        {
            s_TimeIrqStep++;
            PR_DEBUG("one min past\n");
            IO_BIT_SET(RTC->INTCLR, IRQ_MINUTE_EN);
        }
        if (IO_BIT_GET(RTC->FLAG, IRQ_HOUR_EN))
        {
            s_TimeIrqStep++;
            PR_DEBUG("one hour past\n");
            IO_BIT_SET(RTC->INTCLR, IRQ_HOUR_EN);
        }
        if (IO_BIT_GET(RTC->FLAG, IRQ_DAY_EN))
        {
            s_TimeIrqStep++;
            PR_DEBUG("one day past\n");
            IO_BIT_SET(RTC->INTCLR, IRQ_DAY_EN);
        }
        if (IO_BIT_GET(RTC->FLAG, IRQ_MONTH_EN))
        {
            s_TimeIrqStep++;
            PR_DEBUG("one mon past\n");
            IO_BIT_SET(RTC->INTCLR, IRQ_MONTH_EN);
        }
        if (IO_BIT_GET(RTC->FLAG, IRQ_YEAR_EN))
        {
            s_TimeIrqStep++;
            PR_DEBUG("one year past\n");
            IO_BIT_SET(RTC->INTCLR, IRQ_YEAR_EN);
        }
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_WAKEUP_WP4F_EN) &&
            s_WakeUpMaskDisplay & IRQ_WAKEUP_WP4F_EN)
    {
        IO_BIT_SET(RTC->INTCLR, IRQ_WAKEUP_WP4F_EN);
        if (s_WakeUpCnt == 0)
        {
            PR_DEBUG("wakeup wp4@%dHz\t", RTC_Get_Wakeup_Freq(4));
            Time_Print(&s_Softtime);
            PR_DEBUG("...ignore %d times print\n", RTC_Get_Wakeup_Freq(4) - 2);
        }
        s_WakeUpCnt++;
        if (s_WakeUpCnt >= RTC_Get_Wakeup_Freq(4))
        {
            PR_DEBUG("wakeup wp4@%dHz\t", RTC_Get_Wakeup_Freq(4));
            Time_Print(&s_Softtime);
            s_WakeUpMaskDisplay = 0;
            s_WakeUpCnt = 0;
            RTCIrq_Wakeup_Disable();
        }
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_WAKEUP_WP3F_EN) &&
            s_WakeUpMaskDisplay & IRQ_WAKEUP_WP3F_EN)
    {
        PR_DEBUG("wakeup wp3@%dHz\t", RTC_Get_Wakeup_Freq(3));
        IO_BIT_SET(RTC->INTCLR, IRQ_WAKEUP_WP3F_EN);
        Time_Print(&s_Softtime);
        s_WakeUpCnt++;
        if (s_WakeUpCnt == RTC_Get_Wakeup_Freq(3))
        {
            s_WakeUpMaskDisplay = IRQ_WAKEUP_WP4F_EN;
            s_WakeUpCnt = 0;
        }
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_WAKEUP_WP2F_EN) &&
            s_WakeUpMaskDisplay & IRQ_WAKEUP_WP2F_EN)
    {
        PR_DEBUG("wakeup wp2@%dHz\t", RTC_Get_Wakeup_Freq(2));
        IO_BIT_SET(RTC->INTCLR, IRQ_WAKEUP_WP2F_EN);
        Time_Print(&s_Softtime);
        s_WakeUpCnt++;
        if (s_WakeUpCnt == RTC_Get_Wakeup_Freq(2))
        {
            s_WakeUpMaskDisplay = IRQ_WAKEUP_WP3F_EN;
            s_WakeUpCnt = 0;
        }
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_WAKEUP_WP1F_EN) &&
            s_WakeUpMaskDisplay & IRQ_WAKEUP_WP1F_EN)
    {
        PR_DEBUG("wakeup wp1@%dHz\t", RTC_Get_Wakeup_Freq(1));
        IO_BIT_SET(RTC->INTCLR, IRQ_WAKEUP_WP1F_EN);
        Time_Print(&s_Softtime);
        s_WakeUpCnt++;
        if (s_WakeUpCnt == RTC_Get_Wakeup_Freq(1))
        {
            s_WakeUpMaskDisplay = IRQ_WAKEUP_WP2F_EN;
            s_WakeUpCnt = 0;
        }
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_WAKEUP_WP0F_EN) &&
            s_WakeUpMaskDisplay & IRQ_WAKEUP_WP0F_EN)
    {
        PR_DEBUG("wakeup wp0@%dHz\t", RTC_Get_Wakeup_Freq(0));
        IO_BIT_SET(RTC->INTCLR, IRQ_WAKEUP_WP0F_EN);
        Time_Print(&s_Softtime);
        s_WakeUpCnt++;
        if (s_WakeUpCnt == RTC_Get_Wakeup_Freq(0))
        {
            s_WakeUpMaskDisplay = IRQ_WAKEUP_WP1F_EN;
            s_WakeUpCnt = 0;
        }
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_TAMPER1_IE))
    {
        HAL_RTC_GetTimeStamp(&s_rtc_time);
        IO_BIT_SET(RTC->INTCLR, IRQ_TAMPER1_IE);
        PR_DEBUG("Tamper1 time\t");
        Time_Print(&s_rtc_time);
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_TAMPER2_IE))
    {
        HAL_RTC_GetTimeStamp(&s_rtc_time);
        IO_BIT_SET(RTC->INTCLR, IRQ_TAMPER2_IE);
        PR_DEBUG("Tamper2 time\t");
        Time_Print(&s_rtc_time);
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_ALARM_IE))
    {
        s_SecondCounter = 0;
        IO_BIT_SET(RTC->INTCLR, IRQ_ALARM_IE);
        HAL_RTC_GetTimeEx(&s_rtc_time);
        PR_DEBUG("Alarm[%d]:\t", s_AlarmStep++);
        Time_Print(&s_rtc_time);
    }
    if (IO_BIT_GET(RTC->FLAG, IRQ_PES_EN))
    {
        IO_BIT_SET(RTC->INTCLR, IRQ_PES_EN);
        s_basetimer_pes_trigger_counter++;
        PR_DEBUG("Basetimer causes PES issue\n");
    }
    NVIC_ClearPendingIRQ(RTC_IRQn);
}

/**
* @brief Enable Second irq
* @param
* @return
* @note
*/
void RTCIrq_Second_Enable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL, IRQ_SECOND_EN);
    HAL_RTC_Lock();

    NVIC_EnableIRQ(RTC_IRQn);
}

/**
* @brief Disable Second irq
* @param
* @return
* @note
*/
void RTCIrq_Second_Disable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();

    IO_BIT_CLEAR(RTC->INTEN, IRQ_SECOND_EN);
    HAL_RTC_Lock();

    NVIC_DisableIRQ(RTC_IRQn);
}

/**
* @brief Disable Wakeup irq
* @param
* @return
* @note
*/
void RTCIrq_Wakeup_Disable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, IRQ_WAKEUP_WP4F_EN | IRQ_WAKEUP_WP3F_EN |
                 IRQ_WAKEUP_WP2F_EN | IRQ_WAKEUP_WP1F_EN | IRQ_WAKEUP_WP0F_EN);
    HAL_RTC_Lock();

    NVIC_DisableIRQ(RTC_IRQn);
}

/**
* @brief Enable Wakeup irq
* @param
* @return
* @note
*/
void RTCIrq_Wakeup_Enable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL, IRQ_WAKEUP_WP4F_EN | IRQ_WAKEUP_WP3F_EN |
                   IRQ_WAKEUP_WP2F_EN | IRQ_WAKEUP_WP1F_EN | IRQ_WAKEUP_WP0F_EN);
    HAL_RTC_Lock();

    NVIC_EnableIRQ(RTC_IRQn);
}

/**
* @brief Enable Alarm irq
* @param
* @return
* @note
*/
void RTCIrq_Alarm_Enable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL, IRQ_ALARM_IE | IRQ_SECOND_EN);
    HAL_RTC_Lock();

    NVIC_EnableIRQ(RTC_IRQn);
}

/**
* @brief Set all RTC register to the resetValue
* @param
* @return
* @note
*/
void RTC_Register_Clear(void)
{
    int i = 0;
    RTC_Reg_T *pRTCReg = NULL;

    HAL_RTC_Unlock();
    for (i = 0; i < RTC_REGISTER_LENGTH ; i++)
    {
        pRTCReg = &(s_RTCRegs[i]);
        HAL_RTC_WaitReady();
        ADDR_BITMASK_SET(pRTCReg->addr, MASK_ALL, pRTCReg->resetValue);
    }

    HAL_RTC_Lock();

}

/**
* @brief Initialize the harware RTC time and software time
* @param
* @return
* @note
*/
void Time_Init(void)
{
    uint32_t year   = 2017;
    uint32_t mon  = 4;
    uint32_t day    = 8;
    uint32_t hour   = 1;
    uint32_t min = 2;
    uint32_t sec = 3;
    memset(&s_Softtime, 0, sizeof(UTCTime_T));
    s_Softtime.year   = year;
    s_Softtime.mon  = mon;
    s_Softtime.day    = day;
    s_Softtime.hour   = hour;
    s_Softtime.min = min;
    s_Softtime.sec = sec;
    s_Softtime.wday   = WeekDay_Calculate(year, mon, day);
    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);
}

/**
* @brief print the RTC module version number
* @param
* @return
* @note
*/
void RTCVersion_Read(void)
{
    PR_DEBUG("RTC_CLK:%d Hz", HAL_RTC_GetClockFreq());
    PR_DEBUG("\nRTC Module Version:0x%x\n", IO_BITS_GET(RTC->VER, MASK_ALL));
}

/**
* @brief Enable rtc timer interrupt, including sec, min, hour, day, mon, year
* @param
* @return
* @note
*/
void RTCIrq_Time_Enable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL,
                   IRQ_SECOND_EN | IRQ_MINUTE_EN | IRQ_HOUR_EN | IRQ_DAY_EN | IRQ_MONTH_EN | IRQ_YEAR_EN);
    HAL_RTC_Lock();

    NVIC_EnableIRQ(RTC_IRQn);
}

/**
* @brief Disable rtc timer interrupt
* @param
* @return
* @note
*/
void RTCIrq_Time_Disable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    HAL_RTC_Unlock();
    HAL_RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN,
                 IRQ_SECOND_EN | IRQ_MINUTE_EN | IRQ_HOUR_EN | IRQ_DAY_EN | IRQ_MONTH_EN | IRQ_YEAR_EN);
    HAL_RTC_Lock();

    NVIC_DisableIRQ(RTC_IRQn);
}

/**
* @brief Test the time interrupt, including sec, min, hour, day, mon, year
* @param
* @return
* @note
*/
int RTCTimeInterrupt_Test(void)
{
    int step = 0;
    s_TimeIrqStep = 0;

    PR_DEBUG("\n\n*** RTCTimeInterrupt Test Start***\n");
    PR_DEBUG("__________________________________________________\n");

    sBinMode = 1;
    BinMode_Set(1);

    HAL_RTC_Unlock();
    IO_BIT_SET(RTC->INTCLR, IRQ_CLEAR_ALL_PENDING_FLAG_MASK);
    HAL_RTC_Lock();

    s_TimeIrqDisplay = 1;
    s_HardwareTimerDisplay = 1;

    PR_DEBUG("\ntest min irq\n");
    /* min irq */
    HAL_RTC_SetDateTime(2017, 4, 18, 12, 25, 50);

    RTCIrq_Time_Enable();

    step += 1;
    while (s_TimeIrqStep < step)
    {
        small_delay();
    }

    HAL_RTC_Unlock();
    PR_DEBUG("\ntest hour irq\n");
    /* hour irq */
    HAL_RTC_SetTime(12, 59, 50);

    step += 2;
    while (s_TimeIrqStep < step)
    {
        small_delay();
    }
    PR_DEBUG("\ntest day irq\n");
    /* day irq */
    HAL_RTC_SetTime(23, 59, 50);

    step += 3;
    while (s_TimeIrqStep < step)
    {
        small_delay();
    }
    PR_DEBUG("\ntest mon irq\n");
    /* mon irq */
    HAL_RTC_SetDateTime(2017, 4, 30, 23, 59, 50);

    step += 4;
    while (s_TimeIrqStep < step)
    {
        small_delay();
    }
    PR_DEBUG("\ntest year irq\n");
    /* year irq */
    HAL_RTC_SetDateTime(2017, 12, 31, 23, 59, 50);

    step += 5;
    while (s_TimeIrqStep < step)
    {
        small_delay();
    }

    RTCIrq_Time_Disable();
    s_HardwareTimerDisplay = 0;
    s_TimeIrqDisplay = 0;
    PR_DEBUG("__________________________________________________\n");
    PR_DEBUG("*** RTCTimeInterrupt Test End***\n");
    return 0;
}

#define CPU_ENTER_SLEEP_MODE()                 do {                \
        __WFE();   \
        __WFE();   \
    } while (0)

int RTCPES_TriggerCPU()
{
    RET_CODE ret = RET_OK;
    PES_ChannelIndex_T chIndex;
    PES_ChannelConfig_T chCfg;

    PR_DEBUG("FPGA_PES_TEST_CASE8\n ");
    PR_DEBUG("IO As Master To Triger CPU\n");

    /*request pes channel*/
    chIndex = HAL_PES_RequestEventChannel();
    if (PES_INVAILD_CHANNEL == chIndex)
    {
        ret = RET_ERROR;
        PR_DEBUG("%s:Err:PES_RequestEventChannel\n", __FUNCTION__);
        return ret;
    }
    /*for fpag debug,it is xpa11 vaild, so foce to xpa11(map to channel8)*/
    //chIndex = PES_EVENT_CHANNEL8;

    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    chCfg.syncMode = PES_SYNC_RISING_EDGE;//PES_ASYNC;
    chCfg.masterSrcConfig = PES_MASTER_RTCC;
    //chCfg.masterSrcConfig.masterTrigerOutput.ioPinOutput = IO_PIN_XPA11;

    ret = HAL_PES_EventToCpuRegConfig(&chCfg, chIndex);
    if (ret)
    {
        PR_ERR("%s:Err:PES_CPUAsSlaveConfig\n", __FUNCTION__);
        goto out;
    }

    RTC_TriggerPESEnable();

#ifdef CPU_DEEP_SLEEP_ENABLE
    PR_DEBUG("CPU Enter Deep Sleep Mode, Wait Pes To Triger WakeUp\n");
    IO_BIT_SET(HW32_REG(0x40039018), 0x1 << 0);
    SCB->SCR |= (1 << 2);
    CPU_ENTER_SLEEP_MODE();
#else
    PR_DEBUG("CPU Enter Only Sleep Mode, Wait Pes To Triger WakeUp\n");
    SCB->SCR &= ~(1 << 2);
    CPU_ENTER_SLEEP_MODE();
#endif
out:
    return 0;
}

/* Disable timer 0 interrupt */
static void Systick_DisableInterrupt(void)
{
    NVIC_DisableIRQ(SysTick_IRQn);
}

int RTCSleepMode_Test(void)
{
    int sleep_sec = 10;
    UTCTime_T *time = HAL_RTC_GetTime();
    PR_DEBUG("Sleep for %ds\n", sleep_sec);

    /* sleep */
    Systick_DisableInterrupt();

    printf("run here \n");
    time->sec += sleep_sec;
    if (time->sec >= 60)
    {
        time->sec = 0;
        time->min += 1;
    }
    HAL_RTC_SetAlarm(time);

    __WFI();

    PR_DEBUG("Wake up\n");

    WakeUp_End();

    return 0;
}

int main(void)
{
    int ret = 0;
    HAL_RTC_Config_T rtcConfig;

    rtcConfig.clock         = HAL_RTC_CLOCK_LIRC;

    HAL_RTC_Init(&rtcConfig);

    HAL_RequestIrq(RTC_IRQn, RTC_IRQ_PRIORITY, RTC_InterruptHandler, 0);
    HAL_IRQ_ENABLE(RTC_IRQn);

    RTCVersion_Read();

    HAL_RTC_WaitClockReady();

    PR_DEBUG("\n ====RTC Test==== \n");

    RTC_Register_Clear();

    ret += RTCRegisterRW_Test();

    RTC_Register_Clear();

    ret += RTCRegisterProtect_Test();

    HAL_RTC_Init(&rtcConfig);
    HAL_RequestIrq(RTC_IRQn, RTC_IRQ_PRIORITY, RTC_InterruptHandler, 0);
    HAL_IRQ_ENABLE(RTC_IRQn);

    HAL_RTC_WaitClockReady();

    ret += RTCLeapYear_Test();

    Time_Init();

    RTCIrq_Second_Enable();

    ret += RTCAddAndSub_Test();

    ret += RTCWakeup_Test();

    ret += RTCTimerHold_Test();

    ret += RTCPreScale_Test();

    ret += RTCTamper_Test();

    ret += RTCPES_Test();

    ret += RTCAlarm_Test();

    //ret += RTCCalibration_Test();

    ret += RTCTimeInterrupt_Test();

    /* ret += BaseTimer_Start(0x3fFFff); */

    ret += RTCPES_TriggerCPU();

    ret += RTCSleepMode_Test();

    PR_DEBUG("All tests are finished\n");

    if (ret < 0)
    {
        PR_DEBUG("test error %x\n", ret);
    }

    return ret;
}
