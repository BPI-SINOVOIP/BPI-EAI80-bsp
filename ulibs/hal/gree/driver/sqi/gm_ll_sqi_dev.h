/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_sflash_dev.h
 *
 * @author        Colin.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief        
 *
 * @note
 *    2018/03/24, Colin.Yang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_SQI_DEV_H_
#define _GM_LL_SQI_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SQI Controller Register Layout Type Definition */
typedef struct {
    __IO uint32_t VS;                        /* SQI Version Register: 0x0 */
    __IO uint32_t BR;                        /* SQI Baud Rate Register: 0x4 */
    __IO uint32_t CFG;                       /* SQI Configuration Register 1: 0x8 */
    __IO uint32_t DMY;                       /* SQI Dummy Register£º 0xc */
    __IO uint32_t CNT;                       /* SQI Counter: 0x10 */
    __IO uint32_t ADDRCFG;                   /* SQI Address Configuration Register: 0x14 */
    __IO uint32_t TCR;                       /* SQI Timing Control Register: 0x18 */
    __IO uint32_t PREFCTRL;                  /* SQI Prefetch Control Register: 0x1C */
    __IO uint32_t CRCCTRL;                   /* SQI CRC Control Register: 0x20 */
    __IO uint32_t CRCIV;                     /* SQI CRC Initial Value Register: 0x24 */
    __IO uint32_t CRCRES;                    /* SQI CRC Result Register: 0x28 */
} SQI_Device_T;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_SQI_DEV_H_ */
