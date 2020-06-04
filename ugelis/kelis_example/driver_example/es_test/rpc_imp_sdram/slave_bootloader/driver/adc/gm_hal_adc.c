/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_adc.c
 *
 * @author        Richard.liu
 *
 * @version       1.0.0
 *
 * @date          2018/03/19
 *
 * @brief        ADC APIs for application.
 *
 * @note
 *    2018/03/19, Richard.liu, V1.0.0
 *        Initial version.
 */
#include <gm_common.h>
#include <gm_soc.h>
#include <adc/gm_ll_adc.h>
#include <gm_hal_adc.h>
#include "stdio.h"

#ifdef RABBIT_NO_OS_SYSTEM
    STATIC_ISR(ADC1_IRQn, ADC1_InterruptHandler);
    STATIC_ISR(ADC2_IRQn, ADC2_InterruptHandler);
    STATIC_ISR(ADC3_IRQn, ADC3_InterruptHandler);
#endif

static ADC_HandleTypeDef *s_pADCHandle[MAX_ADC_DEVICE] = {NULL};
ADC_Callback_T s_adcCallback = NULL;
static DMA_Handle_T s_DMAHandle;
static RET_CODE ADC_Stop(ADC_HandleTypeDef *hADC);
RET_CODE ADC_TryLock(ADC_HandleTypeDef *hADC);
static void ADC_DmaCallback(struct DMA_Handle *pDMAHandle, uint8_t event, unsigned long param);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef  HAL_ADC_FifoConvertValue(ADC_HandleTypeDef *hadc, unsigned char  resolution, uint32_t *pConvertData, uint8_t length);
static void (*s_ADCIsr)(ADC_HandleTypeDef *hADC);
RET_CODE HAL_ADC_DmaConfig(ADC_HandleTypeDef *hADC, uint32_t *pConvertData, uint8_t length);
void HAL_ADC_CLOCK(int en);
void HAL_ADC_RESET(void);
RET_CODE ADC_TryLock(ADC_HandleTypeDef *hADC);

static void ADC_InterruptConfig(ADC_HandleTypeDef *hADC)
{
    /*make sure overfow match eos interrupt is enable */
    ADC_INTERRUPT_ENABLE(hADC->Instance, ADC_COMP_MAT_INTEN  | \
                         ADC_DFIFO_OVF_INTEN | ADC_EOS_INTEN);

    if (hADC->Instance == ADC1)
    {
        NVIC_EnableIRQ(ADC1_IRQn);
    }
    if (hADC->Instance == ADC2)
    {
        NVIC_EnableIRQ(ADC2_IRQn);
    }
    if (hADC->Instance == ADC3)
    {
        NVIC_EnableIRQ(ADC3_IRQn);
    }
}

static void ADC_DmaCallback(struct DMA_Handle *pDMAHandle, uint8_t event, unsigned long param)
{
    int i = 0;
    ADC_HandleTypeDef *hADC = GET_PARENTHDL_FROM_DMAHDL(pDMAHandle, ADC_HandleTypeDef);

    for (i = 0; i < hADC->dma_rst_len; i++)
    {
        printk("hADC->dma_rst_data=0x%x\n", hADC->dma_rst_data[i]);
    }

    hADC->State = HAL_ADC_STATE_DMA_FINISH;

    if (event == DMA_EVENT_TRANSFER_DONE)
    {

    }
    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {

    }
    else
    {

    }
    __HAL_UNLOCK(hADC);

    /* release DMA device */
    HAL_DMA_Abort(hADC->pDMAHandle);
}

void HAL_ADC_IRQ_handler(ADC_HandleTypeDef *hADC)
{
    ADC_Device_T *pDevice = (ADC_Device_T *)hADC->Instance;
    unsigned char  len = hADC->Init.NbrOfConversion;
    unsigned char resolution = hADC->Init.Resolution;
    uint32_t convert_value[SEQUECE_LEN_MAX];

    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_EOC_INT) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_EOC_INTEN))
    {

        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_EOC_INT);
    }

    /* cpu mode need send event by irqstatus */
    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_DOFI) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_DFIFO_OVF_INTEN))
    {
        /* clear overflower irq and sent this event */
        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_DOFI);
    }
    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_CMI) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_COMP_MAT_INTEN))
    {
        printk("ADC_ST_CMI\n");
        /* clear compare match irq and sent this event */
        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_CMI);
    }

    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_EOCS_INT) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_EOS_INTEN))
    {

        /* clear eos irq */
        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_EOCS_INT);

        if (hADC->expectFinishStatus == HAL_ADC_STATE_DMA_FINISH)
        {
            /*
             * DMA sync Machine will process at ADC_DmaCallback
             * so here only clear irq and return
             */
            return ;
        }

        HAL_ADC_FifoConvertValue(hADC, resolution, &convert_value[0], len);
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine status*/
            hADC->State = HAL_ADC_STATE_EOS_REG;
        }
        else
        {
            /* no block send convert finish event to usr */
            if (s_adcCallback)
            {

            }
            else
            {

            }

            hADC->State = HAL_ADC_STATE_READY;
        }
    }
    printk("pDevice->ST =0x%x, pDevice->IE = 0x%x\n", pDevice->ST, pDevice->IE);
    __HAL_ADC_CLEAR_FLAG(hADC, 0xFFFFFFFF);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

}

void HAL_ADC_Sample_Time(ADC_Device_T *pADC)
{
    if (pADC !=  NULL)
    {
        pADC->SAMPTM[0] |= 0xFFFFFFFF;
        pADC->SAMPTM[1] |= 0xFFFFFFFF;
        pADC->SAMPTM[2] |= 0xFFFFFFFF;
        pADC->SAMPTM[3] |= 0xFFFFFFFF;
    }
}

static void HAL_ADC_Triger(ADC_Device_T *pADC, ADC_TrigerChannel_T trigerchannel)
{
    if (pADC !=  NULL)
    {
        if (trigerchannel == ADC_SW_REGU_TRIGER)
        {
            pADC->TRIG = ADC_TRIG_CT;
        }
        else
        {
            pADC->TRIG = ADC_TRIG_JCT;
        }
    }
}

static RET_CODE ADC_Init(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL)
    {
        return RET_ERROR;
    }
    /* Set ADC work(scan) mode */
    hadc->Instance->CTRL &= ~(ADC_CTRL_WM);
    hadc->Instance->CTRL |=  ADC_CTRL_WORKMODE(hadc->Init.workMode);

    /* Set ADC resolution */
    hadc->Instance->CTRL &= ~(ADC_CTRL_BNIT);
    hadc->Instance->CTRL |=  ADC_CTRL_RESOLUTION(hadc->Init.Resolution);

    /* Set ADC lowpower mode */
    hadc->Instance->CTRL &= ~(ADC_CTRL_LPEN);
    hadc->Instance->CTRL |=  ADC_CTRL_LOWPOWER(hadc->Init.lowpowerEn);

    /* Set ADC Discontinuous mode on regular channels*/
    hadc->Instance->CTRL &= ~(ADC_CTRL_DCONTEN);
    hadc->Instance->CTRL |=  ADC_CTRL_DISCONTINUOUS(hadc->Init.discontinuousConvEn);

    /* Set ADC number of conversion */
    hadc->Instance->RCHN2 &= ~(ADC_RCHN2_RSL);
    hadc->Instance->RCHN2 |=  ADC_RCHN_L(hadc->Init.NbrOfConversion);
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct without initializing the ADC MSP.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc)
{
    /* Check ADC handle */
    if (hadc == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
    assert_param(IS_ADC_RESOLUTION(hadc->Init.Resolution));

    if (hadc->Instance == ADC1)
    {
        s_pADCHandle[0] = hadc;
    }
    if (hadc->Instance == ADC2)
    {
        s_pADCHandle[1] = hadc;
    }
    if (hadc->Instance == ADC3)
    {
        s_pADCHandle[2] = hadc;
    }

    HAL_ADC_CLOCK(1);
    HAL_ADC_RESET();

    if (hadc->State == HAL_ADC_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        hadc->Lock = HAL_UNLOCKED;
        /* Init the low level hardware */
        HAL_ADC_MspInit(hadc);
    }

    /* Initialize the ADC state */
    hadc->State = HAL_ADC_STATE_BUSY;

    /* Set ADC parameters */
    ADC_Init(hadc);

    /* Set ADC error code to none */
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;

    /* Initialize the ADC state */
    hadc->State = HAL_ADC_STATE_READY;

    /* Release Lock */
    __HAL_UNLOCK(hadc);

    s_ADCIsr = HAL_ADC_IRQ_handler;

    /* Return function status */
    return HAL_OK;

}

/**
  * @brief  Deinitializes the ADCx peripheral registers to their default reset values.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc)
{
    /* Check ADC handle */
    if (hadc == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_BUSY;

    /* DeInit the low level hardware */
    HAL_ADC_MspDeInit(hadc);

    /* Set ADC error code to none */
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;

    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_RESET;

    /* Return function status */
    return HAL_OK;
}


/**
  * @brief  DeInitializes the ADC MSP.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
__weak void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hadc);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_ADC_MspDeInit could be implemented in the user file
     */
}

/**
  * @brief  Enables the interrupt and starts ADC conversion of regular channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc)
{
    __IO uint32_t counter = 0;

    /* Process locked */
    __HAL_LOCK(hadc);

    /* Check if an injected conversion is ongoing */
    if (hadc->State == HAL_ADC_STATE_BUSY_INJ)
    {
        /* Change ADC state */
        hadc->State = HAL_ADC_STATE_BUSY_INJ_REG;
    }
    else
    {
        /* Change ADC state */
        hadc->State = HAL_ADC_STATE_BUSY_REG;
    }

    /* Set ADC error code to none */
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;

    /* Check if ADC peripheral is disabled in order to enable it and wait during
       Tstab time the ADC's stabilization */
    if ((hadc->Instance->CTRL & ADC_CTRL_EN) != ADC_CTRL_EN)
    {
        /* Enable the Peripheral */
        __HAL_ADC_ENABLE(hadc);

    }

    __HAL_ADC_ENABLE_IT(hadc, ADC_EOS_INTEN);

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    if (hadc->Init.dma_en == ENABLE)
    {
        HAL_ADC_DmaConfig(hadc, &hadc->dma_rst_data[0], hadc->dma_rst_len);
        ADC_CONFIG_DMA_ENABLE(hadc->Instance, ENABLE);
        hadc->expectFinishStatus = HAL_ADC_STATE_DMA_FINISH;
    }

    ADC_InterruptConfig(hadc);

    /* Check if Multimode enabled */
    if (HAL_IS_BIT_CLR(ADC->ADC_MCTRL, ADC_MULTI_MODE))
    {
        HAL_ADC_Triger(hadc->Instance, ADC_SW_REGU_TRIGER);
    }
    else
    {

    }

    hadc->State = HAL_ADC_STATE_RESET;

    /* Return function status */
    return HAL_OK;

}

HAL_StatusTypeDef HAL_ADC_Start_IT_TEST(ADC_HandleTypeDef *hadc)
{
    __IO uint32_t counter = 0;

    /* Process locked */
    __HAL_LOCK(hadc);

    /* Check if an injected conversion is ongoing */
    if (hadc->State == HAL_ADC_STATE_BUSY_INJ)
    {
        /* Change ADC state */
        hadc->State = HAL_ADC_STATE_BUSY_INJ_REG;
    }
    else
    {
        /* Change ADC state */
        hadc->State = HAL_ADC_STATE_BUSY_REG;
    }

    /* Set ADC error code to none */
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;

    /* Check if ADC peripheral is disabled in order to enable it and wait during
       Tstab time the ADC's stabilization */
    if ((hadc->Instance->CTRL & ADC_CTRL_EN) != ADC_CTRL_EN)
    {
        /* Enable the Peripheral */
        //  __HAL_ADC_ENABLE(hadc);

    }

    //__HAL_ADC_ENABLE_IT(hadc, ADC_EOS_INTEN);

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    if (hadc->Init.dma_en == ENABLE)
    {
        //  HAL_ADC_DmaConfig(hadc,  hadc->dma_rst_data[0], hadc->dma_rst_len);
        //  ADC_CONFIG_DMA_ENABLE(hadc->Instance, 1);
        //  hadc->expectFinishStatus = HAL_ADC_STATE_DMA_FINISH;
    }

    //ADC_InterruptConfig(hadc);

    /* Check if Multimode enabled */
    if (HAL_IS_BIT_CLR(ADC->ADC_MCTRL, ADC_MULTI_MODE))
    {
        HAL_ADC_Triger(hadc->Instance, ADC_SW_REGU_TRIGER);
    }
    else
    {

    }


    hadc->State = HAL_ADC_STATE_RESET;

    /* Return function status */
    return HAL_OK;
}


/**
  * @brief  Disables the interrupt and stop ADC conversion of regular channels.
  *
  * @note   Caution: This function will stop also injected channels.
  *
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc)
{
    /* Disable the ADC end of conversion interrupt for regular group */
    __HAL_ADC_DISABLE_IT(hadc, ADC_EOC_INTEN);

    /* Disable the ADC end of conversion interrupt for injected group */
    __HAL_ADC_DISABLE_IT(hadc, ADC_JEOC_INTEN);

    /* Enable the Peripheral */
    __HAL_ADC_DISABLE(hadc);

    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_READY;

    /* Return function status */
    return HAL_OK;
}

/**
  * @brief  Enables the interrupt and starts ADC conversion of injected channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  *
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef *hadc)
{
    __IO uint32_t counter = 0;

    /* Process locked */
    __HAL_LOCK(hadc);

    /* Check if a regular conversion is ongoing */
    if (hadc->State == HAL_ADC_STATE_BUSY_REG)
    {
        /* Change ADC state */
        hadc->State = HAL_ADC_STATE_BUSY_INJ_REG;
    }
    else
    {
        /* Change ADC state */
        hadc->State = HAL_ADC_STATE_BUSY_INJ;
    }

    /* Set ADC error code to none */
    hadc->ErrorCode = HAL_ADC_ERROR_NONE;

    /* Check if ADC peripheral is disabled in order to enable it and wait during
       Tstab time the ADC's stabilization */
    if ((hadc->Instance->CTRL & ADC_CTRL_EN) != ADC_CTRL_EN)
    {
        /* Enable the Peripheral */
        __HAL_ADC_ENABLE(hadc);

    }

    /* Enable the ADC end of conversion interrupt for injected group */
    __HAL_ADC_ENABLE_IT(hadc, ADC_JEOC_INTEN);


    /* Check if Multimode enabled */
    if (HAL_IS_BIT_CLR(ADC->ADC_MCTRL, ADC_MULTI_MODE))
    {

        HAL_ADC_Triger(hadc->Instance, ADC_SW_JTRIG_TRIGER);

    }
    else
    {

    }

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;
}

/**
  * @brief  Disables the interrupt and stop ADC conversion of injected channels.
  *
  * @note   Caution: This function will stop also regular channels.
  *
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef *hadc)
{
    /* Disable the ADC end of conversion interrupt for regular group */
    __HAL_ADC_DISABLE_IT(hadc, ADC_EOC_INTEN);

    /* Disable the ADC end of conversion interrupt for injected group */
    __HAL_ADC_DISABLE_IT(hadc, ADC_JEOC_INTEN);

    /* Enable the Peripheral */
    __HAL_ADC_DISABLE(hadc);

    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_READY;

    /* Return function status */
    return HAL_OK;
}

/**
  * @brief  Gets the converted value from data register of injected channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  InjectedRank: the ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg ADC_INJECTED_RANK_0: Injected Channel0 selected
  *            @arg ADC_INJECTED_RANK_1: Injected Channel1 selected
  *            @arg ADC_INJECTED_RANK_2: Injected Channel2 selected
  *            @arg ADC_INJECTED_RANK_3: Injected Channel3 selected
  * @retval None
  */
uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef *hadc, uint32_t InjectedRank)
{
    __IO uint32_t tmp = 0;

    /* Check the parameters */
    //assert_param(IS_ADC_INJECTED_RANK(InjectedRank));

    /* Clear the ADCx's flag for injected end of conversion */
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_ST_JEOC_INT);

    /* Return the selected ADC converted value */
    switch (InjectedRank)
    {
        case ADC_INJECTED_RANK_0:
        {
            tmp =  hadc->Instance->JDRST[0];
        }
        break;
        case ADC_INJECTED_RANK_1:
        {
            tmp =  hadc->Instance->JDRST[1];
        }
        break;
        case ADC_INJECTED_RANK_2:
        {
            tmp =  hadc->Instance->JDRST[2];
        }
        break;
        case ADC_INJECTED_RANK_3:
        {
            tmp =  hadc->Instance->JDRST[3];
        }
        break;
        default:
            break;
    }
    return tmp;
}

/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfigInjected: ADC configuration structure for injected channel.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected)
{
    int jchn_sel = 0;

    /* Check the parameters */
    assert_param(IS_ADC_CHANNEL(sConfigInjected->InjectedChannel));
    assert_param(IS_ADC_SAMPLE_TIME(sConfigInjected->InjectedSamplingTime));
    assert_param(IS_ADC_INJECTED_LENGTH(sConfigInjected->InjectedNbrOfConversion));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->AutoInjectedConv));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->InjectedDiscontinuousConvMode));

    /* Process locked */
    __HAL_LOCK(hadc);

    if (sConfigInjected->InjectedChannel < 8)
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[0] &= ~ADC_SAMPTM0(0xF, sConfigInjected->InjectedChannel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[0] |= ADC_SAMPTM0(sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);
    }
    else if (sConfigInjected->InjectedChannel < 16)
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[1] &= ~ADC_SAMPTM1(0xF, sConfigInjected->InjectedChannel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[1] |= ADC_SAMPTM1(sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);
    }
    else if (sConfigInjected->InjectedChannel < 24)
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[2] &= ~ADC_SAMPTM2(0xF, sConfigInjected->InjectedChannel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[2] |= ADC_SAMPTM2(sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);
    }
    else
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[3] &= ~ADC_SAMPTM3(0xF, sConfigInjected->InjectedChannel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[3] |= ADC_SAMPTM3(sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);
    }

    /*---------------------------- ADCx JSQR Configuration -----------------*/
    hadc->Instance->JCHN &= ~(ADCX_JSL);
    hadc->Instance->JCHN |=  ADC_JSLEN((sConfigInjected->InjectedNbrOfConversion - 1));

    /* Rank configuration */


    if (sConfigInjected->AutoInjectedConv != DISABLE)
    {
        /* Enable the selected ADC automatic injected group conversion */
        hadc->Instance->CTRL |= ADC_JAUTO_EN;
    }
    else
    {
        /* Disable the selected ADC automatic injected group conversion */
        hadc->Instance->CTRL &= ~(ADC_JAUTO_EN);
    }

    if (sConfigInjected->InjectedDiscontinuousConvMode != DISABLE)
    {
        /* Enable the selected ADC injected discontinuous mode */
        hadc->Instance->CTRL |= ADC_JDCONT_EN;
    }
    else
    {
        /* Disable the selected ADC injected discontinuous mode */
        hadc->Instance->CTRL &= ~(ADC_JDCONT_EN);
    }

    for (jchn_sel = 0; jchn_sel < sConfigInjected->InjectedNbrOfConversion; jchn_sel++)
    {
        if (jchn_sel == 0)
        {
            hadc->Instance->JCHN |=  sConfigInjected->InjectedChannel << ADCX_JCHN0_SEL_POS;
        }

        if (jchn_sel == 1)
        {
            hadc->Instance->JCHN |=  sConfigInjected->InjectedChannel << ADCX_JCHN1_SEL_POS;
        }
        if (jchn_sel == 2)
        {
            hadc->Instance->JCHN |=  sConfigInjected->InjectedChannel << ADCX_JCHN2_SEL_POS;
        }
        if (jchn_sel == 3)
        {
            hadc->Instance->JCHN |=  sConfigInjected->InjectedChannel << ADCX_JCHN3_SEL_POS;
        }

    }

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig)
{
    __IO uint32_t counter = 0;
    int i = 0;

    ADC_ChannelConfTypeDef *mConfig = sConfig;

    /* Check the parameters */
    assert_param(IS_ADC_CHANNEL(sConfig->Channel));
    //assert_param(IS_ADC_REGULAR_RANK(sConfig->Rank));
    assert_param(IS_ADC_SAMPLE_TIME(sConfig->SamplingTime));
    assert_param(IS_ADC_REGULAR_SEQ_MAX_LEN(hadc->Init.NbrOfConversion));


    /* Process locked */
    __HAL_LOCK(hadc);

    if (sConfig->Channel < 8)
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[0] &= ~ADC_SAMPTM0(0xF, sConfig->Channel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[0] |= ADC_SAMPTM0(sConfig->SamplingTime, sConfig->Channel);
    }
    else if (sConfig->Channel < 16)
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[1] &= ~ADC_SAMPTM1(0xF, sConfig->Channel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[1] |= ADC_SAMPTM1(sConfig->SamplingTime, sConfig->Channel);
    }
    else if (sConfig->Channel < 24)
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[2] &= ~ADC_SAMPTM2(0xF, sConfig->Channel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[2] |= ADC_SAMPTM2(sConfig->SamplingTime, sConfig->Channel);
    }
    else
    {
        /* Clear the old sample time */
        hadc->Instance->SAMPTM[3] &= ~ADC_SAMPTM3(0xF, sConfig->Channel);

        /* Set the new sample time */
        hadc->Instance->SAMPTM[3] |= ADC_SAMPTM3(sConfig->SamplingTime, sConfig->Channel);
    }

    /* For Regular channel sequence length 0 to 5 */

    for (i = 0; i < hadc->Init.NbrOfConversion; i++)
    {
        if (i < 6)
        {
            /* For Regular channel sequence length 0 to 5 */
            /* Clear the old SQx bits for the selected rank */
            hadc->Instance->RCHN0 &= ~(ADC_SQR0_RK(0x1F, i));

            /* Set the SQx bits for the selected rank */
            hadc->Instance->RCHN0 |= ADC_SQR0_RK(mConfig->Channel, i);
        }
        /* For Rank 6 to 11 */
        else if (i < 12)
        {
            /* Clear the old SQx bits for the selected rank */
            hadc->Instance->RCHN1 &= ~(ADC_SQR1_RK(0x1F, i));

            /* Set the SQx bits for the selected rank */
            hadc->Instance->RCHN1 |= ADC_SQR1_RK(mConfig->Channel, i);
        }
        else if (i < 16)
        {
            /* Clear the old SQx bits for the selected rank */
            hadc->Instance->RCHN2 &= ~(ADC_SQR2_RK(0x1F, i));

            /* Set the SQx bits for the selected rank */
            hadc->Instance->RCHN2 |= ADC_SQR2_RK(mConfig->Channel, i);
        }
        else
        {

        }
        mConfig++;
    }

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;
}

RET_CODE HAL_ADC_DmaConfig(ADC_HandleTypeDef *hADC, uint32_t *pConvertData, uint8_t length)
{
    RET_CODE ret = RET_OK;
    DMA_Request_Result_T reqResult;
    DMA_Config_T dmaConfig;
    DMA_Config_T *pDMAConfig  = &dmaConfig;
    ADC_Device_T *pDevice     = hADC->Instance;
    unsigned int adc_rst_data_reg = ADC1_READ_FIFO_PHY_ADDRESS;

    /* Data fifo have one data will triger dma to transfer */
    ADC_DFIFO_PREFULL_NUM(pDevice, ADC_DFIFO_PRE_FULL_NUM_1);

    if (ADC1 == (ADC_Device_T *)hADC->Instance)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_ADC1, &reqResult);
        adc_rst_data_reg = ADC1_READ_FIFO_PHY_ADDRESS;
    }
    else if (ADC2 == (ADC_Device_T *)hADC->Instance)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_ADC2, &reqResult);
        adc_rst_data_reg = ADC2_READ_FIFO_PHY_ADDRESS;
    }
    else if (ADC3 == (ADC_Device_T *)hADC->Instance)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_ADC3, &reqResult);
        adc_rst_data_reg = ADC3_READ_FIFO_PHY_ADDRESS;
    }

    if (ret != RET_OK)
    {
        printf("%s error: %d\n", __FUNCTION__, __LINE__);
        return RET_ERROR;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
    dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    dmaConfig.width = DMA_WIDTH_32BIT;
    dmaConfig.burst = 0;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PRIMARY;
    pDMAConfig->callback = ADC_DmaCallback;

    if ((ret = HAL_DMA_Init(DMA_DEV, pDMAConfig, &s_DMAHandle)) != RET_OK)
    {
        printf("%s error: %d\n", __FUNCTION__, __LINE__);
        return ret;
    }
    LINK_DMAHDL_TO_PARENTHDL(hADC, pDMAHandle, s_DMAHandle);

    ret = HAL_DMA_Start(hADC->pDMAHandle, (uint32_t)(adc_rst_data_reg), \
                        (uint32_t)(pConvertData), length);
    if (ret != RET_OK)
    {
        printf("%s error: %d\n", __FUNCTION__, __LINE__);
    }

    return ret;
}

HAL_StatusTypeDef HAL_ADC_ConfigCompChannel(ADC_HandleTypeDef *hadc, ADC_CompareConfig_T *pcompConfig)
{
    __HAL_LOCK(hadc);

    if (hadc->Init.compareEn == ENABLE)
    {
        ADC_SET_COMPARE_VALUE(hadc->Instance, pcompConfig->compareLowValue, pcompConfig->compareHighValue);
        ADC_SET_COMPARE_MODE(hadc->Instance, pcompConfig->compareMode);
        hadc->Instance->CTRL =  ADC_COMPARE_ENABLE(hadc->Instance);
    }
    else
    {
        hadc->Instance->CTRL = ADC_COMPARE_DISABLE(hadc->Instance);
    }

    if (hadc->Init.compareSingleEn == ENABLE)
    {
        hadc->Instance->CTRL = ADC_COMPARE_SINGLE_ENABLE(hadc->Instance);
        hadc->Instance->COM_CHN = ADC_AUTOCOMP_CHN_SELECT(pcompConfig->autoCompChSelect);
    }
    else
    {
        hadc->Instance->CTRL = ADC_COMPARE_SINGLE_DISABLE(hadc->Instance);
    }

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;

}

HAL_ADC_StateTypeDef HAL_ADC_GetState(ADC_HandleTypeDef *hadc)
{
    /* Return ADC state */
    return hadc->State;
}

/**
  * @brief  Return the ADC error code
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval ADC Error Code
  */
uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc)
{
    return hadc->ErrorCode;
}

static uint32_t ADC_ConvertToResolutionValue(unsigned char resolution, uint32_t data)
{
    if (resolution == ADC_RESOLUTION_10BIT)
    {
        data = data & 0x3FF;
    }
    else if (resolution == ADC_RESOLUTION_8BIT)
    {
        data = data & 0xFF;
    }

    return data;
}

/*cpu read fifo convert*/
HAL_StatusTypeDef  HAL_ADC_FifoConvertValue(ADC_HandleTypeDef *hadc, unsigned char  resolution, uint32_t *pConvertData, uint8_t length)
{
    uint8_t i = 0 ;
    uint32_t val = 0;

    for (i = 0; i < length; i++)
    {
        /*only low 12bit vaild data*/
        val = (hadc->Instance->DRST & 0xfff);
        pConvertData[i] = ADC_ConvertToResolutionValue(resolution, val);
        printf("convert_value[%d]=%d\n", i,  pConvertData[i]);
    }

    return  HAL_OK;
}

/**
  * @brief  Gets the converted value from data register of regular channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval Converted value
  */
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    /* Return the selected ADC converted value */
    return hadc->Instance->DRST;
}

static RET_CODE ADC_Stop(ADC_HandleTypeDef *hADC)
{
    RET_CODE ret = HAL_OK;

    if (ADC_TryLock(hADC) != HAL_OK)
    {
        return RET_BUSY;
    }

    /* disabel adc */
    ADC_DISABLE(hADC->Instance);
    __HAL_UNLOCK(hADC);

    return ret;
}

void ADC1_InterruptHandler(void)
{
    s_ADCIsr(s_pADCHandle[0]);
}

void ADC2_InterruptHandler(void)
{
    s_ADCIsr(s_pADCHandle[1]);
}

void ADC3_InterruptHandler(void)
{
    s_ADCIsr(s_pADCHandle[2]);
}

void HAL_ADC_CLOCK(int en)
{
    if (en == 0)
    {
        *(volatile unsigned int *)(0x40000080) &= ~(1 << 12); //disabled the ADC clock
    }
    else
    {
        *(volatile unsigned int *)(0x40000080) |= 1 << 12; //enable (release) the ADC clock
    }
}

void HAL_ADC_RESET(void)
{
    int delay = 0;

    *(volatile unsigned int *)(0x40000070) &= ~(1 << 12);
    while (delay++ < 10000);
    *(volatile unsigned int *)(0x40000070) |= 1 << 12; //release the ADC reset
}

RET_CODE ADC_TryLock(ADC_HandleTypeDef *hADC)
{
    if (hADC->Lock == HAL_LOCKED)
    {
        return RET_BUSY;
    }
    else
    {
        hADC->Lock = HAL_LOCKED;
        return RET_OK;
    }
}
