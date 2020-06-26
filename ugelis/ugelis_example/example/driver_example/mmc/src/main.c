/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <ugelis.h>
#include <misc/printk.h>
#include <mmc.h>


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
        printk("#### HOTPLUG: OFF-->ON : MMC Inserd To Card Slot ####\n");
        if (card_capacity)
        {
            printk("#### Card Capactity :%d MB ####\n", card_capacity);
        }
        else
        {
            printk("#### ERR Card Capactity  ####\n");
        }


    }
    else
    {
        printk("##### HOTPLUG: ON-->OFF:MMC Move Form Card Slot ####\n");
    }

    return 0;
}


static uint32_t cnt = 0;

int main()
{
    uint32_t i = 0;
    uint8_t ret = 0;
    struct device *dev;
    uint32_t testtime = TEST_TIME;

    dev = device_get_binding(DEV_MMC0_NAME);
    if (!dev)
    {
        printk("Main Err: device_get_binding\n");
        return ret;
    }

    /*set callback to receive sdcard inserd or output event*/
    mmc_set_callback(dev, mmc_event_callback);

    while (1)
    {

retry:

        /*check sd card is inserd*/
        while (mmc_in_slot(dev) != true)
        {
            printk("########################## wait card inserd ###########################\n");
            k_sleep(1000);
        }

        /*set buf to magic*/
        for (i = 0; i < LENGTH ; i++)
        {
            buf[i] = i % 256;
        }

        /*writ mmc*/
        ret = mmc_write(dev, SECTOR_ADD, SECTOR_NR, buf);
        if (ret)
        {
            printk("#######################Main Err: MMC Write###########################\n");
            goto retry;
        }

        /*read mmc*/
        ret = mmc_read(dev, SECTOR_ADD, SECTOR_NR, buf);
        if (ret)
        {
            printk("########################B:Main Err:%d: MMC READ#######################\n", ret);
            goto retry;
        }

        /*test mmc*/
        for (i = 0; i < LENGTH ; i++)
        {
            if (buf[i] != i % 256)
            {
                ret  = 1;
                break;
            }
        }

        /*read mmc*/
        if (ret)
        {
            printk("####################test error####################\n");
            while (1);

        }
        else
        {
            printk("test ok cnt:%d\n", ++cnt);
        }
    }

}





