/**
 *
 * Copyright (C) 2018 Edgeless Semiconductors.  All Rights Reserved.
 * 
 * @file          disk_access_usb.c
 *
 * @author        wizephen.wang
 *
 * @version       1.1.0
 * 
 * @date          2019/4/16 
 *   
 * @brief         Simple implementation of wfst decoder.
 * 
 * @note
 *    2019/4/16, wizephen.wang, V1.0.0
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
#include "gm_usb_disk.h"

#define USB_PDRV      0

static int disk_usb_access_status(struct disk_info *disk)
{
	ARG_UNUSED(disk);
	int res = DISK_STATUS_OK;
	
	res = USB_HostMsdGetDiskStatus(USB_PDRV);
	
    return res;
}

static int disk_usb_access_init(struct disk_info *disk)
{   
	ARG_UNUSED(disk);
	
	USB_HostMsdInitializeDisk(USB_PDRV);	

    return DISK_STATUS_OK;
}

static int disk_usb_access_read(struct disk_info *disk, u8_t *buff,
                u32_t start_sector, u32_t sector_count)
{   
	ARG_UNUSED(disk);
	int res = DISK_STATUS_OK;	
	
    res = USB_HostMsdReadDisk(USB_PDRV, buff, start_sector, sector_count);
	
    return res;
}

static int disk_usb_access_write(struct disk_info *disk, const u8_t *buff,
                 u32_t start_sector, u32_t sector_count)
{
	ARG_UNUSED(disk);
	int res = DISK_STATUS_OK;
	
    res = USB_HostMsdWriteDisk(USB_PDRV, buff, start_sector, sector_count);
	
    return res;
}

static int disk_usb_access_ioctl(struct disk_info *disk, u8_t cmd, void *buff)
{
	ARG_UNUSED(disk);
	int res = DISK_STATUS_OK;
	
    res = USB_HostMsdIoctlDisk(USB_PDRV, cmd, buff);
    return res;
}

static const struct disk_operations usb_disk_ops = {
    .init = disk_usb_access_init,
    .status = disk_usb_access_status,
    .read = disk_usb_access_read,
    .write = disk_usb_access_write,
    .ioctl = disk_usb_access_ioctl,
};

static struct disk_info usb_disk = {
    .name = CONFIG_DISK_USB_VOLUME_NAME,  //"USB0"
    .ops = &usb_disk_ops,
};

static int disk_usb_init(struct device *dev)
{
    ARG_UNUSED(dev);

    return disk_access_register(&usb_disk);
}

SYS_INIT(disk_usb_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
