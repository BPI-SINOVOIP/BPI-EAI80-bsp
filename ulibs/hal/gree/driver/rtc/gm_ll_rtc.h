/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_rtc.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/06/13
 *
 * @brief         RTC Header For GM6721
 *
 * @note
 *    2017/06/13, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __GM_LL_RTC_H_
#define __GM_LL_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_soc.h>

//#define LL_RTC_FPGA_CLK 32768

/* RTC Controller Register Layout Type Definition */
typedef struct
{
    __IO uint32_t TM0;              /**< RTC Timer Register 0,                      offset: 0x0 (6611B off + 0xC00) */
    __IO uint32_t TM1;              /**< RTC Timer Register 1,                      offset: 0x4         */
    __IO uint32_t CTRL;             /**< RTC Control Register,                      offset: 0x8         */
    __IO uint32_t ALARM;            /**< RTC Alarm Register,                        offset: 0xC         */
    __IO uint32_t ALRMASK;          /**< RTC Alarm Mask Register,                   offset: 0x10        */
    __IO uint32_t TICKVAL;          /**< RTC Tick Value Register,                   offset: 0x14        */
    __IO uint32_t SUBSEC;           /**< RTC Sub Second Register,                   offset: 0x18        */
    __IO uint32_t TIMSTAMP0;        /**< RTC Time Stamp Register 0,                 offset: 0x1C        */
    __IO uint32_t TIMSTAMP1;        /**< RTC Time Stamp Register 1,                 offset: 0x20        */
    __IO uint32_t DCALCTL;          /**< RTC Calibration Register,                  offset: 0x24        */
    __IO uint32_t FLAG;             /**< RTC Flag Register,                         offset: 0x28        */
    __IO uint32_t INTEN;            /**< RTC Interrupt Mask Register,               offset: 0x2C        */
    __IO uint32_t INTCLR;           /**< RTC Interrupt Clear Register,              offset: 0x30        */
    __IO uint32_t TAMPER1;          /**< RTC Tamper1 Control Register,              offset: 0x34        */
    __IO uint32_t TAMPER2;          /**< RTC Tamper2 Control Register,              offset: 0x38        */
    __IO uint32_t TAMPER3;          /**< RTC Tamper3 Control Register,              offset: 0x3C        */
    __IO uint32_t PROTECT;          /**< RTC Protect Register,                      offset: 0x40        */
    __IO uint32_t ALASSEC;          /**< RTC Alarm of Subsecond Register,           offset: 0x44        */
    __IO uint32_t WAKEUPT0;         /**< RTC Wakeup Timer Register0,                offset: 0x48        */
    __IO uint32_t WAKEUPT1;         /**< RTC Wakeup Timer Register1,                offset: 0x4C        */
    __IO uint32_t PES;              /**< RTC Event System Control Register,         offset: 0x50        */
    __IO uint32_t SPECIAL;          /**< RTC Special Register,                      offset: 0x54        */
    __IO uint32_t RTCIOCTRL;        /**< RTC Special_IO_CONTROL_REGISTER,           offset: 0x58        */
    __IO uint32_t RESV3[9];         /**< RTC Reserved Space,                                0x5A~0x7C   */
    __IO uint32_t BACKUP[20];       /**< RTC Backup Register0~19,                   0x80~0xCC           */
    __IO uint32_t RESV4[11];        /**< RTC Reserved Space,                                0xD0~0xF8   */
    __IO uint32_t VER;              /**< RTC Version Register,                      offset: 0xFC        */
} RTC_Device_T;

#define RTC                         ((RTC_Device_T *)RTC_TOP_BASE)

#define LL_RTC_MASK_ALL             (0xFFFFFFFF)

typedef enum
{
    DATE_TIME_RTCTIME   = 0,
    DATE_TIME_TIMESTAMP = 1,
} DATE_TIME_TYPE;

/*************** RTC Protect Bitfiled ******************/
#define LL_RTC_PROTECT_UNLOCK  ((uint32_t)0x1ACCE551)
#define LL_RTC_PROTECT_LOCK    ((uint32_t)0x0)

/*************** RTC Bin Bcd Common Bitfiled ******************/
#define LL_RTC_BIN_BCD_MODE        ((uint32_t)0x00000080)
#define LL_RTC_BCD2BIN(x)          (((x) & 0x0f) + ((x) >> 4) * 10)
#define LL_RTC_BIN2BCD(x)          ((((x) / 10) << 4) + (x) % 10)

/*************** RTC Bin Mode Bitfiled ******************/
#define LL_RTC_BIN_YEAR_POS        (16)
#define LL_RTC_BIN_YEAR_MASK       ((uint32_t)(0x3FFF<<LL_RTC_BIN_YEAR_POS))
#define LL_RTC_BIN_WEEK_POS        (13)
#define LL_RTC_BIN_WEEK_MASK       ((uint32_t)(0x7<<LL_RTC_BIN_WEEK_POS))
#define LL_RTC_BIN_MONTH_POS       (8)
#define LL_RTC_BIN_MONTH_MASK      ((uint32_t)(0x1F<<LL_RTC_BIN_MONTH_POS))
#define LL_RTC_BIN_DAY_POS         (0)
#define LL_RTC_BIN_DAY_MASK        ((uint32_t)(0x3F<<LL_RTC_BIN_DAY_POS))
#define LL_RTC_BIN_AMPM_POS        (22)
#define LL_RTC_BIN_AMPM_MASK       ((uint32_t)(0x1<<LL_RTC_BIN_AMPM_POS))
#define LL_RTC_BIN_HOUR_POS        (16)
#define LL_RTC_BIN_HOUR_MASK       ((uint32_t)(0x3F<<LL_RTC_BIN_HOUR_POS))
#define LL_RTC_BIN_MINUTE_POS      (8)
#define LL_RTC_BIN_MINUTE_MASK     ((uint32_t)(0x7F<<LL_RTC_BIN_MINUTE_POS))
#define LL_RTC_BIN_SECOND_POS      (0)
#define LL_RTC_BIN_SECOND_MASK     ((uint32_t)(0x7F<<LL_RTC_BIN_SECOND_POS))
#define LL_RTC_BIN_SUBSECOND_POS   (0)
#define LL_RTC_BIN_SUBSECOND_MASK  ((uint32_t)(0xFFFF<<LL_RTC_BIN_SUBSECOND_POS))
#define LL_RTC_BIN_FORMAT12_POS    (0)
#define LL_RTC_BIN_FORMAT12_MASK   ((uint32_t)(0x1<<LL_RTC_BIN_FORMAT12_POS))
#define LL_RTC_BIN_TIME(h, m, s, ampm) (((ampm)<<LL_RTC_BIN_AMPM_POS)|((h)<<LL_RTC_BIN_HOUR_POS)|((m)<<LL_RTC_BIN_MINUTE_POS)|((s)<<LL_RTC_BIN_SECOND_POS))
#define LL_RTC_BIN_YEAR(y, m, d, w)    (((y)<<LL_RTC_BIN_YEAR_POS)|((w)<<LL_RTC_BIN_WEEK_POS)|((m)<<LL_RTC_BIN_MONTH_POS)|((d)<<LL_RTC_BIN_DAY_POS))

/*************** RTC Bcd Mode Bitfiled ******************/
#define LL_RTC_BCD_H(x)                    (((x)&0xF0)>>4)
#define LL_RTC_BCD_L(x)                    ((x)&0x0F)
#define LL_RTC_BCD_YEAR(year_h, year_l)    (((LL_RTC_BCD_H(year_h))<<12)|((LL_RTC_BCD_L(year_h))<<8)|((LL_RTC_BCD_H(year_l))<<4)|((LL_RTC_BCD_L(year_l))<<0))
#define LL_RTC_BCD_MONTH(month)            (((LL_RTC_BCD_H(month))<<4)|((LL_RTC_BCD_L(month))<<0))
#define LL_RTC_BCD_DAY(day)                (((LL_RTC_BCD_H(day))<<4)|((LL_RTC_BCD_L(day))<<0))
#define LL_RTC_BCD_WEEK(week)              ((LL_RTC_BCD_L(week))<<0)
#define LL_RTC_BCD_TIME(h,m,s,ampm)        (((LL_RTC_BCD_L(ampm))<<22)|((LL_RTC_BCD_H(h))<<20)|((LL_RTC_BCD_L(h))<<16)|((LL_RTC_BCD_H(m))<<12)|((LL_RTC_BCD_L(m))<<8)|\
        ((LL_RTC_BCD_H(s))<<4)|((LL_RTC_BCD_L(s))<<0))
#define LL_RTC_BCD_DATE(y,m,d,w)           ((y<<LL_RTC_BIN_YEAR_POS)|(m<<LL_RTC_BIN_MONTH_POS)|(d<<LL_RTC_BIN_DAY_POS)|(w<<LL_RTC_BIN_WEEK_POS))
#define LL_RTC_BCD_YEAR_HIGH_GET_VALUE(v)  ((((v)&(0x3<<28))>>24)|(((v)&(0xF<<24))>>24))
#define LL_RTC_BCD_YEAR_LOW_GET_VALUE(v)   ((((v)&(0xF<<20))>>16)|(((v)&(0xF<<16))>>16))
#define LL_RTC_BCD_MONTH_GET_VALUE(v)      ((((v)&(0x1<<12))>>8)|(((v)&(0xF<<8))>>8))
#define LL_RTC_BCD_DAY_GET_VALUE(v)        (((v)&(0x3<<4))|((v)&(0xF)))
#define LL_RTC_BCD_WEEK_GET_VALUE(v)       (((v)&(0x7<<13))>>13)
#define LL_RTC_BCD_YEAR_HIGH(v)            LL_RTC_BCD_YEAR_HIGH_GET_VALUE(v)
#define LL_RTC_BCD_YEAR_LOW(v)             LL_RTC_BCD_YEAR_LOW_GET_VALUE(v)
#define LL_RTC_BCD_HOUR_GET_VALUE(v)       ((((v)&(0x3<<20))>>16)|(((v)&(0xF<<16))>>16))
#define LL_RTC_BCD_MINUTE_GET_VALUE(v)     ((((v)&(0x7<<12))>>8)|(((v)&(0xF<<8))>>8))
#define LL_RTC_BCD_SECOND_GET_VALUE(v)     ((((v)&(0x7<<4)))|(((v)&(0xF))))
#define LL_RTC_BCD_SUBSECOND_GET_VALUE(v)  ((((v)&(0x7<<4)))|(((v)&(0xF))))
#define LL_RTC_BCD_AMPM_GET_VALUE(v)       (((v)&(0x1<<22))>22)

/*************** RTC Clock Frequency Bitfiled ******************/
#define LL_RTC_CLK_FREQ_32K            ((uint32_t)32768)
#define LL_RTC_CLK_FREQ_8M             ((uint32_t)8000000)
#define LL_RTC_CLK_FREQ_48M            ((uint32_t)48000000)

/*************** RTC Tick Cycles Bitfiled ******************/
#define LL_RTC_PRESCALE_POS        ((uint32_t)16)
#define LL_RTC_PRESCALE_MASK       ((uint32_t)(0x7F<<LL_RTC_PRESCALE_POS))

#define LL_RTC_TICKVAULE_POS       ((uint32_t)0)
#define LL_RTC_TICKVALUE_MASK      ((uint32_t)(0xFFFF<<LL_RTC_TICKVAULE_POS))

/*************** RTC Interrupt Bitfiled ******************/
#define LL_RTC_IRQ_PES_EN              (1<<17)
#define LL_RTC_IRQ_YEAR_EN             (1<<16)
#define LL_RTC_IRQ_MONTH_EN            (1<<15)
#define LL_RTC_IRQ_DAY_EN              (1<<14)
#define LL_RTC_IRQ_HOUR_EN             (1<<13)
#define LL_RTC_IRQ_MINUTE_EN           (1<<12)
#define LL_RTC_IRQ_SECOND_EN           (1<<11)
#define LL_RTC_IRQ_WAKEUP_WP4F_EN      (1<<10) /**< 1000ms@RTCCLK  */
#define LL_RTC_IRQ_WAKEUP_WP3F_EN      (1<<9)  /**< 62.5ms@RTCCLK  */
#define LL_RTC_IRQ_WAKEUP_WP2F_EN      (1<<8)  /**< 125ms@RTCCLK   */
#define LL_RTC_IRQ_WAKEUP_WP1F_EN      (1<<7)  /**< 250ms@RTCCLK   */
#define LL_RTC_IRQ_WAKEUP_WP0F_EN      (1<<6)  /**< 500ms@RTCCLK   */
#define LL_RTC_IRQ_TIMSTAMPOV_IE       (1<<5)  /**< time-stamp overflow event  */
#define LL_RTC_IRQ_TIMSTAMP_IE         (1<<4)
#define LL_RTC_IRQ_TAMPER3_IE          (1<<3)
#define LL_RTC_IRQ_TAMPER2_IE          (1<<2)
#define LL_RTC_IRQ_TAMPER1_IE          (1<<1)
#define LL_RTC_IRQ_ALARM_IE            (1<<0)
#define LL_RTC_IRQ_CLEAR_ALL_PENDING_FLAG_MASK     ((uint32_t)1<<31)

/*************** RTC Tamper Bitfiled ******************/
#define LL_RTC_TAMPPUDIS_POS               (10)    /**< 0:precharge, enable internal pull-up */
#define LL_RTC_TAMPPRCH_POS                (8)     /**< Pull-up duration: 2^x RTCLK cycles (0~3) */
#define LL_RTC_TAMPFLT_POS                 (6)     /**< Filter count: 2^x consecutive samples (0~3) */
#define LL_RTC_TAMPFRE_POS                 (3)     /**< Tamper input sample frequency: RTCLK/(2^x)  (0~7) */
#define LL_RTC_TAMPTS                      (2)     /**< 1:save timestamp */
#define LL_RTC_TAMPTRG                     (1)     /**< Trigger 1: high level */
#define LL_RTC_TAMPE                       (0)     /**< 1:enable */

/**< tamper1 input is XPD5(IO 109), the default level is high. */
/**< tamper2 input is XPA4(IO  92), the default level is high. */
#define LL_RTC_TAMPER_TRIGGER_LEVEL        (0) /**< low level to trigger */

/*************** RTC Alarm Bitfiled ******************/
#define LL_RTC_ALARM_EN                    (1<<1)

#define LL_RTC_ALARM_SUBSEC_ALLBIT_CHECK   (15)

#define LL_RTC_ALARM_SUBSEC_AE_POS     (0x4)   /**< SUBSEC[14:x] are don't care, only SUBCSEC[x-1:0]are compared */
#define LL_RTC_ALARM_DAY_AE_POS        (0x3)
#define LL_RTC_ALARM_HOUR_AE_POS       (0x2)
#define LL_RTC_ALARM_MIN_AE_POS        (0x1)
#define LL_RTC_ALARM_SEC_AE_POS        (0x0)

#define LL_RTC_ALARM_SUBSEC_AE_MASK    (0xF  << LL_RTC_ALARM_SUBSEC_AE_POS)

#define LL_RTC_ALARM_WD_TEST           (0x1)
#define LL_RTC_ALARM_WD_POS            (30)    /**< 0: the DAY is the day of month, 1: the DAY[3:0] is the week day */
#define LL_RTC_ALARM_WD_MASK           (LL_RTC_ALARM_WD_TEST  << LL_RTC_ALARM_WD_POS)
#define LL_RTC_ALARM_DAY_TEST          (0x3F)
#define LL_RTC_ALARM_DAY_POS           (24)
#define LL_RTC_ALARM_DAY_MASK          (LL_RTC_ALARM_DAY_TEST << LL_RTC_ALARM_DAY_AE_POS)
#define LL_RTC_ALARM_AMPM_POS          (22)
#define LL_RTC_ALARM_AMPM_MASK         (0x1  << LL_RTC_ALARM_AMPM_POS)
#define LL_RTC_ALARM_HOUR_TEST         (0x3F)
#define LL_RTC_ALARM_HOUR_POS          (16)
#define LL_RTC_ALARM_HOUR_MASK         (LL_RTC_ALARM_HOUR_TEST << LL_RTC_ALARM_HOUR_POS)
#define LL_RTC_ALARM_MIN_POS           (8)
#define LL_RTC_ALARM_MIN_TEST          (0x3F)
#define LL_RTC_ALARM_MIN_MASK          (LL_RTC_ALARM_MIN_TEST << LL_RTC_ALARM_MIN_POS)
#define LL_RTC_ALARM_SEC_POS           (0)
#define LL_RTC_ALARM_SEC_TEST          (0x3F)
#define LL_RTC_ALARM_SEC_MASK          (LL_RTC_ALARM_SEC_TEST << LL_RTC_ALARM_SEC_POS)

/**< set RTC_OUT Output  */
/**< GM6621 FPGA - XPB5 - IO3 */
/**< GM6621 ES   - XPB8       */
#define LL_RTC_RTCOUT1                 (HW32_REG(0x4000015C))
#define LL_RTC_RTCOUT2                 (HW32_REG(0x40000144))

#define LL_RTC_RTCOUT_COE_EN           (0x1<<6)
#define LL_RTC_RTCOUT_COSEL_EN         (0x1<<5)

/**< Bin Mode */
#define LL_RTC_ALARM_BIN_TIME(wd, day, h, m, s, ampm) (((wd)<<LL_RTC_ALARM_WD_POS)|((day)<<LL_RTC_ALARM_DAY_POS)|((ampm)<<LL_RTC_ALARM_AMPM_POS)|\
        ((h)<<LL_RTC_ALARM_HOUR_POS)|((m)<<LL_RTC_ALARM_MIN_POS)|((s)<<LL_RTC_ALARM_SEC_POS))
/**< Bcd Mode */
#define LL_RTC_ALARM_BCD_TIME(wd, day, h, m, s, ampm) (((LL_RTC_BCD_L(ampm))<<22)|((LL_RTC_BCD_H(h))<<20)|((LL_RTC_BCD_L(h))<<16)|\
        ((LL_RTC_BCD_H(m))<<12)|((LL_RTC_BCD_L(m))<<8)|((LL_RTC_BCD_H(s))<<4)|\
        ((LL_RTC_BCD_L(s))<<0)|((LL_RTC_BCD_H(day))<<28)|((LL_RTC_BCD_L(day))<<24)|((LL_RTC_BCD_L(wd))<<30))

/*************** RTC Timestamp Bitfiled ******************/
#define LL_RTC_TIMESTAMP_SUBSECOND_POS     (16)
#define LL_RTC_TIMESTAMP_SUBSECOND_MASK    ((uint32_t)(((uint32_t)0xFFFF)<<LL_RTC_TIMESTAMP_SUBSECOND_POS))

/*************** RTC Wakeup Bitfiled ******************/
#define LL_RTC_MAX_WAKEUP_CNT          ((1<<16)-1) /**< 65535 */

/*************** RTC Timer Hold Bitfiled ******************/
#define RTC_LL_TIMER_HOLD_MASK         (1<<21)


/*************** RTC Safety Bitfiled ******************/
/* Ctrl */
#define LL_RTC_CK_CHG              (0x1<<30)
#define LL_RTC_WRDY_ERR            (0x1<<28)

#define LL_RTC_EVSEL_EN            (0x1<<26)
#define LL_RTC_WAKEUP_EN           (0x1<<25)
#define LL_RTC_ACMP_TRIEN          (0x1<<24)

#define LL_RTC_WRDY                (0x1<<22)
#define LL_RTC_HOLD                (0x1<<21)
#define LL_RTC_ACMP_SEL_MASK       (0x7<<17)
#define LL_RTC_EV_SEL_MASK         (0xF<<13)

#define LL_RTC_LOCK                (0x1<<11)

/* Specail */
#define LL_RTC_SAFE                (0x1<<18)
#define LL_RTC_EVSEL_WEF           (0x1<<17)
#define LL_RTC_ACMPTRI_WEF         (0x1<<16)
#define LL_RTC_WAKEUP_WEF          (0x1<<15)
#define LL_RTC_ALARM_WEF           (0x1<<14)
#define LL_RTC_TAMP1_WEF           (0x1<<13)
#define LL_RTC_TAMP2_WEF           (0x1<<12)

#define LL_RTC_TICK_WEF            (0x1<<10)
#define LL_RTC_PES_WEF             (0x1<<9)

#define LL_RTC_SOFT_TST2           (0x1<<3)
#define LL_RTC_SOFT_TST            (0x1<<2)

#define LL_RTC_ACMP_SEL_WAKEUP4     (0x5<<17)
#define LL_RTC_EV_SEL_WAKEUP4       (0xF<<13)
#define LL_RTC_EV_SEL_ALARM         (0x4<<13)
#define LL_RTC_EV_SEL_TAMPER1       (0x1<<13)
#define LL_RTC_EV_SEL_TAMPER2       (0x2<<13)


/*************** RTC Special IO CONTROL Bitfiled ******************/
#if defined(CHIP_GM6721)
#define LL_RTC_RTCIOCTRL_XPA7_WKEN_POS          (0)
#define LL_RTC_RTCIOCTRL_XPA7_WKEN_MASK         ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA7_WKEN_POS))
#define LL_RTC_RTCIOCTRL_XPA7_EDGE_SEL_POS      (1)
#define LL_RTC_RTCIOCTRL_XPA7_EDGE_SEL_MASK     ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA7_EDGE_SEL_POS))
#define LL_RTC_RTCIOCTRL_XPA7_PEND_CLEAR_POS    (2)
#define LL_RTC_RTCIOCTRL_XPA7_PEND_CLEAR_MASK   ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA7_PEND_CLEAR_POS))
#define LL_RTC_RTCIOCTRL_XPA8_WKEN_POS          (3)
#define LL_RTC_RTCIOCTRL_XPA8_WKEN_MASK         ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA8_WKEN_POS))
#define LL_RTC_RTCIOCTRL_XPA8_EDGE_SEL_POS      (4)
#define LL_RTC_RTCIOCTRL_XPA8_EDGE_SEL_MASK     ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA8_EDGE_SEL_POS))
#define LL_RTC_RTCIOCTRL_XPA8_PEND_CLEAR_POS    (5)
#define LL_RTC_RTCIOCTRL_XPA8_PEND_CLEAR_MASK   ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA8_PEND_CLEAR_POS))

#define LL_RTC_RTCIOCTRL_XPA7_WKPEND_POS        (8)
#define LL_RTC_RTCIOCTRL_XPA7_WKPEND_MASK       ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA7_WKPEND_POS))
#define LL_RTC_RTCIOCTRL_XPA8_WKPEND_POS        (9)
#define LL_RTC_RTCIOCTRL_XPA8_WKPEND_MASK       ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPA8_WKPEND_POS))


#define LL_RTC_XPA7_WAKEUP_EN(pDev, val)        IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPA7_WKEN_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPA7_WKEN_POS)
#define LL_RTC_XPA8_WAKEUP_EN(pDev, val)        IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPA8_WKEN_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPA8_WKEN_POS)
#define LL_RTC_XPA7_SEL_EDGE_VAILD(pDev, val)   IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPA7_EDGE_SEL_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPA7_EDGE_SEL_POS)
#define LL_RTC_XPA8_SEL_EDGE_VAILD(pDev, val)   IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPA8_EDGE_SEL_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPA8_EDGE_SEL_POS)

#define LL_RTC_XPA7_CLEAR_WAKEUP_PENDING(pDev)  IO_BIT_SET(pDev->RTCIOCTRL ,LL_RTC_RTCIOCTRL_XPA7_WKPEND_MASK)
#define LL_RTC_XPA8_CLEAR_WAKEUP_PENDING(pDev)  IO_BIT_SET(pDev->RTCIOCTRL ,LL_RTC_RTCIOCTRL_XPA8_WKPEND_MASK)
#define LL_RTC_IS_XPA7_WAKEUP_PENGING(pDev)     (!!(pDev->RTCIOCTRL & LL_RTC_RTCIOCTRL_XPA7_WKPEND_MASK))
#define LL_RTC_IS_XPA8_WAKEUP_PENGING(pDev)     (!!(pDev->RTCIOCTRL & LL_RTC_RTCIOCTRL_XPA8_WKPEND_MASK))

#elif defined(CHIP_F6721B)
#define LL_RTC_RTCIOCTRL_XPB12_WKEN_POS          (0)
#define LL_RTC_RTCIOCTRL_XPB12_WKEN_MASK         ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB12_WKEN_POS))
#define LL_RTC_RTCIOCTRL_XPB12_EDGE_SEL_POS      (1)
#define LL_RTC_RTCIOCTRL_XPB12_EDGE_SEL_MASK     ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB12_EDGE_SEL_POS))
#define LL_RTC_RTCIOCTRL_XPB12_PEND_CLEAR_POS    (2)
#define LL_RTC_RTCIOCTRL_XPB12_PEND_CLEAR_MASK   ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB12_PEND_CLEAR_POS))
#define LL_RTC_RTCIOCTRL_XPB13_WKEN_POS          (3)
#define LL_RTC_RTCIOCTRL_XPB13_WKEN_MASK         ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB13_WKEN_POS))
#define LL_RTC_RTCIOCTRL_XPB13_EDGE_SEL_POS      (4)
#define LL_RTC_RTCIOCTRL_XPB13_EDGE_SEL_MASK     ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB13_EDGE_SEL_POS))
#define LL_RTC_RTCIOCTRL_XPB13_PEND_CLEAR_POS    (5)
#define LL_RTC_RTCIOCTRL_XPB13_PEND_CLEAR_MASK   ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB13_PEND_CLEAR_POS))

#define LL_RTC_RTCIOCTRL_XPB12_WKPEND_POS        (8)
#define LL_RTC_RTCIOCTRL_XPB12_WKPEND_MASK       ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB12_WKPEND_POS))
#define LL_RTC_RTCIOCTRL_XPB13_WKPEND_POS        (9)
#define LL_RTC_RTCIOCTRL_XPB13_WKPEND_MASK       ((uint32_t)(0x01<<LL_RTC_RTCIOCTRL_XPB13_WKPEND_POS))


#define LL_RTC_XPB12_WAKEUP_EN(pDev, val)        IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPB12_WKEN_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPB12_WKEN_POS)
#define LL_RTC_XPB13_WAKEUP_EN(pDev, val)        IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPB13_WKEN_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPB13_WKEN_POS)
#define LL_RTC_XPB12_SEL_EDGE_VAILD(pDev, val)   IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPB12_EDGE_SEL_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPB12_EDGE_SEL_POS)
#define LL_RTC_XPB13_SEL_EDGE_VAILD(pDev, val)   IO_BITMASK_SET(pDev->RTCIOCTRL,LL_RTC_RTCIOCTRL_XPB13_EDGE_SEL_MASK,\
        (val)<< LL_RTC_RTCIOCTRL_XPB13_EDGE_SEL_POS)

#define LL_RTC_XPB12_CLEAR_WAKEUP_PENDING(pDev)  IO_BIT_SET(pDev->RTCIOCTRL ,LL_RTC_RTCIOCTRL_XPB12_WKPEND_MASK)
#define LL_RTC_XPB13_CLEAR_WAKEUP_PENDING(pDev)  IO_BIT_SET(pDev->RTCIOCTRL ,LL_RTC_RTCIOCTRL_XPB13_WKPEND_MASK)
#define LL_RTC_IS_XPB12_WAKEUP_PENGING(pDev)     (!!(pDev->RTCIOCTRL & LL_RTC_RTCIOCTRL_XPB12_WKPEND_MASK))
#define LL_RTC_IS_XPB13_WAKEUP_PENGING(pDev)     (!!(pDev->RTCIOCTRL & LL_RTC_RTCIOCTRL_XPB13_WKPEND_MASK))
#endif

#define SYS_PWR_MODE_REG                        (*(volatile uint32_t *)(0x40000018))
#define BKP_WR_UNLOCK()                         (SYS_PWR_MODE_REG |= (uint32_t)(1<<12))

#define LL_RTC_SUBSEC_INIT(pDev, val)           IO_BITMASK_SET(pDev->SUBSEC, LL_RTC_BIN_SUBSECOND_MASK, (val) << LL_RTC_BIN_SECOND_POS)
#define LL_RTC_CLEAR_IRQ_FLAG(pDev)             IO_BITMASK_SET(pDev->INTCLR, LL_RTC_MASK_ALL, LL_RTC_IRQ_CLEAR_ALL_PENDING_FLAG_MASK)

/**
 * @brief Open RTC register protection.
 *
 * @param               None.
 * @retval              None.
 */
void HAL_RTC_Unlock(void);

/**
 * @brief Close RTC register protection.
 *
 * @param               None.
 * @retval              None.
 */
void HAL_RTC_Lock(void);

uint32_t HAL_RTC_CanReadSafely(void);

/**
 * @brief Waiting for the finish of RTC initialization.
 *
 * @param               None.
 * @retval              None.
 */
void HAL_RTC_WaitClockReady(void);

void HAL_RTC_DisableSecondIrq(void);
void HAL_RTC_EnableSecondIrq(void);
void HAL_RTC_DisableWakeupIrq(uint8_t wakeupIndex);
void HAL_RTC_EnableWakeupIrq(uint8_t wakeupIndex);

#ifdef __cplusplus
}
#endif


#endif //__GM_LL_RTC_H_
