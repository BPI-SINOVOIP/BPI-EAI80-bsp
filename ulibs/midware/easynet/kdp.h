#ifndef __KDP_H__
#define __KDP_H__

#if 0
    #define DEBUG_KDP_LEVEL_0               1
    #define DEBUG_KDP_LEVEL_1               1
    #define DEBUG_KDP_LEVEL_2               2
    #define DEBUG_KDP_LEVEL_3               3
    #define DEBUG_KDP_ALL                     8
#endif

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

/*
 * Address Space for KDP310 IP
 */
#define BRAM_OUTPUT_A_BASEARDDR     0x20020000
#define BRAM_OUTPUT_B_BASEARDDR     0x20030000
#define BRAM_OUTPUT_C_BASEARDDR     0x20040000
#define BRAM_OUTPUT_D_BASEARDDR     0x20050000
#define BRAM_CMD_BASEARDDR          0x20060000

#define KDP_CTRL        0x46000000
#define KDP_STATUS      0x46000004
#define SRAM_SIZE          128*1024
#define SPAD_SIZE          16*1024

#define FORCE_COL_NUM_EVEN


#define MAX_SHORTCUT_NUM       4
#define MAX_YOLO_OUT_NUM     3
#define MAX_LAYER_BUFFER_NUM  5
#define INVALID_BUFFER_IDX     0xff
#define MAX_CROSS_NUM          6

/* sync word for each operator */
#define SYNC_WORD 0x78515255

/*
 * Abstract operator and parameters
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
    OP_GAUSSIAN_YOLO,
    OP_FINAL_INPUT_CHANNEL_CUT,
    OP_REORG,
    OP_CHECK_LAYER,
} OP_TYPE;

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
    int32_t layer_num; /*< for debug */
    int32_t buffer;    /*< input buffer */
    int32_t dst_buffer;/*< output buffer */
    uint32_t col;      /*< input column */
    uint32_t row;      /*< input row */
    uint32_t ch;       /*< input channel */
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
 * Operator parameter of gaussian_yolo
 */
struct op_gaussian_yolo_param
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
    int32_t out_buffer;   /*< output buffer */
    int32_t ch_num;       /*< input  channel */
    int32_t row_num;      /*< input  row */
    int32_t col_num;      /*< input  column */
    int32_t out_row;      /*< output row */
    int32_t out_col;      /*< output column */
    int32_t stride;       /*< upsample stride */
};

/*
 * Operator parameter of reorg
 */
struct op_reorg_param
{
    int32_t layer_num;    /*< for debug */
    int32_t buffer;       /*< input buffer */
    int32_t out_buffer;   /*< output buffer */
    int32_t col_num;      /*< input  column */
    int32_t row_num;      /*< input  row */
    int32_t ch_num;       /*< input  channel */
    int32_t batch;        /*< reorg  batch */
    int32_t stride;       /*< reorg  stride */
    int32_t forward;      /*< reorg forward */
};

/*
 * Operator parameter of full connected soft operation
 */
struct op_connected_param
{
    int32_t layer_num;      /*< for debug */
    int32_t buffer;         /*< input buffer */
    int32_t dst_buffer;     /*< output buffer */
    int32_t in_ch;          /*< Input size */
    int32_t out_ch;         /*< Output size */
    uint32_t weight_offset; /*<  Start of weight array */
    uint32_t weight_len;    /*< weight length */
    int32_t activation;
    int32_t batch_normalize;
    int32_t before_bn_ch;
};

/*
 * Operator parameter of image input
 */
struct op_image_input_param
{
    uint32_t ch;       /*< input channel */
    uint32_t row;      /*< input row */
    uint32_t col;      /*< input column */
    uint32_t fmt;      /*< data format, 0:int16, 1:float */
};

/*
 * Operator parameter of conv output
 */
struct op_output_param
{
    uint32_t out_ch;       /*< output channel */
    uint32_t out_row;      /*< output row */
    uint32_t out_col;      /*< output column */
    uint32_t buffer;       /*< output buffer */
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
    int32_t  out_buffer;  /*< Output buffer selection */
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
 * Operator parameter of layer indicator
 */
struct op_layer_check_param
{
    uint32_t end_layer;
    uint32_t conv;        /*< network conv layer */
    int32_t  out_buffer;  /*< Output buffer selection */
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

enum
{
    FMT_INT16 = 0,
    FMT_FLOAT = 1,
};

struct conv_hw_context
{
    uint32_t last_layer_out_fmt;    /* 0: int16, 1: float */
    //struct simu_dumper dumper;
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
    uint32_t last_out_row;   /*< Total output row for last layer */
    uint32_t last_out_col;   /*< Total output column for last layer */
    uint32_t last_out_ch;    /*< Total output channel for last layer */
    uint32_t last_out_buffer;/*< Output buffer index for last layer */
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
    uint32_t layer_number;    /*< Start conv layer of one combined layer */
    uint8_t *image_input;     /*< Input of convolution */
    uint8_t *user_output;     /*< Output of convolution */
    int32_t  verified_layer;  /*< Last verified layer for LAYER_BY_LAYER_DEBUG */
    float   *yolo_out_buffer[MAX_YOLO_OUT_NUM]; /*< Buffer for yolo layer */
};

#endif

