/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		kdp.h
 *
 * @author		Wen.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/03/23
 *
 * @brief		KDP Header For easynet
 *
 * @note		
 *				2019/03/23, Wen.Liu, v1.0.0
 *					Initial version.
 *			
 **/

#ifndef __KDP_H__
#define __KDP_H__
#include "layer.h"

//#define STEP_BY_STEP_DEBUG
#define MAX_SHORTCUT_NUM   4

#define BANK_A_ADDRESS     0x20020000
#define BANK_B_ADDRESS     0x20030000

#define BANK_C_ADDRESS     0x20040000
#define BANK_D_ADDRESS     0x20050000

#define SRAM_SIZE          128*1024
#define SPAD_SIZE          16*1024

#define PIPELINE_CONV      0x1
#define PIPELINE_BN        0x2
#define PIPELINE_ACTIVE    0x4
#define PIPELINE_POOL      0x8

#define HW_CMD_MAX_SIZE   0x10000

#define HW_OP_MAX_SIZE    0x10000

#define ROW_GROUP_SIZE          8 
#define WEIGHT_GROUP_SIZE       12
#define CONV_UNIT_SIZE          3
#define CONV_UNIT_WEIGHT_GROUPS 3   

#define FLOAT_PRECISION         8

#define DIVCEIL(x, y)   ((x) + (y) - 1)/(y)
#define DIVFLOOR(x, y)  ((x))/(y)

#define COMPILE_BUFFER_CHECK(cxt) do { \
                                      if (((cxt)->max_len - ((cxt)->weight_addr - (cxt)->ops_start_addr)) < 16) \
                                      {                    \
                                          return RET_ERROR;\
                                      };\
                                     }while(0)
                                     
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
};
 
/* 
 * Operator parameter of datafetch
 */
struct op_datafetch_param
{
    uint32_t start_row; /*< start row */
    uint32_t len_row;   /*< row length */
    uint32_t col;       /*< input column */
    uint32_t ch;        /*< input channel */
    uint32_t row;       /*< input row */
    uint32_t buffer;    /*< Select Buffer 0 or 1 */
};

/* 
 * Operator parameter of writeout
 */
struct op_writeout_param
{
    uint32_t start_row;          /*< Start row output to buffer */
    uint32_t len_row;            /*< Length of rows in bank output to buffer */
    uint32_t valid_len_row;      /*< Length of valid rows in bank output to buffer  */
    uint32_t out_col;            /*< Output columns */
    uint32_t out_ch;             /*< Output channels */
    uint32_t out_row;            /*< Output rows */
    uint32_t buffer;             /*< Output buffer selection */
    uint32_t start_channel;      /*< Start channel output to buffer */
    uint32_t len_channel;        /*< Length of channels  output to buffer */
};

/* 
 * Operator parameter of shortcut_buffer
 */
struct op_shortcut_buffer_param
{
    uint32_t size;               /*< Output size */
    uint32_t output;             /*< Output buffer */
    uint32_t shortcut_index;     /*< Which shortcut buffer is used, zero if there is no shortcut */
    uint32_t layer_num;          /*< Which layer is put into the shortcut buffer */
};

/*
 * Operator parameter of  avgpool
 */
struct op_avgpool_param
{
    uint32_t col;      /*< input column */
    uint32_t row;      /*< input row */
    uint32_t ch;       /*< input channel */
    uint32_t valid_col;    /*<  */
    uint32_t valid_row;    /*< */
};

/* 
 * Operator parameter of switch_input
 */
struct op_switch_inout_param
{
    uint32_t ch;       /*< input channel */
    uint32_t row;      /*< input row */
    uint32_t col;      /*< input column */
    uint32_t pad_col;  /*<  */
};

/* 
 * Operator parameter of maxpool
 */
struct op_maxpool_param
{
	int32_t layer_num;    /*< for debug */
	int32_t buffer;       /*< input buffer */
    int32_t ch_num;       /*< input  channel */
    int32_t row_num;      /*< input  row */
    int32_t col_num;      /*< input  column */
    int32_t out_row;      /*< output row */
    int32_t out_col;      /*< output column */
    int32_t pool_size;    /*< Maxpool size */
    int32_t pool_stride;  /*< Maxpool stride */
    int32_t pad_left;     /*< Maxpool pad  */
};

/* 
 * Operator parameter of image input
 */
struct op_image_input_param
{
    uint32_t ch;       /*< input channel */
    uint32_t row;      /*< input row */
    uint32_t col;      /*< input column */
};

/* 
 * Operator parameter of layer indicator
 */
struct op_layer_param
{
    uint32_t conv;        /*< network conv layer */
    uint32_t active;      /*< network active layer */
    uint32_t bn;          /*< network bn layer */
    uint32_t pool;        /*< network pool layer */  
    uint32_t filter_size;    /*< Kernel size of this convolution layer */
    uint32_t total_ch_num;   /*< Total input channel for this layer */
    uint32_t total_row_num;  /*< Total input row for this layer */
    uint32_t total_col_num;  /*< Total input column for this layer */
    uint32_t total_pad_up;   /*< Padding on up side for this layer */
    uint32_t total_pad_bottom;  /*< Padding on bottom side for this layer */
    uint32_t total_out_ch;   /*< Total output channel of this layer */
    uint32_t total_out_row;  /*< Total output row of this layer */
    uint32_t total_out_col;  /*< Total output column of this layer */
};

/* 
 * Operator parameter of shortcut operation
 */
struct op_shortcut_param
{
	int32_t layer_num;    /*< for debug */
	int32_t buffer;       /*< Input buffer selection */
	int32_t source;       /*< Input layer */
    int32_t ch_num;       /*< input  channel */
    int32_t row_num;      /*< input  row */
    int32_t col_num;      /*< input  column */
    int32_t out_ch;       /*< output channel */
    int32_t out_row;      /*< output row */
    int32_t out_col;      /*< output column */
    int32_t alpha;        /*< Maxpool size */
    int32_t beta;         /*< Maxpool stride */
    int32_t activation;   /*< Activation type */
    int32_t out_cnt;  
};

/* 
 * Operator parameter of route operation
 */
struct op_route_param
{
	int32_t layer_num;                /*< for debug */
	int32_t buffer;                   /*< Input buffer selection */
	int32_t num;                      /*< input number */
	int32_t  layers[MAX_SHORTCUT_NUM];  /*< input  layer */
    uint32_t buffers[MAX_SHORTCUT_NUM]; /*< input  buffer */
    int32_t  sizes[MAX_SHORTCUT_NUM];   /*< input  size */
};

/*
 * Operator parameter of shortcut operation
 */
struct op_activation_param
{
	int32_t layer_num;    /*< for debug */
	int32_t buffer;       /*< Input buffer selection */
    int32_t activation;   /*< Activation type */
    int32_t out_cnt;
};
/*
 * Format of all kdp 310 commands
 */
struct init0_cmd 
{
    uint16_t col_num_rd:8;     /*< [7:0]  col num rd     */
    uint16_t bank_sel_layer:1; /*< [8]    bank sel layer */
    uint16_t pool_mode:1;      /*< [9]    pool mode      */
    uint16_t cmd:6;            /*< [15:10]  command      */      
};

struct init1_cmd 
{
    uint16_t col_num:8;        /*< [7:0] col num         */
    uint16_t row_frame_num:8;  /*< [15:8]  row frame num */    
};

struct init2_cmd 
{
    uint16_t relu_en:1;        /*< [0] relu en     */
    uint16_t pool_size:2;      /*< [2:1]  pool num */
    uint16_t conv_mode:1;      /*< [3]    conv num */
    uint16_t stride:2;         /*< [5:4]  stride   */
    uint16_t channel_num:10;   /*< [15:6] channel num */
};

struct init3_cmd 
{
    uint16_t right_zero:4;     /*< [3:0]    right padding with zero  */
    uint16_t bottom_zero:4;    /*< [7:4]    bottom padding with zero */
    uint16_t left_zero:4;      /*< [11:8]   left padding with zero   */
    uint16_t up_zero:4;        /*< [15:12]  up padding with zero     */
};

struct init4_cmd 
{
    uint16_t pad_right:4;     /*< [3:0]    right padding with zero  */
    uint16_t pad_bottom:4;    /*< [7:4]    bottom padding with zero */
    uint16_t pad_left:2;      /*< [9:8]    left padding with zero   */
    uint16_t pad_up:2;        /*< [11:10]  up padding with zero     */
    uint16_t relu_num:4;      /*< [15:12]  relu num                 */
};

struct conv_cmd 
{
    uint16_t shift_addr_y:5;   /*< [4:0] shift_addr_y  */
    uint16_t shift_addr_x:5;   /*< [9:5]  shift_addr_x */ 
    uint16_t cmd:6;            /*< [15:10]  command    */ 
};

struct nop_cmd 
{
    uint16_t nop_after_conv:10; /*< [9:0] nop count  */
    uint16_t cmd:6;             /*< [15:10]  command */ 
};

struct read_conv_cmd 
{
    uint16_t bank_sel_spad:1;     /*< [0] bank_sel_spad  */
    uint16_t bias_sel:1;          /*< [1] bias_sel       */
    uint16_t resv:8;              /*< [9:2] resv         */
    uint16_t cmd:6;               /*< [15:10]  command   */ 
};

struct bias_fetch_cmd 
{
    uint16_t sram_i_num_shift:9;   /*< [8:0] sram_i_num_shift  */
    uint16_t addr_back_en:1;       /*< [9]   addr_back_en      */ 
    uint16_t cmd:6;                /*< [15:10]  command        */ 
};

struct fifo_check_cmd 
{
    uint16_t check_data_count:10; /*< [9:0] fifo count  */
    uint16_t cmd:6;               /*< [15:10]  command    */ 
};

struct pool_cmd 
{
    uint16_t bank_sel_spad:1;     /*< [0] bank_sel_spad  */
    uint16_t resv:9;              /*< [9:1] resv         */
    uint16_t cmd:6;               /*< [15:10]  command   */ 
};

struct read_pool_cmd 
{
    uint16_t bank_sel_spad:1;     /*< [0] bank_sel_spad  */
    uint16_t bias_sel:1;          /*< [1] bias_sel       */
    uint16_t resv:8;              /*< [9:2] resv         */
    uint16_t cmd:6;               /*< [15:10]  command   */ 
};

struct wait_rd_cmd 
{
    uint16_t resv:10;             /*< [9:0]   resv        */
    uint16_t cmd:6;               /*< [15:10]  command    */ 
};

struct goto_rd_cmd 
{
    uint16_t loop_num:10;         /*< [9:0]   loop_num    */
    uint16_t cmd:6;               /*< [15:10]  command    */ 
};

struct goto_addr_cmd 
{
    uint16_t destination;         /*< [15:0] destination addr  */
};

struct cnn_fin_cmd 
{
    uint16_t resv:10;             /*< [9:0] resv          */
    uint16_t cmd:6;               /*< [15:10]  command    */ 
};

struct conv_hw_context
{
    uint32_t max_len;        /*< Max length of this compiled ops bin */ 
    void    *ops_start_addr; /*< Start addr of this compiled ops bin */
    void    *cmd_start_addr; /*< Start addr of all commands */
    void    *cmd_seq_start_addr; /*< Start addr of one command sequence */
    void    *cmd_addr;       /*< command addr  */
    void    *weight_start_addr; /*< weight start addr  */
    void    *weight_layer_start_addr; /*< Start weight addr of layer sequence */
    void    *weight_seq_start_addr; /*< Start weight addr of one command sequence */
    void    *weight_addr;   /*< weight addr */
    void    *bank_a_addr;   /*< Address of A-Bank */
    void    *bank_b_addr;   /*< Address of B-Bank */
    void    *bank_c_addr;   /*< Address of C-Bank */
    void    *bank_d_addr;   /*< Address of D-Bank */
    int16_t  gamma;    /*< Converted BN gamma */
    int16_t  beta;     /*< Converted BN beta */
    int16_t  bias;     /*< Convolution bias */
    int16_t  bn_pad;   /*< 32 bit Pad  */
    uint32_t row_num;  /*< input height  */
    uint32_t col_num;  /*< input width   */
    uint32_t ch_num;   /*< input channel */
    uint32_t out_row;  /*< output height  */ 
    uint32_t hardware_out_row;  /*< output height  */
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
    uint32_t previous_row_pad; /*< Previous row pad */
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
    uint32_t max_size;       
    uint32_t cnn_finish;     /*< Stop cnn hardware operation */
    uint32_t total_ch_num;   /*< Total input channel for this layer */
    uint32_t total_row_num;  /*< Total input row for this layer */
    uint32_t total_col_num;  /*< Total input column for this layer */
    uint32_t total_pad_up;   /*< Padding on up side for this layer */
    uint32_t total_pad_bottom;  /*< Padding on bottom side for this layer */
    uint32_t pool_pad_up;    /*< Hardware pool layer pad */
    uint32_t start_row_num;  /*< Start row in input bank */
    uint32_t next_start_row_num; /*< Start row in input bank which shall be loaded into input bank next time */
    uint32_t next_start_out_row; /*< Start row in output bank which shall be generated next time */
    uint32_t total_out_ch;   /*< Total output channel of this layer */
    uint32_t total_out_row;  /*< Total output row of this layer */
    uint32_t total_out_col;  /*< Total output column of this layer */
    uint32_t start_out_ch;   /*< Start channel in output bank */
    uint32_t start_out_row;  /*< Start row in output bank */
    uint32_t input_in_bank;  /*< If input data is already all in inout bank */
    uint32_t pipeline_up;    /*< Occupied pipeline status */
    uint32_t pipeline_down;  /*< Empty pipeline status */
    uint32_t simu_start_out_ch;  /*< Start output channel for this command sequence */
    uint32_t shortcut[MAX_SHORTCUT_NUM]; /*< Layers of each shorcut buffer, zero if not valid */
    uint32_t skip_conv; /*< Simulation skip all convolution */
};

struct compile_context
{
     struct conv_hw_context cnn_hw_cxt;   /*< Point to hardware compiling context */
     void *ops_param;                     /*< Point to all ops */
     uint32_t max_ops_len;                /*< Max length of all ops */
     int32_t conv_layer_number;           /*< Number of this conv layer */
     int32_t active_layer_number;         /*< Number of this active layer */
     int32_t bn_layer_number;             /*< Number of this bn layer */
     int32_t pool_layer_number;           /*< Number of this pool layer */
     layer *conv_layer;
     layer *bn_layer;
     layer *active_layer;
     layer *pool_layer;
};

static inline int64_t simu_relu_activate(int64_t x)
{
    return (x > 0) ? x : 0;
}

/*
 * fix32_round_truncate(fix32_mul(fix32_from_float(.1), x));
 */
static inline int64_t simu_leaky_activate(int64_t x, int64_t relu_num)
{
	if (x > 0)
	{
	    return x;
	}
	else
	{	
        return fix32_round_truncate(x*relu_num*2);
	}
}

void kdp_ops_load(struct easynet_dev *dev, void *bin, uint32_t len, void *input);
uint32_t kdp_ops_compile(struct easynet_dev *dev, network *net, void *bin, uint32_t max_len);
int kdp_simu_ops_init(struct easynet_dev *dev);
int kdp_simu_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param);
int soft_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param);
int weight_covert(struct compile_context *cxt, uint32_t pipeline_status);

#endif //__KDP_H__
