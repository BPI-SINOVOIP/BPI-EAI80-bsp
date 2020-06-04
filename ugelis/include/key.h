/*
 * Copyright (c) 2017 ARM Ltd
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public APIs for MMC drivers
 */

#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <misc/__assert.h>
#include <misc/slist.h>

#include <stdint.h>
#include <stddef.h>
#include <device.h>



struct key_input_event {
    bool status;
    uint8_t type ;
    uint32_t code ;
};

int read_key_event(struct key_input_event *event, int timeout);





#ifdef __cplusplus
}
#endif


#endif /* _MMC_H_ */


