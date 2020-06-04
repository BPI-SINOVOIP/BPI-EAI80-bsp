#include <gm_soc.h>
#include <gm_hal_uart.h>
#include <gm_hal_dma.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#ifndef RABBIT_NO_OS_SYSTEM
#include <misc/__assert.h>
#include <gm_irq.h>
#endif

#define MAX_UART_DEVICE      8
#define UART_DMA_FINISHED    FALSE  /**< No uart dma transaction is on going */
#define UART_DMA_STARTED     TRUE   /**< One uart dma transaction is on going */
#define UART_STATUS_IDLE     0      /**< No uart transaction is ongoing */
#define UART_STATUS_RX_BUSY  1      /**< Uart reception is ongoing */
#define UART_STATUS_TX_BUSY  2      /**< Uart transmission is ongoing */

static lock_t g_lock;
static UART_Handle_T *g_handlers[MAX_UART_DEVICE];

static uint16_t *gReceivedSize = NULL;

static void transmit_data(UART_Handle_T *hUART);
static void receive_data(UART_Handle_T *hUART);
static void UART_DMAConfig(UART_Device_T *pUART, uint8_t dmaRead, DMA_Handle_T *pDMAHandle);
static void UART_TxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback);
static void UART_RxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback, uint8_t fifoRxTimeout);


RET_CODE HAL_UART_Init(UART_Handle_T *hUART, UART_Device_T *pUART, UART_Config_T *pConfig)
{
    Peripheral_Config_T periClkConfig;

    if ((!hUART) || (!pUART) || (!pConfig))
    {
        return -RET_INVPARAM;
    }

    /* enable reset and clock */
    if (pUART == UART0) {
        HAL_Reset_Module(RESET_UART1_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART1);
        periClkConfig.peripheMask = PERIPHERAL_UART1_MASK;
    } else if (pUART == UART1) {
        HAL_Reset_Module(RESET_UART2_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART2);
        periClkConfig.peripheMask = PERIPHERAL_UART2_MASK;
    } else if (pUART == UART2) {
        HAL_Reset_Module(RESET_UART3_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART3);
        periClkConfig.peripheMask = PERIPHERAL_UART3_MASK;
    } else if (pUART == UART3) {
        HAL_Reset_Module(RESET_UART4_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART4);
        periClkConfig.peripheMask = PERIPHERAL_UART4_MASK;
    } else if (pUART == UART4) {
        HAL_Reset_Module(RESET_UART5_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART5);
        periClkConfig.peripheMask = PERIPHERAL_UART5_MASK;
    } else if (pUART == UART5) {
        HAL_Reset_Module(RESET_UART6_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART6);
        periClkConfig.peripheMask = PERIPHERAL_UART6_MASK;
    } else if (pUART == UART6) {
        HAL_Reset_Module(RESET_UART7_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART7);
        periClkConfig.peripheMask = PERIPHERAL_UART7_MASK;
    } else if (pUART == UART7) {
        HAL_Reset_Module(RESET_UART8_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_UART8);
        periClkConfig.peripheMask = PERIPHERAL_UART8_MASK;
    }

    /* configure UARTx clock source and prescaler for 115200 baudrate in 48M clock
     * with default operation without this config, baudrate is 19200 in 8M
     */
    if ((pConfig->clockSrc == UART_CLOCK_48M) && (pConfig->baudRate == UART_BAUDRATE_115200))
    {
        periClkConfig.peripheClkConfig.uartClkConfig.uartPrescaler = CLOCK_UART_DIV_1;
        periClkConfig.peripheClkConfig.uartClkConfig.uartSourceClk = CLOCK_UART_SEL_PLLI2S;
    }
    else
    {
        periClkConfig.peripheClkConfig.uartClkConfig.uartPrescaler = CLOCK_UART_DIV_1;
        periClkConfig.peripheClkConfig.uartClkConfig.uartSourceClk = CLOCK_UART_SEL_HIRC;
    }
    HAL_CLOCK_PeripheralClkSetConfig(&periClkConfig);

    /* Disable all the uart interrupts */
    LL_UART_INTERRUPTES_DISABLE(pUART, 0xFFFFFFFF);

    /* Set data bits */
    LL_UART_SET_DATA_BIT(pUART, pConfig->wordLength);

    /* Set stop bits */
    if (pConfig->stopBits == UART_STOPBITS_1)
    {
        LL_UART_SET_STOPBIT(pUART, UART_STOPBITS_1);
    }
    else
    {
        if ((pConfig->wordLength == UART_DATABITS_5 && pConfig->stopBits == UART_STOPBITS_1_5) ||
            (pConfig->wordLength > UART_DATABITS_5 && pConfig->stopBits == UART_STOPBITS_2))
        {
            LL_UART_SET_STOPBIT(pUART, UART_STOPBITS_2);
        }
        else
        {
            return -RET_INVPARAM;
        }
    }

    /* Set baud rate */
    LL_UART_SET_BAUDRATE_DIV(pUART, (pConfig->clockSrc / (pConfig->baudRate << 4)));

    /* Set parity format */
    if (UART_PARITY_NONE == pConfig->parity)
    {
        LL_UART_PARITY_DISABLE(pUART);
    }
    else
    {
        LL_UART_PARITY_ENABLE(pUART);
        if (UART_PARITY_EVEN == pConfig->parity)
        {
            LL_UART_SET_EVEN_PARITY(pUART);
        }
        else if (UART_PARITY_ODD == pConfig->parity)
        {
            LL_UART_SET_ODD_PARITY(pUART);
        }
    }

    /* Disable Tx/Rx FIFO */
    LL_UART_FIFO_RESET_DISABEL(pUART);

    /* Reset line status */
    LL_UART_RESET_STATUS(pUART);
    LL_UART_ClearIsrFlag(pUART, 0xFFFFFFFF);

    /* Make DTR(DTE ready) and RTS(DTE request DCE to send ) valid, it is modem control signal */
    LL_UART_SET_RTS(pUART, 1);
    LL_UART_SET_DTR(pUART, 1);

    //need by HAL layer and driver api layer
    if (pUART == UART0) {
        g_handlers[0] = hUART;
    } else if (pUART == UART1) {
        g_handlers[1] = hUART;
    } else if (pUART == UART2) {
        g_handlers[2] = hUART;
    } else if (pUART == UART3) {
        g_handlers[3] = hUART;
    } else if (pUART == UART4) {
        g_handlers[4] = hUART;
    } else if (pUART == UART5) {
        g_handlers[5] = hUART;
    } else if (pUART == UART6) {
        g_handlers[6] = hUART;
    } else if (pUART == UART7) {
        g_handlers[7] = hUART;
    }
        
    /* Register UART IR number and callback */
    if (pUART == UART0) {
        hUART->IRQn = UART0_IRQn;
        HAL_RequestIrq(UART0_IRQn, UART0_IRQ_PRIORITY, HAL_UART_IRQHandler, 0);
    } else if (pUART == UART1) {
        hUART->IRQn = UART1_IRQn;
        HAL_RequestIrq(UART1_IRQn, UART1_IRQ_PRIORITY, HAL_UART_IRQHandler, 1);
    } else if (pUART == UART2) {
        hUART->IRQn = UART2_IRQn;
        HAL_RequestIrq(UART2_IRQn, UART2_IRQ_PRIORITY, HAL_UART_IRQHandler, 2);
    } else if (pUART == UART3) {
        hUART->IRQn = UART3_IRQn;
        HAL_RequestIrq(UART3_IRQn, UART3_IRQ_PRIORITY, HAL_UART_IRQHandler, 3);
    } else if (pUART == UART4) {
        hUART->IRQn = UART4_IRQn;
        HAL_RequestIrq(UART4_IRQn, UART4_IRQ_PRIORITY, HAL_UART_IRQHandler, 4);
    } else if (pUART == UART5) {
        hUART->IRQn = UART5_IRQn;
        HAL_RequestIrq(UART5_IRQn, UART5_IRQ_PRIORITY, HAL_UART_IRQHandler, 5);
    } else if (pUART == UART6) {
        hUART->IRQn = UART6_IRQn;
        HAL_RequestIrq(UART6_IRQn, UART6_IRQ_PRIORITY, HAL_UART_IRQHandler, 6);
    } else if (pUART == UART7) {
        hUART->IRQn = UART7_IRQn;
        HAL_RequestIrq(UART7_IRQn, UART7_IRQ_PRIORITY, HAL_UART_IRQHandler, 7);
    }
    HAL_IRQ_ENABLE(hUART->IRQn);

    hUART->device = pUART;
    hUART->enableFifo = FALSE;
    hUART->callback = pConfig->callback;
    hUART->enableRxDMA = hUART->enableTxDMA = FALSE;
    hUART->status = UART_STATUS_IDLE;
    hUART->txExpectedSize = hUART->txSize = 0;
    hUART->rxExpectedSize = hUART->rxSize = 0;
    hUART->tcount = hUART->rcount = 0;
    hUART->rxWatermark = 0;
    hUART->txWatermark = 0;

    /* init UART HAL lock */
    HAL_LockInit(&g_lock);

    return 0;
}

RET_CODE HAL_UART_DeInit(UART_Handle_T *hUART)
{
    ASSERT(hUART != NULL);

    /* Disable FIFO */
    LL_UART_FIFO_RESET_DISABEL(hUART->device);

    /* Clear all interrupt register */
    LL_UART_INTERRUPTES_DISABLE(hUART->device, 0xFFFFFFFF);

    /* Disable this UART interrupt */
    HAL_IRQ_DISABLE(hUART->IRQn);

    return RET_OK;
}

void HAL_UART_SetMode(UART_Device_T *pUART, UART_Mode_T mode)
{
    ASSERT(hUART != NULL);

    if (UART_MODE_RX == mode) {
        LL_UART_TX_DISABLE(pUART);
        LL_UART_RX_ENABLE(pUART);
    } else if (UART_MODE_TX == mode) {
        LL_UART_RX_DISABLE(pUART);
        LL_UART_TX_ENABLE(pUART);
    } else if (UART_MODE_DISABLE == mode) {
        LL_UART_TX_DISABLE(pUART);
        LL_UART_RX_DISABLE(pUART);
    } else if (UART_MODE_DUPLEX == mode) {
        LL_UART_RX_ENABLE(pUART);
        LL_UART_TX_ENABLE(pUART);
    }
}

void HAL_UART_TransferConfig(UART_Handle_T *hUART, UART_TransferConfig_T *pConfig)
{
    volatile uint32_t data;
    uint32_t txWaterVal = 0;
    uint32_t rxWaterVal = 0;
    UART_Device_T *pUART = hUART->device;

    if (hUART == NULL || pConfig == NULL) {
        return;
    }

    HAL_TryLock(&g_lock);

    /* Copy all configuration items to handle structure */
    hUART->enableFifo = pConfig->enableFifo;
    hUART->txWatermark = pConfig->txWatermark;
    hUART->rxWatermark = pConfig->rxWatermark;
    hUART->enableRxDMA = pConfig->enableRxDMA;
    hUART->enableTxDMA = pConfig->enableTxDMA;
    hUART->status = UART_STATUS_IDLE;
    hUART->txExpectedSize = hUART->txSize = hUART->tcount = 0;
    hUART->rxExpectedSize = hUART->rxSize = hUART->rcount = 0;
    hUART->rxDMAStatus = hUART->txDMAStatus = UART_DMA_FINISHED;

    /* It defines non-fifo mode if both txWatermark and rxWatermark <= 1 */
    if (hUART->enableFifo == FALSE) {
        LL_UART_FIFO_RESET_DISABEL(pUART);

        /* Clear pending transmission , should support cancelling for single byte */
        while (!LL_UART_Is_LS_TXE(pUART)) {};

        /* Clear pending data read */
        data = LL_UART_GetChar(pUART);
        data = LL_UART_GetChar(pUART);
    } else {

        LL_UART_FIFO_RESET_ENABLE(pUART);
        LL_UART_TX_FIFO_RESET_ENABLE(pUART);
        LL_UART_RX_FIFO_RESET_ENABLE(pUART);

        LL_UART_Clear_IS_TFT(pUART);
        LL_UART_Clear_IS_RFT(pUART);

        /* Set FIFO default threshhold */
        if (hUART->txWatermark > 0 && hUART->txWatermark <= MAX_UART_TX_FIFO_SIZE) {
            txWaterVal = hUART->txWatermark;
        } else {
            txWaterVal = MAX_UART_TX_FIFO_SIZE / 2;
        }
        if (hUART->rxWatermark > 0 && hUART->rxWatermark <= MAX_UART_RX_FIFO_SIZE) {
            rxWaterVal = hUART->rxWatermark;
        } else {
            rxWaterVal = MAX_UART_RX_FIFO_SIZE / 2;
        }

        LL_UART_SET_TXFIFO_WATERMARK(pUART, txWaterVal);
        LL_UART_SET_RXFIFO_WATERMARK(pUART, rxWaterVal);
    }

    /* DMA Configuration is delayed till user call UART_Receive/UART_Transmit */
    LL_UART_TX_DMA_DISABLE(pUART);
    LL_UART_RX_DMA_DISABLE(pUART);

    HAL_UnLock(&g_lock);
}

void HAL_UART_EnableInterrupts(UART_Device_T *pUART, uint32_t mask)
{
    ASSERT(pUART != NULL);

    if (UART_RX_INT & mask) {
        LL_UART_Enable_Rx_Interrupts(pUART);
        LL_UART_Enable_RxErr_Interrupts(pUART);
    }

    if (UART_TX_INT & mask) {
        LL_UART_Enable_Tx_Interrupts(pUART);
    }

    if (UART_RX_FIFO_INT & mask) {
        LL_UART_Enable_RxFifo_Interrupts(pUART);
    }

    if (UART_TX_FIFO_INT & mask) {
        LL_UART_Enable_TxFifo_Interrupts(pUART);
    }
}

void HAL_UART_DisableInterrupts(UART_Device_T *pUART, uint32_t mask)
{
    ASSERT(pUART != NULL);

    if (UART_RX_INT & mask) {
        LL_UART_Disable_Rx_Interrupts(pUART);
        LL_UART_Disable_RxErr_Interrupts(pUART);
    }

    if (UART_TX_INT & mask) {
        LL_UART_Disable_Tx_Interrupts(pUART);
    }

    if (UART_RX_FIFO_INT & mask) {
        LL_UART_Disable_RxFifo_Interrupts(pUART);
    }

    if (UART_TX_FIFO_INT & mask) {
        LL_UART_Disable_TxFifo_Interrupts(pUART);
    }
}

RET_CODE HAL_UART_Transmit(UART_Handle_T *hUART, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    RET_CODE ret = RET_OK;

    /* Zero transfer not allowed */
    if (hUART == NULL || pData == NULL || size <= 0) {
        return RET_ERROR;
    }

    /* If one transmit is on progess, next transmit is not allowed. */
    if (hUART->status & UART_STATUS_TX_BUSY) {
        return RET_BUSY;
    }

    HAL_TryLock(&g_lock);

    hUART->txExpectedSize = size;
    hUART->pTxData = pData;
    hUART->txSize = 0;
    hUART->tcount = size; //total number need to transmit
    hUART->status |= UART_STATUS_TX_BUSY;

    if (hUART->enableTxDMA) {
        UART_DMAConfig(hUART->device, FALSE, &hUART->TxDMAHandle);
        hUART->txDMAStatus = UART_DMA_STARTED;
        HAL_DMA_Start(&hUART->TxDMAHandle, (uint32_t)hUART->pTxData, (uint32_t)&(hUART->device->DR), hUART->tcount);
    } else {
        while (hUART->tcount > 0) {
            transmit_data(hUART);

            /* Exit if it is non-blocking call */
            if (timeout == TIMEOUT_NO_BLOCKING) {
                ret = RET_ONGOING;
                break;
            }

            /* Just return RET_OK even not all expected data are received */
            if (timeout == TIMEOUT_NO_WAIT) {
                hUART->txExpectedSize = hUART->txSize;
                hUART->tcount = 0;
                break;
            }
        }

        if ((hUART->tcount == 0) && (hUART->txExpectedSize == hUART->txSize)) {
            hUART->status &= ~UART_STATUS_TX_BUSY;
            ret = RET_OK;
        }
    }

    HAL_UnLock(&g_lock);

    return ret;
}

RET_CODE HAL_UART_Receive(UART_Handle_T *hUART, uint8_t *pData, uint16_t expectedSize, uint16_t *pReceivedSize, uint32_t timeout)
{
    RET_CODE ret = RET_OK;

    /* Zero transfer not allowed */
    if (hUART == NULL || pData == NULL || expectedSize <= 0) {
        return RET_ERROR;
    }

    /* If one reception is on progess, next reception is not allowed. */
    if (hUART->status & UART_STATUS_RX_BUSY) {
        return RET_BUSY;
    }

    HAL_TryLock(&g_lock);

    hUART->rxExpectedSize = expectedSize;
    hUART->pRxData = pData;
    hUART->rxSize = 0;
    hUART->rcount = expectedSize;
    hUART->status |= UART_STATUS_RX_BUSY;

    //fixed bug: pReceivedSize will be 0 forever
    if (pReceivedSize != NULL) {
        gReceivedSize = pReceivedSize;
    }
    
    if (hUART->enableRxDMA) {
        UART_DMAConfig(hUART->device, TRUE, &hUART->RxDMAHandle);
        hUART->rxDMAStatus = UART_DMA_STARTED;
        HAL_DMA_Start(&hUART->RxDMAHandle, (uint32_t)&(hUART->device->DR), (uint32_t)(hUART->pRxData), hUART->rcount);
    } else {
        while (hUART->rcount > 0) {
            receive_data(hUART);

            /* Exit if it is non-blocking call */
            if (timeout == TIMEOUT_NO_BLOCKING) {
                ret = RET_ONGOING;
                break;
            }

            /* Just return RET_OK even not all expected data are received */
            if (timeout == TIMEOUT_NO_WAIT) {
                hUART->rxExpectedSize = hUART->rxSize;
                hUART->rcount = 0;
                break;
            }
        }

        if ((hUART->rcount == 0) && (hUART->rxExpectedSize == hUART->rxSize)) {
            hUART->status &= ~UART_STATUS_RX_BUSY;
            ret = RET_OK;
        }

        /* Timeout/nowait call shall return size of data actually transferred */
        if (pReceivedSize != NULL) {
            *pReceivedSize = hUART->rxSize;
        }

        /* Processing error status */
        if (LL_UART_Is_LS_RXES(hUART->device)) {
            ret = RET_ERROR;
        }
    }
    HAL_UnLock(&g_lock);

    return ret;
}

void HAL_UART_GetTransferStatus(UART_Handle_T *hUART, UART_TransferStatus_T *status)
{
    UART_Device_T *pUART;

    ASSERT((hUART != NULL) && (status != NULL));

    pUART = hUART->device;

    HAL_TryLock(&g_lock);

    status->lineStatus = 0;
    if (LL_UART_Is_LS_ROS(pUART)) {
       status->lineStatus |= UART_OVERRUN_ERROR;
    }
    if (LL_UART_Is_LS_RPS(pUART)) {
        status->lineStatus |= UART_PARITY_ERROR;
    }
    if (LL_UART_Is_LS_RFS(pUART)) {
        status->lineStatus |= UART_FRAME_ERROR;
    }
    if (LL_UART_Is_LS_RBS(pUART)) {
        status->lineStatus |= UART_BREAK_CHARACTER;
    }
    if (LL_UART_Is_LS_RFES(pUART)) {
        status->lineStatus |= UART_FIFO_ERROR;
    }
    if (LL_UART_Is_LS_TXE(pUART)) {
        status->lineStatus |= UART_TRANSMITTER_IDLE;
    }

    /* Get size of all received data which have not been moved to user buffer */
    if (!hUART->enableRxDMA) {
        status->receivedSize = LL_UART_GET_RXFIFO_WAIT_NUM(pUART);
    }

    /* Get size of to-be-transmitted data which have not been move to tx fifo */
    if (!hUART->enableTxDMA) {
        status->toBeTransmitedSize = LL_UART_GET_TXFIFO_WAIT_NUM(pUART);
    }

    HAL_UnLock(&g_lock);
}

#ifdef RABBIT_NO_OS_SYSTEM
void HAL_UART0_IRQHandler(void)
{
	HAL_UART_IRQHandler((void *)0);
}

void HAL_UART1_IRQHandler(void)
{
	HAL_UART_IRQHandler((void *)1);
}

void HAL_UART2_IRQHandler(void)
{
	HAL_UART_IRQHandler((void *)2);
}
#endif

void HAL_UART_IRQHandler(void *param)
{
    uint8_t rxChar;
    UART_Handle_T *hUART;
    UART_Device_T *pUART;
    UART_Callback_T callback;

    int index = (int)param;
    if (index < 0 || index >= MAX_UART_DEVICE) {
        return;
    }

    hUART = g_handlers[index];
    pUART = hUART->device;
    callback = hUART->callback;

    /* OPFB error: OE, PE, FE, or BI */
    if (LL_UART_Is_IE_OPFB(pUART) && LL_UART_Is_IS_OPFB(pUART)) {
        LL_UART_Clear_IS_OPFB(pUART);
        if (LL_UART_Is_LS_RXES(pUART) && (NULL != callback)) {
            callback(hUART, UART_EVENT_RX_ERROR, (ulong_t)0);
        }
    }

    /* Rx Fifo Timeout Interrupt */
    if (LL_UART_Is_IS_RFTO(pUART)) {
        UART_RxInterruptHandler(pUART, hUART, callback, TRUE);
    }

    /* NonFifo && Received Data Available */
    if (hUART->enableFifo == FALSE && LL_UART_Is_IS_RXDA(pUART)) {
        if (hUART->rxExpectedSize != hUART->rxSize) {
            UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
        } else {
            if (LL_UART_Is_LS_RXDA(pUART)) /* Make sure data is ready */ {
                rxChar = (uint8_t)LL_UART_GetChar(pUART);
                if (NULL != callback) {
                    callback(hUART, UART_EVENT_RX_CHAR, (ulong_t)rxChar);
                }
            }
        }
    }

    /* Transmitter Holding Register Empty */
    if (LL_UART_Is_IE_TXHE(pUART) && LL_UART_Is_IS_TXHE(pUART)) {
        LL_UART_Clear_IS_TXHE(pUART);
        if (hUART->txExpectedSize != hUART->txSize) {
            UART_TxInterruptHandler(pUART, hUART, callback);
        } else if (NULL != callback) {
            callback(hUART, UART_EVENT_TX_EMPTY, (ulong_t)0);
        }
    }

    /* Process data reception in interrupt handler */
    if (LL_UART_Is_IS_RFT(pUART)) {
        LL_UART_Clear_IS_RFT(pUART);
        UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
    }

    /* Process data transmition in interrupt handler */
    if (LL_UART_Is_IS_TFT(pUART)) {
        LL_UART_Clear_IS_TFT(pUART);
        UART_TxInterruptHandler(pUART, hUART, callback);
    }
}

static void UART_TxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback)
{
    uint8_t fifoDataSize;

    ASSERT((hUART != NULL) && (pUART != NULL));

    if (hUART->enableTxDMA == FALSE) {
        /* Always notify user if user does not call UART_Receive in api context */
        if (hUART->txExpectedSize == hUART->txSize) {
            if (NULL != callback) {
                fifoDataSize = LL_UART_GET_TXFIFO_WAIT_NUM(pUART);
                callback(hUART, UART_EVENT_TX_WATERMARK, (ulong_t)fifoDataSize);
            }
        } else {
            hUART->status |= UART_STATUS_TX_BUSY;
            transmit_data(hUART);

            if ((hUART->tcount == 0) && (hUART->txExpectedSize == hUART->txSize)) {
                hUART->status &= ~UART_STATUS_TX_BUSY;
                if (NULL != callback) {
                    callback(hUART, UART_EVENT_TX_OVER, (ulong_t)0);
                }
            }
        }
    }
}

static void UART_RxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback, uint8_t fifoRxTimeout)
{
    uint8_t fifoDataSize;

    ASSERT((hUART != NULL) && (pUART != NULL));

    if (hUART->enableRxDMA == FALSE) {
        /* Always notify user to receive data in interrupt context if user does not call UART_Receive in api context */
        if (hUART->rxExpectedSize == hUART->rxSize) {
            if (callback != NULL && LL_UART_Is_C2_FE(hUART->device)) {
                fifoDataSize = LL_UART_GET_RXFIFO_WAIT_NUM(pUART);
                /* Watermark event(rx fifo threshhold interrupt) always overlaps rx fifo timeout event */
                if (fifoRxTimeout == TRUE) {
                    callback(hUART, UART_EVENT_RXFIFO_TIMEOUT, (ulong_t)fifoDataSize);
                } else {
                    callback(hUART, UART_EVENT_RX_WATERMARK, (ulong_t)fifoDataSize);
                }
            }
        } else {
            hUART->status |= UART_STATUS_RX_BUSY;
            receive_data(hUART);

            //receive finished
            if ((hUART->rcount == 0) && (hUART->rxExpectedSize == hUART->rxSize)) {
                hUART->status &= ~UART_STATUS_RX_BUSY;
                if (gReceivedSize != NULL) {
                    *gReceivedSize = hUART->rxSize;
                }
                if (callback != NULL) {
                    callback(hUART, UART_EVENT_RX_OVER, (ulong_t)0);
                }
            }
        }
    }
}

static void transmit_data(UART_Handle_T *hUART)
{
    ASSERT(hUART != NULL);

    while (hUART->tcount > 0) {
        if (hUART->enableFifo == TRUE) {
            if (LL_UART_GET_TXFIFO_WAIT_NUM(hUART->device) >= MAX_UART_TX_FIFO_SIZE) {
                break;
            }
        } else {
            if (!LL_UART_Is_LS_TXHE(hUART->device)) {
                break;
            }
        }
        hUART->tcount--;
        LL_UART_PutChar(hUART->device, hUART->pTxData[hUART->txSize++]);
    }
}

static void receive_data(UART_Handle_T *hUART)
{
    ASSERT(hUART != NULL);

    while (hUART->rcount > 0) {
        if (hUART->enableFifo == TRUE) {
            if (LL_UART_GET_RXFIFO_WAIT_NUM(hUART->device) == 0) {
                break;
            }
        } else {
            if (!LL_UART_Is_LS_RXDA(hUART->device)) {
                break;
            }
        }
        hUART->rcount--;
        hUART->pRxData[hUART->rxSize++] = LL_UART_GetChar(hUART->device);
    }
}

static void UART_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    UART_Handle_T *hUART = NULL;

    ASSERT(hDMA != NULL);

    if (event == DMA_EVENT_TRANSFER_DONE) {
        hUART = HAL_CONTAINER_OF(hDMA, UART_Handle_T, TxDMAHandle);
        if (hUART == NULL) {
            return;
        }

        hUART->txDMAStatus = UART_DMA_FINISHED;
        hUART->txSize += hUART->tcount;
        hUART->tcount = 0;
        if (hUART->txExpectedSize == hUART->txSize) {
            hUART->status &= ~UART_STATUS_TX_BUSY;
            if (NULL != hUART->callback) {
                hUART->callback(hUART, UART_EVENT_TX_OVER, (ulong_t)0);
            }
        }

        HAL_DMA_Abort(hDMA);
    }
}

static void UART_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    UART_Handle_T *hUART = NULL;

    ASSERT(hDMA != NULL);

    if (event == DMA_EVENT_TRANSFER_DONE) {
        hUART = HAL_CONTAINER_OF(hDMA, UART_Handle_T, RxDMAHandle);
        if (hUART == NULL) {
            return;
        }

        hUART->rxDMAStatus = UART_DMA_FINISHED;
        hUART->rxSize += hUART->rcount;
        hUART->rcount = 0;
        if (hUART->rxExpectedSize == hUART->rxSize) {
            hUART->status &= ~UART_STATUS_RX_BUSY;
            if (NULL != hUART->callback) {
                hUART->callback(hUART, UART_EVENT_RX_OVER, (ulong_t)0);
            }
        }

        HAL_DMA_Abort(hDMA);
    }
}

static void UART_DMAConfig(UART_Device_T *pUART, uint8_t dmaRead, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;

    ASSERT((pUART != NULL) && (pDMAHandle != NULL));

    if (dmaRead) {
        if (pUART == UART0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART1_RX, &reqResult);
        }
        else if (pUART == UART1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART2_RX, &reqResult);
        }
        else if (pUART == UART2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART3_RX, &reqResult);
        }
        else if (pUART == UART3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART4_RX, &reqResult);
        }
        else if (pUART == UART4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART5_RX, &reqResult);
        }
        else if (pUART == UART5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART6_RX, &reqResult);
        }
        else if (pUART == UART6)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART7_RX, &reqResult);
        }
        else if (pUART == UART7)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART8_RX, &reqResult);
        }
                
        if (ret != RET_OK)
        {
            //fail, need return fail status
            return;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
        dmaConfig.width = DMA_WIDTH_8BIT;
        dmaConfig.burst = (DMA_Burst_T)0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
        dmaConfig.callback = UART_RxDMACallback;

        LL_UART_RX_DMA_RDL(pUART, 0);
        LL_UART_RX_DMA_ENABLE(pUART);
        LL_UART_TX_DMA_DISABLE(pUART);
    } else {
        if (pUART == UART0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART1_TX, &reqResult);
        }
        else if(pUART == UART1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART2_TX, &reqResult);
        }
        else if(pUART == UART2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART3_TX, &reqResult);
        }
        else if(pUART == UART3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART4_TX, &reqResult);
        }
        else if(pUART == UART4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART5_TX, &reqResult);
        }
        else if(pUART == UART5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART6_TX, &reqResult);
        }
        else if(pUART == UART6)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART7_TX, &reqResult);
        }
        else if(pUART == UART7)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART8_TX, &reqResult);
        }
                
        if (ret != RET_OK)
        {
            //fail, need return fail status
            return;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width = DMA_WIDTH_8BIT;
        dmaConfig.burst = (DMA_Burst_T)0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
        dmaConfig.callback = UART_TxDMACallback;

        LL_UART_TX_DMA_TDL(pUART, MAX_UART_TX_FIFO_SIZE/2);
        LL_UART_TX_DMA_ENABLE(pUART);
        LL_UART_RX_DMA_DISABLE(pUART);
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
}


//tx interrupts
int HAL_UART_Tx_Complete(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_TXE(pUART);
}

int HAL_UART_TxInt_Enabled(UART_Device_T *pUART)
{
    return LL_UART_Is_IE_TXHE(pUART);
}

int HAL_UART_TxInt_Ready(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_TXHE(pUART);
}

void HAL_UART_Clear_TxInt(UART_Device_T *pUART)
{
    LL_UART_Clear_IS_TXHE(pUART);
}

//tx fifo interrupts
int HAL_UART_TxFifoInt_Ready(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_TFT(pUART);
}

int HAL_UART_TxFifoInt_Enabled(UART_Device_T *pUART)
{
    return LL_UART_Is_IE_TFT(pUART);
}

void HAL_UART_Clear_TxFifoInt(UART_Device_T *pUART)
{
    LL_UART_Clear_IS_TFT(pUART);
}

int HAL_UART_Get_TxFifo_DataSize(UART_Device_T *pUART)
{
    return LL_UART_GET_TXFIFO_WAIT_NUM(pUART);
}

//rx interrupts
int HAL_UART_RxInt_Enabled(UART_Device_T *pUART)
{
    return LL_UART_Is_IE_RXDA(pUART);
}

int HAL_UART_RxInt_Ready(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_RXDA(pUART);
}

uint16_t HAL_UART_DataReady(UART_Device_T *pUART)
{
    return LL_UART_Is_LS_RXDA(pUART);
}

//rx fifo interrupts
int HAL_UART_RxFifoTmo_Ready(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_RFTO(pUART);
}

int HAL_UART_RxFifoInt_Ready(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_RFT(pUART);
}

void HAL_UART_Clear_RxFifoInt(UART_Device_T *pUART)
{
    LL_UART_Clear_IS_RFT(pUART);
}

int HAL_UART_Get_RxFifo_DataSize(UART_Device_T *pUART)
{
    return LL_UART_GET_RXFIFO_WAIT_NUM(pUART);
}

//error interrupts
void HAL_UART_Enable_Error_Interrupts(UART_Device_T *pUART)
{
    LL_UART_Enable_RxErr_Interrupts(pUART);
}

void HAL_UART_Disable_Error_Interrupts(UART_Device_T *pUART)
{
    LL_UART_Disable_RxErr_Interrupts(pUART);
}

int HAL_UART_Error_Enabled(UART_Device_T *pUART)
{
    return LL_UART_Is_IE_OPFB(pUART);
}

int HAL_UART_Error_Ready(UART_Device_T *pUART)
{
    return LL_UART_Is_IS_OPFB(pUART);
}

void HAL_UART_Clear_ErrInt(UART_Device_T *pUART)
{
    LL_UART_Clear_IS_OPFB(pUART);
}

//other
int HAL_UART_Fifo_Enabled(UART_Device_T *pUART)
{
    return LL_UART_Is_C2_FE(pUART);
}

void HAL_UART_PutChar(UART_Device_T *pUART, uint8_t ch)
{
    ASSERT(pUART != NULL);

    /* Wait until XMIT FIFO empty or Transmitter Holding Register empty */
    while (!LL_UART_Is_LS_TXHE(pUART));

    /* Send out this character */
    LL_UART_PutChar(pUART, ch);
}

uint16_t HAL_UART_GetChar(UART_Device_T *pUART)
{
    ASSERT(pUART != NULL);

    return LL_UART_GetChar(pUART);
}

int HAL_UART_GetIsrFlag(UART_Device_T *pUART, uint32_t mask)
{
	return LL_UART_GetIsrFlag(pUART, mask);
}

void HAL_UART_ClearIsrFlag(UART_Device_T *pUART, uint32_t mask)
{
	LL_UART_ClearIsrFlag(pUART, mask);
}

