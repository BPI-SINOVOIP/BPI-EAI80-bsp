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

//#ifdef CHIP_GM6721


static DMA_Handle_T s_DMAHandle;


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

    while (IO_BITS_GET(hHash->Instance->H_INTST, HASH_INTST_CALINT_SET) != HASH_INTST_CALINT_SET)
    {
        HAL_DelayMs(1);
        if (--cnt == 0)
        {
            hHash->state = HASH_STATE_TIMEOUT;
            return HAL_TIMEOUT;
        }
    }

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
    uint32_t digest = (uint32_t)pMsgDigest;
    uint32_t algo = hHash->init->algo;

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
    dmaConfig.burst = 0;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PRIMARY;
    dmaConfig.callback = HASH_DMACallback;
    HAL_DMA_Init(DMA_DEV, &dmaConfig, &s_DMAHandle);

    LINK_DMAHDL_TO_PARENTHDL(hHash, hdmain, s_DMAHandle);

    hHash->hdmain = &s_DMAHandle;

    return HAL_OK;
}


/**
  * @brief  DMA HASH Input Data transfer complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void HASH_DMAXferCplt(DMA_Handle_T *hDma)
{
    HASH_Handle_T *hHash = (HASH_Handle_T *)(hDma->pParent);
    uint32_t inputAddr = 0;
    uint32_t tranSize = 0;
    uint32_t keySize = hHash->init->keySize;

    if (IO_BITS_GET(hHash->Instance->H_CFG, HASH_CFG_MODE_HMAC) != HASH_CFG_MODE_HMAC)
    {
        /* HASH mode data transfer complete */
        IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_DMA_EN);
        hHash->state = HASH_PHASE_READY;
        HAL_UNLOCK(hHash);
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

            /* Configure the size of message need to hash */
            hHash->Instance->H_MSGLEN[0] = tranSize;

            hHash->hdmain->callback = HASH_DMACallback;
            HAL_DMA_Start(hHash->hdmain, inputAddr, hHash->Instance->H_DIN, tranSize);

            /* Enable DMA requests */
            IO_BIT_SET(hHash->Instance->H_CR, HASH_CR_DMA_EN);
        }
        else
        {
            IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_DMA_EN);
            hHash->hashITCount = 0;
            hHash->state = HASH_PHASE_READY;
            HAL_UNLOCK(hHash);
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
    HASH_Handle_T *hHash = (HASH_Handle_T *)(hDma->pParent);

    hHash->state = HAL_ERROR;
    HAL_UNLOCK(hHash);
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
  * @brief  enable multi DMA transfer or not
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  flag: TRUE to enable, FALSE to disable.
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_SetMulDMATrans(HASH_Handle_T *hHash, uint8_t flag)
{
    if (flag != 0)
    {
        IO_BITMASK_SET(hHash->Instance->H_CR, HASH_CR_MULDMA_Msk, HASH_CR_MULDMA_TRAN);
    }
    else
    {
        IO_BIT_CLEAR(hHash->Instance->H_CR, HASH_CR_MULDMA_TRAN);
    }

    return HAL_OK;
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
    if (hHash == NULL)
    {
        return HAL_ERROR;
    }

    if (hHash->state == HASH_STATE_RESET)
    {
        hHash->lock = HAL_UNLOCKED;
        /* set the HASH and DMA clock */


    }

    hHash->state = HASH_STATE_BUSY;
    hHash->hashITCount = 0;
    hHash->hashBuffSize = 0;
    IO_BITMASK_SET(hHash->Instance->H_CFG, HASH_CFG_DATATYPE_Msk, hHash->init->dataType);
    HAL_HASH_SetMulDMATrans(hHash, config->mulDmaTrans);
    hHash->init = config;
    hHash->callback = config->callback;
    hHash->state = HASH_STATE_READY;
    hHash->phase = HASH_PHASE_READY;

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
        return HAL_ERROR;
    }

    hHash->state = HASH_STATE_BUSY;
    hHash->hashBuffSize = 0;
    hHash->hashITCount = 0;
    hHash->phase = HASH_PHASE_READY;
    hHash->state = HASH_STATE_RESET;
    HAL_UNLOCK(hHash);

    return HAL_OK;
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
HAL_Status_T HAL_HASH_Start_DMA(HASH_Handle_T *hHash, uint8_t *pInBuffer, uint32_t size)
{
    uint32_t inputAddr  = 0;
    uint32_t tranSize = 0;
    uint32_t keySize = hHash->init->keySize;

    if (hHash->state == HASH_STATE_READY)
    {
        HAL_LOCK(hHash);
        hHash->state = HASH_STATE_BUSY;

        if (hHash->init->mode == HASH_ALGOMODE_HMAC)
        {
            /* Save buffer pointer and size in handle */
            hHash->pHashInBuffPtr = pInBuffer;
            hHash->hashBuffSize = size;
            hHash->hashITCount = 0;
        }

        if (hHash->phase == HASH_PHASE_READY)
        {
            HASH_DMAInit(hHash);

            /* set the algo and init the HASH */
            IO_BITMASK_SET(hHash->Instance->H_CFG, HASH_CFG_ALGO_Msk, hHash->init->algo);
            IO_BITMASK_SET(hHash->Instance->H_CFG, HASH_CFG_MODE_Msk, hHash->init->mode);
            IO_BIT_SET(hHash->Instance->H_CR, HASH_CR_INIT_HASH);
            hHash->phase = HASH_PHASE_PROCESS;
        }

        /* Configure the size of message need to hash */
        hHash->Instance->H_MSGLEN[0] = size;

        if (hHash->init->mode == HASH_ALGOMODE_HASH)
        {
            inputAddr = (uint32_t)pInBuffer;
            tranSize = (size % 4) ? ((size + 3) / 4) : (size / 4);
        }
        else if (hHash->init->mode == HASH_ALGOMODE_HMAC)
        {
            inputAddr = (uint32_t)(hHash->init->pKey);
            tranSize = (uint32_t)((keySize % 4) ? ((keySize + 3) / 4) : (keySize / 4));
        }
        hHash->hdmain->callback = HASH_DMACallback;
        HAL_DMA_Start(hHash->hdmain, inputAddr, hHash->Instance->H_DIN, tranSize);

        /* Enable DMA requests */
        IO_BIT_SET(hHash->Instance->H_CR, HASH_CR_DMA_EN);
        //HAL_UNLOCK(hHash);
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

    HAL_LOCK(hHash);
    hHash->state = HASH_STATE_BUSY;
    ret = HASH_WaitDigestComleteTimeout(hHash, timeout);
    if (ret == HAL_TIMEOUT)
    {
        PR_DEBUG("HASH: wait calculate digest timeout.\n");
        return ret;
    }

    HASH_GetDigest(hHash, pOutBuffer);

    hHash->state = HASH_STATE_READY;
    HAL_UNLOCK(hHash);

    return ret;
}


//#endif
