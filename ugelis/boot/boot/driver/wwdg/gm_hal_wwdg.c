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


//WDOG_Callback_T pWdogCallback = NULL;

void HAL_WDOG_IRQHandler(WDOG_Handle_T *hWDOG)
{

    HAL_WDOG_IntClear(hWDOG);

    /*notice usr todo some action before chip reset*/
//    if (pWdogCallback != NULL)
//    {
//        pWdogCallback(hWDOG, WDOG_EVENT_REOOT_WARNING, 0);
//    }
//    //WDOG_PR_ERR("\rthen return mainloop\n");

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

static RET_CODE HAL_WDOG_SetClkSrc(uint32_t clk_divFactor)
{
    RET_CODE ret = RET_OK;

	#define CLK_ENREG       (*(volatile uint32_t *)(0x40000088))
    #define WDOG_CLKEN_BIT     28   
	CLK_ENREG |=(1 << WDOG_CLKEN_BIT);
	
    #define WDOG_CLKDIV_REG    (*(volatile uint32_t *)(0x40000094))
    #define WDOG_CLKDIV_BIT     4   
	WDOG_CLKDIV_REG |= (clk_divFactor << WDOG_CLKDIV_BIT);
	
	
#if 0	
    PeriphClk_Config_T config;

    config.periphClkMask = CLOCK_PERIPH_WDT_MASK;
    config.wdtClock = clkSrc;
    ret = PeriphClk_SetConfig(&config);
#endif
    return ret;

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
//    pWdogCallback = pConfig->callBack;
//    NVIC_EnableIRQ(WDOG_IRQn);

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




