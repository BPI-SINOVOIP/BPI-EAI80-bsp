/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file             gm_hal_sdram.h
 *
 * @author        hongze
 *
 * @version       1.0.0
 * 
 * @date          2018/03/13
 *    
 * @brief          Header file of SDRAM module.
 * 
 * @note
 *    2018/03/13, hongze, V1.0.0
 *        Initial version.
 */

#ifndef __GM_HAL_SDRAM_H
#define __GM_HAL_SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef enum 
{
  SDRAM_4M,
  SDRAM_8M,
  SDRAM_16M,
  SDRAM_32M,
  SDRAM_64M,
  SDRAM_128M
} HAL_SdramSizeTypeDef;

typedef enum 
{
  SDRAM_16BIT,
  SDRAM_32BIT
} HAL_SdramWidthTypeDef;

typedef enum 
{
  SDRAM_1,
  SDRAM_2
} HAL_SdramNumTypeDef;

/** 
  * @brief  SDRAM handle Structure definition  
  */ 
typedef struct
{
  HAL_SdramSizeTypeDef           size;       /*!< SDRAM size                  */
  HAL_SdramWidthTypeDef          width;      /*!< SDRAM width                  */  
}SDRAM_HandleTypeDef;

/* Initialization/de-initialization functions *********************************/
HAL_StatusTypeDef HAL_SDRAM_Init(SDRAM_HandleTypeDef *hsdram);
HAL_StatusTypeDef HAL_SDRAM_DeInit(SDRAM_HandleTypeDef *hsdram);
#ifdef __cplusplus
}
#endif

#endif /* __GM_HAL_SDRAM_H */
