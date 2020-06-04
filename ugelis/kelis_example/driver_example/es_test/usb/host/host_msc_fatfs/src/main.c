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
#include "usb_host_msd.h"
#include "host_msd_fatfs.h"
#include "main.h"
#include <ugelis.h>
#include <soc.h>
#include "usb_host_khci.h"

#if ((!USB_HOST_CONFIG_KHCI) && (!USB_HOST_CONFIG_EHCI) && (!USB_HOST_CONFIG_OHCI) && (!USB_HOST_CONFIG_IP3516HS))
    #error Please enable USB_HOST_CONFIG_KHCI, USB_HOST_CONFIG_EHCI, USB_HOST_CONFIG_OHCI, or USB_HOST_CONFIG_IP3516HS in file usb_host_config.
#endif

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
 * @param deviceHandle        device handle.
 * @param configurationHandle attached device's configuration descriptor information.
 * @param eventCode           callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The application don't support the configuration.
 */
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);

/*!
 * @brief app initialization.
 */
static void USB_HostApplicationInit(void);

static void USB_HostTask(void *param);

static void USB_HostApplicationTask(void *param);

extern void USB_HostIsrEnable(void);
extern void USB_HostTaskFn(void *param);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief USB host msd fatfs instance global variable */
extern usb_host_msd_fatfs_instance_t g_MsdFatfsInstance;
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
    usb_status_t status = kStatus_USB_Success;
    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            status = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
            break;

        case kUSB_HostEventNotSupported:
            usb_echo("device not supported.\r\n");
            break;

        case kUSB_HostEventEnumerationDone:
            status = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
            break;

        case kUSB_HostEventDetach:
            status = USB_HostMsdEvent(deviceHandle, configurationHandle, eventCode);
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

    //USB_HostIsrEnable();
    USB_Host_SetStackParam(&g_HostHandle);

    usb_echo("host init done\r\n");
}


typedef void *usb_osa_event_handle;
usb_osa_event_handle khciEventPointer;      /*!< KHCI event*/

typedef void *usb_osa_mutex_handle;
usb_osa_mutex_handle khciEventPointerMutex;



static void USB_HostTask(void *param)
{
#if 0
#if 1
    /*test event flag*/
    uint32_t eventBit = 0;
    USB_OsaEventCreate(&khciEventPointer, 1U);

    while (1)
    {
        USB_OsaEventWait(khciEventPointer, 0xffffffff, 0U, 1, &eventBit);  //K_FOREVER
        printk("eventBit = 0x%x \n", eventBit);//printf
    }
#else

    USB_OsaMutexCreate(&khciEventPointerMutex);

    while (1)
    {
        USB_OsaMutexLock(khciEventPointerMutex);
        printk("Locked by %s \n", __FUNCTION__); //printf
        k_sleep(1000);
        USB_OsaMutexUnlock(khciEventPointerMutex);
        k_sleep(1000);
    }

#endif

#else
    while (1)
    {
        USB_HostTaskFn(param);
        //k_sleep(1);
        k_yield();
    }
#endif
}

static void USB_HostApplicationTask(void *param)
{
#if 0
#if 1
    uint32_t bitmask, tmpbitmask;
    uint32_t i, j;

    bitmask = 0x01;
    i = 0;
    j = 0;

    k_sleep(500);
    /*test single bitmask!*/
    while (j < 32)
    {
        tmpbitmask = bitmask << i;
        USB_OsaEventSet(khciEventPointer, tmpbitmask);
        k_sleep(500);
        i ++;
        if (i == 32)
        {
            i = 0;
        }
        j ++;
    }

    while (1)
    {
        USB_OsaEventSet(khciEventPointer, 0x84);
        k_sleep(500);
        USB_OsaEventSet(khciEventPointer, 0xffffffff);
        k_sleep(500);
    }
#else
    while (1)
    {
        USB_OsaMutexLock(khciEventPointerMutex);
        printk("Locked by %s \n", __FUNCTION__); //printf
        k_sleep(8000);
        USB_OsaMutexUnlock(khciEventPointerMutex);
        k_sleep(8000);
    }
#endif

#else
    while (1)
    {
        USB_HostMsdTask(param);
        //k_sleep(1);
        k_yield();
    }
#endif
}

#define STACKSIZE                4096
#define UGELIS_NEW_VERSION       1

#ifndef UGELIS_NEW_VERSION

    char __noinit __stack hosttask_stack_area[STACKSIZE];
    char __noinit __stack apptask_stack_area[STACKSIZE];

#else

    K_THREAD_STACK_DEFINE(hosttask_stack_area, STACKSIZE);
    K_THREAD_STACK_DEFINE(apptask_stack_area, STACKSIZE);

    static struct k_thread hosttask_thread;
    static struct k_thread apptask_thread;

#endif

int main(void)
{

    /*Set as USB-Host*/
    *(volatile uint8_t *)(0x50000210) = 0x1C;

    USB_HostApplicationInit();

#ifndef UGELIS_NEW_VERSION

    /*Create task as zephyr requires*/
    k_thread_spawn(hosttask_stack_area, STACKSIZE, USB_HostTask, g_HostHandle, NULL, NULL,  K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    /*Create task as zephyr requires*/
    k_thread_spawn(apptask_stack_area, STACKSIZE, USB_HostApplicationTask, &g_MsdFatfsInstance, NULL, NULL, K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

#else
    k_thread_create(&hosttask_thread, hosttask_stack_area,
                    K_THREAD_STACK_SIZEOF(hosttask_stack_area),
                    (k_thread_entry_t)USB_HostTask,  g_HostHandle, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    k_thread_create(&apptask_thread, apptask_stack_area,
                    K_THREAD_STACK_SIZEOF(apptask_stack_area),
                    (k_thread_entry_t)USB_HostApplicationTask, &g_MsdFatfsInstance, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
#endif

    while (1)
    {
        k_sleep(1000);
    }
}
