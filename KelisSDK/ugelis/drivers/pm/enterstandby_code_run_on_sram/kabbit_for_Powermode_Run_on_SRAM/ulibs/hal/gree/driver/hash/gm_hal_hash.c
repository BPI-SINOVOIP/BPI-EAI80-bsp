/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file            crypto.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 *
 * @date           2018/03/13
 *
 * @brief           Implementation of Hash Driver.
 *
 * @note
 *    2018/03/13, terry.wu, V1.0.0
 *        Initial version.
 *
 */


#include <gm_hal_hash.h>
#include <gm_hal_dma.h>
#ifdef  RABBIT_NO_OS_SYSTEM
    #include <soc_gm6721x.h>
#endif




static DMA_Handle_T s_DMAHandle;
static HASH_Handle_T *tmpHandle;


static void HASH_DMACallback(struct DMA_Handle *hDma, uint8_t event, unsigned long param);


/**
  * @brief  Wait HASH calculate complete
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T HASH_WaitDigestComleteTimeout(HASH_Handle_T *hHash, uint32_t timeout)
{
    uint32_t cnt = timeout;

    if (hHash == NULL)
    {
        PR_ERR("HASH parameter invalid.\n");
        return HAL_ERROR;
    }

    //while (IO_BITS_GET(hHash->Instance->H_INTST, HASH_INTST_CALINT_SET) != HASH_INTST_CALINT_SET)
    while (IO_BITS_GET(hHash->Instance->H_SR, HASH_SR_BUSY))
    {
        //      HAL_DelayMs(1);
        //      if (--cnt == 0)
        //      {
        //          hHash->state = HASH_STATE_TIMEOUT;
        //          return HAL_TIMEOUT;
        //      }
    }
    //HAL_HASH_CLEAR_ITSTATUS(hHash, HASH_INTST_CALINT_SET);

    return HAL_OK;
}


/**
  * @brief  Provides the message digest result.
  * @param  hHash: pointer to a HASH_Handle_T structure
  * @param  pMsgDigest: Pointer to the message digest
  * @retval Hal status
  */
static HAL_Status_T HASH_GetDigest(HASH_Handle_T *hHash, uint8_t *pMsgDigest)
{
    uint32_t i = 0;
    HAL_Status_T ret = HAL_OK;
    uint32_t digest = 0;
    uint32_t algo = 0;

    if ((hHash == NULL) || (pMsgDigest == NULL))
    {
        PR_ERR("HASH parameter invalid.\n");
        return HAL_ERROR;
    }

    digest = (uint32_t)pMsgDigest;
    algo = hHash->init->algo;
    switch (algo)
    {
        case HASH_ALGO_SHA1:
            for (i = 0; i < 5; i++)
            {
                *((uint32_t *)digest) = (hHash->Instance->H_DIGEST[i]);
                digest += 4;
            }
            break;
        case HASH_ALGO_SHA224:
            for (i = 0; i < 7; i++)
            {
                *((uint32_t *)digest) = (hHash->Instance->H_DIGEST[i]);
                digest += 4;
            }
            break;
        case HASH_ALGO_SHA256:
            for (i = 0; i < 8; i++)
            {
                *((uint32_t *)digest) = (hHash->Instance->H_DIGEST[i]);
                digest += 4;
            }
            break;
        case HASH_ALGO_SHA384:
            for (i = 0; i < 12; i++)
            {
                *((uint32_t *)digest) = (hHash->Instance->H_DIGEST[i]);
                digest += 4;
            }
            break;
        case HASH_ALGO_SHA512:
            for (i = 0; i < 16; i++)
            {
                *((uint32_t *)digest) = (hHash->Instance->H_DIGEST[i]);
                digest += 4;
            }
            break;
        default:
            ret = HAL_ERROR;
            break;
    }

    return ret;
}


/**
  * @brief  Initialize DMA for HASH peripheral.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @retval HAL status
  */
static HAL_Status_T HASH_DMAInit(HASH_Handle_T *hHash)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;

    if (hHash == NULL)
    {
        PR_ERR("HASH parameter invalid.\n");
        return HAL_ERROR;
    }

    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_HASH_TX, &reqResult);
    if (ret != RET_OK)
    {
        return HAL_ERROR;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
    dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
    dmaConfig.width = DMA_WIDTH_32BIT;
    dmaConfig.burst = DMA_BURST_1;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PRIMARY;
    dmaConfig.callback = HASH_DMACallback;
    HAL_DMA_Init(DMA_DEV, &dmaConfig, &s_DMAHandle);

    LINK_DMAHDL_TO_PARENTHDL(hHash, hdmain, s_DMAHandle);

    return HAL_OK;
}


/**
  * @brief  DMA HASH Input Data transfer complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void HASH_DMAXferCplt(DMA_Handle_T *hDma)
{
    HASH_Handle_T *hHash = NULL;
    uint32_t inputAddr = 0;
    uint32_t tranSize = 0;
    uint32_t keySize = 0;

    if (hDma == NULL)
    {
        PR_ERR("HASH parameter invalid.\n");
    }

    hHash = (HASH_Handle_T *)(hDma->pParent);
    keySize = hHash->init->keySize;

    if (IO_BITS_GET(hHash->Instance->H_CFG, HASH_CFG_MODE_HMAC) != HASH_CFG_MODE_HMAC)
    {
        /* HASH mode data transfer complete */
        IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_DMA_EN);
        HAL_DMA_Abort(hDma);
        hHash->state = HASH_STATE_READY;
        hHash->phase = HASH_PHASE_READY;
        if (hHash->callback)
        {
            hHash->callback(hHash, HASH_EVENT_TRANS_DONE, 0);
        }
    }
    else
    {
        /* HMAC mode data transfer process */
        IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_DMA_EN);
        hHash->hashITCount++;
        if (hHash->hashITCount <= 2)
        {
            if (hHash->hashITCount == 1)
            {
                inputAddr = (uint32_t)(hHash->pHashInBuffPtr);
                tranSize = (uint32_t)(hHash->hashBuffSize % 4) ? ((hHash->hashBuffSize + 3) / 4) : (hHash->hashBuffSize / 4);
            }
            else
            {
                inputAddr = (uint32_t)(hHash->init->pKey);
                tranSize = (uint32_t)((keySize % 4) ? ((keySize + 3) / 4) : (keySize / 4));
            }

            hHash->hdmain->callback = HASH_DMACallback;
            /* Enable DMA requests */
            IO_BIT_SET(hHash->Instance->H_CR, HASH_CR_DMA_EN);
            HAL_DMA_Start(hHash->hdmain, (uint32_t)inputAddr, (uint32_t) & (hHash->Instance->H_DIN), tranSize);
        }
        else
        {
            IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_DMA_EN);
            hHash->hashITCount = 0;
            HAL_DMA_Abort(hDma);
            hHash->state = HASH_STATE_READY;
            hHash->phase = HASH_PHASE_READY;
            if (hHash->callback)
            {
                hHash->callback(hHash, HASH_EVENT_TRANS_DONE, 0);
            }
        }
    }

}


/**
  * @brief  DMA HASH communication error callback.
  * @param  hDma: DMA handle
  * @retval None
  */
static void HASH_DMAError(DMA_Handle_T *hDma)
{
    HASH_Handle_T *hHash = NULL;

    if (hDma == NULL)
    {
        PR_ERR("HASH parameter invalid.\n");
    }

    hHash = (HASH_Handle_T *)(hDma->pParent);
    hHash->state = HASH_STATE_ERROR;
    HAL_UnLock(&(hHash->lock));
    if (hHash->callback)
    {
        hHash->callback(hHash, HASH_EVENT_TRANS_ERR, 0);
    }
}


/**
  * @brief  DMA HASH callback.
  * @param  hDma: point to the DMA handle.
  * @param  event: specify the DMA event.
  * @param  param: the parameter of the specify DMA event.
  * @retval None
  */
static void HASH_DMACallback(struct DMA_Handle *hDma, uint8_t event, unsigned long param)
{
    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        HASH_DMAXferCplt(hDma);
    }
    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        HASH_DMAError(hDma);
    }
}


/**
  * @brief  Initializes the HASH according to the specified parameters in the
            HASH_Config_T and creates the associated handle.
  * @param  hHash: pointer to a HASH_Handle_T structure
  * @param config: contains the configuration information for HASH module
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_Init(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    if ((hHash == NULL) || (config == NULL))
    {
        PR_ERR("HASH parameter invalid,Error!!!!\n");
        return HAL_ERROR;
    }

    if (hHash->state == HASH_STATE_RESET)
    {
        hHash->lock = HAL_UNLOCK;
        hHash->Instance = HASH;
        /* set the HASH clock */
        *((volatile uint32_t *)0x40000080) |= (1 << 5);
        *((volatile uint32_t *)0x40000070) |= (1 << 5);

#if 1
        /* open debug mode, take care */
        IO_BITMASK_SET(hHash->Instance->H_SK_CS, HASH_SKCS_DBGKEYSEL_Msk, HASH_SKCS_DBGKEYSEL_HMAC);
#endif
    }

    hHash->state = HASH_STATE_BUSY;
    hHash->hashITCount = 0;
    hHash->hashBuffSize = 0;
    hHash->init = config;
    hHash->callback = config->callback;
    hHash->state = HASH_STATE_READY;
    hHash->phase = HASH_PHASE_READY;

    //test temp
    tmpHandle = hHash;

    return HAL_OK;
}


/**
  * @brief  DeInitializes the HASH peripheral.
  * @note   This API must be called before starting a new processing.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_DeInit(HASH_Handle_T *hHash)
{
    if (hHash == NULL)
    {
        PR_ERR("HASH parameter invalid.\n");
        return HAL_ERROR;
    }

    hHash->state = HASH_STATE_RESET;
    hHash->hashBuffSize = 0;
    hHash->hashITCount = 0;
    hHash->phase = HASH_PHASE_READY;
    hHash->state = HASH_STATE_RESET;
    HAL_UnLock(&(hHash->lock));

    return HAL_OK;
}

/**
  * @brief  process HASH interrupt
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *                the configuration information for HASH.
  *                Not support HMAC now.
  * @none
  */
void HAL_HASH_IRQHandler_Func(void **handle)
{
    HASH_Handle_T *hHash = (HASH_Handle_T *)(*handle);
    uint32_t inputAddr = 0;
    uint32_t i = 0;
    uint32_t tranSize = 0;
    uint32_t tranMax = 0;

    if (HAL_HASH_GET_ITSTATUS(hHash, HASH_INTST_CALINT_SET))
    {
        HAL_HASH_CLEAR_ITSTATUS(hHash, HASH_INTST_CALINT_SET);
        HAL_HASH_DISABLE_IT(hHash, HASH_IMR_CALINT_EN);
        hHash->state = HASH_STATE_READY;
        hHash->phase = HASH_PHASE_READY;
        hHash->callback(hHash, HASH_EVENT_DIGEST_AVAILABEL, 0);

        return;
    }

    if (HAL_HASH_GET_ITSTATUS(hHash, HASH_INTST_DININT_SET))
    {
        HAL_HASH_CLEAR_ITSTATUS(hHash, HASH_INTST_DININT_SET);
        inputAddr = (uint32_t)(hHash->pHashInBuffPtr);
        tranSize = (hHash->hashBuffSize % 4) ? ((hHash->hashBuffSize + 3) / 4) : (hHash->hashBuffSize / 4);
        if ((hHash->init->algo == HASH_ALGO_SHA384) || (hHash->init->algo == HASH_ALGO_SHA512))
        {
            tranMax = 32;
        }
        else
        {
            tranMax = 16;
        }
        if (tranSize >= tranMax)
        {



            for (i = 0; i < tranMax; i++)
            {
                hHash->Instance->H_DIN = *((uint32_t *)inputAddr);
                inputAddr += 4;
            }
            hHash->pHashInBuffPtr += (tranMax * 4);
            hHash->hashBuffSize -= (tranMax * 4);
        }
        else
        {
            for (i = 0; i < tranSize; i++)
            {
                hHash->Instance->H_DIN = *((uint32_t *)inputAddr);
                inputAddr += 4;
            }
            hHash->pHashInBuffPtr += (tranSize * 4);
            hHash->hashBuffSize = 0;

            HAL_HASH_DISABLE_IT(hHash, HASH_IMR_DININT_EN);
            hHash->callback(hHash, HASH_EVENT_TRANS_DONE, 0);
        }

        return;
    }
}

void HAL_HASH_IRQHandler(void)
{
    HAL_HASH_IRQHandler_Func((void **)&tmpHandle);
}

/**
  * @brief  Initializes the HASH peripheral then enables DMA to
            control data transfer. Use HAL_HASH_Final() to get the digest.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64/128 bytes, the padding is managed by hardware.
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_Start(HASH_Handle_T *hHash, uint8_t *pInBuffer, uint32_t size)
{
    uint32_t inputAddr  = 0;
    uint32_t tranSize = 0;
    uint32_t keySize = 0;
    uint32_t i = 0;

    if ((hHash == NULL) || (pInBuffer == NULL))
    {
        PR_ERR("HASH parameter invalid.\n");
        return HAL_ERROR;
    }

    keySize = hHash->init->keySize;
    if (hHash->state == HASH_STATE_READY)
    {
        if (HAL_TryLock(&(hHash->lock)) == RET_BUSY)
        {
            return HAL_BUSY;
        }
        hHash->state = HASH_STATE_BUSY;

        /* Save buffer pointer and size in handle */
        hHash->pHashInBuffPtr = pInBuffer;
        hHash->hashBuffSize = size;
        if (hHash->init->mode == HASH_ALGOMODE_HMAC)
        {
            hHash->hashITCount = 0;
        }

        if (hHash->phase == HASH_PHASE_READY)
        {
            if (hHash->init->enableDMA == TRUE)
            {
                HASH_DMAInit(hHash);
            }

            /* set the algo and init the HASH */
            IO_BITMASK_SET(hHash->Instance->H_CFG, HASH_CFG_ALGO_Msk, hHash->init->algo);
            IO_BITMASK_SET(hHash->Instance->H_CFG, HASH_CFG_MODE_Msk, hHash->init->mode);
            IO_BITMASK_SET(hHash->Instance->H_CFG, HASH_CFG_DATATYPE_Msk, hHash->init->dataType);
            IO_BITMASK_SET(hHash->Instance->H_SK_CFG, HASH_SKCFG_HMACKEY_FROM_Msk, hHash->init->keySel);
            IO_BIT_SET(hHash->Instance->H_CR, HASH_CR_INIT_HASH);
            IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_INIT_HASH);
            hHash->phase = HASH_PHASE_PROCESS;
        }

        /* Configure the size of message need to hash */
        hHash->Instance->H_MSGLEN[0] = ((size * 8) & 0xFFFFFFFF);
        hHash->Instance->H_MSGLEN[1] = ((uint64_t)(size * 8) >> 32);

        if (hHash->init->mode == HASH_ALGOMODE_HASH)
        {
            inputAddr = (uint32_t)pInBuffer;
            tranSize = (size % 4) ? ((size + 3) / 4) : (size / 4);
        }
        else if (hHash->init->mode == HASH_ALGOMODE_HMAC)
        {
            if (hHash->init->keySel == HASH_KEYFROM_DIN)
            {
                /* Configure the size of message need to hash */
                hHash->Instance->H_KEYLEN = (keySize * 8);

                inputAddr = (uint32_t)(hHash->init->pKey);
                tranSize = (uint32_t)((keySize % 4) ? ((keySize + 3) / 4) : (keySize / 4));
            }
            else
            {
                /* Configure the size of message need to hash */
                hHash->Instance->H_KEYLEN = (keySize * 8);
                IO_BITMASK_SET(hHash->Instance->H_SK_CFG, HASH_SKCFG_HMACKEY_SEL_Msk, hHash->init->otpPos);
                IO_BIT_SET(hHash->Instance->H_SK_CS, HASH_SKCS_HMAC_GETKEY_Msk);

                inputAddr = (uint32_t)pInBuffer;
                tranSize = (size % 4) ? ((size + 3) / 4) : (size / 4);

                hHash->hashITCount = 2;

            }
        }

        if (hHash->init->enableDMA == TRUE)
        {
            hHash->hdmain->callback = HASH_DMACallback;
            /* Enable DMA requests */
            IO_BIT_SET(hHash->Instance->H_CR, HASH_CR_DMA_EN);
            HAL_DMA_Start(hHash->hdmain, (uint32_t)inputAddr, (uint32_t) & (hHash->Instance->H_DIN), tranSize);
        }
        else
        {
            if (hHash->init->mode == HASH_ALGOMODE_HASH)
            {
                /* currently for HASH only */
                /* enable HASH interrupt */
                HAL_HASH_ENABLE_IT(hHash, HASH_IMR_DININT_EN | HASH_IMR_CALINT_EN);
                //NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
                //NVIC_EnableIRQ(CRYP_IRQn);
                HAL_RequestIrq(HASH_IRQn, HASH_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_HASH_IRQHandler, &tmpHandle/*(const void *)hCryp*/);
                HAL_IRQ_ENABLE(HASH_IRQn);
            }
            else
            {
                /* for test HMAC only */
                for (i = 0; i < tranSize; i++)
                {
                    hHash->Instance->H_DIN = *((uint32_t *)inputAddr);
                    inputAddr += 4;
                }
                while (!IO_BIT_GET(hHash->Instance->H_SR, HASH_SR_INNERKEY_FIN))
                {
                    ;
                }

                inputAddr = (uint32_t)(hHash->pHashInBuffPtr);
                tranSize = (hHash->hashBuffSize % 4) ? ((hHash->hashBuffSize + 3) / 4) : (hHash->hashBuffSize / 4);
                for (i = 0; i < tranSize; i++)
                {
                    while (IO_BIT_GET(hHash->Instance->H_SR, HASH_SR_BUSY))
                    {
                        ;
                    }
                    hHash->Instance->H_DIN = *((uint32_t *)inputAddr);
                    inputAddr += 4;
                }
                while (IO_BIT_GET(hHash->Instance->H_SR, HASH_SR_BUSY))
                {
                    ;
                }

                inputAddr = (uint32_t)(hHash->init->pKey);
                tranSize = (uint32_t)((keySize % 4) ? ((keySize + 3) / 4) : (keySize / 4));
                for (i = 0; i < tranSize; i++)
                {
                    hHash->Instance->H_DIN = *((uint32_t *)inputAddr);
                    inputAddr += 4;
                }
                while (!IO_BIT_GET(hHash->Instance->H_SR, HASH_SR_OUTERMSG_FIN))
                {
                    ;
                }
            }
        }
        HAL_UnLock(&(hHash->lock));
    }

    return HAL_OK;
}


/**
  * @brief  Returns the computed digest.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  pOutBuffer: Pointer to the computed digest.
  * @param  timeout: Timeout value
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_Final(HASH_Handle_T *hHash, uint8_t *pOutBuffer, uint32_t timeout)
{
    HAL_Status_T ret = HAL_OK;

    if ((hHash == NULL) || (pOutBuffer == NULL))
    {
        PR_ERR("HASH parameter invalid.\n");
        return HAL_ERROR;
    }

    if (HAL_TryLock(&(hHash->lock)) == RET_BUSY)
    {
        return HAL_BUSY;
    }

    hHash->state = HASH_STATE_BUSY;
    ret = HASH_WaitDigestComleteTimeout(hHash, timeout);
    if (ret == HAL_TIMEOUT)
    {
        PR_DEBUG("HASH: wait calculate digest timeout.\n");
        return ret;
    }

    HASH_GetDigest(hHash, pOutBuffer);

    hHash->state = HASH_STATE_READY;
    HAL_UnLock(&(hHash->lock));

    return ret;
}


//#endif
