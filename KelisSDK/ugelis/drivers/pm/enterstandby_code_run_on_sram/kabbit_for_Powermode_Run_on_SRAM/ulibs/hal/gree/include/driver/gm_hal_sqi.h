/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_sflash.h
 *
 * @author        Colin.Yang
 *
 * @version       1.1.0
 *
 * @date          2018/03/27
 *
 * @brief         Header file of Sflash driver.
 *
 * @note
 *    2018/03/27, Colin.Yang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_HAL_SQI_H_
#define _GM_HAL_SQI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <sqi/gm_ll_sqi_dev.h>

/*Declaration of UART handle structure*/
struct SQI_Handle;

/**
 * SFlash read mode
 */

typedef enum
{
    SQI_READ_BY_CPU                                        = 0x0,                      /**< SQI read use dma */
    SQI_READ_BY_DMA                                                                   /**< SQI read use cpu*/
} SQI_READ_MODE_T;


/**
 *  read bit mode
 */

typedef enum
{
    SQI_4BIT_MODE                                        = 0x0,                      /**< SQI read use 4bit  */
    SQI_1BIT_MODE                                                                   /**< SQI read use 1bit */
} SQI_READ_BIT_MODE_T;


/**
 * General Sflash configuration structure.
 */
typedef struct SQI_Config
{
    int config;                                                               /*<avoid error >*/
} SQI_Config_T;

/**
 * Configuration of transfer parameters
 */
typedef struct
{
    int tfconfig;                                                            /*<avoid error >*/
} SQI_TransferConfig_T;

/**
 * Configuration of transfer parameters
 */
typedef struct
{
    int state;                                                               /*<avoid error >*/

} SQI_TransferStatus_T;


/**
 * SQI user handle structure.
 */
typedef struct SQI_Handle
{
    SQI_Device_T *pSQIDev;
    SQI_Config_T *pConfig;
    SQI_READ_MODE_T read_mode;
    SQI_READ_BIT_MODE_T bit_mode;
    uint32_t         clk_div;
} SQI_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/

/**
 * @brief Initializes SQI device and SQI handle with user configuration.
 *
 * This function configures one SQI device with the user-defined settings. The transaction
 * handle is also initialized if it is not NULL.
 * Example code:
 * @code
 * @endcode
 *
 * @param  pSQIDev          SQI device handle
 * @param  pConfig        Pointer to user-defined configuration
 * @param  hSQI          SQI handle
 *
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK           Configure successfully
 */
RET_CODE       HAL_SQI_Init(SQI_Handle_T *hSQI, SQI_Device_T *pSQIDev, SQI_Config_T *pConfig);

/**
 * @brief Initializes SQI device and SQI handle with user configuration.
 *
 * This function configures one SQI device with the user-defined settings. The transaction
 * handle is also initialized if it is not NULL.
 * Example code:
 * @code
 * @endcode
 *
 * @param  pSQIDev          SQI device handle
 * @param  pConfig        Pointer to user-defined configuration
 * @param  hSQI          SQI handle
 *
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK           Configure successfully
 */
RET_CODE       HAL_SQI_UnInit(SQI_Handle_T *hSQI);
/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE       HAL_SQI_Read(SQI_Handle_T *hSQI, uint32_t offset, void *buff, uint32_t length);

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE       HAL_SQI_Erase(SQI_Handle_T *hSQI, uint32_t offset, uint32_t length);

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE       HAL_SQI_WriteAll(SQI_Handle_T *hSQI, uint32_t offset, const void *buff, uint32_t length);

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE       HAL_SQI_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buff, uint32_t length);

/**
 * @brief  SQI Flash write on 4Line mode.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  buffer:  the data will write to flash.
 * @param  len:  write size.
 * @retval HAL status
 */

RET_CODE HAL_SQI_MX25_4Line_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, size_t len);


/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
uint32_t       HAL_SQI_GetFlashSize(SQI_Handle_T *hSQI);

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
uint32_t       HAL_SQI_GetSectorSize(SQI_Handle_T *hSQI);

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
uint32_t       HAL_SQI_GetBlockSize(SQI_Handle_T *hSQI);

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
uint32_t       HAL_SQI_GetMaxRWSize(SQI_Handle_T *hSQI);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_SQI_H_ */



