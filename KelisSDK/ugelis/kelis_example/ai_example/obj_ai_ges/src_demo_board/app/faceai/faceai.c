/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          faceai.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Faceai app interface.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <stdlib.h>

#include "worker/worker.h"
#include "worker/loader_worker.h"
#include "inc/config.h"
#include "faceai.h"

extern unsigned char WEI_BASE[];

int faceai_exit();
int faceai_enter();

int faceai_cmd_exit(int argc, char *argv[], void *data)
{
    printf("bye faceai\n");
    faceai_exit();
    return 0;
}

int faceai_cmd_enter(int argc, char *argv[], void *data)
{
    printf("hello faceai\n");
    faceai_enter();
    return 0;
}

int stream_start_faceai(uint32_t ops, uint32_t base)
{
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *post      = get_worker("post");
    worker_t *easynet   = get_worker("easynet");
    worker_t *painter   = get_worker("painter");
    worker_t *event     = get_worker("event");
    easynet->init(easynet, ops);
    post->init(post, base);
    kfree(base);
    pre->init(pre, 0);
    post->next[0]    = painter;
    easynet->next[0] = post;
    pre->next[0]     = easynet;
    cam->next[1]     = pre;
    event->init(event, 0);
    event->next[0] = post;

    return 0;
}


int stream_start_objectai(uint32_t ops, uint32_t base)
{
#if 0
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *post      = get_worker("post");
    worker_t *easynet   = get_worker("easynet");
    worker_t *painter   = get_worker("painter");
    worker_t *event     = get_worker("event");
    easynet->init(easynet, ops);
    post->init(post, base);
    kfree(base);
    pre->init(pre, 0);
    post->next[0]    = painter;
    easynet->next[0] = post;
    pre->next[0]     = easynet;
    cam->next[1]     = pre;
    event->init(event, 0);
    event->next[0] = post;
#else
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *post      = get_worker("post");
    worker_t *easynet   = get_worker("easynet");
    worker_t *painter   = get_worker("painter");
    worker_t *event     = get_worker("event");
    easynet->init(easynet, 0);
    post->init(post, base);
    //kfree(base);
    pre->init(pre, 0);
    pre->next[0]     = easynet;
    cam->next[1]     = pre;


#endif
    return 0;
}



int stream_stop_faceai()
{
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *easynet   = get_worker("easynet");
    worker_t *post      = get_worker("post");
    worker_t *painter   = get_worker("painter");
    worker_t *event     = get_worker("event");

    event->next[0] = 0;
    event->deinit(event, 0);

    post->deinit(post, 0);
    easynet->deinit(easynet, 0);
    pre->deinit(pre, 0);

    post->next[0]    = 0;
    easynet->next[0] = 0;
    pre->next[0]     = 0;
    cam->next[1]     = 0;

    return 0;
}

int faceai_enter(void)
{
    worker_io_t info;
    info.src   = BASE_BIN_FLASH_START;
    info.size  = BASE_BIN_FLASH_SIZE;
    info.dst   = kmalloc(BASE_BIN_FLASH_SIZE);
    uint32_t base = stream_start_loaddata(&info);
    info.src   = OPS_BIN_FLASH_START;
    info.size  = OPS_BIN_FLASH_SIZE;
    info.dst   = WEI_BASE;
    uint32_t ops = stream_start_loaddata(&info);
    //stream_start_faceai(ops, base);
}

int objectai_enter(void)
{
    worker_io_t info;
    info.src   = BASE_BIN_FLASH_START;
    info.size  = BASE_BIN_FLASH_SIZE;
    info.dst   = kmalloc(BASE_BIN_FLASH_SIZE);
    uint32_t base = stream_start_loaddata(&info);
    info.src   = OPS_BIN_FLASH_START;
    info.size  = OPS_BIN_FLASH_SIZE;
    info.dst   = WEI_BASE;
    uint32_t ops = stream_start_loaddata(&info);
    stream_start_objectai(ops, base);
}

int objectai_exit(void)
{
    //stream_stop_faceai();
}


int faceai_exit(void)
{
    stream_stop_faceai();
}
