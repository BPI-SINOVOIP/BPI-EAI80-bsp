/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file Uart_Test.c
 *
 * @version 1.0.0
 *
 * @date 2015/08/21
 *
 * @contents Verification and example code of Cyclic redundancy check (CRC).
 *
 * @history
 *     Date              By              Comments       Ver.
 *  ============    ===============     =========     =========
 *   2016/02/16      Wizephen Wang       Create        1.0.0
 **********************************************************************/

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <soc_gm6721x.h>
#include <gm_hal_lvd.h>
#include <device.h>
#include <watchdog.h>
#include "board.h"
#include <core_cm4.h>


/******************************************************************************
* Constants and macros
******************************************************************************/

/******************************************************************************
* Global variables
******************************************************************************/

#define CMSDK_SYSCTRL_BASE      (0x40000000UL)
#define RST_STATUS_REG          (0x40000004UL)
#define HW32_REG(ADDRESS)       (*((volatile unsigned long  *)(ADDRESS)))

#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printk
#endif

//#define RESET_TEST_CASE_1  // LVD1 RESET TEST
//#define RESET_TEST_CASE_2  // LVD3 RESET TEST
//#define RESET_TEST_CASE_3  // IWDG RESET TEST
//#define RESET_TEST_CASE_4   // WWDG RESET TEST
//#define RESET_TEST_CASE_5   // LOCKUP RESET TEST
//#define RESET_TEST_CASE_6   // SYSRESETREQ RESET TEST

#define CYCLE_RESET_TEST  //CYCLE RESET TEST

#if defined CYCLE_RESET_TEST

    #ifndef RESET_TEST_CASE_3
        #define RESET_TEST_CASE_3
    #endif

    #ifndef RESET_TEST_CASE_4
        #define RESET_TEST_CASE_4
    #endif

    #ifndef RESET_TEST_CASE_5
        #define RESET_TEST_CASE_5
    #endif

    #ifndef RESET_TEST_CASE_6
        #define RESET_TEST_CASE_6
    #endif

#endif

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   (*((volatile unsigned long  *)(REG)) = (VAL))
#define READ_REG(REG)         (*((volatile unsigned long  *)(REG)))

#define __CPU_HALT_DEBUG__    __asm ("bkpt 0")
#define __TEST_CASE_DONE__    do{}while(1);


#if (defined RESET_TEST_CASE_1) || (defined RESET_TEST_CASE_2)
    static int vd1_level = LVD_VD1_Level_7;
    static int vd2_level = LVD_VD2_Level_5;
    static int vd3_level = LVD_VD3_Level_3;

    static int enable_vd1 = 1;
    static int enable_vd2 = 1;
    static int enable_vd3 = 1;

    static int enable_vd1_reset = 1;
    static int enable_vd3_reset = 1;
#endif

#if defined RESET_TEST_CASE_3
#define   reset_time  1000

#define SYS_DB_CTRL                         (*(volatile uint32_t *)(0x40000058))
#define IWDG_DB_CTRL                        1
#define IWDG_STOP_DB_CTRL                   0

/* Debug Ctrl Reg about wdog stoped and runned when cpu halted and running*/
static void SysDebugCtlReg(uint32_t SetVal)
{
    /*
        In case 1 CPU halt,
        SYS_TICK will not active and WDG should stop counting and continue
        while recover from halt condition;
        ==>DBG_WDOG_STOP == 1'b0, WDT continue running;
        ==>DBG_WDOG_STOP == 1'b1, Debug mode ,WDT stoped, you can debug software;
        In case 2 CPU runing,
        SYS_TICK clock will active and WDG counter should continue running;
    */
    printk("\rDBGCTRL:0X%X\n", SYS_DB_CTRL);
    if (SetVal != 0)
    {
        SYS_DB_CTRL |= SetVal;
        printk("\rSet DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
    else
    {
        SYS_DB_CTRL &= ~1;
        printk("\rClear DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
}


/* WDT Requires a callback, there is no interrupt enable / disable. */
//void wdt_example_cb(struct device *dev)
//{
//  printk("watchdog fired\n");
//  wdt_reload(dev);
//}
#endif

#if defined RESET_TEST_CASE_4
#define RESET_TIME     5000
#define WINDOW_TIME    3000

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_example_cb(struct device *dev)
{

    wdt_reload(dev);
    printk("watchdog fired\n");
}
#endif

#if defined RESET_TEST_CASE_5
    #define BAD_ADDRESS   0xC0000001
    //__asm void set_pc(void)
    //{
    //  LDR   PC,=BAD_ADDRESS

    //}
#endif

#if defined RESET_TEST_CASE_6

void sysresetirq_test(void)
{
    NVIC_SystemReset();
}
#endif

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    /*chip 22 pin144*/
    /*HW32_REG(0x1ffff0a8) = 0xfffffbb2;
     HW32_REG(0x1ffff0ac) = 0xff43ffff;
     HW32_REG(0x1ffff0b0) = 0xff3fffff;*/
    /*chip 14 pin176*/
    HW32_REG(0x1ffff0a8) = 0xfffffbae;
    HW32_REG(0x1ffff0ac) = 0xff7dffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}

//LVD1 LVD3  or POR　RESET TEST
void reset_test_lvd_or_por(void)
{

#if (defined RESET_TEST_CASE_1) || (defined RESET_TEST_CASE_2) //lvd reset test

    LVD_Config_T lvdInfo1, lvdInfo2, lvdInfo3;
    LVD_Config_T *pVd1, *pVd2, *pVd3;

    memset(&lvdInfo1, 0, sizeof(LVD_Config_T));
    memset(&lvdInfo2, 0, sizeof(LVD_Config_T));
    memset(&lvdInfo3, 0, sizeof(LVD_Config_T));

    pVd1 = pVd2 = pVd3 = NULL;

    PR_DEBUG("\n\n****************************\n");
    PR_DEBUG(" lvd reset test start......\n");
    PR_DEBUG("****************************\n");

    if (HAL_LVD_CheckResetStatus())
    {
        PR_DEBUG("(last reset is caused by VOL_DET or POR)\n");
        HAL_LVD_ClearResetStatus();
    }

#if defined RESET_TEST_CASE_1
    lvdInfo1.enable     = enable_vd1;
#endif
    //lvdInfo2.enable       = enable_vd2;
#if defined RESET_TEST_CASE_2
    lvdInfo3.enable     = enable_vd3;
#endif

#if defined RESET_TEST_CASE_1
    lvdInfo1.detectionLevel = vd1_level;
#endif

    //lvdInfo2.detectionLevel = vd2_level;
#if defined RESET_TEST_CASE_2
    lvdInfo3.detectionLevel = vd3_level;
#endif

#if defined RESET_TEST_CASE_1
    lvdInfo1.reset          = enable_vd1_reset;
#endif

#if defined RESET_TEST_CASE_1
    lvdInfo3.reset          = enable_vd3_reset;
#endif


    if (lvdInfo1.enable)
    {
        pVd1 = &lvdInfo1;
    }
    if (lvdInfo2.enable)
    {
        pVd2 = &lvdInfo2;
    }
    if (lvdInfo3.enable)
    {
        pVd3 = &lvdInfo3;
    }

    HAL_LVD_Init(pVd1, pVd2, pVd3);

    while (1)
    {
#if defined RESET_TEST_CASE_1
        PR_DEBUG("\n\n****************************\n");
        PR_DEBUG("LVD1 RESET TEST START.....\n");
        PR_DEBUG("****************************\n");
        PR_DEBUG(".^_^.\n");
        k_sleep(100);
#endif

#if defined RESET_TEST_CASE_2
        PR_DEBUG("\n\n****************************\n");
        PR_DEBUG("LVD3 RESET TEST START.....\n");
        PR_DEBUG("****************************\n");
        PR_DEBUG("\n0x40000148 = 0x%x\n", HW32_REG(0x40000148));
        PR_DEBUG("\n0x40000130_11_8 = 0x%x\n", HW32_REG(0x40000130));
        PR_DEBUG("\n0x40000130_7_4 = 0x%x\n", HW32_REG(0x40000130));
        HW32_REG(0x40000148) |= 0x00000300;
        HW32_REG(0x40000130) = 0x00008441;
        k_sleep(50)
        PR_DEBUG(".^_^.\n");
        PR_DEBUG("      .^_^.\n");
        PR_DEBUG("            .^_^.\n");
#endif

    }
#endif

}


//IWDG RESET TEST
void reset_test_iwdg(void)
{
#if (defined RESET_TEST_CASE_3) //iwdg reset test

    PR_DEBUG("\n\n****************************\n");
    PR_DEBUG("IWDG RESET TEST START.....\n");
    PR_DEBUG("****************************\n");
    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    unsigned int i = 5;

    PR_DEBUG("iwdg status bit5:0x%x\n", (*(volatile uint32_t *)(0x40000004) & 0xFF));
    /*clear reset iwdg  status bit*/
    *(volatile uint32_t *)(0x40000004) |= (1 << 5);
    PR_DEBUG("clear iwdg status bit5:0x%x\n", (*(volatile uint32_t *)(0x40000004) & 0xFF));

    SysDebugCtlReg(IWDG_DB_CTRL);
    PR_DEBUG("Start iwdg test,sysclk:%d\n", SYS_CLOCK);
    wdt_dev = device_get_binding("IWDG");//("CONFIG_IWDG_STM32_DEVICE_NAME");
    if (!wdt_dev)
    {
        PR_DEBUG("Cann't get GM-IWDG Device\n");
    }

    wr_cfg.timeout = reset_time;
    wr_cfg.mode = WDT_MODE_RESET;
    wr_cfg.clkdiv = IWDG_CLKDIV_FACTOR_256;
    wr_cfg.interrupt_fn = NULL;

    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_get_config(wdt_dev, &wr_cfg);

    wdt_enable(wdt_dev);

    while (1)
    {
        if (i--)
        {
            wdt_reload(wdt_dev);
            k_sleep(800);
            PR_DEBUG("feediwdg\n");
        }
        if (i == 0)
        {
            PR_DEBUG("waiting for iwdg reset!\r\n");
            while (1)
            {
                k_sleep(50);
                PR_DEBUG(".^_^.");
            }
        }
    }

#endif
}

//wwdg reset test
void reset_test_wwdg(void)
{

#if defined RESET_TEST_CASE_4  //wwdg reset test

    PR_DEBUG("\n\n****************************\n");
    PR_DEBUG("WWDG RESET TEST START.....\n");
    PR_DEBUG("****************************\n");
    struct wdt_config wr_cfg;
    struct device *wdt_dev;

    unsigned int count = 5;
    PR_DEBUG("wwdg status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));
    /*clear reset iwdg  status bit*/
    *(volatile uint32_t *)(0x40000004) |= (1 << 1);
    PR_DEBUG("clear wwdg status bit1:0x%x\n", (*(volatile uint32_t *)(0x40000004)));

    PR_DEBUG("Start wwdg test,sysclk:%d\n", SYS_CLOCK);
    k_sleep(1000);

    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        PR_DEBUG("Cann't get GM-WWDG Device\n");
    }

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.windowtimeout = WINDOW_TIME;
    wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
    wr_cfg.workmode = WWDG_NORMAL_MODE;
    wr_cfg.clkdiv = WWDG_CLKDIV_FACTOR_0;
    wr_cfg.winLen = 4;
    wr_cfg.interrupt_fn = wdt_example_cb;
    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_enable(wdt_dev);

    while (1)
    {
        if (count--)
        {
            PR_DEBUG("waiting for .....!\n");
            k_sleep(1000);
            /*if you already had configed WWDG support EWI interrupt
              in makemenuconfig,the flow cann't run */
#ifndef CONFIG_WWDG_GM_INTERUPT_DRIVEN
            PR_DEBUG("feedwdg\n");
            wdt_reload(wdt_dev);
#endif
        }
        else
        {
            PR_DEBUG("waiting for wwdg reset!\r\n");
            while (1)
            {
                k_sleep(50);
                PR_DEBUG(".^_^.");
            }
        }
    }
#endif

}

//lockup reset test
extern void set_err_pc();
void reset_test_lockup()
{

#if defined RESET_TEST_CASE_5 //lockup reset test
    PR_DEBUG("\n\n****************************\n");
    PR_DEBUG("LOCKUP RESET TEST START.....\n");
    PR_DEBUG("****************************\n");


    PR_DEBUG("\n0x40000014 = 0x%x\n", HW32_REG(0x40000014));
    //lockup enable
    //HW32_REG(0x40000004) |= 0x10;
    HW32_REG(0x40000014) |= 0x10;

    HW32_REG(0x400000BC) |= 0x4000;
    PR_DEBUG("\n0x40000014 = 0x%x\n", HW32_REG(0x40000014));

    PR_DEBUG("\n0x40000004 = 0x%x\n", HW32_REG(0x40000004));

    PR_DEBUG("\n-------0x40000014 = 0x%x\n", HW32_REG(0x40000014));

    set_err_pc();


    PR_DEBUG("\n------->>>>>>0x40000014 = 0x%x\n", HW32_REG(0x40000014));

    while (1);
#endif

}

void set_err_pc()
{
    void (*set_pc)(void);
    //set_pc = ((void(*)(void))(BAD_ADDRESS));
    //set_pc = NULL;

    set_pc = ((void(*)(void))(0xffffffff));
    set_pc();

}



//sysresetirq reset test
void reset_test_sysresetirp(void)
{
#if defined RESET_TEST_CASE_6 //sysresetirq reset test
    PR_DEBUG("\n\n****************************\n");
    PR_DEBUG("SYSRESETREQ RESET TEST START.....\n");
    PR_DEBUG("****************************\n");
    sysresetirq_test();
    k_sleep(100);
#endif
}

int main(void)
{

    patch_config();
    PR_DEBUG("\n\n");
    PR_DEBUG("\n\n****************************\n");
    PR_DEBUG(" ----reset test start... -----\n");
    PR_DEBUG("****************************\n");
#if defined CYCLE_RESET_TEST
    unsigned char reset_cause;
    reset_cause = READ_REG(RST_STATUS_REG) & 0xFF;
    PR_DEBUG("\n******reset_cause_1 = %x ********\n", reset_cause);
    PR_DEBUG("\n******reset_cause_2 = %x ********\n", reset_cause);
    PR_DEBUG("\n******reset_cause_3 = %x ********\n", reset_cause);
    if ((reset_cause & 0x01) == 0x01)
    {
        WRITE_REG(RST_STATUS_REG, READ_REG(RST_STATUS_REG));
        reset_test_wwdg();
    }
    else if ((reset_cause & 0x02) == 0x02)
    {
        WRITE_REG(RST_STATUS_REG, READ_REG(RST_STATUS_REG));
        reset_test_lockup();
    }
    else if ((reset_cause & 0x04) == 0x04)
    {
        WRITE_REG(RST_STATUS_REG, READ_REG(RST_STATUS_REG));
        reset_test_sysresetirp();
    }
    else if ((reset_cause & 0x08) == 0x08)
    {
        WRITE_REG(RST_STATUS_REG, READ_REG(RST_STATUS_REG));
        reset_test_iwdg();
    }
    else if ((reset_cause & 0x20) == 0x20)
    {
        WRITE_REG(RST_STATUS_REG, READ_REG(RST_STATUS_REG));
        reset_test_wwdg();
    }
#else

#if (defined RESET_TEST_CASE_1) || (defined RESET_TEST_CASE_2)
    reset_test_lvd_or_por();
#endif

#if defined RESET_TEST_CASE_3
    reset_test_iwdg();
#endif

#if defined RESET_TEST_CASE_4
    reset_test_wwdg();
#endif

#if defined RESET_TEST_CASE_5
    reset_test_lockup();
#endif

#if defined RESET_TEST_CASE_6
    reset_test_sysresetirp();
#endif

#endif

    __TEST_CASE_DONE__;
    return 0;
}


