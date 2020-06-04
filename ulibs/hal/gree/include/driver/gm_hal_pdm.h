/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_pdm.h
 *
 * @author      Robert_hu
 *
 * @version     v1.0.0
 *
 * @date        2019/11/1
 *
 * @brief       PDM Source file
 *
 * @note
 *              2019/11/01, Robert_hu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_HAL_PDM_H_
#define _GM_HAL_PDM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_gpio.h>
#include <gm_hal_dma.h>
#include <pdm/gm_ll_pdm_dev.h>
#include <pdm/gm_ll_pdm.h>

#ifndef PDM_DEBUG_PRINTF
#define PDM_PR_DEBUG(...)      do {} while (0)
#else
#define PDM_PR_DEBUG           printf
#endif

#ifndef PDM_ERR_PRINTF
#define PDM_PR_ERR(...)        do {} while (0)
#else
#define PDM_PR_ERR             printf
#endif

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))


/**
 * @brief Initializes PDM device with user configuration
 *
 *
 * @param  pPDM                PDM device point
 * @param  pConfig              pointer to user-defined configuration.
 * @param  hPDM               pointer to PDM handle.
 *
 * @retval   RET_OK             HAL_PDM_Init success
 * @retval   RET_ERROR          HAL_PDM_Init error
 * @retval   RET_INVPARAM       Usr input paramter is error
 */
HAL_StatusTypeDef HAL_PDM_Init(PDM_Handle_T *pHandle, PDM_Device_T *pACMP, PDM_Config_T *pConfig);
HAL_StatusTypeDef HAL_PDM_Exit(PDM_Handle_T *pHandle);

static void PDM_DMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param);

static HAL_StatusTypeDef PDM_DMAConfig(PDM_Device_T *pPDM, DMA_Handle_T *hDMA);

HAL_StatusTypeDef HAL_PDM_Receive_DMA(PDM_Handle_T *pHandle, PDM_WorkMode_T mode, int16_t *pData, uint32_t Size);



#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_PDM_H_*/
