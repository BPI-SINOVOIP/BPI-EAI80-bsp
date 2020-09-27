/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_hal_usb.c
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 * 
 * @date          2018/11/01 
 *    
 * @brief         Implementation of USB OTG Driver.
 * 
 * @note
 *    2018/11/01, wizephen.wang, V1.0.0
 *        Initial version.
 *    
 */

#include <gm_soc.h>
#include <gm_irq.h>
#include <gm_hal_usb.h>
#include "gm_ll_usb_dev.h"
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>


#define USB_ENDPOINT_NUMBER_MASK (0x0FU)

#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK (0x80U)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT (7U)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT (0U)
#define USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN (0x80U)

#define USB_CONTROL_ENDPOINT (0U)

#define USB_OTG_AS_DEVICE   (*(volatile uint8_t *)(0x50000210) = 0x1F)
#define USB_OTG_AS_HOST     (*(volatile uint8_t *)(0x50000210) = 0x1C)


USB_IrqHandler_Callback_Struct_T s_IrqCallback;


static inline void HAL_USB_DoubleFlushTxFifo(USB_Otg_T *pUSB_Otg)
{
    /*In case of double fifo is used*/
    HAL_USB_SetTxCSR1(pUSB_Otg, USB_OTG_TXCSR1_FLUSHFIFO);
    HAL_USB_SetTxCSR1(pUSB_Otg, USB_OTG_TXCSR1_FLUSHFIFO);
}

static inline void HAL_USB_DoubleFlushRxFifo(USB_Otg_T *pUSB_Otg)
{
    /*In case of double fifo is used*/
    HAL_USB_SetRxCSR1(pUSB_Otg, USB_OTG_RXCSR1_FLUSHFIFO);
    HAL_USB_SetRxCSR1(pUSB_Otg, USB_OTG_RXCSR1_FLUSHFIFO);    
}

/*
 * Endpoint 0 Tx & Rx: Fixed 64bytes
 * Endpoint 1 Tx & Rx: Fixed 128bytes
 * Endpoint 2 Tx & Rx: Fixed 128bytes
 * Endpoint 3 Tx & Rx: Fixed 128bytes
 * Endpoint 0 Occupied 64 * 2 bytes by USB IP pre-designed
 * Endpoint 1 ~ 3 Occupied 128 * 2 bytes dynamic allocation
 */

#define DYNAMIC_FIFOSIZEOF_TXENDPOINT_X  128
#define DYNAMIC_FIFOSIZEOF_RXENDPOINT_X  128
#define DYNAMIC_TXRXENDPOINT_OFFSET      128
#define DYNAMIC_MAXPKT_SIZE              3

void HAL_USB_SetDynamicSize(USB_Otg_T *pUSB_Otg)
{
    uint8_t ep;
    uint16_t addr = 0;
    addr += DYNAMIC_TXRXENDPOINT_OFFSET;
    for (ep = 1; ep < 4; ep ++)
    {           
        /*Set index register*/
        pUSB_Otg->Index = ep;
        /*Set Tx FIFO size for endpoint ep*/
        pUSB_Otg->DyTxFIFO1 = (addr / 8) & 0xFF;
        /*Double Packet Disabled*/
        pUSB_Otg->DyTxFIFO2 = ((addr / 8) >> 8 & 0x0F) | (DYNAMIC_MAXPKT_SIZE << 5);
        
        addr += DYNAMIC_FIFOSIZEOF_TXENDPOINT_X;

        /*Set Rx FIFO size for endpoint ep*/
        pUSB_Otg->DyRxFIFO1 = (addr / 8) & 0xFF;
        /*Double Packet Disabled*/
        pUSB_Otg->FIFO_Union.DyRxFIFO2 = ((addr / 8) >> 8 & 0x0F) | (DYNAMIC_MAXPKT_SIZE << 5);
        addr += DYNAMIC_FIFOSIZEOF_RXENDPOINT_X;
    }
}

void HAL_USB_FlushTxFifo (USB_Otg_T *pUSB_Otg, uint32_t num )
{
    uint8_t ep = 1;

    /*Flush the FIFO of Tx Endpoints, where num is the numeric of endpoint*/
    if (num == 16)
    {
        /*Only for Tx Endpoint, not for Endpoint 0*/
        for (ep = 1; ep < num; ep ++)
        {
            pUSB_Otg->Index = ep;
            HAL_USB_DoubleFlushTxFifo(pUSB_Otg);
        }

        /*Finally flush the endpoint 0 FIFO, but it has no effect while TxPktRdy or RxPktRdy  is not set*/
        pUSB_Otg->Index = 0;
        pUSB_Otg->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
    }
    else if (num != 0)
    {
        /*Indexed for a dedicated Endpoint*/
        pUSB_Otg->Index = num;
        HAL_USB_DoubleFlushTxFifo(pUSB_Otg);
    }
    else if (num == 0)
    {
        pUSB_Otg->Index = num;
        pUSB_Otg->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
    }  
}


void HAL_USB_FlushRxFifo(USB_Otg_T *pUSB_Otg, uint32_t num)
{
    uint8_t ep = 1;

    /*Flush the FIFO of Rx Endpoints, where num is the numeric of endpoint*/
    if (num == 16)
    {
        /*Only for Rx Endpoint, not for Endpoint 0*/
        for (ep = 1; ep < num; ep ++)
        {
            pUSB_Otg->Index = ep; 
            HAL_USB_DoubleFlushRxFifo(pUSB_Otg);
        }

        /*Finally flush the endpoint 0 FIFO, but it has no effect while TxPktRdy or RxPktRdy  is not set*/
        pUSB_Otg->Index = 0;
        pUSB_Otg->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
    }
    else if (num != 0)
    {
        /*Indexed for a dedicated Endpoint*/
        pUSB_Otg->Index = num;
        HAL_USB_DoubleFlushRxFifo(pUSB_Otg);
    }
    else if (num == 0)
    {
        pUSB_Otg->Index = num;
        pUSB_Otg->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
    }
}

void HAL_USB_ClearEndpointRxPktRdy(USB_Otg_T *pUSB_Otg, uint8_t ep)
{
    /*Indexed for a dedicated Endpoint*/
    pUSB_Otg->Index = ep & 0x0F;

    if (ep == 0)
    {
        pUSB_Otg->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_SERVRXPKTRDY;
    }
    else
    {
        pUSB_Otg->RxCSR1 &= ~(USB_OTG_RXCSR1_RXPKTRDY);
    }
}

void HAL_USB_SetEndpointxTxPktRdy(USB_Otg_T *pUSB_Otg, uint8_t ep, uint8_t dataend)
{
    pUSB_Otg->Index = ep & 0x0F;

    /* Control Endpoint 0 */
    if ((ep & 0x0F) == 0)
    {
        if (dataend)
        {
            /*it's the end of transfer*/
            pUSB_Otg->CtrlSts0_Union.CSR0 |= (USB_OTG_CSR0_TXPKTRDY | USB_OTG_CSR0_DATAEND);
        }
        else
        {
            /*trigger to send data*/
            pUSB_Otg->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_TXPKTRDY;
        }
    }
    /*Tx Endpoints x other than Endpoint 0*/
    else
    {
        /* trigger to send data, now it will trigger next interrupt */
        pUSB_Otg->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_TXPKTRDY;
    }
}

void HAL_USB_WritePacket(USB_Otg_T *pUSB_Otg, uint8_t *src, uint8_t ep, uint16_t len)
{
    uint16_t i = 0U;

    /* Write index register */ 
    pUSB_Otg->Index = ep & 0x0F;

    for (i = 0U; i < len; i++, src += 1U)
    {   
       /*write data to the FIFO*/
       pUSB_Otg->FIFOx[ep * 4] = *((uint8_t *)src);
    }
}

uint8_t HAL_USB_ReadPacket(USB_Otg_T *pUSB_Otg, uint8_t *dest, uint8_t ep, uint16_t len)
{
    uint16_t i=0U; 
    uint8_t error = 0;
    
    /* Write index register */ 
    pUSB_Otg->Index = ep & 0x0F;

    if (len != ((pUSB_Otg->RxCount2) << 8 | pUSB_Otg->Count_Union.RxCount1))
    {
        error = 1;
        goto err;
    }

    for ( i = 0U; i < len; i++, dest += 1U )
    {
        *(uint8_t *)dest = pUSB_Otg->FIFOx[ep * 4];
    }
    
err:
    return error;
}

#ifdef CONFIG_GM_USB_DEVICE_STACK
void HAL_USB_InitializeDevice(USB_Otg_T *pUSB_Otg)
{   
    /*PLL_I2S enable */
    HAL_CLOCK_ClockCmd(CLOCK_PLLI2S_MASK,SYS_PLLI2S_EN_MASK);
    /*USB clock enable*/
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_USB);
    /* Release USB reset */
    HAL_Reset_Module(RESET_USB_SW_RSTJ);

    /*Configure as USB device*/
    USB_OTG_AS_DEVICE;

    HAL_USB_FlushTxFifo(pUSB_Otg, 0x10);
    HAL_USB_FlushRxFifo(pUSB_Otg, 0x10);

    USB_CLEAR_AND_DISABLE_INTERRUPTS;

    /*Interrupts for USB Device or Host, now only implement that for Device*/
    pUSB_Otg->IntrUSBE |= (USB_OTG_INTRUSBE_SUSPEND | USB_OTG_INTRUSBE_RESUME | USB_OTG_INTRUSBE_RSTBABL |\
                           USB_OTG_INTRUSBE_SOF | USB_OTG_INTRUSBE_CON | USB_OTG_INTRUSBE_DISCON |\
                           USB_OTG_INTRUSBE_SESSION | USB_OTG_INTRUSBE_VBUSERR);

    /*just for debugging*/
    pUSB_Otg->InTrTx1E |= 0x0F;
    pUSB_Otg->InTrRx1E |= 0x0F;
    pUSB_Otg->IntrUSBE = 0xFF;
    
    /*Set dynamic size for USB FIFO*/
    HAL_USB_SetDynamicSize(pUSB_Otg);
}
#endif

#ifdef CONFIG_GM_USB_HOST_STACK
void HAL_USB_InitializeDevice(USB_Otg_T *pUSB_Otg)
{  
    /*PLL_I2S enable */
    HAL_CLOCK_ClockCmd(CLOCK_PLLI2S_MASK,SYS_PLLI2S_EN_MASK);
    /*USB clock enable*/
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_USB);
      /* Release USB reset */
    HAL_Reset_Module(RESET_USB_SW_RSTJ);
    
    HAL_USB_FlushTxFifo(pUSB_Otg, 0x10);
    HAL_USB_FlushRxFifo(pUSB_Otg, 0x10);

    /*Configure as USB host*/
    USB_OTG_AS_HOST;

    USB_CLEAR_AND_DISABLE_INTERRUPTS;
    
    /*Interrupts for USB Device or Host, now only implement that for Device*/
    pUSB_Otg->IntrUSBE |= (USB_OTG_INTRUSBE_RESUME | USB_OTG_INTRUSBE_RSTBABL |\
                       USB_OTG_INTRUSBE_SOF | USB_OTG_INTRUSBE_CON | USB_OTG_INTRUSBE_DISCON);
    
    /*just for debugging*/
    pUSB_Otg->InTrTx1E |= 0x0F;
    pUSB_Otg->InTrRx1E |= 0x0F;

    /*Set dynamic size for USB FIFO*/
    HAL_USB_SetDynamicSize(pUSB_Otg);

    /*Start a session*/
    pUSB_Otg->DevCtl |= USB_OTG_DEVCTL_SESSION;    
}
#endif

void HAL_USB_DeInitializeDevice(USB_Otg_T *pUSB_Otg)
{
    USB_CLEAR_AND_DISABLE_INTERRUPTS;
    
    /*Disable corresponding USB interrupt flags*/
    pUSB_Otg->IntrUSBE &= ~(USB_OTG_INTRUSBE_SUSPEND | USB_OTG_INTRUSBE_RESUME | USB_OTG_INTRUSBE_RSTBABL |\
                            USB_OTG_INTRUSBE_SOF | USB_OTG_INTRUSBE_CON | USB_OTG_INTRUSBE_DISCON |\
                            USB_OTG_INTRUSBE_SESSION | USB_OTG_INTRUSBE_VBUSERR);
}


void HAL_USB_EnableEndpoints(USB_Otg_T *pUSB_Otg, uint8_t ep)
{

    uint8_t endpoint = (ep & USB_ENDPOINT_NUMBER_MASK);
    uint8_t direction = (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;

    pUSB_Otg->Index = endpoint;

    if (direction == 1U)
    {
        /*Configure the endpoint as In endpoint*/
        pUSB_Otg->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_MODE;

        /*Enable the endpoint's interrupt of Tx endpoint*/
        if (endpoint > 7)
        {
            pUSB_Otg->InTrTx2E |= 1 << (endpoint - 7);
        }
        else
        {
           pUSB_Otg->InTrTx1E |= 1 << (endpoint);
        }
    }
    else
    {   
        /*Configure the endpoint as Out endpoint*/
        pUSB_Otg->CtrlSts1_Union.TxCSR2 &= ~(USB_OTG_TXCSR2_MODE);

        /*Enable the endpoint's interrupt of Rx endpoint*/
        if (endpoint > 7)
        {
            pUSB_Otg->InTrRx2E |= 1 << (endpoint - 7);
        }
        else
        {
           pUSB_Otg->InTrRx1E |= 1 << (endpoint);
        }        
    }
}

void HAL_USB_DisableEndpoints(USB_Otg_T *pUSB_Otg, uint8_t ep)
{
#if 0
    /*not implemented for the time being ...*/
#endif    
}

void HAL_USB_EndpointsStall(USB_Otg_T *pUSB_Otg, uint8_t ep)
{    
    uint8_t endpoint = (ep & USB_ENDPOINT_NUMBER_MASK);
    uint8_t direction = (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;

    pUSB_Otg->Index = endpoint;

    /*Control Endpoint*/
    if (USB_CONTROL_ENDPOINT != endpoint)
    {
        if (direction == 1)
        {
            pUSB_Otg->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_SENDSTLL;
        }
        else
        {
            pUSB_Otg->RxCSR1 |= USB_OTG_RXCSR1_SENDSTLL;
        }
    }
    else
    {
        pUSB_Otg->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_SENDSTALL; // | USB_OTG_CSR0_SERVRXPKTRDY;
    }
}

void HAL_USB_EndpointsUnstall(USB_Otg_T *pUSB_Otg, uint8_t ep)
{    
    uint8_t endpoint = (ep & USB_ENDPOINT_NUMBER_MASK);
    uint8_t direction = (ep & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT;

    pUSB_Otg->Index = endpoint;

    if (endpoint != USB_CONTROL_ENDPOINT)
    {
        if (direction == 1U)
        {
            /*Send STALL condition when as a Tx endpoint*/
            pUSB_Otg->CtrlSts0_Union.TxCSR1 &= ~(USB_OTG_TXCSR1_SENTSTLL);
            //if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
            {
                pUSB_Otg->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_CLRDATATOG; /* DATA0 */
            }
        }
        else
        {
            /*Send STALL condition when as a RX endpoint*/
            pUSB_Otg->RxCSR1 &= ~(USB_OTG_RXCSR1_SENTSTLL);
            //if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
            {
                pUSB_Otg->RxCSR1 |= USB_OTG_RXCSR1_CLRDATATOG; /* DATA0 */
            }
        }
    }
    else
    {
        pUSB_Otg->CtrlSts0_Union.CSR0&= ~(USB_OTG_CSR0_SENTSTALL);
    }
}

/**
  * @brief  Returns Device OUT EP Interrupt register
  * @param  USBx : Selected device
  * @param  epnum : endpoint number
  *         This parameter can be a value from 0 to 3
  * @retval Device OUT EP Interrupt register
  */
uint32_t HAL_USB_ReadDevOutEPInterrupt (USB_Otg_T *pUSB_Otg, uint8_t ep)
{
    uint16_t intr = 0;

    pUSB_Otg->Index = ep;
    /*Control Endpoint is not handled here!!!*/
    if (ep != 0)
    {
        intr = (pUSB_Otg->RxCSR2) << 8 | pUSB_Otg->RxCSR1;
    }

    return intr;
}

/**
  * @brief  Returns Device IN EP Interrupt register
  * @param  USBx : Selected device
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 3
  * @retval Device IN EP Interrupt register
  */
uint32_t HAL_USB_ReadDevInEPInterrupt (USB_Otg_T *pUSB_Otg, uint8_t ep)
{
    uint16_t intr;

    pUSB_Otg->Index = ep;
    /*Control Endpoint is handled here!!!*/
    if (ep == 0)
    {
        intr = pUSB_Otg->CtrlSts0_Union.CSR0;
    }
    else
    {
        intr = (pUSB_Otg->CtrlSts1_Union.TxCSR2) << 8 | pUSB_Otg->CtrlSts0_Union.TxCSR1;
    }

    return intr;
}

/*The USB Interrupt Handler is acutally implemented in usb stack (Middleware Layer)*/
void HAL_USB_IRQHandler(void)
{
    /*do nothing here !*/   
    if ((s_IrqCallback.callbackFn != NULL) && (s_IrqCallback.callbackParam !=NULL ))
    {
        s_IrqCallback.callbackFn(s_IrqCallback.callbackParam);
    }
}

/*Set callback and its parameters for callback*/
void HAL_USB_SetIrqCallback(USB_Device_Irq_Callback_t callbackFn, void *callbackParam)
{
    if (callbackFn != NULL)
    {
        s_IrqCallback.callbackFn = callbackFn;
    }
    
    if (callbackParam != NULL)
    {
        s_IrqCallback.callbackParam = callbackParam;
    }
}
