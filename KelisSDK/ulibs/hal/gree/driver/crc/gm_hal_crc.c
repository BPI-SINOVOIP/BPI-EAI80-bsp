/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          CRC.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 * 
 * @date          2016/04/01 
 *    
 * @brief         Header file of CRC driver.
 * 
 * @note
 *    2016/04/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/05, Mike.Zheng, V1.1.0
 *        Update coding style.
 */
 
#include <gm_hal_crc.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>


#ifndef DEBUG_PRINTF
	#define PR_DEBUG(...)		do{}while(0)
#else
	#define PR_DEBUG			printk
#endif

#ifndef ERR_PRINTF
	#define PR_ERR(...) 	   do{}while(0)
#else
	#define PR_ERR			   printk
#endif

#ifndef WARNING_PRINTF
	#define PR_WARNINR(...)    do{}while(0)
#else
	#define PR_WARNINR		   printk
#endif

static CRC_Handle_T *s_pHandle = NULL;

//static RET_CODE HAL_CRC_WaitCalcFinish(CRC_Device_T *pCRC, int32_t timeoutUs);
static uint32_t HAL_CRC_Handle8(CRC_Device_T *pCRC, uint8_t *pBuffer, uint32_t length); 
static uint32_t HAL_CRC_Handle16(CRC_Device_T *pCRC, uint16_t *pBuffer, uint32_t length);
static uint32_t HAL_CRC_Handle32(CRC_Device_T *pCRC, uint32_t *pBuffer, uint32_t length);


static RET_CODE HAL_CRC_WaitCalcFinish(CRC_Device_T *pCRC, int32_t timeoutUs)
{
	uint32_t cnt = timeoutUs;
	uint32_t valid = 0;
	
	if(pCRC == NULL)
	{
		return RET_INVPARAM;
	}

	valid = IO_READ_REG(pCRC->ST);
	while((valid & CRC_ST_DNE) == 0x0)
	{
		HAL_DelayUs(1);
		if(--cnt == 0)
		{
			return RET_TIMEOUT;
		}
		valid = IO_READ_REG(pCRC->ST);
	}

	return RET_OK;
}



RET_CODE HAL_CRC_Init(CRC_Device_T *pCRC, CRC_Handle_T *hCRC)
{
	RET_CODE ret = RET_OK;

    if (!(pCRC  && hCRC))
    {
    	PR_ERR("\r%s: input parameter error\r\n",__FUNCTION__);
        ret = RET_INVPARAM;
        return ret;
    }
    /* Crc module reset and enable clock */
    HAL_Reset_Module(RESET_CRC_SW_RSTJ);
	HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_CRC);
    /* Remove Compler Warning */
    s_pHandle = s_pHandle;
    /* Remember Usr Define Handle */
    s_pHandle = hCRC;
    hCRC->device = pCRC;
    LL_XORVAL_ENABLE(pCRC);
    LL_INTERRUPT_DISABLE(pCRC);
//	HAL_RequestIrq(CRC_IRQn, CRC_IRQ_PRIORITY, HAL_CRC_IRQHandler, &tmpHandle/*(const void *)hTrng*/);
//	HAL_IRQ_ENABLE(CRC_IRQn);
    return ret; 
}

/**             
  * @brief  Enter 8-bit inputdata to the CRC calculator.
  *         Specific data handling to optimize processing time.  
  * @param  pCRC: CRC device point
  * @param  pBuffer: pointer to the input data buffer
  * @param  length: input data buffer length(byte length)
  * @retval uint32_t CRC 
*/
static uint32_t HAL_CRC_Handle8(CRC_Device_T *pCRC, uint8_t *pBuffer, uint32_t length)
{
    uint32_t i = 0; /* input data buffer index */

    /* Processing time optimization: 4 bytes are entered in a row with a single word write,
     * last bytes must be carefully fed to the CRC calculator to ensure a correct type
     * handling by the IP
     */

    if (!(pCRC  && pBuffer))
    {
    	PR_ERR("\r%s: input parameter error\r\n",__FUNCTION__);
        return RET_INVPARAM;
    }
	
    /* calc 32bit at one time */
    LL_SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_32B);
    for (i = 0; i < (length / 4); i++)
    {
        pCRC->DA = (((uint32_t)pBuffer[4*i]<<24)      |\
                      ((uint32_t)pBuffer[4*i+1]<<16)    |\
                      ((uint32_t)pBuffer[4*i+2]<<8)     |\
                      ((uint32_t)pBuffer[4*i+3]));
    }

    /* last bytes specific handling according different byte rest */
    if (length % 4)
    {
        /* calc 8bit at one time */
        if (length % 4 == 1)
        {
            LL_SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_8B); 
            pCRC->DA = pBuffer[4*i];
        }
        /* calc 16bit at one time */ 
        else if (length % 4 == 2)
        {
            LL_SET_DATA_PROCESS_BITWIDTH(pCRC,DATA_PROCESS_WIDTH_16B); 
            pCRC->DA = (pBuffer[4*i] << 8) | (pBuffer[4*i+1]);
        }
        else if (length % 4 == 3)
        {
            /* calc 16bit at one time */ 
            LL_SET_DATA_PROCESS_BITWIDTH(pCRC,DATA_PROCESS_WIDTH_16B); 
            pCRC->DA = (pBuffer[4*i] << 8) | (pBuffer[4*i+1]);
            /* calc 8bit at one time */ 
            LL_SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_8B);
            pCRC->DA = pBuffer[4*i+2];       
        }
    }

    if(HAL_CRC_WaitCalcFinish(pCRC, CRC_ACCUMULATE_TIEMOUT_US) == RET_TIMEOUT)
    {
        PR_ERR("\r%s:CRC format words accumulate timeout\n",__FUNCTION__);
    }
    /* Return the CRC computed value */ 
    return pCRC->RST;
}

/**             
 * @brief  Enter 16-bit input data to the CRC calculator.
 *         Specific data handling to optimize processing time.  
 * @param  pCRC: CRC device point
 * @param  pBuffer: pointer to the input data buffer
 * @param  length: input data buffer length(half word length)
 * @retval uint32_t CRC 
*/  
static uint32_t HAL_CRC_Handle16(CRC_Device_T *pCRC, uint16_t *pBuffer, uint32_t length)
{
    uint32_t i = 0;  /* input data buffer index */
	
    if (!(pCRC  && pBuffer))
    {
        PR_ERR("\r%s: input par error\r\n",__FUNCTION__);
        return RET_INVPARAM;
    }
	
    if (length % 2)
    {
        PR_ERR("\rerr Buffer_Length %d INPUT_FORMAT_HALFWORDS Buffer_Length\n",length);
        return RET_INVPARAM;
    }
    
    /**
     * Processing time optimization:
     * 2 HalfWords are entered in a row with a single word write,
     * in case of odd length, last HalfWord must be carefully fed to
     * the CRC calculator to ensure correct type handling by the IP 
     */

    /*byte length convert to HalfWords length*/  
    length = length >> 1;
    /* calc 32bit at one time */
    for (i = 0; i < (length/2); i++)
    {
        LL_SET_DATA_PROCESS_BITWIDTH(pCRC,DATA_PROCESS_WIDTH_32B); 
        pCRC->DA = (pBuffer[2*i]<<16) | pBuffer[2*i+1];     
    }
    /* last bytes specific handling according different byte rest*/  
    if (length % 2)
    {
        /* calc 16bit at one time */ 
        LL_SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_16B); 
        pCRC->DA = pBuffer[2*i]; 
    }
    
    if (HAL_CRC_WaitCalcFinish(pCRC, CRC_ACCUMULATE_TIEMOUT_US) == RET_TIMEOUT)
    {
        PR_ERR("\r%s:CRC format words accumulate timeout\n",__FUNCTION__);
        return RET_TIMEOUT;
    }

    /* Return the CRC computed value */ 
    return pCRC->RST;	
}

/**             
  * @brief  Enter 32-bit inputdata to the CRC calculator. 
  * @param  pCRC: CRC device point
  * @param  pBuffer: pointer to the input data buffer
  * @param  length: input data buffer length(byte length)
  * @retval uint32_t CRC 
*/

static uint32_t HAL_CRC_Handle32(CRC_Device_T *pCRC, uint32_t *pBuffer, uint32_t length)
{
    uint32_t i = 0; /* input data buffer index */
	
    if (!(pCRC  && pBuffer))
    {
        PR_ERR("\r%s: input par error\r\n",__FUNCTION__);
        return RET_INVPARAM;
    }

    if (length % 4)
    {
        PR_ERR("\rerr Buffer_Length %d INPUT_FORMAT_WORDS Buffer_Length\n",length);
        return RET_INVPARAM;
    }

    LL_SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_32B);

    /*byte length convert to word length*/
    length = length >> 2;
    /**
     * Processing time optimization: 4 bytes are entered in a row with a single word write, 
     * calc 32bit at one time 
     */
    for (i = 0; i < length; i++)
    {
        pCRC->DA = pBuffer[i];
    }
    if (RET_TIMEOUT == HAL_CRC_WaitCalcFinish(pCRC, CRC_ACCUMULATE_TIEMOUT_US))
    {
        PR_ERR("\r%s:CRC format words accumulate timeout\n",__FUNCTION__);
        return RET_TIMEOUT;
    }
    
    /* Return the CRC computed value */ 
    return pCRC->RST;
}


void HAL_CRC_AccumulateConfig(CRC_Handle_T *hCRC, CRC_Config_T *pConfig)
{
    CRC_Device_T *pCRC = hCRC->device;

    hCRC->inputFormat = pConfig->inputFormat;
    LL_SET_INITVAL(pCRC, pConfig->initValue);
    LL_SET_POL_BITWIDTH(pCRC, pConfig->polyLength);
    LL_SET_POL(pCRC, pConfig->polynomial);
    LL_SET_XORVAl(pCRC, pConfig->xorValue);
    LL_SET_INPUTDATA_INVERSION(pCRC, pConfig->inputInversion);
    LL_SET_OUTPUTDATA_INVERSION(pCRC, pConfig->outputInversionEn);
}

uint32_t HAL_CRC_GetAccumulateResult(CRC_Handle_T *hCRC, uint8_t *pBuffer, uint32_t length)
{
    uint32_t temp = 0;  /* CRC calc result  */
	
    CRC_Device_T *pCRC = hCRC->device;
    /* get usr set inputFormat */
    CRC_InputFormat_T inputFormat = hCRC->inputFormat;
    /*load init data,ready for start calc*/
    LL_CALC_RESTART(pCRC);
    
    switch (inputFormat)
    {
    case INPUT_FORMAT_BYTES:         
        temp = HAL_CRC_Handle8(pCRC, (uint8_t*)pBuffer, length);
        break; 
     case INPUT_FORMAT_HALFWORDS: 
        temp = HAL_CRC_Handle16(pCRC, (uint16_t*)pBuffer, length);
        break;   
    case INPUT_FORMAT_WORDS:
        temp = HAL_CRC_Handle32(pCRC, (uint32_t*)pBuffer, length);
        break;

    default:
        
        PR_ERR("\r%s:%d inputdata format error\n",__FUNCTION__,__LINE__);
        temp = RET_INVPARAM;
        break;
	}

    return temp;
}




