/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        layer.h
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       LAYER Header For easynet
 *
 * @note
 *              2019/01/24, Angrad.Yang, v1.0.0
 *                  Initial version.
 *              2019/04/03, Angrad.Yang, v1.0.1
 *                  Add fix8 data.
 *
 **/

#ifndef __LAYER_H__
#define __LAYER_H__

#include "data_type.h"
#include "activations.h"

struct network;
typedef struct layer layer;
typedef struct network network;
struct layer;

typedef enum
{
    CONVOLUTIONAL,
    CONNECTED,
    MAXPOOL,
    SOFTMAX,
    DETECTION,
    DROPOUT,
    CROP,
    COST,
    NORMALIZATION,
    AVGPOOL,
    LOCAL,
    SHORTCUT,
    ACTIVE,
    BATCHNORM,
    REGION,
    YOLO,
    NETWORK,
    BLANK,
    DEPTHWISE_CONVOLUTIONAL,
    ROUTE,
} LAYER_TYPE;

typedef enum
{
    WEIGHT_FLOAT,
    WEIGHT_INT8,
    WEIGHT_NPU,
} WEIGHT_TYPE;

typedef struct layer
{
    LAYER_TYPE type;
    int h;
    int w;
    int in_ch;
    int out_ch;             //output channel
    int pad;
    int stride;
    int kernel_size;
    int out_w;
    int out_h;
    int out_cnt;
    int in_cnt;
    int dontload;
    int weight_type; 
    int workspace_size;

    float *weights;
    float *biases;
    float *output;
    float bflops;
	void (*forward)(struct layer,  struct network);
    ACTIVATION activation;

    //batchborm
    float *mu;
    float *sigma;
    float *gamma;
    float *beta;

    //yolo
    int classes;
    int *mask;
    int layers;
    int n;
    int * input_layers;
    int * input_sizes;
	int index;
    float shcut_alpha;
    float shcut_beta;
	int batch_normalize;
	float temperature;
    int *indexes;
} layer;

char *layer_type_to_string(LAYER_TYPE t);

#endif //__LAYER_H__


