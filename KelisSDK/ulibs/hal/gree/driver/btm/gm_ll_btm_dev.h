/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_btm_dev.h
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2019/10/18
 *
 * @brief         BTM device file for F6721B
 *
 * @note
 *
 *    2019/10/18, edward.zhu, V1.1.0
 *        Compatible with F6721B
 */

#ifndef _GM_LL_BTM_DEV_H_
#define _GM_LL_BTM_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_soc.h>

typedef struct
{
    __IO uint32_t CTRL;            /**< BaseTimer1 Control Register, Offset: 0x0 */
    __IO uint32_t COMPARE;         /**< BaseTimer1 Compare Register, Offset: 0x4 */
    __IO uint32_t RTCCNT;          /**< BaseTimer1 Counter Register, offset: 0x8 */
} BTM_Device_T;

#ifdef __cplusplus
}
#endif

#endif //_GM_LL_BTM_DEV_H_

