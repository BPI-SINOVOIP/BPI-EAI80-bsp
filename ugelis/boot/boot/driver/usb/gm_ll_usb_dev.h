/**
  ******************************************************************************
  * @file    stm32f469xx.h
  * @author  MCD Application Team
  * @version V2.6.1
  * @date    14-February-2017
  * @brief   CMSIS STM32F469xx Device Peripheral Access Layer Header File.
  *
  *          This file contains:
  *           - Data structures and the address mapping for all peripherals
  *           - peripherals registers declarations and bits definition
  *           - Macros to access peripheral’s registers hardware
  *
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

/** @addtogroup CMSIS_Device
  * @{
  */

/** @addtogroup stm32f469xx
  * @{
  */
    
#ifndef __STM32F469xx_H
#define __STM32F469xx_H

#include <gm_common.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


#define __IO volatile

/** @addtogroup Configuration_section_for_CMSIS
  * @{
  */

/**
  * @brief Configuration of the Cortex-M4 Processor and Core Peripherals 
  */
#define __FPU_PRESENT             1U       /*!< FPU present                                   */

/**
  * @}
  */
  
/** @addtogroup Peripheral_interrupt_number_definition
  * @{
  */
     

//#include <stdint.h>

/** @addtogroup Peripheral_registers_structures
  * @{
  */   

/** 
  * @brief Analog to Digital Converter  
  */


/** 
  * @brief Controller Area Network TxMailBox 
  */

/** 
  * @brief USB_OTG_Core_Registers
  */
typedef struct
{
  /*The following for GM6721 USB Spec*/
  __IO uint8_t FAddr;               /*!< Function Address                        00h */  
  __IO uint8_t Power;               /*!< Function Power Management               01h */  
  __IO uint8_t IntrTx1;             /*!< Interrupt register for Endpoint 0 plus Tx Endpoints 1 to 7.   02h */
  __IO uint8_t IntrTx2;             /*!< Interrupt register for Tx Endpoints 8 to 15.   03h */
  __IO uint8_t IntrRx1;             /*!< Interrupt register for Rx Endpoints 1 to 7.    04h */ 
  __IO uint8_t IntrRx2;             /*!< Interrupt register for Rx Endpoints 8 to 15.   05h */
  __IO uint8_t IntrUSB;             /*!< Interrupt register for common USB interrupts.  06h */
  __IO uint8_t InTrTx1E;            /*!< Interrupt enable register for IntrTx1.   07h */
  __IO uint8_t InTrTx2E;            /*!< Interrupt enable register for IntrTx2.   08h */
  __IO uint8_t InTrRx1E;            /*!< Interrupt enable register for IntrRx1.   09h */
  __IO uint8_t InTrRx2E;            /*!< Interrupt enable register for IntrRx2.   0Ah */
  __IO uint8_t IntrUSBE;            /*!< Interrupt enable register for IntrUSB.   0Bh */
  __IO uint8_t Frame1;              /*!< Frame number bits 0 to 7.   0Ch */
  __IO uint8_t Frame2;              /*!< Frame number bits 8 to 10.  0Dh */
  __IO uint8_t Index;               /*!< Index register for selecting the endpoint status and control registers.   0Eh */
  __IO uint8_t DevCtl;              /*!<  USB device control register.   0Fh */
} USB_OTG_GlobalTypeDef;

/** 
  * @brief USB_OTG_device_Registers
  */
typedef struct 
{
  /*The following for GM6721 USB Spec*/
  __IO uint8_t TxMaxP;           /*!< Maximum packet size for peripheral IN endpoint. (Index register set to select Endpoints 1 ~ 15 only) 10h */
  union
  {
    __IO uint8_t CSR0;           /*!< Main Control Status register for Endpoint 0. (Index register set to select Endpoint 11h) */
    __IO uint8_t TxCSR1;         /*!< Control Status register 1 for peripheral IN endpoint. (Index register set to select Endpoints 1 ~ 15, 11h) */
  }CtrlSts0_Union;  
  union
  {
    __IO uint8_t CSR02;          /*!< Main Control Status register for Endpoint 0. (Index register set to select Endpoint 12h) */
    __IO uint8_t TxCSR2;         /*!< Control Status register 1 for peripheral IN endpoint. (Index register set to select Endpoints 1 ~ 15, 12h) */
  }CtrlSts1_Union;
  
  __IO uint8_t RxMaxP;           /*!< Maximum packet size for peripheral OUT endpoint. (Index register set to select Endpoints 1 ~ 15 only) 13h */  
  __IO uint8_t RxCSR1;           /*!< Control Status register 1 for peripheral OUT endpoint. (Index register set to select Endpoints 1 ~ 15 only) 14h */
  __IO uint8_t RxCSR2;           /*!< Control Status register 2 for peripheral OUT endpoint. (Index register set to selectEndpoints 1 ~ 15 only) 15h */  
  union
  {
    __IO uint8_t Count0;         /*!< Number of received bytes in Endpoint 0 FIFO. (Index register set to select Endpoint 0), 16h */
    __IO uint8_t RxCount1;       /*!< Number of bytes in peripheral OUT endpoint FIFO (lower byte). (Index register set to select Endpoints 1 ~ 15) 16h */      
  }Count_Union;
  
  __IO uint8_t RxCount2;         /*!< Number of bytes in peripheral OUT endpoint FIFO (upper byte). (Index register set to select Endpoints 1 ~ 15 only) 17h*/
  uint8_t Reserved1[4];          /*!< Reserved 18h ~ 1Bh */
  uint8_t DyTxFIFO1;             /*!< Dynamic FIFO for TxFIFO 1 0x1C*/
  uint8_t DyTxFIFO2;             /*!< Dynamic FIFO for TxFIFO 2 0x1D*/
  uint8_t DyRxFIFO1;             /*!< Dynamic FIFO for RxFIFO 1 0x1E*/
  union
  {
    __IO uint8_t DyRxFIFO2;      /*!< Dynamic FIFO for RxFIFO 2 0x1F*/
    __IO uint8_t FIFOSize;       /*!< Number of bytes in peripheral OUT endpoint FIFO (upper byte). (Index register set to select Endpoints 1~ 15 only) 1Fh*/
  }FIFO_Union;    
} USB_OTG_DeviceTypeDef;


#if defined(CHIP_GM6721)
typedef struct 
{
    __IO uint8_t FIFOx[16];       /*!< FIFOs for Endpoints 0 ~ 15. */
}USB_OTG_FIFOTypedef;
#endif


#if 0
/** 
  * @brief USB_OTG_IN_Endpoint-Specific_Register
  */
typedef struct 
{
    
} USB_OTG_INEndpointTypeDef;

/** 
  * @brief USB_OTG_OUT_Endpoint-Specific_Registers
  */
typedef struct 
{
    
} USB_OTG_OUTEndpointTypeDef;

/** 
  * @brief USB_OTG_Host_Mode_Register_Structures
  */
typedef struct 
{  
} USB_OTG_HostTypeDef;

/** 
  * @brief USB_OTG_Host_Channel_Specific_Registers
  */
typedef struct
{

} USB_OTG_HostChannelTypeDef;
#endif
/**
  * @}
  */


/*!< USB registers base address */
#if !defined(CHIP_GM6721)
#define USB_OTG_HS_PERIPH_BASE               0x40040000U
#define USB_OTG_FS_PERIPH_BASE               0x50000000U
#define USB_OTG_GLOBAL_BASE                  0x000U
#define USB_OTG_DEVICE_BASE                  0x800U
#define USB_OTG_IN_ENDPOINT_BASE             0x900U
#define USB_OTG_OUT_ENDPOINT_BASE            0xB00U
#define USB_OTG_EP_REG_SIZE                  0x20U
#define USB_OTG_HOST_BASE                    0x400U
#define USB_OTG_HOST_PORT_BASE               0x440U
#define USB_OTG_HOST_CHANNEL_BASE            0x500U
#define USB_OTG_HOST_CHANNEL_SIZE            0x20U
#define USB_OTG_PCGCCTL_BASE                 0xE00U
#define USB_OTG_FIFO_BASE                    0x1000U
#define USB_OTG_FIFO_SIZE                    0x1000U
#else
#define USB_OTG_FS_PERIPH_BASE               0x50000000U

#endif

#if defined(CHIP_GM6721)
#define USB_OTG_GMFIFO_BASE                  0x20U
#define USB_OTG_DEVICE_BASE                  0x10U
#endif

/**
  * @}
  */

/** @addtogroup Peripheral_declaration
  * @{
  */  

#define USB_OTG_FS          ((USB_OTG_GlobalTypeDef *) USB_OTG_FS_PERIPH_BASE)
//#define USB_OTG_HS          ((USB_OTG_GlobalTypeDef *) USB_OTG_HS_PERIPH_BASE)

/**
  * @}
  */

/** @addtogroup Exported_constants
  * @{
  */
  
  /** @addtogroup Peripheral_Registers_Bits_Definition
  * @{
  */
    

#if defined(CHIP_GM6721)
/**************Bit definition for USB_OTG_GOTGCTL register *******************/
/*Flags for DEVCTL register*/
#define USB_OTG_DEVCTL_HOSTMODE_Pos              (2U)
#define USB_OTG_DEVCTL_HOSTMODE_Msk              (0x1U << USB_OTG_DEVCTL_HOSTMODE_Pos) /*!< 0x00000004 */
#define USB_OTG_DEVCTL_HOSTMODE                  USB_OTG_DEVCTL_HOSTMODE_Msk  /*!< USB Mode */ 


/*Flags for POWER register*/
#define USB_OTG_POWER_ISOUPDATE_Pos              (7U)
#define USB_OTG_POWER_ISOUPDATE_Msk              (0x1U << USB_OTG_POWER_ISOUPDATE_Pos) /*!< 0x00000080 */
#define USB_OTG_POWER_ISOUPDATE                  USB_OTG_POWER_ISOUPDATE_Msk  
#define USB_OTG_POWER_RESUME_Pos                 (2U)
#define USB_OTG_POWER_RESUME_Msk                 (0x1U << USB_OTG_POWER_RESUME_Pos) /*!< 0x00000004 */
#define USB_OTG_POWER_RESUME                     USB_OTG_POWER_RESUME_Msk  /*!< Resume */ 
#define USB_OTG_POWER_SUSPENDMODE_Pos            (1U)
#define USB_OTG_POWER_SUSPENDMODE_Msk            (0x1U << USB_OTG_POWER_SUSPENDMODE_Pos) /*!< 0x00000002 */
#define USB_OTG_POWER_SUSPENDMODE                 USB_OTG_POWER_SUSPENDMODE_Msk  /*!< Suspend Mode */ 
#define USB_OTG_POWER_SUSPENDEN_Pos              (0U)
#define USB_OTG_POWER_SUSPENDEN_Msk              (0x1U << USB_OTG_POWER_SUSPENDEN_Pos) /*!< 0x00000001 */
#define USB_OTG_POWER_SUSPENDEN                   USB_OTG_POWER_SUSPENDEN_Msk  /*!< Suspend Enable */ 


/*Flags for TXCSR1 register*/
#define USB_OTG_TXCSR1_TXPKTRDY_Pos              (0U)
#define USB_OTG_TXCSR1_TXPKTRDY_Msk              (0x1U << USB_OTG_TXCSR1_TXPKTRDY_Pos) /*!< 0x00000001 */
#define USB_OTG_TXCSR1_TXPKTRDY                  USB_OTG_TXCSR1_TXPKTRDY_Msk  /*!< Tx Packet Ready */ 
#define USB_OTG_TXCSR1_FIFONOTEMPTY_Pos          (1U)
#define USB_OTG_TXCSR1_FIFONOTEMPTY_Msk          (0x1U << USB_OTG_TXCSR1_FIFONOTEMPTY_Pos) /*!< 0x00000002 */
#define USB_OTG_TXCSR1_FIFONOTEMPTY              USB_OTG_TXCSR1_FIFONOTEMPTY_Msk  /*!< At least 1 packet in Tx FIFO */ 
#define USB_OTG_TXCSR1_UNDERRUN_Pos              (2U)
#define USB_OTG_TXCSR1_UNDERRUN_Msk              (0x1U << USB_OTG_TXCSR1_UNDERRUN_Pos) /*!< 0x00000004 */
#define USB_OTG_TXCSR1_UNDERRUN                  USB_OTG_TXCSR1_UNDERRUN_Msk  /*!< In token received while TxPktRdy not set */ 
#define USB_OTG_TXCSR1_FLUSHFIFO_Pos             (3U)
#define USB_OTG_TXCSR1_FLUSHFIFO_Msk             (0x1U << USB_OTG_TXCSR1_FLUSHFIFO_Pos) /*!< 0x00000008 */
#define USB_OTG_TXCSR1_FLUSHFIFO                 USB_OTG_TXCSR1_FLUSHFIFO_Msk  /*!< Flush Tx Endpoint FIFO */ 
#define USB_OTG_TXCSR1_SENDSTLL_Pos              (4U)
#define USB_OTG_TXCSR1_SENDSTLL_Msk              (0x1U << USB_OTG_TXCSR1_SENDSTLL_Pos) /*!< 0x00000010 */
#define USB_OTG_TXCSR1_SENDSTLL                  USB_OTG_TXCSR1_SENDSTLL_Msk  /*!< Send STALL handshake to IN token */ 
#define USB_OTG_TXCSR1_SENTSTLL_Pos              (5U)
#define USB_OTG_TXCSR1_SENTSTLL_Msk              (0x1U << USB_OTG_TXCSR1_SENTSTLL_Pos) /*!< 0x00000020 */
#define USB_OTG_TXCSR1_SENTSTLL                  USB_OTG_TXCSR1_SENTSTLL_Msk  /*!< STALL handshake to IN token has been transmitted*/ 
#define USB_OTG_TXCSR1_CLRDATATOG_Pos            (6U)
#define USB_OTG_TXCSR1_CLRDATATOG_Msk            (0x1U << USB_OTG_TXCSR1_CLRDATATOG_Pos) /*!< 0x00000040 */
#define USB_OTG_TXCSR1_CLRDATATOG                USB_OTG_TXCSR1_CLRDATATOG_Msk  /*!< Reset endpoint data toggle to 0*/


/*Flags for TXCSR2 register*/
#define USB_OTG_TXCSR2_MODE_Pos                  (5U)
#define USB_OTG_TXCSR2_MODE_Msk                  (0x1U << USB_OTG_TXCSR2_MODE_Pos) /*!< 0x00000020 */
#define USB_OTG_TXCSR2_MODE                      USB_OTG_TXCSR2_MODE_Msk /*!< Enable endpoint direction */
#define USB_OTG_TXCSR2_ISO_Pos                   (6U)
#define USB_OTG_TXCSR2_ISO_Msk                   (0x1U << USB_OTG_TXCSR2_ISO_Pos) /*!< 0x00000040 */
#define USB_OTG_TXCSR2_ISO                        USB_OTG_TXCSR2_ISO_Msk  /*!< Set 1 enable Isochronous protocol*/


/*Flags for RXCSR1 register*/
#define USB_OTG_RXCSR1_RXPKTRDY_Pos              (0U)
#define USB_OTG_RXCSR1_RXPKTRDY_Msk              (0x1U << USB_OTG_RXCSR1_RXPKTRDY_Pos) /*!< 0x00000001 */
#define USB_OTG_RXCSR1_RXPKTRDY                  USB_OTG_RXCSR1_RXPKTRDY_Msk  /*!< A data packet (setup packet included ??) recevied */ 
#define USB_OTG_RXCSR1_FIFOFULL_Pos              (1U)
#define USB_OTG_RXCSR1_FIFOFULL_Msk              (0x1U << USB_OTG_RXCSR1_FIFOFULL_Pos) /*!< 0x00000002 */
#define USB_OTG_RXCSR1_FIFOFULL                  USB_OTG_RXCSR1_FIFOFULL_Msk  /*!< No more packets can loaded into Rx FIFO */ 
#define USB_OTG_RXCSR1_OVERRUN_Pos               (2U)
#define USB_OTG_RXCSR1_OVERRUN_Msk               (0x1U << USB_OTG_RXCSR1_OVERRUN_Pos) /*!< 0x00000004 */
#define USB_OTG_RXCSR1_OVERRUN                   USB_OTG_RXCSR1_OVERRUN_Msk  /*!< No more packets can loaded into Rx FIFO */ 
#define USB_OTG_RXCSR1_DATAERR_Pos               (3U)
#define USB_OTG_RXCSR1_DATAERR_Msk               (0x1U << USB_OTG_RXCSR1_DATAERR_Pos) /*!< 0x00000008 */
#define USB_OTG_RXCSR1_DATAERR                   USB_OTG_RXCSR1_DATAERR_Msk  /*!< CRC or bit stuff error */
#define USB_OTG_RXCSR1_FLUSHFIFO_Pos             (4U)
#define USB_OTG_RXCSR1_FLUSHFIFO_Msk             (0x1U << USB_OTG_RXCSR1_FLUSHFIFO_Pos) /*!< 0x00000010 */
#define USB_OTG_RXCSR1_FLUSHFIFO                  USB_OTG_RXCSR1_FLUSHFIFO_Msk  /*!< Flush RX FIFO */
#define USB_OTG_RXCSR1_SENDSTLL_Pos              (5U)
#define USB_OTG_RXCSR1_SENDSTLL_Msk              (0x1U << USB_OTG_RXCSR1_SENDSTLL_Pos) /*!< 0x00000020 */
#define USB_OTG_RXCSR1_SENDSTLL                  USB_OTG_RXCSR1_SENDSTLL_Msk  /*!< Send STALL handshake */ 
#define USB_OTG_RXCSR1_SENTSTLL_Pos              (6U)
#define USB_OTG_RXCSR1_SENTSTLL_Msk              (0x1U << USB_OTG_RXCSR1_SENTSTLL_Pos) /*!< 0x00000040 */
#define USB_OTG_RXCSR1_SENTSTLL                  USB_OTG_RXCSR1_SENTSTLL_Msk  /*!< STALL handshake has been transmitted*/ 
#define USB_OTG_RXCSR1_CLRDATATOG_Pos            (7U)
#define USB_OTG_RXCSR1_CLRDATATOG_Msk            (0x1U << USB_OTG_RXCSR1_CLRDATATOG_Pos) /*!< 0x00000080 */
#define USB_OTG_RXCSR1_CLRDATATOG                USB_OTG_RXCSR1_CLRDATATOG_Msk  /*!< Reset endpoint data toggle to 0*/

/*Flags for RXCSR2 register*/
#define USB_OTG_RXCSR2_DMAMODE_Pos               (4U)
#define USB_OTG_RXCSR2_DMAMODE_Msk               (0x1U << USB_OTG_RXCSR2_DMAMODE_Pos) /*!< 0x00000010 */
#define USB_OTG_RXCSR2_DMAMODE                   USB_OTG_RXCSR2_DMAMODE_Msk  /*!< DMA Mode */ 
#define USB_OTG_RXCSR2_DMAEN_Pos                 (5U)
#define USB_OTG_RXCSR2_DMAEN_Msk                 (0x1U << USB_OTG_RXCSR2_DMAMODE_Pos) /*!< 0x00000020 */
#define USB_OTG_RXCSR2_DMAEN                     USB_OTG_RXCSR2_DMAMODE_Msk  /*!< DMA Enable */ 
#define USB_OTG_RXCSR2_ISO_Pos                   (6U)
#define USB_OTG_RXCSR2_ISO_Msk                   (0x1U << USB_OTG_RXCSR2_ISO_Pos) /*!< 0x00000040 */
#define USB_OTG_RXCSR2_ISO                       USB_OTG_RXCSR2_ISO_Msk  /*!< ISO/AuToReq */ 
#define USB_OTG_RXCSR2_AUTOREQ_Pos               (6U)
#define USB_OTG_RXCSR2_AUTOREQ_Msk               (0x1U << USB_OTG_RXCSR2_AUTOREQ_Pos) /*!< 0x00000040 */
#define USB_OTG_RXCSR2_AUTOREQ                   USB_OTG_RXCSR2_ISO_Msk  /*!< ISO/AuToReq */ 
#define USB_OTG_RXCSR2_AUTOCLEAR_Pos             (7U)
#define USB_OTG_RXCSR2_AUTOCLEAR_Msk             (0x1U << USB_OTG_RXCSR2_AUTOCLEAR_Pos) /*!< 0x00000080 */
#define USB_OTG_RXCSR2_AUTOCLEAR                 USB_OTG_RXCSR2_ISO_Msk  /*!< AutoClear */ 

/*Flags for INTRUSB register*/
#define USB_OTG_INTRUSB_SUSPEND_Pos              (0U)
#define USB_OTG_INTRUSB_SUSPEND_MSK              (0x1U << USB_OTG_INTRUSB_SUSPEND_Pos) /*!< 0x00000001 */
#define USB_OTG_INTRUSB_SUSPEND                  USB_OTG_INTRUSB_SUSPEND_MSK  /*!< Suspend request */ 
#define USB_OTG_INTRUSB_RESUME_Pos               (1U)
#define USB_OTG_INTRUSB_RESUME_MSK               (0x1U << USB_OTG_INTRUSB_RESUME_Pos) /*!< 0x00000002 */
#define USB_OTG_INTRUSB_RESUME                   USB_OTG_INTRUSB_RESUME_MSK  /*!< Resume request */ 
#define USB_OTG_INTRUSB_RSTBABL_Pos              (2U)
#define USB_OTG_INTRUSB_RSTBABL_MSK              (0x1U << USB_OTG_INTRUSB_RSTBABL_Pos) /*!< 0x00000004 */
#define USB_OTG_INTRUSB_RSTBABL                  USB_OTG_INTRUSB_RSTBABL_MSK  /*!< Reset or Babble request */ 
#define USB_OTG_INTRUSB_SOF_Pos                  (3U)
#define USB_OTG_INTRUSB_SOF_MSK                  (0x1U << USB_OTG_INTRUSB_SOF_Pos) /*!< 0x00000008 */
#define USB_OTG_INTRUSB_SOF                      USB_OTG_INTRUSB_SOF_MSK  /*!< SOF request */ 
#define USB_OTG_INTRUSB_CON_Pos                  (4U)
#define USB_OTG_INTRUSB_CON_MSK                  (0x1U << USB_OTG_INTRUSB_CON_Pos) /*!< 0x00000010 */
#define USB_OTG_INTRUSB_CON                      USB_OTG_INTRUSB_CON_MSK  /*!< Connect request */ 
#define USB_OTG_INTRUSB_DISCON_Pos               (5U)
#define USB_OTG_INTRUSB_DISCON_MSK               (0x1U << USB_OTG_INTRUSB_DISCON_Pos) /*!< 0x00000020 */
#define USB_OTG_INTRUSB_DISCON                   USB_OTG_INTRUSB_DISCON_MSK  /*!< Connect request */ 
#define USB_OTG_INTRUSB_SESSION_Pos              (6U)
#define USB_OTG_INTRUSB_SESSION_MSK              (0x1U << USB_OTG_INTRUSB_SESSION_Pos) /*!< 0x00000040 */
#define USB_OTG_INTRUSB_SESSION                  USB_OTG_INTRUSB_SESSION_MSK  /*!< Session request */ 
#define USB_OTG_INTRUSB_VBUSERR_Pos              (7U)
#define USB_OTG_INTRUSB_VBUSERR_MSK              (0x1U << USB_OTG_INTRUSB_VBUSERR_Pos) /*!< 0x00000080 */
#define USB_OTG_INTRUSB_VBUSERR                  USB_OTG_INTRUSB_VBUSERR_MSK  /*!< VBUS Error request */ 

/*Flags for INTRUSBE register*/
#define USB_OTG_INTRUSBE_SUSPEND_Pos             (0U)
#define USB_OTG_INTRUSBE_SUSPEND_MSK             (0x1U << USB_OTG_INTRUSBE_SUSPEND_Pos) /*!< 0x00000001 */
#define USB_OTG_INTRUSBE_SUSPEND                 USB_OTG_INTRUSBE_SUSPEND_MSK  /*!< Suspend request */ 
#define USB_OTG_INTRUSBE_RESUME_Pos              (1U)
#define USB_OTG_INTRUSBE_RESUME_MSK              (0x1U << USB_OTG_INTRUSBE_RESUME_Pos) /*!< 0x00000002 */
#define USB_OTG_INTRUSBE_RESUME                  USB_OTG_INTRUSBE_RESUME_MSK  /*!< Resume request */ 
#define USB_OTG_INTRUSBE_RSTBABL_Pos             (2U)
#define USB_OTG_INTRUSBE_RSTBABL_MSK             (0x1U << USB_OTG_INTRUSBE_RSTBABL_Pos) /*!< 0x00000004 */
#define USB_OTG_INTRUSBE_RSTBABL                 USB_OTG_INTRUSBE_RSTBABL_MSK  /*!< Reset or Babble request */ 
#define USB_OTG_INTRUSBE_SOF_Pos                 (3U)
#define USB_OTG_INTRUSBE_SOF_MSK                 (0x1U << USB_OTG_INTRUSBE_SOF_Pos) /*!< 0x00000008 */
#define USB_OTG_INTRUSBE_SOF                     USB_OTG_INTRUSBE_SOF_MSK  /*!< SOF request */ 
#define USB_OTG_INTRUSBE_CON_Pos                 (4U)
#define USB_OTG_INTRUSBE_CON_MSK                 (0x1U << USB_OTG_INTRUSBE_CON_Pos) /*!< 0x00000010 */
#define USB_OTG_INTRUSBE_CON                     USB_OTG_INTRUSBE_CON_MSK  /*!< Connect request */ 
#define USB_OTG_INTRUSBE_DISCON_Pos              (5U)
#define USB_OTG_INTRUSBE_DISCON_MSK              (0x1U << USB_OTG_INTRUSBE_DISCON_Pos) /*!< 0x00000020 */
#define USB_OTG_INTRUSBE_DISCON                  USB_OTG_INTRUSBE_DISCON_MSK  /*!< Connect request */ 
#define USB_OTG_INTRUSBE_SESSION_Pos             (6U)
#define USB_OTG_INTRUSBE_SESSION_MSK             (0x1U << USB_OTG_INTRUSBE_SESSION_Pos) /*!< 0x00000040 */
#define USB_OTG_INTRUSBE_SESSION                 USB_OTG_INTRUSBE_SESSION_MSK  /*!< Session request */ 
#define USB_OTG_INTRUSBE_VBUSERR_Pos             (7U)
#define USB_OTG_INTRUSBE_VBUSERR_MSK             (0x1U << USB_OTG_INTRUSBE_VBUSERR_Pos) /*!< 0x00000080 */
#define USB_OTG_INTRUSBE_VBUSERR                 USB_OTG_INTRUSBE_VBUSERR_MSK  /*!< VBUS Error request */ 

/*Flags for CSR0 register*/
#define USB_OTG_CSR0_RXPKTRDY_Pos                (0U)
#define USB_OTG_CSR0_RXPKTRDY_MSK                (0x1U << USB_OTG_CSR0_RXPKTRDY_Pos) /*!< 0x00000001 */
#define USB_OTG_CSR0_RXPKTRDY                    USB_OTG_CSR0_RXPKTRDY_MSK  /*!< Datapacket received*/ 
#define USB_OTG_CSR0_TXPKTRDY_Pos                (1U)
#define USB_OTG_CSR0_TXPKTRDY_MSK                (0x1U << USB_OTG_CSR0_TXPKTRDY_Pos) /*!< 0x00000002 */
#define USB_OTG_CSR0_TXPKTRDY                    USB_OTG_CSR0_TXPKTRDY_MSK  /*!< Packet has been loaded into FIFO*/ 
#define USB_OTG_CSR0_SENTSTALL_Pos               (2U)
#define USB_OTG_CSR0_SENTSTALL_MSK               (0x1U << USB_OTG_CSR0_SENTSTALL_Pos) /*!< 0x00000004 */
#define USB_OTG_CSR0_SENTSTALL                   USB_OTG_CSR0_SENTSTALL_MSK  /*!< STALL handshake is transmitted*/
#define USB_OTG_CSR0_DATAEND_Pos                 (3U)
#define USB_OTG_CSR0_DATAEND_MSK                 (0x1U << USB_OTG_CSR0_DATAEND_Pos) /*!< 0x00000008 */
#define USB_OTG_CSR0_DATAEND                     USB_OTG_CSR0_DATAEND_MSK  /*!< Data transmission end*/
#define USB_OTG_CSR0_SETUPEND_Pos                (4U)
#define USB_OTG_CSR0_SETUPEND_MSK                (0x1U << USB_OTG_CSR0_SETUPEND_Pos) /*!< 0x00000010 */
#define USB_OTG_CSR0_SETUPEND                    USB_OTG_CSR0_SETUPEND_MSK  /*!< Set up end, an execption*/
#define USB_OTG_CSR0_SENDSTALL_Pos               (5U)
#define USB_OTG_CSR0_SENDSTALL_MSK               (0x1U << USB_OTG_CSR0_SENDSTALL_Pos) /*!< 0x00000020 */
#define USB_OTG_CSR0_SENDSTALL                   USB_OTG_CSR0_SENDSTALL_MSK  /*!< Send Stall*/
#define USB_OTG_CSR0_SERVRXPKTRDY_Pos            (6U)
#define USB_OTG_CSR0_SERVRXPKTRDY_MSK            (0x1U << USB_OTG_CSR0_SERVRXPKTRDY_Pos) /*!< 0x00000040 */
#define USB_OTG_CSR0_SERVRXPKTRDY                USB_OTG_CSR0_SERVRXPKTRDY_MSK  /*!< Server RxPktRdy Flag*/
#define USB_OTG_CSR0_SERVSETUPEND_Pos            (7U)
#define USB_OTG_CSR0_SERVSETUPEND_MSK            (0x1U << USB_OTG_CSR0_SERVSETUPEND_Pos) /*!< 0x00000080 */
#define USB_OTG_CSR0_SERVSETUPEND                USB_OTG_CSR0_SERVSETUPEND_MSK  /*!< Server SetupEnd Flag*/

/*Flags for CSR02 register*/
#define USB_OTG_CSR02_FLUSHFIFO_Pos             (0U)
#define USB_OTG_CSR02_FLUSHFIFO_MSK             (0x1U << USB_OTG_CSR02_FLUSHFIFO_Pos) /*!< 0x00000001 */
#define USB_OTG_CSR02_FLUSHFIFO                 USB_OTG_CSR02_FLUSHFIFO_MSK  /*!< FlushFIFO for endpoint 0*/ 
#endif

/**
  * @}
  */ 

/**
  * @}
  */

/** @addtogroup Exported_macros
  * @{
  */

/****************************** QSPI Instances ********************************/
#define IS_QSPI_ALL_INSTANCE(__INSTANCE__) ((__INSTANCE__) == QUADSPI)
/****************************** USB Exported Constants ************************/
#define USB_OTG_FS_HOST_MAX_CHANNEL_NBR                12U
#define USB_OTG_FS_MAX_IN_ENDPOINTS                    6U    /* Including EP0 */
#define USB_OTG_FS_MAX_OUT_ENDPOINTS                   6U    /* Including EP0 */
#define USB_OTG_FS_TOTAL_FIFO_SIZE                     1280U /* in Bytes */
#define USB_OTG_HS_HOST_MAX_CHANNEL_NBR                16U
#define USB_OTG_HS_MAX_IN_ENDPOINTS                    8U    /* Including EP0 */
#define USB_OTG_HS_MAX_OUT_ENDPOINTS                   8U    /* Including EP0 */
#define USB_OTG_HS_TOTAL_FIFO_SIZE                     4096U /* in Bytes */


/******************************************************************************/
/*  For a painless codes migration between the STM32F4xx device product       */
/*  lines, the aliases defined below are put in place to overcome the         */
/*  differences in the interrupt handlers and IRQn definitions.               */
/*  No need to update developed interrupt code when moving across             */
/*  product lines within the same STM32F4 Family                              */
/******************************************************************************/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F469xx_H */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
