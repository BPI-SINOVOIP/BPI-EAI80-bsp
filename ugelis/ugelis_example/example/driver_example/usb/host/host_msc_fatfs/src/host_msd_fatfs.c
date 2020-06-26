/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016, 2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_msd.h"
#include "host_msd_fatfs.h"
#include "ff.h"
#include "diskio.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
#include "fs.h"

#define FATFS_MNTP       "/USB0:"
#define TEST_FILE        FATFS_MNTP"/testfile.txt"
#define TEST_DIR         FATFS_MNTP"/testdir"
#define TEST_DIR_FILE    FATFS_MNTP"/testdir/testfile.txt"

struct fs_file_t filep;

/* FatFs work area */
static FATFS fat_fs;

/* mounting info */
static struct fs_mount_t fatfs_mnt =
{
    .type = FS_FATFS,
    .mnt_point = FATFS_MNTP,
    .fs_data = &fat_fs,
};


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief host msd control transfer callback.
 *
 * This function is used as callback function for control transfer .
 *
 * @param param      the host msd fatfs instance pointer.
 * @param data       data buffer pointer.
 * @param dataLength data length.
 * @status           transfer result status.
 */
void USB_HostMsdControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*!
 * @brief msd fatfs test code execute done.
 */
static void USB_HostMsdFatfsTestDone(void);

/*!
 * @brief host msd fatfs test.
 *
 * This function implements msd fatfs test.
 *
 * @param msdFatfsInstance   the host fatfs instance pointer.
 */
static void USB_HostMsdFatfsTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief msd class handle array for fatfs */
extern usb_host_class_handle g_UsbFatfsClassHandle;

usb_host_msd_fatfs_instance_t g_MsdFatfsInstance; /* global msd fatfs instance */

/* control transfer on-going state. It should set to 1 when start control transfer, it is set to 0 in the callback */
volatile uint8_t controlIng;
/* control transfer callback status */
volatile usb_status_t controlStatus;


/*******************************************************************************
 * Code
 ******************************************************************************/

void USB_HostMsdControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_msd_fatfs_instance_t *msdFatfsInstance = (usb_host_msd_fatfs_instance_t *)param;

    if (msdFatfsInstance->runWaitState == kUSB_HostMsdRunWaitSetInterface) /* set interface finish */
    {
        msdFatfsInstance->runWaitState = kUSB_HostMsdRunIdle;
        msdFatfsInstance->runState = kUSB_HostMsdRunMassStorageTest;
    }
    controlIng = 0;
    controlStatus = status;
}

static void USB_HostMsdFatfsTestDone(void)
{
    usb_echo("............................test done......................\r\n");
}

int Check_File_Dir_Exists(const char *path)
{
    int res;
    struct fs_dirent entry;

    /* Verify fs_stat() */
    res = fs_stat(path, &entry);

    return !res;
}

static int USB_HostTestStatvfs(void)
{
    struct fs_statvfs stat;
    int res;

    res = fs_statvfs(FATFS_MNTP, &stat);
    if (res)
    {
        usb_echo("Error getting volume stats [%d]\n", res);
        return res;
    }

    usb_echo("Optimal transfer block size   = %lu\n", stat.f_bsize);
    usb_echo("Allocation unit size          = %lu\n", stat.f_frsize);
    usb_echo("Volume size in f_frsize units = %lu\n", stat.f_blocks);
    usb_echo("Free space in f_frsize units  = %lu\n", stat.f_bfree);

    return 0;
}

static int USB_HostTestFileopen(void)
{
    int res;

    if (Check_File_Dir_Exists(TEST_FILE))
    {
        usb_echo("Opening existing file %s\n", TEST_FILE);
    }
    else
    {
        usb_echo("Creating new file %s\n", TEST_FILE);
    }

    res = fs_open(&filep, TEST_FILE);
    if (res)
    {
        usb_echo("Failed opening file [%d]\n", res);
        return res;
    }

    usb_echo("Opened file %s\n", TEST_FILE);

    return res;
}

const char test_str[] = "hello world!";

int USB_HostTestFileWrite(void)
{
    ssize_t brw;
    int res;

    res = fs_seek(&filep, 0, FS_SEEK_SET);
    if (res)
    {
        usb_echo("fs_seek failed [%d]\n", res);
        fs_close(&filep);
        return res;
    }

    usb_echo("Data written:\"%s\"\n\n", test_str);

    brw = fs_write(&filep, (char *)test_str, strlen(test_str));
    if (brw < 0)
    {
        usb_echo("Failed writing to file [%zd]\n", brw);
        fs_close(&filep);
        return brw;
    }

    if (brw < strlen(test_str))
    {
        usb_echo("Unable to complete write. Volume full.\n");
        usb_echo("Number of bytes written: [%zd]\n", brw);
        fs_close(&filep);
        return -1;
    }

    usb_echo("Data successfully written!\n");

    return res;
}

static int USB_HostTestFileSync(void)
{
    int res;

    res = fs_sync(&filep);
    if (res)
    {
        usb_echo("Error syncing file [%d]\n", res);
        return res;
    }

    return res;
}

static int USB_HostTestFileRead(void)
{
    ssize_t brw;
    int res;
    char read_buff[80];
    size_t sz = strlen(test_str);

    res = fs_seek(&filep, 0, FS_SEEK_SET);
    if (res)
    {
        usb_echo("fs_seek failed [%d]\n", res);
        fs_close(&filep);
        return res;
    }

    brw = fs_read(&filep, read_buff, sz);
    if (brw < 0)
    {
        usb_echo("Failed reading file [%zd]\n", brw);
        fs_close(&filep);
        return brw;
    }

    read_buff[brw] = 0;

    usb_echo("Data read:\"%s\"\n\n", read_buff);

    if (strcmp(test_str, read_buff))
    {
        usb_echo("Error - Data read does not match data written\n");
        usb_echo("Data read:\"%s\"\n\n", read_buff);
        return -1;
    }

    usb_echo("Data read matches data written\n");

    return res;
}

static int USB_HostTestFileTruncate(void)
{
    int res;
    off_t orig_pos;
    char read_buff[80];
    ssize_t brw;

    /* Test truncating to 0 size */
    usb_echo("\nTesting shrink to 0 size\n");
    res = fs_truncate(&filep, 0);
    if (res)
    {
        usb_echo("fs_truncate failed [%d]\n", res);
        fs_close(&filep);
        return res;
    }

    fs_seek(&filep, 0, FS_SEEK_END);
    /* Verify fs_tell() */
    if (fs_tell(&filep) > 0)
    {
        usb_echo("Failed truncating to size 0\n");
        fs_close(&filep);
        return -1;
    }

    usb_echo("Testing write after truncating\n");
    res = USB_HostTestFileWrite();
    if (res)
    {
        usb_echo("Write failed after truncating\n");
        return res;
    }

    fs_seek(&filep, 0, FS_SEEK_END);

    orig_pos = fs_tell(&filep);
    usb_echo("Original size of file = %d\n", orig_pos);

    /* Test shrinking file */
    usb_echo("\nTesting shrinking\n");
    res = fs_truncate(&filep, orig_pos - 5);
    if (res)
    {
        usb_echo("fs_truncate failed [%d]\n", res);
        fs_close(&filep);
        return res;
    }

    fs_seek(&filep, 0, FS_SEEK_END);
    usb_echo("File size after shrinking by 5 bytes = %d\n",
             fs_tell(&filep));
    if (fs_tell(&filep) != (orig_pos - 5))
    {
        usb_echo("File size after fs_truncate not as expected\n");
        fs_close(&filep);
        return -1;
    }

    /* Test expanding file */
    usb_echo("\nTesting expanding\n");
    fs_seek(&filep, 0, FS_SEEK_END);
    orig_pos = fs_tell(&filep);
    res = fs_truncate(&filep, orig_pos + 10);
    if (res)
    {
        usb_echo("fs_truncate failed [%d]\n", res);
        fs_close(&filep);
        return res;
    }

    fs_seek(&filep, 0, FS_SEEK_END);
    usb_echo("File size after expanding by 10 bytes = %d\n",
             fs_tell(&filep));
    if (fs_tell(&filep) != (orig_pos + 10))
    {
        usb_echo("File size after fs_truncate not as expected\n");
        fs_close(&filep);
        return -1;
    }

    /* Check if expanded regions are zeroed */
    usb_echo("Testing for zeroes in expanded region\n");
    fs_seek(&filep, -5, FS_SEEK_END);

    brw = fs_read(&filep, read_buff, 5);

    if (brw < 5)
    {
        usb_echo("Read failed after truncating\n");
        fs_close(&filep);
        return -1;
    }

    for (int i = 0; i < 5; i++)
    {
        if (read_buff[i])
        {
            usb_echo("Expanded regions are not zeroed\n");
            fs_close(&filep);
            return -1;
        }
    }

    return 0;
}

static int USB_HostTestFileClose(void)
{
    int res;

    res = fs_close(&filep);
    if (res)
    {
        usb_echo("Error closing file [%d]\n", res);
        return res;
    }

    usb_echo("Closed file %s\n", TEST_FILE);

    return res;
}

static int USB_HostTestFileDelete(void)
{
    int res;

    res = fs_unlink(TEST_FILE);
    if (res)
    {
        usb_echo("Error deleting file [%d]\n", res);
        return res;
    }

    /* Check if file was deleted */
    if (Check_File_Dir_Exists(TEST_FILE))
    {
        usb_echo("Failed deleting %s\n", TEST_FILE);
        return -1;
    }

    usb_echo("File (%s) deleted successfully!\n", TEST_FILE);

    return res;
}

static int USB_HostTestFsMount(void)
{
    int res;

    res = fs_mount(&fatfs_mnt);
    if (res < 0)
    {
        usb_echo("Error mounting fs [%d]\n", res);
        return -1;
    }

    usb_echo("Mount successfully!\n");

    return res;
}

static void USB_HostMsdFatfsTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance)
{
    FRESULT fatfsCode;
    uint32_t freeClusterNumber;

    /* time delay */
    for (freeClusterNumber = 0; freeClusterNumber < 10000; ++freeClusterNumber)
    {
        __asm("nop");
    }

    usb_echo("........usb host file system test...........\r\n\n");

    usb_echo("usb mount ...\n");
    fatfsCode = USB_HostTestFsMount();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb stat ...\r\n");
    fatfsCode = USB_HostTestStatvfs();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb file open ...\r\n");
    fatfsCode = USB_HostTestFileopen();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb file write ...\r\n");
    fatfsCode = USB_HostTestFileWrite();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb sync ...\n");
    fatfsCode = USB_HostTestFileSync();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb read ...\n");
    fatfsCode = USB_HostTestFileRead();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb truncate ...\n");
    fatfsCode = USB_HostTestFileTruncate();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb fileclose ...\n");
    fatfsCode = USB_HostTestFileClose();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

    usb_echo("usb file delete ...\n");
    fatfsCode = USB_HostTestFileDelete();
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        goto testdone;
    }
    usb_echo("success !!!\r\n");

testdone:

    USB_HostMsdFatfsTestDone();
}


void USB_HostMsdTask(void *arg)
{
    usb_status_t status;
    usb_host_msd_fatfs_instance_t *msdFatfsInstance = (usb_host_msd_fatfs_instance_t *)((uint32_t)arg);

    if (msdFatfsInstance->deviceState != msdFatfsInstance->prevDeviceState)
    {
        msdFatfsInstance->prevDeviceState = msdFatfsInstance->deviceState;
        switch (msdFatfsInstance->deviceState)
        {
            case kStatus_DEV_Idle:
                break;

            case kStatus_DEV_Attached: /* deivce is attached and numeration is done */
                status = USB_HostMsdInit(msdFatfsInstance->deviceHandle,
                                         &msdFatfsInstance->classHandle); /* msd class initialization */
                g_UsbFatfsClassHandle = msdFatfsInstance->classHandle;
                if (status != kStatus_USB_Success)
                {
                    usb_echo("usb host msd init fail\r\n");
                    return;
                }
                msdFatfsInstance->runState = kUSB_HostMsdRunSetInterface;
                break;

            case kStatus_DEV_Detached: /* device is detached */
                msdFatfsInstance->deviceState = kStatus_DEV_Idle;
                msdFatfsInstance->runState = kUSB_HostMsdRunIdle;
                USB_HostMsdDeinit(msdFatfsInstance->deviceHandle,
                                  msdFatfsInstance->classHandle); /* msd class de-initialization */
                msdFatfsInstance->classHandle = NULL;

                usb_echo("mass storage device detached\r\n");
                break;

            default:
                break;
        }
    }

    /* run state */
    switch (msdFatfsInstance->runState)
    {
        case kUSB_HostMsdRunIdle:
            break;

        case kUSB_HostMsdRunSetInterface: /* set msd interface */
            msdFatfsInstance->runState = kUSB_HostMsdRunIdle;
            msdFatfsInstance->runWaitState = kUSB_HostMsdRunWaitSetInterface;
            status = USB_HostMsdSetInterface(msdFatfsInstance->classHandle, msdFatfsInstance->interfaceHandle, 0,
                                             USB_HostMsdControlCallback, msdFatfsInstance);
            if (status != kStatus_USB_Success)
            {
                usb_echo("set interface fail\r\n");
            }
            break;

        case kUSB_HostMsdRunMassStorageTest: /* set interface succeed */
#if ((defined MSD_FATFS_THROUGHPUT_TEST_ENABLE) && (MSD_FATFS_THROUGHPUT_TEST_ENABLE))
            USB_HostMsdFatfsThroughputTest(msdFatfsInstance); /* test throughput */
#else
            USB_HostMsdFatfsTest(msdFatfsInstance); /* test msd device */
#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */
            msdFatfsInstance->runState = kUSB_HostMsdRunIdle;
            break;

        default:
            break;
    }
}

usb_status_t USB_HostMsdEvent(usb_device_handle deviceHandle,
                              usb_host_configuration_handle configurationHandle,
                              uint32_t eventCode)
{
    usb_status_t status = kStatus_USB_Success;
    usb_host_configuration_t *configuration;
    uint8_t interfaceIndex;
    usb_host_interface_t *interface;
    uint32_t infoValue;
    uint8_t id;

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[interfaceIndex];
                id = interface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_MSD_CLASS_CODE)
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceSubClass;
                if ((id != USB_HOST_MSD_SUBCLASS_CODE_UFI) && (id != USB_HOST_MSD_SUBCLASS_CODE_SCSI))
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceProtocol;
                if (id != USB_HOST_MSD_PROTOCOL_BULK)
                {
                    continue;
                }
                else
                {
                    if (g_MsdFatfsInstance.deviceState == kStatus_DEV_Idle)
                    {
                        /* the interface is supported by the application */
                        g_MsdFatfsInstance.deviceHandle = deviceHandle;
                        g_MsdFatfsInstance.interfaceHandle = interface;
                        g_MsdFatfsInstance.configHandle = configurationHandle;
                        return kStatus_USB_Success;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventNotSupported:
            break;

        case kUSB_HostEventEnumerationDone:
            if (g_MsdFatfsInstance.configHandle == configurationHandle)
            {
                if ((g_MsdFatfsInstance.deviceHandle != NULL) && (g_MsdFatfsInstance.interfaceHandle != NULL))
                {
                    /* the device enumeration is done */
                    if (g_MsdFatfsInstance.deviceState == kStatus_DEV_Idle)
                    {
                        g_MsdFatfsInstance.deviceState = kStatus_DEV_Attached;

                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &infoValue);
                        usb_echo("mass storage device attached:pid=0x%x", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &infoValue);
                        usb_echo("vid=0x%x ", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                        usb_echo("address=%d\r\n", infoValue);
                    }
                    else
                    {
                        usb_echo("not idle msd instance\r\n");
                        status = kStatus_USB_Error;
                    }
                }
            }
            break;

        case kUSB_HostEventDetach:
            if (g_MsdFatfsInstance.configHandle == configurationHandle)
            {
                /* the device is detached */
                g_UsbFatfsClassHandle = NULL;
                g_MsdFatfsInstance.configHandle = NULL;
                if (g_MsdFatfsInstance.deviceState != kStatus_DEV_Idle)
                {
                    g_MsdFatfsInstance.deviceState = kStatus_DEV_Detached;
                }
            }
            break;

        default:
            break;
    }
    return status;
}
