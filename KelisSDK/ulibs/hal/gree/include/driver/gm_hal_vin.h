/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_hal_vin.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/06/21 
 *    
 * @brief         Header file of Video Input Capture HAL module.
 * 
 * @note
 *    2017/11/28, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GM_HAL_VIN_H_
#define _GM_HAL_VIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <vin/gm_ll_vin_dev.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** @defgroup VIN_PATH VIN Path
 * @{
 */
#define VIN_PATH_1                0x00000000U     /**< VIN Path 1 */
#define VIN_PATH_2                0x00000001U     /**< VIN Path 2 */
/**
 * @}
 */
     

/** @defgroup VIN_Mode Video Input Caputre Mode
 * @{
 */ 
#define VIN_MODE_CONTINUOUS         0x00000000U      /**< The received data are transferred continuously
                                                          into the destination memory */
#define VIN_MODE_SNAPSHOT           0x00000002U      /**< Once activated, the interface waits for the start of
                                                          frame and then transfers a single frame */
/**
 * @}
 */
    
/** @defgroup VIN_Synchronization_Mode VIN Synchronization Mode
 * @{
 */
#define VIN_SYNCHRO_HARDWARE        0x00000000U      /**< Hardware synchronization data capture (frame/line start/stop)
                                                          is synchronized with the HSYNC/VSYNC signals */
#define VIN_SYNCHRO_EMBEDDED        0x00000010U      /**< Embedded synchronization data capture is synchronized with
                                                          synchronization codes embedded in the data flow */    
/**
 * @}
 */
    
/** @defgroup VIN_PIXCK_Polarity VIN PIXCK Polarity
 * @{
 */
#define VIN_PCKPOLARITY_FALLING     0x00000000U     /**< Pixel clock active on Falling edge */
#define VIN_PCKPOLARITY_RISING      0x00000020U     /**< Pixel clock active on Rising edge  */
    
/**
 * @}
 */
    
/** @defgroup VIN_VSYNC_Polarity VIN VSYNC Polarity
 * @{
 */
#define VIN_VSPOLARITY_LOW          0x00000000U     /**< Vertical synchronization active Low  */
#define VIN_VSPOLARITY_HIGH         0x00000080U     /**< Vertical synchronization active High */
    
/**
 * @}
 */
    
/** @defgroup VIN_HSYNC_Polarity VIN HSYNC Polarity
 * @{
 */ 
#define VIN_HSPOLARITY_LOW          0x00000000U     /**< Horizontal synchronization active Low  */
#define VIN_HSPOLARITY_HIGH         0x00000040U     /**< Horizontal synchronization active High */
    
/**
 * @}
 */
    
/** @defgroup VIN_MODE_JPEG VIN MODE JPEG
 * @{
 */
#define VIN_JPEG_DISABLE            0x00000000U     /**< Mode JPEG Disabled  */
#define VIN_JPEG_ENABLE             0x00000008U     /**< Mode JPEG Enabled   */
    
/**
 * @}
 */
    
/** @defgroup VIN_Capture_Rate VIN Capture Rate
 * @{
 */
#define VIN_CR_ALL_FRAME            0x00000000U    /**< All frames are captured        */
#define VIN_CR_ALTERNATE_2_FRAME    0x00000100U    /**< Every alternate frame captured */
#define VIN_CR_ALTERNATE_4_FRAME    0x00000200U    /**< One frame in 4 frames captured */
    
/**
 * @}
 */
    
/** @defgroup VIN_Extended_Data_Mode VIN Extended Data Mode
 * @{
 */
#define VIN_EXTEND_DATA_8B     0x00000000U         /**< Interface captures 8-bit data on every pixel clock  */
#define VIN_EXTEND_DATA_10B    0x00000400U         /**< Interface captures 10-bit data on every pixel clock */
#define VIN_EXTEND_DATA_12B    0x00000800U         /**< Interface captures 12-bit data on every pixel clock */
#define VIN_EXTEND_DATA_14B    0x00000C00U         /**< Interface captures 14-bit data on every pixel clock */
    
/**
 * @}
 */
        
/** @defgroup VIN_interrupt_sources  VIN interrupt sources
 * @{
 */
#define VIN_IT_FRAME    0x00000001U    /**< Capture complete interrupt      */
#define VIN_IT_OVR      0x00000002U    /**< Overrun interrupt               */
#define VIN_IT_ERR      0x00000004U    /**< Synchronization error interrupt */
#define VIN_IT_VSYNC    0x00000008U    /**< VSYNC interrupt                 */
#define VIN_IT_LINE     0x00000010U    /**< Line interrupt                  */
/**
 * @}
 */

/** @defgroup VIN_interrupt_flag  VIN interrupt flag
 * @{
 */
#define VIN_FLAG_FRAMERI    0x00000001U   /**< Frame capture complete interrupt flag */
#define VIN_FLAG_OVRRI      0x00000002U   /**< Overrun interrupt flag                */
#define VIN_FLAG_ERRRI      0x00000004U   /**< Synchronization error interrupt flag  */
#define VIN_FLAG_VSYNCRI    0x00000008U   /**< VSYNC interrupt flag                  */
#define VIN_FLAG_LINERI     0x00000010U   /**< Line interrupt flag                   */

/**
 * @}
 */

/** @defgroup VIN_fifo_flag  VIN fifo flag
 * @{
 */
#define VIN_FLAG_FIFO    	0x00000004U   /**< Fifo empty flag */

/**
 * @}
 */

/** 
 * @brief   VIN Embedded Synchronisation CODE Init structure definition
 */ 
typedef struct
{
  uint8_t frameStartCode; /**< Specifies the code of the frame start delimiter. */
  uint8_t lineStartCode;  /**< Specifies the code of the line start delimiter.  */
  uint8_t lineEndCode;    /**< Specifies the code of the line end delimiter.    */
  uint8_t frameEndCode;   /**< Specifies the code of the frame end delimiter.   */
}VIN_EmbededSyncCodes_T;

/** 
 * @brief   VIN configuration structure definition
 */  
typedef struct
{
    uint32_t  synchroMode;                /**< Specifies the Synchronization Mode: Hardware or Embedded.
                                             This parameter can be a value of @ref VIN_Synchronization_Mode   */
    
    uint32_t  PCKPolarity;                /**< Specifies the Pixel clock polarity: Falling or Rising.
                                             This parameter can be a value of @ref VIN_PIXCK_Polarity         */
    
    uint32_t  VSPolarity;                 /**< Specifies the Vertical synchronization polarity: High or Low.
                                             This parameter can be a value of @ref VIN_VSYNC_Polarity         */
    
    uint32_t  HSPolarity;                 /**< Specifies the Horizontal synchronization polarity: High or Low.
                                             This parameter can be a value of @ref VIN_HSYNC_Polarity         */
    
    uint32_t  captureRate;                /**< Specifies the frequency of frame capture: All, 1/2 or 1/4.
                                             This parameter can be a value of @ref VIN_Capture_Rate           */
    
    uint32_t  extendedDataMode;           /**< Specifies the data width: 8-bit, 10-bit, 12-bit or 14-bit.
                                             This parameter can be a value of @ref VIN_Extended_Data_Mode     */
    
    VIN_EmbededSyncCodes_T syncroCode;    /**< Specifies the code of the frame start delimiter.               */
    
    uint32_t JPEGMode;                    /**< Enable or Disable the JPEG mode
                                             This parameter can be a value of @ref VIN_MODE_JPEG              */
}VIN_Config_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Initializes the Video Input Caputre Module according to the specified
 *         parameters in the VIN_Config_T.
 *
 * @param  pVIN      VIN peripheral base address.
 * @param  pConfig   Pointer to VIN_Config_T structure.
 * @retval HAL status
 */
RET_CODE HAL_VIN_Init(VIN_Device_T *pVIN, VIN_Config_T *pConfig);

/**
 * @brief  Disable DCMI capture
 * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
 *                the configuration information for DCMI.
 * @retval HAL status
 */
RET_CODE HAL_VIN_Stop(VIN_Device_T *pVIN);

/**
 * @brief  Enables DCMI capture
 *
 * @param  pVIN       VIN peripheral base address.
 * @param  mode:      Video Input capture mode snapshot or continuous grab.
 * @param  pData:     The destination memory Buffer address (LCD Frame buffer).
 * @param  length:    The length of capture to be transferred.
 * @retval HAL status
 */
RET_CODE HAL_VIN_Start(VIN_Device_T *pVIN, uint32_t mode, uint32_t pData, uint32_t length);

/**
  * @brief  Reconfigure destination memory Buffer address and length.
  *
  * @param  pVIN      VIN peripheral base address.
  * @param  address   The destination memory Buffer address (LCD Frame buffer).
  * @param  length    The length of capture to be transferred.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
RET_CODE HAL_VIN_SetAddress(VIN_Device_T *pVIN, uint32_t address, uint32_t length);

/**
 * @brief  Configure the VIN CROP coordinate.
 *
 * @param  pVIN   VIN peripheral base address.
 * @param  X0:    window X offset
 * @param  Y0:    window Y offset
 * @param  XSize: Pixel per line
 * @param  YSize: Line number
 * @retval HAL status
 */
RET_CODE HAL_VIN_ConfigCrop(VIN_Device_T *pVIN, uint32_t X0, uint32_t Y0, uint32_t XSize, uint32_t YSize);

/**
 * @brief  Disable the Crop feature.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_DisableCrop(VIN_Device_T *pVIN);

/**
 * @brief  Enable the Crop feature.
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_EnableCrop(VIN_Device_T *pVIN);

/**
 * @brief  Disable VIN capture feature.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_Disable(VIN_Device_T *pVIN);

/**
 * @brief  Enable VIN capture feature.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_Enable(VIN_Device_T *pVIN);

/**
 * @brief  Disable Memory Write feature.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_DisableMemoryWrite(VIN_Device_T *pVIN);

/**
 * @brief  Enable Memory Write feature.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_EnableMemoryWrite(VIN_Device_T *pVIN);

/**
 * @brief  Disable the Color Space Convert feature(YUV422 to RGB888).
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_DisableColorSpaceConv(VIN_Device_T *pVIN);

/**
 * @brief  Enable the Color Space Convert feature(YUV422 to RGB888).
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_EnableColorSpaceConv(VIN_Device_T *pVIN);

/**
 * @brief  Disable JPEG mode.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_DisableJPEGMode(VIN_Device_T *pVIN);

/**
 * @brief  Enable JPEG mode.
 * @param  pVIN      VIN peripheral base address.
 * @retval HAL status
 */
RET_CODE HAL_VIN_EnableJPEGMode(VIN_Device_T *pVIN);

/**
 * @brief  Get the length of jpeg image.
 * @param  pVIN      VIN peripheral base address.
 * @retval length
 */
uint32_t HAL_VIN_GetJPEGLength(VIN_Device_T *pVIN);

/**
 * @brief  Select VIN path.
 * @param  pVIN    VIN peripheral base address.
 * @param  index   path index.
 *                 This parameter can be one of the following values:
 *                 VIN_PATH_1 (0) or VIN_PATH_2 (1)
 * @retval HAL status
 */

RET_CODE HAL_VIN_SwitchPath(VIN_Device_T *pVIN, uint32_t index);

/**
 * @brief Enables the selected VIN interrupts.
 *
 * @param  pVIN      VIN peripheral base address.
 * @param mask The interrupts to enable. This parameter can be a value of @ref VIN_interrupt_sources.
 * @retval None
 */
void HAL_VIN_EnableInterrupts(VIN_Device_T *pVIN, uint32_t mask);

/**
 * @brief Disables the selected VIN interrupts.
 *
 * @param  pVIN      VIN peripheral base address.
 * @param mask The interrupts to enable. This parameter can be a value of @ref VIN_interrupt_sources.
 * @retval None
 */
void HAL_VIN_DisableInterrupts(VIN_Device_T *pVIN, uint32_t mask);

/**
 * @brief Backup the vin register data.
 *
 * @param  pVIN      VIN peripheral base address.
 * @retval None
 */
void HAL_VIN_BackupData(VIN_Device_T *pVIN);

/**
 * @brief  Check whether the specified VIN interrupt has occurred or not.
 * @param  pVIN      VIN peripheral base address.
 * @param  __INTERRUPT__: specifies the DCMI interrupt source to check.
 *         This parameter can be one of the following values:
 *            @arg VIN_IT_FRAME: Frame capture complete interrupt mask
 *            @arg VIN_IT_OVR: Overrun interrupt mask
 *            @arg VIN_IT_ERR: Synchronization error interrupt mask
 *            @arg VIN_IT_VSYNC: VSYNC interrupt mask
 *            @arg VIN_IT_LINE: Line interrupt mask
 * @retval The state of INTERRUPT.
 */
#define __HAL_VIN_GET_IT_SOURCE(pVIN, __INTERRUPT__) (pVIN->IER & (__INTERRUPT__))

/**
 * @brief  Get the VIN pending flag.
 * @param  pVIN      VIN peripheral base address.
 * @param  __FLAG__: Get the specified flag.
 *         This parameter can be one of the following values (no combination allowed)
 *            @arg VIN_FLAG_FRAMERI: Frame capture complete flag mask
 *            @arg VIN_FLAG_OVRRI: Overrun flag mask
 *            @arg VIN_FLAG_ERRRI: Synchronization error flag mask
 *            @arg VIN_FLAG_VSYNCRI: VSYNC flag mask
 *            @arg VIN_FLAG_LINERI: Line flag mask
 * @retval The state of FLAG.
 */
 #define __HAL_VIN_GET_FLAG(pVIN, __FLAG__)   (pVIN->RISR & (__FLAG__))
 
/**
  * @brief  Clear the VIN pending flags.
  * @param  pVIN      VIN peripheral base address.
  * @param  __FLAG__: specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg VIN_FLAG_FRAMERI: Frame capture complete flag mask
  *            @arg VIN_FLAG_OVRRI: Overrun flag mask
  *            @arg VIN_FLAG_ERRRI: Synchronization error flag mask
  *            @arg VIN_FLAG_VSYNCRI: VSYNC flag mask
  *            @arg VIN_FLAG_LINERI: Line flag mask
  * @retval None
  */
#define __HAL_VIN_CLEAR_FLAG(pVIN, __FLAG__) (pVIN->RISR = (__FLAG__))

/**
  * @brief  Get the VIN fifo flags.
  * @param  pVIN      VIN peripheral base address.
  * @param  __FLAG__: specifies the flag to clear.
  *         This parameter can be any combination of the following values:
  *            @arg VIN_FLAG_FIFO: Fifo empty flag mask
  * @retval None
  */
#define __HAL_VIN_FIFO_FLAG(pVIN, __FLAG__) (pVIN->SR &= (__FLAG__))

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_VIN_H_ */

