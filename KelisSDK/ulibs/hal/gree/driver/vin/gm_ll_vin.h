/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_vin.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/11/28 
 *    
 * @brief         VIN peripheral PHY device header file
 * 
 * @note
 *    2016/11/28, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GM_LL_VIN_H_
#define _GM_LL_VIN_H_

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_vin_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/********************  Bits definition for VIN_CR register  ******************/
#define VIN_CR_CAPTURE_Pos        (0U)                                        
#define VIN_CR_CAPTURE_Msk        (0x1U << VIN_CR_CAPTURE_Pos)               /**< 0x00000001 */
#define VIN_CR_CAPTURE            VIN_CR_CAPTURE_Msk                         
#define VIN_CR_CM_Pos             (1U)                                        
#define VIN_CR_CM_Msk             (0x1U << VIN_CR_CM_Pos)                    /**< 0x00000002 */
#define VIN_CR_CM                 VIN_CR_CM_Msk                              
#define VIN_CR_CROP_Pos           (2U)                                        
#define VIN_CR_CROP_Msk           (0x1U << VIN_CR_CROP_Pos)                  /**< 0x00000004 */
#define VIN_CR_CROP               VIN_CR_CROP_Msk                            
#define VIN_CR_JPEG_Pos           (3U)                                        
#define VIN_CR_JPEG_Msk           (0x1U << VIN_CR_JPEG_Pos)                  /**< 0x00000008 */
#define VIN_CR_JPEG               VIN_CR_JPEG_Msk                            
#define VIN_CR_ESS_Pos            (4U)                                        
#define VIN_CR_ESS_Msk            (0x1U << VIN_CR_ESS_Pos)                   /**< 0x00000010 */
#define VIN_CR_ESS                VIN_CR_ESS_Msk                             
#define VIN_CR_PCKPOL_Pos         (5U)                                        
#define VIN_CR_PCKPOL_Msk         (0x1U << VIN_CR_PCKPOL_Pos)                /**< 0x00000020 */
#define VIN_CR_PCKPOL             VIN_CR_PCKPOL_Msk                          
#define VIN_CR_HSPOL_Pos          (6U)                                        
#define VIN_CR_HSPOL_Msk          (0x1U << VIN_CR_HSPOL_Pos)                 /**< 0x00000040 */
#define VIN_CR_HSPOL              VIN_CR_HSPOL_Msk                           
#define VIN_CR_VSPOL_Pos          (7U)                                        
#define VIN_CR_VSPOL_Msk          (0x1U << VIN_CR_VSPOL_Pos)                 /**< 0x00000080 */
#define VIN_CR_VSPOL              VIN_CR_VSPOL_Msk                           
#define VIN_CR_FCRC_0             0x00000100U                                 
#define VIN_CR_FCRC_1             0x00000200U                                 
#define VIN_CR_EDM_0              0x00000400U                                 
#define VIN_CR_EDM_1              0x00000800U                                 
#define VIN_CR_CRE_Pos            (12U)                                       
#define VIN_CR_CRE_Msk            (0x1U << VIN_CR_CRE_Pos)                   /**< 0x00001000 */
#define VIN_CR_CRE                VIN_CR_CRE_Msk                             
#define VIN_CR_ENABLE_Pos         (14U)                                       
#define VIN_CR_ENABLE_Msk         (0x1U << VIN_CR_ENABLE_Pos)                /**< 0x00004000 */
#define VIN_CR_ENABLE             VIN_CR_ENABLE_Msk                          

#define VIN_CR_CSC_Pos            (15U)                                       
#define VIN_CR_CSC_Msk            (0x1U << VIN_CR_CSC_Pos)                  /**< 0x00008000 */
#define VIN_CR_CSC                 VIN_CR_CSC_Msk           

#define VIN_CR_MEMWR_Pos          (16U)                                       
#define VIN_CR_MEMWR_Msk          (0x1U << VIN_CR_MEMWR_Pos)                /**< 0x00010000 */
#define VIN_CR_MEMWR              VIN_CR_MEMWR_Msk           

#define VIN_CR_PATHSEL_Pos        (17U)                                       
#define VIN_CR_PATHSEL_Msk        (0x1U << VIN_CR_PATHSEL_Pos)              /**< 0x00020000 */
#define VIN_CR_PATHSEL            VIN_CR_PATHSEL_Msk           

/********************  Bits definition for VIN_SR register  ******************/
#define VIN_SR_HSYNC_Pos          (0U)                                        
#define VIN_SR_HSYNC_Msk          (0x1U << VIN_SR_HSYNC_Pos)                 /**< 0x00000001 */
#define VIN_SR_HSYNC              VIN_SR_HSYNC_Msk                           
#define VIN_SR_VSYNC_Pos          (1U)                                        
#define VIN_SR_VSYNC_Msk          (0x1U << VIN_SR_VSYNC_Pos)                 /**< 0x00000002 */
#define VIN_SR_VSYNC              VIN_SR_VSYNC_Msk                           
#define VIN_SR_FNE_Pos            (2U)                                        
#define VIN_SR_FNE_Msk            (0x1U << VIN_SR_FNE_Pos)                   /**< 0x00000004 */
#define VIN_SR_FNE                VIN_SR_FNE_Msk                             

/********************  Bits definition for VIN_RIS register  *****************/
#define VIN_RIS_FRAME_RIS_Pos     (0U)                                        
#define VIN_RIS_FRAME_RIS_Msk     (0x1U << VIN_RIS_FRAME_RIS_Pos)            /**< 0x00000001 */
#define VIN_RIS_FRAME_RIS         VIN_RIS_FRAME_RIS_Msk                      
#define VIN_RIS_OVR_RIS_Pos       (1U)                                        
#define VIN_RIS_OVR_RIS_Msk       (0x1U << VIN_RIS_OVR_RIS_Pos)              /**< 0x00000002 */
#define VIN_RIS_OVR_RIS           VIN_RIS_OVR_RIS_Msk                        
#define VIN_RIS_ERR_RIS_Pos       (2U)                                        
#define VIN_RIS_ERR_RIS_Msk       (0x1U << VIN_RIS_ERR_RIS_Pos)              /**< 0x00000004 */
#define VIN_RIS_ERR_RIS           VIN_RIS_ERR_RIS_Msk                        
#define VIN_RIS_VSYNC_RIS_Pos     (3U)                                        
#define VIN_RIS_VSYNC_RIS_Msk     (0x1U << VIN_RIS_VSYNC_RIS_Pos)            /**< 0x00000008 */
#define VIN_RIS_VSYNC_RIS         VIN_RIS_VSYNC_RIS_Msk                      
#define VIN_RIS_LINE_RIS_Pos      (4U)                                        
#define VIN_RIS_LINE_RIS_Msk      (0x1U << VIN_RIS_LINE_RIS_Pos)             /**< 0x00000010 */
#define VIN_RIS_LINE_RIS          VIN_RIS_LINE_RIS_Msk                       

/********************  Bits definition for VIN_IER register  *****************/
#define VIN_IER_FRAME_IE_Pos      (0U)                                        
#define VIN_IER_FRAME_IE_Msk      (0x1U << VIN_IER_FRAME_IE_Pos)             /**< 0x00000001 */
#define VIN_IER_FRAME_IE          VIN_IER_FRAME_IE_Msk                       
#define VIN_IER_OVR_IE_Pos        (1U)                                        
#define VIN_IER_OVR_IE_Msk        (0x1U << VIN_IER_OVR_IE_Pos)               /**< 0x00000002 */
#define VIN_IER_OVR_IE            VIN_IER_OVR_IE_Msk                         
#define VIN_IER_ERR_IE_Pos        (2U)                                        
#define VIN_IER_ERR_IE_Msk        (0x1U << VIN_IER_ERR_IE_Pos)               /**< 0x00000004 */
#define VIN_IER_ERR_IE            VIN_IER_ERR_IE_Msk                         
#define VIN_IER_VSYNC_IE_Pos      (3U)                                        
#define VIN_IER_VSYNC_IE_Msk      (0x1U << VIN_IER_VSYNC_IE_Pos)             /**< 0x00000008 */
#define VIN_IER_VSYNC_IE          VIN_IER_VSYNC_IE_Msk                       
#define VIN_IER_LINE_IE_Pos       (4U)                                        
#define VIN_IER_LINE_IE_Msk       (0x1U << VIN_IER_LINE_IE_Pos)              /**< 0x00000010 */
#define VIN_IER_LINE_IE           VIN_IER_LINE_IE_Msk                        

/********************  Bits definition for VIN_ESCR register  ******************/
#define VIN_ESCR_FSC_Pos          (0U)                                        
#define VIN_ESCR_FSC_Msk          (0xFFU << VIN_ESCR_FSC_Pos)                /**< 0x000000FF */
#define VIN_ESCR_FSC_Field(x)     (((uint32_t)(((uint32_t)(x)) << VIN_ESCR_FSC_Pos)) & VIN_ESCR_FSC_Msk)
                        
#define VIN_ESCR_LSC_Pos          (8U)                                        
#define VIN_ESCR_LSC_Msk          (0xFFU << VIN_ESCR_LSC_Pos)                /**< 0x0000FF00 */
#define VIN_ESCR_LSC_Field(x)     (((uint32_t)(((uint32_t)(x)) << VIN_ESCR_LSC_Pos)) & VIN_ESCR_LSC_Msk)
                         
#define VIN_ESCR_LEC_Pos          (16U)                                       
#define VIN_ESCR_LEC_Msk          (0xFFU << VIN_ESCR_LEC_Pos)                /**< 0x00FF0000 */
#define VIN_ESCR_LEC_Field(x)     (((uint32_t)(((uint32_t)(x)) << VIN_ESCR_LEC_Pos)) & VIN_ESCR_LEC_Msk)
                        
#define VIN_ESCR_FEC_Pos          (24U)                                       
#define VIN_ESCR_FEC_Msk          (0xFFU << VIN_ESCR_FEC_Pos)                /**< 0xFF000000 */
#define VIN_ESCR_FEC_Field(x)     (((uint32_t)(((uint32_t)(x)) << VIN_ESCR_FEC_Pos)) & VIN_ESCR_FEC_Msk)                         

/********************  Bits definition for VIN_ESUR register  ******************/
#define VIN_ESUR_FSU_Pos          (0U)                                        
#define VIN_ESUR_FSU_Msk          (0xFFU << VIN_ESUR_FSU_Pos)                /**< 0x000000FF */
#define VIN_ESUR_FSU              VIN_ESUR_FSU_Msk                           
#define VIN_ESUR_LSU_Pos          (8U)                                        
#define VIN_ESUR_LSU_Msk          (0xFFU << VIN_ESUR_LSU_Pos)                /**< 0x0000FF00 */
#define VIN_ESUR_LSU              VIN_ESUR_LSU_Msk                           
#define VIN_ESUR_LEU_Pos          (16U)                                       
#define VIN_ESUR_LEU_Msk          (0xFFU << VIN_ESUR_LEU_Pos)                /**< 0x00FF0000 */
#define VIN_ESUR_LEU              VIN_ESUR_LEU_Msk                           
#define VIN_ESUR_FEU_Pos          (24U)                                       
#define VIN_ESUR_FEU_Msk          (0xFFU << VIN_ESUR_FEU_Pos)                /**< 0xFF000000 */
#define VIN_ESUR_FEU              VIN_ESUR_FEU_Msk                           

/********************  Bits definition for VIN_CWSTRT register  ******************/
#define VIN_CWSTRT_HOFFCNT_Pos       (0U)                                        
#define VIN_CWSTRT_HOFFCNT_Msk       (0x3FFFU << VIN_CWSTRT_HOFFCNT_Pos)        /**< 0x00003FFF */
#define VIN_CWSTRT_HOFFCNT_Field(x)  (((uint32_t)(((uint32_t)(x)) << VIN_CWSTRT_HOFFCNT_Pos)) & VIN_CWSTRT_HOFFCNT_Msk) 
                  
#define VIN_CWSTRT_VST_Pos           (16U)                                       
#define VIN_CWSTRT_VST_Msk           (0x1FFFU << VIN_CWSTRT_VST_Pos)            /**< 0x1FFF0000 */
#define VIN_CWSTRT_VST_Field(x)      (((uint32_t)(((uint32_t)(x)) << VIN_CWSTRT_VST_Pos)) & VIN_CWSTRT_VST_Msk)                        

/********************  Bits definition for VIN_CWSIZE register  ******************/
#define VIN_CWSIZE_CAPCNT_Pos        (0U)                                        
#define VIN_CWSIZE_CAPCNT_Msk        (0x3FFFU << VIN_CWSIZE_CAPCNT_Pos)         /**< 0x00003FFF */
#define VIN_CWSIZE_CAPCNT_Field(x)   (((uint32_t)(((uint32_t)(x)) << VIN_CWSIZE_CAPCNT_Pos)) & VIN_CWSIZE_CAPCNT_Msk) 
                     
#define VIN_CWSIZE_VLINE_Pos         (16U)                                       
#define VIN_CWSIZE_VLINE_Msk         (0x3FFFU << VIN_CWSIZE_VLINE_Pos)          /**< 0x3FFF0000 */
#define VIN_CWSIZE_VLINE_Field(x)    (((uint32_t)(((uint32_t)(x)) << VIN_CWSIZE_VLINE_Pos)) & VIN_CWSIZE_VLINE_Msk)                     

/********************  Bits definition for VIN_DR register  *********************/
#define VIN_DR_BYTE0_Pos          (0U)                                        
#define VIN_DR_BYTE0_Msk          (0xFFU << VIN_DR_BYTE0_Pos)                /**< 0x000000FF */
#define VIN_DR_BYTE0              VIN_DR_BYTE0_Msk                           
#define VIN_DR_BYTE1_Pos          (8U)                                        
#define VIN_DR_BYTE1_Msk          (0xFFU << VIN_DR_BYTE1_Pos)                /**< 0x0000FF00 */
#define VIN_DR_BYTE1              VIN_DR_BYTE1_Msk                           
#define VIN_DR_BYTE2_Pos          (16U)                                       
#define VIN_DR_BYTE2_Msk          (0xFFU << VIN_DR_BYTE2_Pos)                /**< 0x00FF0000 */
#define VIN_DR_BYTE2              VIN_DR_BYTE2_Msk                           
#define VIN_DR_BYTE3_Pos          (24U)                                       
#define VIN_DR_BYTE3_Msk          (0xFFU << VIN_DR_BYTE3_Pos)                /**< 0xFF000000 */
#define VIN_DR_BYTE3              VIN_DR_BYTE3_Msk                           

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Enable the VIN.
 * @param  pVIN: pVIN devive Point.
 * @retval None
 */
#define LL_VIN_ENABLE(pVIN)    	(pVIN->CR |= VIN_CR_ENABLE)
    
/**
 * @brief  Disable the DCMI.
 * @param  pVIN: pVIN devive Point.
 * @retval None
 */
#define LL_VIN_DISABLE(pVIN)   	(pVIN->CR &= ~(VIN_CR_ENABLE))

#define VINDEV           		((VIN_Device_T*)0x40025000u)

#ifdef __cplusplus
 }
#endif
#endif /* #ifndef _GM_LL_VIN_H_ */

