/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          tim.c
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2018/03/19
 *
 * @brief        TIM APIs for application.
 *
 * @note
 *    2018/03/19, edward.zhu, V1.0.0
 *        Initial version.
 */

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_tim.h"
#include "gm_ll_tim_dev.h"
#include "gm_hal_tim.h"
#include <gm_irq.h>

/*******************************************************************************
* Global variables
*******************************************************************************/
#define __ASSERT(test, fmt, ...) \
    do {/* nothing */        \
    } while ((0))

/*******************************************************************************
* Static variables
*******************************************************************************/
/** @brief Pointers to ETM handles for each instance. */
static uint32_t instance = 0;
static TIM_Handle_T *s_TIMHandle[MAX_TIM_NUM] = {NULL};

/** @brief Pointers to ETM IRQ number for each instance. */
#if defined(CHIP_GM6721)
const IRQn_Type s_TIMIrqs[] = {TIM1_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn,
                               TIM5_IRQn, TIM8_IRQn, TIM9_IRQn, TIM10_IRQn,
                               TIM11_IRQn, TIM12_IRQn, TIM13_IRQn, TIM14_IRQn
                              };
#endif

#if defined(CHIP_F6721B)
const IRQn_Type s_TIMIrqs[] = {TIM1_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn,
                               TIM5_IRQn, TIM8_IRQn
                              };
#endif

TIM_Handle_T *g_htim;


TIM_IRQCallback_T pTimCallback = NULL;

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/

/*******************************************************************************
* Codes
*******************************************************************************/
RET_CODE HAL_TIM_DMAConfig(TIM_Device_T *TIMx, uint8_t dmaRead, uint32_t src, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_OK;

    __ASSERT((TIMx != NULL) && (pDMAHandle != NULL), "");

    if (src == TIM_DMA_CC1)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_CC1D, &reqResult);
        }
        else if (TIMx == TIM2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM2_CC1D, &reqResult);
        }
        else if (TIMx == TIM3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM3_CC1D, &reqResult);
        }
        else if (TIMx == TIM4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM4_CC1D, &reqResult);
        }
        else if (TIMx == TIM5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM5_CC1D, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_CC1D, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Capture/Compare 1 DMA request */
        LL_TIM_CC1DE_ENABLE(TIMx);
    }

    else if (src == TIM_DMA_CC2)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_CC2D, &reqResult);
        }
        else if (TIMx == TIM2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM2_CC2D, &reqResult);
        }
        else if (TIMx == TIM3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM3_CC2D, &reqResult);
        }
        else if (TIMx == TIM4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM4_CC2D, &reqResult);
        }
        else if (TIMx == TIM5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM5_CC2D, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_CC2D, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Capture/Compare 2 DMA request */
        LL_TIM_CC2DE_ENABLE(TIMx);

    }

    else if (src == TIM_DMA_CC3)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_CC3D, &reqResult);
        }
        else if (TIMx == TIM2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM2_CC3D, &reqResult);
        }
        else if (TIMx == TIM3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM3_CC3D, &reqResult);
        }
        else if (TIMx == TIM4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM4_CC3D, &reqResult);
        }
        else if (TIMx == TIM5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM5_CC3D, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_CC3D, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Capture/Compare 3 DMA request */
        LL_TIM_CC3DE_ENABLE(TIMx);

    }

    else if (src == TIM_DMA_CC4)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_CC4D, &reqResult);
        }
        else if (TIMx == TIM2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM2_CC4D, &reqResult);
        }
        else if (TIMx == TIM3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM3_CC4D, &reqResult);
        }
        else if (TIMx == TIM4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM4_CC4D, &reqResult);
        }
        else if (TIMx == TIM5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM5_CC4D, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_CC4D, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Capture/Compare 4 DMA request */
        LL_TIM_CC4DE_ENABLE(TIMx);

    }

    else if (src == TIM_DMA_UPDATE)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_UD, &reqResult);
        }
        else if (TIMx == TIM2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM2_UD, &reqResult);
        }
        else if (TIMx == TIM3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM3_UD, &reqResult);
        }
        else if (TIMx == TIM4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM4_UD, &reqResult);
        }
        else if (TIMx == TIM5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM5_UD, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_UD, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Update DMA request */
        LL_TIM_UDE_ENABLE(TIMx);

    }

    else if (src == TIM_DMA_TRIGGER)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_TD, &reqResult);
        }
        else if (TIMx == TIM2)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM2_TD, &reqResult);
        }
        else if (TIMx == TIM3)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM3_TD, &reqResult);
        }
        else if (TIMx == TIM4)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM4_TD, &reqResult);
        }
        else if (TIMx == TIM5)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM5_TD, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_TD, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Trigger DMA request */
        LL_TIM_TDE_ENABLE(TIMx);

    }

    else if (src == TIM_DMA_COM)
    {
        if (TIMx == TIM1)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM1_COMD, &reqResult);
        }
        else if (TIMx == TIM8)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_TIM8_COMD, &reqResult);
        }
        else
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
        }

        /* Enable the TIM Commutation DMA request */
        LL_TIM_COMDE_ENABLE(TIMx);
    }

    else
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_INVALID, &reqResult);
    }

    /*Enable Capture/Compare DMA selection*/
    //LL_TIM_CCDS_ENABLE(TIMx);

    memset(&dmaConfig, 0, sizeof(DMA_Config_T));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.width = DMA_WIDTH_32BIT;
    dmaConfig.burst = DMA_BURST_1;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PRIMARY;
    dmaConfig.callback = TIM_DMACallback;

    if (dmaRead)
    {
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    }
    else
    {
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
    }

    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);

    return HAL_OK;

}

void TIM_InterruptConfig(TIM_Handle_T *htim)
{
#ifdef CONFIG_USE_HAL_TEST

#endif

#ifdef RABBIT_NO_OS_SYSTEM
    if (htim->instance == TIM1)
    {
        HAL_IRQ_ENABLE(TIM1_IRQn);
    }
    else if (htim->instance == TIM2)
    {
        HAL_IRQ_ENABLE(TIM2_IRQn);
    }
    else if (htim->instance == TIM3)
    {
        HAL_IRQ_ENABLE(TIM3_IRQn);
    }
    else if (htim->instance == TIM4)
    {
        HAL_IRQ_ENABLE(TIM4_IRQn);
    }
    else if (htim->instance == TIM5)
    {
        HAL_IRQ_ENABLE(TIM5_IRQn);
    }
    else if (htim->instance == TIM8)
    {
        HAL_IRQ_ENABLE(TIM8_IRQn);
    }

#if defined(CHIP_GM6721)
    else if (htim->instance == TIM9)
    {
        HAL_IRQ_ENABLE(TIM9_IRQn);
    }
    else if (htim->instance == TIM10)
    {
        HAL_IRQ_ENABLE(TIM10_IRQn);
    }
    else if (htim->instance == TIM11)
    {
        HAL_IRQ_ENABLE(TIM11_IRQn);
    }
    else if (htim->instance == TIM12)
    {
        HAL_IRQ_ENABLE(TIM12_IRQn);
    }
    else if (htim->instance == TIM13)
    {
        HAL_IRQ_ENABLE(TIM13_IRQn);
    }
    else if (htim->instance == TIM14)
    {
        HAL_IRQ_ENABLE(TIM14_IRQn);
    }
#endif

    else
    {

    }
#endif

}

RET_CODE HAL_TIM_InitCommon(TIM_Handle_T *htim)
{

    if (htim == NULL)
    {
        return HAL_ERROR;
    }

#if 1//defined(RABBIT_NO_OS_SYSTEM) || defined(CONFIG_USE_HAL_TEST)
    if (htim->instance == TIM1)
    {
        instance = 1;

    }
    if (htim->instance == TIM2)
    {
        instance = 2;
    }
    if (htim->instance == TIM3)
    {
        instance = 3;
    }
    if (htim->instance == TIM4)
    {
        instance = 4;
    }
    if (htim->instance == TIM5)
    {
        instance = 5;
    }
    if (htim->instance == TIM8)
    {
        instance = 8;
    }

#if defined(CHIP_GM6721)
    if (htim->instance == TIM9)
    {
        instance = 9;
    }
    if (htim->instance == TIM10)
    {
        instance = 10;
    }
    if (htim->instance == TIM11)
    {
        instance = 11;
    }
    if (htim->instance == TIM12)
    {
        instance = 12;
    }
    if (htim->instance == TIM13)
    {
        instance = 13;
    }
    if (htim->instance == TIM14)
    {
        instance = 14;
    }
#endif

    g_htim = htim;
#endif
    s_TIMHandle[instance] = htim;

    HAL_TIM_SW_Reset();
    HAL_TIM_CLK_ENABLE(instance);

    HAL_TIM_RequestIrq(instance);
    TIM_InterruptConfig(htim);

    return HAL_OK;

}

RET_CODE HAL_TIM_InitTimeBase(TIM_Handle_T *htim)
{
    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_COUNTER_MODE(htim->init.counterMode));
    assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->init.clockDivision));
    assert_param(IS_TIM_PERIOD(htim->init.period));
    assert_param(IS_TIM_PRESCALER(htim->init.prescaler));

    if (htim->state == HAL_TIM_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        htim->lock = HAL_TIM_UNLOCKED;
    }

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Set the Time Base configuration */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Initialize the TIM state */
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;

}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartTimeBase(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Enable the TIM Update interrupt */
    LL_TIM_UIE_ENABLE(TIMx);

    /* Enable the Peripheral */
    LL_TIM_CNT_ENABLE(TIMx);

    /* Change the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    /* Return function status */
    return RET_OK;

}

RET_CODE HAL_TIM_StopTimeBase(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Disable the TIM Update interrupt */
    LL_TIM_UIE_DISABLE(TIMx);

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Change the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    /* Return function status */
    return RET_OK;

}

RET_CODE HAL_TIM_InitOutputCompare(TIM_Handle_T *htim)
{
    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_COUNTER_MODE(htim->init.counterMode));
    assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->init.clockDivision));
    assert_param(IS_TIM_PERIOD(htim->init.period));
    assert_param(IS_TIM_PRESCALER(htim->init.prescaler));

    if (htim->state == HAL_TIM_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        htim->lock = HAL_TIM_UNLOCKED;
    }

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Init the base time for the Output Compare */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Initialize the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;

}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartOutputCompare(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, channel));
    assert_param(IS_TIM_CCXN_INSTANCE(htim->instance, channel));

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            LL_TIM_CC1IE_DISABLE(TIMx);
            /* Enable the TIM Capture/Compare 1 interrupt */
            //LL_TIM_CC1IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_2:
        {
            LL_TIM_CC2IE_DISABLE(TIMx);
            /* Enable the TIM Capture/Compare 2 interrupt */
            //LL_TIM_CC2IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_3:
        {
            LL_TIM_CC3IE_DISABLE(TIMx);
            /* Enable the TIM Capture/Compare 3 interrupt */
            //LL_TIM_CC3IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_4:
        {
            LL_TIM_CC4IE_DISABLE(TIMx);
            /* Enable the TIM Capture/Compare 4 interrupt */
            //LL_TIM_CC4IE_ENABLE(TIMx);
        }
        break;

        default:
            break;
    }

    /* Enable the Output compare channel/channel N */
    HAL_TIM_CCxChannelCmd(htim->instance, channel, TIM_CCx_ENABLE);
    HAL_TIM_CCxNChannelCmd(htim->instance, channel, TIM_CCxN_ENABLE);

    if (IS_TIM_ADVANCED_INSTANCE(htim->instance) != RESET)
    {
        /* Enable the main output */
        LL_TIM_MOE_ENABLE(TIMx);
    }

    /* Enable the Peripheral */
    LL_TIM_CNT_ENABLE(TIMx);

    /* Return function status */
    return RET_OK;

}

RET_CODE HAL_TIM_StopOutputCompare(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, channel));
    assert_param(IS_TIM_CCXN_INSTANCE(htim->instance, channel));

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Disable the TIM Capture/Compare 1 interrupt */
            LL_TIM_CC1IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Disable the TIM Capture/Compare 2 interrupt */
            LL_TIM_CC2IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Disable the TIM Capture/Compare 3 interrupt */
            LL_TIM_CC3IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Disable the TIM Capture/Compare 4 interrupt */
            LL_TIM_CC4IE_DISABLE(TIMx);
        }
        break;

        default:
            break;
    }

    /* Disable the Output compare channel/channel N */
    HAL_TIM_CCxChannelCmd(htim->instance, channel, TIM_CCx_DISABLE);
    HAL_TIM_CCxNChannelCmd(htim->instance, channel, TIM_CCxN_DISABLE);

    if (IS_TIM_ADVANCED_INSTANCE(htim->instance) != RESET)
    {
        /* Disable the Main Output */
        if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
        {
            LL_TIM_MOE_DISABLE(TIMx);
        }
    }

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Return function status */
    return RET_OK;

}

RET_CODE HAL_TIM_InitPWM(TIM_Handle_T *htim)
{
    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_COUNTER_MODE(htim->init.counterMode));
    assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->init.clockDivision));
    assert_param(IS_TIM_PERIOD(htim->init.period));
    assert_param(IS_TIM_PRESCALER(htim->init.prescaler));

    if (htim->state == HAL_TIM_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        htim->lock = HAL_TIM_UNLOCKED;
    }

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;
    /* Init the base time for the PWM */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Initialize the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;
}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartPWM(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, channel));
    assert_param(IS_TIM_CCXN_INSTANCE(htim->instance, channel));

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            LL_TIM_CC1IE_DISABLE(TIMx);
            /* Enable the TIM Capture/Compare 1 interrupt */
            //LL_TIM_CC1IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Enable the TIM Capture/Compare 2 interrupt */
            LL_TIM_CC2IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Enable the TIM Capture/Compare 3 interrupt */
            LL_TIM_CC3IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Enable the TIM Capture/Compare 4 interrupt */
            LL_TIM_CC4IE_DISABLE(TIMx);
        }
        break;

        default:
            break;
    }

    /* Enable the Capture compare channel/channel N */
    HAL_TIM_CCxChannelCmd(htim->instance, channel, TIM_CCx_ENABLE);
    HAL_TIM_CCxNChannelCmd(htim->instance, channel, TIM_CCxN_ENABLE);

    if (IS_TIM_ADVANCED_INSTANCE(htim->instance) != RESET)
    {
        /* Enable the main output */
        LL_TIM_MOE_ENABLE(TIMx);
    }

#if (defined TEST_CASE_14)
    LL_TIM_CNT_DISABLE(TIMx);
#else
    /* Enable the Peripheral */
    LL_TIM_CNT_ENABLE(TIMx);
#endif

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_StopPWM(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, channel));
    assert_param(IS_TIM_CCXN_INSTANCE(htim->instance, channel));

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Disable the TIM Capture/Compare 1 interrupt */
            LL_TIM_CC1IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Disable the TIM Capture/Compare 2 interrupt */
            LL_TIM_CC2IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Disable the TIM Capture/Compare 3 interrupt */
            LL_TIM_CC3IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Disable the TIM Capture/Compare 4 interrupt */
            LL_TIM_CC4IE_DISABLE(TIMx);
        }
        break;

        default:
            break;
    }

    /* Disable the Capture compare channel/channel N */
    HAL_TIM_CCxChannelCmd(htim->instance, channel, TIM_CCx_DISABLE);
    HAL_TIM_CCxNChannelCmd(htim->instance, channel, TIM_CCxN_DISABLE);

    if (IS_TIM_ADVANCED_INSTANCE(htim->instance) != RESET)
    {
        /* Disable the Main Output */
        if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
        {
            LL_TIM_MOE_DISABLE(TIMx);
        }
    }

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Change the htim state */
    htim->state = HAL_TIM_STATE_READY;

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_InitInputCapture(TIM_Handle_T *htim)
{
    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_COUNTER_MODE(htim->init.counterMode));
    assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->init.clockDivision));
    assert_param(IS_TIM_PERIOD(htim->init.period));
    assert_param(IS_TIM_PRESCALER(htim->init.prescaler));

    if (htim->state == HAL_TIM_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        htim->lock = HAL_TIM_UNLOCKED;
    }

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Init the base time for the input capture */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Initialize the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;
}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartInputCapture(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, channel));

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            //LL_TIM_CC1IE_DISABLE(TIMx);
            /* Enable the TIM Capture/Compare 1 interrupt */
            LL_TIM_CC1IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Enable the TIM Capture/Compare 2 interrupt */
            LL_TIM_CC2IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Enable the TIM Capture/Compare 3 interrupt */
            LL_TIM_CC3IE_ENABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Enable the TIM Capture/Compare 4 interrupt */
            LL_TIM_CC4IE_ENABLE(TIMx);
        }
        break;

        default:
            break;
    }

    /* Enable the Input Capture channel */
    HAL_TIM_CCxChannelCmd(htim->instance, channel, TIM_CCx_ENABLE);

    LL_TIM_CNT_ENABLE(TIMx);

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_StopInputCapture(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, channel));

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Disable the TIM Capture/Compare 1 interrupt */
            LL_TIM_CC1IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Disable the TIM Capture/Compare 2 interrupt */
            LL_TIM_CC2IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Disable the TIM Capture/Compare 3 interrupt */
            LL_TIM_CC3IE_DISABLE(TIMx);
        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Disable the TIM Capture/Compare 4 interrupt */
            LL_TIM_CC4IE_DISABLE(TIMx);
        }
        break;

        default:
            break;
    }

    /* Disable the Input Capture channel */
    HAL_TIM_CCxChannelCmd(htim->instance, channel, TIM_CCx_DISABLE);

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_InitOnePulse(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t onePulseMode)
{
    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_COUNTER_MODE(htim->init.counterMode));
    assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->init.clockDivision));
    assert_param(IS_TIM_OPM_MODE(onePulseMode));

    if (htim->state == HAL_TIM_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        htim->lock = HAL_TIM_UNLOCKED;
    }

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Configure the Time base in the One Pulse Mode */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Reset the OPM Bit */
    LL_TIM_OPM_DISABLE(TIMx);

    /* Configure the OPM Mode */
    TIMx->CR1 |= onePulseMode;

    /* Initialize the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;
}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartOnePulse(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t outputChannel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, outputChannel));
    assert_param(IS_TIM_CCXN_INSTANCE(htim->instance, outputChannel));

    /* Enable the TIM Capture/Compare 1 interrupt */
    //LL_TIM_CC1IE_ENABLE(TIMx);

    /* Enable the TIM Capture/Compare 2 interrupt */
    //LL_TIM_CC2IE_ENABLE(TIMx);

    /* Enable the Capture compare and the Input Capture channels
      (in the OPM Mode the two possible channels that can be used are TIM_CHANNEL_1 and TIM_CHANNEL_2)
      if TIM_CHANNEL_1 is used as output, the TIM_CHANNEL_2 will be used as input and
      if TIM_CHANNEL_1 is used as input, the TIM_CHANNEL_2 will be used as output
      in all combinations, the TIM_CHANNEL_1 and TIM_CHANNEL_2 should be enabled together

      No need to enable the counter, it's enabled automatically by hardware
      (the counter starts in response to a stimulus and generate a pulse */
    HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);

    /* Enable the complementary One Pulse output */
    HAL_TIM_CCxNChannelCmd(htim->instance, outputChannel, TIM_CCxN_ENABLE);

    if (IS_TIM_ADVANCED_INSTANCE(htim->instance) != RESET)
    {
        /* Enable the main output */
        LL_TIM_MOE_ENABLE(TIMx);
    }

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_StopOnePulse(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t outputChannel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CCX_INSTANCE(htim->instance, outputChannel));
    assert_param(IS_TIM_CCXN_INSTANCE(htim->instance, outputChannel));

    /* Disable the TIM Capture/Compare 1 interrupt */
    LL_TIM_CC1IE_DISABLE(TIMx);

    /* Disable the TIM Capture/Compare 2 interrupt */
    LL_TIM_CC2IE_DISABLE(TIMx);

    /* Disable the Capture compare and the Input Capture channels
    (in the OPM Mode the two possible channels that can be used are TIM_CHANNEL_1 and TIM_CHANNEL_2)
    if TIM_CHANNEL_1 is used as output, the TIM_CHANNEL_2 will be used as input and
    if TIM_CHANNEL_1 is used as input, the TIM_CHANNEL_2 will be used as output
    in all combinations, the TIM_CHANNEL_1 and TIM_CHANNEL_2 should be disabled together */

    HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);
    HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_2, TIM_CCx_DISABLE);

    /* Disable the complementary One Pulse output */
    HAL_TIM_CCxNChannelCmd(htim->instance, outputChannel, TIM_CCxN_DISABLE);

    if (IS_TIM_ADVANCED_INSTANCE(htim->instance) != RESET)
    {
        /* Disable the Main Output */
        if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
        {
            LL_TIM_MOE_DISABLE(TIMx);
        }
    }

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_InitEncoder(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_EncoderInit_T *sConfig)
{
    uint32_t tmpsmcr = 0U;
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccer = 0U;

    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    /* Check the parameters */
    assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
    assert_param(IS_TIM_ENCODER_MODE(sConfig->encoderMode));
    assert_param(IS_TIM_IC_SELECTION(sConfig->ic1Selection));
    assert_param(IS_TIM_IC_SELECTION(sConfig->ic2Selection));
    assert_param(IS_TIM_IC_POLARITY(sConfig->ic1Polarity));
    assert_param(IS_TIM_IC_POLARITY(sConfig->ic2Polarity));
    assert_param(IS_TIM_IC_PRESCALER(sConfig->ic1Prescaler));
    assert_param(IS_TIM_IC_PRESCALER(sConfig->ic2Prescaler));
    assert_param(IS_TIM_IC_FILTER(sConfig->ic1Filter));
    assert_param(IS_TIM_IC_FILTER(sConfig->ic2Filter));

    if (htim->state == HAL_TIM_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        htim->lock = HAL_TIM_UNLOCKED;
    }

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Reset the SMS bits */
    TIMx->SMCR &= ~TIM_SMCR_SMS;

    /* Configure the Time base in the Encoder Mode */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Get the TIMx SMCR register value */
    tmpsmcr = TIMx->SMCR;

    /* Get the TIMx CCMR1 register value */
    tmpccmr1 = TIMx->CCMR1;

    /* Get the TIMx CCER register value */
    tmpccer = TIMx->CCER;

    /* Set the encoder Mode */
    tmpsmcr |= sConfig->encoderMode;

    /* Select the Capture Compare 1 and the Capture Compare 2 as input */
    tmpccmr1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    tmpccmr1 |= (sConfig->ic1Selection | (sConfig->ic2Selection << 8U));

    /* Set the Capture Compare 1 and the Capture Compare 2 prescalers and filters */
    tmpccmr1 &= ~(TIM_CCMR1_IC1PSC | TIM_CCMR1_IC2PSC);
    tmpccmr1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC2F);
    tmpccmr1 |= sConfig->ic1Prescaler | (sConfig->ic2Prescaler << 8U);
    tmpccmr1 |= (sConfig->ic1Filter << 4U) | (sConfig->ic2Filter << 12U);

    /* Set the TI1 and the TI2 Polarities */
    tmpccer &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
    tmpccer &= ~(TIM_CCER_CC1NP | TIM_CCER_CC2NP);
    tmpccer |= sConfig->ic1Polarity | (sConfig->ic2Polarity << 4U);

    /* Write to TIMx SMCR */
    TIMx->SMCR = tmpsmcr;

    /* Write to TIMx CCMR1 */
    TIMx->CCMR1 = tmpccmr1;

    /* Write to TIMx CCER */
    TIMx->CCER = tmpccer;

    /* Initialize the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;
}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartEncoder(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC2_INSTANCE(htim->instance));

    /* Enable the encoder interface channels */
    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
            LL_TIM_CC1IE_ENABLE(TIMx);
            break;
        }
        case TIM_CHANNEL_2:
        {
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
            LL_TIM_CC2IE_ENABLE(TIMx);
            break;
        }
        default :
        {
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);
            LL_TIM_CC1IE_ENABLE(TIMx);
            LL_TIM_CC2IE_ENABLE(TIMx);
            break;
        }
    }

    /* Enable the Peripheral */
    LL_TIM_CNT_ENABLE(TIMx);

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_StopEncoder(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC2_INSTANCE(htim->instance));

    /* Disable the Input Capture channels 1 and 2
     (in the EncoderInterface the two possible channels that can be used are TIM_CHANNEL_1 and TIM_CHANNEL_2) */
    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);
            LL_TIM_CC1IE_DISABLE(TIMx);
            break;
        }
        case TIM_CHANNEL_2:
        {
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_2, TIM_CCx_DISABLE);
            LL_TIM_CC2IE_DISABLE(TIMx);
            break;
        }
        default :
        {
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);
            HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_2, TIM_CCx_DISABLE);
            LL_TIM_CC1IE_DISABLE(TIMx);
            LL_TIM_CC2IE_DISABLE(TIMx);
            break;
        }
    }

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Change the htim state */
    htim->state = HAL_TIM_STATE_READY;


    /* Return function status */
    return RET_OK;
}
RET_CODE HAL_TIM_InitHallSensor(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_HallSensorInit_T *sConfig)
{
    TIM_OutputCompareInit_T sOCConfig;

    /* Check the TIM handle allocation */
    if (htim == NULL)
    {
        return RET_ERROR;
    }

    HAL_TIM_InitCommon(htim);

    assert_param(IS_TIM_XOR_INSTANCE(htim->instance));
    assert_param(IS_TIM_COUNTER_MODE(htim->init.counterMode));
    assert_param(IS_TIM_CLOCKDIVISION_DIV(htim->init.clockDivision));
    assert_param(IS_TIM_IC_POLARITY(sConfig->ic1Polarity));
    assert_param(IS_TIM_IC_PRESCALER(sConfig->ic1Prescaler));
    assert_param(IS_TIM_IC_FILTER(sConfig->ic1Filter));

    /* Set the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Configure the Time base in the Encoder Mode */
    HAL_TIM_TimeBaseConfig(htim->instance, &htim->init);

    /* Configure the Channel 1 as Input Channel to interface with the three Outputs of the Hall sensor */
    HAL_TIM_TIConfig(htim->instance, sConfig->ic1Polarity, TIM_ICSELECTION_TRC,
                     sConfig->ic1Filter, TIM_CHANNEL_1);

    /* Reset the IC1PSC Bits */
    TIMx->CCMR1 &= ~TIM_CCMR1_IC1PSC;

    /* Set the IC1PSC value */
    TIMx->CCMR1 |= sConfig->ic1Prescaler;

    /* Enable the Hall sensor interface (XOR function of the three inputs) */
    LL_TIM_TI1S_ENABLE(TIMx);

    /* Select the TIM_TS_TI1F_ED signal as Input trigger for the TIM */
    TIMx->SMCR &= ~TIM_SMCR_TS;
    TIMx->SMCR |= TIM_TS_TI1F_ED;

    /* Use the TIM_TS_TI1F_ED signal to reset the TIM counter each edge detection */
    TIMx->SMCR &= ~TIM_SMCR_SMS;
    TIMx->SMCR |= TIM_SLAVEMODE_RESET;

    /* Program channel 2 in PWM 2 mode with the desired Commutation_Delay*/
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.ocIdleState = TIM_OCIDLESTATE_RESET;
    sOCConfig.ocMode = TIM_OCMODE_PWM2;
    sOCConfig.ocNIdleState = TIM_OCNIDLESTATE_RESET;
    sOCConfig.ocNPolarity = TIM_OCNPOLARITY_HIGH;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.pulse = sConfig->commutation_Delay;

    HAL_TIM_ConfigOCChannel(htim, TIMx, &sOCConfig, TIM_CHANNEL_2);

    /* Select OC2REF as trigger output on TRGO: write the MMS bits in the TIMx_CR2
      register to 101 */
    TIMx->CR2 &= ~TIM_CR2_MMS;
    TIMx->CR2 |= TIM_TRGO_OC2REF;

    /* Initialize the TIM state*/
    htim->state = HAL_TIM_STATE_READY;

    return RET_OK;
}

/* Blocking mode: Polling */
RET_CODE HAL_TIM_StartHallSensor(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_XOR_INSTANCE(htim->instance));

    /* Enable the capture compare Interrupts 1 event */
    //LL_TIM_CC1IE_ENABLE(TIMx);

    /* Enable the Input Capture channels 1
      (in the Hall Sensor Interface the Three possible channels that can be
      used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
    HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

    /* Enable the Peripheral */
    LL_TIM_CNT_ENABLE(TIMx);

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_StopHallSensor(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    /* Check the parameters */
    assert_param(IS_TIM_XOR_INSTANCE(htim->instance));

    /* Disable the Input Capture channels 1, 2 and 3
      (in the Hall Sensor Interface the Three possible channels that can be
      used are TIM_CHANNEL_1, TIM_CHANNEL_2 and TIM_CHANNEL_3) */
    HAL_TIM_CCxChannelCmd(htim->instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

    /* Disable the capture compare Interrupts event */
    LL_TIM_CC1IE_DISABLE(TIMx);

    /* Disable the Peripheral */
    if ((LL_TIM_CCXE_GET(TIMx) == 0) && (LL_TIM_CCXNE_GET(TIMx) == 0))
    {
        LL_TIM_CNT_DISABLE(TIMx);
    }

    /* Return function status */
    return RET_OK;
}

RET_CODE HAL_TIM_ConfigOCChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx,
                                 TIM_OutputCompareInit_T *sOCConfig, uint32_t channel)
{
    uint32_t tmpccmrx = 0U;
    uint32_t tmpccer = 0U;
    uint32_t tmpcr2 = 0U;

    /* Check the parameters */
    assert_param(IS_TIM_CHANNELS(channel));
    assert_param(IS_TIM_OC_MODE(sOCConfig->ocMode));
    assert_param(IS_TIM_OC_POLARITY(sOCConfig->ocPolarity));

    /* Check input state */
    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    htim->state = HAL_TIM_STATE_BUSY;

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Disable the Channel 1: Reset the CC1E Bit */
            LL_TIM_CC1E_DISABLE(TIMx);

            /* Get the TIMx CCER register value */
            tmpccer = TIMx->CCER;

            /* Get the TIMx CR2 register value */
            tmpcr2 = TIMx->CR2;

            /* Get the TIMx CCMR1 register value */
            tmpccmrx = TIMx->CCMR1;

            /* Reset the Output Compare Mode Bits */
            tmpccmrx &= ~TIM_CCMR1_OC1M;
            tmpccmrx &= ~TIM_CCMR1_CC1S;

            /* Select the Output Compare Mode */
            tmpccmrx |= sOCConfig->ocMode;

            /* Reset the Output Polarity level */
            tmpccer &= ~TIM_CCER_CC1P;

            /* Set the Output Compare Polarity */
            tmpccer |= sOCConfig->ocPolarity;

            if (IS_TIM_ADVANCED_INSTANCE(TIMx) != RESET)
            {
                /* Reset the Output N Polarity level */
                tmpccer &= ~TIM_CCER_CC1NP;

                /* Set the Output N Polarity */
                tmpccer |= sOCConfig->ocNPolarity;

                /* Reset the Output N State */
                tmpccer &= ~TIM_CCER_CC1NE;

                /* Reset the Output Compare and Output Compare N IDLE State */
                tmpcr2 &= ~TIM_CR2_OIS1;
                tmpcr2 &= ~TIM_CR2_OIS1N;

                /* Set the Output Idle state */
                tmpcr2 |= sOCConfig->ocIdleState;

                /* Set the Output N Idle state */
                tmpcr2 |= sOCConfig->ocNIdleState;
            }

            /* Write to TIMx CR2 */
            TIMx->CR2 = tmpcr2;

            /* Write to TIMx CCMR1 */
            TIMx->CCMR1 = tmpccmrx;

            /* Set the Capture Compare Register value */
            TIMx->CCR1 = sOCConfig->pulse;

        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Disable the Channel 2: Reset the CC2E Bit */
            LL_TIM_CC2E_DISABLE(TIMx);

            /* Get the TIMx CCER register value */
            tmpccer = TIMx->CCER;

            /* Get the TIMx CR2 register value */
            tmpcr2 = TIMx->CR2;

            /* Get the TIMx CCMR1 register value */
            tmpccmrx = TIMx->CCMR1;

            /* Reset the Output Compare mode and Capture/Compare selection Bits */
            tmpccmrx &= ~TIM_CCMR1_OC2M;
            tmpccmrx &= ~TIM_CCMR1_CC2S;

            /* Select the Output Compare Mode */
            tmpccmrx |= (sOCConfig->ocMode << 8U);

            /* Reset the Output Polarity level */
            tmpccer &= ~TIM_CCER_CC2P;

            /* Set the Output Compare Polarity */
            tmpccer |= (sOCConfig->ocPolarity << 4U);

            if (IS_TIM_ADVANCED_INSTANCE(TIMx) != RESET)
            {
                assert_param(IS_TIM_OCN_POLARITY(sOCConfig->ocNPolarity));

                /* Reset the Output N Polarity level */
                tmpccer &= ~TIM_CCER_CC2NP;

                /* Set the Output N Polarity */
                tmpccer |= (sOCConfig->ocNPolarity << 4U);

                /* Reset the Output N State */
                tmpccer &= ~TIM_CCER_CC2NE;

                /* Reset the Output Compare and Output Compare N IDLE State */
                tmpcr2 &= ~TIM_CR2_OIS2;
                tmpcr2 &= ~TIM_CR2_OIS2N;

                /* Set the Output Idle state */
                tmpcr2 |= (sOCConfig->ocIdleState << 2U);

                /* Set the Output N Idle state */
                tmpcr2 |= (sOCConfig->ocNIdleState << 2U);
            }

            /* Write to TIMx CR2 */
            TIMx->CR2 = tmpcr2;

            /* Write to TIMx CCMR1 */
            TIMx->CCMR1 = tmpccmrx;

            /* Set the Capture Compare Register value */
            //TIMx->CCR2 = sOCConfig->pulse;
            TIMx->CCR2 = sOCConfig->pulse;
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Disable the Channel 3: Reset the CC2E Bit */
            LL_TIM_CC3E_DISABLE(TIMx);

            /* Get the TIMx CCER register value */
            tmpccer = TIMx->CCER;

            /* Get the TIMx CR2 register value */
            tmpcr2 = TIMx->CR2;

            /* Get the TIMx CCMR2 register value */
            tmpccmrx = TIMx->CCMR2;

            /* Reset the Output Compare mode and Capture/Compare selection Bits */
            tmpccmrx &= ~TIM_CCMR2_OC3M;
            tmpccmrx &= ~TIM_CCMR2_CC3S;

            /* Select the Output Compare Mode */
            tmpccmrx |= sOCConfig->ocMode;

            /* Reset the Output Polarity level */
            tmpccer &= ~TIM_CCER_CC3P;

            /* Set the Output Compare Polarity */
            tmpccer |= (sOCConfig->ocPolarity << 8U);

            if (IS_TIM_ADVANCED_INSTANCE(TIMx) != RESET)
            {
                assert_param(IS_TIM_OCN_POLARITY(sOCConfig->ocNPolarity));
                assert_param(IS_TIM_OCNIDLE_STATE(sOCConfig->ocNIdleState));
                assert_param(IS_TIM_OCIDLE_STATE(sOCConfig->ocIdleState));

                /* Reset the Output N Polarity level */
                tmpccer &= ~TIM_CCER_CC3NP;

                /* Set the Output N Polarity */
                tmpccer |= (sOCConfig->ocNPolarity << 8U);

                /* Reset the Output N State */
                tmpccer &= ~TIM_CCER_CC3NE;

                /* Reset the Output Compare and Output Compare N IDLE State */
                tmpcr2 &= ~TIM_CR2_OIS3;
                tmpcr2 &= ~TIM_CR2_OIS3N;

                /* Set the Output Idle state */
                tmpcr2 |= (sOCConfig->ocIdleState << 4U);

                /* Set the Output N Idle state */
                tmpcr2 |= (sOCConfig->ocNIdleState << 4U);
            }

            /* Write to TIMx CR2 */
            TIMx->CR2 = tmpcr2;

            /* Write to TIMx CCMR2 */
            TIMx->CCMR2 = tmpccmrx;

            /* Set the Capture Compare Register value */
            TIMx->CCR3 = sOCConfig->pulse;

        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Disable the Channel 4: Reset the CC4E Bit */
            LL_TIM_CC4E_DISABLE(TIMx);

            /* Get the TIMx CCER register value */
            tmpccer = TIMx->CCER;

            /* Get the TIMx CR2 register value */
            tmpcr2 = TIMx->CR2;

            /* Get the TIMx CCMR2 register value */
            tmpccmrx = TIMx->CCMR2;

            /* Reset the Output Compare mode and Capture/Compare selection Bits */
            tmpccmrx &= ~TIM_CCMR2_OC4M;
            tmpccmrx &= ~TIM_CCMR2_CC4S;

            /* Select the Output Compare Mode */
            tmpccmrx |= (sOCConfig->ocMode << 8U);

            /* Reset the Output Polarity level */
            tmpccer &= ~TIM_CCER_CC4P;

            /* Set the Output Compare Polarity */
            tmpccer |= (sOCConfig->ocPolarity << 12U);

            /*if((TIMx == TIM1) || (TIMx == TIM8))*/
            if (IS_TIM_ADVANCED_INSTANCE(TIMx) != RESET)
            {
                assert_param(IS_TIM_OCN_POLARITY(sOCConfig->ocNPolarity));
                assert_param(IS_TIM_OCNIDLE_STATE(sOCConfig->ocNIdleState));
                assert_param(IS_TIM_OCIDLE_STATE(sOCConfig->ocIdleState));

                /* Reset the Output N Polarity level */
                tmpccer &= ~TIM_CCER_CC4NP;

                /* Set the Output N Polarity */
                tmpccer |= (sOCConfig->ocNPolarity << 12U);

                /* Reset the Output N State */
                tmpccer &= ~TIM_CCER_CC4NE;

                /* Reset the Output Compare IDLE State */
                tmpcr2 &= ~TIM_CR2_OIS4;
                tmpcr2 &= ~TIM_CR2_OIS4N;

                /* Set the Output Idle state */
                tmpcr2 |= (sOCConfig->ocIdleState << 6U);

                /* Set the Output N Idle state */
                tmpcr2 |= (sOCConfig->ocNIdleState << 6U);
            }

            /* Write to TIMx CR2 */
            TIMx->CR2 = tmpcr2;

            /* Write to TIMx CCMR2 */
            TIMx->CCMR2 = tmpccmrx;

            /* Set the Capture Compare Register value */
            TIMx->CCR4 = sOCConfig->pulse;

        }
        break;

        default:
            break;
    }

    /* Write to TIMx CCER */
    TIMx->CCER = tmpccer;

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

RET_CODE HAL_TIM_ConfigICChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx,
                                 TIM_InputCaptureInit_T *sConfig, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_IC_POLARITY(sConfig->icPolarity));
    assert_param(IS_TIM_IC_SELECTION(sConfig->icSelection));
    assert_param(IS_TIM_IC_PRESCALER(sConfig->icPrescaler));
    assert_param(IS_TIM_IC_FILTER(sConfig->icFilter));

    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    htim->state = HAL_TIM_STATE_BUSY;

    HAL_TIM_TIConfig(htim->instance, sConfig->icPolarity, sConfig->icSelection,
                     sConfig->icFilter, channel);

    if (channel == TIM_CHANNEL_1)
    {
        /* TI1 Configuration */
        assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

        /* Reset the IC1PSC Bits */
        LL_TIM_IC1PSC_RESET(TIMx);

        /* Set the IC1PSC value */
        TIMx->CCMR1 |= sConfig->icPrescaler;
    }
    else if (channel == TIM_CHANNEL_2)
    {
        /* TI2 Configuration */
        assert_param(IS_TIM_CC2_INSTANCE(htim->instance));

        /* Reset the IC2PSC Bits */
        LL_TIM_IC2PSC_RESET(TIMx);

        /* Set the IC2PSC value */
        TIMx->CCMR1 |= (sConfig->icPrescaler << 8U);
    }
    else if (channel == TIM_CHANNEL_3)
    {
        /* TI3 Configuration */
        assert_param(IS_TIM_CC3_INSTANCE(TIMx));

        /* Reset the IC3PSC Bits */
        LL_TIM_IC3PSC_RESET(TIMx);

        /* Set the IC3PSC value */
        TIMx->CCMR2 |= sConfig->icPrescaler;
    }
    else
    {
        /* TI4 Configuration */
        assert_param(IS_TIM_CC4_INSTANCE(htim->instance));

        /* Reset the IC4PSC Bits */
        LL_TIM_IC4PSC_RESET(TIMx);

        /* Set the IC4PSC value */
        TIMx->CCMR2 |= (sConfig->icPrescaler << 8U);
    }

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

void HAL_TIM_OC1SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig)
{
    uint32_t tmpccmrx = 0;
    uint32_t tmpccer = 0;
    uint32_t tmpcr2 = 0;

    /* Disable the Channel 1: Reset the CC1E Bit */
    TIMx->CCER &= ~TIM_CCER_CC1E;

    /* Get the TIMx CCER register value */
    tmpccer = TIMx->CCER;
    /* Get the TIMx CR2 register value */
    tmpcr2 =  TIMx->CR2;

    /* Get the TIMx CCMR1 register value */
    tmpccmrx = TIMx->CCMR1;

    /* Reset the Output Compare Mode Bits */
    tmpccmrx &= ~TIM_CCMR1_OC1M;
    tmpccmrx &= ~TIM_CCMR1_CC1S;
    /* Select the Output Compare Mode */
    tmpccmrx |= sOCConfig->ocMode;

    /* Reset the Output Polarity level */
    tmpccer &= ~TIM_CCER_CC1P;
    /* Set the Output Compare Polarity */
    tmpccer |= sOCConfig->ocPolarity;

    if (IS_TIM_CCXN_INSTANCE(htim->instance, TIM_CHANNEL_1))
    {
        /* Check parameters */
        assert_param(IS_TIM_OCN_POLARITY(sOCConfig->ocNPolarity));

        /* Reset the Output N Polarity level */
        tmpccer &= ~TIM_CCER_CC1NP;
        /* Set the Output N Polarity */
        tmpccer |= sOCConfig->ocNPolarity;
        /* Reset the Output N State */
        tmpccer &= ~TIM_CCER_CC1NE;
    }

    if (IS_TIM_BREAK_INSTANCE(TIMx))
    {
        /* Check parameters */
        assert_param(IS_TIM_OCNIDLE_STATE(sOCConfig->ocNIdleState));
        assert_param(IS_TIM_OCIDLE_STATE(sOCConfig->ocIdleState));

        /* Reset the Output Compare and Output Compare N IDLE State */
        tmpcr2 &= ~TIM_CR2_OIS1;
        tmpcr2 &= ~TIM_CR2_OIS1N;
        /* Set the Output Idle state */
        tmpcr2 |= sOCConfig->ocIdleState;
        /* Set the Output N Idle state */
        tmpcr2 |= sOCConfig->ocNIdleState;
    }
    /* Write to TIMx CR2 */
    TIMx->CR2 = tmpcr2;

    /* Write to TIMx CCMR1 */
    TIMx->CCMR1 = tmpccmrx;

    /* Set the Capture Compare Register value */
    TIMx->CCR1 = sOCConfig->pulse;

    /* Write to TIMx CCER */
    TIMx->CCER = tmpccer;
}

/**
  * @brief  Time Ouput Compare 2 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config: The ouput configuration structure
  * @retval None
  */
void HAL_TIM_OC2SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig)
{
    uint32_t tmpccmrx = 0;
    uint32_t tmpccer = 0;
    uint32_t tmpcr2 = 0;

    /* Disable the Channel 2: Reset the CC2E Bit */
    TIMx->CCER &= ~TIM_CCER_CC2E;

    /* Get the TIMx CCER register value */
    tmpccer = TIMx->CCER;
    /* Get the TIMx CR2 register value */
    tmpcr2 =  TIMx->CR2;

    /* Get the TIMx CCMR1 register value */
    tmpccmrx = TIMx->CCMR1;

    /* Reset the Output Compare mode and Capture/Compare selection Bits */
    tmpccmrx &= ~TIM_CCMR1_OC2M;
    tmpccmrx &= ~TIM_CCMR1_CC2S;

    /* Select the Output Compare Mode */
    tmpccmrx |= (sOCConfig->ocMode << 8);

    /* Reset the Output Polarity level */
    tmpccer &= ~TIM_CCER_CC2P;
    /* Set the Output Compare Polarity */
    tmpccer |= (sOCConfig->ocPolarity << 4);

    if (IS_TIM_CCXN_INSTANCE(htim->instance, TIM_CHANNEL_2))
    {
        assert_param(IS_TIM_OCN_POLARITY(sOCConfig->ocNPolarity));

        /* Reset the Output N Polarity level */
        tmpccer &= ~TIM_CCER_CC2NP;
        /* Set the Output N Polarity */
        tmpccer |= (sOCConfig->ocNPolarity << 4);
        /* Reset the Output N State */
        tmpccer &= ~TIM_CCER_CC2NE;

    }

    if (IS_TIM_BREAK_INSTANCE(TIMx))
    {
        /* Check parameters */
        assert_param(IS_TIM_OCNIDLE_STATE(sOCConfig->ocNIdleState));
        assert_param(IS_TIM_OCIDLE_STATE(sOCConfig->ocIdleState));

        /* Reset the Output Compare and Output Compare N IDLE State */
        tmpcr2 &= ~TIM_CR2_OIS2;
        tmpcr2 &= ~TIM_CR2_OIS2N;
        /* Set the Output Idle state */
        tmpcr2 |= (sOCConfig->ocIdleState << 2);
        /* Set the Output N Idle state */
        tmpcr2 |= (sOCConfig->ocNIdleState << 2);
    }

    /* Write to TIMx CR2 */
    TIMx->CR2 = tmpcr2;

    /* Write to TIMx CCMR1 */
    TIMx->CCMR1 = tmpccmrx;

    /* Set the Capture Compare Register value */
    TIMx->CCR2 = sOCConfig->pulse;

    /* Write to TIMx CCER */
    TIMx->CCER = tmpccer;
}

/**
  * @brief  Time Ouput Compare 3 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config: The ouput configuration structure
  * @retval None
  */
void HAL_TIM_OC3SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig)
{
    uint32_t tmpccmrx = 0;
    uint32_t tmpccer = 0;
    uint32_t tmpcr2 = 0;

    /* Disable the Channel 3: Reset the CC2E Bit */
    TIMx->CCER &= ~TIM_CCER_CC3E;

    /* Get the TIMx CCER register value */
    tmpccer = TIMx->CCER;
    /* Get the TIMx CR2 register value */
    tmpcr2 =  TIMx->CR2;

    /* Get the TIMx CCMR2 register value */
    tmpccmrx = TIMx->CCMR2;

    /* Reset the Output Compare mode and Capture/Compare selection Bits */
    tmpccmrx &= ~TIM_CCMR2_OC3M;
    tmpccmrx &= ~TIM_CCMR2_CC3S;
    /* Select the Output Compare Mode */
    tmpccmrx |= sOCConfig->ocMode;

    /* Reset the Output Polarity level */
    tmpccer &= ~TIM_CCER_CC3P;
    /* Set the Output Compare Polarity */
    tmpccer |= (sOCConfig->ocPolarity << 8);

    if (IS_TIM_CCXN_INSTANCE(htim->instance, TIM_CHANNEL_3))
    {
        assert_param(IS_TIM_OCN_POLARITY(sOCConfig->ocNPolarity));

        /* Reset the Output N Polarity level */
        tmpccer &= ~TIM_CCER_CC3NP;
        /* Set the Output N Polarity */
        tmpccer |= (sOCConfig->ocNPolarity << 8);
        /* Reset the Output N State */
        tmpccer &= ~TIM_CCER_CC3NE;
    }

    if (IS_TIM_BREAK_INSTANCE(TIMx))
    {
        /* Check parameters */
        assert_param(IS_TIM_OCNIDLE_STATE(sOCConfig->ocNIdleState));
        assert_param(IS_TIM_OCIDLE_STATE(sOCConfig->ocIdleState));

        /* Reset the Output Compare and Output Compare N IDLE State */
        tmpcr2 &= ~TIM_CR2_OIS3;
        tmpcr2 &= ~TIM_CR2_OIS3N;
        /* Set the Output Idle state */
        tmpcr2 |= (sOCConfig->ocIdleState << 4);
        /* Set the Output N Idle state */
        tmpcr2 |= (sOCConfig->ocNIdleState << 4);
    }

    /* Write to TIMx CR2 */
    TIMx->CR2 = tmpcr2;

    /* Write to TIMx CCMR2 */
    TIMx->CCMR2 = tmpccmrx;

    /* Set the Capture Compare Register value */
    TIMx->CCR3 = sOCConfig->pulse;

    /* Write to TIMx CCER */
    TIMx->CCER = tmpccer;
}

/**
  * @brief  Time Ouput Compare 4 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config: The ouput configuration structure
  * @retval None
  */
void HAL_TIM_OC4SetConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OutputCompareInit_T *sOCConfig)
{
    uint32_t tmpccmrx = 0;
    uint32_t tmpccer = 0;
    uint32_t tmpcr2 = 0;

    /* Disable the Channel 4: Reset the CC4E Bit */
    TIMx->CCER &= ~TIM_CCER_CC4E;

    /* Get the TIMx CCER register value */
    tmpccer = TIMx->CCER;
    /* Get the TIMx CR2 register value */
    tmpcr2 =  TIMx->CR2;

    /* Get the TIMx CCMR2 register value */
    tmpccmrx = TIMx->CCMR2;

    /* Reset the Output Compare mode and Capture/Compare selection Bits */
    tmpccmrx &= ~TIM_CCMR2_OC4M;
    tmpccmrx &= ~TIM_CCMR2_CC4S;

    /* Select the Output Compare Mode */
    tmpccmrx |= (sOCConfig->ocMode << 8);

    /* Reset the Output Polarity level */
    tmpccer &= ~TIM_CCER_CC4P;
    /* Set the Output Compare Polarity */
    tmpccer |= (sOCConfig->ocPolarity << 12);

    if (IS_TIM_BREAK_INSTANCE(TIMx))
    {
        assert_param(IS_TIM_OCIDLE_STATE(sOCConfig->ocIdleState));

        /* Reset the Output Compare IDLE State */
        tmpcr2 &= ~TIM_CR2_OIS4;
        /* Set the Output Idle state */
        tmpcr2 |= (sOCConfig->ocIdleState << 6);
    }

    /* Write to TIMx CR2 */
    TIMx->CR2 = tmpcr2;

    /* Write to TIMx CCMR2 */
    TIMx->CCMR2 = tmpccmrx;

    /* Set the Capture Compare Register value */
    TIMx->CCR4 = sOCConfig->pulse;

    /* Write to TIMx CCER */
    TIMx->CCER = tmpccer;
}

RET_CODE HAL_TIM_ConfigPWMChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx,
                                  TIM_OutputCompareInit_T *sConfig, uint32_t channel)
{

    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    /* Check the parameters */
    assert_param(IS_TIM_CHANNELS(channel));
    assert_param(IS_TIM_PWM_MODE(sConfig->ocMode));
    assert_param(IS_TIM_OC_POLARITY(sConfig->ocPolarity));
    assert_param(IS_TIM_FAST_STATE(sConfig->ocFastMode));

    htim->state = HAL_TIM_STATE_BUSY;

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

            HAL_TIM_OC1SetConfig(htim, TIMx, sConfig);

            /* Set the Preload enable bit for channel1 */
            LL_TIM_OC1PE_ENABLE(TIMx);

            /* Configure the Output Fast mode */
            LL_TIM_OC1FE_DISABLE(TIMx);
            TIMx->CCMR1 |= sConfig->ocFastMode;
        }
        break;

        case TIM_CHANNEL_2:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));

            HAL_TIM_OC2SetConfig(htim, TIMx, sConfig);

            /* Set the Preload enable bit for channel2 */
            LL_TIM_OC2PE_ENABLE(TIMx);

            /* Configure the Output Fast mode */
            LL_TIM_OC2FE_DISABLE(TIMx);
            TIMx->CCMR1 |= sConfig->ocFastMode << 8U;
        }
        break;

        case TIM_CHANNEL_3:
        {
            assert_param(IS_TIM_CC3_INSTANCE(htim->instance));

            HAL_TIM_OC3SetConfig(htim, TIMx, sConfig);

            /* Set the Preload enable bit for channel3 */
            LL_TIM_OC3PE_ENABLE(TIMx);

            /* Configure the Output Fast mode */
            LL_TIM_OC3FE_DISABLE(TIMx);
            TIMx->CCMR2 |= sConfig->ocFastMode;
        }
        break;

        case TIM_CHANNEL_4:
        {
            assert_param(IS_TIM_CC4_INSTANCE(htim->instance));

            HAL_TIM_OC4SetConfig(htim, TIMx, sConfig);

            /* Set the Preload enable bit for channel4 */
            LL_TIM_OC4PE_ENABLE(TIMx);

            /* Configure the Output Fast mode */
            LL_TIM_OC4FE_DISABLE(TIMx);
            TIMx->CCMR2 |= sConfig->ocFastMode << 8U;
        }
        break;

        default:
            break;
    }

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

void HAL_TIM_TI1InputStageConfig(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICFilter)
{
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccer = 0U;

    /* Disable the Channel 1: Reset the CC1E Bit */
    tmpccer = TIMx->CCER;
    LL_TIM_CC1E_DISABLE(TIMx);
    tmpccmr1 = TIMx->CCMR1;

    /* Set the filter */
    tmpccmr1 &= ~TIM_CCMR1_IC1F;
    tmpccmr1 |= (TIM_ICFilter << 4U);

    /* Select the Polarity and set the CC1E Bit */
    tmpccer &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    tmpccer |= TIM_ICPolarity;

    /* Write to TIMx CCMR1 and CCER registers */
    TIMx->CCMR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

void HAL_TIM_TI2Config(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter)
{
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccer = 0U;

    /* Disable the Channel 2: Reset the CC2E Bit */
    LL_TIM_CC2E_DISABLE(TIMx);
    tmpccmr1 = TIMx->CCMR1;
    tmpccer = TIMx->CCER;

    /* Select the Input */
    tmpccmr1 &= ~TIM_CCMR1_CC2S;
    tmpccmr1 |= (TIM_ICSelection << 8U);

    /* Set the filter */
    tmpccmr1 &= ~TIM_CCMR1_IC2F;
    tmpccmr1 |= ((TIM_ICFilter << 12U) & TIM_CCMR1_IC2F);

    /* Select the Polarity and set the CC2E Bit */
    tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
    tmpccer |= ((TIM_ICPolarity << 4U) & (TIM_CCER_CC2P | TIM_CCER_CC2NP));

    /* Write to TIMx CCMR1 and CCER registers */
    TIMx->CCMR1 = tmpccmr1 ;
    TIMx->CCER = tmpccer;
}

void HAL_TIM_TI2InputStageConfig(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICFilter)
{
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccer = 0U;

    /* Disable the Channel 2: Reset the CC2E Bit */
    LL_TIM_CC2E_DISABLE(TIMx);
    tmpccmr1 = TIMx->CCMR1;
    tmpccer = TIMx->CCER;

    /* Set the filter */
    tmpccmr1 &= ~TIM_CCMR1_IC2F;
    tmpccmr1 |= (TIM_ICFilter << 12U);

    /* Select the Polarity and set the CC2E Bit */
    tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
    tmpccer |= (TIM_ICPolarity << 4U);

    /* Write to TIMx CCMR1 and CCER registers */
    TIMx->CCMR1 = tmpccmr1 ;
    TIMx->CCER = tmpccer;
}

void HAL_TIM_TI3Config(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter)
{
    uint32_t tmpccmr2 = 0U;
    uint32_t tmpccer = 0U;

    /* Disable the Channel 3: Reset the CC3E Bit */
    LL_TIM_CC3E_DISABLE(TIMx);
    tmpccmr2 = TIMx->CCMR2;
    tmpccer = TIMx->CCER;

    /* Select the Input */
    tmpccmr2 &= ~TIM_CCMR2_CC3S;
    tmpccmr2 |= TIM_ICSelection;

    /* Set the filter */
    tmpccmr2 &= ~TIM_CCMR2_IC3F;
    tmpccmr2 |= ((TIM_ICFilter << 4U) & TIM_CCMR2_IC3F);

    /* Select the Polarity and set the CC3E Bit */
    tmpccer &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
    tmpccer |= ((TIM_ICPolarity << 8U) & (TIM_CCER_CC3P | TIM_CCER_CC3NP));

    /* Write to TIMx CCMR2 and CCER registers */
    TIMx->CCMR2 = tmpccmr2;
    TIMx->CCER = tmpccer;
}

void HAL_TIM_TI4Config(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter)
{
    uint32_t tmpccmr2 = 0U;
    uint32_t tmpccer = 0U;

    /* Disable the Channel 4: Reset the CC4E Bit */
    LL_TIM_CC4E_DISABLE(TIMx);
    tmpccmr2 = TIMx->CCMR2;
    tmpccer = TIMx->CCER;

    /* Select the Input */
    tmpccmr2 &= ~TIM_CCMR2_CC4S;
    tmpccmr2 |= (TIM_ICSelection << 8U);

    /* Set the filter */
    tmpccmr2 &= ~TIM_CCMR2_IC4F;
    tmpccmr2 |= ((TIM_ICFilter << 12U) & TIM_CCMR2_IC4F);

    /* Select the Polarity and set the CC4E Bit */
    tmpccer &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
    tmpccer |= ((TIM_ICPolarity << 12U) & (TIM_CCER_CC4P | TIM_CCER_CC4NP));

    /* Write to TIMx CCMR2 and CCER registers */
    TIMx->CCMR2 = tmpccmr2;
    TIMx->CCER = tmpccer ;
}

RET_CODE HAL_TIM_ConfigOPMChannel(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_OnePulseInit_T *sConfig,
                                  uint32_t outputChannel,  uint32_t inputChannel)
{
    TIM_OutputCompareInit_T temp1;

    /* Check the parameters */
    assert_param(IS_TIM_OPM_CHANNELS(outputChannel));
    assert_param(IS_TIM_OPM_CHANNELS(inputChannel));

    if (outputChannel != inputChannel)
    {
        if (htim->lock == HAL_TIM_LOCKED)
        {
            return HAL_BUSY;
        }
        else
        {
            htim->lock = HAL_TIM_LOCKED;
        }

        htim->state = HAL_TIM_STATE_BUSY;

        /* Extract the Output compare configuration from sConfig structure */
        temp1.ocMode = sConfig->ocMode;
        temp1.pulse = sConfig->pulse;
        temp1.ocPolarity = sConfig->ocPolarity;
        temp1.ocNPolarity = sConfig->ocNPolarity;
        temp1.ocIdleState = sConfig->ocIdleState;
        temp1.ocNIdleState = sConfig->ocNIdleState;

        switch (outputChannel)
        {
            case TIM_CHANNEL_1:
            {
                assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
                HAL_TIM_ConfigOCChannel(htim, TIMx, &temp1, TIM_CHANNEL_1);
            }
            break;

            case TIM_CHANNEL_2:
            {
                assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
                HAL_TIM_ConfigOCChannel(htim, TIMx, &temp1, TIM_CHANNEL_2);
            }
            break;
            default:
                break;
        }

        switch (inputChannel)
        {
            case TIM_CHANNEL_1:
            {
                assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
                HAL_TIM_TIConfig(htim->instance, sConfig->icPolarity, sConfig->icSelection,
                                 sConfig->icFilter, TIM_CHANNEL_1);

                /* Reset the IC1PSC Bits */
                LL_TIM_IC1PSC_RESET(TIMx);

                /* Select the Trigger source */
                TIMx->SMCR &= ~TIM_SMCR_TS;
                TIMx->SMCR |= TIM_TS_TI1FP1;

                /* Select the Slave Mode */
                TIMx->SMCR &= ~TIM_SMCR_SMS;
                TIMx->SMCR |= TIM_SLAVEMODE_TRIGGER;
            }
            break;

            case TIM_CHANNEL_2:
            {
                assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
                HAL_TIM_TIConfig(htim->instance, sConfig->icPolarity, sConfig->icSelection,
                                 sConfig->icFilter, TIM_CHANNEL_2);

                /* Reset the IC2PSC Bits */
                LL_TIM_IC2PSC_RESET(TIMx);

                /* Select the Trigger source */
                TIMx->SMCR &= ~TIM_SMCR_TS;
                TIMx->SMCR |= TIM_TS_TI2FP2;

                /* Select the Slave Mode */
                TIMx->SMCR &= ~TIM_SMCR_SMS;
                TIMx->SMCR |= TIM_SLAVEMODE_TRIGGER;
            }
            break;

            default:
                break;
        }

        htim->state = HAL_TIM_STATE_READY;
        htim->lock = HAL_TIM_UNLOCKED;

        return RET_OK;
    }
    else
    {
        return RET_ERROR;
    }
}

RET_CODE HAL_TIM_ConfigOCrefClear(TIM_Handle_T *htim, TIM_Device_T *TIMx,
                                  TIM_ClearInputConfig_T *sClearInputConfig, uint32_t channel)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_CHANNELS(channel));
    assert_param(IS_TIM_CLEARINPUT_SOURCE(sClearInputConfig->clearInputSource));
    assert_param(IS_TIM_CLEARINPUT_POLARITY(sClearInputConfig->clearInputPolarity));
    assert_param(IS_TIM_CLEARINPUT_PRESCALER(sClearInputConfig->clearInputPrescaler));
    assert_param(IS_TIM_CLEARINPUT_FILTER(sClearInputConfig->clearInputFilter));

    /* Process Locked */
    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    htim->state = HAL_TIM_STATE_BUSY;

    if (sClearInputConfig->clearInputSource == TIM_CLEARINPUTSOURCE_ETR)
    {
        HAL_TIM_ETRConfig(htim->instance,
                          sClearInputConfig->clearInputPrescaler,
                          sClearInputConfig->clearInputPolarity,
                          sClearInputConfig->clearInputFilter);
    }

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            if (sClearInputConfig->clearInputState != RESET)
            {
                /* Enable the Ocref clear feature for Channel 1 */
                LL_TIM_OC1CE_ENABLE(TIMx);
            }
            else
            {
                /* Disable the Ocref clear feature for Channel 1 */
                LL_TIM_OC1CE_DISABLE(TIMx);
            }
        }
        break;

        case TIM_CHANNEL_2:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
            if (sClearInputConfig->clearInputState != RESET)
            {
                /* Enable the Ocref clear feature for Channel 2 */
                LL_TIM_OC2CE_ENABLE(TIMx);
            }
            else
            {
                /* Disable the Ocref clear feature for Channel 2 */
                LL_TIM_OC2CE_DISABLE(TIMx);
            }
        }
        break;

        case TIM_CHANNEL_3:
        {
            assert_param(IS_TIM_CC3_INSTANCE(htim->instance));
            if (sClearInputConfig->clearInputState != RESET)
            {
                /* Enable the Ocref clear feature for Channel 3 */
                LL_TIM_OC3CE_ENABLE(TIMx);
            }
            else
            {
                /* Disable the Ocref clear feature for Channel 3 */
                LL_TIM_OC3CE_DISABLE(TIMx);
            }
        }
        break;

        case TIM_CHANNEL_4:
        {
            assert_param(IS_TIM_CC4_INSTANCE(htim->instance));
            if (sClearInputConfig->clearInputState != RESET)
            {
                /* Enable the Ocref clear feature for Channel 4 */
                LL_TIM_OC4CE_ENABLE(TIMx);
            }
            else
            {
                /* Disable the Ocref clear feature for Channel 4 */
                LL_TIM_OC4CE_DISABLE(TIMx);
            }
        }
        break;

        default:
            break;
    }

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

RET_CODE HAL_TIM_ConfigClockSource(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_ClockConfig_T *sClockSourceConfig)
{
    uint32_t tmpsmcr = 0U;

    /* Process Locked */
    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    htim->state = HAL_TIM_STATE_BUSY;

    /* Check the parameters */
    assert_param(IS_TIM_CLOCKSOURCE(sClockSourceConfig->clockSource));

    /* Reset the SMS, TS, ECE, ETPS and ETRF bits */
    tmpsmcr = TIMx->SMCR;
    tmpsmcr &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);
    tmpsmcr &= ~(TIM_SMCR_ETF | TIM_SMCR_ETPS | TIM_SMCR_ECE | TIM_SMCR_ETP);
    TIMx->SMCR = tmpsmcr;

    switch (sClockSourceConfig->clockSource)
    {
        case TIM_CLOCKSOURCE_INTERNAL:
        {
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

            /* Disable slave mode to clock the prescaler directly with the internal clock */
            LL_TIM_SMS_DISABLE(TIMx);
        }
        break;

        case TIM_CLOCKSOURCE_ETRMODE1:
        {
            assert_param(IS_TIM_ETR_INSTANCE(htim->instance));
            assert_param(IS_TIM_CLOCKPOLARITY(sClockSourceConfig->clockPolarity));
            assert_param(IS_TIM_CLOCKPRESCALER(sClockSourceConfig->clockPrescaler));
            assert_param(IS_TIM_CLOCKFILTER(sClockSourceConfig->clockFilter));

            /* Configure the ETR Clock source */
            HAL_TIM_ETRConfig(htim->instance,
                              sClockSourceConfig->clockPrescaler,
                              sClockSourceConfig->clockPolarity,
                              sClockSourceConfig->clockFilter);

            /* Get the TIMx SMCR register value */
            tmpsmcr = TIMx->SMCR;

            /* Reset the SMS and TS Bits */
            tmpsmcr &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);

            /* Select the External clock mode1 and the ETRF trigger */
            tmpsmcr |= (TIM_SLAVEMODE_EXTERNAL1 | TIM_CLOCKSOURCE_ETRMODE1);

            /* Write to TIMx SMCR */
            TIMx->SMCR = tmpsmcr;
        }
        break;

        case TIM_CLOCKSOURCE_ETRMODE2:
        {
            assert_param(IS_TIM_ETR_INSTANCE(htim->instance));
            assert_param(IS_TIM_CLOCKPOLARITY(sClockSourceConfig->clockPolarity));
            assert_param(IS_TIM_CLOCKPRESCALER(sClockSourceConfig->clockPrescaler));
            assert_param(IS_TIM_CLOCKFILTER(sClockSourceConfig->clockFilter));

            /* Configure the ETR Clock source */
            HAL_TIM_ETRConfig(htim->instance,
                              sClockSourceConfig->clockPrescaler,
                              sClockSourceConfig->clockPolarity,
                              sClockSourceConfig->clockFilter);

            /* Enable the External clock mode2 */
            LL_TIM_ECE_ENABLE(TIMx);
        }
        break;

        case TIM_CLOCKSOURCE_TI1:
        {
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

            /* Check TI1 input conditioning related parameters */
            assert_param(IS_TIM_CLOCKPOLARITY(sClockSourceConfig->clockPolarity));
            assert_param(IS_TIM_CLOCKFILTER(sClockSourceConfig->clockFilter));

            HAL_TIM_TIInputStageConfig(htim->instance,
                                       sClockSourceConfig->clockPolarity,
                                       sClockSourceConfig->clockFilter, TIM_CHANNEL_1);

            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_TI1);
        }
        break;

        case TIM_CLOCKSOURCE_TI2:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));

            /* Check TI1 input conditioning related parameters */
            assert_param(IS_TIM_CLOCKPOLARITY(sClockSourceConfig->clockPolarity));
            assert_param(IS_TIM_CLOCKFILTER(sClockSourceConfig->clockFilter));

            HAL_TIM_TIInputStageConfig(htim->instance,
                                       sClockSourceConfig->clockPolarity,
                                       sClockSourceConfig->clockFilter, TIM_CHANNEL_2);

            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_TI2);
        }
        break;

        case TIM_CLOCKSOURCE_TI1ED:
        {
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

            /* Check TI1 input conditioning related parameters */
            assert_param(IS_TIM_CLOCKPOLARITY(sClockSourceConfig->clockPolarity));
            assert_param(IS_TIM_CLOCKFILTER(sClockSourceConfig->clockFilter));

            HAL_TIM_TIInputStageConfig(htim->instance,
                                       sClockSourceConfig->clockPolarity,
                                       sClockSourceConfig->clockFilter, TIM_CHANNEL_1);

            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_TI1ED);
        }
        break;

        case TIM_CLOCKSOURCE_ITR0:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_ITR0);
        }
        break;

        case TIM_CLOCKSOURCE_ITR1:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_ITR1);
        }
        break;

        case TIM_CLOCKSOURCE_ITR2:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_ITR2);
        }
        break;

        case TIM_CLOCKSOURCE_ITR3:
        {
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
            HAL_TIM_ITRxConfig(htim->instance, TIM_CLOCKSOURCE_ITR3);
        }
        break;

        default:
            break;
    }

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

RET_CODE HAL_TIM_ConfigMasterSyn(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_MasterConfig_T *sMasterConfig)
{
    /* Check the parameters */
    assert_param(IS_TIM_MASTER_INSTANCE(htim->instance));
    assert_param(IS_TIM_TRGO_SOURCE(sMasterConfig->masterOutputTrigger));
    assert_param(IS_TIM_MSM_STATE(sMasterConfig->masterSlaveMode));

    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    htim->state = HAL_TIM_STATE_BUSY;

    /* Reset the MMS Bits */
    TIMx->CR2 &= ~TIM_CR2_MMS;

    /* Select the TRGO source */
    TIMx->CR2 |= sMasterConfig->masterOutputTrigger;

    /* Reset the MSM Bit */
    TIMx->SMCR &= ~TIM_SMCR_MSM;

    /* Set or Reset the MSM Bit */
    TIMx->SMCR |= sMasterConfig->masterSlaveMode;

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

RET_CODE HAL_TIM_ConfigSlaveSyn(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_SlaveConfig_T *sSlaveConfig)
{
    /* Check the parameters */
    assert_param(IS_TIM_SLAVE_INSTANCE(htim->instance));
    assert_param(IS_TIM_SLAVE_MODE(sSlaveConfig->slaveMode));
    assert_param(IS_TIM_TRIGGER_SELECTION(sSlaveConfig->inputTrigger));

    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    htim->state = HAL_TIM_STATE_BUSY;
    HAL_TIM_SlaveTimerConfig(htim, TIMx, sSlaveConfig);

    /* Disable Trigger Interrupt */
    LL_TIM_TIE_DISABLE(TIMx);

    /* Disable Trigger DMA request */
    LL_TIM_TDE_DISABLE(TIMx);

    htim->state = HAL_TIM_STATE_READY;
    htim->lock = HAL_TIM_LOCKED;

    return RET_OK;
}

void HAL_TIM_SlaveTimerConfig(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_SlaveConfig_T *sSlaveConfig)
{
    uint32_t tmpsmcr = 0U;
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccer = 0U;

    /* Get the TIMx SMCR register value */
    tmpsmcr = TIMx->SMCR;

    /* Reset the Trigger Selection Bits */
    tmpsmcr &= ~TIM_SMCR_TS;

    /* Set the Input Trigger source */
    tmpsmcr |= sSlaveConfig->inputTrigger;

    /* Reset the slave mode Bits */
    tmpsmcr &= ~TIM_SMCR_SMS;

    /* Set the slave mode */
    tmpsmcr |= sSlaveConfig->slaveMode;

    /* Write to TIMx SMCR */
    TIMx->SMCR = tmpsmcr;

    /* Configure the trigger prescaler, filter, and polarity */
    switch (sSlaveConfig->inputTrigger)
    {
        case TIM_TS_ETRF:
        {
            /* Check the parameters */
            assert_param(IS_TIM_ETR_INSTANCE(htim->instance));
            assert_param(IS_TIM_TRIGGERPRESCALER(sSlaveConfig->triggerPrescaler));
            assert_param(IS_TIM_TRIGGERPOLARITY(sSlaveConfig->triggerPolarity));
            assert_param(IS_TIM_TRIGGERFILTER(sSlaveConfig->triggerFilter));

            /* Configure the ETR Trigger source */
            HAL_TIM_ETRConfig(htim->instance,
                              sSlaveConfig->triggerPrescaler,
                              sSlaveConfig->triggerPolarity,
                              sSlaveConfig->triggerFilter);
        }
        break;

        case TIM_TS_TI1F_ED:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
            assert_param(IS_TIM_TRIGGERFILTER(sSlaveConfig->triggerFilter));

            /* Disable the Channel 1: Reset the CC1E Bit */
            tmpccer = TIMx->CCER;
            TIMx->CCER &= ~TIM_CCER_CC1E;
            tmpccmr1 = TIMx->CCMR1;

            /* Set the filter */
            tmpccmr1 &= ~TIM_CCMR1_IC1F;
            tmpccmr1 |= ((sSlaveConfig->triggerFilter) << 4U);

            /* Write to TIMx CCMR1 and CCER registers */
            TIMx->CCMR1 = tmpccmr1;
            TIMx->CCER = tmpccer;
        }
        break;

        case TIM_TS_TI1FP1:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
            assert_param(IS_TIM_TRIGGERPOLARITY(sSlaveConfig->triggerPolarity));
            assert_param(IS_TIM_TRIGGERFILTER(sSlaveConfig->triggerFilter));

            /* Configure TI1 Filter and Polarity */
            HAL_TIM_TIInputStageConfig(htim->instance,
                                       sSlaveConfig->triggerPolarity,
                                       sSlaveConfig->triggerFilter, TIM_CHANNEL_1);
        }
        break;

        case TIM_TS_TI2FP2:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
            assert_param(IS_TIM_TRIGGERPOLARITY(sSlaveConfig->triggerPolarity));
            assert_param(IS_TIM_TRIGGERFILTER(sSlaveConfig->triggerFilter));

            /* Configure TI2 Filter and Polarity */
            HAL_TIM_TIInputStageConfig(htim->instance,
                                       sSlaveConfig->triggerPolarity,
                                       sSlaveConfig->triggerFilter, TIM_CHANNEL_2);
        }
        break;

        case TIM_TS_ITR0:
        {
            /* Check the parameter */
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
        }
        break;

        case TIM_TS_ITR1:
        {
            /* Check the parameter */
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
        }
        break;

        case TIM_TS_ITR2:
        {
            /* Check the parameter */
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
        }
        break;

        case TIM_TS_ITR3:
        {
            /* Check the parameter */
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));
        }
        break;

        default:
            break;
    }
}

RET_CODE HAL_TIM_GenerateEvent(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t eventSource)
{
    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(htim->instance));
    assert_param(IS_TIM_EVENT_SOURCE(eventSource));

    /* Process Locked */
    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    /* Change the TIM state */
    htim->state = HAL_TIM_STATE_BUSY;

    /* Set the event sources */
    TIMx->EGR = eventSource;

    /* Change the TIM state */
    htim->state = HAL_TIM_STATE_READY;

    htim->lock = HAL_TIM_UNLOCKED;

    /* Return function status */
    return RET_OK;
}

uint32_t HAL_TIM_ReadCapturedValue(TIM_Handle_T *htim, TIM_Device_T *TIMx, uint32_t channel)
{
    uint32_t tmpreg = 0U;

    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC1_INSTANCE(htim->instance));

            /* Return the capture 1 value */
            tmpreg = TIMx->CCR1;
            break;
        }

        case TIM_CHANNEL_2:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC2_INSTANCE(htim->instance));

            /* Return the capture 2 value */
            tmpreg = TIMx->CCR2;
            break;
        }

        case TIM_CHANNEL_3:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC3_INSTANCE(htim->instance));

            /* Return the capture 3 value */
            tmpreg = TIMx->CCR3;
            break;
        }

        case TIM_CHANNEL_4:
        {
            /* Check the parameters */
            assert_param(IS_TIM_CC4_INSTANCE(htim->instance));

            /* Return the capture 4 value */
            tmpreg = TIMx->CCR4;
            break;
        }

        default:
            break;
    }

    htim->lock = HAL_TIM_UNLOCKED;
    return tmpreg;
}

void HAL_TIM_TimeBaseConfig(TIM_Device_T *TIMx, TIM_TimeBaseInit_T *structure)
{
    uint32_t tmpcr1 = 0U;
    tmpcr1 = TIMx->CR1;

    /* Set TIM Time Base Unit parameters ---------------------------------------*/
    if (IS_TIM_CC4_INSTANCE(TIMx) != RESET)
    {
        /* Select the Counter Mode */
        tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
        tmpcr1 |= structure->counterMode;
    }

    if (IS_TIM_CC1_INSTANCE(TIMx) != RESET)
    {
        /* Set the clock division */
        tmpcr1 &= ~TIM_CR1_CKD;
        tmpcr1 |= (uint32_t)structure->clockDivision;
    }

    TIMx->CR1 = tmpcr1;

    /* Set the Auto-reload value */
    TIMx->ARR = (uint32_t)structure->period ;

    /* Set the Prescaler value */
    TIMx->PSC = (uint32_t)structure->prescaler;

    if (IS_TIM_ADVANCED_INSTANCE(TIMx) != RESET)
    {
        /* Set the Repetition Counter value */
        TIMx->RCR = structure->repetitionCounter;
    }

    /* Generate an update event to reload the prescaler
       and the repetition counter(only for TIM1 and TIM8) value immediately */
    TIMx->EGR = TIM_EGR_UG;
}

void HAL_TIM_CCxChannelCmd(TIM_Device_T *TIMx, uint32_t channel, uint32_t channelState)
{
    uint32_t tmp = 0U;

    /* Check the parameters */
    assert_param(IS_TIM_CC1_INSTANCE(TIMx));
    assert_param(IS_TIM_CHANNELS(channel));
    tmp = TIM_CCER_CC1E << channel;

    /* Reset the CCxE Bit */
    TIMx->CCER &= ~tmp;

    /* Set or reset the CCxE Bit */
    TIMx->CCER |= (uint32_t)(channelState << channel);
}

void HAL_TIM_CCxNChannelCmd(TIM_Device_T *TIMx, uint32_t channel, uint32_t channelNState)
{
    uint32_t tmp = 0U;

    /* Check the parameters */
    assert_param(IS_TIM_CC4_INSTANCE(TIMx));
    assert_param(IS_TIM_COMPLEMENTARY_CHANNELS(channel));
    tmp = TIM_CCER_CC1NE << channel;

    /* Reset the CCxNE Bit */
    TIMx->CCER &= ~tmp;

    /* Set or reset the CCxNE Bit */
    TIMx->CCER |= (uint32_t)(channelNState << channel);
}

void HAL_TIM_TIConfig(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter, uint32_t channel)
{
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccmr2 = 0U;
    uint32_t tmpccer = 0U;

    switch (channel)
    {
        case TIM_CHANNEL_1:
        {
            /* Disable the Channel 1: Reset the CC1E Bit */
            LL_TIM_CC1E_DISABLE(TIMx);
            tmpccmr1 = TIMx->CCMR1;
            tmpccer = TIMx->CCER;

            /* Select the Input */
            if (IS_TIM_CC2_INSTANCE(TIMx) != RESET)
            {
                tmpccmr1 &= ~TIM_CCMR1_CC1S;
                tmpccmr1 |= TIM_ICSelection;
            }
            else
            {
                tmpccmr1 &= ~TIM_CCMR1_CC1S;
                tmpccmr1 |= TIM_CCMR1_CC1S_0;
            }

            /* Set the filter */
            tmpccmr1 &= ~TIM_CCMR1_IC1F;
            tmpccmr1 |= ((TIM_ICFilter << 4U) & TIM_CCMR1_IC1F);

            /* Select the Polarity and set the CC1E Bit */
            tmpccer &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
            tmpccer |= (TIM_ICPolarity & (TIM_CCER_CC1P | TIM_CCER_CC1NP));

            /* Write to TIMx CCMR1 and CCER registers */
            TIMx->CCMR1 = tmpccmr1;
            TIMx->CCER = tmpccer;
        }
        break;

        case TIM_CHANNEL_2:
        {
            /* Disable the Channel 2: Reset the CC2E Bit */
            LL_TIM_CC2E_DISABLE(TIMx);
            tmpccmr1 = TIMx->CCMR1;
            tmpccer = TIMx->CCER;

            /* Select the Input */
            tmpccmr1 &= ~TIM_CCMR1_CC2S;
            tmpccmr1 |= (TIM_ICSelection << 8U);

            /* Set the filter */
            tmpccmr1 &= ~TIM_CCMR1_IC2F;
            tmpccmr1 |= ((TIM_ICFilter << 12U) & TIM_CCMR1_IC2F);

            /* Select the Polarity and set the CC2E Bit */
            tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
            tmpccer |= ((TIM_ICPolarity << 4U) & (TIM_CCER_CC2P | TIM_CCER_CC2NP));

            /* Write to TIMx CCMR1 and CCER registers */
            TIMx->CCMR1 = tmpccmr1 ;
            TIMx->CCER = tmpccer;
        }
        break;

        case TIM_CHANNEL_3:
        {
            /* Disable the Channel 3: Reset the CC3E Bit */
            LL_TIM_CC3E_DISABLE(TIMx);
            tmpccmr2 = TIMx->CCMR2;
            tmpccer = TIMx->CCER;

            /* Select the Input */
            tmpccmr2 &= ~TIM_CCMR2_CC3S;
            tmpccmr2 |= TIM_ICSelection;

            /* Set the filter */
            tmpccmr2 &= ~TIM_CCMR2_IC3F;
            tmpccmr2 |= ((TIM_ICFilter << 4U) & TIM_CCMR2_IC3F);

            /* Select the Polarity and set the CC3E Bit */
            tmpccer &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
            tmpccer |= ((TIM_ICPolarity << 8U) & (TIM_CCER_CC3P | TIM_CCER_CC3NP));

            /* Write to TIMx CCMR2 and CCER registers */
            TIMx->CCMR2 = tmpccmr2;
            TIMx->CCER = tmpccer;
        }
        break;

        case TIM_CHANNEL_4:
        {
            /* Disable the Channel 4: Reset the CC4E Bit */
            LL_TIM_CC4E_DISABLE(TIMx);
            tmpccmr2 = TIMx->CCMR2;
            tmpccer = TIMx->CCER;

            /* Select the Input */
            tmpccmr2 &= ~TIM_CCMR2_CC4S;
            tmpccmr2 |= (TIM_ICSelection << 8U);

            /* Set the filter */
            tmpccmr2 &= ~TIM_CCMR2_IC4F;
            tmpccmr2 |= ((TIM_ICFilter << 12U) & TIM_CCMR2_IC4F);

            /* Select the Polarity and set the CC4E Bit */
            tmpccer &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
            tmpccer |= ((TIM_ICPolarity << 12U) & (TIM_CCER_CC4P | TIM_CCER_CC4NP));

            /* Write to TIMx CCMR2 and CCER registers */
            TIMx->CCMR2 = tmpccmr2;
            TIMx->CCER = tmpccer ;
        }
        break;

        default:
            break;
    }
}

void HAL_TIM_TIInputStageConfig(TIM_Device_T *TIMx, uint32_t TIM_ICPolarity,
                                uint32_t TIM_ICFilter, uint32_t channel)
{
    uint32_t tmpccmr1 = 0U;
    uint32_t tmpccer = 0U;

    if (channel == TIM_CHANNEL_1)
    {
        /* Disable the Channel 1: Reset the CC1E Bit */
        tmpccer = TIMx->CCER;
        LL_TIM_CC1E_DISABLE(TIMx);
        tmpccmr1 = TIMx->CCMR1;

        /* Set the filter */
        tmpccmr1 &= ~TIM_CCMR1_IC1F;
        tmpccmr1 |= (TIM_ICFilter << 4U);

        /* Select the Polarity and set the CC1E Bit */
        tmpccer &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
        tmpccer |= TIM_ICPolarity;

        /* Write to TIMx CCMR1 and CCER registers */
        TIMx->CCMR1 = tmpccmr1;
        TIMx->CCER = tmpccer;
    }

    else
    {
        /* Disable the Channel 2: Reset the CC2E Bit */
        LL_TIM_CC2E_DISABLE(TIMx);
        tmpccmr1 = TIMx->CCMR1;
        tmpccer = TIMx->CCER;

        /* Set the filter */
        tmpccmr1 &= ~TIM_CCMR1_IC2F;
        tmpccmr1 |= (TIM_ICFilter << 12U);

        /* Select the Polarity and set the CC2E Bit */
        tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
        tmpccer |= (TIM_ICPolarity << 4U);

        /* Write to TIMx CCMR1 and CCER registers */
        TIMx->CCMR1 = tmpccmr1 ;
        TIMx->CCER = tmpccer;
    }

}

void HAL_TIM_ITRxConfig(TIM_Device_T *TIMx, uint16_t TIM_ITRx)
{
    uint32_t tmpsmcr = 0U;

    /* Get the TIMx SMCR register value */
    tmpsmcr = TIMx->SMCR;

    /* Reset the TS Bits */
    tmpsmcr &= ~TIM_SMCR_TS;

    /* Set the Input Trigger source and the slave mode*/
    tmpsmcr |= TIM_ITRx | TIM_SLAVEMODE_EXTERNAL1;

    /* Write to TIMx SMCR */
    TIMx->SMCR = tmpsmcr;
}

void HAL_TIM_ETRConfig(TIM_Device_T *TIMx, uint32_t TIM_ExtTRGPrescaler, uint32_t TIM_ExtTRGPolarity, uint32_t extTRGFilter)
{
    uint32_t tmpsmcr = 0U;
    tmpsmcr = TIMx->SMCR;

    /* Reset the ETR Bits */
    tmpsmcr &= ~(TIM_SMCR_ETF | TIM_SMCR_ETPS | TIM_SMCR_ECE | TIM_SMCR_ETP);

    /* Set the Prescaler, the Filter value and the Polarity */
    tmpsmcr |= (uint32_t)(TIM_ExtTRGPrescaler | (TIM_ExtTRGPolarity | (extTRGFilter << 8)));

    /* Write to TIMx SMCR */
    TIMx->SMCR = tmpsmcr;
}

RET_CODE HAL_TIM_ConfigCOMEventInterrupt(TIM_Handle_T *htim, TIM_Device_T *TIMx,
        uint32_t inputTrigger, uint32_t commutationSource)
{
    /* Check the parameters */
    assert_param(IS_TIM_ADVANCED_INSTANCE(htim->instance));
    assert_param(IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(inputTrigger));

    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    if ((inputTrigger == TIM_TS_ITR0) || (inputTrigger == TIM_TS_ITR1) ||
            (inputTrigger == TIM_TS_ITR2) || (inputTrigger == TIM_TS_ITR3) ||
            (inputTrigger == TIM_TS_TI1FP1) || (inputTrigger == TIM_TS_TI2FP2) ||
            (inputTrigger == TIM_TS_TI1F_ED) || (inputTrigger == TIM_TS_ETRF))
    {
        /* Select the Input trigger */
        TIMx->SMCR &= ~TIM_SMCR_TS;
        TIMx->SMCR |= inputTrigger;
    }

    /* Select the Capture Compare preload feature */
    LL_TIM_CCPC_ENABLE(TIMx);

    /* Select the Commutation event source */
    LL_TIM_CCUS_DISABLE(TIMx);

    TIMx->CR2 |= commutationSource;

    /* Enable the Commutation Interrupt Request */
    LL_TIM_COMIE_ENABLE(TIMx);

    /* Enable the Trigger Interrupt Request */
    //LL_TIM_TIE_ENABLE(TIMx);

    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;
}

RET_CODE HAL_TIM_ConfigBreakDeadTime(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_BreakDeadTimeConfig_T *sBreakDeadTimeConfig)
{
    uint32_t tmpbdtr = 0U;

    /* Check the parameters */
    assert_param(IS_TIM_BREAK_INSTANCE(htim->instance));
    assert_param(IS_TIM_LOCK_LEVEL(sBreakDeadTimeConfig->lockLevel));
    assert_param(IS_TIM_DEADTIME(sBreakDeadTimeConfig->deadTime));
    assert_param(IS_TIM_BREAK_STATE(sBreakDeadTimeConfig->breakState));
    assert_param(IS_TIM_BREAK_POLARITY(sBreakDeadTimeConfig->breakPolarity));
    assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(sBreakDeadTimeConfig->automaticOutput));

    /* Check input state */
    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    /* Set the Lock level, the Break enable Bit and the Polarity,
    the dead time value and the Automatic Output Enable Bit */
    MODIFY_REG(tmpbdtr, TIM_BDTR_DTG, sBreakDeadTimeConfig->deadTime);
    MODIFY_REG(tmpbdtr, TIM_BDTR_LOCK, sBreakDeadTimeConfig->lockLevel);
    MODIFY_REG(tmpbdtr, TIM_BDTR_BKE, sBreakDeadTimeConfig->breakState);
    MODIFY_REG(tmpbdtr, TIM_BDTR_BKP, sBreakDeadTimeConfig->breakPolarity);
    MODIFY_REG(tmpbdtr, TIM_BDTR_AOE, sBreakDeadTimeConfig->automaticOutput);

    /* Set TIMx_BDTR */
    TIMx->BDTR = tmpbdtr;
    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;

}

void TIMx_SetPWMDuty(TIM_Device_T *TIMx, uint8_t apre, uint16_t duty)
{
    if (apre == TRUE)
    {
        IO_BIT_SET(TIMx->CR1, TIM_CR1_ARPE);
    }
    else
    {
        IO_BIT_CLEAR(TIMx->CR1, TIM_CR1_ARPE);
    }

    TIMx->CCR1 = duty;

}


RET_CODE HAL_TIM_ConfigDmaReg(TIM_Handle_T *htim, TIM_Device_T *TIMx, TIM_DmaConfig_T *sDmaConfig)
{
    uint32_t tmpdcr = 0U;


    /* Check DMA transmit parameter in continuous mode */
    assert_param(IS_TIM_DMA_INSTANCE(htim->instance));
    assert_param(IS_TIM_DMA_BASE(sDmaConfig->dmaBaseAddress));
    assert_param(IS_TIM_DMA_LENGTH(sDmaConfig->dmaBurstLength));

    /* Check input state */
    if (htim->lock == HAL_TIM_LOCKED)
    {
        return HAL_BUSY;
    }
    else
    {
        htim->lock = HAL_TIM_LOCKED;
    }

    /* Set the DMA base address and DMA burst length */
    MODIFY_REG(tmpdcr, TIM_DCR_DBA, sDmaConfig->dmaBaseAddress);
    MODIFY_REG(tmpdcr, TIM_DCR_DBL, sDmaConfig->dmaBurstLength);

    /* Set TIMx_DCR */
    TIMx->DCR = tmpdcr;

    htim->lock = HAL_TIM_UNLOCKED;

    return RET_OK;

}

__weak void HAL_TIM_OCDelayElapsedCallback(TIM_Handle_T *htim)
{
    UNUSED(htim);
}

__weak void HAL_TIM_PWMPulseFinishedCallback(TIM_Handle_T *htim)
{
    UNUSED(htim);
}

__weak void HAL_TIM_TriggerCallback(TIM_Handle_T *htim)
{
    UNUSED(htim);
}

__weak void HAL_TIM_BreakCallback(TIM_Handle_T *htim)
{
    UNUSED(htim);
}

__weak void HAL_TIM_ErrorCallback(TIM_Handle_T *htim)
{
    UNUSED(htim);
}


__weak void HAL_TIM_ICCaptureCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    //    UNUSED(htim);

    if (TIMx != NULL && htim != NULL)
    {
        pTimCallback =  s_TIMHandle[instance]->init.callback;
        pTimCallback(TIMx, 0, 0);
    }

}

__weak void HAL_TIM_PeriodElapsedCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    UNUSED(htim);
}

__weak void HAL_TIM_CommutationCallback(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    UNUSED(htim);
}

void HAL_TIM_IRQ_handler(TIM_Handle_T *htim, TIM_Device_T *TIMx)
{
    uint32_t interruptMask = 0;
    uint32_t interruptFlags = 0;
    uint32_t event = 0;

    interruptMask = TIMx->DIER;
    interruptFlags = TIMx->SR;

    /* Capture compare 1 event */
    if ((interruptMask & TIM_IT_CC1) && (interruptFlags & TIM_FLAG_CC1))
    {
        event |= TIM_FLAG_CC1;
        htim->channel = HAL_TIM_ACTIVE_CHANNEL_1;

        /* Input capture event */
        if ((TIMx->CCMR1 & TIM_CCMR1_CC1S) != 0x00U)
        {
            HAL_TIM_ICCaptureCallback(htim, TIMx);
        }

        /* Output compare event */
        else
        {
            HAL_TIM_OCDelayElapsedCallback(htim);
            HAL_TIM_PWMPulseFinishedCallback(htim);
        }

        htim->channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }

    /* Capture compare 2 event */
    if ((interruptMask & TIM_IT_CC2) && (interruptFlags & TIM_FLAG_CC2))
    {
        event |= TIM_FLAG_CC2;
        htim->channel = HAL_TIM_ACTIVE_CHANNEL_2;

        /* Input capture event */
        if (((TIMx->CCMR1) & TIM_CCMR1_CC2S) != 0x00U)
        {
            HAL_TIM_ICCaptureCallback(htim, TIMx);
        }

        /* Output compare event */
        else
        {
            HAL_TIM_OCDelayElapsedCallback(htim);
            HAL_TIM_PWMPulseFinishedCallback(htim);
        }

        htim->channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }

    /* Capture compare 3 event */
    if ((interruptMask & TIM_IT_CC3) && (interruptFlags & TIM_FLAG_CC3))
    {
        event |= TIM_FLAG_CC3;
        htim->channel = HAL_TIM_ACTIVE_CHANNEL_3;

        /* Input capture event */
        if (((TIMx->CCMR2) & TIM_CCMR2_CC3S) != 0x00U)
        {
            HAL_TIM_ICCaptureCallback(htim, TIMx);
        }

        /* Output compare event */
        else
        {
            HAL_TIM_OCDelayElapsedCallback(htim);
            HAL_TIM_PWMPulseFinishedCallback(htim);
        }

        htim->channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }

    /* Capture compare 4 event */
    if ((interruptMask & TIM_IT_CC4) && (interruptFlags & TIM_FLAG_CC4))
    {
        event |= TIM_FLAG_CC4;
        htim->channel = HAL_TIM_ACTIVE_CHANNEL_4;

        /* Input capture event */
        if ((TIMx->CCMR2 & TIM_CCMR2_CC4S) != 0x00U)
        {
            HAL_TIM_ICCaptureCallback(htim, TIMx);
        }

        /* Output compare event */
        else
        {
            HAL_TIM_OCDelayElapsedCallback(htim);
            HAL_TIM_PWMPulseFinishedCallback(htim);
        }

        htim->channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }

    /* TIM Update event */
    if ((interruptMask & TIM_IT_UPDATE) && (interruptFlags & TIM_FLAG_UPDATE))
    {
        event |= TIM_FLAG_UPDATE;
        HAL_TIM_PeriodElapsedCallback(htim, TIMx);
    }

    /* TIM Break input event */
    if ((interruptMask & TIM_IT_BREAK) && (interruptFlags & TIM_FLAG_BREAK))
    {
        event |= TIM_FLAG_BREAK;
        HAL_TIM_BreakCallback(htim);
    }

    /* TIM Trigger detection event */
    if ((interruptMask & TIM_IT_TRIGGER) && (interruptFlags & TIM_FLAG_TRIGGER))
    {
        event |= TIM_FLAG_TRIGGER;
        HAL_TIM_TriggerCallback(htim);
    }

    /* TIM commutation event */
    if ((interruptMask & TIM_IT_COM) && (interruptFlags & TIM_FLAG_COM))
    {
        event |= TIM_FLAG_COM;
        HAL_TIM_CommutationCallback(htim, TIMx);
    }

    TIMx->SR = event;
}

void HAL_TIM1_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[0], TIM1);
}

void HAL_TIM2_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[1], TIM2);
}

void HAL_TIM3_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[2], TIM3);
}

void HAL_TIM4_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[3], TIM4);
}

void HAL_TIM5_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[4], TIM5);
}

void HAL_TIM8_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[8], TIM8);
}

#if defined(CHIP_GM6721)
void HAL_TIM9_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[9], TIM9);
}

void HAL_TIM10_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[10], TIM10);
}

void HAL_TIM11_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[11], TIM11);
}

void HAL_TIM12_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[12], TIM12);
}

void HAL_TIM13_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[13], TIM13);
}

void HAL_TIM14_InterruptHandler(void)
{
    HAL_TIM_IRQ_handler(s_TIMHandle[14], TIM14);
}
#endif

void HAL_TIM_RequestIrq(int tim_sel)
{
    if (tim_sel == 1)
    {
        HAL_RequestIrq(TIM1_IRQn, TIM1_IRQ_PRIORITY, HAL_TIM1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM1_IRQn);
    }
    else if (tim_sel == 2)
    {
        HAL_RequestIrq(TIM2_IRQn, TIM2_IRQ_PRIORITY, HAL_TIM2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM2_IRQn);
    }
    else if (tim_sel == 3)
    {
        HAL_RequestIrq(TIM3_IRQn, TIM3_IRQ_PRIORITY, HAL_TIM3_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM3_IRQn);
    }
    else if (tim_sel == 4)
    {
        HAL_RequestIrq(TIM4_IRQn, TIM4_IRQ_PRIORITY, HAL_TIM4_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM4_IRQn);
    }
    else if (tim_sel == 5)
    {
        HAL_RequestIrq(TIM5_IRQn, TIM5_IRQ_PRIORITY, HAL_TIM5_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM5_IRQn);
    }
    else if (tim_sel == 8)
    {
        HAL_RequestIrq(TIM8_IRQn, TIM8_IRQ_PRIORITY, HAL_TIM8_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM8_IRQn);
    }

#if defined(CHIP_GM6721)
    else if (tim_sel == 9)
    {
        HAL_RequestIrq(TIM9_IRQn, TIM9_IRQ_PRIORITY, HAL_TIM9_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM9_IRQn);
    }
    else if (tim_sel == 10)
    {
        HAL_RequestIrq(TIM10_IRQn, TIM10_IRQ_PRIORITY, HAL_TIM10_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM10_IRQn);
    }
    else if (tim_sel == 11)
    {
        HAL_RequestIrq(TIM11_IRQn, TIM11_IRQ_PRIORITY, HAL_TIM11_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM11_IRQn);
    }
    else if (tim_sel == 12)
    {
        HAL_RequestIrq(TIM12_IRQn, TIM12_IRQ_PRIORITY, HAL_TIM12_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM12_IRQn);
    }
    else if (tim_sel == 13)
    {
        HAL_RequestIrq(TIM13_IRQn, TIM13_IRQ_PRIORITY, HAL_TIM13_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM13_IRQn);
    }
    else if (tim_sel == 14)
    {
        HAL_RequestIrq(TIM14_IRQn, TIM14_IRQ_PRIORITY, HAL_TIM14_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM14_IRQn);
    }
#endif

    else
    {
        HAL_RequestIrq(TIM1_IRQn, TIM1_IRQ_PRIORITY, HAL_TIM1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM1_IRQn);

        HAL_RequestIrq(TIM2_IRQn, TIM2_IRQ_PRIORITY, HAL_TIM2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM2_IRQn);

        HAL_RequestIrq(TIM3_IRQn, TIM3_IRQ_PRIORITY, HAL_TIM3_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM3_IRQn);

        HAL_RequestIrq(TIM4_IRQn, TIM4_IRQ_PRIORITY, HAL_TIM4_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM4_IRQn);

        HAL_RequestIrq(TIM5_IRQn, TIM5_IRQ_PRIORITY, HAL_TIM5_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM5_IRQn);

        HAL_RequestIrq(TIM8_IRQn, TIM8_IRQ_PRIORITY, HAL_TIM8_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM8_IRQn);

#if defined(CHIP_GM6721)
        HAL_RequestIrq(TIM9_IRQn, TIM9_IRQ_PRIORITY, HAL_TIM9_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM9_IRQn);

        HAL_RequestIrq(TIM10_IRQn, TIM10_IRQ_PRIORITY, HAL_TIM10_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM10_IRQn);

        HAL_RequestIrq(TIM11_IRQn, TIM11_IRQ_PRIORITY, HAL_TIM11_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM11_IRQn);

        HAL_RequestIrq(TIM12_IRQn, TIM12_IRQ_PRIORITY, HAL_TIM12_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM4_IRQn);

        HAL_RequestIrq(TIM13_IRQn, TIM13_IRQ_PRIORITY, HAL_TIM13_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM13_IRQn);

        HAL_RequestIrq(TIM14_IRQn, TIM14_IRQ_PRIORITY, HAL_TIM14_InterruptHandler, 0);
        HAL_IRQ_ENABLE(TIM14_IRQn);
#endif

    }
}

void HAL_TIM_CLK_ENABLE(int tim_sel)
{
    if (tim_sel == 1)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM1);
    }
    else if (tim_sel == 2)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM2);
    }
    else if (tim_sel == 3)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM3);
    }
    else if (tim_sel == 4)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM4);
    }
    else if (tim_sel == 5)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM5);
    }
    else if (tim_sel == 8)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM8);
    }

#if defined(CHIP_GM6721)
    else if (tim_sel == 9)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM9);
    }
    else if (tim_sel == 10)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM10);
    }
    else if (tim_sel == 11)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM11);
    }
    else if (tim_sel == 12)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM12);
    }
    else if (tim_sel == 13)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM13);
    }
    else if (tim_sel == 14)
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM14);
    }
#endif

    else
    {
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM1);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM2);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM3);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM4);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM5);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM8);

#if defined(CHIP_GM6721)
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM9);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM10);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM11);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM12);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM13);
        HAL_CLOCK_PeripheralClockEnable2(SYS_ENCLK2_TIM14);
#endif
    }
}

void HAL_TIM_SW_Reset(void)
{
    HAL_Reset_Module(RESET_TIM1_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM2_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM3_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM4_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM5_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM8_SW_RSTJ);

#if defined(CHIP_GM6721)
    HAL_Reset_Module(RESET_TIM9_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM10_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM11_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM12_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM13_SW_RSTJ);
    HAL_Reset_Module(RESET_TIM14_SW_RSTJ);
#endif
}

void TIM_DMACallback(DMA_Handle_T *pDMAHandle, uint8_t event, unsigned long param)
{
    ARG_UNUSED(pDMAHandle);
}

