#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc/printk.h>
#include <gm_hal_uart.h>

#include "protocol.h"

static UART_Handle_T        g_uartHandle;
static uint8_t              g_rxData[MAX_RX_BUF];
static uint8_t              g_txData[MAX_TX_BUF];
static uint8_t              g_txSize;
static Frame_T              g_frameData;


#if(FRAME_PROCESS==FACE_FRAME)
    extern faceinfo_t   faceout;
    #elif(FRAME_PROCESS==OBJECT_FRAME)
    #include "ai_common.h"

    extern objshow_t    objout;
    extern void set_personinfo(Frame_T *frame, objshow_t *objdata);
    #elif(FRAME_PROCESS==GESTURE_FRAME)
    #include "ai_common.h"

    extern objshow_t    objout;
    extern void set_gestureinfo(Frame_T *frame, objshow_t *objdata);
#endif

static volatile uint32_t s_rcvSize = 0;  /**< Size of received data */
uint8_t  g_frame_length = 0;
uint8_t  g_frame_recv_finished = 0;/**<Means the frame data is receive finished*/
uint8_t  g_frame_be_read = 0;

//#define SUPPORT_RX_THREAD
//#define SUPPORT_TX_THREAD
//#define SUPPORT_TX_TIMER

#define STACKSIZE                4096

#ifdef SUPPORT_RX_THREAD
    K_THREAD_STACK_DEFINE(rxdata_parse_stack_area, STACKSIZE);
    static struct k_thread rxdata_parse_task_thread;
#endif

#ifdef SUPPORT_TX_THREAD
K_THREAD_STACK_DEFINE(txdata_stack_area, STACKSIZE);
static struct k_thread txdata_task_thread;
#else
#ifdef SUPPORT_TX_TIMER
static struct k_timer       g_DataTimer;

static void data_frame_timer_handler(struct k_timer *timer)
{
    //Send control status frame data
    int i = 8;
    int j = 0;
    int timeout = 0;

    set_txframedata();

    for (i = 0; i < g_txSize; i++)
    {
        PRINTK("%x ", g_txData[i]);
        HAL_UART_PutChar((&g_uartHandle)->device, g_txData[i]);
    }
    PRINTK("\n");
#if 0
    //for(j = 0; j < 3; j++)//Colin>>Change 3 to 2 as below,because 300ms send data ,if 3*100ms will send the same data twice
    for (j = 0; j < 3; j++)
    {
        timeout = 7;
        k_sleep(30);  //the slave board will send the response data 30ms later after recieved data

        while (timeout--)
        {
            k_sleep(9);

            if (1 == g_frame_recv_finished)
            {
                g_frame_be_read = 1;

                goto FUNC_RETURN;
            }
        }

        for (i = 0; i < g_txSize; i++)
        {
            PRINTK("%x ", g_txData[i]);
            HAL_UART_PutChar((&g_uartHandle)->device, g_txData[i]);
        }

    }
#endif
FUNC_RETURN:

    return;
}
#endif
#endif

/*Clear the RX or TX buffer*/
static void clear_frame_buffer(uint8_t *buf, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        buf[i] = 0x00;
    }
}


static uint8_t checksum(uint8_t *data, uint8_t from, uint8_t to)
{
    uint8_t i;
    int sum;
    sum = 0;
    for (i = from; i < to; i++)
    {
        sum += data[i];
    }
    return (uint8_t)(sum & 0xFFu);
}

void set_txframedata()
{
    clear_frame_buffer(g_txData, MAX_TX_BUF);

#if(FRAME_PROCESS==FACE_FRAME)
    set_faceinfo(&g_frameData, &faceout);
#elif(FRAME_PROCESS==OBJECT_FRAME)
    set_personinfo(&g_frameData, &objout);
#elif(FRAME_PROCESS==GESTURE_FRAME)
    set_gestureinfo(&g_frameData, &objout);
#endif

    memcpy(g_txData, &g_frameData, g_frameData.frameLen + 4);
    g_txData[g_frameData.frameLen + 4] = g_frameData.check;
    g_txSize = g_frameData.frameLen + 5;
}


static bool b_check_rxdata_valid(uint8_t *rxdata, uint32_t size)
{
    int from                = 0;
    int to                  = 0;
    uint8_t frame_length    = rxdata[FRAME_LENGTH_INDEX];
    from                    = FRAME_LENGTH_INDEX;   //4: frame length position index
    to                      = FRAME_LENGTH_INDEX + frame_length;

    if ((FRAME_HEADER1 == rxdata[0]) && (FRAME_HEADER2 == rxdata[1])
            && (checksum(rxdata, from, to) == rxdata[to]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void uart_recv_data_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    uint32_t index;

    if (event == UART_EVENT_RX_CHAR)
    {
        index = s_rcvSize++;
        g_rxData[index] = param;
        if (FRAME_LENGTH_INDEX == index)
        {
            g_frame_length = param;
        }
        else if ((FRAME_LENGTH_INDEX + g_frame_length) == index)
        {
            g_frame_recv_finished = 1;
        }
    }
    else
    {
        //do nothing
    }
}

void uart_recv_data(UART_Device_T *pUART)
{
    UART_Config_T uartConfig;
    UART_Handle_T *hUart = &g_uartHandle;
    UART_TransferConfig_T transConfig;
    UART_TransferConfig_T *pTransConfig = &transConfig;
    //uint16_t copiedSize = 0;

    /* Uart device configuration */
    uartConfig.baudRate         = UART_BAUD_RATE;
    uartConfig.parity           = UART_PARITY_EVEN;
    uartConfig.wordLength       = UART_DATABITS_8;
    uartConfig.stopBits         = UART_STOPBITS_1;
    uartConfig.clockSrc         = 8000000;
    uartConfig.callback         = uart_recv_data_callback; //use callback to receive data
    HAL_UART_Init(hUart, pUART, &uartConfig);

    /* Data transfer configuration(Disable tx fifo mode) */
    pTransConfig->enableFifo  = FALSE;

    /*Disable tx and rx fifo ,not enable rxDMA and txDMA*/
    pTransConfig->txWatermark   = 0;
    pTransConfig->rxWatermark   = 0;
    pTransConfig->enableRxDMA   = FALSE;
    pTransConfig->enableTxDMA   = FALSE;

    HAL_UART_TransferConfig(hUart, pTransConfig);

    /* Enable  general uart rx interrupt */
    HAL_UART_EnableInterrupts(hUart->device, UART_RX_INT);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    //g_event = UART_EVENT_NONE;//Colin>>This is used?
    /* Reset receive buffer (padding with 0x00) */
    clear_frame_buffer(g_rxData, MAX_RX_BUF);
}

#ifdef SUPPORT_RX_THREAD
static void rxdata_parse_process_thread()
{
    while (1)
    {
        if (1 == g_frame_be_read)//g_frame_be_read is seted means g_frame_recv_finished is also seted
        {
            g_frame_be_read = 0;
            g_frame_recv_finished = 0;
            s_rcvSize = 0;
            if (TRUE == b_check_rxdata_valid(g_rxData, MAX_RX_BUF))
            {
                //parse the data;but parsing the data is no useful
                switch (g_rxData[FRAME_COMMAND_INDEX])
                {
#if(FRAME_PROCESS==FACE_FRAME)
                    case FRAME_FACE_START_CAP_CMD:
                        faceai_capstart();
                        break;

                    case FRAME_FACE_STOP_CAP_CMD:
                        uint8_t  info1 = g_rxData[FRAME_INFO1_INDEX];
                        uint8_t  id = (info1 & 0xFC) >> 2;
                        faceai_capstop(id);
                        break;

                    case FRAME_FACE_DELBASE_CMD:
                        int number = g_rxData[FRAME_INFO1_INDEX];
                        uint8_t  info2 = g_rxData[FRAME_INFO2_INDEX];
                        uint8_t  info3 = g_rxData[FRAME_INFO3_INDEX];
                        faceai_delbase(number, info2, info3);
                        break;
#endif
                    default:
                        break;
                }

            }
            clear_frame_buffer(g_rxData, MAX_RX_BUF);
        }

        k_sleep(5);
        k_yield();
    }
}
#endif

#ifdef SUPPORT_TX_THREAD
static void txdata_process_thread()
{
    int i = 8;

    while (1)
    {
        //Send control status frame data
        set_txframedata();

        for (int i = 0; i < g_txSize; i++)
        {
            PRINTK("%x ", g_txData[i]);
            HAL_UART_PutChar((&g_uartHandle)->device, g_txData[i]);
        }

        k_sleep(1000);
    }
}
#endif

void protocol_send()
{
    set_txframedata();

    for (int i = 0; i < g_txSize; i++)
    {
        PRINTK("%x ", g_txData[i]);
        HAL_UART_PutChar((&g_uartHandle)->device, g_txData[i]);
    }
}

void protocol_start()
{
    /***used for uart protocal**/
    uart_recv_data(UART2);
    //uart_recv_data(UART2);

#ifdef SUPPORT_RX_THREAD
    /**2. Create task to parse the received data  */
    k_tid_t rxtid = k_thread_create(&rxdata_parse_task_thread, rxdata_parse_stack_area,
                                    K_THREAD_STACK_SIZEOF(rxdata_parse_stack_area), (k_thread_entry_t)rxdata_parse_process_thread,  NULL, NULL, NULL,
                                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_start(rxtid);
#endif

#ifdef SUPPORT_TX_THREAD
    /**2. Create task to parse the received data  */
    k_tid_t txtid = k_thread_create(&txdata_task_thread, txdata_stack_area,
                                    K_THREAD_STACK_SIZEOF(txdata_stack_area), (k_thread_entry_t)txdata_process_thread,  NULL, NULL, NULL,
                                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_start(txtid);
#else
#ifdef SUPPORT_TX_TIMER
    k_timer_init(&g_DataTimer, data_frame_timer_handler, NULL);

    /**3. Start timer to send data*/
    //k_timer_start(&ctrl_frame_timer,300,300);
    k_timer_start(&g_DataTimer, 300, 600);
#endif
#endif

}