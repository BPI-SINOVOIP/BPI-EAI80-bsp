/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          worker_manager.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Interfacet to manage all stream workers
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <string.h>

#include "worker.h"

extern worker_t event_worker;
extern worker_t loader_worker;

extern worker_t camera_worker;
extern worker_t lcd_worker;

extern worker_t pre_worker;
extern worker_t easynet_worker;
extern worker_t post_worker;

extern worker_t painter_worker;

static worker_registry_entry_t worker_registry[] =
{
    {"event", &event_worker},
    {"loader", &loader_worker},

    {"camera", &camera_worker},
    {"lcd", &lcd_worker},

    {"pre", &pre_worker},
    {"easynet", &easynet_worker},
    {"post", &post_worker},


    {"painter", &painter_worker},
};

static const int worker_registry_entry_count = sizeof(worker_registry) / sizeof(worker_registry_entry_t);

worker_t *get_worker(const char *name)
{
    int i = 0;
    for (i = 0; i < worker_registry_entry_count; i++)
    {
        if (strcmp(name, worker_registry[i].name) == 0)
        {
            return worker_registry[i].worker;
        }
    }
    return 0;
}

int do_next_process(worker_t *worker, int cmd, void *param)
{
    int i = 0;
    for (i = 0; i < 2; i++)
    {
        if (worker->next[i])
        {
            worker->next[i]->run(worker->next[i], cmd, param);
        }
    }
    return 0;
}
