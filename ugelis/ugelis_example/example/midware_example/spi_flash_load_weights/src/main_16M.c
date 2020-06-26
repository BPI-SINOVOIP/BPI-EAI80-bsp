/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          main.c
 *
 * @author        Colin.Yang
 *
 * @version       1.1.0
 *
 * @date          2018/03/27
 *
 * @brief         FPGA Test entry file
 *
 * @note
 *    2018/03/27, Colin.Yang, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <misc/printk.h>
//#include <gm_hal_sqi.h>

#include "spi_drv.h"


typedef struct
{
    uint32_t part_num;
    uint32_t info_addr;
    uint32_t info_size;
    uint32_t info_load_addr;
    uint32_t boot1_addr;
    uint32_t boot1_size;
    uint32_t boot1_load_addr;
    uint32_t system_addr;
    uint32_t system_size;
    uint32_t system_load_addr;
    uint32_t weights_addr;
    uint32_t weights_size;
    uint32_t weights_load_addr;
    uint32_t cmd_addr;
    uint32_t cmd_size;
    uint32_t cmd_load_addr;
    uint32_t input_addr;
    uint32_t input_size;
    uint32_t input_load_addr;
} info_t;

info_t info;


int write_test()
{
    unsigned long i = 0;
    unsigned long addr = 0x10000000;
    unsigned long size = 1 * 1024 * 1024;

    for (i = 0; i < size / SECTOR_LENGTH; i++)
    {
        EraseSector(addr + i * SECTOR_LENGTH);
        ProgramPage(addr + i * SECTOR_LENGTH, SECTOR_LENGTH, 0xC0100000 + i * SECTOR_LENGTH);
    }

    return 0;
}

int main(void)
{
    int i = 0;
    int ret = 0;

    ret = Init(0, 0, 0);

    //ret = write_test();

    ret = ReadPage(0x10000000, sizeof(info), &info);

    ret = ReadPage(info.weights_addr, info.weights_size, info.weights_load_addr);

    ret = ReadPage(info.cmd_addr, info.cmd_size, info.cmd_load_addr);

    ret = ReadPage(info.input_addr, info.input_size, info.input_load_addr);

    return ret;
}



