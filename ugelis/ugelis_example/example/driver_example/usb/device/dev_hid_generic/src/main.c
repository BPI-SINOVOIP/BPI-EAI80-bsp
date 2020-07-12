#include "main.h"
#include <ugelis.h>

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_hid.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "usb_device_khci.h"

#include <logging/sys_log.h>

#define USB_Echo printf


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static usb_status_t USB_DeviceHidGenericCallback(class_handle_t handle, uint32_t event, void *param);
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);
/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DMA_NONINIT_DATA_ALIGN(4) static uint32_t s_GenericBuffer0[USB_HID_GENERIC_IN_BUFFER_LENGTH >> 2];
USB_DMA_NONINIT_DATA_ALIGN(4) static uint32_t s_GenericBuffer1[USB_HID_GENERIC_IN_BUFFER_LENGTH >> 2];
usb_hid_generic_struct_t g_UsbDeviceHidGeneric;

extern usb_device_class_struct_t g_UsbDeviceHidGenericConfig;

/* Set class configurations */
usb_device_class_config_struct_t g_UsbDeviceHidConfig[1] = {{
        USB_DeviceHidGenericCallback, /* HID generic class callback pointer */
        (class_handle_t)NULL,         /* The HID class handle, This field is set by USB_DeviceClassInit */
        &g_UsbDeviceHidGenericConfig, /* The HID mouse configuration, including class code, subcode, and protocol, class
                             type,
                             transfer type, endpoint address, max packet size, etc.*/
    }
};

/* Set class configuration list */
usb_device_class_config_list_struct_t g_UsbDeviceHidConfigList =
{
    g_UsbDeviceHidConfig, /* Class configurations */
    USB_DeviceCallback,   /* Device callback pointer */
    1U,                   /* Class count */
};

/*******************************************************************************
 * Code
 ******************************************************************************/
/* The hid class callback */
static usb_status_t USB_DeviceHidGenericCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_endpoint_callback_message_struct_t *message;
    message = (usb_device_endpoint_callback_message_struct_t *) param;
    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            //usb_echo("%kUSB_DeviceHidEventSendResponse \r\n");
            break;
        case kUSB_DeviceHidEventRecvResponse:
            //usb_echo("kUSB_DeviceHidEventRecvResponse \r\n");
            if (g_UsbDeviceHidGeneric.attach)
            {
                USB_DeviceHidSend(g_UsbDeviceHidGeneric.hidHandle, USB_HID_GENERIC_ENDPOINT_IN,
                                  (uint8_t *)&g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0],
                                  USB_HID_GENERIC_OUT_BUFFER_LENGTH);

                g_UsbDeviceHidGeneric.bufferIndex ^= 1U;
                return USB_DeviceHidRecv(g_UsbDeviceHidGeneric.hidHandle, USB_HID_GENERIC_ENDPOINT_OUT,
                                         (uint8_t *)&g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0],
                                         USB_HID_GENERIC_OUT_BUFFER_LENGTH);

            }
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            error = kStatus_USB_InvalidRequest;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
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
            /* USB bus reset signal detected */
            g_UsbDeviceHidGeneric.attach = 0U;
        }
        break;
        case kUSB_DeviceEventSetConfiguration:
            if (param)
            {
                /* Set device configuration request */
                g_UsbDeviceHidGeneric.attach = 1U;
                g_UsbDeviceHidGeneric.currentConfiguration = *temp8;
                if (USB_HID_GENERIC_CONFIGURE_INDEX == (*temp8))
                {
                    error = USB_DeviceHidRecv(
                                            g_UsbDeviceHidGeneric.hidHandle, USB_HID_GENERIC_ENDPOINT_OUT,
                                            (uint8_t *)&g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0],
                                            USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                }
            }
            break;
        case kUSB_DeviceEventSetInterface:
            if (g_UsbDeviceHidGeneric.attach)
            {
                /* Set device interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
                if (interface < USB_HID_GENERIC_INTERFACE_COUNT)
                {
                    g_UsbDeviceHidGeneric.currentInterfaceAlternateSetting[interface] = alternateSetting;
                    if (alternateSetting == 0U)
                    {
                        error = USB_DeviceHidRecv(
                                                g_UsbDeviceHidGeneric.hidHandle, USB_HID_GENERIC_ENDPOINT_OUT,
                                                (uint8_t *)&g_UsbDeviceHidGeneric.buffer[g_UsbDeviceHidGeneric.bufferIndex][0],
                                                USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                    }
                }
            }
            break;
        case kUSB_DeviceEventGetConfiguration:
            if (param)
            {
                /* Get current configuration request */
                *temp8 = g_UsbDeviceHidGeneric.currentConfiguration;
                error = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceEventGetInterface:
            if (param)
            {
                /* Get current alternate setting of the interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                if (interface < USB_HID_GENERIC_INTERFACE_COUNT)
                {
                    *temp16 = (*temp16 & 0xFF00U) | g_UsbDeviceHidGeneric.currentInterfaceAlternateSetting[interface];
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
                /* Get device descriptor request */
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param)
            {
                /* Get device configuration descriptor request */
                error = USB_DeviceGetConfigurationDescriptor(handle,
                        (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetStringDescriptor:
            if (param)
            {
                /* Get device string descriptor request */
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetHidDescriptor:
            if (param)
            {
                /* Get hid descriptor request */
                error = USB_DeviceGetHidDescriptor(handle, (usb_device_get_hid_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetHidReportDescriptor:
            if (param)
            {
                /* Get hid report descriptor request */
                error =
                                USB_DeviceGetHidReportDescriptor(handle, (usb_device_get_hid_report_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetHidPhysicalDescriptor:
            if (param)
            {
                /* Get hid physical descriptor request */
                error = USB_DeviceGetHidPhysicalDescriptor(handle,
                        (usb_device_get_hid_physical_descriptor_struct_t *)param);
            }
            break;
        default:
            break;
    }

    return error;
}

#include <gm_hal_gpio.h>
#include <device.h>
#include <gpio.h>

void gpio_usb(void)
{
    int val ;
    struct device *PB, *PC, *PE, *PF;

    printk("Run Func[%s]\n", __FUNCTION__);

    PB = device_get_binding("GPIOB");
    PC = device_get_binding("GPIOC");
    PE = device_get_binding("GPIOE");
    PF = device_get_binding("GPIOF");

    gpio_pin_configure(PB, 13, GPIO_MODE_INPUT);
    gpio_pin_configure(PB, 14, GPIO_MODE_INPUT);

    gpio_pin_configure(PB, 10, GPIO_MODE_OUTPUT);
    gpio_pin_write(PB,10, GPIO_PIN_SET);

    gpio_pin_read(PB, 14, &val);
    printk("USB-SEL = [%d]\n", val);
    if(val == 1) {
	    gpio_pin_configure(PB, 14, GPIO_MODE_OUTPUT);
	    gpio_pin_write(PB,14, GPIO_PIN_SET);
	    gpio_pin_write(PB,10, GPIO_PIN_RESET);
    }


    //printk("LED ON\n");
    //gpio_pin_write(PB,10, GPIO_PIN_RESET);

}

int main(void)
{
    usb_echo("usb hid generic test....\n");
    gpio_usb();
    /* Set HID generic to default state */
    g_UsbDeviceHidGeneric.speed = USB_SPEED_FULL;
    g_UsbDeviceHidGeneric.attach = 0U;
    g_UsbDeviceHidGeneric.hidHandle = (class_handle_t)NULL;
    g_UsbDeviceHidGeneric.deviceHandle = NULL;
    g_UsbDeviceHidGeneric.buffer[0] = (uint8_t *)&s_GenericBuffer0[0];
    g_UsbDeviceHidGeneric.buffer[1] = (uint8_t *)&s_GenericBuffer1[0];

    /* Initialize the usb stack and class drivers */
    if (kStatus_USB_Success !=
            USB_DeviceClassInit(CONTROLLER_ID, &g_UsbDeviceHidConfigList, &g_UsbDeviceHidGeneric.deviceHandle))
    {
        usb_echo("USB device HID generic failed\r\n");
        return -1;
    }
    else
    {
        usb_echo("USB device HID generic demo\r\n");
        /* Get the HID mouse class handle */
        g_UsbDeviceHidGeneric.hidHandle = g_UsbDeviceHidConfigList.config->classHandle;
    }

    USB_Device_SetStackParam(&g_UsbDeviceHidGeneric.deviceHandle);

    /* Start USB device HID generic */
    USB_DeviceRun(g_UsbDeviceHidGeneric.deviceHandle);
    while (1)
    {
        k_sleep(1000);
    }
}
