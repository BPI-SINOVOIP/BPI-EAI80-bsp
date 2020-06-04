/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        pinmux.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       BOARD PINMUX source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <stdio.h>
#include <gm_config.h>
#include <gm_common.h>
#include <pinmux.h>
#include <gm_hal_gpio.h>
#include <gm_hal_tim.h>
#include <gm_hal_spi.h>

/**
 * ADC channel index
 */
typedef enum
{
    ADC_CHANNEL_0                          = 0x00,  /**< ADC sample channel num index is 0 */
    ADC_CHANNEL_1,                                  /**< ADC sample channel num index is 1 */
    ADC_CHANNEL_2,                                  /**< ADC sample channel num index is 2 */
    ADC_CHANNEL_3,                                  /**< ADC sample channel num index is 3 */
    ADC_CHANNEL_4,                                  /**< ADC sample channel num index is 4 */
    ADC_CHANNEL_5,                                  /**< ADC sample channel num index is 5 */
    ADC_CHANNEL_6,                                  /**< ADC sample channel num index is 6 */
    ADC_CHANNEL_7,                                  /**< ADC sample channel num index is 7 */
    ADC_CHANNEL_8,                                  /**< ADC sample channel num index is 8 */
    ADC_CHANNEL_9,                                  /**< ADC sample channel num index is 9 */
    ADC_CHANNEL_10,                                 /**< ADC sample channel num index is 10 */
    ADC_CHANNEL_11,                                 /**< ADC sample channel num index is 11 */
    ADC_CHANNEL_12,                                 /**< ADC sample channel num index is 12 */
    ADC_CHANNEL_13,                                 /**< ADC sample channel num index is 13 */
    ADC_CHANNEL_14,                                 /**< ADC sample channel num index is 14 */
    ADC_CHANNEL_15,                                 /**< ADC sample channel num index is 15 */
} ADC_ChannelIndex_T;

/** Configure pinmux for an available UART device */
RET_CODE UartPinMuxConfig(UART_Device_T *pUART)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T s_uartPinMux[2];

#if defined(CHIP_F6721B)
    if (pUART == UART1)
    {
        s_uartPinMux[0].pin = GPIO_PIN_13;
        s_uartPinMux[0].device = GPIOB;
        s_uartPinMux[0].alternate = GPIOB13_AF1_UART1_RX;
        s_uartPinMux[1].pin = GPIO_PIN_12;
        s_uartPinMux[1].device = GPIOB;
        s_uartPinMux[1].alternate = GPIOB12_AF1_UART1_TX;
    }
#endif

#if defined(CHIP_GM6721)
    if (pUART == UART0)
    {
        s_uartPinMux[0].pin = GPIO_PIN_2;
        s_uartPinMux[0].device = GPIOK;
        s_uartPinMux[0].alternate = GPIOK2_AF13_UART1_RX;
        s_uartPinMux[1].pin = GPIO_PIN_14;
        s_uartPinMux[1].device = GPIOJ;
        s_uartPinMux[1].alternate = GPIOJ14_AF13_UART1_TX;
    }
    else if (pUART == UART1)
    {
        s_uartPinMux[0].pin = GPIO_PIN_12;
        s_uartPinMux[0].device = GPIOE;
        s_uartPinMux[0].alternate = GPIOE12_AF6_UART2_RX;
        s_uartPinMux[1].pin = GPIO_PIN_10;
        s_uartPinMux[1].device = GPIOE;
        s_uartPinMux[1].alternate = GPIOE10_AF6_UART2_TX;
    }
    else if (pUART == UART2)
    {
        s_uartPinMux[0].pin = GPIO_PIN_5;
        s_uartPinMux[0].device = GPIOE;
        s_uartPinMux[0].alternate = GPIOE5_AF13_UART3_RX;
        s_uartPinMux[1].pin = GPIO_PIN_7;
        s_uartPinMux[1].device = GPIOE;
        s_uartPinMux[1].alternate = GPIOE7_AF13_UART3_TX;
    }
    else if (pUART == UART3)
    {
        s_uartPinMux[0].pin = GPIO_PIN_1;
        s_uartPinMux[0].device = GPIOC;
        s_uartPinMux[0].alternate = GPIOC1_AF7_UART4_RX;
        s_uartPinMux[1].pin = GPIO_PIN_0;
        s_uartPinMux[1].device = GPIOC;
        s_uartPinMux[1].alternate = GPIOC0_AF7_UART4_TX;
    }
    else if (pUART == UART4)
    {
        //UART4 occupied by SDRMA_D[8]
#ifndef CONFIG_SDRAM_BOOT
        s_uartPinMux[0].pin = GPIO_PIN_5;
        s_uartPinMux[0].device = GPIOI;
        s_uartPinMux[0].alternate = GPIOI5_AF7_UART5_RX;
        s_uartPinMux[1].pin = GPIO_PIN_2;
        s_uartPinMux[1].device = GPIOI;
        s_uartPinMux[1].alternate = GPIOI2_AF7_UART5_TX;
#endif
    }
    else if (pUART == UART5)
    {
        s_uartPinMux[0].pin = GPIO_PIN_14;
        s_uartPinMux[0].device = GPIOG;
        s_uartPinMux[0].alternate = GPIOG14_AF7_UART6_RX;
        s_uartPinMux[1].pin = GPIO_PIN_13;
        s_uartPinMux[1].device = GPIOG;
        s_uartPinMux[1].alternate = GPIOG13_AF7_UART6_TX;
    }
    else if (pUART == UART6)
    {
        s_uartPinMux[0].pin = GPIO_PIN_10;
        s_uartPinMux[0].device = GPIOG;
        s_uartPinMux[0].alternate = GPIOG10_AF14_UART7_RX;
        s_uartPinMux[1].pin = GPIO_PIN_11;
        s_uartPinMux[1].device = GPIOG;
        s_uartPinMux[1].alternate = GPIOG11_AF14_UART7_TX;
    }
    else if (pUART == UART7)
    {
        //UART7 occupied by SDRMA_D[6]
#ifndef CONFIG_SDRAM_BOOT
        s_uartPinMux[0].pin = GPIO_PIN_3;
        s_uartPinMux[0].device = GPIOK;
        s_uartPinMux[0].alternate = GPIOK3_AF7_UART8_RX;
        s_uartPinMux[1].pin = GPIO_PIN_4;
        s_uartPinMux[1].device = GPIOK;
        s_uartPinMux[1].alternate = GPIOK4_AF7_UART8_TX;
#endif
    }
#endif

    GPIO_InitStruct.pin = s_uartPinMux[0].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_uartPinMux[0].alternate;

    ret = HAL_GPIO_Init(s_uartPinMux[0].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** UART3 GPIO Configuration */
    GPIO_InitStruct.pin = s_uartPinMux[1].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_uartPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_uartPinMux[1].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
}

RET_CODE DAC_ConfigPinmux(void)
{

#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorder to not affect analog input*/
    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_8; /*GPIO_PIN_9*/
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;

    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        printf("Err:dac_ConfigPinmux\n");
    }

    return ret;

#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}


RET_CODE acmp_configoutput_pinmux(void)
{

#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /* config pinmux as acmp output pin */

    pGPIO = GPIOC;
    GPIO_InitStruct.pin = GPIO_PIN_14;   /*XPC14 ,XPE7*/
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC14_AF10_ACMP_OUT;

    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        goto out;
    }

out:
    return ret;

#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}

/*
    ACMP3(GPIO12)  ACMP0(GPIOC13) ACMP1(GPIOC14) ACMP2(GPIO15)
*/
RET_CODE ACMP_ConfigPinmux(void)
{
#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorde to not affect analog input*/
    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_12 | GPIO_PIN_14; /* GPIO_PIN_13 | GPIO_PIN_14 |GPIO_PIN_15 |*/
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}

RET_CODE ADC_ConfigPinmux(void)
{
#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    int i = 0;

    Device_Pinmux_T s_pinmux[] =
    {
        {GPIO_PIN_0,  GPIOC, 0}, /* ADC_CHANNEL_0 pinmux config paramter */
        {GPIO_PIN_1,  GPIOC, 0}, /* ADC_CHANNEL_1 pinmux config paramter */
#ifdef CONFIG_SRAM_BOOT
        {GPIO_PIN_2,  GPIOC, 0}, /* ADC_CHANNEL_2 pinmux config paramter */
        {GPIO_PIN_7,  GPIOC, 0}, /* ADC_CHANNEL_3 pinmux config paramter */
        {GPIO_PIN_8,  GPIOC, 0}, /* ADC_CHANNEL_4 pinmux config paramter */
        {GPIO_PIN_9,  GPIOC, 0}, /* ADC_CHANNEL_5 pinmux config paramter */
        {GPIO_PIN_10,  GPIOC, 0}, /* ADC_CHANNEL_6 pinmux config paramter */
        {GPIO_PIN_11,  GPIOC, 0}, /* ADC_CHANNEL_7 pinmux config paramter */
        {GPIO_PIN_14,  GPIOC, 0}, /* ADC_CHANNEL_8 pinmux config paramter */
        {GPIO_PIN_15,  GPIOC, 0}, /* ADC_CHANNEL_9 pinmux config paramter */
        {GPIO_PIN_12,  GPIOB, 0}, /* ADC_CHANNEL_10 pinmux config paramter */
        {GPIO_PIN_13,  GPIOB, 0}, /* ADC_CHANNEL_11 pinmux config paramter */
        {GPIO_PIN_14,  GPIOB, 0}, /* ADC_CHANNEL_12 pinmux config paramter */
        {GPIO_PIN_15,  GPIOB, 0}, /* ADC_CHANNEL_13 pinmux config paramter */
        {GPIO_PIN_12,  GPIOC, 0}, /* ADC_CHANNEL_14 pinmux config paramter */
        {GPIO_PIN_13,  GPIOC, 0}, /* ADC_CHANNEL_15 pinmux config paramter */
#endif
    };

    Device_Pinmux_T *pADCPinmux = &s_pinmux[0];

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  pADCPinmux->pin;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = 0;
    for (i = 0; i < sizeof(s_pinmux) / sizeof(s_pinmux[0]); i++)
    {
        pADCPinmux = &s_pinmux[i];
        GPIO_InitStruct.pin =  pADCPinmux->pin;
        HAL_GPIO_Init(pADCPinmux->device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    return ret;

#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}

RET_CODE SPI_ConfigPinmux(SPI_Device_T *pSPIDev)
{

#ifdef CHIP_GM6721

    int  i = 0;
    int  index = 0;
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;

    Device_Pinmux_T s_spiPinMux[6][4] =
    {
        {
            {GPIO_PIN_8, GPIOC, GPIOC8_AF4_SPI1_SS},
            {GPIO_PIN_9, GPIOC, GPIOC9_AF4_SPI1_SCK},
            {GPIO_PIN_11, GPIOC, GPIOC11_AF4_SPI1_MOSI},
            {GPIO_PIN_10,  GPIOC, GPIOC10_AF4_SPI1_MISO},
        },
        {
            {GPIO_PIN_1, GPIOF, GPIOF1_AF4_SPI2_SS},
            {GPIO_PIN_2, GPIOF, GPIOF2_AF4_SPI2_SCK},
            {GPIO_PIN_4, GPIOF, GPIOF4_AF4_SPI2_MOSI},
            {GPIO_PIN_3,  GPIOF, GPIOF3_AF4_SPI2_MISO},
        },
        {
            {GPIO_PIN_15, GPIOH, GPIOH15_AF5_SPI3_SS},
            {GPIO_PIN_0, GPIOI, GPIOI0_AF5_SPI3_SCK},
            {GPIO_PIN_2, GPIOI, GPIOI2_AF5_SPI3_MOSI},
            {GPIO_PIN_1,  GPIOI, GPIOI1_AF5_SPI3_MISO},
        },
        {
            {GPIO_PIN_2, GPIOA, GPIOA2_AF4_SPI4_SS},
            {GPIO_PIN_0, GPIOA, GPIOA0_AF4_SPI4_SCK},
            {GPIO_PIN_4, GPIOA, GPIOA4_AF4_SPI4_MOSI},
            {GPIO_PIN_3,  GPIOA, GPIOA3_AF4_SPI4_MISO},
        },
        {
            {GPIO_PIN_5, GPIOB, GPIOB5_AF4_SPI5_SS},
            {GPIO_PIN_6, GPIOB, GPIOB6_AF4_SPI5_SCK},
            {GPIO_PIN_8, GPIOB, GPIOB8_AF4_SPI5_MOSI},
            {GPIO_PIN_7,  GPIOB, GPIOB7_AF4_SPI5_MISO},
        },
        {
            {GPIO_PIN_12, GPIOG, GPIOG12_AF4_SPI6_SS},
            {GPIO_PIN_3, GPIOJ, GPIOJ3_AF4_SPI6_SCK},
            {GPIO_PIN_4, GPIOJ, GPIOJ4_AF4_SPI6_MOSI},
            {GPIO_PIN_2,  GPIOJ, GPIOJ2_AF4_SPI6_MISO},
        },
    };

    if (pSPIDev == SPI0_DEV)
    {
        index = 0;
    }
    else if (pSPIDev == SPI1_DEV)
    {
        index = 1;
    }
    else if (pSPIDev == SPI2_DEV)
    {
        index = 2;
    }
    else if (pSPIDev == SPI3_DEV)
    {
        index = 3;
    }
    else if (pSPIDev == SPI4_DEV)
    {
        index = 4;
    }
    else if (pSPIDev == SPI5_DEV)
    {
        index = 5;
    }
    else
    {
        index = 0;
    }

    for (i = 0; i < 4; i++)
    {
        GPIO_InitStruct.pin = s_spiPinMux[index][i].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_spiPinMux[index][i].alternate;

        ret = HAL_GPIO_Init(s_spiPinMux[index][i].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    return ret;

#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}

RET_CODE SFlash_ConfigPinmux(void)
{
#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;
    Device_Pinmux_T s_sflashPinMux[6] =
    {
        {GPIO_PIN_8, GPIOF, GPIOF8_EXT_SFLASH_CSJ},
        {GPIO_PIN_9, GPIOF, GPIOF9_EXT_SFLASH_WJ},
        {GPIO_PIN_13, GPIOG, GPIOG13_EXT_SFLASH_HOLDJ},
        {GPIO_PIN_14, GPIOG, GPIOG14_EXT_SFLASH_SCK},
        {GPIO_PIN_15, GPIOG, GPIOG15_EXT_SFLASH_MOSI},
        {GPIO_PIN_0, GPIOH, GPIOH0_EXT_SFLASH_MISO},
    };


    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = s_sflashPinMux[0].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[0].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[0].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[1].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[1].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[1].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[2].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[2].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[2].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[3].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[3].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[3].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[4].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[4].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[4].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[5].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[5].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[5].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;

#else
    /*add f6721b seting here if you need*/
    RET_CODE ret = RET_OK;
    Device_Pinmux_T s_sflashPinMux[6] =
    {
        {GPIO_PIN_12, GPIOF, GPIOF12_AF4_EXT_SFLASH_MISO},
        {GPIO_PIN_13, GPIOF, GPIOF13_AF4_EXT_SFLASH_SCK},
        {GPIO_PIN_14, GPIOF, GPIOF14_AF4_EXT_SFLASH_HOLDJ},
        {GPIO_PIN_15, GPIOF, GPIOF15_AF4_EXT_SFLASH_WJ},
        {GPIO_PIN_0, GPIOG, GPIOG0_AF4_EXT_SFLASH_MOSI},
        {GPIO_PIN_1, GPIOG, GPIOG1_AF4_EXT_SFLASH_CSJ},
    };


    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = s_sflashPinMux[0].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[0].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[0].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[1].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[1].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[1].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[2].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[2].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[2].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[3].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[3].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[3].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[4].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[4].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[4].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_sflashPinMux[5].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_sflashPinMux[5].alternate;

    ret = HAL_GPIO_Init(s_sflashPinMux[5].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
#endif
}

RET_CODE LCD_ConfigPinmux(void)
{

#ifdef CHIP_GM6721
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_VOUTPinMux[28] =
    {
        {GPIO_PIN_12, GPIOA, GPIOA12_AF13_VOUT_HSYNC},
        {GPIO_PIN_13, GPIOA, GPIOA13_AF13_VOUT_VSYNC},
        {GPIO_PIN_14, GPIOA, GPIOA14_AF13_VOUT_R0},
        {GPIO_PIN_15, GPIOA, GPIOA15_AF13_VOUT_R1},
        {GPIO_PIN_0, GPIOB, GPIOA15_AF13_VOUT_R1},
        {GPIO_PIN_1, GPIOB, GPIOB1_AF13_VOUT_R3},
        {GPIO_PIN_2, GPIOB, GPIOB2_AF13_VOUT_R4},
        {GPIO_PIN_3, GPIOB, GPIOB3_AF13_VOUT_R5},
        {GPIO_PIN_4, GPIOB, GPIOB4_AF13_VOUT_R6},
        {GPIO_PIN_9, GPIOB, GPIOB9_AF13_VOUT_R7},
        {GPIO_PIN_14, GPIOE, GPIOE14_AF13_VOUT_G0},
        {GPIO_PIN_0, GPIOF, GPIOF0_AF13_VOUT_G1},
        {GPIO_PIN_5, GPIOF, GPIOF5_AF13_VOUT_G2},
        {GPIO_PIN_6, GPIOF, GPIOF6_AF13_VOUT_G3},
        {GPIO_PIN_7, GPIOF, GPIOF7_AF13_VOUT_G4},
        {GPIO_PIN_10, GPIOF, GPIOF10_AF13_VOUT_G5},
        {GPIO_PIN_11, GPIOF, GPIOF11_AF13_VOUT_G6},
        {GPIO_PIN_12, GPIOF, GPIOF12_AF13_VOUT_G7},
        {GPIO_PIN_6, GPIOG, GPIOG6_AF13_VOUT_OUT_CLK},
        {GPIO_PIN_7, GPIOG, GPIOG7_AF13_VOUT_DE},
        {GPIO_PIN_8, GPIOG, GPIOG8_AF13_VOUT_B0},
        {GPIO_PIN_9, GPIOG, GPIOG9_AF13_VOUT_B1},
        {GPIO_PIN_10, GPIOG, GPIOG10_AF13_VOUT_B2},
        {GPIO_PIN_11, GPIOG, GPIOG11_AF13_VOUT_B3},
        {GPIO_PIN_7, GPIOH, GPIOH7_AF13_VOUT_B4},
        {GPIO_PIN_8, GPIOH, GPIOH8_AF13_VOUT_B5},
        {GPIO_PIN_9, GPIOH, GPIOH9_AF13_VOUT_B6},
        {GPIO_PIN_10, GPIOH, GPIOH10_AF13_VOUT_B7},
    };


    pin_cfg.pin = s_VOUTPinMux[0].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[0].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[0].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[1].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[1].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[2].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[2].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[2].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[3].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[3].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[3].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[4].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[4].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[4].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[5].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[5].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[5].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[6].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[6].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[6].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[7].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[7].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[7].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[8].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[8].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[8].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[9].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[9].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[9].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[10].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[10].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[10].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[11].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[11].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[11].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[12].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[12].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[12].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[13].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[13].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[13].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[14].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[14].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[14].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[15].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[15].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[15].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[16].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[16].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[16].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[17].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[17].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[17].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[18].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[18].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[18].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[19].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[19].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[19].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[20].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[20].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[20].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[21].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[21].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[21].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[22].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[22].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[22].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[23].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[23].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[23].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[24].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[24].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[24].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[25].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[25].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[25].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[26].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[26].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[26].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_VOUTPinMux[27].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[27].alternate;
    ret = HAL_GPIO_Init(s_VOUTPinMux[27].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}

RET_CODE USB_ConfigPinmux(void)
{

#ifdef CHIP_GM6721
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorder to not affect analog input*/
    pGPIO = GPIOH;
    GPIO_InitStruct.pin  = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;

    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        printf("Err:usb_ConfigPinmux\n");
    }

    return ret;

#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}


RET_CODE CAN0_ConfigPinmux(void)
{

#ifdef CHIP_GM6721

    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_CANPinMux[2] =
    {
        {GPIO_PIN_3, GPIOI, GPIOI3_AF7_CAN1_RX},
        {GPIO_PIN_4, GPIOI, GPIOI4_AF7_CAN1_TX},
    };

    pin_cfg.pin = s_CANPinMux[0].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[0].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[0].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_CANPinMux[1].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[1].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

#else
    /*add f6721b seting here if you need*/
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_CANPinMux[3] =
    {
        {GPIO_PIN_14, GPIOH, GPIOH14_AF1_CAN0_RX},
        {GPIO_PIN_15, GPIOH, GPIOH15_AF1_CAN0_TX},
        {GPIO_PIN_0, GPIOI, GPIOI0_AF1_CAN0_DBG},
    };

    pin_cfg.pin = s_CANPinMux[0].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[0].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[0].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_CANPinMux[1].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[1].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_CANPinMux[2].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[2].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[2].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
#endif
}



RET_CODE MMC_ConfigPinmux(void)
{

#ifdef CHIP_GM6721

    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_mmcPinMux[6] =
    {
        {GPIO_PIN_0, GPIOG, GPIOG0_AF10_SDMMC_CMD},
        {GPIO_PIN_1, GPIOG, GPIOG1_AF10_SDMMC_D2},
        {GPIO_PIN_2, GPIOG, GPIOG2_AF10_SDMMC_D3},

        {GPIO_PIN_13, GPIOF, GPIOF13_AF10_SDMMC_CLK},
        {GPIO_PIN_14, GPIOF, GPIOF14_AF10_SDMMC_D0},
        {GPIO_PIN_15, GPIOF, GPIOF15_AF10_SDMMC_D1},
    };


    pin_cfg.pin = s_mmcPinMux[0].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[0].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[0].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[1].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[1].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[2].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[2].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[2].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[3].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[3].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[3].device, &pin_cfg);;
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_mmcPinMux[4].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[4].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[4].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[5].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[5].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[5].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

#else
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_mmcPinMux[6] =
    {
        {GPIO_PIN_2, GPIOG, GPIOG2_AF6_SDMMC_D2},
        {GPIO_PIN_3, GPIOG, GPIOG3_AF6_SDMMC_D3},
        {GPIO_PIN_4, GPIOG, GPIOG4_AF6_SDMMC_CMD},
        {GPIO_PIN_5, GPIOG, GPIOG5_AF6_SDMMC_D1},
        {GPIO_PIN_6, GPIOG, GPIOG6_AF6_SDMMC_D0},
        {GPIO_PIN_7, GPIOG, GPIOG7_AF6_SDMMC_CLK},
    };


    pin_cfg.pin = s_mmcPinMux[0].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[0].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[0].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[1].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[1].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[2].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[2].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[2].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[3].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[3].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[3].device, &pin_cfg);;
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_mmcPinMux[4].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[4].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[4].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = s_mmcPinMux[5].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[5].alternate;
    ret = HAL_GPIO_Init(s_mmcPinMux[5].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

#endif


}


RET_CODE TIM_ConfigPinmux(TIM_Device_T *TIMx)
{

#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;


    if (TIMx == TIM1)
    {
        Device_Pinmux_T s_timPinMux[9] =
        {
            {PINMUX_GPIO_PIN_0,  GPIOE, GPIOE0_AF1_TIM1_CH1},
            {PINMUX_GPIO_PIN_2,  GPIOE, GPIOE2_AF1_TIM1_CH2},
            {PINMUX_GPIO_PIN_1,  GPIOE, GPIOE1_AF1_TIM1_CH2N},
            {PINMUX_GPIO_PIN_4,  GPIOE, GPIOE4_AF1_TIM1_CH3},
            {PINMUX_GPIO_PIN_3,  GPIOE, GPIOE3_AF1_TIM1_CH3N},
            {PINMUX_GPIO_PIN_5,  GPIOE, GPIOE5_AF1_TIM1_CH4},
            {PINMUX_GPIO_PIN_6,  GPIOE, GPIOE6_AF1_TIM1_BKIN},
            {PINMUX_GPIO_PIN_11, GPIOC, GPIOC11_AF1_TIM1_CH1N},
            {PINMUX_GPIO_PIN_14, GPIOD, GPIOD14_AF1_TIM1_ETR},
            //{PINMUX_GPIO_PIN_1,  GPIOH, GPIOH1_AF2_TIM1_CH1},
            //{PINMUX_GPIO_PIN_15, GPIOD, GPIOD15_AF1_TIM1_CH1N},
            //{PINMUX_GPIO_PIN_2,  GPIOF, GPIOF2_AF1_TIM1_CH1N},
            //{PINMUX_GPIO_PIN_2,  GPIOH, GPIOH2_AF2_TIM1_CH2},
            //{PINMUX_GPIO_PIN_14, GPIOC, GPIOC14_AF1_TIM1_CH2N},
            //{PINMUX_GPIO_PIN_3,  GPIOF, GPIOF3_AF1_TIM1_CH2N},
            //{PINMUX_GPIO_PIN_3,  GPIOH, GPIOH3_AF2_TIM1_CH3},
            //{PINMUX_GPIO_PIN_15, GPIOC, GPIOC15_AF1_TIM1_CH3N},
            //{PINMUX_GPIO_PIN_4,  GPIOF, GPIOF4_AF1_TIM1_CH3N},
            //{PINMUX_GPIO_PIN_4,  GPIOH, GPIOH4_AF2_TIM1_CH4},
            //{PINMUX_GPIO_PIN_5,  GPIOH, GPIOH5_AF2_TIM1_ETR},
            //{PINMUX_GPIO_PIN_10, GPIOC, GPIOC10_AF1_TIM1_BKIN},
            //{PINMUX_GPIO_PIN_1,  GPIOF, GPIOF1_AF1_TIM1_BKIN},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[2].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[3].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[4].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[4].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[4].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[5].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[5].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[5].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[6].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[6].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[6].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[7].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[7].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[7].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[8].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[8].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[8].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM2)
    {
        Device_Pinmux_T s_timPinMux[5] =
        {
            {PINMUX_GPIO_PIN_0,  GPIOC, GPIOC0_AF1_TIM2_CH1},
            {PINMUX_GPIO_PIN_1,  GPIOC, GPIOC1_AF1_TIM2_CH2},
            {PINMUX_GPIO_PIN_2,  GPIOC, GPIOC2_AF1_TIM2_CH3},
            {PINMUX_GPIO_PIN_7,  GPIOC, GPIOC7_AF1_TIM2_CH4},
            {PINMUX_GPIO_PIN_9,   GPIOC, GPIOC9_AF2_TIM2_ETR},
            //{PINMUX_GPIO_PIN_9,   GPIOC, GPIOC9_AF1_TIM2_CH1},
            //{PINMUX_GPIO_PIN_15,  GPIOH, GPIOH15_AF2_TIM2_CH1},
            //{PINMUX_GPIO_PIN_11,  GPIOJ, GPIOJ11_AF2_TIM2_CH2},
            //{PINMUX_GPIO_PIN_7,   GPIOE, GPIOE7_AF1_TIM2_CH3},
            //{PINMUX_GPIO_PIN_4,   GPIOH, GPIOH4_AF2_TIM1_CH4},
            //{PINMUX_GPIO_PIN_0,  GPIOC, GPIOC0_AF0_TIM2_ETR},
            //{PINMUX_GPIO_PIN_15,  GPIOH, GPIOH15_AF3_TIM2_ETR},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[2].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[3].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[4].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[4].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[4].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM3)
    {
        Device_Pinmux_T s_timPinMux[5] =
        {
            {PINMUX_GPIO_PIN_10,  GPIOC, GPIOC10_AF2_TIM3_CH1},
            {PINMUX_GPIO_PIN_11,  GPIOC, GPIOC11_AF2_TIM3_CH2},
            {PINMUX_GPIO_PIN_14,  GPIOC, GPIOC14_AF2_TIM3_CH3},
            {PINMUX_GPIO_PIN_15,  GPIOC, GPIOC15_AF2_TIM3_CH4},
            {PINMUX_GPIO_PIN_5,   GPIOI, GPIOI5_AF2_TIM3_ETR},
            //{PINMUX_GPIO_PIN_13,  GPIOG, GPIOG13_AF2_TIM3_CH1},
            //{PINMUX_GPIO_PIN_12,  GPIOJ, GPIOJ12_AF2_TIM3_CH1},
            //{PINMUX_GPIO_PIN_14,  GPIOG, GPIOG14_AF2_TIM3_CH2},
            //{PINMUX_GPIO_PIN_13,  GPIOJ, GPIOJ13_AF2_TIM3_CH2},
            //{PINMUX_GPIO_PIN_15,  GPIOG, GPIOG15_AF2_TIM3_CH3},
            //{PINMUX_GPIO_PIN_0,   GPIOH, GPIOH0_AF2_TIM3_CH4},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[2].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[3].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[4].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[4].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[4].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM4)
    {
        Device_Pinmux_T s_timPinMux[5] =
        {
            {PINMUX_GPIO_PIN_9,   GPIOF, GPIOF9_AF2_TIM4_CH1},
            {PINMUX_GPIO_PIN_10,  GPIOF, GPIOF10_AF2_TIM4_CH2},
            {PINMUX_GPIO_PIN_11,  GPIOF, GPIOF11_AF2_TIM4_CH3},
            {PINMUX_GPIO_PIN_12,  GPIOF, GPIOF12_AF2_TIM4_CH4},
            {PINMUX_GPIO_PIN_3,   GPIOK, GPIOK3_AF2_TIM4_ETR},
            //{PINMUX_GPIO_PIN_14,  GPIOJ, GPIOJ14_AF2_TIM4_CH1},
            //{PINMUX_GPIO_PIN_15,  GPIOJ, GPIOJ15_AF2_TIM4_CH2},
            //{PINMUX_GPIO_PIN_1,   GPIOK, GPIOK1_AF2_TIM4_CH3},
            //{PINMUX_GPIO_PIN_2,   GPIOK, GPIOK2_AF2_TIM4_CH4},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[2].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[3].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[4].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[4].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[4].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM5)
    {
        Device_Pinmux_T s_timPinMux[5] =
        {
            {PINMUX_GPIO_PIN_0,  GPIOC, GPIOC0_AF2_TIM5_CH1},
            {PINMUX_GPIO_PIN_1,  GPIOC, GPIOC1_AF2_TIM5_CH2},
            {PINMUX_GPIO_PIN_2,  GPIOC, GPIOC2_AF2_TIM5_CH3},
            {PINMUX_GPIO_PIN_7,  GPIOC, GPIOC7_AF2_TIM5_CH4},
            //{PINMUX_GPIO_PIN_14,  GPIOE, GPIOE14_AF2_TIM5_CH1},
            //{PINMUX_GPIO_PIN_15,  GPIOE, GPIOE15_AF2_TIM5_CH2},
            //{PINMUX_GPIO_PIN_0,   GPIOF, GPIOF0_AF2_TIM5_CH3},
            //{PINMUX_GPIO_PIN_10,  GPIOH, GPIOH10_AF2_TIM5_CH4},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[2].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[3].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[4].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[4].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[4].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM8)
    {
        Device_Pinmux_T s_timPinMux[9] =
        {
            {PINMUX_GPIO_PIN_9,   GPIOC, GPIOC9_AF3_TIM8_CH1N},
            {PINMUX_GPIO_PIN_14,  GPIOC, GPIOC14_AF3_TIM8_CH2N},
            {PINMUX_GPIO_PIN_15,  GPIOC, GPIOC15_AF3_TIM8_CH3N},
            {PINMUX_GPIO_PIN_0,   GPIOC, GPIOC0_AF3_TIM8_ETR},
            {PINMUX_GPIO_PIN_10,  GPIOC, GPIOC10_AF3_TIM8_BKIN},
            {PINMUX_GPIO_PIN_13,  GPIOG, GPIOG13_AF3_TIM8_CH1},
            {PINMUX_GPIO_PIN_14,  GPIOG, GPIOG14_AF3_TIM8_CH2},
            {PINMUX_GPIO_PIN_15,  GPIOG, GPIOG15_AF3_TIM8_CH3},
            {PINMUX_GPIO_PIN_0,   GPIOH, GPIOH0_AF3_TIM8_CH4},
            //{PINMUX_GPIO_PIN_6,  GPIOK, GPIOK6_AF3_TIM8_CH1},
            //{PINMUX_GPIO_PIN_11, GPIOC, GPIOC11_AF3_TIM8_CH1N},
            //{PINMUX_GPIO_PIN_7, GPIOH, GPIOH7_AF3_TIM8_CH1N},
            //{PINMUX_GPIO_PIN_7,  GPIOK, GPIOK7_AF3_TIM8_CH2},
            //{PINMUX_GPIO_PIN_3,  GPIOF, GPIOF3_AF3_TIM8_CH2N},
            //{PINMUX_GPIO_PIN_8,  GPIOH, GPIOH8_AF3_TIM8_CH2N},
            //{PINMUX_GPIO_PIN_8,  GPIOK, GPIOK8_AF3_TIM8_CH3},
            //{PINMUX_GPIO_PIN_4,  GPIOF, GPIOF4_AF3_TIM8_CH3N},
            //{PINMUX_GPIO_PIN_9,  GPIOH, GPIOH9_AF3_TIM8_CH3N},
            //{PINMUX_GPIO_PIN_12,  GPIOH, GPIOH12_AF3_TIM8_CH4},
            //{PINMUX_GPIO_PIN_13, GPIOH, GPIOH13_AF3_TIM8_ETR},
            //{PINMUX_GPIO_PIN_5, GPIOK, GPIOK5_AF3_TIM8_BKIN},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[2].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[3].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[4].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[4].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[4].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[5].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[5].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[5].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[6].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[6].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[6].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[7].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[7].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[7].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[8].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[8].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[8].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM9)
    {
        Device_Pinmux_T s_timPinMux[2] =
        {
            {PINMUX_GPIO_PIN_3,  GPIOA, GPIOA3_AF3_TIM9_CH1},
            {PINMUX_GPIO_PIN_4,  GPIOA, GPIOA4_AF3_TIM9_CH2},
            //{PINMUX_GPIO_PIN_2,  GPIOC, GPIOC2_AF3_TIM9_CH1},
            //{PINMUX_GPIO_PIN_7,  GPIOC, GPIOC7_AF3_TIM9_CH2},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM10)
    {
        Device_Pinmux_T s_timPinMux[1] =
        {
            {PINMUX_GPIO_PIN_5,  GPIOB, GPIOB5_AF3_TIM10_CH1},
            //{PINMUX_GPIO_PIN_1,  GPIOK, GPIOK1_AF3_TIM10_CH1},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM11)
    {
        Device_Pinmux_T s_timPinMux[1] =
        {
            {PINMUX_GPIO_PIN_6,  GPIOB, GPIOB6_AF3_TIM11_CH1},
            //{PINMUX_GPIO_PIN_2,  GPIOK, GPIOK2_AF3_TIM11_CH1},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM12)
    {
        Device_Pinmux_T s_timPinMux[2] =
        {
            {PINMUX_GPIO_PIN_10,  GPIOE, GPIOE10_AF7_TIM12_CH1},
            {PINMUX_GPIO_PIN_13,  GPIOE, GPIOE13_AF7_TIM12_CH2},
            //{PINMUX_GPIO_PIN_3,  GPIOF, GPIOF3_AF7_TIM12_CH1},
            //{PINMUX_GPIO_PIN_4,  GPIOF, GPIOF4_AF7_TIM12_CH2},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = s_timPinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[1].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else if (TIMx == TIM13)
    {
        Device_Pinmux_T s_timPinMux[1] =
        {
            {PINMUX_GPIO_PIN_10,  GPIOC, GPIOC10_AF7_TIM13_CH1},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    else
    {
        Device_Pinmux_T s_timPinMux[1] =
        {
            {PINMUX_GPIO_PIN_8,  GPIOB, GPIOB8_AF7_TIM14_CH1},
            //{PINMUX_GPIO_PIN_11,  GPIOC, GPIOC11_AF7_TIM14_CH1},
        };

        GPIO_PinConfig_T GPIO_InitStruct;
        GPIO_InitStruct.pin = s_timPinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = s_timPinMux[0].alternate;
        ret = HAL_GPIO_Init(s_timPinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }

    return ret;
#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}
/** Configure pinmux for an available I2C device */
RET_CODE I2CPinMuxConfig(I2C_Device_T *pI2C)
{

#ifdef CHIP_GM6721

    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T s_i2cPinMux[2];
    GPIO_InputGlitchFilter_T GPIO_InputGlitchFilter;

    memset(&GPIO_InputGlitchFilter, 0, sizeof(GPIO_InputGlitchFilter_T));
    //GPIO_InputGlitchFilter.parent_clk_src_sel = DB_PARENT_SRC_CLK_SEL_FCLK;
    //GPIO_InputGlitchFilter.child_clk_sel = PAD_FLT_CLK3;
    //GPIO_InputGlitchFilter.prescaler =    FLT_CLK3_PSC_DIV_64K;
    GPIO_InputGlitchFilter.db_bypass_en = ENABLE;
    if (pI2C == I2C0)
    {
        s_i2cPinMux[0].pin = GPIO_PIN_0;
        s_i2cPinMux[0].device = GPIOI;
        s_i2cPinMux[0].alternate = GPIOI0_AF0_SCL;
        //HAL_GPIO_SetPinFilter(GPIOI0, &GPIO_InputGlitchFilter);
        s_i2cPinMux[1].pin = GPIO_PIN_1;
        s_i2cPinMux[1].device = GPIOI;
        s_i2cPinMux[1].alternate = GPIOI1_AF0_SDA;
        //HAL_GPIO_SetPinFilter(GPIOI1, &GPIO_InputGlitchFilter);
    }
    else if (pI2C == I2C1)
    {
        s_i2cPinMux[0].pin = GPIO_PIN_13;
        s_i2cPinMux[0].device = GPIOA;
        s_i2cPinMux[0].alternate = GPIOA13_AF0_SCL;
        s_i2cPinMux[1].pin = GPIO_PIN_12;
        s_i2cPinMux[1].device = GPIOA;
        s_i2cPinMux[1].alternate = GPIOA12_AF0_SDA;
    }
    else if (pI2C == I2C2)
    {
        s_i2cPinMux[0].pin = GPIO_PIN_10;
        s_i2cPinMux[0].device = GPIOE;
        s_i2cPinMux[0].alternate = GPIOE10_AFD_SCL;
        //HAL_GPIO_SetPinFilter(GPIOE10, &GPIO_InputGlitchFilter);
        s_i2cPinMux[1].pin = GPIO_PIN_12;
        s_i2cPinMux[1].device = GPIOE;
        s_i2cPinMux[1].alternate = GPIOE12_AF0_SDA;
        //HAL_GPIO_SetPinFilter(GPIOE12, &GPIO_InputGlitchFilter);
    }

    /** I2C SCL GPIO Configuration */
    GPIO_InitStruct.pin = s_i2cPinMux[0].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = s_i2cPinMux[0].alternate;

    ret = HAL_GPIO_Init(s_i2cPinMux[0].device, &GPIO_InitStruct);

    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** I2C SDA GPIO Configuration */
    GPIO_InitStruct.pin = s_i2cPinMux[1].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = s_i2cPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_i2cPinMux[1].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    if (pI2C == I2C2)
    {
        HAL_GPIO_SetPinFilter(GPIOE10, &GPIO_InputGlitchFilter);
        HAL_GPIO_SetPinFilter(GPIOE12, &GPIO_InputGlitchFilter);
    }
    if (pI2C == I2C1)
    {
        HAL_GPIO_SetPinFilter(GPIOA13, &GPIO_InputGlitchFilter);
        HAL_GPIO_SetPinFilter(GPIOA12, &GPIO_InputGlitchFilter);
    }
    if (pI2C == I2C0)
    {
        HAL_GPIO_SetPinFilter(GPIOE10, &GPIO_InputGlitchFilter);
        HAL_GPIO_SetPinFilter(GPIOE12, &GPIO_InputGlitchFilter);
    }
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;

#else
    /*add f6721b seting here if you need*/
    return RET_OK;
#endif
}
