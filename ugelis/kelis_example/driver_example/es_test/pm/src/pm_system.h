/*=================================================================================
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file pm_system.h
 *
 *
 * @version 1.0.0
 *
 * @date  2018/03/27
 *
 * @Description  system header file.
 *
 * @history
 *      Date                By               Comments              Ver.
 *  ============       ===============      ============       ===========
 *  2018/03/27           David.Peng           Create              1.0.0
 *=================================================================================*/

#ifndef _PM_SYSTEM_H_
#define _PM_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_soc.h>


/*-----------------------
 * Print macros
 *-----------------------*/
#define DBG_PRINTF              1
#define WARNING_PRINTF          1
#define ERR_PRINTF              1

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

/* Base Address */
#define SYSTEM_BASE              (0x40000000)
#define WATCHDOG_BASE            (0x4003E000)//0x40008000
#define IWATCHDOG_BASE           (0x40008000)//0x4003E000
#define RTC_BASE                 (0x40009000)
//#define PES_BASE                 (0x40037000)

/* Register Define */

#define CHIP_ID                  (SYSTEM_BASE + 0x00 )                 /* Chip ID register                       */
#define RST_STATUS_REG           (SYSTEM_BASE + 0x04 )                 /* Reset status register                  */
#define STRAP_TRG_CTRL           (SYSTEM_BASE + 0x08 )                 /* Strap trigger control register         */
#define STRAP_CFG_VLE            (SYSTEM_BASE + 0x0C )                 /* Strap configuration register           */
#define STRAP_INF_VLE            (SYSTEM_BASE + 0x10 )                 /* Strap information register             */
#define SYS_CFG_CTRL             (SYSTEM_BASE + 0x14 )                 /* System configuration register          */
#define PWR_MOD_REG              (SYSTEM_BASE + 0x18 )                 /* Power mode configuration register      */
#define STCALIB_REG              (SYSTEM_BASE + 0x24 )                 /* System tick calibration register       */
#define PWR_DBG_STUS             (SYSTEM_BASE + 0x40 )                 /* Power mode FSM debug register          */
#define DBG_CPU_REG1             (SYSTEM_BASE + 0x44 )                 /* CPU access record register1            */
#define DBG_CPU_REG2             (SYSTEM_BASE + 0x48 )                 /* CPU access record register2            */
#define DBG_CPU_REG3             (SYSTEM_BASE + 0x4C )                 /* CPU access record register3            */
#define DBG_CPU_REG4             (SYSTEM_BASE + 0x50 )                 /* CPU access record register4            */
#define WDT_RCD_REG              (SYSTEM_BASE + 0x54 )                 /* Watch Dog record register              */
#define WDT_DBG_CTRL             (SYSTEM_BASE + 0x58 )                 /* Watch Dog debug register               */
#define IP_RST_CTRL1             (SYSTEM_BASE + 0x70 )                 /* IP SW reset register1                  */
#define IP_RST_CTRL2             (SYSTEM_BASE + 0x74 )                 /* IP SW reset register2                  */
#define IP_RST_CTRL3             (SYSTEM_BASE + 0x78 )                 /* IP SW reset register3                  */
#define CLK_EN_CTRL1             (SYSTEM_BASE + 0x80 )                 /* Clock enable register1                 */
#define CLK_EN_CTRL2             (SYSTEM_BASE + 0x84 )                 /* Clock enable register2                 */
#define CLK_EN_CTRL3             (SYSTEM_BASE + 0x88 )                 /* Clock enable register3                 */
#define SYS_CLK_CTRL             (SYSTEM_BASE + 0x90 )                 /* System clock control register          */
#define CLK_SRC_CTRL1            (SYSTEM_BASE + 0x94 )                 /* Clock source control register1         */
#define CLK_SRC_CTRL2            (SYSTEM_BASE + 0x98 )                 /* Clock source control register2         */
#define CLK_SRC_CTRL3            (SYSTEM_BASE + 0x9C )                 /* Clock source control register3         */
#define CLK_SRC_CTRL4            (SYSTEM_BASE + 0xA0 )                 /* Clock source control register4         */
#define CLK_SRC_CTRL5            (SYSTEM_BASE + 0xA4 )                 /* Clock source control register5         */
#define CLK_SRC_CTRL6            (SYSTEM_BASE + 0xA8 )                 /* Clock source control register6         */
#define CLK_SRC_CTRL7            (SYSTEM_BASE + 0xAC )                 /* Clock source control register7         */
#define CLK_SRC_CTRL8            (SYSTEM_BASE + 0xB0 )                 /* Clock source control register8         */
#define CLK_SRC_CTRL9            (SYSTEM_BASE + 0xB4 )                 /* Clock source control register9         */
#define CLK_SRC_CTRL10           (SYSTEM_BASE + 0xB8 )                 /* Clock source control register10        */
#define DB_CLK_CTRL1             (SYSTEM_BASE + 0xBC )                 /* Debounce clock control Register1       */
#define DB_CLK_CTRL2             (SYSTEM_BASE + 0xC0 )                 /* Debounce clock control Register2       */
#define LDO_TRIM_CTRL            (SYSTEM_BASE + 0x130)                 /* LDO trimming control register          */
#define TS_CTRL                  (SYSTEM_BASE + 0x134)                 /* Temperature sensor control register    */
#define VOL_DET_CTRL1            (SYSTEM_BASE + 0x138)                 /* LVD1 configuration register            */
#define VOL_DET_ST1              (SYSTEM_BASE + 0x13C)                 /* LVD1 status register                   */
#define VOL_DET_CTRL2            (SYSTEM_BASE + 0x140)                 /* LVD2 configuration register            */
#define VOL_DET_ST2              (SYSTEM_BASE + 0x144)                 /* LVD2 status register                   */
#define VOL_DET_CTRL3            (SYSTEM_BASE + 0x148)                 /* LVD3 configuration register            */
#define VOL_DET_ST3              (SYSTEM_BASE + 0x14C)                 /* LVD3 status register                   */
#define HIRC_CTRL                (SYSTEM_BASE + 0x170)                 /* HIRC control register                  */
#define LIRC_CTRL                (SYSTEM_BASE + 0x174)                 /* LIRC control register                  */
#define HOSC_CTRL                (SYSTEM_BASE + 0x178)                 /* HOSC control register                  */
#define LOSC_CTRL                (SYSTEM_BASE + 0x17C)                 /* LOSC control register                  */
#define PLL_CTRL1                (SYSTEM_BASE + 0x180)                 /* PLL control register1                  */
#define PLL_CTRL2                (SYSTEM_BASE + 0x184)                 /* PLL control register2                  */
#define PLLKDP_CTRL1             (SYSTEM_BASE + 0x188)                 /* PLLKDP control register1               */
#define PLLKDP_CTRL2             (SYSTEM_BASE + 0x18C)                 /* PLLKDP control register2               */
#define PLLI2S_CTRL1             (SYSTEM_BASE + 0x190)                 /* PLLI2S control register1               */
#define PLLI2S_CTRL2             (SYSTEM_BASE + 0x194)                 /* PLLI2S control register2               */
#define PLLSAI_CTRL1             (SYSTEM_BASE + 0x198)                 /* PLLSAI control register1               */
#define PLLSAI_CTRL2             (SYSTEM_BASE + 0x19C)                 /* PLLSAI control register2               */
#define REFCLK_SRC_CTRL          (SYSTEM_BASE + 0x1A0)                 /* PLL reference clock select register    */
#define CMU_CLK_ST               (SYSTEM_BASE + 0x1A4)                 /* CMU clock  status register             */
#define HOSC_MNT_CTRL1           (SYSTEM_BASE + 0x1C8)                 /* HOSC monitor control register1         */
#define HOSC_MNT_CTRL2           (SYSTEM_BASE + 0x1CC)                 /* HOSC monitor control register2         */
#define APB_TIMEOUT_VALUE        (SYSTEM_BASE + 0x200)                 /* APB timeout control register           */
#define M_M4_TXEV_REG            (SYSTEM_BASE + 0x204)                 /* Main M4 TXEV control register          */
#define S_M4_TXEV_REG            (SYSTEM_BASE + 0x208)                 /* Slave M4 TXEV control register         */
#define IO_BPS_REG1              (SYSTEM_BASE + 0x220)                 /* IO debounce enable register1           */
#define IO_BPS_REG2              (SYSTEM_BASE + 0x224)                 /* IO debounce enable register2           */
#define IO_BPS_REG3              (SYSTEM_BASE + 0x228)                 /* IO debounce enable register3           */
#define IO_BPS_REG4              (SYSTEM_BASE + 0x22C)                 /* IO debounce enable register4           */
#define IO_BPS_REG5              (SYSTEM_BASE + 0x230)                 /* IO debounce enable register5           */
#define IO_BPS_REG6              (SYSTEM_BASE + 0x234)                 /* IO debounce enable register6           */
#define IO_OD_REG1               (SYSTEM_BASE + 0x238)                 /* IO open drain enable register1         */
#define IO_OD_REG2               (SYSTEM_BASE + 0x23C)                 /* IO open drain enable register2         */
#define IO_OD_REG3               (SYSTEM_BASE + 0x240)                 /* IO open drain enable register3         */
#define IO_OD_REG4               (SYSTEM_BASE + 0x244)                 /* IO open drain enable register4         */
#define IO_OD_REG5               (SYSTEM_BASE + 0x248)                 /* IO open drain enable register5         */
#define IO_OD_REG6               (SYSTEM_BASE + 0x24C)                 /* IO open drain enable register6         */
#define IO_IE_REG1               (SYSTEM_BASE + 0x250)                 /* IO input enable register1              */
#define IO_IE_REG2               (SYSTEM_BASE + 0x254)                 /* IO input enable register2              */
#define IO_IE_REG3               (SYSTEM_BASE + 0x258)                 /* IO input enable register3              */
#define IO_IE_REG4               (SYSTEM_BASE + 0x25C)                 /* IO input enable register4              */
#define IO_IE_REG5               (SYSTEM_BASE + 0x260)                 /* IO input enable register5              */
#define IO_IE_REG6               (SYSTEM_BASE + 0x264)                 /* IO input enable register6              */
#define IO_PU_REG1               (SYSTEM_BASE + 0x268)                 /* IO pull up enable register1            */
#define IO_PU_REG2               (SYSTEM_BASE + 0x26C)                 /* IO pull up enable register2            */
#define IO_PU_REG3               (SYSTEM_BASE + 0x270)                 /* IO pull up enable register3            */
#define IO_PU_REG4               (SYSTEM_BASE + 0x274)                 /* IO pull up enable register4            */
#define IO_PU_REG5               (SYSTEM_BASE + 0x278)                 /* IO pull up enable register5            */
#define IO_PU_REG6               (SYSTEM_BASE + 0x27C)                 /* IO pull up enable register6            */
#define IO_PD_REG1               (SYSTEM_BASE + 0x280)                 /* IO pull down enable register1          */
#define IO_PD_REG2               (SYSTEM_BASE + 0x284)                 /* IO pull down enable register2          */
#define IO_PD_REG3               (SYSTEM_BASE + 0x288)                 /* IO pull down enable register3          */
#define IO_PD_REG4               (SYSTEM_BASE + 0x28C)                 /* IO pull down enable register4          */
#define IO_PD_REG5               (SYSTEM_BASE + 0x290)                 /* IO pull down enable register5          */
#define IO_PD_REG6               (SYSTEM_BASE + 0x294)                 /* IO pull down enable register6          */
#define IO_DS_REG1               (SYSTEM_BASE + 0x298)                 /* IO driven strength register1           */
#define IO_DS_REG2               (SYSTEM_BASE + 0x29C)                 /* IO driven strength register2           */
#define IO_DS_REG3               (SYSTEM_BASE + 0x2A0)                 /* IO driven strength register3           */
#define IO_DS_REG4               (SYSTEM_BASE + 0x2A4)                 /* IO driven strength register4           */
#define IO_DS_REG5               (SYSTEM_BASE + 0x2A8)                 /* IO driven strength register5           */
#define IO_DS_REG6               (SYSTEM_BASE + 0x2AC)                 /* IO driven strength register6           */
#define IO_DS_REG7               (SYSTEM_BASE + 0x2B0)                 /* IO driven strength register7           */
#define IO_DS_REG8               (SYSTEM_BASE + 0x2B4)                 /* IO driven strength register8           */
#define IO_DS_REG9               (SYSTEM_BASE + 0x2B8)                 /* IO driven strength register9           */
#define IO_DS_REG10              (SYSTEM_BASE + 0x2BC)                 /* IO driven strength register10          */
#define IO_DS_REG11              (SYSTEM_BASE + 0x2C0)                 /* IO driven strength register11          */
#define IO_MOD_REG1              (SYSTEM_BASE + 0x2C4)                 /* IO mode selection register1            */
#define IO_MOD_REG2              (SYSTEM_BASE + 0x2C8)                 /* IO mode selection register2            */
#define IO_MOD_REG3              (SYSTEM_BASE + 0x2CC)                 /* IO mode selection register3            */
#define IO_MOD_REG4              (SYSTEM_BASE + 0x2D0)                 /* IO mode selection register4            */
#define IO_MOD_REG5              (SYSTEM_BASE + 0x2D4)                 /* IO mode selection register5            */
#define IO_MOD_REG6              (SYSTEM_BASE + 0x2D8)                 /* IO mode selection register6            */
#define IO_MOD_REG7              (SYSTEM_BASE + 0x2DC)                 /* IO mode selection register7            */
#define IO_MOD_REG8              (SYSTEM_BASE + 0x2E0)                 /* IO mode selection register8            */
#define IO_MOD_REG9              (SYSTEM_BASE + 0x2E4)                 /* IO mode selection register9            */
#define IO_MOD_REG10             (SYSTEM_BASE + 0x2E8)                 /* IO mode selection register10           */
#define IO_MOD_REG11             (SYSTEM_BASE + 0x2EC)                 /* IO mode selection register11           */
#define AF_SEL_REG1              (SYSTEM_BASE + 0x2F0)                 /* IO mode selection register1            */
#define AF_SEL_REG2              (SYSTEM_BASE + 0x2F4)                 /* IO mode selection register2            */
#define AF_SEL_REG3              (SYSTEM_BASE + 0x2F8)                 /* IO mode selection register3            */
#define AF_SEL_REG4              (SYSTEM_BASE + 0x2FC)                 /* IO mode selection register4            */
#define AF_SEL_REG5              (SYSTEM_BASE + 0x300)                 /* IO mode selection register5            */
#define AF_SEL_REG6              (SYSTEM_BASE + 0x304)                 /* IO mode selection register6            */
#define AF_SEL_REG7              (SYSTEM_BASE + 0x308)                 /* IO mode selection register7            */
#define AF_SEL_REG8              (SYSTEM_BASE + 0x30C)                 /* IO mode selection register8            */
#define AF_SEL_REG9              (SYSTEM_BASE + 0x310)                 /* IO mode selection register9            */
#define AF_SEL_REG10             (SYSTEM_BASE + 0x314)                 /* IO mode selection register10           */
#define AF_SEL_REG11             (SYSTEM_BASE + 0x318)                 /* IO mode selection register11           */
#define AF_SEL_REG12             (SYSTEM_BASE + 0x31C)                 /* IO mode selection register12           */
#define AF_SEL_REG13             (SYSTEM_BASE + 0x320)                 /* IO mode selection register13           */
#define AF_SEL_REG14             (SYSTEM_BASE + 0x324)                 /* IO mode selection register14           */
#define AF_SEL_REG15             (SYSTEM_BASE + 0x328)                 /* IO mode selection register15           */
#define AF_SEL_REG16             (SYSTEM_BASE + 0x32C)                 /* IO mode selection register16           */
#define AF_SEL_REG17             (SYSTEM_BASE + 0x330)                 /* IO mode selection register17           */
#define AF_SEL_REG18             (SYSTEM_BASE + 0x334)                 /* IO mode selection register18           */
#define AF_SEL_REG19             (SYSTEM_BASE + 0x338)                 /* IO mode selection register19           */
#define AF_SEL_REG20             (SYSTEM_BASE + 0x33C)                 /* IO mode selection register20           */
#define AF_SEL_REG21             (SYSTEM_BASE + 0x340)                 /* IO mode selection register21           */
#define AF_SEL_REG22             (SYSTEM_BASE + 0x344)                 /* IO mode selection register22           */

#define IO_WK_PENDING            (1UL << 26)
#define PES_WK_PENDING           (1UL << 25)
#define HOSC_LOSS_PENDING        (1UL << 24)

#define FROM_SHUT_DOWN_MODE      (1UL << 10)
#define FROM_STANDBY_MODE        (1UL << 9)
#define FROM_STOP2_MODE          (1UL << 8)

#define RESET_BY_POR             (1UL << 0)
#define RESET_BY_WDG             (1UL << 1)
#define RESET_BY_LOCKUP          (1UL << 2)
#define RESET_BY_SYSREQ          (1UL << 3)
#define RESET_BY_STANDBY         (1UL << 4)
#define RESET_BY_IWDG            (1UL << 5)

#define IWDG_KR                  (IWATCHDOG_BASE + 0x00 )         /* Key register                       */
#define IWDG_PR                  (IWATCHDOG_BASE + 0x04 )         /* Prescaler register                 */
#define IWDG_RLR                 (IWATCHDOG_BASE + 0x08 )         /* Reload register                    */
#define IWDG_SR                  (IWATCHDOG_BASE + 0x0C )         /* Status register                    */
#define IWDG_VER                 (IWATCHDOG_BASE + 0xFC )         /* Version register                   */


#define POWER_MODE_STOP1         0
#define POWER_MODE_STOP2         1
#define POWER_MODE_STANDBY       2
#define POWER_MODE_SHUTDOWM      3

#define LDO_NORMAL_MODE          0
#define LDO_LOW_POWER_MODE       1


#define LP_WAKEUP_IRQn (FMC_IRQn)


/*   IO   */
#define readb(addr)     (*((volatile unsigned char  *)(addr)))
#define readw(addr)     (*((volatile unsigned short *)(addr)))
#define readl(addr)     (*((volatile unsigned long  *)(addr)))
#define writeb(v, addr) (*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
#define writew(v, addr) (*((volatile unsigned short *)(addr)) = (unsigned short)(v))
#define writel(v, addr) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))


/*  Type  */

typedef signed char             s8;
typedef signed short            s16;
typedef signed int              s32;
typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;


/* API */
void hirc_enable(void);
void hirc_disable(void);
u32  hirc_lock_flag(void);
u32  get_hirc_enable_bit(void);
void lirc_enable(void);
void lirc_disable(void);
u32  lirc_lock_flag(void);
void lirc_config(u32 tmr, u32 tmc);
u32  get_lirc_enable_bit(void);
void hosc_enable(void);
void hosc_disable(void);
u32  hosc_lock_flag(void);
u32  get_hosc_enable_bit(void);
void losc_enable(void);
void losc_disable(void);
u32  losc_lock_flag(void);
u32  get_losc_enable_bit(void);
void pll_refclk_config(u32 sel, u32 div);
u32  get_pll_refclk_sel(void);
void pll_enable(void);
void pll_disable(void);
u32  pll_lock_flag(void);
u32  get_pll_enable_bit(void);
void pllkdp_enable(void);
void pllkdp_disable(void);
u32  pllkdp_lock_flag(void);
u32  get_pllkdp_enable_bit(void);
void plli2s_enable(void);
void plli2s_disable(void);
u32  plli2s_lock_flag(void);
u32  get_plli2s_enable_bit(void);
void pllsai_enable(void);
void pllsai_disable(void);
u32  pllsai_lock_flag(void);
u32  get_pllsai_enable_bit(void);
void fclk_config(u32 clk_sel, u32 div);
u32  get_fclk_source(void);
u32  get_fclk_err_flag(void);
void clear_fclk_err_flag(void);
void hosc_monitor_enable(void);
void hosc_monitor_disable(void);
void hosc_loss_pend_enable(void);
void hosc_loss_pend_disable(void);
u32  get_hosc_period(void);
u32  hosc_loss_pend_flag(void);
void hosc_loss_pend_clear(void);
void lockup_enable(void);
void backup_reg_unlock(void);
void backup_reg_lock(void);
void pmu_enable(void);
void pmu_disable(void);
void power_mode_enable(u32 mode);
void ldo_mode_sel(u32 mode);
void ldo_hw_trim_enable(void);
void ldo_hw_trim_disable(void);
void goto_stop1_mode(void);
void goto_stop2_mode(void);
void goto_standby_mode(void);
void goto_sleep_mode(void);
void goto_shut_down_mode(void);
void reset_pend_clear(u32 pending_flag);

void dly(u32 ms);
void SysTick_Disable(void);

#ifdef __cplusplus
}
#endif

#endif
