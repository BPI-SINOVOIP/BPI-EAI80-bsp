/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>
#include <mmc.h>
#include <ztest.h>
#include <device.h>
#include "mmc_test.h"
#include "board.h"

#define TEST_TIME 2000

#define SECTOR_NR   110

#define SECTOR_ADD  6538

#define LENGTH     (512*SECTOR_NR)

static uint8_t __aligned(4) buf[LENGTH] = {0};


static int mmc_event_callback(enum mmc_slot_status status)
{

    struct device *dev;

    dev = device_get_binding(DEV_MMC0_NAME);
    uint32_t card_capacity = 0;

    if (status == MMC_INSERD_SLOT)
    {
        card_capacity = mmc_get_capacity(dev);
        TC_PRINT("#### HOTPLUG: OFF-->ON : MMC Inserd To Card Slot ####\n");
        if (card_capacity)
        {
            TC_PRINT("#### Card Capactity :%d MB ####\n", card_capacity);
        }
        else
        {
            TC_PRINT("#### ERR Card Capactity  ####\n");
        }


    }
    else
    {
        TC_PRINT("##### HOTPLUG: ON-->OFF:MMC Move Form Card Slot ####\n");
    }

    return 0;
}

int mmc_readwrite_test(void)
{
    uint32_t i = 0;
    uint8_t ret = 0;
    struct device *dev;
    uint32_t testtime = TEST_TIME;
    uint32_t mmc_capacity = 0;

    TC_PRINT("ENTER MMC Drvier TEST\n");
    dev = device_get_binding(DEV_MMC0_NAME);

    if (!dev)
    {
        TC_PRINT("Main Err: device_get_binding\n");
        return TC_FAIL;
    }

    /*set callback to receive sdcard inserd or output event*/
    mmc_set_callback(dev, mmc_event_callback);


    /*check sd card is inserd*/
    while (mmc_in_slot(dev) != true)
    {
        TC_PRINT("########################## wait card inserd ###########################\n");
        k_sleep(1000);
    }
    mmc_capacity = mmc_get_capacity(dev);
    TC_PRINT("#### Card Capactity :%d MB ####\n", mmc_capacity);
    /*set buf to magic*/
    for (i = 0; i < LENGTH ; i++)
    {
        buf[i] = i % 256;
    }

    /*writ mmc*/
    TC_PRINT("##########begain write##############\n");
    ret = mmc_write(dev, SECTOR_ADD, SECTOR_NR, buf);
    if (ret)
    {
        TC_PRINT("#######################Main Err: MMC Write###########################\n");
        return TC_FAIL;
    }
    TC_PRINT("##########write successful!##############\n");

    /*read mmc*/
    TC_PRINT("##########begain read##############\n");
    ret = mmc_read(dev, SECTOR_ADD, SECTOR_NR, buf);
    if (ret)
    {
        TC_PRINT("########################B:Main Err:%d: MMC READ#######################\n", ret);
        return TC_FAIL;
    }
    TC_PRINT("##########read successful!##############\n");
    /*test mmc*/
    for (i = 0; i < LENGTH ; i++)
    {
        if (buf[i] != i % 256)
        {
            return TC_FAIL;
        }
    }
    TC_PRINT("##########check data is OK!##############\n");
    return TC_PASS;
}


void mmc_test(void)
{
    zassert_true(mmc_readwrite_test() == TC_PASS, NULL);
}

