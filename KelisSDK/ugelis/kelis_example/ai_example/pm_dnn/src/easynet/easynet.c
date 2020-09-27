/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        processor.c
 *
 * @author      Wen.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/03/19
 *
 * @brief       KDP 310 runner for easynet
 *
 * @note
 *              2019/03/19, Wen.Liu, v1.0.0
 *                  Initial version.
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "easynet.h"
#include <stdio.h>
#include <stdint.h>
#include <gm_common.h>
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include <misc/printk.h>


#ifndef __USE_KEIL_
    #define AI_SECTION __in_section_unique(mem_ai_data) __aligned(32)
#else
    #define AI_SECTION __attribute__((section(".mem_ai_data"),zero_init)) __aligned(32)
#endif
float inputImage[256];
float OutputImage[256];
unsigned char dnn_first_weight[0xA000] AI_SECTION;
unsigned char dnn_second_weight[0xA000] AI_SECTION;

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

static float asm inline fast_sqrtf_in(float x)
{
    VSQRT.F32 S2, S0
    VMOV.F32  S0, S2
    BX LR
}

static float  inline fast_sqrtf(float x)
{
    return fast_sqrtf_in(x);
}

/*****************************************************************************/
/**
*
* This function will do pre-processing of data (data / 256 - 0.5)
* Then reformat the data into convolution format (8 line in a group)
*
* @param    none
*
* @return   none
*
* @note     None.
*
******************************************************************************/
void pre_processing(struct easynet_ops_param *param, void *image_input_addr, void *inputImage)
{
    struct op_image_input_param *op_param = (struct op_image_input_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int r;
    int c;
    int channel;
    int data;
    uint32_t ch_num, row_num, col_num;
    int index = 0;
    index = 0;
    float *buffer = NULL;
    float *image_input = NULL;

    ch_num = op_param->ch;
    row_num = op_param->row;
    col_num = op_param->col;

    buffer = (float *)inputImage;
    image_input = (float *)image_input_addr;
	
    /*
    * Move data from UART buffer to input Buffer and do pre-processing, also convert data from 8 bit to 16 bit
    */
    for (channel = 0; channel < ch_num; ++channel)
    {
        for (r = 0; r < row_num; ++r)
        {
            for (c = 0; c < col_num; ++c)
            {
                if (channel != ch_num - 1)
                {
                    buffer[index++] = image_input[channel * row_num * col_num + r * col_num + c];
                }
                else
                {
                    buffer[index++] = 0;
                }
            }
        }
    }
}

/**
*
* This function run the fully connect layer
*
* @param
*
* @return
*
*
* @note     None.
*
******************************************************************************/
void soft_fullconnected(struct easynet_ops_param *param, void *network_input, void *network_output, void *weight_start_addr)
{
    struct op_connected_param *op_param = (struct op_connected_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int i, j, feature;
    int size = 0;
    float sum, local_sum;
    float *input = NULL;
    float *output = NULL;
    float *fc_allbias = NULL;
    float *gamma = NULL;
    float *beta = NULL;
    int in_ch  = op_param->in_ch;
    int out_ch  = op_param->out_ch;
	
     /* Set start address of weight groups */
    fc_allbias = weight_start_addr + op_param->weight_offset;

    if (op_param->batch_normalize)
    {
        size = op_param->out_ch * op_param->in_ch + op_param->out_ch;
        beta = fc_allbias + size;
        size = op_param->out_ch;
        gamma = beta + size;
    }
	
    if ((op_param->layer_num)%2 == 0)
    {
        input = (float *)network_input;
        output = (float *)network_output;
    }
    else
    {
        input = (float *)network_output;
        output = (float *)network_input;
    }

     i = out_ch;
         
    for (j = 0; j < out_ch; ++j)
    {
        sum = 0;
        for (feature = 0; feature < in_ch; ++feature)
        {
            local_sum = input[feature] * fc_allbias[i];
            sum += local_sum;
            ++i;
        }
        
        output[j] = sum + fc_allbias[j];
    }
	
    if(op_param->batch_normalize)
    {
        for (i = 0; i < out_ch; ++i)
        {
            output[i] = output[i] * gamma[i] + beta[i];
        }
    }
    
    if (op_param->activation == RELU)
    {
        for (i = 0; i < out_ch; ++i)
        {
            output[i] = (output[i] > 0) ? output[i] : 0 ;
        }
    }
	
    return;
}

static int npu_output(struct easynet_ops_param *param, void *network_input, void *network_output)
{
    struct op_output_param *op_param = (struct op_output_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int32_t ch_num, row_num, col_num;
    int32_t ch, row, col, off;
    float *out;
    int32_t out_val;
    int buf_idx = 0;

    out = (float *)network_output;
    buf_idx = op_param->buffer;

    ch_num  = op_param->out_ch;
    row_num = op_param->out_row;
    col_num = op_param->out_col;

    for (ch = 0; ch < ch_num; ++ch)
    {
        ((float *)network_output)[ch] = out[ch];
    }
   
    return buf_idx;
}

static void print_op(int code)
{
    char *opers[] = {"Hw", "Wait", "MaxPool", "AvgPool", "Cnnected", "WriteOut", "DataFetch", "Input", "Switch"};
    if (code <= 8)
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "default");
    }
}

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param)
{
    int ret = TRUE;
    struct op_hardware_param *op_param = (struct op_hardware_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    kdp_processor_t *processor = &kdprocessor;
    //print_op(param->op);
    switch (param->op)
    {
        case OP_CONNECTED:
            soft_fullconnected(param, processor->network_input, processor->network_output, processor->weight_addr_start);
            break;
        case OP_INPUT:
            pre_processing(param, dev->input, processor->network_input);
            break;
        case OP_OUTPUT:
            npu_output(param, processor->network_input, processor->network_output);
            break;
        default:
            break;
    }

    return ret;

}

int ops_parse(unsigned char *bin, unsigned char **param, unsigned char **cmd_addr, unsigned char **weight_addr)
{
    int ops_len;
    int cmd_len;
    int weight_len;
    unsigned int magic = 0;

    magic = bin[0] | (bin[1] << 8) | (bin[2] << 16) | (bin[3] << 24);

    if (OPS_FILE_MAGIC_NUMBER == magic)
    {
        struct easynet_file_head *head = (struct easynet_file_head *)bin;
        *param = (struct easynet_ops_param *)(bin + head->ops_offset);
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

struct easynet_dev *easynet_process_init(char *seq_buf)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = (float *)inputImage;
    processor->network_output = (float *)OutputImage;

    len = ops_parse(seq_buf, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

struct easynet_dev *easynet_process_update(char *seq_buf, void *pInputImage, void *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = (float *)pInputImage;
    processor->network_output = (float *)pOutputImage;

    len = ops_parse(seq_buf, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

void easynet_process_deinit()
{

}

int easynet_ops_process(struct easynet_dev *dev)
{
    int ret;
    struct easynet_ops_param *param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);// + sizeof(struct easynet_ops_param);

    /* Check sync word */
    if (param->sync != SYNC_WORD)
    {
        return RET_ERROR;
    }

    /* Process one software command/operator or one group of hardware operators */
    ret = dev->process(dev, param);

    /* Continue polling if it is wait command and HW IP is busy on processing last command */
    if (param->op == OP_WAIT && ret == RET_BUSY)
    {
        return RET_BUSY;
    }

    /* Next operator */
    dev->cur_addr += param->len;

    param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);
    /* If it is last operator */
    if (param->len == 0)
    {
        return RET_OVER;
    }

    return RET_ONGOING;
}

