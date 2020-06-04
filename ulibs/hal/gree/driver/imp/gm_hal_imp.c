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
 * @brief        IMP HAL module driver.
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

#define IMP_CLUT_SIZE            0x1FF                   /**< IMP CLUT size */

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
    /* Configure IMP data size */
    pIMP->NLR &= ~(IMP_NLR_NL|IMP_NLR_PL);
    pIMP->NLR |= (height | ((width << IMP_NLR_PL_Pos) & IMP_NLR_PL));

    /* Configure IMP destination address */
    pIMP->OMAR = dstAddress;

    /* Register to memory IMP mode selected */
    if (s_cfg.mode == IMP_R2M)
    {
        /* Write to IMP OCOLR register */
        pIMP->OCOLR = pdata;
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
    if((pIMP == NULL) || ((pConfig->outputOffset) > IMP_OFFSET))
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
    regValue = pLayerCfg->inputColorMode | ((pLayerCfg->alphaMode << IMP_BGPFCCR_AM_Pos) & IMP_BGPFCCR_AM) | ((pLayerCfg->inputAlpha << IMP_BGPFCCR_ALPHA_Pos) & IMP_BGPFCCR_ALPHA);


    /* Configure the background IMP layer */
    if(layerIdx == 0)
    {
        /* Write IMP BGPFCCR register */
        pIMP->BGPFCCR &= ~(IMP_BGPFCCR_CM | IMP_BGPFCCR_AM | IMP_BGPFCCR_ALPHA);
        pIMP->BGPFCCR |= regValue;

        /* IMP BGOR register configuration -------------------------------------*/
        pIMP->BGOR = pLayerCfg->inputOffset;

        /* IMP BGCOLR register configuration -------------------------------------*/
        if ((pLayerCfg->inputColorMode == IMP_INPUT_A4) || (pLayerCfg->inputColorMode == IMP_INPUT_A8))
        {
            pIMP->BGCOLR = pLayerCfg->inputAlpha & (IMP_BGCOLR_BLUE|IMP_BGCOLR_GREEN|IMP_BGCOLR_RED);
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
            pIMP->FGCOLR = pLayerCfg->inputAlpha & (IMP_FGCOLR_BLUE|IMP_FGCOLR_GREEN|IMP_FGCOLR_RED);
        }
    }

    return RET_OK;
}

RET_CODE HAL_IMP_Start(IMP_Device_T *pIMP, uint32_t pdata, uint32_t dstAddress, IMP_ScaleCfg_T *scaleCfg)
{
    /* Check the parameters */
    if ((scaleCfg->oriHeight > IMP_LINE) || (scaleCfg->oriWidth > IMP_PIXEL))
    {
        return RET_INVPARAM;
    }

    /* Configure the source, destination address and the data size */
    IMP_SetConfig(pIMP, pdata, dstAddress, scaleCfg->oriWidth, scaleCfg->oriHeight);

    if (scaleCfg->enable)
    {
        pIMP->SNLR &= ~(IMP_SNLR_NL|IMP_SNLR_PL);
        pIMP->SNLR |= (scaleCfg->scaleHeight | ((scaleCfg->scaleWidth << IMP_SNLR_PL_Pos) & IMP_SNLR_PL));

        pIMP->SCR &= ~(IMP_SCR_SHF|IMP_SCR_SHI|IMP_SCR_SVF|IMP_SCR_SVI);
        pIMP->SCR |= (scaleCfg->shf | ((scaleCfg->shi << IMP_SCR_SHI_Pos) & IMP_SCR_SHI)  \
                      | ((scaleCfg->svf << IMP_SCR_SVF_Pos) & IMP_SCR_SVF) \
                      | ((scaleCfg->svi << IMP_SCR_SVI_Pos) & IMP_SCR_SVI));

        pIMP->CR |= IMP_CR_SCA;
    }
    else
    {
        pIMP->CR &= ~IMP_CR_SCA;
    }
#if 0   /*harris enable */
		pIMP->CR |= IMP_CR_HAR_EN;
		pIMP->THR = 8000; /*3500*/
#endif

    /* Enable the Peripheral */
    LL_IMP_ENABLE(pIMP);

    return RET_OK;
}

RET_CODE HAL_IMP_HarrisStart(IMP_Device_T *pIMP, uint32_t pdata, uint32_t dstAddress, IMP_ScaleCfg_T *scaleCfg, int harEn, unsigned int thrvalue)
{
    /* Check the parameters */
    if ((scaleCfg->oriHeight > IMP_LINE) || (scaleCfg->oriWidth > IMP_PIXEL))
    {
        return RET_INVPARAM;
    }

    /* Configure the source, destination address and the data size */
    IMP_SetConfig(pIMP, pdata, dstAddress, scaleCfg->oriWidth, scaleCfg->oriHeight);

    if (scaleCfg->enable)
    {
        pIMP->SNLR &= ~(IMP_SNLR_NL|IMP_SNLR_PL);
        pIMP->SNLR |= (scaleCfg->scaleHeight | ((scaleCfg->scaleWidth << IMP_SNLR_PL_Pos) & IMP_SNLR_PL));

        pIMP->SCR &= ~(IMP_SCR_SHF|IMP_SCR_SHI|IMP_SCR_SVF|IMP_SCR_SVI);
        pIMP->SCR |= (scaleCfg->shf | ((scaleCfg->shi << IMP_SCR_SHI_Pos) & IMP_SCR_SHI)  \
                      | ((scaleCfg->svf << IMP_SCR_SVF_Pos) & IMP_SCR_SVF) \
                      | ((scaleCfg->svi << IMP_SCR_SVI_Pos) & IMP_SCR_SVI));

        pIMP->CR |= IMP_CR_SCA;
    }
    else
    {
        pIMP->CR &= ~IMP_CR_SCA;
    }
    /*harris enable */

    if (harEn == 1)
    {
		pIMP->CR |= IMP_CR_HAR_EN;
		pIMP->THR = thrvalue;/*8000,3500*/
    }
    else
    {
		pIMP->CR &= ~(IMP_CR_HAR_EN);
    }
		
    /* Enable the Peripheral */
    LL_IMP_ENABLE(pIMP);

    return RET_OK;
}


RET_CODE HAL_IMP_BlendingStart(IMP_Device_T *pIMP, uint32_t srcAddress1, uint32_t  srcAddress2, uint32_t dstAddress, IMP_ScaleCfg_T *scaleCfg)
{
    /* Check the parameters */
    if ((scaleCfg->oriHeight > IMP_LINE) || (scaleCfg->oriWidth > IMP_PIXEL))
    {
        return RET_INVPARAM;
    }

    /* Configure DMA2D Stream source2 address */
    pIMP->BGMAR = srcAddress2;

    /* Configure the source, destination address and the data size */
    IMP_SetConfig(pIMP, srcAddress1, dstAddress, scaleCfg->oriWidth, scaleCfg->oriHeight);

    if (scaleCfg->enable)
    {
        pIMP->SNLR &= ~(IMP_SNLR_NL|IMP_SNLR_PL);
        pIMP->SNLR |= (scaleCfg->scaleHeight | ((scaleCfg->scaleWidth << IMP_SNLR_PL_Pos) & IMP_SNLR_PL));

        pIMP->SCR &= ~(IMP_SCR_SHF|IMP_SCR_SHI|IMP_SCR_SVF|IMP_SCR_SVI);
        pIMP->SCR |= (scaleCfg->shf | ((scaleCfg->shi << IMP_SCR_SHI_Pos) & IMP_SCR_SHI)  \
                      | ((scaleCfg->svf << IMP_SCR_SVF_Pos) & IMP_SCR_SVF) \
                      | ((scaleCfg->svi << IMP_SCR_SVI_Pos) & IMP_SCR_SVI));

        pIMP->CR |= IMP_CR_SCA;
    }
    else
    {
        pIMP->CR &= ~IMP_CR_SCA;
    }

    /* Enable the Peripheral */
    LL_IMP_ENABLE(pIMP);

    return RET_OK;
}

RET_CODE HAL_IMP_Abort(IMP_Device_T *pIMP)
{
    /* Abort the IMP transfer */
    pIMP->CR &= ~(IMP_CR_ABORT|IMP_CR_START);
    pIMP->CR |= IMP_CR_ABORT;

    /* Check if the IMP is effectively disabled */
    while((pIMP->CR & IMP_CR_START) != 0);

    /* Disable the Transfer Complete, Transfer Error and Configuration Error interrupts */
    __HAL_IMP_DISABLE_IT(pIMP, IMP_IT_TC|IMP_IT_TE|IMP_IT_CE);

    return RET_OK;
}

RET_CODE HAL_IMP_Suspend(IMP_Device_T *pIMP)
{
    /* Suspend the IMP transfer */
    pIMP->CR &= ~(IMP_CR_SUSP|IMP_CR_START);
    pIMP->CR |= IMP_CR_SUSP;

    return RET_OK;
}

RET_CODE HAL_IMP_Resume(IMP_Device_T *pIMP)
{
    /* Suspend the IMP transfer */
    pIMP->CR &= ~(IMP_CR_SUSP|IMP_CR_START);

    return RET_OK;
}

RET_CODE HAL_IMP_CLUTLoad(IMP_Device_T *pIMP, IMP_CLUTCfg_T *CLUTCfg, uint32_t layerIdx)
{
    uint32_t tmp = 0U;
    uint32_t counter = 0U;
    uint32_t pcounter = 0U;
    uint32_t *pCLUT = NULL;

    /* Check the parameters */
    if ((layerIdx >= MAX_IMP_LAYER) || (((CLUTCfg->CLUTColorMode) != IMP_CCM_ARGB8888) && ((CLUTCfg->CLUTColorMode) != IMP_CCM_RGB888))
        || (CLUTCfg->pCLUT == NULL) || ((CLUTCfg->size) > IMP_CLUT_SIZE))
    {
        return RET_INVPARAM;
    }

    pCLUT = CLUTCfg->pCLUT;

    /* Configure the CLUT of the background IMP layer */
    if(layerIdx == 0U)
    {
        if (CLUTCfg->enable)
        {
            for(counter = 0U; (counter < CLUTCfg->size); counter++)
            {
                tmp  = (uint32_t)(*pCLUT);

                pcounter = (uint32_t)pCLUT + sizeof(*pCLUT);
                pCLUT = (uint32_t *)pcounter;


                /* Specifies the C-LUT address and RGB value */
                pIMP->BGCLUT[counter] = tmp;
            }
            pIMP->BGPFCCR |= IMP_BGPFCCR_CLUTEN;
        }
        else
        {
            pIMP->BGPFCCR &= ~IMP_BGPFCCR_CLUTEN;
        }
    }
    /* Configure the CLUT of the foreground IMP layer */
    else
    {
        if (CLUTCfg->enable)
        {
            for(counter = 0U; (counter < CLUTCfg->size); counter++)
            {
                tmp  = (uint32_t)(*pCLUT);

                pcounter = (uint32_t)pCLUT + sizeof(*pCLUT);
                pCLUT = (uint32_t *)pcounter;


                /* Specifies the C-LUT address and RGB value */
                pIMP->FGCLUT[counter] = tmp;
            }
            pIMP->FGPFCCR |= IMP_FGPFCCR_CLUTEN;
        }
        else
        {
            pIMP->FGPFCCR &= ~IMP_FGPFCCR_CLUTEN;
        }
    }

    return RET_OK;
}

RET_CODE HAL_IMP_ProgramLineEvent(IMP_Device_T *pIMP, uint32_t line)
{
    /* Check the parameters */
    if (line > IMP_LWR_LW)
    {
        return RET_INVPARAM;
    }

    /* Sets the Line watermark configuration */
    pIMP->LWR = line;

    /* Enable the Line interrupt */
    __HAL_IMP_ENABLE_IT(pIMP, IMP_IT_TW);

    return RET_OK;
}

