/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          cmd.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Parse user's commands from shell and call relative functions.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <shell/shell.h>
#include "cmd.h"

#define CMD_TAG "ai_cmd"

static dev_cmd_t *gCmdDev;
static inline dev_cmd_t *cmd_get_dev()
{
    return gCmdDev;
}

static void cmd_set_dev(dev_cmd_t *dev)
{
    gCmdDev = dev;
}

static int cmd_print_help(int argc, char *argv[])
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    printf("faceai_exit\n\tExit Faceai Demo\n");
    printf("faceai_enter\n\tStart Faceai Demo\n");
    return 0;
}

static struct shell_cmd cmd_set[] =
{
    {"helper", cmd_print_help, 0},
    {0, 0}
};

static int cmd_handler(int argc, char *argv[])
{
    int i = 0;
    dev_cmd_t *dev = cmd_get_dev();
    for (i = 0; i < dev->cur_cnt; i++)
    {
        if (strcmp(dev->cbs[i].cmd, argv[0]) == 0)
        {
            dev->cbs[i].cb(argc - 1, &argv[1], dev->cbs[i].data);
        }
    }
    return 0;
}

int cmd_register(char *cmd, cmd_cb cb, void *data)
{
    dev_cmd_t *sh = cmd_get_dev();
    if (sh->cur_cnt < CMD_MAX_CNT)
    {
        strcpy(sh->cbs[sh->cur_cnt].cmd, cmd);
        sh->cbs[sh->cur_cnt].cb = cb;
        sh->cbs[sh->cur_cnt].data = data;
        sh->cur_cnt++;
    }
    else
    {
        return -1;
    }
    return 0;
}

int cmd_unregister(char *cmd)
{
    //TODO HERE
    return 0;
}

int cmd_init(dev_cmd_t *shell)
{
    cmd_set_dev(shell);
    shell->cur_cnt = 0;

    SHELL_REGISTER(CMD_TAG, cmd_set);
    shell_register_default_module(CMD_TAG);
    shell_register_app_cmd_handler(cmd_handler);
    return 0;
}

int cmd_exit(dev_cmd_t *shell)
{
    return 0;
}
