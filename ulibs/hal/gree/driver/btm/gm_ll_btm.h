/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_btm.h
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

#ifndef _GM_LL_BTM_H_
#define _GM_LL_BTM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_ll_btm_dev.h>

#define MAX_BTM_NUM  2

/*************** BTM1 Control Register Bitfiled ******************/

#define BTM_CTRL_INT_POS              ((uint32_t) 9)
#define BTM_CTRL_INT                  ((uint32_t) ((uint32_t)1 << BTM_CTRL_INT_POS))

#define BTM_CTRL_IER_POS              ((uint32_t) 8)
#define BTM_CTRL_IER                  ((uint32_t) ((uint32_t)1 << BTM_CTRL_IER_POS))

#define BTM_CTRL_CLK_DIV_POS          ((uint32_t) 4)
#define BTM_CTRL_CLK_DIV              ((uint32_t) ((uint32_t)3 << BTM_CTRL_CLK_DIV_POS))
#define BTM_CTRL_CLKDIV_FIELD(x)      (((uint32_t)(((uint32_t)(x)) << BTM_CTRL_CLK_DIV_POS)) & BTM_CTRL_CLK_DIV)

#define BTM_CTRL_EN_POS               ((uint32_t) 0)
#define BTM_CTRL_EN                   ((uint32_t) ((uint32_t)1 << BTM_CTRL_EN_POS))

/*************** BTM1 Compare Register Bitfiled ******************/

#define BTM_CMP_CMP_POS               ((uint32_t) 0)
#define BTM_CMP_CMP                   ((uint32_t) ((uint32_t)0xFFFFFFFF << BTM_CMP_CMP_POS))

/*************** BTM1 Counter Register Bitfiled ******************/

#define BTM_RTCCNT_CNT_POS            ((uint32_t) 0)
#define BTM_RTCCNT_CNT                ((uint32_t) ((uint32_t)0xFFFFFFFF << BTM_RTCCNT_CNT_POS))

#define BTM1                          ((BTM_Device_T *)BASETIMER1_BASE)
#define BTM2                          ((BTM_Device_T *)BASETIMER2_BASE)

#ifdef __cplusplus
}
#endif

#endif //_BASETIMER_DEV_H_

