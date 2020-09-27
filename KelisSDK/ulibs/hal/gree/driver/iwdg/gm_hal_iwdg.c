#include<gm_soc.h>

/* Includes ------------------------------------------------------------------*/
#include<gm_hal_iwdg.h>
#include"gm_ll_iwdg.h"
#include<gm_hal_reset.h>




/** @addtogroup IWDG
  * @brief IWDG HAL module driver.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup IWDG_Private_Defines IWDG Private Defines
  * @{
  */
/* Status register need 5 RC LSI divided by prescaler clock to be updated. With 
   higher prescaler (256), and according to HSI variation, we need to wait at
   least 6 cycles so 48 ms. */
#define HAL_IWDG_DEFAULT_TIMEOUT            48U
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/**
  * @brief  Reset bakcup domain
  * @param  void
  * @retval null
  */
void HAL_IWDG_BKD_SW_Reset(void)
{
	HAL_Reset_Module(RESET_BKD_SW_RSTJ);
}

/**
  * @brief  Initialize the IWDG according to the specified parameters in the
  *         IWDG_InitTypeDef and start watchdog. Before exiting function,
  *         watchdog is refreshed in order to have correct time base.
  * @param  hiwdg  pointer to a IWDG_HandleTypeDef structure that contains
  *                the configuration information for the specified IWDG module.
  * @retval HAL status
  */
RET_CODE HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg)
{
  IWDG_TypeDef *pIWDG = hiwdg->Instance;
  
  /* Check the IWDG handle allocation */
  if(pIWDG == NULL)
  {
    return RET_ERROR;
  }
  /* Enable write access to IWDG_PR and IWDG_RLR registers by writing 0x5555 in KR */
  LL_IWDG_EnableWriteAccess(pIWDG);
  
  /* Write to IWDG registers the Prescaler & Reload values to work with */
  LL_IWDG_SetPrescaler(pIWDG,hiwdg->Init.Prescaler);
  LL_IWDG_SetReloadCounter(pIWDG,hiwdg->Init.Reload);

  LL_IWDG_DisableWriteAccess(pIWDG);
  /* Return function status */
  return RET_OK;
}

/**
  * @}
  */


/**
  * @brief  Enable the IWDG peripheral.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
RET_CODE HAL_IWDG_START(IWDG_HandleTypeDef *hiwdg)
{
    IWDG_TypeDef *pIWDG = hiwdg->Instance;

    if(pIWDG == NULL)
    {
        return RET_ERROR;
    }

    LL_IWDG_Enable(pIWDG);
    return RET_OK;
}

/**
  * @brief  Reload IWDG counter with value defined in the reload register
  *         (write access to IWDG_PR & IWDG_RLR registers disabled).
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
RET_CODE HAL_IWDG_RELOAD_COUNTER(IWDG_HandleTypeDef *hiwdg)
{
    IWDG_TypeDef *pIWDG = hiwdg->Instance;

    if(pIWDG == NULL)
    {
        return RET_ERROR;
    }

    LL_IWDG_ReloadCounter(hiwdg->Instance);
    return RET_OK;

}

/**
  * @brief  Refresh the IWDG.
  * @param  hiwdg  pointer to a IWDG_HandleTypeDef structure that contains
  *                the configuration information for the specified IWDG module.
  * @retval HAL status
  */
RET_CODE HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg)
{
  /* Reload IWDG counter with value defined in the reload register */
   HAL_IWDG_RELOAD_COUNTER(hiwdg);

  /* Return function status */
  return RET_OK;
}



/**
  * @brief  Get the selected prescaler of the IWDG
  * @param  IWDGx IWDG Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_IWDG_PRESCALER_4
  *         @arg @ref LL_IWDG_PRESCALER_8
  *         @arg @ref LL_IWDG_PRESCALER_16
  *         @arg @ref LL_IWDG_PRESCALER_32
  *         @arg @ref LL_IWDG_PRESCALER_64
  *         @arg @ref LL_IWDG_PRESCALER_128
  *         @arg @ref LL_IWDG_PRESCALER_256
  */

uint32_t HAL_IWDG_GetPrescaler(IWDG_HandleTypeDef *hiwdg)
{
	return ((uint32_t)LL_IWDG_GetPrescaler(hiwdg->Instance));		
}


/**
  * @brief  Get the specified IWDG down-counter reload value
  * @param  IWDGx IWDG Instance
  * @retval Value between Min_Data=0 and Max_Data=0x0FFF
  */

uint32_t HAL_IWDG_GetReloadCounter(IWDG_HandleTypeDef *hiwdg)
{
  return((uint32_t) LL_IWDG_GetReloadCounter(hiwdg->Instance));
}



/**
 * @brief  accord to divider value ,confirm to prescaler value
 * @param  uint32_t prescaler
 * @retval the actual value
 */
uint32_t HAL_IWDG_Get_Div_Value(uint32_t prescaler)
{
    uint32_t ret  = 0;

	switch(prescaler)
	{
		case HAL_IWDG_CLKDIV_FACTOR_4:
			ret = 4;
			break;
		case HAL_IWDG_CLKDIV_FACTOR_8:
			ret = 8;
			break;
		case HAL_IWDG_CLKDIV_FACTOR_16:
			ret = 16;
			break;
		case HAL_IWDG_CLKDIV_FACTOR_32:
			ret = 32;
			break;
		case HAL_IWDG_CLKDIV_FACTOR_64:
			ret = 64;
			break;
		case HAL_IWDG_CLKDIV_FACTOR_128:
			ret = 128;
			break;
		case HAL_IWDG_CLKDIV_FACTOR_256:
			ret = 256;
			break;
		case HAL_IWDG_CLKDIV_FACTOR256:
			ret = 256;
			break;
        default:
			ret = 128;
			break;

   }
   return ret;
}
	


/**
  * @}
  */


