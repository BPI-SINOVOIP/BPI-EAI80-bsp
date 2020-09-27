/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          flash.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Header file of flash reader
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */
#ifndef __FLASH_H__
#define __FLASH_H__

#include <gm_hal_sqi.h>

typedef struct dev_flash
{
    SQI_Device_T *sqi_dev;
    struct SQI_Handle handler;
    struct SQI_Config config;
} dev_flash_t;

int32_t flash_init(dev_flash_t *dev);
int32_t flash_dev_read(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t read_size);
int32_t flash_dev_write(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t write_size);
int32_t flash_dev_writeall(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t write_size);

#endif //__FLASH_H__