/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_sqi.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>

#include <gm_hal_dma.h>
#include "dma_test.h"

#define BLOCK_SIZE          (16)//0x1000//
#define RX_BUFF_SIZE        (BLOCK_SIZE*4)//0x1000//(BLOCK_SIZE*4)


static DMA_Handle_T s_dmaHandle;

static volatile  uint32_t s_dmaComplete = 0;
static volatile  uint32_t s_dmaError = 0;
static char src_data[RX_BUFF_SIZE] = { 0 };
static char dst_data[RX_BUFF_SIZE] = { 0 };

static void dmaCallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
    }
    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
    }
}

static int test_dma(void)
{
    TC_PRINT("ENTER DMA HAL TEST\n");
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;
    DMA_Handle_T *pDMAHandle = &s_dmaHandle;
    uint32_t i = 0;
    uint32_t compare_size;

    memset(src_data, RX_BUFF_SIZE, 0);
    memset(dst_data, RX_BUFF_SIZE, 0);
    for (i = 0; i < RX_BUFF_SIZE; i++)
    {
        src_data[i] = i;
    }

    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_M2M, &reqResult);
    if (ret != RET_OK)
    {
        //fail, need return fail status
        return TC_FAIL;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_MEMORY_TO_MEMORY;
    dmaConfig.increment    = DMA_SRC_INC_DST_INC;
    dmaConfig.width        = DMA_WIDTH_32BIT;
    dmaConfig.priority     = DMA_PRIORITY_LOW;//DMA_PRIORITY_HIGH;
    dmaConfig.burst        = 0;
    dmaConfig.alter        = DMA_PRIMARY;//DMA_ALTER;
    dmaConfig.callback = dmaCallback;

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);

    HAL_DMA_Start(pDMAHandle, (uint32_t)src_data, (uint32_t)dst_data, BLOCK_SIZE);

    /* Wait till either DMA completes or error occures */
    while ((s_dmaComplete == 0) && (s_dmaError == 0))
    {};

    if (dmaConfig.width == DMA_WIDTH_32BIT)
    {
        compare_size = BLOCK_SIZE * 4;
    }
    else if (dmaConfig.width == DMA_WIDTH_16BIT)
    {
        compare_size = BLOCK_SIZE * 2;
    }
    else if (dmaConfig.width == DMA_WIDTH_8BIT)
    {
        compare_size = BLOCK_SIZE * 1;
    }
    if (s_dmaComplete == 1)
    {
        if (memcmp(src_data, dst_data, compare_size) != 0)
        {
            TC_PRINT("ERROR: dma data error\n");
            return RET_ERROR;
        }
    }
    HAL_DMA_Abort(pDMAHandle);

    TC_PRINT("PASS\n");
    return TC_PASS;

}

void dma_test(void)
{
    zassert_true(test_dma() == TC_PASS, NULL);
}