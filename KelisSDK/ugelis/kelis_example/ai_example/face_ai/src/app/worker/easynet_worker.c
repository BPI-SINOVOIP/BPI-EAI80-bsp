/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          easynet.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer to call easynet.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <easynet/easynet.h>

#include "easynet_worker.h"
#include "post_worker.h"
#include "worker.h"

K_THREAD_STACK_DEFINE(easynet_stack_area, 4 * 1024);

static void predict_process(worker_t *self, easynet_worker_t *easynet);

static void easynet_detect_thread(void *data, void *param, void *p3)
{
    printf("easynet_detect_thread\n");
    worker_t *self = (worker_t *)data;
    predict_process(self, self->data);
}

static float *predict(easynet_worker_t *easynet)
{
    char *name = "unknown";
    int ret = 0;
    /* Process all ops */
    do
    {
        //unsigned int t_last, t_now, delta;
        //t_last = k_cycle_get_32();
        ret = easynet_ops_process(easynet->dev);
        //t_now = k_cycle_get_32();
        //delta = t_now - t_last;
        //printf("easynet_ops_process cost %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                AveragePooling(OUTPUT_WIDTH, OUTPUT_HEIGHT, FC_CHANNEL, OUTPUT_WIDTH, OUTPUT_HEIGHT);
                FC((float *)&OutputVectorFC, easynet->ops + WEIGHT_LENGTH);
                easynet_process_update(easynet->ops, &inputImage[0], &OutputImage[0]);
                return OutputVectorFC;
            }
            return 0;
        }
    } while (1);

    return 0;
}

static void predict_process(worker_t *self, easynet_worker_t *easynet)
{
    float *vector = 0;
    while (1)
    {
        int ret = 0;
        ret = k_sem_take(&easynet->predict_lock, 5000);
        if (ret == 0)
        {
            easynet->dev = easynet_process_init(easynet->ops, &inputImage[0], &OutputImage[0]);
            vector = predict(easynet);
            if (vector)
            {
                /* network is finished, to do post work */
                do_next_process(self, CMD_POST_FACEFRAME, vector);
            }
        }
        if (easynet->exit)
        {
            break;
        }
    }
}

static int easynet_worker_init(worker_t *self, void *param)
{
    easynet_worker_t *easynet = kmalloc(sizeof(easynet_worker_t));
    easynet->ops = param;
    easynet->exit = 0;
    k_sem_init(&easynet->predict_lock, 0, UINT_MAX);
    easynet->dev = easynet_process_init(easynet->ops, &inputImage[0], &OutputImage[0]);
    self->data = easynet;

    easynet->dev->isfaceai = 1;

    k_thread_create(&easynet->task_thread, easynet_stack_area,
                    K_THREAD_STACK_SIZEOF(easynet_stack_area),
                    (k_thread_entry_t)easynet_detect_thread,
                    self, 0, 0,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
}

static int easynet_worker_run(worker_t *self, int cmd, void *param)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;
    if (cmd == CMD_EASYNET_DATAREADY)
    {
        easynet->dev->input = param;
        k_sem_give(&easynet->predict_lock);
    }
    else if (cmd == CMD_EASYNET_FRAMEREADY)
    {
        do_next_process(self, CMD_POST_NORMALFRAME, 0);
    }
}

static int easynet_worker_deinit(worker_t *self)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;
    easynet->exit = 1;
    kfree(self->data);
}

worker_t easynet_worker =
{
    .run    = easynet_worker_run,
    .init   = easynet_worker_init,
    .deinit = easynet_worker_deinit,
};
