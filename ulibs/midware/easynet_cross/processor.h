#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
//#include "ai_common.h"
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
 *  Abstract Device Running Operators
 */
struct easynet_dev
{
    void *param;
    int (*process)(struct easynet_dev *dev, struct easynet_ops_param *);
    void (*load)(struct easynet_dev *dev, void *bin, uint32_t len, void *input);
    uint32_t cur_addr;
    uint32_t len;
    unsigned char *input;
    void *context;
    void *predict_out;
    int  network_type;
    int  isfaceai;  //0: not faceai;    1 : faceai
    float *yolo_out_buffer[2];
    int16_t *shortcut_buf[MAX_SHORTCUT_NUM];
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
struct easynet_dev *easynet_process_update(unsigned char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage);
struct easynet_dev *easynet_process_init(char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage);

typedef struct kdp_processor
{
    unsigned short int *network_input;
    unsigned short *network_output;
    char *network_output_vector;
    unsigned char *cmd_addr_start;
    unsigned char *weight_addr_start;
} kdp_processor_t;


//void bubleSort(Box_t data[], int n);
//float box_iou(Box_t a, Box_t b);

#endif
