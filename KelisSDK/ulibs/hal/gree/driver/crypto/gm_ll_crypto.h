/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file             crypto_dev.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date          2018/02/26 
 *    
 * @brief          Header file of crypto engine module.
 * 
 * @note
 *    2018/02/26, terry.wu, V1.0.0
 *        Initial version.
 */
 
#ifndef _CRYPTO_DEV_H_
#define _CRYPTO_DEV_H_

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
  * @brief Crypto Processor
  */
typedef struct
{
	__IO uint32_t CRYP_VER;        /**< crypto engine version, offset:0x00 */
	__IO uint32_t reserved_0[3];
	__IO uint32_t ES_CR;           /**< DES/AES control register, offset:0x10 */
	__IO uint32_t ES_BL;           /**< Bit length of AAD/Payload used in AES-GCM, offset:0x14 */
	__IO uint32_t ES_SR;           /**< DES/AES status register, offset:0x18 */
	__IO uint32_t ES_DIN;          /**< DES/AES data input register, offset:0x1C */
	__IO uint32_t ES_DOUT;         /**< DES/AES data output register, offset:0x20 */
	__IO uint32_t ES_DMACR;        /**< DES/AES DMA control register, offset:0x24 */
	__IO uint32_t ES_IMR;          /**< DES/AES interrupt mask register, offset:0x28 */
	__IO uint32_t ES_ISR;          /**< DES/AES interrupt status register, offset:0x2C */
	__IO uint32_t ES_MIS;          /**< DES/AES masked interrupt status, offset:0x30 */
	__IO uint32_t reserved_3[3];
	__IO uint32_t ES_IV0LR;        /**< DES/AES initialization vector left-word  register 0, offset:0x40 */
	__IO uint32_t ES_IV0RR;        /**< DES/AES initialization vector right-word  register 0, offset:0x44 */
	__IO uint32_t ES_IV1LR;        /**< DES/AES initialization vector left-word  register 1, offset:0x48 */
	__IO uint32_t ES_IV1RR;        /**< DES/AES initialization vector right-word  register 1, offset:0x4C */
	__IO uint32_t CRYP_SK_CFG;     /**< CRYP secure key configuration register, offset:0x50 */
	__IO uint32_t CRYP_SK_CS;      /**< CRYP secure key control and status register, offset:0x54 */
	__IO uint32_t reserved_4[2];
	__IO uint32_t ES_K0LR;         /**< DES/AES key left register 0, offset:0x60 */
	__IO uint32_t ES_K0RR;         /**< DES/AES key right register 0, offset:0x64 */
	__IO uint32_t ES_K1LR;         /**< DES/AES key left register 1, offset:0x68 */
	__IO uint32_t ES_K1RR;         /**< DES/AES key right register 1, offset:0x6C */
	__IO uint32_t ES_K2LR;         /**< DES/AES key left register 2, offset:0x70 */
	__IO uint32_t ES_K2RR;         /**< DES/AES key right register 2, offset:0x74 */
	__IO uint32_t ES_K3LR;         /**< DES/AES key left register 3, offset:0x78 */
	__IO uint32_t ES_K3RR;         /**< DES/AES key right register 3, offset:0x7C */
	__IO uint32_t ES_OTP_K0L;      /**< DES/AES OTP key left register 0, offset:0x80 */
	__IO uint32_t ES_OTP_K0R;      /**< DES/AES OTP key left register 0, offset:0x84 */
	__IO uint32_t ES_OTP_K1L;      /**< DES/AES OTP key left register 1, offset:0x88 */
	__IO uint32_t ES_OTP_K1R;      /**< DES/AES OTP key left register 1, offset:0x8C */
	__IO uint32_t ES_OTP_K2L;      /**< DES/AES OTP key left register 2, offset:0x90 */
	__IO uint32_t ES_OTP_K2R;      /**< DES/AES OTP key left register 2, offset:0x94 */
	__IO uint32_t ES_OTP_K3L;      /**< DES/AES OTP key left register 3, offset:0x98 */
	__IO uint32_t ES_OTP_K3R;      /**< DES/AES OTP key left register 3, offset:0x9C */
	__IO uint32_t ES_KB0_0LR;      /**< DES/AES key buffer 0 left register 0, offset:0xA0 */
	__IO uint32_t ES_KB0_0RR;      /**< DES/AES key buffer 0 right register 0, offset:0xA4 */
	__IO uint32_t ES_KB0_1LR;      /**< DES/AES key buffer 0 left register 1, offset:0xA8 */
	__IO uint32_t ES_KB0_1RR;      /**< DES/AES key buffer 0 right register 1, offset:0xAC */
	__IO uint32_t ES_KB1_0LR;      /**< DES/AES key buffer 1 left register 0, offset:0xB0 */
	__IO uint32_t ES_KB1_0RR;      /**< DES/AES key buffer 1 right register 0, offset:0xB4 */
	__IO uint32_t ES_KB1_1LR;      /**< DES/AES key buffer 1 left register 1, offset:0xB8 */
	__IO uint32_t ES_KB1_1RR;      /**< DES/AES key buffer 1 right register 1, offset:0xBC */
	__IO uint32_t ES_KB2_0LR;      /**< DES/AES key buffer 2 left register 0, offset:0xC0 */
	__IO uint32_t ES_KB2_0RR;      /**< DES/AES key buffer 2 right register 0, offset:0xC4 */
	__IO uint32_t ES_KB2_1LR;      /**< DES/AES key buffer 2 left register 1, offset:0xC8 */
	__IO uint32_t ES_KB2_1RR;      /**< DES/AES key buffer 2 right register 1, offset:0xCC */
	__IO uint32_t ES_KB3_0LR;      /**< DES/AES key buffer 3 left register 0, offset:0xD0 */
	__IO uint32_t ES_KB3_0RR;      /**< DES/AES key buffer 3 right register 0, offset:0xD4 */
	__IO uint32_t ES_KB3_1LR;      /**< DES/AES key buffer 3 left register 1, offset:0xD8 */
	__IO uint32_t ES_KB3_1RR;      /**< DES/AES key buffer 3 right register 1, offset:0xDC */
	__IO uint32_t ES_KB4_0LR;      /**< DES/AES key buffer 4 left register 0, offset:0xE0 */
	__IO uint32_t ES_KB4_0RR;      /**< DES/AES key buffer 4 right register 0, offset:0xE4 */
	__IO uint32_t ES_KB4_1LR;      /**< DES/AES key buffer 4 left register 1, offset:0xE8 */
	__IO uint32_t ES_KB4_1RR;      /**< DES/AES key buffer 4 right register 1, offset:0xEC */
	__IO uint32_t ES_KB5_0LR;      /**< DES/AES key buffer 5 left register 0, offset:0xF0 */
	__IO uint32_t ES_KB5_0RR;      /**< DES/AES key buffer 5 right register 0, offset:0xF4 */
	__IO uint32_t ES_KB5_1LR;      /**< DES/AES key buffer 5 left register 1, offset:0xF8 */
	__IO uint32_t ES_KB5_1RR;      /**< DES/AES key buffer 5 right register 1, offset:0xFC */
	__IO uint32_t ES_KB6_0LR;      /**< DES/AES key buffer 6 left register 0, offset:0x100 */
	__IO uint32_t ES_KB6_0RR;      /**< DES/AES key buffer 6 right register 0, offset:0x104 */
	__IO uint32_t ES_KB6_1LR;      /**< DES/AES key buffer 6 left register 1, offset:0x108 */
	__IO uint32_t ES_KB6_1RR;      /**< DES/AES key buffer 6 right register 1, offset:0x10C */
	__IO uint32_t ES_KB7_0LR;      /**< DES/AES key buffer 7 left register 0, offset:0x110 */
	__IO uint32_t ES_KB7_0RR;      /**< DES/AES key buffer 7 right register 0, offset:0x114 */
	__IO uint32_t ES_KB7_1LR;      /**< DES/AES key buffer 7 left register 1, offset:0x118 */
	__IO uint32_t ES_KB7_1RR;      /**< DES/AES key buffer 7 right register 1, offset:0x11C */
}CRYPTO_Device_T;


/** Crypto engine device */
#define CRYPTO             ((CRYPTO_Device_T *)(CRYPTO_BASE))


/******************* Bits definition for ES_CR register  ********************/
#define CRYP_CR_CRYPEN_Pos               (0U)  
#define CRYP_CR_CRYPEN_Msk               (0x1U << CRYP_CR_CRYPEN_Pos)          /**< 0x00000001 */
#define CRYP_CR_CRYPEN                   CRYP_CR_CRYPEN_Msk     

#define CRYP_CR_ALGODIR_Pos              (8U)     
#define CRYP_CR_ALGODIR_Msk              (0x1U << CRYP_CR_ALGODIR_Pos) 
#define CRYP_CR_ALGODIR_ENC              (0x00000000)
#define CRYP_CR_ALGODIR_DEC              CRYP_CR_ALGODIR_Msk                   /**< 0x00000100 */

#define CRYP_CR_ALGOSEL_Pos              (9U)
#define CRYP_CR_ALGOSEL_Msk              (0x7 << CRYP_CR_ALGOSEL_Pos)
#define CRYP_CR_ALGOSEL_AES              (0x00000000)
#define CRYP_CR_ALGOSEL_TDES             (0x1U << CRYP_CR_ALGOSEL_Pos)         /**< 0x00000200 */
#define CRYP_CR_ALGOSEL_DES              (0x2U << CRYP_CR_ALGOSEL_Pos)         /**< 0x00000400 */

#define CRYP_CR_ALGOMODE_Pos             (12U)
#define CRYP_CR_ALGOMODE_Msk             (0xf << CRYP_CR_ALGOMODE_Pos)
#define CRYP_CR_ALGOMODE_ECB             (0x00000000)
#define CRYP_CR_ALGOMODE_CBC             (0x1U << CRYP_CR_ALGOMODE_Pos)
#define CRYP_CR_ALGOMODE_CTR             (0x4U << CRYP_CR_ALGOMODE_Pos)
#define CRYP_CR_ALGOMODE_GCM             (0x8U << CRYP_CR_ALGOMODE_Pos)
#define CRYP_CR_ALGOMODE_CCM             (0x9U << CRYP_CR_ALGOMODE_Pos)

#define CRYP_CR_DATATYPE_Pos             (16U)                                  
#define CRYP_CR_DATATYPE_Msk             (0x3U << CRYP_CR_DATATYPE_Pos)
#define CRYP_CR_DATATYPE_32BIT           (0x00000000)
#define CRYP_CR_DATATYPE_16BIT           (0x1U << CRYP_CR_DATATYPE_Pos)
#define CRYP_CR_DATATYPE_8BIT            (0x2U << CRYP_CR_DATATYPE_Pos)
#define CRYP_CR_DATATYPE_1BIT            (0x3U << CRYP_CR_DATATYPE_Pos)

#define CRYP_CR_KEYSIZE_Pos              (18U)                                  
#define CRYP_CR_KEYSIZE_Msk              (0x3U << CRYP_CR_KEYSIZE_Pos)
#define CRYP_CR_KEYSIZE_128BIT           (0x00000000)                   
#define CRYP_CR_KEYSIZE_192BIT           (0x1U << CRYP_CR_KEYSIZE_Pos)
#define CRYP_CR_KEYSIZE_256BIT           (0x2U << CRYP_CR_KEYSIZE_Pos)

#define CRYP_CR_FFLUSH_Pos               (20U)                                 
#define CRYP_CR_FFLUSH_Msk               (0x1U << CRYP_CR_FFLUSH_Pos)
#define CRYP_CR_FFLUSH                   CRYP_CR_FFLUSH_Msk                    

#define CRYP_CR_GCM_CCMPH_Pos            (24U)          
#define CRYP_CR_GCM_CCMPH_Msk            (0x3U << CRYP_CR_GCM_CCMPH_Pos)
#define CRYP_CR_GCM_CCMPH_0              (0x00000000)                 
#define CRYP_CR_GCM_CCMPH_1              (0x1U << CRYP_CR_GCM_CCMPH_Pos)
#define CRYP_CR_GCM_CCMPH_2              (0x2U << CRYP_CR_GCM_CCMPH_Pos)     
#define CRYP_CR_GCM_CCMPH_3              (0x3U << CRYP_CR_GCM_CCMPH_Pos)

/****************** Bits definition for ES_SR register  *********************/
#define CRYP_SR_IFEM_Pos                 (0U)                                  
#define CRYP_SR_IFEM_Msk                 (0x1U << CRYP_SR_IFEM_Pos)            /**< 0x00000001 */
#define CRYP_SR_IFEM                     CRYP_SR_IFEM_Msk  

#define CRYP_SR_IFNF_Pos                 (1U)                                  
#define CRYP_SR_IFNF_Msk                 (0x1U << CRYP_SR_IFNF_Pos)            /**< 0x00000002 */
#define CRYP_SR_IFNF                     CRYP_SR_IFNF_Msk     

#define CRYP_SR_OFNE_Pos                 (2U)                                  
#define CRYP_SR_OFNE_Msk                 (0x1U << CRYP_SR_OFNE_Pos)            /**< 0x00000004 */
#define CRYP_SR_OFNE                     CRYP_SR_OFNE_Msk 

#define CRYP_SR_OFFU_Pos                 (3U)                                  
#define CRYP_SR_OFFU_Msk                 (0x1U << CRYP_SR_OFFU_Pos)            /**< 0x00000008 */
#define CRYP_SR_OFFU                     CRYP_SR_OFFU_Msk   

#define CRYP_SR_BUSY_Pos                 (4U)                                  
#define CRYP_SR_BUSY_Msk                 (0x1U << CRYP_SR_BUSY_Pos)            /**< 0x00000010 */
#define CRYP_SR_BUSY                     CRYP_SR_BUSY_Msk           

/****************** Bits definition for ES_DMACR register  ******************/
#define CRYP_DMACR_DIEN_Pos              (0U)                                  
#define CRYP_DMACR_DIEN_Msk              (0x1U << CRYP_DMACR_DIEN_Pos)         /**< 0x00000001 */
#define CRYP_DMACR_DIEN                  CRYP_DMACR_DIEN_Msk 

#define CRYP_DMACR_DMATXLVL_Pos          (4U)
#define CRYP_DMACR_DMATXLVL_Msk          (0xf << CRYP_DMACR_DMATXLVL_Pos)

#define CRYP_DMACR_DOEN_Pos              (16U)                                  
#define CRYP_DMACR_DOEN_Msk              (0x1U << CRYP_DMACR_DOEN_Pos)         /**< 0x00000002 */
#define CRYP_DMACR_DOEN                  CRYP_DMACR_DOEN_Msk        

#define CRYP_DMACR_DMARXLVL_Pos          (20U)
#define CRYP_DMACR_DMARXLVL_Msk          (0xf << CRYP_DMACR_DMARXLVL_Pos)

/*****************  Bits definition for ES_IMR register  ******************/
#define CRYP_IMR_INFIFOIM_Pos            (0U)                                  
#define CRYP_IMR_INFIFOIM_Msk            (0x1U << CRYP_IMR_INFIFOIM_Pos)         /**< 0x00000001 */
#define CRYP_IMR_INFIFOIM                CRYP_IMR_INFIFOIM_Msk   

#define CRYP_IMR_OUTFIFOIM_Pos           (1U)                                  
#define CRYP_IMR_OUTFIFOIM_Msk           (0x1U << CRYP_IMR_OUTFIFOIM_Pos)        /**< 0x00000002 */
#define CRYP_IMR_OUTFIFOIM               CRYP_IMR_OUTFIFOIM_Msk                  

/****************** Bits definition for ES_ISR register  *******************/
#define CRYP_ISR_INRIS_Pos              (0U)                                  
#define CRYP_ISR_INRIS_Msk              (0x1U << CRYP_ISR_INRIS_Pos)         /**< 0x00000001 */
#define CRYP_ISR_INRIS                  CRYP_ISR_INRIS_Msk                   

#define CRYP_ISR_OUTRIS_Pos             (1U)                                  
#define CRYP_ISR_OUTRIS_Msk             (0x1U << CRYP_ISR_OUTRIS_Pos)        /**< 0x00000002 */
#define CRYP_ISR_OUTRIS                 CRYP_ISR_OUTRIS_Msk                  

/****************** Bits definition for CRYP_SK_CFG register  *********************/
#define CRYP_SKCFG_KEY_FROM_Pos          (0U)
#define CRYP_SKCFG_KEY_FROM_Msk          (0x3U << CRYP_SKCFG_KEY_FROM_Pos)
#define CRYP_SKCFG_KEY_FROM_REG          (0x00000000)
#define CRYP_SKCFG_KEY_FROM_OTP          (0x1U << CRYP_SKCFG_KEY_FROM_Pos)
#define CRYP_SKCFG_KEY_FROM_KEYBUF       (0x2U << CRYP_SKCFG_KEY_FROM_Pos)

#define CRYP_SKCFG_OUT_SEL_Pos           (2U)
#define CRYP_SKCFG_OUT_SEL_Msk           (0x1U << CRYP_SKCFG_OUT_SEL_Pos)
#define CRYP_SKCFG_OUT_TO_DOUT           (0x00000000)
#define CRYP_SKCFG_OUT_TO_KEYBUF         (0x1U << CRYP_SKCFG_OUT_SEL_Pos)

#define CRYP_SKCFG_HMACKEY_FROM_Pos      (3U)
#define CRYP_SKCFG_HMACKEY_FROM_Msk      (0x1U << CRYP_SKCFG_HMACKEY_FROM_Pos)
#define CRYP_SKCFG_HMACKEY_FROM_DIN      (0x00000000)
#define CRYP_SKCFG_HMACKEY_FROM_OTP      (0x1U << CRYP_SKCFG_HMACKEY_FROM_Pos)

#define CRYP_SKCFG_KEYBUF_RD_SEL_Pos     (8U)
#define CRYP_SKCFG_KEYBUF_RD_SEL_Msk     (0xf << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K0        (0x00000000)
#define CRYP_SKCFG_KEYBUF_FROM_K1        (0x1U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K2        (0x2U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K3        (0x3U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K4        (0x4U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K5        (0x5U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K6        (0x6U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_FROM_K7        (0x7U << CRYP_SKCFG_KEYBUF_RD_SEL_Pos)

#define CRYP_SKCFG_KEYBUF_KEY3SEL_Pos    (12U)
#define CRYP_SKCFG_KEYBUF_KEY3SEL_Msk    (0x1U << CRYP_SKCFG_KEYBUF_KEY3SEL_Pos)
#define CRYP_SKCFG_KEYBUF_KEY3SEL_LOW    (0x00000000)
#define CRYP_SKCFG_KEYBUF_KEY3SEL_HIGH   (0x1U << CRYP_SKCFG_KEYBUF_KEY3SEL_Pos)

#define CRYP_SKCFG_KEYBUF_WR_SEL_Pos     (16U)
#define CRYP_SKCFG_KEYBUF_WR_SEL_Msk     (0xf << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K0_LOW      (0x00000000)
#define CRYP_SKCFG_KEYBUF_TO_K0_HIGH     (0x1U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K1_LOW      (0x2U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K1_HIGH     (0x3U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K2_LOW      (0x4U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K2_HIGH     (0x5U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K3_LOW      (0x6U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K3_HIGH     (0x7U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K4_LOW      (0x8U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K4_HIGH     (0x9U << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K5_LOW      (0xAU << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K5_HIGH     (0xBU << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K6_LOW      (0xCU << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K6_HIGH     (0xDU << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K7_LOW      (0xEU << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)
#define CRYP_SKCFG_KEYBUF_TO_K7_HIGH     (0xFU << CRYP_SKCFG_KEYBUF_WR_SEL_Pos)

#define CRYP_SKCFG_OTP_SEL_Pos           (24U)
#define CRYP_SKCFG_OTP_SEL_Msk           (0xf << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K0                (0x00000000)
#define CRYP_SKCFG_OTP_K1                (0x1U << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K2                (0x2U << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K3                (0x3U << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K4                (0x4U << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K5                (0x5U << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K6                (0x6U << CRYP_SKCFG_OTP_SEL_Pos)
#define CRYP_SKCFG_OTP_K7                (0x7U << CRYP_SKCFG_OTP_SEL_Pos)

#define CRYP_SKCFG_HMACKEY_SEL_Pos       (28U)
#define CRYP_SKCFG_HMACKEY_SEL_Msk       (0x3U << CRYP_SKCFG_HMACKEY_SEL_Pos)
#define CRYP_SKCFG_HMACKEY_OTP_KEY6      (0x00000000)
#define CRYP_SKCFG_HMACKEY_OTP_KEY7      (0x1U << CRYP_SKCFG_HMACKEY_SEL_Pos)
#define CRYP_SKCFG_HMACKEY_OTP_KEY6_7    (0x2U << CRYP_SKCFG_HMACKEY_SEL_Pos)

#define CRYP_SKCFG_OTP_64_SEL_Pos        (31U)
#define CRYP_SKCFG_OTP_64_SEL_Msk        ((uint32_t)(0x1U << CRYP_SKCFG_OTP_64_SEL_Pos))
#define CRYP_SKCFG_OTP_LOW64             (0x00000000)
#define CRYP_SKCFG_OTP_HIGH64            (0x1U << CRYP_SKCFG_OTP_64_SEL_Pos)

/****************** Bits definition for CRYP_SK_CS register  *********************/
#define CRYP_SKCS_AES_GETKEY_Pos         (0U)
#define CRYP_SKCS_AES_GETKEY_Msk         (0x1U << CRYP_SKCS_AES_GETKEY_Pos)

#define CRYP_SKCS_DES_GETKEY_Pos         (8U)
#define CRYP_SKCS_DES_GETKEY_Msk         (0x1U << CRYP_SKCS_DES_GETKEY_Pos)

#define CRYP_SKCS_HMAC_GETKEY_Pos        (16U)
#define CRYP_SKCS_HMAC_GETKEY_Msk        (0x1U << CRYP_SKCS_HMAC_GETKEY_Pos)

#define CRYP_SKCS_DBGKEYSEL_Pos          (24U)
#define CRYP_SKCS_DBGKEYSEL_Msk          (0x3U << CRYP_SKCS_DBGKEYSEL_Pos)
#define CRYP_SKCS_DBGKEYSEL_DES          (0x00000000)
#define CRYP_SKCS_DBGKEYSEL_AES          (0x1U << CRYP_SKCS_DBGKEYSEL_Pos)
#define CRYP_SKCS_DBGKEYSEL_HMAC         (0x2U << CRYP_SKCS_DBGKEYSEL_Pos)



/** @defgroup CRYP_Interrupt
  * @{
  */
#define CRYP_IT_INFIFO               ((uint32_t)CRYP_IMR_INFIFOIM)   /**< Input FIFO Interrupt */
#define CRYP_IT_OUTFIFO              ((uint32_t)CRYP_IMR_OUTFIFOIM)  /**< Output FIFO Interrupt */
/**
  * @}
  */

/** @defgroup CRYP_interrupt_status
 * @{
 */
#define CRYP_ITSTATUS_INFIFO         ((uint32_t)CRYP_ISR_INRIS)      /**< Input FIFO interrupt is pending */
#define CRYP_ITSTATUS_OUTFIFO        ((uint32_t)CRYP_ISR_OUTRIS)    /**< Ouput FIFO interrupt is pending */
/**
 * @}
 */

/** @defgroup CRYP_status_flag
 * @{
 */
#define CRYP_FLAG_BUSY               ((uint32_t)CRYP_SR_BUSY)        /**< The CRYP core is currently processing a block of data 
											                                                   or a key preparation (for AES decryption). */
#define CRYP_FLAG_IFEM               ((uint32_t)CRYP_SR_IFEM)        /**< Input FIFO is empty */
#define CRYP_FLAG_IFNF               ((uint32_t)CRYP_SR_IFNF)        /**< Input FIFO is not Full */
#define CRYP_FLAG_OFNE               ((uint32_t)CRYP_SR_OFNE)        /**< Output FIFO is not empty */
#define CRYP_FLAG_OFFU               ((uint32_t)CRYP_SR_OFFU)        /**< Output FIFO is Full */
/**
 *@}
 */

/** @defgroup CRYP_GCMCCM_PHASE
 * @{
 */
#define CRYP_GCMCCM_PHASE_INIT       ((uint32_t)CRYP_CR_GCM_CCMPH_0) /**< The init phase */
#define CRYP_GCMCCM_PHASE_HEADER     ((uint32_t)CRYP_CR_GCM_CCMPH_1) /**< The header phase */
#define CRYP_GCMCCM_PHASE_PAYLOAD    ((uint32_t)CRYP_CR_GCM_CCMPH_2) /**< The payload phase */
#define CRYP_GCMCCM_PHASE_FINAL      ((uint32_t)CRYP_CR_GCM_CCMPH_3) /**< The final phase */
/**
 *@}
 */



/**
  * @brief  Set the algorithm mode.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param algo: Select algorithm, AES, DES/TDES.
  * @param  mode: The algorithm mode.
  * @param dir: encrypt or decrypt.
  * @retval None
  */
#define HAL_CRYP_SET_MODE(__HANDLE__, algo, mode, dir)   \
	do{                                         \
		IO_BITMASK_SET((__HANDLE__)->Instance->ES_CR, CRYP_CR_ALGOSEL_Msk, algo);     \
	    IO_BITMASK_SET((__HANDLE__)->Instance->ES_CR, CRYP_CR_ALGOMODE_Msk, mode);    \
	    IO_BITMASK_SET((__HANDLE__)->Instance->ES_CR, CRYP_CR_ALGODIR_Msk, dir);      \
	}while (0U)

/**
  * @brief	Flush the data FIFO.
  * @param	__HANDLE__: specifies the CRYP handle.
  * @retval None
  */
#define HAL_CRYP_FLUSH_FIFO(__HANDLE__)      (IO_BIT_SET((__HANDLE__)->Instance->ES_CR, CRYP_CR_FFLUSH))

/**
  * @brief  Enable/Disable the CRYP peripheral.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @retval None
  */
#define HAL_CRYP_ENABLE(__HANDLE__)          (IO_BIT_SET((__HANDLE__)->Instance->ES_CR, CRYP_CR_CRYPEN))
#define HAL_CRYP_DISABLE(__HANDLE__)         (IO_BIT_CLEAR((__HANDLE__)->Instance->ES_CR, CRYP_CR_CRYPEN))

/**
  * @brief Get the algorithm of the CRYP peripheral.
  * @param __HANDLE__: specifies the CRYP handle.
  * @retval the algorithm current use.
  */
#define HAL_CRYP_GET_ALGO(__HANDLE__)        ((IO_BITS_GET((__HANDLE__)->Instance->ES_CR, CRYP_CR_ALGOSEL_Msk)) >> CRYP_CR_ALGOSEL_Pos)

/**
  * @brief  Enable the CRYP interrupt.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  irq: CRYP Interrupt.
  * @retval None
  */
#define HAL_CRYP_ENABLE_IT(__HANDLE__, irq)   (IO_BITS_SET((__HANDLE__)->Instance->ES_IMR, irq))

/**
  * @brief  Disable the CRYP interrupt.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  __INTERRUPT__: CRYP interrupt.
  * @retval None
  */  
#define HAL_CRYP_DISABLE_IT(__HANDLE__, irq)  (IO_BIT_CLEAR((__HANDLE__)->Instance->ES_IMR, irq))

/** @brief  Check whether the specified CRYP interrupt is set or not.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  irq: specifies the interrupt to check.
  *         This parameter can be one of the following values:
  *            @arg CRYP_ITSTATUS_INFIFO: Input FIFO service raw interrupt is pending
  *            @arg CRYP_ITSTATUS_OUTFIFO: Output FIFO service raw interrupt is pending
  * @retval The new state of irq (TRUE or FALSE).
  */
#define HAL_CRYP_GET_ITSTATUS(__HANDLE__, irq) ((IO_BIT_GET((__HANDLE__)->Instance->ES_ISR, irq)) == irq)

/** @brief  Check whether the specified CRYP flag is set or not.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  flag: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg CRYP_FLAG_BUSY: The CRYP core is currently processing a block of data 
  *                                 or a key preparation (for AES decryption). 
  *            @arg CRYP_FLAG_IFEM: Input FIFO is empty
  *            @arg CRYP_FLAG_IFNF: Input FIFO is not full
  *            @arg CRYP_FLAG_OFNE: Output FIFO is not empty
  *            @arg CRYP_FLAG_OFFU: Output FIFO is full
  * @retval The new state of flag (TRUE or FALSE).
  */
#define HAL_CRYP_GET_FLAG(__HANDLE__, flag)    ((IO_BIT_GET((__HANDLE__)->Instance->ES_SR, flag)) == flag)

/**
  * @brief  Set the phase: Init, header, payload, final. 
  *         This is relevant only for GCM and CCM modes.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  phase: The phase.
  * @retval None
  */
#define HAL_CRYP_GCMCCM_SET_PHASE(__HANDLE__, phase)  (IO_BITMASK_SET((__HANDLE__)->Instance->ES_CR, CRYP_CR_GCM_CCMPH_Msk, phase))

/**
  * @brief  request to read key from OTP, Key Buffer or Key Register.
  * @param  __HANDLE__: specifies the CRYP handle.
  * @param  algo: AES/TDES,DES,HAMC
  *            @arg CRYP_SKCS_AES_GETKEY_Msk
  *            @arg CRYP_SKCS_DES_GETKEY_Msk
  *            @arg CRYP_SKCS_HMAC_GETKEY_Msk
  * @retval None
  */
#define HAL_CRYP_READ_KEY_REQUEST(__HANDLE__, algo)   (IO_BITS_SET((__HANDLE__)->Instance->CRYP_SK_CS, algo))

//#endif

#ifdef __cplusplus
}
#endif

#endif
