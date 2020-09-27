/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_hal_dma.h>
#include <gm_hal_uart.h>
#include <../driver/uart/gm_ll_uart.h>
#include <gm_hal_pes.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>


#define TC_PASS 0
#define TC_FAIL 1
//#define TC_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__)//while(0){}//
//#define TC_PRINTF(...)
#define TC_PRINTF printk

#define BLOCK_SIZE          (16)//0x1000//
#define RX_BUFF_SIZE        (BLOCK_SIZE*4)//0x1000//(BLOCK_SIZE*4)

typedef enum
{
    TEST_M2M,
    TEST_M2P,
    TEST_P2M,
    TEST_PINGPONG_P2M,
    TEST_PINGPONG_M2P,
} Test_Case;

static DMA_Handle_T s_dmaHandle;

static volatile  uint32_t s_dmaComplete = 0;
static volatile  uint32_t s_dmaError = 0;
static char src_data[RX_BUFF_SIZE] = { 0 };
static char dst_data[RX_BUFF_SIZE] = { 0 };

/************************************************************
 * M2M *
 ************************************************************/

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

static int test_m2m()
{
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
        return ret;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_MEMORY_TO_MEMORY;
    dmaConfig.increment    = DMA_SRC_INC_DST_INC;
    dmaConfig.width        = DMA_WIDTH_8BIT;
    dmaConfig.priority     = DMA_PRIORITY_LOW;//DMA_PRIORITY_HIGH;
    dmaConfig.burst        = 0;
    dmaConfig.alter        = DMA_PRIMARY;//DMA_ALTER;
    dmaConfig.callback = dmaCallback;

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);

    HAL_DMA_Start(pDMAHandle, (uint32_t)src_data, (uint32_t)dst_data, BLOCK_SIZE);

    /* Wait till either DMA completes or error occures */
    while ((s_dmaComplete == 0) && (s_dmaError == 0))
    {};

    if (s_dmaComplete == 1)
    {
        if (memcmp(src_data, dst_data, BLOCK_SIZE) != 0)
        {
            TC_PRINTF("ERROR: dma data error\n");
            return RET_ERROR;
        }
    }

    HAL_DMA_Abort(pDMAHandle);

    TC_PRINTF("PASS\n");
    return TC_PASS;


}

/************************************************************
 * M2P P2M *
 ************************************************************/
#define BAUD_RATE 19200L

#define UART_USERBUFFER_SIZE 32   /**< Specify user buffer size */

static UART_Handle_T g_uartHandle;
static UART_Handle_T g_uartHandle2;
static uint8_t g_rxData[UART_USERBUFFER_SIZE];  /**< User reception buffer */
static uint8_t g_txData[UART_USERBUFFER_SIZE];  /**< User transmission buffer */
static volatile uint32_t s_tranSize = 0; /**< Size of transmitted user data */
static volatile uint32_t s_rcvSize = 0;  /**< Size of received user data */
static volatile uint32_t g_event = UART_EVENT_NONE; /**< Global event uart driver  */

static void SetUserBuffer(void)
{
    uint32_t i;
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        g_rxData[i] = 0xFF;
        g_txData[i] = (i & 0x1FF);
    }
}

static void UartCallback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{

    if (event == UART_EVENT_RX_CHAR) //No fifo mode
    {
    }
    else if (event == UART_EVENT_RX_WATERMARK || event == UART_EVENT_RXFIFO_TIMEOUT) //Fifo mode
    {
    }
    else if (event == UART_EVENT_RX_OVER)
    {
    }

    s_dmaComplete = 1;
}


static int test_p2m()
{
    UART_Device_T *pUART_RECV = UART2;

    SetUserBuffer();


    //1, ready to receive
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle;
    UART_TransferConfig_T uart0TransConfig;
    uint16_t copiedSize;

    /* Uart device configuration */
    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.callback = UartCallback;//NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */

    uart0TransConfig.enableFifo = TRUE;
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = TRUE;//FALSE;//
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts((UART_Device_T *)uart0Handle, UART_RX_FIFO_INT | UART_RX_INT);

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    /* Wait till either DMA completes or error occures */
    while ((s_dmaComplete == 0) && (s_dmaError == 0))
    {};

    /* manual check the rx data with the PC tool tx data */
    printf("test P2M OK\n");

    return RET_OK;
}

static int test_m2p()
{
    UART_Device_T *pUART_SND = UART2;

    SetUserBuffer();

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = UartCallback;//NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart1TransConfig.enableFifo = TRUE;
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = TRUE;//FALSE;//
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    HAL_UART_EnableInterrupts((UART_Device_T *)uart1Handle, UART_TX_FIFO_INT);

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    /* Wait till either DMA completes or error occures */
    while ((s_dmaComplete == 0) && (s_dmaError == 0))
    {};

    //printf("test M2P OK\n");
    /* manual check the data in the PC tool */

    return RET_OK;
}


/************************************************************
 * PingPong *
 ************************************************************/

extern DMA_Control_T *p_dmaCtrl;

int dma_done_cnt = 0;


static RET_CODE UART_Transmit_Pingpong(UART_Handle_T *hUART, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    RET_CODE ret = RET_OK;
    UART_Device_T *pUART = hUART->device;
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;


    /* UART regs */
    LL_UART_TX_DMA_TDL(pUART, MAX_UART_TX_FIFO_SIZE / 2);
    LL_UART_TX_DMA_ENABLE(pUART);
    LL_UART_RX_DMA_DISABLE(pUART);


    /* DAM regs */
    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART3_TX, &reqResult);
    if (ret != RET_OK)
    {
        //fail, need return fail status
        return ret;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
    dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
    dmaConfig.width = DMA_WIDTH_8BIT;
    dmaConfig.burst = 0;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PINGPONG;
    dmaConfig.callback = NULL;
    HAL_DMA_Init(DMA_DEV, &dmaConfig, &hUART->TxDMAHandle);
    HAL_DMA_MultiBuffer_Start(&hUART->TxDMAHandle, (uint32_t)(&pData[0]), (uint32_t) & (hUART->device->DR), (uint32_t)(&pData[size / 2]), size / 2);


    return RET_OK;
}

static int test_pingpong_m2p()
{
    UART_Device_T *pUART_SND = UART2;

    SetUserBuffer();

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;//UartCallback;//
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart1TransConfig.enableFifo = TRUE;
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = TRUE;//FALSE;//
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    HAL_UART_EnableInterrupts((UART_Device_T *)uart1Handle, UART_TX_INT | UART_TX_FIFO_INT);

    g_event = UART_EVENT_NONE;
    UART_Transmit_Pingpong(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    /* manual check the data in the PC tool */
    return RET_OK;
}


#define PINGPONG_BUFFER_LEN 64
static char pingpong_buffer[PINGPONG_BUFFER_LEN];
static int  pingpong_offset = 0;
static void DMA_PingPong_Rx_Callback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    //UART_Handle_T *hUART = NULL;

    __ASSERT(hDMA != NULL, "");

    if (event == DMA_EVENT_PINGPONG_PRIM_DONE)
    {
        memcpy(&pingpong_buffer[pingpong_offset], &g_rxData[0], UART_USERBUFFER_SIZE / 2);
        pingpong_offset += UART_USERBUFFER_SIZE / 2;
    }
    else
    {
        memcpy(&pingpong_buffer[pingpong_offset], &g_rxData[UART_USERBUFFER_SIZE / 2], UART_USERBUFFER_SIZE / 2);
        pingpong_offset += UART_USERBUFFER_SIZE / 2;
    }

    if (pingpong_offset >= PINGPONG_BUFFER_LEN)
    {
        HAL_DMA_Abort(hDMA);
        g_event = UART_EVENT_RX_OVER;
    }

}

static RET_CODE UART_Receive_Pingpong(UART_Handle_T *hUART, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    RET_CODE ret = RET_OK;
    UART_Device_T *pUART = hUART->device;
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;


    /* UART regs */
    LL_UART_RX_DMA_RDL(pUART, 0);
    LL_UART_RX_DMA_ENABLE(pUART);
    LL_UART_TX_DMA_DISABLE(pUART);


    /* DAM regs */
    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_UART3_RX, &reqResult);
    if (ret != RET_OK)
    {
        //fail, need return fail status
        return ret;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
    dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    dmaConfig.width = DMA_WIDTH_8BIT;
    dmaConfig.burst = 0;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PINGPONG;
    dmaConfig.callback = DMA_PingPong_Rx_Callback;
    HAL_DMA_Init(DMA_DEV, &dmaConfig, &hUART->RxDMAHandle);
    HAL_DMA_MultiBuffer_Start(&hUART->RxDMAHandle, (uint32_t) & (hUART->device->DR), (uint32_t)(&pData[0]), (uint32_t)(&pData[size / 2]), size / 2);

    return RET_OK;
}

static int test_pingpong_p2m()
{
    UART_Device_T *pUART_RECV = UART2;

    SetUserBuffer();

    //1, ready to receive
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle;
    UART_TransferConfig_T uart0TransConfig;

    /* Uart device configuration */
    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.callback = UartCallback;//NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */

    uart0TransConfig.enableFifo = TRUE;
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = TRUE;//FALSE;//
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts((UART_Device_T *)uart0Handle, UART_RX_FIFO_INT | UART_RX_INT);

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);

    /* Initialize variable about recevied size */
    g_event = UART_EVENT_NONE;
    UART_Receive_Pingpong(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    /* manual check the data in the PC tool */
    while (g_event == UART_EVENT_NONE)
    {
        //wait for uart input 64B data
    }

    /* attrive here means test pass */

    return RET_OK;
}

int main(void)
{
    int ret = TC_FAIL;//debug
    Test_Case T_Case = TEST_M2M;

    switch ((int)T_Case)
    {
        case TEST_M2M:
            ret = test_m2m();
            break;
        case TEST_M2P:
            ret = test_m2p();
            break;
        case TEST_P2M:
            ret = test_p2m();
            break;
        case TEST_PINGPONG_M2P:
            ret = test_pingpong_m2p();
            break;
        case TEST_PINGPONG_P2M:
            ret = test_pingpong_p2m();
            break;
        default:
            break;
    }
    return ret;
}



