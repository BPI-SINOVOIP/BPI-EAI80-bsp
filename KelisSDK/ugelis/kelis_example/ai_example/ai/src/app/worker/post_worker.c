/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          post_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer worker to do post stage process
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <stdio.h>
#include <string.h>

#include "lib/math.h"
#include "lib/algo.h"
#include "lib/print.h"
#include "worker.h"
#include "post_worker.h"
#include "painter_worker.h"
#include "dev/dev.h"

#define THRESHOLD           0.75//DEBUG
#define FC_OUT              256

static void post_save_base(post_worker_t *post, float *vector);
static int post_clear_base(post_worker_t *post);
void post_import_vector(post_worker_t *post, float *data, int label);

void post_start_cap(post_worker_t *post)
{
    k_mutex_lock(&post->mutex, K_FOREVER);
    post->mode = MODE_POST_CAP;
    k_mutex_unlock(&post->mutex);
}

void post_start_clear(post_worker_t *post)
{
    post->available_size = dev_available_size();
    post_clear_base(post);
}

void post_enter_predict(post_worker_t *post, int label, float dist)
{
    paint_info_t info;
    char buf[32];
    info.x = 0;
    info.y = 0;
    memset(buf, 0, 32);
    sprintf(buf, "%.3f", dist);
    info.str = buf;
    if (label >= 0)
    {
        info.w = CAM_DISP_W;
        info.h = CAM_DISP_W / 10;
        info.x = CAM_DISP_X;
        info.y = CAM_DISP_H - info.h ;
        info.color = 0xFF0000FF;
        post->painter->run(post->painter, CMD_PAINT_PASS, &info);
    }
    else
    {
        info.color = 0xFFFFFF00;
        post->painter->run(post->painter, CMD_PAINT_TEXT, &info);
    }
}

void post_enter_cap(post_worker_t *post, float *vector)
{
    paint_info_t info;

    k_mutex_lock(&post->mutex, K_FOREVER);
    post->mode = MODE_POST_PREDICT;
    k_mutex_unlock(&post->mutex);

    post_import_vector(post, vector, 0);
    post_save_base(post, vector);

    char buf[32];
    sprintf(buf, "%d/%d", post->total_imgs_cnt, post->available_size / VECTOR_SPACE_SIZE);
    info.str = buf;
    info.color = 0xFF0000FF;
    info.w = CAM_DISP_W;
    info.h = CAM_DISP_W / 10;
    info.x = CAM_DISP_X;
    info.y = CAM_DISP_H - info.h;
    post->painter->run(post->painter, CMD_PAINT_CAP, &info);
    post->person_cnt++;
}

static void post_save_header(post_worker_t *post)
{
    worker_io_t info;
    worker_t *loader = get_worker("loader");
    post->header.magic    = 0x12345678;
    post->header.version  = 0x0001;
    post->header.label_cnt = 1;
    strcpy(post->header.label[0].name, "temp");

    info.src = (int)&post->header;
    info.dst = post->base_ofst;
    info.size = sizeof(base_header_t) -16;
    loader->run(loader, CMD_LOADER_WRITE, &info);
}

/*
    bin struction:
    Low Bytes ---> High Bytes
    |------------------------------Header------------------------------|-------------data-----------|
    |MAGIC   |VER.|LabelCnt |LabelName0|LabelName1|...|LabelName[Cnt-1]|Pic0     |Pic1|Pic2|...|PicN|
    |4Bytes  |4B  |4B       |16B       |16B       |...|16B             |(w*h*3+2)|    |    |...|    |
    |ANGRAD__|1.0 |         |          |          |...|                |         |    |    |   |    |

*/

int post_load_base(post_worker_t *post, char *contents)
{
    int i = 0, j = 0;
    char *data_off;
    int vector_size = FC_OUT * sizeof(float);

    base_header_t *header = &post->header;
    label_t *label = &header->label[0];
    data_off = contents;
    memcpy(header, data_off, sizeof(base_header_t));
    if (header->magic != 0x12345678 || label->cnt > 10000)
    {
        printf("magic:0x%x version:%d label_cnt:%d\n", header->magic, header->version, header->label_cnt);
        header->label_cnt = 1;
        label->cnt = 0;
        post_save_header(post);
    }
    data_off += sizeof(base_header_t);
    for (i = 0; i < header->label_cnt; i++)
    {
        for (j = 0; j < label->cnt; j++)
        {
            post_import_vector(post, (float *)data_off, i);
            data_off += vector_size;
        }
        post->person_cnt++;
    }
    return (int)data_off;
}

static void post_save_base(post_worker_t *post, float *vector)
{
    worker_io_t info;
    worker_t *loader = get_worker("loader");

    post->header.label[0].cnt  = post->person_img_cnt;
    info.src = (int)&post->header;
    info.dst = post->base_ofst;
    info.size = sizeof(base_header_t);
    loader->run(loader, CMD_LOADER_WRITE, &info);

    info.src = (int)vector;
    info.dst = post->base_ofst + sizeof(base_header_t) + (post->total_imgs_cnt - 1) * FC_OUT * sizeof(float); //0x0064041c;//
    info.size = FC_OUT * sizeof(float);
    loader->run(loader, CMD_LOADER_WRITE, &info);
}

static int post_clear_base(post_worker_t *post)
{
    node_t *currentNode, *last;
    currentNode = post->node_head;
    while (currentNode)
    {
        last = currentNode;
        currentNode = currentNode ->next;
        if (last->baseVector)
        {
            kfree(last->baseVector);
            last->baseVector = NULL;
        }
        kfree(last);
        last = NULL;
    }
    post->node_head = NULL;
    post->previous_node = NULL;
    //memset(&post->dist, 0, sizeof(distance_calc_t));
    post->person_img_cnt = 0;
    post->person_cnt = 0;
    post->total_imgs_cnt = 0;
    return 0;
}

void post_import_vector(post_worker_t *post, float *data, int label)
{
    node_t *currentNode;
    float *vector = (float *)kmalloc(FC_OUT * sizeof(float));
    memcpy(vector, data, FC_OUT * sizeof(float));
    if (post->previous_node == NULL)
    {
        post->previous_node = (node_t *)kmalloc(sizeof(node_t));
        if (post->previous_node == NULL)
        {
            printf("Failed to malloc_ext\b\r");
            return;
        }
        post->node_head = post->previous_node;
        currentNode = post->node_head;
    }
    else
    {
        currentNode = (node_t *)kmalloc(sizeof(node_t));
        if (currentNode == NULL)
        {
            printf("Failed to malloc_ext\b\r");
            return;
        }
        post->previous_node->next = currentNode;
    }
    currentNode->label = label;
    currentNode->next = NULL;

    currentNode->baseVector = vector;
    post->previous_node = currentNode;
    post->total_imgs_cnt++;
    post->person_img_cnt++;
    post->available_size -= VECTOR_SPACE_SIZE ;
}

static int post_calc_distance(post_worker_t *post, float *vector, float *dist)
{
    int i = 0;
    float v, sum;
    float min = 1000.f;
    node_t *currentNode = post->node_head;
    int person_label = post->person_cnt;
#if 0
    printf("vectors: ");
    for (i = 0; i < 10; i++)
    {
        printf("%f ", vector[i]);
    }
    printf("\n");
#endif

    while (currentNode != NULL)
    {
        sum = 0;
        for (i = 0; i < FC_OUT; ++i)
        {
            v = currentNode ->baseVector[i] - vector[i];
            sum += v * v;
        }
        sum = sqrt_ex(sum);
        if (post->debug_print)
        {
            printf("current base: %d\tand dist: %f\n", currentNode ->label, sum);
        }
        if (sum < min)
        {
            min = sum;
            *dist = min;
        }
        if ((sum > 0.01f) && (sum <= post->threshold))
        {
            return person_label;
        }
        currentNode = currentNode ->next;
    }
    return -1;
}

static int post_init_base(post_worker_t *post, int dst)
{
    worker_io_t info;
    worker_t *loader = get_worker("loader");

    info.src   = post->base_ofst;
    info.size  = post->base_size;
    info.dst   = dst;
    loader->run(loader, CMD_LOADER_READ, &info);

    return 0;
}

static int post_worker_init(worker_t *self, void *param)
{
    post_worker_t *post = (post_worker_t *)kmalloc(sizeof(post_worker_t));
    int base_contents;

    memset(post, 0, sizeof(post_worker_t));

    post->painter   = get_worker("painter");
    post->threshold = THRESHOLD;
    k_mutex_init(&post->mutex);

    post->base_ofst = ((worker_post_param_t *)param)->base_ofst;
    post->base_size = ((worker_post_param_t *)param)->base_size;

    post_clear_base(post);
    post->debug_print = 0;

    base_contents = (int)kmalloc(post->base_size);
    post_init_base(post, base_contents);
    post_load_base(post, (char *)base_contents);
    kfree((void *)base_contents);

    post->available_size = dev_available_size();

    self->data = post;

    return 0;
}

static inline float fabs_ex(float a)
{
    return a > 0 ? a : -a;
}
static int post_check_vector(float *vector)
{
    int i = 0;
    float max = 0;
    for (i = 0; i < FC_OUT; i++)
    {
        float a = fabs_ex(vector[i]);
        if (a > max)
        {
            max = a;
        }
    }
    if (max > 100)
    {
        printf("vector error occurs, but ignore that\n");
        return -1;
    }
    return 0;
}

static int post_worker_run(worker_t *self, int cmd, void *param)
{

    post_worker_t *post = self->data;

    if (cmd == CMD_EVENT_SHORTPRESS)
    {
        post_start_cap(post);
    }
    else if (cmd == CMD_EVENT_LONGPRESS)
    {
        post_start_clear(post);
    }
    else if (cmd == CMD_POST_FACEFRAME)
    {
        float *vector = param;
        if (post_check_vector(vector))
        {
            return -1;
        }

        int mode = post->mode;
        if (mode == MODE_POST_PREDICT)
        {
            float dist = 8.24f;
            int label = post_calc_distance(post, vector, &dist);
            post_enter_predict(post, label, dist);
        }
        else if (mode == MODE_POST_CAP)
        {
            post_enter_cap(post, vector);
        }
    }
    return 0;
}

static int post_worker_deinit(worker_t *self)
{
    post_worker_t *post = self->data;
    post_clear_base(post);
    kfree(post);
    return 0;
}

worker_t post_worker =
{
    .run    = post_worker_run,
    .init   = post_worker_init,
    .deinit = post_worker_deinit,
};
