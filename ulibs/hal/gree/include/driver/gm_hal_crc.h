/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          crc.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 * 
 * @date          2016/07/05 
 *    
 * @brief         Header file of crc driver.
 * 
 * @note
 *    2016/04/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/05, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/23, Mike.Zheng, V1.2.0
 *        Update For Release.
 */

#ifndef _GM_HAL_CRC_H_
#define _GM_HAL_CRC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include <gm_ll_crc.h>

#define CRC_ACCUMULATE_TIEMOUT_US        10000

/**
 * Accumulate inputdata format  
 */	 
typedef enum    
{
    INPUT_FORMAT_BYTES                      = 0x00,   /**< The inputdata are 8 bits lenght */
    INPUT_FORMAT_HALFWORDS                  = 0x01,   /**< The inputdata are 16 bits lenght */
    INPUT_FORMAT_WORDS                      = 0x02,   /**< The inputdata are 32 bits lenght */
} CRC_InputFormat_T;

/**
 * Inputdata format 
 */	
typedef enum  
{
    INPUT_INVERSION_NONE                    = 0x00,  /**< inputdata not inversion */
    INPUT_INVERSION_BY_BYTE                 = 0x01,  /**< inputdata 8bit as uint to inversion */
    INPUT_INVERSION_BY_HALFWORD             = 0x02,  /**< inputdata 16bit as uint to inversion */
    INPUT_INVERSION_BY_WORD                 = 0x03,  /**< inputdata 32bit as uint to inversion */
} CRC_InputInversion_T;

/**
 * Polynomial bitwidth 
 */
typedef enum  
{
    CRC_POLYLENGTH_8B  						= 0x00, /**< polynomial bitwidth 8bit(crc8) */    
    CRC_POLYLENGTH_16B 						= 0x01, /**< polynomial bitwidth 16bit(crc16) */  
    CRC_POLYLENGTH_32B 						= 0x02, /**< polynomial bitwidth 32bit(crc32) */ 
    CRC_POLYLENGTH_7B  						= 0x03, /**< polynomial bitwidth 7bit(crc7) */
} CRC_PolyLength_T;

/**
 * Accumulate CRC InputData Parameter configuration structure.
 */
typedef struct
{
    uint32_t 				      			initValue;          /**< CRC calc init value */   
    uint32_t 				      			polynomial;         /**< CRC calc polynomial */
    uint32_t 				      			xorValue;           /**< CRC calc xor value */
    CRC_PolyLength_T              			polyLength;         /**< CRC calc polynomial BitWidth,support crc7,crc16,crc8,crc32 */
    CRC_InputFormat_T             			inputFormat;        /**< CRC calc inputdata format */
    CRC_InputInversion_T          			inputInversion;     /**< CRC calc inputdata inversion mode */
    uint8_t         			            outputInversionEn;  /**< CRC calc outputdata inversion enable or disable */
} CRC_Config_T;

/**
 * CRC user handle structure.
 */
typedef struct 
{
    CRC_Device_T   							*device;            /**< Pointer to crc device */
    CRC_InputFormat_T                       inputFormat;        /**< inputFormat: refer explain CRC_InputFormat_T */
} CRC_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief Initializes CRC device.
 *
 * @param pCRC CRC Device Point
 * @param hCRC Pointer to CRC handle.
 *
 * @retval RET_OK CRC_Init success
 * @retval RET_ERROR CRC_Init error
 * @retval RET_INVPARAM Usr input paramter is error
 */
RET_CODE HAL_CRC_Init(CRC_Device_T *pCRC, CRC_Handle_T *hCRC);

/**
 * @brief Config Config To CRC Accumulate Parameter 
 * This function used to config different CRC Accumulate Parameter when
 * first Accumulate CRC Value or change to another Accumulate Parameter 
 * later 
 
 * @param hCRC Pointer to CRC handle.
 * @param pConfig CRC Accumulate Parameter
 * @return NO return value(return void)
 */
void HAL_CRC_AccumulateConfig(CRC_Handle_T *hCRC, CRC_Config_T *pConfig);

/**
 * @brief calc inputdata crc value 
 *
 * @param hCRC Pointer to CRC handle.
 * @param pBuffer inputdata buf  
 * @param length inputdata length (length by byte)
 *
 * @retval The Value For CRC Accumulate Result  
 */
uint32_t HAL_CRC_GetAccumulateResult(CRC_Handle_T *hCRC, uint8_t *pBuffer, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_CRC_H_*/



 




