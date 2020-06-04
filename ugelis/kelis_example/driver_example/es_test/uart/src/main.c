#include <ugelis.h>
#include <gm_hal_uart.h>

#define UART_USERBUFFER_SIZE 32   /**< Specify user buffer size */

UART_Handle_T g_uartHandle;
UART_Handle_T g_uartHandle2;
uint8_t g_rxData[UART_USERBUFFER_SIZE];  /**< User reception buffer */
uint8_t g_txData[UART_USERBUFFER_SIZE];  /**< User transmission buffer */
static volatile uint32_t s_tranSize = 0; /**< Size of transmitted user data */
static volatile uint32_t s_rcvSize = 0;  /**< Size of received user data */
volatile uint32_t g_event = UART_EVENT_NONE; /**< Global event uart driver  */


#define WITH_FIFO
#define BAUD_RATE 19200L
//#define BAUD_RATE 115200L

static void ResetUserBuffer(uint8_t *buf, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        buf[i] = 0xFF;
    }
}

static void SetUserBuffer(void)
{
    uint32_t i;
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        g_rxData[i] = 0xFF;
        g_txData[i] = (i & 0x1FF);
    }
}

void UartRcvFifoCallback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    //No fifo mode
    if (event == UART_EVENT_RX_CHAR)
    {
        g_rxData[s_rcvSize++] = param;

        //Fifo mode
    }
    else if (event == UART_EVENT_RX_WATERMARK || event == UART_EVENT_RXFIFO_TIMEOUT)
    {
        /* param is data count stored in rx fifo */
        uint32_t rcvCount = (uint32_t)param;
        while (rcvCount--)
        {
            g_rxData[s_rcvSize++] = HAL_UART_GetChar(hUART->device); //UART0-2
        }
    }
    else if (event == UART_EVENT_RX_OVER || UART_EVENT_RX_ERROR)
    {
        g_event = event;
    }
}


#define UART2_DE_PINMUX_MOD (*((volatile unsigned long *)(0x400002D8)))
#define UART2_DE_PINMUX_MOD_MSK (0x3 << 18)
#define UART2_DE_PINMUX_MOD_VAL (0x1 << 18)
#define UART2_DE_PINMUX_AF (*((volatile unsigned long *)(0x4000031C)))
#define UART2_DE_PINMUX_AF_MSK (0xF << 4)
#define UART2_DE_PINMUX_AF_VAL (0x6 << 4)

void ConfigUartPinMux_DE(UART_Device_T *pUART)
{
    if (pUART == UART2)
    {
        IO_BITMASK_SET(UART2_DE_PINMUX_MOD, UART2_DE_PINMUX_MOD_MSK, UART2_DE_PINMUX_MOD_VAL);
        IO_BITMASK_SET(UART2_DE_PINMUX_AF, UART2_DE_PINMUX_AF_MSK, UART2_DE_PINMUX_AF_VAL);
    }
}

#define UART0_CTS_PINMUX_MOD (*((volatile unsigned long *)(0x400002E0)))
#define UART0_CTS_PINMUX_MOD_MSK (0x3 << 8)
#define UART0_CTS_PINMUX_MOD_VAL (0x1 << 8)
#define UART0_CTS_PINMUX_AF (*((volatile unsigned long *)(0x40000328)))
#define UART0_CTS_PINMUX_AF_MSK (0xF << 16)
#define UART0_CTS_PINMUX_AF_VAL (0x6 << 16)

#define UART0_RTS_PINMUX_MOD (*((volatile unsigned long *)(0x400002E0)))
#define UART0_RTS_PINMUX_MOD_MSK (0x3 << 10)
#define UART0_RTS_PINMUX_MOD_VAL (0x1 << 10)
#define UART0_RTS_PINMUX_AF (*((volatile unsigned long *)(0x40000328)))
#define UART0_RTS_PINMUX_AF_MSK (0xF << 20)
#define UART0_RTS_PINMUX_AF_VAL (0x6 << 20)

#define UART1_CTS_PINMUX_MOD (*((volatile unsigned long *)(0x400002E4)))
#define UART1_CTS_PINMUX_MOD_MSK (0x3 << 12)
#define UART1_CTS_PINMUX_MOD_VAL (0x1 << 12)
#define UART1_CTS_PINMUX_AF (*((volatile unsigned long *)(0x40000330)))
#define UART1_CTS_PINMUX_AF_MSK (0xF << 24)
#define UART1_CTS_PINMUX_AF_VAL (0x6 << 24)

#define UART1_RTS_PINMUX_MOD (*((volatile unsigned long *)(0x400002E4)))
#define UART1_RTS_PINMUX_MOD_MSK (0x3 << 14)
#define UART1_RTS_PINMUX_MOD_VAL (0x1 << 14)
#define UART1_RTS_PINMUX_AF (*((volatile unsigned long *)(0x40000330)))
#define UART1_RTS_PINMUX_AF_MSK (0xF << 28)
#define UART1_RTS_PINMUX_AF_VAL (0x6 << 28)

void ConfigUartPinMux_RTSCTS(UART_Device_T *pUART)
{
    if (pUART == UART0)
    {
        IO_BITMASK_SET(UART0_RTS_PINMUX_MOD, UART0_RTS_PINMUX_MOD_MSK, UART0_RTS_PINMUX_MOD_VAL);
        IO_BITMASK_SET(UART0_RTS_PINMUX_AF, UART0_RTS_PINMUX_AF_MSK, UART0_RTS_PINMUX_AF_VAL);
        IO_BITMASK_SET(UART0_CTS_PINMUX_MOD, UART0_CTS_PINMUX_MOD_MSK, UART0_CTS_PINMUX_MOD_VAL);
        IO_BITMASK_SET(UART0_CTS_PINMUX_AF, UART0_CTS_PINMUX_AF_MSK, UART0_CTS_PINMUX_AF_VAL);
    }
    else if (pUART == UART1)
    {
        IO_BITMASK_SET(UART1_RTS_PINMUX_MOD, UART1_RTS_PINMUX_MOD_MSK, UART1_RTS_PINMUX_MOD_VAL);
        IO_BITMASK_SET(UART1_RTS_PINMUX_AF, UART1_RTS_PINMUX_AF_MSK, UART1_RTS_PINMUX_AF_VAL);
        IO_BITMASK_SET(UART1_CTS_PINMUX_MOD, UART1_CTS_PINMUX_MOD_MSK, UART1_CTS_PINMUX_MOD_VAL);
        IO_BITMASK_SET(UART1_CTS_PINMUX_AF, UART1_CTS_PINMUX_AF_MSK, UART1_CTS_PINMUX_AF_VAL);
    }
}

void TestCase_Tx(UART_Device_T *pUART)
{
    /* Initialize uart device for printk port */
    UART_Config_T config;
    config.baudRate = BAUD_RATE;  //8M: 19200 ; 48M: 115200
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 8000000;
    config.callback = NULL;
    HAL_UART_Init(&g_uartHandle, pUART, &config);

    /* Data transfer configuration(enable tx fifo mode) */
    UART_TransferConfig_T transConfig;
#ifdef WITH_FIFO
    transConfig.enableFifo = TRUE;
#else
    transConfig.enableFifo = FALSE;
#endif
    transConfig.enableRxDMA = FALSE;
    transConfig.enableTxDMA = FALSE;
    transConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    transConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    HAL_UART_TransferConfig(&g_uartHandle, &transConfig);

    //IO_BITMASK_SET(UART0->TH, UART_TH_TFTH, 15); //TX_TRIG_LEVEL test
    //UART_SetMode(hUart, UART_MODE_RX);
    //IO_BIT_SET(UART0->C2, UART_C2_TXDIS);
    //IO_BIT_SET(UART0->C2, UART_C2_TXINV); //work here

    int i = 0;
    while (i++ < 100000)
    {
        printk("TxFifoTestCase ~~~~ %d.\n", i);
        HAL_DelayMs(1000);
    }
}

//#define WITH_TX_DE_SW
void TestCase_Tx_DE(UART_Device_T *pUART)
{
    ConfigUartPinMux_DE(pUART);

    /* Initialize uart device for printk port */
    UART_Config_T config;
    config.baudRate = BAUD_RATE;  //8M: 19200 ; 48M: 115200
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 8000000;
    config.callback = NULL;
    HAL_UART_Init(&g_uartHandle, pUART, &config);

    /* Data transfer configuration(enable tx fifo mode) */
    UART_TransferConfig_T transConfig;
    transConfig.enableFifo = FALSE;
    transConfig.enableRxDMA = FALSE;
    transConfig.enableTxDMA = FALSE;
    transConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    transConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    HAL_UART_TransferConfig(&g_uartHandle, &transConfig);

    //with DE485 mode
    IO_BIT_SET(pUART->C1, UART_C1_DME);
    IO_BITS_SET(pUART->C1, UART_C1_DAT);
    IO_BITS_SET(pUART->C1, UART_C1_DDT);

#ifdef WITH_TX_DE_SW
    IO_BIT_SET(pUART->C1, UART_C1_DSS); //software control
    IO_BIT_SET(pUART->C1, UART_C1_DSV); //1: transmit
#endif

    int i = 0;
    while (i++ < 1000)
    {
        printk("TxNoFifoTestCrr56788ase ~~~~ %d.\n", i);
        HAL_DelayMs(1000);
    }
}

int is_mute_mode(UART_Device_T *pUART)
{
    uint32_t myval = IO_BIT_GET(pUART->LS, UART_LS_RS);
    if (myval == 0x01000000)
    {
        printk("TestCase_Uart1ToUart0_FIFO_IDLE is in [mute] mode!! \n");
        return TRUE;
    }
    else
    {
        printk("TestCase_Uart1ToUart0_FIFO_IDLE is in normal mode. \n");
        return FALSE;
    }
}

void TestCase_IDLE(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
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
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_INT);
#endif

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_INT);
#endif

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    //delay to exit mute mode
    //SysTick_DelayUS(10);
    HAL_DelayMs(1000);

    g_txData[0] = 0xAA;
    g_txData[10] = 0xCC;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_IDLE_DMA_Fifo(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
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
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */

    uart0TransConfig.enableFifo = TRUE;
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = TRUE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_FIFO_INT | UART_RX_INT);

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart1TransConfig.enableFifo = TRUE;
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_FIFO_INT);

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    //delay to exit mute mode
    //SysTick_DelayUS(10);
    HAL_DelayMs(1000);

    g_txData[0] = 0xAA;
    g_txData[10] = 0xCC;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_IDLE_DMA(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
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
    uart0Config.callback = NULL;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart0TransConfig.enableFifo = TRUE;
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = TRUE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    //important: UART_RX_DMA_INT must be enabled when using Rx DMA
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_FIFO_INT | UART_RX_INT | UART_RX_DMA_INT);

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart1TransConfig.enableFifo = TRUE;
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = TRUE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    //important: UART_TX_DMA_INT must be enabled when using Tx DMA
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_FIFO_INT | UART_TX_DMA_INT);

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    //delay to exit mute mode
    //SysTick_DelayUS(10);
    HAL_DelayMs(1000);

    g_txData[0] = 0xAA;
    g_txData[10] = 0xCC;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_AddressMark_4bit(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
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
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */

#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_INT);
#endif

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    //IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);  //MUTE_RQ or address mark
    IO_BIT_SET(pUART_RECV->C3, UART_C3_WS);
    IO_BIT_CLEAR(pUART_RECV->C3, UART_C3_ADDM);   //4bit
    IO_BITMASK_SET(pUART_RECV->C3, UART_C3_ADD3_0 | UART_C3_ADD7_4, 0x05000000);
    //is_mute_mode(pUART_RECV);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_INT);
#endif

    g_txData[5] = 0x80;  //enter mute, can not receive
    g_txData[20] = 0xF5;  //exit mute

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_AddressMark_7bit(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
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
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */

#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_INT);
#endif

    IO_BIT_SET(pUART_RECV->C3, UART_C3_MME);
    //IO_BIT_SET(pUART_RECV->C3, UART_C3_MR);  //MUTE_RQ or address mark
    IO_BIT_SET(pUART_RECV->C3, UART_C3_WS);
    IO_BIT_SET(pUART_RECV->C3, UART_C3_ADDM);   //7bit
    IO_BITMASK_SET(pUART_RECV->C3, UART_C3_ADD3_0 | UART_C3_ADD7_4, 0x2A000000);
    //is_mute_mode(pUART_REC);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.clockSrc = 8000000;
    uart1Config.callback = NULL;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_INT);
#endif

    g_txData[6] = 0xAC;
    g_txData[16] = 0xAA;

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_RXFromPCorSTM32(UART_Device_T *pUART)
{
    UART_Config_T config;
    UART_Handle_T *hUart = &g_uartHandle;
    UART_TransferConfig_T transConfig;
    UART_TransferConfig_T *pTransConfig = &transConfig;

    /* Uart device configuration */
    config.baudRate = BAUD_RATE;
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 8000000;
    config.callback = UartRcvFifoCallback; //use callback or HAL_UART_Receive
    HAL_UART_Init(hUart, pUART, &config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    pTransConfig->enableFifo = TRUE;
#else
    pTransConfig->enableFifo = FALSE;
#endif
    pTransConfig->txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    pTransConfig->rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    pTransConfig->enableRxDMA = FALSE;
    pTransConfig->enableTxDMA = FALSE;
    HAL_UART_TransferConfig(hUart, pTransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(hUart->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(hUart->device, UART_RX_INT);
#endif

    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
}

#define WITH_RX_DE_SW
void TestCase_RX_DE(UART_Device_T *pUART)
{
    ConfigUartPinMux_DE(pUART);

    UART_Config_T config;
    UART_Handle_T *hUart = &g_uartHandle;
    UART_TransferConfig_T transConfig;
    UART_TransferConfig_T *pTransConfig = &transConfig;

    /* Uart device configuration */
    config.baudRate = BAUD_RATE;
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 8000000;
    config.callback = UartRcvFifoCallback;
    HAL_UART_Init(hUart, pUART, &config);

    /* Data transfer configuration(enable tx fifo mode) */
    pTransConfig->enableFifo = FALSE;
    pTransConfig->txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    pTransConfig->rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    pTransConfig->enableRxDMA = FALSE;
    pTransConfig->enableTxDMA = FALSE;
    HAL_UART_TransferConfig(hUart, pTransConfig);

    IO_BIT_SET(pUART->C1, UART_C1_DME);
    IO_BITS_SET(pUART->C1, UART_C1_DAT);
    IO_BITS_SET(pUART->C1, UART_C1_DDT);

#ifdef WITH_RX_DE_SW
    IO_BIT_SET(pUART->C1, UART_C1_DSS);  //software control
    IO_BIT_CLEAR(pUART->C1, UART_C1_DSV);  //0: receive
#endif

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts(hUart->device, UART_RX_INT);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
}

void TestCase_Loopback(UART_Device_T *pUART)
{
    UART_Config_T config;
    UART_Handle_T *hUart = &g_uartHandle;
    UART_TransferConfig_T transConfig;
    UART_TransferConfig_T *pTransConfig = &transConfig;

    /* Uart device configuration */
    config.baudRate = BAUD_RATE;
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.callback = UartRcvFifoCallback;
    config.clockSrc = 8000000;
    HAL_UART_Init(hUart, pUART, &config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    pTransConfig->enableFifo = TRUE;
#else
    pTransConfig->enableFifo = FALSE;
#endif
    pTransConfig->txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    pTransConfig->rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    pTransConfig->enableRxDMA = FALSE;
    pTransConfig->enableTxDMA = FALSE;
    HAL_UART_TransferConfig(hUart, pTransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(hUart->device, UART_TX_FIFO_INT | UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(hUart->device, UART_TX_INT | UART_RX_INT);
#endif

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);

    /* Connect uar0 rx and tx by loop back feature */
    IO_BIT_SET(pUART->C3, UART_C3_LME);

    /* 1. Receive data in rx fifo thershhold and timeout interrupt handler,
       never call HAL_UART_Receive() */
    /* You don't need to wait till all data transmitted */
    HAL_UART_Transmit(hUart, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}


void TestCase_TxRx(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle;
    UART_TransferConfig_T uart1TransConfig;
    uint16_t copiedSize;

    /* Uart device configuration */
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.clockSrc = 8000000;
    uart1Config.callback = NULL;
    HAL_UART_Init(uart1Handle, pUART_RECV, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_INT);
#endif

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart1Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle2;
    UART_TransferConfig_T uart0TransConfig;

    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.clockSrc = 8000000;
    uart0Config.callback = NULL;
    HAL_UART_Init(uart0Handle, pUART_SND, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_INT);
#endif

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart0Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_RTSCTS(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
    ConfigUartPinMux_RTSCTS(pUART_SND);
    ConfigUartPinMux_RTSCTS(pUART_RECV);

    //1, ready to receive
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle;
    UART_TransferConfig_T uart1TransConfig;
    uint16_t copiedSize;

    /* Uart device configuration */
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.clockSrc = 8000000;
    uart1Config.callback = NULL;
    HAL_UART_Init(uart1Handle, pUART_RECV, &uart1Config);

    IO_BIT_SET(pUART_RECV->C1, UART_C1_RHCE);
    IO_BIT_SET(pUART_RECV->C1, UART_C1_CHCE);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_INT);
#endif

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart1Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle2;
    UART_TransferConfig_T uart0TransConfig;

    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.clockSrc = 8000000;
    uart0Config.callback = NULL;
    HAL_UART_Init(uart0Handle, pUART_SND, &uart0Config);

    IO_BIT_SET(pUART_SND->C1, UART_C1_RHCE);
    IO_BIT_SET(pUART_SND->C1, UART_C1_CHCE);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_INT);
#endif

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart0Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_MSB(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
    //1, ready to receive
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle;
    UART_TransferConfig_T uart1TransConfig;
    uint16_t copiedSize;

    /* Uart device configuration */
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_RECV, &uart1Config);

    IO_BIT_SET(pUART_RECV->C2, UART_C2_MS);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_INT);
#endif
    //HAL_UART_EnableInterrupts(uart1Handle, UART_RX_INT);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart1Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle2;
    UART_TransferConfig_T uart0TransConfig;

    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_EVEN;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.clockSrc = 8000000;
    uart0Config.callback = NULL;
    HAL_UART_Init(uart0Handle, pUART_SND, &uart0Config);

    IO_BIT_SET(pUART_SND->C2, UART_C2_MS);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_INT);
#endif

    //HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_INT);
    //IO_BIT_SET(UART0->C2, UART_IE_TIDL);

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart0Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_9bit_NoFifo(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
    //1, ready to receive
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle;
    UART_TransferConfig_T uart1TransConfig;
    uint16_t copiedSize;

    /* Uart device configuration */
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_NONE;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_RECV, &uart1Config);

    IO_BIT_SET(pUART_RECV->C2, UART_C2_MS);
    IO_BIT_SET(pUART_RECV->C2, UART_C2_R9BDE);
    IO_BIT_SET(pUART_RECV->C2, UART_C2_T9BDE);

    /* Data transfer configuration(enable tx fifo mode) */
    uart1TransConfig.enableFifo = FALSE;
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_INT);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart1Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle2;
    UART_TransferConfig_T uart0TransConfig;

    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_NONE;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_1;
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_SND, &uart0Config);

    IO_BIT_SET(pUART_SND->C2, UART_C2_MS);
    IO_BIT_SET(pUART_SND->C2, UART_C2_R9BDE);
    IO_BIT_SET(pUART_SND->C2, UART_C2_T9BDE);

    /* Data transfer configuration(enable tx fifo mode) */
    uart0TransConfig.enableFifo = FALSE;
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_INT);

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart0Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_TxRxDMA_Fifo(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
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
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_RECV, &uart0Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart0TransConfig.enableFifo = TRUE;
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_RX_FIFO_INT | UART_RX_INT | UART_RX_DMA_INT);
    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart0Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle2;
    UART_TransferConfig_T uart1TransConfig;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_SND, &uart1Config);

    /* Data transfer configuration(enable tx fifo mode) */
    uart1TransConfig.enableFifo = TRUE;
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = TRUE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_TX_FIFO_INT | UART_TX_DMA_INT);

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart1Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}

void TestCase_Single(UART_Device_T *pUART_SND, UART_Device_T *pUART_RECV)
{
    //no need anymore
    //ConfigUartPinMux_S(pUART_SND);
    //ConfigUartPinMux_S(pUART_RECV);

    //1, ready to receive
    UART_Config_T uart1Config;
    UART_Handle_T *uart1Handle = &g_uartHandle;
    UART_TransferConfig_T uart1TransConfig;
    uint16_t copiedSize;

    /* Uart device configuration */
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_ODD;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_2;
    uart1Config.callback = NULL;
    uart1Config.clockSrc = 8000000;
    HAL_UART_Init(uart1Handle, pUART_RECV, &uart1Config);

    IO_BIT_SET(pUART_RECV->C1, UART_C1_HDE);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart1TransConfig.enableFifo = TRUE;
#else
    uart1TransConfig.enableFifo = FALSE;
#endif
    uart1TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart1TransConfig.enableRxDMA = FALSE;
    uart1TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart1Handle, &uart1TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_FIFO_INT | UART_RX_INT);
#else
    HAL_UART_EnableInterrupts(uart1Handle->device, UART_RX_INT);
#endif

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    g_event = UART_EVENT_NONE;
    /* Reset receive buffer (padding with 0xff) */
    ResetUserBuffer(&g_rxData[0], UART_USERBUFFER_SIZE);
    HAL_UART_Receive(uart1Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    //2,ready to transmit
    UART_Config_T uart0Config;
    UART_Handle_T *uart0Handle = &g_uartHandle2;
    UART_TransferConfig_T uart0TransConfig;

    uart0Config.baudRate = BAUD_RATE;
    uart0Config.parity = UART_PARITY_ODD;
    uart0Config.wordLength = UART_DATABITS_8;
    uart0Config.stopBits = UART_STOPBITS_2;
    uart0Config.callback = NULL;
    uart0Config.clockSrc = 8000000;
    HAL_UART_Init(uart0Handle, pUART_SND, &uart0Config);

    IO_BIT_SET(pUART_SND->C1, UART_C1_HDE);

    /* Data transfer configuration(enable tx fifo mode) */
#ifdef WITH_FIFO
    uart0TransConfig.enableFifo = TRUE;
#else
    uart0TransConfig.enableFifo = FALSE;
#endif
    uart0TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uart0TransConfig.enableRxDMA = FALSE;
    uart0TransConfig.enableTxDMA = FALSE;
    HAL_UART_TransferConfig(uart0Handle, &uart0TransConfig);

    /* Enable TX/RX FIFO interrupt and general uart rx interrupt */
#ifdef WITH_FIFO
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_FIFO_INT);
#else
    HAL_UART_EnableInterrupts(uart0Handle->device, UART_TX_INT);
#endif

    g_event = UART_EVENT_NONE;
    HAL_UART_Transmit(uart0Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);
}


//#define TEST_TX
//#define TEST_RX
//#define TEST_TX_DE
//#define TEST_RX_DE
//#define TEST_LOOPBACK
#define TEST_TXRX
//#define TEST_TXRX_DMA
//#define TEST_9BIT
//#define TEST_SINGLE_DEPLUX
//#define TEST_MSB
//#define TEST_ADDRESS_MARK_7
//#define TEST_ADDRESS_MARK_4
//#define TEST_IDLE
//#define TEST_IDLE_DMA
//#define TEST_RTSCTS

#if 0
void UART0_Raw_Init()
{
#define SET_WORD(reg, val)   (*((volatile uint32_t *)(reg)) = (val))
#define SET_BIT(reg, bit)    (*((volatile uint32_t *)(reg)) |= (bit))
#define UART0_BASE           (0x40004000UL)
#define UART_BARD_DIV        0x04
#define UART_C3              0x10
#define UART_IE              0x1C

    /* Set UART0 pinmux */
    *((volatile uint32_t *)(0x400002E8)) = ((*((volatile uint32_t *)(0x400002E8)) & (~(0x3 << 28))) | (0x1 << 28));
    *((volatile uint32_t *)(0x4000033C)) = ((*((volatile uint32_t *)(0x4000033C)) & (~(0xF << 24))) | (0xD << 24));
    *((volatile uint32_t *)(0x400002E8)) = ((*((volatile uint32_t *)(0x400002E8)) & (~(0x3 << 30))) | (0x1 << 30));
    *((volatile uint32_t *)(0x4000033C)) = ((*((volatile uint32_t *)(0x4000033C)) & (~(0xF << 28))) | (0x6 << 28));

    /* clear reset&clock */
    SET_BIT(0x40000078, 1 << 0);
    SET_BIT(0x40000088, 1 << 0);

    /* UART0 initialize */
    SET_WORD(UART0_BASE + UART_IE, 0x00);   /*disable all interrupt*/
    SET_BIT(UART0_BASE + UART_BARD_DIV, 0x1A);//bardrate(115200 for 48MHz, 19200 for 8MHz)
    SET_WORD(UART0_BASE + UART_C3, 0x3B);//8 data bit, 1 stop bit, even parity 115200, because FPGA uart clock is 48MHz

    printk("GM6721 ES UART test start\n");
}
#endif

void uart_transfer_test(void)
{
    //UART0_Raw_Init();

    printk("GM6721 ES UART test start\n");

    SetUserBuffer();

#ifdef TEST_TX
    TestCase_Tx(UART0);   //if use UART1/2 printk, need to modify uart0 to be UART1/2
#endif

#ifdef TEST_RX
    TestCase_RXFromPCorSTM32(UART2);   //need to disconnet uart0'RX and uart1'TX
#endif

#ifdef TEST_LOOPBACK
    TestCase_Loopback(UART2);   //need to modify UARTx in UartRcvFifoCallback()
#endif

#ifdef TEST_TXRX
    TestCase_TxRx(UART0, UART1);
#endif

#ifdef TEST_TXRX_DMA   //dma must be fifo mode, and just can enable 1 DMA at one time!!! do not support p2p mode, just support m2p or p2m
    TestCase_TxRxDMA_Fifo(UART1, UART0);
#endif

#ifdef TEST_9BIT   //9bit must be nofifo mode
    TestCase_9bit_NoFifo(UART0, UART1);
#endif

#ifdef TEST_ADDRESS_MARK_7
    TestCase_AddressMark_7bit(UART1, UART0);
#endif

#ifdef TEST_ADDRESS_MARK_4
    TestCase_AddressMark_4bit(UART0, UART1);
#endif

#ifdef TEST_IDLE
    TestCase_IDLE(UART1, UART0);
#endif

#ifdef TEST_IDLE_DMA   //idle dma must be fifo mode
    TestCase_IDLE_DMA_Fifo(UART0, UART1);
#endif

#ifdef TEST_MSB
    TestCase_MSB(UART0, UART1);
#endif

#ifdef TEST_SINGLE_DEPLUX   //uart2's single deplux share pin with SWD, must recover SWD function at the end of this test if use UART2
    TestCase_Single(UART0, UART1);
#endif

#ifdef TEST_RX_DE
    TestCase_RX_DE(UART2);
#endif

#ifdef TEST_TX_DE
    TestCase_Tx_DE(UART2);
#endif

#ifdef TEST_RTSCTS
    TestCase_RTSCTS(UART1, UART0);
#endif
}


int main(void)
{
    uart_transfer_test();

    while (1)
    {
        HAL_DelayMs(20);
    }

    return 0;
}

