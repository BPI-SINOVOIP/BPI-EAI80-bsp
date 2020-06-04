/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          dma.h
*
* @author        wizephen.wang
*
* @version       1.0.0
*
* @date          2016/07/04
*
* @brief         Header file of dma driver.
*
* @note
*    2016/07/04, wizephen.wang, V1.0.0
*        Initial version.
*    2017/06/12, evan.wu, V1.0.1
*        Support 6621.
*/

#ifndef _DMA_HAL_H_
#define _DMA_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <dma/gm_ll_dma.h>

typedef struct
{
    __IO uint32_t  SRC_END ; //Source End Pointer
    __IO uint32_t  DES_END ; //Destination End Pointer
    __IO uint32_t  CTRL    ; //Control
    __IO uint32_t  RESERVED; //Unused
} DMA_Channel_Control_T;

typedef struct
{
    DMA_Channel_Control_T  Prim_CH[16] ;    //Primary data struct of all 16 channel
    DMA_Channel_Control_T  Alter_CH[16];    //Alternate data struct of all 16 channel
} DMA_Control_T;


/** Declaration of DMA handle structure */
struct DMA_Handle;

/** Prototype of the user callback function */
typedef void (*DMA_Callback_T)(struct DMA_Handle *hDMA, uint8_t event, unsigned long param);

/** Associate DMA handle with its parent module handle */
#define LINK_DMAHDL_TO_PARENTHDL(PARENTDHL, SUBDMAHDL, DMAHDL)      \
    do{                                         \
        (PARENTDHL)->SUBDMAHDL = &(DMAHDL);   \
        (DMAHDL).pParent = (PARENTDHL);       \
    } while(0)

#define GET_PARENTHDL_FROM_DMAHDL(DMAHDL, PARENTDHL_TYPE)  ((PARENTDHL_TYPE *)(DMAHDL->pParent))


/**
 * DMA channel index
 */
typedef enum
{
    DMA_CHANNEL_0,         /**< DMA channel index 0 */
    DMA_CHANNEL_1,         /**< DMA channel index 1 */
    DMA_CHANNEL_2,         /**< DMA channel index 2 */
    DMA_CHANNEL_3,         /**< DMA channel index 3 */
    DMA_CHANNEL_4,         /**< DMA channel index 4 */
    DMA_CHANNEL_5,         /**< DMA channel index 5 */
    DMA_CHANNEL_6,         /**< DMA channel index 6 */
    DMA_CHANNEL_7,         /**< DMA channel index 7 */
    DMA_CHANNEL_8,         /**< DMA channel index 8 */
    DMA_CHANNEL_9,         /**< DMA channel index 9 */
    DMA_CHANNEL_10,        /**< DMA channel index 10 */
    DMA_CHANNEL_11,        /**< DMA channel index 11 */
    DMA_CHANNEL_12,        /**< DMA channel index 12 */
    DMA_CHANNEL_13,        /**< DMA channel index 13 */
    DMA_CHANNEL_14,        /**< DMA channel index 14 */
    DMA_CHANNEL_15,        /**< DMA channel index 15 */
    DMA_CHANNEL_INVALID,  /**< DMA channel invalid index */
} DMA_ChannelIdx_T;

/**
 * DMA device index
 */
typedef enum
{
    DMA_DEV_0,         /**< DMA device index 0 */
    DMA_DEV_1,         /**< DMA device index 1 */
    DMA_DEV_2,         /**< DMA device index 2 */
    DMA_DEV_3,         /**< DMA device index 3 */
    DMA_DEV_4,         /**< DMA device index 4 */
    DMA_DEV_5,         /**< DMA device index 5 */
    DMA_DEV_6,         /**< DMA device index 6 */
    DMA_DEV_7,         /**< DMA device index 7 */
} DMA_ChnDeviceIdx_T;

/**
 * DMA event type
 */
typedef enum
{
    DMA_EVENT_TRANSFER_DONE   = 0x00,  /**< DMA transaction is completed. */
    DMA_EVENT_TRANSFER_ERROR  = 0x01,   /**< DMA transaction error */
    DMA_EVENT_PINGPONG_PRIM_DONE  = 0x02,  /**< DMA pingpong mode prim is completed. */
    DMA_EVENT_PINGPONG_ALTER_DONE = 0x03,  /**< DMA pingpong mode alter is completed. */
} DMA_Event_T;


/**
 * DMA transaction direction type
 */
typedef enum
{
    DMA_MEMORY_TO_MEMORY = 0x00,  /**< Memory to memory as DMA transaction direction. */
    DMA_MEMORY_TO_PERIPH = 0x01,  /**< Memory to peripheral as DMA transaction direction. */
    DMA_PERIPH_TO_MEMORY = 0x02,  /**< Peripheral to memory as DMA transaction direction. */
} DMA_Direction_T;

/**
 * DMA address increment type
 */
typedef enum
{
    DMA_SRC_INC_DST_INC  = 0x00,   /**< DMA address increment mode */
    DMA_SRC_KEEP_DST_INC = 0x01,   /**< DMA address unchanged mode */
    DMA_SRC_INC_DST_KEEP = 0x02    /**< DMA address unchanged mode */
} DMA_AddrIncrement_T;

/**
 * DMA transaction data width
 */
typedef enum
{
    DMA_WIDTH_8BIT    = 0x00,  /**< 8-Bit transaction width */
    DMA_WIDTH_16BIT   = 0x01,  /**< 16-Bit transaction width */
    DMA_WIDTH_32BIT   = 0x02   /**< 32-Bit transaction width */
} DMA_Width_T;


/**
 * DMA transaction burst
 */
typedef enum
{
    DMA_BURST_1,
    DMA_BURST_2,
    DMA_BURST_4,
    DMA_BURST_8,
} DMA_Burst_T;


typedef enum
{
    DMA_PRIORITY_LOW  = 0x00,     /**< DMA channel priority 0(low) */
    DMA_PRIORITY_HIGH = 0x01,    /**< DMA channel priority 1(high) */
} DMA_ChnPriority_T;

typedef enum
{
    DMA_SCATTER_DISABLE = 0x00,   /**< DMA scatter disable */
    DMA_SCATTER_ENABLE  = 0x01    /**< DMA scatter enable */
} DMA_Scatter_T;

typedef enum
{
    DMA_GATHER_DISABLE = 0x00,   /**< DMA gather disable */
    DMA_GATHER_ENABLE  = 0x01    /**< DMA gather enable */
} DMA_Gather_T;


/**
 * DMA alter type
 */
typedef enum
{
    DMA_PRIMARY  = 0x00,   /**< DMA use primary struct */
    DMA_ALTER    = 0x01,   /**< DMA use alter struct */
    DMA_PINGPONG = 0x02,   /**< DMA use pingpong mode */
} DMA_Alter_T;

#if 0
/**
 * DMA operating mode
 */
typedef enum
{
    DMA_OP_STOP             = 0x00,   /**< DMA sotp mode */
    DMA_OP_BASIC            = 0x01,   /**< DMA basic mode */
    DMA_OP_AUTO_REQ         = 0x02,   /**< DMA auto request mode */
    DMA_OP_PINGPONG         = 0x03,   /**< DMA ping-pong mode */
    DMA_OP_MEM_SCT_GTR_M    = 0x04,   /**< DMA memory scatter gather in primary */
    DMA_OP_MEM_SCT_GTR_A    = 0x05,   /**< DMA memory scatter gather in alter */
    DMA_OP_PER_SCT_GTR_M    = 0x06,   /**< DMA peripheral scatter gather in primary */
    DMA_OP_PER_SCT_GTR_A    = 0x07,   /**< DMA peripheral scatter gather in alter */
} DMA_Operating_Mode_T;
#endif

typedef enum
{
    /* M2M */
    DMA_REQUEST_M2M,
    /* ADC */
    DMA_REQUEST_ADC1,
    DMA_REQUEST_ADC2,
    DMA_REQUEST_ADC3,
    /* DAC */
    DMA_REQUEST_DAC1,
    DMA_REQUEST_DAC2,
    /* UART */
    DMA_REQUEST_UART1_RX,
    DMA_REQUEST_UART1_TX,
    DMA_REQUEST_UART2_RX,
    DMA_REQUEST_UART2_TX,
    DMA_REQUEST_UART3_RX,
    DMA_REQUEST_UART3_TX,
    DMA_REQUEST_UART4_RX,
    DMA_REQUEST_UART4_TX,
    DMA_REQUEST_UART5_RX,
    DMA_REQUEST_UART5_TX,
    DMA_REQUEST_UART6_RX,
    DMA_REQUEST_UART6_TX,
    DMA_REQUEST_UART7_RX,
    DMA_REQUEST_UART7_TX,
    DMA_REQUEST_UART8_RX,
    DMA_REQUEST_UART8_TX,
    /* I2C */
    DMA_REQUEST_I2C1_RX,
    DMA_REQUEST_I2C1_TX,
    DMA_REQUEST_I2C2_RX,
    DMA_REQUEST_I2C2_TX,
    DMA_REQUEST_I2C3_RX,
    DMA_REQUEST_I2C3_TX,
    /* SPI */
    DMA_REQUEST_SPI1_RX,
    DMA_REQUEST_SPI1_TX,
    DMA_REQUEST_SPI2_RX,
    DMA_REQUEST_SPI2_TX,
    DMA_REQUEST_SPI3_RX,
    DMA_REQUEST_SPI3_TX,
    DMA_REQUEST_SPI4_RX,
    DMA_REQUEST_SPI4_TX,
    DMA_REQUEST_SPI5_RX,
    DMA_REQUEST_SPI5_TX,
    DMA_REQUEST_SPI6_RX,
    DMA_REQUEST_SPI6_TX,
    /* TIM */
    DMA_REQUEST_TIM1_CC1D,
    DMA_REQUEST_TIM1_CC2D,
    DMA_REQUEST_TIM1_CC3D,
    DMA_REQUEST_TIM1_CC4D,
    DMA_REQUEST_TIM1_TD,
    DMA_REQUEST_TIM1_UD,
    DMA_REQUEST_TIM1_COMD,
    DMA_REQUEST_TIM2_CC1D,
    DMA_REQUEST_TIM2_CC2D,
    DMA_REQUEST_TIM2_CC3D,
    DMA_REQUEST_TIM2_CC4D,
    DMA_REQUEST_TIM2_TD,
    DMA_REQUEST_TIM2_UD,
    DMA_REQUEST_TIM3_CC1D,
    DMA_REQUEST_TIM3_CC2D,
    DMA_REQUEST_TIM3_CC3D,
    DMA_REQUEST_TIM3_CC4D,
    DMA_REQUEST_TIM3_TD,
    DMA_REQUEST_TIM3_UD,
    DMA_REQUEST_TIM4_CC1D,
    DMA_REQUEST_TIM4_CC2D,
    DMA_REQUEST_TIM4_CC3D,
    DMA_REQUEST_TIM4_CC4D,
    DMA_REQUEST_TIM4_TD,
    DMA_REQUEST_TIM4_UD,
    DMA_REQUEST_TIM5_CC1D,
    DMA_REQUEST_TIM5_CC2D,
    DMA_REQUEST_TIM5_CC3D,
    DMA_REQUEST_TIM5_CC4D,
    DMA_REQUEST_TIM5_TD,
    DMA_REQUEST_TIM5_UD,
    DMA_REQUEST_TIM6_UD,
    DMA_REQUEST_TIM8_CC1D,
    DMA_REQUEST_TIM8_CC2D,
    DMA_REQUEST_TIM8_CC3D,
    DMA_REQUEST_TIM8_CC4D,
    DMA_REQUEST_TIM8_TD,
    DMA_REQUEST_TIM8_UD,
    DMA_REQUEST_TIM8_COMD,
    /* SDIO */
    DMA_REQUEST_SDIO_RX,
    DMA_REQUEST_SDIO_TX,
    /* USB */
    DMA_REQUEST_USB,
    /* HASH */
    DMA_REQUEST_HASH_TX,
    /* DCMI_VIN */
    DMA_REQUEST_DCMI_VIN,
    /* SAI */
    DMA_REQUEST_SAI1_A,
    DMA_REQUEST_SAI1_B,
    /* PDM */
    DMA_REQUEST_PDM_A,
    DMA_REQUEST_PDM_B,
    DMA_REQUEST_PDM_C,
    DMA_REQUEST_PDM_D,
    /* CRYPTO */
    DMA_REQUEST_CRYP_TX,
    DMA_REQUEST_CRYP_RX,
    /* NAND */
    DMA_REQUEST_NAND,
    /* INVALID */
    DMA_REQUEST_INVALID,      /**< Invalid DMA request */
} DMA_RequestType_T;

typedef enum
{
    DMA_STATE_INITIAL = 0x00,   /**< DMA is in initial state, and not initialized yet */
    DMA_STATE_READY   = 0x01,   /**< DMA is initialized and ready to use */
    DMA_STATE_BUSY    = 0x02,   /**< DMA process busy on transferring data */
    DMA_STATE_ERROR   = 0x03    /**< DMA error */
} DMA_State_T;


typedef struct
{
    DMA_ChannelIdx_T   chIdx;
    DMA_ChnDeviceIdx_T devIdx;
    DMA_RequestType_T  requestType;
} DMA_Channel_Info_T;

typedef struct
{
    DMA_ChannelIdx_T   channelIdx;
    DMA_ChnDeviceIdx_T deviceIdx;
} DMA_Request_Result_T;

/**
 * DMA configuraion structure
 */
typedef struct
{
    DMA_ChannelIdx_T    channelIdx;          /**< DMA channel index */
    DMA_ChnDeviceIdx_T  deviceIdx;           /**< DMA device index of channel */

    DMA_Direction_T     direction;           /**< DMA transaction direction */

    DMA_AddrIncrement_T increment;           /**< DMA address increment mode */

    DMA_Width_T         width;               /**< DMA transactin width */

    DMA_Burst_T         burst;               /**< DMA transactin burst */

    DMA_ChnPriority_T   priority;         /**< DMA channel priority */

    DMA_Alter_T         alter;               /**< DMA use alter or primary */

    //DMA_Operating_Mode_T operatingMode;      /**< DMA operating mode */

#if 0
    DMA_Gather_T        srcGather;           /**< DMA source gather */
    uint32_t            srcGatherInterval;   /**< DMA source gather interval */
    uint32_t            srcGatherCount;      /**< DMA source gather count */
    DMA_Scatter_T       destScatter;         /**< DMA dest scatter */
    uint32_t            destScatterInterval; /**< DMA dest gather interval */
    uint32_t            destScatterCount;    /**< DMA dest gather count */

    uint8_t             pesEnable;           /**< DMA pes mode enable */
    uint32_t            pesChnIndex;         /**< DMA pes channel index */
#endif

    DMA_Callback_T      callback;            /**< DMA transaction callback */
} DMA_Config_T;

/**
 * DMA tansaction handler
 */
typedef struct DMA_Handle
{
    DMA_Device_T       *pDevice;     /**< DMA device handler */
    DMA_ChannelIdx_T    chIdx;/**< DMA handshake channel index */
    DMA_ChnDeviceIdx_T  devIdx;   /**< DMA channel_x dev_n */
    DMA_Direction_T     direction;    /**< DMA transaction direction */
    DMA_AddrIncrement_T increment;           /**< DMA address increment mode */
    DMA_Width_T         width;       /**< DMA transactin width */
    //uint8_t             pesEnable;   /**< DMA pes mode enable */
    //uint32_t            pesChnIndex; /**< DMA pes channel index */
    DMA_Alter_T         alter;       /**< DMA use alter or primary */
    DMA_State_T         state;       /**< DMA state */
    void               *pParent;     /**< A pointer to IP module corresponding to DMA transaction, I2C0, etc*/
    DMA_Callback_T      callback;    /**< DMA transaction callback */
    uint32_t            size;         /**< DMA trans size */
} DMA_Handle_T;

/**
 * @brief Request DMA channel with specified parameters.
 *
 * @param  pDMA         Pointer to DMA device handler.
 * @param  pConfig      Pointer to DMA configuration structure.
 * @param  channelIdx   DMA channelIdx which can be used.
 *
 * @retval RET_OK       Api was successfully executed.
 * @retval RET_BUSY     DMA peripheral is being used.
 * @retval RET_INVPARAM Invalid parameter.
 */
RET_CODE HAL_DMA_Request_Channel(DMA_Device_T *pDMA, DMA_RequestType_T reqType, DMA_Request_Result_T *reqResult);

/**
 * @brief Initialize DMA peripheral with specified parameters.
 *
 * @param  pDMA         Pointer to DMA device handler.
 * @param  pConfig      Pointer to DMA configuration structure.
 * @param  hDMA         Pointer to DMA transaction handler.
 *
 * @retval RET_OK       Api was successfully executed.
 * @retval RET_BUSY     DMA peripheral is being used.
 * @retval RET_INVPARAM Invalid parameter.
 */
RET_CODE HAL_DMA_Init(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA);

/**
 * @brief DeInitializes the DMA peripheral.
 *
 * @param  hDMA     Pointer to DMA transaction handle.
 *
 * @retval RET_OK   Always return RET_OK.
 */
RET_CODE HAL_DMA_Abort(DMA_Handle_T *hDMA);

/**
 * @brief Starts the DMA Transfer.
 *
 * @param  hDMA         Pointer to DMA transaction handle.
 * @param  srcAddress   DMA source address.
 * @param  destAddress  DMA destination address.
 * @param  size         DMA data transcation length.
 *
 * @retval RET_OK       Always return RET_OK.
 */
RET_CODE HAL_DMA_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t size);


/**
 * @brief Starts the DMA Transfer(PingPong/Alter).
 *
 * @param  hDMA         Pointer to DMA transaction handle.
 * @param  srcAddress   DMA source address.
 * @param  destAddress  DMA destination address.
 * @param  size         DMA data transcation length.
 *
 * @retval RET_OK       Always return RET_OK.
 */
RET_CODE HAL_DMA_GetDefaultConfig(DMA_Device_T *pDMA, DMA_Config_T *pConfig);


/**
 * @brief Starts the DMA Transfer(PingPong/Alter).
 *
 * @param  hDMA         Pointer to DMA transaction handle.
 * @param  srcAddress   DMA source address.
 * @param  destAddress  DMA destination address.
 * @param  size         DMA data transcation length.
 *
 * @retval RET_OK       Always return RET_OK.
 */
RET_CODE HAL_DMA_MultiBuffer_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t SecondMemAddress, uint32_t size);

void DMA_IRQHandler(void);


#ifdef __cplusplus
}
#endif

#endif /*_DMA_HAL_H_*/
