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

#include "lib/math.h"
#include "lib/algo.h"
#include "lib/print.h"
#include "worker.h"
#include "post_worker.h"
#include "painter_worker.h"

int post_start_cap(int argc, char *argv[], void *data)
{
    worker_t *self = (worker_t *)data;
    post_worker_t *post = self->data;
    k_mutex_lock(&post->mutex, K_FOREVER);
    post->mode = MODE_POST_CAP;
    k_mutex_unlock(&post->mutex);
    post->cap_cnt = 0;
    printf("post_worker_start_import\n");
    return 0;
}

int post_stop_cap(int argc, char *argv[], void *data)
{
    worker_t *self = (worker_t *)data;
    post_worker_t *post = self->data;
    printf("post_worker_stop_import\n");
    paint_info_t info;
    char buf[32];
    info.x = 0;
    info.y = 0;
    sprintf(buf, "%d/%d", post->total_imgs_cnt, post->available_size / VECTOR_SPACE_SIZE);
    info.str = buf;
    info.color = 0xFF0000FF;
    do_next_process(self, CMD_PAINT_CAP, &info);
    post->person_cnt++;
    //do_next_process(self, CMD_PAINT_CLEAR, 0);
    return 0;
}

static void post_save_header(post_worker_t *post)
{
    worker_io_t info;
    worker_t *loader = get_worker("loader");
    post->header.magic    = 0x12345678;
    post->header.version  = 0x0001;
    post->header.label_cnt = 1;
    strcpy(post->header.label[0].name, "temp");

    info.src = &post->header;
    info.dst = BASE_BIN_FLASH_START;
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
    printf("magic:0x%x version:%d label_cnt:%d\n", header->magic, header->version, header->label_cnt);
    if (header->magic != 0x12345678 || label->cnt > 10000)
    {
        header->label_cnt = 1;
        label->cnt = 0;
        post_save_header(post);
    }
    data_off += sizeof(base_header_t);
    for (i = 0; i < header->label_cnt; i++)
    {
        for (j = 0; j < label->cnt; j++)
        {
            post_import_vector(post, data_off, i);
            data_off += vector_size;
        }
        post->person_cnt++;
    }
    return data_off;
}

static void post_save_base(post_worker_t *post, float *vector)
{
    worker_io_t info;
    worker_t *loader = get_worker("loader");

    post->header.label[0].cnt  = post->person_img_cnt;
    info.src = &post->header;
    info.dst = BASE_BIN_FLASH_START;
    info.size = sizeof(base_header_t);
    loader->run(loader, CMD_LOADER_WRITE, &info);

    info.src = vector;
    info.dst = BASE_BIN_FLASH_START + sizeof(base_header_t) + (post->total_imgs_cnt - 1) * FC_OUT * sizeof(float); //0x0064041c;//
    info.size = FC_OUT * sizeof(float);
    loader->run(loader, CMD_LOADER_WRITE, &info);
}

static int post_clear_base(post_worker_t *post)
{
    int i = 0;
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
}

void post_import_vector(post_worker_t *post, float *data, int label)
{
    node_t *currentNode;
    int i = 0;
    float *vector = kmalloc(FC_OUT * sizeof(float));
    memcpy(vector, data, FC_OUT * sizeof(float));
    if (post->previous_node == NULL)
    {
        post->previous_node = kmalloc(sizeof(node_t));
        if (post->previous_node == NULL)
        {
            printf("Failed to malloc_ext\b\r");
            return (-1);
        }
        post->node_head = post->previous_node;
        currentNode = post->node_head;
    }
    else
    {
        currentNode = kmalloc(sizeof(node_t));
        if (currentNode == NULL)
        {
            printf("Failed to malloc_ext\b\r");
            return (-1);
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

static int post_calc_distance(worker_t *self, float *vector, float *dist)
{
    post_worker_t *post = self->data;
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
    //printf("THRESHOLD: %f FC_CHANNEL:%d\n", prev_sum, FC_CHANNEL);

    while (currentNode != NULL)
    {
        sum = 0;
        for (i = 0; i < FC_OUT; ++i)
        {
            v = currentNode ->baseVector[i] - vector[i];
            sum += v * v;
        }
        sum = sqrt_ex(sum);
        //if (post->debug_print)
        //{
        //    printf("current base: %d\tand dist: %f\n", currentNode ->label, sum);
        //}
        if (sum < min)
        {
            min = sum;
            *dist = min;
        }
        if (sum <= post->threshold)
        {
            return person_label;
        }
        currentNode = currentNode ->next;
    }
    return -1;
}

static int post_worker_init(worker_t *self, void *param)
{
    post_worker_t *post = kmalloc(sizeof(post_worker_t));
    //printf("sizeof post :%d %d\n", sizeof(post_worker_t), sizeof(distance_calc_t));
    memset(post, 0, sizeof(post_worker_t));
    post->threshold = THRESHOLD;
    k_mutex_init(&post->mutex);

    post_clear_base(post);
    post->debug_print = 1;
    self->data = post;
    post_load_base(post, param);
    post->available_size = dev_available_size();
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
        return -1;
    }
    return 0;
}

static int retain_type = 0;
static int retain_frames = 5;

static int post_worker_run(worker_t *self, int cmd, void *param)
{

    post_worker_t *post = self->data;

    if (cmd == CMD_POST_NORMALFRAME)
    {
        //printf("CMD_POST_NORMALFRAME\n");
        if (retain_type)
        {
            retain_frames--;
            if (retain_frames <= 0)
            {
                post->need_clean_ui = 1;
                if (retain_type == 1)
                {
                    k_mutex_lock(&post->mutex, K_FOREVER);
                    post->mode = MODE_POST_PREDICT;
                    k_mutex_unlock(&post->mutex);
                }
                retain_type = 0;
            }
        }
        if (post->need_clean_ui)
        {
            do_next_process(self, CMD_PAINT_CLEAR, 0);
            post->need_clean_ui = 0;
        }
    }
    else if (cmd == CMD_EVENT_SHORTPRESS)
    {
        post_start_cap(0, 0, self);
        return 0;
    }
    else if (cmd == CMD_EVENT_LONGPRESS)
    {
        //printf("post long press\n");
        post->available_size = dev_available_size();
        post_clear_base(post);
    }
    else if (cmd == CMD_POST_FACEFRAME)
    {
        float *vector = param;
        int i = 0;
        if (post_check_vector(vector))
        {
            return -1;
        }

        unsigned int t_last, t_now, delta;
        t_last = k_cycle_get_32();
        float dist;
        int label = post_calc_distance(self, vector, &dist);
        t_now = k_cycle_get_32();
        delta = t_now - t_last;
        //printf("post_calc_distance cost %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);
        k_mutex_lock(&post->mutex, K_FOREVER);
        int mode = post->mode;
        k_mutex_unlock(&post->mutex);
        if (mode == MODE_POST_PREDICT)
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
                info.color = 0xFF0000FF;
                do_next_process(self, CMD_PAINT_PASS, &info);
                post->need_clean_ui = 0;
                retain_frames = 2;
                retain_type = 2;
            }
            else
            {
                info.color = 0xFFFFFF00;
                do_next_process(self, CMD_PAINT_TEXT, &info);
                post->need_clean_ui = 1;
            }
        }
        else if (mode == MODE_POST_CAP)
        {
            post_import_vector(post, vector, 0);
            post->cap_cnt++;
            if (post->cap_cnt == 1)
            {
                post_stop_cap(0, 0, self);
                post->need_clean_ui = 0;
                retain_frames = 5;
                retain_type = 1;
            }
            post_save_base(post, vector);
            k_mutex_lock(&post->mutex, K_FOREVER);
            post->mode = MODE_POST_IDLE;
            k_mutex_unlock(&post->mutex);
        }
    }
    //do_next_process(self, 0, param);
}

static int post_worker_deinit(worker_t *self)
{
    kfree(self->data);
}

worker_t post_worker =
{
    .run    = post_worker_run,
    .init   = post_worker_init,
    .deinit = post_worker_deinit,
};
