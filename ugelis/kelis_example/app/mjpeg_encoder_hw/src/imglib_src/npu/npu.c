/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        npu.c
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
 *              2020/03/13, Wen.Liu, v1.0.0
 *                  Move some low-level function here.
 *              2020/03/26, Wen.Liu, v1.0.0
 *                  Move to image lib.
 *
 **/


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "npu.h"
#include "image.h"

#define CMDGEN_PRINTF(...)

static uint32_t g_hw_cmd_num = 0;
static char binary_string[64];

static char *bin_str(uint32_t d, int num)
{
    int i, j;

    for (j = 0, i = num; (i > 0) && (j < 63); i--)
    {
        if ((d >> (i - 1)) & 0x1)
        {
            binary_string[j++] = '1';
        }
        else
        {
            binary_string[j++] = '0';
        }
    }

    binary_string[j] = 0;

    return &binary_string[0];
}

static uint8_t init_0(void *cmd_addr, uint32_t pool_mode, uint32_t bank_sel_layer, uint32_t col_num_rd)
{
    struct init0_cmd *cmd = (struct init0_cmd *)cmd_addr;

    cmd->col_num_rd = col_num_rd;
    cmd->bank_sel_layer = bank_sel_layer;
    cmd->pool_mode = pool_mode;
    cmd->cmd = 0;

    CMDGEN_PRINTF("//INIT0: cmdHeader_poolMode_bankSelLayer_colNumRd\n");
    CMDGEN_PRINTF("// poolMode=%d bankSelLayer=%d colNumRd=%d\n", pool_mode, bank_sel_layer, col_num_rd);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000000_%d_%d_%s;\n", g_hw_cmd_num++, cmd->pool_mode, cmd->bank_sel_layer, bin_str(cmd->col_num_rd, 8));

    return 2;
}

static uint8_t init_1(void *cmd_addr, uint32_t row_frame_num, uint32_t col_num)
{
    struct init1_cmd *cmd = (struct init1_cmd *)cmd_addr;

    cmd->col_num = col_num;
    cmd->row_frame_num = row_frame_num;

    CMDGEN_PRINTF("//INIT1: rowFrameNum_colNum\n");
    CMDGEN_PRINTF("// rowFrameNum=%d colNum=%d\n", row_frame_num, col_num);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b%s", g_hw_cmd_num++, bin_str(cmd->row_frame_num, 8));
    CMDGEN_PRINTF("_%s;\n", bin_str(cmd->col_num, 8));
    return 2;
}

static uint8_t init_2(void *cmd_addr, uint32_t channel_num, uint32_t stride, uint32_t conv_mode, uint32_t pool_size, uint32_t relu_en)
{
    struct init2_cmd *cmd = (struct init2_cmd *)cmd_addr;
    uint16_t pool_size_bin, stride_bin;

    /* set conv stride as conv stride - 1 */
    if (stride == 2)
    {
        stride_bin = 1;
    }
    else if (stride == 3)
    {
        stride_bin = 2;
    }
    else
    {
        stride_bin = 0;
    }

    /* Only support 2x2 max pool */
    if (pool_size == 2)
    {
        pool_size_bin = 1;
    }
    else
    {
        pool_size_bin = 0;
    }

    cmd->relu_en = relu_en;
    cmd->pool_size = pool_size_bin;
    cmd->conv_mode = conv_mode;
    cmd->stride = stride_bin;
    cmd->channel_num = (channel_num + 1) / 2; /* Only even channel supported , set input channel num as even channel /2 */

    CMDGEN_PRINTF("//INIT2: channelNum_stride_convMode_poolSize_reluEN\n");
    CMDGEN_PRINTF("// channel_num=%d+1/2 stride=%d convMode=%d poolSize=%d reluEN=%d\n", \
                  channel_num, stride, conv_mode, pool_size_bin, relu_en);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b%s", g_hw_cmd_num++, bin_str(cmd->channel_num, 10));
    CMDGEN_PRINTF("_%s_%d", bin_str(cmd->stride, 2), cmd->conv_mode);
    CMDGEN_PRINTF("_%s_%d;\n", bin_str(cmd->pool_size, 2), cmd->relu_en);

    return 2;
}

static uint8_t init_3(void *cmd_addr, uint32_t up_zero, uint32_t left_zero, uint32_t bottom_zero, uint32_t right_zero)
{
    struct init3_cmd *cmd = (struct init3_cmd *)cmd_addr;

    cmd->right_zero = right_zero;
    cmd->bottom_zero = bottom_zero;
    cmd->left_zero = left_zero;
    cmd->up_zero = up_zero;

    CMDGEN_PRINTF("//INIT3: UP_LEFT_BOTTOM_RIGHT\n");
    CMDGEN_PRINTF("//CLEAR ZERO UP=%d LEFT=%d BOTTOM=%d RIGHT=%d\n", up_zero, left_zero, bottom_zero, right_zero);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b%s", g_hw_cmd_num++, bin_str(cmd->up_zero, 4));
    CMDGEN_PRINTF("_%s", bin_str(cmd->left_zero, 4));
    CMDGEN_PRINTF("_%s", bin_str(cmd->bottom_zero, 4));
    CMDGEN_PRINTF("_%s;\n", bin_str(cmd->right_zero, 4));

    return 2;
}

static uint8_t init_4(void *cmd_addr, uint32_t relu_num, uint32_t pad_up, uint32_t pad_left, uint32_t pad_bottom, uint32_t pad_right)
{
    struct init4_cmd *cmd = (struct init4_cmd *)cmd_addr;

    cmd->pad_right  = 0;
    cmd->pad_bottom = 0;
    cmd->pad_left = pad_left;
    cmd->pad_up = pad_up;
    cmd->relu_num = relu_num;
    CMDGEN_PRINTF("//INIT4: RELUNUM_UP_LEFT_BOTTOM_RIGHT\n");
    CMDGEN_PRINTF("//PAD ZERO: XXX=%d UP=%d LEFT=%d BOTTOM=%d RIGHT=%d\n", cmd->relu_num, cmd->pad_up, cmd->pad_left, cmd->pad_bottom, cmd->pad_right);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b%s",  g_hw_cmd_num++, bin_str(cmd->relu_num, 4));
    CMDGEN_PRINTF("_%s", bin_str(cmd->pad_up, 2));
    CMDGEN_PRINTF("_%s", bin_str(cmd->pad_left, 2));
    CMDGEN_PRINTF("_%s", bin_str(cmd->pad_bottom, 4));
    CMDGEN_PRINTF("_%s;\n", bin_str(cmd->pad_right, 4));

    return 2;
}

static uint8_t bias_fetch(void *cmd_addr, uint32_t addr_back_en, uint32_t sram_i_num_shift)
{
    struct bias_fetch_cmd *cmd = (struct bias_fetch_cmd *)cmd_addr;

    cmd->sram_i_num_shift = sram_i_num_shift;
    cmd->addr_back_en = addr_back_en;
    cmd->cmd = 0x0B;

    CMDGEN_PRINTF("// BIAS_FETCH: cmdHeader_addrBackEn_sramNumShift\n");
    CMDGEN_PRINTF("// addr_back_en=%d sram_i_num_shift=%d\n", addr_back_en, sram_i_num_shift);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b001011_%d_%s;\n", g_hw_cmd_num++, cmd->addr_back_en, bin_str(cmd->sram_i_num_shift, 9));

    return 2;
}

static uint8_t fifo_check(void *cmd_addr, uint32_t check_data_count)
{
    struct fifo_check_cmd *cmd = (struct fifo_check_cmd *)cmd_addr;

    cmd->check_data_count = check_data_count;
    cmd->cmd = 0x0C;

    CMDGEN_PRINTF("// FIFO_CHECK: cmdHeader_checkDataCount\n");
    CMDGEN_PRINTF("// checkDataCount=%d\n", check_data_count);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b001100_%s;\n", g_hw_cmd_num++, bin_str(cmd->check_data_count, 10));

    return 2;
}

static uint8_t conv(void *cmd_addr, uint32_t shift_addr_x, uint32_t shift_addr_y)
{
    struct conv_cmd *cmd = (struct conv_cmd *)cmd_addr;

    /* input: shift_addr_x and y are int instead of string */
    cmd->shift_addr_y = shift_addr_y;
    cmd->shift_addr_x = shift_addr_x;
    cmd->cmd = 0x1;

    CMDGEN_PRINTF("// CONV: cmdHeader_shiftAddrX_shiftAddrY\n");
    CMDGEN_PRINTF("// shift_addr_x=%d shift_addr_y=%d\n", shift_addr_x, shift_addr_y);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000001_%s", g_hw_cmd_num++, bin_str(cmd->shift_addr_x, 5));
    CMDGEN_PRINTF("_%s;\n", bin_str(cmd->shift_addr_y, 5));

    return 2;
}

static uint8_t nop(void *cmd_addr, uint32_t nop_after_conv)
{
    struct nop_cmd *cmd = (struct nop_cmd *)cmd_addr;

    cmd->nop_after_conv = nop_after_conv;      /* cycles of NOP */
    cmd->cmd = 0x3F;

    CMDGEN_PRINTF("// NOP after CONV: cmdHeader_nopAfterConv\n");
    CMDGEN_PRINTF("// nop_after_conv=%d\n", nop_after_conv);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b111111_%s;\n", g_hw_cmd_num++, bin_str(cmd->nop_after_conv, 10));

    return 2;
}

static uint8_t read_conv(void *cmd_addr, uint32_t bias_sel, uint32_t bank_sel_spad)
{
    struct read_conv_cmd *cmd = (struct read_conv_cmd *)cmd_addr;

    cmd->bank_sel_spad = bank_sel_spad;
    cmd->bias_sel = bias_sel;
    cmd->resv = 0;
    cmd->cmd = 0x04;

    CMDGEN_PRINTF("// READ_CONV: cmdHeader_reserved_biasSel_bankSelSpad\n");
    CMDGEN_PRINTF("// bias_sel=%d bank_sel_spad=%d\n", bias_sel, bank_sel_spad);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000100_00000000_%d_%d;\n", g_hw_cmd_num++, cmd->bias_sel, cmd->bank_sel_spad);

    return 2;
}

static uint8_t pool(void *cmd_addr, uint32_t bank_sel_spad)
{
    struct pool_cmd *cmd = (struct pool_cmd *)cmd_addr;

    cmd->bank_sel_spad = bank_sel_spad;
    cmd->resv = 0;
    cmd->cmd = 0x02;

    CMDGEN_PRINTF("// POOL feature: cmdHeader_reserved_bankSelSpad\n");
    CMDGEN_PRINTF("// bank_sel_spad=%d\n", bank_sel_spad);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000010_000000000_%d;\n", g_hw_cmd_num++, cmd->bank_sel_spad);

    return 2;
}

static uint8_t read_pool(void *cmd_addr, uint32_t bias_sel, uint32_t bank_sel_spad)
{
    struct read_pool_cmd *cmd = (struct read_pool_cmd *)cmd_addr;

    cmd->bank_sel_spad = bank_sel_spad;
    cmd->bias_sel = bias_sel;
    cmd->resv = 0;
    cmd->cmd = 0x03;

    CMDGEN_PRINTF("// READ_POOL: cmdHeader_reserved_biasSel_bankSelSpad\n");
    CMDGEN_PRINTF("// bias_sel=%d bank_sel_spad=%d\n", bias_sel, bank_sel_spad);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000011_00000000_%d_%d;\n", g_hw_cmd_num++, cmd->bias_sel, cmd->bank_sel_spad);

    return 2;
}

static uint8_t wait_rd(void *cmd_addr)
{
    struct wait_rd_cmd *cmd = (struct wait_rd_cmd *)cmd_addr;

    cmd->resv = 0;
    cmd->cmd = 0x07;

    CMDGEN_PRINTF("//WAIT_RD\n");
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000111_00000_00000;\n", g_hw_cmd_num++);

    return 2;
}

static uint8_t goto_rd(void *cmd_addr, uint32_t loop_num)
{
    struct goto_rd_cmd *cmd = (struct goto_rd_cmd *)cmd_addr;

    cmd->loop_num = loop_num;
    cmd->cmd = 0x05;

    CMDGEN_PRINTF("// GOTO_RD: cmdHeader_loopNum\n");
    CMDGEN_PRINTF("// loop_num=%d\n", loop_num);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000101_%s;\n", g_hw_cmd_num++, bin_str(cmd->loop_num, 10));

    return 2;
}

static void goto_addr(uint8_t **cmd_addr, uint32_t loop_destination)
{
    struct goto_addr_cmd *cmd = (struct goto_addr_cmd *)(*cmd_addr);

    cmd->destination = loop_destination;

    *cmd_addr += 2;

    CMDGEN_PRINTF("// GOTO_ADDR: destinationAddr\n");
    CMDGEN_PRINTF("// destination=%d\n", cmd->destination);
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b%s;\n", g_hw_cmd_num++, bin_str(cmd->destination, 16));

}

static uint8_t cnn_fin(void *cmd_addr)
{
    struct cnn_fin_cmd *cmd = (struct cnn_fin_cmd *)cmd_addr;

    cmd->resv = 0;
    cmd->cmd = 0x06;
    CMDGEN_PRINTF("// CNN_FIN: called after network or before switching to CPU\n");
    CMDGEN_PRINTF("16'd%d:   CMD_IN<=16'b000110_00000_00000;\n", g_hw_cmd_num++);

    return 2;
}


static void conv_cmd_gen(struct npu_hw_config *cxt, int32_t fifo_count, int32_t fifo_cut, uint32_t fifo_conv_num)
{
    int32_t  ch_fifo_count, cut_fifo_count, cut_cnt;
    int32_t  shift_addr_x, shift_addr_y, shift_addr_x_val, shift_addr_y_val;

    cxt->loop_num = cxt->out_ch;

    /* Load weights unit after unit */
    ch_fifo_count = DIVCEIL(fifo_count, fifo_conv_num);
    cut_fifo_count  = DIVCEIL(fifo_count, fifo_cut);

    cxt->loop_destination = (cxt->cmd_addr - cxt->cmd_seq_start_addr) / 2;
    do
    {
        /* Generate all hardware convolution operation for one channel of one layer
         * by filter decomposition
         */
        shift_addr_x = 0;
        shift_addr_y = 0;
        while (shift_addr_y < cxt->filter_size)
        {
            /* Hardware bug fixing */
            /* It may work only for 3x3 */
            for (cut_cnt = 0; cut_cnt * cut_fifo_count < ch_fifo_count; cut_cnt++)
            {
                /* weight fifo shall be ready before convolution */
                if (fifo_cut > 1)
                {
                    cxt->cmd_addr += fifo_check(cxt->cmd_addr, cut_fifo_count);
                }
                else
                {
                    /* Load all input weights one time  */
                    if (shift_addr_x == 0 && shift_addr_y == 0)
                    {
                        cxt->cmd_addr += fifo_check(cxt->cmd_addr, fifo_count);
                    }
                }

                /* Read bias firstly for one group of input weights */
                if (cut_cnt == 0 && shift_addr_x == 0 && shift_addr_y == 0)
                {
                    cxt->cmd_addr += bias_fetch(cxt->cmd_addr, cxt->addr_back_en, cxt->sram_i_num_shift);
                }

                shift_addr_y_val =  shift_addr_y;
                shift_addr_x_val = ((shift_addr_x - cxt->pad_left) < 0) ? 0 : (shift_addr_x - cxt->pad_left);

                /* It is first conv */
                if (fifo_cut > 1 && cut_cnt == 0 && cut_fifo_count < ch_fifo_count)
                {
                    shift_addr_x_val += 16;
                }
                /* It is last conv */
                else if (fifo_cut > 1 && ((cut_cnt + 1)*cut_fifo_count >= ch_fifo_count))
                {
                    shift_addr_y_val += 15;
                }

                cxt->cmd_addr += conv(cxt->cmd_addr, shift_addr_x_val, shift_addr_y_val);
                cxt->cmd_addr += nop(cxt->cmd_addr, cxt->nop_after_conv);

            }

            shift_addr_x += 3;
            if (shift_addr_x >= cxt->filter_size)
            {
                shift_addr_x = 0;
                shift_addr_y += 3;
                if (shift_addr_y >= cxt->filter_size)
                {
                    break;
                }
            }
        }

        /* Finish convolution operation */
        if (cxt->pool_size == 1)
        {
            cxt->cmd_addr += read_conv(cxt->cmd_addr, cxt->bias_sel, cxt->bank_sel_spad);
        }
        else
        {
            cxt->cmd_addr += pool(cxt->cmd_addr, cxt->bank_sel_spad);
            cxt->cmd_addr += read_pool(cxt->cmd_addr, cxt->bias_sel, cxt->bank_sel_spad);
        }

        cxt->cmd_addr += wait_rd(cxt->cmd_addr);

        /* Loop for all output channels */
        cxt->cmd_addr += goto_rd(cxt->cmd_addr, cxt->out_ch - 1);

        goto_addr(&cxt->cmd_addr, cxt->loop_destination);

    } while (0);

    cxt->weight_seq_end_addr += cxt->out_ch * (8 + (((cxt->ch_num + 1) / 2) * 2) * fifo_conv_num * WEIGHT_GROUP_SIZE);
}

void npu_cmd_gen(struct npu_hw_config *cxt)
{
    uint32_t row_frame_num;
    uint32_t fifo_conv_num;
    int32_t  fifo_count, fifo_cut;

    fifo_conv_num = DIVCEIL(cxt->filter_size, CONV_UNIT_SIZE);
    fifo_conv_num = fifo_conv_num * fifo_conv_num;
    /* fifo check data count = input channels x number of convolution (5x5 -> 4) x 3 */
    fifo_count = (((cxt->ch_num + 1) / 2) * 2) * fifo_conv_num * CONV_UNIT_WEIGHT_GROUPS / 4; /* Total 3x3 count for this output channel */
    /* Load weights unit after unit */
    fifo_cut = DIVCEIL(fifo_count, 256);

    /* Generate init command */
    cxt->cmd_addr += init_0(cxt->cmd_addr, cxt->pool_mode, cxt->isAB, cxt->out_col);
    row_frame_num = DIVCEIL(cxt->row_num, 8) + 1;
    cxt->cmd_addr += init_1(cxt->cmd_addr, row_frame_num, cxt->col_num);
    cxt->cmd_addr += init_2(cxt->cmd_addr, cxt->ch_num / fifo_cut, cxt->conv_stride, cxt->conv_mode, cxt->pool_size, cxt->relu_en);
    cxt->cmd_addr += init_3(cxt->cmd_addr, cxt->clear_up, cxt->clear_left, cxt->clear_bottom, cxt->clear_right);
    cxt->cmd_addr += init_4(cxt->cmd_addr, cxt->relu_num, cxt->pad_up, cxt->pad_left, cxt->pad_bottom, cxt->pad_right);

    /* Generate all hardware convolution operation for all channels */
    conv_cmd_gen(cxt, fifo_count, fifo_cut, fifo_conv_num);

    /* Now loop for all channels is over */
    cxt->cmd_addr += nop(cxt->cmd_addr, cxt->nop_after_conv);

    /* Finish hardware command queue */
    if (cxt->cnn_finish)
    {
        cxt->cmd_addr += cnn_fin(cxt->cmd_addr);
    }
}

#ifndef SIMULATOR_EXEC

#ifdef ARMC4_SIMULATOR

#undef  BANK_A_ADDRESS
#undef  BANK_B_ADDRESS
#undef  BANK_C_ADDRESS
#undef  BANK_D_ADDRESS
#define BANK_A_ADDRESS      ((uint8_t *)(&bank_ab[0]))
#define BANK_B_ADDRESS      (BANK_A_ADDRESS + 0x10000)
#define BANK_C_ADDRESS      ((uint8_t *)(&bank_cd[0]))
#define BANK_D_ADDRESS      (BANK_C_ADDRESS + 0x10000)

static int16_t bank_ab[SRAM_SIZE / 2];
static int16_t bank_cd[SRAM_SIZE / 2];

void npu_dev_init(struct npu_hw_config *cxt)
{
    cxt->bank_a_addr = (uint8_t *)BANK_A_ADDRESS;
    cxt->bank_b_addr = (uint8_t *)BANK_B_ADDRESS;
    cxt->bank_c_addr = (uint8_t *)BANK_C_ADDRESS;
    cxt->bank_d_addr = (uint8_t *)BANK_D_ADDRESS;

    return;
}

void npu_cmd_exec(struct npu_hw_config *config)
{
    return;
}
#else
/***********************
 * KDP hw *
 ***********************/
#include "gm_hal_kdp.h"
void npu_dev_init(struct npu_hw_config *cxt)
{
    cxt->bank_a_addr = (void *)BANK_A_ADDRESS;
    cxt->bank_b_addr = (void *)BANK_B_ADDRESS;
    cxt->bank_c_addr = (void *)BANK_C_ADDRESS;
    cxt->bank_d_addr = (void *)BANK_D_ADDRESS;

    return;
}

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
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
        else if (index % 2 == 1)
        {
            i = index / 2;
            HW32_REG(BRAM_CMD_BASEARDDR + i * 4) = data;
            data = 0;
        }
    }

}

void npu_cmd_exec(struct npu_hw_config *cxt)
{
    struct op_hardware_param hw_param;
    hw_param.cmd_offset = 0;
    hw_param.weight_offset = 0;
    hw_param.cmd_len = cxt->cmd_addr - cxt->cmd_start_addr;
    hw_param.weight_len = cxt->weight_seq_end_addr - cxt->weight_start_addr;

    HAL_DMA_Send(hw_param.weight_len, (unsigned int)(cxt->weight_start_addr + hw_param.weight_offset)); // stream fifo  address must 32 byte aliged
    put_cmd(cxt->cmd_start_addr, hw_param.cmd_offset, hw_param.cmd_len);
    HAL_CNN_Start();
    HAL_CNN_Wait();
}

#endif

#else

#include "ops.h"
#include "kdp.h"

#undef  BANK_A_ADDRESS
#undef  BANK_B_ADDRESS
#undef  BANK_C_ADDRESS
#undef  BANK_D_ADDRESS
#define BANK_A_ADDRESS      ((void *)(&bank_ab[0]))
#define BANK_B_ADDRESS      (BANK_A_ADDRESS + 0x10000)
#define BANK_C_ADDRESS      ((void *)(&bank_cd[0]))
#define BANK_D_ADDRESS      (BANK_C_ADDRESS + 0x10000)

extern struct easynet_dev kdp_dev;
extern int16_t bank_ab[SRAM_SIZE / 2];
extern int16_t bank_cd[SRAM_SIZE / 2];

void npu_dev_init(struct npu_hw_config *cxt)
{
    cxt->bank_a_addr = BANK_A_ADDRESS;
    cxt->bank_b_addr = BANK_B_ADDRESS;
    cxt->bank_c_addr = BANK_C_ADDRESS;
    cxt->bank_d_addr = BANK_D_ADDRESS;
}

void npu_cmd_exec(struct npu_hw_config *config)
{
    struct conv_hw_context *cxt = ((struct conv_hw_context *)kdp_dev.context);
    struct easynet_ops_hardware_param param;

    cxt->cmd_addr = cxt->cmd_start_addr = cxt->cmd_seq_start_addr = config->cmd_addr;
    cxt->weight_addr = cxt->weight_start_addr = cxt->weight_layer_start_addr = cxt->weight_seq_start_addr = cxt->weight_seq_end_addr = config->weight_addr;

    cxt->row_num = config->row_num;
    cxt->col_num = config->col_num;
    cxt->ch_num  = config->ch_num;
    cxt->out_row = config->out_row;
    cxt->out_col = config->out_col;
    cxt->out_ch  = config->out_ch;

    /* We always make rows as multiples of 8 */
    cxt->clear_up   = config->clear_up;
    cxt->clear_left = config->clear_up;
    cxt->clear_right = config->clear_up;
    cxt->clear_bottom = config->clear_up;

    cxt->pad_up = config->pad_up;
    cxt->pad_left = config->pad_left;
    cxt->pad_right = config->pad_right;
    cxt->pad_bottom = config->pad_bottom;
    cxt->pool_pad_up = config->pool_pad_up;

    /* Inout image is not in bank first of all */
    cxt->isAB = config->isAB;

    cxt->nop_after_conv = config->nop_after_conv;
    cxt->relu_en        = config->relu_en;
    cxt->relu_num       = config->relu_num;

    /* No meaning now, fixed to 1 */
    cxt->bank_sel_spad = config->bank_sel_spad;
    cxt->filter_size = config->filter_size;
    cxt->pool_size  = config->pool_size;
    cxt->pool_mode   = config->pool_mode;
    cxt->conv_stride = config->conv_stride;
    cxt->cnn_finish = config->cnn_finish;

    param.header.op = OP_HW;
    param.header.len = sizeof(struct easynet_ops_param) + sizeof(struct op_hardware_param);
    param.hw_param.cmd_offset = 0;
    param.hw_param.weight_offset = 0;
    param.hw_param.cmd_len = cxt->cmd_addr - cxt->cmd_start_addr;
    param.hw_param.weight_len = cxt->weight_seq_end_addr - cxt->weight_start_addr;

    /* I do it for simulation debug */
    cxt->start_out_row = 0;
    cxt->start_out_col = 0;
    cxt->simu_start_out_ch = 0;
    /* Execute conv command */
    kdp_hw_simu((struct easynet_ops_param *)&param, cxt);

}
#endif
