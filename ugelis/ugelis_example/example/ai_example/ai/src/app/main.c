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

#include "objai/objai.h"
#include "dev/cmd.h"
#include "dev/dev.h"
#include "inc/config.h"
#include "inc/common.h"

typedef void (*app_handler)(void);

typedef struct app
{
    dev_cmd_t shell;
    int current;
    int dest;
    app_handler start[APP_END];
    app_handler exit[APP_END];
    int status[APP_END];
} app_t;

app_t gAI =
{
    .start =
    {
        NULL,
        bodyai_start, gesai_start //speechai_start
    },
    .exit  =
    {
        NULL,
        bodyai_exit,  gesai_exit //speechai_exit
    },
    .current = -1,
    .dest = START_APP,
};

int app_switcher(app_t *app, int index)
{
    if (index < APP_END && app->current != index)
    {
        if (app->current >= 0)
        {
            app->exit[app->current]();
            app->status[app->current] = 0;
        }
        app->current = index;
        if (app->current == APP_SPEECH)
        {
            gui_refresh(0);
        }
        else
        {
            gui_refresh(1);
        }
        app->start[index]();
        app->status[index] = 1;
    }

    return 0;
}

void gui_cb(int index)
{
    if (index >= 1 && index <= 4)
    {
        gAI.dest = index - 1;
    }
}

void app_init(app_t *app)
{
    dev_init();
    stream_init();
    cmd_init(&app->shell);
    gui_init(gui_cb);
}

void app_deinit(app_t *app)
{
    gui_exit();
    cmd_exit(&app->shell);
    stream_deinit();
    dev_exit();
}

int app_bind_cmd()
{
    cmd_register("ge", gesai_cmd_exit, 0);
    cmd_register("gs", gesai_cmd_start, 0);

    cmd_register("be", bodyai_cmd_exit, 0);
    cmd_register("bs", bodyai_cmd_start, 0);

    //cmd_register("se", speechai_cmd_exit, 0);
    //cmd_register("ss", speechai_cmd_start, 0);

    //cmd_register("cap", app_capture_screen, 0);

    return 0;
}

int app_unbind_event()
{
    cmd_unregister("faceai_exit");
    return 0;
}

int main()
{
    app_t *app = &gAI;
    printf("\n ------- AI Demo ver.20200421 ------- \n");

    app_init(app);
    stream_start_videoview();
    app_bind_cmd();

    k_thread_priority_set(k_current_get(), 1);

    while (1)
    {
        app_switcher(&gAI, gAI.dest);
        k_sleep(10);
    }

    //stream_stop_videoview();
    //app_deinit(app);

    //return 0;
}
