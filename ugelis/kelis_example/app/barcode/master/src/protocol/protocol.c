#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc/printk.h>
#include <gm_hal_uart.h>

#include "protocol.h"

#define MAX_CMD_INFO_SIZE     128
#define FRAME_HEADER1         0x5E
#define FRAME_HEADER2         0x5E
/** commands */
#define CMD_HEARTBEAT         0x03
#define CMD_BARCODE           0x05     

#define TIMEOUT_FOREVER       0xFFFFFFFF

#define ACK                   ((uint8_t)0x06)  /** acknowledge: ok */
#define NAK                   ((uint8_t)0x15)  /** acknowledge: fail */

typedef struct
{
    uint8_t         header1;                    /**< Header 1 */
    uint8_t         header2;                    /**< Header 2 */
    uint8_t         squence;                    /**< Squence */
    uint8_t         len;                        /**< Length */
    uint8_t         cmd;                        /**< Command */
    uint8_t         info[MAX_CMD_INFO_SIZE];    /**< Information. The feedback info will be 1B only. */
    uint8_t         crc;                        /**< Verfication */
} protocol_frame_t;
#define RECV_CMD_SIZE         7 /* The feedback protocol_frame->info will be 1B status only. */

static UART_Handle_T g_uart_handle;
static uint8_t s_protocol_busy = 0; 
static uint8_t s_frame_sequence = 0; 


/******************************************************************************
* Ring Buffer
******************************************************************************/
typedef struct
{
    uint8_t* buff;                /**< Ring buffer to store received data */
    uint32_t size;                /**< Ring buffer total size */
    uint32_t unread_data_len;     /**< Unread data length of ring buffer */
    uint32_t rd_pos;              /**< Read position of ring buffer */
    uint32_t wr_pos;              /**< Write position of ring buffer */
}ringbuffer_t;

#define RING_BUFF_SIZE   (64)          /**<  Ring buffer size */
static uint8_t s_ringbuff_data[RING_BUFF_SIZE];  /**<  Ring buffer */
static ringbuffer_t s_ringbuff;
static uint8_t s_ringbuff_inited = 0;
static uint8_t s_ringbuff_busy = 0;

RET_CODE ringbuffer_init()
{
    if(!s_ringbuff_inited)
    {
        memset(&s_ringbuff, 0, sizeof(s_ringbuff));
        s_ringbuff.buff = s_ringbuff_data;
        s_ringbuff.size = RING_BUFF_SIZE;
        s_ringbuff_inited = 1;
    }

    return RET_OK;
}

/* It'd better call ringbuffer_reset() before each CMD_xxxx() call, to ensure the last communication ack is clear */
RET_CODE ringbuffer_reset()
{
    if(s_ringbuff_busy)
    {
        return RET_BUSY;
    }

    s_ringbuff_busy = 1;
    
    memset(&s_ringbuff, 0, sizeof(s_ringbuff));
    s_ringbuff.buff = s_ringbuff_data;
    s_ringbuff.size = RING_BUFF_SIZE;

    s_ringbuff_busy = 0;
    return RET_OK;
}


/**< Insert data to ring buffer */
RET_CODE ringbuffer_set_data(uint8_t *data, uint32_t len)
{
    if(s_ringbuff_busy)
    {
        return RET_BUSY;
    }

    s_ringbuff_busy = 1;
    
    if(data != NULL && len > 0)
    {
        if(len <= (s_ringbuff.size - s_ringbuff.unread_data_len))
        {
            if(s_ringbuff.wr_pos >= s_ringbuff.rd_pos)
            {
                if(len < (s_ringbuff.size - s_ringbuff.wr_pos))
                {
                    memcpy(s_ringbuff.buff + s_ringbuff.wr_pos, data, len);
                    s_ringbuff.wr_pos = s_ringbuff.wr_pos+len;
                }
                else
                {
                    memcpy(s_ringbuff.buff + s_ringbuff.wr_pos, data, s_ringbuff.size - s_ringbuff.wr_pos);
                    memcpy(s_ringbuff.buff, data + (s_ringbuff.size - s_ringbuff.wr_pos), len - (s_ringbuff.size - s_ringbuff.wr_pos));
                    s_ringbuff.wr_pos = len - (s_ringbuff.size - s_ringbuff.wr_pos);
                }
            }
            else// (s_ringbuff.wr_pos < s_ringbuff.rd_pos)
            {
                memcpy(s_ringbuff.buff + s_ringbuff.wr_pos, data, len);
                s_ringbuff.wr_pos = s_ringbuff.wr_pos+len;
            }

            s_ringbuff.unread_data_len += len;
        }
        else
        {
            printf("Error: Ring buffer full, data lost!\n");
            s_ringbuff_busy = 0;
            return RET_ERROR;
        }
    }
    else
    {
        printf("Error: invalid param!\n");
        s_ringbuff_busy = 0;
        return RET_ERROR;
    }

    s_ringbuff_busy = 0;
    return RET_OK;
}

/*
 * Get data from ring buffer 
 * data: address to save data
 * len: data len need to get from ring buffer
 * ret: (1)RET_ONGOING-->no enough data in ring buffer (2)RET_OK-->successfully get 'len' bytes data from ring buffer
 */
RET_CODE ringbuffer_get_data(uint8_t* data, uint32_t len)
{   
    if(s_ringbuff_busy)
    {
        return RET_BUSY;
    }

    s_ringbuff_busy = 1;

    if(len > s_ringbuff.size)
    {
        printf("Error: input len can not be larger than ring buffer size!\n");
        s_ringbuff_busy = 0;
        return RET_INVPARAM;
    }

    if(len > s_ringbuff.unread_data_len)
    {
        /* no enough data in ring buffer, return let SPI go on receive data */
        s_ringbuff_busy = 0;
        return RET_ONGOING;
    }

    if(s_ringbuff.wr_pos >= s_ringbuff.rd_pos)
    {
        memcpy(data, s_ringbuff.buff+s_ringbuff.rd_pos, len);
        s_ringbuff.rd_pos = s_ringbuff.rd_pos+len;
    }
    else
    {
        if(len < (s_ringbuff.size-s_ringbuff.rd_pos))
        {
            memcpy(data, s_ringbuff.buff+s_ringbuff.rd_pos, len);
            s_ringbuff.rd_pos = s_ringbuff.rd_pos+len;
        }
        else
        {
            memcpy(data, s_ringbuff.buff+s_ringbuff.rd_pos, s_ringbuff.size-s_ringbuff.rd_pos);
            memcpy(data+(s_ringbuff.size-s_ringbuff.rd_pos), s_ringbuff.buff, len-(s_ringbuff.size-s_ringbuff.rd_pos));
            s_ringbuff.rd_pos = len-(s_ringbuff.size-s_ringbuff.rd_pos);
        }
    }

    s_ringbuff.unread_data_len -= len;

    s_ringbuff_busy = 0;
    return RET_OK;
}
/******************************************************************************
* Ring Buffer End
******************************************************************************/

static void uart_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    uint8_t data = 0;
    
    if (event == UART_EVENT_RX_CHAR)
    {
        data = (uint8_t)param;
        ringbuffer_set_data(&data, 1);
    }
    
    if (event == UART_EVENT_TX_EMPTY)
    {
    }
    
    if (event == UART_EVENT_RX_OVER)
    {
    }

    if (event == UART_EVENT_TX_OVER)
    {
    }
}

static void uart_init(UART_Device_T *pUART)
{
    UART_Config_T uartConfig;
    UART_Handle_T *hUart = &g_uart_handle;
    UART_TransferConfig_T transConfig;
    UART_TransferConfig_T *pTransConfig = &transConfig;
    //uint16_t copiedSize = 0;

    /* Uart device configuration */
    uartConfig.baudRate         = 19200;
    uartConfig.parity           = UART_PARITY_EVEN;
    uartConfig.wordLength       = UART_DATABITS_8;
    uartConfig.stopBits         = UART_STOPBITS_1;
    uartConfig.clockSrc         = 8000000;
    uartConfig.callback         = uart_callback; //use callback to receive data
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

    /* RX ring buffer init */
    ringbuffer_init();
}

static RET_CODE frame_send(protocol_frame_t *frame)
{
    RET_CODE ret = 0;
    UART_Handle_T *hUart = &g_uart_handle;
    uint8_t info_size = frame->len - 2;
    int i = 0;

    HAL_UART_PutChar(hUart->device, frame->header1);
    HAL_UART_PutChar(hUart->device, frame->header2);
    HAL_UART_PutChar(hUart->device, frame->squence);
    HAL_UART_PutChar(hUart->device, frame->len);
    HAL_UART_PutChar(hUart->device, frame->cmd);
    if(info_size > 0)
    {
        for(i=0; i<info_size; i++)
        {
            if(i > MAX_CMD_INFO_SIZE) 
            {
                ret = 1;
                break;
            }
            HAL_UART_PutChar(hUart->device, frame->info[i]);
        }
    }
    HAL_UART_PutChar(hUart->device, frame->crc);
    
    return ret;
}

static RET_CODE frame_recv(protocol_frame_t *frame, uint32_t timeout)
{
    RET_CODE ret = RET_ERROR;
    UART_Handle_T *hUart = &g_uart_handle;
    uint8_t buffer[RECV_CMD_SIZE];
    volatile int32_t time_old, time_new;
    uint8_t crc = 0;
    
    time_old = k_uptime_get_32();
    while (1)
    {
        ret = ringbuffer_get_data(buffer, RECV_CMD_SIZE);
        if(ret == RET_OK)
        {
            /* buffer check */
            ret = RET_ERROR;
            if((buffer[0] == FRAME_HEADER1) && (buffer[1] == FRAME_HEADER2)
                && (buffer[2] == frame->squence) && (buffer[3] == 3) && (buffer[4] == frame->cmd))
            {
                crc = buffer[3];
                crc += buffer[4];
                crc += buffer[5];
                if(crc == buffer[6])
                {
                    ret = buffer[5];//frame->info is feedback status
                }
            } 
            return ret;
        }
        else if(ret == RET_ONGOING)
        {
            if(timeout != TIMEOUT_FOREVER)
            {
                time_new = k_uptime_get_32();
                if ((time_new-time_old) >= timeout)
                {
                    ret = RET_TIMEOUT;
                    break;
                }
            }
        }
        else
        {
            return ret;
        }
        k_sleep(1);
    }

    return ret;
}

static void frame_data_fill(protocol_frame_t *frame, uint8_t cmd, uint8_t *info, uint8_t info_size)
{    
    frame->header1 = FRAME_HEADER1;
    frame->header2 = FRAME_HEADER2;
    frame->squence = s_frame_sequence++;
    frame->len = 1 + 1 + info_size;
    frame->cmd = cmd;
    frame->crc = 0;
    frame->crc += frame->len;
    frame->crc += frame->cmd;
    for(int i=0; i<info_size; i++)
    {
        if(i > MAX_CMD_INFO_SIZE) break;
        frame->info[i] = info[i];
        frame->crc += frame->info[i];
    }
    return;
}


/******************************************************************************
* High level API 
******************************************************************************/

void protocol_init(void)
{
    /***used for uart protocal**/
    uart_init(UART7);
}

RET_CODE protocol_send_heartbeat(uint8_t wait_feedback)
{
    RET_CODE ret = RET_ERROR;
    protocol_frame_t frame;
    int retry = 3;

    if(s_protocol_busy)
    {
        return RET_BUSY;
    }
    s_protocol_busy = 1;
    ringbuffer_reset();
    frame_data_fill(&frame, CMD_HEARTBEAT, NULL, 0);

    while(retry--)
    {
        ret = frame_send(&frame);
        if(ret != RET_OK)
        {
            s_protocol_busy = 0;
            return ret;
        }
        
        if(wait_feedback)
        {
            ret = frame_recv(&frame, TIMEOUT_FOREVER);
        }
        
        if(ret == RET_OK)
        {
            break;
        }
    }

    s_protocol_busy = 0;
    return ret;
}

RET_CODE protocol_send_barcode(uint8_t *data, uint32_t data_len)
{
    RET_CODE ret = RET_ERROR;
    protocol_frame_t frame;
    int retry = 3;

    if(s_protocol_busy)
    {
        return RET_BUSY;
    }
    s_protocol_busy = 1;
    ringbuffer_reset();
    frame_data_fill(&frame, CMD_BARCODE, data, data_len);

    while(retry--)
    {
        ret = frame_send(&frame);
        if(ret != RET_OK)
        {
            s_protocol_busy = 0;
            return ret;
        }
        ret = frame_recv(&frame, 500);
        if(ret == RET_OK)
        {
            break;
        }
    }
    
    s_protocol_busy = 0;
    return ret;
}



/******************************************************************************
* Example
******************************************************************************/
#if 1
int protocol_test (void)
{
    RET_CODE ret = RET_ERROR;
    uint8_t barcode_data[] = "code128";
    
    protocol_init();

    while(1)
    {
#if 1
        /* test heartbeat */
        ret = protocol_send_heartbeat(0);
        if(ret != RET_OK)
        {
            printf("[%s(%d)] error\n", __FUNCTION__, __LINE__);
        }

        ret = protocol_send_heartbeat(1);
        if(ret != RET_OK)
        {
            printf("[%s(%d)] error\n", __FUNCTION__, __LINE__);
        }
#endif

#if 1      
        /* test barcode msg */
        ret = protocol_send_barcode(barcode_data, strlen(barcode_data));
        if(ret != RET_OK)
        {
            printf("[%s(%d)] error\n", __FUNCTION__, __LINE__);
        }
#endif     

        k_sleep(100);
    }
}
#endif


