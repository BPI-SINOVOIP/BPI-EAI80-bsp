#ifndef _SYS_H_
#define _SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>

/* System Control Register Layout Type Definition */
typedef struct {
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< System Configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                    /**< System Information Value Register: 0x10 */
    __IO uint32_t CFGCTL;                    /**< System Configuration Control Register: 0x14 */
    __IO uint32_t WSC;                       /**< Work Source Configuration Register: 0x18 */
    __IO uint32_t DBGCTRL;                   /**< Debug Ctrl Register: 0x1C */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2c */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[2];                   /**< Reserved System Clock Gating Register: 0x34~0x38 */
    __IO uint32_t STPMD;                     /**< Stop Mode Register: 0x3C */
    __IO uint32_t CLK0;                      /**< System Clock Selection Register 0: 0x40 */
    __IO uint32_t CLK1;                      /**< System Clock Selection Register 1: 0x44 */
    __IO uint32_t CLK2;                      /**< System Clock Selection Register 2: 0x48 */
    __IO uint32_t RCS[3];                    /**< Reserved Clock Source  Register: 0x4c~0x54 */
    __IO uint32_t DBCS[5];                   /**< Debounce Clock Selection Register: 0x58~0x68 */
    __IO uint32_t RESERVED0;                 /**< Reserved 0x6C */
    __IO uint32_t LIRCM;                     /**< LIRC Monitor Register: 0x70 */
    __IO uint32_t HIRCM0;                    /**< HIRC Monitor Register 1: 0x74 */
    __IO uint32_t HIRCM1;                    /**< HIRC Monitor Register 2: 0x78 */
    __IO uint32_t RESERVED1;                 /**< Reserved: 0x7C */
    __IO uint32_t ACR;                       /**< Analog PMU Control Register: 0x80 */
    __IO uint32_t VDCR;                      /**< Voltage Detector Control Register: 0x84 */
    __IO uint32_t VDSR;                      /**< Voltage Detector Status Register: 0x88 */
    __IO uint32_t RSVREG;                    /**< Reserved: 0x8C */
    __IO uint32_t IRC;                       /**< System IRC Control Register 0x90*/
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t TSC;                       /**< System TS Control Register 0x9c*/
    __IO uint32_t ANASEL;                    /**< System Analog Function Selection Register 0xA0*/
    __IO uint32_t RESERVED2[7];			     /**< Reserved: 0xA4 ~ 0xBC */
    __IO uint32_t DIPS;                      /**< Digital IP System Register: 0xC0 */
    __IO uint32_t RESERVED3[14];             /**< Reserved: 0xC4 ~ 0xF8 */
    __IO uint32_t APBTIMEOUTVAL;             /**< APB interface time-out counter value Register: 0xFC*/
    __IO uint32_t FLTBP[3];                  /**< Pad Filter Bypass: 0x100~0x108 */
    __IO uint32_t RESERVED4;                 /**< Reserved: 0x10C */
    __IO uint32_t PADE[3];                   /**< Pad Input Enable: 0x110 ~ 0x118 */
    __IO uint32_t RESERVED5;                 /**< Reserved: 0x11C */
    __IO uint32_t PINMUX[3];                 /**< Pad Multiplexer Control Register: 0x120~0x128 */
    __IO uint32_t RESERVED6;                 /**< Reserved: 0x12C */
    __IO uint32_t PUR[3];                    /**< GPIO Port Pull-up Register: 0x130~0x138 */
    __IO uint32_t RESERVED7;                 /**< Reserved: 0x13C */
    __IO uint32_t PMERR[3];                  /**< Pin Multiplexer Error Register: 0x140~0x148 */
    __IO uint32_t RESERVED8[3];              /**< Reserved: 0x14C ~ 0x154*/
} SYS_Device_T;


#define  SYS_CFGVAL_MODE                     ((uint32_t)0x00000001)            /* Selection of Function Mode */
#define  SYS_CFGVAL_SUBMODE                  ((uint32_t)0x0000000E)            /* Selection of Test SubMode */
#define  SYS_CFGVAL_SUBMODE_SHIFT            (1)
#define  SYS_CFGVAL_SUBMODE_FIELD(x)         (((uint32_t)(((uint32_t)(x))<<SYS_CFGVAL_SUBMODE_SHIFT))&SYS_CFGVAL_SUBMODE)
#define  SYS_CFGVAL_PLL_CS                   ((uint32_t)0x00000010)            /* PLL Clock Source, 0: Internal RC OSC 1: External Crystal OSC */

#define  SYS_CFGTRG_MODE                     ((uint32_t)0x00000001)            /* System Function Mode Trigger */
#define  SYS_CFGTRG_SUBMODE                  ((uint32_t)0x00000002)            /* System Test SubMode Trigger */
#define  SYS_CFGTRG_PLL_CS                   ((uint32_t)0x00000004)            /* PLL Clock Source Trigger */

#define  SYS_CFGMISC_BOOTSEQ_MASK            ((uint32_t)0x00000003)
#define  SYS_CFGMISC_BOOTSEQ_FIELD(x)        ((uint32_t)(((uint32_t)(x))&SYS_CFGMISC_BOOTSEQ_MASK))
#define  SYS_CFGMISC_BOOTSEQ(reg)            ((reg)&SYS_CFGMISC_BOOTSEQ_MASK)
#define  SYS_CFGMISC_PMU_EN                  ((uint32_t)0x00000008)
#define  SYS_CFGMISC_LOCKUP_RESET            ((uint32_t)0x00000010)
//#define  SYS_CFGMISC_PLL_BYPASS            ((uint32_t)0x00000100)            /* Bypass PLL */

#define  SYS_SCG_ADC                         ((uint32_t)0x00000800)             /* ADC peripheral clock gate */
#define  SYS_SCG_ETM                         ((uint32_t)0x00000400)             /* ETM peripheral clock gate */
#define  SYS_SCG_SPI                         ((uint32_t)0x00000100)             /* SPI peripheral clock gate */
#define  SYS_SCG_CRC                         ((uint32_t)0x00000080)             /* CRC peripheral clock gate */
#define  SYS_SCG_HWIR                        ((uint32_t)0x00000040)             /* HWIR peripheral clock gate */
#define  SYS_SCG_I2C2                        ((uint32_t)0x00000020)             /* I2C2 peripheral clock gate */
#define  SYS_SCG_UART2                       ((uint32_t)0x00000010)             /* UART2 peripheral clock gate */
#define  SYS_SCG_UART1                       ((uint32_t)0x00000008)             /* UART1 peripheral clock gate */
#define  SYS_SCG_UART0                       ((uint32_t)0x00000004)             /* UART0 peripheral clock gate */
#define  SYS_SCG_I2C1                        ((uint32_t)0x00000002)             /* I2C1 peripheral clock gate */
#define  SYS_SCG_I2C0                        ((uint32_t)0x00000001)             /* I2C0 peripheral clock gate */

#define  SYS_MCS_WDT_SWITCH                  ((uint32_t)0x00040000)            /* Watchdog multiplex switch */
#define  SYS_MCS_RTC2_SWITCH                 ((uint32_t)0x00004000)            /* RTC2 multiplex switch */
#define  SYS_MCS_RTC1_SWITCH                 ((uint32_t)0x00000400)            /* RTC1 multiplex switch */
#define  SYS_MCS_RTC0_SWITCH                 ((uint32_t)0x00000040)            /* RTC0 multiplex switch */

#define  SYS_SCS_CS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SCS_CS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SCS_CS_MASK))
#define  SYS_SCS_CS(reg)                     ((reg)&SYS_SCS_CS_MASK)
#define  SYS_SCS_PLL_RC                      0                                 /* System Clock Source is PLL Referrence Clock (Internal RC Oscillator or External High Frequency Oscillator */
#define  SYS_SCS_PLL_6X                      1                                 /* System Clock Source is PLL output with Multiplication Factor 6 */
#define  SYS_SCS_PLL_3X                      2                                 /* System Clock Source is PLL output with Multiplication Factor 3 */
#define  SYS_SCS_LF_OSC                      3                                 /* System Clock Source is Low Frequency Oscillator cowork with external crystal or resonator */
#define  SYS_SCS_PLL_BYPASS                  ((uint32_t)0x00000100)            /* Bypass PLL */


#define  SYS_SCS_PLLCLK_REF                  ((uint32_t)0x00000010)            /**< PLL reference clock */

#define  SYS_SCS_SWT_EN                      ((uint32_t)0x00000004)            /* Clock Switch Module Enabled */

#define  SYS_MCS_UART0_CLKSRC                ((uint32_t)0x00000001)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART1_CLKSRC                ((uint32_t)0x00000002)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART2_CLKSRC                ((uint32_t)0x00000004)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */

#define  SYS_SPS_PS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SPS_PS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SPS_PS_MASK))
#define  SYS_SPS_PS(reg)                     ((reg)&SYS_SPS_PS_MASK)
#define  SYS_SPS_1                           0                                 /* System Clock Prescaler Factor is 1 */
#define  SYS_SPS_2                           1                                 /* System Clock Prescaler Factor is 2 */
#define  SYS_SPS_4                           2                                 /* System Clock Prescaler Factor is 4 */
#define  SYS_SPS_8                           3                                 /* System Clock Prescaler Factor is 8 */
#define  SYS_SPS_16                          4                                 /* System Clock Prescaler Factor is 16 */
#define  SYS_SPS_32                          5                                 /* System Clock Prescaler Factor is 32 */
#define  SYS_SPS_64                          6                                 /* System Clock Prescaler Factor is 64 */
#define  SYS_SPS_128                         7                                 /* System Clock Prescaler Factor is 128 */

#define  SYS_IRC_CRY8M                       ((uint32_t)0x00000001)            /* IRC 8MHz power */
#define  SYS_IRC_CRY1K                       ((uint32_t)0x00000002)            /* IRC 1KHz power  */

#define  SYS_OSC_CRY8M                       ((uint32_t)0x00000001)            /* OSC 8MHz power */
#define  SYS_OSC_CRY32K                      ((uint32_t)0x00000100)            /* OSC 32KHz power  */

#define  SYS_ANASEL_HOSCIN                   ((uint32_t)0x00000001)            /* OSC 8MHz selection in */
#define  SYS_ANASEL_HOSCOUT                  ((uint32_t)0x00000002)            /* OSC 8MHz selection out */
#define  SYS_ANASEL_LOSCIN                   ((uint32_t)0x00000004)            /* OSC 32KHz selection in */
#define  SYS_ANASEL_LOSCOUT                  ((uint32_t)0x00000008)            /* OSC 32KHz selection out */

#define  SYS_PLL_PDB                         ((uint32_t)0x00000001)            /* PLL power down  */



#if ((defined(CHIP_GM6611)) || (defined(CHIP_GM6621)) || (defined(CHIP_GM6611B)))
#define  SYS_SRS_LVD                         ((uint32_t)0x00000001)            /* System Reset by Low Voltage Detect(LVD) Circuit or Power On Reset(POR) Circuit  */
#define  SYS_SRS_WDOG                        ((uint32_t)0x00000002)            /* System Reset by Watchdog Module */
#define  SYS_SRS_LOCKUP                      ((uint32_t)0x00000004)            /* System Reset by Cortex-M Lockup Event */
#define  SYS_SRS_SW                          ((uint32_t)0x00000008)            /* System Reset by Software Request */
#define  SYS_SRS_HOSC_LSRSTPD                ((uint32_t)0x00000010)            /* System Reset by HOSC Loss */
#define  SYS_SRS_HOSC_LSPD                   ((uint32_t)0x00000080)            /* HOSC Loss Pending */

#define  SYS_CFGVAL_MODE                     ((uint32_t)0x00000001)            /* Selection of Function Mode */
#define  SYS_CFGVAL_SUBMODE                  ((uint32_t)0x0000000E)            /* Selection of Test SubMode */
#define  SYS_CFGVAL_SUBMODE_SHIFT            (1)
#define  SYS_CFGVAL_SUBMODE_FIELD(x)         (((uint32_t)(((uint32_t)(x))<<SYS_CFGVAL_SUBMODE_SHIFT))&SYS_CFGVAL_SUBMODE)
#define  SYS_CFGVAL_PLL_CS                   ((uint32_t)0x00000010)            /* PLL Clock Source, 0: Internal RC OSC 1: External Crystal OSC */

#define  SYS_CFGTRG_MODE                     ((uint32_t)0x00000001)            /* System Function Mode Trigger */
#define  SYS_CFGTRG_SUBMODE                  ((uint32_t)0x00000002)            /* System Test SubMode Trigger */
#define  SYS_CFGTRG_PLL_CS                   ((uint32_t)0x00000004)            /* PLL Clock Source Trigger */

#define  SYS_CFGMISC_BOOTSEQ_MASK            ((uint32_t)0x00000003)
#define  SYS_CFGMISC_BOOTSEQ_FIELD(x)        ((uint32_t)(((uint32_t)(x))&SYS_CFGMISC_BOOTSEQ_MASK))
#define  SYS_CFGMISC_BOOTSEQ(reg)            ((reg)&SYS_CFGMISC_BOOTSEQ_MASK)
#define  SYS_CFGMISC_PMU_EN                  ((uint32_t)0x00000008)
#define  SYS_CFGMISC_LOCKUP_RESET            ((uint32_t)0x00000010)
//#define  SYS_CFGMISC_PLL_BYPASS            ((uint32_t)0x00000100)            /* Bypass PLL */

#define  SYS_SCG_ADC                         ((uint32_t)0x00000800)             /* ADC peripheral clock gate */
#define  SYS_SCG_ETM                         ((uint32_t)0x00000400)             /* ETM peripheral clock gate */
#define  SYS_SCG_SPI                         ((uint32_t)0x00000100)             /* SPI peripheral clock gate */
#define  SYS_SCG_CRC                         ((uint32_t)0x00000080)             /* CRC peripheral clock gate */
#define  SYS_SCG_HWIR                        ((uint32_t)0x00000040)             /* HWIR peripheral clock gate */
#define  SYS_SCG_I2C2                        ((uint32_t)0x00000020)             /* I2C2 peripheral clock gate */
#define  SYS_SCG_UART2                       ((uint32_t)0x00000010)             /* UART2 peripheral clock gate */
#define  SYS_SCG_UART1                       ((uint32_t)0x00000008)             /* UART1 peripheral clock gate */
#define  SYS_SCG_UART0                       ((uint32_t)0x00000004)             /* UART0 peripheral clock gate */
#define  SYS_SCG_I2C1                        ((uint32_t)0x00000002)             /* I2C1 peripheral clock gate */
#define  SYS_SCG_I2C0                        ((uint32_t)0x00000001)             /* I2C0 peripheral clock gate */

#define  SYS_MCS_WDT_SWITCH                  ((uint32_t)0x00040000)            /* Watchdog multiplex switch */
#define  SYS_MCS_RTC2_SWITCH                 ((uint32_t)0x00004000)            /* RTC2 multiplex switch */
#define  SYS_MCS_RTC1_SWITCH                 ((uint32_t)0x00000400)            /* RTC1 multiplex switch */
#define  SYS_MCS_RTC0_SWITCH                 ((uint32_t)0x00000040)            /* RTC0 multiplex switch */

#define  SYS_SCS_CS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SCS_CS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SCS_CS_MASK))
#define  SYS_SCS_CS(reg)                     ((reg)&SYS_SCS_CS_MASK)
#define  SYS_SCS_PLL_RC                      0                                 /* System Clock Source is PLL Referrence Clock (Internal RC Oscillator or External High Frequency Oscillator */
#define  SYS_SCS_PLL_6X                      1                                 /* System Clock Source is PLL output with Multiplication Factor 6 */
#define  SYS_SCS_PLL_3X                      2                                 /* System Clock Source is PLL output with Multiplication Factor 3 */
#define  SYS_SCS_LF_OSC                      3                                 /* System Clock Source is Low Frequency Oscillator cowork with external crystal or resonator */
#define  SYS_SCS_PLL_BYPASS                  ((uint32_t)0x00000100)            /* Bypass PLL */
#define  SYS_SCS_CORE_SWITCH                 ((uint32_t)0x00000004)            /* FCLK multiplex switch */
#define  SYS_PLL_PLLCLK_REF                  ((uint32_t)0x00000002)            /**< PLL reference clock */

#define  SYS_SCS_SWT_EN                      ((uint32_t)0x00000004)            /* Clock Switch Module Enabled */

#define  SYS_MCS_UART0_CLKSRC                ((uint32_t)0x00000001)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART1_CLKSRC                ((uint32_t)0x00000002)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART2_CLKSRC                ((uint32_t)0x00000004)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */

#define  SYS_SPS_PS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SPS_PS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SPS_PS_MASK))
#define  SYS_SPS_PS(reg)                     ((reg)&SYS_SPS_PS_MASK)
#define  SYS_SPS_1                           0                                 /* System Clock Prescaler Factor is 1 */
#define  SYS_SPS_2                           1                                 /* System Clock Prescaler Factor is 2 */
#define  SYS_SPS_4                           2                                 /* System Clock Prescaler Factor is 4 */
#define  SYS_SPS_8                           3                                 /* System Clock Prescaler Factor is 8 */
#define  SYS_SPS_16                          4                                 /* System Clock Prescaler Factor is 16 */
#define  SYS_SPS_32                          5                                 /* System Clock Prescaler Factor is 32 */
#define  SYS_SPS_64                          6                                 /* System Clock Prescaler Factor is 64 */
#define  SYS_SPS_128                         7                                 /* System Clock Prescaler Factor is 128 */

#define  SYS_IRC_CRY8M                       ((uint32_t)0x00000001)            /* IRC 8MHz power */
#define  SYS_IRC_CRY1K                       ((uint32_t)0x00000002)            /* IRC 1KHz power  */

#define  SYS_OSC_CRY8M                       ((uint32_t)0x00000001)            /* OSC 8MHz power */
#define  SYS_OSC_CRY32K                      ((uint32_t)0x00000100)            /* OSC 32KHz power */

#if defined(CHIP_GM6621)
#define  SYS_OSC_OUTSEL                      ((uint32_t)0x00010000)            /* OSC out select */
#define  SYS_AOP_CRY32K                      ((uint32_t)0x00000001)            /* OSC 32K power */
#define  SYS_AOP_SRS_STANDBYFLG              ((uint32_t)0x80000000)            /* Reset from Standby Flag*/
#define  SYS_AOP_LSCCTRL_OSCPINSEL           ((uint32_t)0x80000000)            /* LOSC use HIN/OUT pin*/

#define  SYS_PLL_CLKREF_MASK                 ((uint32_t)0x00000006)            /* PLL reference clock mask */
#define  SYS_PLL_CLKREF_HIRC                 ((uint32_t)0x00000000)            /* PLL reference from HIRC */
#define  SYS_PLL_CLKREF_HOSC                 ((uint32_t)0x00000002)            /* PLL reference from HOSC */
#define  SYS_PLL_CLKREF_LOSC                 ((uint32_t)0x00000004)            /* PLL reference from LOSC */
#define  SYS_PLL_DIVSEL_LFEN                 ((uint32_t)0x00001000)            /* Divider selection valid only for LOSC*/
#endif

#define  SYS_ANASEL_HOSCIN                   ((uint32_t)0x00000001)            /* OSC 8MHz selection in */
#define  SYS_ANASEL_HOSCOUT                  ((uint32_t)0x00000002)            /* OSC 8MHz selection out */
#define  SYS_ANASEL_LOSCIN                   ((uint32_t)0x00000004)            /* OSC 32KHz selection in */
#define  SYS_ANASEL_LOSCOUT                  ((uint32_t)0x00000008)            /* OSC 32KHz selection out */

#define  SYS_PLL_PWRON                       ((uint32_t)0x00000001)            /* PLL power on */
#define  SYS_DBG_WDOG_STOP                   ((uint32_t)0x00000001)            /* DBG_CFG_CTRL: bit0  WDOG will be stoped*/
#define  SYS_DGB_WDOG_WRITEDONE              ((uint32_t)0x00010000)            /* DBG_CFG_CTRL: bit16 WDOG wirte done flag*/
#endif




/* ETM_Sys_Reg Bit Fields */
#define SYS_ESS_EN                           ((uint32_t)0x00000004)            /* Enable ETM sync */
#define SYS_ESS_SEL_MASK                     ((uint32_t)0x00000003)            /* ETM Sync source selection */

#if (defined(CHIP_GM6611) || defined(CHIP_GM6621) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
#define  SYS_FLTCPS_CLK0_MASK               ((uint32_t)0x00000600)
#define  SYS_FLTCPS_CLK0_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<9)&SYS_FLTCPS_CLK0_MASK))
#define  SYS_FLTCPS_CLK1_MASK               ((uint32_t)0x00003800)
#define  SYS_FLTCPS_CLK1_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<11)&SYS_FLTCPS_CLK1_MASK))
#define  SYS_FLTCPS_CLK2_MASK               ((uint32_t)0x0000c000)
#define  SYS_FLTCPS_CLK2_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<14)&SYS_FLTCPS_CLK2_MASK))
#else
#define  SYS_FLTCPS_CLK0_MASK               ((uint32_t)0x00000003)
#define  SYS_FLTCPS_CLK0_FIELD(x)           ((uint32_t)(((uint32_t)(x))&SYS_FLTCPS_CLK0_MASK))
#define  SYS_FLTCPS_CLK1_MASK               ((uint32_t)0x00000070)
#define  SYS_FLTCPS_CLK1_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<4)&SYS_FLTCPS_CLK1_MASK))
#define  SYS_FLTCPS_CLK2_MASK               ((uint32_t)0x00000300)
#define  SYS_FLTCPS_CLK2_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<8)&SYS_FLTCPS_CLK2_MASK))
#endif

#define  SYSDEV                               ((SYS_Device_T *)(0x40000000u))

#if defined(CHIP_GM6621)
#define  AOPDEV                               ((AOP_Device_T *)(0x40039000u))
#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _SYS_H_ */
