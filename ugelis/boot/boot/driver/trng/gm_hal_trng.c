/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            trng.c
 *
 * @author        terry.wu 
 *
 * @version       1.0.0
 * 
 * @date           2018/03/26 
 *    
 * @brief           Implementation of trng Driver.
 * 
 * @note
 *    2018/03/26, terry.wu, V1.0.0
 *        Initial version.
 *    
 */



#include <gm_hal_trng.h>

//#ifdef CHIP_GM6721


#define TRNG_TIMEOUT_VALUE     5U

TRNG_Callback_T s_TrngCallback = NULL;
TRNG_Handle_T * tmpHandle;


//STATIC_ISR(TRNG_IRQn, ADC_IrqHandler);

/**
  * @brief  Initializes the TRNG peripheral and creates the associated handle.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_Init(TRNG_Handle_T * hTrng)
{
	uint32_t sampleCnt = 17;
	HAL_Status_T ret = HAL_OK;
	
	if (hTrng == NULL)
	{
		PR_DEBUG("TRNG parameter invalid.\n");
		return HAL_ERROR;
	}

	HAL_LOCK(hTrng);
	if (hTrng->state == TRNG_STATE_RESET)
	{
		hTrng->Instance = TRNG;
		
		/* set the clock for TRNG */
		

		/* reset TRNG */
		IO_BITMASK_SET(hTrng->Instance->RST, TRNG_RST_SOFTRESET_Msk, TRNG_RST_SOFTRESET);
		/* set RNG clock */
		IO_WRITE_REG(hTrng->Instance->SAMCNT, sampleCnt);
		while(IO_READ_REG(hTrng->Instance->SAMCNT) != sampleCnt)
		{
			IO_WRITE_REG(hTrng->Instance->SAMCNT, sampleCnt);
		}
		/* config TRNG, select the number of inverters */
		IO_BITMASK_SET(hTrng->Instance->CFG, TRNG_CFG_RNDSRC_Msk, TRNG_CFG_RNDSRC_SHORT);
		/* enable all hardware test blocks */
		IO_WRITE_REG(hTrng->Instance->DBG, 0x00000000/*0x0e*/);
	}
	
	hTrng->state = TRNG_STATE_BUSY;
	s_TrngCallback = hTrng->callback;
	HAL_TRNG_ENABLE(hTrng);
	hTrng->state = TRNG_STATE_READY;

	//test temp
	tmpHandle = hTrng;
	
	HAL_UNLOCK(hTrng);

	return ret;
}

/**
  * @brief  DeInitializes the TRNG peripheral. 
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_DeInit(TRNG_Handle_T * hTrng)
{
	if (hTrng == NULL)
	{
		PR_DEBUG("TRNG parameter invalid.\n");
		return HAL_ERROR;
	}

	HAL_LOCK(hTrng);
	HAL_TRNG_DISABLE(hTrng);
	IO_BITS_SET(hTrng->Instance->IMR, TRNG_IMR_EHRVALID_INT | TRNG_IMR_AUTOCORRERR_INT \
		| TRNG_IMR_CRNGTERR_INT | TRNG_IMR_CRNGTERR_INT);
	IO_BITS_SET(hTrng->Instance->ICR, TRNG_ICR_EHRVALID_CLR | TRNG_ICR_AUTOCORRERR_CLR \
		| TRNG_ICR_AUTOCORRERR_CLR | TRNG_ICR_VNERR_CLR);
	HAL_IRQ_DISABLE(TRNG_IRQn);
	hTrng->state = TRNG_STATE_RESET;
	HAL_UNLOCK(hTrng);
	
	return HAL_OK;
}


/**
  * @brief  Wait random number available 
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *         the configuration information for TRNG module
  * @param  timeout: the maximum time to wait
  * @retval HAL_STATUS
  */
static HAL_Status_T TRNG_WaitEHRValidTimeout(TRNG_Handle_T * hTrng, uint32_t timeout)
{
	uint32_t cnt = timeout;
	uint32_t valid = 0;

	valid = IO_READ_REG(hTrng->Instance->VALID);
	while ((valid & 0x3) == 0x0)
	{
		valid = IO_READ_REG(hTrng->Instance->ISR);

		HAL_DelayMs(1);
		if (--cnt == 0)
		{
			hTrng->state = TRNG_STATE_TIMEOUT;
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}


/**
  * @brief  check the status of TRNG engine
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval data valid or error status
  */
static uint32_t TRNG_CheckISR(TRNG_Handle_T * hTrng)
{
	uint32_t ret = HAL_OK;;
	uint32_t status = 0;

	status = IO_READ_REG(hTrng->Instance->ISR);
	if (status & TRNG_ISR_EHRVALID)
	{
		ret = HAL_OK;
	}
	if (status & TRNG_ISR_AUTOCORRERR)
	{
		ret = TRNG_ISR_AUTOCORRERR;
		IO_BITS_SET(hTrng->Instance->ICR, TRNG_ICR_AUTOCORRERR_CLR);
	}
	if (status & TRNG_ISR_CRNGTERR)
	{
		ret = TRNG_ISR_CRNGTERR;
		IO_BITS_SET(hTrng->Instance->ICR, TRNG_ICR_CRNGTERR_CLR);
	}
	if (status & TRNG_ISR_VNERR)
	{
		ret = TRNG_ISR_VNERR;
		IO_BITS_SET(hTrng->Instance->ICR, TRNG_ICR_VNERR_CLR);
	}

	if (ret != HAL_OK)
	{
		hTrng->state = TRNG_STATE_ERROR;
		/* reset TRNG */
		IO_BITMASK_SET(hTrng->Instance->RST, TRNG_RST_SOFTRESET_Msk, TRNG_RST_SOFTRESET);
	}

	return ret;
}


/**
  * @brief  Generates  random number, maximum size is 192bit.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @param  random: pointer to generated random number if successful.
  * @param  size: the len of random to get
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_GenerateRandomNumber(TRNG_Handle_T * hTrng, uint32_t * random, uint8_t size)
{
	uint8_t i = 0;
	uint32_t status = 0;
	HAL_Status_T ret = HAL_OK;
	uint32_t * temp = random;
	uint8_t len = ((size > 6) ? 6 : size);

	HAL_LOCK(hTrng);
	if (hTrng->state == TRNG_STATE_READY)
	{
		hTrng->state = TRNG_STATE_BUSY;

		ret = TRNG_WaitEHRValidTimeout(hTrng, TRNG_TIMEOUT_VALUE);
		if (ret != HAL_OK)
		{
			PR_DEBUG("%s: TRNG generate random number timeout.\n", __FUNCTION__);
			ret = HAL_TIMEOUT;
			goto EXIT;
		}

		/* check error status */
		ret = TRNG_CheckISR(hTrng);
		if (ret != HAL_OK)
		{
			PR_DEBUG("%s: TRNG generate random number error, ret = 0x%x.\n", __FUNCTION__, ret);
			ret = HAL_ERROR;
			goto EXIT;
		}
		else
		{
			for (i = 0; i < len; i++)
			{
				hTrng->randomNumber[i] = hTrng->Instance->DATA[i];
				*temp = hTrng->randomNumber[i];
				temp++;
			}

			IO_BITS_SET(hTrng->Instance->ICR, TRNG_ICR_EHRVALID_CLR);
		}
		
		hTrng->size = len;
		hTrng->state = TRNG_STATE_READY;

		IO_BITMASK_SET(hTrng->Instance->RST_BITS_CNT, TRNG_RST_BITS_CNT_Msk, TRNG_RST_BITS_CNT_Msk);
		
		// another read to reset the bits-counter
		//status = IO_READ_REG(hTrng->Instance->ISR);
	}
	else
	{
		ret = HAL_ERROR;
	}

EXIT:
	
	hTrng->state = TRNG_STATE_READY;
	HAL_UNLOCK(hTrng);

	return ret;
}


/**
  * @brief  process TRNG interrupt
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @none
  */
void HAL_TRNG_IRQHandler(void ** handle/*TRNG_Handle_T * htrng*/)
{
	uint8_t i = 0;
	HAL_Status_T ret = HAL_OK;
	//temparaly for IRQ
	TRNG_Handle_T * hTrng = (TRNG_Handle_T *)(*handle);

	HAL_TRNG_DISABLE_IT(hTrng, (TRNG_IMR_EHRVALID_INT | TRNG_IMR_AUTOCORRERR_INT \
		| TRNG_IMR_AUTOCORRERR_INT | TRNG_IMR_VNERR_INT));
	HAL_IRQ_DISABLE(TRNG_IRQn);
	ret = TRNG_CheckISR(hTrng);
	if (ret == HAL_OK)
	{
		hTrng->size = 6;
		for (i = 0; i < 6; i++)
		{
			hTrng->randomNumber[i] = hTrng->Instance->DATA[i];
		}

		s_TrngCallback(hTrng, TRNG_EVENT_DATA_VALID, 0);
	}
	else
	{
		s_TrngCallback(hTrng, TRNG_EVENT_ERROR, ret);
	}

	hTrng->state = TRNG_STATE_READY;
}


/**
  * @brief  Generates  random number, maximum size is 192bit with interrupt mode.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_GenerateRandomNumber_IT(TRNG_Handle_T * hTrng)
{
	HAL_Status_T ret = HAL_OK;

	HAL_LOCK(hTrng);
	if (hTrng->state == TRNG_STATE_READY)
	{
		hTrng->state = TRNG_STATE_BUSY;
		HAL_TRNG_ENABLE_IT(hTrng, (TRNG_IMR_EHRVALID_INT | TRNG_IMR_AUTOCORRERR_INT \
			| TRNG_IMR_CRNGTERR_INT | TRNG_IMR_VNERR_INT));
		//NVIC_SetPriority(TRNG_IRQn ,hTrng->priority);
		//NVIC_EnableIRQ(TRNG_IRQn);
		//temparaly for IRQ
		HAL_RequestIrq(TRNG_IRQn, TRNG_IRQ_PRIORITY/*hTrng->priority*/, HAL_TRNG_IRQHandler, &tmpHandle/*(const void *)hTrng*/);
		HAL_IRQ_ENABLE(TRNG_IRQn);
	}
	else
	{
		ret = HAL_ERROR;
	}

	HAL_UNLOCK(hTrng);
	return ret;
}


/**
  * @brief  Read latest generated random number. 
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @param  random: pointer to generated random number variable if successful.
  * @param  size: the len of random to get
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_ReadLastRandomNumber(TRNG_Handle_T * hTrng, uint32_t * random, uint8_t size)
{
	uint8_t i = 0;
	uint32_t * temp = random;
	uint8_t len = ((size > 6) ? 6 : size);

	for (i = 0; i < len; i++)
	{
		*temp = hTrng->randomNumber[i];
		temp++;
	}

	return HAL_OK;
}



//#endif
