/**
  *
  * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
  *
  * @file          rtc_test.h
  *
  * @author        yangy
  *
  * @version       1.0.0
  *
  * @date          2017/04/06
  *
  * @brief         RTC functionality test for GM6621.
  *
  * @note
  *    2017/04/06, yangy, V1.0.0
  *        Initial version.
  *    2017/06/06, yangy, V1.1.0
  *        Add some security operations.(GM6621_20170605_1219_K7_RTC.bin)
  */

#ifndef _RTC_TEST_H_
#define _RTC_TEST_H_

#include <gm_common.h>
#include <gm_soc.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    RTC_RW_RO   = 0x1,  /**< Read only      */
    RTC_RW_WO   = 0x2,  /**< Write only     */
    RTC_RW_RW   = 0x3,  /**< Read & write   */
    RTC_RW_SKIP = 0x4   /**< Skip to test register's protect prop */
}
RTC_RW_T;

typedef enum
{
    RtcOut_Mode_DCALCTL_512Hz   = 0x1, /**< Calibration 512Hz(32.768KHz)    */
    RtcOut_Mode_DCALCTL_1Hz     = 0x2, /**< Calibration 1Hz(32.768KHz)      */
    RtcOut_Mode_ALARMOUT        = 0x3, /**< Alarm_out                       */
} RTC_RtcOutMode_T;

typedef struct
{
    __IO uint32_t   *addr;
    uint8_t         *name;
    RTC_RW_T         rwType;
    uint32_t         resetValue;
    uint32_t         writeMask;         /**< Ignore the bits WriteOnly, Reserved */
} RTC_Reg_T;


typedef struct
{
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t week;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t subsecond;
    uint32_t pm;
    uint32_t format12;
} RTC_Time_T;

/*----------------------------------------------------------
 * Register
 *-----------------------------------------------------------*/

#ifndef RTC_BASE
#define RTC_BASE                (0x40009000u)
#endif

#define RTC_REG(OFFSET)  ((__IO uint32_t *)(RTC_BASE+OFFSET))

#define RTC_TM0_NAME        "TM0"
#define RTC_TM1_NAME        "TM1"
#define RTC_CTRL_NAME       "CTRL"
#define RTC_ALARM_NAME      "ALARM"
#define RTC_ALRMASK_NAME    "ALRMASK"
#define RTC_TICKVAL_NAME    "TICKVAL"
#define RTC_SUBSEC_NAME     "SUBSEC"
#define RTC_TIMSTAMP0_NAME  "TIMSTAMP0"
#define RTC_TIMSTAMP1_NAME  "TIMSTAMP1"
#define RTC_DCALCTL_NAME    "DCALCTL"
#define RTC_FLAG_NAME       "FLAG"
#define RTC_INTEN_NAME      "INTEN"
#define RTC_INTCLR_NAME     "INTCLR"
#define RTC_TAMPER1_NAME    "TAMPER1"
#define RTC_TAMPER2_NAME    "TAMPER2"
#define RTC_TAMPER3_NAME    "TAMPER3"
#define RTC_PROTECT_NAME    "PROTECT"
#define RTC_ALASSEC_NAME    "ALASSEC"
#define RTC_WAKEUPT0_NAME   "WAKEUPT0"
#define RTC_WAKEUPT1_NAME   "WAKEUPT1"
#define RTC_PES_NAME        "PES"
#define RTC_SPECIAL_NAME    "SPECIAL"
#define RTC_VER_NAME        "VER"

#define RTC_TM0         (RTC_REG(0x0))      /**<  RTC Timer Register 0,             offset: 0x0 */
#define RTC_TM1         (RTC_REG(0x4))      /**<  RTC Timer Register 1,             offset: 0x4 */
#define RTC_CTRL        (RTC_REG(0x8))      /**<  RTC Control Register,             offset: 0x8 */
#define RTC_ALRM        (RTC_REG(0xC))      /**<  RTC Alarm Register,               offset: 0xC */
#define RTC_ALRMASK     (RTC_REG(0x10))     /**<  RTC Alarm Mask Register,          offset: 0x10 */
#define RTC_TICKVAL     (RTC_REG(0x14))     /**<  RTC Tick Value Register,          offset: 0x14 */
#define RTC_SUBSEC      (RTC_REG(0x18))     /**<  RTC Sub Second Register,          offset: 0x18 */
#define RTC_TIMSTAMP0   (RTC_REG(0x1C))     /**<  RTC Time Stamp Register 0,        offset: 0x1C */
#define RTC_TIMSTAMP1   (RTC_REG(0x20))     /**<  RTC Time Stamp Register 1,        offset: 0x20 */
#define RTC_DCALCTL     (RTC_REG(0x24))     /**<  RTC Calibration Register,         offset: 0x24 */
#define RTC_FLAG        (RTC_REG(0x28))     /**<  RTC Flag Register,                offset: 0x28 */
#define RTC_INTEN       (RTC_REG(0x2C))     /**<  RTC Interrupt Mask Register,      offset: 0x2C */
#define RTC_INTCLR      (RTC_REG(0x30))     /**<  RTC Interrupt Clear Register,     offset: 0x30 */
#define RTC_TAMPER1     (RTC_REG(0x34))     /**<  RTC Tamper1 Control Register,     offset: 0x34 */
#define RTC_TAMPER2     (RTC_REG(0x38))     /**<  RTC Tamper2 Control Register,     offset: 0x38 */
#define RTC_TAMPER3     (RTC_REG(0x3C))     /**<  RTC Tamper3 Control Register,     offset: 0x3C */
#define RTC_PROTECT     (RTC_REG(0x40))     /**<  RTC Protect Register,             offset: 0x40 */
#define RTC_ALASSEC     (RTC_REG(0x44))     /**<  RTC Alarm of Subsecond Register,  offset: 0x44 */
#define RTC_WAKEUPT0    (RTC_REG(0x48))     /**<  RTC Wakeup Timer Register0,       offset: 0x48 */
#define RTC_WAKEUPT1    (RTC_REG(0x4C))     /**<  RTC Wakeup Timer Register1,       offset: 0x4C */
#define RTC_PES         (RTC_REG(0x50))     /**<  RTC Event System Control Register,offset: 0x50 */
#define RTC_SPECIAL     (RTC_REG(0x54))     /**<  RTC Special Register,             offset: 0x54 */
#define RTC_VER         (RTC_REG(0xFC))     /**<  RTC Version Register,             offset: 0xFC */

/*----------------------------------------------------------
 * IO BIT Operations
 *-----------------------------------------------------------*/
#define ADDR_BIT_GET(Reg, Bit)                      IO_BIT_GET((*Reg), Bit)
#define ADDR_BIT_SET(Reg, Bit)                      IO_BIT_SET((*Reg), Bit)
#define ADDR_BIT_CLEAR(Reg, Bit)                    IO_BIT_CLEAR((*Reg), Bit)
#define ADDR_BITS_GET(Reg, Bits, Pos)               (IO_BITS_GET((*Reg), Bits) >> Pos)
#define ADDR_BITS_SET(Reg, Bits)                    IO_BITS_SET((*Reg), Bits)
#define ADDR_BITMASK_SET(Reg, Bitmask, Value)       IO_BITMASK_SET((*Reg), Bitmask, Value)

/*----------------------------------------------------------
 * Error
 *-----------------------------------------------------------*/
#define ERR_WRITE   (1<<1)
#define ERR_READ    (1<<2)
#define ERR_UNLOCK  (1<<3)
#define ERR_LOCK    (1<<4)
#define ERR_DATE    (1<<5)
#define ERR_ADDSUB  (1<<6)
#define ERR_HOLD    (1<<7)

/*----------------------------------------------------------
 * Protect
 *-----------------------------------------------------------*/
#define PROTECT_UNLOCK  ((uint32_t)0x1ACCE551)
#define PROTECT_LOCK    ((uint32_t)0x0)

#define ADDRESS_WRITE_UINT32(Address, Value)    ((*Address) = ((uint32_t)(Value)))
#define ADDRESS_READ_UINT32(Address)            ((uint32_t)(*Address))

#define WRITE_UNLOCK()  ADDRESS_WRITE_UINT32(RTC_PROTECT, PROTECT_UNLOCK)
#define WRITE_LOCK()    ADDRESS_WRITE_UINT32(RTC_PROTECT, PROTECT_LOCK)


/*----------------------------------------------------------
 * Bin Bcd Common
 *-----------------------------------------------------------*/
#define BIN_BCD_MODE        ((uint32_t)0x00000080)
#define BCD2BIN(x)          (((x) & 0x0f) + ((x) >> 4) * 10)
#define BIN2BCD(x)          ((((x) / 10) << 4) + (x) % 10)

/*----------------------------------------------------------
 * Bin Mode
 *-----------------------------------------------------------*/
#define BIN_YEAR_POS        (16)
#define BIN_YEAR_MASK       ((uint32_t)(0x3FFF<<BIN_YEAR_POS))

#define BIN_WEEK_POS        (13)
#define BIN_WEEK_MASK       ((uint32_t)(0x7<<BIN_WEEK_POS))

#define BIN_MONTH_POS       (8)
#define BIN_MONTH_MASK      ((uint32_t)(0x1F<<BIN_MONTH_POS))

#define BIN_DAY_POS         (0)
#define BIN_DAY_MASK        ((uint32_t)(0x3F<<BIN_DAY_POS))

/**< 1: PM */
#define BIN_AMPM_POS        (22)
#define BIN_AMPM_MASK       ((uint32_t)(0x1<<BIN_AMPM_POS))

#define BIN_HOUR_POS        (16)
#define BIN_HOUR_MASK       ((uint32_t)(0x3F<<BIN_HOUR_POS))

#define BIN_MINUTE_POS      (8)
#define BIN_MINUTE_MASK     ((uint32_t)(0x7F<<BIN_MINUTE_POS))

#define BIN_SECOND_POS      (0)
#define BIN_SECOND_MASK     ((uint32_t)(0x7F<<BIN_SECOND_POS))

#define BIN_SUBSECOND_POS   (0)
#define BIN_SUBSECOND_MASK  ((uint32_t)(0xFFFF<<BIN_SUBSECOND_POS))

#define BIN_FORMAT12_POS    (0)
#define BIN_FORMAT12_MASK   ((uint32_t)(0x1<<BIN_FORMAT12_POS))

#define BIN_TIME(h, m, s, ampm) (((ampm)<<BIN_AMPM_POS)|((h)<<BIN_HOUR_POS)|((m)<<BIN_MINUTE_POS)|((s)<<BIN_SECOND_POS))
#define BIN_YEAR(y, m, d, w)    (((y)<<BIN_YEAR_POS)|((w)<<BIN_WEEK_POS)|((m)<<BIN_MONTH_POS)|((d)<<BIN_DAY_POS))

/*----------------------------------------------------------
 * Bcd Mode
 *-----------------------------------------------------------*/
#define BCD_H(x)                    (((x)&0xF0)>>4)
#define BCD_L(x)                    ((x)&0x0F)

#define BCD_YEAR(year_h, year_l)    (((BCD_H(year_h))<<12)|((BCD_L(year_h))<<8)|((BCD_H(year_l))<<4)|((BCD_L(year_l))<<0))
#define BCD_MONTH(month)            (((BCD_H(month))<<4)|((BCD_L(month))<<0))
#define BCD_DAY(day)                (((BCD_H(day))<<4)|((BCD_L(day))<<0))
#define BCD_WEEK(week)              ((BCD_L(week))<<0)
#define BCD_TIME(h,m,s,ampm)        (((BCD_L(ampm))<<22)|((BCD_H(h))<<20)|((BCD_L(h))<<16)|((BCD_H(m))<<12)|((BCD_L(m))<<8)|\
                                     ((BCD_H(s))<<4)|((BCD_L(s))<<0))
#define BCD_DATE(y,m,d,w)           ((y<<BIN_YEAR_POS)|(m<<BIN_MONTH_POS)|(d<<BIN_DAY_POS)|(w<<BIN_WEEK_POS))


#define BCD_YEAR_HIGH_GET_VALUE(v)  ((((v)&(0x3<<28))>>24)|(((v)&(0xF<<24))>>24))
#define BCD_YEAR_LOW_GET_VALUE(v)   ((((v)&(0xF<<20))>>16)|(((v)&(0xF<<16))>>16))
#define BCD_MONTH_GET_VALUE(v)      ((((v)&(0x1<<12))>>8)|(((v)&(0xF<<8))>>8))
#define BCD_DAY_GET_VALUE(v)        (((v)&(0x3<<4))|((v)&(0xF)))
#define BCD_WEEK_GET_VALUE(v)       (((v)&(0x7<<13))>>13)

#define BCD_YEAR_HIGH_GET()         BCD_YEAR_HIGH_GET_VALUE(*RTC_TM1)
#define BCD_YEAR_LOW_GET()          BCD_YEAR_LOW_GET_VALUE(*RTC_TM1)
#define BCD_MONTH_GET()             BCD_MONTH_GET_VALUE(*RTC_TM1)
#define BCD_DAY_GET()               BCD_DAY_GET_VALUE(*RTC_TM1)
#define BCD_WEEK_GET()              BCD_WEEK_GET_VALUE(*RTC_TM1)

#define BCD_HOUR_GET_VALUE(v)       ((((v)&(0x3<<20))>>16)|(((v)&(0xF<<16))>>16))
#define BCD_MINUTE_GET_VALUE(v)     ((((v)&(0x7<<12))>>8)|(((v)&(0xF<<8))>>8))
#define BCD_SECOND_GET_VALUE(v)     ((((v)&(0x7<<4)))|(((v)&(0xF))))
#define BCD_AMPM_GET_VALUE(v)       (((v)&(0x1<<22))>22)

#define BCD_HOUR_GET()              BCD_HOUR_GET_VALUE(*RTC_TM0)
#define BCD_MINUTE_GET()            BCD_MINUTE_GET_VALUE(*RTC_TM0)
#define BCD_SECOND_GET()            BCD_SECOND_GET_VALUE(*RTC_TM0)
#define BCD_AMPM_GET()              BCD_AMPM_GET_VALUE(*RTC_TM0)

/*----------------------------------------------------------
 * Tick Cycles
 *-----------------------------------------------------------*/
/* Unit: HZ */

/* Modify those MACROS in the file rtc_dev.h */
/* #define RTC_CLK 1000000 */

/*
 * RTC_CLK 8Mhz
 * prescale = (8000/62.500)-1=127
 * tickvalue= (62500/2)- 1 = 31249
 * #define PRESCALE         (121)
 * #define TICKS_PER_1S     (RTC_CLK / 2 / (PRESCALE + 1) - 1)  [32785]
 */

/*
 * RTC_CLK 1Mhz
 * prescale = (1000/62.500)-1=15
 * tickvalue= (62500/2)- 1 = 31249
 * #define PRESCALE         (15)
 * #define TICKS_PER_1S     (RTC_CLK / 2 / (PRESCALE + 1) - 1)  [31249]
 */

#define PRESCALE_POS        (16)
#define PRESCALE_MASK       ((uint32_t)(0x7F<<PRESCALE_POS))

#define TICKVAULE_POS       (0)
#define TICKVALUE_MASK      ((uint32_t)(0xFFFF<<TICKVAULE_POS))

/*----------------------------------------------------------
* Interrupt
*-----------------------------------------------------------*/
#define IRQ_PES_EN              (1<<17)
#define IRQ_YEAR_EN             (1<<16)
#define IRQ_MONTH_EN            (1<<15)
#define IRQ_DAY_EN              (1<<14)
#define IRQ_HOUR_EN             (1<<13)
#define IRQ_MINUTE_EN           (1<<12)
#define IRQ_SECOND_EN           (1<<11)
#define IRQ_WAKEUP_WP4F_EN      (1<<10) /**< 1000ms@RTC_CLK             */
#define IRQ_WAKEUP_WP3F_EN      (1<<9)  /**< 62.5ms@RTC_CLK             */
#define IRQ_WAKEUP_WP2F_EN      (1<<8)  /**< 125ms@RTC_CLK              */
#define IRQ_WAKEUP_WP1F_EN      (1<<7)  /**< 250ms@RTC_CLK              */
#define IRQ_WAKEUP_WP0F_EN      (1<<6)  /**< 500ms@RTC_CLK              */
#define IRQ_TIMSTAMPOV_IE       (1<<5)  /**< time-stamp overflow event  */
#define IRQ_TIMSTAMP_IE         (1<<4)
#define IRQ_TAMPER3_IE          (1<<3)
#define IRQ_TAMPER2_IE          (1<<2)
#define IRQ_TAMPER1_IE          (1<<1)
#define IRQ_ALARM_IE            (1<<0)

/*----------------------------------------------------------
* Tamper
*-----------------------------------------------------------*/
#define TAMPPUDIS_POS               (10) /**< 0:precharge, enable internal pull-up              */
#define TAMPPRCH_POS                (8)  /**< pull-up duration: 2^x RTCLK cycles (0~3)          */
#define TAMPFLT_POS                 (6)  /**< filter count: 2^x consecutive samples (0~3)       */
#define TAMPFRE_POS                 (3)  /**< tamper input sample frequency: RTCLK/(2^x)  (0~7) */
#define TAMPTS                      (2)  /**< 1:save timestamp                                  */
#define TAMPTRG                     (1)  /**< trigger 1: high level                             */
#define TAMPE                       (0)  /**< 1:enable                                          */

/**< Tamper1 input is XPD5(IO 109), the default level is high. */
/**< Tamper2 input is XPA4(IO  92), the default level is high. */
#define TAMPER_TRIGGER_LEVEL        (1) /**< Low level to trigger */

/*----------------------------------------------------------
 * Alarm
 *-----------------------------------------------------------*/
#define ALARM_EN                    (1<<1)

#define ALARM_SUBSEC_ALLBIT_CHECK   (15)

#define ALARM_SUBSEC_AE_POS     (0x4)   /**< SUBSEC[14:x] are don't care, only SUBCSEC[x-1:0]are compared */
#define ALARM_DAY_AE_POS        (0x3)
#define ALARM_HOUR_AE_POS       (0x2)
#define ALARM_MIN_AE_POS        (0x1)
#define ALARM_SEC_AE_POS        (0x0)

#define ALARM_SUBSEC_AE_MASK    (0xF  << ALARM_SUBSEC_AE_POS)

#define ALARM_WD_TEST           (0x1)
#define ALARM_WD_POS            (30)    /**< 0: the DAY is the day of month, 1: the DAY[3:0] is the week day */
#define ALARM_WD_MASK           (ALARM_WD_TEST  << ALARM_WD_POS)
#define ALARM_DAY_TEST          (0x3F)
#define ALARM_DAY_POS           (24)
#define ALARM_DAY_MASK          (ALARM_DAY_TEST << ALARM_DAY_AE_POS)
#define ALARM_AMPM_POS          (22)
#define ALARM_AMPM_MASK         (0x1  << ALARM_AMPM_POS)
#define ALARM_HOUR_TEST         (0x3F)
#define ALARM_HOUR_POS          (16)
#define ALARM_HOUR_MASK         (ALARM_HOUR_TEST << ALARM_HOUR_POS)
#define ALARM_MIN_POS           (8)
#define ALARM_MIN_TEST          (0x3F)
#define ALARM_MIN_MASK          (ALARM_MIN_TEST << ALARM_MIN_POS)
#define ALARM_SEC_POS           (0)
#define ALARM_SEC_TEST          (0x3F)
#define ALARM_SEC_MASK          (ALARM_SEC_TEST << ALARM_SEC_POS)

/**< Bin Mode */
#define ALARM_BIN_TIME(wd, day, h, m, s, ampm) (((wd)<<ALARM_WD_POS)|((day)<<ALARM_DAY_POS)|((ampm)<<ALARM_AMPM_POS)|\
        ((h)<<ALARM_HOUR_POS)|((m)<<ALARM_MIN_POS)|((s)<<ALARM_SEC_POS))
/**< Bcd Mode */
#define ALARM_BCD_TIME(wd, day, h, m, s, ampm) (((BCD_L(ampm))<<22)|((BCD_H(h))<<20)|((BCD_L(h))<<16)|\
        ((BCD_H(m))<<12)|((BCD_L(m))<<8)|((BCD_H(s))<<4)|\
        ((BCD_L(s))<<0)|((BCD_H(day))<<28)|((BCD_L(day))<<24)|((BCD_L(wd))<<30))

#define TDIVSEL 0
#define WP0_FREQ                (RTC_CLK/2/(PRESCALE+1)/(1<<(14-TDIVSEL)))
#define WP1_FREQ                (RTC_CLK/2/(PRESCALE+1)/(1<<(13-TDIVSEL)))
#define WP2_FREQ                (RTC_CLK/2/(PRESCALE+1)/(1<<(12-TDIVSEL)))
#define WP3_FREQ                (RTC_CLK/2/(PRESCALE+1)/(1<<(11-TDIVSEL)))
#define WP4_FREQ                (RTC_CLK/2/(PRESCALE+1)/((MAX_WAKEUP_CNT)+1))

/**< set RTC_OUT Output  */
/**< GM6621 FPGA - XPB5 - IO3 */
/**< GM6621 ES   - XPB8       */
#define RTCOUT1                 (*(__IO uint32_t *)(0x4000015C))
#define RTCOUT2                 (*(__IO uint32_t *)(0x40000144))
#define TAMPER_XP_MOD_SEL       (*(__IO uint32_t *)(0x400002C4))
#define TAMPER_XP_AF_SEL        (*(__IO uint32_t *)(0x400002F0))
#define RTCOUT_COE_EN           (0x1<<6)
#define RTCOUT_COSEL_EN         (0x1<<5)

/*----------------------------------------------------------
 * Timer Hold
 *-----------------------------------------------------------*/
#define TIMER_HOLD_MASK         (1<<21)

/*----------------------------------------------------------
 * Leap Year
 *-----------------------------------------------------------*/
#define LEAP_FLAG               (0x1<<19)

/*----------------------------------------------------------
 * Add/Substract one hour
 *-----------------------------------------------------------*/
#define ADD_1HOUR               (0x1<<0)
#define SUB_1HOUR               (0x1<<1)

/*----------------------------------------------------------
 * BaseTimer
 *-----------------------------------------------------------*/
/**< #define BTM_INT        (ETM5_IRQn) */

#define BTM_BASE            (0x40036000u)

#define BTM_CR              (HW32_REG(BTM_BASE+0x0))
#define BTM_LR              (HW32_REG(BTM_BASE+0x4))
#define BTM_CNT             (HW32_REG(BTM_BASE+0x8))
#define BTM_RST_REG         (HW32_REG(0x40000020))

#define BTM_RST             (1<<21)

#define BTM_IE              (1<<8)
#define BTM_EN              (1<<0)

#define BTM_INI_CLEAR       (1<<9)

#if !defined (PES_BASE)
#define PES_BASE            (0x40037000u)
#endif
#define PES_REG(OFFSET)     ((__IO uint32_t *)(PES_BASE+OFFSET))
#define PES_CR              (HW32_REG(PES_BASE+0x20))


/*----------------------------------------------------------
 * Safety
 *-----------------------------------------------------------*/
/* Ctrl */
#define CK_CHG              (0x1<<30)
#define WRDY_ERR            (0x1<<28)

#define EVSEL_EN            (0x1<<26)
#define WAKEUP_EN           (0x1<<25)
#define ACMP_TRIEN          (0x1<<24)

#define WRDY                (0x1<<22)
#define HOLD                (0x1<<21)
#define ACMP_SEL_MASK       (0x7<<17)
#define EV_SEL_MASK         (0xF<<13)


#define LOCK                (0x1<<11)

/* Specail */
#define SAFE                (0x1<<18)
#define EVSEL_WEF           (0x1<<17)
#define ACMPTRI_WEF         (0x1<<16)
#define WAKEUP_WEF          (0x1<<15)
#define ALARM_WEF           (0x1<<14)
#define TAMP1_WEF           (0x1<<13)
#define TAMP2_WEF           (0x1<<12)

#define TICK_WEF            (0x1<<10)
#define PES_WEF             (0x1<<9)

#define SOFT_TST2           (0x1<<3)
#define SOFT_TST            (0x1<<2)

#define ACMP_SEL_WAKEUP4    (0x5<<17)
#define EV_SEL_WAKEUP4      (0xF<<13)
#define EV_SEL_ALARM        (0x4<<13)
#define EV_SEL_TAMPER1      (0x1<<13)
#define EV_SEL_TAMPER2      (0x2<<13)


#define MASK_ALL                (0xFFFFFFFF)
#define IRQ_CLEAR_ALL_PENDING_FLAG_MASK     ((uint32_t)1<<31)

#ifdef __cplusplus
}
#endif

#endif /*_RTC_TEST_H_*/
