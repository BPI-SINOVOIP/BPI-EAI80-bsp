/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          rtc_demo.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/06/14
 *
 * @brief         RTC Demo Calls RTC Driver
 *
 * @note
 *    2018/06/14, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <rtc.h>


/*-----------------------
 * Print macros
 *-----------------------*/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do {} while (0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{} while (0)
#else
    #define PR_WARNINR         printk
#endif

static int step = 0;

struct device *rtc_dev;

void Time_Print(void)
{
    rtc_datetime *pUtcTime;
    pUtcTime = rtc_get_time(rtc_dev);
    PR_DEBUG("%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
             pUtcTime->year, pUtcTime->mon, pUtcTime->day,
             pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
}

void Exit(void)
{
    rtc_stop_timer(rtc_dev);
    rtc_disable(rtc_dev);

    PR_DEBUG("bye\n");
}

void clock_info()
{
    switch (RTC_CLK_SRC)
    {
        case RTC_CLK_SRC_LIRC:
            PR_DEBUG("use clock LIRC\n");
            break;
        case RTC_CLK_SRC_LOSC:
            PR_DEBUG("use clock LOSC\n");
            break;
        case RTC_CLK_SRC_HOSC:
            PR_DEBUG("use clock HOSC\n");
            break;
    }
}

/* will be called upon expiration of the timer or the alarm */
static void rtc_callback(rtc_callback_type type)
{
    static int cnt = 0;
    if (type == RTC_CALLBACK_ALARM)
    {
        rtc_timer timer;
        uint32_t cycle = 1;

        cycle = 1;
        PR_DEBUG("now start the timer every %ds\n", cycle);
        timer.cycle.tv_sec  = cycle;
        timer.cycle.tv_msec = 0;
        rtc_cancel_alarm(rtc_dev);
        rtc_start_timer(rtc_dev, &timer);
    }
    else if (type == RTC_CALLBACK_TIMER)
    {
        Time_Print();
        if (cnt++ == 40)
        {
            cnt = 0;
            step++;
            rtc_stop_timer(rtc_dev);
            PR_DEBUG("now start next step\n");

        }
        else if (cnt == 10)
        {
            rtc_timer timer;
            uint32_t cycle = 50;
            PR_DEBUG("now reset the timer every %dms\n", cycle);
            timer.cycle.tv_sec  = 0;
            timer.cycle.tv_msec = cycle;

            rtc_start_timer(rtc_dev, &timer);
        }
    }
}

int main(void)
{
    PR_DEBUG("--------------\n");
    PR_DEBUG("RTC Demo start\n");
    PR_DEBUG("--------------\n");
    PR_DEBUG("\n");

    clock_info();

    rtc_dev = device_get_binding("GMRTC");
    if (!rtc_dev)
    {
        PR_DEBUG("Cann't get GM-RTC Device\n");
    }


    struct rtc_cfg cfg;
    cfg.callback = rtc_callback;

    rtc_enable(rtc_dev);

    rtc_set_config(rtc_dev, cfg);

    rtc_datetime rdt;
    memset(&rdt, 0, sizeof(rtc_datetime));

    //Set a standard utc time
    rdt.year = 2017;
    rdt.mon  = 5;
    rdt.day  = 11;
    rdt.hour = 14;
    rdt.min  = 7;
    rdt.sec  = 0;
    rtc_set_datetime(rtc_dev, &rdt);

    PR_DEBUG("Alarm after 5 seconds\n");
    rtc_next_sec(rtc_dev, &rdt, 5);

    rtc_set_alarm(rtc_dev, &rdt);

    while (step < 1)
    {
        //Time_Print();
        //k_sleep(1000);
    }

    Exit();

    return 0;
}
