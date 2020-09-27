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
#include <pinmux.h>
#include <gm_hal_gpio.h>


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
    SAIB_Init.FSOEn = DISABLE;
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
    //    SAIA_Init.MCKOEn = SAI_xCR1_MCKSOEN;//MCLK OUTPUT ENABLE
    SAIA_Init.MCKOEn = (1 << 20);//MCLK OUTPUT ENABLE
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

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;


#if defined(CONFIG_SAIA)
static int saia_pinmux_config(void)
{
    int ret = 0;
#ifdef CHIP_GM6721
    struct device *porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }
#endif
#ifdef CHIP_F6721B
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (portc == NULL)
    {
        return -1;
    }
#endif
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_saiPinMux[4] =
    {
#ifdef CHIP_GM6721
        {PINMUX_GPIO_PIN_0, GPIOI, GPIOI0_AF0_SCL},
        {PINMUX_GPIO_PIN_1, GPIOI, GPIOI1_AF0_SDA},
#endif
#ifdef CHIP_F6721B
        {PINMUX_GPIO_PIN_12, GPIOC, GPIOC12_AF4_SAI_MCLK_A},
        {PINMUX_GPIO_PIN_13, GPIOC, GPIOC13_AF4_SAI_FS_A},
        {PINMUX_GPIO_PIN_14, GPIOC, GPIOC14_AF4_SAI_BCK_A},
        {PINMUX_GPIO_PIN_15, GPIOC, GPIOC15_AF4_SAI_SD_A},
#endif
    };

    pin_cfg.pin = BIT(s_saiPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[0].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portc, s_saiPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[1].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portc, s_saiPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[2].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[2].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portc, s_saiPinMux[2].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[3].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[3].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portc, s_saiPinMux[3].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    return ret;



}
#endif

#if defined(CONFIG_SAIB)
static int saib_pinmux_config(void)
{
    int ret = 0;
#ifdef CHIP_GM6721
    struct device *porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }
#endif
#ifdef CHIP_F6721B
    struct device *portd = device_get_binding(CONFIG_PINMUX_GM_PORTD_NAME);
    if (portd == NULL)
    {
        return -1;
    }
#endif
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_saiPinMux[4] =
    {
#ifdef CHIP_GM6721
        {PINMUX_GPIO_PIN_0, GPIOI, GPIOI0_AF0_SCL},
        {PINMUX_GPIO_PIN_1, GPIOI, GPIOI1_AF0_SDA},
#endif
#ifdef CHIP_F6721B
        {PINMUX_GPIO_PIN_0, GPIOD, GPIOD0_AF4_SAI_SD_B},
        {PINMUX_GPIO_PIN_1, GPIOD, GPIOD1_AF4_SAI_MCLK_B},
        {PINMUX_GPIO_PIN_2, GPIOD, GPIOD2_AF4_SAI_BCK_B},
        {PINMUX_GPIO_PIN_3, GPIOD, GPIOD3_AF4_SAI_FS_B},
#endif
    };

    pin_cfg.pin = BIT(s_saiPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[0].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portd, s_saiPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[1].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portd, s_saiPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[2].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[2].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portd, s_saiPinMux[2].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[3].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_saiPinMux[3].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portd, s_saiPinMux[3].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    return ret;
}
#endif

void sai_test(void)
{
#if defined(CONFIG_SAIA)
    saia_pinmux_config();
#endif
#if defined(CONFIG_SAIB)
    saib_pinmux_config();
#endif
    zassert_true(test_sai_int() == TC_PASS, NULL);
    //zassert_true(test_sai_dma() == TC_PASS, NULL);
}
