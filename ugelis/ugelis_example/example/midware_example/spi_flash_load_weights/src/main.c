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
    uint32_t body_weights_addr;
    uint32_t body_weights_size;
    uint32_t body_weights_load_addr;
    uint32_t body_cmd_addr;
    uint32_t body_cmd_size;
    uint32_t body_cmd_load_addr;
    uint32_t body_input_addr;
    uint32_t body_input_size;
    uint32_t body_input_load_addr;
    uint32_t gest_weights_addr;
    uint32_t gest_weights_size;
    uint32_t gest_weights_load_addr;
    uint32_t gest_cmd_addr;
    uint32_t gest_cmd_size;
    uint32_t gest_cmd_load_addr;
    uint32_t gest_input_addr;
    uint32_t gest_input_size;
    uint32_t gest_input_load_addr;
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

    /*
    read partition info :
    xxx_addr --> the flash address of partition xxx
    xxx_size --> the real data size of partition xxx
    xxx_load_addr --> the memory address to store the partiton xxx
    */
    ret = ReadPage(0x10000000, sizeof(info), &info);

    /* read body info */
    ret = ReadPage(info.body_weights_addr, info.body_weights_size, info.body_weights_load_addr);
    ret = ReadPage(info.body_cmd_addr, info.body_cmd_size, info.body_cmd_load_addr);
    ret = ReadPage(info.body_input_addr, info.body_input_size, info.body_input_load_addr);

    /* read gest info */
    ret = ReadPage(info.gest_weights_addr, info.gest_weights_size, info.gest_weights_load_addr);
    ret = ReadPage(info.gest_cmd_addr, info.gest_cmd_size, info.gest_cmd_load_addr);
    ret = ReadPage(info.gest_input_addr, info.gest_input_size, info.gest_input_load_addr);

    return ret;
}



