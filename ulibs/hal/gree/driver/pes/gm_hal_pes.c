/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          gm_hal_pes.c
*
* @author        Christal.Xu
*
* @version       1.1.0
*
* @date          2019/05/05
*
* @brief         Header file of pes driver.
*
* @note
*     2017/05/19, Mike.Zheng, V1.0.0
*     2019/05/05, Christal.Xu, V1.1.0
*        Initial version.
*/
#include<gm_hal_pes.h>
#include<gm_irq.h>
#include"gm_ll_pes.h"


#define PES_IO_MAX_NUM                  16  /* PES io max number */
#define PES_CHANNLE_NUM                 12  /* PES channel number */
#define PES_CAHNNEL_FREE                0   /* current PES channel is not using*/
#define PES_CAHNNEL_USING               1   /* current PES channel is using*/

typedef struct
{
    uint32_t pin;                           /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;                  /**< GPIO device */
    uint32_t mode;                          /**< Specifies the operating mode for the selected pins */
    uint32_t pull;                          /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins */
    uint32_t alternate;                     /**< Peripheral to be connected to the selected pins
                                                 This parameter can be a value of @ref GPIOEx_Alternate_function_selection */
} PES_IOPinmux_T;

static RET_CODE     HAL_PES_TryLockPesChIndex(void);

static void         HAL_PES_UnlockPesChIndex(void);

static void         HAL_PES_ConfigCascadeEventChannel(PES_ChannelCascadeEnConfig_T *pcfg,
        PES_ChannelIndex_T chIndex);

static RET_CODE     HAL_PES_SyncModeConfig(PES_TrigerSlaveSyncMode_T syncMode, PES_ChannelIndex_T chIndex);

static void     HAL_PES_IOAsMasterSendEventEn(PES_ChannelIndex_T chIndex, uint8_t en);


static uint8_t      pesChIndex[PES_CHANNLE_NUM] = { PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                    PES_CAHNNEL_FREE,
                                                  };

/* pes gloab var lock */
static uint8_t spesChIndexLock = PES_UNLOCKED;

static RET_CODE HAL_PES_TryLockPesChIndex(void)
{
    RET_CODE ret = RET_OK;

    HAL_DisableIrq();

    if (spesChIndexLock == PES_LOCKED)
    {
        ret =  RET_BUSY;
        goto out;
    }
    else
    {
        spesChIndexLock = PES_LOCKED;
    }

out:
    HAL_EnableIrq();

    return ret;
}

static void HAL_PES_UnlockPesChIndex(void)
{
    HAL_DisableIrq();

    spesChIndexLock = PES_UNLOCKED;

    HAL_EnableIrq();
}

RET_CODE HAL_PES_IOPinmuxConfig(GPIO_Device_T *bank, uint32_t pinnum,  PES_Io_T mode)
{
    RET_CODE            ret = RET_OK;
    GPIO_PinConfig_T    GPIO_InitStruct;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    switch (mode)
    {
        case IO_MASTER:
            GPIO_InitStruct.pin         = pinnum;
            GPIO_InitStruct.mode        = GPIO_MODE_INPUT;
            GPIO_InitStruct.pull        = GPIO_NOPULL;
            GPIO_InitStruct.alternate   = 0;

            ret = HAL_GPIO_Init(bank, &GPIO_InitStruct);
            break;

        case IO_SLAVE:
            GPIO_InitStruct.pin         = pinnum;
            GPIO_InitStruct.mode        = GPIO_MODE_AF;
            GPIO_InitStruct.pull        = GPIO_NOPULL;
            GPIO_InitStruct.alternate   = 9;

            ret = HAL_GPIO_Init(bank, &GPIO_InitStruct);
            break;

        default:
            return RET_INVPARAM ;
    }
    return ret;
}

PES_ChannelIndex_T HAL_PES_RequestEventChannel()
{
    PES_ChannelIndex_T chIndex = PES_INVAILD_CHANNEL;
    uint8_t i = 0;

    /* protect write gloab pesChIndex: need lock */
    if (HAL_PES_TryLockPesChIndex() != RET_OK)
    {
        chIndex = PES_INVAILD_CHANNEL;
        goto out;
    }

    for (i = 0; i < PES_CHANNLE_NUM; i++)
    {
        if (pesChIndex[i] == PES_CAHNNEL_FREE)
        {
            chIndex = (PES_ChannelIndex_T)i;
            /* get a vaild channel and set flag */
            pesChIndex[i] = PES_CAHNNEL_USING;
            break;
        }
    }

    HAL_PES_UnlockPesChIndex();
out:
    return chIndex;
}

RET_CODE HAL_PES_ReleaseEventChannel(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    if (chIndex == PES_INVAILD_CHANNEL)
    {
        PES_PR_WARNING("Waring: Not Regist Callback Before\n");
    }

    /*protect write gloab pesChIndex: need lock*/
    if (HAL_PES_TryLockPesChIndex() != RET_OK)
    {
        ret =  RET_BUSY;
        goto out;
    }

    pesChIndex[chIndex] = PES_CAHNNEL_FREE;

    HAL_PES_UnlockPesChIndex();

out:
    return ret;

}

RET_CODE HAL_PES_MasterConfig(PES_MasterSourceConfig_T masterSource,
                              PES_MasterEventSourceConfig_T *pmasterTrigerOutput,
                              PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    /* selet master source*/
    LL_PES_MASTER_SOURCE_SELECT(PES_DEV, chIndex, masterSource);

    switch (masterSource)
    {
        case PES_MASTER_CPU:
        case PES_MASTER_DMA:
        case PES_MASTER_ACMP:
        case PES_MASTER_RTCC:
        case PES_MASTER_BTM2:
        case PES_MASTER_USB:
            break;
        case PES_MASTER_PESOUT:/*
             if (pmasterTrigerOutput->pesChannelOutput == chIndex)
             {
                PES_PR_ERR("Err:PES_Event_channel is same to channel!\n");
                break;
             }*/
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->pesChannelOutput);
            break;

        case PES_MASTER_IO:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->ioPinOutput);
            HAL_PES_IOAsMasterSendEventEn(chIndex, TRUE);
            break;

        case PES_MASTER_ADC:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->adcOutput);
            break;

        case PES_MASTER_TIM1:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->timOutput);
            break;

        case PES_MASTER_TIM2:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->timOutput);
            break;

        case PES_MASTER_TIM3:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->timOutput);
            break;

        case PES_MASTER_TIM4:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->timOutput);
            break;

        case PES_MASTER_TIM5:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->timOutput);
            break;

        case PES_MASTER_TIM8:
            LL_PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->timOutput);
            break;

        default:
            return RET_INVPARAM ;
    }

    return ret;
}

static void HAL_PES_ConfigDetectEdgeChannel(PES_DetectEdge_T detectMode,
        PES_ChannelIndex_T chIndex)
{
    if (detectMode == PES_DET_DISABLE)
    {
        LL_PES_OUT_DETECT_DISABLE(PES_DEV, chIndex);
    }
    else
    {
        LL_PES_OUT_DETECT_EDGE_SET(PES_DEV, chIndex, detectMode);
        LL_PES_OUT_DETECT_ENABLE(PES_DEV, chIndex);
    }
}

static void HAL_PES_ConfigCascadeEventChannel(PES_ChannelCascadeEnConfig_T *pcfg,
        PES_ChannelIndex_T chIndex)
{
    if (pcfg->andNextChannelEnable == TRUE)
    {
        LL_PES_AND_NEXT_CHANNEL_ENABLE(PES_DEV, chIndex);
    }
    else
    {
        LL_PES_AND_NEXT_CHANNEL_DISABLE(PES_DEV, chIndex);
    }
    if (pcfg->orpreChannelEnable == TRUE)
    {
        LL_PES_OR_PRE_CHANNEL_ENABLE(PES_DEV, chIndex);
    }
    else
    {
        LL_PES_OR_PRE_CHANNEL_DISABLE(PES_DEV, chIndex);
    }
}

static void HAL_PES_ConfigEventChannelOutputInvertedEn(uint8_t invertedEn,
        PES_ChannelIndex_T chIndex)
{
    if (invertedEn == TRUE)
    {
        LL_PES_OUT_INVERT_ENABLE(PES_DEV, chIndex);
    }
    else
    {
        LL_PES_OUT_INVERT_DISABLE(PES_DEV, chIndex);
    }
}

static void HAL_PES_IOAsMasterSendEventEn(PES_ChannelIndex_T chIndex, uint8_t en)
{
    if (en)
    {
        LL_PES_CONFIG_IO_DIR(PES_DEV, chIndex, IO_SEND_EVENT);
    }
    else
    {
        LL_PES_CONFIG_IO_DIR(PES_DEV, chIndex, IO_RECEIVE_EVENT);
    }
}

static RET_CODE HAL_PES_SyncModeConfig(PES_TrigerSlaveSyncMode_T syncMode,
                                       PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    PES_Device_T *pDev = PES_DEV;

    switch (syncMode)
    {
        case PES_SYNC_BYPASS:
        case PES_SYNC_RISING_EDGE:
        case PES_SYNC_FALLING_EDGE:
        case PES_SYNC_RISINGF_FALLING_EDGE:
            LL_PES_RESYNC_ENABLE(pDev, chIndex);
            LL_PES_CONFIG_EVENT_OUTPUT_MODE(pDev, chIndex, syncMode);
            break;

        case PES_ASYNC:
            LL_PES_ASYNC_ENABLE(pDev, chIndex);
            break;

        default:
            return RET_INVPARAM ;
    }

    return ret;
}

RET_CODE HAL_PES_ConfigInterruptEN(uint8_t interruptEn)
{
    RET_CODE ret = RET_OK;
    if (interruptEn == TRUE)
    {
        LL_PES_INTERRUPT_ENABLE(PES_DEV);
    }
    else
    {
        LL_PES_INTERRUPT_DISABLE(PES_DEV);
    }
    return ret;
}

/* config the interrupt enable and status register*/
RET_CODE HAL_PES_ConfigIEandStatusReg(uint8_t interruptEn, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    if (interruptEn == TRUE)
    {
        HAL_PES_ConfigInterruptEN(interruptEn);
        LL_PES_DET_STATUS_CLEAR(PES_DEV, chIndex);
        HAL_PES_ConfigInterruptEN(interruptEn);
    }
    return ret;
}

/* config the channel configuration register*/
RET_CODE HAL_PES_ConfigEventChannel(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    /* config Master */
    ret =  HAL_PES_MasterConfig(pchCfg->masterSrcConfig,
                                &(pchCfg->masterEvtSrcConfig),
                                chIndex);
    if (ret != RET_OK)
    {
        PES_PR_ERR("%s:Err:PES_MasterConfig\n", __FUNCTION__);
        goto out;
    }

    /* config cascade event channel */
    HAL_PES_ConfigCascadeEventChannel(&(pchCfg->cascadeConfig), chIndex);

    /* config envetoutput invert or not */
    HAL_PES_ConfigEventChannelOutputInvertedEn(pchCfg->channelOutputInverted, chIndex);

    /* config sync mode */
    ret = HAL_PES_SyncModeConfig(pchCfg->syncMode, chIndex);

    /*config detect enable and detect edge*/
    HAL_PES_ConfigDetectEdgeChannel(pchCfg->detectMode, chIndex);

    if (ret != RET_OK)
    {
        goto out;
    }

out:
    return ret;
}

/*PES event to CPU channel select*/
static void HAL_PES_EventToCpuChannelSelect(PES_ChannelIndex_T chIndex)
{
    LL_PES_TO_CPU_EVENT_CHANNEL(PES_DEV, chIndex);
}

/*PES event to CPU enable*/
static void HAL_PES_EventToCpuEn(uint8_t en)
{
    if (en)
    {
        LL_PES_TO_CPU_EVENT_ENABLE(PES_DEV);
    }
    else
    {
        LL_PES_TO_CPU_EVENT_DISABLE(PES_DEV);
    }
}

RET_CODE  HAL_PES_EventToCpuRegConfig(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    /* config master */
    ret = HAL_PES_ConfigEventChannel(pchCfg, chIndex);
    if (ret != RET_OK)
    {
        goto out;
    }

    /*as slave select pes event channel */
    HAL_PES_EventToCpuChannelSelect(chIndex);

    /* as slave enable receive master pes event */
    HAL_PES_EventToCpuEn(TRUE);

out:
    return ret;
}


RET_CODE HAL_PES_GenerateSWTrigerEvent(PES_MasterCpuTriger_T trigerMode, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    if (trigerMode == PES_PULSE)
    {
        LL_PES_SW_GENERATE_PLUSE(PES_DEV, chIndex);
    }
    else
    {
        LL_PES_CONFIG_SW_LEVEL(PES_DEV, chIndex, trigerMode);
    }
    return ret;
}

RET_CODE HAL_PES_GetIrqStatus(PES_ChannelIndex_T chIndex)
{

    RET_CODE ret = RET_OK;
    ret = LL_PES_GET_INTERRUPT_STATUS(PES_DEV, chIndex);
    return ret;
}


