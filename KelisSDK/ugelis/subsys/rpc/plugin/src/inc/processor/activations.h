/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        activations.h
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       ACTIVATIONS Header For easynet
 *
 * @note
 *              2019/01/24, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef __ACTIVATIONS_H__
#define __ACTIVATIONS_H__

#include "data_type.h"

typedef enum
{
    RELU, LEAKY, LOGISTIC, LINEAR
} ACTIVATION;

ACTIVATION get_activation(char *s);

float activate(float x, ACTIVATION a);
void activate_array(float *x, const int n, const ACTIVATION a);

data_t activate_fix8(data_t x, ACTIVATION a);
void activate_array_fix8(data_t *x, const int n, const ACTIVATION a);

#endif //__ACTIVATIONS_H__

