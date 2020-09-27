/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_dac_dev.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		DAC source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _GM_LL_DAC_DEV_H_
#define _GM_LL_DAC_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DAC register layout type definition */
typedef struct
{
    __IO uint32_t DAC_CTRL;                            /* DACx Controller Register :offset: 0x00 */
    __IO uint32_t DAC_STRIG;                            /* DACx Software Trigger Register :offset: 0x04 */
    __IO uint32_t DAC_HTRIG;                           /* DACx Hardware Trigger Register:offset: 0x08 */
	__IO uint32_t DAC_DH;                              /* DACx Data Hold Register :offset: 0x0C */
    __IO uint32_t DAC_DO;                              /* DACx Data Out Register: 0x10 */
    __IO uint32_t DAC_IE;                              /* DACx Interrupt Enable Register:offset: 0x14 */
	__IO uint32_t DAC_STATUS;                          /* DACx Status Register :offset: 0x18 */
    __IO uint32_t DAC_SHST;                            /* DACx Sample and Hold Sample Time Register :offset: 0x1C */
    __IO uint32_t DAC_SHHT;                            /* DACx Sample and Hold Hold Time Register:offset: 0x20 */
	__IO uint32_t DAC_SHRF;                            /* DACx Sample and Hold Hold Time Register:offset: 0x24 */

} DAC_Device_T;

/* DAC common register layout type definition */
typedef struct
{
    __IO uint32_t DAC_DTRIG;                          /* DAC Dual Software Trigger Register:offset: 0x00 */
    __IO uint32_t DAC_DDH;                            /* DAC Dual Data Hold Register:offset: 0x04 */

} DAC_ComDevice_T;


#ifdef __cplusplus
}
#endif

#endif /*  #ifndef _GM_LL_ACMP_DEV_H_ */
