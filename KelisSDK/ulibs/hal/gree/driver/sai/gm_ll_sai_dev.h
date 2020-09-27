/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          sai_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/02/25, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_LL_SAI_DEV_H_
#define _GM_LL_SAI_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif
#if 1
/* SAI Control Register Layout Type Definition */
typedef struct
{
  __IO uint32_t CR1;      /*!< SAI block x configuration register 1,     Address offset: 0x00 */
  __IO uint32_t CR2;      /*!< SAI block x configuration register 2,     Address offset: 0x04 */
  __IO uint32_t FRCR;     /*!< SAI block x frame configuration register, Address offset: 0x08 */
  __IO uint32_t SLOTR;    /*!< SAI block x slot register,                Address offset: 0x0C */
  __IO uint32_t IMR;      /*!< SAI block x interrupt mask register,      Address offset: 0x10 */
  __IO uint32_t SR;       /*!< SAI block x status register,              Address offset: 0x14 */
  __IO uint32_t CLRFR;    /*!< SAI block x clear flag register,          Address offset: 0x18 */
  __IO uint32_t DR;       /*!< SAI block x data register,                Address offset: 0x1C */
  __IO uint32_t CLKCR;    /*!< SAI block x clock control register,       Address offset: 0x20 */
  __IO uint32_t VER;      /*!< SAI block x version register,             Address offset: 0xFC */
} SAI_Block_TypeDef;
#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_SAI_DEV_H_ */
