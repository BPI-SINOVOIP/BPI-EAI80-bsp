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


#include <misc/printk.h>
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
} HASH_Device_T;

/** Base address of crypto engine */
#define HASH_BASE          (0x0x50041200)

/** Crypto engine device */
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

#define HASH_CR_TRIG_Pos                 (8U)
#define HASH_CR_TRIG_Msk                 (0x1U << HASH_CR_TRIG_Pos)
#define HASH_CR_CALC_TRIG                HASH_CR_TRIG_Msk

#define HASH_CR_DMA_Pos                  (16U)
#define HASH_CR_DMA_Msk                  (0x1U << HASH_CR_DMA_Pos)
#define HASH_CR_DMA_EN                   HASH_CR_DMA_Msk

#define HASH_CR_MULDMA_Pos               (17U)
#define HASH_CR_MULDMA_Msk               (0x1U << HASH_CR_MULDMA_Pos)
#define HASH_CR_MULDMA_TRAN              HASH_CR_MULDMA_Msk

#define HASH_CR_DMALVL_Pos               (20U)
#define HASH_CR_DMALVL_MSK               (0xf << HASH_CR_DMALVL_Pos)
#define HASH_CR_DMA_LVL                  HASH_CR_DMALVL_MSK

/******************* Bits definition for H_IMR register  ********************/
#define HASH_IMR_DININT_Pos              (0U)
#define HASH_IMR_DININT_Msk              (0x1U << HASH_IMR_DININT_Pos)
#define HASH_IMR_DININT_EN               HASH_IMR_DININT_Msk

#define HASH_IMR_CALINT_Pos              (1U)
#define HASH_IMR_CALINT_Msk              (0x1U << HASH_IMR_CALINT_Msk)
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
#define HASH_SR_INNERMSK_FIN             HASH_SR_INNERMSG_Msk

#define HASH_SR_OUTERKEY_Pos             (18U)
#define HASH_SR_OUTERKEY_MSk             (0x1U << HASH_SR_OUTERKEY_Pos)
#define HASH_SR_OUTERKEY_FIN             HASH_SR_OUTERKEY_MSk

#define HASH_SR_OUTERMSG_Pos             (19U)
#define HASH_SR_OUTERMSG_Msk             (0x1U << HASH_SR_OUTERMSG_Pos)
#define HASH_SR_OUTERMSK_FIN             HASH_SR_OUTERMSG_Msk


#ifdef OS_ENABLE
/* Reserved for future use */
#error "USE_RTOS should be 0 in the current HAL release"
#else
#define HAL_LOCK(__HANDLE__)               \
    do{                                        \
        if((__HANDLE__)->lock == HAL_LOCKED)   \
        {                                      \
            return HAL_BUSY;                    \
        }                                      \
        else                                   \
        {                                      \
            (__HANDLE__)->lock = HAL_LOCKED;    \
        }                                      \
    }while (0U)

#define HAL_UNLOCK(__HANDLE__)                                          \
    do{                                       \
        (__HANDLE__)->lock = HAL_UNLOCKED;    \
    }while (0U)
#endif /* USE_RTOS */


//#endif

#ifdef __cplusplus
}
#endif

#endif
