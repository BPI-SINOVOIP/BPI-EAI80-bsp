/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_wwdg.h
 *
 * @author        kenny.cmt
 *
 * @version       1.0.0
 *
 * @date          2018/03/20
 *
 * @brief         Header file of wdog driver.
 *
 * @note
 *    2018/03/20    kenny.cmt     V1.0.0
 *        Update For GM6721

 */
#ifndef _GM_HAL_WWDG_H_
#define _GM_HAL_WWDG_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <gm_common.h>
#include <wwdg/gm_ll_wwdg_dev.h>

/**
 * @defgroup WDOG_work_mode
 * @{
 */
typedef enum
{
	WDOG_NORMAL_MODE = 0,                           /**<  WDOG work mode: normal */
	WDOG_WINDOW_MODE    ,                           /**<  WDOG work mode: windows */
	WDOG_NORMAL_EWI     ,                           /**<  WDOG work mode: normal and early warning interrupt */
	WDOG_WINDOW_EWI                                 /**<  WDOG work mode: windows and early warning interrupt */   
}WDOG_WORKMODE_T;
/**
 * @}
 */

/**
 * General WDOG Events
 */
typedef enum
{
    WDOG_EVENT_REOOT_WARNING = 0,                   /**< when WDOG almost reboot chip, it will send warning event to notify usr) */
} WDOG_Event_T;


/*
 * WWDG CLK division factor
 * WWDG_CLK = system clock/(WWDG_CLK_DIV+1);
 */
typedef enum
{
	HAL_WWDG_CLKDIV_FACTOR_1   = 0,                      /**< Select SYSCLK/(0+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_2   = 1,                      /**< Select SYSCLK/(1+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_4   = 3,                      /**< Select SYSCLK/(3+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_8   = 7,                      /**< Select SYSCLK/(7+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_16  = 15,					           /**< Select SYSCLK/(15+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_32  = 31,					           /**< Select SYSCLK/(31+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_64  = 63,					           /**< Select SYSCLK/(63+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_128 = 127,					           /**< Select SYSCLK/(127+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_256 = 255,					           /**< Select SYSCLK/(255+1) As WDOG Clksrc () */
	HAL_WWDG_CLKDIV_FACTOR_511 = 511					           /**< Select SYSCLK/(512+1) As WDOG Clksrc () */

}HAL_Wwdg_Clkdiv;


/*
 * 
 * An WWDG interrupt is generated when WWDG counter decreased down to 0x40 or 0x80 or 0x100 or 0x200 or 0x400 
 * which is prior to watchdog time-out reset.
 */
typedef enum
{

	HAL_WWDG_WINLIN_0 = 0,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_1    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_2    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_3    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_4    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_5    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_6    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_7    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_8    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_9    ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_10   ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_11   ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_12   ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_13   ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_14   ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
	HAL_WWDG_WINLIN_15   ,                      /**< 0X40*(HAL_WWDG_WINLIN+1) */
}HAL_Wwdg_WinLen;


/**
 * WDOG user handle structure
 */
typedef struct
{
    WDOG_Device_T   *Instance;                       /**< Pointer to WDOG device */
} WDOG_Handle_T;


/**
 * @brief     Callback definition of WDOG interrupt handler to notify user an event with parameters.
 *
 * @param     hWDOG        Pointer to WDOG handler.
 * @param     event        WDOG event, it can be one of enum @ref  WDOG_Event_T.
 * @param     param        None.
 * @retval    None.
 */
typedef void (*WDOG_Callback_T) (WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param);

/**
 * WDOG configuration structure.
 */
typedef struct
{
    uint32_t              mode;                    /**< config mode reset mode and interrupt mode */
    uint32_t              workEwiMode;             /**< Config work mode and Early warning interrupt were select */
    uint32_t              resetTime;               /**< System will reset after resetTime(uint:ms) pass if wdog is not feed again */
    uint32_t              windowTime;              /**< config wdog windows time*/
	uint32_t              winLen;
    HAL_Wwdg_Clkdiv       clkDivFactor;            /**< Config wdog clk division factor */
    WDOG_Callback_T       callBack;              /**< Drivr send usr event to call this function */
} WDOG_Config_T;







/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief    Initializes WDOG device with user configuration and user handle.
 *
 * @param    pWDOG         WDOG device point
 * @param    pConfig       Pointer to user-defined configuration.
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   RET_OK        WDOG_Init success
 * @retval   RET_ERROR     WDOG_Init error
 * @retval   RET_INVPARAM  Usr input paramter is error
 */
RET_CODE HAL_WDOG_Init(WDOG_Device_T *pWDOG, WDOG_Config_T *pConfig, WDOG_Handle_T *hWDOG);

/**
 * @brief    Enable wdog to start work,watdog cnt start to decrease.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_Enable(WDOG_Handle_T *hWDOG);

/**
 * @brief    Disable wdog to work , the watchdog cnt wil not change.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_Disable(WDOG_Handle_T *hWDOG);

/**
  * @brief  if the EWI is enabled, an interrupt is generated  when wdog counter decreased down
            to 0x40 0x80 ....0x40*(winLen+1)
  * @param  pWDOG WDOG Instance
  * @param  winLen base number,winLen max number is 0xF
  * @retval None
  */
void HAL_WDOG_SetWinLen(WDOG_Handle_T *pHandle , uint8_t winLen);


/**
 * @brief    Feed wdog to prevent system reset.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_FeedWdog(WDOG_Handle_T *hWDOG);

/**
 * @brief    Watch Dog Interrupt Clear.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_IntClear(WDOG_Handle_T* hWDOG);

/**
 * @brief    Set wdog window mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_SetWindowMode(WDOG_Handle_T *hWDOG);

/**
 * @brief    Set wdog window and EWI mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_SetWindowMode_EWI(WDOG_Handle_T *hWDOG);

/**
 * @brief    Set wdog Normal and EWI mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_SetNormalMode_EWI(WDOG_Handle_T *hWDOG);

/**
 * @brief    Set wdog normal mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void HAL_WDOG_SetNormalMode(WDOG_Handle_T *hWDOG);

/**
 * @brief    Get wdog value.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
RET_CODE HAL_WDOG_GetCountVal(WDOG_Handle_T *hWDOG);

/**
 * @brief    Get wdog CNT outside window flag.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
RET_CODE HAL_WDOG_GetCntOutWinFlag(WDOG_Handle_T *hWDOG);


/**
 * @brief WDOG interrupt handling routine.
 *
 * This function handles the WDOG IRQ.
 *
 * @param                  void.
 *
 * @retval                 None
 */
void HAL_WDOG_IRQHandler(void);


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_WWDG_H_ */



