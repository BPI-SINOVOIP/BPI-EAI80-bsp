/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_pdm.c
 *
 * @author      Robert_hu
 *
 * @version     v1.0.0
 *
 * @date        2019/11/1
 *
 * @brief       PDM Source file
 *
 * @note
 *             2019/11/1, Robert_hu, v1.0.0
 *                  Initial version.
 *
 **/

#include <gm_hal_pdm.h>
#include <gm_hal_dma.h>
#include "../dma/gm_ll_dma.h"
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

#define PDM_FCLK   48000000
#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
static uint8_t s_dmaError    = 0;
uint32_t s_dmaComplete =  0;
static DMA_Handle_T *s_dmaHandle;
static DMA_Handle_T s_dmaHandle_a;
static DMA_Handle_T s_dmaHandle_b;
static DMA_Handle_T s_dmaHandle_c;
static DMA_Handle_T s_dmaHandle_d;
#if (!defined(require_action_quiet))
#define require_action_quiet( X, LABEL, ACTION )    \
    do                                              \
    {                                               \
        if( ( !(X) ) )                              \
        {                                           \
            { ACTION; }                             \
            goto LABEL;                             \
        }                                           \
        \
    }   while( 1==0 )
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


PDM_Handle_T *g_pdm_handle = NULL;

/**
  * @brief  The following  function are used to  PDM  Irq handler.
  * @note  software has to enable peripheral interrput  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_PDM_InterruptHandler(void)
{
    PR_DEBUG("Enter fun[%s]\n", __FUNCTION__);
    if (g_pdm_handle != NULL)
    {
        //PDM_CLEAR_INTERRUPT(g_pdm_handle->device);
    }
    //PDM_CommonIRQHandler();
}

/**
  * @brief  Initializes the ACMP according to the specified
  *         parameters in the ACMP_Config_T.
  * @param  pConfig: pointer to a ACMP_Handle_T structure.
  * @param config: pointer to a ACMP_Config_T structure that contains
  *         the configuration information for ACMP module
  * @retval void
  */
static void PDM_InterruptConfig(PDM_Handle_T *pHandle, PDM_Config_T *pConfig)
{

    // COMPARE_MATCH_TRIGGER(pHandle->device, pConfig->triggerMode);
    // ACMP_INTERRUPT_ENABLE(pHandle->device);

}
/**
  * @brief  The following  function are used to  ACMP peripheral Request Irq.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_PDM_RequestIrq(void)
{
    //  HAL_RequestIrq(PDM_IRQn, PDM_IRQ_PRIORITY, HAL_PDM_InterruptHandler, 0);
    //    HAL_IRQ_ENABLE(PDM_IRQn);
}

/**
  * @brief  The following  function are used to reset ACMP peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_PDM_SW_Reset(void)
{
    HW32_REG(0x40000084) |= (PDM_SW_RSTJ);
}

/**
  * @brief  The following two functions are used to enable/disable peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_PDM_CLK_ENABLE(void)
{
    HW32_REG(0x40000074) |= PDM_CLK_EN;
}

void HAL_PDM_CLK_DISABLE(void)
{
    HW32_REG(0x40000074) &= ~ PDM_CLK_EN;
}

/**
  * @brief  Initializes the PDM according to the specified
  *         parameters in the PDM_Config_T.
  * @param  pConfig: pointer to a PDM_Handle_T structure.
  * @param config: pointer to a PDM_Config_T structure that contains
  *         the configuration information for PDM module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PDM_Init(PDM_Handle_T *pHandle, PDM_Device_T *pPDM, PDM_Config_T *pConfig)
{
    HAL_StatusTypeDef ret = HAL_PDM_OK;
    uint32_t clkfrqs = PDM_FCLK;
    if (!(pHandle && pPDM && pConfig))
    {
        ret = RET_INVPARAM;
        goto out;
    }
    pConfig->bck = clkfrqs / (pConfig->bckdiv + 1);
    pConfig->DECIMATION = pConfig->bck / pConfig->FS + 1;
    pHandle->device = pPDM;

    g_pdm_handle = pHandle;
    HAL_PDM_CLK_ENABLE();
    HAL_PDM_SW_Reset();
    pHandle->pdm_callback  = pConfig->pdm_callback;
    pHandle->param         = pConfig->param;
    pHandle->enableDMA     = pConfig->enableDMA;
    PDM_DISABLE(pHandle->device);

    if (pConfig->Cid == ENABLE)
    {
        pPDM->PDM |= PDM_CID_ENABLE;
    }
    else
    {
        pPDM->PDM &= ~ PDM_CID_ENABLE;
    }

    if (pConfig->Capt == ENABLE)
    {
        pPDM->PDM |= PDM_CAPT_ENABLE;
    }

    if (pConfig->CH_FLAG == PDM_CH_RIGHT)
    {
        pPDM->PDM |= PDM_CHANNEL_RIGHT;
    }
    else
    {
        pPDM->PDM &= ~ PDM_CHANNEL_RIGHT;
    }

    if (pConfig->Dual_Ch == PDM_Dual_R)
    {
        pPDM->PDM |= PDM_DUAL_CH_RIGHT;
    }
    else if (pConfig->Dual_Ch == PDM_Dual_L)
    {
        pPDM->PDM |= PDM_DUAL_CH_LEFT;
    }
    else
    {
        pPDM->PDM |= PDM_DUAL_CH;
    }

    pPDM->PDM |= (pConfig->DECIMATION << PDM_DECIMATION_POS);

    pPDM->PDM |= pConfig->Shift_Level;
    pPDM->CCR |= pConfig->bckdiv;
    pPDM->CFG2 |= pConfig->Fifo_Thr;

    _PDM_SLOT_ENABLE(pHandle->device);

    pPDM->SLOT |= pConfig->NBSlot;
    *(volatile uint32_t *)0x4003f00c |= (1 << 6);

    pPDM->SLOT |= pConfig->SlotSize;

    _PDM_DMA_ENABLE(pHandle->device);

    pPDM->CFG1 &= ~ PDM_DATASIZE_32;
    pPDM->CFG1 |= pConfig->DataSize;

    _PDM_MCK_ENABLE(pHandle->device);

    _PDM_BCK_ENABLE(pHandle->device);

    _PDM_ENABLE(pHandle->device);
    /*
     *config irq match trigger mode
     *enable acmp irq and nvic for acmp
     *regist acmp event callback
     */
    //PDM_InterruptConfig(pHandle, pConfig);

    //HAL_PDM_RequestIrq();
#ifdef RABBIT_NO_OS_SYSTEM
    //HAL_IRQ_ENABLE(PDM_IRQn);
#endif

out:
    return ret;
}

HAL_StatusTypeDef HAL_PDM_Exit(PDM_Handle_T *pHandle)
{
    PDM_DISABLE(pHandle->device);
    _PDM_BCK_DISABLE(pHandle->device);
    _PDM_MCK_DISABLE(pHandle->device);
    _PDM_DMA_DISABLE(pHandle->device);
    _PDM_SLOT_DISABLE(pHandle->device);
    return 0;
}

static void PDM_DMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    PDM_Handle_T *pHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, struct PDM_Handle);

    //    if (event == DMA_EVENT_TRANSFER_DONE)
    if (event == DMA_EVENT_PINGPONG_PRIM_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
        pHandle->ErrorCode = PDM_ERRCODE_NOERROR;
        if (pHandle->pdm_callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pHandle->pdm_callback(pHandle, PDM_DMAMST_RECEIVE_EVENT, pHandle->param);
        }
    }

    if (event == DMA_EVENT_PINGPONG_ALTER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
        pHandle->ErrorCode = PDM_ERRCODE_NOERROR;
        if (pHandle->pdm_callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pHandle->pdm_callback(pHandle, PDM_DMASLV_RECEIVE_EVENT, pHandle->param);
        }
    }
    if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
        pHandle->ErrorCode = PDM_ERRCODE_DMAERROR;
        if (pHandle->pdm_callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pHandle->pdm_callback(pHandle, PDM_RECEIVE_EVENT_ERROR, pHandle->param);
        }
    }
    //HAL_DMA_Abort(hDMA);
}

static HAL_StatusTypeDef PDM_DMAConfig(PDM_Device_T *pPDM, DMA_Handle_T *hDMA)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = HAL_PDM_ERROR;

    ASSERT(pPDM != NULL && hDMA != NULL);

    if (pPDM == PDMA_DEV)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_PDM_A, &reqResult);
    }
    else if (pPDM == PDMB_DEV)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_PDM_B, &reqResult);
    }
    else if (pPDM == PDMC_DEV)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_PDM_C, &reqResult);
    }
    else if (pPDM == PDMD_DEV)
    {
        ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_PDM_D, &reqResult);
    }
    if (ret != HAL_PDM_OK)
    {
        return HAL_PDM_ERROR;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));

    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx  = reqResult.deviceIdx;
    dmaConfig.direction  = DMA_PERIPH_TO_MEMORY;
    dmaConfig.increment  = DMA_SRC_KEEP_DST_INC;
    dmaConfig.width      = DMA_WIDTH_16BIT;
    dmaConfig.burst      = DMA_BURST_1;
    dmaConfig.priority   = DMA_PRIORITY_HIGH;
    dmaConfig.alter      = DMA_PINGPONG;
    dmaConfig.callback   = PDM_DMACallback;

    HAL_DMA_Init(DMA_DEV, &dmaConfig, hDMA);
    return  HAL_PDM_OK;
}


/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PDM_Receive_DMA(PDM_Handle_T *pHandle, PDM_WorkMode_T mode, int16_t *pData, uint32_t Size)
{
    HAL_StatusTypeDef error = HAL_PDM_OK;
    uint32_t srcAddress = 0x0;

    if (pHandle->device == PDMA_DEV)
    {
        s_dmaHandle = &s_dmaHandle_a;
    }
    if (pHandle->device == PDMB_DEV)
    {
        s_dmaHandle = &s_dmaHandle_b;
    }
    if (pHandle->device == PDMC_DEV)
    {
        s_dmaHandle = &s_dmaHandle_c;
    }
    if (pHandle->device == PDMD_DEV)
    {
        s_dmaHandle = &s_dmaHandle_d;
    }
    require_action_quiet(PDM_DMAConfig(pHandle->device, s_dmaHandle) == HAL_PDM_OK, \
                         exit, error = HAL_PDM_BUSY; pHandle->ErrorCode = PDM_ERRCODE_DMAERROR;);

    /* Associate DMA Handle with PDM handle */
    LINK_DMAHDL_TO_PARENTHDL(pHandle, hDMA, *s_dmaHandle);
    /* Configure DMA source address */
    srcAddress = (uint32_t)(&(pHandle->device->DR));

    if ((s_dmaHandle->devIdx == DMA_DEV_3 && s_dmaHandle->chIdx == DMA_CHANNEL_0) ||
            (s_dmaHandle->devIdx == DMA_DEV_1 && s_dmaHandle->chIdx == DMA_CHANNEL_1) ||
            (s_dmaHandle->devIdx == DMA_DEV_3 && s_dmaHandle->chIdx == DMA_CHANNEL_3) ||
            (s_dmaHandle->devIdx == DMA_DEV_2 && s_dmaHandle->chIdx == DMA_CHANNEL_2) ||
            (s_dmaHandle->devIdx == DMA_DEV_1 && s_dmaHandle->chIdx == DMA_CHANNEL_4) ||
            (s_dmaHandle->devIdx == DMA_DEV_0 && s_dmaHandle->chIdx == DMA_CHANNEL_6) ||
            (s_dmaHandle->devIdx == DMA_DEV_0 && s_dmaHandle->chIdx == DMA_CHANNEL_15))
    {
        error = HAL_PDM_OK;
    }
    else
    {
        return PDM_ERRCODE_DMAERROR;
    }
    /* Start DMA transcation */
    //    require_action_quiet(HAL_DMA_Start(pHandle->hDMA, srcAddress, (uint32_t)pData, Size) == HAL_PDM_OK, \
    //                         exit, error = HAL_PDM_ERROR; pHandle->ErrorCode = PDM_ERRCODE_DMAERROR;);
    HAL_DMA_MultiBuffer_Start(pHandle->hDMA, srcAddress, (uint32_t)pData, (uint32_t)(&pData[Size / 2]), Size / 2);
exit:
    return error;
}

// void PDM_CommonIRQHandler(PDM_Handle_T *pHandle)
//{
//    __IO uint32_t intStatus = pHandle->device->IM;
//    __IO uint32_t Status = pHandle->device->ST;
//    __IO uint8_t curRxFIFO;

//    uint32_t cbPara = 0;
//    uint8_t i;
//    curRxFIFO = IPDM_GET_FIFOTHRD_RX(pHandle->device);

//  if (pHandle->enableDMA != TRUE)
//  {
//      if (intStatus & PDM_FTH_FREQ_ENABLE)
//      {
//          /* Read the data from FIFO */
////            for(i = 0;i < curRxFIFO; i ++)
////            {
////                *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) ;
////                 hI2C->rxRemained --;
////            }

////            if (curRxFIFO == 0)
////            {
////                *hI2C->pRxBuffer ++ = (hI2C->pDevice->DT) ;
////                 hI2C->rxRemained --;

////                I2C_SET_FIFOTHRD_RX(hI2C->pDevice, hI2C->rxThreshld);
////            }
////            else
////            {
////                  if (hI2C->rxRemained <= hI2C->rxThreshld)
////                  {
////                      I2C_SET_FIFOTHRD_RX(hI2C->pDevice, MIN(hI2C->rxRemained, curRxFIFO) - 1);
////                  }
////            }
////            /* Clear FIFO  Rx almost FULL. Hardware clear it automatically, without involvement of software */
//      }

////        if (hI2C->rxRemained == 0)
////        {
////            hI2C->state = I2C_STATE_IDLE;
////            hI2C->errorCode = I2C_ERRCODE_NOERROR;

////            /* So that no any other interrupt is underway */
////            I2C_DISABLE_INTERRUPT(hI2C->pDevice, I2C_IE_RFAF | I2C_IE_RFO |
////                                                 I2C_IE_RFU  | I2C_IE_TXL);

////            if(hI2C->callback != NULL)
////            {
////                hI2C->callback(hI2C, event, cbPara);
////            }
////        }
//  }

////    if (intStatus & I2C_IS_TXL)
////    {
////        if (hI2C->txRemained != 0 || hI2C->rxRemained != 0)
////        {
////            event = I2C_EVENT_ARB_LOST;
////            cbPara = hI2C->pDevice->ERR;
////            hI2C->errorCode |= I2C_ERRCODE_ARBITRATION_LOST;

////            if (hI2C->callback != NULL)
////            {
////                hI2C->callback(hI2C, event, cbPara);
////            }
////        }

////        /* Clear ARB LOST */
////        I2C_CLEAR_ABRT_LOST(hI2C->pDevice);
////    }

//}

















































































































