/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_sqi.h
 *
 * @author        Steven.lee
 *
 * @version       1.1.2
 *
 * @date          2020/5/18
 *
 * @brief         Header file of SQI driver.
 *
 * @note
 *    2019/10/1, Steven.lee, V1.0.0
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

//default value
#define SCLK_DIV                           1          //clk default value is 1

//Manufacturer ID
#define MT_MF_ID     0x20
#define MX_MF_ID     0xc2
#define GD_MF_ID     0xc8
#define WB_MF_ID     0xef

//Memory type
#define MT_MT_ID     0xba
#define MX_MT_ID     0x20
#define GD_MT_ID     0x40
#define WB_MT_ID     0x40
#define KH_MT_ID     0x23


#define SQI_256B_PAGE_SIZE        256

#define SQI_1M_SIZE        0x100000
#define SQI_2M_SIZE        0x200000
#define SQI_4M_SIZE        0x400000
#define SQI_8M_SIZE        0x800000
#define SQI_16M_SIZE       0x1000000
#define SQI_32M_SIZE       0x2000000
#define SQI_64M_SIZE       0x4000000
#define SQI_128M_SIZE      0x8000000
#define SQI_256M_SIZE      0x10000000






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
 *  sflash Manufacturer
 */

typedef enum
{
    SQI_MX                   = 0x0,                     /**< Macronix>**/
    SQI_MT,                                             /**< Micron>**/
    SQI_GD,                                             /**< GigaDevice>**/
    SQI_WB                                              /**< Winbond>**/
} SQI_Manufacturer_T;


/**
 *  sflash  model
 */

typedef enum
{
    MX25L1605D                 = 0x0,                      /**< Micron>**/
    MX25L3205D,                                            /**< GigaDevice>**/
    MX25L6433F,                                            /**< Winbond>**/
    MX25L12835F,
    MX25L25633F,
    MX25L51245G,
    MT25QL128ABA,
    MT25QL256ABA,
    GD25Q127C,
    GD25Q257D,
    W25Q128JV,
    W25Q256JV,
    KH25V8035F
} SQI_MODEL_T;


/**
 * General Sflash configuration structure.
 */
typedef struct SQI_Config
{
    SQI_READ_MODE_T      read_mode;
    SQI_READ_BIT_MODE_T  bit_mode;
    uint32_t             clk_div;

} SQI_Config_T;


/**
 * SQI user handle structure.
 */
typedef struct SQI_Handle
{
    SQI_Device_T *pSQIDev;
    SQI_Config_T *pConfig;
    uint32_t             flashsize;
    uint32_t             pagesize;
    SQI_Manufacturer_T   Manufacturer;
    SQI_MODEL_T          model;
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
uint32_t       HAL_SQI_GetPageSize(SQI_Handle_T *hSQI);


/**
 * @brief  printf flash information
 * @param  hSQI:   Device handle.
 * @return  result
 */
RET_CODE HAL_SQI_Print_Device(SQI_Handle_T *hSQI);



#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_SQI_H_ */



