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
    uint32_t              workEwiMode;             /**< Config work mode and Early warning interrupt were select */
    uint32_t              resetTime;               /**< System will reset after resetTime(uint:ms) pass if wdog is not feed again */
    uint32_t              windowTime;              /**< config wdog windows time*/
	uint32_t              winLen;
    uint32_t              clkDivFactor;            /**< Config wdog clk division factor */
 //   WDOG_Callback_T       callBack;              /**< Drivr send usr event to call this function */
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
 * @param  WDOG         Pointer to WDOG handle.
 *
 * @retval                 None
 */
void HAL_WDOG_IRQHandler(WDOG_Handle_T *hWDOG);


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_WWDG_H_ */



