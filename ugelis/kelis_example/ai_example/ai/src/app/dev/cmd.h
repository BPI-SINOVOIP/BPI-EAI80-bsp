/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          cmd.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of command parser
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __CMD_H__
#define __CMD_H__

#define CMD_MAX_CNT 20

typedef int (*cmd_cb)(int argc, char *argv[], void *data);

typedef struct dev_cmd_cb
{
    char cmd[32];
    void *data;
    cmd_cb cb;
} dev_cmd_cb_t;

typedef struct dev_cmd
{
    int cur_cnt;
    dev_cmd_cb_t cbs[CMD_MAX_CNT];
} dev_cmd_t;

int cmd_register(char *cmd, cmd_cb cb, void *data);
int cmd_unregister(char *cmd);
int cmd_init(dev_cmd_t *shell);
int cmd_exit(dev_cmd_t *shell);

#endif //__CMD_H__