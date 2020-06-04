/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          clock.c
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/15
 *
 * @brief         Implementation of Clock Driver.
 *
 * @note
 *    2016/06/27, wizephen.wang, V1.0.0
 *        Initial version.
 *    2016/05/22, wizephen.wang, V1.0.1
 *        Second version.
 *
 */

#include <driver/clock.h>

RET_CODE OSC_SetConfig(OSC_Config_T *pConfig)
{
    ASSERT(pConfig != NULL);

#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
    /* Configure exeternal high oscillator */
    if (pConfig->oscMask & OSC_MASK_EXTERNAL_HIGH)
    {
        if ((pConfig->oscState & OSC_EXTERNAL_HIGH_STATE) == OSC_EXTERNAL_HIGH_ON)
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
#if defined(CHIP_GM6601B)
            SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
#endif
        }
        else
        {
            SYSDEV->OSC |= SYS_OSC_CRY8M;
#if defined(CHIP_GM6601B)
            SYSDEV->ANASEL &= ~(SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
#endif
        }
    }

    /* Configure internal high oscillator */
    if (pConfig->oscMask & OSC_MASK_INTERNAL_HIGH)
    {
        if ((pConfig->oscState & OSC_INTERNAL_HIGH_STATE) == OSC_INTERNAL_HIGH_ON)
        {
            SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
        }
        else
        {
            SYSDEV->IRC |= SYS_IRC_CRY8M;
        }
    }

    /* Configure exeternal low oscillator */
    if (pConfig->oscMask & OSC_MASK_EXTERNAL_LOW)
    {
        if ((pConfig->oscState & OSC_EXTERNAL_LOW_STATE) == OSC_EXTERNAL_LOW_ON)
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY32K);
#if defined(CHIP_GM6601B)
            SYSDEV->ANASEL |= (SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
#endif
        }
        else
        {
            SYSDEV->OSC |= SYS_OSC_CRY32K;
#if defined(CHIP_GM6601B)
            SYSDEV->ANASEL &= ~(SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
#endif
        }
    }

    /* Configure exeternal low oscillator */
    if (pConfig->oscMask & OSC_MASK_INTERNAL_LOW)
    {
        if ((pConfig->oscState & OSC_INTERNAL_LOW_STATE) == OSC_INTERNAL_LOW_ON)
        {
            SYSDEV->IRC &= ~(SYS_IRC_CRY1K);
        }
        else
        {
            SYSDEV->IRC |= SYS_IRC_CRY1K;
        }
    }

    /* Configure PLL reference clock */
    Clock_SetPllReferenceClock(pConfig->pllConfig.referenceClock);

    /* Configure PLL bypass option */
    if (pConfig->pllConfig.pllBypassEnable)
    {
        Clock_PllBypassEnable();
    }
    else
    {
        Clock_PllBypassDisable();
    }
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    /* Configure exeternal high oscillator */
    if (pConfig->oscMask & OSC_MASK_EXTERNAL_HIGH)
    {
        /*Power on HOSC and then make it enabled*/
        if ((pConfig->oscState & OSC_EXTERNAL_HIGH_STATE) == OSC_EXTERNAL_HIGH_ON)
        {
            SYSDEV->OSC |= (SYS_OSC_CRY8M);
            SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
        }
        else
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
            SYSDEV->ANASEL &= ~(SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
        }
    }

    /* Configure internal high oscillator */
    if (pConfig->oscMask & OSC_MASK_INTERNAL_HIGH)
    {
        if ((pConfig->oscState & OSC_INTERNAL_HIGH_STATE) == OSC_INTERNAL_HIGH_ON)
        {
            SYSDEV->IRC |= SYS_IRC_CRY8M;
        }
        else
        {
            SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
        }
    }

    /* Configure exeternal low oscillator */
    if (pConfig->oscMask & OSC_MASK_EXTERNAL_LOW)
    {
        /*Power on LOSC and then make it enabled*/
        if ((pConfig->oscState & OSC_EXTERNAL_LOW_STATE) == OSC_EXTERNAL_LOW_ON)
        {
            SYSDEV->OSC |= SYS_OSC_CRY32K;
            SYSDEV->ANASEL |= (SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
        }
        else
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY32K);
            SYSDEV->ANASEL &= ~(SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
        }
    }

    /* Configure exeternal low oscillator */
    if (pConfig->oscMask & OSC_MASK_INTERNAL_LOW)
    {
        /* Do nothing here, since LIRC is always powered on */
    }

    /* Configure PLL reference clock */
    Clock_SetPllReferenceClock(pConfig->pllConfig.referenceClock);
#endif

#if defined(CHIP_GM6621)
    /*Valid only when LOSC as PLL reference clock, otherwise, it should be cleared*/
    SYSDEV->PLL &= ~(SYS_PLL_DIVSEL_LFEN);

    /*Clear Reset-From_Standby Flag for consequent IP I/O access*/
    if (AOPDEV->SRS & SYS_AOP_SRS_STANDBYFLG)
    {
        AOPDEV->SRS = SYS_AOP_SRS_STANDBYFLG;
    }
    /* Configure exeternal high oscillator */
    if (pConfig->oscMask & OSC_MASK_EXTERNAL_HIGH)
    {
        /*Power on HOSC and then make it enabled*/
        if ((pConfig->oscState & OSC_EXTERNAL_HIGH_STATE) == OSC_EXTERNAL_HIGH_ON)
        {
            SYSDEV->OSC |= (SYS_OSC_CRY8M);
            SYSDEV->OSC |= (SYS_OSC_OUTSEL);
        }
        else
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
            SYSDEV->OSC &= ~(SYS_OSC_OUTSEL);
        }
    }

    /* Configure internal high oscillator */
    if (pConfig->oscMask & OSC_MASK_INTERNAL_HIGH)
    {
        if ((pConfig->oscState & OSC_INTERNAL_HIGH_STATE) == OSC_INTERNAL_HIGH_ON)
        {
            AOPDEV->IRC &= ~(SYS_IRC_CRY8M);
        }
        else
        {
            AOPDEV->IRC |= SYS_IRC_CRY8M;
        }
    }

    /* Configure exeternal low oscillator */
    if (pConfig->oscMask & OSC_MASK_EXTERNAL_LOW)
    {
        /*For the case when both High and Low oscillator exist on board*/
        AOPDEV->LSCCTRL |= SYS_AOP_LSCCTRL_OSCPINSEL;
        /*Power on LOSC and then make it enabled*/
        if ((pConfig->oscState & OSC_EXTERNAL_LOW_STATE) == OSC_EXTERNAL_LOW_ON)
        {
            AOPDEV->LSCCTRL |= SYS_AOP_CRY32K;
            AOPDEV->LSCCTRL |= SYS_OSC_OUTSEL;
        }
        else
        {
            AOPDEV->LSCCTRL &= ~(SYS_AOP_CRY32K);
            AOPDEV->LSCCTRL &= ~(SYS_OSC_OUTSEL);
        }
    }

    /* Configure PLL reference clock */
    Clock_SetPllReferenceClock(pConfig->pllConfig.referenceClock);
#endif

    return RET_OK;
}

RET_CODE OSC_GetConfig(OSC_Config_T *pConfig)
{
    ASSERT(pConfig != NULL);
    pConfig->oscMask = OSC_MASK_EXTERNAL_HIGH | OSC_MASK_INTERNAL_HIGH | OSC_MASK_EXTERNAL_LOW | OSC_MASK_INTERNAL_LOW;
    pConfig->oscState = 0;

#if defined(CHIP_GM6601)
    /* Check whether external high oscillator is available */
    if (SYSDEV->OSC & SYS_OSC_CRY8M)
    {
        pConfig->oscState |= OSC_EXTERNAL_HIGH_ON;
    }

    /* Check whether internal high oscillator is available */
    if (SYSDEV->IRC & SYS_IRC_CRY8M)
    {
        pConfig->oscState |= OSC_INTERNAL_HIGH_ON;
    }

    /* Check whether external low oscillator is available */
    if (SYSDEV->OSC & SYS_OSC_CRY32K)
    {
        pConfig->oscState |= OSC_EXTERNAL_LOW_ON;
    }

    /* Check whether internal low oscillator is available */
    if (SYSDEV->IRC & SYS_IRC_CRY1K)
    {
        pConfig->oscState |= OSC_INTERNAL_LOW_ON;
    }

    /* Check whether PLL bypass is enabled */
    if (SYSDEV->SCS & SYS_SCS_PLL_BYPASS)
    {
        pConfig->pllConfig.pllBypassEnable = 1;
    }
    else
    {
        pConfig->pllConfig.pllBypassEnable = 0;
    }

    /* Check PLL reference source */
    pConfig->pllConfig.referenceClock = Clock_GetPllReferenceClock();
#endif

#if defined(CHIP_GM6601B)
    /* Check whether external high oscillator is available */
    if (!(SYSDEV->OSC & SYS_OSC_CRY8M))
    {
        pConfig->oscState |= OSC_EXTERNAL_HIGH_ON;
    }

    /* Check whether internal high oscillator is available */
    if (!(SYSDEV->IRC & SYS_IRC_CRY8M))
    {
        pConfig->oscState |= OSC_INTERNAL_HIGH_ON;
    }

    /* Check whether external low oscillator is available */
    if (!(SYSDEV->OSC & SYS_OSC_CRY32K))
    {
        pConfig->oscState |= OSC_EXTERNAL_LOW_ON;
    }

    /* Check whether internal low oscillator is available */
    pConfig->oscState |= OSC_INTERNAL_LOW_ON;

    /* Check whether PLL bypass is enabled */
    if (SYSDEV->SCS & SYS_SCS_PLL_BYPASS)
    {
        pConfig->pllConfig.pllBypassEnable = 1;
    }
    else
    {
        pConfig->pllConfig.pllBypassEnable = 0;
    }

    /* Check PLL reference source */
    pConfig->pllConfig.referenceClock = Clock_GetPllReferenceClock();
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    /* Check whether external high oscillator is available */
    if (SYSDEV->OSC & SYS_OSC_CRY8M)
    {
        pConfig->oscState |= OSC_EXTERNAL_HIGH_ON;
    }

    /* Check whether internal high oscillator is available */
    if (SYSDEV->IRC & SYS_IRC_CRY8M)
    {
        pConfig->oscState |= OSC_INTERNAL_HIGH_ON;
    }

    /* Check whether external low oscillator is available */
    if (SYSDEV->OSC & SYS_OSC_CRY32K)
    {
        pConfig->oscState |= OSC_EXTERNAL_LOW_ON;
    }

    pConfig->oscState |= OSC_INTERNAL_LOW_ON;

    /* Check PLL reference source */
    pConfig->pllConfig.referenceClock = Clock_GetPllReferenceClock();
#endif
#if defined(CHIP_GM6621)
    /* Check whether external high oscillator is available */
    if (SYSDEV->OSC & SYS_OSC_CRY8M)
    {
        pConfig->oscState |= OSC_EXTERNAL_HIGH_ON;
    }

    /* Check whether internal high oscillator is available */
    if (!(AOPDEV->IRC & SYS_IRC_CRY8M))
    {
        pConfig->oscState |= OSC_INTERNAL_HIGH_ON;
    }

    /* Check whether external low oscillator is available */
    if (AOPDEV->LSCCTRL & SYS_AOP_CRY32K)
    {
        pConfig->oscState |= OSC_EXTERNAL_LOW_ON;
    }

    /* Check PLL reference source */
    pConfig->pllConfig.referenceClock = Clock_GetPllReferenceClock();
#endif
    return RET_OK;
}

RET_CODE CLOCK_SetConfig(CLOCK_Config_T *pConfig)
{
    ASSERT(pConfig != NULL);

#if defined(CHIP_GM6601)
    /* To ensure a stable clock switch */
    Clock_CoreSwithModuleEnable();
    if (pConfig->clkMask & CLOCK_MASK_SYSCLK)
    {
        /* DO NOT CHANGE THE FOLLOWING CONFIGURATION SEQUENCE !!! */
        /* Ensure a safe latency switch */
        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, 2 << 16);

        /* Configure CPU clock prescaler */
        IO_BITMASK_SET(SYSDEV->SPS, 0x07, pConfig->sysClkPrescaler);

        /* Check whether clock source is stable */
        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_PLL || pConfig->sysClkSource == SYSCLOCK_SOURCE_PLLDIV2)
        {
            while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_OSCLS)
        {
            while (!Clock_IsNowStable(OSC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_CLK8M)
        {
            if (SYSDEV->SCS & PLL_REFERENCE_CLOCK_MASK)
            {
                while (!Clock_IsNowStable(OSC8M_SOURCE_STABLE_MASK))
                {
                    __asm("nop");
                }
            }
            else
            {
                while (!Clock_IsNowStable(IRC8M_SOURCE_STABLE_MASK))
                {
                    __asm("nop");
                }
            }
        }

        /* Configure CPU clock source */
        IO_BITMASK_SET(SYSDEV->SCS, 0x03, pConfig->sysClkSource);

        /* Configure flash read lantency */
        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, pConfig->flashLatency << 16);
    }
#endif


#if  defined(CHIP_GM6601B)

    if (pConfig->clkMask & CLOCK_MASK_SYSCLK)
    {
        /* DO NOT CHANGE THE FOLLOWING CONFIGURATION SEQUENCE !!! */
        /* Ensure a safe latency switch */
        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, 2 << 16);
        /* Configure CPU clock prescaler */
        IO_BITMASK_SET(SYSDEV->SCS, (0x07 << 5), (pConfig->sysClkPrescaler << 5));

        /* Check whether clock source is stable */
        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_PLL48M)
        {
            /*Power on PLL clock*/
            SYSDEV->PLL &= ~(SYS_PLL_PDB);
            while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_LOSC32K)
        {
            while (!Clock_IsNowStable(LOSC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_HOSC8M)
        {
            while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_HIRC8M)
        {
            while (!Clock_IsNowStable(HIRC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_LIRC32K)
        {
            while (!Clock_IsNowStable(LIRC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        /* Clear possible HOSC Loss reset or reset pend flags to avoid
         * CPU reset due to the case when HOSC is used as FCLK or as PLL
         * reference clock
         */
        SYSDEV->SRS = SYS_SRS_HOSC_LSPD | SYS_SRS_HOSC_LSRSTPD;

        /* Configure CPU clock source */
        IO_BITMASK_SET(SYSDEV->SCS, 0x07, pConfig->sysClkSource);

        /* Configure flash read lantency */
        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, pConfig->flashLatency << 16);
    }
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    if (pConfig->clkMask & CLOCK_MASK_SYSCLK)
    {
        /* DO NOT CHANGE THE FOLLOWING CONFIGURATION SEQUENCE !!! */
        /* Ensure a safe latency switch */
        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, 2 << 16);
        /* Configure CPU clock prescaler */
        IO_BITMASK_SET(SYSDEV->CLK0, (0xFF << 4), (pConfig->sysClkPrescaler << 4));

        /* Check whether clock source is stable */
        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_PLL48M)
        {
            /*Power on PLL clock*/
            SYSDEV->PLL |= SYS_PLL_PWRON;
            while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_LOSC32K)
        {
            while (!Clock_IsNowStable(LOSC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_HOSC8M)
        {
            while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_HIRC8M)
        {
            while (!Clock_IsNowStable(HIRC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_LIRC32K)
        {
            while (!Clock_IsNowStable(LIRC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        /* Clear possible HOSC Loss reset or reset pend flags to avoid
         * CPU reset due to the case when HOSC is used as FCLK or as PLL
         * reference clock
         */
        SYSDEV->SRS = SYS_SRS_HOSC_LSPD | SYS_SRS_HOSC_LSRSTPD;
        /* Configure CPU clock source */
        IO_BITMASK_SET(SYSDEV->CLK0, 0x07, pConfig->sysClkSource);

        /* Configure flash read lantency */
        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, pConfig->flashLatency << 16);
    }
#endif

#if defined(CHIP_GM6621)
    if (pConfig->clkMask & CLOCK_MASK_SYSCLK)
    {
        /* DO NOT CHANGE THE FOLLOWING CONFIGURATION SEQUENCE !!! */
        /* Ensure a safe latency switch */
        //IO_BITMASK_SET(FLASH->CTRL, 3 << 16, 2 << 16); /* richard debug */
        /* Configure CPU clock prescaler */
        IO_BITMASK_SET(SYSDEV->CLK0, 0xFF0, pConfig->sysClkPrescaler);

        /* Check whether clock source is stable */
        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_PLL48M)
        {
            /*Power on PLL clock*/
            SYSDEV->PLL |= SYS_PLL_PWRON;
            while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }


        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_LOSC32K)
        {
            while (!Clock_IsNowStable(OSC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_HOSC8M)
        {
            while (!Clock_IsNowStable(OSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_HIRC8M)
        {
            while (!Clock_IsNowStable(IRC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->sysClkSource == SYSCLOCK_SOURCE_LIRC32K)
        {
            while (!Clock_IsNowStable(IRC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        /* Clear possible HOSC Loss reset or reset pend flags to avoid
         * CPU reset due to the case when HOSC is used as FCLK or as PLL
         * reference clock
         */
        SYSDEV->SRS = SYS_SRS_HOSC_LSPD;
        /* Configure CPU clock source */
        IO_BITMASK_SET(SYSDEV->CLK0, 0x07, pConfig->sysClkSource);

        /* Configure flash read lantency */
        //        IO_BITMASK_SET(FLASH->CTRL, 3 << 16, pConfig->flashLatency << 16); // richard debug
    }
#endif
    return RET_OK;
}

RET_CODE CLOCK_GetConfig(CLOCK_Config_T *pConfig)
{
    ASSERT(pConfig != NULL);

#if defined(CHIP_GM6601)
    pConfig->clkMask = CLOCK_MASK_SYSCLK;

    /*Get CPU clock prescaler*/
    pConfig->sysClkPrescaler = (CLOCK_Prescaler_T)(SYSDEV->SPS & 0x07);

    /*Get CPU clock source*/
    pConfig->sysClkSource    = (CLOCK_SysClockSource_T)(SYSDEV->SCS & 0x03);

    /*Get flash read lantency*/
    pConfig->flashLatency    = (FLASH_LATNECY_T)(((FLASH->CTRL) >> 16) & 0x03);
#endif

#if defined(CHIP_GM6601B)
    pConfig->clkMask = CLOCK_MASK_SYSCLK;

    /*Get CPU clock prescaler*/
    pConfig->sysClkPrescaler = (CLOCK_Prescaler_T)(SYSDEV->SCS >> 5 & 0x07);

    /*Get CPU clock source*/
    pConfig->sysClkSource    = (CLOCK_SysClockSource_T)(SYSDEV->SCS & 0x03);

    /*Get flash read lantency*/
    pConfig->flashLatency    = (FLASH_LATNECY_T)(((FLASH->CTRL) >> 16) & 0x03);
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    pConfig->clkMask = CLOCK_MASK_SYSCLK;

    /*Get CPU clock prescaler*/
    pConfig->sysClkPrescaler = (CLOCK_Prescaler_T)((SYSDEV->CLK0 >> 4) & 0xFF);

    /*Get CPU clock source*/
    pConfig->sysClkSource    = (CLOCK_SysClockSource_T)(SYSDEV->CLK0 & 0x07);

    /*Get flash read lantency*/
    pConfig->flashLatency    = (FLASH_LATNECY_T)(((FLASH->CTRL) >> 16) & 0x03);
#endif

    return RET_OK;
}

RET_CODE PeriphClk_SetConfig(PeriphClk_Config_T *pConfig)
{
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B))
    /*First check the CPU clock is 48MHz*/
    if ((pConfig->periphClkMask & CLOCK_PERIPH_ETM_MASK) == CLOCK_PERIPH_ETM_MASK)
    {
        /*Configure clock for etm*/
        CLOCK_PERIPH_ETM_CONFIG(pConfig->etmClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_ADC_MASK) == CLOCK_PERIPH_ADC_MASK)
    {
        /*Configure clock for adc*/
        CLOCK_PERIPH_ADC_CONFIG(pConfig->adcClock);
    }

#if defined(CHIP_GM6601)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_WDT_MASK) == CLOCK_PERIPH_WDT_MASK)
    {
        /*To ensure a stable clock for Watchdog*/
        Clock_WdtClkSwithModuleEnable();

        /*Configure clock for watchdog*/
        if (pConfig->wdtClock == WDT_CLOCK_IRCLS)
        {
            while (!Clock_IsNowStable(IRC1K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->wdtClock == WDT_CLOCK_OSCLS)
        {
            while (!Clock_IsNowStable(OSC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->wdtClock == WDT_CLOCK_PLLSRC)
        {
            /*Check whether PLL is powered down*/
            if (SYSDEV->PLL & SYS_PLL_PDB)
            {
                do {} while (1);
            }
            else
            {
                while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
                {
                    __asm("nop");
                }
            }
        }

        CLOCK_PERIPH_WDT_CONFIG(pConfig->wdtClock);
    }
#endif

#if defined(CHIP_GM6601B)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_WDT_MASK) == CLOCK_PERIPH_WDT_MASK)
    {
        if (pConfig->wdtClock == WDT_CLOCK_LIRC)
        {
            while (!Clock_IsNowStable(LIRC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->wdtClock == WDT_CLOCK_LOSC)
        {
            /*Power on LOSC*/
            SYSDEV->OSC &= ~(SYS_OSC_CRY32K);
            SYSDEV->ANASEL |= (SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
            while (!Clock_IsNowStable(LOSC32K_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        if (pConfig->wdtClock == WDT_CLOCK_HIRC)
        {
            /*Power on HIRC*/
            SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
            while (!Clock_IsNowStable(HIRC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }

        CLOCK_PERIPH_WDT_CONFIG(pConfig->wdtClock);
    }
#endif

#if defined(CHIP_GM6601)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC2_MASK) == CLOCK_PERIPH_RTC2_MASK)
    {
        /* To ensure a stable clock for RTC2 */
        Clock_Rtc2ClkSwithModuleEnable();
        /* Configure clock for rtc2 */
        CLOCK_PERIPH_RTC2_CONFIG(pConfig->rtc2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC1_MASK) == CLOCK_PERIPH_RTC1_MASK)
    {
        /* To ensure a stable clock for RTC1 */
        Clock_Rtc1ClkSwithModuleEnable();
        /* Configure clock for rtc1 */
        CLOCK_PERIPH_RTC1_CONFIG(pConfig->rtc1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC0_MASK) == CLOCK_PERIPH_RTC0_MASK)
    {
        /* To ensure a stable clock for RTC0 */
        Clock_Rtc0ClkSwithModuleEnable();
        /* Configure clock for rtc0 */
        CLOCK_PERIPH_RTC0_CONFIG(pConfig->rtc0Clock);
    }
#endif

#if defined(CHIP_GM6601B)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC2_MASK) == CLOCK_PERIPH_RTC2_MASK)
    {
        if (pConfig->rtc2Clock == RTC2_CLOCK_PLLSRC)
        {
            /*Check PLL reference clock*/
            if (Clock_GetPllReferenceClock() == PLL_OSC_HSCLK)
            {
                SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
                SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
                while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
                {
                    __asm("nop");
                }
            }
            else
            {
                SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
                while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
                {
                    __asm("nop");
                }
            }
        }
        /* Configure clock for rtc2 */
        CLOCK_PERIPH_RTC2_CONFIG(pConfig->rtc2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC1_MASK) == CLOCK_PERIPH_RTC1_MASK)
    {
        /*Check PLL reference clock*/
        if (Clock_GetPllReferenceClock() == PLL_OSC_HSCLK)
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
            SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
            while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }
        else
        {
            SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
            while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }
        /* Configure clock for rtc1 */
        CLOCK_PERIPH_RTC1_CONFIG(pConfig->rtc1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC0_MASK) == CLOCK_PERIPH_RTC0_MASK)
    {
        /*Check PLL reference clock*/
        if (Clock_GetPllReferenceClock() == PLL_OSC_HSCLK)
        {
            SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
            SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
            while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }
        else
        {
            SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
            while (!Clock_IsNowStable(HOSC8M_SOURCE_STABLE_MASK))
            {
                __asm("nop");
            }
        }
        /* Configure clock for rtc0 */
        CLOCK_PERIPH_RTC0_CONFIG(pConfig->rtc0Clock);
    }
#endif

#if defined(CHIP_GM6601)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART2_MASK) == CLOCK_PERIPH_UART2_MASK)
    {
        /* Configure clock for uart2 */
        CLOCK_PERIPH_UART2_CONFIG(pConfig->uart2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART1_MASK) == CLOCK_PERIPH_UART1_MASK)
    {
        /* Configure clock for uart1 */
        CLOCK_PERIPH_UART1_CONFIG(pConfig->uart1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART0_MASK) == CLOCK_PERIPH_UART0_MASK)
    {
        /* Configure clock for uart0 */
        CLOCK_PERIPH_UART0_CONFIG(pConfig->uart0Clock);
    }
#endif

#if defined(CHIP_GM6601B)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART2_MASK) == CLOCK_PERIPH_UART2_MASK)
    {
        /*Power on PLL clock*/
        SYSDEV->PLL &= ~(SYS_PLL_PDB);
        while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
        {
            __asm("nop");
        }
        /* Configure clock for uart2 */
        CLOCK_PERIPH_UART2_CONFIG(pConfig->uart2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART1_MASK) == CLOCK_PERIPH_UART1_MASK)
    {
        /*Power on PLL clock*/
        SYSDEV->PLL &= ~(SYS_PLL_PDB);
        while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
        {
            __asm("nop");
        }

        /* Configure clock for uart1 */
        CLOCK_PERIPH_UART1_CONFIG(pConfig->uart1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART0_MASK) == CLOCK_PERIPH_UART0_MASK)
    {
        /*Power on PLL clock*/
        SYSDEV->PLL &= ~(SYS_PLL_PDB);
        while (!Clock_IsNowStable(PLL_SOURCE_STABLE_MASK))
        {
            __asm("nop");
        }

        /* Configure clock for uart0 */
        CLOCK_PERIPH_UART0_CONFIG(pConfig->uart0Clock);
    }
#endif
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    /*First check the CPU clock is 48MHz*/
    if ((pConfig->periphClkMask & CLOCK_PERIPH_ETM_MASK) == CLOCK_PERIPH_ETM_MASK)
    {
        /*Configure clock for etm*/
        CLOCK_PERIPH_ETM_CONFIG(pConfig->etmClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_ADC_MASK) == CLOCK_PERIPH_ADC_MASK)
    {
        /*Configure clock for adc*/
        CLOCK_PERIPH_ADC_CONFIG(pConfig->adcClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC2_MASK) == CLOCK_PERIPH_RTC2_MASK)
    {
        /* Configure clock for rtc2 */
        CLOCK_PERIPH_RTC2_CONFIG(pConfig->rtc2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC1_MASK) == CLOCK_PERIPH_RTC1_MASK)
    {
        /* Configure clock for rtc1 */
        CLOCK_PERIPH_RTC1_CONFIG(pConfig->rtc1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC0_MASK) == CLOCK_PERIPH_RTC0_MASK)
    {
        /* Configure clock for rtc0 */
        CLOCK_PERIPH_RTC0_CONFIG(pConfig->rtc0Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART2_MASK) == CLOCK_PERIPH_UART2_MASK)
    {
        /* Configure clock for uart2 */
        CLOCK_PERIPH_UART2_CONFIG(pConfig->uart2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART1_MASK) == CLOCK_PERIPH_UART1_MASK)
    {
        /* Configure clock for uart1 */
        CLOCK_PERIPH_UART1_CONFIG(pConfig->uart1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART0_MASK) == CLOCK_PERIPH_UART0_MASK)
    {
        /* Configure clock for uart0 */
        CLOCK_PERIPH_UART0_CONFIG(pConfig->uart0Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_HIRX_MASK) == CLOCK_PERIPH_HIRX_MASK)
    {
        /* Configure clock for hirx */
        CLOCK_PERIPH_HIRX_CONFIG(pConfig->hirxClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_I2C0_MASK) == CLOCK_PERIPH_I2C0_MASK)
    {
        /* Configure clock for i2c0 */
        CLOCK_PERIPH_I2C0_CONFIG(pConfig->i2c0Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_I2C1_MASK) == CLOCK_PERIPH_I2C1_MASK)
    {
        /* Configure clock for i2c1 */
        CLOCK_PERIPH_I2C1_CONFIG(pConfig->i2c1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_I2C2_MASK) == CLOCK_PERIPH_I2C2_MASK)
    {
        /* Configure clock for i2c2 */
        CLOCK_PERIPH_I2C2_CONFIG(pConfig->i2c2Clock);
    }

#if defined (CHIP_GM6611B)
    if ((pConfig->periphClkMask & CLOCK_PERIPH_CALENDAR_MASK) == CLOCK_PERIPH_CALENDAR_MASK)
    {
        /* Configure clock for calendar */
        CLOCK_PERIPH_CALENDAR_CONFIG(pConfig->calendarClock);
    }
#endif

    if ((pConfig->periphClkMask & CLOCK_PERIPH_LCD_MASK) == CLOCK_PERIPH_LCD_MASK)
    {
        /* Configure clock for i2c2 */
        CLOCK_PERIPH_LCD_CONFIG(pConfig->lcdClock);
    }
#endif

#if defined(CHIP_GM6621)

    /*First check the CPU clock is 48MHz*/
    if ((pConfig->periphClkMask & CLOCK_PERIPH_ETM_MASK) == CLOCK_PERIPH_ETM_MASK)
    {
        /*Configure clock for etm*/
        CLOCK_PERIPH_ETM_CONFIG(pConfig->etmClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_ADC_MASK) == CLOCK_PERIPH_ADC_MASK)
    {
        /*Configure clock for adc*/
        CLOCK_PERIPH_ADC_SRC_CONFIG(pConfig->adc_config_t.adcSrcClock);
        CLOCK_PERIPH_ADC_DIV_CONFIG(pConfig->adc_config_t.adcDivider);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART2_MASK) == CLOCK_PERIPH_UART2_MASK)
    {
        /* Configure clock for uart2 */
        CLOCK_PERIPH_UART2_CONFIG(pConfig->uart2Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART1_MASK) == CLOCK_PERIPH_UART1_MASK)
    {
        /* Configure clock for uart1 */
        CLOCK_PERIPH_UART1_CONFIG(pConfig->uart1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_UART0_MASK) == CLOCK_PERIPH_UART0_MASK)
    {
        /* Configure clock for uart0 */
        CLOCK_PERIPH_UART0_CONFIG(pConfig->uart0Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_IRTX_MASK) == CLOCK_PERIPH_IRTX_MASK)
    {
        /* Configure clock for irtx */
        CLOCK_PERIPH_IRTX_SRC_CONFIG(pConfig->irtx_config_t.irtxSrcClock);
        CLOCK_PERIPH_IRTX_DIV_CONFIG(pConfig->irtx_config_t.irtxDivider);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_I2C0_MASK) == CLOCK_PERIPH_I2C0_MASK)
    {
        /* Configure clock for i2c0 */
        CLOCK_PERIPH_I2C0_CONFIG(pConfig->i2c0Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_I2C1_MASK) == CLOCK_PERIPH_I2C1_MASK)
    {
        /* Configure clock for i2c1 */
        CLOCK_PERIPH_I2C1_CONFIG(pConfig->i2c1Clock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_WDT_MASK) == CLOCK_PERIPH_WDT_MASK)
    {
        /* Configure clock for watch dog */
        CLOCK_PERIPH_WDT_CONFIG(pConfig->wdtClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_LPSTM_MASK) == CLOCK_PERIPH_LPSTM_MASK)
    {
        /* Configure clock for i2c1 */
        CLOCK_PERIPH_LPSTM_CONFIG(pConfig->lpstmClock);
    }

    if ((pConfig->periphClkMask & CLOCK_PERIPH_RTC0_MASK) == CLOCK_PERIPH_RTC0_MASK)
    {

#define RTC_CLOCK_OSCLS             1
#define RTC_CLOCK_OSCHS             4

#define HOSC_CLOCK_XINOUT_ENABLE    (SYSDEV->OSC        |= 1 << 16)
#define HOSC_CLOCK_ENABLE           (SYSDEV->OSC        |= 1 << 0)
#define LOSC_OSCPIL_SEL_H           (AOPDEV->LSCCTRL    |= 0x80000000)
#define LOSC_CLOCK_XINOUT_ENABLE    (AOPDEV->LSCCTRL    |= 1 << 16)
#define LOSC_CLOCK_CAPSEL_MASK      (AOPDEV->LSCCTRL    &= ~(0x3F << 8))
#define LOSC_CLOCK_CORESEL_ENABLE   (AOPDEV->LSCCTRL    |= 1 << 7)
#define LOSC_CLOCK_ENABLE           (AOPDEV->LSCCTRL    |= 1 << 0)

#define OSC_STABLE_REG              (*(volatile uint32_t *)(0x40000004))
#define LOSC_STABLE_MASK            (1<<27)
#define HOSC_STABLE_MASK            (1<<26)

        if (pConfig->rtc0Clock == RTC_CLOCK_OSCLS)
        {
            /* Enableing HOSC is necessary before LOSC is enabled. */
            HOSC_CLOCK_XINOUT_ENABLE;
            HOSC_CLOCK_ENABLE;
            LOSC_OSCPIL_SEL_H;
            LOSC_CLOCK_XINOUT_ENABLE;
            LOSC_CLOCK_CAPSEL_MASK;
            LOSC_CLOCK_CORESEL_ENABLE;
            LOSC_CLOCK_ENABLE;

            /*Check whether stable or not*/
            while (!(OSC_STABLE_REG & (LOSC_STABLE_MASK)) || !(OSC_STABLE_REG & (HOSC_STABLE_MASK)));
        }
        else if (pConfig->rtc0Clock == RTC_CLOCK_OSCHS)
        {
            HOSC_CLOCK_XINOUT_ENABLE;
            HOSC_CLOCK_ENABLE;
            while (!(OSC_STABLE_REG & (HOSC_STABLE_MASK)));
        }
        /* Configure clock for rtc0 */
        CLOCK_PERIPH_RTC0_CONFIG(pConfig->rtc0Clock);
    }
#endif
    return RET_OK;
}

RET_CODE PeriphClk_GetConfig(PeriphClk_Config_T *pConfig)
{
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
    pConfig->periphClkMask = CLOCK_PERIPH_ETM_MASK | CLOCK_PERIPH_ADC_MASK | CLOCK_PERIPH_WDT_MASK | \
                             CLOCK_PERIPH_RTC2_MASK | CLOCK_PERIPH_RTC1_MASK | CLOCK_PERIPH_RTC0_MASK | \
                             CLOCK_PERIPH_UART2_MASK | CLOCK_PERIPH_UART1_MASK | CLOCK_PERIPH_UART0_MASK;

    /* Get clock for etm */
    pConfig->etmClock   = ((SYSDEV->MCS) >> CLOCK_SEL1_ETM_POS) & 0x01;
    /* Get clock for adc */
    pConfig->adcClock   = ((SYSDEV->MCS) >> CLOCK_SEL1_ADC_POS) & 0x07;
    /* Get clock for watchdog */
    pConfig->wdtClock   = ((SYSDEV->MCS) >> CLOCK_SEL1_WDT_POS) & 0x03;
    /* Get clock for rtc2 */
    pConfig->rtc2Clock  = ((SYSDEV->MCS) >> CLOCK_SEL1_RTC2_POS) & 0x03;
    /* Get clock for rtc1 */
    pConfig->rtc1Clock  = ((SYSDEV->MCS) >> CLOCK_SEL1_RTC1_POS) & 0x03;
    /* Get clock for rtc0 */
    pConfig->rtc0Clock  = ((SYSDEV->MCS) >> CLOCK_SEL1_RTC0_POS) & 0x03;
    /* Get clock for uart2 */
    pConfig->uart2Clock = ((SYSDEV->MCS) >> CLOCK_SEL1_UART2_POS) & 0x01;
    /* Get clock for uart1 */
    pConfig->uart1Clock = ((SYSDEV->MCS) >> CLOCK_SEL1_UART1_POS) & 0x01;
    /* Get clock for uart0 */
    pConfig->uart0Clock = ((SYSDEV->MCS) >> CLOCK_SEL1_UART0_POS) & 0x01;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    pConfig->periphClkMask = CLOCK_PERIPH_ETM_MASK | CLOCK_PERIPH_ADC_MASK | CLOCK_PERIPH_HIRX_MASK | \
                             CLOCK_PERIPH_RTC2_MASK | CLOCK_PERIPH_RTC1_MASK | CLOCK_PERIPH_RTC0_MASK | \
                             CLOCK_PERIPH_UART2_MASK | CLOCK_PERIPH_UART1_MASK | CLOCK_PERIPH_UART0_MASK | \
                             CLOCK_PERIPH_I2C2_MASK | CLOCK_PERIPH_I2C1_MASK | CLOCK_PERIPH_I2C0_MASK | CLOCK_PERIPH_LCD_MASK;
#if defined(CHIP_GM6611B)
    pConfig->periphClkMask |= CLOCK_PERIPH_CALENDAR_MASK;
#endif

    /* Get clock for etm */
    pConfig->etmClock   = ((SYSDEV->CLK1) >> CLOCK_SEL1_ETM_POS) & 0x07;
    /* Get clock for adc */
    pConfig->adcClock   = ((SYSDEV->CLK2) >> CLOCK_SEL2_ADC_POS) & 0x7F;
    /* Get clock for rtc2 */
    pConfig->rtc2Clock  = ((SYSDEV->CLK1) >> CLOCK_SEL1_RTC2_POS) & 0x01;
    /* Get clock for rtc1 */
    pConfig->rtc1Clock  = ((SYSDEV->CLK1) >> CLOCK_SEL1_RTC1_POS) & 0x01;
    /* Get clock for rtc0 */
    pConfig->rtc0Clock  = ((SYSDEV->CLK1) >> CLOCK_SEL1_RTC0_POS) & 0x01;
    /* Get clock for uart2 */
    pConfig->uart2Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_UART2_POS) & 0x1F;
    /* Get clock for uart1 */
    pConfig->uart1Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_UART1_POS) & 0x1F;
    /* Get clock for uart0 */
    pConfig->uart0Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_UART0_POS) & 0x1F;
    /* Get clock for hirx */
    pConfig->hirxClock = ((SYSDEV->CLK1) >> CLOCK_SEL1_HIRX_POS) & 0x03;
    /* Get clock for i2c2 */
    pConfig->i2c2Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_I2C2_POS) & 0x07;
    /* Get clock for i2c1 */
    pConfig->i2c1Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_I2C1_POS) & 0x07;
    /* Get clock for i2c0 */
    pConfig->i2c0Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_I2C0_POS) & 0x07;
    /* Get clock for hirx */
    pConfig->hirxClock = ((SYSDEV->CLK1) >> CLOCK_SEL1_HIRX_POS) & 0x03;
#if defined(CHIP_GM6611B)
    /* Get clock for calendar */
    pConfig->calendarClock = ((SYSDEV->CLK2) >> CLOCK_SEL2_CALENDAR_POS) & 0x07;
#endif
    /* Get clock for lcd */
    pConfig->lcdClock = ((SYSDEV->CLK2) >> CLOCK_SEL2_LCD_POS) & 0x01;
#endif

#if defined(CHIP_GM6621)
    pConfig->periphClkMask = CLOCK_PERIPH_I2C1_MASK | CLOCK_PERIPH_I2C0_MASK | CLOCK_PERIPH_ETM_MASK | \
                             CLOCK_PERIPH_UART2_MASK | CLOCK_PERIPH_UART1_MASK | CLOCK_PERIPH_UART0_MASK | \
                             CLOCK_PERIPH_IRTX_MASK | CLOCK_PERIPH_WDT_MASK | CLOCK_PERIPH_ADC_MASK | CLOCK_PERIPH_LPSTM_MASK;

    /* Get clock for etm */
    pConfig->etmClock   = ((SYSDEV->CLK1) >> CLOCK_SEL1_ETM_POS) & 0x07;
    /* Get clock source for adc */
    pConfig->adc_config_t.adcSrcClock   = ((SYSDEV->CLK2) >> CLOCK_SEL2_ADC_SRC_POS) & 0x7F;
    /* Get clock divider for adc */
    pConfig->adc_config_t.adcDivider   = ((SYSDEV->CLK2) >> CLOCK_SEL2_ADC_DIV_POS) & 0x7F;
    /* Get clock for uart2 */
    pConfig->uart2Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_UART2_POS) & 0x1F;
    /* Get clock for uart1 */
    pConfig->uart1Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_UART1_POS) & 0x1F;
    /* Get clock for uart0 */
    pConfig->uart0Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_UART0_POS) & 0x1F;
    /* Get clock source for irtx */
    pConfig->irtx_config_t.irtxSrcClock = ((SYSDEV->CLK2) >> CLOCK_SEL2_IRTX_SRC_POS) & 0x03;
    /* Get clock divider for irtx */
    pConfig->irtx_config_t.irtxDivider = ((SYSDEV->CLK2) >> CLOCK_SEL2_IRTX_DIV_POS) & 0x03;
    /* Get clock for i2c1 */
    pConfig->i2c1Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_I2C1_POS) & 0x07;
    /* Get clock for i2c0 */
    pConfig->i2c0Clock = ((SYSDEV->CLK1) >> CLOCK_SEL1_I2C0_POS) & 0x07;
    /* Get clock for lpstm */
    pConfig->lpstmClock = ((SYSDEV->CLK3) >> CLOCK_SEL3_LPSTM_POS) & 0x03;
    /* Get clock for watch dog */
    pConfig->wdtClock = ((SYSDEV->CLK2) >> CLOCK_SEL2_WDT_POS) & 0x03;
#endif

    return RET_OK;
}


