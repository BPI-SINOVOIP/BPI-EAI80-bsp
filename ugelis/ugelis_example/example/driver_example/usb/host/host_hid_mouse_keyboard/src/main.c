/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
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
#include "usb_host_hid.h"
#include "host_keyboard_mouse.h"
#include "host_keyboard.h"
#include "host_mouse.h"
#include "usb_host_khci.h"

#include "main.h"
#include "ugelis.h"
#include "soc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief host callback function.
 *
 * device attach/detach callback function.
 *
 * @param deviceHandle          device handle.
 * @param configurationHandle   attached device's configuration descriptor information.
 * @param eventCode             callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The application don't support the configuration.
 */
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);

/*!
 * @brief application initialization.
 */
static void USB_HostApplicationInit(void);

/*!
 * @brief host freertos task function.
 *
 * @param g_HostHandle   host handle
 */
static void USB_HostTask(void *param);

/*!
 * @brief host keybaord freertos task function.
 *
 * @param param   the host keybaord instance pointer.
 */
static void USB_HostApplicationKeyboardTask(void *param);

extern void USB_HostIsrEnable(void);
extern void USB_HostTaskFn(void *param);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief USB host mouse instance global variable */
extern usb_host_mouse_instance_t g_HostHidMouse;
/*! @brief USB host keyboard instance global variable */
extern usb_host_keyboard_instance_t g_HostHidKeyboard;
usb_host_handle g_HostHandle;

/*******************************************************************************
 * Code
 ******************************************************************************/

void USB_HostIsrEnable(void)
{
    /*register usbd interrupt handler*/
    IRQ_CONNECT(USB_IRQn, 1, USB_HostKhciIsrFunction, &g_HostHandle, 0);

    NVIC_ClearPendingIRQ(USB_IRQn);
    /*enable usb interrupt*/
    irq_enable(USB_IRQn);
}

void USB_HostTaskFn(void *param)
{
#if defined(USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI > 0U)
    USB_HostKhciTaskFunction(param);
#endif
}

/*!
 * @brief USB isr function.
 */

static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode)
{
    usb_status_t status1;
    usb_status_t status2;
    usb_status_t status = kStatus_USB_Success;
    printk("%s,eventCode:%d\n", __FUNCTION__, eventCode);
    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            status1 = USB_HostHidKeyboardEvent(deviceHandle, configurationHandle, eventCode);
            status2 = USB_HostHidMouseEvent(deviceHandle, configurationHandle, eventCode);
            if ((status1 == kStatus_USB_NotSupported) && (status2 == kStatus_USB_NotSupported))
            {
                status = kStatus_USB_NotSupported;
            }
            break;

        case kUSB_HostEventNotSupported:
            usb_echo("device not supported.\r\n");
            break;

        case kUSB_HostEventEnumerationDone:
            status1 = USB_HostHidKeyboardEvent(deviceHandle, configurationHandle, eventCode);
            status2 = USB_HostHidMouseEvent(deviceHandle, configurationHandle, eventCode);
            if ((status1 != kStatus_USB_Success) && (status2 != kStatus_USB_Success))
            {
                status = kStatus_USB_Error;
            }
            break;

        case kUSB_HostEventDetach:
            status1 = USB_HostHidKeyboardEvent(deviceHandle, configurationHandle, eventCode);
            status2 = USB_HostHidMouseEvent(deviceHandle, configurationHandle, eventCode);
            if ((status1 != kStatus_USB_Success) && (status2 != kStatus_USB_Success))
            {
                status = kStatus_USB_Error;
            }
            break;

        default:
            break;
    }
    return status;
}

static void USB_HostApplicationInit(void)
{
    usb_status_t status = kStatus_USB_Success;
    status = USB_HostInit(CONTROLLER_ID, &g_HostHandle, USB_HostEvent);
    if (status != kStatus_USB_Success)
    {
        usb_echo("host init error\r\n");
        return;
    }

    USB_Host_SetStackParam(&g_HostHandle);

    usb_echo("host init done\r\n");
}

static void USB_HostTask(void *param)
{

    while (1)
    {
        USB_HostTaskFn(param);
        k_sleep(1);
        //k_yield();
    }
}

static void USB_HostApplicationMouseTask(void *param)
{
    while (1)
    {
        USB_HostHidMouseTask(param);
        k_sleep(1);
        // k_yield();
    }
}

static void USB_HostApplicationKeyboardTask(void *param)
{
    while (1)
    {
        USB_HostHidKeyboardTask(param);
        k_sleep(1);
        //k_yield();
    }
}

#define STACKSIZE              2048
#define UGELIS_NEW_VERSION     1

#ifndef UGELIS_NEW_VERSION
    char __noinit __stack hosttask_stack_area[STACKSIZE];
    char __noinit __stack appMousetask_stack_area[STACKSIZE];
    char __noinit __stack appKeyBoardtask_stack_area[STACKSIZE];
#else

    K_THREAD_STACK_DEFINE(hosttask_stack_area, STACKSIZE);
    K_THREAD_STACK_DEFINE(appMousetask_stack_area, STACKSIZE);
    K_THREAD_STACK_DEFINE(appKeyBoardtask_stack_area, STACKSIZE);

    static struct k_thread hosttask_thread;
    static struct k_thread appMousetask_thread;
    static struct k_thread appKeyBoardtask;
#endif


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
    /*Set as USB-Host*/
    //   *(volatile uint8_t *)(0x50000210) = 0x1C;
    gpio_usb();

    USB_HostApplicationInit();
#ifndef UGELIS_NEW_VERSION
    /*Create task as zephyr requires*/
    k_thread_spawn(hosttask_stack_area, STACKSIZE, USB_HostTask, g_HostHandle, NULL, NULL,  K_PRIO_PREEMPT(3), 0, K_NO_WAIT);

    /*Create task as zephyr requires*/
    k_thread_spawn(appMousetask_stack_area, STACKSIZE, USB_HostApplicationMouseTask, &g_HostHidMouse, NULL, NULL,   K_PRIO_PREEMPT(4), 0, K_NO_WAIT);

    k_thread_spawn(appKeyBoardtask_stack_area, STACKSIZE, USB_HostApplicationKeyboardTask, &g_HostHidKeyboard, NULL, NULL,  K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
#else
    usb_echo("usb host mode:keyboard and mouse\r\n");
    k_thread_create(&hosttask_thread, hosttask_stack_area,
                    K_THREAD_STACK_SIZEOF(hosttask_stack_area),
                    (k_thread_entry_t)USB_HostTask,  g_HostHandle, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    k_thread_create(&appMousetask_thread, appMousetask_stack_area,
                    K_THREAD_STACK_SIZEOF(appMousetask_stack_area),
                    (k_thread_entry_t)USB_HostApplicationMouseTask, &g_HostHidMouse, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    k_thread_create(&appKeyBoardtask, appKeyBoardtask_stack_area,
                    K_THREAD_STACK_SIZEOF(appKeyBoardtask_stack_area),
                    (k_thread_entry_t)USB_HostApplicationKeyboardTask, &g_HostHidKeyboard, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

#endif

    while (1)
    {
        k_sleep(1000);
    }
}
