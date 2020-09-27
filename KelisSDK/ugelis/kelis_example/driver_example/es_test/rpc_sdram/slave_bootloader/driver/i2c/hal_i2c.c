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

#include <hal_i2c.h>
#include <hal_adc.h>

#if 0//defined(CHIP_GM6621) || defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B)
    #include <drv/dev/i2c.h>
    #include <drv/dev/clock.h>
#endif

#if defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B)

    /*I2C clock is 8MHz*/
    #define I2C_CLOCKFREQUENCY_8M     8000000
    /*I2C clock is 16MHz*/
    #define I2C_CLOCKFREQUENCY_16M   16000000

#endif

/**
 * I2C peripherals list
 */
typedef enum I2CDevn
{
    I2CDEV_0 = 0,
    I2CDEV_1 = 1,
    I2CDEV_2 = 2
} I2C_Devn_T;


/*Declarition of i2c interrupt service routine*/
#ifdef I2C0
    static void I2C0_IRQHandler(void);
#endif

#ifdef I2C1
    static void I2C1_IRQHandler(void);
#endif

#ifdef I2C2
    static void I2C2_IRQHandler(void);
#endif

#ifdef I2C0
    //STATIC_ISR(I2C0_IRQn, I2C0_IRQHandler);
#endif

#ifdef I2C1
    //STATIC_ISR(I2C1_IRQn, I2C1_IRQHandler);
#endif

#ifdef I2C2
    //STATIC_ISR(I2C2_IRQn, I2C2_IRQHandler);
#endif

/* DMA handle instance */
#if DMA_66XX_OK
    static DMA_Handle_T s_dmaHandle;
#endif

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

#if 0//(!defined(i2cwait_till_timeout))
#define i2cwait_till_timeout \
    if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT) \
    {                                               \
        hI2C->errorCode = I2C_ERRCODE_TIMEOUT;      \
        return RET_TIMEOUT;                         \
    }
#endif

/*
 *DMA module is occupied exclusively, once one DMA channel is occupied by
 *some module (I2C, etc.), any other channel of DMA can be used until such
 *channel is released. That's why here there's only a common callback subroutine
 *serving for each individual callback request
 */
static volatile uint8_t s_dmaComplete = 0;
static volatile uint8_t s_dmaError    = 0;

/**
 * @brief A common DMA callback I2C data-processing.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_CommonCallback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
#if DMA_66XX_OK

    I2C_Handle_T *pI2CHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, struct I2C_Handle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
        DMA_DeInit(hDMA);
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

    if (event == DMA_EVNET_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
        DMA_DeInit(hDMA);
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
#endif
}

/**
 * @brief DMA callback function for master data-transferring completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_MasterTxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    DMA_CommonCallback(hDMA, event, param);
}

/**
 * @brief DMA callback function for master data-receiving completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_MasterRxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    DMA_CommonCallback(hDMA, event, param);
}

/**
 * @brief DMA callback function for slave data-transferring completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_SlaveTxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    DMA_CommonCallback(hDMA, event, param);
}

/**
 * @brief DMA callback function for slave data-receiving completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_SlaveRxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    DMA_CommonCallback(hDMA, event, param);
}

static void I2C_CommonIRQHandler(I2C_Handle_T *hI2C);

static void I2C0_IRQHandler(void)
{
    I2C_CommonIRQHandler(s_i2cHandle[I2CDEV_0]);
}

static void I2C1_IRQHandler(void)
{
    I2C_CommonIRQHandler(s_i2cHandle[I2CDEV_1]);
}

static void I2C2_IRQHandler(void)
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

static __inline void I2C_NVICEnable(I2C_Device_T *pI2C)
{
#ifdef NO_OS
    if (pI2C == I2C0)
    {
        NVIC_ClearPendingIRQ(I2C0_IRQn);
        NVIC_EnableIRQ(I2C0_IRQn);
    }
    else if (pI2C == I2C1)
    {
        NVIC_ClearPendingIRQ(I2C1_IRQn);
        NVIC_EnableIRQ(I2C1_IRQn);
    }
    else if (pI2C == I2C2)
    {
        NVIC_ClearPendingIRQ(I2C2_IRQn);
        NVIC_EnableIRQ(I2C2_IRQn);
    }
#endif
}

/**
 * @brief Configure DMA for I2C peripheral
 *
 * @param pI2C A pointer to I2C device handler.
 * @para dmaWrite Indicate a DMA write/read.
 * @para i2cMode I2C working mode, eihter master or slave mode.
 * @para pDMAHandle A pointer to DMA operation handler.
 */
static RET_CODE I2C_DMAConfig(I2C_Device_T *pI2C, uint8_t dmaWrite, uint8_t i2cMode, DMA_Handle_T *hDMA)
{
#if DMA_66XX_OK
    DMA_Config_T dmaConfig;
    DMA_Config_T *pDMAConfig = &dmaConfig;

    RET_CODE error = RET_ERROR;

    /* DMA configurations for I2C */
    if (dmaWrite)
    {
        pDMAConfig->destHwHSChannel = ((pI2C == I2C0) ? DMA_I2C0_TX_CHANNEL :
                                       ((pI2C == I2C1) ? DMA_I2C1_TX_CHANNEL :
                                        ((pI2C == I2C2) ? DMA_I2C2_TX_CHANNEL : DMA_INVALID_CHANNEL)));
        /* Validity checking */
        if (pDMAConfig->destHwHSChannel == DMA_INVALID_CHANNEL)
        {
            goto exit;
        }

        pDMAConfig->callback = ((i2cMode == I2C_MASTER_MODE) ? DMA_MasterTxComplete_Callback :
                                DMA_SlaveTxComplete_Callback);
    }
    else
    {
        pDMAConfig->srcHwHSChannel = ((pI2C == I2C0) ? DMA_I2C0_RX_CHANNEL :
                                      ((pI2C == I2C1) ? DMA_I2C1_RX_CHANNEL :
                                       ((pI2C == I2C2) ? DMA_I2C2_RX_CHANNEL : DMA_INVALID_CHANNEL)));
        /* Validity checking */
        if (pDMAConfig->srcHwHSChannel == DMA_INVALID_CHANNEL)
        {
            goto exit;
        }

        pDMAConfig->callback = ((i2cMode == I2C_MASTER_MODE) ? DMA_MasterRxComplete_Callback :
                                DMA_SlaveRxComplete_Callback);
    }

    pDMAConfig->direction     = (dmaWrite != 0) ? DMA_MEMORY_TO_PERIPH : DMA_PERIPH_TO_MEMORY;
    pDMAConfig->srcIncrement  = (dmaWrite != 0) ? DMA_ADDR_INCREMENT : DMA_ADDR_CONSTANT;
    pDMAConfig->destIncrement = (dmaWrite != 0) ? DMA_ADDR_CONSTANT : DMA_ADDR_INCREMENT;
    pDMAConfig->srcHandshake  = (dmaWrite != 0) ? DMA_SOFTWARE_HANDSHAKE : DMA_HARDWARE_HANDSHAKE;
    pDMAConfig->destHandshake = (dmaWrite != 0) ? DMA_HARDWARE_HANDSHAKE : DMA_SOFTWARE_HANDSHAKE;

    if (pDMAConfig->direction == DMA_MEMORY_TO_PERIPH)
    {
        pDMAConfig->srcWidth  = DMA_WIDTH_16BIT;
        pDMAConfig->destWidth = DMA_WIDTH_16BIT;
    }
    else
    {
        pDMAConfig->srcWidth  = DMA_WIDTH_8BIT;
        pDMAConfig->destWidth = DMA_WIDTH_8BIT;
    }

    /* Initialize DMA */
    require_action_quiet(DMA_Init(DMA0, pDMAConfig, hDMA) == RET_OK, exit, error = RET_BUSY);

    error = RET_OK;

exit:
    return error;
#endif
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
static RET_CODE I2C_ConfigBaudRate(I2C_Handle_T *hI2C, I2C_Config_T *pConfig)
{
    uint32_t clkFrq;
    uint16_t lpcc, hpcc;
    OSC_Config_T oscConfig;

    if (pConfig->workMode == I2C_MASTER_MODE)
    {
        if (I2C_DEV_ISENABLED(hI2C->pDevice))
        {
            return RET_ERROR;
        }

        /* Get oscilator configuration */
        OSC_GetConfig(&oscConfig);

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

#if defined(CHIP_GM6611)
        /*actually, it equals to FCLK, PLL bypass feature not exist in GM6611*/
        clkFrq = I2C_CLOCKFREQUENCY_16M;
#endif

        /* I2C baudrate calculation formula */
        lpcc = (clkFrq / (pConfig->baudrate * 1000)) / 2 - 3;
        hpcc = (clkFrq / (pConfig->baudrate * 1000)) / 2 + 1;

        I2C_SET_HPLPCC_HIGHCNT(hI2C->pDevice, hpcc, lpcc);
    }

    /* DO NOT CHANGE THE FOLLOWING SEQUENCE !!! */
    hI2C->pDevice->SH = 0x8008000C;
    hI2C->pDevice->SH = 0x0008000C;

    return RET_OK;
}

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
            *((volatile uint16_t *)(&(hI2C->pDevice->DT))) = (uint8_t)(value);
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

#if DMA_66XX_OK
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
    if (!(DMA0->HSCFG & DMA_HSCFG_DH))
    {
        I2C_DMACTRL_TXEN_ENABLE(hI2C->pDevice);
    }

    if (!(DMA0->HSCFG & DMA_HSCFG_SH))
    {
        I2C_DMACTRL_RXEN_ENABLE(hI2C->pDevice);
    }

    /* Start DMA transcation */
    require_action_quiet(DMA_Start(hI2C->hDMA, srcAddress, (uint32_t)pData, size) == RET_OK, \
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

#if DMA_66XX_OK
    uint32_t destAddress = 0x0;
    uint16_t *pBuffer = hI2C->pdmaBuffer;
    uint32_t tmp = 0;
    uint8_t  i = 0;

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
            *pBuffer = *pData | I2C_DT_ASTP;
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

    /*Enable DMA correpoding bitfields of I2C peripheral*/
    if (!(DMA0->HSCFG & DMA_HSCFG_DH))
    {
        I2C_DMACTRL_TXEN_ENABLE(hI2C->pDevice);
    }

    if (!(DMA0->HSCFG & DMA_HSCFG_SH))
    {
        I2C_DMACTRL_RXEN_ENABLE(hI2C->pDevice);
    }

    /* Start DMA transcation */
    require_action_quiet(DMA_Start(hI2C->hDMA, (uint32_t)hI2C->pdmaBuffer, (uint32_t)destAddress, size) == RET_OK, \
                         exit, error = RET_ERROR; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);
exit:
#endif
    return error;
}


static RET_CODE Wait_tillDMACompletes(I2C_Handle_T *hI2C, WaitTimer_T *waitTimer)
{
    RET_CODE error = RET_OK;

#if 0//DMA_66XX_OK


    /* Wait till DMA completes */
    while (1)
    {
        if (s_dmaComplete == 1 || s_dmaError == 1)
        {
            break;
        }

        if (WaitTillTimeout(waitTimer) == RET_TIMEOUT)
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
    require_action_quiet(DMA_DeInit(&s_dmaHandle) == RET_OK, exit, error = RET_ERROR; hI2C->errorCode = I2C_ERRCODE_DMAERROR;);
    /* Change back to IDLE state */
    hI2C->state = I2C_STATE_IDLE;

exit:
#endif
    return error;
}

static RET_CODE Write_Data2TxFIFO(I2C_Handle_T *hI2C, WaitTimer_T *waitTimer,
                                  uint8_t data, I2C_DataCmdType_T dataCmdType,
                                  I2C_RdWrType_T write)
{
    RET_CODE error = RET_OK;
    I2C_DataCmdRegManipulate(hI2C, dataCmdType, data, write);
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

#ifdef NO_OS
        if (WaitTillTimeout(waitTimer) == RET_TIMEOUT)
        {
            hI2C->errorCode = I2C_ERRCODE_TIMEOUT;
            return RET_TIMEOUT;
        }
#endif
    }



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
    RET_CODE error = RET_ERROR;

    ASSERT(pI2C != NULL && pConfig != NULL && hI2C != NULL);

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

    /* Enable I2C peripheral */
    I2C_DEV_ENABLE(hI2C->pDevice);

    error = RET_OK;

exit:
    return error;
}

RET_CODE I2C_MasterTransmit(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_ERROR;
    WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#ifdef NO_OS
    WaitTimerInit(&waitTimer, timeout, 1);
#endif
    if (pData == NULL || size == 0 || (hI2C->state != I2C_STATE_READY &&  hI2C->state != I2C_STATE_IDLE))
    {
        if (hI2C->state != I2C_STATE_READY)
        {
            hI2C->errorCode = I2C_ERRCODE_NOTREADY;
        }

        goto exit;
    }

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
        I2C_NVICEnable(hI2C->pDevice);

        /* Ensure interrupt to be generated */
        if (hI2C->enableDMA != TRUE)
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_TXL);
        }
        else
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFO | I2C_IE_TXL);
        }

        /* Enable I2C, now I2C is working */
        I2C_DEV_ENABLE(hI2C->pDevice);
    }
    /* Master transmit using Polling Mode (blocking mode) */
    else
    {
        I2C_DEV_ENABLE(hI2C->pDevice);
        /* Initialize these two variables */
        s_dmaComplete = s_dmaError = 0;

        if (hI2C->enableDMA == TRUE)
        {
            //error = Wait_tillDMACompletes(hI2C, &waitTimer);
            if (error != RET_OK)
            {
                return error;
            }
        }
        /* While DMA is inactive */
        else
        {
            I2C_DEV_ENABLE(hI2C->pDevice);

            for (; size > 0; size--)
            {
                if (size == 1)
                {
                    error = Write_Data2TxFIFO(hI2C, &waitTimer, *pData, I2C_DATA_CMD_STOP, I2C_DATACMD_WRITE);
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
    RET_CODE error = RET_ERROR;
    WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#ifdef NO_OS
    WaitTimerInit(&waitTimer, timeout, 1);
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
        I2C_NVICEnable(hI2C->pDevice);

        /* Eusure RXFIFO FULL interrupt is enabled to be generated when RXFIFO_FULL is configured */
        if (size <= hI2C->rxThreshld)
        {
            I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(size, hI2C->rxThreshld) - 1);
        }

        /* Ensure interrupt to be generated */
        if (hI2C->enableDMA != TRUE)
        {
            I2C_ENABLE_INTERRUPT(hI2C->pDevice, I2C_IE_TFAE | I2C_IE_TFO | I2C_IE_RFAF | \
                                 I2C_IE_RFO  | I2C_IE_RFU | I2C_IE_TXL);
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
                    //error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                }
                else
                {
                    //error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
                }

                if (error != RET_OK)
                {
                    return error;
                }

                hI2C->txRemained --;
            }

            //error = Wait_tillDMACompletes(hI2C, &waitTimer);
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
                    //error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                    error = Write_Data2TxFIFO(hI2C, 0, 0x00, I2C_DATA_CMD_STOP, I2C_DATACMD_READ);
                }
                else
                {
                    //error = Write_Data2TxFIFO(hI2C, &waitTimer, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
                    error = Write_Data2TxFIFO(hI2C, 0, 0x00, I2C_DATA_CMD_NULL, I2C_DATACMD_READ);
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
}

RET_CODE I2C_SlaveTransmit(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_ERROR;
    WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#ifdef NO_OS
    WaitTimerInit(&waitTimer, timeout, 1);
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
        I2C_NVICEnable(hI2C->pDevice);

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
                    if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
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
    //WaitTimer_T waitTimer;

    /* Initialize timeout structure */
#ifdef NO_OS
    WaitTimerInit(&waitTimer, timeout, 1);
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
        I2C_NVICEnable(hI2C->pDevice);

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
            //error = Wait_tillDMACompletes(hI2C, &waitTimer);
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

                    //i2cwait_till_timeout;
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

static void I2C_CommonIRQHandler(I2C_Handle_T *hI2C)
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
                        /* Issue a stop bit at the end of the data stream */
                        I2C_DataCmdRegManipulate(hI2C, I2C_DATA_CMD_STOP, *hI2C->pTxBuffer, I2C_DATACMD_WRITE);
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

