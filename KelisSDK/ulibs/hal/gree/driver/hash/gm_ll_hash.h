/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file             hash.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date          2018/03/13
 *    
 * @brief          Header file of hash module.
 * 
 * @note
 *    2018/03/13, terry.wu, V1.0.0
 *        Initial version.
 */

#ifndef _HASH_DEV_H_
#define _HASH_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  RABBIT_NO_OS_SYSTEM
#include <misc/printk.h>
#endif

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_basic_types.h>

//#ifdef CHIP_GM6721


/*-----------------------
 * Print macros
 *-----------------------*/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
#define PR_DEBUG(...)       do {} while (0)
#else
#define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
#define PR_ERR(...)        do {} while (0)
#else
#define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
#define PR_WARNINR(...)    do{} while (0)
#else
#define PR_WARNINR         printk
#endif

/** 
  * @brief Hash Processor
  */
typedef struct
{
	__IO uint32_t H_CFG;                  /**< HASH configuration register, Address offset: 0x00 */
	__IO uint32_t H_CR;                   /**< HASH control register, Address offset: 0x04 */
	__IO uint32_t H_DIN;                  /**< HASH data input register, Address offset: 0x08 */
	__IO uint32_t H_KEYLEN;               /**< HMAC key length register, Address offset: 0x0C */
	__IO uint32_t H_MSGLEN[4];            /**< HASH message length register, Address offset: 0x10~0x1C */
	__IO uint32_t H_DIGEST[16];           /**< HASH digest register, Address offset: 0x20~0x5C */
	__IO uint32_t H_IMR;                  /**< HASH interrupt mask register, Address offset: 0x60 */
	__IO uint32_t H_INTST;                /**< HASH interrupt status register, Address offset: 0x64 */
	__IO uint32_t H_SR;                   /**< HASH status register, Address offset: 0x68 */
	uint32_t reserved;
	__IO uint32_t H_BLKST[4];             /**< HASH block status register, Address offset: 0x70~0x7C */
	__IO uint32_t H_SK_CFG;               /**< HASH secure key configuration, Address offset: 0x80 */
	__IO uint32_t H_SK_CS;                /**< HASH secure key control and status, Address offset:0x84 */
	uint32_t reserved1[6];
	__IO uint32_t H_OTP_K0L;              /**< HASH OTP key left register 0, offset:0x100 */
	__IO uint32_t H_OTP_K0R;              /**< HASH OTP key left register 0, offset:0x104 */
	__IO uint32_t H_OTP_K1L;              /**< HASH OTP key left register 1, offset:0x108 */
	__IO uint32_t H_OTP_K1R;              /**< HASH OTP key left register 1, offset:0x10C */
	__IO uint32_t H_OTP_K2L;              /**< HASH OTP key left register 2, offset:0x110 */
	__IO uint32_t H_OTP_K2R;              /**< HASH OTP key left register 2, offset:0x114 */
	__IO uint32_t H_OTP_K3L;              /**< HASH OTP key left register 3, offset:0x118 */
	__IO uint32_t H_OTP_K3R;              /**< HASH OTP key left register 3, offset:0x11C */
}HASH_Device_T;


/** HASH device */
#define HASH               ((HASH_Device_T *)(HASH_BASE))


/******************* Bits definition for H_CFG register  ********************/
#define HASH_CFG_MODE_Pos                (0U)  
#define HASH_CFG_MODE_Msk                (0x1U << HASH_CFG_MODE_Pos)
#define HASH_CFG_MODE_HASH               (0x00000000)
#define HASH_CFG_MODE_HMAC               HASH_CFG_MODE_Msk

#define HASH_CFG_ALGO_Pos                (8U)
#define HASH_CFG_ALGO_Msk                (0x7U << HASH_CFG_ALGO_Pos)
#define HASH_CFG_ALGO_SHA1               (0x00000000)
#define HASH_CFG_ALGO_SHA224             (0x1U << HASH_CFG_ALGO_Pos)
#define HASH_CFG_ALGO_SHA256             (0x2U << HASH_CFG_ALGO_Pos)
#define HASH_CFG_ALGO_SHA384             (0x3U << HASH_CFG_ALGO_Pos)
#define HASH_CFG_ALGO_SHA512             (0x4U << HASH_CFG_ALGO_Pos)

#define HASH_CFG_DATATYPE_Pos            (16U)                                  
#define HASH_CFG_DATATYPE_Msk            (0x3U << HASH_CFG_DATATYPE_Pos)
#define HASH_CFG_DATATYPE_32BIT          (0x00000000)
#define HASH_CFG_DATATYPE_16BIT          (0x1 << HASH_CFG_DATATYPE_Pos)
#define HASH_CFG_DATATYPE_8BIT           (0x2 << HASH_CFG_DATATYPE_Pos)
#define HASH_CFG_DATATYPE_1BIT           (0x3 << HASH_CFG_DATATYPE_Pos)

/******************* Bits definition for H_CR register  ********************/
#define HASH_CR_INIT_Pos                 (0U)
#define HASH_CR_INIT_Msk                 (0x1U << HASH_CR_INIT_Pos)
#define HASH_CR_INIT_HASH                HASH_CR_INIT_Msk

#define HASH_CR_DMA_Pos                  (16U)
#define HASH_CR_DMA_Msk                  (0x1U << HASH_CR_DMA_Pos)
#define HASH_CR_DMA_EN                   HASH_CR_DMA_Msk

/******************* Bits definition for H_IMR register  ********************/
#define HASH_IMR_DININT_Pos              (0U)
#define HASH_IMR_DININT_Msk              (0x1U << HASH_IMR_DININT_Pos)
#define HASH_IMR_DININT_EN               HASH_IMR_DININT_Msk

#define HASH_IMR_CALINT_Pos              (1U)
#define HASH_IMR_CALINT_Msk              (0x1U << HASH_IMR_CALINT_Pos)
#define HASH_IMR_CALINT_EN               HASH_IMR_CALINT_Msk

/******************* Bits definition for H_INTST register  ********************/
#define HASH_INTST_DININT_Pos            (0U)
#define HASH_INTST_DININT_Msk            (0x1U << HASH_INTST_DININT_Pos)
#define HASH_INTST_DININT_SET            HASH_INTST_DININT_Msk

#define HASH_INTST_CALINT_Pos            (1U)
#define HASH_INTST_CALINT_Msk            (0x1U << HASH_INTST_CALINT_Pos)
#define HASH_INTST_CALINT_SET            HASH_INTST_CALINT_Msk

/******************* Bits definition for H_SR register  ********************/
#define HASH_SR_BUSY_Pos                 (0U)
#define HASH_SR_BUSY_Msk                 (0x1U << HASH_SR_BUSY_Pos)
#define HASH_SR_BUSY                     HASH_SR_BUSY_Msk

#define HASH_SR_DMATRAN_Pos              (1U)
#define HASH_SR_DMATRAN_Msk              (0x1U << HASH_SR_DMATRAN_Pos)
#define HASH_SR_DMATRAN_ONGING           HASH_SR_DMATRAN_Msk

#define HASH_SR_IBUFNOTEMPTY_Pos         (2U)
#define HASH_SR_IBUFNOTEMPTY_MSk         (0x1U << HASH_SR_IBUFNOTEMPTY_Pos)
#define HASH_SR_IBUF_NOTEMPTY            HASH_SR_IBUFNOTEMPTY_MSk

#define HASH_SR_INNERKEY_Pos             (16U)
#define HASH_SR_INNERKEY_MSk             (0x1U << HASH_SR_INNERKEY_Pos)
#define HASH_SR_INNERKEY_FIN             HASH_SR_INNERKEY_MSk

#define HASH_SR_INNERMSG_Pos             (17U)
#define HASH_SR_INNERMSG_Msk             (0x1U << HASH_SR_INNERMSG_Pos)
#define HASH_SR_INNERMSG_FIN             HASH_SR_INNERMSG_Msk

#define HASH_SR_OUTERKEY_Pos             (18U)
#define HASH_SR_OUTERKEY_MSk             (0x1U << HASH_SR_OUTERKEY_Pos)
#define HASH_SR_OUTERKEY_FIN             HASH_SR_OUTERKEY_MSk

#define HASH_SR_OUTERMSG_Pos             (19U)
#define HASH_SR_OUTERMSG_Msk             (0x1U << HASH_SR_OUTERMSG_Pos)
#define HASH_SR_OUTERMSG_FIN             HASH_SR_OUTERMSG_Msk

/******************* Bits definition for H_SK_CFG register  ********************/
#define HASH_SKCFG_HMACKEY_FROM_Pos      (3U)
#define HASH_SKCFG_HMACKEY_FROM_Msk      (0x1U << HASH_SKCFG_HMACKEY_FROM_Pos)
#define HASH_SKCFG_HMACKEY_FROM_DIN      (0x00000000)
#define HASH_SKCFG_HMACKEY_FROM_OTP      (0x1U << HASH_SKCFG_HMACKEY_FROM_Pos)

#define HASH_SKCFG_HMACKEY_SEL_Pos       (28U)
#define HASH_SKCFG_HMACKEY_SEL_Msk       (0x3 << HASH_SKCFG_HMACKEY_SEL_Pos)
#define HASH_SKCFG_HMACKEY_OTP_KEY6      (0x00000000)
#define HASH_SKCFG_HMACKEY_OTP_KEY7      (0x1 << HASH_SKCFG_HMACKEY_SEL_Pos)
#define HASH_SKCFG_HMACKEY_OTP_KEY6_7    (0x2 << HASH_SKCFG_HMACKEY_SEL_Pos)

/******************* Bits definition for H_SK_CS register  ********************/
#define HASH_SKCS_HMAC_GETKEY_Pos        (16U)
#define HASH_SKCS_HMAC_GETKEY_Msk        (0x1 << HASH_SKCS_HMAC_GETKEY_Pos)

#define HASH_SKCS_DBGKEYSEL_Pos          (24U)
#define HASH_SKCS_DBGKEYSEL_Msk          (0x3 << HASH_SKCS_DBGKEYSEL_Pos)
#define HASH_SKCS_DBGKEYSEL_DES          (0x00000000)
#define HASH_SKCS_DBGKEYSEL_AES          (0x1 << HASH_SKCS_DBGKEYSEL_Pos)
#define HASH_SKCS_DBGKEYSEL_HMAC         (0x2 << HASH_SKCS_DBGKEYSEL_Pos)


/**
  * @brief  Enable the HASH interrupt.
  * @param  __HANDLE__: specifies the HASH handle.
  * @param  irq: HASH Interrupt.
  *        This parameter can be one of the following values:
  *        @arg HASH_IMR_DININT_EN: data input interrupt.
  *        @arg HASH_IMR_CALINT_EN: digest calculation completion interrupt.
  * @retval None
  */
#define HAL_HASH_ENABLE_IT(__HANDLE__, irq)   (IO_BITS_SET((__HANDLE__)->Instance->H_IMR, irq))

/**
  * @brief  Disable the HASH interrupt.
  * @param  __HANDLE__: specifies the HASH handle.
  * @param  irq: HASH Interrupt.
  *        This parameter can be one of the following values:
  *        @arg HASH_IMR_DININT_EN: data input interrupt.
  *        @arg HASH_IMR_CALINT_EN: digest calculation completion interrupt.
  * @retval None
  */
#define HAL_HASH_DISABLE_IT(__HANDLE__, irq)   (IO_BIT_CLEAR((__HANDLE__)->Instance->H_IMR, irq))

/** @brief  Check whether the specified HASH interrupt is set or not.
  * @param  __HANDLE__: specifies the HASH handle.
  * @param  irq: specifies the interrupt to check.
  *         This parameter can be one of the following values:
  *            @arg HASH_INTST_DININT_SET: data input interrupt status
  *            @arg HASH_INTST_CALINT_SET: digest calculation completion interrupt status
  * @retval The new state of irq (TRUE or FALSE).
  */
#define HAL_HASH_GET_ITSTATUS(__HANDLE__, irq) ((IO_BIT_GET((__HANDLE__)->Instance->H_INTST, irq)) == irq)

/** @brief  clear the specified HASH interrupt.
  * @param  __HANDLE__: specifies the HASH handle.
  * @param  irq: specifies the interrupt to clear.
  *         This parameter can be one of the following values:
  *            @arg HASH_INTST_DININT_SET: data input interrupt status
  *            @arg HASH_INTST_CALINT_SET: digest calculation completion interrupt status
  * @retval The new state of irq (TRUE or FALSE).
  */
#define HAL_HASH_CLEAR_ITSTATUS(__HANDLE__, irq) (IO_WRITE_REG((__HANDLE__)->Instance->H_INTST, irq))


//#endif

#ifdef __cplusplus
}
#endif

#endif
