/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#include "stdint.h"
#include "usb.h"
#include "usb_osa.h"
#include "kernel.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MSEC_TO_TICK(msec) ((1000L + ((uint32_t)configTICK_RATE_HZ * (uint32_t)(msec - 1U))) / 1000L)
#define TICKS_TO_MSEC(tick) ((tick)*1000uL / (uint32_t)configTICK_RATE_HZ)

#define MAX_EVENT_SINGAL_COUNT   (32U)

static struct k_poll_signal event_signals[MAX_EVENT_SINGAL_COUNT];
static struct k_poll_event wait_events[MAX_EVENT_SINGAL_COUNT];
uint32_t wait_event_length = ARRAY_SIZE(wait_events);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void *USB_OsaMemoryAllocate(uint32_t length)
{
    void *p = malloc_ext(0, length);
    uint8_t *tmp = (uint8_t *)p;
    uint32_t cnt = 0;

    if (p)
    {
        for (cnt = 0; cnt < length; cnt ++)
        {
            tmp[cnt] = 0u;
        }
    }
 
    return p;
}

void USB_OsaMemoryFree(void *p)
{
    free_ext(0, p);
}

void USB_OsaEnterCritical(void)
{
#ifndef __USE_KEIL_	
    __asm volatile ("cpsid i" : : : "memory");
#else
    __asm volatile
	{
		CPSID I
		DSB
		ISB
	}
#endif	
}

void USB_OsaExitCritical(void)
{
#ifndef __USE_KEIL_		
    __asm volatile ("cpsie i" : : : "memory");    
#else
	__asm volatile
	{
		CPSIE I
		DSB
		ISB
	}
#endif	
}

usb_osa_status_t USB_OsaEventCreate(usb_osa_event_handle *handle, uint32_t flag)
{
    uint32_t i = 0;

    for (i = 0;i < MAX_EVENT_SINGAL_COUNT; i++)
    {
		/*changed due to ugelis kernel higher version update*/
        //event_signals[i].poll_event = NULL;
		sys_dlist_init(&event_signals[i].poll_events);
        event_signals[i].result = 0;
        event_signals[i].signaled = 0;
        k_poll_event_init(&wait_events[i], K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &event_signals[i]);
    }
    
    *handle = wait_events;
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventDestroy(usb_osa_event_handle handle)
{
    /*Not implemented now !!!*/
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventSet(usb_osa_event_handle handle, uint32_t bitMask)
{    
    struct k_poll_event *event = (struct k_poll_event *)handle;
    int rc = 0;
    
    int i = 0;
    uint32_t tmpBitMask = bitMask;

    while(bitMask)
    {
        if (bitMask & 0x01)
        {
            /*set singal to trigger event*/
            rc = k_poll_signal((struct k_poll_signal *)event[i].obj, tmpBitMask);
            
            if (rc == -EAGAIN)
            {
                return kStatus_USB_OSA_Error;
            }
        }
        
        i ++;
        bitMask = bitMask >> 1;
    }

    return  kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventWait(
    usb_osa_event_handle handle, uint32_t bitMask, uint32_t flag, uint32_t timeout, uint32_t *bitSet)
{
    int rc = 0;
    int i =0;
    struct k_poll_event *event = (struct k_poll_event *)handle;
    struct k_poll_signal *signal = NULL;
    
    uint32_t tmpBitSet = 0;

    rc = k_poll(event, wait_event_length, K_MSEC(timeout));

    if (rc == -EAGAIN)
    {
        return kStatus_USB_OSA_TimeOut;
    }

    for (i = 0;i < wait_event_length; i ++)
    {
        if (event[i].state == K_POLL_STATE_SIGNALED)
        {
            signal = (struct k_poll_signal *)event[i].obj;
            
            tmpBitSet |= signal->result;
            signal->signaled = 0;
            event[i].state = K_POLL_STATE_NOT_READY;
        }
    }

    *bitSet = tmpBitSet & bitMask;

    return kStatus_USB_OSA_Success; 
}

usb_osa_status_t USB_OsaEventCheck(usb_osa_event_handle handle, uint32_t bitMask, uint32_t *bitSet)
{
    /*Not implemented now !!!*/
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventClear(usb_osa_event_handle handle, uint32_t bitMask)
{
    /*Not implemented now !!!*/
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaSemCreate(usb_osa_sem_handle *handle, uint32_t count)
{
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaSemDestroy(usb_osa_sem_handle handle)
{
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemPost(usb_osa_sem_handle handle)
{
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemWait(usb_osa_sem_handle handle, uint32_t timeout)
{
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexCreate(usb_osa_mutex_handle *handle)
{    
    struct k_mutex *osa_mutex = (struct k_mutex *)USB_OsaMemoryAllocate(sizeof(struct k_mutex));
    
  	k_mutex_init(osa_mutex);
    *handle = osa_mutex;
    
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexDestroy(usb_osa_mutex_handle handle)
{
    USB_OsaMemoryFree((struct k_mutex *)handle);
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexLock(usb_osa_mutex_handle handle)
{
    struct k_mutex *pmutex = (struct k_mutex *)handle;
    k_mutex_lock(pmutex, K_FOREVER);
    
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexUnlock(usb_osa_mutex_handle handle)
{
    struct k_mutex *pmutex = (struct k_mutex *)handle;    
    k_mutex_unlock(pmutex);    
    
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCreate(usb_osa_msgq_handle *handle, uint32_t count, uint32_t size)
{
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqDestroy(usb_osa_msgq_handle handle)
{
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqSend(usb_osa_msgq_handle handle, void *msg)
{
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaMsgqRecv(usb_osa_msgq_handle handle, void *msg, uint32_t timeout)
{
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCheck(usb_osa_msgq_handle handle, void *msg)
{
    return kStatus_USB_OSA_Error;
}
