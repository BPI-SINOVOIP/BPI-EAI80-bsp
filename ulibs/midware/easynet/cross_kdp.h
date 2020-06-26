#ifndef __CROSS_KDP_H__
#define __CROSS_KDP_H__

#include <stdint.h>
#include "kdp.h"
#include "ops.h"

#define BANK_OP_MAX_STAGE 8
#define BANK_OP_STAGE(x, idx) (((x) & (0x7 << (4*(idx)))) >> (4*(idx))) /* Extra pipeline stage, 3 bits for each stage */
#define BANK_OP_SHORTCUT  0x1
#define BANK_OP_ROUTE     0x2
#define BANK_OP_MAXPOOL   0x3
#define BANK_OP_ACTIVE    0x4
#define BANK_OP_BATCHNORM 0x5
#define BANK_OP_STAGE_SET(x, idx, val) do{(x) |= (((val)&0x7) << (4*(idx)));}while(0)


#define INVALID_BUFFER_IDX     0xff



/*
 * Operator parameter of general bank operation
 */
#define BANK_CP_FROM_MEM 0
#define BANK_CP_TO_MEM   1
#define BANK_CP_TO_BANK  2
#define BANK_ADD_FROM_MEM 3
#define BANK_ADD_TO_MEM   4
#define BANK_PADDING      5
#define BANK_MAXPOOL      6
#define BANK_ADD_BOTH     7
#define BANK_COPY_CHANNEL 8
#define BANK_EXCHANGE_CHANNEL 9
#define BANK_DEMAP_CHANNEL 10
#define BANK_CP_FROM_MEM_WITHBN 11
#define BANK_ACTIVATION_CHANNEL 12
#define BANK_BATCHNORM_CHANNEL 13
#define BANK_NONE_ACTIVATION 0xFF


struct bank_op_param
{
    int32_t layer_num;                /*< for debug */
    int32_t op;                       /*< Bank operation */
    int32_t bank;                     /*< Bank selection */
    int32_t buffer;                   /*< Memory buffer selection */
    int32_t row_num;                  /*< Total rows of this layer */
    int32_t col_num;                  /*< Total cols of this layer */
    int32_t ch_num;                   /*< Total channels of this layer */
    int32_t start_row;                /*< Start row on bank */
    int32_t valid_len_row;            /*< Valid row length on bank */
    int32_t len_row;                  /*< Row length on bank */
    int32_t start_col;                /*< Start col on bank */
    int32_t len_col;                  /*< Col length on bank */
    int32_t start_channel;            /*< Start channel on bank */
    int32_t len_channel;              /*< Channel length on bank */
    int32_t valid_start_row;          /*< Valid start row on bank */
    int32_t valid_start_col;          /*< Valid start col on bank */
    int32_t src_channel_off;          /*< Source channel for bank-to-bank op */
    int32_t dst_channel_off;          /*< Destination channel for bank-to-bank op */
    int32_t activation;               /*< Activation type */
    int32_t pool_size;                /*< Maxpool size */
    int32_t pool_stride;              /*< Maxpool stride */
    uint32_t pool_pad_up;             /*< Software pool layer pad */
    uint32_t pool_pad_left;           /*< Software pool layer pad */
    uint32_t pool_out_col;            /*< Software pool layer out col */
    uint32_t pool_out_row;            /*< Software pool layer out row */
    int32_t  dst_buffer;              /*< Dst buffer selection for ADD_TO_MEM op */
    int32_t  next_1x1;                /*< Combine 4 channel together for 1x1 conv */
    uint32_t weight_offset;           /*< Addr of batchnorm or activation parameter */
};

/*
 * Operator parameter of datafetch
 */
struct op_cross_datafetch_param
{
    int32_t  layer_num;          /*< Which conv layer  */
    uint32_t start_row; /*< start row */
    uint32_t len_row;   /*< row length */
    uint32_t start_col; /*< start col */
    uint32_t len_col;   /*< col length */
    uint32_t col;       /*< input column */
    uint32_t ch;        /*< input channel */
    uint32_t row;       /*< input row */
    uint32_t buffer;    /*< Select Buffer 0 or 1 */
    uint32_t bank;      /*< Input bank selection */
};

/*
 * Operator parameter of writeout
 */
struct op_cross_writeout_param
{
    int32_t  layer_num;          /*< Which conv layer  */
    uint32_t start_row;          /*< Start row output to buffer */
    uint32_t len_row;            /*< Length of rows in bank output to buffer */
    uint32_t valid_len_row;      /*< Length of valid rows in bank output to buffer  */
    uint32_t start_col;          /*< Start col output to buffer */
    uint32_t len_col;            /*< Length of cols in bank output to buffer */
    uint32_t valid_start_col;    /*< Valid start col in bank output to buffer */
    uint32_t out_col;            /*< Output columns */
    uint32_t out_ch;             /*< Output channels */
    uint32_t out_row;            /*< Output rows */
    uint32_t buffer;             /*< Output buffer selection */
    uint32_t bank;               /*< Output bank selection */
    uint32_t start_channel;      /*< Start channel output to buffer */
    uint32_t len_channel;        /*< Length of channels  output to buffer */
};

/*
 * Operator parameter of route operation
 */
struct op_bank_route_param
{
    struct bank_op_param bank_param;
    int32_t  num;                       /*< input number */
    int32_t  layers[MAX_SHORTCUT_NUM];  /*< input  layer */
    uint32_t buffers[MAX_SHORTCUT_NUM]; /*< input  buffer */
    int32_t  sizes[MAX_SHORTCUT_NUM];   /*< input  size */
    int32_t  ch_num[MAX_SHORTCUT_NUM];  /*< input  channels */
};

/* This is structure describing breaked combined operation which should be run on data in bank */
struct bank_op_context
{
    int32_t  before_layer_number;  /*< Number of end layer before bank op */
    uint32_t relu_en;         /*< Enable relu activation */
    uint32_t relu_num;        /*< Leaky relu paramter */
    uint32_t pool_mode;       /*< Method of pooling operation */
    uint32_t pool_size;       /*< Size of pooling operation */
    uint32_t pool_stride;     /*< Two or one operation */
    uint32_t total_row_num;   /*< Total input row for bank op */
    uint32_t total_col_num;   /*< Total input column for bank op */
    uint32_t total_ch_num;    /*< Total input channel for bank op */
    uint32_t row_num;         /*< Valid input row for bank op */
    uint32_t col_num;         /*< Valid input column for bank op */
    uint32_t ch_num;          /*< Valid input channel for bank op */
    uint32_t start_row_num;   /*< Valid start input row for bank op */
    uint32_t start_col_num;   /*< Valid start input column for bank op */
    uint32_t hardware_out_row;/*< Hardware output height for pool bank op */
    uint32_t hardware_out_col;/*< Hardware output width  for pool bank op */
    int32_t  hardware_start_out_col;/*< Hardware start output col */
    uint32_t pool_pad_up;     /*< Software pool layer pad */
    uint32_t pool_pad_left;   /*< Software pool layer pad */
    int32_t  activation;
#if 0
    uint32_t extra_relu_en;   /*< Enable relu activation */
    uint32_t extra_relu_num;  /*< Leaky relu paramter */
#endif
    uint32_t stage;           /*< Which layer of each stage */
    int32_t  stage_layer_num[BANK_OP_MAX_STAGE]; /*< Layer number of each bank-op layer */
    void   *shortcut_layer;  /*< Banked op for shortcut layer ,  notice pc  64bit cpu, c pointer 8 byte, kdp 32 bit cpu pointer 4 byte */
    void   *route_layer;     /*< Banked op for route layer */
};

/*
 * Operator parameter of special op for input channel cut
 */
struct op_channel_cut_param
{
    struct op_hardware_param hw_param;
    struct bank_op_param bank_param;
    uint32_t total_out_ch;
    uint32_t start_ch_num;
    uint32_t ch_num;
    uint32_t in_len_row;
    uint32_t in_valid_len_row;
    uint32_t in_len_col;
    int32_t  referred_index;
    int32_t  referred_layer_num;
    int32_t  end_layer_num;
    uint32_t output_idx;
    uint32_t conv_stride;
    int32_t  shortcut_index;
    int32_t  compress_1x1;
    int32_t  res;  /*Reserved alignment, for pc  64bit cpu, c pointer 8 byte, kdp 32 bit cpu pointer 4 byte*/
    struct bank_op_context bank_op;
};

struct op_bank_depthwise_param
{
    struct op_hardware_param hw_param;
    struct bank_op_param in_bank_param;
    struct bank_op_param out_bank_param;
    int32_t next_1x1;
    uint32_t total_out_ch;
    uint32_t start_ch_num;
    uint32_t ch_num;
    uint32_t in_len_row;
    uint32_t in_valid_len_row;
    uint32_t in_len_col;
    int32_t  referred_index;
    int32_t  referred_layer_num;
    uint32_t output_idx;
    uint32_t conv_stride;
    int32_t  shortcut_index;
    int32_t  res;  /*Reserved alignment, for pc  64bit cpu, pointer 8 byte, kdp 32 bit cpu pointer 4 byte*/
    struct bank_op_context bank_op;
};

#endif
