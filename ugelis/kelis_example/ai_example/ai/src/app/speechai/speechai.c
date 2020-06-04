/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          speechai.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/03/11
 *
 * @brief         Speechai app interface.
 *
 * @note
 *    2020/03/11, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include "worker/worker.h"
#include "inc/config.h"
#include "speechai.h"
#include "speechai/speech_recognize.h"

static worker_easynet_param_t speechai_easynet_param =
{
    .init       = speech_recognize_init,
    .exit       = speech_recognize_exit,
    .process    = speech_recognize_thread,
};

static int stream_start_speechai()
{
    worker_t *easynet   = get_worker("easynet");
    easynet->init(easynet, &speechai_easynet_param);

    return 0;
}

static int stream_stop_speechai()
{
    worker_t *easynet   = get_worker("easynet");
    easynet->deinit(easynet);

    return 0;
}

void speechai_start(void)
{
    stream_start_speechai();
}

void speechai_exit(void)
{
    stream_stop_speechai();
}

int speechai_cmd_exit(int argc, char *argv[], void *data)
{
    speechai_exit();
    return 0;
}

int speechai_cmd_start(int argc, char *argv[], void *data)
{
    speechai_start();
    return 0;
}

