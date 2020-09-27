/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          flash.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/06/27
 *
 * @brief         Header file of flash driver.
 *
 * @note
 *    2016/06/27, wizephen.wang, V1.0.0
 *        Initial version.
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/**
 *  Structure of flash parameters
 */
typedef struct
{
    uint32_t  sizePerPage;    /**< Page size in bytes */
    uint32_t  pagesPerFlash;  /**< Page number in flash */
    uint32_t  sizePerFlash;   /**< Flash size in bytes*/
    uint32_t  baseLgcAddress; /**< Base logical address*/
    uint32_t  basePhyAddress; /**< Base physical address*/
} FLASH_Param_T;

/**
 * Flash erase type
 */
typedef enum
{
    ERASE_TYPE_MASS  = 0x00,  /**< A mass-erasing */
    ERASE_TYPE_PAGE  = 0x01   /**< A page-erasing */
} FLASH_EraseType_T;

/**
 * Flash pages lock
 */
typedef enum
{
    PAGE_IS_UNLOCKED  = 0x00,  /**< One page is free to use */
    PAGE_IS_LOCKED    = 0x01   /**< One page is locked*/
} Page_LockType_T;

/**
 * @brief Initializes flash device.
 *
 * This function does nothing and always return RET_OK.
 * @param  void
 *
 * @retval RET_OK     Api was successfully executed.
 */
RET_CODE Flash_Init(void);

/**
 * @brief De-initializes flash device.
 *
 * This function does nothing and always return RET_OK.
 * @param  void
 *
 * @retval RET_OK      Api was successfully executed.
 */
RET_CODE Flash_DeInit (void);

/**
 * @brief Perform a mass- or page-erase on flash.
 *
 * @param eraseType     Mass- or page-erase
 * @param pageAddress   The page address to be erased. It make no sense when ERASE_TYPE_MASS is taken
 *                      as eraseType argument.
 *
 * @retval RET_OK       Api was successfully executed.
 * @retval RET_ERROR    Failure occurred while api was being executed.
 */
RET_CODE Flash_Erase(FLASH_EraseType_T eraseType, uint32_t pageAddress);

/**
 * @brief Perform a count of data-programing on flash. And ensure that
 *        the flash space ranging from address to address + count is blank
 *        before this API is invoked.
 *
 * @param address       Target address within flash area to be written.
 * @param pData         Pointer to buffer of to-be-written data.
 * @param count         The length of to-be-written data.
 *
 * @retval RET_OK       Api was successfully executed.
 * @retval RET_ERROR    Failure occurred while api was being executed.
 */
RET_CODE Flash_Write(uint32_t address, uint8_t *pData, uint16_t count);

/**
 * @brief Program data to a target address(either 32-Bit or 16-Bit aligned).
 *
 * @param targetAddress Target address within the size of flash.
 * @param wrData        Data to be programmed.
 * @param width         Program flash at 16-Bit or 32-Bit width once a time.
 *
 * @retval RET_OK       Api was successfully executed.
 * @retval RET_ERROR    Failure occurred while api was being executed.
 */
RET_CODE Flash_Program2Address(uint32_t targetAddress, uint32_t wrData, FLASH_ProgWidth_T width);


/**
 * @brief Read a count of data from flash.
 *
 * @param address       Target address within flash area to be read.
 * @param pData         Pointer to buffer of to-be-read data.
 * @param count         The length of to-be-read data.
 *
 * @retval RET_OK       Api was successfully executed.
 * @retval RET_ERROR    Failure occurred while api was being executed.
 */
RET_CODE Flash_Read(uint32_t address, uint8_t *pData, uint16_t count);

/**
 * @brief Get flash parameters.
 *
 * This function does nothing and always return RET_OK.
 *
 * @param pParam         Pointer to flash parameter structure.
 *
 * @retval RET_OK        Api was successfully executed.
 */
RET_CODE Flash_GetParam(FLASH_Param_T *pParam);

/**
 * @brief Check whether a page is locked or not. Once one page is locked,
 *        any manipulation on this page except read is performed in vain.
 *
 * This API gives correct page-lock status only after every hardware reset.
 *
 * @param pageAddress         The address of one page.
 *
 * @retval PAGE_IS_UNLOCKED   This page is free to use.
 * @retval PAGE_IS_LOCKED     This page is locked.
 */
Page_LockType_T Flash_PageIsLocked(uint32_t pageAddress);

#ifdef __cplusplus
}
#endif

#endif /*_FLASH_H_*/


