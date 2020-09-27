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
 * @date           2018/02/26 
 *    
 * @brief           Implementation of crypto engine Driver.
 * 
 * @note
 *    2018/02/26, terry.wu, V1.0.0
 *        Initial version.
 *    
 */



#include <gm_hal_crypto.h>

//#ifdef CHIP_GM6721


#define CRYP_TIMEOUT_VALUE        3U           /* one millisecond as unit */


/**
  * @brief  Wait CRYP stop working 
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T CRYP_WaitCrypStopTimeout(CRYP_Handle_T * hCryp, uint32_t timeout)
{
	uint32_t tickStart = 0;

	tickStart = HAL_GetTick();
	while (IO_BITS_GET(hCryp->Instance->ES_CR, CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
	{
		if ((HAL_GetTick() - tickStart) > timeout)
		{
			hCryp->state = CRYP_STATE_TIMEOUT;
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}


/**
  * @brief  Wait CRYP to idle 
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T CRYP_WaitCrypIdleTimeout(CRYP_Handle_T * hCryp, uint32_t timeout)
{
	uint32_t tickStart = 0;

	tickStart = HAL_GetTick();
	while (IO_BITS_GET(hCryp->Instance->ES_SR, CRYP_SR_BUSY) == CRYP_SR_BUSY)
	{
		if ((HAL_GetTick() - tickStart) > timeout)
		{
			hCryp->state = CRYP_STATE_TIMEOUT;
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}


/**
  * @brief  Wait input FIFO to empty
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T CRYP_WaitInFiFOEmptyTimeout(CRYP_Handle_T * hCryp, uint32_t timeout)
{
	uint32_t tickStart = 0;

	tickStart = HAL_GetTick();
	while (IO_BITS_GET(hCryp->Instance->ES_SR, CRYP_SR_IFEM) != CRYP_SR_IFEM)
	{
		if ((HAL_GetTick() - tickStart) > timeout)
		{
			hCryp->state = CRYP_STATE_TIMEOUT;
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}


/**
  * @brief  Wait output FIFO not empty
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T CRYP_WaitOutFiFONotEmptyTimeout(CRYP_Handle_T * hCryp, uint32_t timeout)
{
	uint32_t tickStart = 0;

	tickStart = HAL_GetTick();
	while (IO_BITS_GET(hCryp->Instance->ES_SR, CRYP_SR_OFNE) != CRYP_SR_OFNE)
	{
		if ((HAL_GetTick() - tickStart) > timeout)
		{
			hCryp->state = CRYP_STATE_TIMEOUT;
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}


/**
  * @brief  Writes the Key in Key registers. 
  * @param  hCryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Key: Pointer to Key buffer
  * @param  KeySize: Size of Key
  * @param  algo: algorithm to used
  * @retval HAL_OK: set success; HAL_ERROR: set fail.
  */
static HAL_Status_T CRYP_SetKey(CRYP_Handle_T * hCryp, uint8_t * key, uint32_t keySize, uint32_t algo)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t keyAddr = (uint32_t)key;

	if (algo == CRYP_ALGO_DES)
	{
		hCryp->Instance->ES_K0LR = *((uint32_t *)keyAddr);
		keyAddr += 4;
		hCryp->Instance->ES_K0RR = *((uint32_t *)keyAddr);
	}
	else if (algo == CRYP_ALGO_TDES)
	{
		hCryp->Instance->ES_K0LR = *((uint32_t *)keyAddr);
		keyAddr += 4;
		hCryp->Instance->ES_K0RR = *((uint32_t *)keyAddr);
		keyAddr += 4;
		hCryp->Instance->ES_K1LR = *((uint32_t *)keyAddr);
		keyAddr += 4;
		hCryp->Instance->ES_K1RR = *((uint32_t *)keyAddr);
		keyAddr += 4;
		hCryp->Instance->ES_K2LR = *((uint32_t *)keyAddr);
		keyAddr += 4;
		hCryp->Instance->ES_K2RR = *((uint32_t *)keyAddr);
	}
	else if (algo == CRYP_ALGO_AES)
	{
		switch (keySize)
		{
			case CRYP_KEYSIZE_128B:
				hCryp->Instance->ES_K0LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K0RR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K1LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K1RR = *((uint32_t *)keyAddr);
				break;
			case CRYP_KEYSIZE_192B:
				hCryp->Instance->ES_K0LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K0RR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K1LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K1RR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K2LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K2RR = *((uint32_t *)keyAddr);
				break;
			case CRYP_KEYSIZE_256B:
				hCryp->Instance->ES_K0LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K0RR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K1LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K1RR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K2LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K2RR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K3LR = *((uint32_t *)keyAddr);
				keyAddr += 4;
				hCryp->Instance->ES_K3RR = *((uint32_t *)keyAddr);
				break;
			default:
				PR_DEBUG("CRYP: don't support this keysize:%d.\n", keySize);
				ret = HAL_ERROR;
				break;
		}
	}
	else
	{
		ret = HAL_ERROR;
		PR_DEBUG("CRYP: don't support algo : 0x%x.\n", algo);
	}

	return ret;
}


/**
  * @brief  Writes the InitVector/InitCounter in IV registers. 
  * @param  hCryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  initVector: Pointer to InitVector/InitCounter buffer
  * @param  ivSize: Size of the InitVector/InitCounter
  * @retval HAL_OK: set success; HAL_ERROR: set fail.
  */
static HAL_Status_T CRYP_SetInitVector(CRYP_Handle_T * hCryp, uint8_t * initVector, uint32_t ivSize)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t initAddr = (uint32_t)initVector;
	uint32_t algoMode = hCryp->init->algoMode;

	if (algoMode == CRYP_ALGOMODE_ECB)
	{
		return ret;
	}

	if (ivSize == CRYP_VECTSIZE_64B)
	{
		hCryp->Instance->ES_IV0LR = *((uint32_t *)initAddr);
		initAddr += 4;
		hCryp->Instance->ES_IV0RR = *((uint32_t *)initAddr);
	}
	else if (ivSize == CRYP_VECTSIZE_128B)
	{
		hCryp->Instance->ES_IV0LR = *((uint32_t *)initAddr);
		initAddr += 4;
		hCryp->Instance->ES_IV0RR = *((uint32_t *)initAddr);
		initAddr += 4;
		hCryp->Instance->ES_IV1LR = *((uint32_t *)initAddr);
		initAddr += 4;
		hCryp->Instance->ES_IV1RR = *((uint32_t *)initAddr);
	}
	else
	{
		ret = HAL_ERROR;
		PR_DEBUG("CRYP: don't support init vector size, ivSize = 0x%x.\n", ivSize);
	}

	return ret;
}


/**
  * @brief  Sets the header phase for AES GCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  input: Pointer to the Input buffer.
  * @param  len: Length of the Input buffer, must be a multiple of 16
  * @param  timeout: Timeout value   
  * @retval HAL status
  */
static HAL_Status_T CRYP_GCMCCM_SetHeaderPhase(CRYP_Handle_T * hCryp, uint8_t * input, uint16_t len, uint32_t timeout)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t i = 0;
	uint32_t headAddr = (uint32_t)input;
	
	if (len != 0)
	{
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_HEADER);
		HAL_CRYP_ENABLE(hCryp);

		for (i = 0; i < len; i += 16)
		{
			ret = CRYP_WaitInFiFOEmptyTimeout(hCryp, timeout);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("CRYP: wait input FIFO empty timeout.\n");
				return ret;
			}

			hCryp->Instance->ES_DIN = *((uint32_t *)headAddr);
			headAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)headAddr);
			headAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)headAddr);
			headAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)headAddr);
			headAddr += 4;
		}

		ret = CRYP_WaitCrypIdleTimeout(hCryp, timeout);
		
	}

	return ret;
}


/**
  * @brief  Format the header block for AES CCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param newHeaderSize: return the updated header size
  * @retval HAL status
  */
static HAL_Status_T CRYP_CCM_FormatHeaderBlock(CRYP_Handle_T * hCryp, uint32_t * newHeaderSize)
{
	uint32_t i = 0;
	uint32_t bufferIdx = 0;
	uint32_t headerSize = hCryp->init->headerSize;

	if (headerSize != 0)
	{
		/* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
		if (headerSize < 65280U)
		{
			hCryp->init->pScratch[bufferIdx++] = (uint8_t)((headerSize >> 8) & 0xFF); 
			hCryp->init->pScratch[bufferIdx++] = (uint8_t)(headerSize & 0xFF);
			headerSize += 2;
		}
		else
		{
			/* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
			hCryp->init->pScratch[bufferIdx++] = 0xFF;
			hCryp->init->pScratch[bufferIdx++] = 0xFE;
			hCryp->init->pScratch[bufferIdx++] = (uint8_t)((headerSize >> 24) & 0xFF);
			hCryp->init->pScratch[bufferIdx++] = (uint8_t)((headerSize >> 16) & 0xFF);
			hCryp->init->pScratch[bufferIdx++] = (uint8_t)((headerSize >> 8)  & 0xFF);
			hCryp->init->pScratch[bufferIdx++] = (uint8_t)((headerSize) & 0xFF);
			headerSize += 6;
		}

		for (i = 0; i < hCryp->init->headerSize; i++)
		{
			hCryp->init->pScratch[bufferIdx++] = hCryp->init->header[i];
		}

		/* Check if the header size is modulo 16 */
		if ((headerSize % 16) != 0)
		{
			for (i = headerSize; i <= ((headerSize / 16 + 1) * 16); i++)
			{
				hCryp->init->pScratch[i] = 0;
			}

			headerSize = (headerSize / 16 + 1) * 16;
		}

		*newHeaderSize = headerSize;
	}

	return HAL_OK;
}


/**
  * @brief  Format the B0 block for AES CCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param blockB0: return the formated B0
  * @retval HAL status
  */
static HAL_Status_T CRYP_CCM_FormatB0Block(CRYP_Handle_T * hCryp, uint8_t * blockB0)
{
	uint32_t i  = 0;
	uint32_t headerSize = hCryp->init->headerSize;
	uint8_t * block = blockB0;

	if (headerSize != 0)
	{
		block[0] = 0x40;
	}
	block[0] |= (uint8_t)(((((uint8_t)(hCryp->init->tagSize - 2)) >> 1) & 0x07) << 3);
	block[0] |= (uint8_t)(((uint8_t)((15 - hCryp->init->IVSize) - 1)) & 0x07);

	for (i = 0; i < hCryp->init->IVSize; i++)
	{
		block[i + 1] = hCryp->init->pInitVect[i];
	}

	for (; i < 13; i++)
	{
		block[i + 1] = 0;
	}

	block[14] = (uint8_t)(((hCryp->crypInCount) >> 8) & 0xFF);
	block[15] = (uint8_t)((hCryp->crypInCount) & 0xFF);

	return HAL_OK;
}


/**
  * @brief  Format the initial counter for AES CCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param blockB0: return the formated B0
  * @retval HAL status
  */
static HAL_Status_T CRYP_CCM_FormatInitCounter(CRYP_Handle_T * hCryp, uint8_t * blockB0, uint8_t * initCounter)
{
	uint32_t i = 0;
	uint8_t * counterBlock = initCounter;
	
	/* Byte 0:
         Bits 7 and 6 are reserved and shall be set to 0
         Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter 
         blocks are distinct from B0
         Bits 0, 1, and 2 contain the same encoding of q as in B0
      */
	counterBlock[0] = blockB0[0] & 0x07;
	for (i = 1; i < hCryp->init->IVSize + 1; i++)
	{
		counterBlock[i] = blockB0[i];
	}
	counterBlock[15] |= 0x01;

	return HAL_OK;
}


/**
  * @brief  Set the init phase for AES CCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param blockB0: the formated B0
  * @retval HAL status
  */
static HAL_Status_T CRYP_CCM_SetInitPhase(CRYP_Handle_T * hCryp, uint8_t * blockB0)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t b0Addr = (uint32_t)blockB0;

	HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_INIT);
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	b0Addr += 4;
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	b0Addr += 4;
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	b0Addr += 4;
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);

	HAL_CRYP_ENABLE(hCryp);
	ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);

	return ret;
}


/**
  * @brief  Initializes the CRYP according to the specified
  *         parameters in the CRYP_Config_T.
  * @param  hCryp: pointer to a CRYP_Handle_T structure. 
  * @param config: pointer to a CRYP_Config_T structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_Init(CRYP_Handle_T * hCryp, CRYP_Config_T * config)
{
	HAL_Status_T ret = HAL_OK;
	
	if ((hCryp == NULL) || (config == NULL))
	{
		ret = HAL_ERROR;
		return ret;
	}

	if (hCryp->state == CRYP_STATE_RESET)
	{
		hCryp->lock = HAL_UNLOCKED;
		/* set the CRYP clock */
		
	}

	/* Config key size and data type */
	hCryp->state = CRYP_STATE_BUSY;
	IO_BITMASK_SET(hCryp->Instance->ES_CR, CRYP_CR_KEYSIZE_Msk, config->keySize);
	IO_BITMASK_SET(hCryp->Instance->ES_CR, CRYP_CR_DATATYPE_Msk, config->dataType);

	/* Reset CrypInCount, CrypOutCount and backup config info */
	hCryp->crypInCount = 0;
	hCryp->crypOutCount = 0;
	hCryp->init = config;
	hCryp->callback = config->callback;

	hCryp->state = CRYP_STATE_READY;
	hCryp->phase = CRYP_PHASE_READY;

	return ret;
}


/**
  * @brief  DeInitializes the CRYP peripheral. 
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DeInit(CRYP_Handle_T * hCryp)
{
	HAL_Status_T ret = HAL_OK;

	if (hCryp == NULL)
	{
		ret = HAL_ERROR;
		return ret;
	}

	hCryp->state = CRYP_STATE_BUSY;
	HAL_CRYP_DISABLE(hCryp);
	/* Reset the CRYP clock */
	

	hCryp->crypInCount = 0;
	hCryp->crypOutCount = 0;
	hCryp->phase = CRYP_PHASE_READY;
	hCryp->state = CRYP_STATE_RESET;

	HAL_UNLOCK(hCryp);

	return ret;
}


/**
  * @brief  Computes the authentication TAG.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  size: Total length of the plain/cyphertext buffer
  * @param  authTag: Pointer to the authentication buffer
  * @param  timeout: Timeout duration
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AESGCM_Final(CRYP_Handle_T * hCryp, uint32_t size, uint8_t * authTag, uint32_t timeout)
{
	uint32_t tagAddr = (uint32_t)authTag;
	uint64_t headerLen = hCryp->init->headerSize * 8;  /* header len in bits */
	uint64_t payloadLen = size * 8;                    /* payload len in bits */
	
	HAL_LOCK(hCryp);
	hCryp->state = CRYP_STATE_BUSY;
	
	if (hCryp->phase == CRYP_PHASE_PROCESS)
	{
		hCryp->phase = CRYP_PHASE_FINAL;
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_FINAL);
		HAL_CRYP_ENABLE(hCryp);

		/* Write the number of bits in header (64 bits) followed by the number of bits
       	in the payload according to the datatype */
       	if (hCryp->init->dataType == CRYP_DATATYPE_1B)
       	{
			hCryp->Instance->ES_DIN = __RBIT(headerLen >> 32);
			hCryp->Instance->ES_DIN = __RBIT(headerLen & 0xFFFFFFFF);
			hCryp->Instance->ES_DIN = __RBIT(payloadLen >> 32);
			hCryp->Instance->ES_DIN = __RBIT(payloadLen & 0xFFFFFFF);
       	}
		else if (hCryp->init->dataType == CRYP_DATATYPE_8B)
		{
			hCryp->Instance->ES_DIN = __REV(headerLen >> 32);
			hCryp->Instance->ES_DIN = __REV(headerLen & 0xFFFFFFFF);
			hCryp->Instance->ES_DIN = __REV(payloadLen >> 32);
			hCryp->Instance->ES_DIN = __REV(payloadLen & 0xFFFFFFF);
		}
		else if (hCryp->init->dataType == CRYP_DATATYPE_16B)
		{
			hCryp->Instance->ES_DIN = __ROR((uint32_t)(headerLen >> 32U), 16U);
			hCryp->Instance->ES_DIN = __ROR((uint32_t)headerLen, 16U);
			hCryp->Instance->ES_DIN = __ROR((uint32_t)(payloadLen >> 32U), 16U);
			hCryp->Instance->ES_DIN = __ROR((uint32_t)payloadLen, 16U);
		}
		else if (hCryp->init->dataType == CRYP_DATATYPE_32B)
		{
			hCryp->Instance->ES_DIN = (uint32_t)(headerLen >> 32);
			hCryp->Instance->ES_DIN = (uint32_t)(headerLen & 0xFFFFFFFF);
			hCryp->Instance->ES_DIN = (uint32_t)(payloadLen >> 32);
			hCryp->Instance->ES_DIN = (uint32_t)(payloadLen & 0xFFFFFFF);
		}

		if (CRYP_WaitOutFiFONotEmptyTimeout(hCryp, timeout) != HAL_OK)
		{
			HAL_UNLOCK(hCryp);
			return HAL_TIMEOUT;
		}

		/* Read the Auth TAG in the out FIFO */
		*((uint32_t *)tagAddr) = hCryp->Instance->ES_DOUT;
		tagAddr += 4;
		*((uint32_t *)tagAddr) = hCryp->Instance->ES_DOUT;
		tagAddr += 4;
		*((uint32_t *)tagAddr) = hCryp->Instance->ES_DOUT;
		tagAddr += 4;
		*((uint32_t *)tagAddr) = hCryp->Instance->ES_DOUT;
	}

	hCryp->state = CRYP_STATE_READY;
	HAL_UNLOCK(hCryp);

	return HAL_OK;
}


/**
  * @brief  Computes the authentication TAG for AES CCM mode.
  * @param  hcryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  authTag: Pointer to the authentication buffer
  * @param  timeout: Timeout duration
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AESCCM_Final(CRYP_Handle_T * hCryp, uint8_t * authTag, uint32_t timeout)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t tagAddr = (uint32_t)authTag;
	uint32_t ctrAddr = (uint32_t)hCryp->init->pScratch;
	uint32_t tempTag[4U] = {0U};
	uint32_t i = 0;
	uint8_t tagSize = ((hCryp->init->tagSize > 4) ? (4) : (hCryp->init->tagSize));
  
	HAL_LOCK(hCryp);
	hCryp->state = CRYP_STATE_BUSY;

	if (hCryp->phase == CRYP_PHASE_PROCESS)
	{
		hCryp->phase = CRYP_PHASE_FINAL;
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_FINAL);
		HAL_CRYP_ENABLE(hCryp);

		hCryp->Instance->ES_DIN = *((uint32_t *)ctrAddr);
		ctrAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)ctrAddr);
		ctrAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)ctrAddr);
		ctrAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)ctrAddr);

		ret = CRYP_WaitOutFiFONotEmptyTimeout(hCryp, timeout);
		if (ret != HAL_OK)
		{
			HAL_UNLOCK(hCryp);
			return ret;
		}

		tempTag[0] = hCryp->Instance->ES_DOUT;
		tempTag[1] = hCryp->Instance->ES_DOUT;
		tempTag[2] = hCryp->Instance->ES_DOUT;
		tempTag[3] = hCryp->Instance->ES_DOUT;

		for (i = 0; i < tagSize; i++)
		{
			*((uint8_t *)tagAddr + i) = *((uint8_t *)tempTag + i);
		}

		hCryp->state = CRYP_STATE_READY;
		HAL_UNLOCK(hCryp);

	}
	return HAL_OK;
}


/**
  * @brief  Initializes the CRYP peripheral in AES encryption mode using Interrupt.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AES_Encrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint16_t size, uint8_t * pCypherData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t i = 0;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;
	uint32_t headerSize = 0;
	uint32_t headerAddr = 0;
	uint8_t blockB0[16] = {0};
	uint8_t initCounter[16] = {0};

	if (hCryp->state == CRYP_STATE_READY)
	{
		HAL_LOCK(hCryp);

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pPlainData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pCypherData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			/* set key, algo, work mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_AES);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_AES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_INIT);
				HAL_CRYP_ENABLE(hCryp);
				ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);
				if (ret == HAL_TIMEOUT)
				{
					PR_DEBUG("AES GCM init phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return ret;
				}

				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, hCryp->init->header, hCryp->init->headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES GCM header phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return HAL_TIMEOUT;
				}
				
				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				CRYP_CCM_FormatHeaderBlock(hCryp, &headerSize);
				/* Set the pointer headeraddr to hcryp->Init.pScratch */
				headerAddr = (uint32_t)(hCryp->init->pScratch);
				CRYP_CCM_FormatB0Block(hCryp, &blockB0[0]);
				CRYP_CCM_FormatInitCounter(hCryp, &blockB0[0], &initCounter[0]);

				if (CRYP_CCM_SetInitPhase(hCryp, &blockB0[0]) != HAL_OK)
				{
					PR_DEBUG("AES CCM init phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return HAL_TIMEOUT;
				}

				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, (uint8_t *)headerAddr, headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES CCM header phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return HAL_TIMEOUT;
				}

				/* Save formatted counter into the scratch buffer pScratch */
				for (i = 0; i < 16; i++)
				{
					hCryp->init->pScratch[i] = initCounter[i];
				}
				/* Reset bit 0 */
				hCryp->init->pScratch[15] &= 0xFE;

				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
			}

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		if (size != 0)
		{
			/* enable CRYP interrupt */
			HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
			NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
			NVIC_EnableIRQ(CRYP_IRQn);

			HAL_CRYP_ENABLE(hCryp);
		}
		else
		{
			HAL_UNLOCK(hCryp);
			hCryp->phase = CRYP_STATE_READY;
		}

		return ret;
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
	{
		inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

		hCryp->pCrypInBuffPtr += 16;
		hCryp->crypInCount -= 16;
		if (hCryp->crypInCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
			hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
		}
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
	{
		outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

		hCryp->pCrypOutBuffPtr += 16;
		hCryp->crypOutCount -= 16;
		if (hCryp->crypOutCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
			NVIC_DisableIRQ(CRYP_IRQn);
			if ((hCryp->init->algoMode != CRYP_ALGOMODE_GCM) && \
				(hCryp->init->algoMode != CRYP_ALGOMODE_CCM))
			{
				HAL_CRYP_DISABLE(hCryp);
			}
			hCryp->state = CRYP_STATE_READY;
			HAL_UNLOCK(hCryp);
			
			hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in AES decryption mode using Interrupt.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  size: Length of the cyphertext buffer, must be a multiple of 16.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AES_Decrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint16_t size, uint8_t * pPlainData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;
	uint32_t i = 0;
	uint32_t headerSize = 0;
	uint32_t headerAddr = 0;
	uint8_t blockB0[16] = {0};
	uint8_t initCounter[16] = {0};

	if (hCryp->state == CRYP_STATE_READY)
	{
		HAL_LOCK(hCryp);

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pCypherData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pPlainData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			/* set key, work mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_AES);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_AES, hCryp->init->algoMode, CRYP_CR_ALGODIR_DEC);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_INIT);
				HAL_CRYP_ENABLE(hCryp);
				ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);
				if (ret == HAL_TIMEOUT)
				{
					PR_DEBUG("AES GCM init phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return ret;
				}

				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, hCryp->init->header, hCryp->init->headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES GCM header phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return HAL_TIMEOUT;
				}
				
				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				CRYP_CCM_FormatHeaderBlock(hCryp, &headerSize);
				/* Set the pointer headeraddr to hcryp->Init.pScratch */
				headerAddr = hCryp->init->pScratch;
				CRYP_CCM_FormatB0Block(hCryp, &blockB0[0]);
				CRYP_CCM_FormatInitCounter(hCryp, &blockB0[0], &initCounter[0]);

				if (CRYP_CCM_SetInitPhase(hCryp, &blockB0[0]) != HAL_OK)
				{
					PR_DEBUG("AES CCM init phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return HAL_TIMEOUT;
				}

				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, (uint8_t *)headerAddr, headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES CCM header phase timeout.\n");
					HAL_UNLOCK(hCryp);
					return HAL_TIMEOUT;
				}

				/* Save formatted counter into the scratch buffer pScratch */
				for (i = 0; i < 16; i++)
				{
					hCryp->init->pScratch[i] = initCounter[i];
				}
				/* Reset bit 0 */
				hCryp->init->pScratch[15] &= 0xFE;

				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
			}

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		if (size != 0)
		{
			/* enable CRYP interrupt */
			HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
			NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
			NVIC_EnableIRQ(CRYP_IRQn);

			HAL_CRYP_ENABLE(hCryp);
		}
		else
		{
			HAL_UNLOCK(hCryp);
			hCryp->phase = CRYP_STATE_READY;
		}
		return ret;
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
	{
		inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

		hCryp->pCrypInBuffPtr += 16;
		hCryp->crypInCount -= 16;
		if (hCryp->crypInCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
			hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
		}
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
	{
		outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

		hCryp->pCrypOutBuffPtr += 16;
		hCryp->crypOutCount -= 16;
		if (hCryp->crypOutCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
			NVIC_DisableIRQ(CRYP_IRQn);
			if ((hCryp->init->algoMode != CRYP_ALGOMODE_GCM) && \
				(hCryp->init->algoMode != CRYP_ALGOMODE_CCM))
			{
				HAL_CRYP_DISABLE(hCryp);
			}
			hCryp->state = CRYP_STATE_READY;
			HAL_UNLOCK(hCryp);
			
			hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in DES encryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DES_Encrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint16_t size, uint8_t * pCypherData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;

	if (hCryp->state == CRYP_STATE_READY)
	{
		HAL_LOCK(hCryp);

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pPlainData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pCypherData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_DES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_DES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		/* enable CRYP interrupt */
		HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
		NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
		NVIC_EnableIRQ(CRYP_IRQn);

		HAL_CRYP_ENABLE(hCryp);

		return ret;
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
	{
		inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

		hCryp->pCrypInBuffPtr += 8;
		hCryp->crypInCount -= 8;
		if (hCryp->crypInCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
			hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
		}
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
	{
		outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

		hCryp->pCrypOutBuffPtr += 8;
		hCryp->crypOutCount -= 8;
		if (hCryp->crypOutCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
			NVIC_DisableIRQ(CRYP_IRQn);
			HAL_CRYP_DISABLE(hCryp);
			hCryp->state = CRYP_STATE_READY;
			HAL_UNLOCK(hCryp);
			
			hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in DES decryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the cyphertext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DES_Decrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint16_t size, uint8_t * pPlainData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;

	if (hCryp->state == CRYP_STATE_READY)
	{
		HAL_LOCK(hCryp);

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pCypherData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pPlainData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_DES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_DES, hCryp->init->algoMode, CRYP_CR_ALGODIR_DEC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		/* enable CRYP interrupt */
		HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
		NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
		NVIC_EnableIRQ(CRYP_IRQn);

		HAL_CRYP_ENABLE(hCryp);

		return ret;
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
	{
		inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

		hCryp->pCrypInBuffPtr += 8;
		hCryp->crypInCount -= 8;
		if (hCryp->crypInCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
			hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
		}
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
	{
		outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

		hCryp->pCrypOutBuffPtr += 8;
		hCryp->crypOutCount -= 8;
		if (hCryp->crypOutCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
			NVIC_DisableIRQ(CRYP_IRQn);
			HAL_CRYP_DISABLE(hCryp);
			hCryp->state = CRYP_STATE_READY;
			HAL_UNLOCK(hCryp);
			
			hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in TDES encryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_TDES_Encrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint16_t size, uint8_t * pCypherData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;

	if (hCryp->state == CRYP_STATE_READY)
	{
		HAL_LOCK(hCryp);

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pPlainData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pCypherData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_TDES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_TDES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		/* enable CRYP interrupt */
		HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
		NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
		NVIC_EnableIRQ(CRYP_IRQn);

		HAL_CRYP_ENABLE(hCryp);

		return ret;
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
	{
		inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

		hCryp->pCrypInBuffPtr += 8;
		hCryp->crypInCount -= 8;
		if (hCryp->crypInCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
			hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
		}
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
	{
		outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

		hCryp->pCrypOutBuffPtr += 8;
		hCryp->crypOutCount -= 8;
		if (hCryp->crypOutCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
			NVIC_DisableIRQ(CRYP_IRQn);
			HAL_CRYP_DISABLE(hCryp);
			hCryp->state = CRYP_STATE_READY;
			HAL_UNLOCK(hCryp);
			
			hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in TDES decryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the cyphertext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_TDES_Decrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint16_t size, uint8_t * pPlainData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;

	if (hCryp->state == CRYP_STATE_READY)
	{
		HAL_LOCK(hCryp);

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pCypherData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pPlainData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_TDES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_TDES, hCryp->init->algoMode, CRYP_CR_ALGODIR_DEC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		/* enable CRYP interrupt */
		HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
		NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
		NVIC_EnableIRQ(CRYP_IRQn);

		HAL_CRYP_ENABLE(hCryp);

		return ret;
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
	{
		inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
		inputAddr += 4;
		hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

		hCryp->pCrypInBuffPtr += 8;
		hCryp->crypInCount -= 8;
		if (hCryp->crypInCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
			hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
		}
	}
	else if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
	{
		outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		outputAddr += 4;
		*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

		hCryp->pCrypOutBuffPtr += 8;
		hCryp->crypOutCount -= 8;
		if (hCryp->crypOutCount == 0)
		{
			HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
			NVIC_DisableIRQ(CRYP_IRQn);
			HAL_CRYP_DISABLE(hCryp);
			hCryp->state = CRYP_STATE_READY;
			HAL_UNLOCK(hCryp);
			
			hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
		}
	}

	return ret;
}



//#endif
