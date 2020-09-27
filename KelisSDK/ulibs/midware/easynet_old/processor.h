#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
#include "ai_common.h"
#include <stdint.h>
#include "processor_common.h"

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

#define IMP_TRANSFER_DATA  1
#define HW_CMD_MAX_SIZE   0x10000
#define HW_OP_MAX_SIZE    0x10000
#define OPS_FILE_MAGIC_NUMBER 0x20198779

#define FLT_MAX 3.402823466e+38F

#define BANK_OP_STAGE(x, idx) (((x) & (0x7 << (4*(idx)))) >> (4*(idx))) /* Extra pipeline stage, 3 bits for each stage */
#define BANK_OP_SHORTCUT  0x1
#define BANK_OP_ROUTE     0x2
#define BANK_OP_MAXPOOL   0x3
#define BANK_OP_ACTIVE    0x4
#define BANK_OP_BATCHNORM 0x5
#define BANK_OP_STAGE_SET(x, idx, val) do{(x) |= (((val)&0x7) << (4*(idx)));}while(0)

/*
 * Address Space for KDP310 IP
 */
#define BRAM_OUTPUT_A_BASEARDDR     0x20020000
#define BRAM_OUTPUT_B_BASEARDDR     0x20030000
#define BRAM_OUTPUT_C_BASEARDDR     0x20040000
#define BRAM_OUTPUT_D_BASEARDDR     0x20050000
#define BRAM_CMD_BASEARDDR          0x20060000
#define BRAM_INPUT_BASEADDR         0x20020000 //Input Bank: BankA, BankB; Output Bank: BankC BankD; Bank Address:0x4000,0000; BankB address: 0x4002,0000;

#define KDP_CTRL        0x46000000
#define KDP_STATUS      0x46000004
#define CORE_BASEADDR   0x46000000

/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_SUCCESS  0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
#define RET_OVER     0x06  /**< The whole processing is finished now */


#define MAX_SHORTCUT_NUM   4
#define BANK_A_ADDRESS     0x20020000
#define BANK_B_ADDRESS     0x20030000
#define BANK_C_ADDRESS     0x20040000
#define BANK_D_ADDRESS     0x20050000
#define SRAM_SIZE          128*1024
#define SPAD_SIZE          16*1024

#define HW_CMD_MAX_SIZE   0x10000
#define HW_OP_MAX_SIZE    0x10000


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
#define BANK_NONE_ACTIVATION 0xFF

#define MAX_LAYER_BUFFER_NUM  6
#define MAX_YOLO_OUT_NUM     2


struct bank_op_param
{
    int32_t layer_num;                /*< for debug */
    int32_t op;                       /*< Bank operation */
    int32_t bank;                     /*< Bank selection */
    int32_t buffer;                   /*< Memory buffer selection */
    int32_t row_num;                  /*< Total rows of this layer */
    int32_t col_num;                  /*< Total cols of this layer */
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
};



typedef enum
{
    TINYYOLOV3 = 3,
    OTHERS
} NETWORK_TYPE;
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
#if 1
    uint32_t bank;      /*< Input bank selection */
#endif
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
    int32_t data_type;    /*< type of layer data, 1 for float, 0 for int16  */
};

/*
 * Operator parameter of yolo
 */
struct op_yolo_param
{
    int32_t layer_num;    /*< for debug */
    int32_t buffer;       /*< input buffer */
    int32_t ch_num;       /*< input  channel */
    int32_t row_num;      /*< input  row */
    int32_t col_num;      /*< input  column */
    int32_t classes;      /*< yolo classes */
    int32_t out_ch;       /*< output channel */
    int32_t index;        /*< current yolo index */
};

/*
 * Operator parameter of upsample
 */
struct op_upsample_param
{
    int32_t layer_num;    /*< for debug */
    int32_t buffer;       /*< input buffer */
    int32_t ch_num;       /*< input  channel */
    int32_t row_num;      /*< input  row */
    int32_t col_num;      /*< input  column */
    int32_t out_row;      /*< output row */
    int32_t out_col;      /*< output column */
    int32_t stride;       /*< upsample stride */
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
    uint32_t end_layer;
    uint32_t conv;        /*< network conv layer */
    uint32_t active;      /*< network active layer */
    uint32_t bn;          /*< network bn layer */
    uint32_t pool;        /*< network pool layer */
    int32_t  buffer;      /*< Input buffer selection */
    uint32_t filter_size;    /*< Kernel size of this convolution layer */
    uint32_t total_ch_num;   /*< Total input channel for this layer */
    uint32_t total_row_num;  /*< Total input row for this layer */
    uint32_t total_col_num;  /*< Total input column for this layer */
    uint32_t total_pad_up;   /*< Padding on up side for this layer */
    uint32_t total_pad_bottom;  /*< Padding on bottom side for this layer */
    uint32_t total_out_ch;   /*< Total output channel of this layer */
    uint32_t total_out_row;  /*< Total output row of this layer */
    uint32_t total_out_col;  /*< Total output column of this layer */
    uint32_t start_out_row;  /*< Start output row of this part */
    uint32_t start_out_col;  /*< Start output column of this part */
    uint32_t start_out_ch;   /*< Start output channel of this part */
    uint32_t out_row;        /*< Valid output row of this part */
    uint32_t out_col;        /*< Valid output column of this part */
    uint32_t out_ch;         /*< Valid output channel of this part */
    uint32_t cross_start;    /*<  First unit of multiple layers */
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
 * The following types are used to store user data
 *
 * */
typedef struct node
{
    struct node *next;
    int label;
    int baseCount;
    float *baseVector;
    //might have other variables such as name
} node_t;

/**
 * Hardware Operators on Abstract Layer
 */

typedef enum
{
    OP_HW,
    OP_WAIT,
    OP_MAXPOOL,
    OP_AVGPOOL,
    OP_CONNECTED,
    OP_WRITEOUT,
    OP_DATAFETCH,
    OP_INPUT,
    OP_SWITCH_INOUT,
    OP_LAYER,          /*< This is layer indicator for debugging step by step */
    OP_BUFFER,
    OP_SHORTCUT,
    OP_ROUTE,
    OP_ACTIVATION,
    OP_OUTPUT,
    OP_CROSS_WRITEOUT,
    OP_CROSS_DATAFETCH,
    OP_BANK_MAXPOOL,
    OP_YOLO,
    OP_UPSAMPLE,
    OP_BANK_GENERAL,
    OP_BANK_ROUTE,
    OP_DEPTHWISE_CONV,
} OP_TYPE;

/**
 *  Information for one combined layer
 */
struct layer_config
{
    uint32_t layer_number_start;    /*< Number of start layer of this combined layer */
    uint32_t layer_number_end;      /*< Number of end layer of this combined layer */
    uint32_t weight_compress;       /*< 0: No compress; 1: 1X1 compress; 2: hufmann compress; 3: gzip compress */
    uint32_t weight_type;           /*< WEIGHT_DEFAULT; WEIGHT_NPU;  */
};

#define MAX_LAYER_NUM  256

struct easynet_file_head
{
    uint32_t magic_number;                /*< File magic number */
    uint32_t version;                     /*< version number */
    uint32_t ops_offset;                  /*< Ops address */
    uint32_t cmd_offset;                  /*< Command address */
    uint32_t weight_offset;               /*< Weight address */
    uint32_t bn_square_sigma_only;        /*< 0(default): Square both sigma and epsilon 1: Square sigma only */
    uint32_t bn_epsilon;                  /*< Default(0.001f), Posenet would be 2e-5, yolo v3 shall be .000001f */
    uint32_t config_number;               /*< Number of config of network layers, conv/bn/activation/maxpool maybe combined together */
    struct layer_config layers_config[MAX_LAYER_NUM];   /*< All configuration for each layer, shall be last member of file header */
};

/* sync word for each operator */
#define SYNC_WORD 0x78515255
/**
 *  Abstract Operator Parameter, hw specific paramters are specified and processed by hardware layer
 */
struct easynet_ops_param
{
    uint32_t sync;
    uint32_t op;
    uint32_t len;
};

/**
 *  Device configuration parameter, including all layer buffers allocated by user, BN parameters etc.
 */
struct easynet_dev_cfg
{
    uint32_t layer_buffer_num;
    void *layer_buffer[MAX_LAYER_BUFFER_NUM];
    float *yolo_buffer[MAX_YOLO_OUT_NUM];
    uint32_t bn_square_sigma_only;  /*< 0(default): Square both sigma and epsilon 1: Square sigma only */
    uint32_t bn_epsilon;            /*< Default(0.001f), Posenet would be 2e-5, yolo v3 shall be .000001f */
    uint32_t optimization;          /*< 0 : No optimization; 1 : Fast; 2 : Special */
};

/**
 *  Abstract Device Running Operators
 */
struct easynet_dev
{
    void *param;
    int (*process)(struct easynet_dev *dev, struct easynet_ops_param *);
    uint32_t (*compile)(struct easynet_dev *dev, void *net, void *bin, uint32_t max_len);
    void (*load)(struct easynet_dev *dev, void *bin, uint32_t len, void *input);
    void (*start)(struct easynet_dev *dev, void *input, void *output);
    void (*hw_cb)(struct easynet_dev *dev, int16_t *output, int index);
    uint32_t cur_addr;
    uint32_t len;
    void *context;
    unsigned char *input;
    void *predict_out;
    struct easynet_dev_cfg *cfg;
#if 0
    int  network_type;
    float *yolo_out_buffer[2];
    int16_t *shortcut_buf[MAX_SHORTCUT_NUM];
#endif
};

/**
 * @brief Process abstract operator stream
 *
 * This function process all operators by calling low layer routine of computing device.
 *
 * @param  dev             handle of computing device
 *
 * @retval                 None
 */
struct easynet_dev *easynet_ops_init(void);
int  easynet_ops_process(struct easynet_dev *dev);
void easynet_process_deinit(struct easynet_dev *dev);

typedef struct kdp_processor
{
    unsigned short int *network_input;
    unsigned short *network_output;
    char *network_output_vector;
    unsigned char *cmd_addr_start;
    unsigned char *weight_addr_start;
} kdp_processor_t;

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
    void    *weight_seq_end_addr;   /*< End weight addr of one command sequence */
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
    uint32_t start_out_col;  /*< Start col in output bank */
    uint32_t input_in_bank;  /*< If input data is already all in inout bank */
    uint32_t pipeline_up;    /*< Occupied pipeline status */
    uint32_t pipeline_down;  /*< Empty pipeline status */
    uint32_t simu_start_out_ch;  /*< Start output channel for this command sequence */
    int16_t *layer_buffer[MAX_LAYER_BUFFER_NUM]; /*<Layer buffer allocated by user */
    int16_t *shortcut_buffer[MAX_SHORTCUT_NUM]; /*< Buffer for shortcut layer */
    uint32_t shortcut[MAX_LAYER_BUFFER_NUM]; /*< Layers of each shorcut buffer, zero if not valid */
    uint32_t out_layer;       /*< Layers of output */
    uint8_t *image_input;     /*< Input of convolution */
    uint8_t *user_output;     /*< Output of convolution */
    int32_t  verified_layer;  /*< Last verified layer for LAYER_BY_LAYER_DEBUG */
    float   *yolo_out_buffer[MAX_YOLO_OUT_NUM]; /*< Buffer for yolo layer */
    //struct hw_debug_helper debug_helper;    /* helper to check the process of calculating about simulator */
    int32_t output_flag;

};

#define BANK_OP_MAX_STAGE 8

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
    uint32_t row_num;         /*< Valid input row for bank op */
    uint32_t col_num;         /*< Valid input column for bank op */
    uint32_t start_row_num;   /*< Valid start input row for bank op */
    uint32_t start_col_num;   /*< Valid start input column for bank op */
    uint32_t hardware_out_row;/*< Hardware output height for pool bank op */
    uint32_t hardware_out_col;/*< Hardware output width  for pool bank op */
    int32_t  hardware_start_out_col;/*< Hardware start output col */
    uint32_t pool_pad_up;     /*< Software pool layer pad */
    uint32_t pool_pad_left;   /*< Software pool layer pad */
    //layer   *shortcut_layer;  /*< Banked op for shortcut layer */
    //layer   *route_layer;     /*< Banked op for route layer */
    int32_t  activation;
#if 0
    uint32_t extra_relu_en;   /*< Enable relu activation */
    uint32_t extra_relu_num;  /*< Leaky relu paramter */
#endif
    uint32_t stage;           /*< Which layer of each stage */
    int32_t  stage_layer_num[BANK_OP_MAX_STAGE]; /*< Layer number of each bank-op layer */
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
    struct bank_op_context bank_op;
};

#define INVALID_BUFFER_IDX     0xff
#define BANK_COPY_CHANNEL 8

#define BANK_EXCHANGE_CHANNEL 9


void bubleSort(Box_t data[], int n);
float box_iou(Box_t a, Box_t b);

#endif



