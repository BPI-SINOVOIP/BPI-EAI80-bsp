/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_ll_acmp.h
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       ACMP Source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_LL_ACMP_H_
#define _GM_LL_ACMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_acmp_dev.h"
#include "gm_hal_dma.h"
#include <gm_soc.h>
#include <gm_basic_types.h>

/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
/**
 * @}
 */

#if (defined(CHIP_GM6721) || defined(CHIP_F6721B))

#define ACMP_SW_RSTJ                                (1 << 10)
#define ACMP_CLK_EN                                 (1 << 10)


/*************** ACMP contral and status register bitfiled ******************/

#define ACMP_CS_BLANKING_POS                        ((uint32_t) 28)   /* ACMP interrupter flag pos */
#define ACMP_CS_BLANKING                            ((uint32_t) ((uint32_t)0x07 << ACMP_CS_BLANKING_POS)) /* ACMP BLANKING TIM filed */

#define ACMP_CS_INT_POS                             ((uint32_t) 25)   /* ACMP interrupter flag pos */
#define ACMP_CS_INT                                 ((uint32_t) ((uint32_t)0x01 << ACMP_CS_INT_POS)) /* ACMP interrupter flag filed */

#define ACMP_CS_OPTVAL_POS                          ((uint32_t) 24)   /* ACMP output level(0 or 1) pos */
#define ACMP_CS_OPTVAL                              ((uint32_t) ((uint32_t)0x01 << ACMP_CS_OPTVAL_POS)) /* ACMP output level(0 or 1) filed */

#define ACMP_CS_INTEN_POS                           ((uint32_t) 19)   /* ACMP interrupter enable pos */
#define ACMP_CS_INTEN                               ((uint32_t) ((uint32_t)0x01 << ACMP_CS_INTEN_POS))  /* ACMP interrupter enable filed */

#define ACMP_CS_OPEN_POS                            ((uint32_t) 18)   /* ACMP output pin function enable pos */
#define ACMP_CS_OPEN                                ((uint32_t) ((uint32_t)0x01 << ACMP_CS_OPEN_POS))   /* ACMP output pin function enable pos*/

#define ACMP_CS_MOD_POS                             ((uint32_t) 16) /* ACMP Interrupt Trigger Mode pos */
#define ACMP_CS_MOD                                 ((uint32_t) ((uint32_t)0x03 << ACMP_CS_MOD_POS))    /* ACMP interrupt trigger mode filed */

#define ACMP_CS_ACHYST_SEL_POS                      ((uint32_t) 14) /* ACMP ACHYST_SEL pos */
#define ACMP_CS_ACHYST_SEL                          ((uint32_t) ((uint32_t)0x03 << ACMP_CS_MOD_POS))    /* ACMP ACHYST_SEL filed */


#define ACMP_CS_AC_EVT_OUT_EN_POS                   ((uint32_t) 13) /* AC_EVT_OUT_EN: Analog comparator event output enable  */
#define ACMP_CS_AC_EVT_OUT_EN                       ((uint32_t) ((uint32_t)0x01 << ACMP_CS_AC_EVT_OUT_EN_POS))    /* ACMP tAnalog comparator event output enable   filed */
#define ACMP_CS_OUTINV_POS                          ((uint32_t) 12) /* ACMP output inversion pos */
#define ACMP_CS_OUTINV                              ((uint32_t) ((uint32_t)0x01 << ACMP_CS_OUTINV_POS))    /* ACMP Output inversion filed */

#define ACMP_CS_NPSEL_POS                           ((uint32_t) 9) /* Analog comparator negative input selection pos */
#define ACMP_CS_NPSEL                               ((uint32_t) ((uint32_t)0x03 << ACMP_CS_NPSEL_POS))  /* Analog comparator negative input selection filed */


#define ACMP_CS_PPSEL_POS                           ((uint32_t) 7)  /* Analog comparator positive input selection pos */
#define ACMP_CS_PPSEL                               ((uint32_t) ((uint32_t)0x03 << ACMP_CS_PPSEL_POS))  /* Analog comparator positive input selection pos */

#define ACMP_CS_EN_POS                              ((uint32_t) 0) /* ACMP modules enable pos */
#define ACMP_CS_EN                                  ((uint32_t) ((uint32_t)0x01 << ACMP_CS_EN_POS)) /* ACMP modules disable pos */

/**
 * @brief Enable one gpio pin as ACMP outputpin
 *
 * @param pACMP                 ACMP devive point
 *
 * @retval                      None
 */
#define OUTPUTPIN_ENABLE(pACMP)                     IO_BIT_SET(pACMP->CS, ACMP_CS_OPEN)

/**
 * @brief Disable one gpio pin as ACMP outputpin
 *
 * @param pACMP                 ACMP devive point
 * @param valule                enable or disable
 *
 * @retval                      None
 */
#define OUTPUTPIN_DISABLE(pACMP)                    IO_BIT_CLEAR(pACMP->CS, ACMP_CS_OPEN)

/**
 * @brief Set pin to negative pin
 *
 * @param pACMP                 ACMP device point
 * @param pin                   Negative pin
 *
 * @retval                      None
 */

/**
 * @brief Enable ACMP compare result inversion
 *
 * @param pACMP                 ACMP device point
 *
 * @retval                      None
 */
#define ACMP_AC_EVT_OUT_ENABLE(pACMP)         IO_BITS_SET(pACMP->CS, ACMP_CS_AC_EVT_OUT_EN)
/**
 * @brief Disable ACMP compare result inversion
 *
 * @param  pACMP                ACMP device point
 *
 * @retval                      None
 */
#define ACMP_AC_EVT_OUT_DISABLE(pACMP)        IO_BIT_CLEAR(pACMP->CS, ACMP_CS_AC_EVT_OUT_EN)



/**
 * @brief Enable ACMP compare result inversion
 *
 * @param pACMP                 ACMP device point
 *
 * @retval                      None
 */
#define ACMP_OUTPUT_INVERSION_ENABLE(pACMP)         IO_BITS_SET(pACMP->CS, ACMP_CS_OUTINV)
/**
 * @brief Disable ACMP compare result inversion
 *
 * @param  pACMP                ACMP device point
 *
 * @retval                      None
 */
#define ACMP_OUTPUT_INVERSION_DISABLE(pACMP)        IO_BIT_CLEAR(pACMP->CS, ACMP_CS_OUTINV)


/**
 * @brief Config timing threshold value for the filter of analog comparator
 *
 * @param pACMP                 ACMP devive point
 * @param valule                Threshold value
 *
 * @retval                      None
 */
#define ACMP_CONFIG_FILTER_THR(pACMP, valule)       (pACMP->FILTTHR = valule)

/**
 * @brief Enable ACMP To triger ADC convert
 *
 * @param pACMP                 ACMP devive point
 * @retval                      None
 */
#define ACMP_TRIGER_ADC_ENABLE(pACMP)               IO_BITS_SET(pACMP->CS, ACMP_CS_ADC_TRIGER)

/**
 * @brief Disable ACMP to triger ADC convert
 *
 * @param pACMP                 ACMP devive point
 * @retval                      None
 */
#define ACMP_TRIGER_ADC_DISABLE(pACMP)              IO_BIT_CLEAR(pACMP->CS, ACMP_CS_ADC_TRIGER)

#define ACMP_ASEL_POS                               4 /*  ACMP anloag pin pos*/

#define ACMP_ASEL                                   ((uint32_t) ((uint32_t)0x07 << ACMP_ASEL_POS))/*  ACMP anloag pin filed*/

/**
 * @brief Config ACMP Anloag Pin As Input
 *
 * @retval                      None
 */
#define ACMP_AMA0_PIN_ENABLE()                      *((volatile uint32_t *)(ANA_IP_ASEL)) |= ACMP_ASEL


#define SET_BLANKING_TIM(pACMP, timSel)                IO_BITMASK_SET(pACMP->CS, ACMP_CS_BLANKING,            \
        (timSel<<ACMP_CS_BLANKING_POS))

#define SET_ACHYST_SEL(pACMP, achyst)                IO_BITMASK_SET(pACMP->CS, ACMP_CS_ACHYST_SEL,            \
        (achyst<<ACMP_CS_ACHYST_SEL_POS))
/**
 * @brief set pin to negative pin
 *
 * @param pACMP                ACMP devive point
 * @param pin                  positive pin
 *
 * @retval                     None
 */

#define SET_NEGATIVE_PIN(pACMP, pin)                IO_BITMASK_SET(pACMP->CS, ACMP_CS_NPSEL,            \
        (pin<<ACMP_CS_NPSEL_POS))
/**
 * @brief set pin to positive pin
 *
 * @param pACMP                ACMP devive point
 * @param pin                  Positive pin
 * @retval                     None
 */
#define SET_POSITIVE_PIN(pACMP, pin)                IO_BITMASK_SET(pACMP->CS, ACMP_CS_PPSEL,            \
        (pin << ACMP_CS_PPSEL_POS))
/**
 * @brief Set compare voltiage match trigger mode
 *
 * @param pACMP                ACMP devive point
 * @param triggerMode          Compare voltiage match trigger mode
 *
 * @retval                     None
 */
#define COMPARE_MATCH_TRIGGER(pACMP, triggerMode)   IO_BITMASK_SET(pACMP->CS, ACMP_CS_MOD,              \
        (triggerMode << ACMP_CS_MOD_POS))
/**
 * @brief ACMP interrupt enable
 *
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_INTERRUPT_ENABLE(pACMP)                IO_BITS_SET(pACMP->CS, ACMP_CS_INTEN)

/**
 * @brief ACMP interrupt disable
 *
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_INTERRUPT_DISABLE(pACMP)               IO_BIT_CLEAR(pACMP->CS, ACMP_CS_INTEN)

/**
 * @brief ACMP function enable
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_ENABLE(pACMP)                          IO_BITS_SET(pACMP->CS, ACMP_CS_EN)

/**
 * @brief ACMP function disable
 *
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_DISABLE(pACMP)                         IO_BIT_CLEAR(pACMP->CS, ACMP_CS_EN)

/**
 * @brief Clear ACMP interrupt
 *
 * @param pACMP                ACMP devive point
 * @retval                     None
 */
#define ACMP_CLEAR_INTERRUPT(pACMP)                 IO_BITS_SET(pACMP->CS, ACMP_CS_INT)

/**
 * @brief Resert ACMP IP
 *
 * @retval                     None
 */
#define ACMP_RESET(void)                           IO_BITS_SET(SYSRSTCLKREG->IP_RST_CTRL[0], ACMP_SW_RSTJ)

/**
 * @brief Enable ACMP clock
 *
 * @retval                     None
 */
#define ACMP_CLKEN(void)                          IO_BITS_SET(SYSRSTCLKREG->CLK_EN_CTRL[0], ACMP_CLK_EN)

#endif

/**
 * Compare result after positivepin and negativepin compare voltage
 */
typedef enum
{
    ACMP_LOW                      = 0x00,             /**< Positivepin voltage is no more than negativepin voltage */
    ACMP_HIHG,                                        /**< Positivepin voltage is more than negativepin voltage */
} ACMP_CompareResult_T;

/**
 *  ACMP events
 */
typedef enum
{
    ACMP_MATCH_EVENT              = 0x00,             /**< When positivepin and negativepin match voltage mode which is
                                                     *   by CompareMatchTrigger mode,it will send event to usr
                                                     */
} ACMP_Event_T;

/**
 *  Compare match trigger mode
 */
typedef enum
{
    ANALOG_MATCH_FALLING        = 0x00,             /**< At begining, positivepin voltage is more than negativepin voltage,
                                                     *   sometime later ,when positivepin voltage get the same voltage as negativepin
                                                     *   it will send this event to usr
                                                     */

    ANALOG_MATCH_RISING         = 0x01,             /**< At begining, positivepin voltage is less than negativepin voltage,
                                                     *   sometime later ,when positivepin voltage get the same voltage as negativepin
                                                     *   it will send this event to usr
                                                     */

    ANALOG_MATCH_FALLINGRISING  = 0x03              /**< The case both ANALOG_MATCH_FALLING and ANALOG_MATCH_RISING will sent event to usr */
} ACMP_CompareMatchTrigger_T;

/**
 *  Agalog compare inputpin
 */
typedef enum
{
    ANALOG_COMPARE_PIN_ACMP0         = 0x0,             /**< ACP_SEL Select agalog compare inputpin0 */
    ANALOG_COMPARE_PIN_ACMP1         = 0x1,             /**< ACP_SEL Select agalog compare inputpin1 */
    ANALOG_COMPARE_DAC1              = 0x2,             /**< Select agalog compare dacoutpin 0 */
    ANALOG_COMPARE_DAC2              = 0x3,             /**< Select agalog compare dacoutpin 1 */

    ANALOG_COMPARE_PIN_ACMP2         = 0x0,          /**< ACN_SEL Select agalog compare inputpin2 */
    ANALOG_COMPARE_PIN_ACMP3         = 0x1,          /**< ACN_SEL Select agalog compare inputpin3 */
    ANALOG_COMPARE_DAC1_OUT          = 0x2,          /**< Select agalog compare dacoutpin 1*/
    ANALOG_COMPARE_DAC2_OUT          = 0x3,          /**< Select agalog compare dacoutpin 2 */

    ANALOG_COMPARE_VREFIN            = 0x4,          /**< Select agalog compare VREFIN */
    ANALOG_COMPARE_VREF              = 0x5,          /**< Select agalog compare VREF */
    ANALOG_COMPARE_VREFIN_HALF       = 0x6,          /**< Select agalog compare 1/2 VREFIN */
    ANALOG_COMPARE_VREFIN_QUAL       = 0x7,          /**< Select agalog compare 1/4 VREFIN */
} ACMP_AnalogComparePin_T;

/**
 *  Comparator blanking source selection bits
 */
typedef enum
{
    NO_BLANKING = 0x0,
    TIM1_OC5_ACMP,
    TIM2_OC5_ACMP,
    TIM3_OC5_ACMP,
    TIM5_OC5_ACMP,
    TIM6_OC5_ACMP,
} ACMP_BLANKING_T;

/**
 *  ACHYST_SEL: Analog comparator 0 hysteresis selection
 */
typedef enum
{
    ACHYST_SEL_0  =  0,                                 /**< hysteresis selection  20mv */
    ACHYST_SEL_1,                                       /**< hysteresis selection  30mv */
    ACHYST_SEL_2,                                       /**< hysteresis selection  40mv */
    ACHYST_SEL_3,                                       /**< hysteresis selection  50mv */
} ACMP_ACHYST_SEL_T;

/**
 * ACMP user handle structure.
 */

typedef void (*ACMP_Callback_T)(void *hACMP, ACMP_Event_T event, unsigned long Param);


typedef struct
{
    uint8_t                     outputInversionEnable;   /**< Enable: compare result will inversion
                                                          *   Disable: compare result will not inversion
                                                          */

    uint8_t                     blankTimesel;
    uint16_t                    filterThr;         /**<  Config timing threshold value for the filter of analog comparator */
    int8_t                     achyst_sel;
    uint8_t                     outputPinEnable;   /**< Enable:enable extoutpin  Disable:disable extoutpin */
    ACMP_AnalogComparePin_T     negativePin;       /**< Select analog negative pin */
    ACMP_AnalogComparePin_T     positivePin;       /**< Select analog positive pin */
    ACMP_CompareMatchTrigger_T  triggerMode;       /**< Inputpin voltage match trigger mode */
    ACMP_Callback_T             callback;          /**< Driver send usr event to call this function */

} ACMP_Config_T;

typedef struct
{
    ACMP_Device_T   *device;                       /**< Pointer to acmp device */
    ACMP_Config_T   config;
} ACMP_Handle_T;


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_LL_ACMP_H_ */

