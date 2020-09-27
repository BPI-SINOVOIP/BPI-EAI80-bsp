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

#include <stdio.h>
#include <string.h>

#include "easynet_worker.h"
#include "post_worker.h"
#include "worker.h"

K_THREAD_STACK_DEFINE(easynet_stack_area, 4 * 1024);

#define BUFFERSIZE          (224*224*4)

unsigned short  inputImage[BUFFERSIZE] AI_SECTION;
unsigned short  OutputImage[BUFFERSIZE] AI_SECTION;
unsigned char   WEI_BASE[WEI_BIN_SIZE] AI_SECTION;

static int easynet_worker_process(void *result, worker_t *self)
{
    if (result)
    {
        /* network is finished, to do post work */
        do_next_process(self, CMD_POST_FACEFRAME, result);
    }
    return 0;
}

static int easynet_worker_finish(void *result, worker_t *self)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;
    k_sem_give(&easynet->lock);
    return 0;
}

static int easynet_worker_init(worker_t *self, void *data)
{
    easynet_worker_t *easynet = (easynet_worker_t *)kmalloc(sizeof(easynet_worker_t));
    worker_easynet_param_t *param = (worker_easynet_param_t *)data;

    memset(easynet, 0, sizeof(easynet_worker_t));

    easynet->ep.ops  = param->ops;
    easynet->ep.data = param->data;
    if (param->init)
    {
        param->init(&easynet->ep);
    }

    if (param->process)
    {
        k_sem_init(&easynet->lock, 0, 1);
        easynet->ep.process_cb   = easynet_worker_process;
        easynet->ep.finish_cb    = easynet_worker_finish;
        easynet->task_id = k_thread_create(&easynet->task_thread, easynet_stack_area,
                                           K_THREAD_STACK_SIZEOF(easynet_stack_area),
                                           (k_thread_entry_t)(param->process),
                                           &easynet->ep, self, 0,
                                           K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    }
    easynet->frame_in = param->frame_in;
    easynet->exit     = param->exit;
    self->data = easynet;

    return 0;
}

static int easynet_worker_run(worker_t *self, int cmd, void *param)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;
    if (cmd == CMD_EASYNET_DATAREADY)
    {
        easynet->ep.in = param;
        if (easynet->frame_in)
        {
            easynet->frame_in(&easynet->ep);
        }
    }
    return 0;
}

static int easynet_worker_deinit(worker_t *self)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;

    if (easynet->exit)
    {
        easynet->exit(&easynet->ep);
    }
    if (easynet->task_id > 0)
    {
        k_sem_take(&easynet->lock, K_FOREVER);
    }
    kfree(easynet);

    return 0;
}

worker_t easynet_worker =
{
    .run    = easynet_worker_run,
    .init   = easynet_worker_init,
    .deinit = easynet_worker_deinit,
};
