/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file        sysinit.c
 *
 * @author      wen.liu
 *
 * @version     1.0.0
 *
 * @date        2015/08/21
 *
 * @brief       System init routine.
 *
 * @note
 *    2016/05/17, wen.liu, V1.0.0
 *        Initial version.
 *
 *
 */
#include "gm_config.h"

#ifndef RABBIT_NO_OS_SYSTEM
    #include <kernel.h>
    #include <device.h>
    #include <init.h>
    #include <soc.h>
    #include <gm_hal_rpc.h>
#endif

#include <gm_hal_cache.h>
#include <gm_hal_clock.h>

#if defined(CONFIG_GM_HAL_RPC_MASTER) || defined(CONFIG_GM_HAL_RPC_SLAVE)
    #include <gm_hal_rpc.h>
#endif




static void sysClkInit()
{
#if CONFIG_SYS_CLOCK_FROM_INTERNAL
    if (SYS_CLOCK == 8000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
    else if (SYS_CLOCK == 150000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLLKDP150;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
    else if (SYS_CLOCK == 100000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLLKDP100;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
#if defined(CHIP_F6721B)
    else if (SYS_CLOCK == 200000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
    else if (SYS_CLOCK == 172000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLLSAI;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
#endif
#elif CONFIG_SYS_CLOCK_FROM_EXTERNAL
    if (SYS_CLOCK == 150000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLLKDP150;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
    else if (SYS_CLOCK == 100000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLLKDP100;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
#if defined(CHIP_F6721B)
    else if (SYS_CLOCK == 200000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
    else if (SYS_CLOCK == 172000000)
    {
        CLOCK_SysClockConfig_T sysClkConfig;
        sysClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLLSAI;
        sysClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
        HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
        HAL_CLOCK_SYSCLKConfig(&sysClkConfig);
    }
#endif
#endif
}

#ifdef RABBIT_NO_OS_SYSTEM
void relocate_vector_table(void)
{
#ifdef CONFIG_SRAM_BOOT
    SCB->VTOR = 0x20000000 & SCB_VTOR_TBLOFF_Msk;
#endif
#ifdef CONFIG_SDRAM_BOOT
    SCB->VTOR = 0xC0010000 & SCB_VTOR_TBLOFF_Msk;
#endif
#ifdef CONFIG_SRAM_BOOTLOADER/*only bootloader project using*/
    SCB->VTOR = 0x20001000 & SCB_VTOR_TBLOFF_Msk;
#endif
    __DSB();
    __ISB();
}

static void config_fpu(void)
{
    /* set CP10 and CP11 Full Access */
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
}

#endif



/**
 * @brief System initialization function
 *
 * @retval None
 *
 * it is similar to start function
 */

#ifdef RABBIT_NO_OS_SYSTEM
void SystemInit(void)
{

#ifndef CHIP_F6721B
    /*f6721b bypass for temp =todo*/
#ifdef CONFIG_SDRAM_BOOT
    /*code:sdram data:sdram : i/d cache enable*/
    HAL_DCACHE_InvalidAll();
    HAL_DCACHE_Enable();
    HAL_ICACHE_Enable();
    HAL_ICACHE_Invalid();
#elif CONFIG_XIP
    /*code:flash data:sram : icache enbale dcache disable*/
    HAL_ICACHE_Enable();
    HAL_ICACHE_Invalid();
    HAL_DCACHE_Disable();
#else
    /*code:sram data:sram : icache disable dcache disable*/
    HAL_ICACHE_Disable();
    HAL_DCACHE_Disable();
#endif

#ifdef CONFIG_GM_HAL_RPC_MASTER
    /*master cpu boot slave cpu*/
    HAL_RPC_MasterBootSlave();
#endif
#endif

    config_fpu();

    relocate_vector_table();

    sysClkInit();
}

#else
void SystemInit(void)
{
#ifndef CONFIG_GM_HAL_RPC_SLAVE
    /*master cpu go here*/
    sysClkInit();

#ifdef CONFIG_GM_HAL_RPC_MASTER
    /**
     *1:dual cpu enable
     *2:only master cpu here
     */
    HAL_RPC_MasterBootSlave();
#endif

#endif
}

#endif


