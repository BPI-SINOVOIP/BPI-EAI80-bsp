/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_hal_acmp.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		ACMP Source file
 *
 * @note		
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *			
 **/

#ifndef _GM_HAL_ACMP_H_
#define _GM_HAL_ACMP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_gpio.h>
#include <acmp/gm_ll_acmp_dev.h>
#include <acmp/gm_ll_acmp.h>

#ifndef ACMP_DEBUG_PRINTF
    #define ACMP_PR_DEBUG(...)      do {} while (0)
#else
    #define ACMP_PR_DEBUG           printf
#endif

#ifndef ACMP_ERR_PRINTF
    #define ACMP_PR_ERR(...)        do {} while (0)
#else
    #define ACMP_PR_ERR             printf
#endif


/**
 * @brief Initializes ACMP device with user configuration
 *
 * This function configures ADC device with the user-defined settings.
 *
 * @param  pACMP                ACMP device point
 * @param  pConfig              pointer to user-defined configuration.
 * @param  hACMP                pointer to ACMP handle.
 *
 * @retval   RET_OK             HAL_ACMP_Init success
 * @retval   RET_ERROR          HAL_ACMP_Init error
 * @retval   RET_INVPARAM       Usr input paramter is error
 */
RET_CODE HAL_ACMP_Init(ACMP_Handle_T *pHandle, ACMP_Device_T *pACMP, ACMP_Config_T *pConfig);

/**
 * @brief Get Rhe ACMP device to compare the voltage between negativepn and positivepin
 *
 * This function is used for get compare result between negativePin and positivepin.
 *
 * @param hACMP Pointer to ACMP handle.
 *
 * @retval ACMP_LOW            Positivepin voltage is no more than negativepin voltage
 * @retval ACMP_HIHG           Positivepin voltage is more than negativepin voltage
 */
ACMP_CompareResult_T HAL_ACMP_GetCompareRestult(ACMP_Handle_T *hACMP);


#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_ACMP_H_*/
