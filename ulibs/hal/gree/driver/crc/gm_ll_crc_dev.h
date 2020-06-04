/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_crc_dev.h
 *
 * @author        bert.Ding
 *
 * @version       1.0.0
 * 
 * @date          2019/10/16 
 *    
 * @brief         CMSIS Peripheral Access Layer for F6721B
 * 
 * @note
 *    2019/10/16, bert.Ding, V1.0.0
 *        Initial version.
 *
 */
#ifndef _GM_LL_CRC_DEV_H_
#define _GM_LL_CRC_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>   

/**
 * CRC Control Register Layout Type Definition 
 */
typedef struct
{
    __IO uint32_t CTRL;                 			  /* CRC Contral Register:offset: 0x00 */
    __IO uint32_t POL;                 		 		  /* CRC Polynomia Register:offset: 0x04 */
    __IO uint32_t IVAL;                 			  /* CRC InitValue Register:offset: 0x08 */
    __IO uint32_t DA;                 			      /* CRC Data Register:offset: 0x0c */
    __IO uint32_t XOR;                  			  /* CRC Xorvalue Register:offset: 0x10 */
    __IO uint32_t RST;               			      /* CRC Accumulate :Result Register:offset: 0x14 */
    __IO uint32_t ST;                   			  /* CRC Status Register:offset: 0x18 */
	__IO uint32_t RESV0[56];         			      /* CRC Reserved Register:offset: 0x1c~0xf8 */
    __IO uint32_t VERNUM;              			      /* CRC Version Num Register:offset: 0xfc */  
} CRC_Device_T;

#ifdef __cplusplus
}
#endif

#endif /*_GM_LL_CRC_DEV_H_*/
