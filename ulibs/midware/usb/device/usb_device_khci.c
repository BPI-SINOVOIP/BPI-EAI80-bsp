/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2018 NXP
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
#ifndef RABBIT_NO_OS_SYSTEM
#include <kernel.h>
#include <soc.h>
#endif
#include "usb_device_config.h"
#include "usb.h"
#ifdef RABBIT_NO_OS_SYSTEM
#include "usb_osa.h"
#include "usb_osa_bm.h"
#endif
#include "usb_device.h"

#if ((defined(USB_DEVICE_CONFIG_KHCI)) && (USB_DEVICE_CONFIG_KHCI > 0U))

#include "usb_khci.h"
#include "usb_device_dci.h"
#include "usb_device_khci.h"

#include "gm_hal_usb.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(USB_STACK_USE_DEDICATED_RAM) && (USB_STACK_USE_DEDICATED_RAM > 0U)
/* USB_STACK_USE_DEDICATED_RAM */
#if defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)

#if (USB_STACK_USE_DEDICATED_RAM == USB_STACK_DEDICATED_RAM_TYPE_BDT_GLOBAL)
#if (FSL_FEATURE_USB_KHCI_USB_RAM > 512U)
#else
#error The dedicated RAM length is not more than 512 Bytes, the SOC does not support this case.
#endif
#endif /* USB_STACK_USE_DEDICATED_RAM */

#else
#error The SOC does not suppoort dedicated RAM case.
#endif /* USB_STACK_USE_DEDICATED_RAM */

#endif

typedef struct _endpoint_message
{    
    uint8_t  buffer[64];
    uint16_t length;
    uint8_t  isSetup;
    uint8_t  ep;
} endpoint_message;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static usb_status_t USB_DeviceKhciEndpointTransfer(
    usb_device_khci_state_struct_t *khciState, uint8_t endpoint, uint8_t direction, uint8_t *buffer, uint32_t length);
static void USB_DeviceKhciPrimeNextSetup(usb_device_khci_state_struct_t *khciState);
static void USB_DeviceKhciSetDefaultState(usb_device_khci_state_struct_t *khciState);
static usb_status_t USB_DeviceKhciEndpointInit(usb_device_khci_state_struct_t *khciState,
                                               usb_device_endpoint_init_struct_t *epInit);
static usb_status_t USB_DeviceKhciEndpointDeinit(usb_device_khci_state_struct_t *khciState, uint8_t ep);
static usb_status_t USB_DeviceKhciEndpointStall(usb_device_khci_state_struct_t *khciState, uint8_t ep);
static usb_status_t USB_DeviceKhciEndpointUnstall(usb_device_khci_state_struct_t *khciState, uint8_t ep);
static void USB_DeviceKhciInterruptTokenDone(usb_device_khci_state_struct_t *khciState , endpoint_message *epmsg);
static void USB_DeviceKhciInterruptReset(usb_device_khci_state_struct_t *khciState);
#if (defined(USB_DEVICE_CONFIG_LOW_POWER_MODE) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
static void USB_DeviceKhciInterruptSleep(usb_device_khci_state_struct_t *khciState);
static void USB_DeviceKhciInterruptResume(usb_device_khci_state_struct_t *khciState);
#endif /* USB_DEVICE_CONFIG_LOW_POWER_MODE */
static void USB_DeviceKhciInterruptStall(usb_device_khci_state_struct_t *khciState);
#if defined(USB_DEVICE_CONFIG_ERROR_HANDLING) && (USB_DEVICE_CONFIG_ERROR_HANDLING > 0U)
static void USB_DeviceKhciInterruptError(usb_device_khci_state_struct_t *khciState);
#endif /* USB_DEVICE_CONFIG_ERROR_HANDLING */

extern usb_status_t USB_DeviceNotificationTrigger(void *handle, void *msg);

volatile uint8_t direction_need_switch = 0;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Apply for khci device state structure */
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(4) static usb_device_khci_state_struct_t 
    s_UsbDeviceKhciState[USB_DEVICE_CONFIG_KHCI];


#if (defined(USB_DEVICE_CHARGER_DETECT_ENABLE) && (USB_DEVICE_CHARGER_DETECT_ENABLE > 0U)) && \
    (defined(FSL_FEATURE_SOC_USBDCD_COUNT) && (FSL_FEATURE_SOC_USBDCD_COUNT > 0U))
/* Apply for device dcd state structure */
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_device_dcd_state_struct_t
    s_UsbDeviceDcdState[USB_DEVICE_CONFIG_KHCI];
#endif

/* Apply for KHCI DMA aligned buffer when marco USB_DEVICE_CONFIG_KHCI_DMA_ALIGN enabled */
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(4) static uint32_t s_UsbDeviceKhciDmaAlignBuffer
    [USB_DEVICE_CONFIG_KHCI][((USB_DEVICE_CONFIG_KHCI_DMA_ALIGN_BUFFER_LENGTH - 1U) >> 2U) + 1U];

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Write the BDT to start a transfer.
 *
 * The function is used to start a transfer by writing the BDT.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 * @param endpoint         Endpoint number.
 * @param direction        The direction of the endpoint, 0U - USB_OUT, 1U - USB_IN.
 * @param buffer           The memory address to save the received data, or the memory address to hold the data need to
 * be sent.
 * @param length           The length of the data.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceKhciEndpointTransfer(
    usb_device_khci_state_struct_t *khciState, uint8_t endpoint, uint8_t direction, uint8_t *buffer, uint32_t length)
{	
	uint32_t index = ((uint32_t)endpoint << 1U) | (uint32_t)direction;
    USB_OSA_SR_ALLOC();

    /* Enter critical */
    USB_OSA_ENTER_CRITICAL();

    /* Flag the endpoint is busy. */
    khciState->endpointState[index].stateUnion.stateBitField.transferring = 1U;

    /*It's in(tx) endpoint*/
    if (direction)
    {
        /* 
           Start the transfer. Data will be transferred through Endpoint x, 
           it will trigger another transfer interrupt
        */		
		if (length >= khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize)
		{
			length = khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize;
			HAL_USB_WritePacket(khciState->pUSB_Otg, buffer, endpoint, length); 	

			if(khciState->endpointState[index].transferRemainLen == 0)
            {
				HAL_USB_SetEndpointxTxPktRdy(khciState->pUSB_Otg, endpoint, 1);
				/* transferring completed flag, if you wouldn't add it ,ISOC will not bus enumaeration*/
				if(endpoint == USB_CONTROL_ENDPOINT)
				{
				  khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;  
				}
			}
			else
			{
				HAL_USB_SetEndpointxTxPktRdy(khciState->pUSB_Otg, endpoint, 0);
			}
		}
		else
		{
			HAL_USB_WritePacket(khciState->pUSB_Otg, buffer, endpoint, length);
			HAL_USB_SetEndpointxTxPktRdy(khciState->pUSB_Otg, endpoint, 1);
            if(endpoint == USB_CONTROL_ENDPOINT)
            {
				khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;  
            }
		}

    }
    /*It's out(rx) endpoint*/
    else
    {
        /* Read data from fifo */

    }

    /* Exit critical */
    USB_OSA_EXIT_CRITICAL();

    return kStatus_USB_Success;
}

/*!
 * @brief Prime a next setup transfer.
 *
 * The function is used to prime a buffer in control out pipe to wait for receiving the host's setup packet.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciPrimeNextSetup(usb_device_khci_state_struct_t *khciState)
{
    /* Update the endpoint state */
    /* Save the buffer address used to receive the setup packet. */
#if defined(FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED) && (FSL_FEATURE_USB_KHCI_KEEP_ALIVE_ENABLED > 0U) && \
        defined(USB_DEVICE_CONFIG_KEEP_ALIVE_MODE) && (USB_DEVICE_CONFIG_KEEP_ALIVE_MODE > 0U) &&             \
        defined(FSL_FEATURE_USB_KHCI_USB_RAM) && (FSL_FEATURE_USB_KHCI_USB_RAM > 0U)
        /* In case of lowpower mode enabled, it requires to put the setup packet buffer(16 bytes) into the USB RAM so
         * that the setup packet would wake up the USB.
         */
        khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].transferBuffer =
            (uint8_t *)(khciState->bdt + 0x200U - 0x10U) +
            khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.bdtOdd *
                USB_SETUP_PACKET_SIZE;
#else
        khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].transferBuffer =
            (uint8_t *)&khciState->setupPacketBuffer[0] +
            khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.bdtOdd *
                USB_SETUP_PACKET_SIZE;
#endif

        /* Clear the transferred length. */
        khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].transferDone = 0U;
        /* Save the data length expected to get from a host. */
        khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].transferLength = USB_SETUP_PACKET_SIZE;
        /* Save the data buffer DMA align flag. */
        khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.dmaAlign = 1U;
        /* Set the DATA0/1 to DATA0. */
        khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.data0 = 0U;

        USB_DeviceKhciEndpointTransfer(khciState, USB_CONTROL_ENDPOINT, USB_OUT,
                                       khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].transferBuffer,
                                       USB_SETUP_PACKET_SIZE);
}

/*!
 * @brief Set device controller state to default state.
 *
 * The function is used to set device controller state to default state.
 * The function will be called when USB_DeviceKhciInit called or the control type kUSB_DeviceControlGetEndpointStatus
 * received in USB_DeviceKhciControl.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciSetDefaultState(usb_device_khci_state_struct_t *khciState)
{
    /*Set default 0 address*/
	HAL_USB_SetAddress(khciState->pUSB_Otg, 0);
    HAL_USB_InitializeDevice(khciState->pUSB_Otg);
    
    khciState->isDmaAlignBufferInusing = 0U;
    /* Clear reset flag */
    khciState->isResetting = 0U;	
}

/*!
 * @brief Initialize a specified endpoint.
 *
 * The function is used to initialize a specified endpoint.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 * @param epInit          The endpoint initialization structure pointer.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceKhciEndpointInit(usb_device_khci_state_struct_t *khciState,
                                               usb_device_endpoint_init_struct_t *epInit)
{
	/*Enable corresponding endpoint flag*/
    uint16_t maxPacketSize = epInit->maxPacketSize;
    uint8_t endpoint = (epInit->endpointAddress & USB_ENDPOINT_NUMBER_MASK);
    uint8_t direction = (epInit->endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;
    uint8_t index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;
	/*cmt*/
    khciState->transfertype = epInit->transferType;
    /*cmt end*/
	/* Make the endpoint max packet size align with USB Specification 2.0. */
    if (USB_ENDPOINT_ISOCHRONOUS == epInit->transferType)
    {
        if (maxPacketSize > USB_DEVICE_MAX_FS_ISO_MAX_PACKET_SIZE)
        {
            maxPacketSize = USB_DEVICE_MAX_FS_ISO_MAX_PACKET_SIZE;
        }
	    /*cmt*/
		/*set endpoint isoc*/
        if(direction == USB_OUT)
        {
            //khciState->pUSB_Otg->Index = endpoint;			
			//khciState->pUSB_Otg->RxCSR2 |= USB_OTG_RXCSR2_ISO;
			HAL_USB_SetIndex(khciState->pUSB_Otg, endpoint);
			HAL_USB_SetRxCSR2(khciState->pUSB_Otg, USB_OTG_RXCSR2_ISO);
        }
		else if (direction == USB_IN)
	    {
            //khciState->pUSB_Otg->Index = endpoint;
			//khciState->pUSB_Otg->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_ISO;
			//khciState->pUSB_Otg->Power |= USB_OTG_POWER_ISOUPDATE;			
            HAL_USB_SetIndex(khciState->pUSB_Otg, endpoint);
			HAL_USB_SetTxCSR2(khciState->pUSB_Otg, USB_OTG_TXCSR2_ISO);
			HAL_USB_SetPower(khciState->pUSB_Otg, USB_OTG_POWER_ISOUPDATE);			
	    }

		/*cmt end*/
    }
    else
    {
        if (maxPacketSize > USB_DEVICE_MAX_FS_NONE_ISO_MAX_PACKET_SIZE)
        {
            maxPacketSize = USB_DEVICE_MAX_FS_NONE_ISO_MAX_PACKET_SIZE;
        }
        /* Enable an endpoint to perform handshaking during a transaction to this endpoint. */
        //khciState->registerBase->ENDPOINT[endpoint].ENDPT |= USB_ENDPT_EPHSHK_MASK;
    }
    /* Set the endpoint idle */
    khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;
    /* Save the max packet size of the endpoint */
    khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize = maxPacketSize;
    /* Set the data toggle to DATA0 */
    khciState->endpointState[index].stateUnion.stateBitField.data0 = 0U;
    /* Clear the endpoint stalled state */
    khciState->endpointState[index].stateUnion.stateBitField.stalled = 0U;
    /* Set the ZLT field */
    khciState->endpointState[index].stateUnion.stateBitField.zlt = epInit->zlt;

    /* Enable the endpoint. */
    HAL_USB_EnableEndpoints(khciState->pUSB_Otg, epInit->endpointAddress);

    /*Flush Rx & Tx FIFO*/
    if (epInit->endpointAddress >> 7)
    {
        HAL_USB_FlushTxFifo(khciState->pUSB_Otg, epInit->endpointAddress & 0x0F);
    }
    else
    {
        HAL_USB_FlushRxFifo(khciState->pUSB_Otg, epInit->endpointAddress & 0x0F);
    }

    /*walk-around, magic*/
    if ((epInit->endpointAddress & 0x0F) != 0)
    {
        HAL_USB_FlushTxFifo(khciState->pUSB_Otg, 0);    
    }

    /* Prime a transfer to receive next setup packet when the endpoint is control out endpoint. */
    if ((USB_CONTROL_ENDPOINT == endpoint) && (USB_OUT == direction))
    {
        USB_DeviceKhciPrimeNextSetup(khciState);
    }

    return kStatus_USB_Success;
}

/*!
 * @brief De-initialize a specified endpoint.
 *
 * The function is used to de-initialize a specified endpoint.
 * Current transfer of the endpoint will be canceled and the specified endpoint will be disabled.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 * @param ep               The endpoint address, Bit7, 0U - USB_OUT, 1U - USB_IN.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceKhciEndpointDeinit(usb_device_khci_state_struct_t *khciState, uint8_t ep)
{
#if 0
    uint8_t endpoint = (ep & USB_ENDPOINT_NUMBER_MASK);
    uint8_t direction =
        (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >> USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;
    uint8_t index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;

    /* Cancel the transfer of the endpoint */
    USB_DeviceKhciCancel(khciState, ep);

    /* Disable the endpoint */
    khciState->registerBase->ENDPOINT[endpoint].ENDPT = 0x00U;
    /* Clear the max packet size */
    khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize = 0U;
#else
    uint8_t endpoint = (ep & USB_ENDPOINT_NUMBER_MASK);
    uint8_t direction =
        (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >> USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;
    uint8_t index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;

    /* Cancel the transfer of the endpoint */
    USB_DeviceKhciCancel(khciState, ep);
    
    /* Disable the endpoint */
    HAL_USB_DisableEndpoints(khciState->pUSB_Otg, ep);
    
    /* Clear the max packet size */
    khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize = 0U;
#endif
    return kStatus_USB_Success;
}

/*!
 * @brief Stall a specified endpoint.
 *
 * The function is used to stall a specified endpoint.
 * Current transfer of the endpoint will be canceled and the specified endpoint will be stalled.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 * @param ep               The endpoint address, Bit7, 0U - USB_OUT, 1U - USB_IN.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceKhciEndpointStall(usb_device_khci_state_struct_t *khciState, uint8_t ep)
{
#if 0
    uint8_t endpoint = ep & USB_ENDPOINT_NUMBER_MASK;
    uint8_t direction =
        (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >> USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;
    uint8_t index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;

    /* Cancel the transfer of the endpoint */
    USB_DeviceKhciCancel(khciState, ep);

    /* Set endpoint stall flag. */
    khciState->endpointState[index].stateUnion.stateBitField.stalled = 1U;
#if defined(FSL_FEATURE_USB_KHCI_HAS_STALL_LOW) && (FSL_FEATURE_USB_KHCI_HAS_STALL_LOW > 0U)
    if (USB_CONTROL_ENDPOINT != endpoint)
    {
        if(USB_IN == direction)
        {
            if (endpoint < 8)
            {
                khciState->registerBase->STALL_IL_DIS &= ~(1<<endpoint);
            }
#if defined(FSL_FEATURE_USB_KHCI_HAS_STALL_HIGH) && (FSL_FEATURE_USB_KHCI_HAS_STALL_HIGH > 0U)
            else if ((endpoint >= 8) && (endpoint < 16))
            {
                khciState->registerBase->STALL_IH_DIS &= ~(1<<(endpoint-8));
            }
#endif
        }
        else
        {
            if (endpoint < 8)
            {
                khciState->registerBase->STALL_OL_DIS &= ~(1<<endpoint);
            }
#if defined(FSL_FEATURE_USB_KHCI_HAS_STALL_HIGH) && (FSL_FEATURE_USB_KHCI_HAS_STALL_HIGH > 0U)
            else if ((endpoint >= 8) && (endpoint < 16))
            {
                khciState->registerBase->STALL_OH_DIS &= ~(1<<(endpoint-8));
            }
#endif
        }
    }
#endif
    /* Set endpoint stall in BDT. And then if the host send a IN/OUT tanscation, the device will response a STALL state.
     */
    USB_KHCI_BDT_SET_CONTROL(
        (uint32_t)khciState->bdt, endpoint, direction, khciState->endpointState[index].stateUnion.stateBitField.bdtOdd,
        USB_LONG_TO_LITTLE_ENDIAN(
            (uint32_t)(USB_KHCI_BDT_BC(khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize) |
                       USB_KHCI_BDT_DTS | USB_KHCI_BDT_STALL | USB_KHCI_BDT_OWN)));

    khciState->registerBase->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
#else
    uint8_t endpoint = ep & USB_ENDPOINT_NUMBER_MASK;
    uint8_t direction =
        (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >> USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;
    uint8_t index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;

    /* Cancel the transfer of the endpoint */
    USB_DeviceKhciCancel(khciState, ep);

    /* Set endpoint stall flag. */
    khciState->endpointState[index].stateUnion.stateBitField.stalled = 1U;

    HAL_USB_EndpointsStall(khciState->pUSB_Otg, ep);
#endif
    return kStatus_USB_Success;
}

/*!
 * @brief Un-stall a specified endpoint.
 *
 * The function is used to un-stall a specified endpoint.
 * Current transfer of the endpoint will be canceled and the specified endpoint will be un-stalled.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 * @param ep               The endpoint address, Bit7, 0U - USB_OUT, 1U - USB_IN.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceKhciEndpointUnstall(usb_device_khci_state_struct_t *khciState, uint8_t ep)
{
	uint8_t endpoint = ep & USB_ENDPOINT_NUMBER_MASK;
    uint8_t direction =
        (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >> USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;
    uint8_t index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;
    uint8_t pre_index = 0;

    /* Clear the endpoint stall state */
    khciState->endpointState[index].stateUnion.stateBitField.stalled = 0U;
    /* Reset the endpoint data toggle to DATA0 */
    khciState->endpointState[index].stateUnion.stateBitField.data0 = 0U;

    //pre_index = khciState->pUSB_Otg->Index;
	pre_index = HAL_USB_GetIndex(khciState->pUSB_Otg);

    if ((USB_CONTROL_ENDPOINT != endpoint))
    {
        /* Cancel the transfer of the endpoint */
        USB_DeviceKhciCancel(khciState, ep);
    }

    /*Unstall operation for endpoint */
    if ((USB_CONTROL_ENDPOINT != endpoint))
    {
        //khciState->pUSB_Otg->Index = endpoint;
        HAL_USB_SetIndex(khciState->pUSB_Otg, endpoint);
		
        if (direction)
        {   
#if 0
            khciState->pUSB_Otg->CtrlSts0_Union.TxCSR1 &= ~(USB_OTG_TXCSR1_SENDSTLL | USB_OTG_TXCSR1_SENTSTLL);
            khciState->pUSB_Otg->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_CLRDATATOG;
            khciState->pUSB_Otg->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_TXPKTRDY;
#endif
			HAL_USB_ClearTxCSR1(khciState->pUSB_Otg, (USB_OTG_TXCSR1_SENDSTLL | USB_OTG_TXCSR1_SENTSTLL));
			HAL_USB_SetTxCSR1(khciState->pUSB_Otg, USB_OTG_TXCSR1_CLRDATATOG);
			HAL_USB_SetTxCSR1(khciState->pUSB_Otg, USB_OTG_TXCSR1_TXPKTRDY);			
        }
        else
        {
            //khciState->pUSB_Otg->RxCSR1 &= ~(USB_OTG_RXCSR1_SENDSTLL);
			HAL_USB_ClearRxCSR1(khciState->pUSB_Otg, USB_OTG_RXCSR1_SENDSTLL);
        }
    }

    //khciState->pUSB_Otg->Index = pre_index;
	HAL_USB_SetIndex(khciState->pUSB_Otg, pre_index);

    /* Prime a transfer to receive next setup packet when the endpoint is a control out endpoint. */
    if ((USB_CONTROL_ENDPOINT == endpoint) && (USB_OUT == direction))
    {
        USB_DeviceKhciPrimeNextSetup(khciState);
    }
	
    return kStatus_USB_Success;
}

/*!
 * @brief Handle the token done interrupt.
 *
 * The function is used to handle the token done interrupt.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */

static void USB_DeviceKhciInterruptTokenDone(usb_device_khci_state_struct_t *khciState, endpoint_message *epmsg)
{
    uint32_t length;
    uint32_t remainingLength;
    usb_device_callback_message_struct_t message;
    uint8_t endpoint;
    uint8_t direction;
    uint8_t bdtOdd;
    uint8_t isSetup;
    uint8_t index;

    endpoint =  (epmsg->ep) & 0x0F;
    direction = (epmsg->ep & 0x80) >> 7 ;
    isSetup = epmsg->isSetup;
    length = epmsg->length;

    /*
     *endpointState[0] --> endpoint 0 OUT
     *endpointState[1] --> endpoint 0 IN
     *endpointState[2] --> endpoint 1 OUT
     *endpointState[3] --> endpoint 1 IN
     *endpointState[4] --> endpoint 2 OUT
     *endpointState[5] --> endpoint 2 IN
     *endpointState[6] --> endpoint 3 OUT
     *endpointState[7] --> endpoint 3 IN
     */
    index = ((uint8_t)((uint32_t)endpoint << 1U)) | (uint8_t)direction;

    //usb_echo("dir: %d, ep: %d, index: %d, len: %d\r\n", direction, endpoint, index, length);    

//    USB_NOPS_1;

	
	bdtOdd = 0;
	
    if (0U == khciState->endpointState[index].stateUnion.stateBitField.transferring)
    {
        return ;
    }

    if (isSetup)
    {
        khciState->setupBufferIndex = bdtOdd;
    }

    /* USB_IN, Send completed */
    if (direction == USB_IN)
    {
       /* The transferred length */
		khciState->endpointState[index].transferDone += length;
		/* Remaining length */
		remainingLength = khciState->endpointState[index].transferLength - khciState->endpointState[index].transferDone;


        /* Change the data toggle flag */
        khciState->endpointState[index].stateUnion.stateBitField.data0 ^= 1U;
        /* Change the BDT odd toggle flag */
        khciState->endpointState[index].stateUnion.stateBitField.bdtOdd ^= 1U;

        /* Whether the transfer is completed or not. */
        /*
         * The transfer is completed when one of the following conditions meet:
         * 1. The remaining length is zero.
         * 2. The length of current transcation is less than the max packet size of the current pipe.
         */
        if ((0U == remainingLength) ||
            (khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize > length))
        {
            message.length = khciState->endpointState[index].transferDone;
			message.buffer = khciState->endpointState[index].transferBuffer;
            khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;
			/*
             * Whether need to send ZLT when the pipe is control in pipe and the transferred length of current
             * transaction equals to max packet size.
             */
            if ((length) && (!(length % khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize)))
            {
                if (USB_CONTROL_ENDPOINT == endpoint)
                {
                    usb_setup_struct_t *setup_packet =
                        (usb_setup_struct_t
                             *)(&khciState->setupPacketBuffer[(USB_SETUP_PACKET_SIZE * khciState->setupBufferIndex)]);
                    /*
                     * Send the ZLT and terminate the token done interrupt service when the tranferred length in data
                     * phase
                     * is less than the host request.
                     */
                    if (USB_SHORT_FROM_LITTLE_ENDIAN(setup_packet->wLength) >
                        khciState->endpointState[index].transferLength)
                    {
                        (void)USB_DeviceKhciEndpointTransfer(khciState, endpoint, USB_IN, (uint8_t *)NULL, 0U);
                        return ;
                    }
                }
                else if (khciState->endpointState[index].stateUnion.stateBitField.zlt)
                {
                    
					(void)USB_DeviceKhciEndpointTransfer(khciState, endpoint, USB_IN, (uint8_t *)NULL, 0U);
                    return ;
                }
                else
                {
                }
            }
        }
        else
        {

            /* Send remaining data and terminate the token done interrupt service. */
            (void)USB_DeviceKhciSend(khciState, endpoint | (USB_IN << 0x07U),
                                     khciState->endpointState[index].transferBuffer, remainingLength);
            return ;
        }
    }
    else
    {
        if ((USB_CONTROL_ENDPOINT == endpoint) && (0U == length))
        {
            message.length = 0U;
            message.buffer = (uint8_t *)NULL;
        }
        else
        {
            if (0U == khciState->endpointState[index].stateUnion.stateBitField.dmaAlign)
            {
#if 0            
                uint8_t *buffer = (uint8_t *)USB_LONG_FROM_LITTLE_ENDIAN(
                   USB_KHCI_BDT_GET_ADDRESS((uint32_t)khciState->bdt, endpoint, USB_OUT,
                                             khciState->endpointState[index].stateUnion.stateBitField.bdtOdd));
#endif
                uint8_t *buffer = epmsg->buffer;
                uint8_t *transferBuffer =
                    khciState->endpointState[index].transferBuffer + khciState->endpointState[index].transferDone;
                if (buffer != transferBuffer)
                {
                    for (uint32_t i = 0U; i < length; i++)
                    {
                        transferBuffer[i] = buffer[i];
                    }
                }
                khciState->isDmaAlignBufferInusing = 0U;
            }
			
            /* The transferred length */
            khciState->endpointState[index].transferDone += length;
            /* Remaining length */
            remainingLength =
                khciState->endpointState[index].transferLength - khciState->endpointState[index].transferDone;

            if ((USB_CONTROL_ENDPOINT == endpoint) && isSetup)
            {
                khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.data0 = 1U;
                khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_IN].stateUnion.stateBitField.data0 = 1U;
            }
            else
            {
                khciState->endpointState[index].stateUnion.stateBitField.data0 ^= 1U;
            }
            khciState->endpointState[index].stateUnion.stateBitField.bdtOdd ^= 1U;
            if ((!khciState->endpointState[index].transferLength) || (!remainingLength) ||
                (khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize > length))
            {
                message.length = khciState->endpointState[index].transferDone;
                if (isSetup)
                {
                    message.buffer = epmsg->buffer; 
                }
                else
                {
					message.buffer = khciState->endpointState[index].transferBuffer;
					/*it didn't judage  bulk communicate mode, it will influence class hid send*/
					if(endpoint && khciState->transfertype != USB_ENDPOINT_BULK)
					{
					     memcpy(message.buffer, (uint8_t *)epmsg->buffer, epmsg->length);
					}
			
                }
                khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;
            }
            else
            {
                /* Receive remaining data and terminate the token done interrupt service. */

                USB_NOPS_2;

				/*debug use*/
				memcpy(khciState->endpointState[index].transferBuffer + khciState->endpointState[index].transferDone - length, epmsg->buffer, length);

                USB_DeviceKhciRecv(khciState, (endpoint) | (USB_OUT << 0x07U),
                                   khciState->endpointState[index].transferBuffer, remainingLength);
                return ;
            }
        }
    }

    message.isSetup = isSetup;
    message.code = (endpoint) | (uint8_t)(((uint32_t)direction << 0x07U));

    /* Notify the up layer the KHCI status changed. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
}

/*!
 * @brief Handle the USB bus reset interrupt.
 *
 * The function is used to handle the USB bus reset interrupt.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciInterruptReset(usb_device_khci_state_struct_t *khciState)
{
#if 0
    usb_device_callback_message_struct_t message;

    /* Set KHCI reset flag */
    khciState->isResetting = 1U;

    /* Clear the reset interrupt */
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptReset);
#if ((defined(USB_DEVICE_CONFIG_LOW_POWER_MODE)) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
    /* Clear the suspend interrupt */
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptSleep);
    khciState->registerBase->USBCTRL &= ~USB_USBCTRL_SUSP_MASK;
#endif

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyBusReset;
    message.length = 0U;
    message.isSetup = 0U;
    /* Notify up layer the USB bus reset signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#else
    usb_device_callback_message_struct_t message;

    /* Set KHCI reset flag */
    khciState->isResetting = 1U;

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyBusReset;
    message.length = 0U;
    message.isSetup = 0U;
    /* Notify up layer the USB bus reset signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#endif
}

/* The USB suspend and resume signals need to be detected and handled when the low power or remote wakeup function
 * enabled. */
#if (defined(USB_DEVICE_CONFIG_LOW_POWER_MODE) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))

/*!
 * @brief Handle the suspend interrupt.
 *
 * The function is used to handle the suspend interrupt when the suspend signal detected.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciInterruptSleep(usb_device_khci_state_struct_t *khciState)
{
#if 0
    usb_device_callback_message_struct_t message;

    /* Enable the resume interrupt */
    khciState->registerBase->INTEN |= kUSB_KhciInterruptResume;
    khciState->registerBase->USBTRC0 |= USB_USBTRC0_USBRESMEN_MASK;
    khciState->registerBase->USBCTRL |= USB_USBCTRL_SUSP_MASK;
    /* Disable the suspend interrupt */
    khciState->registerBase->INTEN &= ~((uint32_t)kUSB_KhciInterruptSleep);

    /* Clear the suspend interrupt */
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptSleep);
    /* Clear the resume interrupt */
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptResume);

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifySuspend;
    message.length = 0U;
    message.isSetup = 0U;

    /* Notify up layer the USB suspend signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#else
	
#endif
}

/*!
 * @brief Handle the resume interrupt.
 *
 * The function is used to handle the resume interrupt when the resume signal detected.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciInterruptResume(usb_device_khci_state_struct_t *khciState)
{
#if 0
    usb_device_callback_message_struct_t message;

    khciState->registerBase->USBCTRL &= ~USB_USBCTRL_SUSP_MASK;
    /* Enable the suspend interrupt */
    khciState->registerBase->INTEN |= kUSB_KhciInterruptSleep;
    /* Disable the resume interrupt */
    khciState->registerBase->INTEN &= ~((uint32_t)kUSB_KhciInterruptResume);
    khciState->registerBase->USBTRC0 &= ~USB_USBTRC0_USBRESMEN_MASK;

    /* Clear the resume interrupt */
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptResume);
    /* Clear the suspend interrupt */
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptSleep);

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyResume;
    message.length = 0U;
    message.isSetup = 0U;

    /* Notify up layer the USB resume signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#else
    usb_device_callback_message_struct_t message;

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyResume;
    message.length = 0U;
    message.isSetup = 0U;

    /* Notify up layer the USB resume signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#endif	
}
#endif /* USB_DEVICE_CONFIG_LOW_POWER_MODE */

#if (defined(USB_DEVICE_CONFIG_DETACH_ENABLE) && (USB_DEVICE_CONFIG_DETACH_ENABLE > 0U)) && \
    (defined(FSL_FEATURE_USB_KHCI_VBUS_DETECT_ENABLED) && (FSL_FEATURE_USB_KHCI_VBUS_DETECT_ENABLED > 0U))
/*!
 * @brief Handle the VBUS rising interrupt.
 *
 * The function is used to handle the VBUS rising interrupt when the VBUS rising signal detected.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciInterruptVbusRising(usb_device_khci_state_struct_t *khciState)
{
#if 0
    usb_device_callback_message_struct_t message;

    /* Disable the VBUS rising interrupt */
    khciState->registerBase->MISCCTRL &= ~USB_MISCCTRL_VREDG_EN_MASK;
    /* Enable the VBUS rising interrupt */
    khciState->registerBase->MISCCTRL |= USB_MISCCTRL_VREDG_EN_MASK;

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyAttach;
    message.length = 0U;
    message.isSetup = 0U;

    /* Notify up layer the USB VBUS rising signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#else
	
#endif
}

/*!
 * @brief Handle the VBUS falling interrupt.
 *
 * The function is used to handle the VBUS falling interrupt when the VBUS falling signal detected.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciInterruptVbusFalling(usb_device_khci_state_struct_t *khciState)
{
#if 0
    usb_device_callback_message_struct_t message;

    /* Disable the VBUS rising interrupt */
    khciState->registerBase->MISCCTRL &= ~USB_MISCCTRL_VFEDG_EN_MASK;
    /* Enable the VBUS rising interrupt */
    khciState->registerBase->MISCCTRL |= USB_MISCCTRL_VFEDG_EN_MASK;

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyDetach;
    message.length = 0U;
    message.isSetup = 0U;

    /* Notify up layer the USB VBUS falling signal detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#else
#endif
}
#endif /* USB_DEVICE_CONFIG_DETACH_ENABLE || FSL_FEATURE_USB_KHCI_VBUS_DETECT_ENABLED */

#if 0U
/*!
 * @brief Handle the sof interrupt.
 *
 * The function is used to handle the sof interrupt.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
void USB_DeviceKhciInterruptSof(usb_device_khci_state_struct_t *khciState)
{
    khciState->registerBase->ISTAT = (kUSB_KhciInterruptSofToken);

    khciState->registerBase->ISTAT = (kUSB_KhciInterruptResume);
}
#endif

/*!
 * @brief Handle endpoint stalled interrupt.
 *
 * The function is used to handle  endpoint stalled interrupt.
 *
 * @param khciState       Pointer of the device KHCI state structure.
 *
 */
static void USB_DeviceKhciInterruptStall(usb_device_khci_state_struct_t *khciState)
{
#if 0
    /* Clear the endpoint stalled interrupt flag */
    while (khciState->registerBase->ISTAT & (kUSB_KhciInterruptStall))
    {
        khciState->registerBase->ISTAT = (kUSB_KhciInterruptStall);
    }

    /* Un-stall the control in and out pipe when the control in or out pipe stalled. */
    if ((khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_IN].stateUnion.stateBitField.stalled) ||
        (khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.stalled))
    {
        USB_DeviceKhciEndpointUnstall(
            khciState, (USB_CONTROL_ENDPOINT | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)));
        USB_DeviceKhciEndpointUnstall(
            khciState, (USB_CONTROL_ENDPOINT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)));
    }
#else
    
    /* Un-stall the control in and out pipe when the control in or out pipe stalled. */
    if ((khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_IN].stateUnion.stateBitField.stalled) ||
        (khciState->endpointState[(USB_CONTROL_ENDPOINT << 1U) | USB_OUT].stateUnion.stateBitField.stalled))
    {
        USB_DeviceKhciEndpointUnstall(
            khciState, (USB_CONTROL_ENDPOINT | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)));
        USB_DeviceKhciEndpointUnstall(
            khciState, (USB_CONTROL_ENDPOINT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)));
    }
#endif
}

#if defined(USB_DEVICE_CONFIG_ERROR_HANDLING) && (USB_DEVICE_CONFIG_ERROR_HANDLING > 0U)
static void USB_DeviceKhciInterruptError(usb_device_khci_state_struct_t *khciState)
{
#if 0
    usb_device_callback_message_struct_t message;

    khciState->registerBase->ISTAT = (kUSB_KhciInterruptError);

    message.buffer = (uint8_t *)NULL;
    message.code = kUSB_DeviceNotifyError;
    message.length = 0U;
    message.isSetup = 0U;

    /* Notify up layer the USB error detected. */
    USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
#else
    
#endif
}
#endif /* USB_DEVICE_CONFIG_ERROR_HANDLING */

/*!
 * @brief Initialize the USB device KHCI instance.
 *
 * This function initizlizes the USB device KHCI module specified by the controllerId.
 *
 * @param controllerId The controller id of the USB IP. Please refer to enumeration type usb_controller_index_t.
 * @param handle        Pointer of the device handle, used to identify the device object is belonged to.
 * @param khciHandle   It is out parameter, is used to return pointer of the device KHCI handle to the caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceKhciInit(uint8_t controllerId,
                                usb_device_handle handle,
                                usb_device_controller_handle *khciHandle)
{
	usb_device_khci_state_struct_t *khciState;
    uint32_t khci_base[] = USB_BASE_ADDRS;

    if (((controllerId - kUSB_ControllerKhci0) >= (uint8_t)USB_DEVICE_CONFIG_KHCI) ||
        ((controllerId - kUSB_ControllerKhci0) >= (sizeof(khci_base) / sizeof(uint32_t))))
    {
        return kStatus_USB_ControllerNotFound;
    }
    khciState = &s_UsbDeviceKhciState[controllerId - kUSB_ControllerKhci0];

    khciState->controllerId = controllerId;

    khciState->pUSB_Otg = (USB_Otg_T *)khci_base[controllerId - kUSB_ControllerKhci0];

    khciState->dmaAlignBuffer = (uint8_t *)&s_UsbDeviceKhciDmaAlignBuffer[controllerId - kUSB_ControllerKhci0][0];    

#if (defined(USB_DEVICE_CONFIG_OTG) && (USB_DEVICE_CONFIG_OTG))
    khciState->otgStatus = 0U;
#else
    /* Disable the device functionality. */
    USB_DeviceKhciControl(khciState, kUSB_DeviceControlStop, NULL);
#endif

    /* Initialize USB device */
    HAL_USB_InitializeDevice(khciState->pUSB_Otg);

    /* Set KHCI device state to default value. */
    USB_DeviceKhciSetDefaultState(khciState);

    *khciHandle = khciState;
    khciState->deviceHandle = (usb_device_struct_t *)handle;
    
    return kStatus_USB_Success;
}

/*!
 * @brief De-initialize the USB device KHCI instance.
 *
 * This function de-initizlizes the USB device KHCI module.
 *
 * @param khciHandle   Pointer of the device KHCI handle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceKhciDeinit(usb_device_controller_handle khciHandle)
{
#if 0
    usb_device_khci_state_struct_t *khciState = (usb_device_khci_state_struct_t *)khciHandle;

    if (!khciHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    /* Clear all interrupt flags. */
    khciState->registerBase->ISTAT = 0xFFU;
    /* Disable all interrupts. */
    khciState->registerBase->INTEN &= ~(0xFFU);
    /* Clear device address. */
    khciState->registerBase->ADDR = (0U);

    /* Clear USB_CTL register */
    khciState->registerBase->CTL = 0x00U;
    khciState->registerBase->USBCTRL |= USB_USBCTRL_PDE_MASK | USB_USBCTRL_SUSP_MASK;
#else
	
#endif
    return kStatus_USB_Success;
}

/*!
 * @brief Send data through a specified endpoint.
 *
 * This function sends data through a specified endpoint.
 *
 * @param khciHandle      Pointer of the device KHCI handle.
 * @param endpointAddress Endpoint index.
 * @param buffer           The memory address to hold the data need to be sent.
 * @param length           The data length need to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the sending request is successful or not; the transfer done is notified by the
 * corresponding callback function.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */


usb_status_t USB_DeviceKhciSend(usb_device_controller_handle khciHandle,
                                uint8_t endpointAddress,
                                uint8_t *buffer,
                                uint32_t length)
{
    usb_device_khci_state_struct_t *khciState = (usb_device_khci_state_struct_t *)khciHandle;
    uint32_t index = ((endpointAddress & USB_ENDPOINT_NUMBER_MASK) << 1U) | USB_IN;
    usb_status_t error = kStatus_USB_Error;

    /* Save the tansfer information */
    if (0U == khciState->endpointState[index].stateUnion.stateBitField.transferring)
    {
        khciState->endpointState[index].transferDone = 0U;
        khciState->endpointState[index].transferBuffer = buffer;
        khciState->endpointState[index].transferLength = length;
        khciState->endpointState[index].stateUnion.stateBitField.dmaAlign = 1U;
    }


    khciState->endpointState[index].transferRemainLen = length;
    /* Data length needs to less than max packet size in each call. */
    if (length > khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize)
    {
        length = khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize;
    }

    if (endpointAddress & 0x0F)
    {
        khciState->endpointState[index].epxTransferredLength = length;
    }

    /* Send data when the device is not resetting. */
    if (0U == khciState->isResetting)
    {
        error = USB_DeviceKhciEndpointTransfer(khciState, endpointAddress & USB_ENDPOINT_NUMBER_MASK, USB_IN,
                                               (uint8_t *)((uint32_t)khciState->endpointState[index].transferBuffer +
                                                           (uint32_t)khciState->endpointState[index].transferDone),
                                               length);
    }

    /* Prime a transfer to receive next setup packet if the dat length is zero in a control in endpoint. */
    if ((0U == khciState->endpointState[index].transferDone) && (0U == length) &&
        (USB_CONTROL_ENDPOINT == (endpointAddress & USB_ENDPOINT_NUMBER_MASK)))
    {
        USB_DeviceKhciPrimeNextSetup(khciState);
    }
    
    return error;
}

/*!
 * @brief Receive data through a specified endpoint.
 *
 * This function Receives data through a specified endpoint.
 *
 * @param khciHandle      Pointer of the device KHCI handle.
 * @param endpointAddress Endpoint index.
 * @param buffer           The memory address to save the received data.
 * @param length           The data length want to be received.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The return value just means if the receiving request is successful or not; the transfer done is notified by the
 * corresponding callback function.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for one specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer could begin only when the previous transfer is done (get notification through the endpoint
 * callback).
 */
usb_status_t USB_DeviceKhciRecv(usb_device_controller_handle khciHandle,
                                uint8_t endpointAddress,
                                uint8_t *buffer,
                                uint32_t length)
{
    usb_device_khci_state_struct_t *khciState = (usb_device_khci_state_struct_t *)khciHandle;
    uint32_t index = ((endpointAddress & USB_ENDPOINT_NUMBER_MASK) << 1U) | USB_OUT;
    usb_status_t error = kStatus_USB_Error;

    if ((0U == length) && (USB_CONTROL_ENDPOINT == (endpointAddress & USB_ENDPOINT_NUMBER_MASK)))
    {
        khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;
        USB_DeviceKhciPrimeNextSetup(khciState);
    }
    else
    {
        /* Save the tansfer information */
        if (0U == khciState->endpointState[index].stateUnion.stateBitField.transferring)
        {
            khciState->endpointState[index].transferDone = 0U;
            khciState->endpointState[index].transferBuffer = buffer;
            khciState->endpointState[index].transferLength = length;
        }
        khciState->endpointState[index].stateUnion.stateBitField.dmaAlign = 1U;

        /* Data length needs to less than max packet size in each call. */
        if (length > khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize)
        {
            length = khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize;
        }

        buffer = (uint8_t *)((uint32_t)buffer + (uint32_t)khciState->endpointState[index].transferDone);

        if ((khciState->dmaAlignBuffer) && (0U == khciState->isDmaAlignBufferInusing) &&
            (USB_DEVICE_CONFIG_KHCI_DMA_ALIGN_BUFFER_LENGTH >= length) &&
            ((length & 0x03U) || (((uint32_t)buffer) & 0x03U)))
        {
            khciState->endpointState[index].stateUnion.stateBitField.dmaAlign = 0U;
            buffer = khciState->dmaAlignBuffer;
            khciState->isDmaAlignBufferInusing = 1U;
        }

        /* Receive data when the device is not resetting. */
        if (0U == khciState->isResetting)
        {
            error = USB_DeviceKhciEndpointTransfer(khciState, endpointAddress & USB_ENDPOINT_NUMBER_MASK, USB_OUT,
                                                   buffer, length);
        }
    }

    return error;
}

/*!
 * @brief Cancel the pending transfer in a specified endpoint.
 *
 * The function is used to cancel the pending transfer in a specified endpoint.
 *
 * @param khciHandle      Pointer of the device KHCI handle.
 * @param ep               Endpoint address, bit7 is the direction of endpoint, 1U - IN, abd 0U - OUT.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceKhciCancel(usb_device_controller_handle khciHandle, uint8_t ep)
{
	usb_device_khci_state_struct_t *khciState = (usb_device_khci_state_struct_t *)khciHandle;
    usb_device_callback_message_struct_t message;
    uint8_t index = ((ep & USB_ENDPOINT_NUMBER_MASK) << 1U) | ((ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >> USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT);

    /* Cancel the transfer and notify the up layer when the endpoint is busy. */
    if (khciState->endpointState[index].stateUnion.stateBitField.transferring)
    {
        message.length = USB_UNINITIALIZED_VAL_32;
        message.buffer = khciState->endpointState[index].transferBuffer;
        message.code = ep;
        message.isSetup = 0U;
        
        khciState->endpointState[index].stateUnion.stateBitField.transferring = 0U;
        USB_DeviceNotificationTrigger(khciState->deviceHandle, &message);
    }

    return kStatus_USB_Success;
}

/*!
 * @brief Control the status of the selected item.
 *
 * The function is used to control the status of the selected item.
 *
 * @param khciHandle      Pointer of the device KHCI handle.
 * @param type             The selected item. Please refer to enumeration type usb_device_control_type_t.
 * @param param            The param type is determined by the selected item.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceKhciControl(usb_device_controller_handle khciHandle, usb_device_control_type_t type, void *param)
{
	usb_device_khci_state_struct_t *khciState = (usb_device_khci_state_struct_t *)khciHandle;
    uint16_t *temp16;
    uint8_t *temp8;

    usb_status_t error = kStatus_USB_Error;

    if (!khciHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    switch (type)
    {
        case kUSB_DeviceControlRun:
            /*Initialize USB device*/
            HAL_USB_InitializeDevice(khciState->pUSB_Otg);
            
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceControlStop:
            /*Uninitialize USB device*/
            HAL_USB_DeInitializeDevice(khciState->pUSB_Otg);
            
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceControlEndpointInit:
            if (param)
            {
                error = USB_DeviceKhciEndpointInit(khciState, (usb_device_endpoint_init_struct_t *)param);
            }
            break;
        case kUSB_DeviceControlEndpointDeinit:
            if (param)
            {
                temp8 = (uint8_t *)param;
                error = USB_DeviceKhciEndpointDeinit(khciState, *temp8);
            }
            break;
        case kUSB_DeviceControlEndpointStall:
            if (param)
            {
                temp8 = (uint8_t *)param;
                error = USB_DeviceKhciEndpointStall(khciState, *temp8);
            }
            break;
        case kUSB_DeviceControlEndpointUnstall:
            if (param)
            {
                temp8 = (uint8_t *)param;
                error = USB_DeviceKhciEndpointUnstall(khciState, *temp8);
            }
            break;
        case kUSB_DeviceControlGetDeviceStatus:
            if (param)
            {
                temp16 = (uint16_t *)param;
                *temp16 = (USB_DEVICE_CONFIG_SELF_POWER << (USB_REQUEST_STANDARD_GET_STATUS_DEVICE_SELF_POWERED_SHIFT))
#if ((defined(USB_DEVICE_CONFIG_REMOTE_WAKEUP)) && (USB_DEVICE_CONFIG_REMOTE_WAKEUP > 0U))
                          | ((uint16_t)(((uint32_t)deviceHandle->remotewakeup)
                                        << (USB_REQUEST_STANDARD_GET_STATUS_DEVICE_REMOTE_WARKUP_SHIFT)))
#endif
                    ;
                error = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceControlGetEndpointStatus:
            if (param)
            {
                usb_device_endpoint_status_struct_t *endpointStatus = (usb_device_endpoint_status_struct_t *)param;

                if (((endpointStatus->endpointAddress) & USB_ENDPOINT_NUMBER_MASK) < USB_DEVICE_CONFIG_ENDPOINTS)
                {
                    endpointStatus->endpointStatus =
                        (uint16_t)(
                            khciState
                                ->endpointState[(((endpointStatus->endpointAddress) & USB_ENDPOINT_NUMBER_MASK) << 1U) |
                                                (((endpointStatus->endpointAddress) &
                                                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                                 USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)]
                                .stateUnion.stateBitField.stalled == 1U) ?
                            kUSB_DeviceEndpointStateStalled :
                            kUSB_DeviceEndpointStateIdle;
                    error = kStatus_USB_Success;
                }
            }
            break;
        case kUSB_DeviceControlSetDeviceAddress:
            if (param)
            {
                temp8 = (uint8_t *)param;
                //khciState->pUSB_Otg->Index = 0;
                HAL_USB_SetIndex(khciState->pUSB_Otg, 0);
                HAL_USB_SetAddress(khciState->pUSB_Otg, (*temp8));
                
                error = kStatus_USB_Success;
            }
            break;
        case kUSB_DeviceControlGetSynchFrame:
            break;
#if ((defined(USB_DEVICE_CONFIG_LOW_POWER_MODE)) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
#if defined(USB_DEVICE_CONFIG_REMOTE_WAKEUP) && (USB_DEVICE_CONFIG_REMOTE_WAKEUP > 0U)
        case kUSB_DeviceControlResume:
            khciState->registerBase->CTL |= USB_CTL_RESUME_MASK;
            startTick = deviceHandle->hwTick;
            while ((deviceHandle->hwTick - startTick) < 10)
            {
                __ASM("nop");
            }
            khciState->registerBase->CTL &= ~USB_CTL_RESUME_MASK;
            error = kStatus_USB_Success;
            break;
#endif /* USB_DEVICE_CONFIG_REMOTE_WAKEUP */
        case kUSB_DeviceControlSuspend:
            error = kStatus_USB_Success;
            break;
#endif /* USB_DEVICE_CONFIG_LOW_POWER_MODE */
        case kUSB_DeviceControlSetDefaultStatus:
            for (uint8_t count = 0U; count < USB_DEVICE_CONFIG_ENDPOINTS; count++)
            {
                USB_DeviceKhciEndpointDeinit(khciState, (count | (USB_IN << 0x07U)));
                USB_DeviceKhciEndpointDeinit(khciState, (count | (USB_OUT << 0x07U)));
            }
            USB_DeviceKhciSetDefaultState(khciState);
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceControlGetSpeed:
            if (param)
            {
                temp8 = (uint8_t *)param;
                *temp8 = USB_SPEED_FULL;
                error = kStatus_USB_Success;
            }
            break;
            
#if (defined(USB_DEVICE_CONFIG_OTG) && (USB_DEVICE_CONFIG_OTG))
        case kUSB_DeviceControlGetOtgStatus:
            *((uint8_t *)param) = khciState->otgStatus;
            break;
        case kUSB_DeviceControlSetOtgStatus:
            khciState->otgStatus = *((uint8_t *)param);
            break;
#endif
        case kUSB_DeviceControlSetTestMode:
            break;

        default:
            break;
    }
    return error;
}

/*!
 * @brief Handle the KHCI device interrupt.
 *
 * The function is used to handle the KHCI device interrupt.
 *
 * @param deviceHandle    The device handle got from USB_DeviceInit.
 *
 */
void USB_DeviceKhciIsrFunction(void *deviceHandle)
{
    usb_device_struct_t *handle = (usb_device_struct_t *)(*(uint32_t *)deviceHandle);
    usb_device_khci_state_struct_t *khciState;
    uint8_t status;
    endpoint_message epmsg;

    uint16_t ep_intr = 0;
    uint16_t epint = 0;
    uint8_t epnum = 0;
    uint16_t cnt = 0;
	  uint8_t index = 0;

    if (NULL == deviceHandle)
    {
        return;
    }

    khciState = (usb_device_khci_state_struct_t *)(handle->controllerHandle);

    /*IntrUSB register is cleared automatically after CPU-reading*/
    status = HAL_USB_GetDeviceIntrStatus(khciState->pUSB_Otg);

    if ((status & kUSB_KhciInterruptSuspend) == kUSB_KhciInterruptSuspend)
    {
        /*Do nothing here!!!*/
    }
    
#if (defined(USB_DEVICE_CONFIG_LOW_POWER_MODE) && (USB_DEVICE_CONFIG_LOW_POWER_MODE > 0U))
    if ((status & kUSB_KhciInterruptResume) == kUSB_KhciInterruptResume)
    {
        /*Do nothing here!!!*/
        USB_DeviceKhciInterruptResume(khciState);
    }
#endif /* USB_DEVICE_CONFIG_LOW_POWER_MODE */

    if ((status & kUSB_KhciInterruptReset) == kUSB_KhciInterruptReset)
    {        
        USB_DeviceKhciInterruptReset(khciState);
    }

    /* Handle interrupts for endpoint */  

    /* Read in the device interrupt bits, only for In Endpoint*/
    ep_intr = USB_ReadDevAllInEpInterrupt(khciState->pUSB_Otg);

    epnum = 0U;

    while (ep_intr)
    {
        if (ep_intr & 0x1U) /* In ITR */
        {
            epint = HAL_USB_ReadDevInEPInterrupt(khciState->pUSB_Otg, epnum);
            if (epnum == 0)
            {
                if ((epint & USB_OTG_CSR0_SENTSTALL) == USB_OTG_CSR0_SENTSTALL)
                {
                    //HAL_USB_ClearEP0SentStall(khciState->pUSB_Otg);
                    HAL_USB_ClearCSR0(khciState->pUSB_Otg, USB_OTG_CSR0_SENTSTALL);
					
                    USB_DeviceKhciInterruptStall(khciState);
                    direction_need_switch = 0;
                }

                /*SetupEnd flag*/
                if ((epint & USB_OTG_CSR0_SETUPEND) == USB_OTG_CSR0_SETUPEND)
                {                
                    //HAL_USB_ClearEP0SetupEnd(khciState->pUSB_Otg);
                    HAL_USB_SetCSR0(khciState->pUSB_Otg, USB_OTG_CSR0_SERVSETUPEND);
                }

                if (!(epint & USB_OTG_CSR0_RXPKTRDY))
                {                             
                    /*Set address for USB device*/
                    if(((usb_device_struct_t *)khciState->deviceHandle)->deviceAddress != 0xFF && ((usb_device_struct_t *)khciState->deviceHandle)->state == kUSB_DeviceStateAddressing)
                    {
                        HAL_USB_SetAddress(khciState->pUSB_Otg, ((usb_device_struct_t *)khciState->deviceHandle)->deviceAddress);
                        ((usb_device_struct_t *)khciState->deviceHandle)->state =  kUSB_DeviceStateAddress;
                    }
                }

                /*handle the interrupt by data_transfer completed !!*/
                 if (direction_need_switch)
                 {
					epmsg.ep = USB_CONTROL_ENDPOINT | USB_IN << 7;
					epmsg.isSetup = 1;
					index = (uint8_t)(USB_CONTROL_ENDPOINT << 1U) | (uint8_t)USB_IN;

					if(khciState->endpointState[index].transferRemainLen > khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize)
					{
					    epmsg.length=khciState->endpointState[index].stateUnion.stateBitField.maxPacketSize;
					}
					else
					{
					    epmsg.length=khciState->endpointState[index].transferRemainLen;
					    direction_need_switch = 0;
					}
					USB_DeviceKhciInterruptTokenDone(khciState, &epmsg);
                }

                /*RxPktRdy flag*/
                if ((epint & USB_OTG_CSR0_RXPKTRDY) == USB_OTG_CSR0_RXPKTRDY)
                {
                    /*An datapacket is received, maybe should retrive data from FIFO*/
                    /*Assume that endpoint 0 only receive SETUP packet and 
                     will reject other packet whose length is less or more than 8 bytes*/
                    HAL_USB_ReadPacket(khciState->pUSB_Otg, (uint8_t *)epmsg.buffer, 0, 8U);
                    /*Clear the flag after reading the fifo*/
                    HAL_USB_ClearEndpointRxPktRdy(khciState->pUSB_Otg, 0);

                    epmsg.ep = USB_CONTROL_ENDPOINT |  USB_OUT << 7;                    
                    epmsg.isSetup = 1;
                    epmsg.length = 8U;

                    direction_need_switch = 1;
                    USB_DeviceKhciInterruptTokenDone(khciState, &epmsg);
              }                            
          }
          /*Tx Endpoints other than control endpoint*/
          else
          {
              direction_need_switch = 0;
              
              /*UnderRun flag*/
              if ((epint & USB_OTG_TXCSR1_UNDERRUN) == USB_OTG_TXCSR1_UNDERRUN)
              {
                  //HAL_USB_ClearEPxTxUnderRun(khciState->pUSB_Otg);
				  HAL_USB_ClearTxCSR1(khciState->pUSB_Otg, USB_OTG_TXCSR1_UNDERRUN);
              }
              
              /*FIFO Not Empty*/
              if ((epint & USB_OTG_TXCSR1_FIFONOTEMPTY) == USB_OTG_TXCSR1_FIFONOTEMPTY)
              {
                  /*Do nothing here !!!*/
              }

              if ((epint & USB_OTG_TXCSR1_SENTSTLL) != USB_OTG_TXCSR1_SENTSTLL)
              {
                  index = ((epnum & USB_ENDPOINT_NUMBER_MASK) << 1U) | USB_IN;                 

                  epmsg.ep = epnum | USB_IN << 7;
                  epmsg.isSetup = 0;
                  epmsg.length = khciState->endpointState[index].epxTransferredLength;
                 
                  USB_DeviceKhciInterruptTokenDone(khciState, &epmsg);
              }

              /*SentStall flag*/
              if ((epint & USB_OTG_TXCSR1_SENTSTLL) == USB_OTG_TXCSR1_SENTSTLL)
              {
                  /*
                   *Do nothing here, just leave SENTSTALL as it was till the endpoint
                   *is re-enabled to use
                   */
              }
           }
        }

        epnum++;
        ep_intr >>= 1U;
    }

    /*Interrupt for Rx Endpoints*/
    epnum = 0U;    
    /* Read in the device interrupt bits */
    ep_intr = USB_ReadDevAllOutEpInterrupt(khciState->pUSB_Otg);

    while ( ep_intr )
    {
        if (ep_intr & 0x1U)
        {
            direction_need_switch = 0;
            
            /*Actually OutEpoint can not handle endpoint 0, here*/
            epint = HAL_USB_ReadDevOutEPInterrupt(khciState->pUSB_Otg, epnum);

            /*Endpoint 0 has different senses from Endpoint 1 ~ 15*/
            if (epnum != 0)
            {                             
                /*FIFOFULL flag*/
                if ((epint & USB_OTG_RXCSR1_FIFOFULL) == USB_OTG_RXCSR1_FIFOFULL)
                {
                    //HAL_USB_ClearEPxRxFIFOFull(khciState->pUSB_Otg);
					HAL_USB_ClearRxCSR1(khciState->pUSB_Otg, USB_OTG_RXCSR1_FIFOFULL);
                }

                /*OVERRUN flag*/
                if ((epint & USB_OTG_RXCSR1_OVERRUN) == USB_OTG_RXCSR1_OVERRUN)
                {
                    //HAL_USB_ClearEPxRxOverRun(khciState->pUSB_Otg);
                    HAL_USB_ClearRxCSR1(khciState->pUSB_Otg, USB_OTG_RXCSR1_OVERRUN);
                }

                /*Dataerr flag*/
                if ((epint & USB_OTG_RXCSR1_DATAERR) == USB_OTG_RXCSR1_DATAERR)
                {
                    /*Only valid for ISO transfer*/
                }

                /*SentStall flag*/
                if ((epint & USB_OTG_RXCSR1_SENTSTLL) == USB_OTG_RXCSR1_SENTSTLL)
                {                    
                    /*
                     *Do nothing here, just leave SENTSTALL as it was till the endpoint
                     *is re-enabled to use
                     */
                }

                
              	if ((epint & USB_OTG_RXCSR1_RXPKTRDY) == USB_OTG_RXCSR1_RXPKTRDY)
        		{
        	        //cnt = (khciState->pUSB_Otg->RxCount2) << 8 | khciState->pUSB_Otg->Count_Union.RxCount1;
        	        cnt = HAL_USB_GetOutEpFifoCnt(khciState->pUSB_Otg);
                    HAL_USB_ReadPacket(khciState->pUSB_Otg, (uint8_t *)epmsg.buffer, epnum, cnt);
					
				    //memcpy((uint8_t *)epmsgx.buffer, (uint8_t *)epmsg.buffer, cnt);
				    //epmsgx.length = cnt;
				
        	        /*Clear the interrupt flag*/
        	        HAL_USB_ClearEndpointRxPktRdy(khciState->pUSB_Otg, epnum);

                    epmsg.ep = epnum |  USB_OUT << 7;
                    epmsg.isSetup = 0;
                    epmsg.length = cnt;

                    USB_DeviceKhciInterruptTokenDone(khciState, &epmsg);
        		}
            }
        }

        epnum++;
        ep_intr >>= 1U;
    } /*end of while*/          

    if ((status & kUSB_KhciInterruptSofToken) == kUSB_KhciInterruptSofToken)
    {
        /*Do nothing here!!!*/
#if 0
        USB_DeviceKhciInterruptSof(khciState);
#endif
    }

}
#ifndef RABBIT_NO_OS_SYSTEM  /*cmt*/

static int USB_Device_StackInit(struct device *dev)
{
	ARG_UNUSED(dev);
	
	/*Set callback function*/
    HAL_USB_SetIrqCallback((USB_Device_Irq_Callback_t) USB_DeviceKhciIsrFunction, NULL);
	
	return 0;
}

void USB_Device_SetStackParam(void *param)
{
    /*Set parameter for USB interrupt use*/    
    HAL_USB_SetIrqCallback(NULL, param);
    
    /*Clear corresponding interrupt for USB*/
    NVIC_ClearPendingIRQ(USB_IRQn);

	  extern void HAL_USB_IRQHandler(void *param);
    /*register usbd interrupt handler*/
    IRQ_CONNECT(USB_IRQn, 1, HAL_USB_IRQHandler, NULL, 0);

    /*enable usb interrupt*/
    irq_enable(USB_IRQn);
}

#if (defined(CONFIG_USB_VIDEO) || defined(CONFIG_USB_MASS_STORAGE) || defined (CONFIG_USB_AUDIO) || \
     defined(CONFIG_USB_HID_CUSTOM) || defined(CONFIG_USB_HID_MOUSE))

#define USB_OTG_NAME "gm-usb"
DEVICE_AND_API_INIT(usb_otg, USB_OTG_NAME,  &USB_Device_StackInit,   NULL, NULL,  APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, NULL);

#endif


#endif /*#if 0 cmt end*/
#endif /* USB_DEVICE_CONFIG_KHCI */
