/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          objai.c
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
#include "objai.h"
#include "objai/obj_recognize.h"

extern unsigned char WEI_BASE[];

static worker_pre_param_t objai_pre_param =
{
    .ai_w       = OBJ_AI_BUF_W,
    .ai_h       = OBJ_AI_BUF_H,
};

obj_recognized_data_t body_data =
{
    .class_channel = 1,
    .type = DET_BODY,
};

obj_recognized_data_t ges_data =
{
    .class_channel = 2,
    .type = DET_GEST,
};

static worker_easynet_param_t objai_easynet_param =
{
    .init       = obj_recognize_init,
    .exit       = obj_recognize_exit,
    .process    = obj_recognize_thread,
    .frame_in   = obj_recognize_frame_in,
};

int objai_reset_scaler()
{
    worker_t *lcd = get_worker("lcd");
    worker_lcd_param_t *param = stream_get_lcd_param();

    param->video.out0_w  = OBJ_AI_BUF_W;
    param->video.out0_h  = OBJ_AI_BUF_H;
    param->video.out0_fmt = FMT_RGB888;

    param->video.out1_w   = param->video.src_w;
    param->video.out1_h   = param->video.src_h;
    param->video.out1_fmt = param->video.src_fmt;
    lcd->run(lcd, CMD_LCD_RESETSCALER, param);
    return 0;
}

int stream_start_objai(uint32_t ops, obj_recognized_data_t *data)
{
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *easynet   = get_worker("easynet");
    worker_t *event     = get_worker("event");
    objai_reset_scaler();
    objai_easynet_param.ops = ops;
    objai_easynet_param.data = data;
    easynet->init(easynet, &objai_easynet_param);

    pre->init(pre, &objai_pre_param);
    pre->next[0]     = easynet;
    cam->next[1]     = pre;
    return 0;
}

int stream_stop_objai()
{
    worker_t *cam       = get_worker("camera");
    worker_t *pre       = get_worker("pre");
    worker_t *easynet   = get_worker("easynet");
    worker_t *event     = get_worker("event");

    cam->next[1]   = 0;
    pre->next[0]   = 0;

    pre->deinit(pre);
    easynet->deinit(easynet);

    return 0;
}

static int objai_enter(int ops_ofst, int ops_size, obj_recognized_data_t *data)
{
    worker_io_t info;
    info.src   = ops_ofst;
    info.size  = ops_size;
    info.dst   = (int)WEI_BASE;
    uint32_t ops = stream_start_loaddata(&info);

    stream_start_objai(ops, data);

    return 0;
}

static int objai_exit(void)
{
    stream_stop_objai();
    return 0;
}

void bodyai_start(void)
{
    objai_enter(OBJ_OPS_FLASH_OFFSET, OBJ_OPS_SIZE, &body_data);
}

void bodyai_exit(void)
{
    objai_exit();
}

void gesai_start(void)
{
    objai_enter(GES_OPS_FLASH_OFFSET, GES_OPS_SIZE, &ges_data);
}

void gesai_exit(void)
{
    objai_exit();
}

int bodyai_cmd_exit(int argc, char *argv[], void *data)
{
    bodyai_exit();
    return 0;
}

int bodyai_cmd_start(int argc, char *argv[], void *data)
{
    bodyai_start();
    return 0;
}

int gesai_cmd_exit(int argc, char *argv[], void *data)
{
    gesai_exit();
    return 0;
}

int gesai_cmd_start(int argc, char *argv[], void *data)
{
    gesai_start();
    return 0;
}


