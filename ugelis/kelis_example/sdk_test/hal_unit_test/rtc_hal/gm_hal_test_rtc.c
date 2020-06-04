/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_rtc.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

extern u32_t sys_rand32_get(void);

#define PROGRESS_UNIT     5
#define CALENDAR_TEST_CNT 10000

#define RTC_TEST_USE_CLOCK HAL_RTC_CLOCK_LIRC

static int gm_test_alarm_flag = 0;
static int gm_test_timer_cnt = 0;

static void gm_test_print(UTCTime_T *pUtcTime, char *info)
{
    TC_PRINT("%s:%04d/%02d/%02d(%d) %02d:%02d:%02d\n", info,
             pUtcTime->year, pUtcTime->mon, pUtcTime->day,
             pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
}

void clock_info(HAL_RTC_Config_T *pConfig)
{
    switch (pConfig->clock)
    {
        case HAL_RTC_CLOCK_LIRC:
            TC_PRINT("Clock: LIRC\n");
            break;
        case HAL_RTC_CLOCK_LOSC:
            TC_PRINT("Clock: LOSC\n");
            break;
        case HAL_RTC_CLOCK_HOSC:
            TC_PRINT("Clock: HOSC\n");
            break;
    }
}

static void gm_test_rtc_callback(int type)
{
    if (type == HAL_RTC_ALARM)
    {
        TC_PRINT("Catch RTC alarm interrupt\n");
        gm_test_alarm_flag = 1;
    }
    else if (type == HAL_RTC_TIMER)
    {
        gm_test_timer_cnt++;
    }
}

static void gm_test_rtc_randtime(UTCTime_T *pTime)
{
    pTime->year = sys_rand32_get() % 4000;
    pTime->mon = sys_rand32_get() % 12 + 1;
    pTime->day = sys_rand32_get() % 31 + 1;
    pTime->hour = sys_rand32_get() % 24;
    pTime->min = sys_rand32_get() % 60;
    pTime->sec = sys_rand32_get() % 60;
    pTime->wday = HAL_UTC_CalcWeekday(pTime->year, pTime->mon, pTime->day);
}

static int gm_test_rtc_nextsec()
{
    UTCTime_T time;
    int cnt = 100;

    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Next Interval Moment Test\n");
    TC_PRINT("----------------------------------\n");

    while (cnt--)
    {
        gm_test_rtc_randtime(&time);
        //gm_test_print(&time, "orig");

        if (HAL_RTC_NextSec(&time, 5) != 0)
        {
            return TC_FAIL;
        }
        //gm_test_print(&time, "alarm");
        //TC_PRINT("\n");
    }

    time.year = 2019;
    time.mon = 12;
    time.day = 31;
    time.hour = 23;
    time.min = 59;
    time.sec = 58;
    gm_test_print(&time, "orig");
    if (HAL_RTC_NextSec(&time, 5) != 0)
    {
        return TC_FAIL;
    }
    gm_test_print(&time, "alarm");
    return TC_PASS;
}

static void print_progress(float progress)
{
    int i = 0;
    int unit = PROGRESS_UNIT;
    int current_unit = (int)(progress / unit);
    TC_PRINT("[");
    for (i = 0; i < 100 / unit; i++)
    {
        if (i < current_unit)
        {
            TC_PRINT("%s", "-");
        }
        else if (i == current_unit)
        {
            TC_PRINT("%s", ">");
        }
        else
        {
            TC_PRINT(" ");
        }
    }
    TC_PRINT("]%02.2f%%\n", progress);
}

static void gm_test_rtc_init()
{
    Timer_Callback callback = gm_test_rtc_callback;
    HAL_RTC_Config_T rtcConfig;

    rtcConfig.alarmCallback = callback;
    rtcConfig.timerCallback = callback;

    rtcConfig.clock         = RTC_TEST_USE_CLOCK;

    clock_info(&rtcConfig);

    HAL_RTC_Init(&rtcConfig);

    HAL_RTC_Start();
}

static int gm_test_rtc_work()
{
    Time_T t1, t2;
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("RTC Work Test\n");
    TC_PRINT("----------------------------------\n");

    gm_test_rtc_init();

    /* 1. Verify HAL_RTC_Start() */
    HAL_RTC_Start();
    t1 = HAL_RTC_GetTickSecond();
    k_sleep(2000);
    t2 = HAL_RTC_GetTickSecond();

    TC_PRINT("val_1: %d, val_2: %d\n", t1, t2);
    if (t2 <= t1)
    {
        TC_PRINT("RTC doesn't work well\n");
        return TC_FAIL;
    }

    /* 2. Verify HAL_RTC_Stop() */
    HAL_RTC_Stop();
    t1 = HAL_RTC_GetTickSecond();
    k_sleep(1000);
    t2 = HAL_RTC_GetTickSecond();

    if (t2 != t1)
    {
        TC_PRINT("val_1: %d, val_2: %d\n", t1, t2);
        TC_PRINT("Fail to disable RTC\n");
        return TC_FAIL;
    }
    return TC_PASS;
}

static int gm_test_rtc_calendar()
{
    int year, mon, day, hour, min, sec;
    int wday = 1;
    int i = 0;
    UTCTime_T *pUtcTime;
    UTCTime_T time;

    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Calendar Setting And Reading Test\n");
    TC_PRINT("----------------------------------\n");

    /* 1. Verify HAL_RTC_SetDateTime() */
    gm_test_rtc_init();
    for (i = 0; i < CALENDAR_TEST_CNT; i++)
    {
        gm_test_rtc_randtime(&time);
        year = time.year;
        mon = time.mon;
        day = time.day;
        hour = time.hour;
        min = time.min;
        sec = time.sec;
        HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);
        wday = HAL_UTC_CalcWeekday(year, mon, day);
        //TC_PRINT("~~%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
        //         year, mon, day, wday, hour, min, sec);

        pUtcTime = HAL_RTC_GetTime();
        if (!(pUtcTime->year == year && pUtcTime->mon == mon && pUtcTime->day == day &&
                pUtcTime->wday == wday &&
                pUtcTime->hour == hour && pUtcTime->min == min && pUtcTime->sec == sec))
        {
            TC_PRINT("%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
                     pUtcTime->year, pUtcTime->mon, pUtcTime->day,
                     pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
            return TC_FAIL;
        }
        if (i % (CALENDAR_TEST_CNT / (100 / PROGRESS_UNIT)) == 0)
        {
            print_progress(100.0 * i / CALENDAR_TEST_CNT);
        }
    }
    print_progress(100.0);

    /* 2. Verify HAL_RTC_GetTime() */
    int wait_sec = 2;
    TC_PRINT("Calendar Precision Test(%ds)\n", wait_sec);
    year = 2019, mon = 12, day = 31, hour = 23, min = 59, sec = 59;
    wday = HAL_UTC_CalcWeekday(year, mon, day);
    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    k_sleep(1000 * wait_sec);
    pUtcTime = HAL_RTC_GetTime();
    if (!(pUtcTime->year == 2020 && pUtcTime->mon == 1 && pUtcTime->day == 1 &&
            pUtcTime->wday == 3 &&
            pUtcTime->hour == 0 && pUtcTime->min == 0 && ((pUtcTime->sec - sec + 60) % 60 <= (wait_sec + 1))))
    {
        TC_PRINT("%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
                 pUtcTime->year, pUtcTime->mon, pUtcTime->day,
                 pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
        return TC_FAIL;
    }
    return TC_PASS;
}

static int gm_test_rtc_timer(void)
{
    Timer_T timer;
    int cycle = 50;
    int test_cnt = 40;
    int dst_cnt = 0;

    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Timer Test\n");
    TC_PRINT("----------------------------------\n");

    /* 1. Verify HAL_RTC_StartTimer() */
    gm_test_rtc_init();
    TC_PRINT("Now reset the timer every %dms\n", cycle);
    timer.cycle.tv_sec  = 0;
    timer.cycle.tv_msec = cycle;
    gm_test_timer_cnt = 0;
    HAL_RTC_StartTimer(&timer);

    k_sleep(cycle * (test_cnt + 5)); //LIRC is not precise
    dst_cnt = gm_test_timer_cnt;
    TC_PRINT("gm_test_timer_cnt(%d), test_cnt(%d)\n", dst_cnt, test_cnt);
    if (dst_cnt < test_cnt)
    {
        return TC_FAIL;
    }

    /* 2. Verify HAL_RTC_StopTimer() */
    HAL_RTC_StopTimer();
    gm_test_rtc_init();
    TC_PRINT("Now reset the timer every %dms\n", cycle);
    timer.cycle.tv_sec  = 0;
    timer.cycle.tv_msec = cycle;
    gm_test_timer_cnt = 0;
    HAL_RTC_StartTimer(&timer);
    k_sleep(1000);
    TC_PRINT("test stopping timer in advance\n");
    HAL_RTC_StopTimer();
    dst_cnt = gm_test_timer_cnt;
    TC_PRINT("gm_test_timer_cnt(%d), test_cnt(%d)\n", dst_cnt, test_cnt);
    if (dst_cnt > test_cnt)
    {
        return TC_FAIL;
    }

    return TC_PASS;
}

static int gm_test_rtc_alarm()
{
    UTCTime_T time;
    UTCTime_T *pTime;
    int year, mon, day, hour, min, sec;

    int wait_sec = 5;


    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Alarm Test\n");
    TC_PRINT("----------------------------------\n");

    /* 1. Verify HAL_RTC_SetAlarm */
    gm_test_rtc_init();
    gm_test_rtc_randtime(&time);
    year = time.year;
    mon = time.mon;
    day = time.day;
    hour = time.hour;
    min = time.min;
    sec = time.sec;
    gm_test_print(&time, "current time:");
    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    if (HAL_RTC_NextSec(&time, wait_sec) != 0)
    {
        return TC_FAIL;
    }
    gm_test_alarm_flag = 0;
    TC_PRINT("Alarm after %ds\n", wait_sec);
    HAL_RTC_SetAlarm(&time);
    k_sleep((wait_sec + 1) * 1000);
    if (!gm_test_alarm_flag)
    {
        gm_test_print(&time, "alarm time:");
        int cnt = 5;
        while (cnt--)
        {
            pTime = HAL_RTC_GetTime();
            gm_test_print(pTime, "");
            k_sleep(1000);
        }
        return TC_FAIL;
    }

    /* 2. Verify RTC Alarm disabled */
    gm_test_rtc_randtime(&time);
    year = time.year;
    mon = time.mon;
    day = time.day;
    hour = time.hour;
    min = time.min;
    sec = time.sec;
    HAL_RTC_SetDateTime(year, mon, day, hour, min, sec);

    if (HAL_RTC_NextSec(&time, wait_sec) != 0)
    {
        return TC_FAIL;
    }
    gm_test_alarm_flag = 0;
    HAL_RTC_SetAlarm(&time);
    k_sleep(1000);
    if (gm_test_alarm_flag)
    {
        return TC_FAIL;
    }
    TC_PRINT("Cancel alarm\n");
    HAL_RTC_CancelAlarm();
    k_sleep(wait_sec);
    if (gm_test_alarm_flag)
    {
        return TC_FAIL;
    }
    return TC_PASS;
}

int gm_hal_test_rtc(void)
{
    printf("================HAL_RTC Test Start=================\n\n\n");

    printf("TEST1\n");
    zassert_true(gm_test_rtc_work() == TC_PASS, NULL);
    printf("TEST2\n");
    zassert_true(gm_test_rtc_nextsec() == TC_PASS, NULL);
    printf("TEST3\n");
    zassert_true(gm_test_rtc_calendar() == TC_PASS, NULL);
    printf("TEST4\n");
    zassert_true(gm_test_rtc_alarm() == TC_PASS, NULL);
    printf("TEST5\n");
    zassert_true(gm_test_rtc_timer() == TC_PASS, NULL);
    printf("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    printf("================RTC Test  End==================\n\n\n");
}
