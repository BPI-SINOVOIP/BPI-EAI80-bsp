 /**
  *
  * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
  *
  * @file		 gm_hal_acmp.c
  *
  * @author 	 Richard.Liu
  *
  * @version	 v1.0.0
  *
  * @date		 2019/01/24
  *
  * @brief		 ACMP Source file
  *
  * @note		 
  * 			 2019/01/24, Richard.Liu, v1.0.0
  * 				 Initial version.
  * 		 
  **/

#include <gm_hal_acmp.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>


#ifdef DEBUG_PRINTF
#undef DEBUG_PRINTF
#endif

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


ACMP_Handle_T *g_acmp_handle = NULL;

/**
  * @brief  The following  function are used to  ACMP  Irq handler.
  * @note  software has to enable peripheral interrput  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_ACMP_InterruptHandler(void)
{
	PR_DEBUG("Enter fun[%s]\n", __FUNCTION__);
	if(g_acmp_handle != NULL)
	{
		ACMP_CLEAR_INTERRUPT(g_acmp_handle->device);
	}
}

/**
  * @brief  Initializes the ACMP according to the specified
  *         parameters in the ACMP_Config_T.
  * @param  pConfig: pointer to a ACMP_Handle_T structure. 
  * @param config: pointer to a ACMP_Config_T structure that contains
  *         the configuration information for ACMP module
  * @retval void
  */
static void ACMP_InterruptConfig(ACMP_Handle_T *pHandle, ACMP_Config_T *pConfig)
{

    COMPARE_MATCH_TRIGGER(pHandle->device, pConfig->triggerMode);
    ACMP_INTERRUPT_ENABLE(pHandle->device);

}

/**
  * @brief  Get the ACMP Compare Result according to the specified
  *         parameters in the ACMP_Handle_T.
  * @param  pHandle: pointer to a ACMP_Handle_T structure. 
  * @retval HAL Compare Result
  */
ACMP_CompareResult_T HAL_ACMP_GetCompareRestult(ACMP_Handle_T *pHandle)
{
    ACMP_CompareResult_T ret = ACMP_LOW;

    /* get output compare level*/
    if (IO_BIT_GET(pHandle->device->CS, ACMP_CS_OPTVAL))
    {
        /*ositivePin voltage is more than negativePin voltage */
        ret = ACMP_HIHG;
    }
    else
    {
        /*positivePin voltage is no more than negativePin voltage */
        ret = ACMP_LOW;
    }

    return ret;
}

/**
  * @brief  The following  function are used to  ACMP peripheral Request Irq.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_ACMP_RequestIrq(void)
{
	HAL_RequestIrq(ACMP_IRQn, ACMP_IRQ_PRIORITY, HAL_ACMP_InterruptHandler, 0);
    HAL_IRQ_ENABLE(ACMP_IRQn);
}

/**
  * @brief  The following  function are used to reset ACMP peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_ACMP_SW_Reset(void)
{
	HAL_Reset_Module(RESET_ACMP_SW_RSTJ);
}

/**
  * @brief  The following two functions are used to enable/disable peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application 
  * 		software has to enable clock before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_ACMP_CLK_ENABLE(void)
{
	HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ACMP);
}

void HAL_ACMP_CLK_DISABLE(void)
{
	HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ACMP);
}

/**
  * @brief  Initializes the ACMP according to the specified
  *         parameters in the ACMP_Config_T.
  * @param  pConfig: pointer to a ACMP_Handle_T structure. 
  * @param config: pointer to a ACMP_Config_T structure that contains
  *         the configuration information for ACMP module
  * @retval HAL status
  */
RET_CODE HAL_ACMP_Init(ACMP_Handle_T *pHandle, ACMP_Device_T *pACMP, ACMP_Config_T *pConfig)
{
    RET_CODE ret = RET_OK;

    if (!(pHandle && pACMP && pConfig))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    pHandle->device = pACMP;

	g_acmp_handle = pHandle;

	HAL_ACMP_CLK_ENABLE();
	HAL_ACMP_SW_Reset();

    ACMP_DISABLE(pHandle->device);

    if(pConfig->outputPinEnable == ENABLE)
    {
        OUTPUTPIN_ENABLE(pACMP);
    }
    else
    {
        OUTPUTPIN_DISABLE(pACMP);
    }

    if(pConfig->outputInversionEnable == ENABLE)
    {
        ACMP_OUTPUT_INVERSION_ENABLE(pACMP);
    }
    else
    {
        ACMP_OUTPUT_INVERSION_DISABLE(pACMP);
    }

	ACMP_AC_EVT_OUT_ENABLE(pACMP);
	
    /*config ac output filer thr*/
    ACMP_CONFIG_FILTER_THR(pACMP, pConfig->filterThr);

    SET_NEGATIVE_PIN(pACMP, pConfig->negativePin);
    SET_POSITIVE_PIN(pACMP, pConfig->positivePin);

	if (pConfig->blankTimesel > 0 && pConfig->blankTimesel < 6)
		SET_BLANKING_TIM(pACMP, pConfig->blankTimesel);

	if (pConfig->achyst_sel >= ACHYST_SEL_0  && pConfig->achyst_sel <= ACHYST_SEL_3)
		SET_ACHYST_SEL(pACMP, pConfig->achyst_sel);

	ACMP_ENABLE(pACMP);
    /*
     *config irq match trigger mode
     *enable acmp irq and nvic for acmp
     *regist acmp event callback
     */
    ACMP_InterruptConfig(pHandle, pConfig);

	HAL_ACMP_RequestIrq();
#ifdef RABBIT_NO_OS_SYSTEM
	NVIC_EnableIRQ(ACMP_IRQn);
#endif

out:
    return ret;
}

RET_CODE HAL_ACMP_UnInit(ACMP_Handle_T *pHandle, ACMP_Config_T *pConfig)
{
    RET_CODE ret = RET_OK;
	ACMP_Device_T *pACMP;

    if (!(pHandle  && pConfig))
    {
        ret = RET_INVPARAM;
        goto out;
    }

	pACMP = pHandle->device;

    ACMP_DISABLE(pHandle->device);
    OUTPUTPIN_DISABLE(pACMP);
    ACMP_OUTPUT_INVERSION_DISABLE(pACMP);
	ACMP_AC_EVT_OUT_DISABLE(pACMP);
	
    /*config ac output filer thr*/
    ACMP_CONFIG_FILTER_THR(pACMP, pConfig->filterThr);

    SET_NEGATIVE_PIN(pACMP, pConfig->negativePin);
    SET_POSITIVE_PIN(pACMP, pConfig->positivePin);

	if (pConfig->blankTimesel > 0 && pConfig->blankTimesel < 6)
		SET_BLANKING_TIM(pACMP, pConfig->blankTimesel);

	if (pConfig->achyst_sel >= ACHYST_SEL_0  && pConfig->achyst_sel <= ACHYST_SEL_3)
		SET_ACHYST_SEL(pACMP, pConfig->achyst_sel);

    /*
     *config irq match trigger mode
     *enable acmp irq and nvic for acmp
     *regist acmp event callback
     */

	ACMP_INTERRUPT_DISABLE(pHandle->device);
	HAL_ACMP_CLK_DISABLE();
	
#ifdef RABBIT_NO_OS_SYSTEM
	NVIC_DisableIRQ(ACMP_IRQn);
#endif

out:
    return ret;
}

