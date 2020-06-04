/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_adc.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_hal_adc.h>
#ifdef CONFIG_GM_HAL_PES
    #include "gm_hal_pes.h"
    #include "gm_hal_rtc.h"
#endif
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>
#include <gm_hal_pes.h>

#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

static ADC_HandleTypeDef *s_pADCHandle[MAX_ADC_DEVICE] = {NULL};

static DMA_Handle_T s_DMA_Handle;
//static HAL_ADC_StateTypeDef ADC_Stop(ADC_HandleTypeDef *hADC);
#ifdef CONFIG_GM_HAL_PES
    PES_ChannelIndex_T HAL_PES_RequestEventChannel();
#endif
void HAL_ADC_BufConvertValue(ADC_HandleTypeDef *hadc, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length);

RET_CODE ADC_TryLock(ADC_HandleTypeDef *hADC);
static void ADC_DmaCallback(struct DMA_Handle *pDMAHandle, uint8_t event, unsigned long param);

ADC_HandleTypeDef *g_hADC;

void HAL_RTC_AlarmPESEnable(void);


RET_CODE ADC_TryLock(ADC_HandleTypeDef *hADC)
{
    if (hADC->Lock == HAL_ADC_LOCKED)
    {
        return RET_BUSY;
    }
    else
    {
        hADC->Lock = HAL_ADC_LOCKED;
        return RET_OK;
    }
}

static void ADC_InterruptConfig(ADC_HandleTypeDef *hADC)
{

#ifdef CONFIG_USE_HAL_TEST
    /*make sure overfow match eos interrupt is enable */
    ADC_INTERRUPT_ENABLE(hADC->device, ADC_COMP_MAT_INTEN  | \
                         ADC_DFIFO_OVF_INTEN | ADC_EOS_INTEN | ADC_JEOS_INTEN);
#endif

#ifdef RABBIT_NO_OS_SYSTEM
    /*make sure overfow match eos interrupt is enable */
    ADC_INTERRUPT_ENABLE(hADC->device, ADC_COMP_MAT_INTEN  | \
                         ADC_DFIFO_OVF_INTEN | ADC_EOS_INTEN | ADC_JEOS_INTEN);
#endif

#ifdef RABBIT_NO_OS_SYSTEM
    if (hADC->device == ADC_DEV0)
    {
        NVIC_EnableIRQ(ADC0_IRQn);
    }
    if (hADC->device == ADC_DEV1)
    {
        NVIC_EnableIRQ(ADC1_IRQn);
    }
    if (hADC->device == ADC_DEV2)
    {
        NVIC_EnableIRQ(ADC2_IRQn);
    }
#endif
}

static void ADC_DmaCallback(struct DMA_Handle *pDMAHandle, uint8_t event, unsigned long param)
{

    ADC_HandleTypeDef *hADC = GET_PARENTHDL_FROM_DMAHDL(pDMAHandle, ADC_HandleTypeDef);
    unsigned char  len = hADC->Init.NbrOfConversion;

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        if (hADC->blockStatus == BLOCK)
        {
            hADC->State = HAL_ADC_STATE_DMA_FINISH;
        }
        else
        {
            HAL_ADC_BufConvertValue(hADC, hADC->Init.Resolution,  &hADC->rst_data[0], len);
            hADC->State = HAL_ADC_STATE_IDLE;
        }
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
    unsigned char  len = hADC->Init.NbrOfConversion;
    unsigned char resolution = hADC->Init.Resolution;
    /*uint32_t convert_value[SEQUECE_LEN_MAX] = {0};*/

    /*ADC_Device_T *pDevice = (ADC_Device_T *)hADC->device;*/
    /*PR_DEBUG("adc_irq_handler ...pDevice->ST =0x%x, pDevice->IE = 0x%x\n", pDevice->ST, pDevice->IE);*/

    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_EOC_INT) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_EOC_INTEN))
    {

        PR_DEBUG("ADC_ST_EOC_INT\n");
        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_EOC_INT);
        HAL_ADC_FifoConvertValue(hADC, resolution, &hADC->rst_data[0], len);
    }

    /* cpu mode need send event by irqstatus */
    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_DOFI) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_DFIFO_OVF_INTEN))
    {
        PR_DEBUG("ADC_ST_DOFI\n");
        /* clear overflower irq and sent this event */
        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_DOFI);
    }

    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_CMI) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_COMP_MAT_INTEN))
    {
        PR_DEBUG("ADC_ST_CMI\n");
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

            __HAL_ADC_CLEAR_FLAG(hADC, 0xFFFFFFFF);

            return ;
        }
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine status*/
            hADC->State = HAL_ADC_STATE_CPU_FINISH;
        }
        else
        {
            HAL_ADC_FifoConvertValue(hADC, resolution, &hADC->rst_data[0], len);
            /* no block send convert finish event to usr */

            hADC->State = HAL_ADC_STATE_IDLE;
        }
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

            __HAL_ADC_CLEAR_FLAG(hADC, 0xFFFFFFFF);

            return ;
        }
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine status*/
            hADC->State = HAL_ADC_STATE_CPU_FINISH;
        }
        else
        {
            HAL_ADC_FifoConvertValue(hADC, resolution, &hADC->rst_data[0], len);
            hADC->State = HAL_ADC_STATE_IDLE;
        }
    }

    /*JEOS*/
    if (__HAL_ADC_GET_FLAG(hADC, ADC_ST_JEOS_INT) && \
            __HAL_ADC_GET_IT_SOURCE(hADC, ADC_JEOS_INTEN))
    {
        /* clear eos irq */
        __HAL_ADC_CLEAR_FLAG(hADC, ADC_ST_JEOS_INT);


        if (hADC->expectFinishStatus == HAL_ADC_STATE_DMA_FINISH)
        {
            /*
             * DMA sync Machine will process at ADC_DmaCallback
             * so here only clear irq and return
             */

            __HAL_ADC_CLEAR_FLAG(hADC, 0xFFFFFFFF);

            return ;
        }


        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine status*/
            hADC->State = HAL_ADC_STATE_CPU_FINISH;
        }
        else
        {

            hADC->State = HAL_ADC_STATE_IDLE;
        }
    }


    __HAL_ADC_CLEAR_FLAG(hADC, 0xFFFFFFFF);

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

void HAL_ADC_Triger(ADC_Device_T *pADC, ADC_TrigerChannel_T trigerchannel, ADC_TrigerMode_T trigermode, ADC_TrigerSource_T trigersel)
{
    if (pADC !=  NULL)
    {
        if (trigermode == ADC_HW_TRIGER)
        {
            ADC_COFNIG_HW_TRIGER_SOUTCE(pADC, trigersel);
            /*enable hw triger mode*/
            ADC_COFNIG_HW_TRIGER_EN(pADC, ENABLE);

        }
        else /*ADC_SW_TRIGER*/
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
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct without initializing the ADC MSP.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
RET_CODE HAL_ADC_Init(ADC_HandleTypeDef *hadc)
{
    /* Check ADC handle */
    if (hadc == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_ADC_ALL_INSTANCE(hadc->device));
    assert_param(IS_ADC_RESOLUTION(hadc->Init.Resolution));

#if defined(RABBIT_NO_OS_SYSTEM) || defined(CONFIG_USE_HAL_TEST)
    if (hadc->device == ADC_DEV0)
    {
        s_pADCHandle[0] = hadc;
    }
    if (hadc->device == ADC_DEV1)
    {
        s_pADCHandle[1] = hadc;
    }
    if (hadc->device == ADC_DEV2)
    {
        s_pADCHandle[2] = hadc;
    }

    g_hADC = hadc;
#endif

    HAL_ADC_SW_Reset();
    HAL_ADC_CLK_ENABLE(3);

    if (hadc->State == HAL_ADC_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        hadc->Lock = HAL_ADC_UNLOCKED;
    }

    /* Initialize the ADC state */
    hadc->State = HAL_ADC_STATE_BUSY;


    /* Set ADC parameters */
    /* Set ADC work(scan) mode */
    hadc->device->CTRL &= ~(ADC_CTRL_WM);
    hadc->device->CTRL |=  ADC_CTRL_WORKMODE(hadc->Init.workMode);

    /* Set ADC resolution */
    hadc->device->CTRL &= ~(ADC_CTRL_BNIT);
    hadc->device->CTRL |=  ADC_CTRL_RESOLUTION(hadc->Init.Resolution);

    /* Set ADC lowpower mode */
    hadc->device->CTRL &= ~(ADC_CTRL_LPEN);

    if (hadc->Init.lowpowerEn == ENABLE)
    {
        hadc->device->CTRL |=  ADC_CTRL_LOWPOWER(ENABLE);
    }
    else
    {
        hadc->device->CTRL |=  ADC_CTRL_LOWPOWER(DISABLE);
    }

    /* Set ADC Discontinuous mode on regular channels*/
    hadc->device->CTRL &= ~(ADC_CTRL_DCONTEN);

    if (hadc->Init.discontinuousConvEn == ENABLE)
    {
        hadc->device->CTRL |=  ADC_CTRL_DISCONTINUOUS(ENABLE);
    }
    else
    {
        hadc->device->CTRL |=  ADC_CTRL_DISCONTINUOUS(DISABLE);
    }

    /* Set ADC number of conversion */
    hadc->device->RCHN2 &= ~(ADC_RCHN2_RSL);
    hadc->device->RCHN2 |=  ADC_RCHN_L(hadc->Init.NbrOfConversion);

    /* Initialize the ADC state */
    hadc->State = HAL_ADC_STATE_READY;

    /* Release Lock */
    __HAL_UNLOCK(hadc);

    HAL_ADC_RequestIrq(3);

    ADC_InterruptConfig(hadc);
    /* Return function status */
    return HAL_OK;

}

/**
  * @brief  Deinitializes the ADCx peripheral registers to their default reset values.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status
  */
RET_CODE HAL_ADC_UnInit(ADC_HandleTypeDef *hadc)
{
    /* Check ADC handle */
    if (hadc == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    assert_param(IS_ADC_ALL_INSTANCE(hadc->device));

    /* Change ADC state */
    hadc->State = HAL_ADC_STATE_RESET;

    /* Return function status */
    return HAL_OK;
}

static void HAL_ADC_PesTrigerSrcSelect(PES_MasterSourceConfig_T masterSource, PES_ChannelIndex_T s_chIndex)
{
#ifdef CONFIG_GM_HAL_PES
    static int i = 0;
    if (masterSource == PES_MASTER_RTCC)
    {
        if (i == 0)
        {
            /*Rtc As Master generate Event*/
            HAL_RTC_AlarmPESEnable();
            UTCTime_T utcTime;

#define YEAR    2017
#define MON     5
#define DAY     11
#define HOUR    14
#define MINUTE  7
#define SEC     0

            HAL_RTC_SetDateTime(2017, MON, DAY, HOUR, MINUTE, SEC);
            memset(&utcTime, 0, sizeof(UTCTime_T));
            utcTime.year = YEAR;
            utcTime.mon  = MON;
            utcTime.day  = DAY;
            utcTime.hour = HOUR;
            utcTime.min  = MINUTE;
            utcTime.sec  = SEC + 2; /* ensure sec < 60 */

            HAL_RTC_SetAlarm(&utcTime);

            i = 1;
        }
    }
    else if (masterSource == PES_MASTER_CPU)
    {
        /*Cpu As Master generate Event*/
        HAL_PES_GenerateSWTrigerEvent(PES_PULSE, (PES_ChannelIndex_T)s_chIndex);
    }
    else
    {
        printf("Warning ,you need select pes triger source\n");
    }
#endif
}

#ifdef CONFIG_GM_HAL_PES
static void rtc_callback(int a)
{
#ifdef CONFIG_GM_HAL_PES
    UTCTime_T *utcTime = HAL_RTC_GetTime();
    printf("rtc_callback");
    utcTime->sec +=  1; /* ensure sec < 60 */
    if (utcTime->sec >= 60)
    {
        utcTime->sec = 0;
        utcTime->min += 1;
        if (utcTime->min >= 60)
        {
            utcTime->hour += 1;
        }
    }
    HAL_RTC_CancelAlarm();
    HAL_RTC_SetAlarm(utcTime);
#endif
}
#endif

#ifdef CONFIG_GM_HAL_PES
static RET_CODE HAL_ADC_AsSlaveConfigPes(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

#ifdef CONFIG_GM_HAL_PES
    HAL_RTC_Config_T rtcConfig;

    rtcConfig.clock = HAL_RTC_CLOCK_LIRC;//HAL_RTC_CLOCK_LIRC ;
    rtcConfig.alarmCallback = rtc_callback;
    rtcConfig.timerCallback = rtc_callback;
    if (pchCfg->masterSrcConfig  == PES_MASTER_RTCC)
    {
        HAL_RTC_Init(&rtcConfig);
        HAL_RTC_Start();
        UTCTime_T utcTime;

#define YEAR    2017
#define MON     5
#define DAY     11
#define HOUR    14
#define MINUTE  7
#define SEC     0

        HAL_RTC_SetDateTime(2017, MON, DAY, HOUR, MINUTE, SEC);

        memset(&utcTime, 0, sizeof(UTCTime_T));
        utcTime.year = YEAR;
        utcTime.mon  = MON;
        utcTime.day  = DAY;
        utcTime.hour = HOUR;
        utcTime.min  = MINUTE;
        utcTime.sec  = SEC + 1; /* ensure sec < 60 */

        HAL_RTC_SetAlarm(&utcTime);

        ret = HAL_PES_ConfigEventChannel(pchCfg, chIndex);
        if (ret != RET_OK)
        {
            printf("%s:Err:PES_ConfigEventChannel\n", __FUNCTION__);
        }
    }


#endif

    return ret;
}
#endif

PES_ChannelIndex_T HAL_ADC_PES_Setting(ADC_HandleTypeDef *hadc, PES_MasterSourceConfig_T masterSource)
{
    PES_ChannelConfig_T  chCfg;
    PES_ChannelIndex_T s_chIndex = PES_EVENT_CHANNEL0;

    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    chCfg.masterSrcConfig = masterSource;
    chCfg.syncMode = PES_SYNC_RISING_EDGE;

    if (hadc->Init.trigermode == ADC_HW_TRIGER)
    {
#ifdef CONFIG_GM_HAL_PES
        s_chIndex = HAL_PES_RequestEventChannel();
        if (PES_INVAILD_CHANNEL == s_chIndex)
        {
            printf("%s:Err:PES_RequestEventChannel\n", __FUNCTION__);
            return PES_INVAILD_CHANNEL;
        }
        HAL_ADC_AsSlaveConfigPes(&chCfg, (PES_ChannelIndex_T)s_chIndex);
#endif
    }

    return s_chIndex;
}

/**
  * @brief  Enables the interrupt and starts ADC conversion of regular channels.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc)
{
    HAL_StatusTypeDef ret = HAL_OK;
    PES_ChannelIndex_T chIndex = PES_EVENT_CHANNEL0;

    PES_MasterSourceConfig_T masterSource = PES_MASTER_RTCC;


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

    /* Enable the Peripheral */
    LL_ADC_ENABLE(hadc);

    LL_ADC_ENABLE_IT(hadc, ADC_EOS_INTEN | ADC_DFIFO_OVF_INTEN);

    //LL_ADC_ENABLE_IT(hadc, ADC_EOC_INTEN|ADC_DFIFO_OVF_INTEN);
    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    if (hadc->Init.trigermode == ADC_HW_TRIGER)
    {
        chIndex = HAL_ADC_PES_Setting(hadc, masterSource);
    }

    hadc->State = HAL_ADC_STATE_RESET;
    /* Check if Multimode enabled */
    if ((ADC_COM_DEV->ADC_MCTRL & ADC_MULTI_MODE) == ADC_INDPENDENT_MODE)
    {
        if (hadc->Init.dma_en == ENABLE)
        {
            ADC_CONFIG_DMA_ENABLE(hadc->device, 1);
            HAL_ADC_DmaConfig(hadc, &(hadc->rst_data[0]), hadc->dma_rst_len);
        }

        HAL_ADC_Triger(hadc->device, ADC_SW_REGU_TRIGER, hadc->Init.trigermode, (ADC_TrigerSource_T)chIndex /*hadc->Init.trigersel*/);
        if (hadc->Init.trigermode == ADC_HW_TRIGER)
        {
            HAL_ADC_PesTrigerSrcSelect(masterSource, chIndex);

        }
    }
    else
    {

    }


    /* Return function status */
    return ret ;

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

    /* Check if ADC peripheral is disabled in order to enable it and wait during
     Tstab time the ADC's stabilization */

    /* Enable the Peripheral */
    LL_ADC_ENABLE(hadc);

    /* Enable the ADC end of conversion interrupt for injected group */
    /*LL_ADC_ENABLE_IT(hadc, ADC_JEOC_INTEN);*/

    /* Check if Multimode enabled */
    if ((ADC_COM_DEV->ADC_MCTRL & ADC_MULTI_MODE) == ADC_INDPENDENT_MODE)
    {
        HAL_ADC_Triger(hadc->device, ADC_SW_JTRIG_TRIGER, hadc->Init.trigermode, hadc->Init.trigersel);
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
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfigInjected: ADC configuration structure for injected channel.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected)
{
    /* Check the parameters */
    assert_param(IS_ADC_SAMPLE_TIME(sConfigInjected->InjectedSamplingTime));
    assert_param(IS_ADC_INJECTED_LENGTH(sConfigInjected->InjectedNbrOfConversion));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->AutoInjectedConv));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->InjectedDiscontinuousConvMode));

    /* Process locked */
    __HAL_LOCK(hadc);


    if (sConfigInjected->injectchn_sel[0] < 4)
    {
        /* Clear the old sample time */
        hadc->device->SAMPTM[0] &= ~ ADC_SAMPTM0(0xF, sConfigInjected->injectchn_sel[0]);

        /* Set the new sample time */
        hadc->device->SAMPTM[0] |= ADC_SAMPTM0(sConfigInjected->InjectedSamplingTime, sConfigInjected->injectchn_sel[0]);
    }

    /*---------------------------- ADCx JSQR Configuration -----------------*/
    hadc->device->JCHN &= ~(ADCX_JSL);
    hadc->device->JCHN |=  ADC_JSLEN((sConfigInjected->InjectedNbrOfConversion - 1));
    /* Rank configuration */
    if (sConfigInjected->AutoInjectedConv == ENABLE)
    {
        /* Enable the selected ADC automatic injected group conversion */
        hadc->device->CTRL |= ADC_JAUTO_EN;
    }
    else
    {
        /* Disable the selected ADC automatic injected group conversion */
        hadc->device->CTRL &= ~(ADC_JAUTO_EN);
    }

    if (sConfigInjected->InjectedDiscontinuousConvMode == ENABLE)
    {
        /* Enable the selected ADC injected discontinuous mode */
        hadc->device->CTRL |= ADC_JDCONT_EN;
    }
    else
    {
        /* Disable the selected ADC injected discontinuous mode */
        hadc->device->CTRL &= ~(ADC_JDCONT_EN);
    }

    hadc->device->JCHN &= ~(ADCX_JCHN0_SEL);
    hadc->device->JCHN |= sConfigInjected->injectchn_sel[0] & (ADCX_JCHN0_SEL);

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;
}

/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  sConfigInjected: ADC configuration structure for injected channel.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedSequenceChannel(ADC_HandleTypeDef *hadc, ADC_InjectionConfTypeDef *sConfigInjected)
{
    int i = 0;

    /* Check the parameters */
    assert_param(IS_ADC_SAMPLE_TIME(sConfigInjected->InjectedSamplingTime));
    assert_param(IS_ADC_INJECTED_LENGTH(sConfigInjected->InjectedNbrOfConversion));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->AutoInjectedConv));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->InjectedDiscontinuousConvMode));

    /* Process locked */
    __HAL_LOCK(hadc);

    for (i = 0; i < sConfigInjected->InjectedNbrOfConversion; i++)
    {
        if (sConfigInjected->injectchn_sel[i] < 4)
        {
            hadc->device->SAMPTM[0] &= ~ ADC_SAMPTM0(0xF, sConfigInjected->injectchn_sel[i]);

            hadc->device->SAMPTM[0] |= ADC_SAMPTM0(sConfigInjected->InjectedSamplingTime, sConfigInjected->injectchn_sel[i]);
        }
    }

    /*---------------------------- ADCx JSQR Configuration -----------------*/
    hadc->device->JCHN &= ~(ADCX_JSL);
    hadc->device->JCHN |=  ADC_JSLEN((sConfigInjected->InjectedNbrOfConversion - 1));

    if (sConfigInjected->AutoInjectedConv == ENABLE)
    {
        /* Enable the selected ADC automatic injected group conversion */
        hadc->device->CTRL |= ADC_JAUTO_EN;
    }
    else
    {
        /* Disable the selected ADC automatic injected group conversion */
        hadc->device->CTRL &= ~(ADC_JAUTO_EN);
    }

    if (sConfigInjected->InjectedDiscontinuousConvMode == ENABLE)
    {
        /* Enable the selected ADC injected discontinuous mode */
        hadc->device->CTRL |= ADC_JDCONT_EN;
    }
    else
    {
        /* Disable the selected ADC injected discontinuous mode */
        hadc->device->CTRL &= ~(ADC_JDCONT_EN);
    }

    for (i = 0; i < sConfigInjected->InjectedNbrOfConversion; i++)
    {
        if (i == 0)
        {
            hadc->device->JCHN &= ~(ADCX_JCHN0_SEL);
            hadc->device->JCHN |= sConfigInjected->injectchn_sel[i] & (ADCX_JCHN0_SEL);
        }
        else if (i == 1)
        {
            hadc->device->JCHN &= ~(ADCX_JCHN1_SEL);
            hadc->device->JCHN |= (sConfigInjected->injectchn_sel[i] << ADCX_JCHN1_SEL_POS) & (ADCX_JCHN1_SEL);
        }
        else if (i == 2)
        {
            hadc->device->JCHN &= ~(ADCX_JCHN2_SEL);
            hadc->device->JCHN |= (sConfigInjected->injectchn_sel[i] << ADCX_JCHN2_SEL_POS) & (ADCX_JCHN2_SEL);
        }
        else
        {
            hadc->device->JCHN &= ~(ADCX_JCHN3_SEL);
            hadc->device->JCHN |= (sConfigInjected->injectchn_sel[i] << ADCX_JCHN3_SEL_POS) & (ADCX_JCHN3_SEL);
        }
    }

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig)
{
    /* Check the parameters */
    assert_param(IS_ADC_SAMPLE_TIME(sConfig->SamplingTime));
    assert_param(IS_ADC_REGULAR_SEQ_MAX_LEN(hadc->Init.NbrOfConversion));

    /* Process locked */
    __HAL_LOCK(hadc);

    if (sConfig->rchn_sel < 8)
    {
        /* Clear the old sample time */
        hadc->device->SAMPTM[0] &= ~(ADC_SAMPTM0(0xF, sConfig->rchn_sel));

        /* Set the new sample time */
        hadc->device->SAMPTM[0] |= ADC_SAMPTM0(sConfig->SamplingTime, sConfig->rchn_sel);
    }
    else if (sConfig->rchn_sel < 16)
    {
        /* Clear the old sample time */
        hadc->device->SAMPTM[1] &= ~(ADC_SAMPTM1(0xF, sConfig->rchn_sel));

        /* Set the new sample time */
        hadc->device->SAMPTM[1] |= ADC_SAMPTM1(sConfig->SamplingTime, sConfig->rchn_sel);
    }

    hadc->device->RCHN0 &= ~(ADC_SQR0_RK(0x1F, 0));

    hadc->device->RCHN0 |= ADC_SQR0_RK(sConfig->rchn_sel, 0);

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_ADC_SequenceConfig(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig)
{
    int i = 0;

    /* Check the parameters */
    assert_param(IS_ADC_SAMPLE_TIME(sConfig->SamplingTime));
    assert_param(IS_ADC_REGULAR_SEQ_MAX_LEN(hadc->Init.NbrOfConversion));

    /* Process locked */
    __HAL_LOCK(hadc);

    for (i = 0; i < 16; i++)
    {
        if (i < 8)
        {
            /* Clear the old sample time */
            hadc->device->SAMPTM[0] &= ~(ADC_SAMPTM0(0xF, i));

            /* Set the new sample time */
            hadc->device->SAMPTM[0] |= ADC_SAMPTM0(sConfig->SamplingTime, i);
        }
        else if (i < 16)
        {
            /* Clear the old sample time */
            hadc->device->SAMPTM[1] &= ~(ADC_SAMPTM1(0xF, i));

            /* Set the new sample time */
            hadc->device->SAMPTM[1] |= ADC_SAMPTM1(sConfig->SamplingTime, i);
        }
    }

    for (i = 0; i < hadc->Init.NbrOfConversion; i++)
    {
        if (i < 6)
        {
            /* Clear the old SQx bits for the selected rank */
            hadc->device->RCHN0 &= ~(ADC_SQR0_RK(0x1F, i));

            /* Set the SQx bits for the selected rank */
            hadc->device->RCHN0 |= ADC_SQR0_RK(i, i);
        }
        /* For Rank 6 to 11 */
        else if (i < 12)
        {
            /* Clear the old SQx bits for the selected rank */
            hadc->device->RCHN1 &= ~(ADC_SQR1_RK(0x1F, i));

            /* Set the SQx bits for the selected rank */
            hadc->device->RCHN1 |= ADC_SQR1_RK(i, i);
        }
        else if (i < 16)
        {
            /* Clear the old SQx bits for the selected rank */
            hadc->device->RCHN2 &= ~(ADC_SQR2_RK(0x1F, i));

            /* Set the SQx bits for the selected rank */
            hadc->device->RCHN2 |= ADC_SQR2_RK(i, i);
        }
        else
        {

        }
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
    ADC_Device_T *pDevice     = hADC->device;
    unsigned int adc_rst_data_reg = ADC0_READ_FIFO_PHY_ADDRESS;

    /* Data fifo have one data will triger dma to transfer */
    ADC_DFIFO_PREFULL_NUM(pDevice, ADC_DFIFO_PRE_FULL_NUM_1);

    if (ADC_DEV0 == (ADC_Device_T *)hADC->device)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_ADC1, &reqResult);
        adc_rst_data_reg = ADC0_READ_FIFO_PHY_ADDRESS;
    }
    else if (ADC_DEV1 == (ADC_Device_T *)hADC->device)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_ADC2, &reqResult);
        adc_rst_data_reg = ADC1_READ_FIFO_PHY_ADDRESS;
    }
    else if (ADC_DEV2 == (ADC_Device_T *)hADC->device)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_ADC3, &reqResult);
        adc_rst_data_reg = ADC2_READ_FIFO_PHY_ADDRESS;
    }

    if (ret != RET_OK)
    {
        //fail, need return fail status
        return RET_ERROR;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
    dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    dmaConfig.width = DMA_WIDTH_32BIT;
    dmaConfig.burst = (DMA_Burst_T)0;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PRIMARY;
    pDMAConfig->callback = ADC_DmaCallback;

    if ((ret = HAL_DMA_Init(DMA_DEV, pDMAConfig, &s_DMA_Handle)) != RET_OK)
    {
        printf("%s error: %d\n", __FUNCTION__, __LINE__);
        return ret;
    }
    LINK_DMAHDL_TO_PARENTHDL(hADC, pDMAHandle, s_DMA_Handle);

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
    /* Process locked */
    __HAL_LOCK(hadc);

    if (hadc->Init.compareEn == ENABLE)
    {
        ADC_SET_COMPARE_VALUE(hadc->device, pcompConfig->compareLowValue, pcompConfig->compareHighValue);
        ADC_SET_COMPARE_MODE(hadc->device, pcompConfig->compareMode);
        hadc->device->CTRL =  ADC_COMPARE_ENABLE(hadc->device);
    }
    else
    {
        hadc->device->CTRL = ADC_COMPARE_DISABLE(hadc->device);
    }

    if (hadc->Init.compareSingleEn == ENABLE)
    {
        hadc->device->CTRL = ADC_COMPARE_SINGLE_ENABLE(hadc->device);
        hadc->device->COM_CHN = ADC_AUTOCOMP_CHN_SELECT(pcompConfig->autoCompChSelect);
    }
    else
    {
        hadc->device->CTRL = ADC_COMPARE_SINGLE_DISABLE(hadc->device);
    }

    /* Process unlocked */
    __HAL_UNLOCK(hadc);

    /* Return function status */
    return HAL_OK;
}

static uint32_t ADC_ConvertToResolutionValue(unsigned char resolution, uint32_t data)
{
    float k = 0.9969f, a = 1.25f;
    uint8_t offset[4] = {0, 8, 10, 12};
    if (resolution == ADC_RESOLUTION_10BIT)
    {
        data = data & 0x3FF;
    }
    else if (resolution == ADC_RESOLUTION_8BIT)
    {
        data = data & 0xFF;
    }
    /*f6721b add adc check algorithm*/
#ifdef CHIP_F6721B
    if (data == 0)
    {
        data = (data + offset[0]) * k + a ;
    }

    else if ((data > 0) && (data < 1857))
    {
        data = (data + offset[1]) * k + a ;
    }

    else if ((data > 1856) && (data < 2182))
    {
        data = (data + offset[2]) * k + a ;
    }

    else
    {
        data = (data + offset[3]) * k + a ;
    }

#endif
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
        val = (hadc->device->DRST & 0xfff);
        pConvertData[i] = ADC_ConvertToResolutionValue(resolution, val);
        /*PR_DEBUG("HAL_ADC_FifoConvertValue :val=%d\n", pConvertData[i]);*/
    }

    return  HAL_OK;
}

/**
  * @brief  Gets the converted value from data register of injected channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @param  InjectedRank: the ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg Injected Channel0 selected
  *            @arg Injected Channel1 selected
  *            @arg Injected Channel2 selected
  *            @arg Injected Channel3 selected
  * @retval None
  */
HAL_StatusTypeDef  HAL_ADC_InjectConvertValue(ADC_HandleTypeDef *hadc, unsigned char  resolution, uint32_t *pConvertData, uint8_t length)
{
    uint8_t i = 0 ;
    uint32_t val = 0;

    for (i = 0; i < length; i++)
    {
        /*only low 12bit vaild data*/
        val = ((hadc->device->JDRST[i]) & 0xfff);
        pConvertData[i] = ADC_ConvertToResolutionValue(resolution, val);
        /*PR_DEBUG("HAL_ADC_InjectConvertValue :val=%d\n", pConvertData[i]);*/
    }

    return  HAL_OK;
}

/*dma read buf convert*/
/**
  * @brief  Gets the converted value from data register of regular channel.
  * @param  hadc: pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval Converted value
  */
void HAL_ADC_BufConvertValue(ADC_HandleTypeDef *hadc, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length)
{
    uint8_t i = 0 ;

    for (i = 0; i < length; i++)
    {
        pConvertData[i] = ADC_ConvertToResolutionValue(resolution, pConvertData[i]);
    }
}

static int ADC_Stop(ADC_HandleTypeDef *hADC)
{
    int ret = HAL_OK;

    if (ADC_TryLock(hADC) != HAL_OK)
    {
        return RET_BUSY;
    }

    /* disabel adc */
    ADC_DISABLE(hADC->device);

    __HAL_UNLOCK(hADC);

    return ret;
}

void HAL_ADC0_InterruptHandler(void)
{
    HAL_ADC_IRQ_handler(s_pADCHandle[0]);
}

void HAL_ADC1_InterruptHandler(void)
{
    HAL_ADC_IRQ_handler(s_pADCHandle[1]);
}

void HAL_ADC2_InterruptHandler(void)
{
    HAL_ADC_IRQ_handler(s_pADCHandle[2]);
}

RET_CODE ADC_DeInit(ADC_Device_T *pADC, ADC_HandleTypeDef *hADC)
{
    int ret = HAL_OK;
    ret = ADC_Stop(hADC);
    hADC->device = NULL ;

    return ret;
}

/**
  * @brief  The following  function are used to  ADC peripheral Request Irq.
  * @note  software has to reset peripheral  before using peripheral
  * @param  adc_sel: select ADCX
  *
  * @retval None
  *
  */
void HAL_ADC_RequestIrq(int adc_sel)
{
    if (adc_sel == 0)
    {
        HAL_RequestIrq(ADC0_IRQn, ADC0_IRQ_PRIORITY, HAL_ADC0_InterruptHandler, 0);
        HAL_IRQ_ENABLE(ADC0_IRQn);
    }
    else if (adc_sel == 1)
    {
        HAL_RequestIrq(ADC1_IRQn, ADC1_IRQ_PRIORITY, HAL_ADC1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(ADC1_IRQn);
    }
    else if (adc_sel == 2)
    {
        HAL_RequestIrq(ADC2_IRQn, ADC2_IRQ_PRIORITY, HAL_ADC2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(ADC2_IRQn);
    }
    else
    {
        HAL_RequestIrq(ADC0_IRQn, ADC0_IRQ_PRIORITY, HAL_ADC0_InterruptHandler, 0);
        HAL_IRQ_ENABLE(ADC0_IRQn);

        HAL_RequestIrq(ADC1_IRQn, ADC1_IRQ_PRIORITY, HAL_ADC1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(ADC1_IRQn);

        HAL_RequestIrq(ADC2_IRQn, ADC2_IRQ_PRIORITY, HAL_ADC2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(ADC2_IRQn);
    }
}


/**
  * @brief  The following  function are used to reset ADC peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_ADC_SW_Reset(void)
{
    HAL_Reset_Module(RESET_ADC_SW_RSTJ);
}

/**
  * @brief  The following two functions are used to enable/disable ADC  peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param adc_sel: select ADCX
  *
  * @retval None
  *
  */
void HAL_ADC_CLK_ENABLE(int adc_sel)
{
    if (adc_sel == 0)
    {
        HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ADC);
    }
    else if (adc_sel == 1)
    {
        HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ADC2);
    }
    else if (adc_sel == 2)
    {
        HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ADC3);
    }
    else
    {
        HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ADC);
        HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ADC2);
        HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_ADC3);
    }
}

void HAL_ADC_CLK_DISABLE(int adc_sel)
{
    if (adc_sel == 0)
    {
        HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ADC);
    }
    else if (adc_sel == 1)
    {
        HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ADC2);
    }
    else if (adc_sel == 2)
    {
        HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ADC3);
    }
    else
    {
        HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ADC);
        HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ADC2);
        HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_ADC3);
    }
}
