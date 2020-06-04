#ifndef __OPS_H__
#define __OPS_H__

#include "kdp.h"

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
    float thresh;
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
    struct easynet_dev_cfg *cfg;
    void *predict_out;
};


typedef struct kdp_processor
{
    unsigned short int *network_input;
    unsigned short *network_output;
    unsigned char *cmd_addr_start;
    unsigned char *weight_addr_start;
} kdp_processor_t;

#endif