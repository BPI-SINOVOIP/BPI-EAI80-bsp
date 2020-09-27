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
#include "usb_main.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if MSD_FATFS_THROUGHPUT_TEST_ENABLE
    #include "fsl_device_registers.h"
    #define THROUGHPUT_BUFFER_SIZE (64 * 1024) /* throughput test buffer */
    #define MCU_CORE_CLOCK (120000000)         /* mcu core clock, user need to configure it. */
#endif                                     /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

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

#if ((defined MSD_FATFS_THROUGHPUT_TEST_ENABLE) && (MSD_FATFS_THROUGHPUT_TEST_ENABLE))
    /*!
    * @brief host msd fatfs throughput test.
    *
    * @param msdFatfsInstance   the host fatfs instance pointer.
    */
    static void USB_HostMsdFatfsThroughputTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance);

#else

    /*!
    * @brief display file information.
    */
    static void USB_HostMsdFatfsDisplayFileInfo(FILINFO *fileInfo);

    /*!
    * @brief list files and sub-directory in one directory, the function don't check all sub-directories recursively.
    */
    static FRESULT USB_HostMsdFatfsListDirectory(const TCHAR *path);

    /*!
    * @brief forward function pointer for fatfs f_forward function.
    *
    * @param data_ptr   forward data pointer.
    * @param dataLength data length.
    */
    #if _USE_FORWARD && _FS_TINY
        static uint32_t USB_HostMsdFatfsForward(const uint8_t *data_ptr, uint32_t dataLength);
    #endif

    /*!
    * @brief host msd fatfs test.
    *
    * This function implements msd fatfs test.
    *
    * @param msdFatfsInstance   the host fatfs instance pointer.
    */
    static void USB_HostMsdFatfsTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance);

#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief msd class handle array for fatfs */
extern usb_host_class_handle g_UsbFatfsClassHandle;

usb_host_msd_fatfs_instance_t g_MsdFatfsInstance; /* global msd fatfs instance */
static FATFS fatfs;
/* control transfer on-going state. It should set to 1 when start control transfer, it is set to 0 in the callback */
volatile uint8_t controlIng;
/* control transfer callback status */
volatile usb_status_t controlStatus;

#if MSD_FATFS_THROUGHPUT_TEST_ENABLE
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint32_t testThroughputBuffer[THROUGHPUT_BUFFER_SIZE / 4]; /* the buffer for throughput test */
uint32_t testSizeArray[] = {20 * 1024, 20 * 1024}; /* test time and test size (uint: K)*/
#else
USB_DMA_NONINIT_DATA_ALIGN(4) static uint8_t testBuffer[512]; /* normal test buffer */
#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

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
    usb_echo("..................................................\r\n");
}

#if ((defined MSD_FATFS_THROUGHPUT_TEST_ENABLE) && (MSD_FATFS_THROUGHPUT_TEST_ENABLE))

static void USB_HostMsdFatfsThroughputTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance)
{
    uint64_t totalTime;
    FRESULT fatfsCode;
    FIL file;
    uint32_t resultSize;
    uint32_t testSize;
    uint8_t testIndex;
    char test_file_name[30];

    /* time delay (~100ms) */
    for (resultSize = 0; resultSize < 400000; ++resultSize)
    {
        __ASM("nop");
    }

    usb_echo("............................fatfs test.....................\r\n");
    CoreDebug->DEMCR |= (1 << CoreDebug_DEMCR_TRCENA_Pos);

    for (testSize = 0; testSize < (THROUGHPUT_BUFFER_SIZE / 4); ++testSize)
    {
        testThroughputBuffer[testSize] = testSize;
    }

    sprintf(test_file_name, "%c:", USBDISK + '0');
    fatfsCode = f_mount(&fatfs, test_file_name, 1);
    if (fatfsCode)
    {
        usb_echo("fatfs mount error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }

    sprintf(test_file_name, "%c:/thput.dat", USBDISK + '0');
    usb_echo("throughput test:\r\n");
    for (testIndex = 0; testIndex < (sizeof(testSizeArray) / 4); ++testIndex)
    {
        fatfsCode = f_unlink(test_file_name); /* delete the file if it is existed */
        if ((fatfsCode != FR_OK) && (fatfsCode != FR_NO_FILE))
        {
            USB_HostMsdFatfsTestDone();
            return;
        }

        fatfsCode = f_open(&file, test_file_name, FA_WRITE | FA_READ | FA_CREATE_ALWAYS); /* create one new file */
        if (fatfsCode)
        {
            USB_HostMsdFatfsTestDone();
            return;
        }

        totalTime = 0;
        testSize = testSizeArray[testIndex] * 1024;
        while (testSize)
        {
            if (msdFatfsInstance->deviceState != kStatus_DEV_Attached)
            {
                USB_HostMsdFatfsTestDone();
                return;
            }
            DWT->CYCCNT = 0;
            DWT->CTRL |= (1 << DWT_CTRL_CYCCNTENA_Pos);
            fatfsCode = f_write(&file, testThroughputBuffer, THROUGHPUT_BUFFER_SIZE, &resultSize);
            if (fatfsCode)
            {
                usb_echo("write error\r\n");
                f_close(&file);
                USB_HostMsdFatfsTestDone();
                return;
            }
            totalTime += DWT->CYCCNT;
            DWT->CTRL &= ~(1 << DWT_CTRL_CYCCNTENA_Pos);
            testSize -= THROUGHPUT_BUFFER_SIZE;
        }
        testSize = testSizeArray[testIndex];
        usb_echo("    write %dKB data the speed is %d KB/s\r\n", testSize,
                 (uint32_t)((uint64_t)testSize * (uint64_t)MCU_CORE_CLOCK / (uint64_t)totalTime));

        fatfsCode = f_lseek(&file, 0);
        if (fatfsCode)
        {
            USB_HostMsdFatfsTestDone();
            return;
        }
        totalTime = 0;
        testSize = testSizeArray[testIndex] * 1024;
        while (testSize)
        {
            if (msdFatfsInstance->deviceState != kStatus_DEV_Attached)
            {
                USB_HostMsdFatfsTestDone();
                return;
            }
            DWT->CYCCNT = 0;
            DWT->CTRL |= (1 << DWT_CTRL_CYCCNTENA_Pos);
            fatfsCode = f_read(&file, testThroughputBuffer, THROUGHPUT_BUFFER_SIZE, &resultSize);
            if (fatfsCode)
            {
                usb_echo("read error\r\n");
                f_close(&file);
                USB_HostMsdFatfsTestDone();
                return;
            }
            totalTime += DWT->CYCCNT;
            DWT->CTRL &= ~(1 << DWT_CTRL_CYCCNTENA_Pos);
            testSize -= THROUGHPUT_BUFFER_SIZE;
        }
        testSize = testSizeArray[testIndex];
        usb_echo("    read %dKB data the speed is %d KB/s\r\n", testSize,
                 (uint32_t)((uint64_t)testSize * (uint64_t)MCU_CORE_CLOCK / (uint64_t)totalTime));

        fatfsCode = f_close(&file);
        if (fatfsCode)
        {
            USB_HostMsdFatfsTestDone();
            return;
        }
    }

    USB_HostMsdFatfsTestDone();
}

#else

static void USB_HostMsdFatfsDisplayFileInfo(FILINFO *fileInfo)
{
    char *fileName;
#if _USE_LFN
    fileName = (fileInfo->lfname[0] ? fileInfo->lfname : fileInfo->fname;
#else
    fileName = fileInfo->fname;
#endif /* _USE_LFN */
                /* note: if this file/directory don't have one attribute, '_' replace the attribute letter ('R' - readonly, 'H' - hide, 'S' - system) */
                usb_echo("    %s - %c%c%c - %s - %dBytes - %d-%d-%d %d:%d:%d\r\n", (fileInfo->fattrib & AM_DIR) ? "dir" : "fil",
                         (fileInfo->fattrib & AM_RDO) ? 'R' : '_',
                         (fileInfo->fattrib & AM_HID) ? 'H' : '_',
                         (fileInfo->fattrib & AM_SYS) ? 'S' : '_',
                         fileName,
                         (fileInfo->fsize),
                         (uint32_t)((fileInfo->fdate >> 9) + 1980) /* year */,
                         (uint32_t)((fileInfo->fdate >> 5) & 0x000Fu) /* month */,
                         (uint32_t)(fileInfo->fdate & 0x001Fu) /* day */,
                         (uint32_t)((fileInfo->ftime >> 11) & 0x0000001Fu) /* hour */,
                         (uint32_t)((fileInfo->ftime >> 5) & 0x0000003Fu) /* minute */,
                         (uint32_t)(fileInfo->ftime & 0x0000001Fu) /* second */
                        );
}

           static FRESULT USB_HostMsdFatfsListDirectory(const TCHAR *path)
{
    FRESULT fatfsCode = FR_OK;
    FILINFO fileInfo;
    DIR dir;
    uint8_t outputLabel = 0;

#if _USE_LFN
    static uint8_t fileNameBuffer[_MAX_LFN];
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = _MAX_LFN;
#endif /* _USE_LFN */

    fatfsCode = f_opendir(&dir, path);
    if (fatfsCode)
    {
        return fatfsCode;
    }
    while (1)
    {
        fatfsCode = f_readdir(&dir, &fileInfo);
        if ((fatfsCode) || (!fileInfo.fname[0]))
        {
            break;
        }
        outputLabel = 1;
        USB_HostMsdFatfsDisplayFileInfo(&fileInfo);
    }
    if (!outputLabel)
    {
        usb_echo("\r\n");
    }

    return fatfsCode;
}

#if _USE_FORWARD && _FS_TINY
static uint32_t USB_HostMsdFatfsForward(const uint8_t *data, uint32_t dataLength)
{
    uint32_t resultCount = dataLength;

    if (dataLength == 0)
    {
        return 1;
    }
    else
    {
        do
        {
            usb_echo("%c", *data);
            data++;
            resultCount--;
        } while (resultCount);
        return dataLength;
    }
}
#endif


extern unsigned char RecvBuffer[];
extern unsigned char CMD_BASE[];
extern unsigned char CMD_BASE_BODY[];
extern unsigned char CMD_BASE_GEST[];
extern unsigned char WEI_BASE[];

extern unsigned char MAGFLG_FOR_MODELLOADED[];
#define MAGFLG_DATA 0x5a5aa5a5


static void USB_HostMsdFatfsTest(usb_host_msd_fatfs_instance_t *msdFatfsInstance)
{
    FRESULT fatfsCode;
    FATFS *fs;
    FIL file;
    FILINFO fileInfo;
    uint32_t freeClusterNumber;
    uint32_t index;
    uint32_t resultSize;
    char *testString;
    uint8_t driverNumberBuffer[3];
    uint32_t i = 0;

#if _USE_LFN
    static uint8_t fileNameBuffer[_MAX_LFN];
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = _MAX_LFN;
#endif /* _USE_LFN */

    /* time delay */
    for (freeClusterNumber = 0; freeClusterNumber < 10000; ++freeClusterNumber)
    {
        __asm("nop");
    }

#if 0
    for (i = 0; i < 4559728; i++)
    {
        WEI_BASE[i] = 0x5A;
    }
#endif

    usb_echo("Detected a USB device attached\r\n");

    usb_echo("fatfs mount as logiacal driver %d...... \r\n", USBDISK);
    sprintf((char *)&driverNumberBuffer[0], "%c:", USBDISK + '0');
    fatfsCode = f_mount(&fatfs, (char const *)&driverNumberBuffer[0], 0);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("Mount USB-Device Success\r\n");

    *(volatile uint32_t *)MAGFLG_FOR_MODELLOADED = MAGFLG_DATA;

    USB_HostMsdFatfsTestDone();
    return ;

}

#endif /* MSD_FATFS_THROUGHPUT_TEST_ENABLE */

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

                usb_echo("USB Attached ... \n");
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

    //printk("usb device-state: %d , msdFatfsInstance->runState: %d \n", msdFatfsInstance->deviceState,msdFatfsInstance->runState);

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


#if 0
typedef enum
{
    MODEL_BODY = 0,
    MODEL_GEST = 1,
    MODEL_FACE = 2,
    MODEL_NOTDEF = 0xff
} model_type;
#endif

void load_ai_model(uint8_t type)  //model_type
{
    FRESULT fatfsCode;
    FATFS *fs;
    FIL file;
    FILINFO fileInfo;
    uint32_t resultSize;
    uint8_t driverNumberBuffer[3];
    uint32_t freeClusterNumber;

    /* time delay */
    for (freeClusterNumber = 0; freeClusterNumber < 10000; ++freeClusterNumber)
    {
        __asm("nop");
    }

#if 0
    usb_echo("fatfs mount as logiacal driver %d......", USBDISK);
    sprintf((char *)&driverNumberBuffer[0], "%c:", USBDISK + '0');
    fatfsCode = f_mount(&fatfs, (char const *)&driverNumberBuffer[0], 0);
    if (fatfsCode)
    {
        usb_echo("error\r\n");
        USB_HostMsdFatfsTestDone();
        return;
    }
    usb_echo("success\r\n");
#endif

    *(volatile uint32_t *)MAGFLG_FOR_MODELLOADED = 0x00000000;

    if (type == 0)
    {
        fatfsCode = f_open(&file, _T("1:/body/cmd.bin"), FA_WRITE | FA_READ);
        if (fatfsCode)
        {
            if (fatfsCode == FR_EXIST)
            {
                usb_echo("file exist\r\n");
            }
            else
            {
                usb_echo("error\r\n");
                USB_HostMsdFatfsTestDone();
                return;
            }
        }
        else
        {
            usb_echo("success\r\n");
        }

        fatfsCode = f_stat(_T("1:/body/cmd.bin"), &fileInfo);
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("body.cmd.bin size = %d \n", fileInfo.fsize);

        fatfsCode = f_read(&file, CMD_BASE, fileInfo.fsize, (UINT *)&resultSize); //CMD_BASE_BODY
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            f_close(&file);
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("read cmd done\r\n");
        f_close(&file);

        fatfsCode = f_open(&file, _T("1:/body/pic.bin"), FA_WRITE | FA_READ);
        if (fatfsCode)
        {
            if (fatfsCode == FR_EXIST)
            {
                usb_echo("file exist\r\n");
            }
            else
            {
                usb_echo("error\r\n");
                USB_HostMsdFatfsTestDone();
                return;
            }
        }
        else
        {
            usb_echo("success\r\n");
        }

        fatfsCode = f_stat(_T("1:/body/pic.bin"), &fileInfo);
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("body.pic.bin size = %d \n", fileInfo.fsize);

        fatfsCode = f_read(&file, RecvBuffer, fileInfo.fsize, (UINT *)&resultSize);// RecvBufferBody
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            f_close(&file);
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("read demo pic done\r\n");

        f_close(&file);

        fatfsCode = f_open(&file, _T("1:/body/weight.bin"), FA_WRITE | FA_READ);
        if (fatfsCode)
        {
            if (fatfsCode == FR_EXIST)
            {
                usb_echo("file exist\r\n");
            }
            else
            {
                usb_echo("error\r\n");
                USB_HostMsdFatfsTestDone();
                return;
            }
        }
        else
        {
            usb_echo("success\r\n");
        }


        fatfsCode = f_stat(_T("1:/body/weight.bin"), &fileInfo);
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("body.weight.bin size = %d \n", fileInfo.fsize);

        fatfsCode = f_read(&file, WEI_BASE, fileInfo.fsize, (UINT *)&resultSize); //WEI_BASE_BODY
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            f_close(&file);
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("read weight done\r\n");

        f_close(&file);
    }

    if (type == 1)
    {
        fatfsCode = f_open(&file, _T("1:/gest/weight.bin"), FA_WRITE | FA_READ);
        if (fatfsCode)
        {
            if (fatfsCode == FR_EXIST)
            {
                usb_echo("file exist\r\n");
            }
            else
            {
                usb_echo("error\r\n");
                USB_HostMsdFatfsTestDone();
                return;
            }
        }
        else
        {
            usb_echo("success\r\n");
        }

        fatfsCode = f_stat(_T("1:/gest/weight.bin"), &fileInfo);
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("gest.weight.bin size = %d \n", fileInfo.fsize);

        fatfsCode = f_read(&file, WEI_BASE, fileInfo.fsize, (UINT *)&resultSize);  //WEI_BASE_GEST
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            f_close(&file);
            USB_HostMsdFatfsTestDone();
            return;
        }

        f_close(&file);

        fatfsCode = f_open(&file, _T("1:/gest/cmd.bin"), FA_WRITE | FA_READ);
        if (fatfsCode)
        {
            if (fatfsCode == FR_EXIST)
            {
                usb_echo("file exist\r\n");
            }
            else
            {
                usb_echo("error\r\n");
                USB_HostMsdFatfsTestDone();
                return;
            }
        }
        else
        {
            usb_echo("success\r\n");
        }

        fatfsCode = f_stat(_T("1:/gest/cmd.bin"), &fileInfo);
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("gest.cmd.bin size = %d \n", fileInfo.fsize);


        fatfsCode = f_read(&file, CMD_BASE, fileInfo.fsize, (UINT *)&resultSize); //CMD_BASE_GEST
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            f_close(&file);
            USB_HostMsdFatfsTestDone();
            return;
        }

        f_close(&file);


        fatfsCode = f_open(&file, _T("1:/gest/pic.bin"), FA_WRITE | FA_READ);
        if (fatfsCode)
        {
            if (fatfsCode == FR_EXIST)
            {
                usb_echo("file exist\r\n");
            }
            else
            {
                usb_echo("error\r\n");
                USB_HostMsdFatfsTestDone();
                return;
            }
        }
        else
        {
            usb_echo("success\r\n");
        }

        fatfsCode = f_stat(_T("1:/gest/pic.bin"), &fileInfo);
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            USB_HostMsdFatfsTestDone();
            return;
        }

        usb_echo("gest.pic.bin size = %d \n", fileInfo.fsize);

        fatfsCode = f_read(&file, RecvBuffer, fileInfo.fsize, (UINT *)&resultSize); // RecvBufferGest
        if (fatfsCode)
        {
            usb_echo("error\r\n");
            f_close(&file);
            USB_HostMsdFatfsTestDone();
            return;
        }

        f_close(&file);
    }

    if (type == 2)
    {

    }

    *(volatile uint32_t *)MAGFLG_FOR_MODELLOADED = MAGFLG_DATA;
    USB_HostMsdFatfsTestDone();
}
