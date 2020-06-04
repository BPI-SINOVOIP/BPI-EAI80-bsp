/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_imp_dev.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2018/1/3
 *    
 * @brief         IMP peripheral PHY device header file
 * 
 * @note
 *    2018/1/3, mark.lee, V1.0.0
 *        Initial version. 
 *
 */

#ifndef _GM_LL_IMP_DEV_H_
#define _GM_LL_IMP_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief IMP Controller
 */

typedef struct
{
  __IO uint32_t CR;            /**< IMP Control Register,                         Address offset: 0x00 */
  __IO uint32_t ISR;           /**< IMP Interrupt Status Register,                Address offset: 0x04 */
  uint32_t      RESERVED0;     /**< Reserved, 0x08 */
  __IO uint32_t FGMAR;         /**< IMP Foreground Memory Address Register,       Address offset: 0x0C */
  __IO uint32_t FGOR;          /**< IMP Foreground Offset Register,               Address offset: 0x10 */
  __IO uint32_t BGMAR;         /**< IMP Background Memory Address Register,       Address offset: 0x14 */
  __IO uint32_t BGOR;          /**< IMP Background Offset Register,               Address offset: 0x18 */
  __IO uint32_t FGPFCCR;       /**< IMP Foreground PFC Control Register,          Address offset: 0x1C */
  __IO uint32_t FGCOLR;        /**< IMP Foreground Color Register,                Address offset: 0x20 */
  __IO uint32_t BGPFCCR;       /**< IMP Background PFC Control Register,          Address offset: 0x24 */
  __IO uint32_t BGCOLR;        /**< IMP Background Color Register,                Address offset: 0x28 */
  __IO uint32_t FGCMAR;        /**< IMP Foreground CLUT Memory Address Register,  Address offset: 0x2C */
  __IO uint32_t BGCMAR;        /**< IMP Background CLUT Memory Address Register,  Address offset: 0x30 */
  __IO uint32_t OPFCCR;        /**< IMP Output PFC Control Register,              Address offset: 0x34 */
  __IO uint32_t OCOLR;         /**< IMP Output Color Register,                    Address offset: 0x38 */
  __IO uint32_t OMAR;          /**< IMP Output Memory Address Register,           Address offset: 0x3C */
  __IO uint32_t OOR;           /**< IMP Output Offset Register,                   Address offset: 0x40 */
  __IO uint32_t NLR;           /**< IMP Number of Line Register,                  Address offset: 0x44 */
  __IO uint32_t LWR;           /**< IMP Line Watermark Register,                  Address offset: 0x48 */
  uint32_t      RESERVED1;     /**< Reserved, 0x4C */
  __IO uint32_t SNLR;          /**< IMP Scaler Number of Line Register,           Address offset: 0x50 */
  __IO uint32_t SCR;           /**< IMP scaler coefficent Register,               Address offset: 0x54 */
  __IO uint32_t THR;           /**< IMP harris threshold Register,                Address offset: 0x58 */
  uint32_t      RESERVED[233]; /**< Reserved, 0x5c-0x3FF */
  __IO uint32_t FGCLUT[256];   /**< IMP Foreground CLUT,                          Address offset:400-7FF */
  __IO uint32_t BGCLUT[256];   /**< IMP Background CLUT,                          Address offset:800-BFF */
} IMP_Device_T;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_IMP_DEV_H_ */
