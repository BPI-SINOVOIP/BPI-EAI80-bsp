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
//#include <gm_hal_pdm.h>
//#include <gm_hal_tim.h>
//#include <gm_hal_spi.h>



/** Configure pinmux for an available UART device */
RET_CODE UartPinMuxConfig(UART_Device_T *pUART)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T s_uartPinMux[2];

#if defined(CHIP_F6721B)
    if (pUART == UART0)
    {
        s_uartPinMux[0].pin = GPIO_PIN_2;
        s_uartPinMux[0].device = GPIOI;
        s_uartPinMux[0].alternate = GPIOI2_AF4_UART0_TX;
        s_uartPinMux[1].pin = GPIO_PIN_3;
        s_uartPinMux[1].device = GPIOI;
        s_uartPinMux[1].alternate = GPIOI3_AF4_UART0_RX;
    }
    else if (pUART == UART1)
    {
        s_uartPinMux[0].pin = GPIO_PIN_13;
        s_uartPinMux[0].device = GPIOB;
        s_uartPinMux[0].alternate = GPIOB13_AF1_UART1_RX;
        s_uartPinMux[1].pin = GPIO_PIN_12;
        s_uartPinMux[1].device = GPIOB;
        s_uartPinMux[1].alternate = GPIOB12_AF1_UART1_TX;
    }
    else if (pUART == UART2)
    {
        s_uartPinMux[0].pin = GPIO_PIN_5;
        s_uartPinMux[0].device = GPIOF;
        s_uartPinMux[0].alternate = GPIOF5_AF3_UART2_TX;
        s_uartPinMux[1].pin = GPIO_PIN_6;
        s_uartPinMux[1].device = GPIOF;
        s_uartPinMux[1].alternate = GPIOF6_AF3_UART2_RX;
    }
    else if (pUART == UART3)
    {
        s_uartPinMux[0].pin = GPIO_PIN_5;
        s_uartPinMux[0].device = GPIOB;
        s_uartPinMux[0].alternate = GPIOB5_AF1_UART3_TX;
        s_uartPinMux[1].pin = GPIO_PIN_7;
        s_uartPinMux[1].device = GPIOB;
        s_uartPinMux[1].alternate = GPIOB7_AF1_UART3_RX;
    }
    else if (pUART == UART4)
    {
        s_uartPinMux[0].pin = GPIO_PIN_4;
        s_uartPinMux[0].device = GPIOC;
        s_uartPinMux[0].alternate = GPIOC4_AF1_UART4_TX;
        s_uartPinMux[1].pin = GPIO_PIN_5;
        s_uartPinMux[1].device = GPIOC;
        s_uartPinMux[1].alternate = GPIOC5_AF1_UART4_RX;
    }
    else if (pUART == UART5)
    {
        s_uartPinMux[0].pin = GPIO_PIN_0;
        s_uartPinMux[0].device = GPIOD;
        s_uartPinMux[0].alternate = GPIOD0_AF1_UART5_TX;
        s_uartPinMux[1].pin = GPIO_PIN_1;
        s_uartPinMux[1].device = GPIOD;
        s_uartPinMux[1].alternate = GPIOD1_AF1_UART5_RX;
    }
    else if (pUART == UART6)
    {
        s_uartPinMux[0].pin = GPIO_PIN_10;
        s_uartPinMux[0].device = GPIOC;
        s_uartPinMux[0].alternate = GPIOC10_AF1_UART6_TX;
        s_uartPinMux[1].pin = GPIO_PIN_11;
        s_uartPinMux[1].device = GPIOC;
        s_uartPinMux[1].alternate = GPIOC11_AF1_UART6_RX;
    }
    else if (pUART == UART7)
    {
        s_uartPinMux[0].pin = GPIO_PIN_8;
        s_uartPinMux[0].device = GPIOC;
        s_uartPinMux[0].alternate = GPIOC8_AF1_UART7_TX;
        s_uartPinMux[1].pin = GPIO_PIN_9;
        s_uartPinMux[1].device = GPIOC;
        s_uartPinMux[1].alternate = GPIOC9_AF1_UART7_RX;
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

