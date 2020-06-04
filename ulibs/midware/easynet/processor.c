/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        processor.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/12/19
 *
 * @brief       KDP 310 runner for easynet
 *
 * @note
 *              2019/12/19, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ugelis.h>
#include <board.h>
#include "processor.h"
#include "gm_hal_kdp.h"
#include "processor_common.h"
#include "cross_kdp.h"
#include "bank_operation.h"

void *malloc_ext(uint8_t memx, uint32_t size);
void free_ext(uint8_t memx, void *ptr);

#define DEBUG_TIME 1

int cntt = 0;
int count_mm = 0;
int count_cd = 0;
int count_output = 0;
int count_input = 0;

uint32_t t_MaxPool = 0, t_Layer = 0, t_AvgPool = 0, t_WriteOut = 0, t_DataFetch = 0, t_Input = 0;
uint32_t t_Hw = 0, t_BankGeneral = 0, t_CrossPut = 0, t_dwcon = 0;
int layer_cnt  = 0;
static struct conv_hw_context kdp_hw_context;

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param);

static struct easynet_dev kdp_dev =
{
    NULL,
    kdp_ops_process,
    0,
    0,
    0,
};

static kdp_processor_t kdprocessor;

#ifdef DEBUG_KDP_LEVEL_1
void compute_total_ab(unsigned char *addr, unsigned int len);
void compute_total_cd(unsigned char *addr, unsigned int len);
void compute_total_out(unsigned char *addr, unsigned int len);
void compute_total_input(unsigned char *addr, unsigned int len);

unsigned char data_tmp[0x20000];

void compute_total_ab(unsigned char *addr, unsigned int len)
{
    unsigned int total = 0;
    memcpy(&data_tmp[0], addr, len);
    int i = 0;


    while (i < len)
    {
        total += data_tmp[i];
        i++;
    }
    printf("total[%d]=[%d]\n", count_mm, total);

    count_mm++;
}

void compute_total_cd(unsigned char *addr, unsigned int len)
{
    unsigned int total = 0;
    memcpy(&data_tmp[0], addr, len);
    int i = 0;

    while (i < len)
    {
        total += data_tmp[i];
        i++;
    }
    printf("totalcd[%d]=[%d]\n", count_cd, total);
    count_cd++;
}

void compute_total_out(unsigned char *addr, unsigned int len)
{
    unsigned int total = 0;
    int i = 0;

    while (i < len)
    {
        total += addr[i];
        i++;
    }

    printf("totalout[%d]=[%d]\n", count_output, total);

    count_output++;
}

void compute_total_input(unsigned char *addr, unsigned int len)
{
    unsigned int total = 0;
    int i = 0;

    while (i < len)
    {
        total += addr[i];
        i++;
    }

    printf("totalinput[%d]=[%d]\n", count_input, total);

    count_input++;

}
#endif

int ops_parse(unsigned char *bin, unsigned char **param, unsigned char **cmd_addr, unsigned char **weight_addr)
{
    int ops_len;
    int cmd_len;
    unsigned int magic = 0;

    count_mm = 0;
    count_cd = 0;

    count_output = 0;
    count_input = 0;

    magic = bin[0] | (bin[1] << 8) | (bin[2] << 16) | (bin[3] << 24);

    if (OPS_FILE_MAGIC_NUMBER == magic)
    {
        struct easynet_file_head *head = (struct easynet_file_head *)bin;
        *param = (unsigned char *)(bin + head->ops_offset);

        *cmd_addr = bin + head->cmd_offset;
        *weight_addr = bin + head->weight_offset;
    }
    else
    {
        ops_len = HW_OP_MAX_SIZE;
        cmd_len = HW_CMD_MAX_SIZE;

        *param = bin;
        *cmd_addr = *param + ops_len;
        *weight_addr = *param + ops_len + cmd_len;
    }

    return 0;
}

struct easynet_dev *easynet_process_init(struct easynet_dev_cfg *cfg, unsigned char *ops_data)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = cfg->layer_buffer[0];
    processor->network_output = cfg->layer_buffer[1];

    len = ops_parse(ops_data, (unsigned char **)&dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;
    kdp_dev.cfg = cfg;

    int i = 0;
    for (i = 0; i < MAX_LAYER_BUFFER_NUM; i++)
    {
        kdp_hw_context.layer_buffer[i] = cfg->layer_buffer[i];
    }
    kdp_hw_context.bank_a_addr = (void *)BRAM_OUTPUT_A_BASEARDDR;
    kdp_hw_context.bank_c_addr = (void *)BRAM_OUTPUT_C_BASEARDDR;

    kdp_hw_context.weight_start_addr = processor->weight_addr_start;
    kdp_hw_context.cmd_start_addr = processor->cmd_addr_start;

    kdp_dev.context = &kdp_hw_context;

    return &kdp_dev;
}

struct easynet_dev *easynet_process_update(struct easynet_dev_cfg *cfg, unsigned char *ops_data)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    extern int cntt;
    cntt = 0;

    processor->network_input = cfg->layer_buffer[0];
    processor->network_output = cfg->layer_buffer[1];

    len = ops_parse(ops_data, (unsigned char **)&dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    kdp_dev.cfg = cfg;

    int i = 0;
    for (i = 0; i < MAX_LAYER_BUFFER_NUM; i++)
    {
        kdp_hw_context.layer_buffer[i] = cfg->layer_buffer[i];
    }

    kdp_hw_context.bank_a_addr = (void *)BRAM_OUTPUT_A_BASEARDDR;
    kdp_hw_context.bank_c_addr = (void *)BRAM_OUTPUT_C_BASEARDDR;

    kdp_hw_context.weight_start_addr = processor->weight_addr_start;
    kdp_hw_context.cmd_start_addr = processor->cmd_addr_start;
    kdp_dev.context = &kdp_hw_context;

    return &kdp_dev;
}

void easynet_process_deinit(struct easynet_dev *dev)
{
    int i = 0;

    for (i = 0; i < MAX_SHORTCUT_NUM; i++)
    {
        //free_ext(0, dev->shortcut_buf[i]);
        //dev->shortcut_buf[i] = NULL;
    }

    for (i = 0; i < 2; i++)
    {
        //free_ext(0, dev->yolo_out_buffer[i]);
        //dev->yolo_out_buffer[i] = NULL;
    }
}

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param)
{
    int ret = 1;
    struct op_hardware_param *op_param = (struct op_hardware_param *)(((unsigned char *)param) + sizeof(struct easynet_ops_param));
    struct conv_hw_context *cxt = ((struct conv_hw_context *)dev->context);
    kdp_processor_t *processor = &kdprocessor;

#ifdef DEBUG_TIME
    uint32_t time_last = 0, time_now = 0;
    time_last = k_uptime_get_32();
#endif

#ifdef DEBUG_KDP_LEVEL_0
    printf("\ncntt=[%d]\n", cntt);
#endif

#ifdef DEBUG_KDP_LEVEL_1

    if (cntt >= 0)
    {
        unsigned int read_val = HW32_REG(0x46000000);
        HW32_REG(0x46000000) = 0;
        compute_total_ab((unsigned char *)0x20020000, 0x20000);
        compute_total_cd((unsigned char *)0x20040000, 0x20000);
        compute_total_input((unsigned char *)cxt->layer_buffer[0], 0x20000);
        compute_total_out((unsigned char *)cxt->layer_buffer[1], 0x20000);

        HW32_REG(0x46000000) = read_val;
    }
    if (cntt >= 772)
    {
        printf("\nwaiter\n");
    }

#endif

#ifdef DEBUG_KDP_LEVEL_0
    kdp_print_op(param->op);
#endif

    switch (param->op)
    {
        case OP_HW:
            HAL_DMA_Send(op_param->weight_len, (unsigned int)(cxt->weight_start_addr + op_param->weight_offset)); // stream fifo  address must 32 byte aliged
            put_cmd(cxt->cmd_start_addr, op_param->cmd_offset, op_param->cmd_len);
            HAL_CNN_Start();
            HAL_CNN_Wait();
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_Hw += time_now - time_last;
#endif
            break;

        default:
            soft_ops_process(dev, param, processor);
            break;
    }

    cntt++;

    return ret;

}

int easynet_ops_process(struct easynet_dev *dev)
{
    int ret;
    struct easynet_ops_param *param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);

    /* Check sync word */
    if (param->sync != SYNC_WORD)
    {
        printf("easynet_ops_process RET_ERROR\n");
        return RET_ERROR;
    }

    /* Process one software command/operator or one group of hardware operators */
    ret = dev->process(dev, param);

    /* Continue polling if it is wait command and HW IP is busy on processing last command */
    if (param->op == OP_WAIT && ret == RET_BUSY)
    {
        printf("easynet_ops_process RET_BUSY\n");
        return RET_BUSY;
    }

    /* Next operator */
    dev->cur_addr += param->len;
    param = (struct easynet_ops_param *)((unsigned char *)dev->param + dev->cur_addr);
    /* If it is last operator */
    if (param->len == 0)
    {
        return RET_OVER;
    }

    return RET_ONGOING;
}
