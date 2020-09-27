/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_hal_vout.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/06/21 
 *    
 * @brief         Header file of LCD-TFT Display Controller module.
 * 
 * @note
 *    2017/11/28, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GM_HAL_VOUT_H_
#define _GM_HAL_VOUT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <vout/gm_ll_vout_dev.h>
#include <clock/gm_ll_clock.h>


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** @defgroup VOUT_Layer VOUT Layer
 * @{
 */
#define VOUT_LAYER_1                0x00000000U     /**< VOUT Layer 1 */
#define VOUT_LAYER_2                0x00000001U     /**< VOUT Layer 2 */
/**
 * @}
 */
    
/** @defgroup VOUT_HS_POLARITY VOUT HS POLARITY
 * @{
 */
#define VOUT_HSPOLARITY_AL          0x00000000U      /**< Horizontal Synchronization is active low. */
#define VOUT_HSPOLARITY_AH          0x80000000U      /**< Horizontal Synchronization is active high. */
/**
 * @}
 */
    
/** @defgroup VOUT_VS_POLARITY VOUT VS POLARITY
 * @{
 */
#define VOUT_VSPOLARITY_AL          0x00000000U      /**< Vertical Synchronization is active low. */
#define VOUT_VSPOLARITY_AH          0x40000000U      /**< Vertical Synchronization is active high. */
/**
 * @}
 */
      
/** @defgroup VOUT_DE_POLARITY VOUT DE POLARITY
 * @{
 */
#define VOUT_DEPOLARITY_AL          0x00000000U      /**< Data Enable, is active low. */
#define VOUT_DEPOLARITY_AH          0x20000000U      /**< Data Enable, is active high. */
/**
 * @}
 */
    
/** @defgroup VOUT_PC_POLARITY VOUT PC POLARITY
 * @{
 */
#define VOUT_PCPOLARITY_IPC         0x00000000U      /**< input pixel clock. */
#define VOUT_PCPOLARITY_IIPC        0x10000000U      /**< inverted input pixel clock. */
/**
 * @}
 */
      
/** @defgroup VOUT_Pixelformat VOUT Pixel format
 * @{
 */
#define VOUT_PIXEL_FORMAT_ARGB8888        0x00000000U   /**< ARGB8888 VOUT pixel format */
#define VOUT_PIXEL_FORMAT_RGB888          0x00000001U   /**< RGB888 VOUT pixel format   */
#define VOUT_PIXEL_FORMAT_RGB565          0x00000002U   /**< RGB565 VOUT pixel format   */
#define VOUT_PIXEL_FORMAT_ARGB1555        0x00000003U   /**< ARGB1555 VOUT pixel format */
#define VOUT_PIXEL_FORMAT_ARGB4444        0x00000004U   /**< ARGB4444 VOUT pixel format */
#define VOUT_PIXEL_FORMAT_L8              0x00000005U   /**< L8 VOUT pixel format       */
#define VOUT_PIXEL_FORMAT_AL44            0x00000006U   /**< AL44 VOUT pixel format     */
#define VOUT_PIXEL_FORMAT_AL88            0x00000007U   /**< AL88 VOUT pixel format     */
/**
 * @}
 */

/** @defgroup VOUT_Interrupts VOUT Interrupts
 * @{
 */
#define VOUT_IT_LI                        0x00000001U           /**< VOUT Line Interrupt            */
#define VOUT_IT_FU                        0x00000002U           /**< VOUT FIFO Underrun Interrupt   */
#define VOUT_IT_TE                        0x00000004U           /**< VOUT Transfer Error Interrupt  */
/**
 * @}
 */
      
/** @defgroup VOUT_Flags VOUT Flags
 * @{
 */
#define VOUT_FLAG_LI                      0x00000001U          /**< VOUT Line Interrupt Flag            */
#define VOUT_FLAG_FU                      0x00000002U          /**< VOUT FIFO Underrun interrupt Flag   */
#define VOUT_FLAG_TE                      0x00000004U          /**< VOUT Transfer Error interrupt Flag  */
/**
 * @}
 */


/** 
 * @brief  VOUT color structure definition
 */
typedef struct
{
    uint8_t blue;                    /**< Configures the blue value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    
    uint8_t green;                   /**< Configures the green value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    
    uint8_t red;                     /**< Configures the red value. 
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    
    uint8_t reserved;                /**< Reserved 0xFF */
} VOUT_Color_T;

/** 
 * @brief  VOUT Init structure definition
 */
typedef struct
{
    uint32_t            HSPolarity;                /**< configures the horizontal synchronization polarity.
                                                      This parameter can be one value of @ref VOUT_HS_POLARITY */
    
    uint32_t            VSPolarity;                /**< configures the vertical synchronization polarity.
                                                      This parameter can be one value of @ref VOUT_VS_POLARITY */
    
    uint32_t            DEPolarity;                /**< configures the data enable polarity. 
                                                      This parameter can be one of value of @ref VOUT_DE_POLARITY */
    
    uint32_t            PCPolarity;                /**< configures the pixel clock polarity. 
                                                      This parameter can be one of value of @ref VOUT_PC_POLARITY */
    
    uint32_t            horizontalSync;            /**< configures the number of Horizontal synchronization width.
                                                      This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
    
    uint32_t            verticalSync;              /**< configures the number of Vertical synchronization height. 
                                                      This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */
                                                        
    uint32_t            accumulatedHBP;            /**< configures the accumulated horizontal back porch width.
                                                      This parameter must be a number between Min_Data = VOUT_HorizontalSync and Max_Data = 0xFFF. */
    
    uint32_t            accumulatedVBP;            /**< configures the accumulated vertical back porch height.
                                                      This parameter must be a number between Min_Data = VOUT_VerticalSync and Max_Data = 0x7FF. */
    
    uint32_t            accumulatedActiveW;        /**< configures the accumulated active width. 
                                                      This parameter must be a number between Min_Data = VOUT_AccumulatedHBP and Max_Data = 0xFFF. */
    
    uint32_t            accumulatedActiveH;        /**< configures the accumulated active height.
                                                      This parameter must be a number between Min_Data = VOUT_AccumulatedVBP and Max_Data = 0x7FF. */
    
    uint32_t            totalWidth;                /**< configures the total width.
                                                      This parameter must be a number between Min_Data = VOUT_AccumulatedActiveW and Max_Data = 0xFFF. */
    
    uint32_t            totalHeigh;                /**< configures the total height.
                                                      This parameter must be a number between Min_Data = VOUT_AccumulatedActiveH and Max_Data = 0x7FF. */
    
    VOUT_Color_T        backColor;                 /**< Configures the background color. */
} VOUT_Config_T;

/** 
  * @brief  VOUT Layer structure definition
  */
typedef struct
{
    uint32_t windowX0;                   /**< Configures the Window Horizontal Start Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
    
    uint32_t windowX1;                   /**< Configures the Window Horizontal Stop Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */
    
    uint32_t windowY0;                   /**< Configures the Window vertical Start Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */
    
    uint32_t windowY1;                   /**< Configures the Window vertical Stop Position.
                                            This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x7FF. */
    
    uint32_t pixelFormat;                /**< Specifies the pixel format. 
                                            This parameter can be one of value of @ref VOUT_Pixelformat */
    
    uint32_t alpha;                      /**< Specifies the constant alpha used for blending.
                                            This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    
    uint32_t alpha0;                     /**< Configures the default alpha value.
                                            This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
    
    uint32_t FBStartAdress;              /**< Configures the color frame buffer address */
    
    uint32_t imageWidth;                 /**< Configures the color frame buffer line length. 
                                            This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x1FFF. */
    
    uint32_t imageHeight;                /**< Specifies the number of line in frame buffer. 
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */
    
    VOUT_Color_T   backColor;            /**< Configures the layer background color. */
} VOUT_LayerConfig_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Initialize the VOUT according to the specified parameters in the VOUT_Config_T.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  pConfig   Pointer to VOUT_Config_T structure.
 * @retval HAL status
 */
RET_CODE HAL_VOUT_Init(VOUT_Device_T *pVOUT, VOUT_Config_T *pConfig);

/**
 * @brief  Configure the VOUT Layer.
 * @param  pVOUT      VOUT peripheral base address.
 * @param  pLayerCfg  pointer to a VOUT_LayerConfig_T structure that contains
 *                    the configuration information for the Layer.
 * @param  layerIdx   Layer index.
 *                    This parameter can be one of the following values:
 *                    VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval HAL status
 */
RET_CODE HAL_VOUT_ConfigLayer(VOUT_Device_T *pVOUT, VOUT_LayerConfig_T *pLayerCfg, uint32_t layerIdx);

/**
 * @brief  Configure the color keying.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  RGBValue  the color key value
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval HAL status
 */
RET_CODE HAL_VOUT_ConfigColorKeying(VOUT_Device_T *pVOUT, uint32_t RGBValue, uint32_t layerIdx);

/**
 * @brief  Load the color lookup table.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  pCLUT     pointer to the color lookup table address.
 * @param  CLUTSize  the color lookup table size.  
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval HAL status
 */
RET_CODE HAL_VOUT_ConfigCLUT(VOUT_Device_T *pVOUT, uint32_t *pCLUT, uint32_t CLUTSize, uint32_t layerIdx);

/**
 * @brief  Enable the color keying.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_EnableColorKeying(VOUT_Device_T *pVOUT, uint32_t layerIdx);

/**
 * @brief  Disable the color keying.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_DisableColorKeying(VOUT_Device_T *pVOUT, uint32_t layerIdx);

/**
 * @brief  Enable the color lookup table.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_EnableCLUT(VOUT_Device_T *pVOUT, uint32_t layerIdx);

/**
 * @brief  Disable the color lookup table.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)  
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_DisableCLUT(VOUT_Device_T *pVOUT, uint32_t layerIdx);

/**
 * @brief  Enable pre multiply.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_EnablePreMultiply(VOUT_Device_T *pVOUT, uint32_t layerIdx);

/**
 * @brief  Disable pre multiply.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)  
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_DisablePreMultiply(VOUT_Device_T *pVOUT, uint32_t layerIdx);

/**
 * @brief  Enable Dither.
 * @param  pVOUT     VOUT peripheral base address.
 *
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_EnableDither(VOUT_Device_T *pVOUT);

/**
 * @brief  Disable Dither.
 * @param  pVOUT     VOUT peripheral base address.
 *
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_DisableDither(VOUT_Device_T *pVOUT);

/**
 * @brief  Set the LTDC window size.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  XSize     Pixel per line
 * @param  YSize     Line number
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_SetWindowSize(VOUT_Device_T *pVOUT, uint32_t XSize, uint32_t YSize, uint32_t layerIdx);

/**
 * @brief  Set the LTDC window position.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  X0        window X offset
 * @param  Y0        window Y offset
 * @param  layerIdx  Layer index.
 *                         This parameter can be one of the following values:
 *                         VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_SetWindowPosition(VOUT_Device_T *pVOUT, uint32_t X0, uint32_t Y0, uint32_t layerIdx);

/**
 * @brief  Reconfigure the pixel format.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  pixelFormat  new pixel format value.
 * @param  layerIdx     Layer index.
 *                      This parameter can be one of the following values:
 *                      VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1).
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_SetPixelFormat(VOUT_Device_T *pVOUT, uint32_t pixelFormat, uint32_t layerIdx);

/**
 * @brief  Reconfigure the layer alpha value.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  alpha     new alpha value.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_SetAlpha(VOUT_Device_T *pVOUT, uint32_t alpha, uint32_t layerIdx);

/**
 * @brief  Reconfigure the frame buffer Address.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  address   new address value.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1).
 * @retval  HAL status
 */
RET_CODE HAL_VOUT_SetAddress(VOUT_Device_T *pVOUT, uint32_t address, uint32_t layerIdx);

/**
 * @brief  Function used to reconfigure the pitch for specific cases where the attached LayerIdx buffer have a width that is
 *         larger than the one intended to be displayed on screen. Example of a buffer 800x480 attached to layer for which we 
 *         want to read and display on screen only a portion 320x240 taken in the center of the buffer. The pitch in pixels 
 *         will be in that case 800 pixels and not 320 pixels as initially configured by previous call to HAL_LTDC_ConfigLayer().
 *
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  New line pitch in pixels to configure for LTDC layer 'LayerIdx'.
 * @param  layerIdx           layer index concerned by the modification of line pitch.
 * @retval HAL status
 */
RET_CODE HAL_VOUT_SetPitch(VOUT_Device_T *pVOUT, uint32_t linePitchInPixels, uint32_t layerIdx);

/**
 * @brief Enables the selected VOUT interrupts.
 *
 * @param  pVOUT     VOUT peripheral base address.
 * @param mask The interrupts to enable. This parameter can be a value of @ref VOUT_Interrupts.
 * @retval None
 */
void HAL_VOUT_EnableInterrupts(VOUT_Device_T *pVOUT, uint32_t mask);

/**
 * @brief Disables the selected VOUT interrupts.
 *
 * @param  pVOUT     VOUT peripheral base address.
 * @param mask The interrupts to enable. This parameter can be a value of @ref VOUT_Interrupts.
 * @retval None
 */
void HAL_VOUT_DisableInterrupts(VOUT_Device_T *pVOUT, uint32_t mask);

/**
 * @brief  Enable/Disable the VOUT.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  onoff  
 *
 * @retval None.
 */
void HAL_VOUT_OnOff(VOUT_Device_T *pVOUT, uint8_t onoff);

/**
 * @brief  Enable/Disable the VOUT Layer.
 * @param  pVOUT     VOUT peripheral base address.
 * @param  layerIdx  Layer index.
 *                   This parameter can be one of the following values:
 *                   VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1).
 * @retval HAL status.
 */
RET_CODE HAL_VOUT_layerOnOff(VOUT_Device_T *pVOUT, uint32_t layerIdx, uint8_t onoff);


/**
 * @brief  Get the VOUT pending flags.
 * @param  __HANDLE__  VOUT handle
 * @param  __FLAG__    Get the specified flag.
 *          This parameter can be any combination of the following values:
 *            @arg VOUT_FLAG_LI: Line Interrupt flag 
 *            @arg VOUT_FLAG_FU: FIFO Underrun Interrupt flag
 *            @arg VOUT_FLAG_TE: Transfer Error interrupt flag
 * @retval The state of FLAG (SET or RESET).
 */
#define __HAL_VOUT_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->ISR & (__FLAG__))

/**
 * @brief  Clears the VOUT pending flags.
 * @param  __HANDLE__  VOUT handle
 * @param  __FLAG__    Specify the flag to clear.
 *          This parameter can be any combination of the following values:
 *            @arg VOUT_FLAG_LI: Line Interrupt flag 
 *            @arg VOUT_FLAG_FU: FIFO Underrun Interrupt flag
 *            @arg VOUT_FLAG_TE: Transfer Error interrupt flag 
 * @retval None
 */
#define __HAL_VOUT_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->ISR = (__FLAG__))

/**
 * @brief  Check whether the specified VOUT interrupt has occurred or not.
 * @param  __HANDLE__  VOUT handle
 * @param __INTERRUPT__ Specify the VOUT interrupt source to check.
 *          This parameter can be one of the following values:
 *            @arg VOUT_IT_LI: Line Interrupt flag 
 *            @arg VOUT_IT_FU: FIFO Underrun Interrupt flag
 *            @arg VOUT_IT_TE: Transfer Error interrupt flag
 * @retval The state of INTERRUPT (SET or RESET).
 */
#define __HAL_VOUT_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->IER & (__INTERRUPT__))

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_VOUT_H_ */

