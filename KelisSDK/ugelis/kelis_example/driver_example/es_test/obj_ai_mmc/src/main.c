#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>

#include "ai_common.h"
#include "tinyyolo_main.h"
#include "math.h"
#include "lcd.h"
#include "camera.h"
#include "draw.h"
#include <mem_cfg.h>
#include <gm_hal_mmc.h>
#include <gpio.h>

/*jpg encoder*/
#include <jinclude.h>
#include <jcapi.h>
#include "test_jpg.h"

/*usb */
#include <ugelis.h>
#include "main.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_video.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "video_data.h"
#include "usb_device_khci.h"

#include <stdio.h>
#include <stdlib.h>

#if 1
    extern unsigned char RecvBuffer[];
    extern unsigned char CMD_BASE[];
    extern unsigned char CMD_BASE_BODY[];
    extern unsigned char CMD_BASE_GEST[];
    extern unsigned char WEI_BASE[];
    #ifdef BODY_DETECTION_DEMO
        extern unsigned char WEI_BASE_BODY[];
        extern unsigned char RecvBufferBody[];
    #endif

    #ifdef GESTURE_DETECTION_DEMO
        extern unsigned char WEI_BASE_GEST[];
        extern unsigned char RecvBufferGest[];
    #endif

    extern unsigned char UNREG_DUMP_BUFF[];
    extern unsigned char REG_DUMP_BUFF[];

    extern unsigned short inputImage[];
    extern unsigned short OutputImage[];
#else
    extern unsigned short *inputImage;
    extern unsigned short *OutputImage;
    extern unsigned char  *CMD_BASE;
    extern unsigned char  *CMD_BASE_BODY;
    extern unsigned char  *CMD_BASE_GEST;
    extern unsigned char  *WEI_BASE;
    extern unsigned char  *WEI_BASE_BODY;
    extern unsigned char  *WEI_BASE_GEST;
    extern unsigned char  *RecvBuffer;
    extern unsigned char  *RecvBufferBody;
    extern unsigned char  *RecvBufferGest;
    extern unsigned char  *UNREG_DUMP_BUFF;
    extern unsigned char  *REG_DUMP_BUFF;

#endif

#define UGELIS_NEW_VERSION 1

extern struct device *dcmi;
extern struct device *ltdc;

ObjDetData obj_det_data;

node_t *base_head = NULL;
node_t *previousNode = NULL;


#define MAX_PERSON_CNT 20
#define MAX_CMP_CNT 5
#define ONE_PERSON_PICS_CNT 5
#define BUF_SIZE (MAX_PERSON_CNT*ONE_PERSON_PICS_CNT)
#define CAM_PIC_SIZE  (224 * 224 * 3)

#define OBJ_PRINTF printk
//#define OBJ_PRINTF(...)  do{} while(0);

//#define OBJ_DBG_PRINTF printk
#define OBJ_DBG_PRINTF do{} while(0);



uint8_t camvedio_read_done = 0;
uint8_t thread_write_done = 0;

extern uint32_t show_buf_addr;

static int index[BUF_SIZE];

#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

static char *object_name[MAX_OBJECTCLASS_CNT] =
{
    "aeroplane",
    "bicyle",
    "bird",
    "boat",
    "bottle",
    "bus",
    "car",
    "cat",
    "chair",
    "cow",
    "diningtable",
    "dog",
    "horse",
    "motorbike",
    "person",
    "pottedplant",
    "sheep",
    "sofa",
    "train",
    "tvmonitor"
};

static char *gesture_name[MAX_GESTURES_CNT] =
{
    "fist",
    "palm"
};

static uint32_t object_colordrawn[MAX_OBJECTCLASS_CNT] =
{
    0xFFFF0000,
    0xFF00FF00,
    0xFFFF00FF,
    0xFFFF0040,
    0xFF00FF40,
    0xFFFF0040,
    0xFFFF0080,
    0xFF00FF80,
    0xFFFF0080,
    0xFFFF00C0,
    0xFF00FFC0,
    0xFFFF00C0,
    0xFFFF00FF,
    0xFF00FFFF,
    0xFFFF00FF,
    0xFFFF40FF,
    0xFF40FFFF,
    0xFFFF40FF,
    0xFFFF80FF,
    0xFF80FFFF,
};



#if 1
/*USB .............. demo*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void USB_DeviceIsrEnable(void);
#if USB_DEVICE_CONFIG_USE_TASK
    void USB_DeviceTaskFn(void *deviceHandle);
#endif

static void USB_DeviceVideoPrepareVideoData(void);
static usb_status_t USB_DeviceVideoRequest(class_handle_t handle, uint32_t event, void *param);
static usb_status_t USB_DeviceVideoCallback(class_handle_t handle, uint32_t event, void *param);
static void USB_DeviceVideoApplicationSetDefault(void);
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);
static void USB_DeviceApplicationInit(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern unsigned char g_UsbDeviceVideoMjpegData[]; //const
//extern const unsigned char g_UsbDeviceVideoMjpegData[];
extern const uint32_t g_UsbDeviceVideoMjpegLength;

extern usb_device_class_struct_t g_UsbDeviceVideoVirtualCameraConfig;

USB_DMA_NONINIT_DATA_ALIGN(4) static usb_device_video_probe_and_commit_controls_struct_t s_ProbeStruct;
USB_DMA_NONINIT_DATA_ALIGN(4) static usb_device_video_probe_and_commit_controls_struct_t s_CommitStruct;
USB_DMA_NONINIT_DATA_ALIGN(4) static usb_device_video_still_probe_and_commit_controls_struct_t s_StillProbeStruct;
USB_DMA_NONINIT_DATA_ALIGN(4) static usb_device_video_still_probe_and_commit_controls_struct_t s_StillCommitStruct;
USB_DMA_NONINIT_DATA_ALIGN(4) static uint32_t s_ClassRequestBuffer[(sizeof(usb_device_video_probe_and_commit_controls_struct_t) >> 2U) + 1U];
USB_DMA_NONINIT_DATA_ALIGN(4) static uint8_t s_ImageBuffer[HS_STREAM_IN_PACKET_SIZE];
usb_video_virtual_camera_struct_t g_UsbDeviceVideoVirtualCamera;

usb_device_class_config_struct_t g_UsbDeviceVideoConfig[1] = {{
        USB_DeviceVideoCallback, (class_handle_t)NULL, &g_UsbDeviceVideoVirtualCameraConfig,
    }
};

usb_device_class_config_list_struct_t g_UsbDeviceVideoConfigList =
{
    g_UsbDeviceVideoConfig, USB_DeviceCallback, 1U,
};

/* Prepare next transfer payload */
static void USB_DeviceVideoPrepareVideoData(void)
{
    usb_device_video_mjpeg_payload_header_struct_t *payloadHeader;
    uint32_t maxPacketSize;
    uint32_t temp32dwFrameInterval;

    camvedio_read_done = 0;

    g_UsbDeviceVideoVirtualCamera.currentTime += 10000U;

    payloadHeader = (usb_device_video_mjpeg_payload_header_struct_t *)&g_UsbDeviceVideoVirtualCamera.imageBuffer[0];

    payloadHeader->bHeaderLength = sizeof(usb_device_video_mjpeg_payload_header_struct_t);
    payloadHeader->headerInfoUnion.bmheaderInfo = 0U;
    payloadHeader->headerInfoUnion.headerInfoBits.frameIdentifier = g_UsbDeviceVideoVirtualCamera.currentFrameId;
    g_UsbDeviceVideoVirtualCamera.imageBufferLength = sizeof(usb_device_video_mjpeg_payload_header_struct_t);

    if (g_UsbDeviceVideoVirtualCamera.stillImageTransmission)
    {
        payloadHeader->headerInfoUnion.headerInfoBits.stillImage = 1U;
        maxPacketSize =
                        USB_LONG_FROM_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.stillCommitStruct->dwMaxPayloadTransferSize);
    }
    else
    {
        maxPacketSize =
                        USB_LONG_FROM_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.commitStruct->dwMaxPayloadTransferSize);
    }

    if (g_UsbDeviceVideoVirtualCamera.waitForNewInterval)
    {
        temp32dwFrameInterval =
                        USB_LONG_FROM_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.commitStruct->dwFrameInterval);
        if (g_UsbDeviceVideoVirtualCamera.currentTime < temp32dwFrameInterval)
        {
            return;
        }
        else
        {
            g_UsbDeviceVideoVirtualCamera.currentTime = 0U;
            g_UsbDeviceVideoVirtualCamera.waitForNewInterval = 0U;
            payloadHeader->headerInfoUnion.headerInfoBits.endOfFrame = 1U;
            g_UsbDeviceVideoVirtualCamera.stillImageTransmission = 0U;
            g_UsbDeviceVideoVirtualCamera.currentFrameId ^= 1U;
            if (USB_DEVICE_VIDEO_STILL_IMAGE_TRIGGER_TRANSMIT_STILL_IMAGE ==
                    g_UsbDeviceVideoVirtualCamera.stillImageTriggerControl)
            {
                g_UsbDeviceVideoVirtualCamera.stillImageTriggerControl =
                                USB_DEVICE_VIDEO_STILL_IMAGE_TRIGGER_NORMAL_OPERATION;
                g_UsbDeviceVideoVirtualCamera.stillImageTransmission = 1U;
            }
            return;
        }
    }

    if (thread_write_done == 1)
    {
        for (; g_UsbDeviceVideoVirtualCamera.imageBufferLength < maxPacketSize;
                g_UsbDeviceVideoVirtualCamera.imageBufferLength++)
        {
            g_UsbDeviceVideoVirtualCamera.imageBuffer[g_UsbDeviceVideoVirtualCamera.imageBufferLength] =
                            g_UsbDeviceVideoMjpegData[g_UsbDeviceVideoVirtualCamera.imageIndex];
            g_UsbDeviceVideoVirtualCamera.imageIndex++;

            if ((0xFFU == g_UsbDeviceVideoMjpegData[g_UsbDeviceVideoVirtualCamera.imageIndex - 2]) &&
                    (0xD9U == g_UsbDeviceVideoMjpegData[g_UsbDeviceVideoVirtualCamera.imageIndex - 1U]))
            {
                temp32dwFrameInterval =
                                USB_LONG_FROM_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.commitStruct->dwFrameInterval);
                if (g_UsbDeviceVideoVirtualCamera.imageIndex >= g_UsbDeviceVideoMjpegLength)
                {
                    g_UsbDeviceVideoVirtualCamera.imageIndex = 0U;
                }
                if (g_UsbDeviceVideoVirtualCamera.currentTime < temp32dwFrameInterval)
                {
                    g_UsbDeviceVideoVirtualCamera.waitForNewInterval = 1U;
                }
                else
                {
                    g_UsbDeviceVideoVirtualCamera.currentTime = 0U;
                    payloadHeader->headerInfoUnion.headerInfoBits.endOfFrame = 1U;
                    g_UsbDeviceVideoVirtualCamera.stillImageTransmission = 0U;
                    g_UsbDeviceVideoVirtualCamera.currentFrameId ^= 1U;
                    if (USB_DEVICE_VIDEO_STILL_IMAGE_TRIGGER_TRANSMIT_STILL_IMAGE ==
                            g_UsbDeviceVideoVirtualCamera.stillImageTriggerControl)
                    {
                        g_UsbDeviceVideoVirtualCamera.stillImageTriggerControl =
                                        USB_DEVICE_VIDEO_STILL_IMAGE_TRIGGER_NORMAL_OPERATION;
                        g_UsbDeviceVideoVirtualCamera.stillImageTransmission = 1U;
                    }
                }
                g_UsbDeviceVideoVirtualCamera.imageBufferLength++;
                break;
            }
        }
    }

    camvedio_read_done = 1;
}

static usb_status_t USB_DeviceVideoRequest(class_handle_t handle, uint32_t event, void *param)
{
    usb_device_control_request_struct_t *request = (usb_device_control_request_struct_t *)param;
    usb_device_video_probe_and_commit_controls_struct_t *probe =
                    (usb_device_video_probe_and_commit_controls_struct_t *)(request->buffer);
    usb_device_video_probe_and_commit_controls_struct_t *commit =
                    (usb_device_video_probe_and_commit_controls_struct_t *)(request->buffer);
    usb_device_video_still_probe_and_commit_controls_struct_t *still_probe =
                    (usb_device_video_still_probe_and_commit_controls_struct_t *)(request->buffer);
    usb_device_video_still_probe_and_commit_controls_struct_t *still_commit =
                    (usb_device_video_still_probe_and_commit_controls_struct_t *)(request->buffer);
    uint32_t temp32;
    usb_status_t error = kStatus_USB_Success;

    switch (event)
    {
        /* probe request */
        case USB_DEVICE_VIDEO_SET_CUR_VS_PROBE_CONTROL:
            if ((request->buffer == NULL) || (request->length == 0U))
            {
                return kStatus_USB_InvalidRequest;
            }
            temp32 = USB_LONG_FROM_LITTLE_ENDIAN_DATA(probe->dwFrameInterval);
            if ((temp32 >= USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MIN_INTERVAL) &&
                    (temp32 <= USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MAX_INTERVAL))
            {
                USB_LONG_TO_LITTLE_ENDIAN_DATA(temp32, g_UsbDeviceVideoVirtualCamera.probeStruct->dwFrameInterval);
            }
            temp32 = USB_LONG_FROM_LITTLE_ENDIAN_DATA(probe->dwMaxPayloadTransferSize);
            if ((temp32) && (temp32 < g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize))
            {
                USB_LONG_TO_LITTLE_ENDIAN_DATA(temp32,
                                               g_UsbDeviceVideoVirtualCamera.probeStruct->dwMaxPayloadTransferSize);
            }
            g_UsbDeviceVideoVirtualCamera.probeStruct->bFormatIndex = probe->bFormatIndex;
            g_UsbDeviceVideoVirtualCamera.probeStruct->bFrameIndex = probe->bFrameIndex;
            break;
        case USB_DEVICE_VIDEO_GET_CUR_VS_PROBE_CONTROL:
            request->buffer = (uint8_t *)g_UsbDeviceVideoVirtualCamera.probeStruct;
            request->length = g_UsbDeviceVideoVirtualCamera.probeLength;
            break;
        case USB_DEVICE_VIDEO_GET_LEN_VS_PROBE_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.probeLength;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.probeLength);
            break;
        case USB_DEVICE_VIDEO_GET_INFO_VS_PROBE_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.probeInfo;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.probeInfo);
            break;
        /* commit request */
        case USB_DEVICE_VIDEO_SET_CUR_VS_COMMIT_CONTROL:
            if ((request->buffer == NULL) || (request->length == 0U))
            {
                return kStatus_USB_InvalidRequest;
            }
            temp32 = USB_LONG_FROM_LITTLE_ENDIAN_DATA(commit->dwFrameInterval);
            if ((temp32 >= USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MIN_INTERVAL) &&
                    (temp32 <= USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MAX_INTERVAL))
            {
                USB_LONG_TO_LITTLE_ENDIAN_DATA(temp32, g_UsbDeviceVideoVirtualCamera.commitStruct->dwFrameInterval);
            }

            temp32 = USB_LONG_FROM_LITTLE_ENDIAN_DATA(commit->dwMaxPayloadTransferSize);
            if ((temp32) && (temp32 < g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize))
            {
                USB_LONG_TO_LITTLE_ENDIAN_DATA(temp32,
                                               g_UsbDeviceVideoVirtualCamera.commitStruct->dwMaxPayloadTransferSize);
            }
            g_UsbDeviceVideoVirtualCamera.commitStruct->bFormatIndex = commit->bFormatIndex;
            g_UsbDeviceVideoVirtualCamera.commitStruct->bFrameIndex = commit->bFrameIndex;
            break;
        case USB_DEVICE_VIDEO_GET_CUR_VS_COMMIT_CONTROL:
            request->buffer = (uint8_t *)g_UsbDeviceVideoVirtualCamera.commitStruct;
            request->length = g_UsbDeviceVideoVirtualCamera.commitLength;
            break;
        case USB_DEVICE_VIDEO_GET_LEN_VS_COMMIT_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.commitLength;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.commitLength);
            break;
        case USB_DEVICE_VIDEO_GET_INFO_VS_COMMIT_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.commitInfo;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.commitInfo);
            break;
        /* still probe request */
        case USB_DEVICE_VIDEO_SET_CUR_VS_STILL_PROBE_CONTROL:
            if ((request->buffer == NULL) || (request->length == 0U))
            {
                return kStatus_USB_InvalidRequest;
            }
            temp32 = USB_LONG_FROM_LITTLE_ENDIAN_DATA(still_probe->dwMaxPayloadTransferSize);
            if ((temp32) && (temp32 < g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize))
            {
                USB_LONG_TO_LITTLE_ENDIAN_DATA(
                                temp32, g_UsbDeviceVideoVirtualCamera.stillProbeStruct->dwMaxPayloadTransferSize);
            }

            g_UsbDeviceVideoVirtualCamera.stillProbeStruct->bFormatIndex = still_probe->bFormatIndex;
            g_UsbDeviceVideoVirtualCamera.stillProbeStruct->bFrameIndex = still_probe->bFrameIndex;
            break;
        case USB_DEVICE_VIDEO_GET_CUR_VS_STILL_PROBE_CONTROL:
            request->buffer = (uint8_t *)g_UsbDeviceVideoVirtualCamera.stillProbeStruct;
            request->length = g_UsbDeviceVideoVirtualCamera.stillProbeLength;
            break;
        case USB_DEVICE_VIDEO_GET_LEN_VS_STILL_PROBE_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.stillProbeLength;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.stillProbeLength);
            break;
        case USB_DEVICE_VIDEO_GET_INFO_VS_STILL_PROBE_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.stillProbeInfo;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.stillProbeInfo);
            break;
        /* still commit request */
        case USB_DEVICE_VIDEO_SET_CUR_VS_STILL_COMMIT_CONTROL:
            if ((request->buffer == NULL) || (request->length == 0U))
            {
                return kStatus_USB_InvalidRequest;
            }
            temp32 = USB_LONG_FROM_LITTLE_ENDIAN_DATA(still_commit->dwMaxPayloadTransferSize);
            if ((temp32) && (temp32 < g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize))
            {
                USB_LONG_TO_LITTLE_ENDIAN_DATA(
                                temp32, g_UsbDeviceVideoVirtualCamera.stillCommitStruct->dwMaxPayloadTransferSize);
            }

            g_UsbDeviceVideoVirtualCamera.stillCommitStruct->bFormatIndex = still_commit->bFormatIndex;
            g_UsbDeviceVideoVirtualCamera.stillCommitStruct->bFrameIndex = still_commit->bFrameIndex;
            break;
        case USB_DEVICE_VIDEO_GET_CUR_VS_STILL_COMMIT_CONTROL:
            request->buffer = (uint8_t *)g_UsbDeviceVideoVirtualCamera.stillCommitStruct;
            request->length = g_UsbDeviceVideoVirtualCamera.stillCommitLength;
            break;
        case USB_DEVICE_VIDEO_GET_LEN_VS_STILL_COMMIT_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.stillCommitLength;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.stillCommitLength);
            break;
        case USB_DEVICE_VIDEO_GET_INFO_VS_STILL_COMMIT_CONTROL:
            request->buffer = &g_UsbDeviceVideoVirtualCamera.stillCommitInfo;
            request->length = sizeof(g_UsbDeviceVideoVirtualCamera.stillCommitInfo);
            break;
        /* still image trigger request */
        case USB_DEVICE_VIDEO_SET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL:
            g_UsbDeviceVideoVirtualCamera.stillImageTriggerControl = *(request->buffer);
            break;
        default:
            error = kStatus_USB_InvalidRequest;
            break;
    }
    return error;
}

/* USB device Video class callback */
static usb_status_t USB_DeviceVideoCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceVideoEventStreamSendResponse:
            /* Stream data dent */
            if (g_UsbDeviceVideoVirtualCamera.attach)
            {
                /* Prepare the next stream data */
                USB_DeviceVideoPrepareVideoData();
                error = USB_DeviceVideoSend(
                                        g_UsbDeviceVideoVirtualCamera.videoHandle, USB_VIDEO_VIRTUAL_CAMERA_STREAM_ENDPOINT_IN,
                                        g_UsbDeviceVideoVirtualCamera.imageBuffer, g_UsbDeviceVideoVirtualCamera.imageBufferLength);
            }
            break;
        case kUSB_DeviceVideoEventClassRequestBuffer:
            if (param && (g_UsbDeviceVideoVirtualCamera.attach))
            {
                /* Get the class-specific OUT buffer */
                usb_device_control_request_struct_t *request = (usb_device_control_request_struct_t *)param;

                if (request->length <= sizeof(usb_device_video_probe_and_commit_controls_struct_t))
                {
                    request->buffer = (uint8_t *)g_UsbDeviceVideoVirtualCamera.classRequestBuffer;
                    error = kStatus_USB_Success;
                }
            }
            break;
        default:
            if (param && (event > 0xFFU))
            {
                /* If the event is the class-specific request(Event > 0xFFU), handle the class-specific request */
                error = USB_DeviceVideoRequest(handle, event, param);
            }
            break;
    }

    return error;
}

/* Set to default state */
static void USB_DeviceVideoApplicationSetDefault(void)
{
    g_UsbDeviceVideoVirtualCamera.speed = USB_SPEED_FULL;
    g_UsbDeviceVideoVirtualCamera.attach = 0U;
    g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize = HS_STREAM_IN_PACKET_SIZE;
    g_UsbDeviceVideoVirtualCamera.imageBuffer = s_ImageBuffer;
    g_UsbDeviceVideoVirtualCamera.probeStruct = &s_ProbeStruct;
    g_UsbDeviceVideoVirtualCamera.commitStruct = &s_CommitStruct;
    g_UsbDeviceVideoVirtualCamera.stillProbeStruct = &s_StillProbeStruct;
    g_UsbDeviceVideoVirtualCamera.stillCommitStruct = &s_StillCommitStruct;
    g_UsbDeviceVideoVirtualCamera.classRequestBuffer = &s_ClassRequestBuffer[0];

    g_UsbDeviceVideoVirtualCamera.probeStruct->bFormatIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FORMAT_INDEX;
    g_UsbDeviceVideoVirtualCamera.probeStruct->bFrameIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_INDEX;
    USB_LONG_TO_LITTLE_ENDIAN_DATA(USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_DEFAULT_INTERVAL,
                                   g_UsbDeviceVideoVirtualCamera.probeStruct->dwFrameInterval);
    USB_LONG_TO_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize,
                                   g_UsbDeviceVideoVirtualCamera.probeStruct->dwMaxPayloadTransferSize);
    USB_LONG_TO_LITTLE_ENDIAN_DATA(USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MAX_FRAME_SIZE,
                                   g_UsbDeviceVideoVirtualCamera.probeStruct->dwMaxVideoFrameSize);

    g_UsbDeviceVideoVirtualCamera.commitStruct->bFormatIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FORMAT_INDEX;
    g_UsbDeviceVideoVirtualCamera.commitStruct->bFrameIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_INDEX;
    USB_LONG_TO_LITTLE_ENDIAN_DATA(USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_DEFAULT_INTERVAL,
                                   g_UsbDeviceVideoVirtualCamera.commitStruct->dwFrameInterval);
    USB_LONG_TO_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize,
                                   g_UsbDeviceVideoVirtualCamera.commitStruct->dwMaxPayloadTransferSize);
    USB_LONG_TO_LITTLE_ENDIAN_DATA(USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MAX_FRAME_SIZE,
                                   g_UsbDeviceVideoVirtualCamera.commitStruct->dwMaxVideoFrameSize);

    g_UsbDeviceVideoVirtualCamera.probeInfo = 0x03U;
    g_UsbDeviceVideoVirtualCamera.probeLength = 26U;
    g_UsbDeviceVideoVirtualCamera.commitInfo = 0x03U;
    g_UsbDeviceVideoVirtualCamera.commitLength = 26U;

    g_UsbDeviceVideoVirtualCamera.stillProbeStruct->bFormatIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FORMAT_INDEX;
    g_UsbDeviceVideoVirtualCamera.stillProbeStruct->bFrameIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_INDEX;
    g_UsbDeviceVideoVirtualCamera.stillProbeStruct->bCompressionIndex = 0x01U;
    USB_LONG_TO_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize,
                                   g_UsbDeviceVideoVirtualCamera.stillProbeStruct->dwMaxPayloadTransferSize);
    USB_LONG_TO_LITTLE_ENDIAN_DATA(USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MAX_FRAME_SIZE,
                                   g_UsbDeviceVideoVirtualCamera.stillProbeStruct->dwMaxVideoFrameSize);

    g_UsbDeviceVideoVirtualCamera.stillCommitStruct->bFormatIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FORMAT_INDEX;
    g_UsbDeviceVideoVirtualCamera.stillCommitStruct->bFrameIndex = USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_INDEX;
    g_UsbDeviceVideoVirtualCamera.stillCommitStruct->bCompressionIndex = 0x01U;
    USB_LONG_TO_LITTLE_ENDIAN_DATA(g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize,
                                   g_UsbDeviceVideoVirtualCamera.stillCommitStruct->dwMaxPayloadTransferSize);
    USB_LONG_TO_LITTLE_ENDIAN_DATA(USB_VIDEO_VIRTUAL_CAMERA_MJPEG_FRAME_MAX_FRAME_SIZE,
                                   g_UsbDeviceVideoVirtualCamera.stillCommitStruct->dwMaxVideoFrameSize);

    g_UsbDeviceVideoVirtualCamera.stillProbeInfo = 0x03U;
    g_UsbDeviceVideoVirtualCamera.stillProbeLength = sizeof(s_StillProbeStruct);
    g_UsbDeviceVideoVirtualCamera.stillCommitInfo = 0x03U;
    g_UsbDeviceVideoVirtualCamera.stillCommitLength = sizeof(s_StillCommitStruct);

    g_UsbDeviceVideoVirtualCamera.currentTime = 0U;
    g_UsbDeviceVideoVirtualCamera.currentFrameId = 0U;
    g_UsbDeviceVideoVirtualCamera.currentStreamInterfaceAlternateSetting = 0U;
    g_UsbDeviceVideoVirtualCamera.imageBufferLength = 0U;
    g_UsbDeviceVideoVirtualCamera.imageIndex = 0U;
    g_UsbDeviceVideoVirtualCamera.waitForNewInterval = 0U;
    g_UsbDeviceVideoVirtualCamera.stillImageTransmission = 0U;
    g_UsbDeviceVideoVirtualCamera.stillImageTriggerControl = USB_DEVICE_VIDEO_STILL_IMAGE_TRIGGER_NORMAL_OPERATION;
}

/* The device callback */
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Success;
    uint8_t *temp8 = (uint8_t *)param;
    uint16_t *temp16 = (uint16_t *)param;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
            /* The device BUS reset signal detected */
            USB_DeviceVideoApplicationSetDefault();
#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)) || \
(defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
            /* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
            if (kStatus_USB_Success == USB_DeviceGetStatus(g_UsbDeviceVideoVirtualCamera.deviceHandle,
                    kUSB_DeviceStatusSpeed,
                    &g_UsbDeviceVideoVirtualCamera.speed))
            {
                USB_DeviceSetSpeed(g_UsbDeviceVideoVirtualCamera.deviceHandle, g_UsbDeviceVideoVirtualCamera.speed);
            }

            if (USB_SPEED_HIGH == g_UsbDeviceVideoVirtualCamera.speed)
            {
                g_UsbDeviceVideoVirtualCamera.currentMaxPacketSize = HS_STREAM_IN_PACKET_SIZE;
            }
#endif
        }
        break;
        case kUSB_DeviceEventSetConfiguration:
            if (USB_VIDEO_VIRTUAL_CAMERA_CONFIGURE_INDEX == (*temp8))
            {
                /* Set the configuration request */
                g_UsbDeviceVideoVirtualCamera.attach = 1U;
                g_UsbDeviceVideoVirtualCamera.currentConfiguration = *temp8;
            }
            break;
        case kUSB_DeviceEventSetInterface:
            if ((g_UsbDeviceVideoVirtualCamera.attach) && param)
            {
                /* Set alternateSetting of the interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);

                if (g_UsbDeviceVideoVirtualCamera.currentInterfaceAlternateSetting[interface] != alternateSetting)
                {
                    if (!g_UsbDeviceVideoVirtualCamera.currentInterfaceAlternateSetting[interface])
                    {
                        if (USB_VIDEO_VIRTUAL_CAMERA_STREAM_INTERFACE_INDEX == interface)
                        {
                            USB_DeviceVideoPrepareVideoData();
                            error = USB_DeviceSendRequest(g_UsbDeviceVideoVirtualCamera.deviceHandle,
                                                          USB_VIDEO_VIRTUAL_CAMERA_STREAM_ENDPOINT_IN,
                                                          g_UsbDeviceVideoVirtualCamera.imageBuffer,
                                                          g_UsbDeviceVideoVirtualCamera.imageBufferLength);
                        }
                    }
                    g_UsbDeviceVideoVirtualCamera.currentInterfaceAlternateSetting[interface] = alternateSetting;
                }
            }
            break;
        case kUSB_DeviceEventGetConfiguration:
            if (param)
            {
                /* Get the current configuration request */
                *temp8 = g_UsbDeviceVideoVirtualCamera.currentConfiguration;
                error = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceEventGetInterface:
            if (param)
            {
                /* Set the alternateSetting of the interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                if (interface < USB_VIDEO_VIRTUAL_CAMERA_INTERFACE_COUNT)
                {
                    *temp16 =
                                    (*temp16 & 0xFF00U) | g_UsbDeviceVideoVirtualCamera.currentInterfaceAlternateSetting[interface];
                    error = kStatus_USB_Success;
                }
                else
                {
                    error = kStatus_USB_InvalidRequest;
                }
            }
            break;
        case kUSB_DeviceEventGetDeviceDescriptor:
            if (param)
            {
                /* Get the device descriptor request */
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param)
            {
                /* Get the configuration descriptor request */
                error = USB_DeviceGetConfigurationDescriptor(handle,
                        (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetStringDescriptor:
            if (param)
            {
                /* Get the string descriptor request */
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
            break;
        default:
            break;
    }

    return error;
}



static void USB_DeviceApplicationInit(void)
{
    USB_DeviceVideoApplicationSetDefault();

    if (kStatus_USB_Success !=
            USB_DeviceClassInit(CONTROLLER_ID, &g_UsbDeviceVideoConfigList, &g_UsbDeviceVideoVirtualCamera.deviceHandle))
    {
        usb_echo("USB device video virtual camera failed\r\n");
        return;
    }
    else
    {
        usb_echo("USB device video virtual camera demo\r\n");
        g_UsbDeviceVideoVirtualCamera.videoHandle = g_UsbDeviceVideoConfigList.config->classHandle;
    }

    USB_Device_SetStackParam(&g_UsbDeviceVideoVirtualCamera.deviceHandle);
    USB_DeviceRun(g_UsbDeviceVideoVirtualCamera.deviceHandle);
}



/*=======================*/
#endif

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOB"

#define LED_GPIO_PIN   10   /*PC10*/
#define LED1_GPIO_PIN  11   /*PC11*/
#define SW0_GPIO_PIN   3    /*PB03*/

static int cnt = 0;

struct device *dev;
static struct gpio_callback gpio_cb;


void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    cnt++;

    if (cnt % 2)
    {
        obj_det_data.det_type_cpy = DET_GEST;
        printf("Swith to Gesture Detection \n");
    }
    else
    {
        obj_det_data.det_type_cpy = DET_BODY;
        printf("Swith to Body Detection \n");
    }
}

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h);

/*Feed rrggbb format to KDP310 for further training */
void rgb_split2rrggbb(uint8_t *src, uint8_t *dst, int w, int h)
{
    uint8_t *dst_b = &(dst[0]);
    uint8_t *dst_g = &(dst[w * h]);
    uint8_t *dst_r = &(dst[w * h * 2]);

    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = 0;
    for (l = 0; l < h; l++)
    {
        for (c = 0; c < w; c++)
        {
            src_rgb = src + (l * w + c) * 3;
            *(dst_b + i) = *(src_rgb + 2);    //b
            *(dst_g + i) = *(src_rgb + 1);    //g
            *(dst_r + i) = *(src_rgb);        //r
            i++;
        }
    }
}


/*Called every frame data captured by Camera*/
void camera_isr(struct device *dev, uint32_t line_num)
{
    struct dcmi_buf dis_buf;
    int ret = 0;

    if (!obj_det_data.stop_disp)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
        /*Camera can display Real-Time*/
        /*Set the address for LCD display, Layer 0*/
        if (!obj_det_data.show_demo_pic)
        {
            if (ret == 0)
            {
                if (!obj_det_data.one_frame_detect_started)
                {
                    /*Captured Picture data*/
                    memcpy(obj_det_data.ex_buf.cam_buf_detected, (void *)dis_buf.addr, CAM_PIC_SIZE);
                    obj_det_data.one_frame_detect_started = 1;
                }
            }

            fb_ioctl(ltdc, 0, FB_X_SETADDR, &(dis_buf.addr));
        }
        /*Release the buffer for next camera capturing*/
        dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
    }
}

#ifndef UGELIS_NEW_VERSION
    #define STACKSIZE                4096
    char __noinit __stack stack_area[STACKSIZE];
#else
    #define STACKSIZE                4096

    K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);

    static struct k_thread task_thread;
#endif


static void cambuf_process_thread()
{
    uint32_t demoaddr = 0;

    while (1)
    {
        if (obj_det_data.show_demo_pic)
        {
            demoaddr = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000;  //0x10400000
            if (obj_det_data.show_demopic_onetime)
            {
                rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)demoaddr, 224, 224);
                fb_ioctl(ltdc, 0, FB_X_SETADDR, &demoaddr);
                obj_det_data.show_demopic_onetime = 0;
                k_sem_give(&obj_det_data.validate_lock);
            }
        }
        else
        {
            if (obj_det_data.one_frame_detect_started)
            {
                /*cam_buf_validate now has the KDP 310 format*/
                rgb_split2rrggbb(obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate, 224, 224);
                k_sem_give(&obj_det_data.validate_lock);
                obj_det_data.one_frame_detect_started = 0;
            }
        }
    }
}




void init_ai_data()
{
    obj_det_data.ex_buf.cam_buf_detected = malloc_ext(0, 224 * 224 * 3); //malloc
    obj_det_data.ex_buf.cam_buf_validate = malloc_ext(0, TEST_BUFFER_SIZE);//malloc

    printf("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[TEST_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[TEST_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/


    obj_det_data.ex_buf.cam_buf_tmp = malloc_ext(0, 224 * 224 * 4);//malloc     /*A, R, G ,B, it's layer 1, blending on layer 2*/

    printf("cam_buf_tmp : 0x%08x \n", obj_det_data.ex_buf.cam_buf_tmp);

    memset((void *)obj_det_data.ex_buf.cam_buf_tmp, 0x00, 224 * 224 * 4);
    obj_det_data.input_data_addr = obj_det_data.ex_buf.cam_buf_validate;

    obj_det_data.boxcnt = 0;
    obj_det_data.object_name = object_name;
    obj_det_data.stop_disp = 0;                 /*Stop display on LCD*/
    obj_det_data.threshold = 0.6;
    obj_det_data.one_frame_detect_started = 0;  /*Start detect one frame*/

    obj_det_data.show_demo_pic = 0;
    obj_det_data.show_demopic_onetime = 0;
    obj_det_data.cap_show = 1; //0

    obj_det_data.object_name = object_name;

    obj_det_data.unreg_imgcnt = 0;
    obj_det_data.reg_imgcnt = 0;
    obj_det_data.det_type = DET_BODY;      // DET_GEST
    obj_det_data.det_type_cpy = DET_BODY;  // DET_GEST

    obj_det_data.pic_saved = 0;            /*save picsture option*/

    obj_det_data.unregdumpdata = (uint32_t)UNREG_DUMP_BUFF + IMG_COUNT_BYTES;
    obj_det_data.regdumpdata = (uint32_t)REG_DUMP_BUFF + IMG_COUNT_BYTES;

    k_sem_init(&obj_det_data.validate_lock, 0, UINT_MAX);
    k_sem_init(&obj_det_data.validate_lock1, 0, UINT_MAX);
}


void exit()
{

}

extern uint32_t show_buf_addr;
//extern uint32_t pingpongbuf1;
//extern uint32_t pingpongbuf0;

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h)
{
    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = src;
    uint8_t *dst_rgb = dst;

    for (i = 0; i < w * h; i ++)
    {
        *dst_rgb = *(src_rgb +  w * h * 2 + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + w * h + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + i);
        dst_rgb ++;
    }
}

int init()
{
    init_ai_data();
    init_shell(&obj_det_data);
    lcd_config();
    printk("Init camera \n");
    camera_config(camera_isr);
    printk("Init camera Done \n");
    return 0;
}

#define BLACK         0x000000
#define BLUE          0x0000FF

//extern unsigned short OutputImage[BUFFERSIZE] ;
detection_type gdet_type = DET_NOTDEF;

#define TEST_BLOCK_NUM              (30)
#define TEST_BLOCK_START            (0)
#define TEST_DATA                   (0xa5)
#define TEST_DATA_SIZE              (512*TEST_BLOCK_NUM)

static uint8_t gbuf[TEST_DATA_SIZE] = {0};
MMC_Handle_T ghMMC;

static uint32_t rec_buff_length;

//unsigned char JPG_enc_buf[7168];  /*buffer to save the encoded jpg pic*/

unsigned char JPG_enc_buf[7168 * 2 * 2]; /*buffer to save the encoded jpg pic*/

jpeg_compress_info info1;

unsigned int pt_buf = 0;

//JQUANT_TBL  JQUANT_TBL_2[2];
JQUANT_TBL  JQUANT_TBL_2[4];

//JHUFF_TBL  JHUFF_TBL_4[4];
JHUFF_TBL  JHUFF_TBL_4[8];

//unsigned char dcttab[3][512];

//volatile unsigned char inbuf_buf[50176];


unsigned char dcttab[3 * 2 * 2][512 * 2 * 2];

volatile unsigned char inbuf_buf[50176 * 2 * 2];

#define DEMO_USE_SD_MMC  1


int main(void)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t kk = 0;
    uint32_t t_last, t_now;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    char str[10];

    RET_CODE ret = RET_OK;
    uint32_t blk_cnt = TEST_BLOCK_NUM;
    uint32_t blk_start = TEST_BLOCK_START;
    static uint32_t cnt = 0;
    uint8_t *buf = gbuf;
    MMC_Config_T config;
    uint64_t totalSector = 0;
    MMC_Handle_T *hMMC = &ghMMC;
    struct device *gpiob;

    /*test jpg encoder*/
    int width, height;
    JSAMPLE *image;
    jpeg_compress_info *cinfo;

    /*Reset All IP*/
    //*(volatile uint32_t *)(0x40000070) = 0xFFFFFFFF;
    rec_buff_length = IMG_DUMP_PIXELS;

    *(volatile uint32_t *)REG_DUMP_BUFF = 0;
    *(volatile uint32_t *)UNREG_DUMP_BUFF = 0;

    memset(REG_DUMP_BUFF, 0x00, 4);
    memset(UNREG_DUMP_BUFF, 0x00, 4);

    memset(inputImage, 0x00, 401408);
    memset(OutputImage, 0x00, 401408);

    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);

    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;
    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

#if 0
    /* Configure USB PLL clock */
    HW32_REG(0x40000190) |= (1 << 0); //PLL_I2S enable
    HW32_REG(0x40000080) |= (1 << 24); //USB clock enable
    /* Release USB reset */
    HW32_REG(0x40000070) |= (1 << 24); //Release usb IP sw reset

    *(volatile uint8_t *)0x50000210 = 0x1F;
#endif

    /*usb vertual camera init*/
    USB_DeviceApplicationInit();

    extern unsigned char jpg_data[];

    /*test jpg encoder*/
#if 0
    cinfo = jpeg_create_compress();
    if (!cinfo)
    {
        printf("error in create cinfo, malloc faild!\n");
    }

    width = 224;
    height = 224;

    cinfo->image_width = width;
    cinfo->image_height = height;
    cinfo->output = JPG_enc_buf;
    jpeg_set_default(cinfo);
#endif

    camvedio_read_done = 1;
    thread_write_done = 0;

#if 0
    image = jpg_data;
    width = 224;
    height = 224;

    cinfo = jpeg_create_compress();
    if (!cinfo)
    {
        printf("error in create cinfo, malloc faild!\n");
    }

    cinfo->image_width = width;
    cinfo->image_height = height;
    cinfo->output = JPG_enc_buf;
    jpeg_set_default(cinfo);

    jpeg_start_compress(cinfo);

    while (cinfo->next_line < cinfo->image_height)
    {
        jpeg_write_scanline(cinfo, &image[(cinfo->next_line * cinfo->image_width * 3)]);
    }

    jpeg_finish_compress(cinfo);

    jpeg_destory_compress(cinfo);
#endif


#if 0
    dev = device_get_binding(PORT);
    gpiob = dev;

    gpio_pin_configure(gpiob, SW0_GPIO_PIN,
                       GPIO_INT |  PULL_UP | GPIO_INT_ACTIVE_FALLING);

    gpio_init_callback(&gpio_cb, button_pressed, BIT(SW0_GPIO_PIN));

    gpio_add_callback(gpiob, &gpio_cb);
    gpio_pin_enable_callback(gpiob, SW0_GPIO_PIN);
#endif


#if 0
    for (i = 0; i < 0x100000; i += 4)
    {
        *(volatile uint32_t *)(0x10400000 + i) = 0x5A5AA5A5;
    }
#endif

    /*print the buffer address*/

#if 0
    printf("inputImage: 0x%08x, INPUT_IMAGE_START_ADDR: 0x%08x \n", inputImage, INPUT_IMAGE_START_ADDR);
    printf("OutputImage: 0x%08x, OUTPUT_IMAGE_START_ADDR: 0x%08x \n", OutputImage, OUTPUT_IMAGE_START_ADDR);

    printf("CMD_BASE: 0x%08x, CMD_BASE_START_ADDR: 0x%08x \n", CMD_BASE, CMD_BASE_START_ADDR);
    printf("CMD_BASE_BODY: 0x%08x, CMD_BASE_BODY_START_ADDR :0x%08x \n", CMD_BASE_BODY, CMD_BASE_BODY_START_ADDR);
    printf("CMD_BASE_GEST: 0x%08x, CMD_BASE_GEST_START_ADDR :0x%08x  \n", CMD_BASE_GEST, CMD_BASE_GEST_START_ADDR);

    printf("WEI_BASE: 0x%08x,  WEI_BASE_START_ADDR : 0x%08x \n", WEI_BASE, WEI_BASE_START_ADDR);
    printf("WEI_BASE_BODY: 0x%08x, WEI_BASE_BODY_START_ADDR : 0x%08x \n", WEI_BASE_BODY, WEI_BASE_BODY_START_ADDR);
    printf("WEI_BASE_GEST: 0x%08x, WEI_BASE_GEST_START_ADDR : 0x%08x \n", WEI_BASE_GEST, WEI_BASE_GEST_START_ADDR);

    printf("RecvBuffer: 0x%08x, RECV_BUF_START_ADDR : 0x%08x \n", RecvBuffer, RECV_BUF_START_ADDR);
    printf("RecvBufferBody: 0x%08x, RECV_BUF_BODY_START_ADDR : 0x%08x \n", RecvBufferBody, RECV_BUF_BODY_START_ADDR);
    printf("RecvBufferGest: 0x%08x, RECV_BUF_GEST_START_ADDR : 0x%08x \n", RecvBufferGest, RECV_BUF_GEST_START_ADDR);

    printf("UNREG_DUMP_BUFF: 0x%08x, UNREG_DUMP_BUFF_START_ADDR : 0x%08x \n", UNREG_DUMP_BUFF, UNREG_DUMP_BUFF_START_ADDR);
    printf("REG_DUMP_BUFF: 0x%08x, REG_DUMP_BUFF_START_ADDR 0x%08x \n", REG_DUMP_BUFF, REG_DUMP_BUFF_START_ADDR);

    printf("CMD_BASE: 0x%08x, WEI_BASE: 0x%08x, RecvBuffer: 0x%08x \n",  CMD_BASE, WEI_BASE, RecvBuffer);
#endif

    OBJ_PRINTF("inputImage: 0x%08x \n", inputImage);
    OBJ_PRINTF("OutputImage: 0x%08x \n", OutputImage);

    OBJ_PRINTF("CMD_BASE: 0x%08x \n", CMD_BASE);
    OBJ_PRINTF("CMD_BASE_BODY: 0x%08x  \n", CMD_BASE_BODY);
    OBJ_PRINTF("CMD_BASE_GEST: 0x%08x \n", CMD_BASE_GEST);

    OBJ_PRINTF("WEI_BASE: 0x%08x \n", WEI_BASE);

#ifdef BODY_DETECTION_DEMO
    OBJ_PRINTF("WEI_BASE_BODY: 0x%08x \n", WEI_BASE_BODY);
#endif

#ifdef GESTURE_DETECTION_DEMO
    OBJ_PRINTF("WEI_BASE_GEST: 0x%08x \n", WEI_BASE_GEST);
#endif

    OBJ_PRINTF("RecvBuffer: 0x%08x \n", RecvBuffer);

#ifdef BODY_DETECTION_DEMO
    OBJ_PRINTF("RecvBufferBody: 0x%08x \n", RecvBufferBody);
#endif

#ifdef GESTURE_DETECTION_DEMO
    OBJ_PRINTF("RecvBufferGest: 0x%08x \n", RecvBufferGest);
#endif

    OBJ_PRINTF("UNREG_DUMP_BUFF: 0x%08x \n", UNREG_DUMP_BUFF);
    OBJ_PRINTF("REG_DUMP_BUFF: 0x%08x \n", REG_DUMP_BUFF);

    OBJ_PRINTF("CMD_BASE: 0x%08x, WEI_BASE: 0x%08x, RecvBuffer: 0x%08x \n",  CMD_BASE, WEI_BASE, RecvBuffer);

#ifdef DEMO_USE_SD_MMC
    /*init mmc*/
    config.busWitch     = MMC_BUS_WIDTH_4;
    config.type         = MMC_SD;
    config.removable    = TRUE;
    //config.callback     = NULL;

    ret = HAL_MMC_Init(hMMC, MMC0, &config);
    if (ret != RET_OK)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Init\n", __FUNCTION__);
        return ret;
    }

#endif
    /*Init for Object AI*/
    init();

    /*debug usage, time-delay for SD card reading*/
    k_sleep(200);

    OBJ_PRINTF("Start to read SD Card ... \n");

#ifdef DEMO_USE_SD_MMC

#ifdef BODY_DETECTION_DEMO
    /*Read Body_Weight.Bin file, 8905.71875 blocks*/
    blk_start = 0;
    blk_cnt = HAL_MMC_Read(hMMC, blk_start, 8906, (unsigned char *)WEI_BASE_BODY); //blk_cnt = HAL_MMC_Read(hMMC, blk_start,8906,(unsigned char *)WEI_BASE_BODY,0);
    if (blk_cnt != 8906)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return RET_ERROR;
    }
#endif


#ifdef GESTURE_DETECTION_DEMO
    /*Read Gesture_Weight.Bin file, 8935.796875 blocks*/
    blk_start = 10240;  //8906 0x500000
    blk_cnt = HAL_MMC_Read(hMMC, blk_start, 8936, (unsigned char *)WEI_BASE_GEST);
    if (blk_cnt != 8936)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return RET_ERROR;
    }
#endif

    /*Read Body_Cmd.Bin file, 0 block 888bytes*/
    blk_start = 20480; //17845 0xa00000
    blk_cnt = HAL_MMC_Read(hMMC, blk_start, 2, (unsigned char *)CMD_BASE_BODY);
    if (blk_cnt != 2)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return RET_ERROR;
    }

    /*Read Gesture_Cmd.Bin file, 0 block 888bytes*/
    blk_start = 20608; //17848 0xa10000
    blk_cnt = HAL_MMC_Read(hMMC, blk_start, 2, (unsigned char *)CMD_BASE_GEST);
    if (blk_cnt != 2)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return RET_ERROR;
    }

#ifdef BODY_DETECTION_DEMO
    /*Read Body_Demo_Pic.Bin file, 294.00390625 blocks */
    blk_start = 20736;  //17852 0xa20000
    blk_cnt = HAL_MMC_Read(hMMC, blk_start, 295, (unsigned char *)RecvBufferBody);
    if (blk_cnt != 295)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return RET_ERROR;
    }
#endif

#ifdef GESTURE_DETECTION_DEMO
    /*Read Gest_Demo_Pic.Bin file, 294.00390625 blocks */
    blk_start = 22528; //18150 0xb00000
    blk_cnt = HAL_MMC_Read(hMMC, blk_start, 295, (unsigned char *)RecvBufferGest);
    if (blk_cnt != 295)
    {
        OBJ_PRINTF("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
        return RET_ERROR;
    }
#endif

#endif

    OBJ_PRINTF("Reading SD Card Done... \n");

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(0x20020000 + i) = 0x00000000;
    }

#ifndef UGELIS_NEW_VERSION
    k_thread_spawn(stack_area, STACKSIZE, cambuf_process_thread, NULL, NULL, NULL,
                   K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
#else
    k_thread_create(&task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)cambuf_process_thread,  NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
#endif

#if 0
    while (1)
    {
        //      obj_det_data.show_demo_pic = 1;
        //      obj_det_data.show_demopic_onetime = 1;
        k_sleep(1000);
    }
#endif

    while (1)
    {
        OBJ_DBG_PRINTF("==== \n");
        k_sem_take(&obj_det_data.validate_lock, 3000); //K_FOREVER
        OBJ_DBG_PRINTF("**** \n");

#ifdef CNN_TIME_TEST
        extern uint8_t gGetdata;
        extern uint8_t gInputdata;
        extern uint8_t gPutdata;
        extern uint8_t gMaxPool;
        extern uint32_t gGetdataTime_ms;
        extern uint32_t gPutdata_ms;
        extern uint32_t gMaxPool_ms;

        gGetdata = 0;
        gInputdata = 0;
        gPutdata = 0;
        gMaxPool = 0;
        gGetdataTime_ms = 0;
        gPutdata_ms = 0;
        gMaxPool_ms = 0;
#endif

        /*Copy weights to target buffer*/
        if (gdet_type != obj_det_data.det_type_cpy)
        {
            OBJ_PRINTF("copy weight buffer %s start ... \n", obj_det_data.det_type_cpy == 0 ? "body" : "gesture");
            if (obj_det_data.det_type_cpy == DET_BODY)
            {
#ifdef  BODY_DETECTION_DEMO
                for (i = 0; i < 5 * 1024 * 1024; i += 4)
                {
                    *(volatile uint32_t *)(WEI_BASE + i) = *(volatile uint32_t *)(WEI_BASE_BODY + i);
                }

                for (i = 0; i < TEST_BUFFER_SIZE; i += 2)
                {
                    *(volatile uint16_t *)(RecvBuffer + i) = *(volatile uint16_t *)(RecvBufferBody + i);
                }

                /*command length is typically less than 4096*/
                for (i = 0; i < 4096; i += 4)
                {
                    *(volatile uint32_t *)(CMD_BASE + i) = *(volatile uint32_t *)(CMD_BASE_BODY + i);
                }
#endif
                obj_det_data.det_type = DET_BODY;
            }

            if (obj_det_data.det_type_cpy == DET_GEST)
            {
#ifdef GESTURE_DETECTION_DEMO
                for (i = 0; i < 5 * 1024 * 1024; i += 4)
                {
                    *(volatile uint32_t *)(WEI_BASE + i) = *(volatile uint32_t *)(WEI_BASE_GEST + i);
                }

                for (i = 0; i < TEST_BUFFER_SIZE; i += 2)
                {
                    *(volatile uint16_t *)(RecvBuffer + i) = *(volatile uint16_t *)(RecvBufferGest + i);
                }

                /*command length is typically less than 4096*/
                for (i = 0; i < 4096; i += 4)
                {
                    *(volatile uint32_t *)(CMD_BASE + i) = *(volatile uint32_t *)(CMD_BASE_GEST + i);
                }
#endif
                obj_det_data.det_type = DET_GEST;
            }

            gdet_type = obj_det_data.det_type_cpy;
            OBJ_PRINTF("copy buffer done !\n");
        }

        /*in case of stack overflow, it's better to spawn a thread for tiny YOLO*/
        t_last = k_uptime_get_32();

        /*debug use*/
        //      obj_det_data.show_demo_pic = 1;

        if (obj_det_data.show_demo_pic)
        {
            /*for CNN computation*/
            call_cnn(RecvBuffer);
        }
        else
        {
            call_cnn(obj_det_data.ex_buf.cam_buf_validate);
        }

#ifdef CNN_TIME_TEST
        t_last = k_uptime_get_32();
#endif
        PostYolo();
        t_now = k_uptime_get_32();
        OBJ_DBG_PRINTF("t_now- t_last = %d ms \n", t_now - t_last);
        OBJ_DBG_PRINTF("%d Box found\n", obj_det_data.boxcnt);

#ifdef CNN_TIME_TEST
        OBJ_PRINTF("Getdata: %d, PutData:%d, MaxPool:%d ms \n", gGetdataTime_ms, gPutdata_ms, gMaxPool_ms);
#endif

        /*ping-pong buffer*/
        //show_buf_addr = (show_buf_addr == pingpongbuf0)? pingpongbuf1: pingpongbuf0;

        /*Layer 1*/
        LCD_clear();

        for (i = 0; i < obj_det_data.boxcnt; i ++)
        {
            OBJ_DBG_PRINTF("class: %d\n\r", obj_det_data.box[i].class);

            if (obj_det_data.det_type == DET_BODY)
            {
                OBJ_DBG_PRINTF("class name: person\n\r");
            }

#if defined(TINY_YOLO_FOR_OBJT)
            OBJ_DBG_PRINTF("class name: %s\n\r", object_name[obj_det_data.box[i].class]);
#endif

            if (obj_det_data.det_type == DET_GEST)
            {
                OBJ_DBG_PRINTF("class name: %s\n\r", gesture_name[obj_det_data.box[i].class]);
            }

            OBJ_DBG_PRINTF("score: %lf\n\r", obj_det_data.box[i].score);
            box_y_min = obj_det_data.box[i].box[0] < 0 ? 0 : obj_det_data.box[i].box[0];
            box_x_min = obj_det_data.box[i].box[1] < 0 ? 0 : obj_det_data.box[i].box[1];
            box_y_max = obj_det_data.box[i].box[2] < 0 ? 0 : obj_det_data.box[i].box[2];
            box_x_max = obj_det_data.box[i].box[3] < 0 ? 0 : obj_det_data.box[i].box[3];

            OBJ_DBG_PRINTF("box_y_min: %lf\n\r", box_y_min);
            OBJ_DBG_PRINTF("box_x_min: %lf\n\r", box_x_min);
            OBJ_DBG_PRINTF("box_y_max: %lf\n\r", box_y_max);
            OBJ_DBG_PRINTF("box_x_max: %lf\n\r", box_x_max);

            if (box_x_max > 223)
            {
                box_x_max = 223;
            }

            if (box_y_max > 223)
            {
                box_y_max = 223;
            }

            if (!obj_det_data.cap_show)
            {
                /*This will draw rectangle on Layer 1*/
                LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, object_colordrawn[obj_det_data.box[i].class]);
            }
            else
            {
                if (i == 0)
                {
                    if (obj_det_data.show_demo_pic)
                    {
                        rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, 224, 224);
                    }
                    else
                    {
                        rrggbb_split2rgb((uint8_t *)obj_det_data.ex_buf.cam_buf_validate, (uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, 224, 224);
                    }
                }

                /*This will show the detected-object on layer 1*/
                LCD_draw_detobjects((uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, box_x_min, box_y_min, box_x_max - box_x_min, box_y_max - box_y_min);
                /*Then draw a color*/
                LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, object_colordrawn[obj_det_data.box[i].class]);
            }

            /*Set the address for LCD display, Layer 1*/
            //fb_ioctl(ltdc, 1, FB_X_SETADDR, &show_buf_addr);
        }

        if (obj_det_data.pic_saved == 0)
        {
            if (obj_det_data.boxcnt == 0) /*record unrecognized pics*/
            {
                sprintf(str, "MS: %d", j + 1);
                LCD_showstring(0, 0, 1000, 24, 24, str, object_colordrawn[0]);

                OBJ_DBG_PRINTF("Ignored the Image-Detection: %d \n", j + 1);
                OBJ_DBG_PRINTF("Recognized pics: %d \n", *(volatile uint32_t *)REG_DUMP_BUFF);
                /*Copy data to dump buffer*/

                memcpy(obj_det_data.unregdumpdata[j % IMG_DUMP_COUNT].vadrgb, (uint8_t *)obj_det_data.ex_buf.cam_buf_validate, IMG_VAD_PIXELS_COUNT_BYTES); //reserved 2 bytes
                rrggbb_split2rgb((uint8_t *)obj_det_data.ex_buf.cam_buf_validate, (uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, 224, 224);
                memcpy(obj_det_data.unregdumpdata[j % IMG_DUMP_COUNT].camrgb, (uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, IMG_CAM_PIXELS_COUNT_BYTES);

                obj_det_data.unreg_imgcnt ++;
                if (obj_det_data.unreg_imgcnt > IMG_DUMP_COUNT)
                {
                    obj_det_data.unreg_imgcnt = IMG_DUMP_COUNT;
                }
                memcpy(UNREG_DUMP_BUFF, (void *)&obj_det_data.unreg_imgcnt, sizeof(uint32_t));
                j ++;
            }
            else /*record recognized pics*/
            {
                /*Copy data to dump buffer*/
                memcpy(obj_det_data.regdumpdata[k % IMG_DUMP_COUNT].vadrgb, (uint8_t *)obj_det_data.ex_buf.cam_buf_validate, IMG_VAD_PIXELS_COUNT_BYTES); //reserved 2 bytes
                rrggbb_split2rgb((uint8_t *)obj_det_data.ex_buf.cam_buf_validate, (uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, 224, 224);

                for (i = 0; i < obj_det_data.boxcnt; i ++)
                {
                    box_y_min = obj_det_data.box[i].box[0] < 0 ? 0 : obj_det_data.box[i].box[0];
                    box_x_min = obj_det_data.box[i].box[1] < 0 ? 0 : obj_det_data.box[i].box[1];
                    box_y_max = obj_det_data.box[i].box[2] < 0 ? 0 : obj_det_data.box[i].box[2];
                    box_x_max = obj_det_data.box[i].box[3] < 0 ? 0 : obj_det_data.box[i].box[3];

                    if (box_x_max > 223)
                    {
                        box_x_max = 223;
                    }

                    if (box_y_max > 223)
                    {
                        box_y_max = 223;
                    }

                    LCD_draw_rectangle2orgpic((uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, box_x_min, box_y_min, box_x_max, box_y_max, object_colordrawn[obj_det_data.box[i].class]);
                }
                memcpy(obj_det_data.regdumpdata[k % IMG_DUMP_COUNT].camrgb, (uint8_t *)obj_det_data.ex_buf.cam_buf_tmp, IMG_CAM_PIXELS_COUNT_BYTES);

                obj_det_data.reg_imgcnt ++;
                if (obj_det_data.reg_imgcnt > IMG_DUMP_COUNT)
                {
                    obj_det_data.reg_imgcnt = IMG_DUMP_COUNT;
                }
                memcpy(REG_DUMP_BUFF, (void *)&obj_det_data.reg_imgcnt, sizeof(uint32_t));

                OBJ_DBG_PRINTF("Recognized pics: %d \n", *(volatile uint32_t *)REG_DUMP_BUFF);
                k ++;
            }
        }
        else
        {
            /*stop here, allow tester to dump data*/
            __asm("bkpt 0");
        }

        /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
        fb_ioctl(ltdc, 1, FB_X_SETADDR, &show_buf_addr);

        /*==============jpg encoder=============*/
        thread_write_done = 0;

        if (camvedio_read_done == 1)
        {
            if (obj_det_data.boxcnt == 0)
            {
                image = (uint8_t *)obj_det_data.unregdumpdata[j % IMG_DUMP_COUNT].camrgb;//(uint8_t *)obj_det_data.ex_buf.cam_buf_tmp;
            }
            else
            {
                image = (uint8_t *)obj_det_data.regdumpdata[k % IMG_DUMP_COUNT].camrgb;
            }

            pt_buf = 0;

            cinfo = jpeg_create_compress();
            if (!cinfo)
            {
                printf("error in create cinfo, malloc faild!\n");
            }

            width = 224;
            height = 224;

            cinfo->image_width = width;
            cinfo->image_height = height;
            cinfo->output = JPG_enc_buf;
            jpeg_set_default(cinfo);

            jpeg_start_compress(cinfo);

            while (cinfo->next_line < cinfo->image_height)
            {
                jpeg_write_scanline(cinfo, &image[(cinfo->next_line * cinfo->image_width * 3)]);
            }

            jpeg_finish_compress(cinfo);


            for (i = 0; i < 7168; i++)
            {
                g_UsbDeviceVideoMjpegData[i] = JPG_enc_buf[i];
            }

            thread_write_done = 1;
        }


        /*=====================================*/

        OBJ_DBG_PRINTF("Done!\n");
    }
    exit();
}

