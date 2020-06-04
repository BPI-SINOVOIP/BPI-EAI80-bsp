/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_sdram.h
 *
 * @author		hongze
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		sdram ll header file
 *
 * @note		
 *				2019/01/24, hongze, v1.0.0
 *					Initial version.
 *			
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GM_LL_SDRAM_H
#define __GM_LL_SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum
{
  SDRAM_NULL,
  SDRAM_D0_2M_4B,
  SDRAM_D0_4M_4B,
  SDRAM_D0_8M_4B,
  SDRAM_D0_16M_4B,
  SDRAM_D0_32M_4B,
  SDRAM_D0_64M_4B,
  SDRAM_D1_2M_4B,
  SDRAM_D1_4M_4B,
  SDRAM_D1_8M_4B,
  SDRAM_D1_16M_4B,
  SDRAM_D1_32M_4B,
  SDRAM_D1_64M_4B  
}HAL_SDRAM_ModeTypeDef;


#ifdef __cplusplus
}
#endif

#endif /* __GM_LL_SDRAM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
