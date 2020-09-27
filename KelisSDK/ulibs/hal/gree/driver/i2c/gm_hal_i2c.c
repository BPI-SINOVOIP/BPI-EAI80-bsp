/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          i2c.c
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/05
 *
 * @brief         Implementation of i2c Driver.
 *
 * @note
 *    2016/07/05, wizephen.wang, V1.0.0
 *        Initial version.
 *
 */

#include <gm_hal_i2c.h>
#include <adc/gm_ll_adc.h>
#include <adc/gm_ll_adc_dev.h>
#include <tim/gm_ll_tim.h>
#include <gm_soc.h>
#include <gm_irq.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>
//#include <systick.h>
//#include <kernel.h>

void HAL_I2C0_InterruptHandler(void);
void HAL_I2C1_InterruptHandler(void);
void HAL_I2C2_InterruptHandler(void);

/* DMA handle instance */
//#define DMA_66XX_OK   1
static DMA_Handle_T s_dmaHandle;

/* Pointers to I2C bases for each instance */
static I2C_Device_T *const s_i2cBases[MAX_I2C_DEVICE] = {I2C0, I2C1, I2C2};
/* Pointers to I2C handles for each instance */
static I2C_Handle_T *s_i2cHandle[MAX_I2C_DEVICE] = {NULL, NULL, NULL};

#if (!defined(require_action_quiet))
#define require_action_quiet( X, LABEL, ACTION )    \
    do                                              \
    {                                               \
        if( ( !(X) ) )                              \
        {                                           \
            { ACTION; }                             \
            goto LABEL;                             \
        }                                           \
        \
    }   while( 1==0 )
#endif
RET_CODE i2c_WaitTillTimeout(i2c_WaitTimer_T *waitTimer)
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
#endif

#ifndef RABBIT_NO_OS_SYSTEM
    HAL_DelayUs(waitTimer->wakeupPeriod);
#endif

    return RET_ONGOING;

}

#if (!defined(i2cwait_till_timeout))
#define i2cwait_till_timeout \
    if (i2c_WaitTillTimeout(&waitTimer) == RET_TIMEOUT) \
    {                                               \
        hI2C->errorCode = I2C_ERRCODE_TIMEOUT;      \
        return RET_TIMEOUT;                         \
    }
#endif

void i2c_WaitTimerInit(i2c_WaitTimer_T *waitTimer, uint32_t timeout, uint32_t wakeupPeriod)
{
    waitTimer->startTime = s_sysTime;
    waitTimer->wakeupPeriod = wakeupPeriod;
    waitTimer->timeout = timeout;
}

/*
 *DMA module is occupied exclusively, once one DMA channel is occupied by
 *some module (I2C, etc.), any other channel of DMA can be used until such
 *channel is released. That's why here there's only a common callback subroutine
 *serving for each individual callback request
 */
static volatile uint8_t s_dmaComplete = 0;
static volatile uint8_t s_dmaError    = 0;

void I2C_CommonIRQHandler(I2C_Handle_T *hI2C);
/*Declarition of i2c interrupt service routine*/
void HAL_I2C0_InterruptHandler(void)
{
    I2C_CommonIRQHandler(s_i2cHandle[I2CDEV_0]);
}
void HAL_I2C1_InterruptHandler(void)
{
    I2C_CommonIRQHandler(s_i2cHandle[I2CDEV_1]);
}
void HAL_I2C2_InterruptHandler(void)
{
    I2C_CommonIRQHandler(s_i2cHandle[I2CDEV_2]);
}

static __inline uint32_t I2C_GetIndex(I2C_Device_T *pI2C)
{
    uint32_t i;

    /* Find the index from base address mappings. */
    for (i = 0; i < MAX_I2C_DEVICE; i++)
    {
        if (s_i2cBases[i] == pI2C)
        {
            break;
        }
    }

    ASSERT(i < MAX_I2C_DEVICE);

    return i;
}

#if 0
static __inline void I2C_NVICEnable(I2C_Device_T *pI2C)
{
#if 1//#ifdef NO_OS
    if (pI2C == I2C0)
    {
        HAL_RequestIrq(I2C0_IRQn, I2C0_IRQ_PRIORITY, HAL_I2C0_InterruptHandler, 0);
        HAL_IRQ_ENABLE(I2C0_IRQn);
    }
    else if (pI2C == I2C1)
    {
        HAL_RequestIrq(I2C1_IRQn, I2C1_IRQ_PRIORITY, HAL_I2C1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(I2C1_IRQn);
    }
    else if (pI2C == I2C2)
    {
        HAL_RequestIrq(I2C2_IRQn, I2C2_IRQ_PRIORITY, HAL_I2C2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(I2C2_IRQn);
    }
#endif
}
#endif

static void I2C_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{

    I2C_Handle_T *pI2CHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, struct I2C_Handle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
        HAL_DMA_Abort(hDMA);
        pI2CHandle->state = I2C_STATE_IDLE;
        pI2CHandle->errorCode = I2C_ERRCODE_NOERROR;

        /* Clear all possible pending interrupt flag */
        I2C_CLEAR_COMBINEDINT(pI2CHandle->pDevice);

        /* Disable all interrupts */
        I2C_DISABLE_ALLINTS(pI2CHandle->pDevice);

        if (pI2CHandle->callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pI2CHandle->callback(pI2CHandle, I2C_EVENT_TRANSFER_DONE, 0);
        }
    }

    if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
        HAL_DMA_Abort(hDMA);
        pI2CHandle->errorCode = I2C_ERRCODE_DMAERROR;

        /* Clear all possible pending interrupt flag */
        I2C_CLEAR_COMBINEDINT(pI2CHandle->pDevice);

        /* Disable all interrupts */
        I2C_DISABLE_ALLINTS(pI2CHandle->pDevice);

        if (pI2CHandle->callback)
        {
            /* Notify usr app that errors occur during data-transcation */
            pI2CHandle->callback(pI2CHandle, I2C_EVNET_TRANSFER_ERROR, 0);
        }
    }

    //HAL_DMA_Abort(hDMA);

}


static void I2C_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    I2C_Handle_T *pI2CHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, struct I2C_Handle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
        HAL_DMA_Abort(hDMA);
        pI2CHandle->state = I2C_STATE_IDLE;
        pI2CHandle->errorCode = I2C_ERRCODE_NOERROR;

        /* Clear all possible pending interrupt flag */
        I2C_CLEAR_COMBINEDINT(pI2CHandle->pDevice);

        /* Disable all interrupts */
        I2C_DISABLE_ALLINTS(pI2CHandle->pDevice);

        if (pI2CHandle->callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pI2CHandle->callback(pI2CHandle, I2C_EVENT_TRANSFER_DONE, 0);
        }
    }

    if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
        HAL_DMA_Abort(hDMA);
        pI2CHandle->errorCode = I2C_ERRCODE_DMAERROR;

        /* Clear all possible pending interrupt flag */
        I2C_CLEAR_COMBINEDINT(pI2CHandle->pDevice);

        /* Disable all interrupts */
        I2C_DISABLE_ALLINTS(pI2CHandle->pDevice);

        if (pI2CHandle->callback)
        {
            /* Notify usr app that errors occur during data-transcation */
            pI2CHandle->callback(pI2CHandle, I2C_EVNET_TRANSFER_ERROR, 0);
        }
    }
}

static RET_CODE I2C_DMAConfig(I2C_Device_T *pI2C, uint8_t dmaWrite, uint8_t i2cMode, DMA_Handle_T *hDMA)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;


    ASSERT(pI2C != NULL && hDMA != NULL);

    //__ASSERT((pI2C != NULL) && (hDMA != NULL), "");

    if (dmaWrite)
    {
        if (pI2C == I2C0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_I2C1_TX, &reqResult);
        }
        else if (pI2C == I2C1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_I2C2_TX, &reqResult);
        }
        else if (pI2C == I2C2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_I2C3_TX, &reqResult);
        }

        if (ret != RET_OK)
        {
            //fail, need return fail status
            return RET_ERROR;
        }
        memset(&dmaConfig, 0, sizeof(dmaConfig));

        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width = DMA_WIDTH_16BIT;
        dmaConfig.burst = DMA_BURST_1;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;


        dmaConfig.callback = I2C_TxDMACallback;

        //LL_I2C_TX_DMA_TDL(pI2C, MAX_TX_FIFO_DEPTH/2);
        //LL_I2C_TX_DMA_ENABLE(pI2C);
        //LL_I2C_RX_DMA_DISABLE(pI2C);

        IO_BITMASK_SET(pI2C->DMA, (uint32_t)((uint32_t)0x1F << ((uint32_t) 24)), ((MAX_TX_FIFO_DEPTH / 2) << ((uint32_t) 24)));
        IO_BIT_SET(pI2C->DMA, ((uint32_t)((uint32_t)0x1 << (uint32_t) 16)));
        IO_BIT_CLEAR(pI2C->DMA, ((uint32_t)((uint32_t)0x1 << (uint32_t) 0)));
    }
    else
    {
        if (pI2C == I2C0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_I2C1_RX, &reqResult);
        }
        else if (pI2C == I2C1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_I2C2_RX, &reqResult);
        }
        else if (pI2C == I2C2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_I2C3_RX, &reqResult);
        }

        if (ret != RET_OK)
        {
            //fail, need return fail status
            return RET_ERROR;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));

        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
        dmaConfig.width = DMA_WIDTH_8BIT;
        dmaConfig.burst = DMA_BURST_1;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;

        dmaConfig.callback = I2C_RxDMACallback;

        //LL_I2C_RX_DMA_RDL(pI2C, 0);
        //LL_I2C_RX_DMA_ENABLE(pI2C);
        //LL_I2C_TX_DMA_DISABLE(pI2C);

        IO_BITMASK_SET(pI2C->DMA, ((uint32_t)((uint32_t)0x1F << ((uint32_t) 8))), ((MAX_RX_FIFO_DEPTH / 2)  << ((uint32_t) 8)));
        IO_BIT_SET(pI2C->DMA, ((uint32_t)((uint32_t)0x1 << ((uint32_t) 0))));
        IO_BIT_CLEAR(pI2C->DMA, ((uint32_t)((uint32_t)0x1 << ((uint32_t) 16))));

    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, hDMA);

    return RET_OK;
}

/**
 * @brief Initialize I2C operation handler.
 *
 * This intializes I2C operation handler elements (specify DMA device handler, etc)
 *
 * @param pI2C Pointer to I2C device handler.
 * @param pConfig Pointer to I2C configuration structure.
 * @param hI2C Pointer to I2C transaction handler.
 *
 * @retval RET_OK.
 * @retval RET_INVPARAM Invalid parameters.
 */
static RET_CODE I2C_InitHandle(I2C_Device_T *pI2C, I2C_Config_T *pConfig, I2C_Handle_T *hI2C)
{
    //uint16_t s_dmabuffer[2] = {0};
    //uint32_t s_dmaBufferLen  = 2;
    ASSERT(pI2C != NULL && pConfig != NULL && hI2C != NULL);

    hI2C->state        = I2C_STATE_IDLE;
    hI2C->errorCode    = I2C_ERRCODE_NOERROR;
    hI2C->pDevice      = pI2C;
    hI2C->pRxBuffer    = NULL;
    hI2C->pTxBuffer    = NULL;
    hI2C->rxFifoDepth  = MAX_RX_FIFO_DEPTH;
    hI2C->rxRemained   = 0;
    hI2C->rxLength     = 0;
    hI2C->rxThreshld   = I2C_GET_FIFOTHRD_RX(hI2C->pDevice);
    hI2C->txFifoDepth  = MAX_TX_FIFO_DEPTH;
    hI2C->txRemained   = 0;
    hI2C->txLength     = 0;
    hI2C->txThreshld   = I2C_GET_FIFOTHRD_TX(hI2C->pDevice);
    hI2C->callback     = pConfig->callback;
    hI2C->enableDMA    = pConfig->enableDMA;

    if (hI2C->enableDMA == TRUE)
    {
        //hI2C->pdmaBuffer      = s_dmabuffer;
        //hI2C->dmaBufferLength = s_dmaBufferLen;
        hI2C->pdmaBuffer      = pConfig->pdmaBuffer;
        hI2C->dmaBufferLength = pConfig->dmaBufferLength;
    }

    return RET_OK;
}

/**
 * @brief Configure I2C peripheral baudrate.
 *
 * @param hI2C Pointer to I2C transaction handler.
 * @param pConfig Pointer to I2C configuration structure.
 *
 * @retval RET_OK.
 * @retval RET_ERROR.
 */

#if 1
static RET_CODE I2C_ConfigBaudRate(I2C_Handle_T *hI2C, I2C_Config_T *pConfig)
{
    uint32_t clkFrq;
    uint16_t lpcc, hpcc;

    //OSC_Config_T oscConfig;

    if (pConfig->workMode == I2C_MASTER_MODE)
    {
        if (I2C_DEV_ISENABLED(hI2C->pDevice))
        {
            return RET_ERROR;
        }

        /* Get oscilator configuration */
        //OSC_GetConfig(&oscConfig);

#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
        if (oscConfig.pllConfig.pllBypassEnable)
        {
            clkFrq = I2C_CLOCKFREQUENCY_8M;
        }
        else
        {
            clkFrq = I2C_CLOCKFREQUENCY_16M;
        }
#endif

#if  defined(CHIP_GM6721) || defined(CHIP_F6721B)
        /*actually, it equals to FCLK, PLL bypass feature not exist in GM6611*/
        //clkFrq = I2C_CLOCKFREQUENCY_8M;
        clkFrq = I2C_CLOCKFREQUENCY_25M;
#endif
#if 1
        /* I2C baudrate calculation formula */
        lpcc = (clkFrq / (pConfig->baudrate * 1000)) / 2 - 3;
        hpcc = (clkFrq / (pConfig->baudrate * 1000)) / 2 + 1;
#endif


        I2C_SET_HPLPCC_HIGHCNT(hI2C->pDevice, hpcc, lpcc);
    }

    /* DO NOT CHANGE THE FOLLOWING SEQUENCE !!! */
    // hI2C->pDevice->SH = 0x8005000C;      //previous  config
    // hI2C->pDevice->SH = 0x0005000C;

    hI2C->pDevice->SH = 0x800D000C;   // new  config
    hI2C->pDevice->SH = 0x000D000C;
    return RET_OK;
}
#endif

/**
 * @brief Configure I2C Data&Command Register.
 *
 * This determines whether I2C writes or reads to/from a couterpart I2C device.
 * Aside from that, it can also stop the transaction after a squence of data by
 * assertting STOP_ASSERT bit or restart the transaction by assertting RESTART_ASSERT bit.
 *
 * @param hI2C Pointer to I2C transaction handler.
 * @param dataCmdType Indicate a restart or stop command.
 * @param value Only valid when writing to a couterpart I2C device.
 * @param write Indicate I2C read or write.
 */
static void I2C_DataCmdRegManipulate(I2C_Handle_T *hI2C, I2C_DataCmdType_T dataCmdType, uint8_t value, I2C_RdWrType_T write)
{
    ASSERT(hI2C != NULL);

    if (write == I2C_DATACMD_WRITE)
    {
        if (dataCmdType == I2C_DATA_CMD_NULL)
        {
            //*((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint8_t)(value);
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint16_t)(value);
        }
        else if (dataCmdType == I2C_DATA_CMD_RESTART)
        {
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint16_t)(I2C_DT_ARST | value);
        }
        else
        {
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint16_t)(I2C_DT_ASTP | value);
        }
    }
    else
    {
        UNUSED(value);
        if (dataCmdType == I2C_DATA_CMD_NULL)
        {
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint16_t)(I2C_DT_RD);
        }
        else if (dataCmdType == I2C_DATA_CMD_RESTART)
        {
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint16_t)(I2C_DT_RD | I2C_DT_ARST);
        }
        else
        {
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint16_t)(I2C_DT_RD | I2C_DT_ASTP);
        }
    }
}

/**
 * @brief Configure DMA specially when I2C receives data.
 *
 * @param hI2C Pointer to I2C transaction handler.
 * @param mode Indicate whether a Master or Slave mode.
 * @para pData  A pointer to data buffer to be received.
 * @para size The count of data to be received.
 */
static RET_CODE DMAConfig_for_I2CRecv(I2C_Handle_T *hI2C, I2C_WorkMode_T mode, uint8_t *pData, uint16_t size)
{
    RET_CODE error = RET_OK;

#if   1//DMA_66XX_OK
    uint32_t srcAddress = 0x0;
    uint32_t tmp = 0;


    require_action_quiet(I2C_DMAConfig(hI2C->pDevice, FALSE, mode, &s_dmaHandle) == RET_OK, \
                         exit, error = RET_BUSY; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);

    /* Associate DMA Handle with I2C handle */
    LINK_DMAHDL_TO_PARENTHDL(hI2C, hDMA, s_dmaHandle);
    /* Configure DMA source address */
    srcAddress = (uint32_t)(&(hI2C->pDevice->DT));

    tmp = 0x00000000;
    tmp |= I2C_DMA_TXTHR_FIELD(0) | I2C_DMA_RXTHR_FIELD(0);
    hI2C->pDevice->DMA = tmp;
    /* Enable DMA correpoding bitfields of I2C peripheral */

    if ((s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_12) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_7) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_8) || \
            (s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_7) || \
            (s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_4) || \
            (s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_6))
    {

        I2C_DMACTRL_TXEN_ENABLE(hI2C->pDevice);

    }


    if ((s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_0) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_2) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_3) || \
            (s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_2) || \
            (s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_9) || \
            (s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_5))
    {

        I2C_DMACTRL_RXEN_ENABLE(hI2C->pDevice);

    }
    /* Start DMA transcation */
    require_action_quiet(HAL_DMA_Start(hI2C->hDMA, srcAddress, (uint32_t)pData, size) == RET_OK, \
                         exit, error = RET_ERROR; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);
exit:
#endif
    return error;
}

/**
 * @brief Configure DMA specially when I2C transmits data.
 *
 * @param hI2C Pointer to I2C transaction handler.
 * @param mode Indicate whether a Master or Slave mode.
 * @para pData  A pointer to data buffer to be transferred.
 * @para size The count of data to be transferred.
 */
static RET_CODE DMAConfig_for_I2CTrmt(I2C_Handle_T *hI2C, I2C_WorkMode_T mode, uint8_t *pData, uint16_t size)
{
    RET_CODE error = RET_OK;

#if   1 //DMA_66XX_OK
    uint32_t destAddress = 0x0;
    uint16_t *pBuffer = hI2C->pdmaBuffer;
    uint32_t tmp = 0;
    uint8_t  i = 0;

    //memset(hI2C->pdmaBuffer, sizeof(hI2C->pdmaBuffer), 0);
    //memset((unsigned int *)0xC010000C,0, 20);

    if ((uint32_t)(hI2C->pdmaBuffer) & 0x01 ||
            hI2C->dmaBufferLength > MAX_BLOCK_SIZE_LENGTH)
    {
        return RET_INVPARAM;
    }

    /* Copy data in pData to DMA buffer, it's 16Bit aligned by usr code */

    for (i = 0; i < size; i++)
    {
        if (i == size - 1)
        {
            /* Issue a stop bit at the end of data stream */
            *pBuffer = *pData;
            *pBuffer |=  I2C_DT_ASTP;
        }
        else
        {
            *pBuffer = *pData;
        }
        pBuffer ++;
        pData ++;
    }


    require_action_quiet(I2C_DMAConfig(hI2C->pDevice, TRUE, mode, &s_dmaHandle) == RET_OK, \
                         exit, error = RET_BUSY; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);
    /* Associate DMA Handle with I2C handle */
    LINK_DMAHDL_TO_PARENTHDL(hI2C, hDMA, s_dmaHandle);

    /* Configure DMA destination address */
    destAddress = (uint32_t)(&(hI2C->pDevice->DT));

    tmp = 0x00000000;
    tmp |= I2C_DMA_TXTHR_FIELD(0) | I2C_DMA_RXTHR_FIELD(0);
    hI2C->pDevice->DMA = tmp;

    if ((s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_12) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_7) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_8) || \
            (s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_7) || \
            (s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_4) || \
            (s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_6))
    {

        I2C_DMACTRL_TXEN_ENABLE(hI2C->pDevice);

    }


    if ((s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_0) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_2) || \
            (s_dmaHandle.devIdx == DMA_DEV_7 && s_dmaHandle.chIdx == DMA_CHANNEL_3) || \
            (s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_2) || \
            (s_dmaHandle.devIdx == DMA_DEV_3 && s_dmaHandle.chIdx == DMA_CHANNEL_9) || \
            (s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_5))
    {

        I2C_DMACTRL_RXEN_ENABLE(hI2C->pDevice);

    }


    /* Start DMA transcation */
    require_action_quiet(HAL_DMA_Start(hI2C->hDMA, (uint32_t)hI2C->pdmaBuffer, (uint32_t)destAddress, size) == RET_OK, \
                         exit, error = RET_ERROR; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);

exit:
#endif
    return error;
}


static RET_CODE Wait_tillDMACompletes(I2C_Handle_T *hI2C, i2c_WaitTimer_T *waitTimer)
{
    RET_CODE error = RET_OK;

#if 1//DMA_66XX_OK
    /* Wait till DMA completes */
    while (1)
    {
        if (s_dmaComplete == 1 || s_dmaError == 1)
        {
            //HAL_DMA_Abort();
            break;
        }

        if (i2c_WaitTillTimeout(waitTimer) == RET_TIMEOUT)
        {
            hI2C->errorCode = I2C_ERRCODE_TIMEOUT;
            return RET_TIMEOUT;
        }
    }

    /* DMA error occures */
    if (s_dmaError == 1)
    {
        require_action_quiet(0, exit, error = RET_ERROR; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);
    }

    /* De-intialize DMA */
    require_action_quiet(HAL_DMA_Abort(&s_dmaHandle) == RET_OK, exit, error = RET_ERROR; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);
    /* Change back to IDLE state */
    hI2C->state = I2C_STATE_IDLE;

exit:
#endif
    return error;
}
//static RET_CODE Write_Data2TxFIFO(I2C_Handle_T *hI2C, i2c_WaitTimer_T *waitTimer,
//uint8_t data, I2C_DataCmdType_T dataCmdType,
//I2C_RdWrType_T write)
static RET_CODE Write_Data2TxFIFO(I2C_Handle_T *hI2C, i2c_WaitTimer_T *waitTimer,
                                  uint16_t data, I2C_DataCmdType_T dataCmdType,
                                  I2C_RdWrType_T write)
{
    RET_CODE error = RET_OK;
    I2C_DataCmdRegManipulate(hI2C, dataCmdType, data, write);
#if 1
    while (1)
    {
        /* Check error status while data-transmitting */
        if (I2C_GET_ERR_SOURCES(hI2C->pDevice, 0xFFFF))
        {
            return RET_ERROR;
        }

        /* Wait till Tx FIFO is not full */
        if (!(hI2C->pDevice->ST & I2C_ST_TXF))
        {
            break;
        }
#if 1//#ifdef NO_OS
        if (i2c_WaitTillTimeout(waitTimer) == RET_TIMEOUT)
        {
            hI2C->errorCode = I2C_ERRCODE_TIMEOUT;
            return RET_TIMEOUT;
        }
#endif
    }
#endif

    return error;
}


RET_CODE I2C_DeInit(I2C_Handle_T *hI2C)
{
    /*Disable all interrupts and clear all interrupts*/
    I2C_DISABLE_ALLINTS(hI2C->pDevice);
    I2C_CLEAR_COMBINEDINT(hI2C->pDevice);

    /*Disable I2C peripheral*/
    I2C_DEV_DISABLE(hI2C->pDevice);

    return RET_OK;
}

RET_CODE I2C_Init(I2C_Device_T *pI2C, I2C_Config_T *pConfig, I2C_Handle_T *hI2C)
{
    uint32_t regtmp;
    uint32_t read_data;
    RET_CODE error = RET_ERROR;

    ASSERT(pI2C != NULL && pConfig != NULL && hI2C != NULL);
    if (pI2C == I2C0)
    {
        HAL_Reset_Module(RESET_I2C1_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_I2C1);
        read_data = READ_REG(*(volatile uint32_t *) 0x400000a4);
        read_data |= (7 << 8);
        WRITE_REG(*(volatile uint32_t *) 0x400000a4, read_data);
    }
    if (pI2C == I2C1)
    {
        HAL_Reset_Module(RESET_I2C2_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_I2C2);
        read_data = READ_REG(*(volatile uint32_t *) 0x400000a4);
        read_data |= (7 << 24);
        WRITE_REG(*(volatile uint32_t *) 0x400000a4, read_data);
    }
    if (pI2C == I2C2)
    {
        HAL_Reset_Module(RESET_I2C3_SW_RSTJ);
        HAL_CLOCK_PeripheralClockEnable1(SYS_ENCLK1_I2C3);
        read_data = READ_REG(*(volatile uint32_t *) 0x400000a8);
        read_data |= (7 << 8);
        WRITE_REG(*(volatile uint32_t *) 0x400000a8, read_data);
    }
    //uint32_t  read_data;
    if (I2C_DEV_ISACTIVE(pI2C))
    {
        return RET_BUSY;
    }

    I2C_DEV_DISABLE(pI2C);

    require_action_quiet(I2C_InitHandle(pI2C, pConfig, hI2C) == RET_OK, \
                         exit, error = RET_INVPARAM);

    s_i2cHandle[I2C_GetIndex(pI2C)] = hI2C;

    /* Enable restart function */
    I2C_RESTART_ENABLE(pI2C);
    /* Enable general call acknowledgement */
    I2C_GCACK_ENABLE(pI2C);

    /* Disable all interrupts and clear all interrupts */
    I2C_DISABLE_ALLINTS(pI2C);

    /*
     *Clear possible pending interrupt flags to avoid unexpected IRQs
     *once some interrupt is enabled
     */
    I2C_CLEAR_COMBINEDINT(pI2C);

    /* Set Stardard, Fast Speed Mode SCL count */
    I2C_ConfigBaudRate(hI2C, pConfig);

    if (pConfig->workMode == I2C_MASTER_MODE)
    {
        I2C_MST_MODE_ENABLE(pI2C);
    }
    else
    {
        I2C_SLV_MODE_ENABLE(hI2C->pDevice);
        /* Configure I2C local address in slave mode */
        pI2C->TSAR = ((uint8_t)(pConfig->localAddress)) << 16;
        //pI2C->TSAR = ((uint32_t)(pConfig->localAddress)) << 16;
    }

    /* General call mode */
    if (pConfig->masterTxMode == I2C_GENERAL_CALL_MODE)
    {
        I2C_GCSTARTSEL_GC(pI2C);
        I2C_GCSTART_ENABLE(pI2C);
    }
    /* Start byte mode */
    else if (pConfig->masterTxMode == I2C_START_BYTE_MODE)
    {
        I2C_GCSTARTSEL_START(pI2C);
        I2C_GCSTART_ENABLE(pI2C);
    }

    /* Configure address mode */
    if (pConfig->addrMode == I2C_10BIT_MODE)
    {
        I2C_10BITADDR_ENABLE(pI2C);
    }
    else
    {
        I2C_7BITADDR_ENABLE(pI2C);
    }

    /* Set I2C peripheral speed mode */
    regtmp = pI2C->CTRL;
    regtmp |= pConfig->speedMode << I2C_CTRL_SPDM_POS;
    pI2C->CTRL = regtmp;

    /* I2C is now ready to use */
    hI2C->state = I2C_STATE_READY;
    hI2C->restartMode = pConfig->restartMode;
    /* Enable I2C peripheral */
    I2C_DEV_ENABLE(hI2C->pDevice);

    error = RET_OK;

exit:
    return error;
}

RET_CODE I2C_MasterTransmit(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_ERROR ;
    i2c_WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#if 1//#ifdef NO_OS
    i2c_WaitTimerInit(&waitTimer, timeout, 1);
#endif
#if 1
    if (pData == NULL || size == 0 || (hI2C->state != I2C_STATE_READY &&  hI2C->state != I2C_STATE_IDLE))
    {
        if (hI2C->state != I2C_STATE_READY)
        {
            hI2C->errorCode = I2C_ERRCODE_NOTREADY;
        }

        goto exit;
    }
#endif
    hI2C->state = I2C_STATE_MASTER_TX;
    hI2C->errorCode = I2C_ERRCODE_NOERROR;

    /* Disable all interrupts */
    I2C_DISABLE_ALLINTS(hI2C->pDevice);
    /* Configure target address */
    I2C_DEV_DISABLE(hI2C->pDevice);
    hI2C->pDevice->TSAR = targetAddress;

    if (hI2C->enableDMA == TRUE)
    {
        /* DMA configuration for I2C transmit */
        error = DMAConfig_for_I2CTrmt(hI2C, I2C_MASTER_MODE, pData, size);
        if (error != RET_OK)
        {
            return error;
        }
    }
    /* Master transmit using Interrupt Mode (none-blocking mode) */
    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        hI2C->pTxBuffer  = pData;
        hI2C->txRemained = size;

        /*
         *Clear possible pending interrupt flags to avoid unexpected IRQs
         *once some interrupt is enabled
         */
        I2C_CLEAR_COMBINEDINT(hI2C->pDevice);

        /* Enable global interrupt of I2Cx */
        //I2C_NVICEnable(hI2C->pDevice);
        if (hI2C->pDevice == I2C0)
        {
            HAL_RequestIrq(I2C0_IRQn, I2C0_IRQ_PRIORITY, HAL_I2C0_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C0_IRQn);
        }
        else if (hI2C->pDevice == I2C1)
        {
            HAL_RequestIrq(I2C1_IRQn, I2C1_IRQ_PRIORITY, HAL_I2C1_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C1_IRQn);
        }
        else if (hI2C->pDevice == I2C2)
        {
            HAL_RequestIrq(I2C2_IRQn, I2C2_IRQ_PRIORITY, HAL_I2C2_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C2_IRQn);
        }

        /* Ensure interrupt to be generated */
        if (hI2C->enableDMA != TRUE)
        {
            //I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_TXL);
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO);
        }
        else
        {
            //I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFO | I2C_IE_TXL);
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_TXL);
        }

        /* Enable I2C, now I2C is working */
        I2C_DEV_ENABLE(hI2C->pDevice);
    }
    else   /* Master transmit using Polling Mode (blocking mode) */
    {
        I2C_DEV_ENABLE(hI2C->pDevice);
        s_dmaComplete = s_dmaError = 0;  /* Initialize these two variables */

        if (hI2C->enableDMA == TRUE)
        {
            error = Wait_tillDMACompletes(hI2C, &waitTimer);
            if (error != RET_OK)
            {
                return error;
            }
        }
        else   /* While DMA is inactive */
        {
            I2C_DEV_ENABLE(hI2C->pDevice);

            for (; size > 0; size--)
            {
                if (size == 1)
                {
                    if (hI2C->restartMode == I2C_DATA_CMD_RESTART)
                    {
                        error = Write_Data2TxFIFO(hI2C, &waitTimer, *pData, I2C_DATA_CMD_RESTART, I2C_DATACMD_WRITE);
                    }
                    else
                    {
                        error = Write_Data2TxFIFO(hI2C, &waitTimer, *pData, I2C_DATA_CMD_STOP, I2C_DATACMD_WRITE);

                    }
                    while ((hI2C->pDevice->ST & I2C_ST_ACT));/*when not active, finish send data*/
                }
                else
                {

                    error = Write_Data2TxFIFO(hI2C, &waitTimer, *pData++, I2C_DATA_CMD_NULL, I2C_DATACMD_WRITE);
                }

                if (error != RET_OK)
                {
                    return error;
                }
            }
            /* Change back to IDLE state */
            hI2C->state = I2C_STATE_IDLE;
        }
    }
    error = RET_OK;

exit:
    return error;
}


RET_CODE I2C_MasterReceive(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout)
{
#if 1
    RET_CODE error = RET_ERROR;
    i2c_WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#if   1//#ifdef   NO_OS
    i2c_WaitTimerInit(&waitTimer, timeout, 1);
#endif
    if (pData == NULL || size == 0 || (hI2C->state != I2C_STATE_READY &&  hI2C->state != I2C_STATE_IDLE))
    {
        if (hI2C->state != I2C_STATE_READY)
        {
            hI2C->errorCode = I2C_ERRCODE_NOTREADY;
        }
        goto exit;
    }

    hI2C->state     = I2C_STATE_MASTER_RX;
    hI2C->errorCode = I2C_ERRCODE_NOERROR;

    /* Disable all interrupts */
    I2C_DISABLE_ALLINTS(hI2C->pDevice);
    /* Configure target address */
    I2C_DEV_DISABLE(hI2C->pDevice);
    hI2C->pDevice->TSAR = targetAddress;

    /*
     *Clear possible pending interrupt flags to avoid unexpected IRQs
     *once some interrupt is enabled
     */
    I2C_CLEAR_COMBINEDINT(hI2C->pDevice);

    /* DMA is used for I2C data transcation */
    if (hI2C->enableDMA == TRUE)
    {
        error = DMAConfig_for_I2CRecv(hI2C, I2C_MASTER_MODE, pData, size);
        if (error != RET_OK)
        {
            return error;
        }
    }
    /* Master transmit using Interrupt Mode (none-blocking mode) */
    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        hI2C->pRxBuffer  = pData;
        hI2C->txRemained = size;
        hI2C->rxRemained = size;

        /* Enable global interrupt of I2Cx */
        //I2C_NVICEnable(hI2C->pDevice);
        if (hI2C->pDevice == I2C0)
        {
            HAL_RequestIrq(I2C0_IRQn, I2C0_IRQ_PRIORITY, HAL_I2C0_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C0_IRQn);
        }
        else if (hI2C->pDevice == I2C1)
        {
            HAL_RequestIrq(I2C1_IRQn, I2C1_IRQ_PRIORITY, HAL_I2C1_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C1_IRQn);
        }
        else if (hI2C->pDevice == I2C2)
        {
            HAL_RequestIrq(I2C2_IRQn, I2C2_IRQ_PRIORITY, HAL_I2C2_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C2_IRQn);
        }
        /* Eusure RXFIFO FULL interrupt is enabled to be generated when RXFIFO_FULL is configured */
        if (size <= hI2C->rxThreshld)
        {
            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(size, hI2C->rxThreshld) - 1);
        }

        /* Ensure interrupt to be generated */
        if (hI2C->enableDMA != TRUE)
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_RFAF | \
                                 I2C_IE_RFO  | I2C_IE_RFU);
        }
        else
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_RFO  | \
                                 I2C_IE_RFU  | I2C_IE_TXL);
        }
        /* Enable I2C, now I2C is working */
        I2C_DEV_ENABLE(hI2C->pDevice);
    }
    /* Master transmit using Polling Mode (blocking mode) */
    else
    {
        hI2C->txRemained = size;
        /* Initialize these two variables */
        s_dmaComplete = s_dmaError = 0;

        I2C_DEV_ENABLE(hI2C->pDevice);
        if (hI2C->enableDMA == TRUE)
        {
            while (hI2C->txRemained)
            {
                if (hI2C->txRemained == 1)
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                }
                else
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
                }

                if (error != RET_OK)
                {
                    return error;
                }

                hI2C->txRemained --;
            }

            error = Wait_tillDMACompletes(hI2C, &waitTimer);
            if (error != RET_OK)
            {
                return error;
            }
        }
        else
        {
            for (; size > 0; size--)
            {
                if (size == 1)
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                }
                else
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
                }

                if (error != RET_OK)
                {
                    return error;
                }

                while (1)
                {
                    /* Wait till data is available in Rx FIFO */
                    if (!(hI2C->pDevice->ST & I2C_ST_RXE))
                    {
                        break;
                    }
                    //i2cwait_till_timeout;
                }

                /* Retrieve valid data */
                *pData++ = (uint8_t)((hI2C->pDevice->DT) & 0xFF);
            }

            hI2C->state = I2C_STATE_IDLE;
        }
    }

    error = RET_OK;


exit:
    return error;

#endif
}


RET_CODE I2C_MasterReceive_restart(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *memAddr, uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_ERROR;
    i2c_WaitTimer_T waitTimer;    /* Initialize timeout structure */
    i2c_WaitTimerInit(&waitTimer, timeout, 1);

    if (pData == NULL || size == 0 || (hI2C->state != I2C_STATE_READY &&  hI2C->state != I2C_STATE_IDLE))
    {
        if (hI2C->state != I2C_STATE_READY)
        {
            hI2C->errorCode = I2C_ERRCODE_NOTREADY;
        }


        goto exit;

    }
    hI2C->state     = I2C_STATE_MASTER_RX_RESTART;
    //hI2C->restartMode_f     = 0;
    hI2C->restartMode     = I2C_DATA_CMD_NULL;
    hI2C->errorCode = I2C_ERRCODE_NOERROR;
    hI2C->memAddr   = *memAddr;
    /* Disable all interrupts */
    I2C_DISABLE_ALLINTS(hI2C->pDevice);
    /* Configure target address */
    I2C_DEV_DISABLE(hI2C->pDevice);
    hI2C->pDevice->TSAR = targetAddress;


    /*
    *Clear possible pending interrupt flags to avoid unexpected IRQs
    *once some interrupt is enabled
    */
    I2C_CLEAR_COMBINEDINT(hI2C->pDevice);
    /* DMA is used for I2C data transcation */
    if (hI2C->enableDMA == TRUE)
    {
        error = DMAConfig_for_I2CRecv(hI2C, I2C_MASTER_MODE, pData, size);
        if (error != RET_OK)
        {
            return error;
        }
    }
    /* Master transmit using Interrupt Mode (none-blocking mode) */
    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        hI2C->pRxBuffer  = pData;
        hI2C->txRemained = size;
        hI2C->rxRemained = size;
        /* Enable global interrupt of I2Cx */
        //I2C_NVICEnable(hI2C->pDevice);
        if (hI2C->pDevice == I2C0)
        {
            HAL_RequestIrq(I2C0_IRQn, I2C0_IRQ_PRIORITY, HAL_I2C0_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C0_IRQn);
        }
        else if (hI2C->pDevice == I2C1)
        {
            HAL_RequestIrq(I2C1_IRQn, I2C1_IRQ_PRIORITY, HAL_I2C1_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C1_IRQn);
        }
        else if (hI2C->pDevice == I2C2)
        {
            HAL_RequestIrq(I2C2_IRQn, I2C2_IRQ_PRIORITY, HAL_I2C2_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C2_IRQn);
        }
        /* Eusure RXFIFO FULL interrupt is enabled to be generated when RXFIFO_FULL is configured */
        if (size <= hI2C->rxThreshld)
        {
            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(size, hI2C->rxThreshld) - 1);
        }

        /* Ensure interrupt to be generated */
        if (hI2C->enableDMA != TRUE)
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_RFAF |
                                 I2C_IE_RFO  | I2C_IE_RFU | I2C_IE_TXL);
        }
        else
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_RFO  |
                                 I2C_IE_RFU  | I2C_IE_TXL);
        }
        I2C_DEV_ENABLE(hI2C->pDevice);
    }

    /* Master transmit using Polling Mode (blocking mode) */
    else
    {
        hI2C->txRemained = size;
        /* Initialize these two variables */
        s_dmaComplete = s_dmaError = 0;
        I2C_DEV_ENABLE(hI2C->pDevice);
        if (hI2C->enableDMA == TRUE)
        {
            while (hI2C->txRemained)
            {
                if (hI2C->txRemained == 1)
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                }
                else
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
                }
                if (error != RET_OK)
                {
                    return error;
                }
                hI2C->txRemained --;
            }
            error = Wait_tillDMACompletes(hI2C, &waitTimer);
            if (error != RET_OK)
            {
                return error;
            }
        }
        else
        {
            Write_Data2TxFIFO(hI2C, &waitTimer, *memAddr, I2C_DATA_CMD_RESTART, I2C_DATACMD_WRITE);  // send write memaddr
            //Write_Data2TxFIFO(hI2C, &waitTimer, 0, I2C_DATA_CMD_RESTART, I2C_DATACMD_READ);// send read memaddr, restart
            for (; size > 0; size--)
            {
                if (size == 1)
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                    while ((hI2C->pDevice->ST & I2C_ST_ACT));/*when not active, finish receive datas*/
                }
                else
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
                }
                if (error != RET_OK)
                {
                    return error;
                }
                while (1)
                {
                    /* Wait till data is available in Rx FIFO */
                    if (!(hI2C->pDevice->ST & I2C_ST_RXE))
                    {
                        break;
                    }
                    i2cwait_till_timeout;
                }

                /* Retrieve valid data */
                *pData++ = (uint8_t)((hI2C->pDevice->DT) & 0xFF);
            }
            hI2C->state = I2C_STATE_IDLE;
        }
    }
    error = RET_OK;
exit:
    return error;
}


RET_CODE I2C_SlaveTransmit(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_ERROR;
    i2c_WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#if 1    //#ifdef NO_OS
    i2c_WaitTimerInit(&waitTimer, timeout, 1);
#endif
    if (pData == NULL || size == 0 || (hI2C->state != I2C_STATE_READY &&  hI2C->state != I2C_STATE_IDLE))
    {
        if (hI2C->state != I2C_STATE_READY)
        {
            hI2C->errorCode = I2C_ERRCODE_NOTREADY;
        }

        goto exit;
    }

    hI2C->state     = I2C_STATE_SLAVE_TX;
    hI2C->errorCode = I2C_ERRCODE_NOERROR;

    /* Disable all interrupts */
    I2C_DISABLE_ALLINTS(hI2C->pDevice);

    /*
     *Clear possible pending interrupt flags to avoid unexpected IRQs
     *once some interrupt is enabled
     */
    I2C_CLEAR_COMBINEDINT(hI2C->pDevice);

    if (hI2C->enableDMA == TRUE)
    {
        error = DMAConfig_for_I2CTrmt(hI2C, I2C_SLAVE_MODE, pData, size);
        if (error != RET_OK)
        {
            return error;
        }
    }

    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        hI2C->pTxBuffer  = pData;
        hI2C->txRemained = size;

        /* Enable global interrupt of I2Cx */
        //I2C_NVICEnable(hI2C->pDevice);
        if (hI2C->pDevice == I2C0)
        {
            HAL_RequestIrq(I2C0_IRQn, I2C0_IRQ_PRIORITY, HAL_I2C0_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C0_IRQn);
        }
        else if (hI2C->pDevice == I2C1)
        {
            HAL_RequestIrq(I2C1_IRQn, I2C1_IRQ_PRIORITY, HAL_I2C1_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C1_IRQn);
        }
        else if (hI2C->pDevice == I2C2)
        {
            HAL_RequestIrq(I2C2_IRQn, I2C2_IRQ_PRIORITY, HAL_I2C2_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C2_IRQn);
        }

        /* Ensure interrupt to be generated */
        I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFO | I2C_IE_SRREQ | I2C_IE_TXL);
    }
    else
    {
        /* Enable I2C, now I2C is working */
        I2C_DEV_ENABLE(hI2C->pDevice);
        /* Initialize these two variables */
        s_dmaComplete = s_dmaError = 0;

        if (hI2C->enableDMA == TRUE)
        {
            error = Wait_tillDMACompletes(hI2C, &waitTimer);
            if (error != RET_OK)
            {
                return error;
            }
        }
        /* While DMA is inactive */
        else
        {
            hI2C->pTxBuffer  = pData;
            hI2C->txRemained = size;

            /*
             * Such that Read-Request-Flag for slave will be pulled high once it's read.
             * However, slave ISR will not involked by CPU due to Global interrupt being
             * not enabled via NVIC_EnableIRQ
             */
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_SRREQ);

            for (; size > 0; size--)
            {
                while (1)
                {
                    if (hI2C->pDevice->IS & I2C_IS_SRREQ)
                    {
                        break;
                    }

                    /* Wait for a while if it is called in blocking function */
#ifdef NO_OS
                    if (i2c_WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
                    {
                        hI2C->errorCode = I2C_ERRCODE_TIMEOUT;
                        return RET_TIMEOUT;
                    }
#endif
                }

                error = Write_Data2TxFIFO(hI2C, &waitTimer, *pData++, I2C_DATA_CMD_NULL, I2C_DATACMD_WRITE);
                if (error != RET_OK)
                {
                    return error;
                }
                I2C_CLEAR_SLV_RD_REQ(hI2C->pDevice);
            }

            hI2C->state = I2C_STATE_IDLE;
        }
    }

    error = RET_OK;

exit:
    return error;
}

/**
  *@brief Slave I2C peripheral receives bytes of data in Polling
  *       or Interrupt Mode depending on Timout parameter.
  *@param Hnd   : A pointer to I2C_Handle_T structure containing Handle
  *               Information of driver for a specified I2C peripheral.
  *@para pData  : A pointer to data buffer to be received.
  *@para Size   : The count of data to be received.
  *@para Timeout: In Millisecond unit. It timeouts when Timeout elapses in ms.
                  Specially, I2C_TIMEOUT_NO_WAIT indicates that Slave
                  receives data in Interrupt Mode. And any other than I2C_TIMEOUT_NO_WAIT
                  means that data-receiving will proceed in Polling mode.
  *@retval      : RET_CODE
  */
RET_CODE I2C_SlaveReceive(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_ERROR;
    i2c_WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#if 1   //#ifdef NO_OS
    i2c_WaitTimerInit(&waitTimer, timeout, 1);
#endif
    ASSERT(hI2C != NULL && pData != NULL && size != 0);

    if (pData == NULL || size == 0 || (hI2C->state != I2C_STATE_READY &&  hI2C->state != I2C_STATE_IDLE))
    {
        if (hI2C->state != I2C_STATE_READY)
        {
            hI2C->errorCode = I2C_ERRCODE_NOTREADY;
        }

        goto exit;
    }

    hI2C->state     = I2C_STATE_SLAVE_RX;
    hI2C->errorCode = I2C_ERRCODE_NOERROR;

    I2C_DISABLE_ALLINTS(hI2C->pDevice);

    /*
     * Clear possible pending interrupt flags to avoid unexpected IRQs
     * once some interrupt is enabled
     */
    I2C_CLEAR_COMBINEDINT(hI2C->pDevice);

    /* DMA is used for I2C data transcation */
    if (hI2C->enableDMA == TRUE)
    {
        error = DMAConfig_for_I2CRecv(hI2C, I2C_SLAVE_MODE, pData, size);
        if (error != RET_OK)
        {
            return error;
        }
    }

    /*Slave receive using Interrupt Mode (none-blocking mode)*/
    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        hI2C->pRxBuffer  = pData;
        hI2C->rxLength   = size;
        hI2C->rxRemained = size;

        /* Enable global interrupt of I2Cx */
        //I2C_NVICEnable(hI2C->pDevice);
        if (hI2C->pDevice == I2C0)
        {
            HAL_RequestIrq(I2C0_IRQn, I2C0_IRQ_PRIORITY, HAL_I2C0_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C0_IRQn);
        }
        else if (hI2C->pDevice == I2C1)
        {
            HAL_RequestIrq(I2C1_IRQn, I2C1_IRQ_PRIORITY, HAL_I2C1_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C1_IRQn);
        }
        else if (hI2C->pDevice == I2C2)
        {
            HAL_RequestIrq(I2C2_IRQn, I2C2_IRQ_PRIORITY, HAL_I2C2_InterruptHandler, 0);
            HAL_IRQ_ENABLE(I2C2_IRQn);
        }

        /* Eusure RXFIFO FULL interrupt to enable to be generated when RXFIFO_FULL is configured */
        if (size <= hI2C->rxThreshld)
        {
            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(size, hI2C->rxThreshld) - 1);
        }

        /* Ensure interrupt to be generated */
        if (hI2C->enableDMA != TRUE)
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_RFAF | I2C_IE_RFO | I2C_IE_RFU  | I2C_IE_TXL);
        }
        else
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_RFO | I2C_IE_RFU | I2C_IE_TXL);
        }

    }
    /* Slave receive using Polling Mode (blocking mode) */
    else
    {
        I2C_DEV_ENABLE(hI2C->pDevice);
        /* Initialize these two variables */
        s_dmaComplete = s_dmaError = 0;

        if (hI2C->enableDMA == TRUE)
        {
            error = Wait_tillDMACompletes(hI2C, &waitTimer);
            if (error != RET_OK)
            {
                return error;
            }
        }
        /* While DMA is inactive */
        else
        {
            hI2C->pTxBuffer  = pData;
            hI2C->rxRemained = size;

            for (; size > 0; size--)
            {
                while (1)
                {
                    if (!(hI2C->pDevice->ST & I2C_ST_RXE))
                    {
                        break;
                    }
                    i2cwait_till_timeout;
                }

                *pData++ = (uint8_t)((hI2C->pDevice->DT) & 0xFF);
            }
        }

        hI2C->state = I2C_STATE_IDLE;
    }

    error = RET_OK;

exit:
    return error;
}

void I2C_CommonIRQHandler(I2C_Handle_T *hI2C)
{
    __IO uint32_t intStatus = hI2C->pDevice->IS;
    __IO uint32_t errStatus = hI2C->pDevice->ERR;
    __IO uint8_t curRxFIFO;

    uint32_t cbPara = 0;
    uint8_t i;

    I2C_Event_T event  = I2C_EVENT_TRANSFER_DONE;
    /* Check Error status */
    if (errStatus & 0xFFFF)
    {
        event = I2C_EVNET_TRANSFER_ERROR;
        hI2C->errorCode = I2C_ERRCODE_TRANSACERR;

        if (hI2C->callback != NULL)
        {
            /* Notify user applcation code */
            hI2C->callback(hI2C, event, cbPara);
        }

        return ;
    }

    switch ((I2C_State_T)(hI2C->state))
    {
        case I2C_STATE_MASTER_TX:

            if (intStatus & I2C_IS_TFAE)
            {
                /* When DMA is active, nothing to be done in ISR */
                if (hI2C->enableDMA != TRUE)
                {
                    if (hI2C->txRemained == 1)
                    {
                        if (hI2C->restartMode == I2C_DATA_CMD_RESTART)
                        {
                            I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_RESTART, *hI2C->pTxBuffer, I2C_DATACMD_WRITE);
                            I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, *hI2C->pTxBuffer, I2C_DATACMD_READ);
                        }
                        else
                        {
                            I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, *hI2C->pTxBuffer, I2C_DATACMD_WRITE);
                        }
                        //while ((hI2C->pDevice->ST & I2C_ST_ACT));/*when not active, finish send data*/
                        /* Issue a stop bit at the end of the data stream */
                        //I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, *hI2C->pTxBuffer, I2C_DATACMD_WRITE);
                    }
                    else
                    {
                        /* Send data to I2C Tx buffer */
                        I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_NULL, *hI2C->pTxBuffer++, I2C_DATACMD_WRITE);
                    }

                    hI2C->txRemained --;

                    /* Clear Tx FIFO almost Empty. Hardware clear it automatically, without involvement of software */
                    if (hI2C->txRemained == 0)
                    {
                        /* Change I2C state to intial state */
                        hI2C->state     = I2C_STATE_IDLE;
                        hI2C->errorCode = I2C_ERRCODE_NOERROR;
                        /* Disable some interrupts */
                        I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_TXL);

                        if (hI2C->callback != NULL && hI2C->enableDMA != TRUE)
                        {
                            hI2C->callback(hI2C, event, cbPara);
                        }
                    }
                }
            }
            break;

        case I2C_STATE_MASTER_RX:

            /* Issue read command to target I2C device */
            if (intStatus & I2C_IS_TFAE)
            {
                if (hI2C->txRemained == 1)
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, 0x00, I2C_DATACMD_READ);
                }
                else
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_NULL, 0x00, I2C_DATACMD_READ);
                }

                hI2C->txRemained --;
                if (hI2C->txRemained == 0)
                {
                    /* Disable Tx FIFO corresponding bitfild */
                    I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO);
                }
                /* Clear Tx FIFO almost Empty. Hardware clear it automatically, without involvement of software */
            }
            curRxFIFO = I2C_GET_FIFOTHRD_RX(hI2C->pDevice);

            if (intStatus & I2C_IS_RFAF)
            {
                /* When DMA is active, nothing to be done in ISR */
                if (hI2C->enableDMA != TRUE)
                {
                    /* Read the data from FIFO */
                    for (i = 0; i < curRxFIFO; i ++)
                    {
                        *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) & 0xFF;
                        hI2C->rxRemained --;
                    }
                    if (curRxFIFO == 0)
                    {
                        *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) & 0xFF;
                        hI2C->rxRemained --;

                        I2C_SET_FIFOTHRD_RX(hI2C->pDevice, hI2C->rxThreshld);
                    }
                    else
                    {
                        if (hI2C->rxRemained <= hI2C->rxThreshld)
                        {
                            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(hI2C->rxRemained, curRxFIFO) - 1);
                        }
                    }
                }
            }
            /*
             * This never happens when DMA is used for Master-Receiving,
             * because rxRemained is left unchanged
             */
            if (hI2C->txRemained == 0 && hI2C->rxRemained == 0)
            {
                hI2C->state = I2C_STATE_IDLE;
                hI2C->errorCode = I2C_ERRCODE_NOERROR;

                I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | \
                                      I2C_IE_RFAF | I2C_IE_RFO | \
                                      I2C_IE_RFU  | I2C_IE_TXL);
                /* Call usr callback */
                if (hI2C->callback != NULL &&  hI2C->enableDMA != TRUE)
                {
                    hI2C->callback(hI2C, event, cbPara);
                }
            }
            break;

        case I2C_STATE_MASTER_RX_RESTART:

            /* Issue read command to target I2C device */
            if (intStatus & I2C_IS_TFAE)
            {
                //if(hI2C->restartMode_f == 0)
                if (hI2C->restartMode == 0)
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_RESTART, hI2C->memAddr, I2C_DATACMD_WRITE);
                    //hI2C->restartMode_f = 1;
                    hI2C->restartMode = I2C_DATA_CMD_RESTART;
                }

                if (hI2C->txRemained == 1)
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, 0x00, I2C_DATACMD_READ);
                    while ((hI2C->pDevice->ST & I2C_ST_ACT));/*when not active, finish receive datas*/
                }
                else
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_NULL, 0x00, I2C_DATACMD_READ);
                }

                hI2C->txRemained --;
                if (hI2C->txRemained == 0)
                {
                    /* Disable Tx FIFO corresponding bitfild */
                    I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO);
                }

                /* Clear Tx FIFO almost Empty. Hardware clear it automatically, without involvement of software */
            }

            curRxFIFO = I2C_GET_FIFOTHRD_RX(hI2C->pDevice);

            if (intStatus & I2C_IS_RFAF)
            {
                /* When DMA is active, nothing to be done in ISR */
                if (hI2C->enableDMA != TRUE)
                {
                    /* Read the data from FIFO */
                    for (i = 0; i < curRxFIFO; i ++)
                    {
                        *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) & 0xFF;
                        hI2C->rxRemained --;
                    }

                    if (curRxFIFO == 0)
                    {
                        *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) & 0xFF;
                        hI2C->rxRemained --;

                        I2C_SET_FIFOTHRD_RX(hI2C->pDevice, hI2C->rxThreshld);
                    }
                    else
                    {
                        if (hI2C->rxRemained <= hI2C->rxThreshld)
                        {
                            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(hI2C->rxRemained, curRxFIFO) - 1);
                        }
                    }
                }
            }

            /*
             * This never happens when DMA is used for Master-Receiving,
             * because rxRemained is left unchanged
             */
            if (hI2C->txRemained == 0 && hI2C->rxRemained == 0)
            {
                hI2C->state = I2C_STATE_IDLE;
                hI2C->errorCode = I2C_ERRCODE_NOERROR;

                I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | \
                                      I2C_IE_RFAF | I2C_IE_RFO | \
                                      I2C_IE_RFU  | I2C_IE_TXL);
                /* Call usr callback */
                if (hI2C->callback != NULL &&  hI2C->enableDMA != TRUE)
                {
                    hI2C->callback(hI2C, event, cbPara);
                }
            }

            break;

        case I2C_STATE_SLAVE_TX:

            if (hI2C->enableDMA != TRUE)
            {
                /* It hanppens when master issue a read request at the very beginning of data-transferring */
                if (intStatus & I2C_IS_SRREQ)
                {
                    /* In case that only one byte of data is request on transferring */
                    if (hI2C->txRemained == 1)
                    {
                        I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, *hI2C->pTxBuffer, I2C_DATACMD_WRITE);
                        hI2C->state = I2C_STATE_IDLE;
                        hI2C->errorCode = I2C_ERRCODE_NOERROR;
                    }
                    else
                    {
                        /* Transfer data to master I2C peripheral */
                        I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_NULL, *hI2C->pTxBuffer++, I2C_DATACMD_WRITE);
                    }

                    hI2C->txRemained --;
                    if (hI2C->txRemained != 0)
                    {
                        /* Enable TX FIFO empty interrupt, if more than one byte is transferred */
                        I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE);
                        hI2C->state = I2C_STATE_SLAVE_TX_AFTER_RDREQ;
                    }

                    /* Clear Read Request from master */
                    I2C_CLEAR_SLV_RD_REQ(hI2C->pDevice);
                }
            }
            break;

        /* This never happens when DMA is used */
        case I2C_STATE_SLAVE_TX_AFTER_RDREQ:

            if (intStatus & I2C_IS_TFAE)
            {
                if (hI2C->txRemained == 1)
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, *hI2C->pTxBuffer, I2C_DATACMD_WRITE);
                }
                else
                {
                    I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_NULL, *hI2C->pTxBuffer++, I2C_DATACMD_WRITE);
                }

                hI2C->txRemained --;
            }

            /* The master issues a NACK before expected bytes of data is transferred */
            if (intStatus & I2C_IS_SRD)
            {
                if (hI2C->txRemained != 0)
                {
                    event = I2C_EVENT_TERMINATED;
                    cbPara = 0;
                    hI2C->errorCode |= I2C_ERRCODE_TERMINATED_UNEXPECTED;
                    I2C_CLEAR_RX_DONE(hI2C->pDevice);
                }
            }

            if (hI2C->txRemained == 0)
            {
                /* Change back to idle state */
                hI2C->state = I2C_STATE_IDLE;
                hI2C->errorCode = I2C_ERRCODE_NOERROR;

                /* Disable associated interrupts */
                I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO |
                                      I2C_IE_SRREQ | I2C_IE_TXL);

                /* Invoke callbcak function */
                if (hI2C->callback != NULL)
                {
                    hI2C->callback(hI2C, event, cbPara);
                }
            }

            break;

        case I2C_STATE_SLAVE_RX:

            curRxFIFO = I2C_GET_FIFOTHRD_RX(hI2C->pDevice);

            if (hI2C->enableDMA != TRUE)
            {
                if (intStatus & I2C_IS_RFAF)
                {
                    /* Read the data from FIFO */
                    for (i = 0; i < curRxFIFO; i ++)
                    {
                        *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) & 0xFF;
                        hI2C->rxRemained --;
                    }

                    if (curRxFIFO == 0)
                    {
                        *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) & 0xFF;
                        hI2C->rxRemained --;

                        I2C_SET_FIFOTHRD_RX(hI2C->pDevice, hI2C->rxThreshld);
                    }
                    else
                    {
                        if (hI2C->rxRemained <= hI2C->rxThreshld)
                        {
                            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(hI2C->rxRemained, curRxFIFO) - 1);
                        }
                    }
                    /* Clear FIFO  Rx almost FULL. Hardware clear it automatically, without involvement of software */
                }

                if (hI2C->rxRemained == 0)
                {
                    hI2C->state = I2C_STATE_IDLE;
                    hI2C->errorCode = I2C_ERRCODE_NOERROR;

                    /* So that no any other interrupt is underway */
                    I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_RFAF | I2C_IE_RFO |
                                          I2C_IE_RFU  | I2C_IE_TXL);

                    if (hI2C->callback != NULL)
                    {
                        hI2C->callback(hI2C, event, cbPara);
                    }
                }
            }

            break;
    }
    if (intStatus & I2C_IS_TXL)
    {
        if (hI2C->txRemained != 0 || hI2C->rxRemained != 0)
        {
            event = I2C_EVENT_ARB_LOST;
            cbPara = hI2C->pDevice->ERR;
            hI2C->errorCode |= I2C_ERRCODE_ARBITRATION_LOST;

            if (hI2C->callback != NULL)
            {
                hI2C->callback(hI2C, event, cbPara);
            }
        }

        /* Clear ARB LOST */
        I2C_CLEAR_ABRT_LOST(hI2C->pDevice);
    }

    if (intStatus & I2C_IE_TFO)
    {
        event = I2C_EVENT_TX_FIFO_OVERFLOW;
        cbPara = intStatus;

        if (hI2C->callback != NULL)
        {
            hI2C->callback(hI2C, event, cbPara);
        }
    }

    if (intStatus & I2C_IE_RFO)
    {
        event = I2C_EVENT_RX_FIFO_OVERFLOW;
        cbPara = intStatus;

        if (hI2C->callback != NULL)
        {
            hI2C->callback(hI2C, event, cbPara);
        }
    }

    if (intStatus & I2C_IE_RFU)
    {
        event = I2C_EVENT_RX_FIFO_UNDERFLOW;
        cbPara = intStatus;

        if (hI2C->callback != NULL)
        {
            hI2C->callback(hI2C, event, cbPara);
        }
    }
}





