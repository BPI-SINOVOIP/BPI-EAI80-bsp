#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc/printk.h>
#include <gm_hal_uart.h>
#include <uart.h>
#include <console/uart_console.h>

#include "protocol.h"

#define UART_NAME "UART_2"
struct device *uart_dev = NULL;

static uint8_t                 g_rxData[MAX_RX_BUF];
static uint8_t                 g_txData[MAX_TX_BUF];
static master_frame_t    g_hostData;
static slave_frame_t      g_slaveData;

extern dnn_context_t dnn_context;
extern power_mgr_t  pm_info;

static volatile uint32_t s_rcvSize = 0;  /**< Size of received data */
uint8_t  g_frame_recv_finished = 0;/**<Means the frame data is receive finished*/

#define SUPPORT_RX_THREAD

#define STACKSIZE                4096

uint32_t size = 0;

#ifdef SUPPORT_RX_THREAD
    K_THREAD_STACK_DEFINE(rxdata_parse_stack_area, STACKSIZE);
    static struct k_thread rxdata_parse_task_thread;
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
    uint8_t sum = 0;

    for (i = from; i < to; i++)
    {
        sum ^= data[i];
    }
    return sum;
}

static void parse_master_rxdata(uint8_t *buf, power_mgr_t *pm_info, dnn_context_t *cxt)
{
    float factor = 0.1;

    g_hostData.signal_type = buf[0];
    g_hostData.duration = buf[1];
    g_hostData.target_temp_high = buf[2];
    g_hostData.target_temp_low = buf[3];
    g_hostData.outdoor_temp_high = buf[4];
    g_hostData.outdoor_temp_low = buf[5];
    g_hostData.indoor_temp_high = buf[6];
    g_hostData.indoor_temp_low = buf[7];
    g_hostData.humidity = buf[8];
    g_hostData.target_freq = buf[9];
    g_hostData.cur_freq = buf[10];
    g_hostData.compressor_status = buf[11];
    g_hostData.indoorfan_speed = buf[12];
    g_hostData.indoorfan_mode = buf[13];
    g_hostData.outdoorfan_speed = buf[14];
    g_hostData.grade0_high = buf[15];
    g_hostData.grade0_low = buf[16];
    g_hostData.grade1_high = buf[17];
    g_hostData.grade1_low = buf[18];

    cxt->duration = g_hostData.duration;
    cxt->indoorfan_speed = (float)g_hostData.indoorfan_speed * 10;
    cxt->target_temp = ((float)g_hostData.target_temp_high - 40) + (float)g_hostData.target_temp_low * factor;
    cxt->indoor_temp = ((float)g_hostData.indoor_temp_high - 40) + (float)g_hostData.indoor_temp_low * factor;
    cxt->outdoor_temp = ((float)g_hostData.outdoor_temp_high - 40) + (float)g_hostData.outdoor_temp_low * factor;
    cxt->humidity = (float)g_hostData.humidity;
    cxt->reset = ((g_hostData.signal_type == RESET_FRAME_HEADER) /*|| (g_hostData.compressor_status == 0x0)*/) ? 0x1 : 0x0;

    cxt->temp_delta = ((int)g_hostData.indoor_temp_high - (int)g_hostData.target_temp_high) * 10 + ((int)g_hostData.indoor_temp_low - (int)g_hostData.target_temp_low);

    cxt->temperature = ((int)g_hostData.indoor_temp_high - 40) * 10 + (int)(g_hostData.indoor_temp_low);

    if ((pm_info->status == AIR_CHANGE)
            && ((cxt->temp_delta <= 10 && pm_info->delta <= 0.0) || (cxt->temp_delta <= 5)))
    {
        /* if change from AIR_CHANGE to AIR_STABLE, target freq is last dnn's output freq*/
        cxt->freq = pm_info->output_freq;
    }
    else
    {
        cxt->freq = (float)g_hostData.target_freq;
    }

    if ((pm_info->status == AIR_IDLE) || (pm_info->output_indoorfan_speed == 0.0) || (cxt->indoorfan_speed != cxt->target_indoorfan_speed))
    {
        cxt->target_indoorfan_speed = cxt->indoorfan_speed;
        pm_info->update_speed_duration = 0;
    }
    else
    {
        cxt->indoorfan_speed = pm_info->output_indoorfan_speed;
    }

    k_sem_give(&cxt->predict_lock);
}

static bool check_rxdata_valid(uint8_t *rxdata)
{
    int from = 0;
    int to = MAX_RX_BUF - 1;

    if (((MASTER_FRAME_HEADER == rxdata[0])  || (RESET_FRAME_HEADER == rxdata[0])) && (checksum(rxdata, from, to) == rxdata[to]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void uart_receive_callback(struct device *dev)
{
    uint32_t    index;
    uint8_t     byte = 0;

    uart_poll_in(dev, &byte);

    index = s_rcvSize++;

    if (index > MAX_RX_BUF - 1)
    {
        index = 0;
    }

    g_rxData[index] = byte;

    if (index == MAX_RX_BUF - 1)
    {
        g_frame_recv_finished = 1;
    }
}

#ifdef SUPPORT_RX_THREAD
static void rxdata_parse_process_thread()
{
    int i = 0;

    while (1)
    {
        if (1 == g_frame_recv_finished)//g_frame_be_read is seted means g_frame_recv_finished is also seted
        {
            g_frame_recv_finished = 0;
            s_rcvSize = 0;

            if (check_rxdata_valid(g_rxData))
            {
                for (i = 0; i < MAX_RX_BUF; i++)
                {
                    PRINTK("%x ", g_rxData[i]);
                    uart_poll_out(uart_dev, g_rxData[i]);
                }

                parse_master_rxdata(g_rxData, &pm_info, &dnn_context);
            }

            clear_frame_buffer(g_rxData, MAX_RX_BUF);
        }

        k_sleep(5);
        k_yield();
    }
}
#endif

static int asm inline fast_floorf_in(float x)
{
    VCVT.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

static int inline fast_floorf(float x)
{
    return fast_floorf_in(x);
}
static int asm inline fast_roundf_in(float x)
{
    VCVTR.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

static int inline fast_roundf(float x)
{
    return fast_roundf_in(x);
}

static void set_slave_txdata(slave_frame_t *frame, power_mgr_t *pm_info, dnn_context_t *cxt)
{
    uint8_t  checksum = 0;
    uint16_t svi = 0;

    if ((frame == NULL) || (pm_info == NULL) || (cxt == NULL))
    {
        return;
    }

    frame->signal_type = SLAVE_FRAME_HEADER;
    checksum ^= frame->signal_type;
    frame->freq = fast_roundf(pm_info->output_freq);
    checksum ^= frame->freq;
    frame->indoorfan_speed = fast_roundf(pm_info->output_indoorfan_speed * 0.1);
    checksum ^= frame->indoorfan_speed;
    frame->outdoorfan_speed = g_hostData.outdoorfan_speed;
    checksum ^= frame->outdoorfan_speed;
    frame->grade0_high = g_hostData.grade0_high;
    checksum ^= frame->grade0_high;
    frame->grade0_low = g_hostData.grade0_low;
    checksum ^= frame->grade0_low;
    frame->grade1_high = g_hostData.grade1_high;
    checksum ^= frame->grade1_high;
    frame->grade1_low = g_hostData.grade1_low;
    checksum ^= frame->grade1_low;

    svi = fast_floorf(cxt->coolingcapacity);
    frame->coolingcapacity_orig_high = svi >> 5;
    checksum ^= frame->coolingcapacity_orig_high;
    frame->coolingcapacity_orig_low = fast_roundf((float)(cxt->coolingcapacity - (frame->coolingcapacity_orig_high << 5)) * 8);
    checksum ^= frame->coolingcapacity_orig_low;

    svi = fast_floorf(pm_info->output_coolingcapacity);
    frame->coolingcapacity_optimize_high = svi >> 5;
    checksum ^= frame->coolingcapacity_optimize_high;
    frame->coolingcapacity_optimize_low = fast_roundf((float)(pm_info->output_coolingcapacity - (frame->coolingcapacity_optimize_high << 5)) * 8);
    checksum ^= frame->coolingcapacity_optimize_low;

    frame->work_status = (pm_info->status == AIR_CHANGE) ? pm_info->capacity_status : pm_info->status;
    checksum ^= frame->work_status;
    frame->res0 = 0x0;
    checksum ^= frame->res0;
    frame->res1 = 0x0;
    checksum ^= frame->res1;

    frame->check = checksum;
}

void protocol_send(power_mgr_t *pm_info, dnn_context_t *cxt)
{
    clear_frame_buffer(g_txData, MAX_TX_BUF);

    set_slave_txdata(&g_slaveData, pm_info, cxt);

    memcpy(g_txData, &g_slaveData, MAX_TX_BUF);

    for (int i = 0; i < MAX_TX_BUF; i++)
    {
        PRINTK("%x ", g_txData[i]);
        uart_poll_out(uart_dev, g_txData[i]);
    }
}

void protocol_start(dnn_context_t *cxt)
{
    /***used for uart protocal**/
    uart_dev = device_get_binding(UART_NAME);
    uart_irq_callback_set(uart_dev, uart_receive_callback);
    uart_irq_rx_enable(uart_dev);

#ifdef SUPPORT_RX_THREAD
    /**2. Create task to parse the received data  */
    k_tid_t rxtid = k_thread_create(&rxdata_parse_task_thread, rxdata_parse_stack_area,
                                    K_THREAD_STACK_SIZEOF(rxdata_parse_stack_area), (k_thread_entry_t)rxdata_parse_process_thread,  NULL, NULL, NULL,
                                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_start(rxtid);
#endif

    k_sem_init(&cxt->predict_lock, 0, UINT_MAX);
}
