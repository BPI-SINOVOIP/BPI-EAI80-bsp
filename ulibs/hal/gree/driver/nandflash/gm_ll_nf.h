/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_nf.h
 *
 * @author        Colin.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/11/24
 *
 * @brief         
 *
 * @note
 *    2018/11/24, Colin.Yang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_NF_H_
#define _GM_LL_NF_H_
#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_nf_dev.h"


#ifdef __cplusplus
extern "C" {
#endif


#define NF_ADDR_BASE                                  0x70000000//这个具体要放到哪个文件里面，待定
#define NFLASH                                        ((NFlash_Device_T *)(NF_ADDR_BASE))    /* nand flash controllers reg base */

/**************************** NF CTRL Version register bitfiled *********************************/


/**************************** NF Mode register bitfiled *********************************/

#define NF_SYNC_MODE_ENABLE_SET_POS                   ((uint32_t)25)
#define NF_SYNC_MODE_ENABLE_SET                       ((uint32_t)((uint32_t)1 << NF_SYNC_MODE_ENABLE_SET_POS))
#define NF_TOGGLE_MODE_ENABLE_SET_POS                 ((uint32_t)24)
#define NF_TOGGLE_MODE_ENABLE_SET                     ((uint32_t)((uint32_t)1 << NF_TOGGLE_MODE_ENABLE_SET_POS))
#define NF_CRYP_ENGINE_ENABLE_SET_POS                 ((uint32_t)16)
#define NF_CRYP_ENGINE_ENABLE_SET                     ((uint32_t)((uint32_t)1 << NF_CRYP_ENGINE_ENABLE_SET_POS))
#define NF_READ_MODE_SEL_SET_POS                      ((uint32_t)13)
#define NF_READ_MODE_SEL_SET                          ((uint32_t)((uint32_t)1 << NF_READ_MODE_SEL_SET_POS))
#define NF_EDO_MODE_ENABLE_SET_POS                    ((uint32_t)11)
#define NF_EDO_MODE_ENABLE_SET                        ((uint32_t)((uint32_t)1 << NF_EDO_MODE_ENABLE_SET_POS))
#define NF_EF_MODE_ENABLE_SET_POS                     ((uint32_t)10)
#define NF_EF_MODE_ENABLE_SET                         ((uint32_t)((uint32_t)1 << NF_EF_MODE_ENABLE_SET_POS))
#define NF_EF_MODE_SELECT_SET_POS                     ((uint32_t)9)
#define NF_EF_MODE_SELECT_SET                         ((uint32_t)((uint32_t)1 << NF_EF_MODE_SELECT_SET_POS))
#define NF_CRC_MODE_ENABLE_SET_POS                    ((uint32_t)8)
#define NF_CRC_MODE_ENABLE_SET                        ((uint32_t)((uint32_t)1 << NF_CRC_MODE_ENABLE_SET_POS))
#define NF_CTRL_ENABLE_SET_POS                          ((uint32_t)0)
#define NF_CTRL_ENABLE_SET                              ((uint32_t)((uint32_t)1 << NF_CTRL_ENABLE_SET_POS))

/****************************NF Control Register (NF_CR) bitfiled *********************************/

#define NF_WP_ENABLE_SET_POS                          ((uint32_t)24)
#define NF_WP_ENABLE_SET                              ((uint32_t)((uint32_t)1 << NF_WP_ENABLE_SET_POS))
#define NF_CLE_ENABLE_SET_POS                         ((uint32_t)16)
#define NF_CLE_ENABLE_SET                             ((uint32_t)((uint32_t)1 << NF_CLE_ENABLE_SET_POS))
#define NF_ALE_ENABLE_SET_POS                         ((uint32_t)8)
#define NF_ALE_ENABLE_SET                             ((uint32_t)((uint32_t)1 << NF_ALE_ENABLE_SET_POS))
#define NF_CEJ_ENABLE_SET_POS                         ((uint32_t)0)
#define NF_CEJ_ENABLE_SET                             ((uint32_t)((uint32_t)1 << NF_CEJ_ENABLE_SET_POS))

/****************************NF ECC Control Register (NF_ECC_CR) bitfiled *********************************/

#define NF_BCH_SEL_SET_POS                            ((uint32_t)16)
#define NF_BCH_SEL_SET                                ((uint32_t)((uint32_t)7 << NF_BCH_SEL_SET_POS))
#define NF_ECC_MODE_SET_POS                           ((uint32_t)10)
#define NF_ECC_MODE_SET                               ((uint32_t)((uint32_t)1 << NF_ECC_MODE_SET_POS))
#define NF_ECC_BYPASS_MODE_SET_POS                    ((uint32_t)8)
#define NF_ECC_BYPASS_MODE_SET                        ((uint32_t)((uint32_t)3 << NF_ECC_BYPASS_MODE_SET_POS))
#define NF_ECC_SW_RST_SET_POS                         ((uint32_t)0)
#define NF_ECC_SW_RST_SET                             ((uint32_t)((uint32_t)1 << NF_ECC_SW_RST_SET_POS))

/****************************NF DMA Control Register (NF_DMA_CR) bitfiled *********************************/

#define NF_FW_REDU_LEN_SET_POS                        ((uint32_t)21)
#define NF_FW_REDU_LEN_SET                            ((uint32_t)((uint32_t)7 << NF_FW_REDU_LEN_SET_POS))
#define NF_DMA_LEN_SET_POS                            ((uint32_t)16)
#define NF_DMA_LEN_SET                                ((uint32_t)((uint32_t)0x1F << NF_DMA_LEN_SET_POS))
#define NF_DMA_BURST_LVL_SET_POS                      ((uint32_t)12)
#define NF_DMA_BURST_LVL_SET                          ((uint32_t)((uint32_t)0xF << NF_DMA_BURST_LVL_SET_POS))
#define NF_DMA_MODE_SET_POS                           ((uint32_t)10)
#define NF_DMA_MODE_SET                               ((uint32_t)((uint32_t)1 << NF_DMA_MODE_SET_POS))
#define NF_DMA_DIR_SET_POS                            ((uint32_t)9)
#define NF_DMA_DIR_SET                                ((uint32_t)((uint32_t)1 << NF_DMA_DIR_SET_POS))
#define NF_DMA_MODE_ENABLE_SET_POS                    ((uint32_t)8)
#define NF_DMA_MODE_ENABLE_SET                        ((uint32_t)((uint32_t)1 << NF_DMA_MODE_ENABLE_SET_POS))
#define NF_DMA_SW_RST_POS                             ((uint32_t)0)
#define NF_DMA_SW_RST                                 ((uint32_t)((uint32_t)1 << NF_DMA_SW_RST_POS))

/****************************NF Data  Register (NF_DR) bitfiled *********************************/

#define NF_DIN_SET_POS                                ((uint32_t)0)
#define NF_DIN_SET                                    ((uint32_t)((uint32_t)0xFF << NF_DIN_SET_POS))

/****************************NF DMA Data  Register (NF_DMA_DR) bitfiled *********************************/

/****************************NF Read Timing  Register (NF_RD_TIM) bitfiled *********************************/

#define NF_DDR_TRPST_SET_POS                          ((uint32_t)12)
#define NF_DDR_TRPST_SET                              ((uint32_t)((uint32_t)7 << NF_DDR_TRPST_SET_POS))
#define NF_DDR_TRPRE_SET_POS                          ((uint32_t)8)
#define NF_DDR_TRPRE_SET                              ((uint32_t)((uint32_t)7 << NF_DDR_TRPRE_SET_POS))
#define NF_TRP_SET_POS                                ((uint32_t)4)
#define NF_TRP_SET                                    ((uint32_t)((uint32_t)7 << NF_TRP_SET_POS))
#define NF_TRPJ_SET_POS                               ((uint32_t)0)
#define NF_TRPJ_SET                                   ((uint32_t)((uint32_t)7 << NF_TRPJ_SET_POS))

/****************************NF Write Timing  Register (NF_WR_TIM) bitfiled *********************************/

#define NF_DDR_TWPRE_SET_POS                          ((uint32_t)8)
#define NF_DDR_TWPRE_SET                              ((uint32_t)((uint32_t)7 << NF_DDR_TWPRE_SET_POS))
#define NF_TWP_SET_POS                                ((uint32_t)4)
#define NF_TWP_SET                                    ((uint32_t)((uint32_t)7 << NF_TWP_SET_POS))
#define NF_TWPJ_SET_POS                               ((uint32_t)0)
#define NF_TWPJ_SET                                   ((uint32_t)((uint32_t)7 << NF_TWPJ_SET_POS))


/****************************NF EDO RB Timing  Register (NF_EDO_RB_TIM) bitfiled *********************************/

#define NF_REJ_DLY_SEL_SET_POS                        ((uint32_t)8)
#define NF_REJ_DLY_SEL_SET                            ((uint32_t)((uint32_t)0xFF << NF_REJ_DLY_SEL_SET_POS))
#define NF_CRYP_ENGINE_CNT_SET_POS                    ((uint32_t)4)
#define NF_CRYP_ENGINE_CNT_SET                        ((uint32_t)((uint32_t)7 << NF_CRYP_ENGINE_CNT_SET_POS))
#define NF_TRB_SET_POS                                ((uint32_t)0)
#define NF_TRB_SET                                    ((uint32_t)((uint32_t)0xF << NF_TRB_SET_POS))


/****************************NF DQS Timing  Register (NF_EDO_RB_TIM) bitfiled *********************************/

#define NF_DQS_DELAY_SEL2_SET_POS                     ((uint32_t)8)
#define NF_DQS_DELAY_SEL2_SET                         ((uint32_t)((uint32_t)0xFF << NF_DQS_DELAY_SEL2_SET_POS))
#define NF_DQS_DELAY_SEL_SET_POS                      ((uint32_t)0)
#define NF_DQS_DELAY_SEL_SET                          ((uint32_t)((uint32_t)0xFF << NF_DQS_DELAY_SEL_SET_POS))

/****************************FW Redundant Word 0 (FW_REDU_W0) bitfiled *********************************/

/****************************FW Redundant Word 1 (FW_REDU_W1) bitfiled *********************************/

/****************************NF Interrupt Enable Register (NF_IE) bitfiled *********************************/

#define NF_DMA_RD_FSH_IE_SET_POS                      ((uint32_t)3)
#define NF_DMA_RD_FSH_IE_SET                          ((uint32_t)((uint32_t)1 << NF_DMA_RD_FSH_IE_SET_POS))
#define NF_DMA_WR_FSH_IE_SET_POS                      ((uint32_t)2)
#define NF_DMA_WR_FSH_IE_SET                          ((uint32_t)((uint32_t)1 << NF_DMA_WR_FSH_IE_SET_POS))
#define NF_DMA_IE_SET_POS                             ((uint32_t)1)
#define NF_DMA_IE_SET                                 ((uint32_t)((uint32_t)1 << NF_DMA_IE_SET_POS))
#define NF_ECC_IE_SET_POS                             ((uint32_t)0)
#define NF_ECC_IE_SET                                 ((uint32_t)((uint32_t)1 << NF_ECC_IE_SET_POS))


/**************************** NF Status Register (NF_SR) bitfiled *********************************/

#define NF_DMA_RD_FSH_FLG_SET_POS                     ((uint32_t)17)
#define NF_DMA_RD_FSH_FLG_SET                         ((uint32_t)((uint32_t)1 << NF_DMA_RD_FSH_FLG_SET_POS))
#define NF_DMA_WR_FSH_FLG_SET_POS                     ((uint32_t)16)
#define NF_DMA_WR_FSH_FLG_SET                         ((uint32_t)((uint32_t)1 << NF_DMA_WR_FSH_FLG_SET_POS))
#define NF_SRAM_EMPTY_SET_POS                         ((uint32_t)10)
#define NF_SRAM_EMPTY_SET                             ((uint32_t)((uint32_t)1 << NF_SRAM_EMPTY_SET_POS))
#define NF_SRAM_FULL_SET_POS                          ((uint32_t)9)
#define NF_SRAM_FULL_SET                              ((uint32_t)((uint32_t)1 << NF_SRAM_FULL_SET_POS))
#define NF_SRAM_AVA_SET_POS                           ((uint32_t)8)
#define NF_SRAM_AVA_SET                               ((uint32_t)((uint32_t)1 << NF_SRAM_AVA_SET_POS))
#define NF_DMA_FLG_SET_POS                            ((uint32_t)7)
#define NF_DMA_FLG_SET                                ((uint32_t)((uint32_t)1 << NF_DMA_FLG_SET_POS))
#define NF_ECC_FLAG_SET_POS                           ((uint32_t)5)
#define NF_ECC_FLAG_SET                               ((uint32_t)((uint32_t)1 << NF_ECC_FLAG_SET_POS))
#define NF_ALL_FF_FLG_SET_POS                         ((uint32_t)0)
#define NF_ALL_FF_FLG_SET                             ((uint32_t)((uint32_t)1 << NF_ALL_FF_FLG_SET_POS))

/**************************** NF ECC Status Register (NF_ECC_SR) bitfiled *********************************/

#define NF_MAX_ERR_CNT_SET_POS                        ((uint32_t)0)
#define NF_MAX_ERR_CNT_SET                            ((uint32_t)((uint32_t)0x1F << NF_MAX_ERR_CNT_SET_POS))

/****************************NF Sector Error Info (NF_SEC_ERR_INFO) bitfiled *********************************/

#define NF_SEC_ERR_TYPE_SET_POS                       ((uint32_t)16)
#define NF_SEC_ERR_TYPE_SET                           ((uint32_t)((uint32_t)0xFFFF << NF_SEC_ERR_TYPE_SET_POS))
#define NF_SEC_ERR_ST_SET_POS                         ((uint32_t)0)
#define NF_SEC_ERR_ST_SET                             ((uint32_t)((uint32_t)0xFFFF << NF_SEC_ERR_ST_SET_POS))

/****************************FW RD Redundant Word 0 (FW_RD_REDU_W0) bitfiled *********************************/

/****************************FW RD Redundant Word 1 (FW_RD_REDU_W1) bitfiled *********************************/

/****************************NF Low Power Control Register (NF_LP_CR) bitfiled *********************************/

#define NF_REDUR_GCLK_EN_SET_POS                      ((uint32_t)7)
#define NF_REDUR_GCLK_EN_SET                          ((uint32_t)((uint32_t)1 << NF_REDUR_GCLK_EN_SET_POS))
#define NF_CPUW_GCLK_EN_SET_POS                       ((uint32_t)6)
#define NF_CPUW_GCLK_EN_SET                           ((uint32_t)((uint32_t)1 << NF_CPUW_GCLK_EN_SET_POS))
#define NF_ECC_GCLK_EN_SET_POS                        ((uint32_t)5)
#define NF_ECC_GCLK_EN_SET                            ((uint32_t)((uint32_t)1 << NF_ECC_GCLK_EN_SET_POS))
#define NF_COR_GCLK_EN_SET_POS                        ((uint32_t)4)
#define NF_COR_GCLK_EN_SET                            ((uint32_t)((uint32_t)1 << NF_COR_GCLK_EN_SET_POS))
#define NF_L4_GCLK_EN_SET_POS                         ((uint32_t)3)
#define NF_L4_GCLK_EN_SET                             ((uint32_t)((uint32_t)1 << NF_L4_GCLK_EN_SET_POS))
#define NF_L3_GCLK_EN_SET_POS                         ((uint32_t)3)
#define NF_L3_GCLK_EN_SET                             ((uint32_t)((uint32_t)1 << NF_L3_GCLK_EN_SET_POS))
#define NF_L2_GCLK_EN_SET_POS                         ((uint32_t)3)
#define NF_L2_GCLK_EN_SET                             ((uint32_t)((uint32_t)1 << NF_L2_GCLK_EN_SET_POS))
#define NF_L1_GCLK_EN_SET_POS                         ((uint32_t)3)
#define NF_L1_GCLK_EN_SET                             ((uint32_t)((uint32_t)1 << NF_L1_GCLK_EN_SET_POS))


/**************************** NF Mode register*********************************/

/** 
* @brief  Clear NF Mode Register
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_MR_CLEAR(pNFLASH)                      IO_WRITE_REG(pNFLASH->MR,0)


/** 
* @brief  Enable  Sync DDR mode 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SYNC_MODE_ENABLE(pNFLASH)               IO_BIT_SET(pNFLASH->MR, NF_SYNC_MODE_ENABLE_SET)

/** 
* @brief  Disable Sync DDR mode 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SYNC_MODE_DISABLE(pNFLASH)              IO_BIT_CLEAR(pNFLASH->MR, NF_SYNC_MODE_ENABLE_SET)

/** 
* @brief  Enable  TOGGLE DDR mode 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_TOGGLE_MODE_ENABLE(pNFLASH)             IO_BIT_SET(pNFLASH->MR, NF_TOGGLE_MODE_ENABLE_SET)

/** 
* @brief  Disable TOGGLE DDR mode 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_TOGGLE_MODE_DISABLE(pNFLASH)            IO_BIT_CLEAR(pNFLASH->MR, NF_TOGGLE_MODE_ENABLE_SET)

/** 
* @brief  Enable  Crypto engine
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CRYP_ENGINE_ENABLE(pNFLASH)             IO_BIT_SET(pNFLASH->MR, NF_CRYP_ENGINE_ENABLE_SET)

/** 
* @brief  Disable Crypto engine 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CRYP_ENGINE_DISABLE(pNFLASH)            IO_BIT_CLEAR(pNFLASH->MR, NF_CRYP_ENGINE_ENABLE_SET)

/** 
* @brief  Select NandFlash read method
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_READ_MODE_SEL_SET(pNFLASH,val)          IO_BITMASK_SET(pNFLASH->MR, NF_READ_MODE_SEL_SET,val<<NF_READ_MODE_SEL_SET_POS)

/** 
* @brief  Enable  NF EDO mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_EDO_MODE_ENABLE(pNFLASH)                IO_BIT_SET(pNFLASH->MR, NF_EDO_MODE_ENABLE_SET)

/** 
* @brief  Disable NF EDO mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_EDO_MODE_DISABLE(pNFLASH)               IO_BIT_CLEAR(pNFLASH->MR, NF_EDO_MODE_ENABLE_SET)

/** 
* @brief  Enable  NF EF mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_EF_MODE_ENABLE(pNFLASH)                 IO_BIT_SET(pNFLASH->MR, NF_EF_MODE_ENABLE_SET)

/** 
* @brief  Disable NF EF mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_EF_MODE_DISABLE(pNFLASH)                IO_BIT_CLEAR(pNFLASH->MR, NF_EF_MODE_ENABLE_SET)

/** 
* @brief  Select NandFlash EF mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_EF_MODE_SEL_SET(pNFLASH,val)            IO_BITMASK_SET(pNFLASH->MR, NF_EF_MODE_SELECT_SET,val<<NF_EF_MODE_SELECT_SET_POS)

/** 
* @brief  Enable  NF CRC mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CRC_MODE_ENABLE(pNFLASH)                IO_BIT_SET(pNFLASH->MR, NF_CRC_MODE_ENABLE_SET)

/** 
* @brief  Disable NF CRC mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CRC_MODE_DISABLE(pNFLASH)               IO_BIT_CLEAR(pNFLASH->MR, NF_CRC_MODE_ENABLE_SET)

/** 
* @brief  Enable  NF Controller
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CTRL_ENABLE(pNFLASH)                      IO_BIT_SET(pNFLASH->MR, NF_CTRL_ENABLE_SET)

/** 
* @brief  Disable NF Controller
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CTRL_DISABLE(pNFLASH)                     IO_BIT_CLEAR(pNFLASH->MR, NF_CTRL_ENABLE_SET)



/****************************NF Control Register *********************************/


/** 
* @brief  Clear NF Control Register
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CR_CLEAR(pNFLASH)                      IO_WRITE_REG(pNFLASH->CR,0)



/** 
* @brief  Enable  NF Write Protect
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_WP_ENABLE(pNFLASH)                      IO_BIT_SET(pNFLASH->CR, NF_WP_ENABLE_SET)

/** 
* @brief  Disable NF Write Protect
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_WP_DISABLE(pNFLASH)                     IO_BIT_CLEAR(pNFLASH->CR, NF_WP_ENABLE_SET)

/** 
* @brief  Enable  NF Command latch
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CLE_ENABLE(pNFLASH)                     IO_BIT_SET(pNFLASH->CR, NF_CLE_ENABLE_SET)

/** 
* @brief  Disable NF Command latch
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CLE_DISABLE(pNFLASH)                    IO_BIT_CLEAR(pNFLASH->CR, NF_CLE_ENABLE_SET)

/** 
* @brief  Enable  NF Address latch
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ALE_ENABLE(pNFLASH)                     IO_BIT_SET(pNFLASH->CR, NF_ALE_ENABLE_SET)

/** 
* @brief  Disable NF Address latch
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ALE_DISABLE(pNFLASH)                    IO_BIT_CLEAR(pNFLASH->CR, NF_ALE_ENABLE_SET)

/** 
* @brief  Enable  NF Chip
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CEJ_ENABLE(pNFLASH)                     IO_BIT_SET(pNFLASH->CR, NF_CEJ_ENABLE_SET)

/** 
* @brief  Disable NF Chip
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CEJ_DISABLE(pNFLASH)                    IO_BIT_CLEAR(pNFLASH->CR, NF_CEJ_ENABLE_SET)

/****************************NF ECC Control Register *********************************/

/** 
* @brief  Select NandFlash BCH algorithm
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_BCH_SEL_SET(pNFLASH,val)                IO_BITMASK_SET(pNFLASH->ECCCR, NF_BCH_SEL_SET,val<<NF_BCH_SEL_SET_POS)

/** 
* @brief  Select ECC algorithm
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_ECC_MODE_SET(pNFLASH,val)               IO_BITMASK_SET(pNFLASH->ECCCR, NF_ECC_MODE_SET,val<<NF_ECC_MODE_SET_POS)

/** 
* @brief  Select ECC BYPASS Mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_ECC_BYPASS_MODE_SET(pNFLASH,val)        IO_BITMASK_SET(pNFLASH->ECCCR, NF_ECC_BYPASS_MODE_SET,val<<NF_ECC_BYPASS_MODE_SET_POS)

/** 
* @brief  Reset NF ECC by soft
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_SW_RST(pNFLASH)                     IO_BIT_SET(pNFLASH->ECCCR, NF_ECC_SW_RST_SET)

/** 
* @brief  Unreset NF ECC by soft
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_SW_UNRST(pNFLASH)                   IO_BIT_CLEAR(pNFLASH->ECCCR, NF_ECC_SW_RST_SET)



/****************************NF DMA Control Register*********************************/

/** 
* @brief  Set Firmware Redundant byte length
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_FW_REDU_LEN_SET(pNFLASH,val)            IO_BITMASK_SET(pNFLASH->DMACR, NF_FW_REDU_LEN_SET,val<<NF_FW_REDU_LEN_SET_POS)

/** 
* @brief  Set DMA transfer length
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DMA_LEN_SET(pNFLASH,val)                IO_BITMASK_SET(pNFLASH->DMACR, NF_DMA_LEN_SET,val<<NF_DMA_LEN_SET_POS)

/** 
* @brief  Set Threshold level for DMA module issue burst access
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DMA_BURST_LVL_SET(pNFLASH,val)          IO_BITMASK_SET(pNFLASH->DMACR, NF_DMA_BURST_LVL_SET,val<<NF_DMA_BURST_LVL_SET_POS)

/** 
* @brief  Select DMA mode
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DMA_MODE_SET(pNFLASH,val)               IO_BITMASK_SET(pNFLASH->DMACR, NF_DMA_MODE_SET,val<<NF_DMA_MODE_SET_POS)

/** 
* @brief  Set DMA Data Direction
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DMA_DIR_SET(pNFLASH,val)                IO_BITMASK_SET(pNFLASH->DMACR, NF_DMA_DIR_SET,val<<NF_DMA_DIR_SET_POS)

/** 
* @brief  Enable  DMA Mode 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_MODE_ENABLE(pNFLASH)                IO_BIT_SET(pNFLASH->DMACR, NF_DMA_MODE_ENABLE_SET)

/** 
* @brief  Disable DMA Mode 
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_MODE_DISABLE(pNFLASH)               IO_BIT_CLEAR(pNFLASH->DMACR, NF_DMA_MODE_ENABLE_SET)

/** 
* @brief  Reset DMA by software
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_SW_RST(pNFLASH)                     IO_BIT_SET(pNFLASH->DMACR, NF_DMA_SW_RST)

/** 
* @brief  Unreset DMA by software
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_SW_UNRST(pNFLASH)                   IO_BIT_CLEAR(pNFLASH->DMACR, NF_DMA_SW_RST)

/****************************NF Data  Register (NF_DR) bitfiled *********************************/

/** 
* @brief  Register write to trigger a PIO Write cycle to NF
* 
* @param  pNFLASH		   pNFLASH device Point
*
* @retval void 
*/
#define LL_NF_DIN_SET(pNFLASH,val)                    IO_BITMASK_SET(pNFLASH->DR, NF_DIN_SET,val<<NF_DIN_SET_POS)

/** 
* @brief  Register read to trigger a PIO read cycle to NF
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DIN_GET(pNFLASH)                    IO_BITS_GET(pNFLASH->DR, NF_DIN_SET)



/****************************NF DMA Data  Register *********************************/

/** 
* @brief  Register write to trigger a DMA Write cycle to NF
* 
* @param  pNFLASH		   pNFLASH device Point
*
* @retval void 
*/
#define LL_NF_DMA_DIN_SET(pNFLASH,val)                    IO_WRITE_REG(pNFLASH->DMADR, val)

/** 
* @brief  Register read to trigger a DMA read cycle to NF
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DMA_DIN_GET(pNFLASH)                    IO_READ_REG(pNFLASH->DMADR)

/****************************NF Read Timing  Register *********************************/

/** 
* @brief  Set DDR  TRPST
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DDR_TRPST_SET(pNFLASH,val)              IO_BITMASK_SET(pNFLASH->RDTIMR, NF_DDR_TRPST_SET,val<<NF_DDR_TRPST_SET_POS)

/** 
* @brief  Set DDR TRPRE
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DDR_TRPRE_SET(pNFLASH,val)              IO_BITMASK_SET(pNFLASH->RDTIMR, NF_DDR_TRPRE_SET,val<<NF_DDR_TRPRE_SET_POS)

/** 
* @brief  Set TRP
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_TRP_SET(pNFLASH,val)                    IO_BITMASK_SET(pNFLASH->RDTIMR, NF_TRP_SET,val<<NF_TRP_SET_POS)

/** 
* @brief  Set TRPJ
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_TRPJ_SET(pNFLASH,val)                   IO_BITMASK_SET(pNFLASH->RDTIMR, NF_TRPJ_SET,val<<NF_TRPJ_SET_POS)


/****************************NF Write Timing  Register (NF_WR_TIM) bitfiled *********************************/

/** 
* @brief  Set NandFlash DDR TWPRE
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DDR_TWPRE_SET(pNFLASH,val)              IO_BITMASK_SET(pNFLASH->WRTIMR, NF_DDR_TWPRE_SET,val<<NF_DDR_TWPRE_SET_POS)

/** 
* @brief  Set NandFlash TWP
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_TWP_SET(pNFLASH,val)                    IO_BITMASK_SET(pNFLASH->WRTIMR, NF_TWP_SET,val<<NF_TWP_SET_POS)

/** 
* @brief  Set NandFlash TWPJ
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_TWPJ_SET(pNFLASH,val)                   IO_BITMASK_SET(pNFLASH->WRTIMR, NF_TWPJ_SET,val<<NF_TWPJ_SET_POS)

/****************************NF EDO RB Timing  Register *********************************/

/** 
* @brief  Set Nand Flash REJ_IN delay chain selection
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_REJ_DLY_SEL_SET(pNFLASH,val)            IO_BITMASK_SET(pNFLASH->EDORDTIMR, NF_REJ_DLY_SEL_SET,val<<NF_REJ_DLY_SEL_SET_POS)

/** 
* @brief  Set CRYPTO engine random seed selection
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_CRYP_ENGINE_CNT_SET(pNFLASH,val)        IO_BITMASK_SET(pNFLASH->EDORDTIMR, NF_CRYP_ENGINE_CNT_SET,val<<NF_CRYP_ENGINE_CNT_SET_POS)

/** 
* @brief  Set TRB
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_TRB_SET(pNFLASH,val)                    IO_BITMASK_SET(pNFLASH->EDORDTIMR, NF_TRB_SET,val<<NF_TRB_SET_POS)



/****************************NF DQS Timing  Register (NF_EDO_RB_TIM) bitfiled *********************************/

/** 
* @brief  Set DQS_IN delay chain selection2
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DQS_DELAY_SEL2_SET(pNFLASH,val)         IO_BITMASK_SET(pNFLASH->DQSTIMR, NF_DQS_DELAY_SEL2_SET,val<<NF_DQS_DELAY_SEL2_SET_POS)

/** 
* @brief  Set DQS_IN delay chain selection
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_DQS_DELAY_SEL_SET(pNFLASH,val)          IO_BITMASK_SET(pNFLASH->DQSTIMR, NF_DQS_DELAY_SEL_SET,val<<NF_DQS_DELAY_SEL_SET_POS)


/****************************FW Redundant Word 0 (FW_REDU_W0) bitfiled *********************************/

/****************************FW Redundant Word 1 (FW_REDU_W1) bitfiled *********************************/

/****************************NF Interrupt Enable Register (NF_IE) bitfiled *********************************/

/** 
* @brief  Enable  DMA_RD_FSH interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_RD_FSH_INTRPT_ENABLE(pNFLASH)       IO_BIT_SET(pNFLASH->IER, NF_DMA_RD_FSH_IE_SET)

/** 
* @brief  Disable DMA_RD_FSH interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_RD_FSH_INTRPT_DISABLE(pNFLASH)      IO_BIT_CLEAR(pNFLASH->IER, NF_DMA_RD_FSH_IE_SET)

/** 
* @brief  Enable  DMA_WR_FSH interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_WR_FSH_INTRPT_ENABLE(pNFLASH)       IO_BIT_SET(pNFLASH->IER, NF_DMA_WR_FSH_IE_SET)

/** 
* @brief  Disable DMA_WR_FSH interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_WR_FSH_INTRPT_DISABLE(pNFLASH)      IO_BIT_CLEAR(pNFLASH->IER, NF_DMA_WR_FSH_IE_SET)

/** 
* @brief  Enable  NF_DMA interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_INTRPT_ENABLE(pNFLASH)              IO_BIT_SET(pNFLASH->IER, NF_DMA_IE_SET)

/** 
* @brief  Disable NF_DMA interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_INTRPT_DISABLE(pNFLASH)             IO_BIT_CLEAR(pNFLASH->IER, NF_DMA_IE_SET)

/** 
* @brief  Enable  NF_ECC interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_INTRPT_ENABLE(pNFLASH)              IO_BIT_SET(pNFLASH->IER, NF_ECC_IE_SET)

/** 
* @brief  Disable NF_ECC interrupt
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_INTRPT_DISABLE(pNFLASH)             IO_BIT_CLEAR(pNFLASH->IER, NF_ECC_IE_SET)

/**************************** NF Status Register *********************************/

/** 
* @brief  Set the flag of DMA reading data from NandFlash by HW handshake
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_RD_FSH_FLG_SET(pNFLASH)             IO_BIT_SET(pNFLASH->SR, NF_DMA_RD_FSH_FLG_SET)

/** 
* @brief  Clear the flag of DMA reading data from NandFlash by HW handshake
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_RD_FSH_FLG_CLEAR(pNFLASH)           IO_BIT_CLEAR(pNFLASH->SR, NF_DMA_RD_FSH_FLG_SET)

/** 
* @brief  Set the flag of DMA Programming data from NandFlash by HW handshake
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_WR_FSH_FLG_SET(pNFLASH)             IO_BIT_SET(pNFLASH->SR, NF_DMA_WR_FSH_FLG_SET)

/** 
* @brief  Clear the flag of DMA Programming data from NandFlash by HW handshake
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_WR_FSH_FLG_CLEAR(pNFLASH)           IO_BIT_CLEAR(pNFLASH->SR, NF_DMA_WR_FSH_FLG_SET)

/** 
* @brief  Set the flag of DMA sram all empty
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SRAM_EMPTY_SET(pNFLASH)                 IO_BIT_SET(pNFLASH->SR, NF_SRAM_EMPTY_SET)

/** 
* @brief  Clear the flag of DMA sram all empty
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SRAM_EMPTY_CLEAR(pNFLASH)               IO_BIT_CLEAR(pNFLASH->SR, NF_SRAM_EMPTY_SET)

/** 
* @brief  Set the flag of DMA sram all full
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SRAM_FULL_SET(pNFLASH)                  IO_BIT_SET(pNFLASH->SR, NF_SRAM_FULL_SET)

/** 
* @brief  Clear the flag of DMA sram all full
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SRAM_FULL_CLEAR(pNFLASH)                 IO_BIT_CLEAR(pNFLASH->SR, NF_SRAM_FULL_SET)

/** 
* @brief  Set the flag of DMA SRAM status if there is valid data in DMA SRAM
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SRAM_AVA_SET(pNFLASH)                   IO_BIT_SET(pNFLASH->SR, NF_SRAM_AVA_SET)

/** 
* @brief  Clear the flag of DMA SRAM status if there is NO valid data in DMA SRAM
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_SRAM_AVA_CLEAR(pNFLASH)                 IO_BIT_CLEAR(pNFLASH->SR, NF_SRAM_AVA_SET)

/** 
* @brief  Set the flag if NF read or write data finish in DMA mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_FLG_SET(pNFLASH)                    IO_BIT_SET(pNFLASH->SR, NF_DMA_FLG_SET)

/** 
* @brief  Clear the flag  if NF read or write data NOT finish in DMA mode
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_DMA_FLG_CLEAR(pNFLASH)                  IO_BIT_CLEAR(pNFLASH->SR, NF_DMA_FLG_SET)

/** 
* @brief  Set the flag if NF ECC finish during reading data from NF
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_FLAG_SET(pNFLASH)                   IO_BIT_SET(pNFLASH->SR, NF_ECC_FLAG_SET)

/** 
* @brief  Clear the flag if NF ECC NOT finish during reading data from NF
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_FLAG_CLEAR(pNFLASH)                 IO_BIT_CLEAR(pNFLASH->SR, NF_ECC_FLAG_SET)

/** 
* @brief  Set NF ALL FF Flag
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ALL_FF_FLG_SET(pNFLASH)                 IO_BIT_SET(pNFLASH->SR, NF_ALL_FF_FLG_SET)

/** 
* @brief  Clear NF ALL FF Flag
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ALL_FF_FLG_CLEAR(pNFLASH)               IO_BIT_CLEAR(pNFLASH->SR, NF_ALL_FF_FLG_SET)


/**************************** NF ECC Status Register (NF_ECC_SR) bitfiled *********************************/

/** 
* @brief  Set Max error bit number for every page
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_MAX_ERR_CNT_SET(pNFLASH,val)            IO_BITMASK_SET(pNFLASH->ECCSR, NF_MAX_ERR_CNT_SET,val<<NF_MAX_ERR_CNT_SET_POS)

/****************************NF Sector Error Info (NF_SEC_ERR_INFO) bitfiled *********************************/

/** 
* @brief  Set Sector error type
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_SEC_ERR_TYPE_SET(pNFLASH,val)           IO_BITMASK_SET(pNFLASH->SECERRINFO, NF_SEC_ERR_TYPE_SET,val<<NF_SEC_ERR_TYPE_SET_POS)

/** 
* @brief  Set Sector error occur status
* 
* @param  pNFLASH		   pNFLASH device Point
* 
* @param  val			  Dtr value 
*
* @retval void 
*/
#define LL_NF_SEC_ERR_ST_SET(pNFLASH,val)             IO_BITMASK_SET(pNFLASH->SECERRINFO, NF_SEC_ERR_ST_SET,val<<NF_SEC_ERR_ST_SET_POS)


/****************************FW RD Redundant Word 0 (FW_RD_REDU_W0) bitfiled *********************************/

/****************************FW RD Redundant Word 1 (FW_RD_REDU_W1) bitfiled *********************************/

/****************************NF Low Power Control Register (NF_LP_CR) bitfiled *********************************/

/** 
* @brief  Enable  Redundant Read Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_REDUR_GCLK_ENABLE(pNFLASH)              IO_BIT_SET(pNFLASH->LPCR, NF_REDUR_GCLK_EN_SET)

/** 
* @brief  Disable Redundant Read Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_REDUR_GCLK_DISABLE(pNFLASH)             IO_BIT_CLEAR(pNFLASH->LPCR, NF_REDUR_GCLK_EN_SET)

/** 
* @brief  Enable  MCU Write Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CPUW_GCLK_ENABLE(pNFLASH)               IO_BIT_SET(pNFLASH->LPCR, NF_CPUW_GCLK_EN_SET)

/** 
* @brief  Disable MCU Write Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_CPUW_GCLK_DISABLE(pNFLASH)              IO_BIT_CLEAR(pNFLASH->LPCR, NF_CPUW_GCLK_EN_SET)

/** 
* @brief  Enable  ECC COR Block Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_GCLK_ENABLE(pNFLASH)                IO_BIT_SET(pNFLASH->LPCR, NF_ECC_GCLK_EN_SET)

/** 
* @brief  Disable ECC COR Block Gating Clock         
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_ECC_GCLK_DISABLE(pNFLASH)               IO_BIT_CLEAR(pNFLASH->LPCR, NF_ECC_GCLK_EN_SET)

/** 
* @brief  Enable  COR Block Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_COR_GCLK_ENABLE(pNFLASH)                IO_BIT_SET(pNFLASH->LPCR, NF_COR_GCLK_EN_SET)

/** 
* @brief  Disable COR Block Gating Clock
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_COR_GCLK_DISABLE(pNFLASH)               IO_BIT_CLEAR(pNFLASH->LPCR, NF_COR_GCLK_EN_SET)

/** 
* @brief  Enable  4th Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L4_GCLK_ENABLE(pNFLASH)                 IO_BIT_SET(pNFLASH->LPCR, NF_L4_GCLK_EN_SET)

/** 
* @brief  4th Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L4_GCLK_DISABLE(pNFLASH)                IO_BIT_CLEAR(pNFLASH->LPCR, NF_L4_GCLK_EN_SET)

/** 
* @brief  Enable  3rd Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L3_GCLK_ENABLE(pNFLASH)                 IO_BIT_SET(pNFLASH->LPCR, NF_L3_GCLK_EN_SET)

/** 
* @brief  Disable 3rd Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L3_GCLK_DISABLE(pNFLASH)                IO_BIT_CLEAR(pNFLASH->LPCR, NF_L3_GCLK_EN_SET)

/** 
* @brief  Enable  2nd Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L2_GCLK_ENABLE(pNFLASH)                 IO_BIT_SET(pNFLASH->LPCR, NF_L2_GCLK_EN_SET)

/** 
* @brief  Disable 2nd Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L2_GCLK_DISABLE(pNFLASH)                IO_BIT_CLEAR(pNFLASH->LPCR, NF_L2_GCLK_EN_SET)

/** 
* @brief  Enable  1st Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L1_GCLK_ENABLE(pNFLASH)                 IO_BIT_SET(pNFLASH->LPCR, NF_L1_GCLK_EN_SET)

/** 
* @brief  Disable 1st Layer Power Control
* 
* @param  pNFLASH          pNFLASH device Point
* 
* @retval void 
*/
#define LL_NF_L1_GCLK_DISABLE(pNFLASH)                IO_BIT_CLEAR(pNFLASH->LPCR, NF_L1_GCLK_EN_SET)

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_NF_H_ */

