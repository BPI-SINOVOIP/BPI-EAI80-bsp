/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/02/25, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_LL_MMC_DEV_H_
#define _GM_LL_MMC_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MMC Host Control Register Layout Type Definition */
typedef struct {
    __IO uint32_t MMCCTRL;                        /* MMC control Register: 0x0 */
    __IO uint32_t ARG;                            /* MMC command argument Register Register: 0x4 */
    __IO uint32_t DBL;                            /* MMC data block length register: 0x8 */
    __IO uint32_t PIO;                            /* MMMC pio mode register: 0xc */
    __IO uint32_t RESP0;                          /* MMC command response register0: 0x10 */
    __IO uint32_t RESP1;                          /* MMC command response register1: 0x14 */
    __IO uint32_t RESP2;                          /* MMC command response register2: 0x18 */
    __IO uint32_t RESP3;                          /* MMC command response register3: 0x1C */
    __IO uint32_t SDIOCTRL;                       /* SDIO control register Register: 0x20 */
    __IO uint32_t IE;                             /* MMC interrput enable register : 0x24 */
    __IO uint32_t IS;                             /* MMC interrput status Register:  0x28 */
    __IO uint32_t DMADATA;                        /* MMC DMA data  Register: 0x2C */
    __IO uint32_t DMACTRL;                       /* MMC DMA contral Register: 0x30 */
    __IO uint32_t ADITLCTRL;                      /* MMC additional control register : 0x34 */
    __IO uint32_t DLYCHN;                         /* MMC clock delay chain control register( : 0x38 */
    __IO uint32_t CBC;                            /* MMC current block counter register : 0x40 */
    __IO uint32_t CBL;                            /* MMC current block length register : 0x44 */
    __IO uint32_t VER;                            /* MMC  IP version register(: 0x48 */

} MMC_Device_T;


#define MMC0                                    ((MMC_Device_T *)(MMC0_BASE))    /* MMC0 reg base */


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_MMC_DEV_H_ */
