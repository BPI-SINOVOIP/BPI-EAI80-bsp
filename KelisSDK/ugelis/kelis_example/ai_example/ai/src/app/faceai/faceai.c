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
#include <display/fb_display.h>

#include "worker/worker.h"
#include "worker/loader_worker.h"
#include "inc/config.h"
#include "faceai.h"
#include "face_detect.h"
#include "face_recognize.h"

extern unsigned char WEI_BASE[];

int faceai_cmd_exit(int argc, char *argv[], void *data)
{
    faceai_exit();
    return 0;
}

int faceai_cmd_start(int argc, char *argv[], void *data)
{
    faceai_start();
    return 0;
}

static worker_pre_param_t faceai_pre_param =
{
    .ai_w       = AI_BUF_W,
    .ai_h       = AI_BUF_H,
    .init       = face_detect_init,
    .exit       = face_detect_exit,
    .frame_in   = face_detect_frame_in,
    .sizes =
    {
        .src_w   = CAM_BUF_W,
        .src_h   = CAM_BUF_H,
        .input_w = OUT_BUF_W,
        .input_h = OUT_BUF_H,
    },
};

static worker_easynet_param_t faceai_easynet_param =
{
    .init       = face_recognize_init,
    .exit       = face_recognize_exit,
    .process    = face_recognize_thread,
    .frame_in   = face_recognize_frame_in,
};


int faceai_reset_scaler()
{
    worker_t *lcd = get_worker("lcd");
    worker_lcd_param_t *param = stream_get_lcd_param();

    param->video.out0_w  = param->video.src_w;
    param->video.out0_h  = param->video.src_h;
    param->video.out0_fmt = FMT_RGB888;
    param->video.out1_w  = OUT_BUF_W;
    param->video.out1_h  = OUT_BUF_H;
    param->video.out1_fmt = FMT_L8;
    lcd->run(lcd, CMD_LCD_RESETSCALER, param);
    return 0;
}

int stream_start_faceai(uint32_t ops)
{
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *post      = get_worker("post");
    worker_t *easynet   = get_worker("easynet");

    worker_t *event     = get_worker("event");
    worker_post_param_t    post_param;

    faceai_reset_scaler();

    faceai_easynet_param.ops = ops;
    easynet->init(easynet, &faceai_easynet_param);

    post_param.base_ofst = FACE_BASE_FLASH_OFFSET;
    post_param.base_size = FACE_BASE_SIZE;
    post->init(post, &post_param);

    pre->init(pre, &faceai_pre_param);

    easynet->next[0] = post;
    pre->next[0]     = easynet;
    cam->next[1]     = pre;
    event->init(event, 0);
    event->next[0] = post;

    return 0;
}

int stream_stop_faceai()
{
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *easynet   = get_worker("easynet");
    worker_t *post      = get_worker("post");
    worker_t *event     = get_worker("event");

    cam->next[1]     = 0;
    pre->next[0]     = 0;
    easynet->next[0] = 0;
    post->next[0]    = 0;
    event->next[0]   = 0;
    event->deinit(event);
    pre->deinit(pre);
    easynet->deinit(easynet);
    post->deinit(post);

    return 0;
}

void faceai_start(void)
{
    worker_io_t info;

    info.src     = FACE_OPS_FLASH_OFFSET;
    info.size    = FACE_OPS_SIZE;
    info.dst     = (int)WEI_BASE;
    uint32_t ops = stream_start_loaddata(&info);

    stream_start_faceai(ops);
}

void faceai_exit(void)
{
    stream_stop_faceai();
}

int faceai_record(int argc, char *argv[], void *data)
{
    worker_t *post = get_worker("post");

    post->run(post, CMD_EVENT_SHORTPRESS, 0);

    return 0;
}

int faceai_clear(int argc, char *argv[], void *data)
{
    worker_t *post = get_worker("post");

    post->run(post, CMD_EVENT_LONGPRESS, 0);

    return 0;
}
