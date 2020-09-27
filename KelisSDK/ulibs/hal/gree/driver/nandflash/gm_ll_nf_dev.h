/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_nf_dev.h
 *
 * @author        Colin.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/11/20
 *
 * @brief        
 *
 * @note
 *    2018/11/20, Colin.Yang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_NF_DEV_H_
#define _GM_LL_NF_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SQI Controller Register Layout Type Definition */
typedef struct {
    __IO uint32_t VS;                        /* NF  Version Register: 0x0 */
    __IO uint32_t RSV0;                      /* NF  Reserve 0: 0x4 */
    __IO uint32_t RSV1;                      /* NF  Reserve 1: 0x8 */
    __IO uint32_t RSV2;                      /* NF  Reserve 2£º 0xc */
    __IO uint32_t MR;                        /* NF  Mode Register: 0x10 */
    __IO uint32_t CR;                        /* NF Control Register: 0x14 */
    __IO uint32_t ECCCR;                     /* NF ECC Control Register: 0x18 */
    __IO uint32_t DMACR;                     /* NF DMA Control Register: 0x1C */
    __IO uint32_t DR;                        /* NF  Data Register: 0x20 */
    __IO uint32_t DMADR;                     /* NF DMA Data Register: 0x24 */
	__IO uint32_t RSV3;                      /* NF  Reserve 3£º 0x28 */
	__IO uint32_t RSV4;                      /* NF  Reserve 4£º 0x2c */
    __IO uint32_t RDTIMR;                    /* NF Read Timing Register: 0x30 */
	__IO uint32_t WRTIMR;                    /* NF Write Timing Register: 0x34 */
	__IO uint32_t EDORDTIMR;                 /* NF EDO Read TimingRegister: 0x38 */
	__IO uint32_t DQSTIMR;                   /* NF DQS Timing Register: 0x3C */
	__IO uint32_t FWREDUW0;                  /* NF FW Redundant Word 0 : 0x40 */
	__IO uint32_t FWREDUW1;                  /* NF FW Redundant Word 1 : 0x44 */
	__IO uint32_t RSV5;                      /* NF  Reserve 5£º 0x48 */
	__IO uint32_t IER;                       /* NF Interupt Enable Register: 0x4c */
	__IO uint32_t SR;                        /* NF Status Register: 0x50 */
	__IO uint32_t ECCSR;                     /* NF ECC Status Register: 0x54 */
	__IO uint32_t SECERRINFO;                /* NF Sector Error Info Register: 0x58*/
	__IO uint32_t RSV6;                      /* NF  Reserve 6£º 0x5C */
	__IO uint32_t FWRDREDUW0;                /* NF FW Read Redundant Word 0 : 0x60 */
	__IO uint32_t FWRDREDUW1;                /* NF FW Read Redundant Word 1 : 0x64 */
	__IO uint32_t RSV7;                      /* NF  Reserve 7£º 0x68 */
	__IO uint32_t RSV8;                      /* NF  Reserve 8£º 0x6C */
	__IO uint32_t LPCR;                      /* NF Low Power Control Register: 0x70 */
} NFlash_Device_T;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_NF_DEV_H_ */

