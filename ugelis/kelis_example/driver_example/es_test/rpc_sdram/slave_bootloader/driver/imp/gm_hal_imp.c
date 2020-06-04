/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_imp.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2018/01/03
 *
 * @brief        VOUT HAL module driver.
 *
 * @note
 *    2018/01/03 , mark.lee, V1.0.0
 *        Initial version.
 */

#include <gm_soc.h>
#include <gm_hal_imp.h>
#include "gm_ll_imp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define IMP_OFFSET               IMP_FGOR_LO             /**< Line Offset */

#define IMP_PIXEL                (IMP_NLR_PL >> 16U)     /**< IMP number of pixels per line */
#define IMP_LINE                 IMP_NLR_NL              /**< IMP number of lines           */

#define IMP_CLUT_SIZE            (IMP_FGPFCCR_CS >> 8U)  /**< IMP CLUT size */

/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Global variables
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/
static IMP_Config_T s_cfg;

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
 * @brief  Set the IMP transfer parameters.
 * @param  pIMP:       IMP peripheral base address.
 * @param  pdata:      The source memory Buffer address
 * @param  dstAddress: The destination memory Buffer address
 * @param  width:      The width of data to be transferred from source to destination.
 * @param  height:     The height of data to be transferred from source to destination.
 * @retval HAL status
 */
static void IMP_SetConfig(IMP_Device_T *pIMP, uint32_t pdata, uint32_t dstAddress, uint32_t width, uint32_t height)
{
    uint32_t tmp = 0U;
    uint32_t tmp1 = 0U;
    uint32_t tmp2 = 0U;
    uint32_t tmp3 = 0U;
    uint32_t tmp4 = 0U;

    /* Configure IMP data size */
    pIMP->NLR &= ~(IMP_NLR_NL | IMP_NLR_PL);
    pIMP->NLR |= (height | (width << IMP_NLR_PL_Pos));

    /* Configure IMP destination address */
    pIMP->OMAR = dstAddress;

    /* Register to memory IMP mode selected */
    if (s_cfg.mode == IMP_R2M)
    {
        tmp1 = pdata & IMP_OCOLR_ALPHA_1;
        tmp2 = pdata & IMP_OCOLR_RED_1;
        tmp3 = pdata & IMP_OCOLR_GREEN_1;
        tmp4 = pdata & IMP_OCOLR_BLUE_1;

        /* Prepare the value to be written to the OCOLR register according to the color mode */
        if (s_cfg.colorMode == IMP_OUTPUT_ARGB8888)
        {
            tmp = (tmp3 | tmp2 | tmp1 | tmp4);
        }
        else if (s_cfg.colorMode == IMP_OUTPUT_RGB888)
        {
            tmp = (tmp3 | tmp2 | tmp4);
        }
        else if (s_cfg.colorMode == IMP_OUTPUT_RGB565)
        {
            tmp2 = (tmp2 >> 19U);
            tmp3 = (tmp3 >> 10U);
            tmp4 = (tmp4 >> 3U);
            tmp  = ((tmp3 << 5U) | (tmp2 << 11U) | tmp4);
        }
        else if (s_cfg.colorMode == IMP_OUTPUT_ARGB1555)
        {
            tmp1 = (tmp1 >> 31U);
            tmp2 = (tmp2 >> 19U);
            tmp3 = (tmp3 >> 11U);
            tmp4 = (tmp4 >> 3U);
            tmp  = ((tmp3 << 5U) | (tmp2 << 10U) | (tmp1 << 15U) | tmp4);
        }
        else /* s_cfg.colorMode = IMP_OUTPUT_ARGB4444 */
        {
            tmp1 = (tmp1 >> 28U);
            tmp2 = (tmp2 >> 20U);
            tmp3 = (tmp3 >> 12U);
            tmp4 = (tmp4 >> 4U);
            tmp  = ((tmp3 << 4U) | (tmp2 << 8U) | (tmp1 << 12U) | tmp4);
        }
        /* Write to IMP OCOLR register */
        pIMP->OCOLR = tmp;
    }
    else /* M2M, M2M_PFC or M2M_Blending IMP Mode */
    {
        /* Configure IMP source address */
        pIMP->FGMAR = pdata;
    }
}

RET_CODE HAL_IMP_Init(IMP_Device_T *pIMP, IMP_Config_T *pConfig)
{
    /* Check the parameters */
    if ((pIMP == NULL) || ((pConfig->outputOffset) > IMP_OFFSET))
    {
        return RET_ERROR;
    }

    memset(&s_cfg, 0, sizeof(IMP_Config_T));

    /* IMP CR register configuration -------------------------------------------*/
    pIMP->CR &= ~(IMP_CR_MODE);
    pIMP->CR |= pConfig->mode;

    /* IMP OPFCCR register configuration ---------------------------------------*/
    pIMP->OPFCCR &= ~(IMP_OPFCCR_CM);
    pIMP->OPFCCR |= pConfig->colorMode;

    /* IMP OOR register configuration ------------------------------------------*/
    pIMP->OOR &= ~(IMP_OOR_LO);
    pIMP->OOR |= pConfig->outputOffset;

    s_cfg.mode = pConfig->mode;
    s_cfg.colorMode = pConfig->colorMode;
    s_cfg.outputOffset = pConfig->outputOffset;

    return RET_OK;
}

RET_CODE HAL_IMP_ConfigLayer(IMP_Device_T *pIMP, IMP_LayerCfg_T *pLayerCfg, uint32_t layerIdx)
{
    uint32_t regValue = 0U;

    /* Check the parameters */
    if ((layerIdx >= MAX_IMP_LAYER) || ((pLayerCfg->inputOffset) > IMP_OFFSET))
    {
        return RET_INVPARAM;
    }

    /* Prepare the value to be written to the BGPFCCR register */
    regValue = pLayerCfg->inputColorMode | (pLayerCfg->alphaMode << IMP_BGPFCCR_AM_Pos) | (pLayerCfg->inputAlpha & IMP_BGPFCCR_ALPHA);


    /* Configure the background IMP layer */
    if (layerIdx == 0)
    {
        /* Write IMP BGPFCCR register */
        pIMP->BGPFCCR &= ~(IMP_BGPFCCR_CM | IMP_BGPFCCR_AM | IMP_BGPFCCR_ALPHA);
        pIMP->BGPFCCR |= regValue;

        /* IMP BGOR register configuration -------------------------------------*/
        pIMP->BGOR = pLayerCfg->inputOffset;

        /* IMP BGCOLR register configuration -------------------------------------*/
        if ((pLayerCfg->inputColorMode == IMP_INPUT_A4) || (pLayerCfg->inputColorMode == IMP_INPUT_A8))
        {
            pIMP->BGCOLR = pLayerCfg->inputAlpha & (IMP_BGCOLR_BLUE | IMP_BGCOLR_GREEN | IMP_BGCOLR_RED);
        }
    }
    /* Configure the foreground IMP layer */
    else
    {
        /* Write IMP FGPFCCR register */
        pIMP->FGPFCCR &= ~(IMP_BGPFCCR_CM | IMP_BGPFCCR_AM | IMP_BGPFCCR_ALPHA);
        pIMP->FGPFCCR |= regValue;

        /* IMP FGOR register configuration -------------------------------------*/
        pIMP->FGOR = pLayerCfg->inputOffset;

        /* IMP FGCOLR register configuration -------------------------------------*/
        if ((pLayerCfg->inputColorMode == IMP_INPUT_A4) || (pLayerCfg->inputColorMode == IMP_INPUT_A8))
        {
            pIMP->FGCOLR = pLayerCfg->inputAlpha & (IMP_FGCOLR_BLUE | IMP_FGCOLR_GREEN | IMP_FGCOLR_RED);
        }
    }

    return RET_OK;
}

RET_CODE HAL_IMP_Start(IMP_Device_T *pIMP, uint32_t pdata, uint32_t dstAddress, uint32_t width,  uint32_t height)
{
    /* Check the parameters */
    if ((height > IMP_LINE) || (width > IMP_PIXEL))
    {
        return RET_INVPARAM;
    }

    /* Configure the source, destination address and the data size */
    IMP_SetConfig(pIMP, pdata, dstAddress, width, height);

    /* Enable the Peripheral */
    LL_IMP_ENABLE(pIMP);

    return RET_OK;
}

RET_CODE HAL_IMP_BlendingStart(IMP_Device_T *pIMP, uint32_t srcAddress1, uint32_t  srcAddress2, uint32_t dstAddress, uint32_t width,  uint32_t height)
{
    /* Check the parameters */
    if ((height > IMP_LINE) || (width > IMP_PIXEL))
    {
        return RET_INVPARAM;
    }

    /* Configure DMA2D Stream source2 address */
    pIMP->BGMAR = srcAddress2;

    /* Configure the source, destination address and the data size */
    IMP_SetConfig(pIMP, srcAddress1, dstAddress, width, height);

    /* Enable the Peripheral */
    LL_IMP_ENABLE(pIMP);

    return RET_OK;
}

RET_CODE HAL_IMP_Abort(IMP_Device_T *pIMP)
{
    /* Abort the IMP transfer */
    /* START bit is reset to make sure not to set it again, in the event the HW clears it
    between the register read and the register write by the CPU (writing 0 has no
    effect on START bitvalue) */
    pIMP->CR &= ~(IMP_CR_ABORT | IMP_CR_START);
    pIMP->CR |= IMP_CR_ABORT;

    /* Check if the DMA2D is effectively disabled */
    while ((pIMP->CR & IMP_CR_START) != 0);

    /* Disable the Transfer Complete, Transfer Error and Configuration Error interrupts */
    __HAL_IMP_DISABLE_IT(pIMP, IMP_IT_TC | IMP_IT_TE | IMP_IT_CE);

    return RET_OK;
}

RET_CODE HAL_IMP_CLUTLoad(IMP_Device_T *pIMP, IMP_CLUTCfg_T *CLUTCfg, uint32_t layerIdx)
{
    /* Check the parameters */
    if ((layerIdx >= MAX_IMP_LAYER) || (((CLUTCfg->CLUTColorMode) != IMP_CCM_ARGB8888) && ((CLUTCfg->CLUTColorMode) == IMP_CCM_RGB888))
            || ((CLUTCfg->size) > IMP_CLUT_SIZE))
    {
        return RET_INVPARAM;
    }

    /* Configure the CLUT of the background IMP layer */
    if (layerIdx == 0U)
    {
        /* Write background CLUT memory address */
        pIMP->BGCMAR = (uint32_t)CLUTCfg->pCLUT;

        /* Write background CLUT size and CLUT color mode */
        pIMP->BGPFCCR &= ~(IMP_BGPFCCR_CS | IMP_BGPFCCR_CCM);
        pIMP->BGPFCCR |= ((CLUTCfg->size << IMP_BGPFCCR_CS_Pos) | (CLUTCfg->CLUTColorMode << IMP_BGPFCCR_CCM_Pos));

        /* Enable the CLUT loading for the background */
        pIMP->BGPFCCR |= IMP_BGPFCCR_START;
    }
    /* Configure the CLUT of the foreground DMA2D layer */
    else
    {
        /* Write foreground CLUT memory address */
        pIMP->FGCMAR = (uint32_t)CLUTCfg->pCLUT;

        /* Write foreground CLUT size and CLUT color mode */
        pIMP->FGPFCCR &= ~(IMP_FGPFCCR_CS | IMP_FGPFCCR_CCM);
        pIMP->FGPFCCR |= ((CLUTCfg->size << IMP_FGPFCCR_CS_Pos) | (CLUTCfg->CLUTColorMode << IMP_FGPFCCR_CCM_Pos));

        /* Enable the CLUT loading for the foreground */
        pIMP->FGPFCCR |= IMP_FGPFCCR_START;
    }

    return RET_OK;
}

