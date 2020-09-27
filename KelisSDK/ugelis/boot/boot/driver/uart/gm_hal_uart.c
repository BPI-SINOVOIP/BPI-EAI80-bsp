#include <gm_soc.h>
#include <gm_hal_uart.h>
#include <gm_hal_dma.h>
#ifndef RABBIT_NO_OS_SYSTEM
#include <misc/__assert.h>
#include <gm_irq.h>
#endif

#define MAX_UART_DEVICE      3
#define UART_DMA_FINISHED    FALSE  /**< No uart dma transaction is on going */
#define UART_DMA_STARTED     TRUE   /**< One uart dma transaction is on going */
#define UART_STATUS_IDLE     0      /**< No uart transaction is ongoing */
#define UART_STATUS_RX_BUSY  1      /**< Uart reception is ongoing */
#define UART_STATUS_TX_BUSY  2      /**< Uart transmission is ongoing */

static lock_t g_lock;
static UART_Handle_T *g_handlers[MAX_UART_DEVICE];

static void transmit_data(UART_Handle_T *hUART);
static void receive_data(UART_Handle_T *hUART);
static void UART_DMAConfig(UART_Device_T *pUART, uint8_t dmaRead, DMA_Handle_T *pDMAHandle);
static void UART_TxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback);
static void UART_RxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback, uint8_t fifoRxTimeout);



int LL_UART_Is_LS_THES(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_THES);
}

 int LL_UART_Is_LS_ROS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_ROS);
}

 int LL_UART_Is_LS_RPS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RPS);
}

 int LL_UART_Is_LS_RFS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RFS);
}

 int LL_UART_Is_LS_RBS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RBS);
}

 int LL_UART_Is_LS_RFES(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RFES);
}

 int LL_UART_Is_LS_TES(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_TES);
}

 int LL_UART_Is_LS_DRS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_DRS);
}

 int LL_UART_Is_LS_RXES(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RXES);
}

 int LL_UART_GET_LS(UART_Device_T *pUART)
{
    return pUART->LS;
}

 int LL_UART_GET_TXFIFO_WAIT_NUM(UART_Device_T *pUART)
{
    return ((pUART->TH & UART_TH_TFC) >> UART_TH_TFC_POS);
}

 int LL_UART_GET_RXFIFO_WAIT_NUM(UART_Device_T *pUART)
{
    return ((pUART->TH & UART_TH_RFC) >> UART_TH_RFC_POS);
}

 int LL_UART_Is_C2_FE(UART_Device_T *pUART)
{
    return !!(pUART->C2 & UART_C2_FE);
}

 int LL_UART_Is_IE_OPFB(UART_Device_T *pUART)
{
    return !!(pUART->IE & UART_IE_OPFB);
}

 int LL_UART_Is_IE_THEI(UART_Device_T *pUART)
{
    return !!(pUART->IE & UART_IE_THEIE);
}

 int LL_UART_Is_IS_OPFB(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_OPFB);
}

 int LL_UART_Is_IS_RFTOI(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_RFTOI);
}

 int LL_UART_Is_IS_RDAI(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_RDAI);
}

 int LL_UART_Is_IS_TFTI(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_TFTI);
}

 int LL_UART_Is_IS_RFTI(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_RFTI);
}

 int LL_UART_Is_IS_THEI(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_THEI);
}

 void LL_UART_Clear_THEI_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_THEI);
}

 void LL_UART_Clear_TFTI_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_TFTI);
}

 void LL_UART_Clear_RFTI_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_RFTI);
}

 void LL_UART_Clear_OPFB_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_OPFB);
}

void LL_UART_Enable_Rx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, (UART_IE_RDAIE | UART_IE_RFTIE | UART_IE_OPFB));
}

void LL_UART_Clear_Rx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, (UART_IE_RDAIE | UART_IE_RFTIE | UART_IE_OPFB));
}

void LL_UART_Enable_Tx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_THEIE);
}

 void LL_UART_Clear_Tx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_THEIE);
}

 void LL_UART_Enable_RxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_RFTRIE);
}

 void LL_UART_CLear_RxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_RFTRIE);
}

 void LL_UART_Enable_TxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_TFTIE);
}

 void LL_UART_Clear_TxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_TFTIE);
}

 void LL_UART_Reset_Enable(int index)
{
    IO_BIT_SET(UART_RESET_BASE, 1 << index);
}

 void LL_UART_Clock_Enable(int index)
{
    IO_BIT_SET(UART_CLOCK_BASE, 1 << index);
}



/**
 * @brief  get input char value
 *
 * @param  pUART           pUART devive Point
 *
 * @retval Input char value
 */
#ifdef RABBIT_NO_OS_SYSTEM
 uint16_t LL_UART_GetChar(UART_Device_T *pUART)
{
    return ((uint16_t)(pUART->DR & 0x1FF));
}

 void LL_UART_PutChar(UART_Device_T *pUART, uint16_t val)
{
    pUART->DR = val;
}


STATIC_ISR(UART0_IRQn, HAL_UART0_IRQHandler);
STATIC_ISR(UART1_IRQn, HAL_UART1_IRQHandler);
STATIC_ISR(UART2_IRQn, HAL_UART2_IRQHandler);
#endif

RET_CODE HAL_UART_Init(UART_Handle_T *hUART, UART_Device_T *pUART, UART_Config_T *pConfig)
{
    int index = -1;

    if ((!hUART) || (!pUART) || (!pConfig))
    {
        return -RET_INVPARAM;
    }

    /* enable reset and clock */
    if (pUART == UART0) {
        index = 0;
    } else if (pUART == UART1) {
        index = 1;
    } else if (pUART == UART2) {
        index = 2;
    }
    if (index != -1) {
        LL_UART_Reset_Enable(index);
        LL_UART_Clock_Enable(index);
    }


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
    LL_UART_SET_BAUDRATE_DIV(pUART, (SYS_CLOCK / (pConfig->baudRate << 4)));

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

    /* Register UART IR number and callback */
#if 0 //def USE_UART_HAL_TEST
    if (pUART == UART0) {
        hUART->IRQn = UART0_IRQn;
        g_handlers[0] = hUART;
        HAL_RequestIrq(UART0_IRQn, UART0_IRQ_PRIORITY, HAL_UART_IRQHandler, 0);
    } else if (pUART == UART1) {
        hUART->IRQn = UART1_IRQn;
        g_handlers[1] = hUART;
        HAL_RequestIrq(UART1_IRQn, UART1_IRQ_PRIORITY, HAL_UART_IRQHandler, 1);
    } else if (pUART == UART2) {
        hUART->IRQn = UART2_IRQn;
        g_handlers[2] = hUART;
        HAL_RequestIrq(UART2_IRQn, UART2_IRQ_PRIORITY, HAL_UART_IRQHandler, 2);
    }
#endif

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

    /* Enable UART IR */
#ifdef USE_UART_HAL_TEST
    if (pUART == UART0) {
        hUART->IRQn = UART0_IRQn;
        g_handlers[0] = hUART;
        NVIC_EnableIRQ(UART0_IRQn);
    } else if (pUART == UART1) {
        hUART->IRQn = UART1_IRQn;
        g_handlers[1] = hUART;
        NVIC_EnableIRQ(UART1_IRQn);
    } else if (pUART == UART2) {
        hUART->IRQn = UART2_IRQn;
        g_handlers[2] = hUART;
        NVIC_EnableIRQ(UART2_IRQn);
    }
	
    //HAL_IRQ_ENABLE(hUART->IRQn);
#endif

    /* init UART HAL lock */
    HAL_LockInit(&g_lock);

    return 0;
}

RET_CODE HAL_UART_DeInit(UART_Handle_T *hUART)
{
//    __ASSERT(hUART != NULL, "");

    /* Disable FIFO */
    LL_UART_FIFO_RESET_DISABEL(hUART->device);

    /* Clear all interrupt register */
    LL_UART_INTERRUPTES_DISABLE(hUART->device, 0xFFFFFFFF);

    /* Disable this UART interrupt */
    //HAL_IRQ_DISABLE(hUART->IRQn);

    return RET_OK;
}

void HAL_UART_SetMode(UART_Device_T *pUART, UART_Mode_T mode)
{
    //__ASSERT(hUART != NULL, "");

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

void HAL_UART_PutChar(UART_Device_T *pUART, uint8_t ch)
{
//    __ASSERT(pUART != NULL, "");

    /* Wait until XMIT FIFO empty or Transmitter Holding Register empty */
    while (!LL_UART_Is_LS_THES(pUART));

    /* Send out this character */
    LL_UART_PutChar(pUART, ch);
}

uint16_t HAL_UART_GetChar(UART_Device_T *pUART)
{
//    __ASSERT(pUART != NULL, "");

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
        while (!LL_UART_Is_LS_TES(pUART)) {};

        /* Clear pending data read */
        data = LL_UART_GetChar(pUART);
        data = LL_UART_GetChar(pUART);
    } else {

        LL_UART_FIFO_RESET_ENABLE(pUART);
        LL_UART_TX_FIFO_RESET_ENABLE(pUART);
        LL_UART_RX_FIFO_RESET_ENABLE(pUART);

        LL_UART_Clear_TFTI_Interrupts(pUART);
        LL_UART_Clear_RFTI_Interrupts(pUART);

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
//    __ASSERT(pUART != NULL, "");

    if (UART_RX_INT & mask) {
        LL_UART_Enable_Rx_Interrupts(pUART);
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
 //   __ASSERT(pUART != NULL, "");

    if (UART_RX_INT & mask) {
        LL_UART_Clear_Rx_Interrupts(pUART);
    }

    if (UART_TX_INT & mask) {
        LL_UART_Clear_Tx_Interrupts(pUART);
    }

    if (UART_RX_FIFO_INT & mask) {
        LL_UART_CLear_RxFifo_Interrupts(pUART);
    }

    if (UART_TX_FIFO_INT & mask) {
        LL_UART_Clear_TxFifo_Interrupts(pUART);
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
    //__ASSERT((hUART != NULL) && (status != NULL), "");

    UART_Device_T *pUART = hUART->device;

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
    if (LL_UART_Is_LS_TES(pUART)) {
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
    if (LL_UART_Is_IS_OPFB(pUART) && LL_UART_Is_IE_OPFB(pUART)) {
        LL_UART_Clear_OPFB_Interrupts(pUART);
        if (LL_UART_Is_LS_RXES(pUART) && (NULL != callback)) {
            callback(hUART, UART_EVENT_RX_ERROR, (ulong_t)0);
        }
    }

    /* Rx Fifo Timeout Interrupt */
    if (LL_UART_Is_IS_RFTOI(pUART)) {
        UART_RxInterruptHandler(pUART, hUART, callback, TRUE);
    }

    /* NonFifo && Received Data Available */
    if (hUART->enableFifo == FALSE && LL_UART_Is_IS_RDAI(pUART)) {
        if (hUART->rxExpectedSize != hUART->rxSize) {
            UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
        } else {
            if (LL_UART_Is_LS_DRS(pUART)) /* Make sure data is ready */ {
                rxChar = (uint8_t)LL_UART_GetChar(pUART);
                if (NULL != callback) {
                    callback(hUART, UART_EVENT_RX_CHAR, (ulong_t)rxChar);
                }
            }
        }
    }
 
    /* Transmitter Holding Register Empty */
    if (LL_UART_Is_IS_THEI(pUART) && LL_UART_Is_IE_THEI(pUART)) {
        LL_UART_Clear_THEI_Interrupts(pUART);
        if (hUART->txExpectedSize != hUART->txSize) {
            UART_TxInterruptHandler(pUART, hUART, callback);
        } else if (NULL != callback) {
            callback(hUART, UART_EVENT_TX_EMPTY, (ulong_t)0);
        }
    }
        
    /* Process data reception in interrupt handler */
    if (LL_UART_Is_IS_RFTI(pUART)) {
        LL_UART_Clear_RFTI_Interrupts(pUART);
        UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
    }

    /* Process data transmition in interrupt handler */
    if (LL_UART_Is_IS_TFTI(pUART)) {
        LL_UART_Clear_TFTI_Interrupts(pUART);
        UART_TxInterruptHandler(pUART, hUART, callback);
    }
}

static void UART_TxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback)
{
    uint8_t fifoDataSize;

//    __ASSERT((hUART != NULL) && (pUART != NULL), "");

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

 //   __ASSERT((hUART != NULL) && (pUART != NULL), "");

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

            if ((hUART->rcount == 0) && (hUART->rxExpectedSize == hUART->rxSize)) {
                hUART->status &= ~UART_STATUS_RX_BUSY;
                if (callback != NULL) {
                    callback(hUART, UART_EVENT_RX_OVER, (ulong_t)0);
                }
            }
        }
    }
}

static void transmit_data(UART_Handle_T *hUART)
{
 //   __ASSERT(hUART != NULL, "");

    while (hUART->tcount > 0) {
        if (hUART->enableFifo == TRUE) {
            if (LL_UART_GET_TXFIFO_WAIT_NUM(hUART->device) >= MAX_UART_TX_FIFO_SIZE) {
                break;
            }
        } else {
            if (!LL_UART_Is_LS_THES(hUART->device)) {
                break;
            }
        }
        hUART->tcount--;
        LL_UART_PutChar(hUART->device, hUART->pTxData[hUART->txSize++]);
    }
}

static void receive_data(UART_Handle_T *hUART)
{
 //   __ASSERT(hUART != NULL, "");

    while (hUART->rcount > 0) {
        if (hUART->enableFifo == TRUE) {
            if (LL_UART_GET_RXFIFO_WAIT_NUM(hUART->device) == 0) {
                break;
            }
        } else {
            if (!LL_UART_Is_LS_DRS(hUART->device)) {
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

 //   __ASSERT(hDMA != NULL, "");

    if (event == DMA_EVENT_TRANSFER_DONE) {
        hUART = CONTAINER_OF(hDMA, UART_Handle_T, TxDMAHandle);
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

//    __ASSERT(hDMA != NULL, "");

    if (event == DMA_EVENT_TRANSFER_DONE) {
        hUART = CONTAINER_OF(hDMA, UART_Handle_T, RxDMAHandle);
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


 //   __ASSERT((pUART != NULL) && (pDMAHandle != NULL), "");

    if (dmaRead) {

        if (pUART == UART0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART1_RX, &reqResult);
        }
        else if(pUART == UART1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART2_RX, &reqResult);
        }
        else if(pUART == UART2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART3_RX, &reqResult);
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
        dmaConfig.burst = 0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
#if 0
        dmaConfig.srcGather = config->srcGather;
        dmaConfig.srcGatherInterval = config->srcGatherInterval;
        dmaConfig.srcGatherCount = config->srcGatherCount;
        dmaConfig.destScatter  = config->destScatter;
        dmaConfig.destScatterInterval = config->destScatterInterval;
        dmaConfig.destScatterCount = config->destScatterCount;
        dmaConfig.pesEnable = config->pesEnable;
        dmaConfig.pesChnIndex = config->pesChnIndex;
#endif
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
        dmaConfig.burst = 0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
#if 0
        dmaConfig.srcGather = config->srcGather;
        dmaConfig.srcGatherInterval = config->srcGatherInterval;
        dmaConfig.srcGatherCount = config->srcGatherCount;
        dmaConfig.destScatter  = config->destScatter;
        dmaConfig.destScatterInterval = config->destScatterInterval;
        dmaConfig.destScatterCount = config->destScatterCount;
        dmaConfig.pesEnable = config->pesEnable;
        dmaConfig.pesChnIndex = config->pesChnIndex;
#endif
        dmaConfig.callback = UART_TxDMACallback;

        LL_UART_TX_DMA_TDL(pUART, MAX_UART_TX_FIFO_SIZE/2);
        LL_UART_TX_DMA_ENABLE(pUART);
        LL_UART_RX_DMA_DISABLE(pUART);
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
}



