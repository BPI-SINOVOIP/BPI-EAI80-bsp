/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <device.h>
#include "board.h"
#include <ztest.h>
#include <gm_hal_dma.h>
#include <gm_hal_sai.h>
#include "sai_test.h"
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

#define SAI_MASTER_DEVICE       (SAI1_Block_A)
#define SAI_SLAVE_DEVICE        (SAI1_Block_B)
#define  SAI_MAX_DATALEN         16
static uint32_t s_wrBuf[SAI_MAX_DATALEN] = {0};
static uint32_t s_rdBuf[SAI_MAX_DATALEN] = {0};
extern uint32_t s_mstTransCmplt;
extern uint32_t s_slvTransCmplt;
static uint32_t s_saia_dmabuffer[SAI_MAX_DATALEN] = {0};
static uint32_t s_saib_dmabuffer[SAI_MAX_DATALEN] = {0};

void sai_reset_and_clk_release(void)
{
    //release the sai_a reset
    HW32_REG(0x40000074) |= (1 << 24);

    //release the sai_b reset
    HW32_REG(0x40000074) |= (1 << 25);

    //release the sai_a clock
    HW32_REG(0x40000084) |= (1 << 24);

    //release the sai_b clock
    HW32_REG(0x40000084) |= (1 << 25);

    HW32_REG(0x40000198) |= (1 << 0); //D_PLLSAI_EN

    HW32_REG(0x40000190) |= (1 << 0); //D_PLLI2S_EN

}
int test_sai_int()
{
    uint32_t i, j = 0;
    RET_CODE ret;
    SAI_HandleTypeDef SAI_B_HandleTypeDef;
    SAI_InitTypeDef  SAIB_Init;
    SAI_FrameInitTypeDef  SAIB_FrameInit;
    SAI_SlotInitTypeDef   SAIB_SlotInit;

    SAI_HandleTypeDef SAI_A_HandleTypeDef;
    SAI_InitTypeDef  SAIA_Init;
    SAI_FrameInitTypeDef  SAIA_FrameInit;
    SAI_SlotInitTypeDef   SAIA_SlotInit;

    memset(&SAIB_Init, 0, sizeof(SAI_InitTypeDef));
    memset(&SAIA_Init, 0, sizeof(SAI_InitTypeDef));

    sai_reset_and_clk_release();
    /**********sai_b_slave_rx_config **********/
    SAIB_Init.AudioMode = SAI_MODESLAVE_RX; //SLV_RX
    SAIB_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;//FTH=2
    SAIB_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIB_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIB_Init.BCKOEn = SAI_BCK_DISABLE;
    SAIB_Init.MCKOEn = DISABLE;
    SAIB_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIB_Init.MonoStereoMode = SAI_STEREOMODE;//MONO mode
    SAIB_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIB_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIB_Init.Protocol = SAI_I2S_STANDARD;
    SAIB_Init.DataSize = SAI_xCR1_DS; //32BIT
    SAIB_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    //SAIB_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;//FS

    SAIB_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1
    SAI_B_HandleTypeDef.pdmaBuffPtr = s_saib_dmabuffer;
    SAI_B_HandleTypeDef.Init = SAIB_Init ;
    SAI_B_HandleTypeDef.FrameInit = SAIB_FrameInit ;
    SAI_B_HandleTypeDef.SlotInit = SAIB_SlotInit ;

    if (HAL_SAI_InitProtocol(SAI_SLAVE_DEVICE, &SAI_B_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    /**********sai_a_master_tx_config **********/
    SAIA_Init.AudioMode = SAI_MODEMASTER_TX;    //MST_TX
    SAIA_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;//FTH=1
    SAIA_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIA_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIA_Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
    SAIA_Init.BCKOEn = SAI_BCK_ENABLE;
    //SAIA_Init.MCKOEn = SAI_xCR1_MCKSOEN;//MCLK OUTPUT ENABLE
    SAIA_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIA_Init.MonoStereoMode = SAI_STEREOMODE;//MONO mode
    SAIA_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIA_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIA_Init.Protocol = SAI_I2S_STANDARD;
    SAIA_Init.DataSize = SAI_xCR1_DS; //32BIT
    SAIA_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    //SAIA_Init.AudioFrequency = 0;//FS

    SAIA_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1
    SAI_A_HandleTypeDef.pdmaBuffPtr = s_saia_dmabuffer;
    SAI_A_HandleTypeDef.Init = SAIA_Init ;
    SAI_A_HandleTypeDef.FrameInit = SAIA_FrameInit ;
    SAI_A_HandleTypeDef.SlotInit = SAIA_SlotInit ;

    if (HAL_SAI_InitProtocol(SAI_MASTER_DEVICE, &SAI_A_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    for (i = 0; i < SAI_MAX_DATALEN; i ++)
    {
        s_wrBuf[j] = 0x12345680 + i;
        s_rdBuf[j++] = 0;
    }
    //ret = HAL_SAI_Receive(&SAI_B_HandleTypeDef , s_rdBuf, SAI_MAX_DATALEN, 1000000);

    ret = HAL_SAI_Receive_IT(&SAI_B_HandleTypeDef, s_rdBuf, SAI_MAX_DATALEN);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    //ret = HAL_SAI_Transmit(&SAI_A_HandleTypeDef , s_wrBuf, SAI_MAX_DATALEN, 1000000);

    ret = HAL_SAI_Transmit_IT(&SAI_A_HandleTypeDef, s_wrBuf, SAI_MAX_DATALEN);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }

    while (s_slvTransCmplt == 0) {};
    while (s_mstTransCmplt == 0) {};

    s_slvTransCmplt = s_mstTransCmplt = 0;
    for (i = 0; i < SAI_MAX_DATALEN; i ++)
    {
        if (s_rdBuf[i] == s_wrBuf[i])
        {

        }
        else
        {
            return HAL_SAI_ERROR;
        }
    }

    if (HAL_SAI_DeInit(&SAI_A_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }

    if (HAL_SAI_DeInit(&SAI_B_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    return ret;
}


int test_sai_dma()
{
    uint32_t i, j = 0;
    RET_CODE ret;
    SAI_HandleTypeDef SAI_B_HandleTypeDef;
    SAI_InitTypeDef  SAIB_Init;
    SAI_FrameInitTypeDef  SAIB_FrameInit;
    SAI_SlotInitTypeDef   SAIB_SlotInit;

    SAI_HandleTypeDef SAI_A_HandleTypeDef;
    SAI_InitTypeDef  SAIA_Init;
    SAI_FrameInitTypeDef  SAIA_FrameInit;
    SAI_SlotInitTypeDef   SAIA_SlotInit;

    memset(&SAIB_Init, 0, sizeof(SAI_InitTypeDef));
    memset(&SAIA_Init, 0, sizeof(SAI_InitTypeDef));

    sai_reset_and_clk_release();
    /**********sai_b_slave_rx_config **********/
    SAIB_Init.AudioMode = SAI_MODESLAVE_RX; //SLV_RX
    //SAIB_Init.AudioMode = SAI_CLOCKSTROBING_RISINGEDGE;   //CKSTR = 1
    SAIB_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;//FTH=2
    SAIB_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIB_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIB_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIB_Init.MonoStereoMode = SAI_STEREOMODE;//MONO mode
    SAIB_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIB_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIB_Init.Protocol = SAI_I2S_STANDARD;
    SAIB_Init.DataSize = SAI_xCR1_DS; //32BIT
    SAIB_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    SAIB_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_500;//FS
    SAIB_Init.ClockSource = SAI_PLL_11;//

    SAIB_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1
    SAI_B_HandleTypeDef.pdmaBuffPtr = s_saib_dmabuffer;
    SAI_B_HandleTypeDef.Init = SAIB_Init ;
    SAI_B_HandleTypeDef.FrameInit = SAIB_FrameInit ;
    SAI_B_HandleTypeDef.SlotInit = SAIB_SlotInit ;

    if (HAL_SAI_InitProtocol(SAI_SLAVE_DEVICE, &SAI_B_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    /**********sai_a_master_tx_config **********/
    SAIA_Init.AudioMode = SAI_MODEMASTER_TX;    //MST_TX
    SAIA_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;//FTH=1
    SAIA_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIA_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIA_Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
    SAIA_Init.BCKOEn = SAI_BCK_ENABLE;
    SAIA_Init.MCKOEn = SAI_xCR1_MCKSOEN;//MCLK OUTPUT ENABLE
    SAIA_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIA_Init.MonoStereoMode = SAI_STEREOMODE;//MONO mode
    SAIA_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIA_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIA_Init.Protocol = SAI_I2S_STANDARD;
    SAIA_Init.DataSize = SAI_xCR1_DS; //32BIT
    SAIA_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    SAIA_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_500;//FS
    SAIA_Init.ClockSource = SAI_PLL_11;//

    SAIA_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1
    SAI_A_HandleTypeDef.pdmaBuffPtr = s_saia_dmabuffer;
    SAI_A_HandleTypeDef.Init = SAIA_Init ;
    SAI_A_HandleTypeDef.FrameInit = SAIA_FrameInit ;
    SAI_A_HandleTypeDef.SlotInit = SAIA_SlotInit ;

    if (HAL_SAI_InitProtocol(SAI_MASTER_DEVICE, &SAI_A_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }

    for (i = 0; i < SAI_MAX_DATALEN; i ++)
    {
        s_wrBuf[j] = 0x86543210 + i;
        s_rdBuf[j++] = 0;
    }
    ret = HAL_SAI_Receive_IT(&SAI_B_HandleTypeDef, s_rdBuf, SAI_MAX_DATALEN);
    //ret = HAL_SAI_Receive_DMA(&SAI_B_HandleTypeDef ,SAI_SLAVE_MODE, s_rdBuf, SAI_MAX_DATALEN);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    //ret = HAL_SAI_Transmit_IT(&SAI_A_HandleTypeDef , s_wrBuf, SAI_MAX_DATALEN);
    ret = HAL_SAI_Transmit_DMA(&SAI_A_HandleTypeDef, SAI_MASTER_MODE, s_wrBuf, SAI_MAX_DATALEN);
    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    while (s_mstTransCmplt == 0);
    while (s_slvTransCmplt == 0);
    s_mstTransCmplt = s_slvTransCmplt = 0;
    for (i = 0; i < SAI_MAX_DATALEN; i ++)
    {
        if (s_rdBuf[i] == s_wrBuf[i])
        {

        }
        else
        {
            return HAL_SAI_ERROR;
        }
    }
    if (HAL_SAI_DeInit(&SAI_A_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }

    if (HAL_SAI_DeInit(&SAI_B_HandleTypeDef) != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    return ret;
}

void sai_test(void)
{
    zassert_true(test_sai_int() == TC_PASS, NULL);
    //zassert_true(test_sai_dma() == TC_PASS, NULL);
}
