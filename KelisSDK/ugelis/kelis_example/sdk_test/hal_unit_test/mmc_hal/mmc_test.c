/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <misc/printk.h>
#include <ztest.h>
#include <device.h>
#include "mmc_test.h"
#include "board.h"
#include <gm_hal_mmc.h>
//#include <gm_hal_clock.h>

#define TEST_BLOCK_NUM              50
#define TEST_BLOCK_START            (0)
#define TEST_DATA                   (0xa5)
#define TEST_DATA_SIZE              (512*TEST_BLOCK_NUM)

static uint8_t gbuf[TEST_DATA_SIZE] = {0};
static const uint8_t tbuf[TEST_DATA_SIZE] = {0};
MMC_Handle_T ghMMC;

int mmc_read_write_test(void)
{
    RET_CODE ret = RET_OK;
    uint32_t blk_cnt = TEST_BLOCK_NUM;
    uint32_t blk_start = TEST_BLOCK_START;
    static uint32_t cnt = 0;
    uint8_t *buf = gbuf;
    uint32_t i = 0;
    MMC_Config_T config;
    MMC_Handle_T *hMMC = &ghMMC;

#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif

    /*init mmc*/
    config.busWitch      = MMC_BUS_WIDTH_4;
    config.type            = MMC_SD;
    config.removable   = TRUE;



    TC_PRINT("##########Try MMC Init ##############\n");

    ret = HAL_MMC_Init(hMMC, MMC0, &config);
    if (ret != RET_OK)
    {
        TC_PRINT("Err:%s:HAL_MMC_Init\n", __FUNCTION__);
        return TC_FAIL;
    }

    TC_PRINT("##########MMC Init Ok##############\n");
    memset(buf, TEST_DATA, TEST_DATA_SIZE);
    /*write mmc*/
    blk_cnt = HAL_MMC_Write(hMMC, blk_start, TEST_BLOCK_NUM, buf);
    if (blk_cnt != TEST_BLOCK_NUM)
    {
        TC_PRINT("Err:%s:HAL_MMC_Write\n", __FUNCTION__);
        return TC_FAIL;
    }



    blk_cnt = HAL_MMC_Read(hMMC, blk_start, TEST_BLOCK_NUM, tbuf);
    if (blk_cnt != TEST_BLOCK_NUM)
    {
        TC_PRINT("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return TC_FAIL;
    }
    /*checdata is ok*/
    for (i = 0 ; i < TEST_DATA_SIZE ; i++)
    {
        if (tbuf[i] != TEST_DATA)
        {
            TC_PRINT("Check Err: sector:%d buf[%d]:0x%08x\n", (i / 512), i, buf[i]);
            return TC_FAIL;
        }
    }
    TC_PRINT("MMC TEST PASS!\n");
    return TC_PASS;
}

void mmc_test(void)
{
    TC_PRINT("================HAL MMC Test Start=================\n\n\n");
    zassert_true(mmc_read_write_test() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================HAL MMC Test  End==================\n\n\n");
}

