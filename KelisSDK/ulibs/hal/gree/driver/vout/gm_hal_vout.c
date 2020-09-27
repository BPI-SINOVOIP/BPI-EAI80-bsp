/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_hal_vout.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/11/28 
 *    
 * @brief        VOUT HAL module driver. 
 * 
 * @note
 *    2017/11/28, mark.lee, V1.0.0
 *        Initial version. 
 */
 
#include <gm_soc.h>
#include <gm_hal_vout.h>
#include "gm_ll_vout.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_LAYER  2U

typedef struct
{
    VOUT_Device_T *base;                       /**< VOUT Register base address   */
    VOUT_LayerConfig_T layerCfg[MAX_LAYER];    /**< VOUT Layers parameters       */
} VOUT_Private_Handle_T;

/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Global variables
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/
static VOUT_Private_Handle_T s_VOUTPrivateHandle;

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/

/*******************************************************************************
* Codes
*******************************************************************************/
/**
* @brief  Configure the VOUT peripheral 
* @param  pVOUT     VOUT peripheral base address.
* @param  pLayerCfg Pointer VOUT Layer Configuration structure
* @param  layerIdx  Layer index.
*                   This parameter can be one of the following values: VOUT_LAYER_1 (0) or VOUT_LAYER_2 (1)
* @retval None
*/
static void VOUT_SetConfig(VOUT_Device_T *pVOUT, VOUT_LayerConfig_T *pLayerCfg, uint32_t layerIdx)
{
    uint32_t tmp = 0U;
    uint32_t tmp1 = 0U;
    uint32_t tmp2 = 0U;
    
    /* Configure the horizontal start and stop position */
    tmp = ((pLayerCfg->windowX1 + ((pVOUT->BPCR & VOUT_BPCR_AHBP) >> 16U)) << 16U);
    VOUT_LAYER(pVOUT, layerIdx)->WHPCR &= ~(VOUT_LxWHPCR_WHSTPOS | VOUT_LxWHPCR_WHSPPOS);
    VOUT_LAYER(pVOUT, layerIdx)->WHPCR = ((pLayerCfg->windowX0 + ((pVOUT->BPCR & VOUT_BPCR_AHBP) >> 16U) + 1U) | tmp);
    
    /* Configure the vertical start and stop position */
    tmp = ((pLayerCfg->windowY1 + (pVOUT->BPCR & VOUT_BPCR_AVBP)) << 16U);
    VOUT_LAYER(pVOUT, layerIdx)->WVPCR &= ~(VOUT_LxWVPCR_WVSTPOS | VOUT_LxWVPCR_WVSPPOS);
    VOUT_LAYER(pVOUT, layerIdx)->WVPCR  = ((pLayerCfg->windowY0 + (pVOUT->BPCR & VOUT_BPCR_AVBP) + 1U) | tmp);  
    
    /* Specifies the pixel format */
    VOUT_LAYER(pVOUT, layerIdx)->PFCR &= ~(VOUT_LxPFCR_PF);
    VOUT_LAYER(pVOUT, layerIdx)->PFCR = (pLayerCfg->pixelFormat);
    
    /* Configure the default color values */
    tmp = ((uint32_t)(pLayerCfg->backColor.green) << 8U);
    tmp1 = ((uint32_t)(pLayerCfg->backColor.red) << 16U);
    tmp2 = (pLayerCfg->alpha0 << 24U);  
    VOUT_LAYER(pVOUT, layerIdx)->DCCR &= ~(VOUT_LxDCCR_DCBLUE | VOUT_LxDCCR_DCGREEN | VOUT_LxDCCR_DCRED | VOUT_LxDCCR_DCALPHA);
    VOUT_LAYER(pVOUT, layerIdx)->DCCR = (pLayerCfg->backColor.blue | tmp | tmp1 | tmp2); 
    
    /* Specifies the constant alpha value */
    VOUT_LAYER(pVOUT, layerIdx)->CACR &= ~(VOUT_LxCACR_CONSTA);
    VOUT_LAYER(pVOUT, layerIdx)->CACR = (pLayerCfg->alpha);
    
    /* Configure the color frame buffer start address */
    VOUT_LAYER(pVOUT, layerIdx)->CFBAR &= ~(VOUT_LxCFBAR_CFBADD);
    VOUT_LAYER(pVOUT, layerIdx)->CFBAR = (pLayerCfg->FBStartAdress);
    
    if (pLayerCfg->pixelFormat == VOUT_PIXEL_FORMAT_ARGB8888)
    {
        tmp = 4U;
    }
    else if (pLayerCfg->pixelFormat == VOUT_PIXEL_FORMAT_RGB888)
    {
        tmp = 3U;
    }
    else if((pLayerCfg->pixelFormat == VOUT_PIXEL_FORMAT_ARGB4444) || \
            (pLayerCfg->pixelFormat == VOUT_PIXEL_FORMAT_RGB565)   || \
            (pLayerCfg->pixelFormat == VOUT_PIXEL_FORMAT_ARGB1555) || \
            (pLayerCfg->pixelFormat == VOUT_PIXEL_FORMAT_AL88))
    {
        tmp = 2U;
    }
    else
    {
        tmp = 1U;
    }
    
    /* Configure the color frame buffer pitch in byte */
    VOUT_LAYER(pVOUT, layerIdx)->CFBLR  &= ~(VOUT_LxCFBLR_CFBLL | VOUT_LxCFBLR_CFBP);
    VOUT_LAYER(pVOUT, layerIdx)->CFBLR  = (((pLayerCfg->imageWidth * tmp) << 16U) | ((pLayerCfg->windowX1 - pLayerCfg->windowX0) * tmp));
    
    /* Configure the frame buffer line number */
    VOUT_LAYER(pVOUT, layerIdx)->CFBLNR  &= ~(VOUT_LxCFBLNR_CFBLNBR);
    VOUT_LAYER(pVOUT, layerIdx)->CFBLNR  = (pLayerCfg->imageHeight);
    
    /* Enable VOUT Layer by setting LEN bit */  
    VOUT_LAYER(pVOUT, layerIdx)->CR |= (uint32_t)VOUT_LxCR_LEN;
}


RET_CODE HAL_VOUT_Init(VOUT_Device_T *pVOUT, VOUT_Config_T *pConfig)
{
    uint32_t tmp = 0U, tmp1 = 0U;
    
    /* Check the VOUT peripheral state */
    if (pVOUT == NULL || pConfig == NULL)
    {
        return RET_ERROR;
    }

    memset(&s_VOUTPrivateHandle, 0, sizeof(VOUT_Private_Handle_T));

    s_VOUTPrivateHandle.base = pVOUT;
    
    /* Configure the HS, VS, DE and PC polarity */
    pVOUT->GCR &= ~(VOUT_GCR_HSPOL | VOUT_GCR_VSPOL | VOUT_GCR_DEPOL | VOUT_GCR_PCPOL);
    pVOUT->GCR |=  (uint32_t)(pConfig->HSPolarity | pConfig->VSPolarity | \
                              pConfig->DEPolarity | pConfig->PCPolarity);
    
    /* Set Synchronization size */
    pVOUT->SSCR &= ~(VOUT_SSCR_VSH | VOUT_SSCR_HSW);
    tmp = (pConfig->horizontalSync << 16U);
    pVOUT->SSCR |= (tmp | pConfig->verticalSync);
    
    /* Set Accumulated Back porch */
    pVOUT->BPCR &= ~(VOUT_BPCR_AVBP | VOUT_BPCR_AHBP);
    tmp = (pConfig->accumulatedHBP << 16U);
    pVOUT->BPCR |= (tmp | pConfig->accumulatedVBP);
    
    /* Set Accumulated Active Width */
    pVOUT->AWCR &= ~(VOUT_AWCR_AAH | VOUT_AWCR_AAW);
    tmp = (pConfig->accumulatedActiveW << 16U);
    pVOUT->AWCR |= (tmp | pConfig->accumulatedActiveH);
    
    /* Set Total Width */
    pVOUT->TWCR &= ~(VOUT_TWCR_TOTALH | VOUT_TWCR_TOTALW);
    tmp = (pConfig->totalWidth << 16U);
    pVOUT->TWCR |= (tmp | pConfig->totalHeigh);
    
    /* Set the background color value */
    tmp = ((uint32_t)(pConfig->backColor.green) << 8U);
    tmp1 = ((uint32_t)(pConfig->backColor.red) << 16U);
    pVOUT->BCCR &= ~(VOUT_BCCR_BCBLUE | VOUT_BCCR_BCGREEN | VOUT_BCCR_BCRED);
    pVOUT->BCCR |= (tmp1 | tmp | pConfig->backColor.blue);
    
    /* Enable VOUT by setting LTDCEN bit */
    LL_VOUT_ENABLE(pVOUT);

    
    return RET_OK;
}

RET_CODE HAL_VOUT_ConfigLayer(VOUT_Device_T *pVOUT, VOUT_LayerConfig_T *pLayerCfg, uint32_t layerIdx)
{   
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Copy new layer configuration into handle structure */
    memcpy(&s_VOUTPrivateHandle.layerCfg[layerIdx], pLayerCfg, sizeof(VOUT_LayerConfig_T));  
    
    /* Configure the VOUT Layer */  
    VOUT_SetConfig(pVOUT, pLayerCfg, layerIdx);
    
    return RET_OK;
}


RET_CODE HAL_VOUT_ConfigColorKeying(VOUT_Device_T *pVOUT, uint32_t RGBValue, uint32_t layerIdx)
{
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Configure the default color values */
    VOUT_LAYER(pVOUT, layerIdx)->CKCR &=  ~(VOUT_LxCKCR_CKBLUE | VOUT_LxCKCR_CKGREEN | VOUT_LxCKCR_CKRED);
    VOUT_LAYER(pVOUT, layerIdx)->CKCR  = RGBValue;
   
    return RET_OK;
}

RET_CODE HAL_VOUT_ConfigCLUT(VOUT_Device_T *pVOUT, uint32_t *pCLUT, uint32_t CLUTSize, uint32_t layerIdx)
{
    uint32_t tmp = 0U;
    uint32_t counter = 0U;
    uint32_t pcounter = 0U;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    for(counter = 0U; (counter < CLUTSize); counter++)
    {
        if (s_VOUTPrivateHandle.layerCfg[layerIdx].pixelFormat == VOUT_PIXEL_FORMAT_AL44)
        {
            tmp  = (((counter + 16U*counter) << 24U) | ((uint32_t)(*pCLUT) & 0xFFU) | ((uint32_t)(*pCLUT) & 0xFF00U) | ((uint32_t)(*pCLUT) & 0xFF0000U));
        }
        else
        { 
            tmp  = ((counter << 24U) | ((uint32_t)(*pCLUT) & 0xFFU) | ((uint32_t)(*pCLUT) & 0xFF00U) | ((uint32_t)(*pCLUT) & 0xFF0000U));
        }
        pcounter = (uint32_t)pCLUT + sizeof(*pCLUT);
        pCLUT = (uint32_t *)pcounter;
    
        /* Specifies the C-LUT address and RGB value */
        VOUT_LAYER(pVOUT, layerIdx)->CLUTWR  = tmp;
    }
    
    return RET_OK;
}

RET_CODE HAL_VOUT_EnableColorKeying(VOUT_Device_T *pVOUT, uint32_t layerIdx)
{ 
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Enable VOUT color keying by setting COLKEN bit */
    VOUT_LAYER(pVOUT, layerIdx)->CR |= (uint32_t)VOUT_LxCR_COLKEN;
    
    return RET_OK;  
}

RET_CODE HAL_VOUT_DisableColorKeying(VOUT_Device_T *pVOUT, uint32_t layerIdx)
{
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Disable VOUT color keying by setting COLKEN bit */
    VOUT_LAYER(pVOUT, layerIdx)->CR &= ~(uint32_t)VOUT_LxCR_COLKEN;

    return RET_OK;
}

RET_CODE HAL_VOUT_EnableCLUT(VOUT_Device_T *pVOUT, uint32_t layerIdx)
{ 
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Enable VOUT color lookup table by setting CLUTEN bit */
    VOUT_LAYER(pVOUT, layerIdx)->CR |= (uint32_t)VOUT_LxCR_CLUTEN;
    
    return RET_OK;
}

RET_CODE HAL_VOUT_DisableCLUT(VOUT_Device_T *pVOUT, uint32_t layerIdx)
{ 
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Disable VOUT color lookup table by setting CLUTEN bit */
    VOUT_LAYER(pVOUT, layerIdx)->CR &= ~(uint32_t)VOUT_LxCR_CLUTEN;
 
    return RET_OK;
}

RET_CODE HAL_VOUT_EnablePreMultiply(VOUT_Device_T *pVOUT, uint32_t layerIdx)
{ 
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Enable VOUT Pre_Multiply function by setting PREMULEN bit */
    VOUT_LAYER(pVOUT, layerIdx)->CR |= (uint32_t)VOUT_LxCR_PREMULEN;
    
    return RET_OK;
}

RET_CODE HAL_VOUT_DisablePreMultiply(VOUT_Device_T *pVOUT, uint32_t layerIdx)
{ 
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Disable VOUT Pre_Multiply function by setting PREMULEN bit */
    VOUT_LAYER(pVOUT, layerIdx)->CR &= ~(uint32_t)VOUT_LxCR_PREMULEN;
 
    return RET_OK;
}

RET_CODE HAL_VOUT_EnableDither(VOUT_Device_T *pVOUT)
{    
    /* Enable Dither by setting DTEN bit */
    pVOUT->GCR |= (uint32_t)VOUT_GCR_DEN;
   
    return RET_OK;
}

RET_CODE HAL_VOUT_DisableDither(VOUT_Device_T *pVOUT)
{    
    /* Disable Dither by setting DTEN bit */
    pVOUT->GCR &= ~(uint32_t)VOUT_GCR_DEN;

    return RET_OK;
}

RET_CODE HAL_VOUT_SetWindowSize(VOUT_Device_T *pVOUT, uint32_t XSize, uint32_t YSize, uint32_t layerIdx)
{
    VOUT_LayerConfig_T *pLayerCfg;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Get layer configuration from handle structure */
    pLayerCfg = &s_VOUTPrivateHandle.layerCfg[layerIdx];
    
    /* update horizontal stop */
    pLayerCfg->windowX1 = XSize + pLayerCfg->windowX0;
    
    /* update vertical stop */  
    pLayerCfg->windowY1 = YSize + pLayerCfg->windowY0;
    
    /* Reconfigures the color frame buffer pitch in byte */
    pLayerCfg->imageWidth = XSize;
    
    /* Reconfigures the frame buffer line number */
    pLayerCfg->imageHeight = YSize;
    
    /* Set VOUT parameters */
    VOUT_SetConfig(pVOUT, pLayerCfg, layerIdx);
    
    return RET_OK;
}

RET_CODE HAL_VOUT_SetWindowPosition(VOUT_Device_T *pVOUT, uint32_t X0, uint32_t Y0, uint32_t layerIdx)
{
    VOUT_LayerConfig_T *pLayerCfg;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Get layer configuration from handle structure */
    pLayerCfg = &s_VOUTPrivateHandle.layerCfg[layerIdx];
    
    /* update horizontal start/stop */
    pLayerCfg->windowX0 = X0;
    pLayerCfg->windowX1 = X0 + pLayerCfg->imageWidth;
    
    /* update vertical start/stop */
    pLayerCfg->windowY0 = Y0;
    pLayerCfg->windowY1 = Y0 + pLayerCfg->imageHeight;
    
    /* Set VOUT parameters */
    VOUT_SetConfig(pVOUT, pLayerCfg, layerIdx);
    
    return RET_OK;
}

RET_CODE HAL_VOUT_SetPixelFormat(VOUT_Device_T *pVOUT, uint32_t pixelFormat, uint32_t layerIdx)
{
    VOUT_LayerConfig_T *pLayerCfg;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Get layer configuration from handle structure */
    pLayerCfg = &s_VOUTPrivateHandle.layerCfg[layerIdx];  
    
    /* Reconfigure the pixel format */
    pLayerCfg->pixelFormat = pixelFormat;
    
    /* Set VOUT parameters */
    VOUT_SetConfig(pVOUT, pLayerCfg, layerIdx);   
    
    return RET_OK;
}

RET_CODE HAL_VOUT_SetAlpha(VOUT_Device_T *pVOUT, uint32_t alpha, uint32_t layerIdx)
{
    VOUT_LayerConfig_T *pLayerCfg;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Get layer configuration from handle structure */
    pLayerCfg = &s_VOUTPrivateHandle.layerCfg[layerIdx];
    
    /* Reconfigure the Alpha value */
    pLayerCfg->alpha = alpha;
    
    /* Set VOUT parameters */
    VOUT_SetConfig(pVOUT, pLayerCfg, layerIdx);
    
    return RET_OK;
}

RET_CODE HAL_VOUT_SetAddress(VOUT_Device_T *pVOUT, uint32_t address, uint32_t layerIdx)
{
    VOUT_LayerConfig_T *pLayerCfg;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* Get layer configuration from handle structure */
    pLayerCfg = &s_VOUTPrivateHandle.layerCfg[layerIdx];
    
    /* Reconfigure the Address */
    pLayerCfg->FBStartAdress = address;
    
    /* Set VOUT parameters */
    VOUT_SetConfig(pVOUT, pLayerCfg, layerIdx);
    
    return RET_OK;
}

RET_CODE HAL_VOUT_SetPitch(VOUT_Device_T *pVOUT, uint32_t linePitchInPixels, uint32_t layerIdx)
{
    uint32_t tmp = 0U;
    uint32_t pitchUpdate = 0U;
    uint32_t pixelFormat = 0U;

    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    /* get LayerIdx used pixel format */
    pixelFormat = s_VOUTPrivateHandle.layerCfg[layerIdx].pixelFormat;
    
    if (pixelFormat == VOUT_PIXEL_FORMAT_ARGB8888)
    {
        tmp = 4U;
    }
    else if (pixelFormat == VOUT_PIXEL_FORMAT_RGB888)
    {
        tmp = 3U;
    }
    else if ((pixelFormat == VOUT_PIXEL_FORMAT_ARGB4444) || \
            (pixelFormat == VOUT_PIXEL_FORMAT_RGB565)   || \
            (pixelFormat == VOUT_PIXEL_FORMAT_ARGB1555) || \
            (pixelFormat == VOUT_PIXEL_FORMAT_AL88))
    {
        tmp = 2U;
    }
    else
    {
        tmp = 1U;
    }
    
    pitchUpdate = ((linePitchInPixels * tmp) << 16U);
    
    /* Clear previously set standard pitch */
    VOUT_LAYER(pVOUT, layerIdx)->CFBLR &= ~VOUT_LxCFBLR_CFBP;
    
    /* Set new line pitch value */
    VOUT_LAYER(pVOUT, layerIdx)->CFBLR |= pitchUpdate;
    
    return RET_OK;  
}

void HAL_VOUT_EnableInterrupts(VOUT_Device_T *pVOUT, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pVOUT->IER;

    pVOUT->IER = interruptMask | mask;
}

void HAL_VOUT_DisableInterrupts(VOUT_Device_T *pVOUT, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pVOUT->IER;

    pVOUT->IER = interruptMask & (~ mask);
}

void HAL_VOUT_OnOff(VOUT_Device_T *pVOUT, uint8_t onoff)
{
    if (onoff == 0)
    {
        pVOUT->GCR &= ~(VOUT_GCR_VOUTEN);
    }
    else
    {
        pVOUT->GCR |= VOUT_GCR_VOUTEN;
    }
}

RET_CODE HAL_VOUT_layerOnOff(VOUT_Device_T *pVOUT, uint32_t layerIdx, uint8_t onoff)
{
    if (layerIdx >= MAX_LAYER)
    {
        return RET_ERROR;
    }
    
    if (onoff == 0)
    {
        /* Disable VOUT Layer by setting LEN bit */  
        VOUT_LAYER(pVOUT, layerIdx)->CR &= ~((uint32_t)VOUT_LxCR_LEN);
    }
    else
    {
        /* Enable VOUT Layer by setting LEN bit */  
        VOUT_LAYER(pVOUT, layerIdx)->CR |= (uint32_t)VOUT_LxCR_LEN;
    }

    return RET_OK; 
}


