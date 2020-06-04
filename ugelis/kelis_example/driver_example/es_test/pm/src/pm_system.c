/*=================================================================================
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file system.c
 *
 *
 * @version 1.0.0
 *
 * @date  2018/03/27
 *
 * @Description  system API define .
 *
 * @history
 *      Date                By               Comments              Ver.
 *  ============       ===============      ============       ===========
 *  2018/03/27           David.Peng           Create              1.0.0
 *=================================================================================*/

#include <ugelis.h>
#include "board.h"
#include <misc/printk.h>
#include "pm_system.h"
#include <gm_soc.h>



void hirc_enable(void)
{
    writel(0x01, HIRC_CTRL);
}

void hirc_disable(void)
{
    writel(0x00, HIRC_CTRL);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t hirc_lock_flag(void)
{
    return (readl(CMU_CLK_ST) & 0x01);
}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_hirc_enable_bit(void)
{
    return (readl(HIRC_CTRL) & 0x01);
}

void lirc_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(LIRC_CTRL);
    reg_value = reg_value | 0x01;
    writel(reg_value, LIRC_CTRL);
}

void lirc_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(LIRC_CTRL);
    reg_value = reg_value & (~0x01);
    writel(reg_value, LIRC_CTRL);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t lirc_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x01) & 0x01);
}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_lirc_enable_bit(void)
{
    return (readl(LIRC_CTRL) & 0x01);
}

/*
TMR:
6bits LIRC accurate trimming,It is supplied by VBATCORE,
and only reset by BKD_RSTJ
Default value£º3'b100

TMC:
3bits LIRC coarse trimming,It is supplied by VBATCORE,
and only reset by BKD_RSTJ
Default value£º6'b100_000

*/
void lirc_config(uint32_t tmr, uint32_t tmc)
{
    uint32_t reg_value;

    reg_value = readl(LIRC_CTRL);
    reg_value = (reg_value & 0x01) | (tmr << 8) | (tmc << 4);
    writel(reg_value, LIRC_CTRL);
}

void hosc_enable(void)
{
    writel(0x01, HOSC_CTRL);
}

void hosc_disable(void)
{
    writel(0x00, HOSC_CTRL);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t hosc_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x02) & 0x01);
}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_hosc_enable_bit(void)
{
    return (readl(HOSC_CTRL) & 0x01);
}

void losc_enable(void)
{
    writel(0x01, LOSC_CTRL);
}

void losc_disable(void)
{
    writel(0x00, LOSC_CTRL);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t losc_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x03) & 0x01);
}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_losc_enable_bit(void)
{
    return (readl(LOSC_CTRL) & 0x01);
}

/*
 sel
 0:From HIRC(default)
 1:From HOSC
 2:From PAD in the ES mode

 div
 4'd(2~15),default 4
*/
void pll_refclk_config(uint32_t sel, uint32_t div)
{
    writel(sel | (div << 8), REFCLK_SRC_CTRL);
}

/*
 return value
 0:From HIRC(default)
 1:From HOSC
 2:From PAD in the ES mode

*/
uint32_t get_pll_refclk_sel(void)
{
    return ((readl(REFCLK_SRC_CTRL) & 0x03));
}

void pll_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value | 0x01;
    writel(reg_value, PLL_CTRL1);
}

void pll_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value & (~0x01);
    writel(reg_value, PLL_CTRL1);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t pll_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x04) & 0x01);
}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_pll_enable_bit(void)
{
    return (readl(PLL_CTRL1) & 0x01);
}

void pllkdp_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value | 0x01;
    writel(reg_value, PLLKDP_CTRL1);
}

void pllkdp_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value & (~0x01);
    writel(reg_value, PLLKDP_CTRL1);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t pllkdp_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x05) & 0x01);

}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_pllkdp_enable_bit(void)
{
    return (readl(PLLKDP_CTRL1) & 0x01);
}

void plli2s_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value | 0x01;
    writel(reg_value, PLLI2S_CTRL1);
}

void plli2s_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value & (~0x01);
    writel(reg_value, PLLI2S_CTRL1);
}

/*
 return value
 0:unlock
 1:lock
*/
uint32_t plli2s_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x06) & 0x01);

}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_plli2s_enable_bit(void)
{
    return (readl(PLLI2S_CTRL1) & 0x01);
}

void pllsai_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value | 0x01;
    writel(reg_value, PLLSAI_CTRL1);
}

void pllsai_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(PLL_CTRL1);
    reg_value = reg_value & (~0x01);
    writel(reg_value, PLLSAI_CTRL1);
}
/*
 return value
 0:unlock
 1:lock
*/
uint32_t pllsai_lock_flag(void)
{
    return ((readl(CMU_CLK_ST) >> 0x07) & 0x01);
}

/*
 return value
 0:disable
 1:enable
*/
uint32_t get_pllsai_enable_bit(void)
{
    return (readl(PLLSAI_CTRL1) & 0x01);
}

/*
clk_sel
3¡¯B000: 8MHz of HIRC
3¡¯B001: 8MHz of HOSC
3¡¯B010: 200MHz of PLLKDP
3¡¯B011: 180~216MHz of PLL
3¡¯B100: 133.33MHz of PLLKDP
3¡¯B101: TEST_CLK(TBD)

div
9¡¯H000~9¡¯H1FF: FCLK is divided-by from 1 to 512
*/
void fclk_config(uint32_t clk_sel, uint32_t div)
{
    uint32_t reg_value;
    reg_value = readl(SYS_CLK_CTRL) & (~(0x7 | 0x1FF << 4));
    reg_value = reg_value | clk_sel | (div << 4);
    if (clk_sel == 0x7)
    {
        reg_value = reg_value | (0xCF << 24);
    }
    writel(reg_value, SYS_CLK_CTRL);
}

/* get the current FCLK source
3¡¯B000: 8MHz of HIRC
3¡¯B001: 8MHz of HOSC
3¡¯B010: 200MHz of PLLKDP
3¡¯B011: 180~216MHz of PLL
3¡¯B100: 133.33MHz of PLLKDP
3¡¯B101: TEST_CLK(TBD)

*/
uint32_t get_fclk_source(void)
{
    return (readl(SYS_CLK_CTRL) >> 16) & 0x7;
}

/* get FCLK_SWT_ERR flag
FCLK_SWT_ERR: The flag will be set when FCLK_SEL_SHD is not equal with FCLK_SEL
*/
uint32_t get_fclk_err_flag(void)
{
    return (readl(SYS_CLK_CTRL) >> 20) & 0x1;
}

/* clear FCLK_SWT_ERR flag
   write 1 to clear
*/
void clear_fclk_err_flag(void)
{
    uint32_t reg_value;
    reg_value = readl(SYS_CLK_CTRL) | (0x1 << 20);
    writel(reg_value, SYS_CLK_CTRL);
}

void hosc_monitor_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(HOSC_MNT_CTRL1);
    reg_value = reg_value | 0x01;
    writel(reg_value, HOSC_MNT_CTRL1);
}

void hosc_monitor_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(HOSC_MNT_CTRL1);
    reg_value = reg_value & (~0x01);
    writel(reg_value, HOSC_MNT_CTRL1);
}

void hosc_loss_pend_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(HOSC_MNT_CTRL1);
    reg_value = reg_value | 0x02;
    writel(reg_value, HOSC_MNT_CTRL1);
}

void hosc_loss_pend_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(HOSC_MNT_CTRL1);
    reg_value = reg_value & (~0x02);
    writel(reg_value, HOSC_MNT_CTRL1);
}

/* get_hosc_period
HOSC_PRD: HOSC clock period, it is valid when HOSC_MNT_EN is HIGH

*/
uint32_t get_hosc_period(void)
{
    return (readl(HOSC_MNT_CTRL1) >> 16);
}

/* get HOSC_LOSS_PEND flag
   0: HOSC not loss
   1: HOSC loss
*/
uint32_t hosc_loss_pend_flag(void)
{
    return ((readl(RST_STATUS_REG) >> 24) & 0x01);
}

/* clear HOSC_LOSS_PEND flag
   write 1 to clear
*/
void hosc_loss_pend_clear(void)
{
    uint32_t reg_value;

    reg_value = readl(RST_STATUS_REG);
    reg_value = reg_value | (0x01 << 24);
    writel(reg_value, RST_STATUS_REG);
}

/* clear reset pending flag
   write 1 to clear
*/
void reset_pend_clear(uint32_t pending_flag)
{

    writel(pending_flag, RST_STATUS_REG);
}


void lockup_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(SYS_CFG_CTRL);
    reg_value = reg_value | 0x10;
    writel(reg_value, SYS_CFG_CTRL);
}

void backup_reg_unlock(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value | (0x01 << 12);
    writel(reg_value, PWR_MOD_REG);
}

void backup_reg_lock(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value & (~(0x01 << 12));
    writel(reg_value, PWR_MOD_REG);
}

void pmu_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value | 0x01;
    writel(reg_value, PWR_MOD_REG);
}

void pmu_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value & (~0x01);
    writel(reg_value, PWR_MOD_REG);
}
/* power mode
2¡¯B00: STOP1 mode
2¡¯B01: STOP2 mode
2¡¯B10: Standby mode
2¡¯B11: Shut down mode
it is only valid when PMUENABLE is HIGH.
*/
void power_mode_enable(uint32_t mode)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = (reg_value & (~(0x03 << 4))) | (mode << 4);
    writel(reg_value, PWR_MOD_REG);
}

/* ldo work mode
0:normal mode(normal BG)
1:low power mode(low power BG)

*/
void ldo_mode_sel(uint32_t mode)
{
    uint32_t reg_value;

    reg_value = readl(LDO_TRIM_CTRL);
    reg_value = (reg_value & (~(0x01 << 16))) | (mode << 16);
    writel(reg_value, LDO_TRIM_CTRL);
}

void ldo_hw_trim_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value | (0x01 << 8);
    writel(reg_value, PWR_MOD_REG);
}

void ldo_hw_trim_disable(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value & (~(0x01 << 8));
    writel(reg_value, PWR_MOD_REG);
}



/*
    go to  sleep power mode config

*/
void goto_sleep_mode(void)
{
    SCB->SCR &= ~(1UL << 2);

    __asm("wfi\n");

}




/*
    go to  stop1 power mode config

*/
void goto_stop1_mode(void)
{

    //cofig stop1 mode enable
    power_mode_enable(POWER_MODE_STOP1);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to stop1 mode
    __WFI();

}


/*
    go to  stop2 power mode config

*/
void goto_stop2_mode(void)
{

    //cofig stop2 mode enable
    power_mode_enable(POWER_MODE_STOP2);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);

    //WFI or WFE go to stop2 mode
    __WFI();

    //__WFE();

}

/*
    go to standby power mode config

*/


void goto_standby_mode(void)
{

    //cofig stop1 mode enable
    power_mode_enable(POWER_MODE_STANDBY);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to stop2 mode

    __WFI();

}


/*
    go to shut down power mode config

*/
void goto_shut_down_mode(void)
{

    //cofig stop1 mode enable
    power_mode_enable(POWER_MODE_SHUTDOWM);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to shut down mode
    __WFI();

}

void dly(uint32_t ms)  //FCLK 8MHz
{
    uint32_t i;

    for (i = 0; i < ms * 2000; i++)
    {
        __ASM volatile("nop");
        __ASM volatile("nop");
    }
}

/*****************************************************************************
*
* @brief Stop standard cortex-m0 systick.
*
* @param     none
* @retval    none
*
******************************************************************************/
void SysTick_Disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Enable(void)
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}





