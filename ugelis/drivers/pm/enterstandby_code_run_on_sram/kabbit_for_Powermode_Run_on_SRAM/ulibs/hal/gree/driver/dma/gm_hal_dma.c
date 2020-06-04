/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          dma.c
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/06/27
 *
 * @brief         Implementation of DMA Driver.
 *
 * @note
 *    2016/06/27, wizephen.wang, V1.0.0
 *        Initial version.
 *    2017/06/12, evan.wu, V1.0.1
 *        Support 6621.
 *    2019/04/18, chris.xie, V1.0.2
 *        code update.
 */
#include <gm_soc.h>

#ifdef CONFIG_GM_HAL_CACHE
    #include <gm_hal_cache.h>
#endif
#include <gm_hal_dma.h>
#include "gm_ll_dma.h"
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

#define ASSERT(expr)          do{}while(0)

/* Pointers to DMA bases for each instance */
static DMA_Device_T *const s_dmaBases = DMA_DEV;

/* Pointers to DMA handles for each instance */
static DMA_Handle_T *s_dmaHandle[DMA_MAX_CHANNEL] = {NULL};

/* Pointers to DMA reserved ram area */
char dma_ram_reserve[1024] __attribute__((aligned(512))) = {0};

//DMA_Control_T *p_dmaCtrl = (DMA_Control_T*)__dma_ram_start;
DMA_Control_T *p_dmaCtrl = (DMA_Control_T *)dma_ram_reserve;

extern RET_CODE HAL_DCACHE_InvalidAll(void);


static DMA_Channel_Info_T s_dmaChannelInfoMap[] =
{
    /* channel 0 */
    {DMA_CHANNEL_0, DMA_DEV_0, DMA_REQUEST_SPI3_RX}, //0
    {DMA_CHANNEL_0, DMA_DEV_1, DMA_REQUEST_I2C1_RX},
    {DMA_CHANNEL_0, DMA_DEV_2, DMA_REQUEST_TIM4_CC1D},
    {DMA_CHANNEL_0, DMA_DEV_4, DMA_REQUEST_UART5_RX},
    {DMA_CHANNEL_0, DMA_DEV_5, DMA_REQUEST_UART8_TX},
    {DMA_CHANNEL_0, DMA_DEV_6, DMA_REQUEST_TIM5_CC3D},
    {DMA_CHANNEL_0, DMA_DEV_6, DMA_REQUEST_TIM5_UD},
    {DMA_CHANNEL_0, DMA_DEV_7, DMA_REQUEST_SDIO_RX},
    /* channel 1 */
    {DMA_CHANNEL_1, DMA_DEV_0, DMA_REQUEST_USB},//8
    {DMA_CHANNEL_1, DMA_DEV_2, DMA_REQUEST_NAND},
    {DMA_CHANNEL_1, DMA_DEV_3, DMA_REQUEST_TIM2_UD},
    {DMA_CHANNEL_1, DMA_DEV_3, DMA_REQUEST_TIM2_CC3D},//10
    {DMA_CHANNEL_1, DMA_DEV_4, DMA_REQUEST_UART3_RX},
    {DMA_CHANNEL_1, DMA_DEV_5, DMA_REQUEST_UART7_TX},
    {DMA_CHANNEL_1, DMA_DEV_6, DMA_REQUEST_TIM5_CC4D},
    {DMA_CHANNEL_1, DMA_DEV_6, DMA_REQUEST_TIM5_TD},
    /* channel 2 */
    {DMA_CHANNEL_2, DMA_DEV_0, DMA_REQUEST_SPI3_RX},//16
    {DMA_CHANNEL_2, DMA_DEV_3, DMA_REQUEST_I2C3_RX},
    {DMA_CHANNEL_2, DMA_DEV_4, DMA_REQUEST_UART4_RX},
    {DMA_CHANNEL_2, DMA_DEV_5, DMA_REQUEST_TIM3_CC4D},
    {DMA_CHANNEL_2, DMA_DEV_5, DMA_REQUEST_TIM3_UD},
    {DMA_CHANNEL_2, DMA_DEV_6, DMA_REQUEST_TIM5_CC1D},//20
    {DMA_CHANNEL_2, DMA_DEV_7, DMA_REQUEST_I2C2_RX},
    /* channel 3 */
    {DMA_CHANNEL_3, DMA_DEV_0, DMA_REQUEST_SPI2_RX},//23
    {DMA_CHANNEL_3, DMA_DEV_1, DMA_REQUEST_DAC1},
    {DMA_CHANNEL_3, DMA_DEV_2, DMA_REQUEST_TIM4_CC2D},
    {DMA_CHANNEL_3, DMA_DEV_4, DMA_REQUEST_UART3_TX},
    {DMA_CHANNEL_3, DMA_DEV_5, DMA_REQUEST_UART7_RX},
    {DMA_CHANNEL_3, DMA_DEV_6, DMA_REQUEST_TIM5_CC4D},
    {DMA_CHANNEL_3, DMA_DEV_6, DMA_REQUEST_TIM5_TD},
    {DMA_CHANNEL_3, DMA_DEV_7, DMA_REQUEST_I2C2_RX},
    /* channel 4 */
    {DMA_CHANNEL_4, DMA_DEV_0, DMA_REQUEST_SPI2_TX},//31
    {DMA_CHANNEL_4, DMA_DEV_2, DMA_REQUEST_TIM2_TD},
    {DMA_CHANNEL_4, DMA_DEV_3, DMA_REQUEST_I2C3_TX},
    {DMA_CHANNEL_4, DMA_DEV_4, DMA_REQUEST_UART4_TX},
    {DMA_CHANNEL_4, DMA_DEV_5, DMA_REQUEST_TIM3_CC1D},
    {DMA_CHANNEL_4, DMA_DEV_5, DMA_REQUEST_TIM3_TD},
    {DMA_CHANNEL_4, DMA_DEV_6, DMA_REQUEST_TIM5_CC2D},
    {DMA_CHANNEL_4, DMA_DEV_7, DMA_REQUEST_UART3_TX},
    /* channel 5 */
    {DMA_CHANNEL_5, DMA_DEV_0, DMA_REQUEST_SPI3_TX},
    {DMA_CHANNEL_5, DMA_DEV_1, DMA_REQUEST_I2C1_RX},
    {DMA_CHANNEL_5, DMA_DEV_2, DMA_REQUEST_TIM4_CC4D},
    {DMA_CHANNEL_5, DMA_DEV_3, DMA_REQUEST_TIM2_CC1D},
    {DMA_CHANNEL_5, DMA_DEV_4, DMA_REQUEST_UART2_RX},//40
    {DMA_CHANNEL_5, DMA_DEV_5, DMA_REQUEST_TIM3_CC2D},
    {DMA_CHANNEL_5, DMA_DEV_6, DMA_REQUEST_SDIO_TX},
    {DMA_CHANNEL_5, DMA_DEV_7, DMA_REQUEST_DAC1},
    /* channel 6 */
    {DMA_CHANNEL_6, DMA_DEV_0, DMA_REQUEST_USB},//47
    {DMA_CHANNEL_6, DMA_DEV_1, DMA_REQUEST_I2C1_TX},
    {DMA_CHANNEL_6, DMA_DEV_2, DMA_REQUEST_TIM4_UD},
    {DMA_CHANNEL_6, DMA_DEV_3, DMA_REQUEST_TIM2_CC2D},
    {DMA_CHANNEL_6, DMA_DEV_3, DMA_REQUEST_TIM2_CC4D},
    {DMA_CHANNEL_6, DMA_DEV_4, DMA_REQUEST_UART2_TX},
    {DMA_CHANNEL_6, DMA_DEV_5, DMA_REQUEST_UART8_RX},//50
    {DMA_CHANNEL_6, DMA_DEV_6, DMA_REQUEST_TIM5_UD},
    {DMA_CHANNEL_6, DMA_DEV_7, DMA_REQUEST_DAC2},
    /* channel 7 */
    {DMA_CHANNEL_7, DMA_DEV_0, DMA_REQUEST_SPI3_TX},//56
    {DMA_CHANNEL_7, DMA_DEV_1, DMA_REQUEST_I2C1_TX},
    {DMA_CHANNEL_7, DMA_DEV_2, DMA_REQUEST_TIM4_CC3D},
    {DMA_CHANNEL_7, DMA_DEV_3, DMA_REQUEST_TIM2_UD},
    {DMA_CHANNEL_7, DMA_DEV_3, DMA_REQUEST_TIM2_CC4D},
    {DMA_CHANNEL_7, DMA_DEV_4, DMA_REQUEST_UART5_TX},
    {DMA_CHANNEL_7, DMA_DEV_5, DMA_REQUEST_TIM3_CC3D},
    {DMA_CHANNEL_7, DMA_DEV_6, DMA_REQUEST_DAC2},//60
    {DMA_CHANNEL_7, DMA_DEV_7, DMA_REQUEST_I2C2_TX},
    /* channel 8 */
    {DMA_CHANNEL_8, DMA_DEV_0, DMA_REQUEST_ADC1},//65
    {DMA_CHANNEL_8, DMA_DEV_1, DMA_REQUEST_HASH_TX},
    {DMA_CHANNEL_8, DMA_DEV_2, DMA_REQUEST_ADC3},
    {DMA_CHANNEL_8, DMA_DEV_3, DMA_REQUEST_SPI1_RX},
    {DMA_CHANNEL_8, DMA_DEV_4, DMA_REQUEST_SPI4_RX},
    {DMA_CHANNEL_8, DMA_DEV_5, DMA_REQUEST_TIM4_TD},
    {DMA_CHANNEL_8, DMA_DEV_6, DMA_REQUEST_TIM1_TD},
    {DMA_CHANNEL_8, DMA_DEV_7, DMA_REQUEST_I2C2_TX},
    /* channel 9 */
    {DMA_CHANNEL_9, DMA_DEV_0, DMA_REQUEST_SAI1_A},
    {DMA_CHANNEL_9, DMA_DEV_1, DMA_REQUEST_DCMI_VIN},//70
    {DMA_CHANNEL_9, DMA_DEV_2, DMA_REQUEST_ADC3},
    {DMA_CHANNEL_9, DMA_DEV_3, DMA_REQUEST_I2C3_RX},
    {DMA_CHANNEL_9, DMA_DEV_4, DMA_REQUEST_SPI4_TX},
    {DMA_CHANNEL_9, DMA_DEV_5, DMA_REQUEST_UART6_RX},
    {DMA_CHANNEL_9, DMA_DEV_6, DMA_REQUEST_TIM1_CC1D},
    {DMA_CHANNEL_9, DMA_DEV_7, DMA_REQUEST_TIM8_UD},
    /* channel 10 */
    {DMA_CHANNEL_10, DMA_DEV_0, DMA_REQUEST_TIM8_CC1D},//81
    {DMA_CHANNEL_10, DMA_DEV_0, DMA_REQUEST_TIM8_CC2D},
    {DMA_CHANNEL_10, DMA_DEV_0, DMA_REQUEST_TIM8_CC3D},
    {DMA_CHANNEL_10, DMA_DEV_1, DMA_REQUEST_ADC2},//80
    {DMA_CHANNEL_10, DMA_DEV_2, DMA_REQUEST_HASH_TX},
    {DMA_CHANNEL_10, DMA_DEV_3, DMA_REQUEST_SPI1_RX},
    {DMA_CHANNEL_10, DMA_DEV_4, DMA_REQUEST_UART1_RX},
    {DMA_CHANNEL_10, DMA_DEV_5, DMA_REQUEST_UART6_RX},
    {DMA_CHANNEL_10, DMA_DEV_6, DMA_REQUEST_TIM1_CC2D},
    {DMA_CHANNEL_10, DMA_DEV_7, DMA_REQUEST_TIM8_CC1D},
    /* channel 11 */
    {DMA_CHANNEL_11, DMA_DEV_0, DMA_REQUEST_SAI1_A},//91
    {DMA_CHANNEL_11, DMA_DEV_1, DMA_REQUEST_ADC2},
    {DMA_CHANNEL_11, DMA_DEV_2, DMA_REQUEST_SPI5_RX},
    {DMA_CHANNEL_11, DMA_DEV_3, DMA_REQUEST_SPI1_TX},//90
    {DMA_CHANNEL_11, DMA_DEV_4, DMA_REQUEST_SDIO_RX},
    {DMA_CHANNEL_11, DMA_DEV_5, DMA_REQUEST_SPI4_RX},
    {DMA_CHANNEL_11, DMA_DEV_6, DMA_REQUEST_TIM1_CC1D},
    {DMA_CHANNEL_11, DMA_DEV_7, DMA_REQUEST_TIM8_CC2D},
    /* channel 12 */
    {DMA_CHANNEL_12, DMA_DEV_0, DMA_REQUEST_ADC1},//99
    {DMA_CHANNEL_12, DMA_DEV_1, DMA_REQUEST_SAI1_B},
    {DMA_CHANNEL_12, DMA_DEV_2, DMA_REQUEST_SPI5_TX},
    {DMA_CHANNEL_12, DMA_DEV_3, DMA_REQUEST_I2C3_TX},
    {DMA_CHANNEL_12, DMA_DEV_4, DMA_REQUEST_TIM4_CC4D},
    {DMA_CHANNEL_12, DMA_DEV_5, DMA_REQUEST_SPI4_TX},
    {DMA_CHANNEL_12, DMA_DEV_6, DMA_REQUEST_TIM1_CC4D},//100
    {DMA_CHANNEL_12, DMA_DEV_6, DMA_REQUEST_TIM1_TD},
    {DMA_CHANNEL_12, DMA_DEV_6, DMA_REQUEST_TIM1_COMD},
    {DMA_CHANNEL_12, DMA_DEV_7, DMA_REQUEST_TIM8_CC3D},
    /* channel 13 */
    {DMA_CHANNEL_13, DMA_DEV_0, DMA_REQUEST_SAI1_B},//109
    {DMA_CHANNEL_13, DMA_DEV_1, DMA_REQUEST_SPI6_TX},
    {DMA_CHANNEL_13, DMA_DEV_2, DMA_REQUEST_CRYP_TX},
    {DMA_CHANNEL_13, DMA_DEV_3, DMA_REQUEST_SPI1_TX},
    {DMA_CHANNEL_13, DMA_DEV_4, DMA_REQUEST_UART1_RX},
    {DMA_CHANNEL_13, DMA_DEV_5, DMA_REQUEST_TIM2_TD},
    {DMA_CHANNEL_13, DMA_DEV_6, DMA_REQUEST_TIM1_UD},
    {DMA_CHANNEL_13, DMA_DEV_7, DMA_REQUEST_SPI5_RX},
    /* channel 14 */
    {DMA_CHANNEL_14, DMA_DEV_0, DMA_REQUEST_TIM1_CC1D},//117
    {DMA_CHANNEL_14, DMA_DEV_0, DMA_REQUEST_TIM1_CC2D},
    {DMA_CHANNEL_14, DMA_DEV_0, DMA_REQUEST_TIM1_CC3D},
    {DMA_CHANNEL_14, DMA_DEV_1, DMA_REQUEST_SPI6_RX},
    {DMA_CHANNEL_14, DMA_DEV_2, DMA_REQUEST_CRYP_RX},
    {DMA_CHANNEL_14, DMA_DEV_3, DMA_REQUEST_NAND},
    {DMA_CHANNEL_14, DMA_DEV_4, DMA_REQUEST_SDIO_TX},
    {DMA_CHANNEL_14, DMA_DEV_5, DMA_REQUEST_UART6_TX},
    {DMA_CHANNEL_14, DMA_DEV_6, DMA_REQUEST_TIM1_CC3D},
    {DMA_CHANNEL_14, DMA_DEV_7, DMA_REQUEST_SPI5_TX},
    /* channel 15 */
    {DMA_CHANNEL_15, DMA_DEV_1, DMA_REQUEST_DCMI_VIN},//127
    {DMA_CHANNEL_15, DMA_DEV_3, DMA_REQUEST_TIM4_TD},
    {DMA_CHANNEL_15, DMA_DEV_4, DMA_REQUEST_UART1_TX},
    {DMA_CHANNEL_15, DMA_DEV_5, DMA_REQUEST_UART6_TX},
    {DMA_CHANNEL_15, DMA_DEV_7, DMA_REQUEST_TIM8_CC4D},
    {DMA_CHANNEL_15, DMA_DEV_7, DMA_REQUEST_TIM8_TD},
    {DMA_CHANNEL_15, DMA_DEV_7, DMA_REQUEST_TIM8_COMD},
};

/*Declarition of DMA interrupt service routine*/
void HAL_DMA_IRQHandler(void);

static void DMA_CommonIRQHandler(DMA_Handle_T **hDMA);

void HAL_DMA_IRQHandler(void)
{
    DMA_CommonIRQHandler(s_dmaHandle);
}

static int8_t irq_reg_done = 0;
static void HAL_DMA_HostIrqConfig(DMA_Handle_T *hDMA)
{
    if (!irq_reg_done)
    {
#ifndef RABBIT_NO_OS_SYSTEM
        HAL_RequestIrq(DMA1_IRQn, DMA1_IRQ_PRIORITY, HAL_DMA_IRQHandler, 0);
#endif
        HAL_IRQ_ENABLE(DMA1_IRQn);
        irq_reg_done = 1;
    }
}

RET_CODE HAL_DMA_Request_Channel(DMA_Device_T *pDMA, DMA_RequestType_T reqType, DMA_Request_Result_T *reqResult)
{
    uint32_t i;
    uint8_t freeHandleFound = FALSE;
    DMA_ChannelIdx_T   chIdx = DMA_CHANNEL_0;
    DMA_ChnDeviceIdx_T devIdx = DMA_DEV_0;

    ASSERT(pConfig != NULL && pDMA != NULL);

    if (pDMA != s_dmaBases)
    {
        return RET_INVPARAM;
    }

    /* find chennel&dev which can be used */
    if (reqType == DMA_REQUEST_M2M)
    {
        for (i = 0; i < DMA_MAX_CHANNEL; i++)
        {
            /* check whether handle is used */
            if (s_dmaHandle[i] == NULL)
            {
                freeHandleFound = TRUE;
                chIdx = (DMA_ChannelIdx_T)i;
                break;
            }
        }
    }
    else
    {
        for (i = 0; i < sizeof(s_dmaChannelInfoMap) / sizeof(s_dmaChannelInfoMap[0]); i++)
        {
            if (s_dmaChannelInfoMap[i].requestType == reqType)
            {
                /* check whether handle is used */
                if (s_dmaHandle[s_dmaChannelInfoMap[i].chIdx] == NULL)
                {
                    freeHandleFound = TRUE;
                    chIdx = s_dmaChannelInfoMap[i].chIdx;
                    devIdx = s_dmaChannelInfoMap[i].devIdx;
                    break;
                }

            }
        }
    }

    if (freeHandleFound == FALSE)
    {
        /* no usable channel */
        return RET_BUSY;
    }

    reqResult->channelIdx = chIdx;
    reqResult->deviceIdx = devIdx;

    return RET_OK;
}



/**
 * @brief Initialize DMA transaction handler.
 *
 * This intializes DMA transcation handler elements (specify DMA device handler, etc)
 *
 * @param  pDMA      Pointer to DMA device handler.
 * @param  pConfig   Pointer to DMA configuration structure.
 * @param  hDMA      Pointer to DMA transaction handler.
 *
 * @retval RET_OK    Always returns RET_OK.
 */
static RET_CODE DMA_InitHandle(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{
    ASSERT(hDMA != NULL && pConfig != NULL && pDMA != NULL);

    if (pDMA != s_dmaBases)
    {
        return RET_INVPARAM;
    }

    /* check whether handle is used */
    if (s_dmaHandle[pConfig->channelIdx] != NULL)
    {
        return RET_BUSY;
    }

    s_dmaHandle[pConfig->channelIdx] = hDMA;
    hDMA->pDevice = pDMA;
    hDMA->chIdx = pConfig->channelIdx;
    hDMA->devIdx = pConfig->deviceIdx;
    hDMA->direction = pConfig->direction;
    hDMA->increment = pConfig->increment;
    hDMA->width = pConfig->width;
    hDMA->alter = pConfig->alter;
    hDMA->callback = pConfig->callback;
    //hDMA->pesEnable = pConfig->pesEnable;
    //hDMA->pesChnIndex = pConfig->pesChnIndex;
    /*DMA is now in initial state*/
    hDMA->state = DMA_STATE_INITIAL;
    //hDMA->pParent  = (void *) NULL;

    return RET_OK;
}


RET_CODE HAL_DMA_Init(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{
    RET_CODE ret = RET_ERROR;

    ASSERT(pDMA != NULL && pConfig != NULL && hDMA != NULL);

    /*DMA clock enable*/
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_DMA);

    /*DMA reset*/
    ret = HAL_Reset_Module(RESET_DMA_SW_RSTJ);
    if (ret)
    {
        return ret;
    }

    if (!LL_DMA_DEV_ISACTIVE(pDMA))
    {
        LL_DMA_DEV_ENABLE(pDMA);
        HAL_DMA_HostIrqConfig((DMA_Handle_T *)pDMA);
    }

    /*Initialize DMA transaction handler*/
    ret = DMA_InitHandle(pDMA, pConfig, hDMA);
    if (ret != RET_OK)
    {
        return ret;
    }

    LL_DMA_CHANNEL_DISABLE(hDMA->pDevice, hDMA->chIdx);
    LL_DMA_INTERRUPT_DISABLE(hDMA->pDevice, hDMA->chIdx);
    LL_DMA_CLEAR_TRCMPLT(hDMA->pDevice, hDMA->chIdx);
    LL_DMA_CLEAR_ERR(hDMA->pDevice, hDMA->chIdx);

    /* CTRL Struct */
    if ((pConfig->alter == DMA_ALTER) || (pConfig->alter == DMA_PINGPONG))
    {
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL = 0;
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL =
                        (0 << 8) |                   //[31:8] n_minus_1, set it in dma_start
                        (pConfig->width << 6) |      //[7:6] src size
                        (pConfig->burst << 4) |      //[5:4] R_power, not used currently
                        (1 << 3) |                   //[3] burst, not used currently
                        (0 << 0); //[2:0] cycle_ctrl

        if (pConfig->alter == DMA_PINGPONG)
        {
            p_dmaCtrl->Alter_CH[pConfig->channelIdx].CTRL |= (3 << 0);
        }
        else
        {
            if (pConfig->direction == DMA_MEMORY_TO_MEMORY)
            {
                p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= (2 << 0); //auto request
            }
            else
            {

                p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= (1 << 0); //base trans
            }
        }
    }
    if ((pConfig->alter == DMA_PRIMARY) || (pConfig->alter == DMA_PINGPONG))
    {
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL = 0;
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL =
                        (0 << 8) |                   //[31:8] n_minus_1, set it in dma_start
                        (pConfig->width << 6) |      //[7:6] src size
                        (pConfig->burst << 4) |      //[5:4] R_power, not used currently
                        (1 << 3) |                   //[3] burst, not used currently
                        (0 << 0); //[2:0] cycle_ctrl

        if (pConfig->alter == DMA_PINGPONG)
        {
            p_dmaCtrl->Prim_CH[pConfig->channelIdx].CTRL |= (3 << 0);
        }
        else
        {
            if (pConfig->direction == DMA_MEMORY_TO_MEMORY)
            {
                p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= (2 << 0); //auto request
            }
            else
            {
                p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= (1 << 0); //base trans
            }
        }
    }

    /* Primary control data struct base address */
    LL_DMA_CTRL_BASE_PTR(hDMA->pDevice, (uint32_t)(&(p_dmaCtrl->Prim_CH[0])));

    /* Alternate control data struct base address */
    LL_DMA_ALT_CTRL_BASE_PTR(hDMA->pDevice, (uint32_t)(&(p_dmaCtrl->Alter_CH[0])));

    if (pConfig->direction == DMA_MEMORY_TO_MEMORY) //memory to memory
    {
        LL_DMA_CHAL_REQ_MASK_SET(hDMA->pDevice, hDMA->chIdx);
    }
    else
    {
        LL_DMA_CHAL_REQ_MASK_CLR(hDMA->pDevice, hDMA->chIdx);
    }

    /* use the Alter data struct or Primary data struct */
    if (pConfig->alter == DMA_ALTER)
    {
        LL_DMA_CHAL_PRI_ALT_SET(hDMA->pDevice, hDMA->chIdx);
    }
    else
    {
        LL_DMA_CHAL_PRI_ALT_CLR(hDMA->pDevice, hDMA->chIdx);
    }

    /* use the Alter data struct or Primary data struct */
    if (pConfig->priority == DMA_PRIORITY_HIGH)
    {
        LL_DMA_CHAL_PRIORITY_SET(hDMA->pDevice, hDMA->chIdx);
    }
    else
    {
        LL_DMA_CHAL_PRIORITY_CLR(hDMA->pDevice, hDMA->chIdx);
    }

    /* master handshake select */
    if (hDMA->chIdx < 8)
    {
        hDMA->devIdx &= 0xf;
        LL_DMA_CHAL_CH_CFG0_CHAL(hDMA->pDevice, hDMA->chIdx * 4);
        LL_DMA_CHAL_CH_CFG0_DEV(hDMA->pDevice, hDMA->devIdx << (hDMA->chIdx * 4));
    }
    else
    {
        hDMA->devIdx &= 0xf;
        LL_DMA_CHAL_CH_CFG1_CHAL(hDMA->pDevice, (hDMA->chIdx - 8) * 4);
        LL_DMA_CHAL_CH_CFG1_DEV(hDMA->pDevice, hDMA->devIdx << ((hDMA->chIdx - 8) * 4));
    }

    /* config the src/dst address change or not */
    LL_DMA_NO_INC_CHAL_SELECT(hDMA->pDevice, hDMA->chIdx * 2);
    LL_DMA_NO_INC_INC_SELECT(hDMA->pDevice, pConfig->increment << (hDMA->chIdx * 2));

    hDMA->state = DMA_STATE_READY;
    return RET_OK;
}


RET_CODE HAL_DMA_Abort(DMA_Handle_T *hDMA)
{
    DMA_ChannelIdx_T chIdx = hDMA->chIdx;

    ASSERT(hDMA != NULL);

    if (hDMA->state == DMA_STATE_BUSY)
    {
        return RET_ERROR;
    }

    LL_DMA_CHANNEL_DISABLE(hDMA->pDevice, chIdx);
    LL_DMA_INTERRUPT_DISABLE(hDMA->pDevice, chIdx);
    LL_DMA_CLEAR_TRCMPLT(hDMA->pDevice, chIdx);
    LL_DMA_CLEAR_ERR(hDMA->pDevice, chIdx);

    memset(s_dmaHandle[chIdx], 0, sizeof(DMA_Handle_T));
    s_dmaHandle[chIdx] = NULL;

    /*Change DMA state back to DMA_STATE_INITIAL*/
    hDMA->state = DMA_STATE_INITIAL;

    return RET_OK;
}

RET_CODE HAL_DMA_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t size)
{
    uint32_t byteSize = 0;
    __IO uint32_t srcAddressTmp, destAddressTmp;

    ASSERT(hDMA != NULL);

    if (size > MAX_BLOCK_SIZE_LENGTH)
    {
        return RET_INVPARAM;
    }

    /*Use physis address if SDRAM mappint to 0 address*/
    if (((srcAddress) & 0xF0000000) == 0)
    {
        srcAddress |= 0xC0000000;
    }
    if (((destAddress) & 0xF0000000) == 0)
    {
        destAddress |= 0xC0000000;
    }


    /*Indicating DMA is busy*/
    hDMA->state = DMA_STATE_BUSY;

    /* transfer the srcAddr/dstAddr to the DMA device need format */
    if (hDMA->width == DMA_WIDTH_16BIT)
    {
        byteSize = size * 2;
    }
    else if (hDMA->width == DMA_WIDTH_32BIT)
    {
        byteSize = size * 4;
    }
    else
    {
        byteSize = size * 1;
    }

    if (hDMA->increment == DMA_SRC_INC_DST_INC)
    {
        srcAddressTmp = srcAddress + byteSize - 1;
        destAddressTmp = destAddress + byteSize - 1;
    }
    else if (hDMA->increment == DMA_SRC_KEEP_DST_INC)
    {
        srcAddressTmp = srcAddress;
        destAddressTmp = destAddress + byteSize - 1;
    }
    else if (hDMA->increment == DMA_SRC_INC_DST_KEEP)
    {
        srcAddressTmp = srcAddress + byteSize - 1;
        destAddressTmp = destAddress;
    }

    /* CTRL Struct */
    if (hDMA->alter == DMA_ALTER)
    {
        p_dmaCtrl->Alter_CH[hDMA->chIdx].SRC_END = srcAddressTmp;
        p_dmaCtrl->Alter_CH[hDMA->chIdx].DES_END = destAddressTmp;
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL &= ~(0xFFFFFF00); //clear CTRL[31:8] --> n_minus_1
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= ((size - 1) << 8);//[31:8] n_minus_1
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL &= ~(0x7); //clear CTRL[2:0]
        if (hDMA->direction == DMA_MEMORY_TO_MEMORY)
        {
            p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= (2 << 0); //auto request
        }
        else
        {
            p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= (1 << 0); //base trans
        }
    }
    if (hDMA->alter == DMA_PRIMARY)
    {
        p_dmaCtrl->Prim_CH[hDMA->chIdx].SRC_END = srcAddressTmp;
        p_dmaCtrl->Prim_CH[hDMA->chIdx].DES_END = destAddressTmp;
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL &= ~(0xFFFFFF00); //clear CTRL[31:8] --> n_minus_1
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= ((size - 1) << 8);//[31:8] n_minus_1
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL &= ~(0x7); //clear CTRL[2:0]
        if (hDMA->direction == DMA_MEMORY_TO_MEMORY)
        {
            p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= (2 << 0); //auto request
        }
        else
        {
            p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= (1 << 0); //base trans
        }
    }

#ifdef CONFIG_GM_HAL_DCACHE
    HAL_DCACHE_Flush();
#endif

    LL_DMA_INTERRUPT_ENABLE(hDMA->pDevice, hDMA->chIdx);
    LL_DMA_CHANNEL_ENABLE(hDMA->pDevice, hDMA->chIdx);

    if (hDMA->direction == DMA_MEMORY_TO_MEMORY)
    {
        LL_DMA_CHAL_SW_REQUEST_SET(hDMA->pDevice, hDMA->chIdx);
    }
    else
    {
        LL_DMA_CHAL_SW_REQUEST_CLR(hDMA->pDevice, hDMA->chIdx);
    }

    return RET_OK;
}


RET_CODE HAL_DMA_MultiBuffer_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t SecondMemAddress, uint32_t size)
{
    uint32_t byteSize = 0;
    __IO uint32_t srcAddressTmp, destAddressTmp;
    __IO uint32_t secondMemAddressTmp;

    ASSERT(hDMA != NULL);

    if (size > MAX_BLOCK_SIZE_LENGTH)
    {
        return RET_INVPARAM;
    }

    /*Use physis address if SDRAM mappint to 0 address*/
    if ((srcAddress & 0xF0000000) == 0)
    {
        srcAddress |= 0xC0000000;
    }
    if ((destAddress & 0xF0000000) == 0)
    {
        destAddress |= 0xC0000000;
    }

    /*Indicating DMA is busy*/
    hDMA->state = DMA_STATE_BUSY;

    hDMA->size = size;

    /* transfer the srcAddr/dstAddr to the DMA device need format */
    if (hDMA->width == DMA_WIDTH_16BIT)
    {
        byteSize = size * 2;
    }
    else if (hDMA->width == DMA_WIDTH_32BIT)
    {
        byteSize = size * 4;
    }
    else
    {
        byteSize = size * 1;
    }

    if (hDMA->increment == DMA_SRC_INC_DST_INC)
    {
        return RET_INVPARAM;
        //srcAddressTmp = srcAddress + byteSize - 1;
        //destAddressTmp = destAddress + byteSize - 1;
    }
    else if (hDMA->increment == DMA_SRC_KEEP_DST_INC)
    {
        srcAddressTmp = srcAddress;
        destAddressTmp = destAddress + byteSize - 1;
    }
    else if (hDMA->increment == DMA_SRC_INC_DST_KEEP)
    {
        srcAddressTmp = srcAddress + byteSize - 1;
        destAddressTmp = destAddress;
    }
    secondMemAddressTmp = SecondMemAddress + byteSize - 1;

    /* CTRL Struct */
    if (1) //((hDMA->alter == DMA_PRIMARY) || (hDMA->alter == DMA_PINGPONG))
    {
        p_dmaCtrl->Prim_CH[hDMA->chIdx].SRC_END = srcAddressTmp;
        p_dmaCtrl->Prim_CH[hDMA->chIdx].DES_END = destAddressTmp;
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL &= ~(0xFFFFFF00); //clear CTRL[31:8] --> n_minus_1
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= ((size - 1) << 8);//[31:8] n_minus_1
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL &= ~(0x7); //clear CTRL[2:0]
        p_dmaCtrl->Prim_CH[hDMA->chIdx].CTRL |= (3 << 0); //pingpong
    }
    if (1) //((hDMA->alter == DMA_ALTER) || (hDMA->alter == DMA_PINGPONG))
    {
        if (hDMA->increment == DMA_SRC_INC_DST_KEEP)
        {
            p_dmaCtrl->Alter_CH[hDMA->chIdx].SRC_END = secondMemAddressTmp;
            p_dmaCtrl->Alter_CH[hDMA->chIdx].DES_END = destAddressTmp;
        }
        else//DMA_SRC_KEEP_DST_INC
        {
            p_dmaCtrl->Alter_CH[hDMA->chIdx].SRC_END = srcAddressTmp;
            p_dmaCtrl->Alter_CH[hDMA->chIdx].DES_END = secondMemAddressTmp;
        }
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL &= ~(0xFFFFFF00); //clear CTRL[31:8] --> n_minus_1
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= ((size - 1) << 8);//[31:8] n_minus_1
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL &= ~(0x7); //clear CTRL[2:0]
        p_dmaCtrl->Alter_CH[hDMA->chIdx].CTRL |= (3 << 0); //pingpong
    }

#ifdef CONFIG_GM_HAL_DCACHE
    HAL_DCACHE_Flush();
#endif

    LL_DMA_INTERRUPT_ENABLE(hDMA->pDevice, hDMA->chIdx);
    LL_DMA_CHANNEL_ENABLE(hDMA->pDevice, hDMA->chIdx);

    if (hDMA->direction == DMA_MEMORY_TO_MEMORY)
    {
        LL_DMA_CHAL_SW_REQUEST_SET(hDMA->pDevice, hDMA->chIdx);
    }
    else
    {
        LL_DMA_CHAL_SW_REQUEST_CLR(hDMA->pDevice, hDMA->chIdx);
    }

    return RET_OK;
}

/**
 * @brief Handles DMA interrupt request.
 *
 * This function handles DMA each individual interrupt, it clears corresponding interrupt flag,
 * and calls callback function if callback is available.
 *
 * @param  pHndCb    Pointer to a DMAHdlCb_T structure that contains
 *                   DMA transaction handler and callback function.
 * @retval None
 */
static void DMA_CommonIRQHandler(DMA_Handle_T **hDMA)
{
    DMA_Device_T *dmaDevice = DMA_DEV;
    uint32_t intStatus = dmaDevice->IS;
    //DMA_Event_T cbEvt = DMA_EVENT_TRANSFER_DONE;
    uint32_t i = 0;

#if 0
    if (intStatus != 0)
    {
        /* Clear IS */
        dmaDevice->IS = 0;
    }
#endif

    for (i = 0; i < DMA_MAX_CHANNEL; i++)
    {
        if ((intStatus & (1 << i)) && (hDMA[i] != NULL))
        {
#ifdef CONFIG_GM_HAL_DCACHE
            HAL_DCACHE_InvalidAll();
#endif
            LL_DMA_CLEAR_TRCMPLT(hDMA[i]->pDevice, i);
            hDMA[i]->state = DMA_STATE_INITIAL;


            if (hDMA[i]->alter == DMA_PINGPONG)
            {
                if ((p_dmaCtrl->Prim_CH[hDMA[i]->chIdx].CTRL >> 8) == 0) //n_minus_1 is 0
                {
                    if (hDMA[i]->callback != NULL)
                    {
                        hDMA[i]->callback(hDMA[i], DMA_EVENT_PINGPONG_PRIM_DONE, 0);
                    }
                    p_dmaCtrl->Prim_CH[hDMA[i]->chIdx].CTRL &= ~(0xFFFFFF00); //clear CTRL[31:8] --> n_minus_1
                    p_dmaCtrl->Prim_CH[hDMA[i]->chIdx].CTRL |= ((hDMA[i]->size - 1) << 8);//[31:8] n_minus_1
                    p_dmaCtrl->Prim_CH[hDMA[i]->chIdx].CTRL &= ~(0x7); //clear CTRL[2:0]
                    p_dmaCtrl->Prim_CH[hDMA[i]->chIdx].CTRL |= (3 << 0); //pingpong
                }
                if ((p_dmaCtrl->Alter_CH[hDMA[i]->chIdx].CTRL >> 8) == 0) //n_minus_1 is 0
                {
                    if (hDMA[i]->callback != NULL)
                    {
                        hDMA[i]->callback(hDMA[i], DMA_EVENT_PINGPONG_ALTER_DONE, 0);
                    }
                    p_dmaCtrl->Alter_CH[hDMA[i]->chIdx].CTRL &= ~(0xFFFFFF00); //clear CTRL[31:8] --> n_minus_1
                    p_dmaCtrl->Alter_CH[hDMA[i]->chIdx].CTRL |= ((hDMA[i]->size - 1) << 8);//[31:8] n_minus_1
                    p_dmaCtrl->Alter_CH[hDMA[i]->chIdx].CTRL &= ~(0x7); //clear CTRL[2:0]
                    p_dmaCtrl->Alter_CH[hDMA[i]->chIdx].CTRL |= (3 << 0); //pingpong
                }
            }
            else
            {
                if (hDMA[i]->callback != NULL)
                {
                    hDMA[i]->callback(hDMA[i], DMA_EVENT_TRANSFER_DONE, 0);
                }
            }
        }
    }
}


