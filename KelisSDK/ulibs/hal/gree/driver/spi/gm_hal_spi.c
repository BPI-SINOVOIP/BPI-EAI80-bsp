/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_spi.c
 *
 * @author        william.wang
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         implement file of spi driver.
 *
 * @note
 *    2015/10/07, william.wang, V1.0.0
 *        Initial version.
 *    2016/06/14, william.wang, V1.1.0
 *        Update coding style.
 */
#include <gm_hal_spi.h>
#include <gm_hal_dma.h>
#include <gm_common.h>
#include "gm_ll_spi_dev.h"
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>

#define SPI_DMA_STATUS_FINISHED TRUE
#define SPI_DMA_STATUS_STARTED  FALSE

static RET_CODE SPI_ProcessData(SPI_Handle_T *hSPI);

static SPI_Device_T *const s_spiBases[MAX_SPI_DEVICE] = {ALL_SPI_DEVICES};

static IRQn_Type s_spiIrq[MAX_SPI_DEVICE] = {ALL_SPI_INTERRUPTS};
static uint32_t s_spiIrqPrio[MAX_SPI_DEVICE] = {ALL_SPI_INTERRUPTS_PRIO};
static SPI_Handle_T *s_spiHandle[MAX_SPI_DEVICE];

static void (*s_spiIsr)(SPI_Handle_T *hSPI) = NULL;


void SPI_InterruptHandler(SPI_Handle_T *hSPI);

uint32_t s_sysTime = 0;

static __inline uint32_t SPI_GetIndex(SPI_Device_T *pSPI)
{
    uint32_t i;

    /* Find the  index from base address mappings. */
    for (i = 0; i < MAX_SPI_DEVICE; i++)
    {
        if (s_spiBases[i] == pSPI)
        {
            break;
        }
    }

    ASSERT(i < MAX_SPI_DEVICE);

    return i;
}

static uint32_t SPI_GetFieldFromBits(uint32_t type, uint32_t value)
{
    switch (type)
    {
        case   SPI_CTRL_XCMD:
        case   SPI_CTRL_XDATA:
        case   SPI_CTRL_XADDR:
        case   SPI_CTRL_XDMY:
        default:
            if (value == 8)
            {
                return 0;
            }
            else if (value == 16)
            {
                return 1;
            }
            else if (value == 24)
            {
                return 2;
            }
            else if (value == 32)
            {
                return 3;
            }
            else
            {
                return 0;
            }
    }
}

RET_CODE SPI_WaitTillTimeout(SPI_WaitTimer_T *waitTimer)
{
    uint32_t distance;
    uint32_t cur_tick = SysTick->VAL;

    if (waitTimer->timeout == 0)
    {
        return RET_OK;
    }
    distance = s_sysTime - waitTimer->startTime;
    if (distance > waitTimer->timeout)
    {
        return RET_TIMEOUT;
    }
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_DelayUS(waitTimer->wakeupPeriod);
#else
    HAL_DelayUs(waitTimer->wakeupPeriod);
#endif

    return RET_ONGOING;

}


void SPI_WaitTimerInit(SPI_WaitTimer_T *waitTimer, uint32_t timeout, uint32_t wakeupPeriod)
{
    waitTimer->startTime = s_sysTime;
    waitTimer->wakeupPeriod = wakeupPeriod;
    waitTimer->timeout = timeout;
}

RET_CODE HAL_SPI_Init(SPI_Device_T *pSPI, SPI_Config_T *pConfig, SPI_Handle_T *hSPI)
{
    uint32_t format, ctrl;
    uint8_t index = MAX_SPI_DEVICE;

    if (pSPI == SPI0_DEV)
    {
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_SPI1);
        HAL_Reset_Module(RESET_SPI1_SW_RSTJ);
    }
    else if (pSPI == SPI1_DEV)
    {
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_SPI2);
        HAL_Reset_Module(RESET_SPI2_SW_RSTJ);
    }
    else if (pSPI == SPI2_DEV)
    {
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_SPI3);
        HAL_Reset_Module(RESET_SPI3_SW_RSTJ);
    }
    else if (pSPI == SPI3_DEV)
    {
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_SPI4);
        HAL_Reset_Module(RESET_SPI4_SW_RSTJ);
    }
    else if (pSPI == SPI4_DEV)
    {
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_SPI5);
        HAL_Reset_Module(RESET_SPI5_SW_RSTJ);
    }
    else if (pSPI == SPI5_DEV)
    {
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_SPI6);
        HAL_Reset_Module(RESET_SPI6_SW_RSTJ);
    }

    /* Disable spi device to exit from running operation before configuration */
    IO_BIT_CLEAR(pSPI->CTRL, SPI_CTRL_EN);

    /* Initialize some local variables about spi configurations */
    ctrl = 0;
    format = pConfig->format;

    /* SPI operation mode */
    ctrl |= (pConfig->mode == SPI_MASTER_MODE) ? SPI_CTRL_MS : 0;
    /* SPI single wire */
    ctrl |= (pConfig->singleWire) ? (SPI_CTRL_BIDIR | ((pConfig->mode == SPI_MASTER_MODE) ? SPI_CTRL_BIDIROE : 0)) : 0;

    /* SPI format,  freescale/ssi/microwire */
    ctrl |= ((format == SPI_FORMAT_MOTOLORA) ? SPI_CTRL_FMT_FREESCALE :
             ((format == SPI_FORMAT_TI_SSP) ? SPI_CTRL_FMT_TI :
              ((format == SPI_FORMAT_MICROWIRE) ? SPI_CTRL_FMT_MICROWIRE : 0)));
    /* Clock polarity */
    ctrl |= ((pConfig->polarity) ? SPI_CTRL_CPOL : 0);
    /* Clock phase */
    ctrl |= ((pConfig->phase) ? SPI_CTRL_CPHA : 0);
    /* Bit order */
    ctrl |= ((pConfig->bitOrder) ? SPI_CTRL_LSB : 0);
    /* Setup fields of spi control register */
    IO_BITMASK_SET(pSPI->CTRL, SPI_CTRL_MS  | SPI_CTRL_BIDIR | SPI_CTRL_BIDIROE | SPI_CTRL_TMODE | SPI_CTRL_XFRM |
                   SPI_CTRL_FMT | SPI_CTRL_CPOL  | SPI_CTRL_CPHA | SPI_CTRL_LSB, ctrl);

    /* Clock should be valid at SS interval for ssp */
    if (format == SPI_FORMAT_TI_SSP)
    {
        IO_BIT_SET(pSPI->CTRL, SPI_CTRL_CVSS);
    }

    /* Baud rate */
    pSPI->BR = SYS_CLOCK / pConfig->baudRate;

    /* Get index of this spi device */
    index = SPI_GetIndex(pSPI);
    if (index < MAX_SPI_DEVICE)
    {
        /* Handle of spi device */
        s_spiHandle[index] = hSPI;
        HAL_RequestIrq(s_spiIrq[index], s_spiIrqPrio[index], SPI_InterruptHandler, s_spiHandle[index]);
        /* Enable NVIC interrupt for this SPI device */
        HAL_IRQ_ENABLE(s_spiIrq[index]);
        /* Write device base address into handle structure */
        if (hSPI != NULL)
        {
            hSPI->device = pSPI;
            hSPI->callback = pConfig->callback;
            hSPI->index = index;
            hSPI->interruptMask = 0;
            hSPI->txExpectedSize = hSPI->txSize = 0;
            hSPI->rxExpectedSize = hSPI->rxSize = 0;
            hSPI->enableRxDMA = FALSE;
            hSPI->enableTxDMA = FALSE;
        }

        /* spi interrupt handler */
        s_spiIsr = SPI_InterruptHandler;
    }

    return RET_OK;
}

/**
 * @brief Sets SPI_Config_T structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for the SPI_Init().
 * User may modify some configurations before calling SPI_Init(). The function simplify configuration
 * process.

 * @param pConfig Pointer to SPI_Config_T structure
 * @param mode    Master mode or slave mode
 */
void HAL_SPI_GetDefaultConfig(SPI_Config_T *pConfig, SPI_TransferConfig_T *pTransferConfig, uint8_t mode)
{
    /* Default SPI fixed configuration, such as format, clock polarity, clock phase, bit order, and baud rate. */
    if (pConfig != NULL)
    {
        pConfig->mode = mode;
        pConfig->format = SPI_FORMAT_MOTOLORA;
        pConfig->polarity = SPI_CLOCK_POLARITY_LOW;
        pConfig->phase = SPI_CLOCK_PHASE_MIDDLE;
        pConfig->bitOrder = SPI_BIT_ORDER_MSB;
        pConfig->baudRate = SYS_CLOCK / 0x80;
        pConfig->singleWire = FALSE;
        pConfig->callback = NULL;
    }

    /* Default SPI configuration related to frames format, coutinous transfer, and dma. */
    if (pTransferConfig != NULL)
    {
        pTransferConfig->duplex = SPI_FULL_DUPLEX;
        pTransferConfig->frameConfig.mask =  SPI_MASK_DATA_FRAME;
        pTransferConfig->frameConfig.dataBits = 8;
        pTransferConfig->frameConfig.commandBits = 0;
        pTransferConfig->frameConfig.addressBits = 0;
        pTransferConfig->frameConfig.dummyBits = 0;
        pTransferConfig->continuedData = FALSE;
        pTransferConfig->enableRxDMA = FALSE;
        pTransferConfig->enableTxDMA = FALSE;
        pTransferConfig->continuousDataCount = 1;
        pTransferConfig->txWatermark = 4;
        pTransferConfig->rxWatermark = 4;
    }

    return;
}

void HAL_SPI_DeInit(SPI_Handle_T *hSPI)
{
    SPI_Device_T *pSPI = hSPI->device;

    /* Disable spi device */
    IO_BIT_CLEAR(pSPI->CTRL, SPI_CTRL_EN);

    return;
}

static void DataWriteByWidth(uint8_t *addr, __IO uint32_t val, uint8_t width)
{
    if (width == 1)
    {
        *((uint8_t *)addr) = (uint8_t)val;
    }
    else if (width == 2)
    {
        *((uint16_t *)addr) = (uint16_t)val;
    }
    else if (width == 4)
    {
        *((uint32_t *)addr) = (uint32_t)val;
    }
}

static void DataReadByWidth(uint8_t *addr, __IO uint32_t *ioAddr, uint8_t width)
{
    if (width == 1)
    {
        *ioAddr = *((uint8_t *)addr);
    }
    else if (width == 2)
    {
        *ioAddr = *((uint16_t *)addr);
    }
    else if (width == 4)
    {
        *ioAddr = *((uint32_t *)addr);
    }
}

static void SPI_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    RET_CODE ret;
    SPI_Handle_T *hSPI =  HAL_CONTAINER_OF(hDMA, SPI_Handle_T, rxDMAHandle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hSPI->rxDMAStatus = SPI_DMA_STATUS_FINISHED;
        /* User callback is always called here if rx dma is enabled. */
        if ((hSPI->enableRxDMA == TRUE) &&
                (hSPI->interruptMask & SPI_RX_DMA_INT))
        {
            ret = SPI_ProcessData(hSPI);
            if (ret == RET_OK)
            {
                if (NULL != hSPI->callback)
                {
                    hSPI->callback(hSPI, SPI_EVENT_TRANSFER_COMPLETE, (ulong_t)0);
                }
            }
        }
        HAL_DMA_Abort(hDMA);
    }
}

static void SPI_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    RET_CODE ret;
    SPI_Handle_T *hSPI =  HAL_CONTAINER_OF(hDMA, SPI_Handle_T, txDMAHandle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hSPI->txDMAStatus = SPI_DMA_STATUS_FINISHED;
        /* User callback is always called here if tx dma is enabled. */
        if ((hSPI->enableTxDMA == TRUE) &&
                (hSPI->interruptMask & SPI_TX_DMA_INT))
        {
            ret = SPI_ProcessData(hSPI);
            if (ret == RET_OK)
            {
                if (NULL != hSPI->callback)
                {
                    hSPI->callback(hSPI, SPI_EVENT_TRANSFER_COMPLETE, (ulong_t)0);
                }
            }
        }
        HAL_DMA_Abort(hDMA);
    }
}

static RET_CODE SPI_DMAConfig(SPI_Device_T *pSPI, uint8_t dataWidth, uint8_t dmaRead, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Device_T *pDMA = DMA_DEV;
    DMA_Request_Result_T reqResult;
    DMA_RequestType_T   type;
    RET_CODE ret = RET_OK;

    if (dmaRead)
    {
        if (pSPI == SPI0_DEV)
        {
            type = DMA_REQUEST_SPI1_RX;
        }
        else if (pSPI == SPI1_DEV)
        {
            type = DMA_REQUEST_SPI2_RX;
        }
        else if (pSPI == SPI2_DEV)
        {
            type = DMA_REQUEST_SPI3_RX;
        }
        else if (pSPI == SPI3_DEV)
        {
            type = DMA_REQUEST_SPI4_RX;
        }
        else if (pSPI == SPI4_DEV)
        {
            type = DMA_REQUEST_SPI5_RX;
        }
        else if (pSPI == SPI5_DEV)
        {
            type = DMA_REQUEST_SPI6_RX;
        }
        else
        {
            type = DMA_REQUEST_INVALID;
        }

        ret = HAL_DMA_Request_Channel(DMA_DEV, type, &reqResult);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
        dmaConfig.width     = ((dataWidth == 1) ? DMA_WIDTH_8BIT :
                               ((dataWidth == 2) ? DMA_WIDTH_16BIT :
                                ((dataWidth == 4) ? DMA_WIDTH_32BIT : DMA_WIDTH_8BIT)));
        dmaConfig.burst     = DMA_BURST_1;
        dmaConfig.priority  = DMA_PRIORITY_HIGH;
        dmaConfig.alter     = DMA_PRIMARY;
        dmaConfig.callback  = SPI_RxDMACallback;

        /* Configure DMA device for SPI Rx transfer */
        IO_BITMASK_SET(pSPI->RDMA, SPI_RDMA_RTH, SPI_RDMA_RTH_FIELD(1));
        IO_BIT_SET(pSPI->RDMA, SPI_RDMA_EN);
    }
    else
    {
        if (pSPI == SPI0_DEV)
        {
            type = DMA_REQUEST_SPI1_TX;
        }
        else if (pSPI == SPI1_DEV)
        {
            type = DMA_REQUEST_SPI2_TX;
        }
        else if (pSPI == SPI2_DEV)
        {
            type = DMA_REQUEST_SPI3_TX;
        }
        else if (pSPI == SPI3_DEV)
        {
            type = DMA_REQUEST_SPI4_TX;
        }
        else if (pSPI == SPI4_DEV)
        {
            type = DMA_REQUEST_SPI5_TX;
        }
        else if (pSPI == SPI5_DEV)
        {
            type = DMA_REQUEST_SPI6_TX;
        }
        else
        {
            type = DMA_REQUEST_INVALID;
        }

        ret = HAL_DMA_Request_Channel(DMA_DEV, type, &reqResult);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width     = ((dataWidth == 1) ? DMA_WIDTH_8BIT :
                               ((dataWidth == 2) ? DMA_WIDTH_16BIT :
                                ((dataWidth == 4) ? DMA_WIDTH_32BIT : DMA_WIDTH_8BIT)));
        dmaConfig.burst     = DMA_BURST_1;
        dmaConfig.priority  = DMA_PRIORITY_HIGH;
        dmaConfig.alter     = DMA_PRIMARY;
        dmaConfig.callback  = SPI_TxDMACallback;

        IO_BITMASK_SET(pSPI->TDMA, SPI_TDMA_TTH, SPI_TDMA_TTH_FIELD(1));
        IO_BIT_SET(pSPI->TDMA, SPI_TDMA_EN);
    }

    HAL_DMA_Init(pDMA, &dmaConfig, pDMAHandle);

    return RET_OK;
}

RET_CODE HAL_SPI_TransferConfig(SPI_Handle_T *hSPI, SPI_TransferConfig_T *pConfig)
{
    uint32_t frameMask, duplex, ctrl;
    SPI_Device_T *pSPI = hSPI->device;

    /* Disable spi device to exit from running operation before configuration */
    IO_BIT_CLEAR(pSPI->CTRL, SPI_CTRL_EN);

    hSPI->status = SPI_STATUS_IDLE;
    hSPI->txExpectedSize = hSPI->txSize = hSPI->tcount = hSPI->ctcount = 0;
    hSPI->rxExpectedSize = hSPI->rxSize = hSPI->rcount = hSPI->crcount = 0;
    hSPI->rxDMAStatus = hSPI->txDMAStatus = SPI_DMA_STATUS_STARTED;

    hSPI->enableTxDMA = pConfig->enableTxDMA;
    hSPI->enableRxDMA = pConfig->enableRxDMA;

    /* Initialize some local variables about spi configurations */
    ctrl = 0;
    hSPI->frameDelay = 0;
    hSPI->fixedFramesSize = 0;
    frameMask = pConfig->frameConfig.mask;
    duplex = pConfig->duplex;

    /* SPI duplex mode */
    ctrl |= ((duplex == SPI_FULL_DUPLEX) ? SPI_CTRL_TMODE_FULL_DUPLEX :
             ((duplex == SPI_HALF_DUPLEX) ? SPI_CTRL_TMODE_HALF_DUPLEX :
              ((duplex == SPI_TX_ONLY) ? SPI_CTRL_TMODE_TX_ONLY :
               ((duplex == SPI_RX_ONLY) ? SPI_CTRL_TMODE_RX_ONLY : 0))));
    /* Mask of fixed frames */
    if (frameMask & SPI_MASK_COMMAND_FRAME)
    {
        ctrl |= SPI_CTRL_XCMD;
        hSPI->fixedFrames[hSPI->fixedFramesSize++] = pConfig->frameConfig.command;
        hSPI->frameDelay++;
    }
    if (frameMask & SPI_MASK_ADDRESS_FRAME)
    {
        ctrl |= SPI_CTRL_XADDR;
        hSPI->fixedFrames[hSPI->fixedFramesSize++] = pConfig->frameConfig.address;
        hSPI->frameDelay++;
    }
    if (frameMask & SPI_MASK_DUMMY_FRAME)
    {
        ctrl |= SPI_CTRL_XDMY;
        pSPI->DMY = pConfig->frameConfig.dummy;
        hSPI->frameDelay++;
    }
    if (frameMask & SPI_MASK_DATA_FRAME)
    {
        ctrl |= SPI_CTRL_XDATA;
    }

    /* Get data frames bytes from bits of data frame */
    hSPI->dataWidth = ((pConfig->frameConfig.dataBits <= 8) ? 1 :
                       ((pConfig->frameConfig.dataBits <= 16) ? 2 : 4));

    /* Initialize continuous transmit/receive data count */
    hSPI->crcount = hSPI->ctcount = 1;
    if (pConfig->continuedData)
    {
        /* Indicate there are multiple data frames */
        ctrl |= SPI_CTRL_MFRM;
        pSPI->CCNT = pConfig->continuousDataCount;
        if (pConfig->duplex == SPI_TX_ONLY)
        {
            /* Get continuous transmit data count for tx only mode */
            hSPI->ctcount = pConfig->continuousDataCount;
        }
        else
        {
#if (SPI_IP_VERSION == 1)
            ctrl |= SPI_CTRL_RCONT;
            pSPI->CCNT = pConfig->continuousDataCount;
#endif
            hSPI->crcount = pConfig->continuousDataCount;
            if (pConfig->duplex == SPI_FULL_DUPLEX)
            {
                /* Get continuous transmit count for full-duplex mode */
                hSPI->ctcount = hSPI->crcount;
            }
            /* There is no continuous transmit data count for half-duplex mode and rx-only mode */
        }
    }
    hSPI->ctcount *= hSPI->dataWidth;
    hSPI->crcount *= hSPI->dataWidth;

    /* Setup fields of spi control register */
    IO_BITMASK_SET(pSPI->CTRL, SPI_CTRL_TMODE | SPI_CTRL_XFRM | SPI_CTRL_MFRM | SPI_CTRL_RCONT, ctrl);

    /* Sample timing: frame delay */
    IO_BITMASK_SET(pSPI->STM, SPI_STM_FRM, SPI_STM_FRM_FIELD(hSPI->frameDelay));

    /* Frame bits */
    IO_BITMASK_SET(pSPI->FRM, SPI_FRM_CMD | SPI_FRM_ADDR | SPI_FRM_DUMMY | SPI_FRM_DATA,
                   SPI_FRM_CMD_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XCMD, pConfig->frameConfig.commandBits))  |
                   SPI_FRM_ADDR_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XADDR, pConfig->frameConfig.addressBits)) |
                   SPI_FRM_DUMMY_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XDMY, pConfig->frameConfig.dummyBits))  |
                   SPI_FRM_DATA_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XDATA, pConfig->frameConfig.dataBits)));

    /* FIFO threshhold */
    IO_BITMASK_SET(pSPI->FFCFG, SPI_FFCFG_RFTH, SPI_FFCFG_RFTH_FIELD(pConfig->rxWatermark));
    IO_BITMASK_SET(pSPI->FFCFG, SPI_FFCFG_TFTH, SPI_FFCFG_TFTH_FIELD(pConfig->txWatermark));
    /* Enable spi device */
    IO_BIT_SET(pSPI->CTRL, SPI_CTRL_EN);

    return RET_OK;
}

/**
 * @brief Process all transferred data.
 *
 * @param hSPI      SPI device handle
 * @param waitTimer Pointer to timer.
 * @retval          Return RET_BUSY if transfter is ongoing.
 */
static RET_CODE SPI_ProcessData(SPI_Handle_T *hSPI)
{
    SPI_Device_T *pSPI = hSPI->device;
    uint8_t index, rcount, size, dataWidth = hSPI->dataWidth;
    uint32_t i = 0;
    uint32_t cnt_tmp = 0;

    /* First stage */
    /* Check dma over flag or transmit/receive data if one transfer is on progress */
    if (hSPI->status == SPI_STATUS_BUSY)
    {
        /* Wait for dma over status if dma enabled */
        if (hSPI->enableTxDMA)
        {
            if (hSPI->txDMAStatus == SPI_DMA_STATUS_FINISHED)
            {
                hSPI->txSize += hSPI->tcount * dataWidth;
                hSPI->tcount = 0;
            }
        }
        else
        {
            if (!hSPI->enableTxDMA)
            {
                /* Send data to data port if dma not enabled */
                while (hSPI->tcount != 0)
                {
                    if (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
                    {
                        break;
                    }
                    DataReadByWidth(&hSPI->pTxData[hSPI->txSize], &pSPI->DR, dataWidth);
                    hSPI->txSize += dataWidth;
                    hSPI->tcount -= dataWidth;
                }
            }
        }

        if (hSPI->enableRxDMA)
        {
            if (hSPI->rxDMAStatus == SPI_DMA_STATUS_FINISHED)
            {
                hSPI->rxSize += hSPI->rcount * dataWidth;
                hSPI->rcount = 0;
            }
        }
        else
        {
            /* Get data from data rx fifo */
            /* cpu code-execute is much faster than SPI data-transfer
             * thus, you may be get a smaller value than expected;
             */
            //rcount = SPI_FFLV_RF_GET(pSPI->FFLV);
            while (hSPI->rcount != 0)
            {

                /*wait till rx-fifo is ready*/
                while (!SPI_FFLV_RF_GET(pSPI->FFLV))
                {
                    __asm volatile("nop");
                }

                DataWriteByWidth(&hSPI->pRxData[hSPI->rxSize], pSPI->DR, dataWidth);
                hSPI->rxSize += dataWidth;
                hSPI->rcount -= dataWidth;
                rcount--;
            }
        }

        /* One transfer is over */
        if (hSPI->tcount == 0 && hSPI->rcount == 0)
        {
            /* Should I wait for fifo empty? */
            if (IO_BIT_GET(pSPI->CTRL, SPI_CTRL_MS))
            {
                while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFET));
            }
            /* Disable all interrupt after one transfer is over */
            IO_BIT_CLEAR(pSPI->CI, SPI_CI_EN);
            /* Total transfer is over if next transfer size == 0 */
            if ((hSPI->txExpectedSize == hSPI->txSize) && (hSPI->rxExpectedSize == hSPI->rxSize))
            {
                hSPI->status = SPI_STATUS_IDLE;
                //pSPI->IE = 0;
                //IO_BIT_CLEAR(pSPI->CI, SPI_CI_EN);
                //IO_BIT_CLEAR(pSPI->IE, SPI_IE_TFTH);
                return RET_OK;
            }
        }
    }

    /* Second stage is only valid when the status is idle or last transfer is over */
    if ((hSPI->status == SPI_STATUS_IDLE) ||
            (hSPI->tcount + hSPI->rcount) == 0)
    {
        /* Calculate next transfer size */
        hSPI->tcount = MIN((hSPI->txExpectedSize - hSPI->txSize), hSPI->ctcount);
        hSPI->rcount = MIN((hSPI->rxExpectedSize - hSPI->rxSize), hSPI->crcount);

        if ((hSPI->tcount + hSPI->rcount) == 0)
        {
            return RET_OK;
        }
        /* Transfer on progress */
        hSPI->status = SPI_STATUS_BUSY;

        /* Push fixed frames into fifo */
        size = hSPI->fixedFramesSize;
        index = 0;
        while (size != 0)
        {
            if (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
            {
                break;
            }
            pSPI->DR = hSPI->fixedFrames[index++];
            size--;
        }

        /* Star DMA operation */
        if (hSPI->enableTxDMA && hSPI->tcount != 0)
        {
            hSPI->txDMAStatus = SPI_DMA_STATUS_STARTED;
            HAL_DMA_Start(&hSPI->txDMAHandle, (((uint32_t)hSPI->pTxData) + hSPI->txSize), (uint32_t)(hSPI->device->DR), hSPI->tcount);
        }
        if (hSPI->enableRxDMA && hSPI->rcount != 0)
        {
            hSPI->rxDMAStatus = SPI_DMA_STATUS_STARTED;
            HAL_DMA_Start(&hSPI->rxDMAHandle, (uint32_t)(hSPI->device->DR), (((uint32_t)hSPI->pRxData) + hSPI->rxSize), hSPI->rcount);
        }

        /* Trigger spi by dummy write if no transmitted data */
        if (hSPI->fixedFramesSize == 0 && hSPI->tcount == 0)
        {
            cnt_tmp = hSPI->rxExpectedSize - hSPI->rxSize; //;*(volatile uint32_t *)(0x40031020);
            i = MIN(cnt_tmp, hSPI->rcount);
            hSPI->rcount = i;

            while (i)
            {
                while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
                {
                    __asm volatile("nop");
                }

                pSPI->DR = 0x00; //i
                i --;
            }
            pSPI->DR = 0;
        }
        else
        {
            /* Trigger master spi by pushing data into tx fifo */
            if (!hSPI->enableTxDMA)
            {
                /* Send data to data port if dma not enabled */

                while (hSPI->tcount != 0)
                {
                    if (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
                    {
                        break;
                    }
                    while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
                    {
                        __asm volatile("nop");
                    }
                    DataReadByWidth(&hSPI->pTxData[hSPI->txSize], &pSPI->DR, dataWidth);
                    hSPI->txSize += dataWidth;
                    hSPI->tcount -= dataWidth;
                }
            }
        }
        //size = SPI_FFLV_TF_GET(pSPI->FFLV);
        //if (size >= hSPI->
        //IO_BIT_SET(pSPI->IE, SPI_IE_TFTH);
        IO_BIT_SET(pSPI->CI, SPI_CI_EN);

    }
    return RET_BUSY;
}

RET_CODE HAL_SPI_Receive(SPI_Handle_T *hSPI, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    /*for full-duplex mode, read out all-data in fifo*/
    uint8_t data_cnt = 0;
    uint8_t val = 0;

    data_cnt = SPI_FFLV_RF_GET(hSPI->device->FFLV);
    while (data_cnt)
    {
        /*only apply for SPI1_DEV*/
        val = *(volatile uint8_t *)(hSPI->device->DR);
        data_cnt --;
    }

    return HAL_SPI_TransmitReceive(hSPI, NULL, pData, 0, size, timeout);
}

RET_CODE HAL_SPI_Transmit(SPI_Handle_T *hSPI, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    return HAL_SPI_TransmitReceive(hSPI, pData, NULL, size, 0, timeout);
}

void SPI_InterruptHandler(SPI_Handle_T *hSPI)
{
    SPI_Device_T *pSPI = hSPI->device;
    uint32_t status;
    RET_CODE ret;

    status = pSPI->IS;

    /* Clear spi interrupt */
    pSPI->IS = status;

    /* Process data transfer in context of interrupt handler */
    ret = SPI_ProcessData(hSPI);

    /* Notify user by callback */
    if (ret == RET_OK)
    {
        if (hSPI->callback != NULL)
        {
            hSPI->callback(hSPI, SPI_EVENT_TRANSFER_COMPLETE, NULL);
        }
    }
    else if (ret == RET_ERROR)
    {
        if (hSPI->callback != NULL)
        {
            hSPI->callback(hSPI, SPI_EVENT_TRANSFER_ERROR, NULL);
        }
    }

    return;
}

RET_CODE HAL_SPI_TransmitReceive(SPI_Handle_T *hSPI, uint8_t *pTxData, uint8_t *pRxData, uint16_t txSize, uint16_t rxSize, uint32_t timeout)
{
    SPI_Device_T *pSPI = hSPI->device;
    SPI_WaitTimer_T waitTimer;
    RET_CODE ret;
    uint8_t dataWidth = hSPI->dataWidth;

    /* Return immediately if SPI is on progress */
    if (hSPI->status == SPI_STATUS_BUSY)
    {
        return RET_BUSY;
    }

    if (pTxData != NULL)
    {
        pSPI->CCNT = txSize;
    }

    if (pRxData != NULL)
    {
        pSPI->CCNT = rxSize;
    }

    /* Process null pointers and zero transfer size */
    if (pTxData == NULL)
    {
        txSize = hSPI->ctcount = 0;
    }
    if (pRxData == NULL)
    {
        rxSize = hSPI->crcount = 0;
    }
    if (txSize == 0 && rxSize == 0)
    {
        return RET_ERROR;
    }

    /* Initialize variables */
    hSPI->txSize = hSPI->rxSize = 0;
    /* Address should be aligned to boundary by data width (byte/half word/word) */
    if (((uint32_t)pRxData) & ((uint32_t)(dataWidth - 1)))
    {
        return RET_ERROR;
    };
    if (((uint32_t)pTxData) & ((uint32_t)(dataWidth - 1)))
    {
        return RET_ERROR;
    };
    /* Size should be aligned to boundary by data width (byte/half word/word) */
    hSPI->txExpectedSize = txSize & (~((uint16_t)(dataWidth - 1)));
    hSPI->rxExpectedSize = rxSize & (~((uint16_t)(dataWidth - 1)));
    hSPI->pRxData = pRxData;
    hSPI->pTxData = pTxData;

    /* Enable spi device */
    IO_BIT_SET(pSPI->CTRL, SPI_CTRL_EN);

    /* Configure DMA parameters */
    if (hSPI->enableTxDMA)
    {
        SPI_DMAConfig(pSPI, dataWidth, FALSE, &hSPI->txDMAHandle);
        hSPI->txDMAStatus = SPI_DMA_STATUS_STARTED;
    }
    if (hSPI->enableRxDMA)
    {
        SPI_DMAConfig(pSPI, dataWidth, TRUE, &hSPI->rxDMAHandle);
        hSPI->rxDMAStatus = SPI_DMA_STATUS_STARTED;
    }

    /* Initialize timeout structure */
    if (timeout != TIMEOUT_NO_BLOCKING)
    {
        SPI_WaitTimerInit(&waitTimer, timeout, 1);
    }

    do
    {
        ret = SPI_ProcessData(hSPI);

        /* Exit if it returns RET_OK. */
        if (ret != RET_BUSY)
        {
            break;
        }
        /* Exit if it is non-blocking call */
        if (timeout == TIMEOUT_NO_BLOCKING)
        {
            ret = RET_ONGOING;
            break;
        }

        /* Wait and do nothing if TX  or Rx interrupt handling is enabled. */
        if (hSPI->interruptMask & (SPI_RX_INT | SPI_TX_INT | SPI_RX_DMA_INT | SPI_TX_DMA_INT))
        {
            while (1)
            {
                /* Exit if all expected user data has been put into user buffer. */
                if ((hSPI->txExpectedSize == hSPI->txSize) && (hSPI->rxExpectedSize == hSPI->rxSize))
                {
                    ret = RET_OK;
                    break;
                }

                /* Wait for a while if it is called in blocking function */
                if (SPI_WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
                {
                    ret = RET_TIMEOUT;
                    break;
                };
            };
            break;
        }
        else
        {
            /* Wait for a while if it is called in blocking function */
            if (SPI_WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
            {
                ret = RET_TIMEOUT;
                break;
            };
        }

    } while (1);

    return ret;
}

void HAL_SPI_EnableInterrupts(SPI_Handle_T *hSPI, uint32_t mask)
{
    SPI_Device_T *pSPI = hSPI->device;

    if (mask & SPI_TX_INT)
    {
        IO_BIT_SET(pSPI->IE, (SPI_IE_TFET | SPI_IE_TFOV));  //| SPI_IE_TFTH));
    }
    if (mask & SPI_RX_INT)
    {
        IO_BIT_SET(pSPI->IE, (SPI_IE_RFOV | SPI_IE_RFFL | SPI_IE_RFTH));
    }
    if (mask & SPI_RX_DMA_INT)
    {
        IO_BIT_SET(pSPI->IE, SPI_IE_DMARX);
    }
    if (mask & SPI_TX_DMA_INT)
    {
        IO_BIT_SET(pSPI->IE, SPI_IE_DMATX);
    }

    hSPI->interruptMask |= mask;
}

void HAL_SPI_DisableInterrupts(SPI_Handle_T *hSPI, uint32_t mask)
{
    SPI_Device_T *pSPI = hSPI->device;

    if (mask & SPI_TX_INT)
    {
        IO_BIT_CLEAR(pSPI->IE, (SPI_IE_TFET | SPI_IE_TFOV | SPI_IE_TFTH));
    }
    if (mask & SPI_RX_INT)
    {
        IO_BIT_CLEAR(pSPI->IE, (SPI_IE_RFUF | SPI_IE_RFOV | SPI_IE_RFFL | SPI_IE_RFTH));
    }
    if (mask & SPI_RX_DMA_INT)
    {
        IO_BIT_CLEAR(pSPI->IE, SPI_IE_DMARX);
    }
    if (mask & SPI_TX_DMA_INT)
    {
        IO_BIT_CLEAR(pSPI->IE, SPI_IE_DMATX);
    }

    hSPI->interruptMask &= ~mask;
}
