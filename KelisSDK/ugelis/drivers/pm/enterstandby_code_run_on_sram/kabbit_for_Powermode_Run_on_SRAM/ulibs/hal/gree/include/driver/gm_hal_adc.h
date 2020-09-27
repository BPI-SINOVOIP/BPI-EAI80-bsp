/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_adc.h
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_HAL_ADC_H_
#define _GM_HAL_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <adc/gm_ll_adc_dev.h>
#include <adc/gm_ll_adc.h>

#define  SEQUECE_LEN_MAX  16
#define MAX_ADC_DEVICE   3

/**
 * Read ADC convert mode: block read or noblock read
 */
typedef enum
{
    NOBLOCK = 1,            /**< Noblock to read ADC convert value, function will return immediately */
    BLOCK,                  /**< Block to read ADC convert value, function will not return until convert finish */
} ADC_BLOCKSTATUS_T;


/**
 * ADC Convert Data Fifo Prefull Num
 */
typedef enum
{
    ADC_DFIFO_PRE_FULL_NUM_1                  = 0x0,  /**< There is 1 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_2,                         /**< There is 2 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_3,                         /**< There is 3 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_4,                         /**< There is 4 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_5,                         /**< There is 5 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_6,                         /**< There is 6 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_7,                         /**< There is 7 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_8                          /**< There is 8 data in data fifo generate prefull event */
} ADC_DfifoPrefullNum_T;

typedef enum
{
    ADC_INDPENDENT_MODE                = 0x0,  /**< Independent mode*/
    ADC_COMBINDREGSMUINJ_1,                           /**< Combined regular simultaneous + injected simultaneous mode */
    ADC_COMBINDREGSMUALT_NUM_2,                       /**< Combined regular simultaneous + alternate trigger mode */
    ADC_RESERVED_3,                                   /**< Reserved*/
    ADC_INJSMUMODEONLY_4,                             /**<  Injected simultaneous mode only */
    ADC_REGSMUMODEONLY_5,                             /**< Regular simultaneous mode only */
    ADC_INTERLEAVEDMODEONLY_NUM_6,                    /**< interleaved mode only */
    ADC_ALTTRIGMODEONLY_7                          /**< Alternate trigger mode only */
} ADC_MultiMode_T;


typedef enum
{
    ADC_DUAL                      = 0x0,  /**< Dual ADC mode*/
    ADC_TRIPLE,                           /**< Triple ADC mode */
} ADC_MULTI_NUM_MODE_T;

typedef enum
{
    ADC_INTER_DELAY_0 = 0,                            /**<5 analog circuit clock cycles*/
    ADC_INTER_DELAY_1,                          /**<6 analog circuit clock cycles*/
    ADC_INTER_DELAY_2,                           /**<7 analog circuit clock cycles*/
    ADC_INTER_DELAY_3,                           /**<8 analog circuit clock cycles*/
    ADC_INTER_DELAY_4,                          /**<9 analog circuit clock cycles*/
    ADC_INTER_DELAY_5,                         /**<10 analog circuit clock cycles*/
    ADC_INTER_DELAY_6,                            /**<11 analog circuit clock cycles*/
    ADC_INTER_DELAY_7,                          /**<12 analog circuit clock cycles*/
    ADC_INTER_DELAY_8,                         /**<13 analog circuit clock cycles*/
    ADC_INTER_DELAY_9,                            /**<14 analog circuit clock cycles*/
    ADC_INTER_DELAY_10,                           /**<15 analog circuit clock cycles*/
    ADC_INTER_DELAY_11,                           /**<16 analog circuit clock cycles*/
    ADC_INTER_DELAY_12,                            /**<17 analog circuit clock cycles*/
    ADC_INTER_DELAY_13,                          /**<18 analog circuit clock cycles*/
    ADC_INTER_DELAY_14,                            /**<19 analog circuit clock cycles*/
    ADC_INTER_DELAY_15,                            /**<20 analog circuit clock cycles*/
} ADC_INTER_DELAY_T;


/**
 * adc compare match mode
 */
typedef enum
{
    ADC_COMPARE_MORE_THAN                  = 0x00,  /**< When convert value more than compareHighValue will send a event to usr */
    ADC_COMPARE_EQUAL_TO,                           /**< When convert equal compareHighValue will send a event to usr */
    ADC_COMPARE_LESS_THAN,                          /**< When convert value less than compareHighValue will send a event to usr */
    ADC_COMPARE_WITHIN_RANGE                        /**< When convert value between compareLowValue and compareHighValue will send a event to usr */
} ADC_CompareMode_T;

/**
 * ADC triger mode
 */
typedef enum
{
    ADC_SINGLE_MODE                        = 0x01,    /**< Need sw triger to start convet everytime */
    ADC_REPEAT_MODE                        = 0x03     /**< ADC auto to start convert after First convert finish */
} ADC_ContinousMode_T;

/**
 * ADC resolution
 */
typedef enum
{
    ADC_RESOLUTION_12BIT                   = 0x00,   /**< Resolution of analog to digital conversion is 12bit */
    ADC_RESOLUTION_10BIT,                            /**< Resolution of analog to digital conversion is 10bit */
    ADC_RESOLUTION_8BIT                              /**< Resolution of analog to digital conversion is 8bit */
} ADC_Resolution_T;

/**
 * ADC sample time
 */
typedef enum
{
    ADC_SAMPLETIME_3CYCLES                    = 0x00,  /**< Convert sample time is 3 adcclk */
    ADC_SAMPLETIME_4CYCLES,                            /**< Convert ample timeis 4 adcclk */
    ADC_SAMPLETIME_6CYCLES,                            /**< Convert sample timeis 6 adcclk */
    ADC_SAMPLETIME_8CYCLES,                           /**< Convert sample time is 8 adcclk */
    ADC_SAMPLETIME_12CYCLES,                          /**< Convert sample time is 12 adcclk */
    ADC_SAMPLETIME_16CYCLES,                           /**< Convert ample timeis 16 adcclk */
    ADC_SAMPLETIME_24CYCLES,                           /**< Convert sample timeis 24 adcclk */
    ADC_SAMPLETIME_32CYCLES,                            /**< Convert sample time is 32 adcclk */
    ADC_SAMPLETIME_48CYCLES,                          /**< Convert ample timeis 48 adcclk */
    ADC_SAMPLETIME_64CYCLES,                          /**< Convert sample timeis 64 adcclk */
    ADC_SAMPLETIME_96CYCLES,                          /**< Convert sample time is 96 adcclk */
    ADC_SAMPLETIME_128CYCLES,                          /**< Convert sample time is 128 adcclk */
    ADC_SAMPLETIME_192CYCLES,                          /**< Convert ample timeis 192 adcclk */
    ADC_SAMPLETIME_256CYCLES,                        /**< Convert sample timeis 256 adcclk */
    ADC_SAMPLETIME_384CYCLES,                         /**< Convert sample time is 384 adcclk */
    ADC_SAMPLETIME_512CYCLES,                         /**< Convert sample time is 512 adcclk */
} ADC_SampleTime_T;

/**
 * ADC sample time
 */
typedef enum
{
    ADC_SAMPLE_POSITIVE                    = 0x00,  /**< Sample at positive edge of ADC clock */
    ADC_SAMPLE_NEGATIVE                             /**< Sample at negative edge of ADC clock */
} ADC_SampleEdge_T;

/**
 * ADC triger source
 */
typedef enum
{
    ADC_HW_JTRIGER_PESCH0                    = 0x0,    /**< Set PES channel 0 as hw triger source */
    ADC_HW_JTRIGER_PESCH1,                             /**< Set PES channel 1 as hw triger source */
    ADC_HW_JTRIGER_PESCH2,                             /**< Set PES channel 2 as hw triger source */
    ADC_HW_JTRIGER_PESCH3,                             /**< Set PES channel 3 as hw triger source */
    ADC_HW_JTRIGER_PESCH4,                             /**< Set PES channel 4 as hw triger source */
    ADC_HW_JTRIGER_PESCH5,                             /**< Set PES channel 5 as hw triger source */
    ADC_HW_JTRIGER_PESCH6,                             /**< Set PES channel 6 as hw triger source */
    ADC_HW_JTRIGER_PESCH7,                             /**< Set PES channel 7 as hw triger source */
    ADC_HW_JTRIGER_PESCH8,                             /**< Set PES channel 8 as hw triger source */
    ADC_HW_JTRIGER_PESCH9,                             /**< Set PES channel 9 as hw triger source */
    ADC_HW_JTRIGER_PESCH10,                            /**< Set PES channel 10 as hw triger source */
    ADC_HW_JTRIGER_PESCH11,                            /**< Set PES channel 11 as hw triger source*/
} ADC_JHTrigerSource_T;
/**
 * ADC triger source
 */
typedef enum
{
    ADC_HW_TRIGER_PESCH0                    = 0x0,    /**< Set PES channel 0 as hw triger source */
    ADC_HW_TRIGER_PESCH1,                             /**< Set PES channel 1 as hw triger source */
    ADC_HW_TRIGER_PESCH2,                             /**< Set PES channel 2 as hw triger source */
    ADC_HW_TRIGER_PESCH3,                             /**< Set PES channel 3 as hw triger source */
    ADC_HW_TRIGER_PESCH4,                             /**< Set PES channel 4 as hw triger source */
    ADC_HW_TRIGER_PESCH5,                             /**< Set PES channel 5 as hw triger source */
    ADC_HW_TRIGER_PESCH6,                             /**< Set PES channel 6 as hw triger source */
    ADC_HW_TRIGER_PESCH7,                             /**< Set PES channel 7 as hw triger source */
    ADC_HW_TRIGER_PESCH8,                             /**< Set PES channel 8 as hw triger source */
    ADC_HW_TRIGER_PESCH9,                             /**< Set PES channel 9 as hw triger source */
    ADC_HW_TRIGER_PESCH10,                            /**< Set PES channel 10 as hw triger source */
    ADC_HW_TRIGER_PESCH11,                            /**< Set PES channel 11 as hw triger source*/
    ADC_HW_TRIGER_NONE,
} ADC_TrigerSource_T;

/**
 * ADC channel index
 */
typedef enum
{
    ADC_CHANNEL_0                          = 0x00,  /**< ADC sample channel num index is 0 */
    ADC_CHANNEL_1,                                  /**< ADC sample channel num index is 1 */
    ADC_CHANNEL_2,                                  /**< ADC sample channel num index is 2 */
    ADC_CHANNEL_3,                                  /**< ADC sample channel num index is 3 */
    ADC_CHANNEL_4,                                  /**< ADC sample channel num index is 4 */
    ADC_CHANNEL_5,                                  /**< ADC sample channel num index is 5 */
    ADC_CHANNEL_6,                                  /**< ADC sample channel num index is 6 */
    ADC_CHANNEL_7,                                  /**< ADC sample channel num index is 7 */
    ADC_CHANNEL_8,                                  /**< ADC sample channel num index is 8 */
    ADC_CHANNEL_9,                                  /**< ADC sample channel num index is 9 */
    ADC_CHANNEL_10,                                 /**< ADC sample channel num index is 10 */
    ADC_CHANNEL_11,                                 /**< ADC sample channel num index is 11 */
    ADC_CHANNEL_12,                                 /**< ADC sample channel num index is 12 */
    ADC_CHANNEL_13,                                 /**< ADC sample channel num index is 13 */
    ADC_CHANNEL_14,                                 /**< ADC sample channel num index is 14 */
    ADC_CHANNEL_15,                                 /**< ADC sample channel num index is 15 */
} ADC_ChannelIndex_T;

/**
 * ADC  triger mode, for GM6621
 */
typedef enum
{
    ADC_SW_TRIGER                          = 0x0,    /**< ADC software triger mode */
    ADC_HW_TRIGER                                    /**< ADC hardware triger mode */
} ADC_TrigerMode_T;

/**
 * ADC  triger mode, for GM6621
 */
typedef enum
{
    ADC_SW_REGU_TRIGER                          = 0x2,    /**< ADC Start a conversion on  regular  channel */
    ADC_SW_JTRIG_TRIGER                                    /**< ADC Start a conversion on injected channel */
} ADC_TrigerChannel_T;


/**
 * Urs revive event type from driver
 */
typedef enum
{
    ADC_EVENT_COMPERE_MATCH                = 0x0,  /**< When convert value match the compare value will send this event to usr */
    ADC_EVENT_OVERFLOW,                            /**< When convet data fifo overflow will send ths event to usr */
    ADC_EVENT_FINISHED,                            /**< When convet finish overflow will send ths event to usr */
    ADC_EVENT_CONVERT_ERROR
} ADC_Event_T;


/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
    HAL_OK       = 0x00,
    HAL_ERROR    = 0x01,
    HAL_BUSY     = 0x02,
    HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

/** @defgroup ADC_Error_Code ADC Error Code
  * @{
  */
#define HAL_ADC_ERROR_NONE        ((uint32_t)0x00)   /*!< No error             */
#define HAL_ADC_ERROR_OVR         ((uint32_t)0x01)   /*!< OVR error            */
#define HAL_ADC_ERROR_DMA         ((uint32_t)0x02)   /*!< DMA transfer error   */
/**
  * @}
  */


/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
    HAL_ADC_UNLOCKED = 0x00,
    HAL_ADC_LOCKED   = 0x01
} HAL_LockTypeDef;


/**
  * @brief HAL State structures definition
  */
typedef enum
{
    HAL_ADC_STATE_RESET                   = 0x00,    /*!< ADC not yet initialized or disabled */
    HAL_ADC_STATE_READY,                       /*!< ADC peripheral ready for use */
    HAL_ADC_STATE_BUSY,                        /*!< An internal process is ongoing */
    HAL_ADC_STATE_BUSY_REG,                    /*!< Regular conversion is ongoing */
    HAL_ADC_STATE_BUSY_INJ,                    /*!< Injected conversion is ongoing */
    HAL_ADC_STATE_BUSY_INJ_REG,                /*!< Injected and regular conversion are ongoing */
    HAL_ADC_STATE_TIMEOUT,                     /*!< Timeout state */
    HAL_ADC_STATE_ERROR,                       /*!< ADC state error */
    HAL_ADC_STATE_EOC,                         /*!< Conversion is completed */
    HAL_ADC_STATE_EOC_REG,                     /*!< Regular conversion is completed */
    HAL_ADC_STATE_EOC_INJ,                     /*!< Injected conversion is completed */
    HAL_ADC_STATE_EOC_INJ_REG,                 /*!< Injected and regular conversion are completed */
    HAL_ADC_STATE_EOS_REG,
    HAL_ADC_STATE_DMA_FINISH,
    HAL_ADC_STATE_CPU_FINISH,
    HAL_ADC_STATE_IDLE,

} HAL_ADC_StateTypeDef;


/**
  * @brief ADC Init structure definition
  */
typedef struct
{
    uint32_t lowpowerEn;

    ADC_Resolution_T Resolution;            /*!< Configures the ADC resolution dual mode.
                                         This parameter can be a value of @ref ADC_Resolution */
    uint32_t discontinuousConvEn;   /*!< Specifies Discontinuous mode on regular channels.*/
    uint32_t injectDiscConvEn;      /*!< Specifies  Discontinuous mode on injected channels.*/

    uint32_t injectAutoEn;          /*!< Specifies  Automatic injected group conversion.*/

    uint32_t compareSingleEn;       /*!< Specifies Only one selected channel is auto compared.*/


    uint32_t workMode;             /*!< Specifies whether the conversion is performed in Single mode or
                                         Repeat mode.*/

    uint32_t compareEn;       /*!< Specifies ADC auto compare enable on regular channels.*/

    uint32_t compareMode;       /*!< Specifies ADC auto compare mode.*/

    uint32_t injectCompEn;       /*!< Specifies ADC auto compare enable on injected channels.*/

    uint32_t dma_en;

    uint32_t NbrOfConversion;       /*!< Specifies the number of ADC conversions that will be done using the sequencer for
                                         regular channel group. This parameter must be a number between Min_Data = 1 and Max_Data = 16. */
    /*This parameter can be set to ENABLE or DISABLE. */
    uint32_t InjectedNbrOfConversion;

    uint32_t NbrOfDiscConversion;   /*!< Specifies the number of ADC discontinuous conversions that will be done
                                         using the sequencer for regular channel group.
                                         This parameter must be a number between Min_Data = 1 and Max_Data = 8. */
    ADC_TrigerSource_T trigersel;
    ADC_TrigerMode_T trigermode;
    int timeOut;
} ADC_InitTypeDef;

/**
  * @brief ADC handle Structure definition
  */
typedef struct
{
    ADC_Device_T                  *device;                   /*!< Register base address */
    ADC_InitTypeDef               Init;                        /*!< ADC required parameters */
    DMA_Handle_T             *pDMAHandle;                 /*!< Pointer DMA Handler */
    __IO HAL_ADC_StateTypeDef     State;                       /*!< ADC communication state */
    uint32_t expectFinishStatus;
    uint32_t blockStatus;
    uint32_t rst_data[16];
    uint32_t dma_rst_len;
    HAL_LockTypeDef               Lock;                        /*!< ADC locking object */
} ADC_HandleTypeDef;

/**
  * @brief ADC Configuration regular Channel structure definition
  */
typedef struct
{
    uint32_t rchn_sel;       /*!< regular channel select */

    uint32_t SamplingTime;   /*!< The sample time value to be set for the selected channel.
                                  This parameter can be a value of @ref ADC_sampling_times */
} ADC_ChannelConfTypeDef;



/**
* ADC compare parameter  structure.
*/
typedef struct
{
    uint16_t                    compareLowValue ;       /**< ADC compare match low vaule */
    uint16_t                    compareHighValue;       /**< ADC compare match high vaule */
    uint16_t                    autoCompChSelect;
    ADC_CompareMode_T           compareMode;            /**< ADC compare mode
                                                          * 1: Usr set compareMode to ADC_COMPARE_EQUAL_TO ,when adc convet
                                                          *    value =  compareHighValue , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          * 2: Usr set compareMode to ADC_COMPARE_MORE_THAN ,when adc convet
                                                          *    value >  compareHighValue , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          * 3: Usr set compareMode to ADC_COMPARE_LESS_THAN ,when adc convet
                                                          *    value <  compareHighValue , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          * 4: Usr set compareMode to ADC_COMPARE_WITHIN_RANGE ,when adc convet
                                                          *    (value > compareLowValue && value < compareHighValue)  , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          */
} ADC_CompareConfig_T;


/**
  * @brief ADC Configuration injected Channel structure definition
  */
typedef struct
{
    uint8_t injectchn_sel[4];  /*!< The chanel in the injected group sequencer
                                                This parameter must be a number between Min_Data = 0 and Max_Data = 3. */

    uint8_t InjectedSamplingTime;           /*!< The sample time value to be set for the selected channel.
                                                This parameter can be a value of @ref ADC_sampling_times */

    uint32_t InjectedNbrOfConversion;        /*!< Specifies the number of ADC conversions that will be done using the sequencer for
                                                injected channel group.
                                                This parameter must be a number between Min_Data = 1 and Max_Data = 4. */
    uint32_t AutoInjectedConv;               /*!< Enables or disables the selected ADC automatic injected group
                                                conversion after regular one */
    uint32_t InjectedDiscontinuousConvMode;  /*!< Specifies whether the conversion is performed in Discontinuous mode or not for injected channels.
                                                This parameter can be set to ENABLE or DISABLE. */
    uint32_t ExternalTrigInjecConvEdge;      /*!< Select the external trigger edge and enable the trigger of an injected channels.
                                                This parameter can be a value of @ref ADCEx_External_trigger_edge_Injected */
    uint32_t ExternalTrigInjecConv;          /*!< Select the external event used to trigger the start of conversion of a injected channels.
                                                This parameter can be a value of @ref ADCEx_External_trigger_Source_Injected */
} ADC_InjectionConfTypeDef;

/**
 * @brief Initializes ADC device with user configuration and user handle.
 *
 * This function configures ADC device with the user-defined settings.
 *
 * @param pADC           ADC device point
 * @param pConfig        Pointer to user-defined configuration.
 * @param hADC           Pointer to ADC handle.
 *
 * @retval RET_OK        ADC_Init success
 * @retval RET_ERROR     ADC_Init error
 * @retval RET_BUSY      ADC is using by others
 * @retval RET_INVPARAM  Usr input paramter is error
 */

/* Initialization/de-initialization functions ***********************************/
RET_CODE HAL_ADC_Init(ADC_HandleTypeDef *hadc);
RET_CODE HAL_ADC_UnInit(ADC_HandleTypeDef *hadc);


/**
 * @brief use to read adc convet value.
 *
 * This function use to read adc sequence convet  value.
 *
 * @param hADC Pointer to ADC handler.
 * @param pConvertData
 *        point to adc convert result buf, when usr config to tx use
 *        dma mode or config noblock mode,please make sure
 *        this pconvertdata must point golbal variable.
 * @param timeout
 *        timeout != TIMEOUT_NO_BLOCKING, this function is block until function return.if return RET_OK,usr can
 *        get the convert result from pConfig->pConvertData,timeout = TIMEOUT_NO_BLOCKING, this function will
 *        return immediately.usr should read the pConfig->convertData at callback recive ADC_EVENT_FINISHED event.
 *
 * @retval RET_OK        ADC convert success
 * @retval RET_ERROR     ADC convert error
 * @retval RET_BUSY      ADC is using by others
 * @retval RET_TIMEOUT   ADC is not convert finish during the seting timeout value
 * @retval RET_INVPARAM  Usr input paramter is error
 * @retval RET_ONGOING   ADC sequence convert is ongong,commonly it will happen when
 *                       usr set timeoutUs = TIMEOUT_NO_WAIT
 */
RET_CODE ADC_ReadConvertValue(ADC_HandleTypeDef *hADC, uint32_t *pConvertData, uint32_t timeout);
HAL_StatusTypeDef  HAL_ADC_FifoConvertValue(ADC_HandleTypeDef *hadc, unsigned char  resolution, uint32_t *pConvertData, uint8_t length);


/**
 * @brief Deinitializes ADC device.
 *
 * This function make ADC to stop mode, and release some resource.
 *
 * @param pADC          ADC device point
 * @param hADC          Pointer to ADC handle.
 *
 * @retval RET_OK       ADC_DeInit success
 * @retval RET_ERROR    ADC_DeInit error
 * @retval RET_BUSY     ADC is used by others
 */
//RET_CODE ADC_DeInit(ADC_Device_T *pADC, ADC_Handle_T *hADC);

/**
 * @brief config singal ADC channel parameter
 *
 * This function Config ADC channel config paramete before start ADC, when finish some
 * convert. if usr want to config another resolution ,sampleTime or channelindex,
 * usr call this function to change it befor read adc convet value,
 * then usr can continue to start to convert again , usr should not
 * need to call ADC_DeInit and ADC_Init to Config again.
 *
 * @param pADC         ADC device point
 * @param pConfig      Pointer to ADC channel config
 *
 * @retval RET_OK      ADC_ChannelConfig success
 * @retval RET_ERROR   ADC_ChannelConfig error
 * @retval RET_BUSY    ADC is used by others
 */

/**
 * @brief config ADC sequence parameter
 *
 * This function config ADC sequence paramete before start ADC, when finish some
 * convert. if usr want to config another resolution ,sampleTime or sequenceConfig,
 * usr call this function to change it befor read adc convet value,
 * then usr can continue to start to convert again , usr should not
 * need to call ADC_DeInit and ADC_Init to Config again.
 *
 * @param hADC         Pointer to ADC handle
 * @param pConfig      Pointer to ADC sequence config
 *
 * @retval RET_OK     ADC_SequenceConfig success
 * @retval RET_ERROR  ADC_SequenceConfig error
 * @retval RET_BUSY   ADC is used by others
 */


/**
 * @brief config ADC compareparameter
 *
 * This function config ADC compare paramete before start ADC if usr want to use
 * ADC compare function should first call  ADC_CompareModeEnable function , when finish some
 * compare convert.if usr want to config another compare parameter , usr call this
 * function to change it, then continue to compare convet, usr should not need to call
 * ADC_DeInit and ADC_Init to config again.
 *
 * @param hADC Pointer to ADC handle.
 * @param pCompareConfig ADC compare parameter  point
 *
 * @retval RET_OK     ADC_CompareConfig success
 * @retval RET_ERROR  ADC_CompareConfig error
 * @retval RET_BUSY   ADC is used by others
 */
RET_CODE ADC_CompareConfig(ADC_HandleTypeDef *hADC, ADC_CompareConfig_T *pCompareConfig);

/**
 * @brief Enable ADC compare Function
 *
 * This function  use to enable ADC compare function ,when usr want to use ADC compare function,
 * it should first call ADC_ChannelSampleConfig and then call ADC_CompareModeEnable
 *
 * @param hADC Pointer to ADC handle.
 *
 * @retval RET_OK      ADC_CompareModeEnable success
 * @retval RET_ERROR   ADC_CompareModeEnable error
 * @retval RET_BUSY    ADC is used by others
 */
RET_CODE ADC_CompareModeEnable(ADC_HandleTypeDef *hADC);

/**
 * @brief Disable ADC compare Function
 *
 * This function use to disable ADC comparefunction ,when usr not want to use ADC comparefunction,
 * it shoud  call ADC_CompareModeDisable,default is disable ADC compare function.
 *
 * @param hADC Pointer to ADC handle.
 *
 * @retval RET_OK      ADC_CompareModeDisable success
 * @retval RET_ERROR   ADC_CompareModeDisable error
 * @retval RET_BUSY    ADC is used by others
 */

RET_CODE ADC_CompareModeDisable(ADC_HandleTypeDef *hADC);
void ADC_FifoConvertValue(ADC_Device_T *pDevice, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length);
void ADC_Unlock(ADC_HandleTypeDef *hADC);
void HAL_ADC_IRQ_handler(ADC_HandleTypeDef *hADC);

/**
  * @brief  The following  function are used to reset ADC peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_ADC_SW_Reset(void);

/**
  * @brief  The following two functions are used to enable/disable ADC  peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param adc_sel: select adcX
  *
  * @retval None
  *
  */
void HAL_ADC_CLK_ENABLE(int adc_sel);
void HAL_ADC_CLK_DISABLE(int adc_sel);

/**
  * @brief  The following  function are used to  ADC peripheral Request Irq.
  * @note  software has to reset peripheral  before using peripheral
  * @param  adc_sel: select ADCX
  *
  * @retval None
  *
  */
void HAL_ADC_RequestIrq(int adc_sel);

/**
  * @brief  The following two functions are used to enable/disable ADC  peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param adc_sel: select adcX
  *
  * @retval None
  *
  */
void HAL_ADC_BufConvertValue(ADC_HandleTypeDef *hadc, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length);

/**
  * @brief  Enables the interrupt and starts ADC conversion of regular channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);

/**
  * @brief  Enables the interrupt and starts ADC conversion of injected channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  *
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef *hadc);

/**
  * @brief  Gets the converted value from data register of injected channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  InjectedRank: the ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg Injected Channel0 selected
  *            @arg Injected Channel1 selected
  *            @arg Injected Channel2 selected
  *            @arg Injected Channel3 selected
  * @retval None
  */
HAL_StatusTypeDef  HAL_ADC_InjectConvertValue(ADC_HandleTypeDef *hadc, unsigned char  resolution, uint32_t *pConvertData, uint8_t length);

HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig);

/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfigInjected: ADC configuration structure for injected channel.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedSequenceChannel(ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected);

/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfigInjected: ADC configuration structure for injected channel.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected);


HAL_StatusTypeDef HAL_ADC_SequenceConfig(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig);

HAL_StatusTypeDef HAL_ADC_ConfigCompChannel(ADC_HandleTypeDef *hadc, ADC_CompareConfig_T *pcompConfig);

RET_CODE HAL_ADC_DmaConfig(ADC_HandleTypeDef *hADC, uint32_t *pConvertData, uint8_t length);


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_ADC_H_ */



