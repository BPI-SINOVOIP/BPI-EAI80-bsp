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
#ifdef RABBIT_NO_OS_SYSTEM
#include <stdlib.h>
#endif

//#ifdef CHIP_GM6721


#define CRYP_TIMEOUT_VALUE        30U           /* one millisecond as unit */

static CRYP_Handle_T * tmpHandle;
static DMA_Handle_T s_DMAHandleTX;
static DMA_Handle_T s_DMAHandleRX;


/**
  * @brief  Wait CRYP stop working 
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T CRYP_WaitCrypStopTimeout(CRYP_Handle_T * hCryp, uint32_t timeout)
{
	uint32_t cnt = timeout;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	while (IO_BITS_GET(hCryp->Instance->ES_SR, CRYP_SR_BUSY) == CRYP_SR_BUSY)
	{
		HAL_DelayMs(1);
		if (--cnt == 0)
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
	uint32_t cnt = timeout;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	while (IO_BITS_GET(hCryp->Instance->ES_SR, CRYP_SR_IFEM) != CRYP_SR_IFEM)
	{
		HAL_DelayMs(1);
		if (--cnt == 0)
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
	uint32_t cnt = timeout;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	while (IO_BITS_GET(hCryp->Instance->ES_SR, CRYP_SR_OFNE) != CRYP_SR_OFNE)
	{
		HAL_DelayMs(1);
		if (--cnt == 0)
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
	uint32_t keyAddr = 0;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	keyAddr = (uint32_t)key;
	IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_KEY_FROM_Msk, hCryp->init->keySel);

	if (hCryp->init->keySel == CRYP_SKCFG_KEY_FROM_OTP)
	{
		if (algo == CRYP_ALGO_DES)
		{
			IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_OTP_64_SEL_Msk, hCryp->init->keyPos);
		}
		else
		{
			IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_OTP_SEL_Msk, hCryp->init->keyPos);
		}
	}
	else if (hCryp->init->keySel == CRYP_SKCFG_KEY_FROM_KEYBUF)
	{
		if (algo == CRYP_ALGO_DES)
		{
			IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_KEYBUF_KEY3SEL_Msk, hCryp->init->keyPos);
		}
		else
		{
			IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_KEYBUF_RD_SEL_Msk, hCryp->init->keyPos);
		}
	}
	else
	{
		if (algo == CRYP_ALGO_DES)
		{
			hCryp->Instance->ES_K0LR = __REV(*((uint32_t *)keyAddr));
			keyAddr += 4;
			hCryp->Instance->ES_K0RR = __REV(*((uint32_t *)keyAddr));
		}
		else if (algo == CRYP_ALGO_TDES)
		{
			hCryp->Instance->ES_K0LR = __REV(*((uint32_t *)keyAddr));
			keyAddr += 4;
			hCryp->Instance->ES_K0RR = __REV(*((uint32_t *)keyAddr));
			keyAddr += 4;
			hCryp->Instance->ES_K1LR = __REV(*((uint32_t *)keyAddr));
			keyAddr += 4;
			hCryp->Instance->ES_K1RR = __REV(*((uint32_t *)keyAddr));
			keyAddr += 4;
			hCryp->Instance->ES_K2LR = __REV(*((uint32_t *)keyAddr));
			keyAddr += 4;
			hCryp->Instance->ES_K2RR = __REV(*((uint32_t *)keyAddr));
		}
		else if (algo == CRYP_ALGO_AES)
		{
			switch (keySize)
			{
				case CRYP_KEYSIZE_128B:
					hCryp->Instance->ES_K0LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K0RR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K1LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K1RR = __REV(*((uint32_t *)keyAddr));
					break;
				case CRYP_KEYSIZE_192B:
					hCryp->Instance->ES_K0LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K0RR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K1LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K1RR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K2LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K2RR = __REV(*((uint32_t *)keyAddr));
					break;
				case CRYP_KEYSIZE_256B:
					hCryp->Instance->ES_K0LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K0RR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K1LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K1RR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K2LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K2RR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K3LR = __REV(*((uint32_t *)keyAddr));
					keyAddr += 4;
					hCryp->Instance->ES_K3RR = __REV(*((uint32_t *)keyAddr));
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
	uint32_t initAddr;
	uint32_t algoMode;
	uint32_t tmpData;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	initAddr = (uint32_t)initVector;
	algoMode = hCryp->init->algoMode;
	if (algoMode == CRYP_ALGOMODE_ECB)
	{
		return ret;
	}
	else if (algoMode == CRYP_ALGOMODE_GCM)
	{
		initAddr = (uint32_t)(hCryp->init->pScratch);
		hCryp->Instance->ES_IV0LR = __REV(*((uint32_t *)initAddr));
		initAddr += 4;
		hCryp->Instance->ES_IV0RR = __REV(*((uint32_t *)initAddr));
		initAddr += 4;
		hCryp->Instance->ES_IV1LR = __REV(*((uint32_t *)initAddr));
		initAddr += 4;
		//tmpData = (uint32_t)((*((uint32_t *)initAddr) + 1) % (1 << 32));
		tmpData = __REV(*((uint32_t *)initAddr));
		hCryp->Instance->ES_IV1RR = (tmpData + 1);
	}
	else
	{
		if (ivSize == CRYP_VECTSIZE_64B)
		{
			hCryp->Instance->ES_IV0LR = __REV(*((uint32_t *)initAddr));
			initAddr += 4;
			hCryp->Instance->ES_IV0RR = __REV(*((uint32_t *)initAddr));
		}
		else if (ivSize == CRYP_VECTSIZE_128B)
		{
			hCryp->Instance->ES_IV0LR = __REV(*((uint32_t *)initAddr));
			initAddr += 4;
			hCryp->Instance->ES_IV0RR = __REV(*((uint32_t *)initAddr));
			initAddr += 4;
			hCryp->Instance->ES_IV1LR = __REV(*((uint32_t *)initAddr));
			initAddr += 4;
			hCryp->Instance->ES_IV1RR = __REV(*((uint32_t *)initAddr));
		}
		else
		{
			ret = HAL_ERROR;
			PR_DEBUG("CRYP: don't support init vector size, ivSize = 0x%x.\n", ivSize);
		}
	}

	return ret;
}


/**
  * @brief  feed the data to CRYP core
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param data: point the data buffer that need to feed to the CRYP core
  * @param len: the buffer length
  * @param  timeout: Timeout value 
  * @retval HAL status
  * @note  refer to spec <nistspecialpublication800-38d>,P15
  */
static HAL_Status_T CRYP_FeedData(CRYP_Handle_T * hCryp, uint8_t * data, uint32_t len, uint32_t timeout)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t inputAddr = (uint32_t)data;
	uint32_t dataLen = len;
	uint8_t dataBuf[16] = {0};

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->init->algo == CRYP_ALGO_AES)
	{
		while (dataLen > 16)
		{
			HAL_CRYP_FLUSH_FIFO(hCryp);
			
			ret = CRYP_WaitInFiFOEmptyTimeout(hCryp, timeout);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("CRYP: wait input FIFO empty timeout.\n");
				return ret;
			}
			
			hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
			inputAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
			inputAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
			inputAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
			inputAddr += 4;

			dataLen -= 16;

			ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("Cryp feed data timeout.\n");
				return ret;
			}
		}

		if (dataLen > 0)    //need to padding
		{
			memset(dataBuf, 0, sizeof(dataBuf));
			memcpy(dataBuf, (void *)inputAddr, dataLen);
			
			ret = CRYP_WaitInFiFOEmptyTimeout(hCryp, timeout);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("CRYP: wait input FIFO empty timeout.\n");
				return ret;
			}
			hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[0]));
			hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[4]));
			hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[8]));
			hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[12]));

			dataLen = 0;

			ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("Cryp feed data timeout.\n");
				return ret;
			}
		}
	}
	else
	{
		while (dataLen > 8)	
		{
			HAL_CRYP_FLUSH_FIFO(hCryp);
			
			ret = CRYP_WaitInFiFOEmptyTimeout(hCryp, timeout);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("CRYP: wait input FIFO empty timeout.\n");
				return ret;
			}
			
			hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
			inputAddr += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
			inputAddr += 4;

			dataLen -= 8;

			ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("Cryp feed data timeout.\n");
				return ret;
			}
		}

		if (dataLen > 0)    //need to padding
		{
			memset(dataBuf, 0, sizeof(dataBuf));
			memcpy(dataBuf, (void *)inputAddr, dataLen);
			
			ret = CRYP_WaitInFiFOEmptyTimeout(hCryp, timeout);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("CRYP: wait input FIFO empty timeout.\n");
				return ret;
			}
			hCryp->Instance->ES_DIN = (uint32_t)(&dataBuf[0]);
			hCryp->Instance->ES_DIN = (uint32_t)(&dataBuf[4]);

			dataLen = 0;

			ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);
			if (ret == HAL_TIMEOUT)
			{
				PR_DEBUG("Cryp feed data timeout.\n");
				return ret;
			}
		}
	}
	
	return HAL_OK;
}


/**
  * @brief  Sets the header phase for AES GCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  input: Pointer to the Input buffer.
  * @param  len: Length of the Input buffer
  * @param  timeout: Timeout value   
  * @retval HAL status
  */
static HAL_Status_T CRYP_GCMCCM_SetHeaderPhase(CRYP_Handle_T * hCryp, uint8_t * input, uint32_t len, uint32_t timeout)
{
	HAL_Status_T ret = HAL_OK;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	if (len != 0)
	{
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_HEADER);
		HAL_CRYP_FLUSH_FIFO(hCryp);
		HAL_CRYP_ENABLE(hCryp);

		ret = CRYP_FeedData(hCryp, input, len, timeout);
	}

	return ret;
}


/**
  * @brief  Sets the header phase for AES GCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  * @note  refer to spec <nistspecialpublication800-38d>,P15
  */
static HAL_Status_T CRYP_GCM_GenerateJ0Block(CRYP_Handle_T * hCryp)
{
	uint32_t i = 0;
	HAL_Status_T ret = HAL_OK;
	uint8_t data[16] = {0};
	uint64_t ivSizeBit = 0;
	uint32_t padLen = 0;
	uint32_t headLen = 0;
	uint8_t * headBuf;
	uint32_t headAddr;
	uint32_t outputAddr;
	uint32_t vectAddr;

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	vectAddr = (uint32_t)(hCryp->init->pInitVect);
	if (hCryp->init->IVSize != 12)
	{
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_FLUSH_FIFO(hCryp);
		/* set key, algo, work mode, IV and flush fifo */		
		HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_AES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_INIT);
		HAL_CRYP_ENABLE(hCryp);
		HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);
		
		memset(data, 0, sizeof(data));
		ret = CRYP_FeedData(hCryp, data, 16, CRYP_TIMEOUT_VALUE);
		if (ret == HAL_TIMEOUT)
		{
			PR_DEBUG("AES GCM init phase timeout.\n");
			HAL_UnLock(&(hCryp->lock));
			return ret;
		}

		ivSizeBit = (hCryp->init->IVSize) * 8;
		padLen = 128 * (ivSizeBit / 128 + 1) - ivSizeBit;
		headLen = (hCryp->init->IVSize + (padLen+64+64)/8);
#ifndef  RABBIT_NO_OS_SYSTEM
		headBuf = (uint8_t *)k_malloc(headLen);
#else
		headBuf = (uint8_t *)malloc(headLen);
#endif
		if (headBuf == NULL)
		{
			HAL_UnLock(&(hCryp->lock));
			return HAL_ERROR;
		}
		memset(headBuf, 0, headLen);
		memcpy(headBuf, (void *)vectAddr, hCryp->init->IVSize);
		headAddr = (uint32_t)(&headBuf[headLen-8]);
		*(uint32_t *)headAddr = __REV(ivSizeBit >> 32);
		headAddr += 4;
		*(uint32_t *)headAddr = __REV(ivSizeBit & 0xFFFFFFFF);
		
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_HEADER);
		HAL_CRYP_FLUSH_FIFO(hCryp);
		HAL_CRYP_ENABLE(hCryp);
		for (i = 0; i < (headLen/16); i++)
		{
			hCryp->Instance->ES_DIN = *((uint32_t *)headBuf);
			headBuf += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)headBuf);
			headBuf += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)headBuf);
			headBuf += 4;
			hCryp->Instance->ES_DIN = *((uint32_t *)headBuf);
			headBuf += 4;

			if (CRYP_WaitOutFiFONotEmptyTimeout(hCryp, CRYP_TIMEOUT_VALUE) != HAL_OK)
			{
				HAL_UnLock(&(hCryp->lock));
				return HAL_TIMEOUT;
			}
			
			outputAddr = (uint32_t)(hCryp->init->pScratch);
			*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
			outputAddr += 4;
			*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
			outputAddr += 4;
			*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
			outputAddr += 4;
			*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
		}
#ifndef RABBIT_NO_OS_SYSTEM
		k_free(headBuf);
#else
		free(headBuf);
#endif
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_FLUSH_FIFO(hCryp);
	}
	else
	{
		memcpy(hCryp->init->pScratch, hCryp->init->pInitVect, 12);
		hCryp->init->pScratch[12] = 0x00;
		hCryp->init->pScratch[13] = 0x00;
		hCryp->init->pScratch[14] = 0x00;
		hCryp->init->pScratch[15] = 0x01;
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
	uint32_t headerSize;

	if ((hCryp == NULL) || (newHeaderSize == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	headerSize = hCryp->init->headerSize;
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
			/* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets
			  * only consider the case : 65280 < header < 2^32
			  */
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
			for (i = headerSize; i < ((headerSize / 16 + 1) * 16); i++)
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

	if (hCryp->crypInCount < 65536)
	{
		for (; i < 13; i++)
		{
			block[i + 1] = 0;
		}
		
		block[14] = (uint8_t)(((hCryp->crypInCount) >> 8) & 0xFF);
		block[15] = (uint8_t)((hCryp->crypInCount) & 0xFF);
	}
	else
	{
		for (; i < 11; i++)
		{
			block[i + 1] = 0;
		}
		
		block[12] = (uint8_t)(((hCryp->crypInCount) >> 24) & 0xFF);
		block[13] = (uint8_t)(((hCryp->crypInCount) >> 16) & 0xFF);
		block[14] = (uint8_t)(((hCryp->crypInCount) >> 8) & 0xFF);
		block[15] = (uint8_t)((hCryp->crypInCount) & 0xFF);
	}

	return HAL_OK;
}


/**
  * @brief  Format the initial counter for AES CCM mode
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param blockB0: the formated B0
  * @param initCounter: return the formated initial counter
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
	HAL_CRYP_ENABLE(hCryp);
	HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);
	
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	b0Addr += 4;
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	b0Addr += 4;
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	b0Addr += 4;
	hCryp->Instance->ES_DIN = *((uint32_t *)b0Addr);
	
	ret = CRYP_WaitCrypStopTimeout(hCryp, CRYP_TIMEOUT_VALUE);

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
	uint32_t tagAddr;
	uint64_t headerLen;
	uint64_t payloadLen = size * 8;                    /* payload len in bits */
	uint32_t initAddr;
	
	if ((hCryp == NULL) || (authTag == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}
	
	tagAddr = (uint32_t)authTag;
	headerLen = hCryp->init->headerSize * 8;  /* header len in bits */
	if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
		return HAL_BUSY;
	
	hCryp->state = CRYP_STATE_BUSY;
	
	if ((hCryp->phase == CRYP_PHASE_READY) || (hCryp->phase == CRYP_PHASE_PROCESS))
	{
		hCryp->phase = CRYP_PHASE_FINAL;
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_FINAL);
		HAL_CRYP_FLUSH_FIFO(hCryp);

		initAddr = (uint32_t)(hCryp->init->pScratch);
		hCryp->Instance->ES_IV0LR = __REV(*((uint32_t *)initAddr));
		initAddr += 4;
		hCryp->Instance->ES_IV0RR = __REV(*((uint32_t *)initAddr));
		initAddr += 4;
		hCryp->Instance->ES_IV1LR = __REV(*((uint32_t *)initAddr));
		initAddr += 4;
		hCryp->Instance->ES_IV1RR = __REV(*((uint32_t *)initAddr));
			
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
			HAL_UnLock(&(hCryp->lock));
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

		HAL_CRYP_DISABLE(hCryp);
	}

	hCryp->state = CRYP_STATE_READY;
	HAL_UnLock(&(hCryp->lock));

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
	uint32_t tagAddr;
	uint32_t ctrAddr;
	uint32_t tempTag[4U] = {0U};
	uint32_t i = 0;
	uint8_t tagSize;

	if ((hCryp == NULL) || (authTag == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	tagAddr = (uint32_t)authTag;
	ctrAddr = (uint32_t)hCryp->init->pScratch;
	tagSize = ((hCryp->init->tagSize > 16) ? (16) : (hCryp->init->tagSize));
	if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
		return HAL_BUSY;
	hCryp->state = CRYP_STATE_BUSY;

	if ((hCryp->phase == CRYP_PHASE_READY) || (hCryp->phase == CRYP_PHASE_PROCESS))
	{
		hCryp->phase = CRYP_PHASE_FINAL;
		HAL_CRYP_DISABLE(hCryp);
		HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_FINAL);
		HAL_CRYP_FLUSH_FIFO(hCryp);
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
			HAL_UnLock(&(hCryp->lock));
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
		HAL_UnLock(&(hCryp->lock));

	}
	return HAL_OK;
}


/**
  * @brief  process CRYP interrupt
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *                the configuration information for CRYP.
  * @none
  */


void HAL_CRYP_IRQHandler_Func(void ** handle/*CRYP_Handle_T * hCryp*/)
{
	CRYP_Handle_T * hCryp = (CRYP_Handle_T *)(*handle);
	uint8_t algo = hCryp->init->algo;
	uint32_t inputAddr = 0;
	uint32_t outputAddr = 0;
	uint8_t dataBuf[16] = {0};
	uint32_t dataOut[4] = {0};

	if ((algo == CRYP_ALGO_DES) || (algo == CRYP_ALGO_TDES))
	{
		if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
		{
			IO_WRITE_REG(hCryp->Instance->ES_ISR, CRYP_ITSTATUS_OUTFIFO);

			if (hCryp->crypOutCount >= 8)
			{
				outputAddr = (uint32_t)(hCryp->pCrypOutBuffPtr);
				*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;
				outputAddr += 4;
				*((uint32_t *)outputAddr) = hCryp->Instance->ES_DOUT;

				hCryp->pCrypOutBuffPtr += 8;
				hCryp->crypOutCount -= 8;
			}
			else if (hCryp->crypOutCount > 0)
			{
				memset(dataOut, 0, sizeof(dataOut));
				dataOut[0] = hCryp->Instance->ES_DOUT;
				dataOut[1] = hCryp->Instance->ES_DOUT;

				memcpy(hCryp->pCrypOutBuffPtr, dataOut, hCryp->crypOutCount);
				hCryp->pCrypOutBuffPtr += hCryp->crypOutCount;
				hCryp->crypOutCount = 0;
			}

			if (hCryp->crypOutCount == 0)
			{
				HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
				HAL_IRQ_DISABLE(CRYP_IRQn);
				HAL_CRYP_DISABLE(hCryp);
				hCryp->state = CRYP_STATE_READY;
				HAL_UnLock(&(hCryp->lock));

				hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
			}
		}

		if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
		{
			IO_WRITE_REG(hCryp->Instance->ES_ISR, CRYP_ITSTATUS_INFIFO);
			if (hCryp->crypInCount >= 8)
			{
				inputAddr = (uint32_t)(hCryp->pCrypInBuffPtr);
				hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);
				inputAddr += 4;
				hCryp->Instance->ES_DIN = *((uint32_t *)inputAddr);

				hCryp->pCrypInBuffPtr += 8;
				hCryp->crypInCount -= 8;
			}
			else if (hCryp->crypInCount > 0)
			{
				memset(dataBuf, 0, sizeof(dataBuf));
				memcpy(dataBuf, hCryp->pCrypInBuffPtr, hCryp->crypInCount);
				hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[0]));
				hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[4]));

				hCryp->pCrypInBuffPtr += hCryp->crypInCount;
				hCryp->crypInCount = 0;
			}

			if (hCryp->crypInCount == 0)
			{
				HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
				hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
			}
		}
	}
	else if (algo == CRYP_ALGO_AES)
	{
		if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_OUTFIFO))
		{
			IO_WRITE_REG(hCryp->Instance->ES_ISR, CRYP_ITSTATUS_OUTFIFO);

			if (hCryp->crypOutCount >= 16)
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
			}
			else if (hCryp->crypOutCount > 0)
			{
				memset(dataOut, 0, sizeof(dataOut));
				dataOut[0] = hCryp->Instance->ES_DOUT;
				dataOut[1] = hCryp->Instance->ES_DOUT;
				dataOut[2] = hCryp->Instance->ES_DOUT;
				dataOut[3] = hCryp->Instance->ES_DOUT;

				memcpy(hCryp->pCrypOutBuffPtr, dataOut, hCryp->crypOutCount);
				hCryp->pCrypOutBuffPtr += hCryp->crypOutCount;
				hCryp->crypOutCount = 0;
			}

			if (hCryp->crypOutCount == 0)
			{
				HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_OUTFIFO);
				HAL_IRQ_DISABLE(CRYP_IRQn);
				HAL_CRYP_DISABLE(hCryp);
				hCryp->state = CRYP_STATE_READY;
				HAL_UnLock(&(hCryp->lock));

				hCryp->callback(hCryp, CRYP_EVENT_OUTFIFO_TRANS_DONE, 0);
			}

		}

		if (HAL_CRYP_GET_ITSTATUS(hCryp, CRYP_ITSTATUS_INFIFO))
		{
			IO_WRITE_REG(hCryp->Instance->ES_ISR, CRYP_ITSTATUS_INFIFO);

			if (hCryp->crypInCount >= 16)
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
			}
			else if (hCryp->crypInCount > 0)
			{
				memset(dataBuf, 0, sizeof(dataBuf));
				memcpy(dataBuf, hCryp->pCrypInBuffPtr, hCryp->crypInCount);
				hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[0]));
				hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[4]));
				hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[8]));
				hCryp->Instance->ES_DIN = *((uint32_t *)(&dataBuf[12]));

				hCryp->pCrypInBuffPtr += hCryp->crypInCount;
				hCryp->crypInCount = 0;
			}

			if (hCryp->crypInCount == 0)
			{
				HAL_CRYP_DISABLE_IT(hCryp, CRYP_IT_INFIFO);
				hCryp->callback(hCryp, CRYP_EVENT_INFIFO_TRANS_DONE, 0);
			}
		}
	}
}

void HAL_CRYP_IRQHandler(void)
{
	HAL_CRYP_IRQHandler_Func((void **)&tmpHandle);
}

/**
  * @brief  DMA CRYP transfer complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYP_DMAXferCplt(DMA_Handle_T * hDma, CRYP_Event_T event)
{
	CRYP_Handle_T * hCryp = (CRYP_Handle_T *)(hDma->pParent);
	
	if (event == CRYP_EVENT_INFIFO_TRANS_DONE)
	{
		IO_BIT_CLEAR(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
		HAL_DMA_Abort(hDma);
		hCryp->state = CRYP_STATE_READY;
		hCryp->phase = CRYP_PHASE_READY;
		if (hCryp->callback)
		{
			hCryp->callback(hCryp, event, 0);
		}
	}
	else if (event == CRYP_EVENT_OUTFIFO_TRANS_DONE)
	{
		IO_BIT_CLEAR(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
		HAL_DMA_Abort(hDma);
		hCryp->state = CRYP_STATE_READY;
		hCryp->phase = CRYP_PHASE_READY;
		if (hCryp->callback)
		{
			hCryp->callback(hCryp, event, 0);
		}
	}

}


/**
  * @brief  DMA CRYP communication error callback. 
  * @param  hDma: DMA handle
  * @retval None
  */
static void CRYP_DMAError(DMA_Handle_T * hDma, CRYP_Event_T event)
{
	CRYP_Handle_T * hCryp = (CRYP_Handle_T *)(hDma->pParent);

	hCryp->state = CRYP_STATE_ERROR;
	HAL_UnLock(&(hCryp->lock));
	if (hCryp->callback)
	{
		hCryp->callback(hCryp, event, 0);
	}
}


/**
  * @brief  DMA CRYP TX callback. 
  * @param  hDma: point to the DMA handle.
  * @param  event: specify the DMA event.
  * @param  param: the parameter of the specify DMA event.
  * @retval None
  */
static void CRYP_DMATXCallback(struct DMA_Handle * hDma, uint8_t event, unsigned long param)
{
	if (event == DMA_EVENT_TRANSFER_DONE)
	{
		CRYP_DMAXferCplt(hDma, CRYP_EVENT_INFIFO_TRANS_DONE);
	}
	else if (event == DMA_EVENT_TRANSFER_ERROR)
	{
		CRYP_DMAError(hDma, CRYP_EVENT_INFIFO_TRANS__ERR);
	}
}


/**
  * @brief  DMA CRYP RX callback. 
  * @param  hDma: point to the DMA handle.
  * @param  event: specify the DMA event.
  * @param  param: the parameter of the specify DMA event.
  * @retval None
  */
static void CRYP_DMARXCallback(struct DMA_Handle * hDma, uint8_t event, unsigned long param)
{
	if (event == DMA_EVENT_TRANSFER_DONE)
	{
		CRYP_DMAXferCplt(hDma, CRYP_EVENT_OUTFIFO_TRANS_DONE);
	}
	else if (event == DMA_EVENT_TRANSFER_ERROR)
	{
		CRYP_DMAError(hDma, CRYP_EVENT_OUTFIFO_TRANS__ERR);
	}
}


/**
  * @brief  Initialize DMA for CRYP peripheral.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
static HAL_Status_T CRYP_DMAInit(CRYP_Handle_T * hCryp)
{
	DMA_Config_T dmaConfig;
	DMA_Request_Result_T reqCRYPTX;
	DMA_Request_Result_T reqCRYPRX;
	RET_CODE ret = RET_ERROR;
	
	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	/* request DMA channel for CRYP TX */
	ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_CRYP_TX, &reqCRYPTX);
	if(ret != RET_OK)
	{
		return HAL_ERROR;
	}

	memset(&dmaConfig, 0, sizeof(dmaConfig));
	dmaConfig.channelIdx = reqCRYPTX.channelIdx;
	dmaConfig.deviceIdx = reqCRYPTX.deviceIdx;
	dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
	dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
	dmaConfig.width = DMA_WIDTH_32BIT;
 	dmaConfig.burst = DMA_BURST_1;
	dmaConfig.priority = DMA_PRIORITY_LOW;//DMA_PRIORITY_HIGH;
	dmaConfig.alter = DMA_PRIMARY;
	dmaConfig.callback = CRYP_DMATXCallback;
	HAL_DMA_Init(DMA_DEV, &dmaConfig, &s_DMAHandleTX);
	LINK_DMAHDL_TO_PARENTHDL(hCryp, hdmain, s_DMAHandleTX);

	/* request DMA channel for CRYP RX */
	ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_CRYP_RX, &reqCRYPRX);
	if(ret != RET_OK)
	{
		return HAL_ERROR;
	}

	memset(&dmaConfig, 0, sizeof(dmaConfig));
	dmaConfig.channelIdx = reqCRYPRX.channelIdx;
	dmaConfig.deviceIdx = reqCRYPRX.deviceIdx;
	dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
	dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
	dmaConfig.width = DMA_WIDTH_32BIT;
	if (hCryp->init->algo == CRYP_ALGO_AES)
	{
		dmaConfig.burst = DMA_BURST_4;
	}
	else
	{
		dmaConfig.burst = DMA_BURST_2;
	}
	dmaConfig.priority = DMA_PRIORITY_HIGH;
	dmaConfig.alter = DMA_PRIMARY;
	dmaConfig.callback = CRYP_DMARXCallback;
	HAL_DMA_Init(DMA_DEV, &dmaConfig, &s_DMAHandleRX);
	LINK_DMAHDL_TO_PARENTHDL(hCryp, hdmaout, s_DMAHandleRX);

	return HAL_OK;
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
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_RESET)
	{
		hCryp->lock = HAL_UNLOCK;
		hCryp->Instance = CRYPTO;
		/* set the CRYP clock and reset CRYP */
		*((volatile uint32_t *)0x40000080) |= (1 << 5);
		*((volatile uint32_t *)0x40000070) |= (1 << 5);
#if 0
		if (config->algo == CRYP_ALGO_DES)
		{			
			/* open debug mode, take care */
			IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CS, CRYP_SKCS_DBGKEYSEL_Msk, CRYP_SKCS_DBGKEYSEL_DES);
		}
		else
		{
			/* open debug mode, take care */
			IO_BITMASK_SET(hCryp->Instance->CRYP_SK_CS, CRYP_SKCS_DBGKEYSEL_Msk, CRYP_SKCS_DBGKEYSEL_AES);
		}

		/* output to key buffer */
		//IO_BITS_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_OUT_TO_KEYBUF);
		//IO_BITS_SET(hCryp->Instance->CRYP_SK_CFG, CRYP_SKCFG_KEYBUF_TO_K1_LOW);
#endif		
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

	//test temp
	tmpHandle = hCryp;
	
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
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	hCryp->state = CRYP_STATE_BUSY;
	HAL_CRYP_DISABLE(hCryp);
	/* Reset the CRYP clock */
	

	hCryp->crypInCount = 0;
	hCryp->crypOutCount = 0;
	hCryp->phase = CRYP_PHASE_READY;
	hCryp->state = CRYP_STATE_RESET;

	HAL_UnLock(&(hCryp->lock));

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in AES encryption mode using Interrupt.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AES_Encrypt(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint32_t size, uint8_t * pCypherData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t i = 0;
	uint32_t headerSize = 0;
	uint32_t headerAddr = 0;
	uint8_t blockB0[16] = {0};
	uint8_t initCounter[16] = {0};
	uint8_t data[16] = {0};

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_READY)
	{
		if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
			return HAL_BUSY;

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pPlainData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pCypherData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			if (hCryp->init->enableDMA == TRUE)
			{
				CRYP_DMAInit(hCryp);
			}
			
			/* set key, algo, work mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_AES);
			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				CRYP_GCM_GenerateJ0Block(hCryp);
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				CRYP_CCM_FormatHeaderBlock(hCryp, &headerSize);
				/* Set the pointer headeraddr to hcryp->Init.pScratch */
				headerAddr = (uint32_t)(hCryp->init->pScratch);
				CRYP_CCM_FormatB0Block(hCryp, &blockB0[0]);
				memset(initCounter, 0, sizeof(initCounter));
				CRYP_CCM_FormatInitCounter(hCryp, &blockB0[0], &initCounter[0]);
				hCryp->init->IVSize = CRYP_VECTSIZE_128B;
			}
			
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_AES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				CRYP_SetInitVector(hCryp, initCounter, hCryp->init->IVSize);
			}
			else
			{
				CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			}
			HAL_CRYP_FLUSH_FIFO(hCryp);

			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_INIT);
				HAL_CRYP_ENABLE(hCryp);
				HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);

				memset(data, 0, sizeof(data));
				if (CRYP_FeedData(hCryp, data, 16, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES GCM init phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return ret;
				}
				
				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, hCryp->init->header, hCryp->init->headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES GCM header phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return HAL_TIMEOUT;
				}
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				if (CRYP_CCM_SetInitPhase(hCryp, &blockB0[0]) != HAL_OK)
				{
					PR_DEBUG("AES CCM init phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return HAL_TIMEOUT;
				}

				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, (uint8_t *)headerAddr, headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES CCM header phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return HAL_TIMEOUT;
				}

				/* Save formatted counter into the scratch buffer pScratch */
				for (i = 0; i < 16; i++)
				{
					hCryp->init->pScratch[i] = initCounter[i];
				}
				/* Reset bit 0 */
				hCryp->init->pScratch[15] &= 0xFE;
			}

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		if (size != 0)
		{
			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
				IO_WRITE_REG(hCryp->Instance->ES_BL, hCryp->crypInCount*8);
			}
			else if(hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
			}
			
			HAL_CRYP_ENABLE(hCryp);
			HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);

			if (hCryp->init->enableDMA == FALSE)
			{
				//temparaly code, need to delete later
				IO_WRITE_REG(hCryp->Instance->ES_ISR, CRYP_ITSTATUS_OUTFIFO);

				/* enable CRYP interrupt */
				HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
				//NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
				//NVIC_EnableIRQ(CRYP_IRQn);
				HAL_RequestIrq(CRYP_IRQn, CRYP_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_CRYP_IRQHandler, &tmpHandle/*(const void *)hCryp*/);
				HAL_IRQ_ENABLE(CRYP_IRQn);
				HAL_UnLock(&(hCryp->lock));
			}
			else
			{
				//DMA mode
				uint32_t tranSize = 0;			
				tranSize = (hCryp->crypInCount % 16) ? ((hCryp->crypInCount + 15) / 16) : (hCryp->crypInCount / 16);

				/* enable Cryp DMA TX mode */
				IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMATXLVL_Msk, (4<<CRYP_DMACR_DMATXLVL_Pos));
				IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
				HAL_DMA_Start(hCryp->hdmain, (uint32_t)hCryp->pCrypInBuffPtr, (uint32_t)&(hCryp->Instance->ES_DIN), tranSize * 4);

				/* enable Cryp DMA RX mode */
				IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMARXLVL_Msk, (4<<CRYP_DMACR_DMARXLVL_Pos));
				IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
				HAL_DMA_Start(hCryp->hdmaout, (uint32_t)&(hCryp->Instance->ES_DOUT), (uint32_t)(hCryp->pCrypOutBuffPtr), tranSize * 4);
				HAL_UnLock(&(hCryp->lock));
			}
		}
		else
		{
			HAL_UnLock(&(hCryp->lock));
			hCryp->phase = CRYP_PHASE_READY;
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in AES decryption mode using Interrupt.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  size: Length of the cyphertext buffer.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AES_Decrypt(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint32_t size, uint8_t * pPlainData)
{
	HAL_Status_T ret = HAL_OK;
	uint32_t i = 0;
	uint32_t headerSize = 0;
	uint32_t headerAddr = 0;
	uint8_t blockB0[16] = {0};
	uint8_t initCounter[16] = {0};
	uint8_t data[16] = {0};

	if (hCryp == NULL)
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_READY)
	{
		if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
			return HAL_BUSY;

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pCypherData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pPlainData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			if (hCryp->init->enableDMA == TRUE)
			{
				CRYP_DMAInit(hCryp);
			}
			
			/* set key, work mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_AES);
			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				CRYP_GCM_GenerateJ0Block(hCryp);
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				CRYP_CCM_FormatHeaderBlock(hCryp, &headerSize);
				/* Set the pointer headeraddr to hcryp->Init.pScratch */
				headerAddr = (uint32_t)(hCryp->init->pScratch);
				CRYP_CCM_FormatB0Block(hCryp, &blockB0[0]);
				memset(initCounter, 0, sizeof(initCounter));
				CRYP_CCM_FormatInitCounter(hCryp, &blockB0[0], &initCounter[0]);
				hCryp->init->IVSize = CRYP_VECTSIZE_128B;
			}
			
			if (hCryp->init->algoMode == CRYP_ALGOMODE_CTR)
			{
				HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_AES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			}
			else
			{
				HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_AES, hCryp->init->algoMode, CRYP_CR_ALGODIR_DEC);
			}
			if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				CRYP_SetInitVector(hCryp, initCounter, hCryp->init->IVSize);
			}
			else
			{
				CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			}
			HAL_CRYP_FLUSH_FIFO(hCryp);

			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_INIT);
				HAL_CRYP_ENABLE(hCryp);
				HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);

				memset(data, 0, sizeof(data));
				if (CRYP_FeedData(hCryp, data, 16, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES GCM init phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return ret;
				}
				
				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, hCryp->init->header, hCryp->init->headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES GCM header phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return HAL_TIMEOUT;
				}
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				if (CRYP_CCM_SetInitPhase(hCryp, &blockB0[0]) != HAL_OK)
				{
					PR_DEBUG("AES CCM init phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return HAL_TIMEOUT;
				}

				if (CRYP_GCMCCM_SetHeaderPhase(hCryp, (uint8_t *)headerAddr, headerSize, CRYP_TIMEOUT_VALUE) != HAL_OK)
				{
					PR_DEBUG("AES CCM header phase timeout.\n");
					HAL_UnLock(&(hCryp->lock));
					return HAL_TIMEOUT;
				}

				/* Save formatted counter into the scratch buffer pScratch */
				for (i = 0; i < 16; i++)
				{
					hCryp->init->pScratch[i] = initCounter[i];
				}
				/* Reset bit 0 */
				hCryp->init->pScratch[15] &= 0xFE;
			}

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		if (size != 0)
		{
			if (hCryp->init->algoMode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
				IO_WRITE_REG(hCryp->Instance->ES_BL, hCryp->crypInCount*8);
			}
			else if (hCryp->init->algoMode == CRYP_ALGOMODE_CCM)
			{
				HAL_CRYP_DISABLE(hCryp);
				HAL_CRYP_GCMCCM_SET_PHASE(hCryp, CRYP_GCMCCM_PHASE_PAYLOAD);
				HAL_CRYP_FLUSH_FIFO(hCryp);
				IO_WRITE_REG(hCryp->Instance->ES_BL, hCryp->crypInCount*8);
			}
			
			HAL_CRYP_ENABLE(hCryp);
			HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);

			if (hCryp->init->enableDMA == FALSE)
			{
				//temparaly code, need to delete later
				IO_WRITE_REG(hCryp->Instance->ES_ISR, CRYP_ITSTATUS_OUTFIFO);

				/* enable CRYP interrupt */
				HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
				//NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
				//NVIC_EnableIRQ(CRYP_IRQn);
				HAL_RequestIrq(CRYP_IRQn, CRYP_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_CRYP_IRQHandler, &tmpHandle/*(const void *)hCryp*/);
				HAL_IRQ_ENABLE(CRYP_IRQn);
				HAL_UnLock(&(hCryp->lock));
			}
			else
			{
				//DMA mode			
				uint32_t tranSize = 0;			
				tranSize = (hCryp->crypInCount % 16) ? ((hCryp->crypInCount + 15) / 16) : (hCryp->crypInCount / 16);
				
				/* enable Cryp DMA TX mode */
				IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMATXLVL_Msk, (4<<CRYP_DMACR_DMATXLVL_Pos));
				IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
				HAL_DMA_Start(hCryp->hdmain, (uint32_t)hCryp->pCrypInBuffPtr, (uint32_t)&(hCryp->Instance->ES_DIN), tranSize * 4);

				/* enable Cryp DMA RX mode */
				IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMARXLVL_Msk, (4<<CRYP_DMACR_DMARXLVL_Pos));
				IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
				HAL_DMA_Start(hCryp->hdmaout, (uint32_t)&(hCryp->Instance->ES_DOUT), (uint32_t)hCryp->pCrypOutBuffPtr, tranSize * 4);
				
				HAL_UnLock(&(hCryp->lock));
			}
		}
		else
		{
			hCryp->phase = CRYP_PHASE_READY;
			HAL_UnLock(&(hCryp->lock));
		}
	}

	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in DES encryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DES_Encrypt(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint32_t size, uint8_t * pCypherData)
{
	HAL_Status_T ret = HAL_OK;

	if ((hCryp == NULL) || (pPlainData == NULL) || (pCypherData == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_READY)
	{
		if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
			return HAL_BUSY;

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pPlainData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pCypherData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			if (hCryp->init->enableDMA == TRUE)
			{
				CRYP_DMAInit(hCryp);
			}
			
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_DES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_DES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		HAL_CRYP_ENABLE(hCryp);
		HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_DES_GETKEY_Msk);

		if (hCryp->init->enableDMA == FALSE)
		{
			/* enable CRYP interrupt */
			HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
			//NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
			//NVIC_EnableIRQ(CRYP_IRQn);
			HAL_RequestIrq(CRYP_IRQn, CRYP_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_CRYP_IRQHandler, &tmpHandle/*(const void *)hTrng*/);
			HAL_IRQ_ENABLE(CRYP_IRQn);

			HAL_UnLock(&(hCryp->lock));
		}
		else
		{
			//DMA mode
			/* enable Cryp DMA TX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMATXLVL_Msk, (2<<CRYP_DMACR_DMATXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
			HAL_DMA_Start(hCryp->hdmain, (uint32_t)hCryp->pCrypInBuffPtr, (uint32_t)&(hCryp->Instance->ES_DIN), (hCryp->crypInCount / 4));
			
			/* enable Cryp DMA RX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMARXLVL_Msk, (2<<CRYP_DMACR_DMARXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
			HAL_DMA_Start(hCryp->hdmaout, (uint32_t)&(hCryp->Instance->ES_DOUT), (uint32_t)hCryp->pCrypOutBuffPtr, (hCryp->crypOutCount / 4));
			HAL_UnLock(&(hCryp->lock));
		}
	}
	
	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in DES decryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the cyphertext buffer
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DES_Decrypt(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint32_t size, uint8_t * pPlainData)
{
	HAL_Status_T ret = HAL_OK;

	if ((hCryp == NULL) || (pPlainData == NULL) || (pCypherData == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_READY)
	{
		if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
			return HAL_BUSY;

		HAL_CRYP_DISABLE(hCryp);
		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pCypherData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pPlainData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			if (hCryp->init->enableDMA == TRUE)
			{
				CRYP_DMAInit(hCryp);
			}
			
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_DES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_DES, hCryp->init->algoMode, CRYP_CR_ALGODIR_DEC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}
		
		HAL_CRYP_ENABLE(hCryp);
		HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_DES_GETKEY_Msk);

		if (hCryp->init->enableDMA == FALSE)
		{
			/* enable CRYP interrupt */
			HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
			//NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
			//NVIC_EnableIRQ(CRYP_IRQn);
			HAL_RequestIrq(CRYP_IRQn, CRYP_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_CRYP_IRQHandler, &tmpHandle/*(const void *)hCryp*/);
			HAL_IRQ_ENABLE(CRYP_IRQn);
			HAL_UnLock(&(hCryp->lock));
		}
		else
		{
			//DMA mode
			/* enable Cryp DMA TX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMATXLVL_Msk, (2<<CRYP_DMACR_DMATXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
			HAL_DMA_Start(hCryp->hdmain, (uint32_t)hCryp->pCrypInBuffPtr, (uint32_t)&(hCryp->Instance->ES_DIN), (hCryp->crypInCount / 4));
			
			/* enable Cryp DMA RX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMARXLVL_Msk, (2<<CRYP_DMACR_DMARXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
			HAL_DMA_Start(hCryp->hdmaout, (uint32_t)&(hCryp->Instance->ES_DOUT), (uint32_t)hCryp->pCrypOutBuffPtr, (hCryp->crypOutCount / 4));
			HAL_UnLock(&(hCryp->lock));
		}
	}
	
	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in TDES encryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_TDES_Encrypt(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint32_t size, uint8_t * pCypherData)
{
	HAL_Status_T ret = HAL_OK;

	if ((hCryp == NULL) || (pPlainData == NULL) || (pCypherData == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_READY)
	{
		if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
			return HAL_BUSY;

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pPlainData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pCypherData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			if (hCryp->init->enableDMA == TRUE)
			{
				CRYP_DMAInit(hCryp);
			}
			
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_TDES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_TDES, hCryp->init->algoMode, CRYP_CR_ALGODIR_ENC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		HAL_CRYP_ENABLE(hCryp);
		HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);

		if (hCryp->init->enableDMA == FALSE)
		{
			/* enable CRYP interrupt */
			HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
			//NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
			//NVIC_EnableIRQ(CRYP_IRQn);
			HAL_RequestIrq(CRYP_IRQn, CRYP_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_CRYP_IRQHandler, &tmpHandle/*(const void *)hTrng*/);
			HAL_IRQ_ENABLE(CRYP_IRQn);

			HAL_UnLock(&(hCryp->lock));
		}
		else
		{
			//DMA mode
			/* enable Cryp DMA TX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMATXLVL_Msk, (2<<CRYP_DMACR_DMATXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
			HAL_DMA_Start(hCryp->hdmain, (uint32_t)hCryp->pCrypInBuffPtr, (uint32_t)&(hCryp->Instance->ES_DIN), (hCryp->crypInCount / 4));
			
			/* enable Cryp DMA RX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMARXLVL_Msk, (2<<CRYP_DMACR_DMARXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
			HAL_DMA_Start(hCryp->hdmaout, (uint32_t)&(hCryp->Instance->ES_DOUT), (uint32_t)hCryp->pCrypOutBuffPtr, (hCryp->crypOutCount / 4));
			HAL_UnLock(&(hCryp->lock));
		}
	}
	
	return ret;
}


/**
  * @brief  Initializes the CRYP peripheral in TDES decryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the cyphertext buffer
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_TDES_Decrypt(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint32_t size, uint8_t * pPlainData)
{
	HAL_Status_T ret = HAL_OK;

	if ((hCryp == NULL) || (pPlainData == NULL) || (pCypherData == NULL))
	{
		PR_ERR("CRYP parameter invalid.\n");
		return HAL_ERROR;
	}

	if (hCryp->state == CRYP_STATE_READY)
	{
		if (HAL_TryLock(&(hCryp->lock)) == RET_BUSY)
			return HAL_BUSY;

		hCryp->state = CRYP_STATE_BUSY;
		hCryp->crypInCount = size;
		hCryp->pCrypInBuffPtr = pCypherData;
		hCryp->crypOutCount = size;
		hCryp->pCrypOutBuffPtr = pPlainData;

		if (hCryp->phase == CRYP_PHASE_READY)
		{
			if (hCryp->init->enableDMA == TRUE)
			{
				CRYP_DMAInit(hCryp);
			}
			
			/* set key, mode, IV and flush fifo */
			CRYP_SetKey(hCryp, hCryp->init->pKey, hCryp->init->keySize, CRYP_ALGO_TDES);
			CRYP_SetInitVector(hCryp, hCryp->init->pInitVect, hCryp->init->IVSize);
			HAL_CRYP_SET_MODE(hCryp, CRYP_CR_ALGOSEL_TDES, hCryp->init->algoMode, CRYP_CR_ALGODIR_DEC);
			HAL_CRYP_FLUSH_FIFO(hCryp);

			hCryp->phase = CRYP_PHASE_PROCESS;
		}

		HAL_CRYP_ENABLE(hCryp);
		HAL_CRYP_READ_KEY_REQUEST(hCryp, CRYP_SKCS_AES_GETKEY_Msk);

		if (hCryp->init->enableDMA == FALSE)
		{
			/* enable CRYP interrupt */
			HAL_CRYP_ENABLE_IT(hCryp, CRYP_IT_INFIFO | CRYP_IT_OUTFIFO);
			//NVIC_SetPriority(CRYP_IRQn, hCryp->init->priority);
			//NVIC_EnableIRQ(CRYP_IRQn);
			HAL_RequestIrq(CRYP_IRQn, CRYP_IRQ_PRIORITY/*hCryp->init->priority*/, HAL_CRYP_IRQHandler, &tmpHandle/*(const void *)hTrng*/);
			HAL_IRQ_ENABLE(CRYP_IRQn);

			HAL_UnLock(&(hCryp->lock));
		}
		else
		{
			//DMA mode
			/* enable Cryp DMA TX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMATXLVL_Msk, (2<<CRYP_DMACR_DMATXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DIEN);
			HAL_DMA_Start(hCryp->hdmain, (uint32_t)hCryp->pCrypInBuffPtr, (uint32_t)&(hCryp->Instance->ES_DIN), (hCryp->crypInCount / 4));
			
			/* enable Cryp DMA RX mode */
			IO_BITMASK_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DMARXLVL_Msk, (2<<CRYP_DMACR_DMARXLVL_Pos));
			IO_BIT_SET(hCryp->Instance->ES_DMACR, CRYP_DMACR_DOEN);
			HAL_DMA_Start(hCryp->hdmaout, (uint32_t)&(hCryp->Instance->ES_DOUT), (uint32_t)hCryp->pCrypOutBuffPtr, (hCryp->crypOutCount / 4));
			HAL_UnLock(&(hCryp->lock));
		}
	}
	
	return ret;
}



//#endif
