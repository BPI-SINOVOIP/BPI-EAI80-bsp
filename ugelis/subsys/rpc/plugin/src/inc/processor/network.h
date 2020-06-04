/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        network.h
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       NETWORK Header For easynet
 *
 * @note
 *              2019/01/24, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "layer.h"
#ifdef RICHARD
#include "box.h"
#endif

typedef struct network
{
    data_t *workspace;
	float *input;
    int n;
    layer *layers;
    int out_cnt;
    float *output;
    int in_cnt;
    int h, w, in_ch;
	unsigned int *seen;
    int index;
	data_t *fix8_input;
	uint32_t *npu_weight;
	uint32_t npu_weight_len;

} network;

int get_network_output_size(network *net);
float *get_network_output(network *net);
float *network_predict(network *net, data_t *in);
void free_network(network *net);
//detection *get_network_boxes(network *net, int w, int h, float thresh, int *num);
int yolo_num_detections(layer l, float thresh);
network *make_network(int n);

/*
 * The following api support convolution verification step by step
 */
void forward_network_step_start(network net, float *input);
void forward_network_one_layer(int32_t layer_num);
int  conv_verify_step(int out_ch, int in_ch, int out_row, int out_col, int shift_addr_x, int shift_addr_y, float result, float precision);  
int  layer_verify_step(int32_t layer_num, int out_ch, int out_row, int out_col, float result, float precision);   
void forward_convolutional_layer_start(void *l, network *net);     
void forward_convolutional_layer_end(void *vl, network *net);
#endif //__NETWORK_H__
