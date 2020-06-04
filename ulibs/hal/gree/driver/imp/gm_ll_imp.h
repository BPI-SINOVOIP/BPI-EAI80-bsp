/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_imp.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2018/1/3 
 *    
 * @brief         IMP peripheral PHY device header file
 * 
 * @note
 *    2018/1/3, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GM_LL_IMP_H_
#define _GM_LL_IMP_H_

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_imp_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/********************  Bit definition for IMP_CR register  ******************/
#define IMP_CR_START_Pos         (0U)                                        
#define IMP_CR_START_Msk         (0x1U << IMP_CR_START_Pos)                /**< 0x00000001 */
#define IMP_CR_START             IMP_CR_START_Msk                          /**< Start transfer */
#define IMP_CR_SUSP_Pos          (1U)                                        
#define IMP_CR_SUSP_Msk          (0x1U << IMP_CR_SUSP_Pos)                 /**< 0x00000002 */
#define IMP_CR_SUSP              IMP_CR_SUSP_Msk                           /**< Suspend transfer */
#define IMP_CR_ABORT_Pos         (2U)                                        
#define IMP_CR_ABORT_Msk         (0x1U << IMP_CR_ABORT_Pos)                /**< 0x00000004 */
#define IMP_CR_ABORT             IMP_CR_ABORT_Msk                          /**< Abort transfer */
#define IMP_CR_TEIE_Pos          (8U)                                        
#define IMP_CR_TEIE_Msk          (0x1U << IMP_CR_TEIE_Pos)                 /**< 0x00000100 */
#define IMP_CR_TEIE              IMP_CR_TEIE_Msk                           /**< Transfer Error Interrupt Enable */
#define IMP_CR_TCIE_Pos          (9U)                                        
#define IMP_CR_TCIE_Msk          (0x1U << IMP_CR_TCIE_Pos)                 /**< 0x00000200 */
#define IMP_CR_TCIE              IMP_CR_TCIE_Msk                           /**< Transfer Complete Interrupt Enable */
#define IMP_CR_TWIE_Pos          (10U)                                       
#define IMP_CR_TWIE_Msk          (0x1U << IMP_CR_TWIE_Pos)                 /**< 0x00000400 */
#define IMP_CR_TWIE              IMP_CR_TWIE_Msk                           /**< Transfer Watermark Interrupt Enable */
#define IMP_CR_CAEIE_Pos         (11U)                                       
#define IMP_CR_CAEIE_Msk         (0x1U << IMP_CR_CAEIE_Pos)                /**< 0x00000800 */
#define IMP_CR_CAEIE             IMP_CR_CAEIE_Msk                          /**< CLUT Access Error Interrupt Enable */
#define IMP_CR_CTCIE_Pos         (12U)                                       
#define IMP_CR_CTCIE_Msk         (0x1U << IMP_CR_CTCIE_Pos)                /**< 0x00001000 */
#define IMP_CR_CTCIE             IMP_CR_CTCIE_Msk                          /**< CLUT Transfer Complete Interrupt Enable */
#define IMP_CR_CEIE_Pos          (13U)                                       
#define IMP_CR_CEIE_Msk          (0x1U << IMP_CR_CEIE_Pos)                 /**< 0x00002000 */
#define IMP_CR_CEIE              IMP_CR_CEIE_Msk                           /**< Configuration Error Interrupt Enable */
#define IMP_CR_MODE_Pos          (16U)                                       
#define IMP_CR_MODE_Msk          (0x3U << IMP_CR_MODE_Pos)                 /**< 0x00030000 */
#define IMP_CR_MODE              IMP_CR_MODE_Msk                           /**< IMP Mode[1:0] */
#define IMP_CR_MODE_0            (0x1U << IMP_CR_MODE_Pos)                 /**< 0x00010000 */
#define IMP_CR_MODE_1            (0x2U << IMP_CR_MODE_Pos)                 /**< 0x00020000 */

#define IMP_CR_SCA_Pos           (24U)                                        
#define IMP_CR_SCA_Msk           (0x1U << IMP_CR_SCA_Pos)                  /**< 0x01000000 */
#define IMP_CR_SCA               IMP_CR_SCA_Msk                            /**< Scaler enable */

#define IMP_CR_HAR_EN_Pos        (25U)
#define IMP_CR_HAR_EN_Msk        (0x1U << IMP_CR_HAR_EN_Pos)                  /**< 0x02000000 */
#define IMP_CR_HAR_EN             IMP_CR_HAR_EN_Msk                            /**<IMP harris enable */
/********************  Bit definition for IMP_ISR register  *****************/
#define IMP_ISR_TEIF_Pos         (0U)                                        
#define IMP_ISR_TEIF_Msk         (0x1U << IMP_ISR_TEIF_Pos)                /**< 0x00000001 */
#define IMP_ISR_TEIF             IMP_ISR_TEIF_Msk                          /**< Transfer Error Interrupt Flag */
#define IMP_ISR_TCIF_Pos         (1U)                                        
#define IMP_ISR_TCIF_Msk         (0x1U << IMP_ISR_TCIF_Pos)                /**< 0x00000002 */
#define IMP_ISR_TCIF             IMP_ISR_TCIF_Msk                          /**< Transfer Complete Interrupt Flag */
#define IMP_ISR_TWIF_Pos         (2U)                                        
#define IMP_ISR_TWIF_Msk         (0x1U << IMP_ISR_TWIF_Pos)                /**< 0x00000004 */
#define IMP_ISR_TWIF             IMP_ISR_TWIF_Msk                          /**< Transfer Watermark Interrupt Flag */
#define IMP_ISR_CAEIF_Pos        (3U)                                        
#define IMP_ISR_CAEIF_Msk        (0x1U << IMP_ISR_CAEIF_Pos)               /**< 0x00000008 */
#define IMP_ISR_CAEIF            IMP_ISR_CAEIF_Msk                         /**< CLUT Access Error Interrupt Flag */
#define IMP_ISR_CTCIF_Pos        (4U)                                        
#define IMP_ISR_CTCIF_Msk        (0x1U << IMP_ISR_CTCIF_Pos)               /**< 0x00000010 */
#define IMP_ISR_CTCIF            IMP_ISR_CTCIF_Msk                         /**< CLUT Transfer Complete Interrupt Flag */
#define IMP_ISR_CEIF_Pos         (5U)                                        
#define IMP_ISR_CEIF_Msk         (0x1U << IMP_ISR_CEIF_Pos)                /**< 0x00000020 */
#define IMP_ISR_CEIF             IMP_ISR_CEIF_Msk                          /**< Configuration Error Interrupt Flag */

/********************  Bit definition for IMP_FGMAR register  ***************/
#define IMP_FGMAR_MA_Pos         (0U)                                        
#define IMP_FGMAR_MA_Msk         (0xFFFFFFFFU << IMP_FGMAR_MA_Pos)         /**< 0xFFFFFFFF */
#define IMP_FGMAR_MA             IMP_FGMAR_MA_Msk                          /**< Memory Address */

/********************  Bit definition for IMP_FGOR register  ****************/
#define IMP_FGOR_LO_Pos          (0U)                                        
#define IMP_FGOR_LO_Msk          (0x3FFFU << IMP_FGOR_LO_Pos)              /**< 0x00003FFF */
#define IMP_FGOR_LO              IMP_FGOR_LO_Msk                           /**< Line Offset */

/********************  Bit definition for IMP_BGMAR register  ***************/
#define IMP_BGMAR_MA_Pos         (0U)                                        
#define IMP_BGMAR_MA_Msk         (0xFFFFFFFFU << IMP_BGMAR_MA_Pos)         /**< 0xFFFFFFFF */
#define IMP_BGMAR_MA             IMP_BGMAR_MA_Msk                          /**< Memory Address */

/********************  Bit definition for IMP_BGOR register  ****************/
#define IMP_BGOR_LO_Pos          (0U)                                        
#define IMP_BGOR_LO_Msk          (0x3FFFU << IMP_BGOR_LO_Pos)              /**< 0x00003FFF */
#define IMP_BGOR_LO              IMP_BGOR_LO_Msk                           /**< Line Offset */

/********************  Bit definition for IMP_FGPFCCR register  *************/
#define IMP_FGPFCCR_CM_Pos       (0U)                                        
#define IMP_FGPFCCR_CM_Msk       (0xFU << IMP_FGPFCCR_CM_Pos)              /**< 0x0000000F */
#define IMP_FGPFCCR_CM           IMP_FGPFCCR_CM_Msk                        /**< Input color mode CM[3:0] */
#define IMP_FGPFCCR_CM_0         (0x1U << IMP_FGPFCCR_CM_Pos)              /**< 0x00000001 */
#define IMP_FGPFCCR_CM_1         (0x2U << IMP_FGPFCCR_CM_Pos)              /**< 0x00000002 */
#define IMP_FGPFCCR_CM_2         (0x4U << IMP_FGPFCCR_CM_Pos)              /**< 0x00000004 */
#define IMP_FGPFCCR_CM_3         (0x8U << IMP_FGPFCCR_CM_Pos)              /**< 0x00000008 */
#define IMP_FGPFCCR_CCM_Pos      (4U)                                        
#define IMP_FGPFCCR_CCM_Msk      (0x1U << IMP_FGPFCCR_CCM_Pos)             /**< 0x00000010 */
#define IMP_FGPFCCR_CCM          IMP_FGPFCCR_CCM_Msk                       /**< CLUT Color mode */
#define IMP_FGPFCCR_CLUTEN_Pos   (6U)                                        
#define IMP_FGPFCCR_CLUTEN_Msk   (0xFFU << IMP_FGPFCCR_CLUTEN_Pos)         /**< 0x0000FF00 */
#define IMP_FGPFCCR_CLUTEN       IMP_FGPFCCR_CLUTEN_Msk                    /**< CLUT Enable */
#define IMP_FGPFCCR_AM_Pos       (16U)                                       
#define IMP_FGPFCCR_AM_Msk       (0x3U << IMP_FGPFCCR_AM_Pos)              /**< 0x00030000 */
#define IMP_FGPFCCR_AM           IMP_FGPFCCR_AM_Msk                        /**< Alpha mode AM[1:0] */
#define IMP_FGPFCCR_AM_0         (0x1U << IMP_FGPFCCR_AM_Pos)              /**< 0x00010000 */
#define IMP_FGPFCCR_AM_1         (0x2U << IMP_FGPFCCR_AM_Pos)              /**< 0x00020000 */
#define IMP_FGPFCCR_ALPHA_Pos    (24U)                                       
#define IMP_FGPFCCR_ALPHA_Msk    (0xFFU << IMP_FGPFCCR_ALPHA_Pos)          /**< 0xFF000000 */
#define IMP_FGPFCCR_ALPHA        IMP_FGPFCCR_ALPHA_Msk                     /**< Alpha value */

/********************  Bit definition for IMP_FGCOLR register  **************/
#define IMP_FGCOLR_BLUE_Pos      (0U)                                        
#define IMP_FGCOLR_BLUE_Msk      (0xFFU << IMP_FGCOLR_BLUE_Pos)            /**< 0x000000FF */
#define IMP_FGCOLR_BLUE          IMP_FGCOLR_BLUE_Msk                       /**< Blue Value */
#define IMP_FGCOLR_GREEN_Pos     (8U)                                        
#define IMP_FGCOLR_GREEN_Msk     (0xFFU << IMP_FGCOLR_GREEN_Pos)           /**< 0x0000FF00 */
#define IMP_FGCOLR_GREEN         IMP_FGCOLR_GREEN_Msk                      /**< Green Value */
#define IMP_FGCOLR_RED_Pos       (16U)                                       
#define IMP_FGCOLR_RED_Msk       (0xFFU << IMP_FGCOLR_RED_Pos)             /**< 0x00FF0000 */
#define IMP_FGCOLR_RED           IMP_FGCOLR_RED_Msk                        /**< Red Value */   

/********************  Bit definition for IMP_BGPFCCR register  *************/
#define IMP_BGPFCCR_CM_Pos       (0U)                                        
#define IMP_BGPFCCR_CM_Msk       (0xFU << IMP_BGPFCCR_CM_Pos)              /**< 0x0000000F */
#define IMP_BGPFCCR_CM           IMP_BGPFCCR_CM_Msk                        /**< Input color mode CM[3:0] */
#define IMP_BGPFCCR_CM_0         (0x1U << IMP_BGPFCCR_CM_Pos)              /**< 0x00000001 */
#define IMP_BGPFCCR_CM_1         (0x2U << IMP_BGPFCCR_CM_Pos)              /**< 0x00000002 */
#define IMP_BGPFCCR_CM_2         (0x4U << IMP_BGPFCCR_CM_Pos)              /**< 0x00000004 */
#define IMP_BGPFCCR_CM_3         0x00000008U                                 /**< Input color mode CM bit 3 */
#define IMP_BGPFCCR_CCM_Pos      (4U)                                        
#define IMP_BGPFCCR_CCM_Msk      (0x1U << IMP_BGPFCCR_CCM_Pos)             /**< 0x00000010 */
#define IMP_BGPFCCR_CCM          IMP_BGPFCCR_CCM_Msk                       /**< CLUT Color mode */
#define IMP_BGPFCCR_CLUTEN_Pos   (6U)                                        
#define IMP_BGPFCCR_CLUTEN_Msk   (0x1U << IMP_BGPFCCR_CLUTEN_Pos)          /**< 0x0000FF00 */
#define IMP_BGPFCCR_CLUTEN       IMP_BGPFCCR_CLUTEN_Msk                    /**< CLUT Enable */
#define IMP_BGPFCCR_AM_Pos       (16U)                                       
#define IMP_BGPFCCR_AM_Msk       (0x3U << IMP_BGPFCCR_AM_Pos)              /**< 0x00030000 */
#define IMP_BGPFCCR_AM           IMP_BGPFCCR_AM_Msk                        /**< Alpha mode AM[1:0] */
#define IMP_BGPFCCR_AM_0         (0x1U << IMP_BGPFCCR_AM_Pos)              /**< 0x00010000 */
#define IMP_BGPFCCR_AM_1         (0x2U << IMP_BGPFCCR_AM_Pos)              /**< 0x00020000 */
#define IMP_BGPFCCR_ALPHA_Pos    (24U)                                       
#define IMP_BGPFCCR_ALPHA_Msk    (0xFFU << IMP_BGPFCCR_ALPHA_Pos)          /**< 0xFF000000 */
#define IMP_BGPFCCR_ALPHA        IMP_BGPFCCR_ALPHA_Msk                     /**< background Input Alpha value */

/********************  Bit definition for IMP_BGCOLR register  **************/
#define IMP_BGCOLR_BLUE_Pos      (0U)                                        
#define IMP_BGCOLR_BLUE_Msk      (0xFFU << IMP_BGCOLR_BLUE_Pos)            /**< 0x000000FF */
#define IMP_BGCOLR_BLUE          IMP_BGCOLR_BLUE_Msk                       /**< Blue Value */
#define IMP_BGCOLR_GREEN_Pos     (8U)                                        
#define IMP_BGCOLR_GREEN_Msk     (0xFFU << IMP_BGCOLR_GREEN_Pos)           /**< 0x0000FF00 */
#define IMP_BGCOLR_GREEN         IMP_BGCOLR_GREEN_Msk                      /**< Green Value */
#define IMP_BGCOLR_RED_Pos       (16U)                                       
#define IMP_BGCOLR_RED_Msk       (0xFFU << IMP_BGCOLR_RED_Pos)             /**< 0x00FF0000 */
#define IMP_BGCOLR_RED           IMP_BGCOLR_RED_Msk                        /**< Red Value */

/********************  Bit definition for IMP_FGCMAR register  **************/
#define IMP_FGCMAR_MA_Pos        (0U)                                        
#define IMP_FGCMAR_MA_Msk        (0xFFFFFFFFU << IMP_FGCMAR_MA_Pos)        /**< 0xFFFFFFFF */
#define IMP_FGCMAR_MA            IMP_FGCMAR_MA_Msk                         /**< Memory Address */

/********************  Bit definition for IMP_BGCMAR register  **************/
#define IMP_BGCMAR_MA_Pos        (0U)                                        
#define IMP_BGCMAR_MA_Msk        (0xFFFFFFFFU << IMP_BGCMAR_MA_Pos)        /**< 0xFFFFFFFF */
#define IMP_BGCMAR_MA            IMP_BGCMAR_MA_Msk                         /**< Memory Address */

/********************  Bit definition for IMP_OPFCCR register  **************/
#define IMP_OPFCCR_CM_Pos        (0U)                                        
#define IMP_OPFCCR_CM_Msk        (0x7U << IMP_OPFCCR_CM_Pos)               /**< 0x00000007 */
#define IMP_OPFCCR_CM            IMP_OPFCCR_CM_Msk                         /**< Color mode CM[2:0] */
#define IMP_OPFCCR_CM_0          (0x1U << IMP_OPFCCR_CM_Pos)               /**< 0x00000001 */
#define IMP_OPFCCR_CM_1          (0x2U << IMP_OPFCCR_CM_Pos)               /**< 0x00000002 */
#define IMP_OPFCCR_CM_2          (0x4U << IMP_OPFCCR_CM_Pos)               /**< 0x00000004 */

/********************  Bit definition for IMP_OCOLR register  ***************/
/**<Mode_ARGB8888/RGB888 */
#define IMP_OCOLR_BLUE_1         0x000000FFU                                 /**< BLUE Value */
#define IMP_OCOLR_GREEN_1        0x0000FF00U                                 /**< GREEN Value  */
#define IMP_OCOLR_RED_1          0x00FF0000U                                 /**< Red Value */
#define IMP_OCOLR_ALPHA_1        0xFF000000U                                 /**< Alpha Channel Value */

/**<Mode_RGB565 */
#define IMP_OCOLR_BLUE_2         0x0000001FU                                 /**< BLUE Value */
#define IMP_OCOLR_GREEN_2        0x000007E0U                                 /**< GREEN Value  */
#define IMP_OCOLR_RED_2          0x0000F800U                                 /**< Red Value */

/**<Mode_ARGB1555 */
#define IMP_OCOLR_BLUE_3         0x0000001FU                                 /**< BLUE Value */
#define IMP_OCOLR_GREEN_3        0x000003E0U                                 /**< GREEN Value  */
#define IMP_OCOLR_RED_3          0x00007C00U                                 /**< Red Value */
#define IMP_OCOLR_ALPHA_3        0x00008000U                                 /**< Alpha Channel Value */

/**<Mode_ARGB4444 */
#define IMP_OCOLR_BLUE_4         0x0000000FU                                 /**< BLUE Value */
#define IMP_OCOLR_GREEN_4        0x000000F0U                                 /**< GREEN Value  */
#define IMP_OCOLR_RED_4          0x00000F00U                                 /**< Red Value */
#define IMP_OCOLR_ALPHA_4        0x0000F000U                                 /**< Alpha Channel Value */

/********************  Bit definition for IMP_OMAR register  ****************/
#define IMP_OMAR_MA_Pos          (0U)                                        
#define IMP_OMAR_MA_Msk          (0xFFFFFFFFU << IMP_OMAR_MA_Pos)          /**< 0xFFFFFFFF */
#define IMP_OMAR_MA              IMP_OMAR_MA_Msk                           /**< Memory Address */

/********************  Bit definition for IMP_OOR register  *****************/
#define IMP_OOR_LO_Pos           (0U)                                        
#define IMP_OOR_LO_Msk           (0x3FFFU << IMP_OOR_LO_Pos)               /**< 0x00003FFF */
#define IMP_OOR_LO               IMP_OOR_LO_Msk                            /**< Line Offset */

/********************  Bit definition for IMP_NLR register  *****************/
#define IMP_NLR_NL_Pos           (0U)                                        
#define IMP_NLR_NL_Msk           (0xFFFFU << IMP_NLR_NL_Pos)               /**< 0x0000FFFF */
#define IMP_NLR_NL               IMP_NLR_NL_Msk                            /**< Number of Lines */
#define IMP_NLR_PL_Pos           (16U)                                       
#define IMP_NLR_PL_Msk           (0x3FFFU << IMP_NLR_PL_Pos)               /**< 0x3FFF0000 */
#define IMP_NLR_PL               IMP_NLR_PL_Msk                            /**< Pixel per Lines */

/********************  Bit definition for IMP_LWR register  *****************/
#define IMP_LWR_LW_Pos           (0U)                                        
#define IMP_LWR_LW_Msk           (0xFFFFU << IMP_LWR_LW_Pos)               /**< 0x0000FFFF */
#define IMP_LWR_LW               IMP_LWR_LW_Msk                            /**< Line Watermark */

/********************  Bit definition for IMP_SNLR register  *****************/
#define IMP_SNLR_NL_Pos          (0U)                                        
#define IMP_SNLR_NL_Msk          (0xFFFFU << IMP_SNLR_NL_Pos)               /**< 0x0000FFFF */
#define IMP_SNLR_NL              IMP_SNLR_NL_Msk                            /**< Number of Lines */
#define IMP_SNLR_PL_Pos          (16U)                                       
#define IMP_SNLR_PL_Msk          (0x3FFFU << IMP_SNLR_PL_Pos)               /**< 0x3FFF0000 */
#define IMP_SNLR_PL              IMP_SNLR_PL_Msk                            /**< Pixel per Lines */

/********************  Bit definition for IMP_SCR register  **************/
#define IMP_SCR_SHF_Pos          (0U)                                        
#define IMP_SCR_SHF_Msk          (0xFFU << IMP_SCR_SHF_Pos)            /**< 0x000000FF */
#define IMP_SCR_SHF              IMP_SCR_SHF_Msk                       /**< shf Value */
#define IMP_SCR_SHI_Pos          (8U)                                        
#define IMP_SCR_SHI_Msk          (0xFFU << IMP_SCR_SHI_Pos)            /**< 0x0000FF00 */
#define IMP_SCR_SHI              IMP_SCR_SHI_Msk                       /**< shi Value */
#define IMP_SCR_SVF_Pos          (16U)                                       
#define IMP_SCR_SVF_Msk          (0xFFU << IMP_SCR_SVF_Pos)             /**< 0x00FF0000 */
#define IMP_SCR_SVF              IMP_SCR_SVF_Msk                        /**< svf Value */
#define IMP_SCR_SVI_Pos          (24U)                                       
#define IMP_SCR_SVI_Msk          (0xFFU << IMP_SCR_SVI_Pos)             /**< 0xFF000000 */
#define IMP_SCR_SVI              IMP_SCR_SVI_Msk                        /**< svi Value */


/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
  * @brief  Enable the IMP.
  * @param  pIMP: pIMP devive Point.
  * @retval None.
  */
#define LL_IMP_ENABLE(pIMP)    (pIMP->CR |= IMP_CR_START)

/**
  * @brief  Disable the IMP.
  * @param  pIMP: pIMP devive Point.
  * @retval None.
  */
#define LL_IMP_DISABLE(pIMP)   (pIMP->CR &= ~(IMP_CR_START))

#ifdef __cplusplus
  }
#endif
#endif /* #ifndef _GM_LL_IMP_H_ */

