/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          rtc_demo.c
 *
 * @author        yangy
 *
 * @version       1.0.0
 *
 * @date          2018/06/14
 *
 * @brief         RTC Demo Calls RTC Driver
 *
 * @note
 *    2018/06/14, yangy, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_rtc.h>
#include <ugelis.h>

#include <device.h>
#include <watchdog.h>
#include "board.h"
#include <misc/printk.h>
#include "gm_hal_wwdg.h"

#include <ugelis.h>

#include <device.h>
#include <watchdog.h>
#include "board.h"
#include <misc/printk.h>


#define GM6721_BUSMATRIX_TEST_CASE_1         0 /*Async test, CPU = FCLK 48MHz, RTC2 = LOSC 32KHz, CPU Hz > 10 * RTC Hz*/
#define GM6721_BUSMATRIX_TEST_CASE_2         0 /*Async test, CPU = FCLK 48MHz, RTC2 = FCLK, CPU Hz = RTC Hz*/
#define GM6721_BUSMATRIX_TEST_CASE_3         0 /*Not Test, Async test, 10 * CPU can not be less than RTC2 clock Hz*/
#define GM6721_BUSMATRIX_TEST_CASE_4         1 /*Async test, CPU = FCLK 48MHz, WDT = LIRC 32KHz, CPU Hz > 10 * RTC Hz*/
#define GM6721_BUSMATRIX_TEST_CASE_5         1 /*Async test, CPU = LIRC, WDT = LIRC, CPU Hz = WDT Hz*/
#define GM6721_BUSMATRIX_TEST_CASE_6         1 /*Not Test, Async test, 10 * CPU can not be less than WDT clock Hz*/
#define GM6721_BUSMATRIX_TEST_CASE_7         1


/*-----------------------
 * Print macros
 *-----------------------*/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
//0x4000904C low 16 bit, for rtc test

#define WDOG_DEV                            ((WDOG_Device_T *)WWDG_BASE) /* WDOG Device Pointer */
#define SYS_RESETSTA                        (*(volatile uint32_t *)(0x40000004))
#define SYS_SRS_WDOG                        ((uint32_t)0x00000002)            /* System Reset by Watchdog Module */


#define SYS_DB_CTRL                         (*(volatile uint32_t *)(0x40000058))

#define RESET_TIME     5000
#define WINDOW_TIME    3000
#define WDOG_TEST_WINCLOSE_TIME  1000


#define NORMAL_WIN_EWI_CASE1          1
#define WIN_EWI_CASE2                 0
#define WIN_EWI_CASE3                 0
#define NORMALEWI_WINEWI_CASE4_RESET  0

#define SYS_DGB_WDOG_WRITEDONE                 ((uint32_t)0x00010000)
#define WDOG_WAIT_WRITEDONE()               do{while((*(volatile uint32_t *)(0x40000054)) & SYS_DGB_WDOG_WRITEDONE);}while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
//#define WDOG_WAIT_WRITEDONE()               do{ }while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define WDOG_UNLOCK(pWDOG)                  do{WDOG_WAIT_WRITEDONE();pWDOG->KR = 0x1acce551;}while(0u)          /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define WDOG_LOCK(pWDOG)                    do{WDOG_WAIT_WRITEDONE();pWDOG->KR = 0;}while(0u)          /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */

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

void cpuGT10rtc()  //CPU clock > 10 * RTC clock
{
    /*Set LOSC ON*/
    *(volatile uint32_t *)0x4000017c |= 0x01;
    /*wait LOSC stable */
    while (!(*(volatile uint32_t *)(0x400001a4) & (1 << 3)));
    /*set LOSC as RTC source clock*/
    *(volatile uint32_t *)0x40000098 &= ~(3 << 0);
    *(volatile uint32_t *)0x40000098 |= 0x01;
}

void cpuEQrtc()   //CPU clock == RTC clock
{
    /*Set HOSC ON*/
    *(volatile uint32_t *)0x40000178 |= 0x01;
    /*wait HOSC stable */
    while (!(*(volatile uint32_t *)(0x400001a4) & (1 << 2)));
    /*set HOSC/HOSC_DIV_RTC as RTC source clock, and HOSC_DIV_RTC == 1*/
    *(volatile uint32_t *)0x40000098 &= ~(3 << 0);
    *(volatile uint32_t *)0x40000098 |= 0x02;
}

void cpu10LTrtc()   //10CPU clock < RTC clock
{
    /*Set HOSC ON*/
    *(volatile uint32_t *)0x40000178 |= 0x01;
    /*wait HOSC stable */
    while (!(*(volatile uint32_t *)(0x400001a4) & (1 << 2)));
    /*Set HOSC as CPU clock*/
    *(volatile uint32_t *)0x40000090 &= ~(7 << 0);
    *(volatile uint32_t *)0x40000090 |= 0x01;
    /*set CPU clock prescaler 12, assume 0x40000090 bit[12:4] is 0*/
    *(volatile uint32_t *)0x40000090 &= ~(0x1ff << 4);
    *(volatile uint32_t *)0x40000090 |= (0xB << 4);
    /*set HOSC/HOSC_DIV_RTC as RTC source clock, and HOSC_DIV_RTC == 1*/
    *(volatile uint32_t *)0x40000098 &= ~(3 << 0);
    *(volatile uint32_t *)0x40000098 |= 0x02;
}

void cpuGT10wdt()  // CPU clock > 10 * WDT
{
    /*Set HOSC ON*/
    *(volatile uint32_t *)0x40000178 |= 0x01;
    /*wait HOSC stable */
    while (!(*(volatile uint32_t *)(0x400001a4) & (1 << 2)));
    /*Set HOSC as CPU clock*/
    *(volatile uint32_t *)0x40000090 &= ~(7 << 0);
    *(volatile uint32_t *)0x40000090 |= 0x01;
    /*Set WWDT clock prescaler: 12  */
    *(volatile uint32_t *)0x40000094 &= ~(0x1ff << 4);
    *(volatile uint32_t *)0x40000094 |= (0xB << 4);
}

void cpuEQwdt() //CPU clock == WDT clock
{
    /*Set HOSC ON*/
    *(volatile uint32_t *)0x40000178 |= 0x01;
    /*wait HOSC stable */
    while (!(*(volatile uint32_t *)(0x400001a4) & (1 << 2)));
    /*Set HOSC as CPU clock*/
    *(volatile uint32_t *)0x40000090 &= ~(7 << 0);
    *(volatile uint32_t *)0x40000090 |= 0x01;
    /*Set WWDT clock prescaler: 1  */
    *(volatile uint32_t *)0x40000094 &= ~(0x1ff << 4);
    *(volatile uint32_t *)0x40000094 |= (0x000 << 4);
}

void cpuLT10wdt()   // CPU clock < WDT clock/10
{
    /*Set HOSC ON*/
    *(volatile uint32_t *)0x40000178 |= 0x01;
    /*wait HOSC stable */
    while (!(*(volatile uint32_t *)(0x400001a4) & (1 << 2)));
    /*Set HOSC as CPU clock*/
    *(volatile uint32_t *)0x40000090 &= ~(7 << 0);
    *(volatile uint32_t *)0x40000090 |= 0x01;
    /*Set CPU prescaler 12*/
    *(volatile uint32_t *)0x40000090 &= ~(0x1ff << 4);
    *(volatile uint32_t *)0x40000090 |= (0xB << 4);
    /*Set WWDT clock prescaler: 1  */
    *(volatile uint32_t *)0x40000094 &= ~(0x1ff << 4);
    *(volatile uint32_t *)0x40000094 |= (0x000 << 4);
}

void Time_Print(void)
{
    UTCTime_T *pUtcTime;
    pUtcTime = HAL_RTC_GetTime();
    PR_DEBUG("%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
             pUtcTime->year, pUtcTime->mon, pUtcTime->day,
             pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
}

void Exit(void)
{
    HAL_RTC_StopTimer();
    HAL_RTC_Stop();
    HAL_RTC_DeInit();
    PR_DEBUG("bye\n");
}

/* will be called upon expiration of the timer or the alarm */
void Callback(int type)
{
    static int cnt = 0;
    if (type == HAL_RTC_ALARM)
    {
        Timer_T timer;
        uint32_t cycle = 1;

        cycle = 1;
        PR_DEBUG("now start the timer every %ds\n", cycle);
        timer.cycle.tv_sec  = cycle;
        timer.cycle.tv_msec = 0;
        HAL_RTC_CancelAlarm();
        HAL_RTC_StartTimer(&timer);
    }
    else if (type == HAL_RTC_TIMER)
    {
        Time_Print();
        if (cnt++ == 40)
        {
            cnt = 0;
            step++;
            HAL_RTC_StopTimer();
            PR_DEBUG("now start next step\n");

        }
        else if (cnt == 10)
        {
            Timer_T timer;
            uint32_t cycle = 50;
            PR_DEBUG("now reset the timer every %dms\n", cycle);
            timer.cycle.tv_sec  = 0;
            timer.cycle.tv_msec = cycle;

            HAL_RTC_StartTimer(&timer);
        }
    }
}

int gm6721_busmartrix_testcase_rtc(void)
{
    UTCTime_T utcTime;
    Timer_Callback callback = Callback;
    HAL_RTC_Config_T rtcConfig;
    int dat = 0;
#define YEAR    2017
#define MON     5
#define DAY     11
#define HOUR    14
#define MINUTE  7
#define SEC     0
    PR_DEBUG("--------------\n");
    PR_DEBUG("RTC Demo start\n");
    PR_DEBUG("--------------\n");
    PR_DEBUG("\n");

    rtcConfig.alarmCallback = callback;
    rtcConfig.timerCallback = callback;

    rtcConfig.clock         = HAL_RTC_CLOCK_LOSC;

    HAL_RTC_Init(&rtcConfig);

    /* RTC works now */
    HAL_RTC_Start();
    HAL_RTC_Unlock();

#if 0
    HAL_RTC_SetDateTime(YEAR, MON, DAY, HOUR, MINUTE, SEC);

    PR_DEBUG("Alarm after 5 seconds\n");
    memset(&utcTime, 0, sizeof(UTCTime_T));
    utcTime.year = YEAR;
    utcTime.mon  = MON;
    utcTime.day  = DAY;
    utcTime.hour = HOUR;
    utcTime.min  = MINUTE;
    utcTime.sec  = SEC + 5; /* ensure sec < 60 */

    HAL_RTC_SetAlarm(&utcTime);
#endif
    while (1)
    {
        HW32_REG(0x4000904C) = 0x125a;
        dat = HW32_REG(0x4000904C);
        if (dat != 0x125a)
        {
            printf("\r busmartrix rtc test failed ...\n");
        }
        else
        {
            //printf("\rAsync test OK ...\n");
        }
    }
    return 0;
}

static void wwdg_RegPrint(WDOG_Device_T *pDev)
{
    WDOG_Device_T *pWDOG = pDev;
    int dat = 0;
    WDOG_UNLOCK(pWDOG);
    printk("\r load:0x%x\n", pWDOG->LD);

    while (1)
    {
        //HW32_REG(0x4000904C) = 0x125a;
        //dat = HW32_REG(0x4000904C);

        pWDOG->LD = 0x125a;
        dat = pWDOG->LD;
        if (dat != 0x125a)
        {
            printf("\r busmartrix rtc test failed ...\n");
        }
        else
        {
            //printf("\rAsync test OK ...\n");
        }
    }
}

RET_CODE WDOG_GetCountVal(WDOG_Device_T *pWDOG)
{
    uint32_t result_val = 0;

    pWDOG->KR = 0x1acce551;
    result_val =  pWDOG->VAL;
    pWDOG->KR = 0;

    return result_val;
}

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_cb(struct device *dev)
{


#if NORMAL_WIN_EWI_CASE1

    printk("\rCASE1:EWI,the action of INT_CLEAR will be reload LOAD\n");

#endif /*#if NORMAL_WIN_EWI_CASE1*/

#if WIN_EWI_CASE2
    uint32_t nu = 1;
    WDOG_Device_T *pWWDG = WDOG_DEV;


    printk("\rCASE2:WIN_EWI,the action of INT_CLEAR will be reload LOAD\n");
    printk("\r		In closed aero(winload < value < load), WDOG reset was triggeed by FeedWdog\n");

    while ((pWWDG->CTRL & 0X04) == 0x04)
    {
        printk("\r value=0x%x > win_load=0x%x \r\n", WDOG_GetCountVal(pWWDG), pWWDG->WL);
        wdt_reload(dev);
    }
#endif /*WIN_EWI_CASE2*/

#if WIN_EWI_CASE3

    uint32_t nu = 1;
    WDOG_Device_T *pWWDG = WDOG_DEV;

    printk("\rCASE3:WIN_EMI,during wdog int,repeat feedwdog as 3 times\n");

    while ((pWWDG->CTRL & 0X04) == 0x04);
    do
    {
        if ((WDOG_GetCountVal(pWWDG) < pWWDG->WL)
                && (pWWDG->WL < pWWDG->LD))
        {
            printk("\r value0x%x < win_load=0x%x \r\n", WDOG_GetCountVal(pWWDG), pWWDG->WL);
            printk("\r FeedWdog\n");
            printk("\r int cnt:%d \n", nu++);
            wdt_reload(dev);
        }

        if (nu > 3)
        {
            printk("\r After 3 times, return mainloop \n");
        }
    } while (nu <= 3);


#endif /*WIN_EWI_CASE3*/


#if NORMALEWI_WINEWI_CASE4_RESET

    printk("\rCASE4:EWI,the action of INT_CLEAR will be reload LOAD,if you delay 5s ,it will be reset\n");
    while (1);
#endif /*#if NORMALEWI_WINEWI_CASE4_RESET*/


}


void wwdg_test(void)
{

    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    WDOG_Device_T *pWWDG = WDOG_DEV;
    unsigned char  wcnt = 5;
    static uint8_t  nu = 0;
    //wwdg_RegPrint(pWWDG);

    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        printk("Cann't get GM-WWDG Device\n");
    }

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.windowtimeout = WINDOW_TIME;
    wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
    //wr_cfg.workmode = CONFIG_WWDG_GM_MODE;
    wr_cfg.clkdiv = WWDG_CLKDIV_FACTOR_3;
    wr_cfg.winLen = 4;
    wr_cfg.interrupt_fn = wdt_cb;
    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_enable(wdt_dev);

    printk("wr_cfg.workmode %d\n", wr_cfg.workmode);

    wwdg_RegPrint(pWWDG);

}










/*
 * slave get master data [1111..][3333...][55555...][777777...] and check result
 * slave update data to [000...][2222...][44444...][666666...] and then send to master
 */

static void slave_if_test()
{
    int dat = 0;
    //dat = HW32_REG(0x01000000);//spi main flash
    //dat = HW32_REG(0x103fffff-4);

    //dat = HW32_REG(0x1fff0000);//otp
    //dat = HW32_REG(0x1fff1fff-4);

    dat = HW32_REG(0x20000000);//sram0
    dat = HW32_REG(0x20017fff - 4);

    dat = HW32_REG(0x20018000);//sram1
    dat = HW32_REG(0x2001bfff - 4);

    dat = HW32_REG(0x2001c000);//sram2
    dat = HW32_REG(0x2001ffff - 4);

    //dat = HW32_REG(0x20020000);//kdp310
    //dat = HW32_REG(0x200607FF-4); //error

    //dat = HW32_REG(0x21000000);//ccm sram,eror
    //dat = HW32_REG(0x21007FFF-4);

    dat = HW32_REG(0x40010000);//gpioa
    dat = HW32_REG(0x40010fff - 4);

    dat = HW32_REG(0x40011000);//gpiob
    dat = HW32_REG(0x40011fff - 4);

    dat = HW32_REG(0x40012000);//gpioc
    dat = HW32_REG(0x40012fff - 4);

    dat = HW32_REG(0x40013000);//gpiod
    dat = HW32_REG(0x40013fff - 4);

    dat = HW32_REG(0x40014000);//gpioe
    dat = HW32_REG(0x40014fff - 4);

    dat = HW32_REG(0x40015000);//gpiof
    dat = HW32_REG(0x40015fff - 4);

    dat = HW32_REG(0x40016000);//gpiog
    dat = HW32_REG(0x40016fff - 4);

    dat = HW32_REG(0x40017000);//gpioh
    dat = HW32_REG(0x40017fff - 4);

    dat = HW32_REG(0x40018000);//gpioi
    dat = HW32_REG(0x40018fff - 4);

    dat = HW32_REG(0x40019000);//gpioj
    dat = HW32_REG(0x40019fff - 4);

    dat = HW32_REG(0x4001A000);//gpiok
    dat = HW32_REG(0x4001Afff - 4); //error

    dat = HW32_REG(0x40020000);//dma1
    dat = HW32_REG(0x40020fff - 4);

    //dat = HW32_REG(0x40021000);//dma2, no dma2
    //dat = HW32_REG(0x40021FFF-4);     //error

    dat = HW32_REG(0x40024000);//imp
    dat = HW32_REG(0x40024FFF - 4);

    dat = HW32_REG(0x40025000);//vin
    dat = HW32_REG(0x40025FFF - 4);

    dat = HW32_REG(0x40026000);//vout
    dat = HW32_REG(0x40026fff - 4);

    dat = HW32_REG(0x42000000);//io bitbanding
    //dat = HW32_REG(0x43FFFFFF-4); //error

    dat = HW32_REG(0x46000000);//kdp310
    dat = HW32_REG(0x46000FFF - 4);

    dat = HW32_REG(0x46001000);//ahb fifo
    dat = HW32_REG(0x46001FFF - 4);

    dat = HW32_REG(0x40000000);//sys reg
    dat = HW32_REG(0x40000FFF - 4);

    dat = HW32_REG(0x40001000);//eeflash
    dat = HW32_REG(0x40001FFF - 4);

    dat = HW32_REG(0x40002000);//i2c2
    dat = HW32_REG(0x40002FFF - 4);

    dat = HW32_REG(0x40003000);//spi0
    dat = HW32_REG(0x40003FFF - 4);

    dat = HW32_REG(0x40004000);//UART0
    dat = HW32_REG(0x40004FFF - 4);

    dat = HW32_REG(0x40005000);//UART1
    dat = HW32_REG(0x40005FFF - 4);

    dat = HW32_REG(0x40006000);//UART2
    dat = HW32_REG(0x40006FFF - 4);

    dat = HW32_REG(0x40007000);//etm
    //dat = HW32_REG(0x40007FFF-4);//error

    dat = HW32_REG(0x40008000);//wwdog
    //dat = HW32_REG(0x40008ffff-4);//error

    dat = HW32_REG(0x40009000);//rtc
    dat = HW32_REG(0x40009FFF - 4);

    dat = HW32_REG(0x4000A000);//crc
    dat = HW32_REG(0x4000AFFF - 4);

    dat = HW32_REG(0x4000B000);//dac
    dat = HW32_REG(0x4000BFFF - 4);

    dat = HW32_REG(0x4000C000);//adc
    dat = HW32_REG(0x4000Cfff - 4);

    dat = HW32_REG(0x4000D000);//acmp
    dat = HW32_REG(0x4000Dfff - 4);

    dat = HW32_REG(0x4000E000);//i2c0
    dat = HW32_REG(0x4000Efff - 4);

    dat = HW32_REG(0x4000F000);//i2c1
    dat = HW32_REG(0x4000Ffff - 4);

    //dat = HW32_REG(0x40030000);//can
    //dat = HW32_REG(0x40030FFF-4);

    dat = HW32_REG(0x40031000);//spi1
    dat = HW32_REG(0x40031fff - 4);

    dat = HW32_REG(0x40032000);//sai
    dat = HW32_REG(0x40032fff - 4);

    dat = HW32_REG(0x40033000);//spi2
    dat = HW32_REG(0x40033fff - 4);

    dat = HW32_REG(0x40034000);//spi3
    dat = HW32_REG(0x40034fff - 4);

    dat = HW32_REG(0x40035000);//kbi
    dat = HW32_REG(0x40035fff - 4);



    dat = HW32_REG(0x40036000);//timer
    //dat = HW32_REG(0x40036fff-4);//error

    dat = HW32_REG(0x40037000);//pes
    dat = HW32_REG(0x40037fff - 4);

    dat = HW32_REG(0x40038000);//spi4
    dat = HW32_REG(0x40038fff - 4);

    dat = HW32_REG(0x40039000);//always on
    dat = HW32_REG(0x40039fff - 4);

    dat = HW32_REG(0x4003A000);//lp stm
    dat = HW32_REG(0x4003AFFF - 4);

    dat = HW32_REG(0x4003B000);//sdmmc
    dat = HW32_REG(0x4003Bfff - 4);

    dat = HW32_REG(0x4003C000);//spi5
    dat = HW32_REG(0x4003Cfff - 4);



    dat = HW32_REG(0x4003D000);//uart3
    dat = HW32_REG(0x4003Dfff - 4);

    dat = HW32_REG(0x4003E000);//iwdog
    dat = HW32_REG(0x4003EFFF - 4);

    dat = HW32_REG(0x4003F000);//bksram
    dat = HW32_REG(0x4003FFFF - 4);

    dat = HW32_REG(0x50000000);//usb
    dat = HW32_REG(0x5003FFFF - 4);

    //dat = HW32_REG(0x50040000);//trng,error,0x40050000
    //dat = HW32_REG(0x50040FFF-4);//error

    //dat = HW32_REG(0x60000000);//fmc
    //dat = HW32_REG(0x);

    dat = HW32_REG(0x70000000);//nand
    //dat = HW32_REG(0x);

    dat = HW32_REG(0xC0000000);//sdram
}




/*
 * slave get master data [1111..][3333...][55555...][777777...] and check result
 * slave update data to [000...][2222...][44444...][666666...] and then send to master
 */

static void slave_if_test2()
{
    int dat = 0;
    dat = HW32_REG(0x01000000);//spi main flash
    dat = HW32_REG(0x103fffff);

    dat = HW32_REG(0x1fff0000);//otp
    dat = HW32_REG(0x1fff1fff);

    dat = HW32_REG(0x20000000);//sram0
    dat = HW32_REG(0x20017fff);

    dat = HW32_REG(0x20018000);//sram1
    dat = HW32_REG(0x2001bfff);

    dat = HW32_REG(0x2001c000);//sram2
    dat = HW32_REG(0x2001ffff);

    dat = HW32_REG(0x20020000);//kdp310
    //dat = HW32_REG(0x200607FF); //error

    //dat = HW32_REG(0x21000000);//ccm sram,eror
    //dat = HW32_REG(0x21007FFF);

    dat = HW32_REG(0x40010000);//gpioa
    dat = HW32_REG(0x40010FFF);

    dat = HW32_REG(0x40011000);//gpiob
    dat = HW32_REG(0x40011FFF);

    dat = HW32_REG(0x40012000);//gpioc
    dat = HW32_REG(0x40012FFF);

    dat = HW32_REG(0x40013000);//gpiod
    dat = HW32_REG(0x40013fff);

    dat = HW32_REG(0x40014000);//gpioe
    dat = HW32_REG(0x40014FFF);

    dat = HW32_REG(0x40015000);//gpiof
    dat = HW32_REG(0x40015FFF);

    dat = HW32_REG(0x40016000);//gpiog
    dat = HW32_REG(0x40016FFF);

    dat = HW32_REG(0x40017000);//gpioh
    dat = HW32_REG(0x40017FFF);

    dat = HW32_REG(0x40018000);//gpioi
    dat = HW32_REG(0x40018FFF);

    dat = HW32_REG(0x40019000);//gpioj
    dat = HW32_REG(0x40019fff);

    dat = HW32_REG(0x4001A000);//gpiok
    //dat = HW32_REG(0x4001AFFF); error

    dat = HW32_REG(0x40020000);//dma1
    dat = HW32_REG(0x40020FFF);

    dat = HW32_REG(0x40021000);//dma2
    //dat = HW32_REG(0x40021FFF);   error

    //dat = HW32_REG(0x40024000);//imp
    //dat = HW32_REG(0x40024FFF);

    //dat = HW32_REG(0x40025000);//vin
    //dat = HW32_REG(0x40025FFF);

    //dat = HW32_REG(0x40026000);//vout
    //dat = HW32_REG(0x40026fff);

    dat = HW32_REG(0x42000000);//io bitbanding
    //dat = HW32_REG(0x43FFFFFF);

    //dat = HW32_REG(0x46000000);//kdp310
    //dat = HW32_REG(0x46000FFF);

    //dat = HW32_REG(0x46001000);//ahb fifo
    //dat = HW32_REG(0x46001FFF);

    dat = HW32_REG(0x40000000);//sys reg
    dat = HW32_REG(0x40000FFF);

    dat = HW32_REG(0x40001000);//eeflash
    dat = HW32_REG(0x40001FFF);

    dat = HW32_REG(0x40002000);//i2c2
    dat = HW32_REG(0x40002FFF);

    dat = HW32_REG(0x40003000);//spi0
    dat = HW32_REG(0x40003FFF);

    dat = HW32_REG(0x40004000);//UART0
    dat = HW32_REG(0x40004FFF);

    dat = HW32_REG(0x40005000);//UART1
    dat = HW32_REG(0x40005FFF);

    dat = HW32_REG(0x40006000);//UART2
    dat = HW32_REG(0x40006FFF);

    dat = HW32_REG(0x40007000);//etm
    dat = HW32_REG(0x40007FFF);

    dat = HW32_REG(0x40008000);//wwdog
    dat = HW32_REG(0x40008ffff);

    dat = HW32_REG(0x40009000);//rtc
    dat = HW32_REG(0x40009FFF);

    dat = HW32_REG(0x4000A000);//crc
    dat = HW32_REG(0x4000AFFF);

    dat = HW32_REG(0x4000B000);//dac
    dat = HW32_REG(0x4000BFFF);

    dat = HW32_REG(0x4000C000);//adc
    dat = HW32_REG(0x4000Cfff);

    dat = HW32_REG(0x4000D000);//acmp
    dat = HW32_REG(0x4000Dfff);

    dat = HW32_REG(0x4000E000);//i2c0
    dat = HW32_REG(0x4000Efff);

    dat = HW32_REG(0x4000F000);//i2c1
    dat = HW32_REG(0x4000Ffff);

    dat = HW32_REG(0x40030000);//can
    dat = HW32_REG(0x40030FFF);

    dat = HW32_REG(0x40031000);//spi1
    dat = HW32_REG(0x40031fff);

    dat = HW32_REG(0x40032000);//sai
    dat = HW32_REG(0x40032fff);

    dat = HW32_REG(0x40033000);//spi2
    dat = HW32_REG(0x40033fff);

    dat = HW32_REG(0x40034000);//spi3
    dat = HW32_REG(0x40034fff);

    dat = HW32_REG(0x40035000);//kbi
    dat = HW32_REG(0x40035fff);



    dat = HW32_REG(0x40036000);//timer
    //dat = HW32_REG(0x40036fff);

    dat = HW32_REG(0x40037000);//pes
    dat = HW32_REG(0x40037fff);

    dat = HW32_REG(0x40038000);//spi4
    dat = HW32_REG(0x40038fff);

    dat = HW32_REG(0x40039000);//always on
    dat = HW32_REG(0x40039fff);

    dat = HW32_REG(0x4003A000);//lp stm
    dat = HW32_REG(0x4003AFFF);

    dat = HW32_REG(0x4003B000);//sdmmc
    dat = HW32_REG(0x4003Bfff);

    dat = HW32_REG(0x4003C000);//spi5
    dat = HW32_REG(0x4003Cfff);



    dat = HW32_REG(0x4003D000);//uart3
    dat = HW32_REG(0x4003Dfff);

    dat = HW32_REG(0x4003E000);//iwdog
    dat = HW32_REG(0x4003EFFF);

    dat = HW32_REG(0x4003F000);//bksram
    dat = HW32_REG(0x4003FFFF);

    //dat = HW32_REG(0x50000000);//usb
    //dat = HW32_REG(0x5003FFFF);

    //dat = HW32_REG(0x50040000);//trng
    //dat = HW32_REG(0x50040FFF);

    //dat = HW32_REG(0x60000000);//fmc
    //dat = HW32_REG(0x);

    //dat = HW32_REG(0x70000000);//nand
    //dat = HW32_REG(0x);

    //dat = HW32_REG(0xC0000000);//sdram
}


#define Reserved_Size            0x00007C00
#define Reserved_MEM             Reserved_Size
#define SRAM_REMAP_SIZE          0x00020000
#define FLASH_REMAP_SIZE         0x00008000
#define INFOPAGE_REMAP_SIZE      0x00000180
#define BTRM_REMAP_SIZE          0x00002000
#define FLASH_PHYSICAL_ADDR      0x10000000
#define SRAM_PHYS_ADDR           0x20000000
#define INFOPAGE_PHYS_ADDR       0x1FFFF080
#define BTRM_PHYS_ADDR           0x1FFF0000

int remap_test()
{
    uint32_t WriteAddr_0 = 0;
    uint32_t ReadAddr_0  = 0;
    uint32_t WriteAddr_1 = 0;
    uint32_t ReadAddr_1  = 0;
    int      i           = 0;
    /* remap sram  to 0x00000000*/
    *(volatile uint32_t *)0x40000014 = 0x1ACC0000;
    WriteAddr_0 = 0x00000000;
    WriteAddr_1 = 0x20000000;

    printf("*** sram remap test......\n");
    for (WriteAddr_0 = 0x00000000; WriteAddr_0 < SRAM_REMAP_SIZE;)
    {
        if (*(uint32_t *)(WriteAddr_0) != *(uint32_t *)(WriteAddr_1))
        {
            printf("*** sram remap error......\n");
            return 1;
        }
        WriteAddr_0 = WriteAddr_0 + 4;
        WriteAddr_1 = WriteAddr_1 + 4;
    }

    /* remap flash to 0x00000000*/
    f
    *(volatile uint32_t *)0x40000014 = 0x1ACC0003;
    WriteAddr_0 = 0x00000000;
    WriteAddr_1 = 0x10000000;

    printf("*** flash remap test......\n");
    for (WriteAddr_0 = 0x00000000; WriteAddr_0 < FLASH_REMAP_SIZE;)
    {
        if (*(uint32_t *)(WriteAddr_0) != *(uint32_t *)(WriteAddr_1))
        {
            printf("*** sram remap error......\n");
            return 1;
        }
        WriteAddr_0 = WriteAddr_0 + 4;
        WriteAddr_1 = WriteAddr_1 + 4;
    }

    /* remap sdram to 0x00000000*/

    /* remap flash to 0x00000000*/
}

int main(void)
{
    *(volatile uint32_t *)0x40000080 = 0xFFFFFFFF;
    *(volatile uint32_t *)0x40000084 = 0xFFFFFFFF;
    *(volatile uint32_t *)0x40000088 = 0xFFFFFFFF;
    *(volatile uint32_t *)0x40000070 = 0xFFFFFFFF;
    *(volatile uint32_t *)0x40000074 = 0xFFFFFFFF;
    *(volatile uint32_t *)0x40000078 = 0xFFFFFFFF;
    remap_test();
#if GM6721_BUSMATRIX_TEST_CASE_1
    //cpuGT10rtc();
    gm6721_busmartrix_testcase_rtc();
#endif

#if GM6721_BUSMATRIX_TEST_CASE_2
    //cpuEQrtc();
    gm6721_busmartrix_testcase_rtc();
#endif

#if GM6721_BUSMATRIX_TEST_CASE_3
    //cpu10LTrtc();
    gm6721_busmartrix_testcase_rtc();
#endif

#if GM6721_BUSMATRIX_TEST_CASE_4
    //cpuGT10wdt();
    wwdg_test();
#endif

#if GM6721_BUSMATRIX_TEST_CASE_5
    //cpuEQwdt();
    wwdg_test();
#endif

#if GM6721_BUSMATRIX_TEST_CASE_6
    //cpuLT10wdt();
    wwdg_test();
#endif

#if GM6721_BUSMATRIX_TEST_CASE_7
    //cpuGT10rtc();
    slave_if_test();
#endif

    return 0;
}

