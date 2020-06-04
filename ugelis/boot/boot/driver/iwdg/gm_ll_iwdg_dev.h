/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_iwdg_dev.h
 *
 * @author        kenny.cmt
 *
 * @version       1.0
 *
 * @date          2018/03/23
 *
 * @brief         CMSIS Peripheral Access Layer for GM67xx
 *
 * @note
 *    2018/03/23, kenny.cmt, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_IWDG_DEV_H_
#define _GM_LL_IWDG_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
  * @brief Independent WATCHDOG
  */

typedef struct
{
  __IO uint32_t KR;        /*!< IWDG Key register,       Address offset: 0x00 */
  __IO uint32_t PR;        /*!< IWDG Prescaler register, Address offset: 0x04 */
  __IO uint32_t RLR;       /*!< IWDG Reload register,    Address offset: 0x08 */
  __IO uint32_t SR;        /*!< IWDG Status register,    Address offset: 0x0C */
  __IO uint32_t RSV[60];   /*!< IWDG Reserve                                  */
  __IO uint32_t VER;       /*!< IWDG Version register,   Address offset: 0xFC */
  
} IWDG_TypeDef;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_IWDG_DEV_H_ */
