/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_acmp_dev.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		ACMP Source file
 *
 * @note		
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *			
 **/

#ifndef _GM_LL_ACMP_DEV_H_
#define _GM_LL_ACMP_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ACMP register layout type definition */
typedef struct
{
    __IO uint32_t CS;                               /* ACMP control and status register:offset: 0x04 */
    __IO uint32_t RESV0;                            /* Resever register:offset: 0x08 */
    __IO uint32_t FILTTHR;                          /* ACMP contral and status register:offset: 0x0c */

} ACMP_Device_T;

#define ACMP_DEV                                    ((ACMP_Device_T *)ACMP_BASE) /* ACMP device pointer  0x4000D004 */

#ifdef __cplusplus
}
#endif

#endif /*  #ifndef _GM_LL_ACMP_DEV_H_ */
