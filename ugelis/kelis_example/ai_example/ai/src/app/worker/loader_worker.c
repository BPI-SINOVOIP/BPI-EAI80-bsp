/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          loader_worker.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer worker to manage io loader
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include "worker.h"
#include "loader_worker.h"
#include "dev/flash.h"

static int loader_worker_init(worker_t *self, void *param)
{
    dev_flash_t *dev = (dev_flash_t *)kmalloc(sizeof(dev_flash_t));

    flash_init(dev);

    self->data = dev;
    return 0;
}

static int loader_worker_run(worker_t *self, int cmd, void *param)
{
    worker_io_t *info = param;
    if (cmd == CMD_LOADER_READ)
    {
        flash_dev_read((dev_flash_t *)(self->data), info->src, info->dst, info->size);
    }
    else if (cmd == CMD_LOADER_WRITE)
    {
        flash_dev_writeall((dev_flash_t *)(self->data), info->src, info->dst, info->size);
    }
    return 0;
}

static int loader_worker_deinit(worker_t *self)
{
    kfree(self->data);
    return 0;
}

worker_t loader_worker =
{
    .run    = loader_worker_run,
    .init   = loader_worker_init,
    .deinit = loader_worker_deinit,
};
