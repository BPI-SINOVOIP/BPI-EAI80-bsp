/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_vout.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/11/28 
 *    
 * @brief         VOUT peripheral PHY device header file
 * 
 * @note
 *    2016/11/28, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GM_LL_VOUT_H_
#define _GM_LL_VOUT_H_

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_vout_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** 
 * @brief LCD-TFT Display layer x Controller
 */
typedef struct
{
  __IO uint32_t CR;            /**< VOUT Layerx Control Register                                  Address offset: 0x30 */
  __IO uint32_t WHPCR;         /**< VOUT Layerx Window Horizontal Position Configuration Register Address offset: 0x34 */
  __IO uint32_t WVPCR;         /**< VOUT Layerx Window Vertical Position Configuration Register   Address offset: 0x38 */
  __IO uint32_t CKCR;          /**< VOUT Layerx Color Keying Configuration Register               Address offset: 0x3C */
  __IO uint32_t PFCR;          /**< VOUT Layerx Pixel Format Configuration Register               Address offset: 0x40 */
  __IO uint32_t CACR;          /**< VOUT Layerx Constant Alpha Configuration Register             Address offset: 0x44 */
  __IO uint32_t DCCR;          /**< VOUT Layerx Default Color Configuration Register              Address offset: 0x48 */
  uint32_t      RESERVED;      /**< VOUT Layerx Blending Factors Configuration Register           Address offset: 0x4C */
  __IO uint32_t CFBAR;         /**< VOUT Layerx Color Frame Buffer Address Register               Address offset: 0x50 */
  __IO uint32_t CFBLR;         /**< VOUT Layerx Color Frame Buffer Length Register                Address offset: 0x54 */
  __IO uint32_t CFBLNR;        /**< VOUT Layerx ColorFrame Buffer Line Number Register            Address offset: 0x58 */
  __IO uint32_t CLUTWR;        /**< VOUT Layerx CLUT Write Register                               Address offset: 0x5C */
} VOUT_Layer_TypeDef;

/********************  Bit definition for VOUT_SSCR register  *****************/
#define VOUT_SSCR_VSH_Pos            (0U)                                      
#define VOUT_SSCR_VSH_Msk            (0x7FFU << VOUT_SSCR_VSH_Pos)             /**< 0x000007FF */
#define VOUT_SSCR_VSH                VOUT_SSCR_VSH_Msk                         /**< Vertical Synchronization Height */
#define VOUT_SSCR_HSW_Pos            (16U)                                     
#define VOUT_SSCR_HSW_Msk            (0xFFFU << VOUT_SSCR_HSW_Pos)             /**< 0x0FFF0000 */
#define VOUT_SSCR_HSW                VOUT_SSCR_HSW_Msk                         /**< Horizontal Synchronization Width */

/********************  Bit definition for VOUT_BPCR register  *****************/
#define VOUT_BPCR_AVBP_Pos           (0U)                                      
#define VOUT_BPCR_AVBP_Msk           (0x7FFU << VOUT_BPCR_AVBP_Pos)            /**< 0x000007FF */
#define VOUT_BPCR_AVBP               VOUT_BPCR_AVBP_Msk                        /**< Accumulated Vertical Back Porch */
#define VOUT_BPCR_AHBP_Pos           (16U)                                     
#define VOUT_BPCR_AHBP_Msk           (0xFFFU << VOUT_BPCR_AHBP_Pos)            /**< 0x0FFF0000 */
#define VOUT_BPCR_AHBP               VOUT_BPCR_AHBP_Msk                        /**< Accumulated Horizontal Back Porch */

/********************  Bit definition for VOUT_AWCR register  *****************/
#define VOUT_AWCR_AAH_Pos            (0U)                                      
#define VOUT_AWCR_AAH_Msk            (0x7FFU << VOUT_AWCR_AAH_Pos)             /**< 0x000007FF */
#define VOUT_AWCR_AAH                VOUT_AWCR_AAH_Msk                         /**< Accumulated Active heigh */
#define VOUT_AWCR_AAW_Pos            (16U)                                     
#define VOUT_AWCR_AAW_Msk            (0xFFFU << VOUT_AWCR_AAW_Pos)             /**< 0x0FFF0000 */
#define VOUT_AWCR_AAW                VOUT_AWCR_AAW_Msk                         /**< Accumulated Active Width */

/********************  Bit definition for VOUT_TWCR register  *****************/
#define VOUT_TWCR_TOTALH_Pos         (0U)                                      
#define VOUT_TWCR_TOTALH_Msk         (0x7FFU << VOUT_TWCR_TOTALH_Pos)          /**< 0x000007FF */
#define VOUT_TWCR_TOTALH             VOUT_TWCR_TOTALH_Msk                      /**< Total Heigh */
#define VOUT_TWCR_TOTALW_Pos         (16U)                                     
#define VOUT_TWCR_TOTALW_Msk         (0xFFFU << VOUT_TWCR_TOTALW_Pos)          /**< 0x0FFF0000 */
#define VOUT_TWCR_TOTALW             VOUT_TWCR_TOTALW_Msk                      /**< Total Width */

/********************  Bit definition for VOUT_GCR register  ******************/
#define VOUT_GCR_VOUTEN_Pos          (0U)                                      
#define VOUT_GCR_VOUTEN_Msk          (0x1U << VOUT_GCR_VOUTEN_Pos)             /**< 0x00000001 */
#define VOUT_GCR_VOUTEN              VOUT_GCR_VOUTEN_Msk                       /**< LCD-TFT controller enable bit */
#define VOUT_GCR_DBW_Pos             (4U)                                      
#define VOUT_GCR_DBW_Msk             (0x7U << VOUT_GCR_DBW_Pos)                /**< 0x00000070 */
#define VOUT_GCR_DBW                 VOUT_GCR_DBW_Msk                          /**< Dither Blue Width */
#define VOUT_GCR_DGW_Pos             (8U)                                      
#define VOUT_GCR_DGW_Msk             (0x7U << VOUT_GCR_DGW_Pos)                /**< 0x00000700 */
#define VOUT_GCR_DGW                 VOUT_GCR_DGW_Msk                          /**< Dither Green Width */
#define VOUT_GCR_DRW_Pos             (12U)                                     
#define VOUT_GCR_DRW_Msk             (0x7U << VOUT_GCR_DRW_Pos)                /**< 0x00007000 */
#define VOUT_GCR_DRW                 VOUT_GCR_DRW_Msk                          /**< Dither Red Width */
#define VOUT_GCR_DEN_Pos             (16U)                                     
#define VOUT_GCR_DEN_Msk             (0x1U << VOUT_GCR_DEN_Pos)                /**< 0x00010000 */
#define VOUT_GCR_DEN                 VOUT_GCR_DEN_Msk                          /**< Dither Enable */
#define VOUT_GCR_PCPOL_Pos           (28U)                                     
#define VOUT_GCR_PCPOL_Msk           (0x1U << VOUT_GCR_PCPOL_Pos)              /**< 0x10000000 */
#define VOUT_GCR_PCPOL               VOUT_GCR_PCPOL_Msk                        /**< Pixel Clock Polarity */
#define VOUT_GCR_DEPOL_Pos           (29U)                                     
#define VOUT_GCR_DEPOL_Msk           (0x1U << VOUT_GCR_DEPOL_Pos)              /**< 0x20000000 */
#define VOUT_GCR_DEPOL               VOUT_GCR_DEPOL_Msk                        /**< Data Enable Polarity */
#define VOUT_GCR_VSPOL_Pos           (30U)                                     
#define VOUT_GCR_VSPOL_Msk           (0x1U << VOUT_GCR_VSPOL_Pos)              /**< 0x40000000 */
#define VOUT_GCR_VSPOL               VOUT_GCR_VSPOL_Msk                        /**< Vertical Synchronization Polarity */
#define VOUT_GCR_HSPOL_Pos           (31U)                                     
#define VOUT_GCR_HSPOL_Msk           (0x1U << VOUT_GCR_HSPOL_Pos)              /**< 0x80000000 */
#define VOUT_GCR_HSPOL               VOUT_GCR_HSPOL_Msk                        /**< Horizontal Synchronization Polarity */

/********************  Bit definition for VOUT_BCCR register  *****************/
#define VOUT_BCCR_BCBLUE_Pos         (0U)                                      
#define VOUT_BCCR_BCBLUE_Msk         (0xFFU << VOUT_BCCR_BCBLUE_Pos)           /**< 0x000000FF */
#define VOUT_BCCR_BCBLUE             VOUT_BCCR_BCBLUE_Msk                      /**< Background Blue value */
#define VOUT_BCCR_BCGREEN_Pos        (8U)                                      
#define VOUT_BCCR_BCGREEN_Msk        (0xFFU << VOUT_BCCR_BCGREEN_Pos)          /**< 0x0000FF00 */
#define VOUT_BCCR_BCGREEN            VOUT_BCCR_BCGREEN_Msk                     /**< Background Green value */
#define VOUT_BCCR_BCRED_Pos          (16U)                                     
#define VOUT_BCCR_BCRED_Msk          (0xFFU << VOUT_BCCR_BCRED_Pos)            /**< 0x00FF0000 */
#define VOUT_BCCR_BCRED              VOUT_BCCR_BCRED_Msk                       /**< Background Red value */

/********************  Bit definition for VOUT_IER register  ******************/
#define VOUT_IER_LIE_Pos             (0U)                                      
#define VOUT_IER_LIE_Msk             (0x1U << VOUT_IER_LIE_Pos)                /**< 0x00000001 */
#define VOUT_IER_LIE                 VOUT_IER_LIE_Msk                          /**< Line Interrupt Enable */
#define VOUT_IER_FUIE_Pos            (1U)                                      
#define VOUT_IER_FUIE_Msk            (0x1U << VOUT_IER_FUIE_Pos)               /**< 0x00000002 */
#define VOUT_IER_FUIE                VOUT_IER_FUIE_Msk                         /**< FIFO Underrun Interrupt Enable */
#define VOUT_IER_TERRIE_Pos          (2U)                                      
#define VOUT_IER_TERRIE_Msk          (0x1U << VOUT_IER_TERRIE_Pos)             /**< 0x00000004 */
#define VOUT_IER_TERRIE              VOUT_IER_TERRIE_Msk                       /**< Transfer Error Interrupt Enable */

/********************  Bit definition for VOUT_ISR register  ******************/
#define VOUT_ISR_LIF_Pos             (0U)                                      
#define VOUT_ISR_LIF_Msk             (0x1U << VOUT_ISR_LIF_Pos)                /**< 0x00000001 */
#define VOUT_ISR_LIF                 VOUT_ISR_LIF_Msk                          /**< Line Interrupt Flag */
#define VOUT_ISR_FUIF_Pos            (1U)                                      
#define VOUT_ISR_FUIF_Msk            (0x1U << VOUT_ISR_FUIF_Pos)               /**< 0x00000002 */
#define VOUT_ISR_FUIF                VOUT_ISR_FUIF_Msk                         /**< FIFO Underrun Interrupt Flag */
#define VOUT_ISR_TERRIF_Pos          (2U)                                      
#define VOUT_ISR_TERRIF_Msk          (0x1U << VOUT_ISR_TERRIF_Pos)             /**< 0x00000004 */
#define VOUT_ISR_TERRIF              VOUT_ISR_TERRIF_Msk                       /**< Transfer Error Interrupt Flag */

/********************  Bit definition for VOUT_LIPCR register  ****************/
#define VOUT_LIPCR_LIPOS_Pos         (0U)                                      
#define VOUT_LIPCR_LIPOS_Msk         (0x7FFU << VOUT_LIPCR_LIPOS_Pos)          /**< 0x000007FF */
#define VOUT_LIPCR_LIPOS             VOUT_LIPCR_LIPOS_Msk                      /**< Line Interrupt Position */

/********************  Bit definition for VOUT_CPSR register  *****************/
#define VOUT_CPSR_CYPOS_Pos          (0U)                                      
#define VOUT_CPSR_CYPOS_Msk          (0xFFFFU << VOUT_CPSR_CYPOS_Pos)          /**< 0x0000FFFF */
#define VOUT_CPSR_CYPOS              VOUT_CPSR_CYPOS_Msk                       /**< Current Y Position */
#define VOUT_CPSR_CXPOS_Pos          (16U)                                     
#define VOUT_CPSR_CXPOS_Msk          (0xFFFFU << VOUT_CPSR_CXPOS_Pos)          /**< 0xFFFF0000 */
#define VOUT_CPSR_CXPOS              VOUT_CPSR_CXPOS_Msk                       /**< Current X Position */

/********************  Bit definition for VOUT_CDSR register  *****************/
#define VOUT_CDSR_VDES_Pos           (0U)                                      
#define VOUT_CDSR_VDES_Msk           (0x1U << VOUT_CDSR_VDES_Pos)              /**< 0x00000001 */
#define VOUT_CDSR_VDES               VOUT_CDSR_VDES_Msk                        /**< Vertical Data Enable Status */
#define VOUT_CDSR_HDES_Pos           (1U)                                      
#define VOUT_CDSR_HDES_Msk           (0x1U << VOUT_CDSR_HDES_Pos)              /**< 0x00000002 */
#define VOUT_CDSR_HDES               VOUT_CDSR_HDES_Msk                        /**< Horizontal Data Enable Status */
#define VOUT_CDSR_VSYNCS_Pos         (2U)                                      
#define VOUT_CDSR_VSYNCS_Msk         (0x1U << VOUT_CDSR_VSYNCS_Pos)            /**< 0x00000004 */
#define VOUT_CDSR_VSYNCS             VOUT_CDSR_VSYNCS_Msk                      /**< Vertical Synchronization Status */
#define VOUT_CDSR_HSYNCS_Pos         (3U)                                      
#define VOUT_CDSR_HSYNCS_Msk         (0x1U << VOUT_CDSR_HSYNCS_Pos)            /**< 0x00000008 */
#define VOUT_CDSR_HSYNCS             VOUT_CDSR_HSYNCS_Msk                      /**< Horizontal Synchronization Status */

/********************  Bit definition for VOUT_LxCR register  *****************/
#define VOUT_LxCR_LEN_Pos            (0U)                                      
#define VOUT_LxCR_LEN_Msk            (0x1U << VOUT_LxCR_LEN_Pos)               /**< 0x00000001 */
#define VOUT_LxCR_LEN                VOUT_LxCR_LEN_Msk                         /**< Layer Enable */
#define VOUT_LxCR_COLKEN_Pos         (1U)                                      
#define VOUT_LxCR_COLKEN_Msk         (0x1U << VOUT_LxCR_COLKEN_Pos)            /**< 0x00000002 */
#define VOUT_LxCR_COLKEN             VOUT_LxCR_COLKEN_Msk                      /**< Color Keying Enable */
#define VOUT_LxCR_CLUTEN_Pos         (4U)                                      
#define VOUT_LxCR_CLUTEN_Msk         (0x1U << VOUT_LxCR_CLUTEN_Pos)            /**< 0x00000010 */
#define VOUT_LxCR_CLUTEN             VOUT_LxCR_CLUTEN_Msk                      /**< Color Lockup Table Enable */

#define VOUT_LxCR_PREMULEN_Pos       (8U)                                      
#define VOUT_LxCR_PREMULEN_Msk       (0x1U << VOUT_LxCR_PREMULEN_Pos)            /**< 0x00000100 */
#define VOUT_LxCR_PREMULEN           VOUT_LxCR_PREMULEN_Msk                    /**< pre multiply Enable */

/********************  Bit definition for VOUT_LxWHPCR register  **************/
#define VOUT_LxWHPCR_WHSTPOS_Pos     (0U)                                      
#define VOUT_LxWHPCR_WHSTPOS_Msk     (0xFFFU << VOUT_LxWHPCR_WHSTPOS_Pos)      /**< 0x00000FFF */
#define VOUT_LxWHPCR_WHSTPOS         VOUT_LxWHPCR_WHSTPOS_Msk                  /**< Window Horizontal Start Position */
#define VOUT_LxWHPCR_WHSPPOS_Pos     (16U)                                     
#define VOUT_LxWHPCR_WHSPPOS_Msk     (0xFFFFU << VOUT_LxWHPCR_WHSPPOS_Pos)     /**< 0xFFFF0000 */
#define VOUT_LxWHPCR_WHSPPOS         VOUT_LxWHPCR_WHSPPOS_Msk                  /**< Window Horizontal Stop Position */

/********************  Bit definition for VOUT_LxWVPCR register  **************/
#define VOUT_LxWVPCR_WVSTPOS_Pos     (0U)                                      
#define VOUT_LxWVPCR_WVSTPOS_Msk     (0xFFFU << VOUT_LxWVPCR_WVSTPOS_Pos)      /**< 0x00000FFF */
#define VOUT_LxWVPCR_WVSTPOS         VOUT_LxWVPCR_WVSTPOS_Msk                  /**< Window Vertical Start Position */
#define VOUT_LxWVPCR_WVSPPOS_Pos     (16U)                                     
#define VOUT_LxWVPCR_WVSPPOS_Msk     (0xFFFFU << VOUT_LxWVPCR_WVSPPOS_Pos)     /**< 0xFFFF0000 */
#define VOUT_LxWVPCR_WVSPPOS         VOUT_LxWVPCR_WVSPPOS_Msk                  /**< Window Vertical Stop Position */

/********************  Bit definition for VOUT_LxCKCR register  ***************/
#define VOUT_LxCKCR_CKBLUE_Pos       (0U)                                      
#define VOUT_LxCKCR_CKBLUE_Msk       (0xFFU << VOUT_LxCKCR_CKBLUE_Pos)         /**< 0x000000FF */
#define VOUT_LxCKCR_CKBLUE           VOUT_LxCKCR_CKBLUE_Msk                    /**< Color Key Blue value */
#define VOUT_LxCKCR_CKGREEN_Pos      (8U)                                      
#define VOUT_LxCKCR_CKGREEN_Msk      (0xFFU << VOUT_LxCKCR_CKGREEN_Pos)        /**< 0x0000FF00 */
#define VOUT_LxCKCR_CKGREEN          VOUT_LxCKCR_CKGREEN_Msk                   /**< Color Key Green value */
#define VOUT_LxCKCR_CKRED_Pos        (16U)                                     
#define VOUT_LxCKCR_CKRED_Msk        (0xFFU << VOUT_LxCKCR_CKRED_Pos)          /**< 0x00FF0000 */
#define VOUT_LxCKCR_CKRED            VOUT_LxCKCR_CKRED_Msk                     /**< Color Key Red value */

/********************  Bit definition for VOUT_LxPFCR register  ***************/
#define VOUT_LxPFCR_PF_Pos           (0U)                                      
#define VOUT_LxPFCR_PF_Msk           (0x7U << VOUT_LxPFCR_PF_Pos)              /**< 0x00000007 */
#define VOUT_LxPFCR_PF               VOUT_LxPFCR_PF_Msk                        /**< Pixel Format */

/********************  Bit definition for VOUT_LxCACR register  ***************/
#define VOUT_LxCACR_CONSTA_Pos       (0U)                                      
#define VOUT_LxCACR_CONSTA_Msk       (0xFFU << VOUT_LxCACR_CONSTA_Pos)         /**< 0x000000FF */
#define VOUT_LxCACR_CONSTA           VOUT_LxCACR_CONSTA_Msk                    /**< Constant Alpha */

/********************  Bit definition for VOUT_LxDCCR register  ***************/
#define VOUT_LxDCCR_DCBLUE_Pos       (0U)                                      
#define VOUT_LxDCCR_DCBLUE_Msk       (0xFFU << VOUT_LxDCCR_DCBLUE_Pos)         /**< 0x000000FF */
#define VOUT_LxDCCR_DCBLUE           VOUT_LxDCCR_DCBLUE_Msk                    /**< Default Color Blue */
#define VOUT_LxDCCR_DCGREEN_Pos      (8U)                                      
#define VOUT_LxDCCR_DCGREEN_Msk      (0xFFU << VOUT_LxDCCR_DCGREEN_Pos)        /**< 0x0000FF00 */
#define VOUT_LxDCCR_DCGREEN          VOUT_LxDCCR_DCGREEN_Msk                   /**< Default Color Green */
#define VOUT_LxDCCR_DCRED_Pos        (16U)                                     
#define VOUT_LxDCCR_DCRED_Msk        (0xFFU << VOUT_LxDCCR_DCRED_Pos)          /**< 0x00FF0000 */
#define VOUT_LxDCCR_DCRED            VOUT_LxDCCR_DCRED_Msk                     /**< Default Color Red */
#define VOUT_LxDCCR_DCALPHA_Pos      (24U)                                     
#define VOUT_LxDCCR_DCALPHA_Msk      (0xFFU << VOUT_LxDCCR_DCALPHA_Pos)        /**< 0xFF000000 */
#define VOUT_LxDCCR_DCALPHA          VOUT_LxDCCR_DCALPHA_Msk                   /**< Default Color Alpha */

/********************  Bit definition for VOUT_LxCFBAR register  **************/
#define VOUT_LxCFBAR_CFBADD_Pos      (0U)                                      
#define VOUT_LxCFBAR_CFBADD_Msk      (0xFFFFFFFFU << VOUT_LxCFBAR_CFBADD_Pos)  /**< 0xFFFFFFFF */
#define VOUT_LxCFBAR_CFBADD          VOUT_LxCFBAR_CFBADD_Msk                   /**< Color Frame Buffer Start Address */

/********************  Bit definition for VOUT_LxCFBLR register  **************/
#define VOUT_LxCFBLR_CFBLL_Pos       (0U)                                      
#define VOUT_LxCFBLR_CFBLL_Msk       (0x1FFFU << VOUT_LxCFBLR_CFBLL_Pos)       /**< 0x00001FFF */
#define VOUT_LxCFBLR_CFBLL           VOUT_LxCFBLR_CFBLL_Msk                    /**< Color Frame Buffer Line Length */
#define VOUT_LxCFBLR_CFBP_Pos        (16U)                                     
#define VOUT_LxCFBLR_CFBP_Msk        (0x1FFFU << VOUT_LxCFBLR_CFBP_Pos)        /**< 0x1FFF0000 */
#define VOUT_LxCFBLR_CFBP            VOUT_LxCFBLR_CFBP_Msk                     /**< Color Frame Buffer Pitch in bytes */

/********************  Bit definition for VOUT_LxCFBLNR register  *************/
#define VOUT_LxCFBLNR_CFBLNBR_Pos    (0U)                                      
#define VOUT_LxCFBLNR_CFBLNBR_Msk    (0x7FFU << VOUT_LxCFBLNR_CFBLNBR_Pos)     /**< 0x000007FF */
#define VOUT_LxCFBLNR_CFBLNBR        VOUT_LxCFBLNR_CFBLNBR_Msk                 /**< Frame Buffer Line Number */

/********************  Bit definition for VOUT_LxCLUTWR register  *************/
#define VOUT_LxCLUTWR_BLUE_Pos       (0U)                                      
#define VOUT_LxCLUTWR_BLUE_Msk       (0xFFU << VOUT_LxCLUTWR_BLUE_Pos)         /**< 0x000000FF */
#define VOUT_LxCLUTWR_BLUE           VOUT_LxCLUTWR_BLUE_Msk                    /**< Blue value */
#define VOUT_LxCLUTWR_GREEN_Pos      (8U)                                      
#define VOUT_LxCLUTWR_GREEN_Msk      (0xFFU << VOUT_LxCLUTWR_GREEN_Pos)        /**< 0x0000FF00 */
#define VOUT_LxCLUTWR_GREEN          VOUT_LxCLUTWR_GREEN_Msk                   /**< Green value */
#define VOUT_LxCLUTWR_RED_Pos        (16U)                                     
#define VOUT_LxCLUTWR_RED_Msk        (0xFFU << VOUT_LxCLUTWR_RED_Pos)          /**< 0x00FF0000 */
#define VOUT_LxCLUTWR_RED            VOUT_LxCLUTWR_RED_Msk                     /**< Red value */
#define VOUT_LxCLUTWR_CLUTADD_Pos    (24U)                                     
#define VOUT_LxCLUTWR_CLUTADD_Msk    (0xFFU << VOUT_LxCLUTWR_CLUTADD_Pos)      /**< 0xFF000000 */
#define VOUT_LxCLUTWR_CLUTADD        VOUT_LxCLUTWR_CLUTADD_Msk                 /**< CLUT address */

/*******************************************************************************
 * Global function
 ******************************************************************************/
#define VOUT_LAYER(__HANDLE__, __LAYER__)    ((VOUT_Layer_TypeDef *)((uint32_t)(((uint32_t)(__HANDLE__)) + 0x30U + (0x30U*(__LAYER__)))))

/**
  * @brief  Enable the VOUT.
  * @param  pVOUT: pVOUT devive Point.
  * @retval None.
  */
#define LL_VOUT_ENABLE(pVOUT)    (pVOUT->GCR |= VOUT_GCR_VOUTEN)

/**
  * @brief  Disable the VOUT.
  * @param  pVOUT: pVOUT devive Point.
  * @retval None.
  */
#define LL_VOUT_DISABLE(pVOUT)   (pVOUT->GCR &= ~(VOUT_GCR_VOUTEN))

#ifdef __cplusplus
  }
#endif
#endif /* #ifndef _GM_LL_VOUT_H_ */

