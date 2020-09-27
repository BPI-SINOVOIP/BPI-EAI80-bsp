/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GM_HAL_SAI_H
#define __GM_HAL_SAI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

//#include <gm_common.h>
#include <gm_hal_dma.h>
#include <sai/gm_ll_sai.h>

#define DEFAULT_SYSTEM_CLOCK           12000000u           /* Default System clock value */
#define SAI_FIFO_SIZE         8U
#define SAI_DEFAULT_TIMEOUT   4U /* 4ms */
#define SAI_xCR2_MUTECNT_OFFSET POSITION_VAL(SAI_xCR2_MUTECNT)
#define HAL_MAX_DELAY      0xFFFFFFFFU
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))
#define ASSERT(expr)          do{}while(0)
/*Threshold depth for Tx FIFO */
#define SAI_MAX_TX_FIFO_DEPTH             8
/*Threshold depth for Rx FIFO */
#define SAI_MAX_RX_FIFO_DEPTH             8

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
typedef int RET_CODE;
/** Prototype of the user callback function */
struct __SAI_HandleTypeDef;
typedef enum
{
    SAI_RECEIVE_EVENT              = 0x00,
    SAI_RECEIVE_EVENT_ERROR,
    SAI_DMAMST_RECEIVE_EVENT,
    SAI_DMASLV_RECEIVE_EVENT,
} SAI_DMA_Event_T;
typedef void (*SAI_Callback_T)(struct __SAI_HandleTypeDef *hsai, uint8_t event, unsigned long param);
typedef void (*HAL_DMA_Callback_T)(DMA_Handle_T *hDMA, SAI_DMA_Event_T event, unsigned long Param);

typedef enum
{
    SAI_MODE_DMA,
    SAI_MODE_IT
} SAI_ModeTypedef;
typedef enum SAIDevn
{
    SAIDEV_0 = 0,
    SAIDEV_1 = 1
} SAI_Devn_T;
typedef enum
{
    HAL_SAI_OK       = 0x00U,
    HAL_SAI_ERROR    = 0x01U,
    HAL_SAI_BUSY     = 0x02U,
    HAL_SAI_TIMEOUT  = 0x03U,
    HAL_SAI_INVPARAM = 0x04U
} HAL_SAI_StatusTypeDef;
/**
* SAI working mode
*/
typedef enum
{
    SAI_MASTER_MODE = 0x00,  /**< Master mode */
    SAI_SLAVE_MODE    = 0x01   /**< Slave mode */
} SAI_WorkMode_T;

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
    HAL_SAI_STATE_RESET      = 0x00U,  /*!< SAI not yet initialized or disabled                */
    HAL_SAI_STATE_READY      = 0x01U,  /*!< SAI initialized and ready for use                  */
    HAL_SAI_STATE_BUSY       = 0x02U,  /*!< SAI internal process is ongoing                    */
    HAL_SAI_STATE_BUSY_TX    = 0x12U,  /*!< Data transmission process is ongoing               */
    HAL_SAI_STATE_BUSY_RX    = 0x22U,  /*!< Data reception process is ongoing                  */
    HAL_SAI_STATE_TIMEOUT    = 0x03U,  /*!< SAI timeout state                                  */
    HAL_SAI_STATE_ERROR      = 0x04U   /*!< SAI error state                                    */
} HAL_SAI_StateTypeDef;

/**
  * @brief  SAI Callback prototype
  */
typedef void (*SAIcallback)(void);

#define SAI_PLL_49 49125000
#define SAI_PLL_11 11050000

/** @defgroup SAI_Init_Structure_definition SAI Init Structure definition
  * @brief  SAI Init Structure definition
  * @{
  */
typedef struct
{
    uint32_t AudioMode;           /*!< Specifies the SAI Block audio Mode.
                                     This parameter can be a value of @ref SAI_Block_Mode                      */

    uint32_t Synchro;             /*!< Specifies SAI Block synchronization
                                     This parameter can be a value of @ref SAI_Block_Synchronization           */

    uint32_t SynchroExt;          /*!< Specifies SAI external output synchronization, this setup is common
                                     for BlockA and BlockB
                                     This parameter can be a value of @ref SAI_Block_SyncExt
                                     @note: If both audio blocks of same SAI are used, this parameter has
                                            to be set to the same value for each audio block                   */

    uint32_t OutputDrive;         /*!< Specifies when SAI Block outputs are driven.
                                     This parameter can be a value of @ref SAI_Block_Output_Drive
                                     @note this value has to be set before enabling the audio block
                                     but after the audio block configuration.                                  */

    uint32_t FSOEn;
    uint32_t BCKOEn;
    uint32_t MCKOEn;
    uint32_t MCKNODIV;            /*!< Specifies SAI Block FIFO threshold. 0 or 1                                */
    uint32_t FIFOThreshold;       /*!< Specifies SAI Block FIFO threshold.
                                     This parameter can be a value of @ref SAI_Block_Fifo_Threshold            */

    uint32_t ClockSource;         /*!< Specifies the SAI Block x Clock source.
                                     This parameter is not used for STM32F446xx devices.                       */

    uint32_t AudioFrequency;      /*!< Specifies the audio frequency sampling.
                                     This parameter can be a value of @ref SAI_Audio_Frequency                 */

    uint32_t MonoStereoMode;      /*!< Specifies if the mono or stereo mode is selected.
                                     This parameter can be a value of @ref SAI_Mono_Stereo_Mode                */

    uint32_t CompandingMode;      /*!< Specifies the companding mode type.
                                     This parameter can be a value of @ref SAI_Block_Companding_Mode           */

    uint32_t TriState;            /*!< Specifies the companding mode type.
                                     This parameter can be a value of @ref SAI_TRIState_Management             */

    /* This part of the structure is automatically filled if your are using the high level intialisation
       function HAL_SAI_InitProtocol                                                                             */

    uint32_t Protocol;        /*!< Specifies the SAI Block protocol.
                                 This parameter can be a value of @ref SAI_Block_Protocol                      */

    uint32_t DataSize;        /*!< Specifies the SAI Block data size.
                                 This parameter can be a value of @ref SAI_Block_Data_Size                     */

    uint32_t FirstBit;        /*!< Specifies whether data transfers start from MSB or LSB bit.
                                 This parameter can be a value of @ref SAI_Block_MSB_LSB_transmission          */

    uint32_t ClockStrobing;   /*!< Specifies the SAI Block clock strobing edge sensitivity.
                                 This parameter can be a value of @ref SAI_Block_Clock_Strobing                */
} SAI_InitTypeDef;
/**
  * @}
  */

/** @defgroup SAI_Frame_Structure_definition SAI Frame Structure definition
  * @brief  SAI Frame Init structure definition
  * @{
  */
typedef struct
{
    uint32_t FrameLength;         /*!< Specifies the Frame length, the number of SCK clocks for each audio frame.
                                     This parameter must be a number between Min_Data = 8 and Max_Data = 256.
                                     @note  If master clock MCLK_x pin is declared as an output, the frame length
                                            should be aligned to a number equal to power of 2 in order to keep
                                            in an audio frame, an integer number of MCLK pulses by bit Clock. */

    uint32_t ActiveFrameLength;  /*!< Specifies the Frame synchronization active level length.
                                    This Parameter specifies the length in number of bit clock (SCK + 1)
                                    of the active level of FS signal in audio frame.
                                    This parameter must be a number between Min_Data = 1 and Max_Data = 128   */

    uint32_t FSDefinition;       /*!< Specifies the Frame synchronization definition.
                                    This parameter can be a value of @ref SAI_Block_FS_Definition             */

    uint32_t FSPolarity;         /*!< Specifies the Frame synchronization Polarity.
                                    This parameter can be a value of @ref SAI_Block_FS_Polarity               */

    uint32_t FSOffset;           /*!< Specifies the Frame synchronization Offset.
                                    This parameter can be a value of @ref SAI_Block_FS_Offset                 */
} SAI_FrameInitTypeDef;
/**
  * @}
  */

/** @defgroup SAI_Slot_Structure_definition SAI Slot Structure definition
  * @brief   SAI Block Slot Init Structure definition
  * @{
  */
typedef struct
{
    uint32_t FirstBitOffset;  /*!< Specifies the position of first data transfer bit in the slot.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 24 */

    uint32_t SlotSize;        /*!< Specifies the Slot Size.
                                 This parameter can be a value of @ref SAI_Block_Slot_Size              */

    uint32_t SlotNumber;      /*!< Specifies the number of slot in the audio frame.
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 16 */

    uint32_t SlotActive;      /*!< Specifies the slots in audio frame that will be activated.
                                 This parameter can be a value of @ref SAI_Block_Slot_Active            */
} SAI_SlotInitTypeDef;

/**
  * @}
  */

typedef enum
{
    HAL_SAI_UNLOCKED = 0x00,
    HAL_SAI_LOCKED   = 0x01
} HAL_LockTypeDef;


typedef enum
{
    HAL_DMA_STATE_RESET             = 0x00U,  /*!< DMA not yet initialized or disabled */
    HAL_DMA_STATE_READY             = 0x01U,  /*!< DMA initialized and ready for use   */
    HAL_DMA_STATE_BUSY              = 0x02U,  /*!< DMA process is ongoing              */
    HAL_DMA_STATE_TIMEOUT           = 0x03U,  /*!< DMA timeout state                   */
    HAL_DMA_STATE_ERROR             = 0x04U,  /*!< DMA error state                     */
    HAL_DMA_STATE_ABORT             = 0x05U,  /*!< DMA Abort state                     */
} HAL_DMA_StateTypeDef;
/**
  * @brief  DMA Configuration Structure definition
  */
typedef struct
{
    uint32_t Channel;              /*!< Specifies the channel used for the specified stream.
                                      This parameter can be a value of @ref DMA_Channel_selection                    */

    uint32_t Direction;            /*!< Specifies if the data will be transferred from memory to peripheral,
                                      from memory to memory or from peripheral to memory.
                                      This parameter can be a value of @ref DMA_Data_transfer_direction              */

    uint32_t PeriphInc;          /*!< Specifies whether the Peripheral address register should be incremented or not.
                                      This parameter can be a value of @ref DMA_Peripheral_incremented_mode          */

    uint32_t MemInc;               /*!< Specifies whether the memory address register should be incremented or not.
                                      This parameter can be a value of @ref DMA_Memory_incremented_mode              */

    uint32_t PeriphDataAlignment;  /*!< Specifies the Peripheral data width.
                                      This parameter can be a value of @ref DMA_Peripheral_data_size                 */

    uint32_t MemDataAlignment;     /*!< Specifies the Memory data width.
                                      This parameter can be a value of @ref DMA_Memory_data_size                     */

    uint32_t Mode;                 /*!< Specifies the operation mode of the DMAy Streamx.
                                      This parameter can be a value of @ref DMA_mode
                                      @note The circular buffer mode cannot be used if the memory-to-memory
                                            data transfer is configured on the selected Stream                        */

    uint32_t Priority;             /*!< Specifies the software priority for the DMAy Streamx.
                                      This parameter can be a value of @ref DMA_Priority_level                       */

    uint32_t FIFOMode;             /*!< Specifies if the FIFO mode or Direct mode will be used for the specified stream.
                                      This parameter can be a value of @ref DMA_FIFO_direct_mode
                                      @note The Direct mode (FIFO mode disabled) cannot be used if the
                                            memory-to-memory data transfer is configured on the selected stream       */

    uint32_t FIFOThreshold;        /*!< Specifies the FIFO threshold level.
                                      This parameter can be a value of @ref DMA_FIFO_threshold_level                  */

    uint32_t MemBurst;             /*!< Specifies the Burst transfer configuration for the memory transfers.
                                      It specifies the amount of data to be transferred in a single non interruptible
                                      transaction.
                                      This parameter can be a value of @ref DMA_Memory_burst
                                      @note The burst mode is possible only if the address Increment mode is enabled. */

    uint32_t PeriphBurst;          /*!< Specifies the Burst transfer configuration for the peripheral transfers.
                                      It specifies the amount of data to be transferred in a single non interruptible
                                      transaction.
                                      This parameter can be a value of @ref DMA_Peripheral_burst
                                      @note The burst mode is possible only if the address Increment mode is enabled. */
} DMA_InitTypeDef;
typedef struct
{
    __IO uint32_t CR;     /*!< DMA stream x configuration register      */
    __IO uint32_t NDTR;   /*!< DMA stream x number of data register     */
    __IO uint32_t PAR;    /*!< DMA stream x peripheral address register */
    __IO uint32_t M0AR;   /*!< DMA stream x memory 0 address register   */
    __IO uint32_t M1AR;   /*!< DMA stream x memory 1 address register   */
    __IO uint32_t FCR;    /*!< DMA stream x FIFO control register       */
} DMA_Stream_TypeDef;


/**
      * @brief  DMA handle Structure definition
      */
typedef struct __DMA_HandleTypeDef
{
    DMA_Stream_TypeDef         *Instance;                                                        /*!< Register base address                  */

    DMA_InitTypeDef            Init;                                                             /*!< DMA communication parameters           */

    HAL_LockTypeDef            Lock;

    DMA_ChannelIdx_T    chIdx;/**< DMA handshake channel index *//*!< DMA locking object                     */

    __IO HAL_DMA_StateTypeDef  State;                                                            /*!< DMA transfer state                     */

    void                       *Parent;                                                          /*!< Parent object state                    */

    void (* XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);                                  /*!< DMA transfer complete callback         */

    void (* XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);                              /*!< DMA Half transfer complete callback    */

    void (* XferM1CpltCallback)(struct __DMA_HandleTypeDef *hdma);                            /*!< DMA transfer complete Memory1 callback */

    void (* XferM1HalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);                           /*!< DMA transfer Half complete Memory1 callback */

    void (* XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);                             /*!< DMA transfer error callback            */

    void (* XferAbortCallback)(struct __DMA_HandleTypeDef *hdma);                             /*!< DMA transfer Abort callback            */

    __IO uint32_t              ErrorCode;                                                        /*!< DMA Error code                          */

    uint32_t                   StreamBaseAddress;                                                /*!< DMA Stream Base Address                */

    uint32_t                   StreamIndex;                                                      /*!< DMA Stream Index                       */

    //HAL_DMA_Callback_T         callback;                                                         /**< DMA transaction callback */

} DMA_HandleTypeDef;

/** @defgroup SAI_Handle_Structure_definition SAI Handle Structure definition
  * @brief  SAI handle Structure definition
  * @{
  */
typedef struct __SAI_HandleTypeDef
{
    SAI_Block_TypeDef         *Instance;  /*!< SAI Blockx registers base address        */

    SAI_InitTypeDef           Init;       /*!< SAI communication parameters             */

    SAI_FrameInitTypeDef      FrameInit;  /*!< SAI Frame configuration parameters       */

    SAI_SlotInitTypeDef       SlotInit;   /*!< SAI Slot configuration parameters        */

    DMA_Handle_T              *hDMA;      /**< DMA device handler                           */

    uint32_t                  *pBuffPtr;  /*!< Pointer to SAI transfer Buffer            */

    uint32_t                  *pBuffRx;  /*!< Pointer to SAI transfer Buffer             */

    uint32_t                  *pdmaBuffPtr;  /*!< Pointer to SAI DMA transfer Buffer            */

    uint16_t                  XferSize;  /*!< SAI transfer size                         */

    uint16_t                  XferCount; /*!< SAI transfer counter                      */

    uint16_t  txLength;    /**< Data length for transfering */

    uint16_t  txRemained;  /**< Data remained for transfering */

    uint16_t  rxLength;    /**< Data length for receiving */

    uint16_t  rxRemained;  /**< Data remained for receiving */

    uint32_t                  IRQn;             /*point to mmc_host*/

    uint8_t                   *pTxBuffer;    /**< Pointer to Tx buffer */

    uint8_t                   *pRxBuffer;    /**< Pointer to Rx buffer */

#if  0//def HAVE_DMA
    DMA_HandleTypeDef         *hdmatx;     /*!< SAI Tx DMA handle parameters            */

    DMA_HandleTypeDef         *hdmarx;     /*!< SAI Rx DMA handle parameters            */
#endif

    SAIcallback               mutecallback;/*!< SAI mute callback                       */

    void (*InterruptServiceRoutine)(struct __SAI_HandleTypeDef *hsai); /* function pointer for IRQ handler   */

    HAL_LockTypeDef           Lock;        /*!< SAI locking object                      */

    __IO HAL_SAI_StateTypeDef State;       /*!< SAI communication state                 */

    __IO uint32_t             ErrorCode;   /*!< SAI Error code                          */

    SAI_Callback_T            callback;    /**< callback function */

    __IO uint16_t             dmaBufferLength;  /**< DMA buffer length */

    SAI_Callback_T            sai_callback;
} SAI_HandleTypeDef;
/**
  * @}
  */


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup SAI_Exported_Constants SAI Exported Constants
  * @{
  */

/** @defgroup SAI_Error_Code SAI Error Code
  * @{
  */
#define HAL_SAI_ERROR_NONE    0x00000000U  /*!< No error                                    */
#define HAL_SAI_ERROR_OVR     0x00000001U  /*!< Overrun Error                               */
#define HAL_SAI_ERROR_UDR     0x00000002U  /*!< Underrun error                              */
#define HAL_SAI_ERROR_AFSDET  0x00000004U  /*!< Anticipated Frame synchronisation detection */
#define HAL_SAI_ERROR_LFSDET  0x00000008U  /*!< Late Frame synchronisation detection        */
#define HAL_SAI_ERROR_CNREADY 0x00000010U  /*!< codec not ready                             */
#define HAL_SAI_ERROR_WCKCFG  0x00000020U  /*!< Wrong clock configuration                   */
#define HAL_SAI_ERROR_TIMEOUT 0x00000040U  /*!< Timeout error                               */
#define HAL_SAI_ERROR_DMA     0x00000080U  /*!< DMA error                                   */
/**
  * @}
  */
/**
* I2C event type
 */
typedef enum
{
    SAI_EVENT_TRANSFER_DONE     = 0x00,  /**< SAI transaction is completed */
    SAI_EVENT_TX_FIFO_OVERFLOW  = 0x01,  /**< SAI TxFIFO overflows */
    SAI_EVENT_RX_FIFO_OVERFLOW  = 0x02,  /**< SAI RxFIFO overflows */
    SAI_EVENT_RX_FIFO_UNDERFLOW = 0x03,  /**< SAI RxFIFO underlows */
    SAI_EVENT_ARB_LOST          = 0x04,  /**< SAI arbitration losts */
    SAI_EVENT_TERMINATED        = 0x08,  /**< SAI transaction terminated unexpectedly*/
    SAI_EVENT_DMAERROR          = 0x10,  /**< SAI DMA error*/
    SAI_EVNET_TRANSFER_ERROR    = 0xFF   /**< SAI transaction error */
} SAI_Event_T;

/** @defgroup SAI_Block_SyncExt SAI External synchronisation
  * @{
  */
#define SAI_SYNCEXT_DISABLE          0U
#define SAI_SYNCEXT_OUTBLOCKA_ENABLE 1U
#define SAI_SYNCEXT_OUTBLOCKB_ENABLE 2U
/**
  * @}
  */

/** @defgroup SAI_Protocol SAI Supported protocol
  * @{
  */
#define SAI_I2S_STANDARD      0U
#define SAI_I2S_MSBJUSTIFIED  1U
#define SAI_I2S_LSBJUSTIFIED  2U
#define SAI_PCM_LONG          3U
#define SAI_PCM_SHORT         4U
/**
  * @}
  */

/** @defgroup SAI_Protocol_DataSize SAI protocol data size
  * @{
  */
#define SAI_PROTOCOL_DATASIZE_8BIT          0U
#define SAI_PROTOCOL_DATASIZE_10BIT         1U
#define SAI_PROTOCOL_DATASIZE_16BIT         2U
#define SAI_PROTOCOL_DATASIZE_16BITEXTENDED 3U
#define SAI_PROTOCOL_DATASIZE_24BIT         4U
#define SAI_PROTOCOL_DATASIZE_32BIT         5U
/**
  * @}
  */

/** @defgroup SAI_Audio_Frequency SAI Audio Frequency
  * @{
  */
#define SAI_AUDIO_FREQUENCY_192K          192000U
#define SAI_AUDIO_FREQUENCY_96K           96000U
#define SAI_AUDIO_FREQUENCY_48K           48000U
#define SAI_AUDIO_FREQUENCY_44K           44100U
#define SAI_AUDIO_FREQUENCY_32K           32000U
#define SAI_AUDIO_FREQUENCY_22K           22050U
#define SAI_AUDIO_FREQUENCY_16K           16000U
#define SAI_AUDIO_FREQUENCY_11K           11025U
#define SAI_AUDIO_FREQUENCY_8K            8000U
#define SAI_AUDIO_FREQUENCY_500           500U
#define SAI_AUDIO_FREQUENCY_MCKDIV        0U

/**
  * @}
  */

/** @defgroup SAI_Block_Mode SAI Block Mode
  * @{
  */
#define SAI_MODEMASTER_TX         0x00000000U
#define SAI_MODEMASTER_RX         ((uint32_t)SAI_xCR1_MODE_0)
#define SAI_MODESLAVE_TX          ((uint32_t)SAI_xCR1_MODE_1)
#define SAI_MODESLAVE_RX          ((uint32_t)SAI_xCR1_MODE_Msk)
/**
  * @}
  */

/** @defgroup SAI_Block_Protocol SAI Block Protocol
  * @{
  */
#define SAI_FREE_PROTOCOL                 0x00000000U
#define SAI_SPDIF_PROTOCOL                ((uint32_t)SAI_xCR1_PRTCFG_0)
#define SAI_AC97_PROTOCOL                 ((uint32_t)SAI_xCR1_PRTCFG_1)
/**
  * @}
  */

/** @defgroup SAI_Block_Data_Size SAI Block Data Size
  * @{
  */
#define SAI_DATASIZE_8                   ((uint32_t)SAI_xCR1_DS_1)
#define SAI_DATASIZE_10                  ((uint32_t)(SAI_xCR1_DS_1 | SAI_xCR1_DS_0))
#define SAI_DATASIZE_16                  ((uint32_t)SAI_xCR1_DS_2)
#define SAI_DATASIZE_20                  ((uint32_t)(SAI_xCR1_DS_2 | SAI_xCR1_DS_))
#define SAI_DATASIZE_24                  ((uint32_t)(SAI_xCR1_DS_2 | SAI_xCR1_DS_0))
#define SAI_DATASIZE_32                  ((uint32_t)(SAI_xCR1_DS_2 | SAI_xCR1_DS_1 | SAI_xCR1_DS_0))
/**
  * @}
  */

/** @defgroup SAI_Block_MSB_LSB_transmission SAI Block MSB LSB transmission
  * @{
  */
#define SAI_FIRSTBIT_MSB                  0x00000000U
#define SAI_FIRSTBIT_LSB                  ((uint32_t)SAI_xCR1_LSBFIRST)
/**
  * @}
  */

/** @defgroup SAI_Block_Clock_Strobing SAI Block Clock Strobing
  * @{
  */
#define SAI_CLOCKSTROBING_FALLINGEDGE     0U
#define SAI_CLOCKSTROBING_RISINGEDGE      1U
/**
  * @}
  */

/** @defgroup SAI_Block_Synchronization SAI Block Synchronization
  * @{
  */
#define SAI_ASYNCHRONOUS                  0x00000000U /*!< Asynchronous                             */
#define SAI_SYNCHRONOUS                   1U /*!< Synchronous with other block of same SAI */
#define SAI_SYNCHRONOUS_EXT_SAI1          2U /*!< Synchronous with other SAI, SAI1         */
#define SAI_SYNCHRONOUS_EXT_SAI2          3U /*!< Synchronous with other SAI, SAI2         */
/**
  * @}
  */
/** @defgroup SAI_Block_FrameInit.FSOffset
  * @{
  */
#define SAI_xFRCR_FSOFF_0                 0x0U /*!<  Frame  offset is 0   */
#define SAI_xFRCR_FSOFF_1                ((uint32_t)SAI_xFRCR_FSOFF)/*!<  Frame  offset is 1   */
/**
  * @}
  */
/** @defgroup SAI_Block_FrameInit.ActiveFrameLength
  * @{
  */
#define SAI_xFRCR_ActiveFrameLength_32                 0x00000020U /*!<  Frame  offset is 0   */
/**
  * @}
  */
/** @defgroup SAI_Block_SlotInit.SlotNumber
* @{
*/
#define SAI_xFRCR_SlotInit_SlotNumber_2                 0x00000002U /*!<  SlotNumbert is 2   */
#define SAI_xFRCR_SlotInit_SlotNumber_4                 0x00000004U /*!<  SlotNumbert is 4   */
#define SAI_xFRCR_SlotInit_SlotNumber_6                 0x00000006U /*!<  SlotNumbert is 6   */
#define SAI_xFRCR_SlotInit_SlotNumber_8                 0x00000008U /*!<  SlotNumbert is 8   */
#define SAI_xFRCR_SlotInit_SlotNumber_10                0x000000010U /*!<  SlotNumbert is 10   */
#define SAI_xFRCR_SlotInit_SlotNumber_12                0x000000012U /*!<  SlotNumbert is 12   */
#define SAI_xFRCR_SlotInit_SlotNumber_14                0x000000014U /*!<  SlotNumbert is 14   */
#define SAI_xFRCR_SlotInit_SlotNumber_16                0x000000016U /*!<  SlotNumbert is 16   */
/**
  * @}
  */
/** @defgroup SAI_Block_SlotInit.SlotSize
* @{
*/
#define SAI_xFRCR_SlotInit_SlotSize_16                 0x00000002U /*!<  SlotNumbert is 16   */
/**
  * @}
  */
/** @defgroup SAI_Block_Output_Drive SAI Block Output Drive
  * @{
  */
#define SAI_OUTPUTDRIVE_DISABLE          0x00000000U
#define SAI_OUTPUTDRIVE_ENABLE           ((uint32_t)SAI_xCR1_OUTDRIV)
/**
  * @}
  */

/** @defgroup SAI_Block_NoDivider SAI Block NoDivider
  * @{
  */
//#define SAI_MASTERDIVIDER_ENABLE         0x00000000U
//#define SAI_MASTERDIVIDER_DISABLE        ((uint32_t)SAI_xCR1_NODIV)
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Definition SAI Block FS Definition
  * @{
  */
#define SAI_FS_STARTFRAME                 0x00000000U
#define SAI_FS_CHANNEL_IDENTIFICATION     ((uint32_t)SAI_xFRCR_FSDEF)
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Polarity SAI Block FS Polarity
  * @{
  */
#define SAI_FS_ACTIVE_LOW                  0x00000000U
#define SAI_FS_ACTIVE_HIGH                 ((uint32_t)SAI_xFRCR_FSPOL)
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Offset SAI Block FS Offset
  * @{
  */
#define SAI_FS_FIRSTBIT                   0x00000000U
#define SAI_FS_BEFOREFIRSTBIT             ((uint32_t)SAI_xFRCR_FSOFF)
/**
  * @}
  */

/** @defgroup SAI_Block_Slot_Size SAI Block Slot Size
  * @{
  */
#define SAI_SLOTSIZE_DATASIZE             0x00000000U
#define SAI_SLOTSIZE_16B                  ((uint32_t)SAI_xSLOTR_SLOTSZ_0)
#define SAI_SLOTSIZE_32B                  ((uint32_t)SAI_xSLOTR_SLOTSZ_1)
/**
  * @}
  */

/** @defgroup SAI_Block_Slot_Active SAI Block Slot Active
  * @{
  */
#define SAI_SLOT_NOTACTIVE           0x00000000U
#define SAI_SLOTACTIVE_0             0x00000001U
#define SAI_SLOTACTIVE_1             0x00000002U
#define SAI_SLOTACTIVE_2             0x00000004U
#define SAI_SLOTACTIVE_3             0x00000008U
#define SAI_SLOTACTIVE_4             0x00000010U
#define SAI_SLOTACTIVE_5             0x00000020U
#define SAI_SLOTACTIVE_6             0x00000040U
#define SAI_SLOTACTIVE_7             0x00000080U
#define SAI_SLOTACTIVE_8             0x00000100U
#define SAI_SLOTACTIVE_9             0x00000200U
#define SAI_SLOTACTIVE_10            0x00000400U
#define SAI_SLOTACTIVE_11            0x00000800U
#define SAI_SLOTACTIVE_12            0x00001000U
#define SAI_SLOTACTIVE_13            0x00002000U
#define SAI_SLOTACTIVE_14            0x00004000U
#define SAI_SLOTACTIVE_15            0x00008000U
#define SAI_SLOTACTIVE_ALL           0x0000FFFFU
/**
  * @}
  */

/** @defgroup SAI_Mono_Stereo_Mode SAI Mono Stereo Mode
  * @{
  */
#define SAI_STEREOMODE                    0x00000000U
#define SAI_MONOMODE                      ((uint32_t)SAI_xCR1_MONO)
/**
  * @}
  */

/** @defgroup SAI_TRIState_Management SAI TRIState Management
  * @{
  */
#define SAI_OUTPUT_NOTRELEASED              0x00000000U
#define SAI_OUTPUT_RELEASED                 ((uint32_t)SAI_xCR2_TRIS)
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Threshold SAI Block Fifo Threshold
  * @{
  */
#define SAI_FIFOTHRESHOLD_EMPTY  0x00000000U
#define SAI_FIFOTHRESHOLD_1QF    ((uint32_t)(SAI_xCR2_FTH_0))
#define SAI_FIFOTHRESHOLD_HF     ((uint32_t)(SAI_xCR2_FTH_1))
#define SAI_FIFOTHRESHOLD_3QF    ((uint32_t)(SAI_xCR2_FTH_1 | SAI_xCR2_FTH_0))
#define SAI_FIFOTHRESHOLD_ALMOSTFULL   ((uint32_t)(SAI_xCR2_FTH_2))
#define SAI_FIFOTHRESHOLD_FULL   ((uint32_t)(SAI_xCR2_FTH_2|SAI_xCR2_FTH_0))
/**
  * @}
  */

/** @defgroup SAI_Block_Companding_Mode SAI Block Companding Mode
  * @{
  */
#define SAI_NOCOMPANDING                 0x00000000U
#define SAI_ULAW_1CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1))
#define SAI_ALAW_1CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1 | SAI_xCR2_COMP_0))
#define SAI_ULAW_2CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1 | SAI_xCR2_CPL))
#define SAI_ALAW_2CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1 | SAI_xCR2_COMP_0 | SAI_xCR2_CPL))
/**
  * @}
  */

/** @defgroup SAI_Block_Mute_Value SAI Block Mute Value
  * @{
  */
#define SAI_ZERO_VALUE                      0x00000000U
#define SAI_LAST_SENT_VALUE                 ((uint32_t)SAI_xCR2_MUTEVAL)
/**
  * @}
  */

/** @defgroup SAI_Block_Interrupts_Definition SAI Block Interrupts Definition
  * @{
  */
#define SAI_IT_OVRUDR                     ((uint32_t)SAI_xIMR_OVRUDRIE)
#define SAI_IT_MUTEDET                    ((uint32_t)SAI_xIMR_MUTEDETIE)
#define SAI_IT_WCKCFG                     ((uint32_t)SAI_xIMR_WCKCFGIE)
#define SAI_IT_FREQ                       ((uint32_t)SAI_xIMR_FREQIE)
#define SAI_IT_CNRDY                      ((uint32_t)SAI_xIMR_CNRDYIE)
#define SAI_IT_AFSDET                     ((uint32_t)SAI_xIMR_AFSDETIE)
#define SAI_IT_LFSDET                     ((uint32_t)SAI_xIMR_LFSDETIE)
/**
  * @}
  */

/** @defgroup SAI_Block_Flags_Definition  SAI Block Flags Definition
  * @{
  */
#define SAI_FLAG_OVRUDR                   ((uint32_t)SAI_xSR_OVRUDR)
#define SAI_FLAG_MUTEDET                  ((uint32_t)SAI_xSR_MUTEDET)
#define SAI_FLAG_WCKCFG                   ((uint32_t)SAI_xSR_WCKCFG)
#define SAI_FLAG_FREQ                     ((uint32_t)SAI_xSR_FREQ)
#define SAI_FLAG_CNRDY                    ((uint32_t)SAI_xSR_CNRDY)
#define SAI_FLAG_AFSDET                   ((uint32_t)SAI_xSR_AFSDET)
#define SAI_FLAG_LFSDET                   ((uint32_t)SAI_xSR_LFSDET)
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Status_Level   SAI Block Fifo Status Level
  * @{
  */
#define SAI_FIFOSTATUS_EMPTY              0x00000000U
#define SAI_FIFOSTATUS_LESS1QUARTERFULL   0x00010000U
#define SAI_FIFOSTATUS_1QUARTERFULL       0x00020000U
#define SAI_FIFOSTATUS_HALFFULL           0x00030000U
#define SAI_FIFOSTATUS_3QUARTERFULL       0x00040000U
#define SAI_FIFOSTATUS_FULL               0x00050000U
/**
  * @}
  */
/**@defgroup SAI_Block_FSOEn   SAI Block FS signal output enable bit
  * {@
  */
#define SAI_FS_SIGNAL_OUTPUT_DISABLE         0x00000000U
#define SAI_FS_SIGNAL_OUTPUT_ENABLE          ((uint32_t)SAI_xCR1_FSSOEN)

/**
  * @}
  */
/**@defgroup SAI_Block_BCKEn   SAI Block BCK enable bit
  * {@
  */
#define SAI_BCK_DISABLE         0x00000000U
#define SAI_BCK_ENABLE          ((uint32_t)SAI_xCR1_BCKSOEN)

/**
  * @}
  */
/**@defgroup SAI_Block_MCKEn   SAI Block MCK enable bit
 * {@
 */
#define SAI_MCK_DISABLE         0x00000000U
#define SAI_MCK_ENABLE          ((uint32_t)SAI_xCR1_MCKSOEN)

/**
* @}
*/
/**@defgroup SAI_Block_MCKNODIVEn   SAI Block MCKNODIV enable bit
 * {@
 */
#define SAI_MCKNODIV_DISABLE          0x00000000U
#define SAI_MCKNODIV_ENABLE           0x1

/**
* @}
*/



HAL_SAI_StatusTypeDef HAL_SAI_InitProtocol(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai);


/* Exported macro ------------------------------------------------------------*/

/** @defgroup SAI_Exported_Macros SAI Exported Macros
 *  @brief macros to handle interrupts and specific configurations
 * @{
 */

/** @brief Reset SAI handle state
  * @param  __HANDLE__: specifies the SAI Handle.
  * @retval NoneS
  */
#define __HAL_SAI_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_SAI_STATE_RESET)

/** @brief  Enable or disable the specified SAI interrupts.
  * @param  __HANDLE__: specifies the SAI Handle.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg SAI_IT_OVRUDR: Overrun underrun interrupt enable
  *            @arg SAI_IT_MUTEDET: Mute detection interrupt enable
  *            @arg SAI_IT_WCKCFG: Wrong Clock Configuration interrupt enable
  *            @arg SAI_IT_FREQ: FIFO request interrupt enable
  *            @arg SAI_IT_CNRDY: Codec not ready interrupt enable
  *            @arg SAI_IT_AFSDET: Anticipated frame synchronization detection interrupt enable
  *            @arg SAI_IT_LFSDET: Late frame synchronization detection interrupt enable
  * @retval None
  */
#define __HAL_SAI_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->IMR |= (__INTERRUPT__))
#define __HAL_SAI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->IMR &= (~(__INTERRUPT__)))

/** @brief  Check if the specified SAI interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the SAI Handle.
  *         This parameter can be SAI where x: 1, 2, or 3 to select the SAI peripheral.
  * @param  __INTERRUPT__: specifies the SAI interrupt source to check.
  *         This parameter can be one of the following values:
  *            @arg SAI_IT_OVRUDR: Overrun underrun interrupt enable
  *            @arg SAI_IT_MUTEDET: Mute detection interrupt enable
  *            @arg SAI_IT_WCKCFG: Wrong Clock Configuration interrupt enable
  *            @arg SAI_IT_FREQ: FIFO request interrupt enable
  *            @arg SAI_IT_CNRDY: Codec not ready interrupt enable
  *            @arg SAI_IT_AFSDET: Anticipated frame synchronization detection interrupt enable
  *            @arg SAI_IT_LFSDET: Late frame synchronization detection interrupt enable
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_SAI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IMR & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified SAI flag is set or not.
  * @param  __HANDLE__: specifies the SAI Handle.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg SAI_FLAG_OVRUDR: Overrun underrun flag.
  *            @arg SAI_FLAG_MUTEDET: Mute detection flag.
  *            @arg SAI_FLAG_WCKCFG: Wrong Clock Configuration flag.
  *            @arg SAI_FLAG_FREQ: FIFO request flag.
  *            @arg SAI_FLAG_CNRDY: Codec not ready flag.
  *            @arg SAI_FLAG_AFSDET: Anticipated frame synchronization detection flag.
  *            @arg SAI_FLAG_LFSDET: Late frame synchronization detection flag.
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SAI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the specified SAI pending flag.
  * @param  __HANDLE__: specifies the SAI Handle.
  * @param  __FLAG__: specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg SAI_FLAG_OVRUDR: Clear Overrun underrun
  *            @arg SAI_FLAG_MUTEDET: Clear Mute detection
  *            @arg SAI_FLAG_WCKCFG: Clear Wrong Clock Configuration
  *            @arg SAI_FLAG_FREQ: Clear FIFO request
  *            @arg SAI_FLAG_CNRDY: Clear Codec not ready
  *            @arg SAI_FLAG_AFSDET: Clear Anticipated frame synchronization detection
  *            @arg SAI_FLAG_LFSDET: Clear Late frame synchronization detection
  * @retval None
  */
#define __HAL_SAI_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->CLRFR = (__FLAG__))

/** @brief  Enable SAI
  * @param  __HANDLE__: specifies the SAI Handle.
  * @retval None
  */
#define __HAL_SAI_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 |=  SAI_xCR1_SAIEN)

/** @brief  Disable SAI
  * @param  __HANDLE__: specifies the SAI Handle.
  * @retval None
  */
#define __HAL_SAI_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 &=  ~SAI_xCR1_SAIEN)

/**
 * @}
 */

/* Include RCC SAI Extension module */


/* Exported functions --------------------------------------------------------*/

/** @addtogroup SAI_Exported_Functions
  * @{
  */

/* Initialization/de-initialization functions  **********************************/
/** @addtogroup SAI_Exported_Functions_Group1
  * @{
  */

//HAL_SAI_StatusTypeDef HAL_SAI_InitProtocol(SAI_Block_TypeDef *pSAI,SAI_HandleTypeDef *hsai);

//HAL_SAI_StatusTypeDef HAL_SAI_InitProtocol(SAI_Block_TypeDef*pSAI,SAI_HandleTypeDef *hsai);
HAL_SAI_StatusTypeDef HAL_SAI_Init(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai);
HAL_SAI_StatusTypeDef HAL_SAI_DeInit(SAI_HandleTypeDef *hsai);
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai);
void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai);

/**
  * @}
  */

/* I/O operation functions  *****************************************************/
/** @addtogroup SAI_Exported_Functions_Group2
  * @{
  */
/* Blocking mode: Polling */
HAL_SAI_StatusTypeDef HAL_SAI_Transmit(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size, uint32_t Timeout);
HAL_SAI_StatusTypeDef HAL_SAI_Receive(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size, uint32_t Timeout);

/* Non-Blocking mode: Interrupt */
HAL_SAI_StatusTypeDef HAL_SAI_Transmit_IT(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size);
HAL_SAI_StatusTypeDef HAL_SAI_Receive_IT(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size);

/* Non-Blocking mode: DMA */
HAL_SAI_StatusTypeDef HAL_SAI_Transmit_DMA(SAI_HandleTypeDef *hsai, SAI_WorkMode_T mode, uint32_t *pData, uint32_t Size);
HAL_SAI_StatusTypeDef HAL_SAI_Receive_DMA(SAI_HandleTypeDef *hsai, SAI_WorkMode_T mode, uint16_t *pData, uint32_t Size);
HAL_SAI_StatusTypeDef HAL_SAI_DMAPause(SAI_HandleTypeDef *hsai);
HAL_SAI_StatusTypeDef HAL_SAI_DMAResume(SAI_HandleTypeDef *hsai);
HAL_SAI_StatusTypeDef HAL_SAI_DMAStop(SAI_HandleTypeDef *hsai);

/* Abort function */
HAL_SAI_StatusTypeDef HAL_SAI_Abort(SAI_HandleTypeDef *hsai);

/* Mute management */
HAL_SAI_StatusTypeDef HAL_SAI_EnableTxMuteMode(SAI_HandleTypeDef *hsai, uint16_t val);
HAL_SAI_StatusTypeDef HAL_SAI_DisableTxMuteMode(SAI_HandleTypeDef *hsai);
HAL_SAI_StatusTypeDef HAL_SAI_EnableRxMuteMode(SAI_HandleTypeDef *hsai, SAIcallback callback, uint16_t counter);
HAL_SAI_StatusTypeDef HAL_SAI_DisableRxMuteMode(SAI_HandleTypeDef *hsai);

/* SAI IRQHandler and Callbacks used in non blocking modes (Interrupt and DMA) */
void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai);
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai);
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai);
/**
  * @}
  */

/** @addtogroup SAI_Exported_Functions_Group3
  * @{
  */
/* Peripheral State functions  ************************************************/
HAL_SAI_StateTypeDef HAL_SAI_GetState(SAI_HandleTypeDef *hsai);
uint32_t HAL_SAI_GetError(SAI_HandleTypeDef *hsai);
/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/** @addtogroup SAI_Private_Macros
  * @{
  */
#define IS_SAI_BLOCK_SYNCEXT(STATE)   (((STATE) == SAI_SYNCEXT_DISABLE)           ||\
                                       ((STATE) == SAI_SYNCEXT_OUTBLOCKA_ENABLE)  ||\
                                       ((STATE) == SAI_SYNCEXT_OUTBLOCKB_ENABLE))

#define IS_SAI_SUPPORTED_PROTOCOL(PROTOCOL)   (((PROTOCOL) == SAI_I2S_STANDARD)     ||\
        ((PROTOCOL) == SAI_I2S_MSBJUSTIFIED) ||\
        ((PROTOCOL) == SAI_I2S_LSBJUSTIFIED) ||\
        ((PROTOCOL) == SAI_PCM_LONG)         ||\
        ((PROTOCOL) == SAI_PCM_SHORT))

#define IS_SAI_PROTOCOL_DATASIZE(DATASIZE)   (((DATASIZE) == SAI_PROTOCOL_DATASIZE_16BIT)         ||\
        ((DATASIZE) == SAI_PROTOCOL_DATASIZE_16BITEXTENDED) ||\
        ((DATASIZE) == SAI_PROTOCOL_DATASIZE_24BIT)         ||\
        ((DATASIZE) == SAI_PROTOCOL_DATASIZE_32BIT))
#if 0
#define IS_SAI_AUDIO_FREQUENCY(AUDIO) (((AUDIO) == SAI_AUDIO_FREQUENCY_192K) || ((AUDIO) == SAI_AUDIO_FREQUENCY_96K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_48K)  || ((AUDIO) == SAI_AUDIO_FREQUENCY_44K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_32K)  || ((AUDIO) == SAI_AUDIO_FREQUENCY_22K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_16K)  || ((AUDIO) == SAI_AUDIO_FREQUENCY_11K) || \
                                       ((AUDIO) == SAI_AUDIO_FREQUENCY_8K)   || ((AUDIO) == SAI_AUDIO_FREQUENCY_MCKDIV))
#endif

#define IS_SAI_BLOCK_MODE(MODE)    (((MODE) == SAI_MODEMASTER_TX) || \
                                    ((MODE) == SAI_MODEMASTER_RX) || \
                                    ((MODE) == SAI_MODESLAVE_TX)  || \
                                    ((MODE) == SAI_MODESLAVE_RX))

#define IS_SAI_BLOCK_PROTOCOL(PROTOCOL) (((PROTOCOL) == SAI_FREE_PROTOCOL)  || \
        ((PROTOCOL) == SAI_AC97_PROTOCOL)  || \
        ((PROTOCOL) == SAI_SPDIF_PROTOCOL))

#define IS_SAI_BLOCK_DATASIZE(DATASIZE) (((DATASIZE) == SAI_DATASIZE_8)  || \
        ((DATASIZE) == SAI_DATASIZE_10) || \
        ((DATASIZE) == SAI_DATASIZE_16) || \
        ((DATASIZE) == SAI_DATASIZE_20) || \
        ((DATASIZE) == SAI_DATASIZE_24) || \
        ((DATASIZE) == SAI_DATASIZE_32))

#define IS_SAI_BLOCK_FIRST_BIT(BIT) (((BIT) == SAI_FIRSTBIT_MSB) || \
                                     ((BIT) == SAI_FIRSTBIT_LSB))

#define IS_SAI_BLOCK_CLOCK_STROBING(CLOCK) (((CLOCK) == SAI_CLOCKSTROBING_FALLINGEDGE) || \
        ((CLOCK) == SAI_CLOCKSTROBING_RISINGEDGE))

#define IS_SAI_BLOCK_SYNCHRO(SYNCHRO) (((SYNCHRO) == SAI_ASYNCHRONOUS)         || \
                                       ((SYNCHRO) == SAI_SYNCHRONOUS)          || \
                                       ((SYNCHRO) == SAI_SYNCHRONOUS_EXT_SAI1) ||\
                                       ((SYNCHRO) == SAI_SYNCHRONOUS_EXT_SAI2))

#define IS_SAI_BLOCK_OUTPUT_DRIVE(DRIVE) (((DRIVE) == SAI_OUTPUTDRIVE_DISABLE) || \
        ((DRIVE) == SAI_OUTPUTDRIVE_ENABLE))

//#define IS_SAI_BLOCK_NODIVIDER(NODIVIDER) (((NODIVIDER) == SAI_MASTERDIVIDER_ENABLE) || \
//                                           ((NODIVIDER) == SAI_MASTERDIVIDER_DISABLE))

#define IS_SAI_BLOCK_MUTE_COUNTER(COUNTER) ((COUNTER) <= 63U)

#define IS_SAI_BLOCK_MUTE_VALUE(VALUE)    (((VALUE) == SAI_ZERO_VALUE)     || \
        ((VALUE) == SAI_LAST_SENT_VALUE))

#define IS_SAI_BLOCK_COMPANDING_MODE(MODE)    (((MODE) == SAI_NOCOMPANDING)         || \
        ((MODE) == SAI_ULAW_1CPL_COMPANDING) || \
        ((MODE) == SAI_ALAW_1CPL_COMPANDING) || \
        ((MODE) == SAI_ULAW_2CPL_COMPANDING) || \
        ((MODE) == SAI_ALAW_2CPL_COMPANDING))

#define IS_SAI_BLOCK_FIFO_THRESHOLD(THRESHOLD) (((THRESHOLD) == SAI_FIFOTHRESHOLD_EMPTY)   || \
        ((THRESHOLD) == SAI_FIFOTHRESHOLD_1QF)     || \
        ((THRESHOLD) == SAI_FIFOTHRESHOLD_HF)      || \
        ((THRESHOLD) == SAI_FIFOTHRESHOLD_3QF)     || \
        ((THRESHOLD) == SAI_FIFOTHRESHOLD_FULL))

#define IS_SAI_BLOCK_TRISTATE_MANAGEMENT(STATE) (((STATE) == SAI_OUTPUT_NOTRELEASED) ||\
        ((STATE) == SAI_OUTPUT_RELEASED))

#define IS_SAI_MONO_STEREO_MODE(MODE) (((MODE) == SAI_MONOMODE) ||\
                                       ((MODE) == SAI_STEREOMODE))

#define IS_SAI_SLOT_ACTIVE(ACTIVE)  ((ACTIVE) <= SAI_SLOTACTIVE_ALL)

#define IS_SAI_BLOCK_SLOT_NUMBER(NUMBER) ((1U <= (NUMBER)) && ((NUMBER) <= 16U))

#define IS_SAI_BLOCK_SLOT_SIZE(SIZE) (((SIZE) == SAI_SLOTSIZE_DATASIZE) || \
                                      ((SIZE) == SAI_SLOTSIZE_16B)      || \
                                      ((SIZE) == SAI_SLOTSIZE_32B))

#define IS_SAI_BLOCK_FIRSTBIT_OFFSET(OFFSET) ((OFFSET) <= 24U)

#define IS_SAI_BLOCK_FS_OFFSET(OFFSET) (((OFFSET) == SAI_FS_FIRSTBIT) || \
                                        ((OFFSET) == SAI_FS_BEFOREFIRSTBIT))

#define IS_SAI_BLOCK_FS_POLARITY(POLARITY) (((POLARITY) == SAI_FS_ACTIVE_LOW) || \
        ((POLARITY) == SAI_FS_ACTIVE_HIGH))

#define IS_SAI_BLOCK_FS_DEFINITION(DEFINITION) (((DEFINITION) == SAI_FS_STARTFRAME) || \
        ((DEFINITION) == SAI_FS_CHANNEL_IDENTIFICATION))

#define IS_SAI_BLOCK_MASTER_DIVIDER(DIVIDER) ((DIVIDER) <= 15U)

#define IS_SAI_BLOCK_FRAME_LENGTH(LENGTH) ((8U <= (LENGTH)) && ((LENGTH) <= 256U))

#define IS_SAI_BLOCK_ACTIVE_FRAME(LENGTH) ((1U <= (LENGTH)) && ((LENGTH) <= 128U))

#define SAIA_DMA_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->CR1, SAI_xCR1_DMAEN))

#define SAIB_DMA_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->CR1, SAI_xCR1_DMAEN))

#define LL_SAI_CPL_2S(DEVICE)           (IO_BIT_SET((DEVICE)->CR2, SAI_xCR2_CPL))

#define LL_SAI_FSPOL_Falling(DEVICE)    (IO_BIT_CLEAR((DEVICE)->FRCR, SAI_xFRCR_FSPOL))

#define LL_SAI_FSOFF_CLR(DEVICE)        (IO_BIT_CLEAR((DEVICE)->FRCR, SAI_xFRCR_FSOFF))

#define LL_SAI_FBOFF_4(DEVICE)          (IO_BIT_SET((DEVICE)->SLOTR, SAI_xSLOTR_FBOFF_2))
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup SAI_Private_Functions SAI Private Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __GM_HAL_SAI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
