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

#include "gm_common.h"
#include <driver/gm_hal_uart.h>
#include <stdio.h>
#include <pinmux.h>

#ifdef DEBUG_PRINTF

    #if defined(CHIP_GM6721)
        #define PRINTF_PORT UART2
    #elif defined(CHIP_F6721B)
        #define PRINTF_PORT UART2
    #endif

#endif

/* microlib printf library would callback fputc() */
int fputc(int ch, FILE *f)
{
#ifdef DEBUG_PRINTF
    /* Use single \n for new empty line */
    if ('\n' == ch)
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

    UartPinMuxConfig(PRINTF_PORT);

    config.baudRate = baudrate;
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 8000000;
    config.callback = NULL;

    HAL_UART_Init(&UART, PRINTF_PORT, &config);
#endif
}
