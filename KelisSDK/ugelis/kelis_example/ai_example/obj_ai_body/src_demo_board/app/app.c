/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          app.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief        Common interface for app
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <inc/config.h>
#include <worker/worker.h>
#include <worker/loader_worker.h>
#include <dev/gpio.h>

int stream_init()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");
    worker_t *loader = get_worker("loader");

    cam->init(cam, 0);
    lcd->init(lcd, 0);
    loader->init(loader, 0);
}

int stream_deinit()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");
    worker_t *loader = get_worker("loader");

    loader->deinit(loader, 0);
    lcd->deinit(lcd, 0);
    cam->deinit(cam, 0);
}

int stream_start_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    //cam->next[0] = lcd;
    cam->run(cam, 0, 0);

    return 0;
}

int stream_stop_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    cam->next[0] = 0;
    cam->stop(cam, 0, 0);

    return 0;
}

uint32_t stream_start_loaddata(worker_io_t *info)
{
    worker_t *loader = get_worker("loader");
    loader->run(loader, CMD_LOADER_READ, info);
    return info->dst;
}
