/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          post_worker.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of post worker
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __POST_WORKER_H__
#define __POST_WORKER_H__

#include <inc/config.h>
#include <processor.h>

#include "worker.h"

enum
{
    MODE_POST_PREDICT,
    MODE_POST_CAP,
    MODE_POST_IDLE,
};

/*
    bin struction:
    Low Bytes ---> High Bytes
    |------------------------------Header------------------------------|-------------data-----------|
    |MAGIC   |VER.|LabelCnt |LabelName0|LabelName1|...|LabelName[Cnt-1]|Pic0     |Pic1|Pic2|...|PicN|
    |8Bytes  |6B  |2B       |16B       |16B       |...|16B             |(w*h*3+2)|    |    |...|    |
    |ANGRAD__|1.0 |         |          |          |...|                |         |    |    |   |    |

    Data:
    |------------------------------Pic0--------------------------------------------------|
    |         w*h          |       w*h            |              w*h     |1B   |1B       |
    |RRRRRRRRRRRRRRRRRR...R|GGGGGGGGGGGGGGGGGG...G|BBBBBBBBBBBBBBBBBB...B|Label|Last Flag|
*/


#define VECTOR_SPACE_SIZE 1040 //FC_OUT * sizeof(float) + sizeof(node_t)

typedef struct label
{
    char name[12];
    unsigned int cnt;
} label_t;

typedef struct base_header
{
    int magic;
    int version;
    int label_cnt;
    label_t label[1];
} base_header_t;

typedef struct post_worker
{
    int person_img_cnt;
    int total_imgs_cnt;
    int person_cnt;
    float threshold;                /* to adjust the network_predict confidence */
    node_t *node_head;
    node_t *previous_node;
    int debug_print;
    int mode;
    struct k_mutex mutex;
    base_header_t header;
    int available_size;

    int base_ofst;
    int base_size;

    worker_t *painter;
} post_worker_t;


#endif //__POST_WORKER_H__

