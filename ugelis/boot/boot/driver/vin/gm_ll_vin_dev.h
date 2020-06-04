/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_vin_dev.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/11/28 
 *    
 * @brief         VIN peripheral PHY device header file
 * 
 * @note
 *    2016/11/28, mark.lee, V1.0.0
 *        Initial version. 
 *
 */

#ifndef _GM_LL_VIN_DEV_H_
#define _GM_LL_VIN_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Video Input Caputre - Register Layout Typedef */
typedef struct
{
    __IO uint32_t CR;            /**< VIN control register,                         Address offset: 0x00 */
    __IO uint32_t SR;            /**< VIN status register,                          Address offset: 0x04 */
    __IO uint32_t RISR;          /**< VIN raw interrupt status register,            Address offset: 0x08 */
    __IO uint32_t IER;           /**< VIN interrupt enable register,                Address offset: 0x0C */
    uint32_t      RESERVED0[2];  /**< Reserved, 0x10-0x14                                                */
    __IO uint32_t ESCR;          /**< VIN embedded synchronization code register,   Address offset: 0x18 */
    __IO uint32_t ESUR;          /**< VIN embedded synchronization unmask register, Address offset: 0x1C */
    __IO uint32_t CWSTRTR;       /**< VIN crop window start,                        Address offset: 0x20 */
    __IO uint32_t CWSIZER;       /**< VIN crop window size,                         Address offset: 0x24 */
    __IO uint32_t DR;            /**< VIN data register,                            Address offset: 0x28 */
    __IO uint32_t FBAR;          /**< VIN Frame Buffer Address Register             Address offset: 0x2C */
    __IO uint32_t FBLR;          /**< VIN Frame Buffer Length Register              Address offset: 0x30 */
    __IO uint32_t IMLR;          /**< VIN Capture Image Length Register             Address offset: 0x34 */
} VIN_Device_T;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_VIN_DEV_H_ */
