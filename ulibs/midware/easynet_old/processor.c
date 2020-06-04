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
#include "processor.h"
#include <stdint.h>
#include <gm_common.h>
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include "gm_hal_kdp.h"


enum
{
    ENTRY_X          = 0,
    ENTRY_Y          = 1,
    ENTRY_W          = 2,
    ENTRY_H          = 3,
    ENTRY_CONFIDENCE = 4,
    ENTRY_CLASS      = 5
};

void *malloc_ext(uint8_t memx, uint32_t size);
void free_ext(uint8_t memx, void *ptr);

#define DEBUG_TIME 1
//#define DEBUG_KDP  1


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
        /*printf("head->ops_offset=0x%x cmd_offset=0x%x\n",head->ops_offset, head->cmd_offset);*/

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

struct easynet_dev *easynet_process_init(struct easynet_dev_cfg *cfg, char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

    len = ops_parse(ops_data, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;
    kdp_dev.cfg = cfg;
    kdp_dev.context = &kdp_hw_context;

    return &kdp_dev;
}

struct easynet_dev *easynet_process_update(struct easynet_dev_cfg *cfg, unsigned char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

    len = ops_parse(ops_data, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    kdp_dev.cfg = cfg;
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

static int imp_trans(int src_addr, int src_offset, int drc_addr, int drc_offset, int image_w, int image_h, int rd16_wr32_en, int rd32_wr16_en)
{
    unsigned int  tmp, i, b, c;
    unsigned int  imp_base;
    unsigned int  imp_cr_addr;
    unsigned int  imp_bpc_addr      ;
    unsigned int  imp_isr_addr      ;
    unsigned int  imp_fgmar_addr        ;
    unsigned int  imp_fgor_addr ;
    unsigned int  imp_bgor_addr     ;
    unsigned int  imp_bgmar_addr    ;
    unsigned int  imp_fgpfccr_addr      ;
    unsigned int  imp_fgclor_addr   ;
    unsigned int  imp_bgpfccr_addr;
    unsigned int  imp_bgclor_addr       ;
    unsigned int  imp_opfccr_addr    ;
    unsigned int  imp_ocolr_addr  ;
    unsigned int  imp_omar_addr   ;
    unsigned int  imp_oor_addr   ;
    unsigned int  imp_nlr_addr   ;
    unsigned int  imp_lwr_addr   ;
    unsigned int  imp_amtcr_addr   ;
    unsigned int  imp_ip_ver_addr  ;

    imp_base = 0x40024000;
    imp_cr_addr         = imp_base + 0x00;
    imp_isr_addr            = imp_base + 0x04;
    imp_fgmar_addr          = imp_base + 0x0C;
    imp_fgor_addr           = imp_base + 0x10;
    imp_bgor_addr           = imp_base + 0x18;
    imp_bgmar_addr          = imp_base + 0x14;
    imp_fgpfccr_addr            = imp_base + 0x1C;
    imp_fgclor_addr         = imp_base + 0x20;
    imp_bgpfccr_addr            = imp_base + 0x24;
    imp_bgclor_addr         = imp_base + 0x28;
    imp_opfccr_addr          = imp_base + 0x34;
    imp_ocolr_addr        = imp_base + 0x38;
    imp_omar_addr         = imp_base + 0x3C;
    imp_oor_addr         = imp_base + 0x40;
    imp_nlr_addr         = imp_base + 0x44;
    imp_lwr_addr         = imp_base + 0x48;
    imp_amtcr_addr         = imp_base + 0x4c;
    //release clock and reset
    HW32_REG(0x40000080) = 0xFFFFFFFF;
    HW32_REG(0x40000070) = 0xFFFFFFFF;
    //config fg memory addr
    HW32_REG(imp_fgmar_addr) = src_addr;
    //config fg offset = 0
    HW32_REG(imp_fgor_addr) = src_offset;
    //config fg color mode = argb8888
    //    HW32_REG(imp_fgpfccr_addr)= 0x0;

    if (rd16_wr32_en)
    {
        HW32_REG(imp_fgpfccr_addr) = 0x2;
    }
    else if (rd32_wr16_en)
    {
        HW32_REG(imp_fgpfccr_addr) = 0x0;
    }
    //config out memory addr

    if (rd16_wr32_en)
    {
        HW32_REG(imp_opfccr_addr) = 0;
    }
    else if (rd32_wr16_en)
    {
        HW32_REG(imp_opfccr_addr) = 2;
    }

    //config out memory addr
    HW32_REG(imp_omar_addr) = drc_addr;
    //config out memory offset 0
    HW32_REG(imp_oor_addr) = drc_offset;
    //config out pl/nl
    HW32_REG(imp_nlr_addr) = (image_w << 16) + image_h;
    //    HW32_REG(imp_cr_addr)=0x00001;
    HW32_REG(imp_cr_addr) = (rd16_wr32_en << 20) + (rd32_wr16_en << 21) + (1 << 16) + 1;
    while (HW32_REG(imp_cr_addr) & 1 != 0);

    return 1;
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
                    temp_data = data;
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
    int cmd_cnt = 0;

    for (index = 0; index < length; index++)
    {
        if (index % 2 == 0)
        {
            tmp = cmd_addr_start[index + start + 1] << 8 | cmd_addr_start[index + start];
            data = tmp;
        }
        else if (index % 2 == 1)
        {
            i = index / 2;
            HW32_REG(BRAM_CMD_BASEARDDR + i * 4) = data;
            data = 0;
        }
    }

}

void maxPool(struct easynet_ops_param *param, unsigned short int  *outputImage)
{
    struct op_maxpool_param *op_param = (struct op_maxpool_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t ch_num, row_num, col_num, out_row, out_col, kernel_size, stride_size;
    int32_t pad;
    int32_t ch, row, col, n, m;
    uint32_t Numrow,  Numcolumn, Numchannel;

    ch_num  = op_param->ch_num;
    row_num = op_param->row_num;
    col_num = op_param->col_num;
    out_row = op_param->out_row;
    out_col = op_param->out_col;
    kernel_size = op_param->pool_size;
    stride_size = op_param->pool_stride;

    Numrow = row_num;
    Numcolumn = col_num;
    Numchannel = ch_num;
    int channel, column, i, max_index, ttt;
    short int  data[4];
    unsigned short int *curBuffer = outputImage;
    for (channel = 0; channel < Numchannel; ++channel)
    {
        ttt = channel * Numcolumn * Numrow;
        for (row = 0; row < Numrow; ++row)
        {
            for (column = 0; column < Numcolumn; ++column)
            {
                max_index = 0;
                data[0] = curBuffer[ttt + row * Numcolumn + column];
                if (row < Numrow - 1 && column < Numcolumn - 1)
                {
                    data[1] = curBuffer[ttt + row * Numcolumn + column + 1];
                    data[2] = curBuffer[ttt + (row + 1) * Numcolumn + column];
                    data[3]  = curBuffer[ttt + (row + 1) * Numcolumn + column + 1];
                }
                else if (column == Numcolumn - 1 && row < Numrow - 1)
                {
                    data[1] = -32768;
                    data[2] = curBuffer[ttt + (row + 1) * Numcolumn + column];
                    data[3] = -32768;
                }
                else if (row == Numrow - 1 && column < Numcolumn - 1)
                {
                    data[1] = curBuffer[ttt + row * Numcolumn + column + 1];
                    data[2] = -32768;
                    data[3] = -32768;
                }
                else
                {
                    data[1] = -32768;
                    data[2] = -32768;
                    data[3] = -32768;
                }

                for (i = 1; i < 4; ++i)
                {
                    if (data[i] > data[max_index])
                    {
                        max_index = i;
                    }
                }
                curBuffer[ttt + row * Numcolumn + column] = data[max_index];
            }
        }
    }
}


void PutData(struct easynet_ops_param *param,  struct conv_hw_context *cxt, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_datafetch_param *op_param = (struct op_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int actualPadSize;
    int channel, begin, index, i, r, c, temp_r, sum_r;
    uint32_t start, length, NumCol, NumChannel, NumRow, buffer, rowStartHL, rowOff, chOff, addressBank;
    unsigned short *input;

    uint32_t bank_a_addr = BRAM_OUTPUT_A_BASEARDDR;
    uint32_t bank_c_addr = BRAM_OUTPUT_C_BASEARDDR;

    start = op_param->start_row;
    length = op_param->len_row;
    NumCol = op_param->col;
    NumChannel = op_param->ch;
    NumRow = op_param->row;
    buffer = op_param->buffer;

    actualPadSize = length * NumCol * NumChannel;

    //odd channel number
    index = 0;
    int data = 0, tmp = 0, j = 0, p = 0, m = 0;

    for (rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
    {
        if (rowStartHL == 1) /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        if (rowStartHL == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);;
        }

        for (i = 0; i < 2 ; ++i)
        {
            for (temp_r = rowStartHL; temp_r < 8; temp_r += 2)
            {
                j = (temp_r / 2) % 4;
                addressBank = i * 0x10000 + j * 0x4000;

                if (buffer == 0)
                {
                    addressBank += bank_a_addr;
                    input = inputImage;
                }
                else
                {
                    addressBank += bank_c_addr;
                    input = outputImage;
                }

                for (channel = i, m = 0; channel < NumChannel; channel += 2, m++)
                {
                    chOff = NumCol * NumRow * channel;
                    for (r = 0; r < length; r += 8)
                    {

                        sum_r = temp_r + r;
                        rowOff = (sum_r + start) * NumCol + chOff;

                        if (sum_r + start < NumRow)
                        {

#if 0
                            for (c = 0; c < NumCol; ++c, addressBank += 4)
                            {
                                HW32_REG(addressBank) = input[c + rowOff];
                            }
#else
                            imp_trans((uint32_t)input + rowOff * 2, 0, (uint32_t)addressBank, 0, NumCol, 1, 1, 0);
                            addressBank += NumCol * 4;

#endif

                        }
                        else
                        {
                            for (c = 0; c < NumCol; ++c, addressBank += 4)
                            {
                                HW32_REG(addressBank) = 0;
                            }

                        }
                    }
                }
            }
        }
    }
}

void kdp_print_op(int code)
{
#ifdef DEBUG_KDP
    char *opers[] = {"Hw", "Wait", "MaxPool", "AvgPool", "Connected", "WriteOut", "DataFetch", "Input", "Switch",
                     "Layer", "Buffer", "Shortcut", "Route", "Activation", "Output", "OP_CROSS_WRITEOUT",
                     "CROSS_DATAFETCH", "OP_BANK_MAXPOOL", "Yolo", "Upsample", "BANK_GENERAL", "OP_BANK_ROUTE",
                     "OP_DEPTHWISE_CONV"
                    };
    if (code < sizeof(opers) / sizeof(char *))
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "UNKOWN");
    }
#endif
}

void GetData(struct easynet_ops_param *param, struct conv_hw_context *cxt, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_writeout_param *op_param = (struct op_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t start, lengthSRAMOutput, length, NumCol, NumChannel, NumRow, buffer, startChannel, lenChannel;
    uint32_t addressBank, chOff, rowOff, rowStartHL, temp_r, i, j, channel, r, c;
    int padding_height, padding_width, padding_channel;
    uint32_t bank_a_addr = BRAM_OUTPUT_A_BASEARDDR;
    uint32_t bank_c_addr = BRAM_OUTPUT_C_BASEARDDR;
    unsigned short *output;

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
    for (rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
    {
        if (rowStartHL == 1)  /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        if (rowStartHL == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
        }

        for (i = 0; i < 2 ; ++i)
        {
            for (temp_r = rowStartHL; temp_r < 8; temp_r += 2)
            {
                j = (temp_r / 2) % 4;
                addressBank = i * 0x10000 + j * 0x4000;

                if (buffer == 0)
                {
                    addressBank += bank_a_addr;
                    output = inputImage;
                }
                else
                {
                    addressBank += bank_c_addr;
                    output = outputImage;
                }

                for (channel = i; channel < lenChannel; channel += 2)
                {
                    chOff = NumCol * NumRow * (channel + startChannel);
                    for (r = temp_r; r < lengthSRAMOutput; r += 8)
                    {
                        rowOff = chOff + (r + start) * NumCol;
                        if (r < length)
                        {
                            for (c = 0; c < NumCol; ++c, addressBank += 4)
                            {
                                output[c + rowOff] = HW32_REG(addressBank);
                            }
                        }
                        else
                        {
                            for (c = 0; c < NumCol; ++c, addressBank += 4);
                        }
                    }
                }
            }
        }
    }
}

void CrossGetData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_cross_writeout_param *op_param = (struct op_cross_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t start_row, len_row, valid_len_row, valid_start_col, start_col, len_col, end_col, out_col, out_ch, out_row, buffer, start_channel, len_channel;
    uint32_t ch_off, row_off, rowOffb, row_idx, bank_row_idx, ch_idx, bank_idx, channel, row, col;
    int16_t *output;
    void *bank_start_addr, *bank_addr, *row_addr, *row_next_addr;
    uint32_t bankval = 0;

    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    valid_len_row = op_param->valid_len_row;
    valid_start_col = op_param->valid_start_col;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    out_col = op_param->out_col;
    out_ch  = op_param->out_ch;
    out_row = op_param->out_row;
    buffer = op_param->buffer;
    start_channel = op_param->start_channel;
    len_channel = op_param->len_channel;


    if (buffer == 0)
    {
        output = inputImage;
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        output = outputImage;
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }
    /*
     * Take data from two banks, and each bank contains channelNum / 2 data
     */
    for (row_idx = 0; row_idx < 1; ++row_idx)
    {
        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;
                for (channel = ch_idx; channel < len_channel; channel += 2)
                {
                    ch_off = out_col * out_row * (channel + start_channel);
                    for (row = bank_row_idx; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + start_row) * out_col;

                        rowOffb = ch_off + (row + start_row + 1) * out_col;

                        end_col = start_col + len_col;
                        if (row < valid_len_row)
                        {

                            bank_addr += (valid_start_col - start_col) * 4;

                            if (row < valid_len_row - 1)
                            {
                                for (col = valid_start_col; col < end_col; ++col, bank_addr += 4)
                                {
                                    bankval =  HW32_REG(bank_addr);
                                    output[col + row_off] = bankval >> 16;  /* Even row , higher half */
                                    output[col + rowOffb] = bankval & 0XFFFF;  /* Odd row , lower half */
                                }
                            }
                            else
                            {
                                for (col = valid_start_col; col < end_col; ++col, bank_addr += 4)
                                {
                                    bankval =  HW32_REG(bank_addr);
                                    output[col + row_off] = bankval >> 16;  /* Even row , higher half */
                                }
                            }
                        }
                        else
                        {
                            bank_addr += (end_col - start_col) * 4;
                        }
                    }
                }
            }
        }
    }

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
}

void CrossGetDataCompiler(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_cross_writeout_param *op_param = (struct op_cross_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t start_row, len_row, valid_len_row, valid_start_col, start_col, len_col, end_col, out_col, out_ch, out_row, buffer, start_channel, len_channel;
    uint32_t ch_off, row_off, row_idx, bank_row_idx, ch_idx, bank_idx, channel, row, col;
    int16_t *output;
    void *bank_start_addr, *bank_addr;

    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    valid_len_row = op_param->valid_len_row;
    valid_start_col = op_param->valid_start_col;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    out_col = op_param->out_col;
    out_ch  = op_param->out_ch;
    out_row = op_param->out_row;
    buffer = op_param->buffer;
    start_channel = op_param->start_channel;
    len_channel = op_param->len_channel;

    if (buffer == 0)
    {
        output = inputImage;
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        output = outputImage;
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }
    /*
     * Take data from two banks, and each bank contains channelNum / 2 data
     */
    for (row_idx = 0; row_idx < 2; ++row_idx)
    {
        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;
                if (row_idx == 0)
                {
                    bank_addr += 2;
                }
                for (channel = ch_idx; channel < len_channel; channel += 2)
                {
                    ch_off = out_col * out_row * (channel + start_channel);
                    for (row = bank_row_idx; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + start_row) * out_col;
                        end_col = start_col + len_col;
                        if (row < valid_len_row)
                        {
#if 0
                            for (col = start_col; col < valid_start_col; ++col, bank_addr += 4);
                            for (col = valid_start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                output[col + row_off] = ((int16_t *)bank_addr)[0];
                            }
#else
                            bank_addr += (valid_start_col - start_col) * 4;
                            for (col = valid_start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                output[col + row_off] = ((int16_t *)bank_addr)[0];
                            }
#endif
                        }
                        else
                        {
                            bank_addr += (end_col - start_col) * 4;
                        }
                    }
                }
            }
        }
    }


    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
}

void CrossFirstPutDataCompilerOptimize(struct easynet_ops_param *param, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, ch_off, row_addr, row_next_addr;

    uint32_t row_offb;

    uint8_t *input = (uint8_t *)first_imginput;
    void *bank_start_addr, *bank_addr;

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    col_num   = op_param->col;
    ch_num    = op_param->ch;
    row_num   = op_param->row;
    buffer    = op_param->buffer;

    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    if (buffer == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    for (row_idx = 0; row_idx < 1; ++row_idx)
    {
        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;

                for (channel = ch_idx; channel < ch_num; channel += 2)
                {
                    ch_off = col_num * row_num * channel;
                    /* Be careful that there is no padding rows in PutData, so you have to start
                             *  from zero , otherwise bank_adder is nor correct
                             */
                    for (row = 0; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + bank_row_idx + start_row) * col_num;

                        row_offb =  ch_off + (row + bank_row_idx + start_row + 1) * col_num;

                        end_col = start_col + len_col;
                        row_addr = ((void *)input) + sizeof(int16_t) * row_off;
                        row_next_addr = row_addr + sizeof(int16_t) * col_num;

                        if (channel == 3 || ((row + bank_row_idx + start_row) >= row_num))
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                ((uint32_t *)bank_addr)[0] = 0;
                            }
                        }
                        else
                        {
                            //for(col = start_col; col < end_col; ++col, bank_addr += 4)
                            //{
                            //  ((uint32_t *)bank_addr)[0] = input[col + row_off] << 16 | input[col + row_offb];
                            //}
                            bank_addr = FastFirstPutDataOptimize(bank_addr, len_col, &input[start_col + row_off], &input[start_col + row_offb]);

                        }
                    }
                }
            }
        }
    }

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
}


void CrossFirstPutDataCompilerNormer(struct easynet_ops_param *param, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, ch_off;
    uint8_t *input = (uint8_t *)first_imginput;
    void *bank_start_addr, *bank_addr;

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    col_num   = op_param->col;
    ch_num    = op_param->ch;
    row_num   = op_param->row;
    buffer    = op_param->bank;

    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    if (buffer == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }


    for (row_idx = 0; row_idx < 2; ++row_idx)
    {
        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;
                if (row_idx == 0)
                {
                    bank_addr += 2;
                }

                for (channel = ch_idx; channel < ch_num; channel += 2)
                {
                    ch_off = col_num * row_num * channel;
                    /* Be careful that there is no padding rows in PutData, so you have to start
                             *  from zero , otherwise bank_adder is nor correct
                             */
                    for (row = 0; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + bank_row_idx + start_row) * col_num;
                        end_col = start_col + len_col;

                        if (channel == 3 || ((row + bank_row_idx + start_row) >= row_num))
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                ((int16_t *)bank_addr)[0] = 0;
                            }
                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                ((int16_t *)bank_addr)[0] = input[col + row_off];
                            }

                        }
                    }
                }
            }
        }
    }

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
}

void FirstPutData(struct easynet_ops_param *param, struct conv_hw_context *cxt, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, ch_off;
    uint8_t *input = (uint8_t *)first_imginput;
    uint32_t bank_start_addr, bank_addr;

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    col_num   = op_param->col;
    ch_num    = op_param->ch;
    row_num   = op_param->row;
    buffer    = op_param->bank;

    if (op_param->bank == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    for (row_idx = 0; row_idx < 2; ++row_idx)
    {

        if (row_idx == 1) /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }
        else if (row_idx == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);;
        }

        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;

                for (channel = ch_idx; channel < ch_num; channel += 2)
                {
                    ch_off = col_num * row_num * channel;
                    /* Be careful that there is no padding rows in PutData, so you have to start
                             *  from zero , otherwise bank_adder is nor correct
                             */
                    for (row = 0; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + bank_row_idx + start_row) * col_num;
                        end_col = start_col + len_col;

                        if (channel == 3 || ((row + bank_row_idx + start_row) >= row_num))
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = 0;
                            }
                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = input[col + row_off];
                            }

                            for (; end_col > col_num; end_col--)
                            {
                                ((int16_t *)(bank_addr - 4 * (end_col - col_num)))[0] = 0;
                            }

                        }
                    }
                }
            }
        }
    }

}

void CrossPutData(struct easynet_ops_param *param, struct conv_hw_context *cxt, unsigned short *inputImage, unsigned short *outputImage, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, ch_off;
    int16_t *input;
    uint32_t bank_start_addr, bank_addr;

    if (op_param->layer_num == 0)
    {
        FirstPutData(param, cxt, first_imginput);
        return;
    }

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    col_num   = op_param->col;
    ch_num    = op_param->ch;
    row_num   = op_param->row;
    buffer    = op_param->buffer;

    if (op_param->bank == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    if (op_param->buffer == 0)
    {
        input = inputImage;
    }
    else
    {
        input = outputImage;
    }

    for (row_idx = 0; row_idx < 2; ++row_idx)
    {
        if (row_idx == 1) /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }
        else if (row_idx == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);;
        }

        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;

                for (channel = ch_idx; channel < ch_num; channel += 2)
                {
                    ch_off = col_num * row_num * channel;
                    /* Be careful that there is no padding rows in PutData, so you have to start
                     *  from zero , otherwise bank_adder is nor correct
                     */
                    for (row = 0; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + bank_row_idx + start_row) * col_num;
                        end_col = start_col + len_col;
                        if ((row + bank_row_idx + start_row) < row_num)
                        {
#if 1
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = input[col + row_off];
                            }
#else
                            int col_len = (end_col - start_col);
                            imp_trans((uint32_t)input + row_off * 2, 0, (uint32_t)bank_addr, 0, col_len, 1, 1, 0);
                            bank_addr += col_len * 4;
#endif
                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = 0;
                            }
                        }
                    }
                }
            }
        }
    }

}
static void layerNumber(struct easynet_ops_param *param)
{
    struct op_layer_param *op_param = (struct op_layer_param *)(((void *)param) + sizeof(struct easynet_ops_param));

    // printf("Process layer = %d\n", op_param->conv);   / richard debug
}

void soft_upsample(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_upsample_param *op_param = (struct op_upsample_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int16_t *input;
    int16_t *tmp;
    uint32_t c, h, w, stride, scale, out_cnt;
    int i, j, k;

    /* Select input buffer */
    if (op_param->buffer == 0)
    {
        input = (int16_t *)inputImage;
    }
    else
    {
        input = (int16_t *)outputImage;
    }

    c = op_param->ch_num;
    h = op_param->row_num;
    w = op_param->col_num;
    stride = op_param->stride;
    out_cnt = op_param->out_col * op_param->out_row * op_param->ch_num;

    tmp = (int16_t *)malloc_ext(0, out_cnt * sizeof(int16_t));
    if (tmp == NULL)
    {
        printf("[%s] malloc error LINE=%d\n", __FUNCTION__, __LINE__);
    }

    for (k = 0; k < c; ++k)
    {
        for (j = 0; j < h * stride; ++j)
        {
            for (i = 0; i < w * stride; ++i)
            {
                int in_index = k * w * h + (j / stride) * w + i / stride;
                int out_index = k * w * h * stride * stride + j * w * stride + i;
                tmp[out_index] = input[in_index];
            }
        }
    }

    memcpy(input, tmp, out_cnt * sizeof(int16_t));
    free_ext(0, tmp);

}

//n: anchor index (0~4)
//entry: (0:x, 1:y, 2:w, 3:h, 4:confidence, 5:classes)
//loc: cell index (0~48)
static int entry_index(int w, int h, int classes, int location, int entry)
{
    int n =   location / (w * h);
    int loc = location % (w * h);
    return  n * w * h * (4 + classes + 1) + entry * w * h + loc;
}

typedef enum
{
    RELU, LEAKY, LOGISTIC, LINEAR
} ACTIVATION;

static inline float linear_activate(float x)
{
    return x;
}

static inline float relu_activate(float x)
{
    return x * (x > 0);
}
static inline float leaky_activate(float x)
{
    return (x > 0) ? x : .1 * x;
}

static float gm_exp(float x)
{
    x = 1.0 + x / 256.0;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    return x;
}

static inline float logistic_activate(float x)
{
    return 1. / (1. + gm_exp(-x));
}

static float activate(float x, ACTIVATION a)
{
    switch (a)
    {
        case RELU:
            return relu_activate(x);
        case LEAKY:
            return leaky_activate(x);
        case LOGISTIC:
            return logistic_activate(x);
        case LINEAR:
            return linear_activate(x);
    }
    return 0;
}

static const int64_t fix8_one = 0x0100;          /*!< fix8_t value of 1 */

int64_t fix32_extend(int64_t value)
{
    return (int16_t)(value);
}

float fix32_to_float(int64_t a)
{
    return (float) fix32_extend(a) / fix8_one;
}

void soft_yolo(struct easynet_ops_param *param,  unsigned short *inputImage, unsigned short *outputImage, float **yolo_out_buffer)
{
    struct op_yolo_param *op_param = (struct op_yolo_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int n, index;
    int16_t *input;
    int i = 0;


    /* Select input buffer */
    if (op_param->buffer == 0)
    {
        input = (int16_t *)inputImage;
    }
    else
    {
        input = (int16_t *)outputImage;
    }

    int w = op_param->col_num;
    int h = op_param->row_num;
    int classes = op_param->classes;
    int out_ch  = op_param->out_ch;

    int out_cnt = h * w * out_ch * (classes + 4 + 1);

    if (yolo_out_buffer[op_param->index] == NULL)
    {
        yolo_out_buffer[op_param->index] = malloc_ext(0, out_cnt * sizeof(float));
    }

    if (yolo_out_buffer[op_param->index] == NULL)
    {
        printf("malloc_ext error\n");
    }
    memset(yolo_out_buffer[op_param->index], 0, out_cnt * sizeof(float));

    for (n = 0; n < out_ch; ++n)
    {
        index = entry_index(w, h, classes,  n * w * h, ENTRY_X);
        for (i = 0; i < 2 * w * h; ++i)
        {
            yolo_out_buffer[op_param->index][index + i] = activate(fix32_to_float(input[index + i]), LOGISTIC);
        }
        index = entry_index(w, h, classes,  n * w * h, ENTRY_CONFIDENCE);
        for (i = 0; i < (1 + classes)*w * h; ++i)
        {
            yolo_out_buffer[op_param->index][index + i] =  activate(fix32_to_float(input[index + i]), LOGISTIC);
        }
    }
}

void soft_maxpool(struct easynet_ops_param *param,  unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_maxpool_param *op_param = (struct op_maxpool_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t ch_num, row_num, col_num, out_row, out_col, kernel_size, stride_size;
    int32_t pad;
    int32_t ch, row, col, n, m;
    int16_t *input;
    float   *float_input;
    int32_t data_type, val;
    float   float_val;

    int32_t w_offset = -op_param->pad_left;
    int32_t h_offset = -op_param->pad_left;

    /* Select input buffer */
    if (op_param->buffer == 0)
    {
        input = (int16_t *)inputImage;
    }
    else
    {
        input = (int16_t *)outputImage;
    }

    data_type = op_param->data_type;
    if (data_type != 0)
    {
        float_input = (float *)input;
    }

    ch_num  = op_param->ch_num;
    row_num = op_param->row_num;
    col_num = op_param->col_num;
    out_row = op_param->out_row;
    out_col = op_param->out_col;
    kernel_size = op_param->pool_size;
    stride_size = op_param->pool_stride;

    pad = op_param->pad_left;

    for (ch = 0; ch < ch_num; ++ch)
    {
        for (row = 0; row < out_row; ++row)
        {
            for (col = 0; col < out_col; ++col)
            {
                int32_t out_index = col + out_col * (row + out_row * ch);
                int32_t max = -32768;
                float float_max = -FLT_MAX;

                for (n = 0; n < kernel_size; ++n)
                {
                    for (m = 0; m < kernel_size; ++m)
                    {
                        int32_t cur_h = h_offset + row * stride_size + n;
                        int32_t cur_w = w_offset + col * stride_size + m;
                        int index = cur_w + col_num * (cur_h + row_num * ch);
                        int valid = (cur_h >= 0 && cur_h < row_num &&
                                     cur_w >= 0 && cur_w < col_num);
                        if (data_type == 0)
                        {
                            val = (valid != 0) ? input[index] : -32768;
                            max   = (val > max) ? val   : max;
                        }
                        else
                        {
                            float_val = (valid != 0) ? float_input[index] : -FLT_MAX;
                            float_max   = (float_val > float_max) ? float_val   : float_max;
                        }
                    }
                }
                if (data_type == 0)
                {
                    input[out_index] = max;
                }
                else
                {
                    float_input[out_index] = float_max;
                }
            }
        }
    }
}

void shortcut_buffer(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage, int16_t **shortcut_buf)
{
    struct op_shortcut_buffer_param *op_param = (struct op_shortcut_buffer_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int i;
    int16_t *output;

    /* Select input buffer */
    if (op_param->output == 0)
    {
        output = inputImage;
    }
    else
    {
        output = outputImage;
    }

    int len = sizeof(int16_t) * op_param->size;

    /* Buffer indexed by 'shortcut_index' save output of the layer indexed by 'layer_num' */

    if (shortcut_buf[op_param->shortcut_index] == NULL)
    {
        shortcut_buf[op_param->shortcut_index] = malloc_ext(0, len);
    }

    if (shortcut_buf[op_param->shortcut_index] == NULL)
    {
        printf("malloc_ext error\n");
    }
    memset(shortcut_buf[op_param->shortcut_index], 0, len);
    memcpy(shortcut_buf[op_param->shortcut_index], output, op_param->size * sizeof(int16_t));

}

void soft_route(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage, int16_t **shortcut_buf)
{
    struct op_route_param *op_param = (struct op_route_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int i;
    int16_t *input;
    static int16_t *s_route_buffer = NULL; /*< Buffer for route layer */
    int16_t *input_base;

    /* Select input buffer */
    if (op_param->buffer == 0)
    {
        input = inputImage;
    }
    else
    {
        input = outputImage;
    }
    input_base = input;

    /* Copy data from shorcut buffers into input buffer */
    for (i = 0; i < op_param->num; i++)
    {

        if (op_param->num == 1)
        {
            if (s_route_buffer == NULL)
            {
                s_route_buffer = malloc_ext(0, op_param->sizes[i] * sizeof(int16_t) * 2);
            }
            if (s_route_buffer == NULL)
            {
                printf("g_route_buffer error\n");
            }

            memcpy(s_route_buffer, shortcut_buf[0], op_param->sizes[i] * sizeof(int16_t) * 2);
        }

        if (i == 0)
        {
            memcpy(input, shortcut_buf[op_param->buffers[i]], op_param->sizes[i] * sizeof(int16_t));
        }
        else
        {
            memcpy(input, s_route_buffer, op_param->sizes[i] * sizeof(int16_t));
        }

        input += op_param->sizes[i];
    }
}

void kdp_print_bank_op(int code)
{
#if 0
    char *opers[] = {"BANK_CP_FROM_MEM", "BANK_CP_TO_MEM", "BANK_CP_TO_BANK", "BANK_ADD_FROM_MEM",
                     "BANK_ADD_TO_MEM", "BANK_PADDING", "BANK_MAXPOOL", "BANK_ADD_BOTH",
                     "BANK_COPY_CHANNEL", "BANK_EXCHANGE_CHANNEL",
                    };
    if (code < sizeof(opers) / sizeof(char *))
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "UNKOWN");
    }
#endif
}

/* General bank copy operation, it shall replace all implementation before */
static void do_bank_op(struct bank_op_param *param, struct conv_hw_context *cxt, unsigned short *inputdat, unsigned short *outputdat)
{
    void *bank_addr, *bank_start_addr, *dst_bank_addr;
    int16_t val, *mem_addr;
    int32_t row_num, col_num, channel, row, col, mem_off;
    int32_t start_row, valid_len_row, valid_len_col, len_row, start_col, len_col, start_channel, len_channel, valid_row_off, valid_col_off;
    int32_t  activation = param->activation;
    int buffer = 0;

    row_num       = param->row_num;
    col_num       = param->col_num;
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    valid_len_row = param->valid_len_row;
    len_col       = param->len_col;
    start_channel = param->start_channel;
    start_row     = param->start_row;
    start_col     = param->start_col;
    valid_row_off = param->valid_start_row - param->start_row;
    valid_col_off = param->valid_start_col - param->start_col;

    buffer = param->buffer;

    if (param->bank == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    if (buffer == 0)
    {
        mem_addr = inputdat;
    }
    else
    {
        mem_addr = outputdat;
    }

    for (channel = 0; channel < len_channel; channel++)
    {
        /* Set bank pointer to channel start of bank */
        bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
        bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
        /* Set row pointer to channel start of shorcut buffer */
        mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;


        //if (param->op== BANK_CP_TO_MEM && (channel + start_channel) == 0 && start_row + valid_len_row > 26)
        //{
        //  printf("pp");
        //}
        /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
        switch (param->op)
        {
            case BANK_CP_FROM_MEM:
                for (row = start_row; row < 0;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = 0;
                    }
                    row++;
                    if (row < 0)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                }
                for (row = 0; row < valid_len_row;)
                {
                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Even row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                        /* Copy valid col */
                        for (col = valid_col_off; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = mem_addr[mem_off + col];
                        }
                        /* Padding right col */
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;

                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                        /* Copy valid col */
                        for (col = valid_col_off; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = mem_addr[mem_off + col_num + col];
                        }
                        /* Padding right col */
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;

                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
                break;
            case BANK_CP_TO_MEM:
                //int32_t valid_len_row = (len_row <= (row_num - start_row)) ? len_row : (row_num - start_row);
                valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);

                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        for (col = valid_col_off; col < valid_len_col; ++col)
                        {
                            mem_addr[mem_off + col] = ((int16_t *)bank_addr)[2 * col + 1];
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        for (col = valid_col_off; col < valid_len_col; ++col)
                        {
                            mem_addr[mem_off + col_num + col] = ((int16_t *)bank_addr)[2 * col];
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
                break;
            case BANK_ADD_TO_MEM: /* Used for input channel cut */
            {
#if 0
                int16_t *dst_mem_addr;
                int32_t dst_mem_off;

                /* Never write beyond valid range */
                valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
                dst_mem_addr   = cxt->layer_buffer[param->dst_buffer];
                dst_mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                /* TODO: Saturation add is necessary */
                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                dst_mem_addr[dst_mem_off + col]  = simu_relu_activate(val);
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                dst_mem_addr[dst_mem_off + col]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                dst_mem_addr[dst_mem_off + col_num + col]  = simu_relu_activate(val);
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                dst_mem_addr[dst_mem_off + col_num + col]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }

                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    dst_mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
#else
                while (1)
                {
                    printf("test\n");
                }

#endif
                break;
            }
            case BANK_ADD_FROM_MEM: /* Used for shortcut operation */
                /* TODO: Saturation add is necessary */
#if 0
                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                val = simu_relu_activate(val);
                                ((int16_t *)bank_addr)[2 * col + 1]  = val;
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                ((int16_t *)bank_addr)[2 * col + 1]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                ((int16_t *)bank_addr)[2 * col + 1] += mem_addr[mem_off + col];
                            }
                        }
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                ((int16_t *)bank_addr)[2 * col]  = simu_relu_activate(val);
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                ((int16_t *)bank_addr)[2 * col]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                ((int16_t *)bank_addr)[2 * col] += mem_addr[mem_off + col_num + col];
                            }
                        }
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
#else
                while (1)
                {
                    printf("testx\n");
                }
#endif
                break;
            case BANK_ADD_BOTH: /* Used for shortcut operation */
            {
#if 0
                int16_t *dst_mem_addr;
                int32_t dst_mem_off;

                valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
                dst_mem_addr   = cxt->layer_buffer[param->dst_buffer];
                dst_mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                /* TODO: Saturation add is necessary */
                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                val = simu_relu_activate(val);
                                ((int16_t *)bank_addr)[2 * col + 1]  = dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                val = simu_leaky_activate(val, 13);
                                ((int16_t *)bank_addr)[2 * col + 1]  = dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = ((int16_t *)bank_addr)[2 * col + 1] = mem_addr[mem_off + col];
                                ((int16_t *)bank_addr)[2 * col + 1]  = dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                        for (col = col_num - start_col; col < valid_len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                val = simu_relu_activate(val);
                                ((int16_t *)bank_addr)[2 * col]  = dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                val = simu_leaky_activate(val, 13);
                                ((int16_t *)bank_addr)[2 * col]  = dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = ((int16_t *)bank_addr)[2 * col] + mem_addr[mem_off + col_num + col];
                                ((int16_t *)bank_addr)[2 * col]  = dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    dst_mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
#else
                while (1)
                {
                    printf("testxx\n");
                }
#endif
                break;
            }
            case BANK_PADDING:
                for (row = start_row; row < 0;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = 0;
                    }
                    row++;
                    if (row < 0)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                }
                for (row = 0; row < valid_len_row;)
                {
                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Even row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                        /* Padding right col */
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;

                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                        /* Padding right col */
                        for (col = col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };

                break;
            default:
                break;
        }
    }
    //if (param->op == BANK_CP_TO_MEM && param->layer_num == 12 && param->start_channel + param->len_channel >= cxt->total_out_ch && param->start_row + param->valid_len_row >= param->row_num && param->start_col + param->len_col >= param->col_num)
    //{
    //  printf("debug\n");
    //}
}

static void do_channel_cp(struct bank_op_param *param, struct conv_hw_context *cxt, unsigned short *inputImage, unsigned short *outputImage)
{
    void *bank_addr, *bank_start_addr, *dst_bank_addr;
    int32_t row_num, col_num, channel, row, col;
    int32_t len_row, len_col, len_channel;
    uint32_t val;

    row_num       = param->row_num;
    col_num       = param->col_num;
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    len_col       = param->len_col;

    if (param->bank == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    for (channel = 0; channel < len_channel; channel++)
    {
        /* Set bank pointer to channel start of bank */
        bank_addr  = bank_start_addr + ((channel + param->src_channel_off) & 0x1) * 0x10000;
        bank_addr += ((channel + param->src_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;

        dst_bank_addr  = bank_start_addr + ((channel + param->dst_channel_off) & 0x1) * 0x10000;
        dst_bank_addr += ((channel + param->dst_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;

        /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
        switch (param->op)
        {
            case BANK_COPY_CHANNEL:
                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        ((uint32_t *)dst_bank_addr)[col] = ((uint32_t *)bank_addr)[col];
                    }

                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                        dst_bank_addr += 0x4000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                        dst_bank_addr += 0x4000;
                    }
                };
                break;
            case BANK_EXCHANGE_CHANNEL:
                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        val = ((uint32_t *)dst_bank_addr)[col];
                        ((uint32_t *)dst_bank_addr)[col] = ((uint32_t *)bank_addr)[col];
                        ((uint32_t *)bank_addr)[col] = val;
                    }

                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                        dst_bank_addr += 0x4000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                        dst_bank_addr += 0x4000;
                    }
                };
                break;
            default:
                break;
        }
    }

}

static void do_bank_op_1x1(struct bank_op_param *param, struct conv_hw_context *cxt, kdp_processor_t *processor)
{
    void *bank_addr, *bank_start_addr, *dst_bank_addr;
    int16_t val, *mem_addr;
    int32_t row_num, col_num, channel, row, col, mem_off, ch_size;
    int32_t start_row, valid_len_row, valid_len_col, len_row, start_col, len_col, start_channel, len_channel, valid_row_off, valid_col_off;
    int32_t  activation = param->activation;

    row_num       = param->row_num;
    col_num       = param->col_num;
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    valid_len_row = param->valid_len_row;
    len_col       = param->len_col;
    start_channel = param->start_channel;
    start_row     = param->start_row;
    start_col     = param->start_col;
    valid_row_off = param->valid_start_row - param->start_row;
    valid_col_off = param->valid_start_col - param->start_col;

    if (param->buffer == 0)
    {
        mem_addr      = processor->network_input;
    }
    else
    {
        //mem_addr      = cxt->layer_buffer[param->buffer];
        mem_addr      = processor->network_output;
    }

    if (param->bank == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    for (channel = (((len_channel - 1) / 4) * 4); channel >= 0; channel -= 4)
    {
        /* Set bank pointer to channel start of bank , first 4 channels are all zero */
        bank_addr  = bank_start_addr + ((channel / 4 + 2) & 0x1) * 0x10000;
        bank_addr += ((channel / 4 + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;
        //bank_addr  = bank_start_addr + (((channel)/4)&0x1) * 0x10000;
        //bank_addr += (((channel)/4)/2) * len_col * ((len_row + 7)/8) * 4 * 4;
        /* Set row pointer to channel start of shorcut buffer */
        mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

        /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
        switch (param->op)
        {
            case BANK_CP_FROM_MEM:
                ch_size  = col_num * row_num;
                for (row = start_row; row < 0;)
                {
                    /* 2 rows and 2 cols */
                    for (col = 0; col < 2 * len_col; ++col)
                    {
                        ((uint32_t *)bank_addr)[col] = 0;
                    }
                    row++;
                }
                for (row = 0; row < valid_len_row;)
                {
                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < 2 * len_col; ++col)
                        {
                            ((uint32_t *)bank_addr)[col] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Even row */
                        /* Padding left col */
                        for (col = 0; col < -2 * start_col; ++col)
                        {
                            ((uint32_t *)bank_addr)[col] = 0;
                        }
                        /* Copy valid col */
                        for (col = valid_col_off; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[4 * col + 1] = mem_addr[mem_off + 0 * ch_size + col];
                            ((int16_t *)bank_addr)[4 * col + 3] = mem_addr[mem_off + 1 * ch_size + col];
                            ((int16_t *)bank_addr)[4 * col + 0] = mem_addr[mem_off + 2 * ch_size + col];
                            ((int16_t *)bank_addr)[4 * col + 2] = mem_addr[mem_off + 3 * ch_size + col];
                        }
                        /* Padding right col */
                        for (col = 2 * (col_num - start_col); col < 2 * len_col; ++col)
                        {
                            ((uint32_t *)bank_addr)[col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if (((2 * row) & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 8 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
                break;
            case BANK_CP_TO_BANK:
            {
                uint32_t val0, val2, val4, val6;

                ch_size    = len_col * ((len_row + 7) / 8);
                /* Set bank pointer to channel start of bank */
                /* first 8 channels are all zero */
                dst_bank_addr  = (param->bank == 0) ? BRAM_OUTPUT_A_BASEARDDR : BRAM_OUTPUT_C_BASEARDDR;
                dst_bank_addr += ((channel / 4 + 2) & 0x1) * 0x10000;
                dst_bank_addr += ((channel / 4 + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;

                bank_addr      = (param->bank == 0) ? BRAM_OUTPUT_A_BASEARDDR : BRAM_OUTPUT_C_BASEARDDR;
                bank_addr     += (channel & 0x1) * 0x10000;
                bank_addr     += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;

                /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; col++)
                    {
                        /* Get data from 2 rows, 1 col, 4 channels */
                        val0 = ((uint32_t *)bank_addr)[col + 0 * ch_size];
                        val2 = ((uint32_t *)(bank_addr + 0x10000))[col + 0 * ch_size];
                        val4 = ((uint32_t *)bank_addr)[col + 1 * ch_size];
                        val6 = ((uint32_t *)(bank_addr + 0x10000))[col + 1 * ch_size];

                        /* Copy data to  4 rows, 2 cols of 1 channel */
                        /* Dst row 0/1 and col0/1 are from src row 0 and col 0 of 4 channels (upper 16 bits of val0/2/4/6) */
                        ((uint32_t *)dst_bank_addr)[2 * col + 0] = (val0 & 0xFFFF0000) | (val4 >> 16);
                        ((uint32_t *)dst_bank_addr)[2 * col + 1] = (val2 & 0xFFFF0000) | (val6 >> 16);
                        /* Dst row 2/3 and col0/1 are from src row 1 and col 0 of 4 channels (lower 16 bits of val0/2/4/6)*/
                        ((uint32_t *)(dst_bank_addr + 0x4000))[2 * col + 0] = ((val0 & 0xFFFF) << 16) | (val4 & 0xFFFF);
                        ((uint32_t *)(dst_bank_addr + 0x4000))[2 * col + 1] = ((val2 & 0xFFFF) << 16) | (val6 & 0xFFFF);
                    }
                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }

                    /* Check if next row return to old bank */
                    if (((2 * row) & 0x7) == 0)
                    {
                        dst_bank_addr += 0x8000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 8 * len_col;
                    }
                    else
                    {
                        dst_bank_addr += 0x8000;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

static void do_channel_cp_1x1(struct bank_op_param *param, struct conv_hw_context *cxt, int32_t len_chanel, int32_t channel_off)
{
    void *bank_addr, *bank_start_addr, *dst_bank_addr;
    int32_t row, col;
    int32_t len_row, len_col, dst_off, ch_off, col_off, row_off;
    uint32_t val;

    len_row       = param->len_row;
    len_col       = param->len_col;

    if (param->bank == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }


    /* Set bank pointer to channel start of bank */
    bank_addr = bank_start_addr;

    /* Get row off and col off in one 3x3 block (combine data from every 9 channels) */
    /* len_channel shall be multiple of 9, channel_off starts from final channel */
    dst_off   = channel_off / 4;
    ch_off    = (channel_off) % 4;
    col_off   = ch_off % 2;
    row_off   = ch_off / 2;

    dst_bank_addr  = (param->bank == 0) ? BRAM_OUTPUT_A_BASEARDDR : BRAM_OUTPUT_C_BASEARDDR;
    dst_bank_addr += ((dst_off + 2) & 0x1) * 0x10000;
    dst_bank_addr += ((dst_off + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;

    /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
    for (row = 0; row < len_row;)
    {
        for (col = 0; col < len_col; col++)
        {
            val = ((uint32_t *)bank_addr)[col];
            ((uint16_t *)dst_bank_addr)[4 * col + 2 * col_off + ((row_off == 1) ? 0 : 1)] = (val >> 16);
            ((uint16_t *)(dst_bank_addr + 0x4000))[4 * col + 2 * col_off + ((row_off == 1) ? 0 : 1)] = (val & 0xFFFF);
        }
        row += 2;

        /* Increase 2 rows to pointer to bank */
        if ((row & 0x7) == 0)
        {
            bank_addr += 0x4000;
            bank_addr -= 0x10000;
            bank_addr += 4 * len_col;
        }
        else
        {
            bank_addr += 0x4000;
        }

        /* Check if next row return to old bank */
        if (((2 * row) & 0x7) == 0)
        {
            dst_bank_addr += 0x8000;
            dst_bank_addr -= 0x10000;
            dst_bank_addr += 8 * len_col;
        }
        else
        {
            dst_bank_addr += 0x8000;
        }
    }

}

static void bank_depthwise(struct easynet_ops_param *param, struct conv_hw_context *cxt, kdp_processor_t *processor, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_bank_depthwise_param *op_param = (struct op_bank_depthwise_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    struct bank_op_param *in_bank_param = &op_param->in_bank_param;
    struct bank_op_param *bank_param = &op_param->out_bank_param;
    struct op_hardware_param *hw_param = &op_param->hw_param;
    uint32_t len_channel, start_channel, in_bank, out_bank, recover_in_bank, recover_out_bank;
    uint32_t op_idx, op;
    int32_t ch;
    uint32_t weight_offset, one_weight_size;
    int i;
    uint32_t recover_out_bank_param_start_channel = 0, recover_out_bank_param_len_channel, rcv_out_bank_param_bank;
    uint32_t rcv_in_bank_param_op;
    uint32_t rcv_weight_offset = 0;
    len_channel = in_bank_param->len_channel;
    start_channel = in_bank_param->start_channel;
    weight_offset = hw_param->weight_offset;
    rcv_weight_offset = hw_param->weight_offset;
    one_weight_size = hw_param->weight_len / len_channel;
    hw_param->weight_offset = weight_offset + (len_channel - 1) * one_weight_size;
    in_bank  = in_bank_param->bank;
    recover_in_bank = in_bank;
    recover_out_bank_param_len_channel = bank_param->len_channel;
    recover_out_bank_param_start_channel = bank_param->start_channel;

    uint32_t rcv_in_bank_param_start_channel =  in_bank_param->start_channel;
    uint32_t rcv_in_bank_param_len_channel = in_bank_param->len_channel;

    uint32_t recover_out_bank_param_op = bank_param->op;
    uint32_t rcv_in_bank_param_src_channel_off = in_bank_param->src_channel_off;
    uint32_t rcv_in_bank_param_dst_channel_off =  in_bank_param->dst_channel_off;
    uint32_t  rcv_out_bank_param_src_channel_off = bank_param->src_channel_off;
    uint32_t  rcv_out_bank_param_dst_channel_off = bank_param->dst_channel_off;

    rcv_out_bank_param_bank = bank_param->bank;
    rcv_in_bank_param_op = in_bank_param->op;
    out_bank = ((in_bank_param->bank == 0) ? 1 : 0);


    for (ch = len_channel - 1; ch >= 0; ch--)
    {

#if 0
        extern int cntt;
        if (cntt == 214 && ch == 0) // 182 error
        {
            printf("\n==========cntt=[%d]\n", cntt);
        }
#endif

        /* Copy one channel data to input start */
        in_bank_param->start_channel = start_channel + ch;
        in_bank_param->len_channel   = 1;
        if (in_bank_param->buffer != INVALID_BUFFER_IDX)
        {
            in_bank_param->op = BANK_CP_FROM_MEM;
            in_bank_param->bank = in_bank;
            HAL_CNN_Bram_hfwd_rw_en(DISABLE);
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
            do_bank_op(in_bank_param, cxt, inputImage, outputImage);
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }
        else
        {
            if (ch == len_channel - 1 || ch == 0)
            {
                in_bank_param->op = BANK_EXCHANGE_CHANNEL;
                in_bank_param->src_channel_off = len_channel - 1;
                in_bank_param->dst_channel_off = 0;
            }
            else
            {
                in_bank_param->op = BANK_COPY_CHANNEL;
                in_bank_param->src_channel_off = ch;
                in_bank_param->dst_channel_off = 0;
            }
            HAL_CNN_Bram_hfwd_rw_en(DISABLE);
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
            do_channel_cp(in_bank_param, cxt, inputImage, outputImage);
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);

        }

        /* I do it for simulation debug */
        cxt->start_out_row = bank_param->start_row;
        cxt->start_out_col = bank_param->start_col;
        cxt->simu_start_out_ch = ch;

        HAL_DMA_Send(hw_param->weight_len, processor->weight_addr_start + hw_param->weight_offset); // stream fifo  address must 32 byte aliged
        put_cmd(processor->cmd_addr_start, hw_param->cmd_offset, hw_param->cmd_len);
        HAL_CNN_Start();
        HAL_CNN_Wait();

        /* Save shortcut buffer if it is not last one of this combined layer, otherwise we do it in after_bank_op */
        if (op_param->referred_index >= 0 && op_param->referred_layer_num ==  op_param->bank_op.before_layer_number)
        {
            bank_param->op  =  BANK_CP_TO_MEM;
            bank_param->buffer =  op_param->referred_index;
            HAL_CNN_Bram_hfwd_rw_en(DISABLE);
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
            do_bank_op(bank_param, cxt, inputImage, outputImage);
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        for (op_idx = 0; op_idx < BANK_OP_MAX_STAGE; op_idx++)
        {
            op = BANK_OP_STAGE(op_param->bank_op.stage, op_idx);
            if (op == 0)
            {
                break;
            }

            switch (op)
            {
                case BANK_OP_MAXPOOL:
                {
                    bank_param->op = BANK_MAXPOOL;
                    bank_param->pool_size   = op_param->bank_op.pool_size;
                    bank_param->pool_stride = op_param->bank_op.pool_stride;
                    bank_param->pool_pad_up = op_param->bank_op.pool_pad_up;
                    bank_param->pool_pad_left = op_param->bank_op.pool_pad_left;
                    bank_param->pool_out_col  = op_param->bank_op.hardware_out_col;
                    bank_param->pool_out_row  = op_param->bank_op.hardware_out_row;
                    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
                    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
                    do_bank_op(bank_param, cxt, inputImage, outputImage);
                    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
                    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
                    break;
                }
                case BANK_OP_SHORTCUT:
                {
                    bank_param->op = BANK_ADD_TO_MEM;
                    bank_param->dst_buffer = op_param->output_idx;
                    bank_param->buffer = op_param->shortcut_index;
                    bank_param->activation = op_param->bank_op.activation;
                    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
                    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
                    do_bank_op(bank_param, cxt, inputImage, outputImage);
                    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
                    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
                    break;
                }
                default:
                    break;
            }

            /* Save bank op result if referred by other layer except shortcut layer */
            if (op != BANK_OP_SHORTCUT && op_param->referred_index >= 0 && op_param->referred_layer_num ==  op_param->bank_op.stage_layer_num[op_idx])
            {
                bank_param->op     =  BANK_CP_TO_MEM;
                bank_param->buffer =  op_param->referred_index;
                HAL_CNN_Bram_hfwd_rw_en(DISABLE);
                HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
                do_bank_op(bank_param, cxt, inputImage, outputImage);
                HAL_CNN_Bram_hfwd_rw_en(ENABLE);
                HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
            }
        }

        /* Padding zero to end of cols to go round hardware bug */
        if (op_param->bank_op.pool_size != 1)
        {
            bank_param->start_col  = op_param->bank_op.hardware_start_out_col;    /* Start col in out bank */
            bank_param->len_col    = op_param->bank_op.hardware_out_col;          /* Cols in out bank */
        }

        if (bank_param->start_col + bank_param->len_col > bank_param->col_num)
        {
            bank_param->op     =  BANK_PADDING;
            HAL_CNN_Bram_hfwd_rw_en(DISABLE);
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
            do_bank_op(bank_param, cxt, inputImage, outputImage);
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        /* Copy one channel data from output start */
        bank_param->start_channel = start_channel + ch;
        bank_param->len_channel   = 1;
        if (bank_param->buffer != INVALID_BUFFER_IDX)
        {
            bank_param->op = BANK_CP_TO_MEM;
            bank_param->bank = out_bank;
            HAL_CNN_Bram_hfwd_rw_en(DISABLE);
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
            do_bank_op(bank_param, cxt, inputImage, outputImage);
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }
        else
        {
            bank_param->op = BANK_COPY_CHANNEL;
            bank_param->src_channel_off = 0;
            bank_param->dst_channel_off = ch;
            HAL_CNN_Bram_hfwd_rw_en(DISABLE);
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
            if (op_param->next_1x1 == 1)
            {
                do_channel_cp_1x1(bank_param, cxt, 1, ch);
            }
            else
            {
                do_channel_cp(bank_param, cxt, inputImage, outputImage);
            }
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        /* Do calculation backwards */
        hw_param->weight_offset -= one_weight_size;
    }

    {
        in_bank_param->bank = recover_in_bank;
        bank_param->op = recover_out_bank_param_op;
        bank_param->start_channel = recover_out_bank_param_start_channel;
        bank_param->len_channel  = recover_out_bank_param_len_channel;
        bank_param->bank = rcv_out_bank_param_bank ;
        in_bank_param->op = rcv_in_bank_param_op;
        in_bank_param->start_channel = rcv_in_bank_param_start_channel;
        in_bank_param->len_channel = rcv_in_bank_param_len_channel;
        in_bank_param->src_channel_off = rcv_in_bank_param_src_channel_off;
        in_bank_param->dst_channel_off = rcv_in_bank_param_dst_channel_off;

        bank_param->src_channel_off  = rcv_out_bank_param_src_channel_off;
        bank_param->dst_channel_off =  rcv_out_bank_param_dst_channel_off;
    }

    hw_param->weight_offset = weight_offset;
}


int bank_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param, kdp_processor_t *processor)
{
    int ret = TRUE;
    struct conv_hw_context *cxt = ((struct conv_hw_context *)dev->context);
#ifdef DEBUG_TIME
    uint32_t time_last = 0, time_now = 0;
    time_last = k_uptime_get_32();
#endif

    switch (param->op)
    {
        case OP_CROSS_WRITEOUT:
            while (1)
            {
                printf("OP_CROSS_WRITEOUT\n");
            }
            //CrossGetData(param, cxt);
            break;
        case OP_CROSS_DATAFETCH:
            CrossPutData(param, cxt, processor->network_input, processor->network_output, dev->input);
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_CrossPut += time_now - time_last;
#endif
            break;
        case OP_BANK_ROUTE:
            while (1)
            {
                printf("OP_BANK_ROUTE\n");
            }
            //bank_route(param, cxt);
            break;
        case OP_BANK_GENERAL:
        {
            struct bank_op_param *op_param = (struct bank_op_param *)(((void *)param) + sizeof(struct easynet_ops_param));

            if (op_param->op == BANK_MAXPOOL)
            {
                while (1)
                {
                    printf("BANK_MAXPOOL\n");
                }
                // do_bank_maxpool(op_param, cxt);
            }
            else
            {

                HAL_CNN_Bram_hfwd_rw_en(DISABLE);
                HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
                kdp_print_bank_op(op_param->op);
                if ((op_param->op == BANK_CP_FROM_MEM || op_param->op == BANK_CP_TO_BANK) && \
                        op_param->next_1x1 == 1)
                {
                    do_bank_op_1x1(op_param, cxt, processor);
                }
                else
                {
                    do_bank_op(op_param, cxt, processor->network_input, processor->network_output);
                }
                HAL_CNN_Bram_hfwd_rw_en(ENABLE);
                HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
                if (op_param->buffer == 0)
                {
                    dev->predict_out = processor->network_input;
                }
                else
                {
                    dev->predict_out = processor->network_output;
                }

            }
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_BankGeneral += time_now - time_last;
#endif
            break;
        }
        case OP_DEPTHWISE_CONV:
            //printf("before OP_DEPTHWISE_CONV\n");
            bank_depthwise(param, cxt, processor, processor->network_input,  processor->network_output);
            //printf("After OP_DEPTHWISE_CONV\n");
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_dwcon += time_now - time_last;
#endif
            break;
        default:
            break;
    }

    return ret;
}

int soft_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param, kdp_processor_t *processor)
{
    int ret = TRUE;
    struct conv_hw_context *cxt = ((struct conv_hw_context *)dev->context);

    switch (param->op)
    {
        case OP_MAXPOOL:

            printf("OP_MAXPOOL warning line=%d\n", __LINE__);

            //soft_maxpool(param, cxt);
            break;
        case OP_AVGPOOL:
            //AveragePooling(param, cxt);
            break;
        case OP_CONNECTED:
            //FC(fcArray);
            break;
        case OP_WRITEOUT:
            GetData(param, cxt, processor->network_input, processor->network_output);
            break;
        case OP_DATAFETCH:
            PutData(param, cxt, processor->network_input, processor->network_output);
            break;
        case OP_INPUT:
            printf("OP_INPUT warning line=%d\n", __LINE__);
            break;
        case OP_SWITCH_INOUT:

            printf("OP_SWITCH_INOUT line=%d\n", __LINE__);

            //moveToInput(param, cxt);
            break;
        case OP_LAYER:

            layerNumber(param);
            break;
        case OP_SHORTCUT:
            while (1)
            {
                printf("Xhello world\n");
            }
            //soft_shortcut(param, cxt);
            break;
        case OP_ROUTE:
            while (1)
            {
                printf("QQhello world\n");
            }
            //soft_route(param, cxt);
            break;
        case OP_BUFFER:
            while (1)
            {
                printf("RRhello world\n");
            }
            //shortcut_buffer(param, cxt);
            break;
        case OP_ACTIVATION:
            while (1)
            {
                printf("hello world\n");
            }
            //soft_activation(param, cxt);
            break;
        case OP_OUTPUT:

            //npu_output(param, cxt);
            break;
        case OP_YOLO:
            while (1)
            {
                printf("GGhello world\n");
            }
            //soft_yolo(param, cxt);
            break;
        case OP_UPSAMPLE:
            while (1)
            {
                printf("HHhello world\n");
            }
            //soft_upsample(param, cxt);
            break;
        default:
            bank_ops_process(dev, param, processor);
            break;
    }

    return ret;
}


int cntt = 0;

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param)
{
    int ret = 1;
    struct op_hardware_param *op_param = (struct op_hardware_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    kdp_processor_t *processor = &kdprocessor;

#ifdef DEBUG_TIME
    uint32_t time_last = 0, time_now = 0;
    time_last = k_uptime_get_32();
#endif

#if 0
    printf("\n==========cntt=[%d]\n", cntt);
    if (cntt == 302)
    {
        printf("\n==========cntt=[%d]\n", cntt);
    }
#endif

    //#ifdef DEBUG_KDP
    kdp_print_op(param->op);
    //#endif
    switch (param->op)
    {
        case OP_HW:
            HAL_DMA_Send(op_param->weight_len, processor->weight_addr_start + op_param->weight_offset); // stream fifo  address must 32 byte aliged
            put_cmd(processor->cmd_addr_start, op_param->cmd_offset, op_param->cmd_len);
            HAL_CNN_Start();
            HAL_CNN_Wait();
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_Hw += time_now - time_last;
#endif
            break;

#ifdef DEBUG_KDP
            layerNumber(param);
            break;
#endif

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


    param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);
    /* If it is last operator */
    if (param->len == 0)
    {
        return RET_OVER;
    }

    return RET_ONGOING;
}

