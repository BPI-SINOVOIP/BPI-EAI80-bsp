/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_imp.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2018/01/03
 *
 * @brief         Header file of IMP Controller module.
 *
 * @note
 *    2018/01/03, mark.lee, V1.0.0
 *        Initial version.
 *
 */
#ifndef _GM_HAL_IMP_H_
#define _GM_HAL_IMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <imp/gm_ll_imp_dev.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_IMP_LAYER  2U

/** @defgroup IMP_Mode IMP Mode
 * @{
 */
#define IMP_M2M                   0x00000000U            /**< IMP memory to memory transfer mode                              */
#define IMP_M2M_PFC               0x00010000U            /**< IMP memory to memory with pixel format conversion transfer mode */
#define IMP_M2M_BLEND             0x00020000U            /**< IMP memory to memory with blending transfer mode                */
#define IMP_R2M                   0x00030000U            /**< IMP register to memory transfer mode                            */
/**
 * @}
 */

/** @defgroup IMP_Output_Color_Mode IMP Output Color Mode
 * @{
 */
#define IMP_OUTPUT_ARGB8888       0x00000000U              /**< ARGB8888 IMP color mode */
#define IMP_OUTPUT_RGB888         0x00000001U              /**< RGB888 IMP color mode   */
#define IMP_OUTPUT_RGB565         0x00000002U              /**< RGB565 IMP color mode   */
#define IMP_OUTPUT_ARGB1555       0x00000003U              /**< ARGB1555 IMP color mode */
#define IMP_OUTPUT_ARGB4444       0x00000004U              /**< ARGB4444 IMP color mode */
/**
 * @}
 */

/** @defgroup IMP_Input_Color_Mode IMP Input Color Mode
 * @{
 */
#define IMP_INPUT_ARGB8888        0x00000000U              /**< ARGB8888 color mode */
#define IMP_INPUT_RGB888          0x00000001U              /**< RGB888 color mode   */
#define IMP_INPUT_RGB565          0x00000002U              /**< RGB565 color mode   */
#define IMP_INPUT_ARGB1555        0x00000003U              /**< ARGB1555 color mode */
#define IMP_INPUT_ARGB4444        0x00000004U              /**< ARGB4444 color mode */
#define IMP_INPUT_L8              0x00000005U              /**< L8 color mode       */
#define IMP_INPUT_AL44            0x00000006U              /**< AL44 color mode     */
#define IMP_INPUT_AL88            0x00000007U              /**< AL88 color mode     */
#define IMP_INPUT_L4              0x00000008U              /**< L4 color mode       */
#define IMP_INPUT_A8              0x00000009U              /**< A8 color mode       */
#define IMP_INPUT_A4              0x0000000AU              /**< A4 color mode       */
/**
 * @}
 */

/** @defgroup IMP_Alpha_Mode IMP Alpha Mode
 * @{
 */
#define IMP_NO_MODIF_ALPHA        0x00000000U              /**< No modification of the alpha channel value                     */
#define IMP_REPLACE_ALPHA         0x00000001U              /**< Replace original alpha channel value by programmed alpha value */
#define IMP_COMBINE_ALPHA         0x00000002U              /**< Replace original alpha channel value by programmed alpha value
                                                                      with original alpha channel value                                */
/**
 * @}
 */

/** @defgroup IMP_CLUT_CM IMP CLUT Color Mode
 * @{
 */
#define IMP_CCM_ARGB8888          0x00000000U              /**< ARGB8888 IMP CLUT color mode */
#define IMP_CCM_RGB888            0x00000001U              /**< RGB888 IMP CLUT color mode   */
/**
 * @}
 */

/** @defgroup IMP_Interrupts IMP Interrupts
 * @{
 */
#define IMP_IT_CE                 0x00002000            /**< Configuration Error Interrupt    */
#define IMP_IT_CTC                0x00001000           /**< CLUT Transfer Complete Interrupt */
#define IMP_IT_CAE                0x00000800           /**< CLUT Access Error Interrupt      */
#define IMP_IT_TW                 0x00000400            /**< Transfer Watermark Interrupt     */
#define IMP_IT_TC                 0x00000200            /**< Transfer Complete Interrupt      */
#define IMP_IT_TE                 0x00000100            /**< Transfer Error Interrupt         */
/**
 * @}
 */

/** @defgroup IMP_Flags IMP Flags
 * @{
 */
#define IMP_FLAG_CE               0x00000020           /**< Configuration Error Interrupt Flag    */
#define IMP_FLAG_CTC              0x00000010           /**< CLUT Transfer Complete Interrupt Flag */
#define IMP_FLAG_CAE              0x00000008           /**< CLUT Access Error Interrupt Flag      */
#define IMP_FLAG_TW               0x00000004           /**< Transfer Watermark Interrupt Flag     */
#define IMP_FLAG_TC               0x00000002           /**< Transfer Complete Interrupt Flag      */
#define IMP_FLAG_TE               0x00000001           /**< Transfer Error Interrupt Flag         */
/**
 * @}
 */

/**
 * @brief IMP CLUT Structure definition
 */
typedef struct
{
    uint8_t              enable;             /**< Enable Clut mode */
    uint32_t             *pCLUT;             /**< Configures the IMP CLUT memory address.*/
    uint32_t             CLUTColorMode;      /**< Configures the IMP CLUT color mode.
                                                This parameter can be one value of @ref IMP_CLUT_CM. */
    uint32_t             size;               /**< Configures the IMP CLUT size.
                                                This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF.*/
} IMP_CLUTCfg_T;

/**
 * @brief IMP Init structure definition
 */
typedef struct
{
    uint32_t             mode;               /**< Configures the IMP transfer mode.
                                                This parameter can be one value of @ref IMP_Mode. */
    uint32_t             colorMode;          /**< Configures the color format of the output image.
                                                This parameter can be one value of @ref IMP_Output_Color_Mode. */
    uint32_t             outputOffset;       /**< Specifies the Offset value.
                                                This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x3FFF. */
} IMP_Config_T;

/**
 * @brief IMP Layer structure definition
 */
typedef struct
{
    uint32_t             inputOffset;        /**< Configures the IMP foreground or background offset.
                                                This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x3FFF. */
    uint32_t             inputColorMode;     /**< Configures the IMP foreground or background color mode.
                                                This parameter can be one value of @ref IMP_Input_Color_Mode. */
    uint32_t             alphaMode;          /**< Configures the IMP foreground or background alpha mode.
                                                This parameter can be one value of @ref IMP_Alpha_Mode. */
    uint32_t             inputAlpha;         /**< Specifies the IMP foreground or background alpha value and color value in case of A8 or A4 color mode.
                                                This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF except for the color modes detailed below.
                                                @note In case of A8 or A4 color mode (ARGB), this parameter must be a number between
                                                Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF where
                                                - InputAlpha[24:31] is the alpha value ALPHA[0:7]
                                                - InputAlpha[16:23] is the red value RED[0:7]
                                                - InputAlpha[8:15] is the green value GREEN[0:7]
                                                - InputAlpha[0:7] is the blue value BLUE[0:7]. */
} IMP_LayerCfg_T;

/**
 * @brief IMP Scale structure definition
 */
typedef struct
{
    uint8_t              enable;          /**< Enable scale mode */
    uint8_t              svi;             /**< scaler vertical increase for integer part. */
    uint8_t              svf;             /**< scaler vertical increase for fraction part. */
    uint8_t              shi;             /**< scaler horizontal increase for integer part. */
    uint8_t              shf;             /**< scaler horizontal increase for fraction part. */
    uint32_t             scaleWidth;      /**< The width of data to be transferred from source to destination (expressed in number of pixels per line). */
    uint32_t             scaleHeight;     /**< The height of data to be transferred from source to destination (expressed in number of lines). */
    uint32_t             oriWidth;        /**< The width of data to be transferred from source to destination (expressed in number of pixels per line). */
    uint32_t             oriHeight;       /**< The height of data to be transferred from source to destination (expressed in number of lines). */

} IMP_ScaleCfg_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Initialize the IMP according to the specified
 *         parameters in the IMP_Config_T and create the associated handle.
 * @param  pIMP:     IMP peripheral base address.
 * @param  pConfig   Pointer to IMP_Config_T structure.
 * @retval HAL status
 */
RET_CODE HAL_IMP_Init(IMP_Device_T *pIMP, IMP_Config_T *pConfig);

/**
 * @brief  Configure the IMP Layer.
 * @param  pIMP:     IMP peripheral base address.
 * @param  pLayerCfg  pointer to a IMP_LayerCfg_T structure that contains
 *                    the configuration information for the Layer.
 * @param  layerIdx: IMP Layer index.
 *                   This parameter can be one of the following values:
 *                   0(background) / 1(foreground)
 * @retval HAL status
 */
RET_CODE HAL_IMP_ConfigLayer(IMP_Device_T *pIMP, IMP_LayerCfg_T *pLayerCfg, uint32_t layerIdx);

/**
 * @brief  Start the IMP Transfer.
 * @param  pIMP:       IMP peripheral base address.
 * @param  pdata:      Configure the source memory Buffer address if
 *                     Memory-to-Memory or Memory-to-Memory with pixel format
 *                     conversion mode is selected, or configure
 *                     the color value if Register-to-Memory mode is selected.
 * @param  dstAddress: The destination memory Buffer address.
 * @param  scaleCfg:   pointer to a IMP_ScaleCfg_T structure that contains
 *                     the configuration information for the scaler.
 * @note   Only support scale function on IMP_M2M_PFC/IMP_M2M_BLEND mode.
 * @retval HAL status
 */
RET_CODE HAL_IMP_Start(IMP_Device_T *pIMP, uint32_t pdata, uint32_t dstAddress, IMP_ScaleCfg_T *scaleCfg);

/**
 * @brief  Start the multi-source IMP Transfer.
 * @param  pIMP:        IMP peripheral base address.
 * @param  srcAddress1: The source memory Buffer address for the foreground layer.
 * @param  srcAddress2: The source memory Buffer address for the background layer.
 * @param  dstAddress:  The destination memory Buffer address.
 * @param  scaleCfg:    pointer to a IMP_ScaleCfg_T structure that contains
 *                      the configuration information for the scaler.
 * @note   Only support scale function on IMP_M2M_PFC/IMP_M2M_BLEND mode.
 * @retval HAL status
 */
RET_CODE HAL_IMP_BlendingStart(IMP_Device_T *pIMP, uint32_t srcAddress1, uint32_t  srcAddress2, uint32_t dstAddress, IMP_ScaleCfg_T *scaleCfg);

/**
  * @brief  Abort the IMP Transfer.
  * @param  pIMP:        IMP peripheral base address.
  *
  * @retval HAL status
  */
RET_CODE HAL_IMP_Abort(IMP_Device_T *pIMP);

/**
  * @brief  Suspend the IMP Transfer.
  * @param  pIMP:        IMP peripheral base address.
  *
  * @retval HAL status
  */
RET_CODE HAL_IMP_Suspend(IMP_Device_T *pIMP);

/**
  * @brief  Resume the IMP Transfer.
  * @param  pIMP:        IMP peripheral base address.
  *
  * @retval HAL status
  */
RET_CODE HAL_IMP_Resume(IMP_Device_T *pIMP);

/**
  * @brief  Start IMP CLUT Loading.
  * @param  pIMP:     IMP peripheral base address.
  * @param  CLUTCfg:  Pointer to a IMP_CLUTCfg structure that contains
  *                   the configuration information for the color look up table.
  * @param  layerIdx: IMP Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
RET_CODE HAL_IMP_CLUTLoad(IMP_Device_T *pIMP, IMP_CLUTCfg_T *CLUTCfg, uint32_t layerIdx);

/**
  * @brief  Configure the line watermark.
  * @param  pIMP:     IMP peripheral base address.
  * @param  line:   Line Watermark configuration (maximum 16-bit long value expected).
  * @note   The transfer watermark interrupt is disabled once it has occurred.
  * @retval HAL status
  */
RET_CODE HAL_IMP_ProgramLineEvent(IMP_Device_T *pIMP, uint32_t line);

/**
 * @brief  Start the Harris corner detection.
 * @param  pIMP:        IMP peripheral base address.
 * @param  pdata:    Configure the source memory Buffer address if
 *                  Memory-to-Memory or Memory-to-Memory with pixel format
 *                  conversion mode is selected, or configure
 *                  the color value if Register-to-Memory mode is selected.
 * @param  dstAddress:  The destination memory Buffer address.
 * @param  scaleCfg:    pointer to a IMP_ScaleCfg_T structure that contains
 *                      the configuration information for the scaler.
 * @param  harEn:  Harris corner detection enable.
 * @param  thrvalue:  Gray threshold.
 * @note   Only support scale function on IMP_M2M_PFC/IMP_M2M_BLEND mode.
 * @retval HAL status
 */
RET_CODE HAL_IMP_HarrisStart(IMP_Device_T *pIMP, uint32_t pdata, uint32_t dstAddress, IMP_ScaleCfg_T *scaleCfg, int harEn, unsigned int thrvalue);

/**
  * @brief  Get the IMP pending flags.
  * @param  __HANDLE__: IMP handle
  * @param  __FLAG__: flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg IMP_FLAG_CE:  Configuration error flag
  *            @arg IMP_FLAG_CTC: CLUT transfer complete flag
  *            @arg IMP_FLAG_CAE: CLUT access error flag
  *            @arg IMP_FLAG_TW:  Transfer Watermark flag
  *            @arg IMP_FLAG_TC:  Transfer complete flag
  *            @arg IMP_FLAG_TE:  Transfer error flag
  * @retval The state of FLAG.
  */
#define __HAL_IMP_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->ISR & (__FLAG__))

/**
  * @brief  Clear the IMP pending flags.
  * @param  __HANDLE__: IMP handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg IMP_FLAG_CE:  Configuration error flag
  *            @arg IMP_FLAG_CTC: CLUT transfer complete flag
  *            @arg IMP_FLAG_CAE: CLUT access error flag
  *            @arg IMP_FLAG_TW:  Transfer Watermark flag
  *            @arg IMP_FLAG_TC:  Transfer complete flag
  *            @arg IMP_FLAG_TE:  Transfer error flag
  * @retval None
  */
#define __HAL_IMP_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->ISR = (__FLAG__))

/**
  * @brief  Enable the specified IMP interrupts.
  * @param  __HANDLE__: IMP handle
  * @param __INTERRUPT__: specifies the IMP interrupt sources to be enabled.
  *          This parameter can be any combination of the following values:
  *            @arg IMP_IT_CE:  Configuration error interrupt mask
  *            @arg IMP_IT_CTC: CLUT transfer complete interrupt mask
  *            @arg IMP_IT_CAE: CLUT access error interrupt mask
  *            @arg IMP_IT_TW:  Transfer Watermark interrupt mask
  *            @arg IMP_IT_TC:  Transfer complete interrupt mask
  *            @arg IMP_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_IMP_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the specified IMP interrupts.
  * @param  __HANDLE__: IMP handle
  * @param __INTERRUPT__: specifies the IMP interrupt sources to be disabled.
  *          This parameter can be any combination of the following values:
  *            @arg IMP_IT_CE:  Configuration error interrupt mask
  *            @arg IMP_IT_CTC: CLUT transfer complete interrupt mask
  *            @arg IMP_IT_CAE: CLUT access error interrupt mask
  *            @arg IMP_IT_TW:  Transfer Watermark interrupt mask
  *            @arg IMP_IT_TC:  Transfer complete interrupt mask
  *            @arg IMP_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __HAL_IMP_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->CR &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified IMP interrupt source is enabled or not.
  * @param  __HANDLE__: IMP handle
  * @param  __INTERRUPT__: specifies the IMP interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg IMP_IT_CE:  Configuration error interrupt mask
  *            @arg IMP_IT_CTC: CLUT transfer complete interrupt mask
  *            @arg IMP_IT_CAE: CLUT access error interrupt mask
  *            @arg IMP_IT_TW:  Transfer Watermark interrupt mask
  *            @arg IMP_IT_TC:  Transfer complete interrupt mask
  *            @arg IMP_IT_TE:  Transfer error interrupt mask
  * @retval The state of INTERRUPT source.
  */
#define __HAL_IMP_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->CR & (__INTERRUPT__))

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_IMP_H_ */

