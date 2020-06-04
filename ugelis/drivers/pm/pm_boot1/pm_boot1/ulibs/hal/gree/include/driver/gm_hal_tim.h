/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          tim.h
 *
 * @author        edward.zhu
 *
 * @version       1.1.0
 *
 * @date          2018/03/19
 *
 * @brief         Header file of tim driver.
 *
 * @note
 *    2018/03/19, edward.zhu, V1.0.0
 *        Initial version.
 *
 */
#ifndef _GM_HAL_TIM_H_
#define _GM_HAL_TIM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <tim/gm_ll_tim_dev.h>
#include <tim/gm_ll_tim.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

/******************************************************************************* * Definitions ******************************************************************************/
/** * TIM IRQ callback typedef. */
typedef void (*TIM_IRQCallback_T)(TIM_Device_T *TIMx, uint32_t event, uint32_t param);

/**
  * TIM Time Base Configuration Structure Definition
  */
typedef struct
{
    uint32_t prescaler;            /*!< Specifies the prescaler value used to divide the TIM clock (0x0000U~0xFFFFU).*/
    uint32_t counterMode;          /*!< Specifies the counter mode.This parameter can be a value of @ref TIM_Counter_Mode */
    uint32_t period;               /*!< Specifies the period value to be loaded into the active Auto-Reload Register at the next update event(0x0000U~0xFFFFU).*/
    uint32_t clockDivision;        /*!< Specifies the clock division.This parameter can be a value of @ref TIM_ClockDivision */
    uint32_t repetitionCounter;    /*!< Specifies the repetition counter value. Each time the RCR downcounter reaches zero, an update event
                                        is generated and counting restarts from the RCR value (N).This means in PWM mode that (N+1) corresponds to:
                                        - the number of PWM periods in edge-aligned mode
                                        - the number of half PWM period in center-aligned mode
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF.
                                        @note This parameter is valid only for TIM1 and TIM8. */
    TIM_IRQCallback_T callback;    /**< Callback pointer to user callback function. */
} TIM_TimeBaseInit_T;

/**
  * TIM Output Compare Configuration Structure definition
  */
typedef struct
{
    uint32_t ocMode;          /*!< Specifies the TIM mode.This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */
    uint32_t pulse;           /*!< Specifies the pulse value to be loaded into the Capture Compare Register(0x0000U~0xFFFFU).*/
    uint32_t ocPolarity;      /*!< Specifies the output polarity.This parameter can be a value of @ref TIM_Output_Compare_Polarity */
    uint32_t ocNPolarity;     /*!< Specifies the complementary output polarity.This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                                   @note This parameter is valid only for TIM1 and TIM8. */
    uint32_t ocFastMode;      /*!< Specifies the Fast mode state.This parameter can be a value of @ref TIM_Output_Fast_State
                                   @note This parameter is valid only in PWM1 and PWM2 mode. */

    uint32_t ocIdleState;     /*!< Specifies the TIM Output Compare pin state during Idle state.This parameter can be a value of @ref
                                   TIM_Output_Compare_Idle_State @note This parameter is valid only for TIM1 and TIM8. */

    uint32_t ocNIdleState;    /*!< Specifies the TIM Output Compare pin state during Idle state.This parameter can be a value of @ref
                                   TIM_Output_Compare_N_Idle_State @note This parameter is valid only for TIM1 and TIM8. */
} TIM_OutputCompareInit_T;

/**
  * TIM One Pulse Mode Configuration Structure definition
  */
typedef struct
{
    uint32_t ocMode;             /*!< Specifies the TIM mode.This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */
    uint32_t pulse;              /*!< Specifies the pulse value to be loaded into the Capture Compare Register.(0x0000U~0xFFFFU).*/
    uint32_t ocPolarity;         /*!< Specifies the output polarity.This parameter can be a value of @ref TIM_Output_Compare_Polarity */
    uint32_t ocNPolarity;        /*!< Specifies the complementary output polarity.This parameter can be a value of @ref
                                    TIM_Output_Compare_N_Polarity @note This parameter is valid only for TIM1 and TIM8. */

    uint32_t ocIdleState;        /*!< Specifies the TIM Output Compare pin state during Idle state.This parameter can be a value of @ref
                                    TIM_Output_Compare_Idle_State @note This parameter is valid only for TIM1 and TIM8. */

    uint32_t ocNIdleState;       /*!< Specifies the TIM Output Compare pin state during Idle state.This parameter can be a value of @ref
                                    TIM_Output_Compare_N_Idle_State @note This parameter is valid only for TIM1 and TIM8. */

    uint32_t icPolarity;         /*!< Specifies the active edge of the input signal.This parameter can be a value of @ref TIM_Input_Capture_Polarity */
    uint32_t icSelection;        /*!< Specifies the input.This parameter can be a value of @ref TIM_Input_Capture_Selection */
    uint32_t icFilter;           /*!< Specifies the input capture filter(0x0~0xF).*/
    TIM_IRQCallback_T callback;  /**< Callback pointer to user callback function. */
} TIM_OnePulseInit_T;

/**
  * TIM Input Capture Configuration Structure definition
  */
typedef struct
{
    uint32_t icPolarity;      /*!< Specifies the active edge of the input signal.This parameter can be a value of @ref TIM_Input_Capture_Polarity */
    uint32_t icSelection;     /*!< Specifies the input.This parameter can be a value of @ref TIM_Input_Capture_Selection */
    uint32_t icPrescaler;     /*!< Specifies the Input Capture Prescaler.This parameter can be a value of @ref TIM_Input_Capture_Prescaler */
    uint32_t icFilter;        /*!< Specifies the input capture filter(0x0~0xF).*/
} TIM_InputCaptureInit_T;

/**
  * TIM Encoder Configuration Structure definition
  */
typedef struct
{
    uint32_t encoderMode;     /*!< Specifies the active edge of the input signal.This parameter can be a value of @ref TIM_Encoder_Mode */
    uint32_t ic1Polarity;     /*!< Specifies the active edge of the input signal.This parameter can be a value of @ref TIM_Input_Capture_Polarity */
    uint32_t ic1Selection;    /*!< Specifies the input.This parameter can be a value of @ref TIM_Input_Capture_Selection */
    uint32_t ic1Prescaler;    /*!< Specifies the Input Capture Prescaler.This parameter can be a value of @ref TIM_Input_Capture_Prescaler */
    uint32_t ic1Filter;       /*!< Specifies the input capture filter(0x0~0xF).*/
    uint32_t ic2Polarity;     /*!< Specifies the active edge of the input signal.This parameter can be a value of @ref TIM_Input_Capture_Polarity */
    uint32_t ic2Selection;    /*!< Specifies the input.This parameter can be a value of @ref TIM_Input_Capture_Selection */
    uint32_t ic2Prescaler;    /*!< Specifies the Input Capture Prescaler.This parameter can be a value of @ref TIM_Input_Capture_Prescaler */
    uint32_t ic2Filter;       /*!< Specifies the input capture filter(0x0~0xF).*/
} TIM_EncoderInit_T;

/**
  * Clock Configuration Handle Structure definition
  */
typedef struct
{
    uint32_t clockSource;      /*!< TIM clock sources.This parameter can be a value of @ref TIM_Clock_Source */
    uint32_t clockPolarity;    /*!< TIM clock polarity.This parameter can be a value of @ref TIM_Clock_Polarity */
    uint32_t clockPrescaler;   /*!< TIM clock prescaler.This parameter can be a value of @ref TIM_Clock_Prescaler */
    uint32_t clockFilter;      /*!< TIM clock filter(0x0~0xF).*/
} TIM_ClockConfig_T;

/**
  * Clear Input Configuration Handle Structure definition
  */
typedef struct
{
    uint32_t clearInputState;      /*!< TIM clear Input state.This parameter can be ENABLE or DISABLE */
    uint32_t clearInputSource;     /*!< TIM clear Input sources.This parameter can be a value of @ref TIM_ClearInput_Source */
    uint32_t clearInputPolarity;   /*!< TIM Clear Input polarity.This parameter can be a value of @ref TIM_ClearInput_Polarity */
    uint32_t clearInputPrescaler;  /*!< TIM Clear Input prescaler.This parameter can be a value of @ref TIM_ClearInput_Prescaler */
    uint32_t clearInputFilter;     /*!< TIM Clear Input filter(0x0~0xF).*/
} TIM_ClearInputConfig_T;

/**
  * TIM Slave configuration Structure definition
  */
typedef struct
{
    uint32_t  slaveMode;          /*!< Slave mode selection.This parameter can be a value of @ref TIM_Slave_Mode */
    uint32_t  inputTrigger;       /*!< Input Trigger source.This parameter can be a value of @ref TIM_Trigger_Selection */
    uint32_t  triggerPolarity;    /*!< Input Trigger polarity.This parameter can be a value of @ref TIM_Trigger_Polarity */
    uint32_t  triggerPrescaler;   /*!< Input trigger prescaler.This parameter can be a value of @ref TIM_Trigger_Prescaler */
    uint32_t  triggerFilter;      /*!< Input trigger filter(0x0~0xF).*/
} TIM_SlaveConfig_T;

/**
  * TIM Hall sensor Configuration Structure definition
  */
typedef struct
{

    uint32_t ic1Polarity;         /*!< Specifies the active edge of the input signal.This parameter can be a value of @ref TIM_Input_Capture_Polarity */
    uint32_t ic1Prescaler;        /*!< Specifies the Input Capture Prescaler.This parameter can be a value of @ref TIM_Input_Capture_Prescaler */
    uint32_t ic1Filter;           /*!< Specifies the input capture filter.This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF */
    uint32_t commutation_Delay;   /*!< Specifies the pulse value to be loaded into the Capture Compare Register(0x0000U~0xFFFFU).*/
} TIM_HallSensorInit_T;

/**
  * TIM Master configuration Structure definition
  */
typedef struct
{
    uint32_t  masterOutputTrigger;   /*!< Trigger output (TRGO) selection.This parameter can be a value of @ref TIM_Master_Mode_Selection */
    uint32_t  masterSlaveMode;       /*!< Master/slave mode selection.This parameter can be a value of @ref TIM_Master_Slave_Mode */
} TIM_MasterConfig_T;

/**
  * TIM Break and Dead time configuration Structure definition
  */
typedef struct
{
    uint32_t lockLevel;            /*!< TIM Lock level.This parameter can be a value of @ref TIM_Lock_level */
    uint32_t deadTime;             /*!< TIM dead Time(0x00U~0xFFU).*/
    uint32_t breakState;           /*!< TIM Break State.This parameter can be a value of @ref TIM_Break_Input_enable_disable */
    uint32_t breakPolarity;        /*!< TIM Break input polarity.This parameter can be a value of @ref TIM_Break_Polarity */
    uint32_t automaticOutput;      /*!< TIM Automatic Output Enable state.This parameter can be a value of @ref TIM_AOE_Bit_Set_Reset */
} TIM_BreakDeadTimeConfig_T;

/**
  * TIM TIM_DMA configuration Structure definition
  */
typedef struct
{
    uint32_t dmaBurstLength;       /* Specifies DMA transmit length in continuous mode.*/
    uint32_t dmaBaseAddress;       /* Specifies DMA transmit base address. */
} TIM_DmaConfig_T;

/**
  * HAL State structures definition
  */
typedef enum
{
    HAL_TIM_STATE_RESET = 0U,       /*!< Peripheral not yet initialized or disabled  */
    HAL_TIM_STATE_READY,            /*!< Peripheral Initialized and ready for use */
    HAL_TIM_STATE_BUSY,             /*!< An internal process is ongoing */
    HAL_TIM_STATE_TIMEOUT,          /*!< Timeout state */
    HAL_TIM_STATE_ERROR,            /*!< Reception process is ongoing */
    HAL_TIM_STATE_DMA_FINISH
} HAL_TIMState_T;

typedef enum
{
    HAL_TIM_UNLOCKED = 0x00U,
    HAL_TIM_LOCKED   = 0x01U
} HAL_Lock_T;

/**
  * HAL Active channel structures definition
  */
typedef enum
{
    HAL_TIM_ACTIVE_CHANNEL_CLEARED = 0x0,  /*!< All active channels cleared */
    HAL_TIM_ACTIVE_CHANNEL_1 = 0x1,        /*!< The active channel is 1 */
    HAL_TIM_ACTIVE_CHANNEL_2 = 0x2,        /*!< The active channel is 2 */
    HAL_TIM_ACTIVE_CHANNEL_3 = 0x4,        /*!< The active channel is 3 */
    HAL_TIM_ACTIVE_CHANNEL_4 = 0x8,        /*!< The active channel is 4 */
} HAL_TIM_ActiveChannel;

/**
  * TIM Time Base Handle Structure definition
  */

typedef struct
{
    TIM_Device_T             *instance;    /*!< Register base address */
    TIM_TimeBaseInit_T       init;         /*!< TIM Time Base required parameters */
    HAL_TIM_ActiveChannel    channel;      /*!< Active channel */
    DMA_Handle_T             *hdma;        /*!< DMA Handlers array.This array is accessed by a @ref DMA_Handle_index */
    HAL_Lock_T               lock;         /*!< Locking object */
    __IO HAL_TIMState_T      state;        /*!< TIM operation state */
    uint32_t IRQn;                         /*!<TIM interrupt request number*/
    TIM_IRQCallback_T        callback;     /*!< Callback pointer to user callback function */
} TIM_Handle_T;

typedef enum
{
    RESET = 0U,
    SET = !RESET
} FlagStatus, ITStatus;

/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_Status_T;

/** @defgroup TIM_Input_Channel_Polarity TIM Input Channel Polarity
  * @{
  */
#define  TIM_INPUTCHANNELPOLARITY_RISING      ((uint32_t)0x00000000U)              /*!< Polarity for TIx source */
#define  TIM_INPUTCHANNELPOLARITY_FALLING     (TIM_CCER_CC1P)                      /*!< Polarity for TIx source */
#define  TIM_INPUTCHANNELPOLARITY_BOTHEDGE    (TIM_CCER_CC1P | TIM_CCER_CC1NP)     /*!< Polarity for TIx source */
/**
  * @}
  */

/** @defgroup TIM_ETR_Polarity  TIM ETR Polarity
  * @{
  */
#define TIM_ETRPOLARITY_INVERTED         (TIM_SMCR_ETP)                          /*!< Polarity for ETR source */
#define TIM_ETRPOLARITY_NONINVERTED      ((uint32_t)0x00000000U)                 /*!< Polarity for ETR source */
/**
  * @}
  */

/** @defgroup TIM_ETR_Prescaler  TIM ETR Prescaler
  * @{
  */
#define TIM_ETRPRESCALER_DIV1       ((uint32_t)0x00000000U)                   /*!< No prescaler is used */
#define TIM_ETRPRESCALER_DIV2       (TIM_SMCR_ETPS_0)                         /*!< ETR input source is divided by 2 */
#define TIM_ETRPRESCALER_DIV4       (TIM_SMCR_ETPS_1)                         /*!< ETR input source is divided by 4 */
#define TIM_ETRPRESCALER_DIV8       (TIM_SMCR_ETPS)                           /*!< ETR input source is divided by 8 */
/**
  * @}
  */

/** @defgroup TIM_Counter_Mode  TIM Counter Mode
  * @{
  */
#define TIM_COUNTERMODE_UP                 ((uint32_t)0x00000000U)
#define TIM_COUNTERMODE_DOWN               TIM_CR1_DIR
#define TIM_COUNTERMODE_CENTERALIGNED1     TIM_CR1_CMS_0
#define TIM_COUNTERMODE_CENTERALIGNED2     TIM_CR1_CMS_1
#define TIM_COUNTERMODE_CENTERALIGNED3     TIM_CR1_CMS
/**
  * @}
  */

/** @defgroup TIM_ClockDivision TIM Clock Division
  * @{
  */
#define TIM_CLOCKDIVISION_DIV1                       ((uint32_t)0x00000000U)
#define TIM_CLOCKDIVISION_DIV2                       (TIM_CR1_CKD_0)
#define TIM_CLOCKDIVISION_DIV4                       (TIM_CR1_CKD_1)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_and_PWM_modes  TIM Output Compare and PWM modes
  * @{
  */
#define TIM_OCMODE_TIMING                   ((uint32_t)0x00000000U)
#define TIM_OCMODE_ACTIVE                   (TIM_CCMR1_OC1M_0)
#define TIM_OCMODE_INACTIVE                 (TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_TOGGLE                   (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1)
#define TIM_OCMODE_PWM1                     (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_PWM2                     (TIM_CCMR1_OC1M)
#define TIM_OCMODE_FORCED_ACTIVE            (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_2)
#define TIM_OCMODE_FORCED_INACTIVE          (TIM_CCMR1_OC1M_2)

/**
  * @}
  */

/** @defgroup TIM_Output_Fast_State  TIM Output Fast State
  * @{
  */
#define TIM_OCFAST_DISABLE                ((uint32_t)0x00000000U)
#define TIM_OCFAST_ENABLE                 (TIM_CCMR1_OC1FE)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Polarity  TIM Output Compare Polarity
  * @{
  */
#define TIM_OCPOLARITY_HIGH                ((uint32_t)0x00000000U)
#define TIM_OCPOLARITY_LOW                 (TIM_CCER_CC1P)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Polarity  TIM Output CompareN Polarity
  * @{
  */
#define TIM_OCNPOLARITY_HIGH               ((uint32_t)0x00000000U)
#define TIM_OCNPOLARITY_LOW                (TIM_CCER_CC1NP)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Idle_State  TIM Output Compare Idle State
  * @{
  */
#define TIM_OCIDLESTATE_SET                (TIM_CR2_OIS1)
#define TIM_OCIDLESTATE_RESET              ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Idle_State  TIM Output Compare N Idle State
  * @{
  */
#define TIM_OCNIDLESTATE_SET               (TIM_CR2_OIS1N)
#define TIM_OCNIDLESTATE_RESET             ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_Channel  TIM Channel
  * @{
  */
#define TIM_CHANNEL_1                      ((uint32_t)0x00000000U)
#define TIM_CHANNEL_2                      ((uint32_t)0x00000004U)
#define TIM_CHANNEL_3                      ((uint32_t)0x00000008U)
#define TIM_CHANNEL_4                      ((uint32_t)0x0000000CU)
#define TIM_CHANNEL_ALL                    ((uint32_t)0x00000018U)

/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Polarity  TIM Input Capture Polarity
  * @{
  */
#define  TIM_ICPOLARITY_RISING        TIM_INPUTCHANNELPOLARITY_RISING
#define  TIM_ICPOLARITY_FALLING       TIM_INPUTCHANNELPOLARITY_FALLING
#define  TIM_ICPOLARITY_BOTHEDGE      TIM_INPUTCHANNELPOLARITY_BOTHEDGE
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Selection  TIM Input Capture Selection
  * @{
  */
#define TIM_ICSELECTION_DIRECTTI        (TIM_CCMR1_CC1S_0)       /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSELECTION_INDIRECTTI      (TIM_CCMR1_CC1S_1)       /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to IC2, IC1, IC4 or IC3, respectively */
#define TIM_ICSELECTION_TRC             (TIM_CCMR1_CC1S)         /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC */

/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Prescaler  TIM Input Capture Prescaler
  * @{
  */
#define TIM_ICPSC_DIV1         ((uint32_t)0x00000000U)       /*!< Capture performed each time an edge is detected on the capture input */
#define TIM_ICPSC_DIV2         (TIM_CCMR1_IC1PSC_0)          /*!< Capture performed once every 2 events */
#define TIM_ICPSC_DIV4         (TIM_CCMR1_IC1PSC_1)          /*!< Capture performed once every 4 events */
#define TIM_ICPSC_DIV8         (TIM_CCMR1_IC1PSC)            /*!< Capture performed once every 8 events */
/**
  * @}
  */

/** @defgroup TIM_One_Pulse_Mode TIM One Pulse Mode
  * @{
  */
#define TIM_OPMODE_SINGLE                  (TIM_CR1_OPM)
#define TIM_OPMODE_REPETITIVE              ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_Encoder_Mode TIM Encoder Mode
  * @{
  */
#define TIM_ENCODERMODE_TI1                (TIM_SMCR_SMS_0)
#define TIM_ENCODERMODE_TI2                (TIM_SMCR_SMS_1)
#define TIM_ENCODERMODE_TI12               (TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0)

/**
  * @}
  */

/** @defgroup TIM_Interrupt_definition  TIM Interrupt definition
  * @{
  */
#define TIM_IT_UPDATE           (TIM_DIER_UIE)
#define TIM_IT_CC1              (TIM_DIER_CC1IE)
#define TIM_IT_CC2              (TIM_DIER_CC2IE)
#define TIM_IT_CC3              (TIM_DIER_CC3IE)
#define TIM_IT_CC4              (TIM_DIER_CC4IE)
#define TIM_IT_COM              (TIM_DIER_COMIE)
#define TIM_IT_TRIGGER          (TIM_DIER_TIE)
#define TIM_IT_BREAK            (TIM_DIER_BIE)
/**
  * @}
  */

/** @defgroup TIM_Mask_Definitions TIM Mask Definition
  * @{
  */
/* The counter of a timer instance is disabled only if all the CCx and CCxN
   channels have been disabled */
#define TIM_CCER_CCxE_MASK   ((uint32_t)(TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E))
#define TIM_CCER_CCxNE_MASK  ((uint32_t)(TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE | TIM_CCER_CC4NE))
/**
  * @}
  */


/** @defgroup TIM_Commutation_Source  TIM Commutation Source
  * @{
  */
#define TIM_COMMUTATION_TRGI              (TIM_CR2_CCUS)
#define TIM_COMMUTATION_SOFTWARE          ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_DMA_sources  TIM DMA sources
  * @{
  */
#define TIM_DMA_UPDATE                     (TIM_DIER_UDE)
#define TIM_DMA_CC1                        (TIM_DIER_CC1DE)
#define TIM_DMA_CC2                        (TIM_DIER_CC2DE)
#define TIM_DMA_CC3                        (TIM_DIER_CC3DE)
#define TIM_DMA_CC4                        (TIM_DIER_CC4DE)
#define TIM_DMA_COM                        (TIM_DIER_COMDE)
#define TIM_DMA_TRIGGER                    (TIM_DIER_TDE)
/**
  * @}
  */

/** @defgroup TIM_Event_Source  TIM Event Source
  * @{
  */
#define TIM_EVENTSOURCE_UPDATE              TIM_EGR_UG
#define TIM_EVENTSOURCE_CC1                 TIM_EGR_CC1G
#define TIM_EVENTSOURCE_CC2                 TIM_EGR_CC2G
#define TIM_EVENTSOURCE_CC3                 TIM_EGR_CC3G
#define TIM_EVENTSOURCE_CC4                 TIM_EGR_CC4G
#define TIM_EVENTSOURCE_COM                 TIM_EGR_COMG
#define TIM_EVENTSOURCE_TRIGGER             TIM_EGR_TG
#define TIM_EVENTSOURCE_BREAK               TIM_EGR_BG

/**
  * @}
  */

/** @defgroup TIM_Flag_definition  TIM Flag definition
  * @{
  */
#define TIM_FLAG_UPDATE                    (TIM_SR_UIF)
#define TIM_FLAG_CC1                       (TIM_SR_CC1IF)
#define TIM_FLAG_CC2                       (TIM_SR_CC2IF)
#define TIM_FLAG_CC3                       (TIM_SR_CC3IF)
#define TIM_FLAG_CC4                       (TIM_SR_CC4IF)
#define TIM_FLAG_COM                       (TIM_SR_COMIF)
#define TIM_FLAG_TRIGGER                   (TIM_SR_TIF)
#define TIM_FLAG_BREAK                     (TIM_SR_BIF)
#define TIM_FLAG_CC1OF                     (TIM_SR_CC1OF)
#define TIM_FLAG_CC2OF                     (TIM_SR_CC2OF)
#define TIM_FLAG_CC3OF                     (TIM_SR_CC3OF)
#define TIM_FLAG_CC4OF                     (TIM_SR_CC4OF)
/**
  * @}
  */

/** @defgroup TIM_Clock_Source  TIM Clock Source
  * @{
  */
#define TIM_CLOCKSOURCE_ETRMODE2    (TIM_SMCR_ETPS_1)
#define TIM_CLOCKSOURCE_INTERNAL    (TIM_SMCR_ETPS_0)
#define TIM_CLOCKSOURCE_ITR0        ((uint32_t)0x00000000U)
#define TIM_CLOCKSOURCE_ITR1        (TIM_SMCR_TS_0)
#define TIM_CLOCKSOURCE_ITR2        (TIM_SMCR_TS_1)
#define TIM_CLOCKSOURCE_ITR3        (TIM_SMCR_TS_0 | TIM_SMCR_TS_1)
#define TIM_CLOCKSOURCE_TI1ED       (TIM_SMCR_TS_2)
#define TIM_CLOCKSOURCE_TI1         (TIM_SMCR_TS_0 | TIM_SMCR_TS_2)
#define TIM_CLOCKSOURCE_TI2         (TIM_SMCR_TS_1 | TIM_SMCR_TS_2)
#define TIM_CLOCKSOURCE_ETRMODE1    (TIM_SMCR_TS)
/**
  * @}
  */

/** @defgroup TIM_Clock_Polarity  TIM Clock Polarity
  * @{
  */
#define TIM_CLOCKPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED           /*!< Polarity for ETRx clock sources */
#define TIM_CLOCKPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED        /*!< Polarity for ETRx clock sources */
#define TIM_CLOCKPOLARITY_RISING             TIM_INPUTCHANNELPOLARITY_RISING    /*!< Polarity for TIx clock sources */
#define TIM_CLOCKPOLARITY_FALLING            TIM_INPUTCHANNELPOLARITY_FALLING   /*!< Polarity for TIx clock sources */
#define TIM_CLOCKPOLARITY_BOTHEDGE           TIM_INPUTCHANNELPOLARITY_BOTHEDGE  /*!< Polarity for TIx clock sources */
/**
  * @}
  */

/** @defgroup TIM_Clock_Prescaler  TIM Clock Prescaler
  * @{
  */
#define TIM_CLOCKPRESCALER_DIV1              TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_CLOCKPRESCALER_DIV2              TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Clock: Capture performed once every 2 events. */
#define TIM_CLOCKPRESCALER_DIV4              TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Clock: Capture performed once every 4 events. */
#define TIM_CLOCKPRESCALER_DIV8              TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Clock: Capture performed once every 8 events. */
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Source TIM Clear Input Source
  * @{
  */
#define TIM_CLEARINPUTSOURCE_ETR           ((uint32_t)0x00000001U)
#define TIM_CLEARINPUTSOURCE_NONE          ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Polarity  TIM Clear Input Polarity
  * @{
  */
#define TIM_CLEARINPUTPOLARITY_INVERTED           TIM_ETRPOLARITY_INVERTED            /*!< Polarity for ETRx pin */
#define TIM_CLEARINPUTPOLARITY_NONINVERTED        TIM_ETRPOLARITY_NONINVERTED         /*!< Polarity for ETRx pin */
/**
  * @}
  */

/** @defgroup TIM_ClearInput_Prescaler TIM Clear Input Prescaler
  * @{
  */
#define TIM_CLEARINPUTPRESCALER_DIV1       TIM_ETRPRESCALER_DIV1      /*!< No prescaler is used */
#define TIM_CLEARINPUTPRESCALER_DIV2       TIM_ETRPRESCALER_DIV2      /*!< Prescaler for External ETR pin: Capture performed once every 2 events. */
#define TIM_CLEARINPUTPRESCALER_DIV4       TIM_ETRPRESCALER_DIV4      /*!< Prescaler for External ETR pin: Capture performed once every 4 events. */
#define TIM_CLEARINPUTPRESCALER_DIV8       TIM_ETRPRESCALER_DIV8      /*!< Prescaler for External ETR pin: Capture performed once every 8 events. */
/**
  * @}
  */

/** @defgroup TIM_Lock_level  TIM Lock level
  * @{
  */
#define TIM_LOCKLEVEL_OFF          ((uint32_t)0x00000000U)
#define TIM_LOCKLEVEL_1            (TIM_BDTR_LOCK_0)
#define TIM_LOCKLEVEL_2            (TIM_BDTR_LOCK_1)
#define TIM_LOCKLEVEL_3            (TIM_BDTR_LOCK)
/**
  * @}
  */
/** @defgroup TIM_Break_Input_enable_disable  TIM Break Input State
  * @{
  */
#define TIM_BREAK_ENABLE          (TIM_BDTR_BKE)
#define TIM_BREAK_DISABLE         ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_Break_Polarity  TIM Break Polarity
  * @{
  */
#define TIM_BREAKPOLARITY_LOW        ((uint32_t)0x00000000U)
#define TIM_BREAKPOLARITY_HIGH       (TIM_BDTR_BKP)
/**
  * @}
  */

/** @defgroup TIM_AOE_Bit_Set_Reset  TIM AOE Bit State
  * @{
  */
#define TIM_AUTOMATICOUTPUT_ENABLE           (TIM_BDTR_AOE)
#define TIM_AUTOMATICOUTPUT_DISABLE          ((uint32_t)0x00000000U)
/**
  * @}
  */

/* Mask used to set the TDG[x:0] of the DTG bits of the TIMx_BDTR register */
#define DT_DELAY_1     ((uint8_t)0x7FU)
#define DT_DELAY_2     ((uint8_t)0x3FU)
#define DT_DELAY_3     ((uint8_t)0x1FU)
#define DT_DELAY_4     ((uint8_t)0x1FU)

/* Mask used to set the DTG[7:5] bits of the DTG bits of the TIMx_BDTR register */
#define DT_RANGE_1     ((uint8_t)0x00U)
#define DT_RANGE_2     ((uint8_t)0x80U)
#define DT_RANGE_3     ((uint8_t)0xC0U)
#define DT_RANGE_4     ((uint8_t)0xE0U)

/** @defgroup TIM_Master_Mode_Selection TIM Master Mode Selection
  * @{
  */
#define TIM_TRGO_RESET            ((uint32_t)0x00000000U)
#define TIM_TRGO_ENABLE           (TIM_CR2_MMS_0)
#define TIM_TRGO_UPDATE           (TIM_CR2_MMS_1)
#define TIM_TRGO_OC1              ((TIM_CR2_MMS_1 | TIM_CR2_MMS_0))
#define TIM_TRGO_OC1REF           (TIM_CR2_MMS_2)
#define TIM_TRGO_OC2REF           ((TIM_CR2_MMS_2 | TIM_CR2_MMS_0))
#define TIM_TRGO_OC3REF           ((TIM_CR2_MMS_2 | TIM_CR2_MMS_1))
#define TIM_TRGO_OC4REF           ((TIM_CR2_MMS_2 | TIM_CR2_MMS_1 | TIM_CR2_MMS_0))
/**
  * @}
  */

/** @defgroup TIM_Slave_Mode TIM Slave Mode
  * @{
  */
#define TIM_SLAVEMODE_DISABLE              ((uint32_t)0x00000000U)
#define TIM_SLAVEMODE_RESET                ((uint32_t)0x00000004U)
#define TIM_SLAVEMODE_GATED                ((uint32_t)0x00000005U)
#define TIM_SLAVEMODE_TRIGGER              ((uint32_t)0x00000006U)
#define TIM_SLAVEMODE_EXTERNAL1            ((uint32_t)0x00000007U)
/**
  * @}
  */

/** @defgroup TIM_Master_Slave_Mode  TIM Master Slave Mode
  * @{
  */
#define TIM_MASTERSLAVEMODE_ENABLE          ((uint32_t)0x00000080U)
#define TIM_MASTERSLAVEMODE_DISABLE         ((uint32_t)0x00000000U)
/**
  * @}
  */

/** @defgroup TIM_Trigger_Selection  TIM Trigger Selection
  * @{
  */
#define TIM_TS_ITR0                        ((uint32_t)0x00000000U)
#define TIM_TS_ITR1                        ((uint32_t)0x00000010U)
#define TIM_TS_ITR2                        ((uint32_t)0x00000020U)
#define TIM_TS_ITR3                        ((uint32_t)0x00000030U)
#define TIM_TS_TI1F_ED                     ((uint32_t)0x00000040U)
#define TIM_TS_TI1FP1                      ((uint32_t)0x00000050U)
#define TIM_TS_TI2FP2                      ((uint32_t)0x00000060U)
#define TIM_TS_ETRF                        ((uint32_t)0x00000070U)
#define TIM_TS_NONE                        ((uint32_t)0x0000FFFFU)
/**
  * @}
  */

/** @defgroup TIM_Trigger_Polarity TIM Trigger Polarity
  * @{
  */
#define TIM_TRIGGERPOLARITY_INVERTED         TIM_ETRPOLARITY_INVERTED             /*!< Polarity for ETRx trigger sources */
#define TIM_TRIGGERPOLARITY_NONINVERTED      TIM_ETRPOLARITY_NONINVERTED          /*!< Polarity for ETRx trigger sources */
#define TIM_TRIGGERPOLARITY_RISING           TIM_INPUTCHANNELPOLARITY_RISING      /*!< Polarity for TIxFPx or TI1_ED trigger sources */
#define TIM_TRIGGERPOLARITY_FALLING          TIM_INPUTCHANNELPOLARITY_FALLING     /*!< Polarity for TIxFPx or TI1_ED trigger sources */
#define TIM_TRIGGERPOLARITY_BOTHEDGE         TIM_INPUTCHANNELPOLARITY_BOTHEDGE    /*!< Polarity for TIxFPx or TI1_ED trigger sources */
/**
  * @}
  */

/** @defgroup TIM_Trigger_Prescaler TIM Trigger Prescaler
  * @{
  */
#define TIM_TRIGGERPRESCALER_DIV1             TIM_ETRPRESCALER_DIV1     /*!< No prescaler is used */
#define TIM_TRIGGERPRESCALER_DIV2             TIM_ETRPRESCALER_DIV2     /*!< Prescaler for External ETR Trigger: Capture performed once every 2 events. */
#define TIM_TRIGGERPRESCALER_DIV4             TIM_ETRPRESCALER_DIV4     /*!< Prescaler for External ETR Trigger: Capture performed once every 4 events. */
#define TIM_TRIGGERPRESCALER_DIV8             TIM_ETRPRESCALER_DIV8     /*!< Prescaler for External ETR Trigger: Capture performed once every 8 events. */
/**
  * @}
  */


/** @defgroup TIM_TI1_Selection TIM TI1 Selection
  * @{
  */
#define TIM_TI1SELECTION_CH1                ((uint32_t)0x00000000U)
#define TIM_TI1SELECTION_XORCOMBINATION     (TIM_CR2_TI1S)
/**
  * @}
  */

/** @defgroup TIM_DMA_Base_address  TIM DMA Base address
  * @{
  */
#define TIM_DMABASE_CR1                    ((uint32_t)0x00000000U)
#define TIM_DMABASE_CR2                    ((uint32_t)0x00000001U)
#define TIM_DMABASE_SMCR                   ((uint32_t)0x00000002U)
#define TIM_DMABASE_DIER                   ((uint32_t)0x00000003U)
#define TIM_DMABASE_SR                     ((uint32_t)0x00000004U)
#define TIM_DMABASE_EGR                    ((uint32_t)0x00000005U)
#define TIM_DMABASE_CCMR1                  ((uint32_t)0x00000006U)
#define TIM_DMABASE_CCMR2                  ((uint32_t)0x00000007U)
#define TIM_DMABASE_CCER                   ((uint32_t)0x00000008U)
#define TIM_DMABASE_CNT                    ((uint32_t)0x00000009U)
#define TIM_DMABASE_PSC                    ((uint32_t)0x0000000AU)
#define TIM_DMABASE_ARR                    ((uint32_t)0x0000000BU)
#define TIM_DMABASE_RCR                    ((uint32_t)0x0000000CU)
#define TIM_DMABASE_CCR1                   ((uint32_t)0x0000000DU)
#define TIM_DMABASE_CCR2                   ((uint32_t)0x0000000EU)
#define TIM_DMABASE_CCR3                   ((uint32_t)0x0000000FU)
#define TIM_DMABASE_CCR4                   ((uint32_t)0x00000010U)
#define TIM_DMABASE_BDTR                   ((uint32_t)0x00000011U)
/**
  * @}
  */

/** @defgroup TIM_DMA_Burst_Length  TIM DMA Burst Length
  * @{
  */
#define TIM_DMABURSTLENGTH_1TRANSFER           ((uint32_t)0x00000000U)
#define TIM_DMABURSTLENGTH_2TRANSFERS          ((uint32_t)0x00000100U)
#define TIM_DMABURSTLENGTH_3TRANSFERS          ((uint32_t)0x00000200U)
#define TIM_DMABURSTLENGTH_4TRANSFERS          ((uint32_t)0x00000300U)
#define TIM_DMABURSTLENGTH_5TRANSFERS          ((uint32_t)0x00000400U)
#define TIM_DMABURSTLENGTH_6TRANSFERS          ((uint32_t)0x00000500U)
#define TIM_DMABURSTLENGTH_7TRANSFERS          ((uint32_t)0x00000600U)
#define TIM_DMABURSTLENGTH_8TRANSFERS          ((uint32_t)0x00000700U)
#define TIM_DMABURSTLENGTH_9TRANSFERS          ((uint32_t)0x00000800U)
#define TIM_DMABURSTLENGTH_10TRANSFERS         ((uint32_t)0x00000900U)
#define TIM_DMABURSTLENGTH_11TRANSFERS         ((uint32_t)0x00000A00U)
#define TIM_DMABURSTLENGTH_12TRANSFERS         ((uint32_t)0x00000B00U)
#define TIM_DMABURSTLENGTH_13TRANSFERS         ((uint32_t)0x00000C00U)
#define TIM_DMABURSTLENGTH_14TRANSFERS         ((uint32_t)0x00000D00U)
#define TIM_DMABURSTLENGTH_15TRANSFERS         ((uint32_t)0x00000E00U)
#define TIM_DMABURSTLENGTH_16TRANSFERS         ((uint32_t)0x00000F00U)
#define TIM_DMABURSTLENGTH_17TRANSFERS         ((uint32_t)0x00001000U)
#define TIM_DMABURSTLENGTH_18TRANSFERS         ((uint32_t)0x00001100U)
/**
  * @}
  */

/** @defgroup DMA_Handle_index  DMA Handle index
  * @{
  */
#define TIM_DMA_ID_UPDATE         ((uint16_t)0x0000)       /*!< Index of the DMA handle used for Update DMA requests */
#define TIM_DMA_ID_CC1            ((uint16_t)0x0001)       /*!< Index of the DMA handle used for Capture/Compare 1 DMA requests */
#define TIM_DMA_ID_CC2            ((uint16_t)0x0002)       /*!< Index of the DMA handle used for Capture/Compare 2 DMA requests */
#define TIM_DMA_ID_CC3            ((uint16_t)0x0003)       /*!< Index of the DMA handle used for Capture/Compare 3 DMA requests */
#define TIM_DMA_ID_CC4            ((uint16_t)0x0004)       /*!< Index of the DMA handle used for Capture/Compare 4 DMA requests */
#define TIM_DMA_ID_COMMUTATION    ((uint16_t)0x0005)       /*!< Index of the DMA handle used for Commutation DMA requests */
#define TIM_DMA_ID_TRIGGER        ((uint16_t)0x0006)       /*!< Index of the DMA handle used for Trigger DMA requests */
/**
  * @}
  */

/** @defgroup Channel_CC_State  Channel CC State
  * @{
  */
#define TIM_CCx_ENABLE                   ((uint32_t)0x00000001U)
#define TIM_CCx_DISABLE                  ((uint32_t)0x00000000U)
#define TIM_CCxN_ENABLE                  ((uint32_t)0x00000004U)
#define TIM_CCxN_DISABLE                 ((uint32_t)0x00000000U)
/**
  * @}
  */


/************************************* * Global function ******************************************************/

/********************************* Time Base functions ********************************************************/

/**
  * @brief  TIM interrupt function entry.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitCommon(TIM_Handle_T *htim);

/**
  * @brief  Initializes the TIM Time base Unit according to the specified
  *         parameters in the TIM_Handle_T and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitTimeBase(TIM_Handle_T *htim);

/**
  * @brief  Starts the TIM Time Base generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StartTimeBase(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
  * @brief  Stops the TIM Base generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StopTimeBase(TIM_Handle_T *htim, TIM_Device_T *TIMx);


/********************************* Timer Output Compare functions **********************************************/

/**
  * @brief  Initializes the TIM Output Compare according to the specified
  *         parameters in the TIM_Handle_T and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitOutputCompare(TIM_Handle_T *htim);

/**
  * @brief  Starts the TIM Output Compare signal generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channel to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StartOutputCompare(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);

/**
  * @brief  Stops the TIM Output Compare signal generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channel to be disabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StopOutputCompare(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);


/********************************* Timer PWM functions *********************************************************/

/**
  * @brief  Initializes the TIM PWM Time Base according to the specified
  *         parameters in the TIM_Handle_T and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitPWM(TIM_Handle_T *htim);

/**
  * @brief  Starts the PWM signal generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StartPWM(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);

/**
  * @brief  Stops the PWM signal generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channels to be disabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StopPWM(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);


/********************************* Timer Input Capture functions ***********************************************/

/**
  * @brief  Initializes the TIM Input Capture Time base according to the specified
  *         parameters in the TIM_Handle_T and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitInputCapture(TIM_Handle_T *htim);

/**
  * @brief  Starts the TIM Input Capture measurement.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StartInputCapture(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);

/**
  * @brief  Stops the TIM Input Capture measurement.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channels to be disabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StopInputCapture(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);


/********************************* Timer One Pulse functions ***************************************************/

/**
  * @brief  Initializes the TIM One Pulse Time Base according to the specified
  *         parameters in the TIM_Handle_T and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  OnePulseMode: Select the One pulse mode.
  *         This parameter can be one of the following values:
  *            @arg TIM_OPMODE_SINGLE: Only one pulse will be generated.
  *            @arg TIM_OPMODE_REPETITIVE: Repetitive pulses will be generated.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitOnePulse(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t onePulseMode);

/**
  * @brief  Starts the TIM One Pulse signal generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  OutputChannel : TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StartOnePulse(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t outputChannel);

/**
  * @brief  Stops the TIM One Pulse signal generation.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  OutputChannel : TIM Channels to be disable.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StopOnePulse(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t outputChannel);


/********************************* Timer Encoder functions *****************************************************/

/**
  * @brief  Initializes the TIM Encoder Interface and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sConfig: TIM Encoder Interface configuration structure
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitEncoder(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_EncoderInit_T *sConfig);

/**
   * @brief  Starts the TIM Encoder Interface.
   * @param  htim: pointer to a TIM_Handle_T structure that contains
   *                the configuration information for TIM module.
   * @param  Channel: TIM Channels to be enabled.
   *          This parameter can be one of the following values:
   *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
   *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
   *            @arg TIM_CHANNEL_ALL: TIM Channel 1 and TIM Channel 2 are selected
   * @retval RET_OK indicates success; Else indicates failure.
   */
RET_CODE HAL_TIM_StartEncoder(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);

/**
  * @brief  Stops the TIM Encoder Interface.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channels to be disabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_ALL: TIM Channel 1 and TIM Channel 2 are selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StopEncoder(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);


/*******************************************  Timer Hall Sensor functions  **********************************************/

/**
  * @brief  Initializes the TIM Hall Sensor Interface and create the associated handle.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sConfig: TIM Hall Sensor configuration structure
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_InitHallSensor(TIM_Handle_T *htim, TIM_Device_T *TIMx,
                                TIM_HallSensorInit_T *sConfig);

/**
  * @brief  Starts the TIM Hall Sensor Interface.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_StartHallSensor(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
   * @brief  Stops the TIM Hall sensor Interface.
   * @param  htim: pointer to a TIM_Handle_T structure that contains
   *                the configuration information for TIM module.
   * @retval RET_OK indicates success; Else indicates failure.
   */
RET_CODE HAL_TIM_StopHallSensor(TIM_Handle_T *htim, TIM_Device_T *TIMx);


/********************************* Control functions  *********************************************************/

/**
  * @brief  Initializes the TIM Output Compare Channels according to the specified
  *         parameters in the TIM_OutputCompareInit_T.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sConfig: TIM Output Compare configuration structure
  * @param  Channel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigOCChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sConfig, uint32_t channel);

/**
  * @brief  Initializes the TIM Input Capture Channels according to the specified
  *         parameters in the TIM_InputCaptureInit_T.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sConfig: TIM Input Capture configuration structure
  * @param  Channel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigICChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_InputCaptureInit_T *sConfig, uint32_t channel);

/**
  * @brief  Initializes the TIM PWM  channels according to the specified
  *         parameters in the TIM_OutputCompareInit_T.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sConfig: TIM PWM configuration structure
  * @param  Channel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigPWMChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sConfig, uint32_t channel);

/**
  * @brief  Initializes the TIM One Pulse Channels according to the specified
  *         parameters in the TIM_OnePulseInit_T.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sConfig: TIM One Pulse configuration structure
  * @param  OutputChannel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @param  InputChannel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigOPMChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OnePulseInit_T *sConfig, uint32_t outputChannel, uint32_t inputChannel);

/**
  * @brief  Configures the OCRef clear feature
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sClearInputConfig: pointer to a TIM_ClearInputConfig_T structure that
  *         contains the OCREF clear feature and parameters for the TIM peripheral.
  * @param  Channel: specifies the TIM Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigOCrefClear(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_ClearInputConfig_T *sClearInputConfig, uint32_t channel);

/**
  * @brief   Configures the clock source to be used
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sClockSourceConfig: pointer to a TIM_ClockConfig_T structure that
  *         contains the clock source information for the TIM peripheral.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigClockSource(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_ClockConfig_T *sClockSourceConfig);

/**
  * @brief  Configures the TIM in master mode.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sMasterConfig: pointer to a TIM_MasterConfigTypeDef structure that
  *         contains the selected trigger output (TRGO) and the Master/Slave
  *         mode.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigMasterSyn(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_MasterConfig_T *sMasterConfig);

/**
  * @brief  Configures the TIM in Slave mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sSlaveConfig: pointer to a TIM_SlaveConfig_T structure that
  *         contains the selected trigger (internal trigger input, filtered
  *         timer input or external trigger input) and the ) and the Slave
  *         mode (Disable, Reset, Gated, Trigger, External clock mode 1).
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigSlaveSyn(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_SlaveConfig_T *sSlaveConfig);

/**
  * @brief  Time Output Compare 4 configuration
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sSlaveConfig: The slave configuration structure
  * @retval None
  */
void HAL_TIM_SlaveTimerConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_SlaveConfig_T *sSlaveConfig);

/**
  * @brief  Generate a software event
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  EventSource: specifies the event source.
  *          This parameter can be one of the following values:
  *            @arg TIM_EVENTSOURCE_UPDATE: Timer update Event source
  *            @arg TIM_EVENTSOURCE_CC1: Timer Capture Compare 1 Event source
  *            @arg TIM_EVENTSOURCE_CC2: Timer Capture Compare 2 Event source
  *            @arg TIM_EVENTSOURCE_CC3: Timer Capture Compare 3 Event source
  *            @arg TIM_EVENTSOURCE_CC4: Timer Capture Compare 4 Event source
  *            @arg TIM_EVENTSOURCE_COM: Timer COM event source
  *            @arg TIM_EVENTSOURCE_TRIGGER: Timer Trigger Event source
  *            @arg TIM_EVENTSOURCE_BREAK: Timer Break event source
  * @note   TIM6 and TIM7 can only generate an update event.
  * @note   TIM_EVENTSOURCE_COM and TIM_EVENTSOURCE_BREAK are used only with TIM1 and TIM8.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_GenerateEvent(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t eventSource);

/**
  * @brief  Read the captured value from Capture Compare unit
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  Channel: TIM Channels to be enabled.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  * @retval Captured value
  */
uint32_t HAL_TIM_ReadCapturedValue(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel);

/********************************************** Extension Control functions  ************************************************/

/**
  * @brief  Time Base configuration
  * @param  TIMx: TIM peripheral
  * @param  Structure: pointer on TIM Time Base required parameters
  * @retval None
  */
void HAL_TIM_TimeBaseConfig(TIM_Device_T *TIMx, TIM_TimeBaseInit_T *structure);

/**
  * @brief  Enables or disables the TIM Capture Compare Channel x.
  * @param  TIMx to select the TIM peripheral
  * @param  Channel: specifies the TIM Channel
  *          This parameter can be one of the following values:
  *            @arg TIM_Channel_1: TIM Channel 1
  *            @arg TIM_Channel_2: TIM Channel 2
  *            @arg TIM_Channel_3: TIM Channel 3
  *            @arg TIM_Channel_4: TIM Channel 4
  * @param  ChannelState: specifies the TIM Channel CCxE bit new state.
  *          This parameter can be: TIM_CCx_ENABLE or TIM_CCx_Disable.
  * @retval None
  */
void HAL_TIM_CCxChannelCmd(TIM_Device_T *TIMx, uint32_t channel, uint32_t channelState);

/**
  * @brief  Enables or disables the TIM Capture Compare Channel xN.
  * @param  TIMx to select the TIM peripheral
  * @param  Channel: specifies the TIM Channel
  *          This parameter can be one of the following values:
  *            @arg TIM_Channel_1: TIM Channel 1
  *            @arg TIM_Channel_2: TIM Channel 2
  *            @arg TIM_Channel_3: TIM Channel 3
  * @param  ChannelNState: specifies the TIM Channel CCxNE bit new state.
  *          This parameter can be: TIM_CCxN_ENABLE or TIM_CCxN_Disable.
  * @retval None
  */
void HAL_TIM_CCxNChannelCmd(TIM_Device_T *TIMx, uint32_t channel, uint32_t channelNState);

/**
  * @brief  Configure the TI1/2/3/4 as Input.
  * @param  TIMx to select the TIM peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPolarity_Rising
  *            @arg TIM_ICPolarity_Falling
  *            @arg TIM_ICPolarity_BothEdge
  * @param  TIM_ICSelection: specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICSelection_DirectTI: TIM Input 1 is selected to be connected to IC1.
  *            @arg TIM_ICSelection_IndirectTI: TIM Input 1 is selected to be connected to IC2.
  *            @arg TIM_ICSelection_TRC: TIM Input 1 is selected to be connected to TRC.
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @note TIM_ICFilter and TIM_ICPolarity are not used in INDIRECT mode as TI2FP1
  *       (on channel2 path) is used as the input signal. Therefore CCMR1 must be
  *        protected against un-initialized filter and polarity values.
  * @retval None
  */
void HAL_TIM_TIConfig(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter, uint32_t channel);

/**
  * @brief  Configure the Polarity and Filter for TI1/2.
  * @param  TIMx to select the TIM peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_ICPolarity_Rising
  *            @arg TIM_ICPolarity_Falling
  *            @arg TIM_ICPolarity_BothEdge
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
void HAL_TIM_TIInputStageConfig(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICFilter, uint32_t channel);

/**
  * @brief  Selects the Input Trigger source
  * @param  TIMx to select the TIM peripheral
  * @param  TIM_ITRx: The Input Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM_TS_ITR0: Internal Trigger 0
  *            @arg TIM_TS_ITR1: Internal Trigger 1
  *            @arg TIM_TS_ITR2: Internal Trigger 2
  *            @arg TIM_TS_ITR3: Internal Trigger 3
  *            @arg TIM_TS_TI1F_ED: TI1 Edge Detector
  *            @arg TIM_TS_TI1FP1: Filtered Timer Input 1
  *            @arg TIM_TS_TI2FP2: Filtered Timer Input 2
  *            @arg TIM_TS_ETRF: External Trigger input
  * @retval None
  */
void HAL_TIM_ITRxConfig(TIM_Device_T *TIMx, uint16_t TIM_ITRx);

/**
  * @brief  Configures the TIMx External Trigger (ETR).
  * @param  TIMx to select the TIM peripheral
  * @param  TIM_ExtTRGPrescaler: The external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM_ETRPRESCALER_DIV1 : ETRP Prescaler OFF.
  *            @arg TIM_ETRPRESCALER_DIV2 : ETRP frequency divided by 2.
  *            @arg TIM_ETRPRESCALER_DIV4 : ETRP frequency divided by 4.
  *            @arg TIM_ETRPRESCALER_DIV8 : ETRP frequency divided by 8.
  * @param  TIM_ExtTRGPolarity: The external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_ETRPOLARITY_INVERTED : active low or falling edge active.
  *            @arg TIM_ETRPOLARITY_NONINVERTED : active high or rising edge active.
  * @param  ExtTRGFilter: External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void HAL_TIM_ETRConfig(TIM_Device_T *TIMx, uint32_t TIM_ExtTRGPrescaler, uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

/**
  * @brief  Configure the TIM commutation event sequence with interrupt.
  * @note  This function is mandatory to use the commutation event in order to
  *        update the configuration at each commutation detection on the TRGI input of the Timer,
  *        the typical use of this feature is with the use of another Timer(interface Timer)
  *        configured in Hall sensor interface, this interface Timer will generate the
  *        commutation at its TRGO output (connected to Timer used in this function) each time
  *        the TI1 of the Interface Timer detect a commutation at its input TI1.
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  * @param  InputTrigger: the Internal trigger corresponding to the Timer Interfacing with the Hall sensor.
  *          This parameter can be one of the following values:
  *            @arg TIM_TS_ITR0: Internal trigger 0 selected
  *            @arg TIM_TS_ITR1: Internal trigger 1 selected
  *            @arg TIM_TS_ITR2: Internal trigger 2 selected
  *            @arg TIM_TS_ITR3: Internal trigger 3 selected
  *            @arg TIM_TS_NONE: No trigger is needed
  * @param  CommutationSource: the Commutation Event source.
  *          This parameter can be one of the following values:
  *            @arg TIM_COMMUTATION_TRGI: Commutation source is the TRGI of the Interface Timer
  *            @arg TIM_COMMUTATION_SOFTWARE:  Commutation source is set by software using the COMG bit
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigCOMEventInterrupt(TIM_Handle_T *htim, TIM_Device_T *TIMx,
        uint32_t inputTrigger, uint32_t commutationSource);

/**
  * @brief   Configures the Break feature, dead time, Lock level, OSSI/OSSR State
  *         and the AOE(automatic output enable).
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sBreakDeadTimeConfig: pointer to a TIM_ConfigBreakDeadConfig_TypeDef structure that
  *         contains the BDTR Register configuration  information for the TIM peripheral.
  * @retval RET_OK indicates success; Else indicates failure.
  */
RET_CODE HAL_TIM_ConfigBreakDeadTime(TIM_Handle_T *htim, TIM_Device_T *TIMx,
                                     TIM_BreakDeadTimeConfig_T *sBreakDeadTimeConfig);

/**
  * @brief  Configures pwm duty value.
  * @param  TIMx to select the TIM peripheral.
  * @param  apre: whether enable auto-reload preload.
  * @param  duty: pwm duty value configuration.
  * @retval RET_OK indicates success; Else indicates failure.
  */
void TIMx_SetPWMDuty(TIM_Device_T *TIMx, uint8_t apre, uint16_t duty);


/**
  * @brief   Configures the DMA transmit parameter.
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @param  sDmaConfig: pointer to a TIM_DmaConfig_T structure that
  *         contains the BDTR Register configuration  information for the TIM peripheral.
  * @retval RET_OK indicates success; Else indicates failure.
  */

RET_CODE HAL_TIM_ConfigDmaReg(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_DmaConfig_T *sDmaConfig);

/********************************* Callback in non blocking modes (Interrupt and DMA) *************************/

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
  * @brief  Output Compare callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_OCDelayElapsedCallback(TIM_Handle_T *htim);

/**
  * @brief  Input Capture callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_ICCaptureCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
  * @brief  PWM Input callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_PWMInputCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
  * @brief  PWM Pulse finished callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_PWMPulseFinishedCallback(TIM_Handle_T *htim);

/**
  * @brief  Hall Trigger detection callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_TriggerCallback(TIM_Handle_T *htim);

/**
  * @brief  Hall commutation changed callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_CommutationCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
  * @brief  Hall Break detection callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_BreakCallback(TIM_Handle_T *htim);

/**
  * @brief  Timer error callback in non blocking mode
  * @param  htim: pointer to a TIM_Handle_T structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_ErrorCallback(TIM_Handle_T *htim);

/**
  * @brief  This function handles TIM interrupts requests.
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_IRQ_handler(TIM_Handle_T *htim, TIM_Device_T *TIMx);

/**
  * @brief  This function handles TIM interrupts requests.
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
 * @retval None
  */
void HAL_TIM1_InterruptHandler(void);
void HAL_TIM2_InterruptHandler(void);
void HAL_TIM3_InterruptHandler(void);
void HAL_TIM4_InterruptHandler(void);
void HAL_TIM5_InterruptHandler(void);
void HAL_TIM8_InterruptHandler(void);
void HAL_TIM9_InterruptHandler(void);
void HAL_TIM10_InterruptHandler(void);
void HAL_TIM11_InterruptHandler(void);
void HAL_TIM12_InterruptHandler(void);
void HAL_TIM13_InterruptHandler(void);
void HAL_TIM14_InterruptHandler(void);

void HAL_BASETIMER1_InterruptHandler(void);
void HAL_BASETIMER2_InterruptHandler(void);

void HAL_TIM_OC1SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig);
void HAL_TIM_OC2SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig);
void HAL_TIM_OC3SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig);
void HAL_TIM_OC4SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig);

void TIM_InterruptConfig(TIM_Handle_T *htim);

void HAL_TIM_SW_Reset(void);
void HAL_BTM_SW_Reset(void);
void HAL_TIM_RequestIrq(int tim_sel);
void HAL_BTM_RequestIrq(int btm_sel);
void HAL_TIM_CLK_ENABLE(int tim_sel);
void HAL_TIM_CLK_DISABLE(int tim_sel);
void HAL_BTM_CLK_ENABLE(int btm_sel);
void HAL_BTM_CLK_DISABLE(int btm_sel);


#ifdef __cplusplus
}
#endif

#endif /* _GM_HAL_TIM_H_ */



