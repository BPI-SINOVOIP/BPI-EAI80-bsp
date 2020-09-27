/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          main.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Main file to launch ai demo
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <kernel.h>

#include "faceai/faceai.h"
#include "dev/cmd.h"
#include "dev/dev.h"
#include "inc/config.h"
#include "inc/common.h"

typedef struct app
{
    dev_cmd_t shell;
} app_t;

void app_init(app_t *app)
{
    dev_init();
    stream_init();
    cmd_init(&app->shell);
}

void app_deinit(app_t *app)
{
    cmd_exit(&app->shell);
    stream_deinit();
    dev_exit();
}

void app_start()
{
    stream_start_videoview();
}

void app_stop()
{
    stream_stop_videoview();
}

int app_bind_event()
{
    cmd_register("faceai_exit", faceai_cmd_exit, 0);
    cmd_register("faceai_enter", faceai_enter, 0);
    cmd_register("record", faceai_record, 0);
    cmd_register("clear", faceai_clear, 0);
    return 0;
}

int app_unbind_event()
{
    cmd_unregister("faceai_exit");
    return 0;
}

int main()
{
    app_t app;

    app_init(&app);

    stream_start_videoview();

    faceai_enter();

    app_bind_event();

    while (1)
    {
        k_sleep(500);
    }
    stream_stop_videoview();
    app_deinit(&app);

    return 0;
}
