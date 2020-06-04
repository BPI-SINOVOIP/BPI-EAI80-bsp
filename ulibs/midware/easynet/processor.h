#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
#include <stdint.h>

#include "cross_kdp.h"
#include "kdp.h"
#include "ops.h"

#define IMP_TRANSFER_DATA  1
#define HW_CMD_MAX_SIZE   0x10000
#define HW_OP_MAX_SIZE    0x10000
#define OPS_FILE_MAGIC_NUMBER 0x20198779
#define FLT_MAX 3.402823466e+38F
#define MAX_LAYER_NUM  256


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


typedef enum
{
    TINYYOLOV3 = 3,
    OTHERS
} NETWORK_TYPE;

/**
 * Hardware Operators on Abstract Layer
 */

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


/**
 * @brief Process abstract operator stream
 *
 * This function process all operators by calling low layer routine of computing device.
 *
 * @param  dev             handle of computing device
 *
 * @retval                 None
 */
struct easynet_dev *easynet_process_init(struct easynet_dev_cfg *cfg, unsigned char *ops_data);
int  easynet_ops_process(struct easynet_dev *dev);
struct easynet_dev *easynet_process_update(struct easynet_dev_cfg *cfg, unsigned char *ops_data);

void easynet_process_deinit(struct easynet_dev *dev);

#endif
