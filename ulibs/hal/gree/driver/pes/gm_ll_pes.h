/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          pes_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2017/05/19
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2017/05/19, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#ifndef _GM_LL_PES_H_
#define _GM_LL_PES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include<gm_soc.h>

#define PES_ERR_PRINTF      1
#define PES_WARNING_PRINTF  1

#ifndef PES_DEBUG_PRINTF
    #define PES_PR_DEBUG(...)      do {} while (0)
#else
    #define PES_PR_DEBUG           printf
#endif

#ifndef PES_ERR_PRINTF
    #define PES_PR_ERR(...)        do {} while (0)
#else
    #define PES_PR_ERR             printf
#endif


#ifndef PES_WARNING_PRINTF
    #define PES_PR_WARNING(...)    do {} while (0)
#else
    #define PES_PR_WARNING         printf
#endif



/* PES Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t PESID;                      /* PES id register:offset: 0x0 */
    __IO uint32_t SWEVT;                      /* PES software event register:offset: 0x4 */
    __IO uint32_t EVTOCPU;                    /* PES event to CPU register:offset: 0x8 */
    __IO uint32_t INTSTS;                     /* PES event to CPU register:offset: 0x0c*/
    __IO uint32_t RESV0[4];                   /* PES resever register:offset: 0x10~0x1c */
    __IO uint32_t CHNCFG[12];                 /* PES channel configuration register:offset: 0x20~0x4c */
}PES_Device_T;

#define PES_DEV                               ((PES_Device_T *)PES_BASE) /* PES device pointer */

#define PES_UNLOCKED                          0x0 /* PES lockstus: unlock */
#define PES_LOCKED                            0x1 /* PES lockstus: lock */


/*************** PES Software Event Register Bitfiled ******************/
#define PES_SWEVT_SLCHN11_POS                 ((uint32_t) 27) /* PES software level bit of Channel1 pos */
#define PES_SWEVT_SLCHN11                     ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN11_POS))/* PES software level bit of Channel1 filed */
#define PES_SWEVT_SLCHN10_POS                 ((uint32_t) 26) /* PES software level bit of Channel0 pos */
#define PES_SWEVT_SLCHN10                     ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN10_POS)) /* PES software level bit of Channel0 filed */
#define PES_SWEVT_SLCHN9_POS                  ((uint32_t) 25) /* PES software level bit of Channe9 pos */
#define PES_SWEVT_SLCHN9                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN9_POS)) /* PES software level bit of Channe9 filed */
#define PES_SWEVT_SLCHN8_POS                  ((uint32_t) 24) /* PES software level bit of Channe8 pos */
#define PES_SWEVT_SLCHN8                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN8_POS)) /* PES software level bit of Channe8 filed */
#define PES_SWEVT_SLCHN7_POS                  ((uint32_t) 23) /* PES software level bit of Channe7 pos */
#define PES_SWEVT_SLCHN7                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN7_POS)) /* PES software level bit of Channe7 filed */
#define PES_SWEVT_SLCHN6_POS                  ((uint32_t) 22) /* PES software level bit of Channe6 pos */
#define PES_SWEVT_SLCHN6                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN6_POS)) /* PES software level bit of Channe6 pos */
#define PES_SWEVT_SLCHN5_POS                  ((uint32_t) 21) /* PES software level bit of Channe5 pos */
#define PES_SWEVT_SLCHN5                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN5_POS)) /* PES software level bit of Channe5 filed */
#define PES_SWEVT_SLCHN4_POS                  ((uint32_t) 20) /* PES software level bit of Channe4 pos */
#define PES_SWEVT_SLCHN4                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN4_POS)) /* PES software level bit of Channe4 filed */
#define PES_SWEVT_SLCHN3_POS                  ((uint32_t) 19) /* PES software level bit of Channe3 pos */
#define PES_SWEVT_SLCHN3                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN3_POS)) /* PES software level bit of Channe3 filed */
#define PES_SWEVT_SLCHN2_POS                  ((uint32_t) 18) /* PES software level bit of Channe2 pos */
#define PES_SWEVT_SLCHN2                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN2_POS)) /* PES software level bit of Channe2 filed */
#define PES_SWEVT_SLCHN1_POS                  ((uint32_t) 17) /* PES software level bit of Channe1 pos */
#define PES_SWEVT_SLCHN1                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN1_POS)) /* PES software level bit of Channe1 filed */
#define PES_SWEVT_SLCHN0_POS                  ((uint32_t) 16) /* PES software level bit of Channe0 pos */
#define PES_SWEVT_SLCHN0                      ((uint32_t) ((uint32_t)1 << PES_SWEVT_SLCHN0_POS)) /* PES software level bit of Channe0 filed */
#define PES_SWEVT_SPLSECHN11_POS              ((uint32_t) 11) /* PES software pulse bit of Channel1 pos */
#define PES_SWEVT_SPLSEHN11                   ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN11_POS)) /* PES software pulse bit of Channel1 filed */
#define PES_SWEVT_SPLSECHN10_POS              ((uint32_t) 10) /* PES software pulse bit of Channel0 pos */
#define PES_SWEVT_SPLSEHN10                   ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN10_POS)) /* PES software pulse bit of Channel0 filed */
#define PES_SWEVT_SPLSECHN9_POS               ((uint32_t) 9) /* PES software pulse bit of Channel9 pos */
#define PES_SWEVT_SPLSEHN9                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN9_POS))  /* PES software pulse bit of Channel9 filed */
#define PES_SWEVT_SPLSECHN8_POS               ((uint32_t) 8) /* PES software pulse bit of Channel8 pos */
#define PES_SWEVT_SPLSEHN8                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN8_POS)) /* PES software pulse bit of Channel8 filed */
#define PES_SWEVT_SPLSECHN7_POS               ((uint32_t) 7) /* PES software pulse bit of Channel7 pos */
#define PES_SWEVT_SPLSEHN7                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN7_POS)) /* PES software pulse bit of Channel7 filed */
#define PES_SWEVT_SPLSECHN6_POS               ((uint32_t) 6) /* PES software pulse bit of Channel6 pos */
#define PES_SWEVT_SPLSEHN6                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN6_POS)) /* PES software pulse bit of Channel6 filed */
#define PES_SWEVT_SPLSECHN5_POS               ((uint32_t) 5) /* PES software pulse bit of Channel5 pos */
#define PES_SWEVT_SPLSEHN5                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN5_POS)) /* PES software pulse bit of Channel5 filed */
#define PES_SWEVT_SPLSECHN4_POS               ((uint32_t) 4) /* PES software pulse bit of Channel4 pos */
#define PES_SWEVT_SPLSEHN4                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN4_POS)) /* PES software pulse bit of Channel4 filed */
#define PES_SWEVT_SPLSECHN3_POS               ((uint32_t) 3) /* PES software pulse bit of Channel3 pos */
#define PES_SWEVT_SPLSEHN3                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN3_POS)) /* PES software pulse bit of Channel3 filed */
#define PES_SWEVT_SPLSECHN2_POS               ((uint32_t) 2) /* PES software pulse bit of Channel2 pos */
#define PES_SWEVT_SPLSEHN2                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN2_POS)) /* PES software pulse bit of Channel2 filed */
#define PES_SWEVT_SPLSECHN1_POS               ((uint32_t) 1) /* PES software pulse bit of Channel1 pos */
#define PES_SWEVT_SPLSEHN1                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN1_POS)) /* PES software pulse bit of Channel1 filed */
#define PES_SWEVT_SPLSECHN0_POS               ((uint32_t) 0) /* PES software pulse bit of Channel0 pos */
#define PES_SWEVT_SPLSEHN0                    ((uint32_t) ((uint32_t)1 << PES_SWEVT_SPLSECHN0_POS)) /* PES software pulse bit of Channel0 filed */

/*************** PES Event To CPU Register Bitfiled ******************/
#define PES_EVTOCPU_CPUEVSEL_POS              ((uint32_t) 4) /* PES event to cpu channel select pos */
#define PES_EVTOCPU_CPUEVSEL                  ((uint32_t) ((uint32_t)0x0f << PES_EVTOCPU_CPUEVSEL_POS)) /* PES event to cpu channel select filed */
#define PES_EVTOCPU_CPUEVEN_POS               ((uint32_t) 0) /* PES event to cpu enable/disable pos */
#define PES_EVTOCPU_CPUEVEN                   ((uint32_t) ((uint32_t)1 << PES_EVTOCPU_CPUEVEN_POS)) /* PES event to cpu enable/disable filed */

/*************** PES Interrupt Enable and Status Register Bitfiled ******************/
#define PES_INTSTS_CHBDETSTS_POS              ((uint32_t) 15) /* PES CHB DET STATUS select pos */
#define PES_INTSTS_CHBDETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CHBDETSTS_POS)) /* PES CHB DET STATUS select filed */
#define PES_INTSTS_CHADETSTS_POS              ((uint32_t) 14) /* PES CHA DET STATUS select pos */
#define PES_INTSTS_CHADETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CHADETSTS_POS)) /* PES CHA DET STATUS select filed */
#define PES_INTSTS_CH9DETSTS_POS              ((uint32_t) 13) /* PES CH9 DET STATUS select pos */
#define PES_INTSTS_CH9DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH9DETSTS_POS)) /* PES CH9 DET STATUS select filed */
#define PES_INTSTS_CH8DETSTS_POS              ((uint32_t) 12) /* PES CH8 DET STATUS select pos */
#define PES_INTSTS_CH8DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH8DETSTS_POS)) /* PES CH8 DET STATUS select filed */
#define PES_INTSTS_CH7DETSTS_POS              ((uint32_t) 11) /* PES CH7 DET STATUS select pos */
#define PES_INTSTS_CH7DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH7DETSTS_POS)) /* PES CH7 DET STATUS select filed */
#define PES_INTSTS_CH6DETSTS_POS              ((uint32_t) 10) /* PES CH6 DET STATUS select pos */
#define PES_INTSTS_CH6DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH6DETSTS_POS)) /* PES CH6 DET STATUS select filed */
#define PES_INTSTS_CH5DETSTS_POS              ((uint32_t) 9) /* PES CH5 DET STATUS select pos */
#define PES_INTSTS_CH5DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH5DETSTS_POS)) /* PES CH5 DET STATUS select filed */
#define PES_INTSTS_CH4DETSTS_POS              ((uint32_t) 8) /* PES CH4 DET STATUS select pos */
#define PES_INTSTS_CH4DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH4DETSTS_POS)) /* PES CH4 DET STATUS select filed */
#define PES_INTSTS_CH3DETSTS_POS              ((uint32_t) 7) /* PES CH3 DET STATUS select pos */
#define PES_INTSTS_CH3DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH3DETSTS_POS)) /* PES CH3 DET STATUS select filed */
#define PES_INTSTS_CH2DETSTS_POS              ((uint32_t) 6) /* PES CH2 DET STATUS select pos */
#define PES_INTSTS_CH2DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH2DETSTS_POS)) /* PES CH2 DET STATUS select filed */
#define PES_INTSTS_CH1DETSTS_POS              ((uint32_t) 5) /* PES CH1 DET STATUS select pos */
#define PES_INTSTS_CH1DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH1DETSTS_POS)) /* PES CH1 DET STATUS select filed */
#define PES_INTSTS_CH0DETSTS_POS              ((uint32_t) 4) /* PES CH0 DET STATUS select pos */
#define PES_INTSTS_CH0DETSTS                  ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_CH0DETSTS_POS)) /* PES CH0 DET STATUS select filed */
#define PES_INTSTS_INTEN_POS              	  ((uint32_t) 0) /* PES CH0 DET STATUS select pos */
#define PES_INTSTS_INTEN                      ((uint32_t) ((uint32_t)0x01 << PES_INTSTS_INTEN_POS)) /* PES CH0 DET STATUS select filed */

/*************** PES Channel Configuration Register Bitfiled ******************/
#define PES_CHNCFG_ASNYEN_POS                 ((uint32_t) 31) /* PES channel transfer mode select pos */
#define PES_CHNCFG_ASNYEN                     ((uint32_t) ((uint32_t)1 << PES_CHNCFG_ASNYEN_POS)) /* PES channel transfer mode select filed */
#define PES_CHNCFG_EVMD_POS                   ((uint32_t) 24) /* PES event output mode select bits pos */
#define PES_CHNCFG_EVMD                       ((uint32_t) ((uint32_t)0x03 << PES_CHNCFG_EVMD_POS)) /* PES event output mode select bits filed */
#define PES_CHNCFG_IOEN_POS                   ((uint32_t) 20) /* PES channel0 receives event dirciton for IO pos */
#define PES_CHNCFG_IOEN                       ((uint32_t) ((uint32_t)1 << PES_CHNCFG_IOEN_POS)) /* PES channel0 receives event dirciton for IO filed */
#define PES_CHNCFG_ADNENTEN_POS               ((uint32_t) 18) /* PES event and with next channel event enable/disable pos */
#define PES_CHNCFG_ADNENTEN                   ((uint32_t) ((uint32_t)1 << PES_CHNCFG_ADNENTEN_POS)) /* PES event and with next channel event enable/disable filed */
#define PES_CHNCFG_ORPREEN_POS                ((uint32_t) 17) /* PES event or with precision channel event enable/disable pos */
#define PES_CHNCFG_ORPREEN                    ((uint32_t) ((uint32_t)1 << PES_CHNCFG_ORPREEN_POS)) /* PES event or with precision channel event enable/disable filed */
#define PES_CHNCFG_INVEN_POS                  ((uint32_t) 16) /* PES channel output inverted enable/disable pos */
#define PES_CHNCFG_INVEN                      ((uint32_t) ((uint32_t)1 << PES_CHNCFG_INVEN_POS)) /* PES channel output inverted enable/disable filed */
#define PES_CHNCFG_DETEDGESEL_POS             ((uint32_t) 15) /* PES DET EDGE SELECTION  pos */
#define PES_CHNCFG_DETEDGESEL_RISING          ((uint32_t) ((uint32_t)1 << PES_CHNCFG_DETEDGESEL_POS)) /* PES DET EDGE SELECTION  Rising edge*/
#define PES_CHNCFG_DETEDGESEL_FALLING         ((uint32_t) ((uint32_t)0 << PES_CHNCFG_DETEDGESEL_POS)) /* PES DET EDGE SELECTION  Falling edge*/
#define PES_CHNCFG_DETEN_POS             	  ((uint32_t) 12) /* PES DET ENABLE  pos */
#define PES_CHNCFG_DETEN          			  ((uint32_t) ((uint32_t)1 << PES_CHNCFG_DETEN_POS)) /* PES DET ENABLE*/
#define PES_CHNCFG_SRCSEL_POS                 ((uint32_t) 8) /* PES ip event source select pos */
#define PES_CHNCFG_SRCSEL                     ((uint32_t) ((uint32_t)0x0f << PES_CHNCFG_SRCSEL_POS)) /* PES ip event source select filed */
#define PES_CHNCFG_MSTSEL_POS                 ((uint32_t) 0) /* PES master source ip select pos */
#define PES_CHNCFG_MSTSEL                     ((uint32_t) ((uint32_t)0x0f << PES_CHNCFG_MSTSEL_POS)) /* PES master source ip select filed */

/**
 * @brief Config PES software generate event level
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 * @level    Software event level
 *
 * @retval   None
 */
#define LL_PES_CONFIG_SW_LEVEL(pPES, channel, level)   IO_BITMASK_SET(pPES->SWEVT, 1 << (PES_SWEVT_SLCHN0_POS + channel),\
                                                                   level << (PES_SWEVT_SLCHN0_POS + channel))
/**
 * @brief PES software generate event plus
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_SW_GENERATE_PLUSE(pPES, channel)        IO_BITS_SET(pPES->SWEVT, 1 << (PES_SWEVT_SPLSECHN0_POS + channel))

/**
 * @brief Config PES event to cpu
 *
 * @param    pPES PES devive point
 * @level    Software event level
 *
 * @retval   None
 */
#define LL_PES_TO_CPU_EVENT_CHANNEL(pPES, channel)     IO_BITMASK_SET(pPES->EVTOCPU, PES_EVTOCPU_CPUEVSEL,\
                                                                   channel << PES_EVTOCPU_CPUEVSEL_POS)
/**
 * @brief Enable PES event to cpu
 *
 * @retval   None
 */
#define LL_PES_TO_CPU_EVENT_ENABLE(pPES)               IO_BITS_SET(pPES->EVTOCPU, PES_EVTOCPU_CPUEVEN)

/**
 * @brief Disable PES event to cpu
 *
 * @retval   None
 */
#define LL_PES_TO_CPU_EVENT_DISABLE(pPES)              IO_BIT_CLEAR(pPES->EVTOCPU, PES_EVTOCPU_CPUEVEN)

/**
 * @brief Clear PES channel status
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_DET_STATUS_CLEAR(pPES,channel)             IO_BITS_SET(pPES->INTSTS, PES_INTSTS_CH0DETSTS<<channel)

/**
 * @brief Enable PES interrupt
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_INTERRUPT_ENABLE(pPES)             IO_BITS_SET(pPES->INTSTS, PES_INTSTS_INTEN)

/**
 * @brief Disable PES interrupt
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_INTERRUPT_DISABLE(pPES)             IO_BIT_CLEAR(pPES->INTSTS, PES_INTSTS_INTEN)

/**
 * @brief Enable PES generate async event
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_ASYNC_ENABLE(pPES, channel)             IO_BITS_SET(pPES->CHNCFG[channel], PES_CHNCFG_ASNYEN)

/**
 * @brief Disable PES generate async event
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_ASYNC_DISABLE(pPES, channel)            IO_BIT_CLEAR(pPES->CHNCFG[channel], PES_CHNCFG_ASNYEN)

/**
 * @brief Enable PES generate async event
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_RESYNC_ENABLE(pPES, channel)            LL_PES_ASYNC_DISABLE(pPES, channel)

/**
 * @brief Disable PES generate async event
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_RESYNC_DISABLE(pPES, channel)           LL_PES_ASYNC_ENABLE(pPES, channel)

/**
 * @brief Config PES generate event mode
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 * @value    PES generate event mode value
 *
 * @retval   None
 */
#define LL_PES_CONFIG_EVENT_OUTPUT_MODE(pPES, channel, value)  \
                                                    IO_BITMASK_SET(pPES->CHNCFG[channel], PES_CHNCFG_EVMD,\
                                                                   value << PES_CHNCFG_EVMD_POS)

/**
 * @brief Config PES IO event dirciton
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 * @value    PES IO event dirciton value
 *
 * @retval   None
 */
#define LL_PES_CONFIG_IO_DIR(pPES, channel, value)     IO_BITMASK_SET(pPES->CHNCFG[channel], PES_CHNCFG_IOEN,\
                                                                   value << PES_CHNCFG_IOEN_POS)
/**
 * @brief Enable current PES channel event level logic and with next PES channel event level
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_AND_NEXT_CHANNEL_ENABLE(pPES, channel)  IO_BITS_SET(pPES->CHNCFG[channel], PES_CHNCFG_ADNENTEN)

/**
 * @brief Disable current PES channel event level logic and with next PES channel event level
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_AND_NEXT_CHANNEL_DISABLE(pPES, channel) IO_BIT_CLEAR(pPES->CHNCFG[channel], PES_CHNCFG_ADNENTEN)

/**
 * @brief Enable current PES channel event level logic or with precision PES channel event level
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_OR_PRE_CHANNEL_ENABLE(pPES, channel)    IO_BITS_SET(pPES->CHNCFG[channel], PES_CHNCFG_ORPREEN)

/**
 * @brief Disable current PES channel event level logic or with precision PES channel event level
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_OR_PRE_CHANNEL_DISABLE(pPES, channel)   IO_BIT_CLEAR(pPES->CHNCFG[channel], PES_CHNCFG_ORPREEN)

/**
 * @brief Enable PES generate event level inverted
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_OUT_INVERT_ENABLE(pPES, channel)        IO_BITS_SET(pPES->CHNCFG[channel], PES_CHNCFG_INVEN)

/**
 * @brief Disable PES generate event level inverted
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_OUT_INVERT_DISABLE(pPES, channel)       IO_BIT_CLEAR(pPES->CHNCFG[channel], PES_CHNCFG_INVEN)

/**
 * @brief Config PES edge detected edge set
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 * @value    PES edge detected select value
 *
 * @retval   None
 */
#define LL_PES_OUT_DETECT_EDGE_SET(pPES, channel, edge)  IO_BITMASK_SET(pPES->CHNCFG[channel], 1 << PES_CHNCFG_DETEDGESEL_POS, \
																				edge << PES_CHNCFG_DETEDGESEL_POS)

/**
 * @brief Enable PES channel output value detect enable
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_OUT_DETECT_ENABLE(pPES, channel)        IO_BITS_SET(pPES->CHNCFG[channel], PES_CHNCFG_DETEN)

/**
 * @brief Enable PES channel output value detect disable
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 *
 * @retval   None
 */
#define LL_PES_OUT_DETECT_DISABLE(pPES, channel)        IO_BIT_CLEAR(pPES->CHNCFG[channel], PES_CHNCFG_DETEN)

/**
 * @brief Select PES event source
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 * @value    PES event source
 *
 * @retval   None
 */
#define LL_PES_EVENT_SOURCE_SELECT(pPES, channel, value) \
                                                    IO_BITMASK_SET(pPES->CHNCFG[channel], PES_CHNCFG_SRCSEL,\
                                                                   value << PES_CHNCFG_SRCSEL_POS)

/**
 * @brief Select PES event master source
 *
 * @param    pPES PES devive point
 * @channel  PES channel index
 * @value    PES master source
 *
 * @retval   None
 */
#define LL_PES_MASTER_SOURCE_SELECT(pPES, channel, value) \
                                                    IO_BITMASK_SET(pPES->CHNCFG[channel], PES_CHNCFG_MSTSEL,\
                                                                   value << PES_CHNCFG_MSTSEL_POS)


#define LL_PES_GET_INTERRUPT_STATUS(pPES, channel)  IO_BITS_GET(pPES->INTSTS, 1 << (channel + 4))

#ifdef __cplusplus
}
#endif

#endif /*_PES_DEV_H_*/
