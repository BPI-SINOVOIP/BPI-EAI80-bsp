/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        ops_save.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/06/10
 *
 * @brief       Save the compiled result to binary.
 *
 * @note
 *              2019/06/10, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/
#include <stdio.h>
#define MAGIC_LEN   8
#define VERSION_LEN 6
#define SIZE_LEN    4

#define HW_CMD_MAX_SIZE   0x10000
#define HW_OP_MAX_SIZE    0x10000

int ops_parse(char *seq_buf, unsigned char **param, unsigned char **cmd_addr, unsigned char **weight_addr)
{
    char magic[MAGIC_LEN];
    char version[VERSION_LEN];
    int ops_len;
    int cmd_len;
    int weight_len;
    int off = 0;
#if 0	
    memcpy(magic,   seq_buf + off, MAGIC_LEN);
    off += MAGIC_LEN;
    memcpy(version, seq_buf + off, VERSION_LEN);
    off += VERSION_LEN;
    printf("magic:%s, version:%s\n", magic, version);
#endif	

    memcpy(&ops_len, seq_buf + off, SIZE_LEN);
    off += SIZE_LEN;
    memcpy(&cmd_len, seq_buf + off, SIZE_LEN);
    off += SIZE_LEN;
    memcpy(&weight_len, seq_buf + off, SIZE_LEN);
    off += SIZE_LEN;

	ops_len = 0x10000;
	off = 0;
	cmd_len = 0x10000;

    *param = seq_buf + off;
    *cmd_addr = *param + ops_len;
    *weight_addr = *param + ops_len + cmd_len;

    return 4690800;   // face ai 5917120;//   ops_len + cmd_len + weight_len;
}
