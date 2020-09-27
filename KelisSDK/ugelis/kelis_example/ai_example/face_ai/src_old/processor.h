#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "kernel.h"
#include <gm_hal_kdp.h>

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
/**
 * @}
 */

#define HW_CMD_MAX_SIZE   0x10000
#define HW_OP_MAX_SIZE    0x10000

#define MAX_SHORTCUT_NUM   4

#define BANK_A_ADDRESS     0x20020000
#define BANK_B_ADDRESS     0x20030000

#define BANK_C_ADDRESS     0x20040000
#define BANK_D_ADDRESS     0x20050000

#define BRAM_CMD_BASEARDDR          0x20060000
#define KDP_STATUS                  0x46000004


/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
/*
 * The following constant controls the length of the buffers for each layer
 */
#define INPUT_WIDTH         96
#define INPUT_HEIGHT        96
#define TEST_BUFFER_SIZE    ((INPUT_WIDTH * INPUT_HEIGHT * 3) + 2)
#define OUTPUT_WIDTH        3
#define OUTPUT_HEIGHT       3
#define FC_CHANNEL          512
#define FC_OUT              256

#define SIZE_1K             (1<<10)
#define OPS_BIN_SIZE        (5917120)

#define WEI_BIN_SIZE        (5917120) //(6*SIZE_1M) //(5917120) //(6*SIZE_1M) //(6*SIZE_1M) //(5917120) //(6*SIZE_1M) //(5917120) //(6*SIZE_1M)   //  (6*SIZE_1M) 

#define AI_FLASH_START_OFFSET  (0x00050000)
#define OPS_BIN_FLASH_START (AI_FLASH_START_OFFSET)
#define IMAGE_BIN_FLASH_START (OPS_BIN_FLASH_START+0x5B0000)


#if defined(F6721B_EVB_176PIN)
    #define BASE_SIZE_RECODRD   (0xB0000)
    #define RECORD_BACKUP_CNT   (100)
    #define CAPTURE_BACKUP_CNT  (10)
    #define VALIDATE_BACKUP_CNT (3)
#endif
#if defined(F6721B_EVB_QFN80PIN )
    #define BASE_SIZE_RECODRD   (691330)
    #define RECORD_BACKUP_CNT   (30)
    #define CAPTURE_BACKUP_CNT  (7)
    #define VALIDATE_BACKUP_CNT (2)
#endif
//#define RECORD_BACKUP_CNT   (50)
#define RECORD_BACKUP_SIZE  (TEST_BUFFER_SIZE * RECORD_BACKUP_CNT)


#define CAPTURE_BACKUP_SIZE (TEST_BUFFER_SIZE * CAPTURE_BACKUP_CNT)


#define VALIDATE_BACKUP_SIZE (TEST_BUFFER_SIZE * VALIDATE_BACKUP_CNT)

#define WEIGHT_LENGTH       (0x5045c0 + 0x20000)
#define FIFO_WEIGHT_LENGTH  WEIGHT_LENGTH

#define THRESHOLD           0.72
#define BUFFERSIZE          55296

#define FC_W_BASEADDR               (WEI_BASE + WEIGHT_LENGTH)

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

/*
 * Abstract operator and parameters
 */
typedef struct kdp_processor
{
    unsigned short int *network_input;
    unsigned short *network_output;
    char *network_output_vector;
    unsigned char *cmd_addr_start;
    unsigned char *weight_addr_start;
} kdp_processor_t;

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
 * Operator parameter of shortcut operation
 */
struct op_activation_param
{
    int32_t layer_num;    /*< for debug */
    int32_t buffer;       /*< Input buffer selection */
    int32_t activation;   /*< Activation type */
    int32_t out_cnt;
};

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
} OP_TYPE;

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
void FC_Init();
void FC(float *fcVector);

void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow);

#endif

