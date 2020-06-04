/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>
#include <gm_hal_uart.h>

#include "gm_hal_test_uart.h"


//#define WITH_FIFO
#define BAUD_RATE 19200L
#define UART_USERBUFFER_SIZE 32           /**< Specify user buffer size */

uint8_t g_rxData[UART_USERBUFFER_SIZE];
uint8_t g_txData[UART_USERBUFFER_SIZE];
uint32_t s_txSize = UART_USERBUFFER_SIZE;
uint32_t s_rxSize = 0;
uint32_t g_event = UART_EVENT_NONE;       /**< Global event of uart driver  */


static void SetUserBuffer(void)
{
    uint32_t i;

    s_rxSize = 0;
    for (i = 0; i < s_txSize; i++)
    {
        g_rxData[i] = 0xFF;
        g_txData[i] = (i & 0x1FF);
    }
}

void UartReceiveCallback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    //No fifo mode
    if (event == UART_EVENT_RX_CHAR)
    {
        g_rxData[s_rxSize++] = param;

        //Fifo mode
    }
    else if (event == UART_EVENT_RX_WATERMARK || event == UART_EVENT_RXFIFO_TIMEOUT)
    {
        uint32_t rcvCount = (uint32_t)param;
        while (rcvCount--)
        {
            g_rxData[s_rxSize++] = HAL_UART_GetChar(hUART->device); //UART0-2
        }

        //over or error
    }
    else if (event == UART_EVENT_RX_OVER || event == UART_EVENT_RX_ERROR)
    {
        g_event = event;
    }
}

static int test_uart_tx()
{
    UART_Handle_T uartHandle;

    int i = 0;
    UART_Config_T config;
    UART_TransferConfig_T transConfig;

    config.baudRate = BAUD_RATE;  //8M: 19200 ; 48M: 115200
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 8000000;
    config.callback = NULL;
    HAL_UART_Init(&uartHandle, UART1, &config);

#ifdef WITH_FIFO
    transConfig.enableFifo = TRUE;
#else
    transConfig.enableFifo = FALSE;
#endif
    transConfig.enableRxDMA = FALSE;
    transConfig.enableTxDMA = FALSE;
    transConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    transConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    HAL_UART_TransferConfig(&uartHandle, &transConfig);

    while (i++ < 3)
    {
        TC_PRINT("ztest uart tx index %d.\n", i);
        HAL_DelayMs(100);
    }

    TC_PRINT("ztest test_uart_tx ok.\n");
    return TC_PASS;
}

static int test_uart_txrx_irq(void)
{
    UART_Handle_T uart0Handle;
    UART_Handle_T uart1Handle;

    SetUserBuffer();

    UART_Config_T uart1Config;
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.clockSrc = 8000000;
    uart1Config.callback = NULL;
    HAL_UART_Init(&uart1Handle, UART0, &uart1Config);

    UART_TransferConfig_T uart1TransConfig;
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(&uart1Handle, &uart1TransConfig);

#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle.device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle.device, UART_RX_INT);
#endif

    HAL_UART_Receive(&uart1Handle, g_rxData, s_txSize, &s_rxSize, TIMEOUT_NO_BLOCKING);


    UART_Config_T uart0Config;
    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.clockSrc = 8000000;
    uart0Config.callback = NULL;
    HAL_UART_Init(&uart0Handle, UART2, &uart0Config);

    UART_TransferConfig_T uart0TransConfig;
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(&uart0Handle, &uart0TransConfig);

#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle.device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle.device, UART_TX_INT);
#endif

    HAL_UART_Transmit(&uart0Handle, g_txData, s_txSize, TIMEOUT_NO_BLOCKING); //TIMEOUT_WAIT_FOREVER is usless

    HAL_DelayMs(100);

    //check the result
    int tag = 1;
    if (s_rxSize != s_txSize)
    {
        tag = 0;
    }
    else
    {
        for (int i = 0; i < s_txSize; i++)
        {
            if (g_rxData[i] != g_txData[i])
            {
                tag = 0;
                break;
            }
        }
    }

    if (tag)
    {
        TC_PRINT("ztest test_uart_txrx_irq successful.\n");
        return TC_PASS;
    }
    else
    {
        TC_PRINT("ztest test_uart_txrx_irq failed");
        return TC_FAIL;
    }
}

static int test_uart_txrx_poll(void)
{
    UART_Handle_T uart0Handle;
    UART_Handle_T uart1Handle;

    SetUserBuffer();

    UART_Config_T uart1Config;
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.clockSrc = 8000000;
    uart1Config.callback = UartReceiveCallback;
    HAL_UART_Init(&uart1Handle, UART0, &uart1Config);

    UART_TransferConfig_T uart1TransConfig;
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(&uart1Handle, &uart1TransConfig);

#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle.device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle.device, UART_RX_INT);
#endif


    UART_Config_T uart0Config;
    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.clockSrc = 8000000;
    uart0Config.callback = NULL;
    HAL_UART_Init(&uart0Handle, UART2, &uart0Config);

    UART_TransferConfig_T uart0TransConfig;
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(&uart0Handle, &uart0TransConfig);

    int sendCount = 0;
    while (sendCount < s_txSize)
    {
        HAL_UART_PutChar(uart0Handle.device, g_txData[sendCount]);
        sendCount++;
    }

    //check the result
    int tag = 1;
    while (1)
    {
        if (s_rxSize == s_txSize)   //transmit over
        {
            for (int i = 0; i < s_txSize; i++)
            {
                if (g_rxData[i] != g_txData[i])
                {
                    tag = 0;
                    break;
                }
            }
            break;
        }

        HAL_DelayMs(50);
    }

    if (tag)
    {
        TC_PRINT("ztest test_uart_txrx_poll successful.\n");
        return TC_PASS;
    }
    else
    {
        TC_PRINT("ztest test_uart_txrx_poll failed");
        return TC_FAIL;
    }
}

void gm_hal_test_uart(void)
{
    zassert_true(test_uart_tx() == TC_PASS, NULL);
    zassert_true(test_uart_txrx_irq() == TC_PASS, NULL);
    zassert_true(test_uart_txrx_poll() == TC_PASS, NULL);
}

