/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        npu.h
 *
 * @author      Wen.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/03/23
 *
 * @brief       NPU Header For 6721 image lib
 *
 * @note
 *              2019/03/23, Wen.Liu, v1.0.0
 *                  Initial version.
 *              2020/03/26, Wen.Liu, v1.0.0
 *                  Make it simple for image processing
 *
 **/

#ifndef __NPU_H__
#define __NPU_H__

#define BANK_A_ADDRESS     0x20020000
#define BANK_B_ADDRESS     0x20030000

#define BANK_C_ADDRESS     0x20040000
#define BANK_D_ADDRESS     0x20050000

#define SRAM_SIZE          128*1024
#define SPAD_SIZE          16*1024


#define ROW_GROUP_SIZE          8
#define WEIGHT_GROUP_SIZE       12
#define CONV_UNIT_SIZE          3
#define CONV_UNIT_WEIGHT_GROUPS 3

#define DIVCEIL(x, y)   ((x) + (y) - 1)/(y)
#define DIVFLOOR(x, y)  ((x))/(y)


/*
 * Abstract operator and parameters
 */

/*
 * Operator parameter of hardware
 */
struct op_hardware_param
{
    uint32_t cmd_offset;    /*<  Start of command queue */
    uint32_t cmd_len;       /*< command length */
    uint32_t weight_offset; /*<  Start of weight array */
    uint32_t weight_len;    /*< weight length */
    uint32_t one_weight_size; /*< weight length of one output channel */
};

/*
 * Format of all npu commands
 */
struct init0_cmd
{
    uint16_t col_num_rd: 8;    /*< [7:0]  col num rd     */
    uint16_t bank_sel_layer: 1; /*< [8]    bank sel layer */
    uint16_t pool_mode: 1;     /*< [9]    pool mode      */
    uint16_t cmd: 6;           /*< [15:10]  command      */
};

struct init1_cmd
{
    uint16_t col_num: 8;       /*< [7:0] col num         */
    uint16_t row_frame_num: 8; /*< [15:8]  row frame num */
};

struct init2_cmd
{
    uint16_t relu_en: 1;       /*< [0] relu en     */
    uint16_t pool_size: 2;     /*< [2:1]  pool num */
    uint16_t conv_mode: 1;     /*< [3]    conv num */
    uint16_t stride: 2;        /*< [5:4]  stride   */
    uint16_t channel_num: 10;  /*< [15:6] channel num */
};

struct init3_cmd
{
    uint16_t right_zero: 4;    /*< [3:0]    right padding with zero  */
    uint16_t bottom_zero: 4;   /*< [7:4]    bottom padding with zero */
    uint16_t left_zero: 4;     /*< [11:8]   left padding with zero   */
    uint16_t up_zero: 4;       /*< [15:12]  up padding with zero     */
};

struct init4_cmd
{
    uint16_t pad_right: 4;    /*< [3:0]    right padding with zero  */
    uint16_t pad_bottom: 4;   /*< [7:4]    bottom padding with zero */
    uint16_t pad_left: 2;     /*< [9:8]    left padding with zero   */
    uint16_t pad_up: 2;       /*< [11:10]  up padding with zero     */
    uint16_t relu_num: 4;     /*< [15:12]  relu num                 */
};

struct conv_cmd
{
    uint16_t shift_addr_y: 5;  /*< [4:0] shift_addr_y  */
    uint16_t shift_addr_x: 5;  /*< [9:5]  shift_addr_x */
    uint16_t cmd: 6;           /*< [15:10]  command    */
};

struct nop_cmd
{
    uint16_t nop_after_conv: 10; /*< [9:0] nop count  */
    uint16_t cmd: 6;            /*< [15:10]  command */
};

struct read_conv_cmd
{
    uint16_t bank_sel_spad: 1;    /*< [0] bank_sel_spad  */
    uint16_t bias_sel: 1;         /*< [1] bias_sel       */
    uint16_t resv: 8;             /*< [9:2] resv         */
    uint16_t cmd: 6;              /*< [15:10]  command   */
};

struct bias_fetch_cmd
{
    uint16_t sram_i_num_shift: 9;  /*< [8:0] sram_i_num_shift  */
    uint16_t addr_back_en: 1;      /*< [9]   addr_back_en      */
    uint16_t cmd: 6;               /*< [15:10]  command        */
};

struct fifo_check_cmd
{
    uint16_t check_data_count: 10; /*< [9:0] fifo count  */
    uint16_t cmd: 6;              /*< [15:10]  command    */
};

struct pool_cmd
{
    uint16_t bank_sel_spad: 1;    /*< [0] bank_sel_spad  */
    uint16_t resv: 9;             /*< [9:1] resv         */
    uint16_t cmd: 6;              /*< [15:10]  command   */
};

struct read_pool_cmd
{
    uint16_t bank_sel_spad: 1;    /*< [0] bank_sel_spad  */
    uint16_t bias_sel: 1;         /*< [1] bias_sel       */
    uint16_t resv: 8;             /*< [9:2] resv         */
    uint16_t cmd: 6;              /*< [15:10]  command   */
};

struct wait_rd_cmd
{
    uint16_t resv: 10;            /*< [9:0]   resv        */
    uint16_t cmd: 6;              /*< [15:10]  command    */
};

struct goto_rd_cmd
{
    uint16_t loop_num: 10;        /*< [9:0]   loop_num    */
    uint16_t cmd: 6;              /*< [15:10]  command    */
};

struct goto_addr_cmd
{
    uint16_t destination;         /*< [15:0] destination addr  */
};

struct cnn_fin_cmd
{
    uint16_t resv: 10;            /*< [9:0] resv          */
    uint16_t cmd: 6;              /*< [15:10]  command    */
};

struct npu_hw_config
{
    uint8_t *cmd_start_addr; /*< Start addr of all commands */
    uint8_t *cmd_seq_start_addr; /*< Start addr of one command sequence */
    uint8_t *cmd_addr;       /*< command addr  */
    uint8_t *weight_start_addr; /*< weight start addr  */
    uint8_t *weight_layer_start_addr; /*< Start weight addr of layer sequence */
    uint8_t *weight_seq_start_addr; /*< Start weight addr of one command sequence */
    uint8_t *weight_seq_end_addr;   /*< End weight addr of one command sequence */
    uint8_t *weight_addr;   /*< weight addr */
    uint8_t *bank_a_addr;   /*< Address of A-Bank */
    uint8_t *bank_b_addr;   /*< Address of B-Bank */
    uint8_t *bank_c_addr;   /*< Address of C-Bank */
    uint8_t *bank_d_addr;   /*< Address of D-Bank */
    uint32_t row_num;  /*< input height  */
    uint32_t col_num;  /*< input width   */
    uint32_t ch_num;   /*< input channel */
    uint32_t out_row;  /*< output height  */
    uint32_t out_col;  /*< output width   */
    uint32_t out_ch;   /*< output channel */
    int32_t  pad_up;   /*< padding on up lines */
    int32_t  pad_left; /*< padding on left lines */
    int32_t  pad_right;/*< padding on right lines */
    int32_t  pad_bottom;/*< padding on bottom lines */
    uint32_t clear_up; /*< clearing up lines */
    uint32_t clear_left; /*< clearing left lines */
    uint32_t clear_right;/*< clearing right lines */
    uint32_t clear_bottom;  /*< clearing bottom lines */
    //uint32_t row_frame_num; /*< kdp process 8 rows each time  */
    uint32_t conv_mode;        /*< Mode of convolution operation */
    uint32_t conv_stride;      /*< Stride of convolution operation */
    uint32_t filter_size;      /*< kernel size of convolutional operation */
    uint32_t nop_after_conv;   /*< Delay for some clocks after one calculation step */
    uint32_t sram_i_num_shift;
    uint32_t addr_back_en;
    uint32_t relu_en;         /*< Enable relu activation */
    uint32_t relu_num;
    uint32_t pool_mode;       /*< Method of pooling operation */
    uint32_t pool_size;       /*< Size of pooling operation */
    uint32_t pool_stride;     /*< Two or one operation */
    uint32_t bias_sel;
    uint32_t bank_sel_spad;   /*< Select a/b bank or c/d bank */
    //uint32_t bank_sel_layer;
    uint32_t isAB;
    uint32_t loop_destination;
    int32_t  loop_num;       /*< Loop counter for command "goto_rd" */
    int32_t  hw_out_ch;      /*< Simulating hardware output channel counter */
    uint32_t cnn_finish;     /*< Stop cnn hardware operation */
    uint32_t total_ch_num;   /*< Total input channel for this layer */
    uint32_t total_row_num;  /*< Total input row for this layer */
    uint32_t total_col_num;  /*< Total input column for this layer */
    uint32_t total_pad_up;   /*< Padding on up side for this layer */
    uint32_t total_pad_bottom;  /*< Padding on bottom side for this layer */
    uint32_t pool_pad_up;    /*< Hardware pool layer pad */
    uint32_t start_row_num;  /*< Start row in input bank */
    uint32_t total_out_ch;   /*< Total output channel of this layer */
    uint32_t total_out_row;  /*< Total output row of this layer */
    uint32_t total_out_col;  /*< Total output column of this layer */
    uint32_t start_out_ch;   /*< Start channel in output bank */
    uint32_t start_out_row;  /*< Start row in output bank */
    uint32_t start_out_col;  /*< Start col in output bank */
    uint32_t simu_start_out_ch;  /*< Start output channel for this command sequence */
};

void npu_dev_init(struct npu_hw_config *cxt);
void npu_cmd_gen(struct  npu_hw_config *cxt);
void npu_cmd_exec(struct npu_hw_config *cxt);

#endif //__KDP_H__
