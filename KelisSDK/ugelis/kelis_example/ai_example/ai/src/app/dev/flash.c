/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          flash.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Flash Reader Interface
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <board.h>
#include <gm_hal_sqi.h>

#include "flash.h"

int32_t flash_init(dev_flash_t *dev)
{
    int32_t ret;

    dev->sqi_dev = (SQI_Device_T *)EFLASH_CTRL_BASE;
    memset(&dev->config, 0, sizeof(struct SQI_Config));
    //    dev->config.bit_mode = SQI_4BIT_MODE;
    //    dev->config.read_mode = SQI_READ_BY_CPU;
    //    dev->config.clk_div = SCLK_DIV;
    ret  = HAL_SQI_Init(&dev->handler, dev->sqi_dev, &dev->config);
    if (ret != 0)
    {
        printf("flash init error\n");
    }
    return ret;
}

int32_t flash_dev_read(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t read_size)
{
    return HAL_SQI_Read(&dev->handler, src_addr, (void *)dest_addr, read_size);
}

//suitable for writing big size, more than 500kb
int32_t flash_dev_write(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t write_size)
{
    HAL_SQI_Erase(&dev->handler, dest_addr, write_size);//maybe erase larger than write_size, since aligned to a sector_size(4K)
    HAL_SQI_Write(&dev->handler, dest_addr, (void *)src_addr, write_size);
    return 0;
}

//suitable for writing small size
int32_t flash_dev_writeall(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t write_size)
{
    return HAL_SQI_WriteAll(&dev->handler, dest_addr, (void *)src_addr, write_size);
}
