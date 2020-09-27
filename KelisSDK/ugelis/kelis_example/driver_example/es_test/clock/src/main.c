/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_hal_clock.h>
#include <gm_irq.h>





#define PLL_REFERENCE_BY_HIRC
#define PLL_REFERENCE_BY_HOSC
#define TEST_LOSC

#define __CPU_HALT_DEBUG__  __asm("bkpt 0")
#define __TEST_CASE_DONE__  do{printk("Test case done!\n");}while(1)


//#define TEST_CASE_1_SWITCH_ON_OFF_CLOCK
//#define TEST_CASE_2_SWITCH_FCLK_AND_PRESCALER
//#define TEST_CASE_3_SWITCH_FCLK_AND_PRESCALER_RANDOM
//#define TEST_CASE_4_DISABLE_FCLK_SOURCE_CLOCK
//#define TEST_CASE_5_SWITCH_FCLK_TO_UNSTABLE_CLOCK
//#define TEST_CASE_6_SWITCH_FCLK_AND_PRESCALER_ONE_BY_ONE
//#define TEST_CASE_7_HOSC_MONITOR
#define TEST_CASE_8_TEST_PERIPHERAL_CONFIG_API

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define HOSC_CLOCK_DISABLE          (SYS_DEV->HOSCCR &= ~(1 << 0))
#define FCLK_SWT_ERR_POS            (20)

static inline void Configure_CPU_CLKSource(uint8_t clkSrc)
{
    MODIFY_REG(*(volatile uint32_t *)(0x40000090), 0x07, clkSrc);
}

static inline void Configure_CPU_CLKDiv(uint32_t Div)
{
    //MODIFY_REG(*(volatile uint32_t *)(0x40000090), 0x1FF << 4, Div << 4);
    *(volatile uint32_t *)(0x40000090) &= ~(0x1ff << 4);
    *(volatile uint32_t *)(0x40000090) |= (Div << 4);
}

#if 0
void NMI_IRQHandler()
{
    while (1)
    {
        printk("NMI Interrupt occured!\n");
    }
}
#endif

void main(void)
{

#ifdef TEST_CASE_1_SWITCH_ON_OFF_CLOCK
    uint8_t i = 3;

    /*The default setting is enable HIRC and LIRC*/
    while (!(HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE)) && !(HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE)));
    printk("GM6721 clock test 1 Start:\n");
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE) || HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*HOSC or LOSC is stable while they are disabled*/
    }

    //  HAL_CLOCK_HOSCcmd(ENABLE);
    //  HAL_CLOCK_LOSCcmd(ENABLE);
    HAL_CLOCK_ClockCmd(CLOCK_HOSC_MASK, ENABLE);
    HAL_CLOCK_ClockCmd(CLOCK_LOSC_MASK, ENABLE);

#ifdef TEST_LOSC
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE) && HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*HOSC or LOSC is not stable*/
    }
#endif

#ifdef PLL_REFERENCE_BY_HOSC
    /*Check PLL enable*/
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLL is stable while it is disabled*/
    }

    HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
    HAL_CLOCK_ClockCmd(CLOCK_PLL_MASK, ENABLE);

    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLL is not stable*/
    }

    /* Check PLLKDP */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLKDP is stable while it is disabled*/
    }

    /*Enable PLLKDP and use HOSC as reference clock*/
    HAL_CLOCK_ClockCmd(CLOCK_PLLKDP_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLKDP is not stable*/
    }

    /* Check PLLI2S */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLI2S is stable while it is disabled*/
    }

    /*Enable PLLI2S and use HOSC as reference clock*/
    HAL_CLOCK_ClockCmd(CLOCK_PLLI2S_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLI2S is not stable*/
    }

    /* Check PLLSAI */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLSAI is stable while it is disabled*/
    }

    /*Enable PLLSAI and use HOSC as reference clock*/
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLSAI is not stable*/
    }
#endif

#ifdef PLL_REFERENCE_BY_HIRC

    HAL_CLOCK_ClockCmd(CLOCK_PLL_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLKDP_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLI2S_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, DISABLE);
    HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);

    /* Check PLL */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLL is stable while it is disabled*/
    }

    HAL_CLOCK_ClockCmd(CLOCK_PLL_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLL is not stable*/
    }

    /* Check PLLKDP */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLKDP is stable while it is disabled*/
    }

    /*Enable PLLKDP and use HOSC as reference clock*/
    HAL_CLOCK_ClockCmd(CLOCK_PLLKDP_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLKDP is not stable*/
    }

    /* Check PLLI2S */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLI2S is stable while it is disabled*/
    }

    /*Enable PLLI2S and use HOSC as reference clock*/
    HAL_CLOCK_ClockCmd(CLOCK_PLLI2S_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLI2S is not stable*/
    }

    /* Check PLLSAI */
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i > 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLSAI is stable while it is disabled*/
    }

    /*Enable PLLSAI and use HOSC as reference clock*/
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, ENABLE);
    i = 3;
    while (i)
    {
        if (HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
        {
            break;
        }
        k_sleep(5);
        i--;
    }
    if (i == 0)
    {
        __CPU_HALT_DEBUG__;     /*PLLSAI is not stable*/
    }
#endif
    HAL_CLOCK_ClockCmd(CLOCK_HOSC_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_LOSC_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLL_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLKDP_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLI2S_MASK, DISABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLSAI_MASK, DISABLE);

    __TEST_CASE_DONE__;

#endif

#ifdef TEST_CASE_2_SWITCH_FCLK_AND_PRESCALER
    uint32_t i = 0;
    uint32_t j = 0;

    HAL_CLOCK_ClockCmd(CLOCK_HOSC_MASK, ENABLE);
    //HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
    HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
    HAL_CLOCK_ClockCmd(CLOCK_PLL_MASK, ENABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLKDP_MASK, ENABLE);
    *(volatile uint32_t *)0x400002e0 |= 0x1;
    *(volatile uint32_t *)0x40000328 |= 0x1;


    while (1)
    {
        Configure_CPU_CLKSource(i);
        printk("FCLK prescaler test\n");
        for (j = 0; j < 512; j++)
        {
            if ((i == 0 || i == 1) && j >= 455)
            {
                break;
            }
            else
            {
                Configure_CPU_CLKDiv(j);
                printk("\tFCLK source is: %d\n", i);
                printk("\tFCLK prescaler is: %d\n", j);
                printk("\n0x40000090 = %x\n", *(volatile uint32_t *)(0x40000090));
            }
        }
        i++;
        if (i == 5)
        {
            break;
        }
    }
    __TEST_CASE_DONE__;
#endif

#ifdef TEST_CASE_3_SWITCH_FCLK_AND_PRESCALER_RANDOM
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 500;
    HAL_CLOCK_ClockCmd(CLOCK_HOSC_MASK, ENABLE);
    HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
    HAL_CLOCK_ClockCmd(CLOCK_PLL_MASK, ENABLE);
    HAL_CLOCK_ClockCmd(CLOCK_PLLKDP_MASK, ENABLE);

    while (1)
    {
        i = sys_rand32_get() % 5;
        Configure_CPU_CLKSource(i);
        j = sys_rand32_get() % 512;
        if ((i == 0 || i == 1) && (j > 455))
        {
            j = 455;
        }
        k = 50;
        while (k--);
        Configure_CPU_CLKDiv(j);
        k = 50;
        while (k--);
        printk("FCLK source is: %d\n", i);
        printk("\tFCLK prescaler is: %d\n", j);
        printk("\t0x40000090 = %x\n", *(volatile uint32_t *)(0x40000090));
    }
#endif

#ifdef TEST_CASE_4_DISABLE_FCLK_SOURCE_CLOCK
    uint8_t i = 0;
    HAL_CLOCK_ClockCmd(CLOCK_HOSC_MASK, ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
    /* The default source clock of FCLK is HIRC */
    /*Disable HIRC*/
    HAL_CLOCK_ClockCmd(CLOCK_HIRC_MASK, DISABLE);
    printk("Test protection for HIRC\n");
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->HIRCCR & (1 << 0)))
    {
        printk("Error! HIRC is disabled\n");
        __CPU_HALT_DEBUG__;
    }

    /* Now, switch FCLK source clock to HOSC */
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_HOSC8M);
    /* Disable HIRC */
    //HAL_CLOCK_HIRCcmd(DISABLE);
    //while(SYS_DEV->HIRCCR & (1 << 0));

    /* Test HOSC */
    /* Now, FCLK source clock is HOSC */
    printk("Test protection for HOSC\n");
    HOSC_CLOCK_DISABLE;
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->HOSCCR & (1 << 0)))
    {
        printk("Error! HOSC is disabled\n");
        __CPU_HALT_DEBUG__;
    }

    /* Switch FCLK source clock to PLL */
    /* HOSC clock is enabled, because HOSC cant not be disabled by above program */
    HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
    HAL_CLOCK_PLLcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLL);
    /* Disable PLL */
    HAL_CLOCK_PLLcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;

    if (!(SYS_DEV->PLL[0] & (1 << 0)))
    {
        printk("Error! PLL is disabled\n");        /*PLL is disabled*/
        __CPU_HALT_DEBUG__;
    }

    /* Disable HOSC */
    HOSC_CLOCK_DISABLE;
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->HOSCCR & (1 << 0)))
    {
        printk("Error! HOSC is PLL reference clock, but HOSC is disabled\n");        /*HOSC is disabled*/
        __CPU_HALT_DEBUG__;
    }

    /* Switch FCLK to PLLKDP133 */
    HAL_CLOCK_PLLKDPcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP100);
    /*disable PLL*/
    HAL_CLOCK_PLLcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;

    if (SYS_DEV->PLL[0] & (1 << 0))
    {
        printk("Error! PLL is still enabled\n");
        /*PLL is still enabled*/
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;

    if (!(SYS_DEV->PLLKDP[0] & (1 << 0)))
    {
        printk("Error! PLLKDP133 is CPU clock source, but it is disabled\n");
        /*PLLKDP is disabled*/
        __CPU_HALT_DEBUG__;
    }
    /* disable HOSC */
    HOSC_CLOCK_DISABLE;
    while (i < 255)
    {
        i++;
    }
    i = 0;

    if (!(SYS_DEV->HOSCCR & (1 << 0)))
    {
        printk("Error! HOSC is PLLKDP133 source clock, but HOSC is disabled\n");
        /*HOSC is disabled*/
        __CPU_HALT_DEBUG__;
    }

    /* Switch FCLK to PLLKDP200 */
    HAL_CLOCK_PLLKDPcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP150);
    /*disable PLLKDP*/
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;

    if (!(SYS_DEV->PLLKDP[0] & (1 << 0)))
    {
        printk("Error! PLLKDP200 is CPU clock source, but PLLKDP200 is disabled\n");
        /*PLLKDP is disabled*/
        __CPU_HALT_DEBUG__;
    }
    /*disable HOSC*/
    HOSC_CLOCK_DISABLE;
    while (i < 255)
    {
        i++;
    }
    i = 0;

    if (!(SYS_DEV->HOSCCR & (1 << 0)))
    {
        printk("Error! HOSC is PLLKDP200 source clock, but HOSC is disabled\n");
        /*HOSC is disabled*/
        __CPU_HALT_DEBUG__;
    }


    /*Using HIRC as PLL reference clock */
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_HIRC8M);
    HAL_CLOCK_PLLcmd(DISABLE);
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
    HAL_CLOCK_PLLcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLL);
    HOSC_CLOCK_DISABLE;
    HAL_CLOCK_PLLcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->PLL[0] & 0x1))
    {
        printk("Error! PLL is source clock, but PLL is dieabled\n");
        /*PLL is disabled*/
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_HIRCcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->HIRCCR & 0x1))
    {
        printk("Error! HIRC is PLL source clock, but HIRC is disabled\n\n");
        /*PLL is disabled*/
        __CPU_HALT_DEBUG__;
    }

    /*PLLKDP133 as FCLK*/
    HAL_CLOCK_PLLKDPcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP100);
    HAL_CLOCK_PLLcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (SYS_DEV->PLL[0] & 0x1)
    {
        printk("PLL is still enabled\n");
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->PLLKDP[0] & 0x1))
    {
        printk("Error! PLLKDP is source clock, but PLLKDP is dieabled\n");
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_HIRCcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->HIRCCR & 0x1))
    {
        printk("Error! HIRC is PLLKDP source clock, but HIRC is disabled\n\n");
        /*PLL is disabled*/
        __CPU_HALT_DEBUG__;
    }

    /*PLLKDP200 as FCLK*/
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP150);
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->PLLKDP[0] & 0x1))
    {
        printk("Error! PLLKDP is source clock, but PLLKDP is dieabled\n");
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_HIRCcmd(DISABLE);
    while (i < 255)
    {
        i++;
    }
    i = 0;
    if (!(SYS_DEV->HIRCCR & 0x1))
    {
        printk("Error! HIRC is PLLKDP source clock, but HIRC is disabled\n\n");
        /*PLL is disabled*/
        __CPU_HALT_DEBUG__;
    }

    __TEST_CASE_DONE__;
#endif

#ifdef TEST_CASE_5_SWITCH_FCLK_TO_UNSTABLE_CLOCK
    uint32_t i = 0;
    printk("Start to test case 5\n");
    SYS_DEV->SYSCLK |= 1 << FCLK_SWT_ERR_POS;
    HAL_CLOCK_HIRCcmd(DISABLE);
    HAL_DelayMs(50);
    if (!(SYS_DEV->HIRCCR & 0x1))
    {
        printk("Error! HIRC disabled\n");
    }
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_HOSC8M);
    HAL_DelayMs(50);
    if (!(SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to unstable clock HOSC\n");
        /*FCLK source clock switched to unstable clock HOSC*/
        __CPU_HALT_DEBUG__;
    }
    SYS_DEV->SYSCLK |= 1 << FCLK_SWT_ERR_POS;
    printk("HIRC test done\n");

    /*Switch FCLK to HOSC*/
    HAL_CLOCK_HOSCcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_HOSC8M);
    HAL_DelayMs(50);
    if ((SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to HOSC failed\n");
        /*FCLK source clock switched to HOSC failed*/
        __CPU_HALT_DEBUG__;
    }
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP150);
    HAL_DelayMs(50);
    if (!(SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to unstable clock PLLKDP200\n");
        /*FCLK source clock switched to unatable clock PLLKDP200*/
        __CPU_HALT_DEBUG__;
    }
    SYS_DEV->SYSCLK |= 1 << FCLK_SWT_ERR_POS;
    printk("HOSC test done\n");

    /*Switch FCLK to PLLKDP200*/
    HAL_CLOCK_PLLKDPcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP150);
    HAL_DelayMs(50);
    if ((SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to PLLKDP200 failed\n");
        /*FCLK source clock switched to PLLKDP200 failed*/
        __CPU_HALT_DEBUG__;
    }
    HOSC_CLOCK_DISABLE;
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLL);
    HAL_DelayMs(50);
    if (!(SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to unstable clock PLL\n");
        /*FCLK source clock switched to unatable clock PLL*/
        __CPU_HALT_DEBUG__;
    }
    SYS_DEV->SYSCLK |= 1 << FCLK_SWT_ERR_POS;
    printk("PLLKDP200 test done\n");

    /*Switch FCLK to PLL*/
    HAL_CLOCK_PLLcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLL);
    HAL_DelayMs(50);
    if ((SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to PLL failed\n");
        /*FCLK source clock switched to PLL failed*/
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    HAL_DelayMs(50);
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP100);
    HAL_DelayMs(50);
    if (!(SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to unstable clock PLLKDP133\n");
        /*FCLK source clock switched to unatable clock PLLKDP133*/
        __CPU_HALT_DEBUG__;
    }
    SYS_DEV->SYSCLK |= 1 << FCLK_SWT_ERR_POS;
    printk("PLL test done\n");

    /*Switch FCLK to PLLKDP133*/
    HAL_CLOCK_PLLKDPcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLLKDP100);
    HAL_DelayMs(50);
    if ((SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to PLLKDP133 failed\n");
        /*FCLK source clock switched to PLL failed*/
        __CPU_HALT_DEBUG__;
    }
    HAL_CLOCK_PLLcmd(DISABLE);
    HAL_DelayMs(50);
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_PLL);
    HAL_DelayMs(50);
    if (!(SYS_DEV->SYSCLK & (0x1 << FCLK_SWT_ERR_POS)))
    {
        printk("Error!Switch FCLK to unstable clock PLL\n");
        /*FCLK source clock switched to unatable clock PLLKDP133*/
        __CPU_HALT_DEBUG__;
    }
    SYS_DEV->SYSCLK |= 1 << FCLK_SWT_ERR_POS;
    printk("PLLKDP133 test done\n");

    __TEST_CASE_DONE__;
#endif

#ifdef TEST_CASE_6_SWITCH_FCLK_AND_PRESCALER_ONE_BY_ONE
    uint32_t i = 0;
    uint32_t j = 0;
    printk("Test case 6 start:\n");
    HAL_CLOCK_HOSCcmd(ENABLE);
    HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
    //HAL_CLOCK_PLLsReferenceClock(PLL_OSC_HSCLK);
    HAL_CLOCK_PLLcmd(ENABLE);
    HAL_CLOCK_PLLKDPcmd(ENABLE);
    *(volatile uint32_t *)0x400002e0 |= 0x1;
    *(volatile uint32_t *)0x40000328 |= 0x1;

    while (1)
    {
        Configure_CPU_CLKSource(i);
        if ((i == 0 || i == 1) && (j > 460))
        {
            j = 0;
        }
        Configure_CPU_CLKDiv(j);
        printk("FCLK source is: %d\n", i);
        printk("\tFCLK prescaler is: %d\n", j);
        printk("\t0x40000090 = %x\n\n", *(volatile uint32_t *)(0x40000090));
        i++;
        j++;
        if (i > 4)
        {
            i = 0;
        }
        if (j > 511)
        {
            j = 0;
        }
    }
    __TEST_CASE_DONE__;
#endif

#ifdef TEST_CASE_7_HOSC_MONITOR
    uint32_t tmp = 0;
    uint32_t i = 0;
    *(volatile uint32_t *)0x40000080 |= (0x1 << 1);
    *(volatile uint32_t *)0x40000070 |= (0x1 << 1);
    /*HIRC clock is enabled default*/
    printk("Start to test case 7:\n");
    //HAL_RequestIrq(NonMaskableInt_IRQn, 0, NMI_IRQHandler, 0);
    HAL_CLOCK_HOSCcmd(ENABLE);
    /*Clear pending flag*/
    SYS_DEV->SRS |= (1 << 24);

    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
    /* HOSC is the source clock of FCLK */
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_HOSC8M);
    HAL_CLOCK_HIRCcmd(DISABLE);
    /* Enable HOSC monitor and enable HOSC_LOSS_PEND to NMI*/
    //SYS_DEV->HOSCM[0] |= 0x03;
    /* Enable HOSC monitor and enable HOSC_LOSS_PEND to NMI by pass*/
    SYS_DEV->HOSCM[0] |= 0x01;
    tmp = SYS_DEV->HOSCM[1];
    SYS_DEV->HOSCM[1] = 0x00030001;

    HAL_DelayMs(5);
    if (!(SYS_DEV->SRS & (1 << 24)))
    {
        __CPU_HALT_DEBUG__;
    }
    printk("CPU is still alive\n");
    SYS_DEV->HOSCM[1] = 0x07D00014;
    HAL_DelayMs(5);
    SYS_DEV->SRS |= 1 << 24;
    /*GPIOA0/1/2/3 LED blink*/
    *(volatile uint32_t *)0x40010010 = 0xF;
    while (i < 10)
    {
        if (i % 2)
        {
            *(volatile uint32_t *)0x40010004 = 0xF;
        }
        else
        {
            *(volatile uint32_t *)0x40010004 = 0x0;
        }
        HAL_DelayMs(500);
        i++;
    }
    HAL_CLOCK_HIRCcmd(DISABLE);
    HAL_CLOCK_HOSCcmd(DISABLE);
    i = 0;
    while (i < 10)
    {
        if (i % 2)
        {
            *(volatile uint32_t *)0x40010004 = 0xF;
        }
        else
        {
            *(volatile uint32_t *)0x40010004 = 0x0;
        }
        HAL_DelayMs(500);
        i++;
    }
    if ((uint8_t)((SYS_DEV->SYSCLK >> 16) & 0x07) == SYSCLOCK_SOURCE_HOSC8M)
    {
        printk("CPU is run at HOSC 8M\n");
    }

    __TEST_CASE_DONE__;
#endif


#ifdef TEST_CASE_8_TEST_PERIPHERAL_CONFIG_API
    /* Test PLLKDP clock select */
    Peripheral_Config_T periConfig;
    periConfig.peripheMask = PERIPHERAL_KDP_MASK;
    periConfig.peripheClkConfig.kdpClkConfig = CLOCK_KDP_SEL_PLLKDP300;
    HAL_CLOCK_PeripheralClkSetConfig(&periConfig);

    /* Test ADC1 clock select */
    periConfig.peripheMask = PERIPHERAL_ADC1_MASK;
    periConfig.peripheClkConfig.adcClkConfig.adcSourceClk = CLOCK_ADC_SEL_PLLI2S;
    periConfig.peripheClkConfig.adcClkConfig.adcPrescaler = CLOCK_ADC_DIV_3;
    HAL_CLOCK_PeripheralClkSetConfig(&periConfig);

    /* Test SDMMC clock select */
    periConfig.peripheMask = PERIPHERAL_SDMMC_MASK;
    periConfig.peripheClkConfig.sdmmcClkConfig = CLOCK_SDMMC_SEL_PLLI2S;
    HAL_CLOCK_PeripheralClkSetConfig(&periConfig);

    /* Test VOUT clock select */
    periConfig.peripheMask = PERIPHERAL_VOUT_MASK;
    periConfig.peripheClkConfig.voutClkConfig = CLOCK_VOUT_DIV_6;
    HAL_CLOCK_PeripheralClkSetConfig(&periConfig);

    /* Test SAIA clock select */
    periConfig.peripheMask = PERIPHERAL_SAIA_MASK;
    periConfig.peripheClkConfig.saiClkConfig = CLOCK_SAI_SEL_PLLI2S;
    HAL_CLOCK_PeripheralClkSetConfig(&periConfig);

    /* Test UART4 clock select */
    periConfig.peripheMask = PERIPHERAL_UART4_MASK;
    periConfig.peripheClkConfig.uartClkConfig.uartSourceClk = CLOCK_UART_SEL_PLLI2S;
    periConfig.peripheClkConfig.uartClkConfig.uartPrescaler = CLOCK_UART_DIV_8;
    HAL_CLOCK_PeripheralClkSetConfig(&periConfig);
    while (1)
    {
        printk("Hello World!\n");
        k_sleep(1000);
    }

#endif


}
