#ifndef _GM_LL_USB_DEV_H_
#define _GM_LL_USB_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RABBIT_NO_OS_SYSTEM
#include <misc/printk.h>
#endif
#include <gm_common.h>
#include <gm_soc.h>
#include <gm_basic_types.h>

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

    /*The following for GM6721 USB Spec*/
    __IO uint8_t TxMaxP;              /*!< Maximum packet size for peripheral IN endpoint. (Index register set to select Endpoints 1 ~ 15 only) 10h */
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

    __IO uint8_t TxType;           /*!< TxType, only for host mode 18h*/
    union
    {
          __IO uint8_t NAKLmt0;      /*!< NAKLimit0, only for host mode 19h*/      
          __IO uint8_t TxIntv;       /*!< RxIntv, only for host mode 19h*/
    }IntvNAKLmt0_Union;  
    __IO uint8_t RxType;           /*!< RxType, only for host mode 1Ah*/
    __IO uint8_t RxIntv;           /*!< RxIntv, only for host mode 1Bh*/

    __IO uint8_t DyTxFIFO1;        /*!< Dynamic FIFO for TxFIFO 1 0x1C*/
    __IO uint8_t DyTxFIFO2;        /*!< Dynamic FIFO for TxFIFO 2 0x1D*/
    __IO uint8_t DyRxFIFO1;        /*!< Dynamic FIFO for RxFIFO 1 0x1E*/
    union
    {
        __IO uint8_t DyRxFIFO2;      /*!< Dynamic FIFO for RxFIFO 2 0x1F*/
        __IO uint8_t FIFOSize;       /*!< Number of bytes in peripheral OUT endpoint FIFO (upper byte). (Index register set to select Endpoints 1~ 15 only) 1Fh*/
    }FIFO_Union;    

    __IO uint8_t FIFOx[16];       /*!< FIFOs for Endpoints 0 ~ 15. 0x20 ~ */
  
} USB_Otg_T;


#ifdef __cplusplus
}
#endif

#endif
