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

static int reg_read(unsigned int addr)
{
    return HW32_REG(addr);
}
/*
#define DEBUG_TIME 0
#define DEBUG_KDP  0
*/
//#define DEBUG_KDP  1


uint32_t t_Hw = 0, t_MaxPool = 0, t_Layer = 0, t_AvgPool = 0, t_WriteOut = 0, t_DataFetch = 0, t_Input = 0;
int layer_cnt  = 0;

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

struct easynet_dev *easynet_process_init(char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

    len = ops_parse(ops_data, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

struct easynet_dev *easynet_process_update(unsigned char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

    len = ops_parse(ops_data, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

void easynet_process_deinit(struct easynet_dev *dev)
{
    int i = 0;

    for (i = 0; i < MAX_SHORTCUT_NUM; i++)
    {
        free_ext(0, dev->shortcut_buf[i]);
        dev->shortcut_buf[i] = NULL;
    }

    for (i = 0; i < 2; i++)
    {
        free_ext(0, dev->yolo_out_buffer[i]);
        dev->yolo_out_buffer[i] = NULL;
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

void pre_processingFaceAI(struct easynet_ops_param *param, unsigned char *image_input_addr, unsigned short int *inputImage)
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

/*
KDP hardware Pool Size only support 2x2 and 3x3  and Stride=2
So, YOLO   must software impl when  Stride=1
*/
void maxPoolOptimizeBank2Bank(struct easynet_ops_param *param)
{
    struct op_maxpool_param *op_param = (struct op_maxpool_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t ch_num, row_num, col_num, out_row, out_col, kernel_size, stride_size;
    int32_t pad;
    int32_t ch, row, col, n, m;

    uint32_t buffer    = op_param->buffer;
    uint32_t bank_start_addr;
    int channel = 0;
    unsigned short int *curBuffer;

    ch_num  = op_param->ch_num;
    row_num = op_param->row_num;
    col_num = op_param->col_num;
    out_row = op_param->out_row;
    out_col = op_param->out_col;
    kernel_size = op_param->pool_size;
    stride_size = op_param->pool_stride;

    if (buffer == 0)
    {
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        bank_start_addr = BRAM_OUTPUT_C_BASEARDDR;
    }

    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    if (stride_size == 1 && kernel_size == 2)
    {
        //R0: Bank start address, R3: Input Cols, R12: Input rows
        //R0: Bank start address, R3: Input Cols, R2: Input rows(SET TO R12), R1:Channel nums
        for (channel = 0; channel < ch_num; ++channel)
        {
            curBuffer = bank_start_addr + 0x10000 * (channel % 2) + (channel / 2) * 4 * col_num * ((row_num + 7) / 8);
            //FastMaxPool_2X2_S1(curBuffer, 7, 7); // must fixed
        }
    }
    else
    {
        printf("soft maxpool not supprot para\n");
    }

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);

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

static void PutDataFaceAi(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
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
                                HW32_REG(BANK_A_ADDRESS + i * 0x10000 + j * 0x4000 + p * 4) = data;
                            }
                            else
                            {
                                HW32_REG(BANK_C_ADDRESS + i * 0x10000 + j * 0x4000 + p * 4) = data ;
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

void PutData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
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

#if 1
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
static void GetDataFaceAi(struct easynet_ops_param *param, int16_t *inputImage, int16_t *outputImage)
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
static void kdp_print_op(int code)
{
    char *opers[] = {"Hw", "Wait", "MaxPool", "AvgPool", "Connected", "WriteOut", "DataFetch", "Input", "Switch",
                     "Layer", "Buffer", "Shortcut", "Route", "Activation", "Output", "CROSSWRITEOUT", "CROSS_DATAFETCH", "BANK_MAXPOOL", "Yolo", "Upsample"
                    };

    if (code < sizeof(opers) / sizeof(char *))
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "default");
    }
}

void GetData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
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
                            for(col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                              ((uint32_t *)bank_addr)[0] = input[col + row_off] << 16 | input[col + row_offb];
                            }
                           // bank_addr = FastFirstPutDataOptimize(bank_addr, len_col, &input[start_col + row_off], &input[start_col + row_offb]);

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

void FirstPutData(struct easynet_ops_param *param, unsigned char *first_imginput)
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
    buffer    = op_param->buffer;

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

                        }
                    }
                }
            }
        }
    }

}

void CrossPutDataCompilerOptimize(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, row_offb, ch_off;
    uint16_t *input;

    void *bank_start_addr, *bank_addr;

    if (op_param->layer_num == 0)
    {
        CrossFirstPutDataCompilerOptimize(param, first_imginput);
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


    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    if (buffer == 0)
    {
        input = inputImage;
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        input = outputImage;
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
                        if ((row + bank_row_idx + start_row) < row_num)
                        {
                            if ((row + bank_row_idx + start_row + 1) < row_num)
                            {
                                  for(col = start_col; col < end_col; ++col, bank_addr += 4)
                                  {
                                      ((uint32_t *)bank_addr)[0] =(input[col + row_off] << 16) | (input[col + row_offb]);
                                    }

                                //bank_addr = FastShortPutDataOptimize(bank_addr, len_col, &input[start_col + row_off], &input[start_col + row_offb]);
                            }
                            else
                            {
                                for (col = start_col; col < end_col; ++col, bank_addr += 4)
                                {
                                    ((uint32_t *)bank_addr)[0] = input[col + row_off] << 16;
                                }
                            }
                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                ((uint32_t *)bank_addr)[0] = 0;
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

void CrossPutDataCompilerNormer(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, row_offb, ch_off;
    uint16_t *input;
    void *bank_start_addr, *bank_addr;

    if (op_param->layer_num == 0)
    {
        CrossFirstPutDataCompilerNormer(param, first_imginput);
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

    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

    if (buffer == 0)
    {
        input = inputImage;
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        input = outputImage;
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
                        row_offb =  ch_off + (row + bank_row_idx + start_row) * col_num;

                        end_col = start_col + len_col;
                        if ((row + bank_row_idx + start_row) < row_num)
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                ((uint16_t *)bank_addr)[0] = input[col + row_off];
                            }

                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                ((uint16_t *)bank_addr)[0] = 0;
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

void CrossPutData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, ch_off;
    int16_t *input;
    uint32_t bank_start_addr, bank_addr;

    if (op_param->layer_num == 0)
    {
        FirstPutData(param, first_imginput);
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

    if (buffer == 0)
    {
        input = inputImage;
        bank_start_addr = BRAM_OUTPUT_A_BASEARDDR;
    }
    else
    {
        input = outputImage;
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

    printf("\n====layerNumber op_param->conv = %d\n", op_param->conv);

    ++layer_cnt;

    if (layer_cnt == 36)
    {
        printf("dump\n");
    }

    printf("\n====layerNumber layer = %d\n", layer_cnt);
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

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param)
{
    int ret = 1;
    struct op_hardware_param *op_param = (struct op_hardware_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    kdp_processor_t *processor = &kdprocessor;

#ifdef DEBUG_TIME
    uint32_t time_last = 0, time_now = 0;
    time_last = k_uptime_get_32();
#endif

#ifdef DEBUG_KDP
    kdp_print_op(param->op);
#endif
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
        case OP_WAIT:
            break;
        case OP_MAXPOOL:

            if (dev->network_type == TINYYOLOV3)
            {
                soft_maxpool(param, processor->network_input, processor->network_output);
            }
            else
            {
                maxPool(param, processor->network_input);
            }
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_MaxPool += time_now - time_last;
#endif
            break;
        case OP_LAYER:
#ifdef DEBUG_KDP
            layerNumber(param);
#endif
            break;

        case OP_ROUTE:
            soft_route(param, processor->network_input, processor->network_output, dev->shortcut_buf);
            break;

        case OP_BUFFER:
            shortcut_buffer(param, processor->network_input, processor->network_output, dev->shortcut_buf);
            break;

        case OP_YOLO:
            soft_yolo(param, processor->network_input, processor->network_output, dev->yolo_out_buffer);
            break;

        case OP_UPSAMPLE:
            soft_upsample(param, processor->network_input, processor->network_output);
            break;

        case OP_AVGPOOL:
            break;
        case OP_CONNECTED:
            break;
        case OP_WRITEOUT:
        {
            struct op_writeout_param *op_param = (struct op_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
            if (op_param->buffer == 0)
            {
                dev->predict_out = processor->network_input;
            }
            else
            {
                dev->predict_out = processor->network_output;
            }
            if (dev->isfaceai == 1)
            {
                GetDataFaceAi(param, processor->network_input, processor->network_output);
            }
            else
            {
                GetData(param, processor->network_input, processor->network_output);
            }

#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_WriteOut += time_now - time_last;
#endif
        }
        break;
        case OP_DATAFETCH:
            if (dev->isfaceai == 1)
            {
                PutDataFaceAi(param, processor->network_input, processor->network_output);
            }
            else
            {
                PutData(param, processor->network_input, processor->network_output);
            }

#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_DataFetch += time_now - time_last;
#endif
            break;
        case OP_INPUT:
            if (dev->isfaceai == 1)
            {
                pre_processingFaceAI(param, dev->input, processor->network_input);
            }
            else
            {
                pre_processing(param, dev->input, processor->network_input);
            }

#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_Input += time_now - time_last;
#endif
            break;
        case OP_CROSS_WRITEOUT:
        {
            struct op_cross_writeout_param *op_cross_param = (struct op_cross_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
            if (op_cross_param->buffer == 0)
            {
                dev->predict_out = processor->network_input;
            }
            else
            {
                dev->predict_out = processor->network_output;
            }

            CrossGetData(param, processor->network_input, processor->network_output); /*CrossGetDataCompiler*/
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_WriteOut += time_now - time_last;
#endif
        }
        break;
        case OP_CROSS_DATAFETCH:
            CrossPutDataCompilerOptimize(param, processor->network_input, processor->network_output, dev->input); /*CrossPutDataCompilerNormer*/
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_DataFetch += time_now - time_last;
#endif
            break;
        case OP_SWITCH_INOUT:
            break;

        case OP_BANK_MAXPOOL:
            maxPoolOptimizeBank2Bank(param);
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_MaxPool += time_now - time_last;
#endif
            break;

        default:
            break;
    }

    return ret;

}

int easynet_ops_process(struct easynet_dev *dev)
{
    int ret;
    struct easynet_ops_param *param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);// + sizeof(struct easynet_ops_param);

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
