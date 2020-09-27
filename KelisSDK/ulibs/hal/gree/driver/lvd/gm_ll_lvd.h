/***********************************************************************
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @filename gm_ll_lvd.h
 *
 * @author Angrad.Yang
 *
 * @version 1.0.2
 *
 * @date 2017/04/19
 *
 * @description LVD Header For GM6621
 *
 * @history
 *  Date                   By         Comments                   Ver.
 *  =========       =========        =========                =========
 *   2017/05/26       Angrad.Yang     Create                	1.0.0
 *   2019/02/28       Angrad.Yang     Modification for GM6721	1.0.1
 **********************************************************************/

#ifndef __GM_LL_LVD_H_
#define __GM_LL_LVD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_soc.h>

/* Lvd Controller Register Layout Type Definition */
typedef struct
{
    __IO uint32_t VD1CR;                        /**< Voltage Detector1 Control Register: 0x138 */
    __IO uint32_t VD1SR;                        /**< Voltage Detector1 Status Register:  0x13C */
    __IO uint32_t VD2CR;                        /**< Voltage Detector2 Control Register: 0x140 */
    __IO uint32_t VD2SR;                        /**< Voltage Detector2 Status Register:  0x144 */
    __IO uint32_t VD3CR;                        /**< Voltage Detector3 Control Register: 0x148 */
    __IO uint32_t VD3SR;                        /**< Voltage Detector3 Status Register:  0x14C */

} LVD_Device_T;

typedef struct
{
	__IO uint32_t RST_STATUS;
} LVD_Status_T;

#define LVD             ((LVD_Device_T *)LVD_BASE)
#define LVDStatus		((LVD_Status_T *)(SYS_BASE + 0x00000004U))

#ifdef __cplusplus
}
#endif


#endif //__GM_LL_LVD_H_

