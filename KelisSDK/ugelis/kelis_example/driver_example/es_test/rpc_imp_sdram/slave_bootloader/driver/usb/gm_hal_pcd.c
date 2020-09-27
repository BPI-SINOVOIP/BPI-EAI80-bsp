/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pcd.c
  * @author  MCD Application Team
  * @version V1.7.1
  * @date    14-April-2017
  * @brief   PCD HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the USB Peripheral Controller:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
    [..]
      The PCD HAL driver can be used as follows:

     (#) Declare a PCD_HandleTypeDef handle structure, for example:
         PCD_HandleTypeDef  hpcd;

     (#) Fill parameters of Init structure in HCD handle

     (#) Call HAL_PCD_Init() API to initialize the PCD peripheral (Core, Device core, ...)

     (#) Initialize the PCD low level resources through the HAL_PCD_MspInit() API:
         (##) Enable the PCD/USB Low Level interface clock using
              (+++) __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
              (+++) __HAL_RCC_USB_OTG_HS_CLK_ENABLE(); (For High Speed Mode)

         (##) Initialize the related GPIO clocks
         (##) Configure PCD pin-out
         (##) Configure PCD NVIC interrupt

     (#)Associate the Upper USB device stack to the HAL PCD Driver:
         (##) hpcd.pData = pdev;

     (#)Enable PCD transmission and reception:
         (##) HAL_PCD_Start();

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <gm_soc.h>
#include <gm_hal_pcd.h>
//#include "usbd_def.h"
//#include "gm_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup PCD PCD
  * @brief PCD HAL module driver
  * @{
  */

#if 1 //def HAL_PCD_MODULE_ENABLED
#if 1
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup PCD_Private_Macros PCD Private Macros
  * @{
  */
#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))
/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup PCD_Private_Functions PCD Private Functions
  * @{
  */
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */

/** @defgroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
            ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and initialize the associated handle.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
    uint32_t i = 0U;

    /* Check the PCD handle allocation */
    if (hpcd == NULL)
    {
        return HAL_ERROR;
    }

    /* Check the parameters */
    //assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

    hpcd->State = HAL_PCD_STATE_BUSY;

    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_PCD_MspInit(hpcd);

    /* Disable the Interrupts */
    __HAL_PCD_DISABLE(hpcd);

    /*Init the Core (common init.) */
    USB_CoreInit(hpcd->Instance, hpcd->Init);

    /* Force Device Mode*/
    USB_SetCurrentMode(hpcd->Instance, USB_OTG_DEVICE_MODE);

    /* Init endpoints structures */
    for (i = 0U; i < 15U; i++)
    {
        /* Init ep structure */
        hpcd->IN_ep[i].is_in = 1U;
        hpcd->IN_ep[i].num = i;
        hpcd->IN_ep[i].tx_fifo_num = i;
        /* Control until ep is activated */
        hpcd->IN_ep[i].type = EP_TYPE_CTRL;
        hpcd->IN_ep[i].maxpacket = 0U;
        hpcd->IN_ep[i].xfer_buff = 0U;
        hpcd->IN_ep[i].xfer_len = 0U;
    }

    for (i = 0U; i < 15U; i++)
    {
        hpcd->OUT_ep[i].is_in = 0U;
        hpcd->OUT_ep[i].num = i;
        hpcd->IN_ep[i].tx_fifo_num = i;
        /* Control until ep is activated */
        hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
        hpcd->OUT_ep[i].maxpacket = 0U;
        hpcd->OUT_ep[i].xfer_buff = 0U;
        hpcd->OUT_ep[i].xfer_len = 0U;

#if !defined(CHIP_GM6721)
        hpcd->Instance->DIEPTXF[i] = 0U;
#endif
    }

    /* Init Device */
    USB_DevInit(hpcd->Instance, hpcd->Init);

    hpcd->State = HAL_PCD_STATE_READY;

#ifdef USB_OTG_GLPMCFG_LPMEN
    /* Activate LPM */
    if (hpcd->Init.lpm_enable == 1U)
    {
        HAL_PCDEx_ActivateLPM(hpcd);
    }
#endif /* USB_OTG_GLPMCFG_LPMEN */

#ifdef USB_OTG_GCCFG_BCDEN
    /* Activate Battery charging */
    if (hpcd->Init.battery_charging_enable == 1U)
    {
        HAL_PCDEx_ActivateBCD(hpcd);
    }
#endif /* USB_OTG_GCCFG_BCDEN */

    USB_DevDisconnect(hpcd->Instance);
    return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
    /* Check the PCD handle allocation */
    if (hpcd == NULL)
    {
        return HAL_ERROR;
    }

    hpcd->State = HAL_PCD_STATE_BUSY;

    /* Stop Device */
    HAL_PCD_Stop(hpcd);

    /* DeInit the low level hardware */
    HAL_PCD_MspDeInit(hpcd);

    hpcd->State = HAL_PCD_STATE_RESET;

    return HAL_OK;
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_MspInit could be implemented in the user file
     */
}

/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_MspDeInit could be implemented in the user file
     */
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group2 Input and Output operation functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the PCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Start The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{
    __HAL_LOCK(hpcd);
    USB_DevConnect(hpcd->Instance);
    __HAL_PCD_ENABLE(hpcd);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}

/**
  * @brief  Stop The USB OTG Device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{
    __HAL_LOCK(hpcd);
    __HAL_PCD_DISABLE(hpcd);
    USB_StopDevice(hpcd->Instance);
    USB_DevDisconnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}

/**
  * @brief  Handles PCD interrupt request.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{

#if !defined(CHIP_GM6721)

#else

    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    uint32_t i = 0U, ep_intr = 0U, epint = 0U, epnum = 0U;
    uint32_t fifoemptymsk = 0U, temp = 0U;
    USB_OTG_EPTypeDef *ep;
    uint8_t intrUsb = 0;
    uint8_t cnt = 0;
    uint32_t nbytes = 0;




    /* ensure that we are in device mode */
    if (USB_GetMode(hpcd->Instance) == USB_OTG_MODE_DEVICE)
    {

        /* INTRUSB register is cleared automatically by reading */
        /* We do not use __HAL_PCD_GET_FLAG, here, as IntrUSB will be cleared automatically once it'read first,
            so, you'll get always ZERO for next readings*/

        intrUsb = USB_ReadInterrupts(USBx);
        /* Handle Resume Interrupt */
        if (intrUsb & USB_OTG_INTRUSB_RESUME)
        {
            /* Clear the Remote Wake-up Signaling */
            HAL_PCD_ResumeCallback(hpcd);
        }

        /* Handle Suspend Interrupt */
        if (intrUsb & USB_OTG_INTRUSB_SUSPEND)
        {
            HAL_PCD_SuspendCallback(hpcd);
        }

        /* Handle Reset Interrupt */
        if (intrUsb & USB_OTG_INTRUSB_RSTBABL)
        {

            /*Flush all Tx endpoint*/
            USB_FlushTxFifo(hpcd->Instance, 0x10U);

            for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
            {
                /*Activate each endpoint here ???*/
            }

            /* Set Default Address to 0 */

            /* setup EP0 to receive SETUP packets */
            USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);

            /*Mentor USB IP does not have Enumeration-Done flag, so reset callback is invoked here*/
            /*But I cannt evaluate the result, FixMe !!!*/
            HAL_PCD_ResetCallback(hpcd);
        }


        /* Handle SOF Interrupt */
        if (intrUsb & USB_OTG_INTRUSB_SOF)
        {
            HAL_PCD_SOFCallback(hpcd);
        }

        /*handle interrupt for IN & OUT endpoints*/
        if (1)
        {
            epnum = 0U;

            /* Read in the device interrupt bits */
            ep_intr = USB_ReadDevAllOutEpInterrupt(hpcd->Instance);

            while (ep_intr)
            {
                if (ep_intr & 0x1U)
                {
                    /*Actually OutEpoint can not handle endpoint 0, here*/
                    epint = USB_ReadDevOutEPInterrupt(hpcd->Instance, epnum);

                    /*Endpoint 0 has different senses from Endpoint 1 ~ 15*/
                    if (epnum != 0)
                    {
                        /*When and where should the following two APIs be involked*/
                        ep = &hpcd->OUT_ep[epnum];

                        if (hpcd->State == HAL_PCD_STATE_BUSY)
                        {
                            if (ep->xfer_len != 0U && ((epint & USB_OTG_RXCSR1_RXPKTRDY) == USB_OTG_RXCSR1_RXPKTRDY))
                            {
                                if (ep->xfer_len > ep->maxpacket)
                                {
                                    cnt = ep->maxpacket;
                                }
                                else
                                {
                                    cnt = (USBx_DEVICE->RxCount2) << 8 | USBx_DEVICE->Count_Union.RxCount1;;
                                    hpcd->State = HAL_PCD_STATE_RESET;
                                }

                                USB_ReadPacket(USBx, ep->xfer_buff, epnum, cnt, 0);
                                /*Clear the interrupt flag*/
                                USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_RXPKTRDY);
                                ep->xfer_len  -= cnt;
                                ep->xfer_buff  += cnt;
                                ep->xfer_count += cnt;
                            }
                            if (hpcd->State == HAL_PCD_STATE_RESET)
                            {
                                HAL_PCD_DataOutStageCallback(hpcd, epnum);
                            }
                        }
                        else
                        {
                            /*RxPktRdy flag*/
                            if ((epint & USB_OTG_RXCSR1_RXPKTRDY) == USB_OTG_RXCSR1_RXPKTRDY)
                            {
                                cnt = (USBx_DEVICE->RxCount2) << 8 | USBx_DEVICE->Count_Union.RxCount1;
                                USB_ReadPacket(USBx, ep->xfer_buff, epnum, cnt, 0);
                                ep->xfer_buff += cnt;
                                ep->xfer_count += cnt;

                                /*Clear the interrupt flag*/
                                USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_RXPKTRDY);
                                /*An datapacket is received, maybe should retrive data from FIFO*/
                                HAL_PCD_DataOutStageCallback(hpcd, epnum);
                            }
                        }

                        //HAL_PCD_DataOutStageCallback(hpcd, epnum);

                        /*FIFOFULL flag*/
                        if ((epint & USB_OTG_RXCSR1_FIFOFULL) == USB_OTG_RXCSR1_FIFOFULL)
                        {
                            USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_FIFOFULL);
                        }

                        /*OVERRUN flag*/
                        if ((epint & USB_OTG_RXCSR1_OVERRUN) == USB_OTG_RXCSR1_OVERRUN)
                        {
                            USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_OVERRUN);
                        }

                        /*Dataerr flag*/
                        if ((epint & USB_OTG_RXCSR1_DATAERR) == USB_OTG_RXCSR1_DATAERR)
                        {
                            /*Only valid for ISO transfer*/
                        }

                        /*SentStall flag*/
                        if ((epint & USB_OTG_RXCSR1_SENTSTLL) == USB_OTG_RXCSR1_SENTSTLL)
                        {
                            USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_SENTSTLL);
                        }
                    }
                }

                epnum++;

                ep_intr >>= 1U;
            } /*end of while*/
        }


        if (1)
        {
            /* Read in the device interrupt bits */
            ep_intr = USB_ReadDevAllInEpInterrupt(hpcd->Instance);

            epnum = 0U;

            while (ep_intr)
            {
                if (ep_intr & 0x1U) /* In ITR */
                {
                    epint = USB_ReadDevInEPInterrupt(hpcd->Instance, epnum);
                    if (epnum == 0)
                    {
                        /*SentStall flag*/
                        if ((epint & USB_OTG_CSR0_SENTSTALL) == USB_OTG_CSR0_SENTSTALL)
                        {
                            USBx_DEVICE->CtrlSts0_Union.CSR0 = epint & (~(USB_OTG_CSR0_SENTSTALL));
                        }

                        /*SetupEnd flag*/
                        if ((epint & USB_OTG_CSR0_SETUPEND) == USB_OTG_CSR0_SETUPEND)
                        {

                            USBx_DEVICE->CtrlSts0_Union.CSR0 = epint | USB_OTG_CSR0_SERVSETUPEND;
                        }


                        ep = &hpcd->IN_ep[epnum];
                        if (hpcd->State == HAL_PCD_STATE_BUSY)
                        {
                            if (ep->xfer_len > 0)
                            {
                                if (ep->xfer_len > ep->maxpacket)
                                {
                                    nbytes = ep->maxpacket;
                                }
                                else
                                {
                                    nbytes = ep->xfer_len;
                                    hpcd->State = HAL_PCD_STATE_RESET;
                                }
                                /*Write index register*/
                                USBx->Index = ep->num;
                                /*write data to fifo, dma not enabled*/
                                USB_WritePacket(USBx, ep->xfer_buff, ep->num, nbytes, 0);

                                if (nbytes <= ep->maxpacket)
                                {
                                    /*it's the end of transfer*/
                                    USBx_DEVICE->CtrlSts0_Union.CSR0 |= (USB_OTG_CSR0_TXPKTRDY | USB_OTG_CSR0_DATAEND);
                                }
                                else
                                {
                                    /*trigger to send data*/
                                    USBx_DEVICE->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_TXPKTRDY;
                                }

                                ep->xfer_len -= nbytes;
                                ep->xfer_buff += nbytes;
                                ep->xfer_count += nbytes;

                            }
                        }
                        else
                        {
                            /*RxPktRdy flag*/
                            if ((epint & USB_OTG_CSR0_RXPKTRDY) == USB_OTG_CSR0_RXPKTRDY)
                            {
                                /*An datapacket is received, maybe should retrive data from FIFO*/
                                /*Assume that endpoint 0 only receive SETUP packet and
                                  will reject other packet whose length is less or more than 8 bytes*/
                                USB_ReadPacket(USBx, (uint8_t *)hpcd->Setup, 0, 8U, 0);

                                /*Clear the flag after reading the fifo*/
                                USBx_DEVICE->CtrlSts0_Union.CSR0 = epint | USB_OTG_CSR0_SERVRXPKTRDY;

                                /*Handle Setup Packet*/
                                HAL_PCD_SetupStageCallback(hpcd);
                            }


                        }

                        if (!(epint & USB_OTG_CSR0_RXPKTRDY))
                        {
                            //extern void HAL_SetAddress(PCD_HandleTypeDef * hpcd);
                            HAL_SetAddress(hpcd);
                        }
                    }
                    else
                    {

                        /*When and where should the following API be involked*/
                        //HAL_PCD_DataInStageCallback(hpcd, epnum);
                        ep = &hpcd->IN_ep[epnum];
                        if (hpcd->State == HAL_PCD_STATE_BUSY)
                        {
                            if (ep->xfer_len > 0)
                            {
                                if (ep->xfer_len > ep->maxpacket)
                                {
                                    nbytes = ep->maxpacket;
                                }
                                else
                                {
                                    nbytes = ep->xfer_len;
                                    hpcd->State = HAL_PCD_STATE_RESET;
                                }

                                /*Write index register*/
                                USBx->Index = ep->num;
                                /*write data to fifo, dma not enabled*/
                                USB_WritePacket(USBx, ep->xfer_buff, ep->num, nbytes, 0);
                                /*trigger to send data, now it will trigger next interrupt*/
                                USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_TXPKTRDY;

                                ep->xfer_len -= nbytes;
                                ep->xfer_buff += nbytes;
                                ep->xfer_count += nbytes;
                            }
                        }
                        else
                        {
                            HAL_PCD_DataInStageCallback(hpcd, epnum);
                        }

                        /*UnderRun flag*/
                        if ((epint & USB_OTG_TXCSR1_UNDERRUN) == USB_OTG_TXCSR1_UNDERRUN)
                        {
                            USBx_DEVICE->CtrlSts0_Union.TxCSR1 = epint & (~(USB_OTG_TXCSR1_UNDERRUN));
                        }

                        /*SentStall flag*/
                        if ((epint & USB_OTG_TXCSR1_SENTSTLL) == USB_OTG_TXCSR1_SENTSTLL)
                        {
                            USBx_DEVICE->CtrlSts0_Union.TxCSR1 = epint & (~(USB_OTG_TXCSR1_SENTSTLL));
                        }

                        /*FIFO Not Empty*/
                        if ((epint & USB_OTG_TXCSR1_FIFONOTEMPTY) == USB_OTG_TXCSR1_FIFONOTEMPTY)
                        {
                            /*Do nothing here !!!*/
                        }
                    }

                }
                epnum++;
                ep_intr >>= 1U;
            }
        }

#if 0  /*Mentor USB IP does not have ISO IN & OUT interrupt*/
        /* Handle Incomplete ISO IN Interrupt */
        if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_IISOIXFR))
        {
            HAL_PCD_ISOINIncompleteCallback(hpcd, epnum);
        }

        /* Handle Incomplete ISO OUT Interrupt */
        if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
        {
            HAL_PCD_ISOOUTIncompleteCallback(hpcd, epnum);
        }
#endif

#if 0  /*Mentor USB IP does not have ISO IN & OUT interrupt*/
        /* Handle Connection event Interrupt */
        if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT))
        {
            HAL_PCD_ConnectCallback(hpcd);
            __HAL_PCD_CLEAR_FLAG(hpcd, USB_OTG_GINTSTS_SRQINT);
        }

        /* Handle Disconnection event Interrupt */
        if (__HAL_PCD_GET_FLAG(hpcd, USB_OTG_GINTSTS_OTGINT))
        {
            temp = hpcd->Instance->GOTGINT;

            if ((temp & USB_OTG_GOTGINT_SEDET) == USB_OTG_GOTGINT_SEDET)
            {
                HAL_PCD_DisconnectCallback(hpcd);
            }
            hpcd->Instance->GOTGINT |= temp;
        }
#endif
    }
#endif
}

/**
  * @brief  Data OUT stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number
  * @retval None
  */
__weak void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_DataOutStageCallback could be implemented in the user file
     */
}

/**
  * @brief  Data IN stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number
  * @retval None
  */
__weak void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_DataInStageCallback could be implemented in the user file
     */
}
/**
  * @brief  Setup stage callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_SetupStageCallback could be implemented in the user file
     */
}

/**
  * @brief  USB Start Of Frame callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_SOFCallback could be implemented in the user file
     */
}

/**
  * @brief  USB Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_ResetCallback could be implemented in the user file
     */
}

/**
  * @brief  Suspend event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_SuspendCallback could be implemented in the user file
     */
}

/**
  * @brief  Resume event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_ResumeCallback could be implemented in the user file
     */
}

/**
  * @brief  Incomplete ISO OUT callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number
  * @retval None
  */
__weak void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_ISOOUTIncompleteCallback could be implemented in the user file
     */
}

/**
  * @brief  Incomplete ISO IN callback.
  * @param  hpcd: PCD handle
  * @param  epnum: endpoint number
  * @retval None
  */
__weak void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_ISOINIncompleteCallback could be implemented in the user file
     */
}

/**
  * @brief  Connection event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_ConnectCallback could be implemented in the user file
     */
}

/**
  * @brief  Disconnection event callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
__weak void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_PCD_DisconnectCallback could be implemented in the user file
     */
}

/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group3 Peripheral Control functions
 *  @brief   management functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the PCD data
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Connect the USB device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd)
{
    __HAL_LOCK(hpcd);
    USB_DevConnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}

/**
  * @brief  Disconnect the USB device.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd)
{
    __HAL_LOCK(hpcd);
    USB_DevDisconnect(hpcd->Instance);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}

/**
  * @brief  Set the USB Device address.
  * @param  hpcd: PCD handle
  * @param  address: new device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
{
    __HAL_LOCK(hpcd);
    USB_SetDevAddress(hpcd->Instance, address);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}
/**
  * @brief  Open and configure an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  ep_mps: endpoint max packet size
  * @param  ep_type: endpoint type
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
    HAL_StatusTypeDef  ret = HAL_OK;
    USB_OTG_EPTypeDef *ep;

    if ((ep_addr & 0x80) == 0x80)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7F];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr & 0x7F];
    }
    ep->num   = ep_addr & 0x7F;

    ep->is_in = (0x80 & ep_addr) != 0;
    ep->maxpacket = ep_mps;
    ep->type = ep_type;
    if (ep->is_in)
    {
        /* Assign a Tx FIFO */
        ep->tx_fifo_num = ep->num;
    }
    /* Set initial data PID. */
    if (ep_type == EP_TYPE_BULK)
    {
        ep->data_pid_start = 0U;
    }

    __HAL_LOCK(hpcd);
    USB_ActivateEndpoint(hpcd->Instance, ep);
    __HAL_UNLOCK(hpcd);
    return ret;
}


/**
  * @brief  Deactivate an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    USB_OTG_EPTypeDef *ep;

    if ((ep_addr & 0x80) == 0x80)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7F];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr & 0x7F];
    }
    ep->num   = ep_addr & 0x7F;

    ep->is_in = (0x80 & ep_addr) != 0;

    __HAL_LOCK(hpcd);
    USB_DeactivateEndpoint(hpcd->Instance, ep);
    __HAL_UNLOCK(hpcd);
    return HAL_OK;
}


/**
  * @brief  Receive an amount of data.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the reception buffer
  * @param  len: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
    USB_OTG_EPTypeDef *ep;
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

    ep = &hpcd->OUT_ep[ep_addr & 0x7F];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;
    ep->xfer_len = len;
    ep->xfer_count = 0U;
    ep->is_in = 0U;
    ep->num = ep_addr & 0x7F;

    if (hpcd->Init.dma_enable == 1U)
    {
        ep->dma_addr = (uint32_t)pBuf;
    }

    if ((ep_addr & 0x7F) == 0)
    {
        USB_EP0StartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
    }
    else
    {
        if (ep->type == EP_TYPE_ISOC)
        {
            //    USBx_DEVICE->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_ISO;
            if (ep->xfer_len > ep->maxpacket)
            {
                hpcd->State = HAL_PCD_STATE_BUSY;
            }
            USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);

        }
        else
        {
            if (ep->xfer_len > ep->maxpacket)
            {
                hpcd->State = HAL_PCD_STATE_BUSY;
            }
            USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
        }
    }

    return HAL_OK;
}

/**
  * @brief  Get Received Data Size.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval Data Size
  */
uint16_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    return hpcd->OUT_ep[ep_addr & 0xF].xfer_count;
}
/**
  * @brief  Send an amount of data.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the transmission buffer
  * @param  len: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
    USB_OTG_EPTypeDef *ep;
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

    ep = &hpcd->IN_ep[ep_addr & 0x7F];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;
    ep->xfer_len = len;
    ep->xfer_count = 0U;
    ep->is_in = 1U;
    ep->num = ep_addr & 0x7F;

    if (hpcd->Init.dma_enable == 1U)
    {
        ep->dma_addr = (uint32_t)pBuf;
    }

    if ((ep_addr & 0x7F) == 0)
    {
        if (ep->xfer_len > ep->maxpacket)
        {
            hpcd->State = HAL_PCD_STATE_BUSY;
        }
        USB_EP0StartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
    }
    else
    {

        if (ep->type == EP_TYPE_ISOC)
        {
            USBx_DEVICE->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_ISO;
            if (ep->xfer_len > ep->maxpacket)
            {
                hpcd->State = HAL_PCD_STATE_BUSY;
            }
            USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);

        }
        else
        {
            if (ep->xfer_len > ep->maxpacket)
            {
                hpcd->State = HAL_PCD_STATE_BUSY;
            }

            USB_EPStartXfer(hpcd->Instance, ep, hpcd->Init.dma_enable);
        }
    }

    return HAL_OK;
}

/**
  * @brief  Set a STALL condition over an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    USB_OTG_EPTypeDef *ep;

    if ((0x80 & ep_addr) == 0x80)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7F];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr];
    }

    ep->is_stall = 1U;
    ep->num   = ep_addr & 0x7F;
    ep->is_in = ((ep_addr & 0x80) == 0x80);


    __HAL_LOCK(hpcd);
    USB_EPSetStall(hpcd->Instance, ep);
    if ((ep_addr & 0x7F) == 0)
    {
        USB_EP0_OutStart(hpcd->Instance, hpcd->Init.dma_enable, (uint8_t *)hpcd->Setup);
    }
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    USB_OTG_EPTypeDef *ep;

    if ((0x80 & ep_addr) == 0x80)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7F];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr];
    }

    ep->is_stall = 0U;
    ep->num   = ep_addr & 0x7F;
    ep->is_in = ((ep_addr & 0x80) == 0x80);

    __HAL_LOCK(hpcd);
    USB_EPClearStall(hpcd->Instance, ep);
    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

/**
  * @brief  Flush an endpoint.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    __HAL_LOCK(hpcd);

    if ((ep_addr & 0x80) == 0x80)
    {
        USB_FlushTxFifo(hpcd->Instance, ep_addr & 0x7F);
    }
    else
    {
#if !defined(CHIP_GM6721)
        USB_FlushRxFifo(hpcd->Instance);
#else
        USB_FlushRxFifo(hpcd->Instance, ep_addr & 0x7F);
#endif
    }

    __HAL_UNLOCK(hpcd);

    return HAL_OK;
}

/**
  * @brief  Activate remote wakeup signalling.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
#if !defined(CHIP_GM6721)
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

    if ((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
    {
        /* Activate Remote wakeup signaling */
        USBx_DEVICE->DCTL |= USB_OTG_DCTL_RWUSIG;
    }
#else
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    if (USBx->Power & USB_OTG_POWER_SUSPENDMODE == USB_OTG_POWER_SUSPENDMODE)
    {
        USBx->Power |= USB_OTG_POWER_RESUME;
    }
#endif
    return HAL_OK;
}

/**
  * @brief  De-activate remote wakeup signalling.
  * @param  hpcd: PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
#if !defined(CHIP_GM6721)
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;

    /* De-activate Remote wakeup signaling */
    USBx_DEVICE->DCTL &= ~(USB_OTG_DCTL_RWUSIG);
#else
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    USBx->Power &= ~(USB_OTG_POWER_RESUME);
#endif
    return HAL_OK;
}
/**
  * @}
  */

/** @defgroup PCD_Exported_Functions_Group4 Peripheral State functions
 *  @brief   Peripheral State functions
 *
@verbatim
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================
    [..]
    This subsection permits to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PCD handle state.
  * @param  hpcd: PCD handle
  * @retval HAL state
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd)
{
    return hpcd->State;
}
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @addtogroup PCD_Private_Functions
  * @{
  */

/**
  * @brief  Check FIFO for the next packet to be loaded.
  * @param  hpcd: PCD handle
  * @param  epnum : endpoint number
  * @retval HAL status
  */
static HAL_StatusTypeDef PCD_WriteEmptyTxFifo(PCD_HandleTypeDef *hpcd, uint32_t epnum)
{
#if !defined(CHIP_GM6721)
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    USB_OTG_EPTypeDef *ep;
    int32_t len = 0U;
    uint32_t len32b;
    uint32_t fifoemptymsk = 0U;

    ep = &hpcd->IN_ep[epnum];
    len = ep->xfer_len - ep->xfer_count;

    if (len > ep->maxpacket)
    {
        len = ep->maxpacket;
    }


    len32b = (len + 3U) / 4U;

    while (((USBx_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) > len32b) &&
            (ep->xfer_count < ep->xfer_len) &&
            (ep->xfer_len != 0U))
    {
        /* Write the FIFO */
        len = ep->xfer_len - ep->xfer_count;

        if (len > ep->maxpacket)
        {
            len = ep->maxpacket;
        }
        len32b = (len + 3U) / 4U;

        USB_WritePacket(USBx, ep->xfer_buff, epnum, len, hpcd->Init.dma_enable);

        ep->xfer_buff  += len;
        ep->xfer_count += len;
    }

    if (len <= 0U)
    {
        fifoemptymsk = 0x1U << epnum;
        USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;

    }
#endif
    return HAL_OK;
}

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx ||
          STM32F412Vx || STM32F412Cx || STM32F413xx || STM32F423xx */
#endif /* HAL_PCD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
