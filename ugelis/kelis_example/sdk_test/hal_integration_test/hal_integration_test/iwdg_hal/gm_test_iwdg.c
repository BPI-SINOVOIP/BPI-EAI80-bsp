/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <device.h>
#include <gm_hal_iwdg.h>
#include "board.h"
#include <ztest.h>
#include "gm_test_iwdg.h"

#define   RESET_TIME  1000

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
    TC_PRINT("\rDBGCTRL:0X%X\n", SYS_DB_CTRL);
    if (SetVal != 0)
    {
        SYS_DB_CTRL |= SetVal;
        TC_PRINT("\rSet DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
    else
    {
        SYS_DB_CTRL &= ~1;
        TC_PRINT("\rClear DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
}

static int test_iwdg(void)
{
    int err;
    IWDG_HandleTypeDef pHIWDG;
    unsigned int i = 5;

    /* if you didn't Reset bakcup domain,you would W/R iwdg regs*/
    HAL_IWDG_BKD_SW_Reset();
    SysDebugCtlReg(IWDG_DB_CTRL);

    if ((*(volatile uint32_t *)(0x40000004) & (1 << 5)) == (1 << 5))
    {
        *(volatile uint32_t *)(0x40000004) |= (1 << 5);
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    TC_PRINT("Testcase: %s,sysclk:%d,date:%s,time:%s\n", __func__, SYS_CLOCK, __DATE__, __TIME__);
    *(volatile uint32_t *)(0x40000004) |= (1 << 5);
    TC_PRINT("IWDG status bit5:0x%x\n", (*(volatile uint32_t *)(0x40000004) & 0xFF));

    pHIWDG.Instance = IWDG;
    pHIWDG.Init.Prescaler = HAL_IWDG_CLKDIV_FACTOR_256;
    pHIWDG.Init.Reload = (((HAL_IWDG_CLKSRC / 1000) * RESET_TIME) / HAL_IWDG_Get_Div_Value(pHIWDG.Init.Prescaler));

    err = HAL_IWDG_Init(&pHIWDG);
    if (err > 0)
    {
        TC_PRINT("IWDG install error\n");
        return TC_FAIL;
    }
    err = HAL_IWDG_START(&pHIWDG);
    if (err > 0)
    {
        TC_PRINT("IWDG Start error\n");
        return TC_FAIL;
    }
    do
    {
        HAL_IWDG_Refresh(&pHIWDG);
        TC_PRINT("feediwdg\n");
    } while (i--);
    TC_PRINT("Waiting to restart MCU\n");
    while (1)
    {
    }
}

void gm_test_iwdg(void)
{
    zassert_true(test_iwdg() == TC_PASS, NULL);
}
