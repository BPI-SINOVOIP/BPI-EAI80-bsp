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
#include "processor.h"
#include <stdio.h>
#include <stdint.h>
#include <gm_common.h>
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <dma.h>
#include "ai_common.h"
#include "math.h"

/*
 * The following buffers are used in this example to send and receive data
 * with the UART.
 */

#ifndef __USE_KEIL_
    #define AI_SECTION __in_section_unique(mem_ai_data)
#else
    #define AI_SECTION __attribute__((section("mem_ai_data"),zero_init))
#endif
unsigned char INPUT_BASE[TEST_BUFFER_SIZE] AI_SECTION __aligned(32);
unsigned short inputImage[BUFFERSIZE] AI_SECTION __aligned(32);
unsigned short OutputImage[BUFFERSIZE] AI_SECTION __aligned(32);
float OutputVectoror[FC_CHANNEL] AI_SECTION __aligned(32);
float OutputVectorFC[FC_OUT] AI_SECTION __aligned(32);
unsigned char WEI_BASE[WEI_BIN_SIZE] AI_SECTION __aligned(32);
frame_data_t VALIDATE_BACKUP[VALIDATE_BACKUP_CNT] AI_SECTION __aligned(32);
frame_data_t RECORD_BASE_BACKUP[RECORD_BACKUP_CNT] AI_SECTION __aligned(32); // ~ 227 * (96 * 96 * 3)
frame_data_t CAPTURE_BACKUP[CAPTURE_BACKUP_CNT] AI_SECTION __aligned(32);

int reg_read(unsigned int addr)
{
    return HW32_REG(addr);
}


extern int debug_ai;

/**
*
* This function run average pooling
*
* @param a array
*
* @return
*
*
* @note     None.
*
******************************************************************************/
void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow)
{
    float localSum, n;
    int r, c, feature;
    int j = 0;
    static int cnt_rr = 0;
    for (feature = 0; feature < NumChannel; ++feature)
    {
        localSum = 0;
        for (r = 0; r < validRow; ++r)
        {
            for (c = 0; c < validCol; ++c)
            {
                n = OutputImage[feature * NumRow * NumCol + r * NumCol + c] / 256.0;
                localSum += n;
            }
        }
        localSum = localSum / (validRow * validCol);
        OutputVectoror[feature] = localSum;
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

#if defined(F6721B_EVB_QFN80PIN )
void FC(float *fcVector)
{

    float norm_denominator = 0;

    int i, j, k, feature;
    short a;
    float m, bias, sum, local_sum;
    float allbias[FC_OUT];
    i = 0;
    for (j = 0; j < FC_OUT; ++j)
    {
        k = reg_read((int)(FC_W_BASEADDR + i * 4));
        a = k & 0xFFFF;
        bias = a / 256.0;
        allbias[j] = bias;
        ++i;
    }
    for (j = 0; j < FC_OUT; ++j)
    {
        sum = 0;
        for (feature = 0; feature < FC_CHANNEL; ++feature)
        {
            k = reg_read((int)(FC_W_BASEADDR + i * 4));

            a = k & 0xFFFF;
            m = a / 256.0;

            local_sum = OutputVectoror[feature] * m;
            sum += local_sum;
            ++i;
        }

        //if(j < 2)
        //    printf("sum[%d]=%f, allbias=%f\n",j, sum, allbias[j]);

        fcVector[j] = sum + allbias[j];
        norm_denominator += pow(fcVector[j], 2);
    }
    norm_denominator = sqrt(norm_denominator);
    for (j = 0; j < FC_OUT; ++j)
    {
        fcVector[j] = fcVector[j] / norm_denominator;
    }

#if 0
    for (j = 0; j < 10; ++j)
    {
        if (j % 8 != 0)
        {
            printf("fcVector[%d] = %f ", j, fcVector[j]);
        }
        else
        {
            printf("\n");
            printf("fcVector[%d] = %f ", j, fcVector[j]);
        }
    }
#endif


}
#elif  defined(F6721B_EVB_176PIN)

float g_fc_allbias[FC_OUT + FC_OUT * FC_CHANNEL];

void FC_Init()
{

    float norm_denominator = 0;

    int i, j, k, feature;
    short a;
    float m, bias, sum, local_sum;
    i = 0;
    for (j = 0; j < FC_OUT; ++j)
    {
        k = reg_read((FC_W_BASEADDR + i * 4));
        a = k & 0xFFFF;
        bias = a / 256.0;
        g_fc_allbias[j] = bias;
        ++i;
    }
    for (j = 0; j < FC_OUT; ++j)
    {
        sum = 0;
        for (feature = 0; feature < FC_CHANNEL; ++feature)
        {
            k = reg_read((int)(FC_W_BASEADDR + i * 4));

            a = k & 0xFFFF;
            m = a / 256.0;

            g_fc_allbias[i] = m;
            ++i;
        }
    }
}

void FC(float *fcVector)
{

    float norm_denominator = 0;
    int i, j, k, feature;
    short a;
    float m, bias, sum, local_sum;

    i = FC_OUT;
    for (j = 0; j < FC_OUT; ++j)
    {
        sum = 0;
        for (feature = 0; feature < FC_CHANNEL; ++feature)
        {
            k = reg_read((int)(FC_W_BASEADDR + i * 4));

            a = k & 0xFFFF;
            m = a / 256.0;

            local_sum = OutputVectoror[feature] * g_fc_allbias[i];
            sum += local_sum;
            ++i;
        }

        fcVector[j] = sum + g_fc_allbias[j];
        norm_denominator += pow(fcVector[j], 2);
    }
    norm_denominator = sqrt(norm_denominator);

    for (j = 0; j < FC_OUT; ++j)
    {
        fcVector[j] = fcVector[j] / norm_denominator;
    }
#if 0
    for (j = 0; j < 10; ++j)
    {
        if (j % 8 != 0)
        {
            printf("fcVector[%d] = %f ", j, fcVector[j]);
        }
        else
        {
            printf("\n");
            printf("fcVector[%d] = %f ", j, fcVector[j]);
        }
    }
#endif
}


#endif

void wait_kdp(void)
{
    HAL_CNN_Wait();
}

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


static void reg_write(unsigned  addr, int data)
{
    HW32_REG(addr) = data;
}

static void start_kdp(void)
{
    HAL_CNN_Start();
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

void pre_processing(struct easynet_ops_param *param, unsigned char *image_input_addr, unsigned short int *inputImage)
{
    struct op_image_input_param *op_param = (struct op_image_input_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int r;
    int c;
    int channel;
    int data;
    unsigned short temp_data;
    uint32_t ch_num, row_num, col_num;
    int index = 0;
    index = 0;

    ch_num = op_param->ch;
    row_num = op_param->row;
    col_num = op_param->col;

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
                    data = image_input_addr[channel * row_num * col_num + r * col_num + c];
                    temp_data = data - 128;
                    inputImage[index++] = temp_data;
                }
                else
                {
                    inputImage[index++] = 0;
                }
            }
        }
    }
}


/*****************************************************************************/
/**
*
* This function puts the command into command BRAM
*
* @param    start of index, end of index
*
* @return   none
*
* @note     None.
*
******************************************************************************/
static void put_cmd(unsigned char *cmd_addr_start, int start, int length)
{

    int data = 0;
    int tmp, i, index;

    for (index = 0; index < length; index++)
    {
        if (index % 2 == 0)
        {
            tmp = cmd_addr_start[index + start + 1] << 8 | cmd_addr_start[index + start];
            data = tmp;
        }

        if (index % 2 == 1)
        {
            i = index / 2;
            reg_write(BRAM_CMD_BASEARDDR + i * 4, data);

            //printf("cmd = 0x%x\t addr = %x\n",data,BRAM_CMD_BASEARDDR+i*4);
            data = 0;
        }
    }

}


static void wait_cnn(void)
{
    int ready;
    ready = reg_read(KDP_STATUS);
    while (ready != 1)
    {
        ready = reg_read(KDP_STATUS);
    }
}

static int kdp_status(void)
{
    int ready;
    ready = reg_read(KDP_STATUS);
    return (ready == 1) ? TRUE : FALSE;
}


/*****************************************************************************/
/**
*
* This function get data of from SRAM
*
*
*
* @param    start index, length
*
* @return   None
*
* @note     None.
*
*******************************************************************************/
static void GetData(struct easynet_ops_param *param, int16_t *inputImage, int16_t *outputImage)
{
    struct op_writeout_param *op_param = (struct op_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t start, lengthSRAMOutput, length, NumCol, NumChannel, NumRow, buffer, startChannel, lenChannel;
    int k1;
    int k2;
    int temp_r = 0;
    int temp_c = 0;
    int temp_channel = 0;
    int k1_1, k1_2, k2_1, k2_2;
    int channel = 0;
    int r = 0;
    int c = 0;
    int padding_height, padding_width, padding_channel;
    static int m = 0;

    start = op_param->start_row;
    lengthSRAMOutput = op_param->len_row;
    length = op_param->valid_len_row;
    NumCol = op_param->out_col;
    NumChannel = op_param->out_ch;
    NumRow = op_param->out_row;
    buffer = op_param->buffer;
    startChannel = op_param->start_channel;
    lenChannel = op_param->len_channel;

    padding_height = lengthSRAMOutput;
    padding_width = NumCol;
    padding_channel = lenChannel;
    /*
     * Take data from two banks, and each bank contains channelNum / 2 data
     */
    for (channel = 0; channel < (padding_channel / 2); ++channel)
    {
        /*
         * 4 lines in SRAM, each time we get 32 bit corresponding to 2 data in a column
         */
        for (r = 0; r < 4; ++r)
        {
            for (c = 0; c < (padding_height / 4 / 2) * padding_width; ++c)
            {
                temp_r = r;
                temp_c = c;
                temp_r =  r + c / padding_width * 4;
                temp_c = c % padding_width;
                if (temp_c < NumCol && temp_r * 2 < length)
                {
                    if (buffer == 0)
                    {
                        k1 = reg_read(BANK_A_ADDRESS + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                        k2 = reg_read(BANK_B_ADDRESS + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                    }
                    else
                    {
                        k1 = reg_read(BANK_C_ADDRESS + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                        k2 = reg_read(BANK_D_ADDRESS + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                    }
                    k1_1 = k1 >> 16;
                    k1_2 = k1 & 0xFFFF;
                    k2_1 = k2 >> 16;
                    k2_2 = k2 & 0xFFFF;

                    /* AB bank, output to Buffer 0; CD Bank, outpu to Buffer 1 */
                    if (buffer == 0)
                    {
                        inputImage[(channel * 2 + startChannel)*NumCol * NumRow + (temp_r * 2 + start) * NumCol + temp_c] = k1_1;
                        inputImage[(channel * 2 + startChannel + 1)*NumCol * NumRow + (temp_r * 2 + start) * NumCol + temp_c] = k2_1;
                        if (temp_r * 2 + 1 < length)
                        {
                            inputImage[(channel * 2 + startChannel)*NumCol * NumRow + (temp_r  * 2 + 1 + start) * NumCol + temp_c] = k1_2;
                            inputImage[(channel * 2 + startChannel + 1)*NumCol * NumRow + (temp_r  * 2 + 1 + start) * NumCol + temp_c] = k2_2;
                        }
                    }
                    else
                    {
                        outputImage[(channel * 2 + startChannel)*NumCol * NumRow + (temp_r * 2 + start) * NumCol + temp_c] = k1_1;
                        outputImage[(channel * 2 + startChannel + 1)*NumCol * NumRow + (temp_r * 2 + start) * NumCol + temp_c] = k2_1;
                        if (temp_r * 2 + 1 < length)
                        {
                            outputImage[(channel * 2 + startChannel)*NumCol * NumRow + (temp_r  * 2 + 1 + start) * NumCol + temp_c] = k1_2;
                            outputImage[(channel * 2 + startChannel + 1)*NumCol * NumRow + (temp_r  * 2 + 1 + start) * NumCol + temp_c] = k2_2;
                        }
                    }

                }
            }
        }
    }

}

/**
*
* This function put data of correct order into SRAM
*
*
*
* @param    start index, length
*
* @return   None
*
* @note     None.
*
*******************************************************************************/

static void PutData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_datafetch_param *op_param = (struct op_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int channel, begin, index, i, r, c, temp_r, sum_r;
    uint32_t start, length, NumCol, NumChannel, NumRow, buffer;

    start = op_param->start_row;
    length = op_param->len_row;
    NumCol = op_param->col;
    NumChannel = op_param->ch;
    NumRow = op_param->row;
    buffer = op_param->buffer;


    //odd channel number
    index = 0;
    int data = 0, tmp = 0, j = 0, p = 0, m = 0;
    for (i = 0; i < 2 ; ++i)
    {
        m = 0;
        for (channel = i; channel < NumChannel; channel += 2)
        {
            for (r = 0; r < length; r += 8)
            {
                for (c = 0; c < NumCol; ++c)
                {
                    for (temp_r = 0; temp_r < 8; ++temp_r)
                    {
                        sum_r = temp_r + r;

                        if (sum_r + start < NumRow)
                        {
                            if (buffer == 0)
                            {
                                tmp = inputImage[c + (sum_r + start) * NumCol + NumCol * NumRow * channel];
                            }
                            else
                            {
                                tmp = outputImage[c + (sum_r + start) * NumCol + NumCol * NumRow * channel];
                            }
                        }
                        else
                        {
                            tmp = 0;
                        }

                        data = data << 16;
                        data = data + tmp;
                        if (index % 2 == 1)
                        {
                            //Put into SRAM
                            j = (temp_r / 2) % 4; //choose small bank according to row number

                            p = ((m * (length / 8) * NumCol) + (r / 8) * NumCol + c);

                            if (buffer == 0)
                            {
                                reg_write(BANK_A_ADDRESS + i * 0x10000 + j * 0x4000 + p * 4, data);
                            }
                            else
                            {
                                reg_write(BANK_C_ADDRESS + i * 0x10000 + j * 0x4000 + p * 4, data);
                            }
                            data = 0;
                        }
                        ++index;
                    }
                }
            }
            ++m;
        }
    }
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
        case OP_HW:
            put_cmd(processor->cmd_addr_start, op_param->cmd_offset, op_param->cmd_len);
            HAL_DMA_Send(op_param->weight_len, processor->weight_addr_start + op_param->weight_offset);
            HAL_CNN_Start();
            HAL_CNN_Wait();

            break;
        case OP_WAIT:
            ret = kdp_status();
            break;
        case OP_MAXPOOL:
            break;
        case OP_AVGPOOL:
            break;
        case OP_CONNECTED:
            break;
        case OP_WRITEOUT:
            GetData(param, processor->network_input, processor->network_output);
            break;
        case OP_DATAFETCH:
            PutData(param, processor->network_input, processor->network_output);
            break;
        case OP_INPUT:
            pre_processing(param, dev->input, processor->network_input);
            break;
        case OP_SWITCH_INOUT:
            break;
        default:
            break;
    }

    return ret;

}

int ops_parse(char *seq_buf, unsigned char **param, unsigned char **cmd_addr, unsigned char **weight_addr)
{

    int ops_len;
    int cmd_len;
    int weight_len;
    int off = 0;

    ops_len = 0x10000;
    off = 0;
    cmd_len = 0x10000;

    *param = seq_buf + off;
    *cmd_addr = *param + ops_len;
    *weight_addr = *param + ops_len + cmd_len;

    return 0;
}

struct easynet_dev *easynet_process_init(char *seq_buf)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &inputImage[0];
    processor->network_output = &OutputImage[0];

    len = ops_parse(seq_buf, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

struct easynet_dev *easynet_process_update(char *seq_buf, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

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
