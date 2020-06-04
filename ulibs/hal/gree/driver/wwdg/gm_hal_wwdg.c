/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_wwdg.c
 *
 * @author        kenny.cmt
 *
 * @version       1.0.0
 *
 * @date          2018/03/20
 *
 * @brief         the hal layer of wwdg driver.
 *
 * @note
 *    2018/03/20    kenny.cmt     V1.0
 *        Update For GM6721       V1.0.0
 */

#include<gm_hal_wwdg.h>
#include"gm_ll_wwdg.h"
#include <gm_hal_clock.h>



static WDOG_Handle_T *s_pHWdog = NULL;
void WDOG_IRQHandler(void);
WDOG_Callback_T pWdogCallback = NULL;


void HAL_WDOG_IRQHandler(void)
{
	HAL_WDOG_IntClear(s_pHWdog);

    /*notice usr todo some action before chip reset*/
    if (pWdogCallback != NULL)
    {
        pWdogCallback(s_pHWdog, WDOG_EVENT_REOOT_WARNING, 0);
    }

}

static RET_CODE HAL_WDOG_SetLoadVal(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig)
{
    RET_CODE ret = RET_OK ;

    LL_WDOG_UNLOCK(pWDOG);

    LL_WDOG_WAIT_WRITEDONE();
	if(pConfig->clkDivFactor > 400)
    {
        pWDOG->LD = (((SYS_CLOCK / (pConfig->clkDivFactor+1))*pConfig->resetTime)/MS_DIV);
	}
	else
	{
        pWDOG->LD = (((SYS_CLOCK/(pConfig->clkDivFactor+1)) / MS_DIV) *pConfig->resetTime);
	
	}

    LL_WDOG_LOCK(pWDOG);

    return ret;
}

static RET_CODE HAL_WDOG_SetWindowLoad(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig)
{
    RET_CODE ret = RET_OK ;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();

	if(pConfig->clkDivFactor > 400)
    {
        pWDOG->WL = (((SYS_CLOCK / (pConfig->clkDivFactor+1))*pConfig->windowTime)/MS_DIV);
	}
	else
	{
        pWDOG->WL = (((SYS_CLOCK/(pConfig->clkDivFactor+1)) / MS_DIV) *pConfig->windowTime);
	
	}

    LL_WDOG_LOCK(pWDOG);

    return ret;
}

static RET_CODE HAL_WDOG_SetClkSrc(HAL_Wwdg_Clkdiv clk_divFactor)
{
  RET_CODE ret = RET_OK;
	Peripheral_Config_T peripheClockSet;

	peripheClockSet.peripheMask = PERIPHERAL_WWDT_MASK;
	peripheClockSet.peripheClkConfig.wwdtClkConfig   = (PeripheClk_WWDT_Config_T)clk_divFactor;

  HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_WWDT);
  HAL_CLOCK_PeripheralClkSetConfig(&peripheClockSet);

  return ret;

}
static void HAL_WDOG_EnableIrq(void)
{
   	HAL_RequestIrq(WDOG_IRQn, 0, HAL_WDOG_IRQHandler, 0);
    HAL_IRQ_ENABLE(WDOG_IRQn);
}
RET_CODE HAL_WDOG_Init(WDOG_Device_T *pWDOG, WDOG_Config_T *pConfig, WDOG_Handle_T *hWDOG)
{
    RET_CODE ret = RET_OK;

    if ((pWDOG && pConfig && hWDOG) == NULL )
    {
        ret = RET_INVPARAM;
        goto out;
    }

    hWDOG->Instance = pWDOG;
	if(hWDOG->Instance == NULL)
	{
        ret = RET_INVPARAM;
        goto out;

	}
	
	s_pHWdog = hWDOG;
	if(s_pHWdog == NULL)
	{
		ret = RET_INVPARAM;
		goto out;
	}

    /* Set WDOG srcclk */
    if (HAL_WDOG_SetClkSrc(pConfig->clkDivFactor) != RET_OK)
    {
        ret = RET_ERROR;
        goto out;
    }

    HAL_WDOG_Disable(hWDOG);

    /* enable wdog to reset chip */
    if (pConfig->workEwiMode == WDOG_WINDOW_MODE)
    {
        if (HAL_WDOG_SetWindowLoad(pWDOG,pConfig) != RET_OK)
        {
            ret = RET_ERROR;
            goto out;
        }
        /* Set WDOG window mode */
        HAL_WDOG_SetWindowMode(hWDOG);
    }
    else if (pConfig->workEwiMode == WDOG_NORMAL_MODE)
    {
        /* Set WDOG normal mode */
        HAL_WDOG_SetNormalMode(hWDOG);
    }
    else if (pConfig->workEwiMode == WDOG_WINDOW_EWI)
    {
        if (HAL_WDOG_SetWindowLoad(pWDOG,pConfig) != RET_OK)
        {
            ret = RET_ERROR;
            goto out;
        }
		HAL_WDOG_SetWinLen(hWDOG, pConfig->winLen);
        /* Set WDOG window and EWI mode */
        HAL_WDOG_SetWindowMode_EWI(hWDOG);
    }
    else if (pConfig->workEwiMode == WDOG_NORMAL_EWI)
    {
        HAL_WDOG_SetWinLen(hWDOG, pConfig->winLen);
		/* Set WDOG normal and EWI mode */
        HAL_WDOG_SetNormalMode_EWI(hWDOG);
    }

    /* Set wdogclk and loadcnt according to usr set reboot time : second is the time uint */
    if (HAL_WDOG_SetLoadVal(pWDOG,pConfig) != RET_OK)
    {
        ret = RET_ERROR;
        goto out;
    }
    /* Enabel nvic WDOG irq and register irq callback */
    pWdogCallback = pConfig->callBack;
    if((pConfig->workEwiMode & WDOG_NORMAL_EWI) == WDOG_NORMAL_EWI )
    {
      HAL_WDOG_EnableIrq();
    }
out:
    return ret;
}



void HAL_WDOG_Enable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;
	
    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
    LL_WDOG_Enable(pWDOG);
    LL_WDOG_LOCK(pWDOG);
}

void HAL_WDOG_Disable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
    LL_WDOG_Disable(pWDOG);
    LL_WDOG_LOCK(pWDOG);
}


void HAL_WDOG_SetWinLen(WDOG_Handle_T *pHandle , uint8_t winLen)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
    LL_WDOG_SetWinLen(pWDOG,winLen);
    LL_WDOG_LOCK(pWDOG);
}


void HAL_WDOG_FeedWdog(WDOG_Handle_T* pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_WAIT_WRITEDONE();
    LL_WDOG_FeedWdog(pWDOG);

}

void HAL_WDOG_IntClear(WDOG_Handle_T* pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
    /* clear wdog irq which is equal to feedwdog */
    LL_WDOG_IntClr(pWDOG);
    LL_WDOG_LOCK(pWDOG);

}

static void HAL_WDOG_EWI_Enable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
    LL_WDOG_EWI_Enable(pWDOG);
    LL_WDOG_LOCK(pWDOG);
}
static void HAL_WDOG_EWI_Disable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG= pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
	LL_WDOG_EWI_Disable(pWDOG);
	LL_WDOG_LOCK(pWDOG);
}
static void HAL_WDOG_WinMode_Enable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
	LL_WDOG_WIN_Enable(pWDOG);
    LL_WDOG_LOCK(pWDOG);
}
static void HAL_WDOG_WinMode_Disable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_WAIT_WRITEDONE();
    LL_WDOG_WIN_Disable(pWDOG);
    LL_WDOG_LOCK(pWDOG);
}

void HAL_WDOG_SetWindowMode(WDOG_Handle_T *pHandle)
{
    HAL_WDOG_WinMode_Enable(pHandle);
    HAL_WDOG_EWI_Disable(pHandle);
}

void HAL_WDOG_SetWindowMode_EWI(WDOG_Handle_T *pHandle)
{
    HAL_WDOG_WinMode_Enable(pHandle);
    HAL_WDOG_EWI_Enable(pHandle);
}
void HAL_WDOG_SetNormalMode(WDOG_Handle_T *pHandle)
{
    HAL_WDOG_WinMode_Disable(pHandle);
    HAL_WDOG_EWI_Disable(pHandle);
}
void HAL_WDOG_SetNormalMode_EWI(WDOG_Handle_T *pHandle)
{
    HAL_WDOG_WinMode_Disable(pHandle);
    HAL_WDOG_EWI_Enable(pHandle);
}

RET_CODE HAL_WDOG_GetCountVal(WDOG_Handle_T *pHandle)
{
    uint32_t result_val = 0;
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    result_val = LL_WDOG_CountVal(pWDOG);
    LL_WDOG_LOCK(pWDOG);

    return result_val;
}

RET_CODE HAL_WDOG_GetCntOutWinFlag(WDOG_Handle_T *pHandle)
{
    uint32_t outFlag = 0;
    WDOG_Device_T *pWDOG = pHandle->Instance;

    LL_WDOG_UNLOCK(pWDOG);
    LL_WDOG_GetWinFlag(pWDOG);
    LL_WDOG_LOCK(pWDOG);

    return outFlag;
}




