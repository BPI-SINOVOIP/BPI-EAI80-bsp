#ifndef _UART_PROTOCOL_H__
#define _UART_PROTOCOL_H__

#include "operation.h"

//#define  DEBUG_PRINT

#define MASTER_FRAME_HEADER        0xE1
#define SLAVE_FRAME_HEADER           0xE2
#define RESET_FRAME_HEADER           0xF0

#define MAX_RX_BUF              32
#define MAX_TX_BUF              16
#define UART_BAUD_RATE      1200L//19200L

#ifndef  DEBUG_PRINT
    #define PRINTF(...)
    #define PRINTK(...)
#else
    #define PRINTF(...)  printf
    #define PRINTK(...)  printf
#endif

typedef struct
{
    uint8_t signal_type;
    uint8_t duration;
    uint8_t target_temp_high;
    uint8_t target_temp_low;
    uint8_t outdoor_temp_high;
    uint8_t outdoor_temp_low;
    uint8_t indoor_temp_high;
    uint8_t indoor_temp_low;
    uint8_t humidity;
    uint8_t target_freq;
    uint8_t cur_freq;
    uint8_t compressor_status;
    uint8_t indoorfan_speed;
    uint8_t indoorfan_mode;
    uint8_t outdoorfan_speed;
    uint8_t grade0_high;
    uint8_t grade0_low;
    uint8_t grade1_high;
    uint8_t grade1_low;
    uint8_t inside_temp_high;
    uint8_t inside_temp_low;
    uint8_t outside_temp_high;
    uint8_t outside_temp_low;
    uint8_t compressor_temp;
    uint8_t res0;
    uint8_t res1;
    uint8_t res2;
    uint8_t res3;
    uint8_t res4;
    uint8_t res5;
    uint8_t res6;
    uint8_t check;
} master_frame_t;

typedef struct
{
    uint8_t signal_type;
    uint8_t freq;
    uint8_t indoorfan_speed;
    uint8_t outdoorfan_speed;
    uint8_t grade0_high;
    uint8_t grade0_low;
    uint8_t grade1_high;
    uint8_t grade1_low;
    uint8_t coolingcapacity_orig_high;
    uint8_t coolingcapacity_orig_low;
    uint8_t coolingcapacity_optimize_high;
    uint8_t coolingcapacity_optimize_low;
    uint8_t work_status;
    uint8_t res0;
    uint8_t res1;
    uint8_t check;
} slave_frame_t;

void protocol_start(dnn_context_t *cxt);
void protocol_send(power_mgr_t *pm_info, dnn_context_t *cxt);

#endif