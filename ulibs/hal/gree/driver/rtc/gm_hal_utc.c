/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_utc.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/06/13
 *
 * @brief         utc transformer For GM6721.
 *
 * @note
 *    2018/06/13, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_hal_utc.h>

/**
 * @defgroup Error_Macros
 * Specify macros of Error
 * @{
 */
#define EINVAL      22
#define EOVERFLOW   EINVAL

/**
 * @}
 */


/* time */
#define TM_YEAR_BASE    1900

#define EPOCH_YEAR      1970

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

/* True if the arithmetic type T is signed.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))

/* Shift A right by B bits portably, by dividing A by 2**B and
truncating towards minus infinity. A and B should be free of side
effects, and B should be in the range 0 <= B <= INT_BITS - 2, where
INT_BITS is the number of useful bits in an int.  GNU code can
assume that INT_BITS is at least 32.

ISO C99 says that A >> B is implementation-defined if A < 0.  Some
implementations (e.g., UNICOS 9.0 on a Cray Y-MP EL) don't shift
right in the usual way when A < 0, so SHR falls back on division if
ordinary A >> B doesn't seem to be the usual signed shift. */
#define SHR(a, b)                                               \
    ((-1 >> 1 == -1                                               \
      && (int) -1 >> 1 == -1                                 \
      && ((Time_T) -1 >> 1 == -1 || ! TYPE_SIGNED (Time_T)))      \
     ? (a) >> (b)                                                 \
     : (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))

/* end of time */

const unsigned short int __mon_yday[2][13] =
{
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/* Compute the `UTCTime_T' representation of *T,
offset OFFSET seconds east of UTC,
and store year, yday, mon, mday, wday, hour, min, sec into *TP.
Return nonzero if successful.  */
static int UtcTime_OfftimeCompute(const Time_T *t, long int offset, UTCTime_T *tp)
{
#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)
    Time_T days, rem, y;
    const unsigned short int *ip;

    days = *t / SECS_PER_DAY;
    rem = *t % SECS_PER_DAY;
    rem += offset;
    while (rem < 0)
    {
        rem += SECS_PER_DAY;
        --days;
    }
    while (rem >= SECS_PER_DAY)
    {
        rem -= SECS_PER_DAY;
        ++days;
    }
    tp->hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tp->min = rem / 60;
    tp->sec = rem % 60;
    /* January 1, 1970 was a Thursday.  */
    tp->wday = (4 + days) % 7;
    if (tp->wday < 0)
    {
        tp->wday += 7;
    }
    y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (isleap(y) ? 366 : 365))
    {
        /* Guess a corrected year, assuming 365 days per year.  */
        Time_T yg = y + days / 365 - (days % 365 < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -= ((yg - y) * 365
                 + LEAPS_THRU_END_OF(yg - 1)
                 - LEAPS_THRU_END_OF(y - 1));
        y = yg;
    }
    tp->year = y - 1900;
    if (tp->year != y - 1900)
    {
        /* The year cannot be represented due to overflow.  */
        //SET_ERROR(EOVERFLOW);
        return 0;
    }
    tp->yday = days;
    ip = __mon_yday[isleap(y)];
    for (y = 11; days < (long int) ip[y]; --y)
    {
        continue;
    }
    days -= ip[y];
    tp->mon = y;
    tp->day = days + 1;
    return 1;
}

/* Return the `UTCTime_T' representation of *TIMER in the local timezone.
Use local time if USE_LOCALTIME is nonzero, UTC otherwise. */
UTCTime_T *HAL_UTC_Convert(const Time_T *timer, UTCTime_T *tp)
{
    if (timer == 0)
    {
        //SET_ERROR(EINVAL);
        return 0;
    }

    if (! UtcTime_OfftimeCompute(timer, 0, tp))
    {
        tp = 0;
    }

    tp->year += TM_YEAR_BASE;
    tp->mon  += 1;
    HAL_RTC_ConvertToWday1(tp->wday);

    return tp;
}

static Time_T UtcTime_DiffYDHMS(int year1, int yday1, int hour1, int min1, int sec1,
                                int year0, int yday0, int hour0, int min0, int sec0)
{

    /* Compute intervening leap days correctly even if year is negative.
    Take care to avoid integer overflow here.  */
    /*              year1   year0
     *              2017    1970
     * 4-years      504     492
     * 100-years    20      19
     * 400-years    5       4
     * intervening_leap_days 12 (there are 12 leap years in 1970~2017)
     */
    int a4 = SHR(year1, 2) + SHR(TM_YEAR_BASE, 2) - !(year1 & 3);
    int b4 = SHR(year0, 2) + SHR(TM_YEAR_BASE, 2) - !(year0 & 3);
    int a100 = a4 / 25 - (a4 % 25 < 0);
    int b100 = b4 / 25 - (b4 % 25 < 0);
    int a400 = SHR(a100, 2);
    int b400 = SHR(b100, 2);
    int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

    /* Compute the desired time in Time_T precision.  Overflow might
    occur here.  */
    Time_T tyear1 = year1;
    Time_T years = tyear1 - year0;
    Time_T days = 365 * years + yday1 - yday0 + intervening_leap_days;
    Time_T hours = 24 * days + hour1 - hour0;
    Time_T minutes = 60 * hours + min1 - min0;
    Time_T seconds = 60 * minutes + sec1 - sec0;
    return seconds;
}

/* Return 1 if YEAR + TM_YEAR_BASE is a leap year.  */
static int UtcTime_Leapyear(int year)
{
    /* Don't add YEAR to TM_YEAR_BASE, as that might overflow.
    Also, work even if YEAR is negative.  */
    return
                    ((year & 3) == 0
                     && (year % 100 != 0
                         || ((year / 100) & 3) == (- (TM_YEAR_BASE / 100) & 3)));
}

/* Convert *TP to a Time_T value */
Time_T HAL_UTC_ToTimeT(UTCTime_T *tp)
{
    Time_T secondFrom1970;

    /* Time requested.  Copy it in case CONVERT modifies *TP; this can
    occur if TP is localtime's returned value and CONVERT is localtime.  */
    int sec = tp->sec;
    int min = tp->min;
    int hour = tp->hour;
    int mday = tp->day;
    int mon = tp->mon;
    int year_requested = tp->year;

    /* Ensure that mon is in range, and set year accordingly.  */
    int mon_remainder = mon % 12;
    int negative_mon_remainder = mon_remainder < 0;
    int mon_years = mon / 12 - negative_mon_remainder;
    int lyear_requested = year_requested;
    int year = lyear_requested + mon_years;

    /* Calculate day of year from year, month, and day of month.
    The result need not be in range.  */
    int mon_yday = ((__mon_yday[UtcTime_Leapyear(year)]
                     [mon_remainder + 12 * negative_mon_remainder])
                    - 1);
    int lmday = mday;
    int yday = mon_yday + lmday;

    /* to = seconds from 1970/01/01 00:00:00 to y/m/d h:m:s */
    secondFrom1970 = UtcTime_DiffYDHMS(year, yday, hour, min, sec,
                                       EPOCH_YEAR - TM_YEAR_BASE, 0, 0, 0, 0);
    return secondFrom1970;
}

/**
* @brief Get the weekday on the day y/m/d
* @param y - year
* @param m - month
* @param d - day
* @return weekday from 1 to 7
* @note
*/
uint32_t HAL_UTC_CalcWeekday(uint32_t y, uint32_t m, uint32_t d)
{
    uint32_t week = 0;

    if (m == 1 || m == 2)
    {
        m += 12;
        y--;
    }

    week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

    return (week + 1); /* from 1 to 7 */
}

