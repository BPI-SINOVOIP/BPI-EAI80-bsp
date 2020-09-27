/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          spi.h
 *
 * @author        william.wang
 *
 * @version       1.0.0
 * 
 * @date          2016/06/22
 *    
 * @brief         Header file of spi driver.
 * 
 * @note
 *    2015/06/22, william.wang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_HAL_SPI_H_
#define _GM_HAL_SPI_H_

#include <gm_common.h>
#include <gm_hal_dma.h>
#include <spi/gm_ll_spi_dev.h>

/**
 * @defgroup SPI_MODE  
 * @brief Specify that SPI operates as master or slave device 
 * @{
 */
#define SPI_MASTER_MODE                ((uint8_t)0x0001U)
#define SPI_SLAVE_MODE                 ((uint8_t)0x0000U)
/**
 * @}
 */

/**
 * @defgroup SPI_CLOCK_POLARITY  
 * @brief Specify SPI clock polarity in idle status 
 * @{
 */
#define SPI_CLOCK_POLARITY_LOW         ((uint8_t)0x0000U) /**< spi clock is low  level in idle status */
#define SPI_CLOCK_POLARITY_HIGH        ((uint8_t)0x0001U) /**< spi clock is high level in idle status */
/**
 * @}
 */

/**
 * @defgroup SPI_CLOCK_PHASE  
 * @brief Specify spi clock phase 
 * @{
 */
#define SPI_CLOCK_PHASE_MIDDLE      ((uint8_t)0x0000U) /**< First clock edge at the middle of first data cycle */
#define SPI_CLOCK_PHASE_START       ((uint8_t)0x0001U) /**< First clock edge at the start of first data cycle */
/**
 * @}
 */

/**
 * @defgroup SPI_BIT_ORDER  
 * @brief Specify bit order of spi frame  
 * @{
 */
#define SPI_BIT_ORDER_LSB          ((uint8_t)0x0001U) /**< Least significant bit first */
#define SPI_BIT_ORDER_MSB          ((uint8_t)0x0000U) /**< Most significant bit first */
/**
 * @}
 */ 
   
/**
 * Synchronous serial peripheral communication format
 */
typedef enum
{
    SPI_FORMAT_MOTOLORA = 0,           /**< Motolora/Freescale/Nxp spi protocol */
    SPI_FORMAT_TI_SSP,                 /**< Texas Instruments ssi protocol */
    SPI_FORMAT_MICROWIRE,              /**< Microwire protocol */
} SPI_Format_T;

/**
 * General duplex operation mode
 */
typedef enum
{
    SPI_TX_ONLY = 0,                   /**< Only transmit data */
    SPI_RX_ONLY,                       /**< Only recieve data */
    SPI_HALF_DUPLEX,                   /**< Transmit then receive */
    SPI_FULL_DUPLEX,                   /**< Transmit and receive simultaneously */
} SPI_Duplex_T;

/**
 * General spi controller status
 */
typedef enum 
{
    SPI_STATUS_IDLE = 0,               /**< There is no activity */
    SPI_STATUS_BUSY,                   /**< SPI device is occupied, and operation is still ongoing */
    SPI_STATUS_ERROR,                  /**< SPI device indicates that error occurs during last transaction */
} SPI_Status_T;

/**
 * General spi events
 */
typedef enum 
{
    SPI_EVENT_NONE = 0,                /**< There is nothing happened */
    SPI_EVENT_TRANSFER_COMPLETE = 0,   /**< All data transmitted and received as expected */
    SPI_EVENT_TRANSFER_ERROR,          /**< Error occurs during last transfer */
} SPI_Event_T;

/**
 * General spi interrupts
 */
typedef enum
{
    SPI_RX_INT = 1,                    /**< Enable general spi rx fifo interrupt, shall be enabled to 
                                            get TRANSFER_COMPLETE event in half-duplex, full-duplex, 
                                            reception-only mode*/                  
    SPI_TX_INT = 2,                    /**< Enable general spi tx fifo interrupt, shall be enabled to 
                                            get TRANSFER_COMPLETE event in half-duplex, full-duplex, 
                                            transmission-only mode*/                  
    SPI_RX_DMA_INT = 4,                /**< Enable rx dma over interrupt, shall be enabled if rx dma 
                                            is used. */
    SPI_TX_DMA_INT = 8 ,               /**< Enable tx dma over interrupt, shall be enabled if tx dma 
                                            is used. */	
} SPI_Interrupt_T;

/**
 * @defgroup SPI_FRAME_MASK  
 * Specify spi frames of specific command, including command/address/dummy/data frames
 *  
 * @{
 */
#define SPI_MASK_COMMAND_FRAME         ((uint32_t)SPI_CTRL_XCMD)     /**< There is command frame in transmmited command */
#define SPI_MASK_ADDRESS_FRAME         ((uint32_t)SPI_CTRL_XADDR)    /**< There is address frame in transmmited command */
#define SPI_MASK_DUMMY_FRAME           ((uint32_t)SPI_CTRL_XDMY)     /**< There is dummy frame in transmmited command */
#define SPI_MASK_DATA_FRAME            ((uint32_t)SPI_CTRL_XDATA)    /**< There are data frames in transmmited command */
/**
 * @}
 */
 
/**
 * Configuration of transmitted SPI frames of specific command,  including OR-combined masks of command/address/dummy/data frame,
 * and bits of each valid frame
 */
typedef struct
{
    uint32_t       mask;               /**< OR-combined mask for command/address/dummy/data frame */
    uint8_t        commandBits;        /**< Valid bits of command frame */      
    uint8_t        addressBits;        /**< Valid bits of address frame */       
    uint8_t        dummyBits;          /**< Valid bits of dummy frame */
    uint8_t        dataBits;           /**< Valid bits of data frame */
    uint32_t       command;            /**< Value of command frame */
    uint32_t       address;            /**< Value of address frame */
    uint32_t       dummy;              /**< Value of dummy frame */
} SPI_FrameConfig_T;

/**
 * Configuration of 
 */
typedef struct
{
    SPI_FrameConfig_T frameConfig;     /**< Configure fixed SPI frames, it can replace configuartion of SPI_Init */ 
    uint8_t        duplex;             /**< Duplex mode 0: full; 1: half; 2: tx only; 3: rx only, it can replace configuartion of SPI_Init */ 
    uint8_t        txWatermark;        /**< It is TX fifo threshhold if ring buffer is not enabled, otherwise 
	                                          it is watermark of tx ring buffer. */
    uint8_t        rxWatermark;        /**< It is RX fifo threshhold if ring buffer is not enabled, otherwise 
	                                          it is watermark of rx ring buffer. */
    uint8_t        continuedData;      /**< Multiple data frames */ 
    uint8_t        enableRxDMA;        /**< Move data to ram by dma */
    uint8_t        enableTxDMA;        /**< Read data from ram by dma */
    uint8_t        continuousDataCount;  /**< How many data frames shall be received if continuedDataFrame is valid. */ 
} SPI_TransferConfig_T;


typedef struct __SPI_Handle_T SPI_Handle_T;
    
typedef void (*SPI_Callback_T)(SPI_Handle_T *hSPI, uint8_t event, ulong_t param);

/**
 * General SPI configuration structure.
 */
typedef struct
{	
    uint8_t        	mode;               /**< SPI operates as master device or slave device */
    uint8_t        	format;             /**< SPI foramt: Motorola, TI SSP, Microwire */
    uint8_t        	polarity;           /**< Serial clock idle state */
    uint8_t        	phase;              /**< Serial clock data bit capture phase */
    uint8_t        	bitOrder;           /**< Data transfers start from MSB or LSB bit */
    uint8_t        	singleWire;         /**< Single wire for both MISO and MOSI */
    uint8_t        	reserved[2];        /**< Reserved field */                                       
    uint32_t       	baudRate;           /**< Configure transmit and receive SCK clock */
    SPI_Callback_T  callback;          /**< Callback function */    
} SPI_Config_T;

/**
 * SPI transfer handle structure.
 */
typedef struct __SPI_Handle_T
{
    SPI_Device_T 			*device;             		 /**< Pointer to spi device handle. */
    uint32_t 				fixedFrames[3];           /**< Fixed frames to be transmited, which are configured by TransferConfig */
    uint32_t 				index;                    /**< Index of all spi irq table , dma channel table, ... */               
    uint8_t 				fixedFramesSize;           /**< Size of fixed frames */
    uint8_t 				frameDelay;                /**< Delay frames to receive data */
    uint8_t 				enableTxDMA;               /**< Enable Tx DMA Transfer*/
    uint8_t 				enableRxDMA;               /**< Enable Rx DMA Transfer*/
    DMA_Handle_T 			txDMAHandle;          		/**< Tx DMA Handle */  
    DMA_Handle_T 			rxDMAHandle;          		/**< Rx DMA Handle */      
    uint8_t 				rxDMAStatus;               /**< Rx DMA Status */
    uint8_t 				txDMAStatus;               /**< Tx DMA Status */
    uint8_t 				status;                    /**< Handle Status */ 
    uint8_t 				dataWidth;                 /**< Bytes of each data frame */ 
    uint8_t 				crcount;                   /**< Continuous received data count */ 
    uint8_t 				ctcount;                   /**< Continuous transferred data count */ 
    uint8_t 				rcount;                    /**< Received data count */ 
    uint8_t 				tcount;                    /**< Transferred data count */ 
    uint8_t 				*pTxData;                  /**< Address of remaining data to send. */
    uint8_t 				*pRxData;                  /**< Address of remaining data to receive. */
    uint16_t 				rxExpectedSize;           /**< Expected received size */
    uint16_t 				txExpectedSize;           /**< Expected transmitted size */
    uint16_t 				txSize;                   /**< Size of transmitted data */
    uint16_t 				rxSize;                   /**< Size of received data */
    uint32_t 				interruptMask;            /**< Interrupt mask */
    SPI_Callback_T 			callback;           	  /**< Callback function. */  
} SPI_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/

/**
 * @brief Initializes SPI device with user configuration and associated handle.
 *
 * This function configures one SPI device with the user-defined settings. The user can also get the 
 * default configuration by using the SPI_GetDefaultConfig() function.
 *
 * @param  pSPI            SPI device handle
 * @param  pConfig         Pointer to user-defined configuration.
 * @param  hSPI            SPI handle
 *
 * @retval RET_ERROR       There is invalid configuration not supported 
 * @retval RET_OK          Configure successfully 
 */
RET_CODE HAL_SPI_Init(SPI_Device_T *pSPI, SPI_Config_T *pConfig, SPI_Handle_T *hSPI);


/**
 * @brief Sets SPI_Config_T structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for the SPI_Init().
 * User may modify some configurations before calling SPI_Init(). The function simplify configuration.
 *
 * @param  pConfig         Pointer to SPI_Config_T structure
 * @param  pTransferConfig Pointer to SPI_TransferConfig_T structure
 * @param  mode            Master mode or slave mode
 *
 * @retval                 None
 */
void HAL_SPI_GetDefaultConfig(SPI_Config_T *pConfig, SPI_TransferConfig_T *pTransferConfig, uint8_t mode);

/**
 * @brief  DeInitialize SPI device.
 *
 * @param  hSPI            SPI handle
 *
 * @retval                 None
 */
void HAL_SPI_DeInit(SPI_Handle_T *hSPI);

/**
 * @brief Update spi changeable configurations such as dma transfer, fixed frames, duplex mode, and so on.   
 *
 * This function updates spi configurations about data transferring parameters, including valid fixed frames, duplex mode, 
 * dma engine usage, watermark of fifo, continuous data frames, and continuous data count.
 *
 * @param  hSPI            SPI handle
 * @param  pConfig         Pointer to updated transfer configurations.
 *
 * @retval RET_ERROR       There is invalid configuration not supported 
 * @retval RET_OK          Configure successfully
 */
RET_CODE HAL_SPI_TransferConfig(SPI_Handle_T *hSPI, SPI_TransferConfig_T *pConfig);

/**
 * @brief  Receive amount of data by reception-only mode.
 *
 * @param  hSPI            SPI handle
 * @param  pData           Pointer to data buffer
 * @param  size            Amount of data expected to be received
 * @param  timeout         Timeout duration in microsecond. The special timeout parameter 
 *                         TIMEOUT_WAIT_FOREVER, TIMEOUT_NO_BLOCKING and TIMEOUT_NO_WAIT 
 *                         means blocking, non-blocking and one-shot transaction seperately. 
 *
 * @retval RET_OK          Expected size of data received.
 * @retval RET_BUSY        Device is occupied and busy.
 * @retval RET_TIMEOUT     Rx buffer does not receive expected size of data within timeout 
 *                         duration. 
 */
RET_CODE HAL_SPI_Receive(SPI_Handle_T *hSPI, uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief  Transmit amount of data by transmission-only mode.
 *
 * @param  hSPI            SPI handle
 * @param  pData           Pointer to data buffer
 * @param  size            Amount of data to be sent
 * @param  timeout         Timeout duration in microsecond. The special timeout parameter 
 *                         TIMEOUT_WAIT_FOREVER, TIMEOUT_NO_BLOCKING and TIMEOUT_NO_WAIT 
 *                         means blocking, non-blocking and one-shot transaction seperately. 
 *
 * @retval RET_OK          Expected size of data tranmitted.
 * @retval RET_BUSY        Device is occupied and busy.
 * @retval RET_TIMEOUT     SPI does not transmit expected size of data within 
 *                         timeout duration. 
 */
RET_CODE HAL_SPI_Transmit(SPI_Handle_T *hSPI, uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief Transmit and receive data by half-duplex or full-duplex SPI operation mode.
 *
 * @param  hSPI            SPI handle
 * @param  pTxData         Pointer to transmission data buffer
 * @param  pRxData         Pointer to reception data buffer
 * @param  txSize          Amount of data to be sent
 * @param  rxSize          Amount of data to be received
 * @param  timeout         Timeout duration in microsecond. The special timeout parameter 
 *                         TIMEOUT_WAIT_FOREVER, TIMEOUT_NO_BLOCKING and TIMEOUT_NO_WAIT 
 *                         means blocking, non-blocking and one-shot transaction seperately. 
 *
 * @retval RET_OK          Expected size of data tranmitted and received.
 * @retval RET_BUSY        Device is occupied and busy.
 * @retval RET_TIMEOUT     Rx fifo does not receive expected size of data
 *                         within timeout duration. 
 */
RET_CODE HAL_SPI_TransmitReceive(SPI_Handle_T *hSPI, uint8_t *pTxData, uint8_t *pRxData, uint16_t txSize, uint16_t rxSize, uint32_t timeout);

/**
 * @brief Enables SPI interrupts.
 *
 * This function enables multiple spi interrupts.  
 *
 * @param  hSPI            SPI handle
 * @param  mask            Interrupt mask, can be logical or of members of enum @ref  SPI_Interrupt_T.
 *                    
 * @retval                 None
 */
void HAL_SPI_EnableInterrupts(SPI_Handle_T *hSPI, uint32_t mask);

/**
 * @brief Disable SPI interrupts.
 *
 * This function disables multiple spi interrupts.  
 *
 * @param  hSPI            SPI handle
 * @param  mask            Interrupt mask, can be logical or of members of enum @ref  SPI_Interrupt_T.
 *                    
 * @retval                  None
 */
void HAL_SPI_DisableInterrupts(SPI_Handle_T *hSPI, uint32_t mask);

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

#endif /*_SPI_H_*/
