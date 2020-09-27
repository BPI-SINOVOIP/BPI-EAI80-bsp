/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_wwdg_dev.h
 *
 * @author        kenny.cmt
 *
 * @version       1.0.0
 *
 * @date          2018/03/20
 *
 * @brief         Header file of wdog driver.
 *
 * @note
 *    2018/03/20    kenny.cmt     V1.0.0
 *        Update For GM6721

 */



#ifndef _GM_LL_WWDG_DEV_H_
#define _GM_LL_WWDG_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __IO  volatile

/** 
  * @brief WDOG Register Layout Type Definition
  */

/* WDOG Register Layout Type Definition */
typedef struct
{
    __IO uint32_t LD;                       /* WDOG Load Register:offset: 0x0 */
    __IO uint32_t VAL;                      /* WDOG Value Register:offset: 0x4 */
    __IO uint32_t CTRL;                     /* WDOG Control Register:offset 0x8 */
    __IO uint32_t IC;                       /* WDOG Clear Interrupt Register:offset: 0xc */
    __IO uint32_t RIS;                      /* WDOG Raw Interrupt Status Register:offset: 0x10 */
    __IO uint32_t MIS;                      /* WDOG Masked Interrupt Status Register:offset: 0x14 */
    __IO uint32_t WL;                       /* WDOG Window Load Register:offset:0x18*/
    __IO uint32_t RESV0[761];               /* WDOG Reserver Register:offset:0x1c~0xbfc */
    __IO uint32_t KR;                       /* WDOG Lock Register :offset: 0xc00 */
    __IO uint32_t RESV1[191];               /* WDOG Reserver Register:offset: 0xc04~0xefc */
    __IO uint32_t TMR;                      /* WDOG Integration Test Control Register:offset: 0xf00 */
    __IO uint32_t TMOR;                     /* WDOG IIntegration Test Output Set Register:offset: 0xf04 */
    __IO uint32_t RESV2[61];                /* WDOG Reserver Register:offset:0xf04~0xffc */
    __IO uint32_t ID;                       /* WDOG Version ID Register:offset:0xFFC*/
} WDOG_Device_T;

#define WWDG                            ((WDOG_Device_T *)WWDG_BASE)
#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_WWDG_DEV_H_ */


