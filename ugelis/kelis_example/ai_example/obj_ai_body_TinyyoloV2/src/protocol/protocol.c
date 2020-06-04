#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc/printk.h>
#include <gm_hal_uart.h>

#include "protocol.h"
#include <obj_info.h>

static UART_Handle_T g_uartHandle;
static uint8_t              g_rxData[MAX_RX_BUF];
static uint8_t              g_txData[MAX_TX_BUF];
static uint8_t              g_txSize;
static Frame_T           g_frameData;

#include "ai_common.h"

extern obj_show_t    objout;
extern void set_personinfo(Frame_T *frame, obj_show_t *objdata);

static volatile uint32_t s_rcvSize = 0;  /**< Size of received data */
uint8_t  g_frame_length = 0;
uint8_t  g_frame_recv_finished = 0;/**<Means the frame data is receive finished*/
uint8_t  g_frame_be_read = 0;

#define SUPPORT_RX_THREAD
#define SUPPORT_TX_TIMER

#define STACKSIZE                4096

#ifdef SUPPORT_RX_THREAD
    K_THREAD_STACK_DEFINE(rxdata_parse_stack_area, STACKSIZE);
    static struct k_thread rxdata_parse_task_thread;
#endif

#ifdef SUPPORT_TX_TIMER
void frame_ctrl_work_handler(struct k_work *work)
{
    uint8_t  tx_data[9];
    uint32_t  check = 0;
    int i = 0;
	
    tx_data[0] = FRAME_HEADER1;
    tx_data[1] = FRAME_HEADER2;
    tx_data[2] = BROADCAST_ADDR;
    tx_data[3] = CAMERAL_BOARD_ADDR;
    tx_data[4] = 0x04;
    tx_data[5] = FRAME_CTRL_STATUS_CMD;
    tx_data[6] = 0x04;
    tx_data[7] = (objout.num != 0) ? 0x6 : 0x4;
    
    check += tx_data[4];
    check += tx_data[5];
    check += tx_data[6];
    check += tx_data[7];
    
    tx_data[8] = (check & 0xff);

    for (i = 0; i < 9; i++)
    {
        HAL_UART_PutChar((&g_uartHandle)->device, tx_data[i]);
    }

}

void frame_person_work_handler(struct k_work *work)
{
    /* do the processing that needs to be done periodically */
    int i = 0;
    int j = 0;
    int timeout = 0;

    set_txframedata();

    for (i = 0; i < g_txSize; i++)
    {
        //PRINTK("%x ", g_txData[i]);
        HAL_UART_PutChar((&g_uartHandle)->device, g_txData[i]);
    }
    //PRINTK("\n");

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
            //PRINTK("%x ", g_txData[i]);
            HAL_UART_PutChar((&g_uartHandle)->device, g_txData[i]);
        }

    }

FUNC_RETURN:

    s_rcvSize = 0;

    return;
}

K_WORK_DEFINE(frame_ctrl_work, frame_ctrl_work_handler);
K_WORK_DEFINE(frame_person_work, frame_person_work_handler);

void frame_ctrl_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&frame_ctrl_work);
}

void frame_person_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&frame_person_work);
}

K_TIMER_DEFINE(frame_ctrl_timer, frame_ctrl_timer_handler, NULL);
K_TIMER_DEFINE(frame_person_timer, frame_person_timer_handler, NULL);
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

    set_personinfo(&g_frameData, &objout);

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


void protocol_start()
{
    /***used for uart protocal**/
    uart_recv_data(UART3);
    //uart_recv_data(UART2);

#ifdef SUPPORT_RX_THREAD
    /**2. Create task to parse the received data  */
    k_tid_t rxtid = k_thread_create(&rxdata_parse_task_thread, rxdata_parse_stack_area,
                                    K_THREAD_STACK_SIZEOF(rxdata_parse_stack_area), (k_thread_entry_t)rxdata_parse_process_thread,  NULL, NULL, NULL,
                                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_start(rxtid);
#endif

#ifdef SUPPORT_TX_TIMER
    k_timer_start(&frame_ctrl_timer, K_MSEC(300), K_MSEC(300));

    /* start periodic timer that expires once every second */
    k_timer_start(&frame_person_timer, K_SECONDS(2), K_SECONDS(2));
#endif

}