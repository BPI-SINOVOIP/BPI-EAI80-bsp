/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart.h
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Header file of uart driver.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_HAL_UART_H_
#define _GM_HAL_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <uart/gm_ll_uart_dev.h>
#include <gm_hal_dma.h>


/*Declaration of UART handle structure*/
struct UART_Handle;

/**
 * @defgroup UART_STOPBITS_COUNT
 * @brief Specify UART stop bit count
 * @{
 */
#define UART_STOPBITS_1                     ((uint32_t)0x0000)
#define UART_STOPBITS_2                     ((uint32_t)0x0001)
#define UART_STOPBITS_1_5                   ((uint32_t)0x0002)
/**
 * @}
 */

/**
 * @defgroup UART_DATABITS_COUNT
 * @brief Specify UART data bit count
 * @{
 */
#define UART_DATABITS_5                     ((uint32_t)0x0000)
#define UART_DATABITS_6                     ((uint32_t)0x0001)
#define UART_DATABITS_7                     ((uint32_t)0x0002)
#define UART_DATABITS_8                     ((uint32_t)0x0003)
#define UART_DATABITS_9                     ((uint32_t)0x0004)
/**
 * @}
 */

/**
 * @defgroup UART_INDEX_COUNT
 * @brief Specify UART index conunt
 * @{
 */
#define UART_INDEX0                         0
#define UART_INDEX1                         1
#define UART_INDEX2                         2
/**
 * @}
 */

/**
 * General uart events
 */
typedef enum
{
    UART_EVENT_NONE           = 0,   /**< There is nothing happened. */
    UART_EVENT_TX_EMPTY       = 1,   /**< Transmit one character over(Holding register empty)
                                          in single-byte mode. */
    UART_EVENT_RX_CHAR        = 2,   /**< Receive one valid character in single-byte mode, param
                                          is the received character. */
    UART_EVENT_RX_ERROR       = 4,   /**< Rx error indication, maybe parity error, frame error... */
    UART_EVENT_TX_OVER        = 8,   /**< Transmission is over, that is to say all user data
                                          is pushed into TX FIFO */
    UART_EVENT_RX_OVER        = 16,  /**< All expected data received and pushed into user buffer. */
    UART_EVENT_RXFIFO_TIMEOUT = 32,  /**< No rx fifo operation occurs during continuous 4
                                          character time, param is received data size. */
    UART_EVENT_TX_WATERMARK   = 64,  /**< It is tx fifo threshhold notification, param is remained
                                          data size in tx fifo. */
    UART_EVENT_RX_WATERMARK   = 128, /**< It is rx fifo threshhold notification, param is received
                                          data size in rx fifo. */
} UART_Event_T;


/**
 * General uart interrupts
 */
typedef enum
{
    UART_RX_INT = 1,                   /**< General UART rx interrupt, shall be enabled to
                                            get RX_ERROR/RX_OVER/RX_CHAR events */
    UART_TX_INT = 2,                   /**< General UART tx interrupt, shall be enabled to
                                            get TX_EMPTY/TX_OVER events */
    UART_RX_FIFO_INT = 4,              /**< UART rx fifo interrupt, shall be enabled to get
                                            RXFIFO_TIMEOUT or RX_WATERMARK events */
    UART_TX_FIFO_INT = 8,              /**< UART tx fifo interrupt, shall be enabled to get
                                            TX_WATERMARK event */
    UART_RX_DMA_INT = 16,              /**< UART rx dma interrupt    */
    UART_TX_DMA_INT = 32,              /**< UART tx dma interrupt    */
} UART_Interrupt_T;


#if 0 //maybe wrong by zhengfeng 
#define UART_RX_INT           (UART_IE_RX | UART_IE_RXTM | UART_IE_RLS) /**< General UART rx interrupt, shall be enabled to
                                                                             get RX_ERROR/RX_OVER/RX_CHAR events */
#define UART_TX_INT           (UART_IE_THRE)                            /**< General UART tx interrupt, shall be enabled to
                                                                             get TX_EMPTY/TX_OVER events */
#define UART_RX_FIFO_INT      (UART_IE_RFTH)                            /**< UART rx fifo interrupt, shall be enabled to get
                                                                             RXFIFO_TIMEOUT or RX_WATERMARK events */
#define UART_TX_FIFO_INT      (UART_IE_TFTH)                            /**< UART tx fifo interrupt, shall be enabled to get
                                                                             TX_WATERMARK event */
#endif

#define MAX_UART_TX_FIFO_SIZE                    16
#define MAX_UART_RX_FIFO_SIZE                    16

/**
 * General uart work mode
 */
typedef enum
{
    UART_MODE_RX = 0,                /**< Receive characters allowed */
    UART_MODE_TX,                    /**< Transmit characters allowed */
    UART_MODE_DISABLE,               /**< Recieve and transmit characters not allowed */
    UART_MODE_DUPLEX,                /**< Recieve and transmit characters both allowed */
} UART_Mode_T;

/**
 * General uart parity types
 */
typedef enum
{
    UART_PARITY_ODD = 0,             /**< Check data by odd parity */
    UART_PARITY_EVEN,                /**< Check data by even parity */
    UART_PARITY_NONE                 /**< No parity bit */
} UART_Parity_T;

/**
 * General receiver line status
 */
typedef enum
{
    UART_FRAME_ERROR      = 1,       /**< Stop bit check fails */
    UART_PARITY_ERROR     = 2,       /**< Parity check fails */
    UART_OVERRUN_ERROR    = 4,       /**< Rx fifo/buffer overrun error */
    UART_BREAK_CHARACTER  = 8,       /**< Break character detected */
    UART_TRANSMITTER_IDLE = 16,      /**< All data transmitted, and tx line in idle status */
    UART_FIFO_ERROR       = 32,      /**< At least there is one error in rx fifo */
} UART_LineStatus_T;

/**
 * @brief Callback of uart interrupt handler to notify user an event along with parameters.
 *
 * @param hUART   Pointer to transaction handle, it maybe NULL if no handle is defined.
 * @param event   Uart event, it can be one of enum @ref  UART_Event_T.
 * @param param   Parameter of uart event, it is the received char if the event is UART_EVENT_RX_CHAR.
 *
 * @retval        None.
 */
typedef void (*UART_Callback_T)(struct UART_Handle *hUART, uint8_t event, ulong_t param);

/**
 * General uart configuration structure.
 */
typedef struct
{
    uint32_t        baudRate;         /**< UART baudrate */
    UART_Parity_T   parity;           /**< UART parity */
    uint8_t         wordLength;       /**< Data bits transmitted in one frame */
    uint8_t         stopBits;         /**< Stop bits between frames */
    uint8_t         reserved[2];
    UART_Callback_T callback;
} UART_Config_T;

/**
 * Configuration of transfer parameters
 */
typedef struct
{
    uint8_t        enableFifo;         /**< Enable rx/tx fifo or not */
    uint8_t        txWatermark;        /**< Tx fifo threshold */
    uint8_t        rxWatermark;        /**< Rx fifo threshold in */
    uint8_t        enableRxDMA;        /**< Move data to ram by dma */
    uint8_t        enableTxDMA;        /**< Read data from ram by dma */
} UART_TransferConfig_T;

/**
 * Configuration of transfer parameters
 */
typedef struct
{
    uint16_t       receivedSize;       /**< Received data size  */
    uint16_t       toBeTransmitedSize; /**< Size of to-be-transmiited data */
    uint32_t       lineStatus;         /**< Receive line status */
} UART_TransferStatus_T;

/**
 * Uart user handle structure.
 */
typedef struct UART_Handle
{
    UART_Device_T *device;             /**< Pointer to uart device handle */
    uint32_t IRQn;                     /**< uart irq number */
    uint32_t status;                   /**< User transaction status */
    uint16_t enableFifo;               /**< Enable rx/rx fifo */
    uint8_t  txWatermark;              /**< Watermark of tx fifo */
    uint8_t  rxWatermark;              /**< Watermark of rx fifo */
    uint8_t  enableRxDMA;              /**< Move data to ram by dma */
    uint8_t  enableTxDMA;              /**< Read data from ram by dma */
    uint8_t  rxDMAStatus;              /**< Rx DMA Status */
    uint8_t  txDMAStatus;              /**< Tx DMA Status */
    DMA_Handle_T RxDMAHandle;          /**< Rx dma handle */
    DMA_Handle_T TxDMAHandle;          /**< Tx dma handle */
    uint8_t *pTxData;                  /**< Address of remaining data to send. */
    uint16_t tcount;                   /**< One-shot transmiited count */
    uint16_t txExpectedSize;           /**< Size of the remaining data to send. */
    uint16_t txSize;                   /**< Size of the data to send out. */
    uint8_t *pRxData;                  /**< Address of remaining data to receive. */
    uint16_t rxExpectedSize;           /**< Size of the remaining data to receive. */
    uint16_t rxSize;                   /**< Size of the data to receive. */
    uint16_t rcount;                   /**< One-shot received count */
    UART_Callback_T callback;          /**< Callback definition to notify user in interrupt handler */
} UART_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/

/**
 * @brief Initializes UART device and uart handle with user configuration.
 *
 * This function configures one UART device with the user-defined settings. The transaction
 * handle is also initialized if it is not NULL.
 * Example code:
 * @code
 *  UART_Config_T uartConfig;
 *
 *  uartConfig.baudRate = 115200U;
 *  uartConfig.parity =   UART_PARITY_EVEN;
 *  uartConfig.stopBits = UART_STOPBITS_1;
 *  UART_Init(UART0, &uartConfig, &g_uartHandle);
 * @endcode
 *
 * @param  pUART          UART device handle
 * @param  pConfig        Pointer to user-defined configuration
 * @param  hUART          Uart handle
 *
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE HAL_UART_Init(UART_Handle_T *hUART, UART_Device_T *pUART, UART_Config_T *pConfig);

/**
 * @brief Deinitializes UART device.
 *
 * This function waits till uart is in idle, then deinitializes UART device, such as disables all
 * interrupts, disables rx/tx, gates off uart source clock,etc.
 *
 * @param  hUART          UART handle
 *
 * @retval                None
 */
RET_CODE HAL_UART_DeInit(UART_Handle_T *hUART);

/**
 * @brief  Disables or enables uart tx/rx function.
 *
 * This function enables/disables uart tx/rx function.
 *
 * @param  hUART          UART handle
 * @param  mode           UART work mode
 *
 * @retval                None
 */

void HAL_UART_SetMode(UART_Device_T *pUART, UART_Mode_T mode);

/**
 * @brief Transmit one character by uart tx bus.
 *
 * This function pushes one character into uart data register, it will wait if tx holding register
 * is not empty.
 *
 * @param  pUART          UART device handle.
 * @param  character      Character to be transmitted.
 *
 * @retval                None
 */
void HAL_UART_PutChar(UART_Device_T *pUART, uint8_t ch);

/**
 * @brief  Receive one character from uart data register.
 *
 * This function gets one character from data register, it will not wait even if no data received.
 *
 * @param  pUART          UART device handler
 *
 * @retval                Received character from uart data register.
 */
uint16_t HAL_UART_GetChar(UART_Device_T *pUART);

/**
 * @brief Enables UART interrupts.
 *
 * This function enables multiple uart interrupts.
 *
 * @param  hUART           UART handle.
 * @param  mask            Interrupt mask, please refer to enum UART_Interrupt_T..
 *
 * @retval                 None
 */
void HAL_UART_EnableInterrupts(UART_Device_T *pUART, uint32_t mask);

/**
 * @brief Disables UART interrupts.
 *
 * This function disables multiple uart interrupts.
 *
 * @param  hUART           UART handle
 * @param  mask            Interrupt mask, please refer to enum UART_IntStatus_T
 *
 * @retval                 None
 */
void HAL_UART_DisableInterrupts(UART_Device_T *pUART, uint32_t mask);

/**
 * @brief UART interrupt handling routine.
 *
 * This function handles the UART transmit and receive IRQ.
 *
 * @param  pUART           UART device handle
 * @param  hUART           UART handle
 * @param  callback        User callback
 *
 * @retval                 None
 */
void HAL_UART_IRQHandler(void *param);

void HAL_UART_TransferConfig(UART_Handle_T *hUART, UART_TransferConfig_T *config);

void HAL_UART_GetTransferStatus(UART_Handle_T *hUART, UART_TransferStatus_T *status);

RET_CODE HAL_UART_Transmit(UART_Handle_T *hUART, uint8_t *pData, uint16_t size, uint32_t timeout);

RET_CODE HAL_UART_Receive(UART_Handle_T *hUART, uint8_t *pData, uint16_t expectedSize, uint16_t *pReceivedSize, uint32_t timeout);

//int HAL_UART_GetIsrFlag(UART_Device_T *pUART, uint32_t mask);
//void HAL_UART_ClearIsrFlag(UART_Device_T *pUART, uint32_t mask);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_UART_H_ */



