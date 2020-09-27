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

#ifndef _DMA_H_
#define _DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#ifdef CHIP_GM6621
#include <drv/dev/pes.h>
#endif


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
 * DMA event type
 */
typedef enum
{
    DMA_EVENT_TRANSFER_DONE   = 0x00,  /**< DMA transaction is completed. */
    DMA_EVENT_SRCTRANS_CMPLT  = 0x01,  /**< DMA source transaction is completed. */
    DMA_EVENT_DESTTRANS_CMPLT = 0x02,  /**< DMA destination transaction is completed. */
    DMA_EVENT_BLKTRANS_CMPLT  = 0x03,  /**< DMA block transaction is completed. */
    DMA_EVENT_TRANSFER_ERROR  = 0x04   /**< DMA transaction error */
} DMA_Event_T;

/**
 * DMA Handshaking type
 */
typedef enum
{
    DMA_HARDWARE_HANDSHAKE = 0x00,  /**< DMA uses hardware handshaking. */
    DMA_SOFTWARE_HANDSHAKE = 0x01   /**< DMA uses software handshaking. */
} DMA_HSMode_T;

/**
 * DMA transaction direction type
 */
typedef enum
{
    DMA_MEMORY_TO_MEMORY = 0x00,  /**< Memory to memory as DMA transaction direction. */
    DMA_MEMORY_TO_PERIPH = 0x01,  /**< Memory to peripheral as DMA transaction direction. */
    DMA_PERIPH_TO_MEMORY = 0x02,  /**< Peripheral to memory as DMA transaction direction. */
    DMA_PERIPH_TO_PERIPH = 0x03   /**< Peripheral to Peripheral as DMA transaction direction. */
} DMA_Direction_T;

/**
 * DMA address increment type
 */
typedef enum
{
    DMA_ADDR_INCREMENT  = 0x00,   /**< DMA address increment mode */
    DMA_ADDR_DECREMENT  = 0x01,   /**< DMA address decrement mode */
    DMA_ADDR_CONSTANT   = 0x02    /**< DMA address unchanged mode */
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
 * DMA hardware handshaking channel
 */
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
typedef enum
{
    DMA_I2C0_RX_CHANNEL  = 0x0000,  /**< DMA channel specified for I2C0 Rx */
    DMA_I2C0_TX_CHANNEL  = 0x0001,  /**< DMA channel specified for I2C0 Tx */
    DMA_I2C1_RX_CHANNEL  = 0x0002,  /**< DMA channel specified for I2C1 Rx */
    DMA_I2C1_TX_CHANNEL  = 0x0003,  /**< DMA channel specified for I2C1 Tx */
    DMA_SPI_RX_CHANNEL   = 0x0004,  /**< DMA channel specified for SPI Rx */
    DMA_SPI_TX_CHANNEL   = 0x0005,  /**< DMA channel specified for SPI Tx */
    DMA_ADC_RX_CHANNEL   = 0x0006,  /**< DMA channel specified for ADC Rx */
    DMA_ADC_TX_CHANNEL   = 0x0007,  /**< DMA channel specified for ADC Tx */
    DMA_I2C2_RX_CHANNEL  = 0x0008,  /**< DMA channel specified for I2C2 Rx */
    DMA_I2C2_TX_CHANNEL  = 0x0009,  /**< DMA channel specified for I2C2 Tx */
    DMA_UART0_RX_CHANNEL = 0x000A,  /**< DMA channel specified for UART0 Rx */
    DMA_UART0_TX_CHANNEL = 0x000B,  /**< DMA channel specified for UART0 Tx */
    DMA_UART1_RX_CHANNEL = 0x000C,  /**< DMA channel specified for UART1 Rx */
    DMA_UART1_TX_CHANNEL = 0x000D,  /**< DMA channel specified for UART1 Tx */
    DMA_UART2_RX_CHANNEL = 0x000E,  /**< DMA channel specified for UART2 Rx */
    DMA_UART2_TX_CHANNEL = 0x000F,  /**< DMA channel specified for UART2 Tx */
    DMA_INVALID_CHANNEL  = 0xFFFF   /**< Invalid DMA channel  */
} DMA_HwHSChannel_T;
#endif

#if defined(CHIP_GM6621)

/**
 * DMA channel index
 */
typedef enum
{
    DMA_CHANNEL0,         /**< DMA channel index 0 */
    DMA_CHANNEL1,         /**< DMA channel index 1 */
    DMA_CHANNEL2,         /**< DMA channel index 2 */
    DMA_CHANNEL3,         /**< DMA channel index 3 */
    DMA_CHANNEL4,         /**< DMA channel index 4 */
    DMA_CHANNEL_INVALID,  /**< DMA channel invalid index */
} DMA_HsChannelIdx_T;

typedef enum
{
    /* CHANNEL 4 */
    DMA_ADC_RX_TO_CHANNEL4 = 0,  /**< ADC_RX map to channel 4 */
    DMA_UART1_TX_TO_CHANNEL4,    /**< UART1_TX map to channel 4 */
    DMA_I2C0_TX_TO_CHANNEL4,     /**< I2C0_TX map to channel 4 */
    DMA_I2C1_TX_TO_CHANNEL4,     /**< I2C1_TX map to channel 4 */
    DMA_SPI0_TX_TO_CHANNEL4,     /**< SPI0_TX map to channel 4 */
    DMA_SPI1_TX_TO_CHANNEL4,     /**< SPI1_TX map to channel 4 */
    DMA_ETM2CH3_TX_TO_CHANNEL4,  /**< ETM2CH3_TX map to channel 4 */
    /* CHANNEL 3 */
    DMA_UART0_TX_TO_CHANNEL3,    /**< UART0_TX map to channel 3 */
    DMA_UART1_RX_TO_CHANNEL3,    /**< UART1_RX map to channel 3 */
    DMA_UART2_TX_TO_CHANNEL3,    /**< UART2_TX map to channel 3 */
    DMA_I2C0_RX_TO_CHANNEL3,     /**< I2C0_RX map to channel 3 */
    DMA_I2C1_RX_TO_CHANNEL3,     /**< I2C1_RX map to channel 3 */
    DMA_SPI0_RX_TO_CHANNEL3,     /**< SPI0_RX map to channel 3 */
    DMA_SPI1_RX_TO_CHANNEL3,     /**< SPI1_RX map to channel 3 */
    DMA_ETM22CH1_TX_TO_CHANNEL3, /**< ETM22CH1_TX map to channel 3 */
    DMA_ETM2CH2_TX_TO_CHANNEL3,  /**< ETM2CH2_TX map to channel 3 */
    /* CHANNEL 2 */
    DMA_UART0_RX_TO_CHANNEL2,    /**< UART0_RX map to channel 2 */
    DMA_UART1_TX_TO_CHANNEL2,    /**< UART1_TX map to channel 2 */
    DMA_UART2_RX_TO_CHANNEL2,    /**< UART2_RX map to channel 2 */
    DMA_I2C1_TX_TO_CHANNEL2,     /**< I2C1_TX map to channel 2 */
    DMA_SPI0_RX_TO_CHANNEL2,     /**< SPI0_RX map to channel 2 */
    DMA_ETM22CH0_TX_TO_CHANNEL2, /**< ETM22CH0_TX map to channel 2 */
    /* CHANNEL 1 */
    DMA_UART0_TX_TO_CHANNEL1,    /**< UART0_TX map to channel 1 */
    DMA_UART1_RX_TO_CHANNEL1,    /**< UART1_RX map to channel 1 */
    DMA_I2C0_TX_TO_CHANNEL1,     /**< I2C0_TX map to channel 1 */
    DMA_I2C1_RX_TO_CHANNEL1,     /**< I2C1_RX map to channel 1 */
    DMA_SPI0_TX_TO_CHANNEL1,     /**< SPI0_TX map to channel 1 */
    DMA_SPI1_TX_TO_CHANNEL1,     /**< UART0_TX map to channel 1 */
    DMA_ETM21CH1_TX_TO_CHANNEL1, /**< ETM21CH1_TX map to channel 1 */
    DMA_ETM2CH1_TX_TO_CHANNEL1,  /**< ETM2CH1_TX map to channel 1 */
    /* CHANNEL 0 */
    DMA_ADC_TX_TO_CHANNEL0,      /**< ADC_TX map to channel 0 */
    DMA_UART0_RX_TO_CHANNEL0,    /**< UART0_RX map to channel 0 */
    DMA_I2C0_RX_TO_CHANNEL0,     /**< I2C0_RX map to channel 0 */
    DMA_SPI1_RX_TO_CHANNEL0,     /**< SPI1_RX map to channel 0 */
    DMA_ETM21CH0_TX_TO_CHANNEL0, /**< ETM21CH0_TX map to channel 0 */
    DMA_ETM2CH0_TX_TO_CHANNEL0,  /**< ETM2CH0_TX map to channel 0 */
    /* M2M */
    DMA_M2M_CHANNEL0,            /**< M2M case map to channel 0 */
    DMA_M2M_CHANNEL1,            /**< M2M case map to channel 1 */
    DMA_M2M_CHANNEL2,            /**< M2M case map to channel 2 */
    DMA_M2M_CHANNEL3,            /**< M2M case map to channel 3 */
    DMA_M2M_CHANNEL4,            /**< M2M case map to channel 4 */
    /* INVALID */
    DMA_HSCHANNEL_INVALID,       /**< Invalid channel map */
} DMA_HsChannel_T;

typedef enum
{
    /* ADC */
    DMA_REQUEST_ADC_TX,       /**< ADC_TX DMA request */
    DMA_REQUEST_ADC_RX,       /**< ADC_RX DMA request */
    /* UART */
    DMA_REQUEST_UART0_TX,     /**< UART0_TX DMA request */
    DMA_REQUEST_UART0_RX,     /**< UART0_RX DMA request */
    DMA_REQUEST_UART1_TX,     /**< UART1_TX DMA request */
    DMA_REQUEST_UART1_RX,     /**< UART1_RX DMA request */
    DMA_REQUEST_UART2_TX,     /**< UART2_TX DMA request */
    DMA_REQUEST_UART2_RX,     /**< UART2_RX DMA request */
    /* I2C */
    DMA_REQUEST_I2C0_TX,      /**< I2C0_TX DMA request */
    DMA_REQUEST_I2C0_RX,      /**< I2C0_RX DMA request */
    DMA_REQUEST_I2C1_TX,      /**< I2C1_TX DMA request */
    DMA_REQUEST_I2C1_RX,      /**< I2C1_RX DMA request */
    /* SPI */
    DMA_REQUEST_SPI0_TX,      /**< SPI0_TX DMA request */
    DMA_REQUEST_SPI0_RX,      /**< SPI0_RX DMA request */
    DMA_REQUEST_SPI1_TX,      /**< SPI1_TX DMA request */
    DMA_REQUEST_SPI1_RX,      /**< SPI1_RX DMA request */
    /* ETM */
    DMA_REQUEST_ETM2CH0_TX,   /**< ETM2CH0_TX DMA request */
    DMA_REQUEST_ETM2CH1_TX,   /**< ETM2CH1_TX DMA request */
    DMA_REQUEST_ETM2CH2_TX,   /**< ETM2CH2_TX DMA request */
    DMA_REQUEST_ETM2CH3_TX,   /**< ETM2CH3_TX DMA request */
    DMA_REQUEST_ETM21CH0_TX,  /**< ETM21CH0_TX DMA request */
    DMA_REQUEST_ETM21CH1_TX,  /**< ETM21CH1_TX DMA request */
    DMA_REQUEST_ETM22CH0_TX,  /**< ETM22CH0_TX DMA request */
    DMA_REQUEST_ETM22CH1_TX,  /**< ETM22CH1_TX DMA request */
    /* M2M */
    DMA_REQUEST_M2M,          /**< M2M DMA request */
    /* INVALID */
    DMA_REQUEST_INVALID,      /**< Invalid DMA request */
} DMA_RequestType_T;

typedef enum
{
    DMA_NORMAL_MODE = 0x00,   /**< DMA normal mode */
    DMA_RELOAD_MODE = 0x01    /**< DMA reload mode */
} DMA_Mode_T;

typedef enum
{
    DMA_CHN_PRIORITY_0 = 0x00,    /**< DMA channel priority 0 */
    DMA_CHN_PRIORITY_1 = 0x01,    /**< DMA channel priority 1 */
    DMA_CHN_PRIORITY_2 = 0x02,    /**< DMA channel priority 2 */
    DMA_CHN_PRIORITY_3 = 0x03,    /**< DMA channel priority 3 */
    DMA_CHN_PRIORITY_4 = 0x04,    /**< DMA channel priority 4 */
    DMA_CHN_PRIORITY_5 = 0x05,    /**< DMA channel priority 5 */
    DMA_CHN_PRIORITY_6 = 0x06,    /**< DMA channel priority 6 */
    DMA_CHN_PRIORITY_7 = 0x07,    /**< DMA channel priority 7 */
    DMA_CHN_PRIORITY_INVALIE,     /**< DMA channel priority invalid */
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

#endif

/**
 * DMA state type
 */
typedef enum
{
    DMA_STATE_INITIAL = 0x00,   /**< DMA is in initial state, and not initialized yet */
    DMA_STATE_READY   = 0x01,   /**< DMA is initialized and ready to use */
    DMA_STATE_BUSY    = 0x02,   /**< DMA process busy on transferring data */
    DMA_STATE_ERROR   = 0x03    /**< DMA error */
} DMA_State_T;

/**
 * DMA configuraion structure
 */
typedef struct
{
    DMA_HSMode_T        destHandshake;       /**< DMA handshaking for destination */
    DMA_HSMode_T        srcHandshake;        /**< DMA handshaking for source */

    DMA_Direction_T     direction;           /**< DMA transaction direction */

    DMA_AddrIncrement_T srcIncrement;        /**< DMA address increment for source */
    DMA_AddrIncrement_T destIncrement;       /**< DMA address increment for destination */

#if defined(CHIP_GM6621)
    DMA_RequestType_T   requestType;         /**< DMA request type */
    DMA_RequestType_T   requestType2;        /**< DMA request type, only use for P2P mode as dest peripheral */
    DMA_ChnPriority_T   chnPriority;         /**< DMA channel priority */
    DMA_Mode_T          srcReload;           /**< DMA source reload mode */
    DMA_Mode_T          destReload;          /**< DMA dest reload mode */
    DMA_Gather_T        srcGather;           /**< DMA source gather */
    uint32_t            srcGatherInterval;   /**< DMA source gather interval */
    uint32_t            srcGatherCount;      /**< DMA source gather count */
    DMA_Scatter_T       destScatter;         /**< DMA dest scatter */
    uint32_t            destScatterInterval; /**< DMA dest gather interval */
    uint32_t            destScatterCount;    /**< DMA dest gather count */
    uint8_t             pesEnable;           /**< DMA pes mode enable */
    PES_ChannelIndex_T  pesChnIndex;         /**< DMA pes channel index */
#elif defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    DMA_HwHSChannel_T   srcHwHSChannel;      /**< DMA hardware handshaking channel for source*/
    DMA_HwHSChannel_T   destHwHSChannel;     /**< DMA hardware handshaking channel for destination*/
#endif

    DMA_Width_T         srcWidth;            /**< DMA transactin width for source */
    DMA_Width_T         destWidth;           /**< DMA transactin width for destination */

    DMA_Callback_T      callback;            /**< DMA transaction callback */
} DMA_Config_T;

/**
 * DMA tansaction handler
 */
typedef struct DMA_Handle
{
    DMA_Device_T       *pDevice;     /**< DMA device handler */
#ifdef CHIP_GM6621
    DMA_HsChannelIdx_T  hsChannelIdx;/**< DMA handshake channel index */
    DMA_HsChannel_T     hsChannel;   /**< DMA handshake channel */
    DMA_HsChannel_T     hsChannel2;  /**< DMA handshake channel, only use for P2P mode as dest peripheral */
    uint8_t             pesEnable;   /**< DMA pes mode enable */
    PES_ChannelIndex_T  pesChnIndex;         /**< DMA pes channel index */
#endif
    DMA_State_T         state;       /**< DMA state */
    void               *pParent;     /**< A pointer to IP module corresponding to DMA transaction, I2C0, etc*/
    DMA_Width_T         srcWidth;    /**< DMA transactin width for source */
    DMA_Width_T         destWidth;   /**< DMA transactin width for destination */
    DMA_Callback_T      callback;    /**< DMA transaction callback */
} DMA_Handle_T;

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
RET_CODE DMA_Init(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA);

/**
 * @brief DeInitializes the DMA peripheral.
 *
 * @param  hDMA     Pointer to DMA transaction handle.
 *
 * @retval RET_OK   Always return RET_OK.
 */
RET_CODE DMA_DeInit(DMA_Handle_T *hDMA);

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
RET_CODE DMA_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t size);

/**
 * @brief  Get DMA default configuration
 *
 * @param  pDMA         Pointer to DMA device handler.
 * @param  pConfig      Pointer to the storage of DMA default configuration.
 *
 * @retval RET_OK       Always return RET_OK.
 */
RET_CODE DMA_GetDefaultConfig(DMA_Device_T *pDMA, DMA_Config_T *pConfig);

#ifdef __cplusplus
}
#endif

#endif /*_DMA_H_*/
