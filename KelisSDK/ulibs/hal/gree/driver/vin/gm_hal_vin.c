/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_hal_vin.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/11/28 
 *    
 * @brief        Video Input Caputre HAL module driver. 
 * 
 * @note
 *    2017/11/28, mark.lee, V1.0.0
 *        Initial version. 
 */
 
#include <gm_soc.h>
#include <gm_hal_vin.h>
#include <gm_hal_reset.h>
#include "gm_ll_vin.h"


/*******************************************************************************
* Constants and macros
*******************************************************************************/


/*******************************************************************************
* Global variables
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/

/*******************************************************************************
* Codes
*******************************************************************************/
RET_CODE HAL_VIN_Init(VIN_Device_T *pVIN, VIN_Config_T *pConfig)
{
    /* Check the DCMI peripheral state */
    if (pVIN == NULL || pConfig == NULL)
    {
        return RET_ERROR;
    }
    
    /* Set VIN parameters */
    /* Configures the HS, VS, DE and PC polarity */
    pVIN->CR &= ~(VIN_CR_PCKPOL | VIN_CR_HSPOL  | VIN_CR_VSPOL  | VIN_CR_EDM_0 |
                 VIN_CR_EDM_1  | VIN_CR_FCRC_0 | VIN_CR_FCRC_1 | VIN_CR_JPEG  |
                 VIN_CR_ESS);
    pVIN->CR |=  (uint32_t)(pConfig->synchroMode | pConfig->captureRate | \
                 pConfig->VSPolarity  | pConfig->HSPolarity  | \
                 pConfig->PCKPolarity | pConfig->extendedDataMode | \
                 pConfig->JPEGMode);
    
    if (pConfig->synchroMode == VIN_SYNCHRO_EMBEDDED)
    {
        pVIN->ESCR = VIN_ESCR_FSC_Field(pConfig->syncroCode.frameStartCode) |
             VIN_ESCR_LSC_Field(pConfig->syncroCode.lineStartCode) |
             VIN_ESCR_LEC_Field(pConfig->syncroCode.lineEndCode) |
             VIN_ESCR_FEC_Field(pConfig->syncroCode.frameEndCode);
    }
    
    return RET_OK;
}

RET_CODE HAL_VIN_Start(VIN_Device_T *pVIN, uint32_t mode, uint32_t pData, uint32_t length)
{ 
    /* Enable the VIN */
    LL_VIN_ENABLE(pVIN);
    
    /* Configure the DCMI Mode */
    pVIN->CR &= ~(VIN_CR_CM);
    pVIN->CR |=  (uint32_t)(mode);
    
    /* Set memory address and length*/
    pVIN->FBAR = (uint32_t)pData;
    pVIN->FBLR = length >> 2;

    /* Enable Capture */
    pVIN->CR |= (VIN_CR_CAPTURE | VIN_CR_MEMWR);
    
    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_VIN_Stop(VIN_Device_T *pVIN)
{   
    /* Disable Capture */
    pVIN->CR &= ~(VIN_CR_CAPTURE | VIN_CR_MEMWR);
    
    /* Disable the VIN */
    LL_VIN_DISABLE(pVIN);
    
    return RET_OK;
}

RET_CODE HAL_VIN_SetAddress(VIN_Device_T *pVIN, uint32_t address, uint32_t length)
{
    /* Set memory address and length*/
    pVIN->FBAR = (uint32_t)address;
    pVIN->FBLR = length >> 2;

    return RET_OK;
}

RET_CODE HAL_VIN_ConfigCrop(VIN_Device_T *pVIN, uint32_t X0, uint32_t Y0, uint32_t XSize, uint32_t YSize)
{ 
    /* Configure CROP */
    pVIN->CWSIZER = VIN_CWSIZE_CAPCNT_Field(XSize) | VIN_CWSIZE_VLINE_Field(YSize);
    pVIN->CWSTRTR = VIN_CWSTRT_HOFFCNT_Field(X0) | VIN_CWSTRT_VST_Field(Y0);
    
    return RET_OK;
}

RET_CODE HAL_VIN_DisableCrop(VIN_Device_T *pVIN)
{   
    /* Disable VIN Crop feature */
    pVIN->CR &= ~(uint32_t)VIN_CR_CROP;
    
    return RET_OK;
}

RET_CODE HAL_VIN_EnableCrop(VIN_Device_T *pVIN)
{ 
    /* Enable VIN Crop feature */
    pVIN->CR |= (uint32_t)VIN_CR_CROP;
    
    return RET_OK;
}

RET_CODE HAL_VIN_Disable(VIN_Device_T *pVIN)
{
    /* Disable VIN capture feature */
    pVIN->CR &= ~(uint32_t)VIN_CR_CAPTURE;
    
    return RET_OK;
}

RET_CODE HAL_VIN_Enable(VIN_Device_T *pVIN)
{
    /* Enable VIN capture feature */
    pVIN->CR |= (uint32_t)VIN_CR_CAPTURE;
    
    return RET_OK;
}

RET_CODE HAL_VIN_DisableMemoryWrite(VIN_Device_T *pVIN)
{
    /* Disable Memory Write feature */
    pVIN->CR &= ~(uint32_t)VIN_CR_MEMWR;
    
    return RET_OK;
}

RET_CODE HAL_VIN_EnableMemoryWrite(VIN_Device_T *pVIN)
{
    /* Enable Memory Write feature */
    pVIN->CR |= (uint32_t)VIN_CR_MEMWR;
    
    return RET_OK;
}

RET_CODE HAL_VIN_DisableColorSpaceConv(VIN_Device_T *pVIN)
{
    /* Disable Color Space Convert feature */
    pVIN->CR &= ~(uint32_t)VIN_CR_CSC;
    
    return RET_OK;
}

RET_CODE HAL_VIN_EnableColorSpaceConv(VIN_Device_T *pVIN)
{
    /* Enable Color Space Convert feature */
    pVIN->CR |= (uint32_t)VIN_CR_CSC;
    
    return RET_OK;
}

RET_CODE HAL_VIN_DisableJPEGMode(VIN_Device_T *pVIN)
{
    /* Disable Jpeg data transfers */
    pVIN->CR &= ~(uint32_t)VIN_CR_JPEG;
    
    return RET_OK;
}

RET_CODE HAL_VIN_EnableJPEGMode(VIN_Device_T *pVIN)
{
    /* Enable Jpeg data transfers */
    pVIN->CR |= (uint32_t)VIN_CR_JPEG;
    
    return RET_OK;
}

uint32_t HAL_VIN_GetJPEGLength(VIN_Device_T *pVIN)
{
    uint32_t length = 0;
    
    /* Capture image length */
    length = (pVIN->IMLR & 0x00ffffff) << 2;
    
    return length;
}

RET_CODE HAL_VIN_SwitchPath(VIN_Device_T *pVIN, uint32_t index)
{
    if (index >= 2)
    {
        return RET_ERROR;
    }

    if (index == VIN_PATH_1)
    {
        /* Select pinmux1 can be alternative on chip */
        pVIN->CR &= ~(uint32_t)VIN_CR_PATHSEL;
    }
    else
    {
         /* Select pinmux2 can be alternative on chip */
        pVIN->CR |= (uint32_t)VIN_CR_PATHSEL;
    }
    
    return RET_OK;
}


void HAL_VIN_EnableInterrupts(VIN_Device_T *pVIN, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pVIN->IER;

    pVIN->IER = interruptMask | mask;
}

void HAL_VIN_DisableInterrupts(VIN_Device_T *pVIN, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pVIN->IER;

    pVIN->IER = interruptMask & (~ mask);
}

void HAL_VIN_BackupData(VIN_Device_T *pVIN)
{
	uint32_t RegCr, RegIer, RegCwstrtr, RegCwsizer, RegFbar, RegFblr;
	RegCr 		= pVIN->CR ;
    RegIer 		= pVIN->IER;
    RegCwstrtr 	= pVIN->CWSTRTR;
    RegCwsizer 	= pVIN->CWSIZER;
    RegFbar 	= pVIN->FBAR;
	RegFblr 	= pVIN->FBLR;
	
	HAL_Reset_Module(RESET_VIN_SW_RSTJ);
	
	pVIN->IER 		= RegIer;
	pVIN->CWSTRTR 	= RegCwstrtr;
	pVIN->CWSIZER 	= RegCwsizer;
	pVIN->FBAR 		= RegFbar;
	pVIN->FBLR 		= RegFblr;
	pVIN->CR 		= RegCr;

}

