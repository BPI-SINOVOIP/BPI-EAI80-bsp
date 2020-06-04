/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          printf.c
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Debug printf library.
 *
 * @note
 *    2015/09/08, wen.liu, V1.0.0
 *        Initial version.
 *    2016/11/08, wen.liu, V1.1.0
 *        Update for uasge with microlib.
 */
#include "gm_common.h"
#include <driver/gm_hal_uart.h>
#include <driver/gpio.h>
#include <stdio.h>

#ifdef DEBUG_PRINTF

#define PRINTF_PORT UART0

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;


/** Configure pinmux for an available UART device */
static RET_CODE UartPinMuxConfig(void)
{
		Device_Pinmux_T s_uartPinMux[2] = {

		#if defined(CHIP_GM6611) || defined(CHIP_GM6721)
				{GPIO_PIN_13, GPIOC, GPIOC13_AF0_UART0TX},
				{GPIO_PIN_12, GPIOC, GPIOC12_AF0_UART0RX},
		#endif
		};

		RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = s_uartPinMux[0].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
		GPIO_InitStruct.alternate = s_uartPinMux[0].alternate;
    ret = GPIO_Init(s_uartPinMux[0].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** UART0 GPIO Configuration */
    GPIO_InitStruct.pin = s_uartPinMux[1].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_uartPinMux[1].alternate;
    ret = GPIO_Init(s_uartPinMux[1].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
}


#endif



/* microlib printf library would callback fputc() */
int fputc(int ch, FILE *f)
{
#ifdef DEBUG_PRINTF
    /* Use single \n for new empty line */
    if( '\n' == ch)
    {
        HAL_UART_PutChar(PRINTF_PORT, '\r');
    }
    HAL_UART_PutChar(PRINTF_PORT, ch);
#endif

    return 1;
}


/** Init uart port */
void DebugConsoleInit(uint32_t baudrate)
{
#ifdef DEBUG_PRINTF
    UART_Config_T config;
	UART_Handle_T UART;

    UartPinMuxConfig();

    config.baudRate = baudrate;
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.callback = NULL;

	HAL_UART_Init( &UART,UART0, &config);

    //UART_Init(UART0, &config, NULL);
#endif
}



