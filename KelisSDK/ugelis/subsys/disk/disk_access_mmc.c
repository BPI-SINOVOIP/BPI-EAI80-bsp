/**
 *
 * Copyright (C) 2018 Edgeless Semiconductors.  All Rights Reserved.
 * 
 * @file          disk_access_mmc.c
 *
 * @author        richard.liu
 *
 * @version       1.1.0
 * 
 * @date          2018/12/6 
 *    
 * @brief         Simple implementation of wfst decoder.
 * 
 * @note
 *    2018/12/6, richard.liu, V1.0.0
 *        Initial version.
 */

#include <string.h>
#include <ugelis/types.h>
#include <misc/__assert.h>
#include <disk_access.h>
#include <errno.h>
#include <init.h>
#include <device.h>
#include <mmc.h>
#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_mmc.h>

#define SECTOR_SIZE 512
#define CONFIG_DISK_ERASE_BLOCK_SIZE (512)
#define CONFIG_DISK_FLASH_START   0
#define CONFIG_DISK_VOLUME_SIZE   (1024*1024*1024)
#define CONFIG_DISK_FLASH_ERASE_ALIGNMENT (512)
#define CONFIG_DISK_FLASH_MAX_RW_SIZE (512)

static struct device *mmc_dev;
MMC_Handle_T ghMMC;
MMC_Handle_T *hMMC = &ghMMC;

static int disk_mmc_access_status(struct disk_info *disk)
{

    if (!mmc_dev) {
        printk("enter func: %s,line[%d] DISK_STATUS_NOMEDIA \n", __func__, __LINE__);
        return DISK_STATUS_NOMEDIA;
    }

    return DISK_STATUS_OK;
}

static int disk_mmc_access_init(struct disk_info *disk)
{
    if (mmc_dev) {
        return 0;
    }

    mmc_dev =  device_get_binding(DEV_MMC0_NAME);  /*CONFIG_DISK_MMC_VOLUME_NAME*/
    if (!mmc_dev) {
           printk("#####failed#####enter line=[%d %s]#####\n",__LINE__, __FUNCTION__);
        return -ENODEV;
    }

    return 0;
}

static int disk_mmc_access_read(struct disk_info *disk, u8_t *buff,
                u32_t start_sector, u32_t sector_count)
{
    int blk_cnt;

    if (mmc_dev == NULL) {
         printk("#####failed#####enter line=[%d %s]#####\n",__LINE__, __FUNCTION__);
    }

    if (mmc_read(mmc_dev, start_sector, sector_count, buff) != 0) {
        return -EIO;
    }

    return 0;
}

static int disk_mmc_access_write(struct disk_info *disk, const u8_t *buff,
                 u32_t start_sector, u32_t sector_count)
{
    int blk_cnt;

    if (mmc_dev == NULL) {
         printk("#####failed#####enter line=[%d %s]#####\n", __LINE__, __FUNCTION__);
    }

    if (mmc_write(mmc_dev, start_sector, sector_count, buff) != 0) {
        printk("mmc_write failed#####enter line=[%d %s]#####\n", __LINE__, __FUNCTION__);
        return -EIO;
    }

    return 0;
}

static int disk_mmc_access_ioctl(struct disk_info *disk, u8_t cmd, void *buff)
{
    switch (cmd) {
    case DISK_IOCTL_CTRL_SYNC:
        break;
    case DISK_IOCTL_GET_SECTOR_COUNT:
        *(u32_t *)buff = CONFIG_DISK_VOLUME_SIZE / SECTOR_SIZE;
        break;
    case DISK_IOCTL_GET_SECTOR_SIZE:
        *(u32_t *)buff = SECTOR_SIZE;
        break;
    case DISK_IOCTL_GET_ERASE_BLOCK_SZ:
        *(u32_t *)buff  = CONFIG_DISK_ERASE_BLOCK_SIZE / SECTOR_SIZE;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

static const struct disk_operations mmc_disk_ops = {
    .init = disk_mmc_access_init,
    .status = disk_mmc_access_status,
    .read = disk_mmc_access_read,
    .write = disk_mmc_access_write,
    .ioctl = disk_mmc_access_ioctl,
};

static struct disk_info mmc_disk = {
    .name = DEV_MMC0_NAME,/*CONFIG_DISK_MMC_VOLUME_NAME,*/
    .ops = &mmc_disk_ops,
};

static int disk_mmc_init(struct device *dev)
{
    ARG_UNUSED(dev);

    return disk_access_register(&mmc_disk);
}

SYS_INIT(disk_mmc_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
