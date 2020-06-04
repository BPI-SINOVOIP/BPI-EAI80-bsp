/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_hal_dac.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		DAC source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _GM_HAL_DAC_H_
#define _GM_HAL_DAC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_gpio.h>
#include <gm_hal_dma.h>
#include <dac/gm_ll_dac_dev.h>
#include <dac/gm_ll_dac.h>

#define DAC_DEBUG_PRINTF  1

#ifndef DAC_DEBUG_PRINTF
    #define DAC_PR_DEBUG(...)      do {} while (0)
#else
    #define DAC_PR_DEBUG           printf
#endif

#ifndef DAC_ERR_PRINTF
    #define DAC_PR_ERR(...)        do {} while (0)
#else
    #define DAC_PR_ERR             printf
#endif

/**
 * @brief Initializes DAC device with user configuration
 *
 * This function configures DAC device with the user-defined settings.
 *
 * @param  pDAC                DAC device point
 * @param  pConfig              pointer to user-defined configuration.
 * @param  hDAC                pointer to ACMP handle.
 *
 * @retval   RET_OK             HAL_DAC_Init success
 * @retval   RET_ERROR          HAL_DAC_Init error
 * @retval   RET_INVPARAM       Usr input paramter is error
 */
RET_CODE HAL_DAC_Init(DAC_Handle_T *pHandle, DAC_Device_T *pDAC, DAC_Config_T *pConfig);

RET_CODE HAL_DAC_Triger(DAC_Handle_T *pHandle, DAC_Config_T *pConfig);
unsigned int HAL_DAC_GetDataOut(DAC_Handle_T *pHandle);
int HAL_DAC_Result_Check(int data_out, int expect_data_out,int convert_mode);

int HAL_DAC_Noise_Result(int data_hold , int mask , int shift_num);

#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_DAC_H_*/
