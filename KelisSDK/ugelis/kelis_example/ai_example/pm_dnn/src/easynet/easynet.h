#ifndef __EASYNET_H__
#define __EASYNET_H__

#include "kernel.h"

extern unsigned char dnn_first_weight[];
extern unsigned char dnn_second_weight[];
extern float inputImage[];
extern float OutputImage[];

#define HW_CMD_MAX_SIZE   0x10000
#define HW_OP_MAX_SIZE    0x10000
#define OPS_FILE_MAGIC_NUMBER 0x20198779

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

typedef enum
{
    RELU, LEAKY, LOGISTIC, LINEAR
} ACTIVATION;

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
    uint32_t one_weight_size; /*< weight length of one output channel */
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
struct easynet_dev *easynet_process_update(char *seq_buf,  void *pInputImage,  void *pOutputImage);
int  easynet_ops_process(struct easynet_dev *dev);

#endif

