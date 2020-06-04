/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file 		 gm_hal_pes.c
*
* @author		 Christal.Xu
*
* @version		 1.1.0
*
* @date 		 2019/05/05
*
* @brief		 Header file of pes driver.
*
* @note
*	  2017/05/19, Mike.Zheng, V1.0.0
*	  2019/05/05, Christal.Xu, V1.1.0
*		 Initial version.
*/

#ifndef _GM_HAL_PES_H_
#define _GM_HAL_PES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_gpio.h>


/**
 *  pes work channel define
 */
typedef enum
{
    PES_EVENT_CHANNEL0          = 0x00,            /**< pes channel0 */
    PES_EVENT_CHANNEL1                ,            /**< pes channel1 */
    PES_EVENT_CHANNEL2                ,            /**< pes channel2 */
    PES_EVENT_CHANNEL3                ,            /**< pes channel3 */
    PES_EVENT_CHANNEL4                ,            /**< pes channel4 */
    PES_EVENT_CHANNEL5                ,            /**< pes channel5 */
    PES_EVENT_CHANNEL6                ,            /**< pes channel6 */
    PES_EVENT_CHANNEL7                ,            /**< pes channel7 */
    PES_EVENT_CHANNEL8                ,            /**< pes channel8 */
    PES_EVENT_CHANNEL9                ,            /**< pes channel9 */
    PES_EVENT_CHANNEL10               ,            /**< pes channel10 */
    PES_EVENT_CHANNEL11               ,            /**< pes channel11 */
    PES_INVAILD_CHANNEL
}PES_ChannelIndex_T;


/**
 *  pes output event wave define
 */
typedef enum
{

    PES_SYNC_BYPASS             = 0x00,             /**< pes channel output event wave keep the same as
                                                     *   master output event wave,
                                                     *   becare: this mode is invaild in deepsleep mode,
                                                     *   but vaild in normal mode
                                                     */

    PES_SYNC_RISING_EDGE             ,              /**< pes channel output is a pulse when receive rasing
                                                     *   edge from pes master source becare: this mode is invaild
                                                     *   in deepsleep mode, but vaild in normal mode
                                                     */

    PES_SYNC_FALLING_EDGE            ,              /**< pes channel output is a pulse when receive falling edge
                                                     *   from pes master source, becare:this mode is invaild in
                                                     *   deepsleep mode, but vaild in normal mode
                                                     */


    PES_SYNC_RISINGF_FALLING_EDGE    ,              /**< pes channel output is a pulse when receive both rasing
                                                     *   or falling edge from pes master source becare: this mode
                                                     *   is invaild in deepsleep mode, but vaild in normal mode
                                                     */

    PES_ASYNC                                       /**< pes channel output event wave keep the same as master
                                                     *   output event wave,and not sync clk this mode is vaild at
                                                     *   both normal and deepsleep mode
                                                     */
}PES_TrigerSlaveSyncMode_T;

/**
 *  pes master source IP define
 */
typedef enum
{
    PES_MASTER_PESOUT                       = 0x00,	/**< pes channel output event as master */
    PES_MASTER_CPU                                ,	/**< cpu as pes channel master */
    PES_MASTER_DMA                                ,	/**< dma as pes channel master */
    PES_MASTER_IO                                 , /**< io as pes channel master */
    PES_MASTER_TIM1                               , /**< tim1 as pes channel master */
    PES_MASTER_ACMP                               , /**< acmp as pes channel master */
    PES_MASTER_ADC                                , /**< adc as pes channel master */
    PES_MASTER_RTCC                               , /**< rtcc as pes channel master */
    PES_MASTER_TIM2                               , /**< tim2 as pes channel master */
    PES_MASTER_TIM3                               , /**< tim3 as pes channel master */
    PES_MASTER_TIM4                               , /**< tim4 as pes channel master */
    PES_MASTER_TIM5                               , /**< tim5 as pes channel master */
    PES_MASTER_USB                                , /**< usb as pes channel master */
    PES_MASTER_BTM2                               , /**< btm2 as pes channel master */
    PES_MASTER_TIM8                               , /**< tim8 as pes channel master */
}PES_MasterSourceConfig_T;

/**
 *  acmp as master generate pes event mode define
 */
typedef enum
{
    PES_ACMP0_OUTPUT                        = 0x00, /**< acmp as master generate event from acmp0 output */
    PES_ACMP1_OUTPUT                                /**< acmp as master generate event from acmp1 output */
}PES_MasterAcmpTrigerOutput_T;

/**
 *  pes channel as master generate pes event mode define
 */
typedef enum
{
    PES_OUTPUT_EVENT_FROM_CHANNEL0          = 0x00, /**< pes channel0 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL1                , /**< pes channel1 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL2                , /**< pes channel2 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL3                , /**< pes channel3 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL4                , /**< pes channel4 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL5                , /**< pes channel5 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL6                , /**< pes channel6 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL7                , /**< pes channel7 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL8                , /**< pes channel8 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL9                , /**< pes channel9 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL10               , /**< pes channel10 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL11               , /**< pes channel11 generate event */
}PES_MasterPesEventTrigerOutput_T;

/**
 *  io as master generate pes event mode define
 */
typedef enum
{
    IO_PIN_PORT_0                             = 0x00, /**< pes IO pin num 0 for current port */
    IO_PIN_PORT_1                                   , /**< pes IO pin num 1 for current port */
    IO_PIN_PORT_2                                   , /**< pes IO pin num 2 for current port */
    IO_PIN_PORT_3                                 	, /**< pes IO pin num 3 for current port */
    IO_PIN_PORT_4                                 	, /**< pes IO pin num 4 for current port */
    IO_PIN_PORT_5                                  	, /**< pes IO pin num 5 for current port */
    IO_PIN_PORT_6                                 	, /**< pes IO pin num 6 for current port */
    IO_PIN_PORT_7                                   , /**< pes IO pin num 7 for current port */
    IO_PIN_PORT_8                                   , /**< pes IO pin num 8 for current port */
    IO_PIN_PORT_9                                   , /**< pes IO pin num 9 for current port */
    IO_PIN_PORT_10                                  , /**< pes IO pin num 10 for current port */
    IO_PIN_PORT_11                                  , /**< pes IO pin num 11 for current port */
    IO_PIN_PORT_12                                  , /**< pes IO pin num 12 for current port */
    IO_PIN_PORT_13                                  , /**< pes IO pin num 13 for current port */
    IO_PIN_PORT_14                                  , /**< pes IO pin num 14 for current port */
    IO_PIN_PORT_15                                  , /**< pes IO pin num 15 for current port */
}PES_MasterIOPinTrigerOutput_T;

/**
 *  adc as master generate pes event mode define
 */
typedef enum
{
    PES_ADC1_EOC                             = 0x00, /**< adc1 as master generate event when adc1 eoc intrupter generate */
    PES_ADC1_EOS                                    , /**< adc1 as master generate event when adc1 eos intrupter generate */
    PES_ADC1_JEOC                             		, /**< adc1 as master generate event when adc1 jeoc intrupter generate */
    PES_ADC1_JEOS                             		, /**< adc1 as master generate event when adc1 jeoc intrupter generate */
    PES_ADC1_COMP_MATCH                             , /**< adc1 as master generate event when adc1 compare match intrupter generate */
	PES_ADC2_EOC                             		, /**< adc2 as master generate event when adc2 eoc intrupter generate */
    PES_ADC2_EOS                                    , /**< adc2 as master generate event when adc2 eos intrupter generate */
    PES_ADC2_JEOC                             		, /**< adc2 as master generate event when adc2 jeoc intrupter generate */
    PES_ADC2_JEOS                             		, /**< adc2 as master generate event when adc2 jeoc intrupter generate */
    PES_ADC2_COMP_MATCH                             , /**< adc2 as master generate event when adc2 compare match intrupter generate */
	PES_ADC3_EOC                             		, /**< adc3 as master generate event when adc3 eoc intrupter generate */
    PES_ADC3_EOS                                    , /**< adc3 as master generate event when adc3 eos intrupter generate */
    PES_ADC3_JEOC                             		, /**< adc3 as master generate event when adc3 jeoc intrupter generate */
    PES_ADC3_JEOS                             		, /**< adc3 as master generate event when adc3 jeoc intrupter generate */
    PES_ADC3_COMP_MATCH                             , /**< adc3 as master generate event when adc3 compare match intrupter generate */
}PES_MasterAdcTrigerOutput_T;

/**
 *  tim as master generate pes event mode define
 */
typedef enum
{
    PES_TIM_CH1                             = 0x00, /**< adc1 as master generate event when adc1 eoc intrupter generate */
    PES_TIM_CH2                                    , /**< adc1 as master generate event when adc1 eos intrupter generate */
    PES_TIM_CH3                             		, /**< adc1 as master generate event when adc1 jeoc intrupter generate */
    PES_TIM_CH4                             		, /**< adc1 as master generate event when adc1 jeoc intrupter generate */
    PES_TIM_TRGO                             , /**< adc1 as master generate event when adc1 compare match intrupter generate */
}PES_MasterTimTrigerOutput_T;

/**
 *   software event generate wave define
 */
typedef enum
{
    PES_LOW_LEVEL                           = 0x00, /**< cpu as master send low level to pes channel */
    PES_HIGH_LEVEL                                , /**< cpu as master send high level to pes channel */
    PES_PULSE                                       /**< cpu as master send pulse to pes channel */
}PES_MasterCpuTriger_T;

/**
 *  detect mode define
 */
typedef enum
{
    PES_FALLING_EDGE                           = 0x00, /**< detect falling edge */
    PES_RISING_EDGE                                , /**< detect rising edge */
    PES_DET_DISABLE                                       /**< detect disable */
}PES_DetectEdge_T;

/**
 *  pes channel IO event direction define
 */
typedef enum
{
    IO_RECEIVE_EVENT                           = 0x00, /**< io as master send event to pes channel */
    IO_SEND_EVENT                              , /**< io as slave to receive event from pes channel */
}PES_IoDir_T;

/**
 *  PES IO as master or slave define
 */
typedef enum
{
    IO_MASTER                           = 0x00, /**< io as master send event to pes channel */
    IO_SLAVE                              , /**< io as slave to receive event from pes channel */
}PES_Io_T;

/**
 * master triger output mode union
 */
typedef union
{
	PES_MasterPesEventTrigerOutput_T      	pesChannelOutput; /**< pes channel output as master generate pes mode enum */
    PES_MasterIOPinTrigerOutput_T         	ioPinOutput;      /**< io as master generate pes mode enum */
    PES_MasterAdcTrigerOutput_T           	adcOutput;        /**< adc as master generate pes mode enum */
    PES_MasterTimTrigerOutput_T				timOutput;		  /**< tim as master generate pes mode enum */
} PES_MasterEventSourceConfig_T;


/**
 * pes channel cascade struct
 */
typedef struct
{
    uint8_t         andNextChannelEnable; /**< enable/disable:pes channel output wave & with next pes channel output wave function */
    uint8_t         orpreChannelEnable;   /**< enable/disable:pes channel output wave | with pre pes channel output wave function */
} PES_ChannelCascadeEnConfig_T;

/**
 * pes event channel config struct
 */
typedef struct
{
	PES_MasterSourceConfig_T              masterSrcConfig;       /**< master src config */
	PES_MasterEventSourceConfig_T		  masterEvtSrcConfig;	 /**< master generate event mode */
	PES_DetectEdge_T					  detectMode;			 /**< enable/disable:detect, and select the detect edge */
	uint8_t                               channelOutputInverted; /**< enable/disable:pes channel output wave inverted */
    PES_ChannelCascadeEnConfig_T          cascadeConfig;         /**< cascade Config */
	PES_TrigerSlaveSyncMode_T             syncMode;              /**< slave reveive event sync mode */
} PES_ChannelConfig_T;

/**
 * @brief request pes channel.
 *
 * This function is used to request a free pes channel.
 *
 * @param  void
 *
 * @retval PES_ChannelIndex_T free pes channel Index
 */
PES_ChannelIndex_T HAL_PES_RequestEventChannel(void);

/**
 * @brief release event channel
 *
 * This function is used to release event channel
 *
 * @param chIndex    The return value get from PES_RequestEventChannel
 *
 * @retval RET_BUSY  PES lock is not get, the resource is locked by others
 * @retval RET_OK    PES release event channel OK
 */
RET_CODE HAL_PES_ReleaseEventChannel(PES_ChannelIndex_T chIndex);

/**
 * @brief config pes event channel
 *
 * This function is used to config pes event channel
 *
 * @param  pchCfg         PES channel config parameter,please refer to PES_ChannelConfig_T for detail
 * @param  chIndex        PES channel index
 *
 * @retval RET_INVPARAM   Input parameter is error
 * @retval RET_OK         PES config event channel OK
 */
RET_CODE HAL_PES_ConfigEventChannel(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex);

/**
 * @brief config pes function for PES Event to CPU Register
 *
 * This function is used to config pes function for PES Event to CPU Registe
 *
 * @param pchCfg          PES channel config par,please refer PES_ChannelConfig_T for detail
 * @param chIndex         PES channel Index
 *
 * @retval RET_OK         PES config event to cpu reg ok
 */
RET_CODE  HAL_PES_EventToCpuRegConfig(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex);


/**
 * @brief generate software event
 *
 * This function is used when generate event to channel by config software event register
 *
 * @param trigerMode     PES event to channel by software mode
 * @param chIndex        PES channel Index
 *
 * @retval               None
 */
RET_CODE HAL_PES_GenerateSWTrigerEvent(PES_MasterCpuTriger_T trigerMode, PES_ChannelIndex_T chIndex);

/**
 * @brief io as slave config
 *
 * This function is used to config PES master source and master event source 
 *
 * @param masterSource     PES master source selected
 * @param pmasterTrigerOutput        PES event source of master source selected
 * @param chIndex        PES channel Index
 *
 * @retval RET_INVPARAM  Input parameter is error
 * @retval RET_OK        PES master config ok
 */
RET_CODE HAL_PES_MasterConfig(PES_MasterSourceConfig_T masterSource, PES_MasterEventSourceConfig_T *pmasterTrigerOutput, 
											PES_ChannelIndex_T chIndex); 

/**
 * @brief config Interrupt Enable and Status Register
 *
 * This function is used to config Interrupt Enable and Status Register
 *
 * @param interruptEn     PES interrupt enable selected
 * @param chIndex        PES channel Index
 *
 * @retval none
 */
RET_CODE HAL_PES_ConfigIEandStatusReg(uint8_t interruptEn,PES_ChannelIndex_T chIndex);

/**
 * @brief IO pinmux config 
 *
 * This function is used to config IO as master mode or slave mode 
 *
 * @param bank     IO bank select
 * @param pinnum       IO pin number select 
 * @param mode        IO as master mode or slave mode 
 *
 * @retval RET_OK	IO config ok
 * @retval RET_INVPARAM  Input parameter is error
 */
RET_CODE HAL_PES_IOPinmuxConfig(GPIO_Device_T *bank, uint32_t pinnum,  PES_Io_T mode);

/**
 * @brief config interrupt enable
 *
 * This function is used to config interrupt enable
 *
 * @param interruptEn    interrupt enable or disable
 *
 * @retval none
 */
RET_CODE HAL_PES_ConfigInterruptEN(uint8_t interruptEn);

/**get interrupt status
 *
 * This function is used to get interrupt status
 *
 * @param chIndex        PES channel Index
 *
 * @retval none
 */
RET_CODE HAL_PES_GetIrqStatus(PES_ChannelIndex_T chIndex);


#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_PES_H_*/



















