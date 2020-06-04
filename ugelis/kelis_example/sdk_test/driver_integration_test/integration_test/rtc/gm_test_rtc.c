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
#include <board.h>
#include <ztest.h>
#include <misc/printk.h>
#include "gm_test_rtc.h"
#include <rtc.h>


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

struct device *rtc_dev;

static void gm_test_print(rtc_datetime *pUtcTime, char *info)
{
    TC_PRINT("%s:%04d/%02d/%02d(%d) %02d:%02d:%02d\n", info,
             pUtcTime->year, pUtcTime->mon, pUtcTime->day,
             pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
}

static void gm_test_rtc_callback(rtc_callback_type type)
{
    if (type == RTC_CALLBACK_ALARM)
    {
        TC_PRINT("Catch RTC alarm interrupt\n");
        gm_test_alarm_flag = 1;
    }
    else if (type == RTC_CALLBACK_TIMER)
    {
        gm_test_timer_cnt++;
    }
}

uint32_t gm_test_calc_weekday(uint32_t y, uint32_t m, uint32_t d)
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

static void gm_test_rtc_randtime(rtc_datetime *pTime)
{
    pTime->year = sys_rand32_get() % 4000;
    pTime->mon = sys_rand32_get() % 12 + 1;
    pTime->day = sys_rand32_get() % 31 + 1;
    pTime->hour = sys_rand32_get() % 24;
    pTime->min = sys_rand32_get() % 60;
    pTime->sec = sys_rand32_get() % 60;
    pTime->wday = gm_test_calc_weekday(pTime->year, pTime->mon, pTime->day);
}

static int gm_test_rtc_dayofmon(int year, int mon)
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

RET_CODE gm_test_rtc_calc_nextsec(rtc_datetime *pUtcTime, int sec)
{
    int maxday;

    if (pUtcTime->sec + sec >= 60)
    {
        pUtcTime->min = (pUtcTime->min + 1) % 60;

        if (pUtcTime->min + 1 >= 60)
        {
            pUtcTime->hour = (pUtcTime->hour + 1) % 24;

            if (pUtcTime->hour + 1 >= 24)
            {
                pUtcTime->day += 1;
                pUtcTime->wday = (pUtcTime->wday + 1) % 7;

                maxday = gm_test_rtc_dayofmon(pUtcTime->year, pUtcTime->mon);

                if (pUtcTime->day >= maxday)
                {
                    pUtcTime->day = 1;
                    pUtcTime->year += 1;
                    if (pUtcTime->year >= 4000)
                    {
                        return RET_INVPARAM;
                    }
                }
            }
        }
    }
    pUtcTime->sec = (pUtcTime->sec + sec) % 60;
    return RET_OK;
}

static int gm_test_rtc_nextsec()
{
    rtc_datetime time;
    int cnt = 100;

    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Next Interval Moment Test\n");
    TC_PRINT("----------------------------------\n");

    while (cnt--)
    {
        gm_test_rtc_randtime(&time);

        if (gm_test_rtc_calc_nextsec(&time, 5) != 0)
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
    if (gm_test_rtc_calc_nextsec(&time, 5) != 0)
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
    rtc_datetime time;
    rtc_dev = device_get_binding("GMRTC");
    if (!rtc_dev)
    {
        TC_PRINT("Cann't get GM-RTC Device\n");
    }

    struct rtc_cfg cfg;
    cfg.callback = gm_test_rtc_callback;
    rtc_reinit(rtc_dev);
    rtc_enable(rtc_dev);
    rtc_set_config(rtc_dev, cfg);
}

static int gm_test_rtc_work()
{
    int t1, t2;
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("RTC Work Test\n");
    TC_PRINT("----------------------------------\n");

    gm_test_rtc_init();

    /* 1. Verify HAL_RTC_Start() */
    rtc_enable(rtc_dev);
    t1 = rtc_read(rtc_dev);
    k_sleep(2000);
    t2 = rtc_read(rtc_dev);

    TC_PRINT("val_1: %d, val_2: %d\n", t1, t2);
    if (t2 <= t1)
    {
        TC_PRINT("RTC doesn't work well\n");
        return TC_FAIL;
    }

    /* 2. Verify rtc_disable(rtc_dev) */
    rtc_disable(rtc_dev);
    t1 = rtc_read(rtc_dev);
    k_sleep(1000);
    t2 = rtc_read(rtc_dev);

    if (t2 != t1)
    {
        TC_PRINT("val_1: %d, val_2: %d\n", t1, t2);
        TC_PRINT("Fail to disable RTC\n");
        return TC_FAIL;
    }
    return TC_PASS;
}

uint32_t gm_test_rtc_weekday(uint32_t y, uint32_t m, uint32_t d)
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

static int gm_test_rtc_calendar()
{
    int year, mon, day, hour, min, sec;
    int wday = 1;
    int i = 0;
    rtc_datetime *pUtcTime;
    rtc_datetime time;

    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Calendar Setting And Reading Test\n");
    TC_PRINT("----------------------------------\n");

    /* 1. Verify rtc_set_datetime() */
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
        rtc_set_datetime(rtc_dev, &time);
        wday = gm_test_rtc_weekday(year, mon, day);
        //TC_PRINT("~~%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
        //         year, mon, day, wday, hour, min, sec);

        pUtcTime = rtc_get_time(rtc_dev);
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

    /* 2. Verify rtc_get_time(rtc_dev) */
    int wait_sec = 2;
    TC_PRINT("Calendar Precision Test(%ds)\n", wait_sec);
    year = 2019, mon = 12, day = 31, hour = 23, min = 59, sec = 59;
    time.year = year;
    time.mon = mon;
    time.day = day;
    time.hour = hour;
    time.min = min;
    time.sec = sec;
    wday = gm_test_rtc_weekday(year, mon, day);
    rtc_set_datetime(rtc_dev, &time);

    k_sleep(1000 * wait_sec);
    pUtcTime = rtc_get_time(rtc_dev);
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

void clock_info()
{
    switch (RTC_CLK_SRC)
    {
        case RTC_CLK_SRC_LIRC:
            TC_PRINT("Clock: LIRC\n");
            break;
        case RTC_CLK_SRC_LOSC:
            TC_PRINT("Clock: LOSC\n");
            break;
        case RTC_CLK_SRC_HOSC:
            TC_PRINT("Clock: HOSC\n");
            break;
    }
}


static int gm_test_rtc_timer(void)
{
    rtc_timer timer;
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
    rtc_start_timer(rtc_dev, &timer);

    k_sleep(cycle * (test_cnt + 5)); //LIRC is not precise
    dst_cnt = gm_test_timer_cnt;
    TC_PRINT("gm_test_timer_cnt(%d), test_cnt(%d)\n", dst_cnt, test_cnt);
    if (dst_cnt < test_cnt)
    {
        return TC_FAIL;
    }

    /* 2. Verify rtc_disable(rtc_dev)Timer() */
    rtc_stop_timer(rtc_dev);
    gm_test_rtc_init();
    TC_PRINT("Now reset the timer every %dms\n", cycle);
    timer.cycle.tv_sec  = 0;
    timer.cycle.tv_msec = cycle;
    gm_test_timer_cnt = 0;
    rtc_start_timer(rtc_dev, &timer);

    k_sleep(1000);
    TC_PRINT("test stopping timer in advance\n");
    rtc_stop_timer(rtc_dev);
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
    rtc_datetime time;
    rtc_datetime *pTime;
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
    rtc_set_datetime(rtc_dev, &time);

    if (gm_test_rtc_calc_nextsec(&time, wait_sec) != 0)
    {
        return TC_FAIL;
    }
    gm_test_alarm_flag = 0;
    TC_PRINT("Alarm after %ds\n", wait_sec);
    rtc_set_alarm(rtc_dev, &time);
    k_sleep((wait_sec + 1) * 1000);
    if (!gm_test_alarm_flag)
    {
        gm_test_print(&time, "alarm time:");
        int cnt = 5;
        while (cnt--)
        {
            pTime = rtc_get_time(rtc_dev);
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
    rtc_set_datetime(rtc_dev, &time);

    if (gm_test_rtc_calc_nextsec(&time, wait_sec) != 0)
    {
        return TC_FAIL;
    }
    gm_test_alarm_flag = 0;
    rtc_set_alarm(rtc_dev, &time);
    k_sleep(1000);
    if (gm_test_alarm_flag)
    {
        return TC_FAIL;
    }
    TC_PRINT("Cancel alarm\n");
    rtc_cancel_alarm(rtc_dev);
    k_sleep(wait_sec);
    if (gm_test_alarm_flag)
    {
        return TC_FAIL;
    }
    return TC_PASS;
}

void gm_test_rtc(void)
{
    printf("================GM_RTC Test Start=================\n\n\n");

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
