/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_vout.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/11/28 
 *    
 * @brief         VOUT peripheral PHY device header file
 * 
 * @note
 *    2016/11/28, mark.lee, V1.0.0
 *        Initial version. 
 *
 */

#ifndef _GM_LL_VOUT_DEV_H_
#define _GM_LL_VOUT_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief LCD-TFT Display Controller
 */
typedef struct
{
    __IO uint32_t SSCR;          /**< VOUT Synchronization Size Configuration Register,    Address offset: 0x00 */
    __IO uint32_t BPCR;          /**< VOUT Back Porch Configuration Register,              Address offset: 0x04 */
    __IO uint32_t AWCR;          /**< VOUT Active Width Configuration Register,            Address offset: 0x08 */
    __IO uint32_t TWCR;          /**< VOUT Total Width Configuration Register,             Address offset: 0x0C */
    __IO uint32_t GCR;           /**< VOUT Global Control Register,                        Address offset: 0x10 */
   uint32_t       RESERVED;      /**< Reserved, 0x14                                                            */
    __IO uint32_t BCCR;          /**< VOUT Background Color Configuration Register,        Address offset: 0x18 */
    __IO uint32_t IER;           /**< VOUT Interrupt Enable Register,                      Address offset: 0x1C */
    __IO uint32_t ISR;           /**< VOUT Interrupt Status Register,                      Address offset: 0x20 */
    __IO uint32_t LIPCR;         /**< VOUT Line Interrupt Position Configuration Register, Address offset: 0x24 */
    __IO uint32_t CPSR;          /**< VOUT Current Position Status Register,               Address offset: 0x28 */
    __IO uint32_t CDSR;          /**< VOUT Current Display Status Register,                Address offset: 0x2C */
} VOUT_Device_T;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_VOUT_DEV_H_ */
