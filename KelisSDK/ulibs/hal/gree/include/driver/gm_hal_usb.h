#ifndef _GM_HAL_USB_H_
#define _GM_HAL_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <usb/gm_ll_usb_dev.h>


#define USB_CLEAR_AND_DISABLE_INTERRUPTS do {\
                                                  /* Clear all pending Device Interrupts */ \
                                                  pUSB_Otg->IntrTx1; \
                                                  pUSB_Otg->IntrTx2; \
                                                  \
                                                  pUSB_Otg->IntrRx1; \
                                                  pUSB_Otg->IntrRx2; \
                                                  pUSB_Otg->IntrUSB; \
                                                  \
                                                  /*Disable each individual interrupt for Tx endpoints*/ \
                                                  pUSB_Otg->InTrTx1E &= 0x00; \
                                                  pUSB_Otg->InTrTx2E &= 0x00; \
                                                  \
                                                  /*Disable each individual interrupt for Rx endpoints*/ \
                                                  pUSB_Otg->InTrRx1E &= 0x00; \
                                                  pUSB_Otg->InTrRx2E &= 0x00; \
                                          } while(0)

#define USB_NOPS_1 do {\
                      __asm__ __volatile__ ("nop"); \
                      __asm__ __volatile__ ("nop"); \
                      __asm__ __volatile__ ("nop"); \
                      __asm__ __volatile__ ("nop"); \
                      \
                      __asm__ __volatile__ ("nop"); \
                      __asm__ __volatile__ ("nop"); \
                      __asm__ __volatile__ ("nop"); \
                      __asm__ __volatile__ ("nop"); \
                      }while(0)

#define  USB_NOPS_2 do {\
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       __asm__ __volatile__ ("nop"); \
                       }while(0)


/**************Bit definition for USB_OTG_GOTGCTL register *******************/
/*Flags for DEVCTL register*/
#define USB_OTG_DEVCTL_SESSION_Pos               (0U)
#define USB_OTG_DEVCTL_SESSION_Msk               (0x1U << USB_OTG_DEVCTL_SESSION_Pos) /*!< 0x00000001 */
#define USB_OTG_DEVCTL_SESSION                   USB_OTG_DEVCTL_SESSION_Msk
#define USB_OTG_DEVCTL_HOSTMODE_Pos              (2U)
#define USB_OTG_DEVCTL_HOSTMODE_Msk              (0x1U << USB_OTG_DEVCTL_HOSTMODE_Pos) /*!< 0x00000004 */
#define USB_OTG_DEVCTL_HOSTMODE                  USB_OTG_DEVCTL_HOSTMODE_Msk  /*!< USB Mode */ 
#define USB_OTG_DEVCTL_LSDEV_Pos                 (5U)
#define USB_OTG_DEVCTL_LSDEV_Msk                 (0x1U << USB_OTG_DEVCTL_LSDEV_Pos) /*!< 0x00000020 */
#define USB_OTG_DEVCTL_LSDEV                     USB_OTG_DEVCTL_LSDEV_Msk
#define USB_OTG_DEVCTL_FSDEV_Pos                 (6U)
#define USB_OTG_DEVCTL_FSDEV_Msk                 (0x1U << USB_OTG_DEVCTL_FSDEV_Pos) /*!< 0x00000040 */
#define USB_OTG_DEVCTL_FSDEV                     USB_OTG_DEVCTL_FSDEV_Msk


/*Flags for POWER register*/
#define USB_OTG_POWER_RESET_Pos                  (3U)
#define USB_OTG_POWER_RESET_Msk                  (0x1U << USB_OTG_POWER_RESET_Pos) /*!< 0x00000008 */
#define USB_OTG_POWER_RESET                      USB_OTG_POWER_RESET_Msk  /*!< Reset */ 

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


/*Flags for TXCSR1 register for BOTH peripheral & host*/
#define USB_OTG_TXCSR1_TXPKTRDY_Pos              (0U)
#define USB_OTG_TXCSR1_TXPKTRDY_Msk              (0x1U << USB_OTG_TXCSR1_TXPKTRDY_Pos) /*!< 0x00000001 */
#define USB_OTG_TXCSR1_TXPKTRDY                  USB_OTG_TXCSR1_TXPKTRDY_Msk  /*!< Tx Packet Ready */ 
#define USB_OTG_TXCSR1_FIFONOTEMPTY_Pos          (1U)
#define USB_OTG_TXCSR1_FIFONOTEMPTY_Msk          (0x1U << USB_OTG_TXCSR1_FIFONOTEMPTY_Pos) /*!< 0x00000002 */
#define USB_OTG_TXCSR1_FIFONOTEMPTY              USB_OTG_TXCSR1_FIFONOTEMPTY_Msk  /*!< At least 1 packet in Tx FIFO */ 
#define USB_OTG_TXCSR1_FLUSHFIFO_Pos             (3U)
#define USB_OTG_TXCSR1_FLUSHFIFO_Msk             (0x1U << USB_OTG_TXCSR1_FLUSHFIFO_Pos) /*!< 0x00000008 */
#define USB_OTG_TXCSR1_FLUSHFIFO                 USB_OTG_TXCSR1_FLUSHFIFO_Msk  /*!< Flush Tx Endpoint FIFO */ 
#define USB_OTG_TXCSR1_CLRDATATOG_Pos            (6U)
#define USB_OTG_TXCSR1_CLRDATATOG_Msk            (0x1U << USB_OTG_TXCSR1_CLRDATATOG_Pos) /*!< 0x00000040 */
#define USB_OTG_TXCSR1_CLRDATATOG                USB_OTG_TXCSR1_CLRDATATOG_Msk  /*!< Reset endpoint data toggle to 0*/

/*Flags for TXCSR1 register for Peripheral*/
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

/*Flags for TXCSR1 register for Host*/
#define USB_OTG_TXCSR1_ERR_Pos                   (2U)
#define USB_OTG_TXCSR1_ERR_Msk                   (0x1U << USB_OTG_TXCSR1_ERR_Pos) /*!< 0x00000004 */
#define USB_OTG_TXCSR1_ERR                       USB_OTG_TXCSR1_ERR_Msk  /*!< In token received while TxPktRdy not set */ 
#define USB_OTG_TXCSR1_RXSTALL_Pos               (5U)
#define USB_OTG_TXCSR1_RXSTALL_Msk               (0x1U << USB_OTG_TXCSR1_RXSTALL_Pos) /*!< 0x00000004 */
#define USB_OTG_TXCSR1_RXSTALL                   USB_OTG_TXCSR1_RXSTALL_Msk  /*!< In token received while TxPktRdy not set */ 
#define USB_OTG_TXCSR1_NAKTMOUT_Pos              (7U)
#define USB_OTG_TXCSR1_NAKTMOUT_Msk              (0x1U << USB_OTG_TXCSR1_NAKTMOUT_Pos) /*!< 0x00000004 */
#define USB_OTG_TXCSR1_NAKTMOUT                  USB_OTG_TXCSR1_NAKTMOUT_Msk  /*!< In token received while TxPktRdy not set */ 


/*Flags for TXCSR2 register*/
#define USB_OTG_TXCSR2_MODE_Pos                  (5U)
#define USB_OTG_TXCSR2_MODE_Msk                  (0x1U << USB_OTG_TXCSR2_MODE_Pos) /*!< 0x00000020 */
#define USB_OTG_TXCSR2_MODE                      USB_OTG_TXCSR2_MODE_Msk /*!< Enable endpoint direction */
#define USB_OTG_TXCSR2_ISO_Pos                   (6U)
#define USB_OTG_TXCSR2_ISO_Msk                   (0x1U << USB_OTG_TXCSR2_ISO_Pos) /*!< 0x00000040 */
#define USB_OTG_TXCSR2_ISO                        USB_OTG_TXCSR2_ISO_Msk  /*!< Set 1 enable Isochronous protocol*/


/*Flags for RXCSR1 register for BOTH peripheral & host*/
#define USB_OTG_RXCSR1_RXPKTRDY_Pos              (0U)
#define USB_OTG_RXCSR1_RXPKTRDY_Msk              (0x1U << USB_OTG_RXCSR1_RXPKTRDY_Pos) /*!< 0x00000001 */
#define USB_OTG_RXCSR1_RXPKTRDY                  USB_OTG_RXCSR1_RXPKTRDY_Msk  /*!< A data packet (setup packet included) recevied */ 
#define USB_OTG_RXCSR1_FIFOFULL_Pos              (1U)
#define USB_OTG_RXCSR1_FIFOFULL_Msk              (0x1U << USB_OTG_RXCSR1_FIFOFULL_Pos) /*!< 0x00000002 */
#define USB_OTG_RXCSR1_FIFOFULL                  USB_OTG_RXCSR1_FIFOFULL_Msk  /*!< No more packets can loaded into Rx FIFO */ 
#define USB_OTG_RXCSR1_FLUSHFIFO_Pos             (4U)
#define USB_OTG_RXCSR1_FLUSHFIFO_Msk             (0x1U << USB_OTG_RXCSR1_FLUSHFIFO_Pos) /*!< 0x00000010 */
#define USB_OTG_RXCSR1_FLUSHFIFO                  USB_OTG_RXCSR1_FLUSHFIFO_Msk  /*!< Flush RX FIFO */
#define USB_OTG_RXCSR1_CLRDATATOG_Pos            (7U)
#define USB_OTG_RXCSR1_CLRDATATOG_Msk            (0x1U << USB_OTG_RXCSR1_CLRDATATOG_Pos) /*!< 0x00000080 */
#define USB_OTG_RXCSR1_CLRDATATOG                USB_OTG_RXCSR1_CLRDATATOG_Msk  /*!< Reset endpoint data toggle to 0*/


/*Flags for RXCSR1 register for Peripheral*/
#define USB_OTG_RXCSR1_OVERRUN_Pos               (2U)
#define USB_OTG_RXCSR1_OVERRUN_Msk               (0x1U << USB_OTG_RXCSR1_OVERRUN_Pos) /*!< 0x00000004 */
#define USB_OTG_RXCSR1_OVERRUN                   USB_OTG_RXCSR1_OVERRUN_Msk  /*!< No more packets can loaded into Rx FIFO */ 
#define USB_OTG_RXCSR1_DATAERR_Pos               (3U)
#define USB_OTG_RXCSR1_DATAERR_Msk               (0x1U << USB_OTG_RXCSR1_DATAERR_Pos) /*!< 0x00000008 */
#define USB_OTG_RXCSR1_DATAERR                   USB_OTG_RXCSR1_DATAERR_Msk  /*!< CRC or bit stuff error */
#define USB_OTG_RXCSR1_SENDSTLL_Pos              (5U)
#define USB_OTG_RXCSR1_SENDSTLL_Msk              (0x1U << USB_OTG_RXCSR1_SENDSTLL_Pos) /*!< 0x00000020 */
#define USB_OTG_RXCSR1_SENDSTLL                  USB_OTG_RXCSR1_SENDSTLL_Msk  /*!< Send STALL handshake */ 
#define USB_OTG_RXCSR1_SENTSTLL_Pos              (6U)
#define USB_OTG_RXCSR1_SENTSTLL_Msk              (0x1U << USB_OTG_RXCSR1_SENTSTLL_Pos) /*!< 0x00000040 */
#define USB_OTG_RXCSR1_SENTSTLL                  USB_OTG_RXCSR1_SENTSTLL_Msk  /*!< STALL handshake has been transmitted*/ 


/*Flags for RXCSR1 register for Host*/
#define USB_OTG_RXCSR1_ERR_Pos                   (2U)
#define USB_OTG_RXCSR1_ERR_Msk                   (0x1U << USB_OTG_RXCSR1_ERR_Pos) /*!< 0x00000004 */
#define USB_OTG_RXCSR1_ERR                       USB_OTG_RXCSR1_ERR_Msk  /*!< No more packets can loaded into Rx FIFO */ 
#define USB_OTG_RXCSR1_ERRTMOUT_Pos              (3U)
#define USB_OTG_RXCSR1_ERRTMOUT_Msk              (0x1U << USB_OTG_RXCSR1_ERRTMOUT_Pos) /*!< 0x00000008 */
#define USB_OTG_RXCSR1_ERRTMOUT                  USB_OTG_RXCSR1_ERRTMOUT_Msk  /*!< CRC or bit stuff error */
#define USB_OTG_RXCSR1_REQPKT_Pos                (5U)
#define USB_OTG_RXCSR1_REQPKT_Msk                (0x1U << USB_OTG_RXCSR1_REQPKT_Pos) /*!< 0x00000008 */
#define USB_OTG_RXCSR1_REQPKT                    USB_OTG_RXCSR1_REQPKT_Msk  /*!< CRC or bit stuff error */
#define USB_OTG_RXCSR1_RXSTALL_Pos               (6U)
#define USB_OTG_RXCSR1_RXSTALL_Msk               (0x1U << USB_OTG_RXCSR1_RXSTALL_Pos) /*!< 0x00000008 */
#define USB_OTG_RXCSR1_RXSTALL                   USB_OTG_RXCSR1_RXSTALL_Msk  /*!< CRC or bit stuff error */


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

/*Flags for CSR0 register for usb peripheral & host*/
#define USB_OTG_CSR0_RXPKTRDY_Pos                (0U)
#define USB_OTG_CSR0_RXPKTRDY_MSK                (0x1U << USB_OTG_CSR0_RXPKTRDY_Pos) /*!< 0x00000001 */
#define USB_OTG_CSR0_RXPKTRDY                    USB_OTG_CSR0_RXPKTRDY_MSK  /*!< Datapacket received*/ 
#define USB_OTG_CSR0_TXPKTRDY_Pos                (1U)
#define USB_OTG_CSR0_TXPKTRDY_MSK                (0x1U << USB_OTG_CSR0_TXPKTRDY_Pos) /*!< 0x00000002 */
#define USB_OTG_CSR0_TXPKTRDY                    USB_OTG_CSR0_TXPKTRDY_MSK  /*!< Packet has been loaded into FIFO*/ 

/*Flags for CSR0 register for usb peripheral*/
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

/*Flags for CSR0 register for usb host*/
#define USB_OTG_CSR0_RXSTALL_Pos                 (2U)
#define USB_OTG_CSR0_RXSTALL_MSK                 (0x1U << USB_OTG_CSR0_RXSTALL_Pos) /*!< 0x00000004 */
#define USB_OTG_CSR0_RXSTALL                     USB_OTG_CSR0_RXSTALL_MSK  /*!< STALL handshake is received*/
#define USB_OTG_CSR0_SETUPPKT_Pos                (3U)
#define USB_OTG_CSR0_SETUPPKT_MSK                (0x1U << USB_OTG_CSR0_SETUPPKT_Pos) /*!< 0x00000008 */
#define USB_OTG_CSR0_SETUPPKT                    USB_OTG_CSR0_SETUPPKT_MSK  /*!< Setup request*/
#define USB_OTG_CSR0_ERR_Pos                     (4U)
#define USB_OTG_CSR0_ERR_MSK                     (0x1U << USB_OTG_CSR0_ERR_Pos) /*!< 0x00000010 */
#define USB_OTG_CSR0_ERR                         USB_OTG_CSR0_ERR_MSK  /*!< Error In transaction*/
#define USB_OTG_CSR0_REQPKT_Pos                  (5U)
#define USB_OTG_CSR0_REQPKT_MSK                  (0x1U << USB_OTG_CSR0_REQPKT_Pos) /*!< 0x00000020 */
#define USB_OTG_CSR0_REQPKT                      USB_OTG_CSR0_REQPKT_MSK  /*!< Resquest an In transaction*/
#define USB_OTG_CSR0_STATUSPKT_Pos               (6U)
#define USB_OTG_CSR0_STATUSPKT_MSK               (0x1U << USB_OTG_CSR0_STATUSPKT_Pos) /*!< 0x00000040 */
#define USB_OTG_CSR0_STATUSPKT                   USB_OTG_CSR0_STATUSPKT_MSK  /*!< Perform a status stage transaction*/

/*Flags for CSR02 register*/
#define USB_OTG_CSR02_FLUSHFIFO_Pos              (0U)
#define USB_OTG_CSR02_FLUSHFIFO_MSK              (0x1U << USB_OTG_CSR02_FLUSHFIFO_Pos) /*!< 0x00000001 */
#define USB_OTG_CSR02_FLUSHFIFO                  USB_OTG_CSR02_FLUSHFIFO_MSK  /*!< FlushFIFO for endpoint 0*/ 

/* USB - Peripheral instance base addresses */
/** Peripheral USB0 base address */
#define USB0_BASE                                (USB_BASE)
/** Peripheral USB0 base pointer */
#define USB0                                     ((USB_Otg_T *)USB0_BASE)
/** Array initializer of USB peripheral base addresses */
#define USB_BASE_ADDRS                           { USB0_BASE }
/** Array initializer of USB peripheral base pointers */
#define USB_BASE_PTRS                            { USB0 }

/* callback function */
typedef void (*USB_Device_Irq_Callback_t)(void * Callback_Param);

typedef struct _USB_IrqHandler_Callback_Struct
{
    USB_Device_Irq_Callback_t callbackFn;      /*!< USB Irq callback function*/
    void *callbackParam;                       /*!< Parameter for callback function*/    
} USB_IrqHandler_Callback_Struct_T;


/**
 * @brief  Get USB periphral general interrupt status
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval None
 */
static inline uint8_t HAL_USB_GetDeviceIntrStatus(USB_Otg_T *pUSB_Otg)
{
    return pUSB_Otg->IntrUSB;
}

/**
 * @brief  Set USB periphral Address register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  address : USB address assigned by USB host
 *
 * @retval None
 */
static inline void HAL_USB_SetAddress(USB_Otg_T *pUSB_Otg, uint8_t address)
{
    pUSB_Otg->FAddr = address;
}

/**
 * @brief  Clear USB periphral TxCSR1 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for TxCSR1 register
 *
 * @retval None
 */
static inline void HAL_USB_ClearTxCSR1(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->CtrlSts0_Union.TxCSR1 &= ~(bitfield);
}

/**
 * @brief  Set USB periphral TxCSR1 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for TxCSR1 register
 *
 * @retval None
 */
static inline void HAL_USB_SetTxCSR1(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->CtrlSts0_Union.TxCSR1 |= (bitfield);
}

/**
 * @brief  Clear USB periphral TxCSR2 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for TxCSR2 register
 *
 * @retval None
 */
static inline void HAL_USB_ClearTxCSR2(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->CtrlSts1_Union.TxCSR2 &= ~(bitfield);
}

/**
 * @brief  Set USB periphral TxCSR2 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for TxCSR2 register
 *
 * @retval None
 */
static inline void HAL_USB_SetTxCSR2(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->CtrlSts1_Union.TxCSR2 |= (bitfield);
}

/**
 * @brief  Clear USB periphral Power register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for Power register
 *
 * @retval None
 */
static inline void HAL_USB_ClearPower(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->Power &= ~(bitfield);
}

/**
 * @brief  Set USB periphral Power register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for Power register
 *
 * @retval None
 */
static inline void HAL_USB_SetPower(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->Power |= (bitfield);
}

/**
 * @brief  Clear USB periphral CSR0 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for CSR0 register
 *
 * @retval None
 */
static inline void HAL_USB_ClearCSR0(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->CtrlSts0_Union.CSR0 &= ~(bitfield);
}

/**
 * @brief  Set USB periphral CSR0 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for CSR0 register
 *
 * @retval None
 */
static inline void HAL_USB_SetCSR0(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->CtrlSts0_Union.CSR0 |= (bitfield);
}

/**
 * @brief  Set USB periphral RxType register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  rxIntv : RxType value
 *
 * @retval None
 */
static inline void HAL_USB_SetRxType(USB_Otg_T *pUSB_Otg, uint8_t rxType)
{
	pUSB_Otg->RxType = rxType;
}

/**
 * @brief  Set USB periphral TxType register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  rxIntv : TxType value
 *
 * @retval None
 */
static inline void HAL_USB_SetTxType(USB_Otg_T *pUSB_Otg, uint8_t txType)
{
	pUSB_Otg->TxType = txType;
}

/**
 * @brief  Set USB periphral RxIntv register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  rxIntv : RxIntv value
 *
 * @retval None
 */
static inline void HAL_USB_SetRxIntv(USB_Otg_T *pUSB_Otg, uint8_t rxIntv)
{
	pUSB_Otg->RxIntv = rxIntv;
}

/**
 * @brief  Set USB periphral TxIntv register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  txIntv : TxIntv value
 *
 * @retval None
 */
static inline void HAL_USB_SetTxIntv(USB_Otg_T *pUSB_Otg, uint8_t txIntv)
{
	pUSB_Otg->IntvNAKLmt0_Union.TxIntv = txIntv;
}

/**
 * @brief  Get the data count in USB periphral's endpoint FIFO, 
 *         usually HAL_USB_SetIndex should be invoked previously
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval The data count in USB periphral's endpoint FIFO
 */
static inline uint16_t HAL_USB_GetOutEpFifoCnt(USB_Otg_T *pUSB_Otg)
{
	return (pUSB_Otg->RxCount2) << 8 | pUSB_Otg->Count_Union.RxCount1;
}

/**
 * @brief  Get USB periphral's frame count
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval USB periphral's frame count
 */
static inline uint16_t HAL_USB_GetFrameCnt(USB_Otg_T *pUSB_Otg)
{
	return (uint16_t)((pUSB_Otg->Frame2 << 8U) | pUSB_Otg->Frame1);
}

/**
 * @brief  Clear USB periphral NAKLmt0 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  limit : the value for NAKLmt0 register
 *
 * @retval None
 */
static inline void HAL_USB_SetNakLmt0(USB_Otg_T *pUSB_Otg, uint8_t limit)
{
	pUSB_Otg->IntvNAKLmt0_Union.NAKLmt0	= limit;
}

/**
 * @brief  Clear USB periphral RxCSR1 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for RxCSR1 register
 *
 * @retval None
 */
static inline void HAL_USB_ClearRxCSR1(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->RxCSR1 &= ~(bitfield);
}

/**
 * @brief  Set USB periphral RxCSR1 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for RxCSR1 register
 *
 * @retval None
 */
static inline void HAL_USB_SetRxCSR1(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->RxCSR1 |= (bitfield);
}

/**
 * @brief  Clear USB periphral RxCSR2 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for RxCSR2 register
 *
 * @retval None
 */
static inline void HAL_USB_ClearRxCSR2(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->RxCSR2 &= ~(bitfield);
}

/**
 * @brief  Set USB periphral RxCSR2 register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  bitfield : the bitfield for RxCSR2 register
 *
 * @retval None
 */
static inline void HAL_USB_SetRxCSR2(USB_Otg_T *pUSB_Otg, uint8_t bitfield)
{
	pUSB_Otg->RxCSR2 |= (bitfield);
}

/**
 * @brief  Set USB periphral Index register
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  index : index value
 *
 * @retval None
 */
static inline void HAL_USB_SetIndex(USB_Otg_T *pUSB_Otg, uint8_t index)
{
	pUSB_Otg->Index = index;
}

/**
 * @brief  Return the value of USB periphral Inded register
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval The value of USB periphral Inded register
 */
static inline uint8_t HAL_USB_GetIndex(USB_Otg_T *pUSB_Otg)
{			
	return pUSB_Otg->Index;
}

/**
 * @brief  Return the value of USB periphral DevCtrl register
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval The value of USB periphral DevCtrl register
 */
static inline uint8_t HAL_USB_GetDevCtrl(USB_Otg_T *pUSB_Otg)
{
	return pUSB_Otg->DevCtl;
}

/**
 * @brief  Return the USB peripheral OUT endpoints interrupt status
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval USB all Out Enpoints interrupt status
 */
static inline uint16_t USB_ReadDevAllOutEpInterrupt (USB_Otg_T *pUSB_Otg)
{
    uint16_t intr;
    
    intr = pUSB_Otg->IntrRx1 | (pUSB_Otg->IntrRx2 << 8);
    return intr;
}

/**
  * @brief  Return the USB peripheral IN endpoints interrupt status
  *
  * @param  pUSB_Otg : USB OTG base address
  *
  * @retval USB all In Enpoints interrupt status
  */
static inline uint16_t USB_ReadDevAllInEpInterrupt (USB_Otg_T *pUSB_Otg)
{
  uint16_t intr;
  
  intr = pUSB_Otg->IntrTx1 | (pUSB_Otg->IntrTx2 << 8);
  return intr;
}

/**
 * @brief  Allocate USB internal sram as dynamic mode
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval None
 */
void HAL_USB_SetDynamicSize(USB_Otg_T *pUSB_Otg);

/**
 * @brief  Flush a USB endpoint or all endpoints TxFIFO
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  num : endpoint number
 *         This parameter can be a value from 0 to 3, or 16
 *
 * @retval None
 */
void HAL_USB_FlushTxFifo(USB_Otg_T *pUSB_Otg, uint32_t num);

/**
 * @brief  Flush a USB endpoint or all endpoints RxFIFO
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  num : endpoint number
 *         This parameter can be a value from 0 to 3, or 16
 *
 * @retval None
 */
void HAL_USB_FlushRxFifo(USB_Otg_T *pUSB_Otg, uint32_t num);

/**
 * @brief  Clear RxPkt Ready for USB endpoint
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval None
 */
void HAL_USB_ClearEndpointRxPktRdy(USB_Otg_T *pUSB_Otg, uint8_t ep);

/**
 * @brief  Set TxPkt Ready for USB endpoint
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 * @param  dataend : indication of the last data written to endpoint's FIFO
 *         and it's valid only for USB Control Enpoint
 *
 * @retval None
 */
void HAL_USB_SetEndpointxTxPktRdy(USB_Otg_T *pUSB_Otg, uint8_t ep, uint8_t dataend);

/**
 * @brief  Wirte a length of data to USB endpoint's FIFO
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  src : the buffer pointer to data
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 * @param  len : the length of data to be written
 *
 * @retval None
 */
void HAL_USB_WritePacket(USB_Otg_T *pUSB_Otg, uint8_t *src, uint8_t ep, uint16_t len);

/**
 * @brief  Read a length of data from USB endpoint's FIFO
 *
 * @param  pUSB_Otg : USB OTG base address 
 * @param  dest : the buffer pointer to data
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 * @param  len : the length of data to be read
 *
 * @retval None
 */
uint8_t HAL_USB_ReadPacket(USB_Otg_T *pUSB_Otg, uint8_t *dest, uint8_t ep, uint16_t len);

/**
 * @brief  Initialize USB OTG peripheral
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval None
 */
void HAL_USB_InitializeDevice(USB_Otg_T *pUSB_Otg);

/**
 * @brief  Set callback function and its parameter for usb-irq handler
 *
 * @param  callbackFn : usb-irq callback function
 * @param  callbackParam : parameter of usb-irq callback function
 *
 * @retval None
 */
void HAL_USB_SetIrqCallback(USB_Device_Irq_Callback_t callbackFn, void *callbackParam);

/**
 * @brief  Enable a USB endpoint's general interrupt
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval None
 */
void HAL_USB_EnableEndpoints(USB_Otg_T *pUSB_Otg, uint8_t ep);

/**
 * @brief  Disable a USB endpoint's general interrupt
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval None
 */
void HAL_USB_DisableEndpoints(USB_Otg_T *pUSB_Otg, uint8_t ep);

/**
 * @brief  De-initialize USB OTG peripheral
 *
 * @param  pUSB_Otg : USB OTG base address
 *
 * @retval None
 */
void HAL_USB_DeInitializeDevice(USB_Otg_T *pUSB_Otg);

/**
 * @brief  Returns a USB In-Endpoint Interrupt
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval USB In-Endpoint interrupt status
 */
uint32_t HAL_USB_ReadDevInEPInterrupt (USB_Otg_T *pUSB_Otg, uint8_t ep);

/**
 * @brief  Returns a USB Out-Endpoint Interrupt
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval USB Out-Endpoint interrupt status
 */
uint32_t HAL_USB_ReadDevOutEPInterrupt (USB_Otg_T *pUSB_Otg, uint8_t ep);

/**
 * @brief  Stall an Endpoint data-transaction
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval None
 */
void HAL_USB_EndpointsStall(USB_Otg_T *pUSB_Otg, uint8_t ep);

/**
 * @brief  Unstall an Endpoint data-transaction
 *
 * @param  pUSB_Otg : USB OTG base address
 * @param  ep : endpoint number
 *         This parameter can be a value from 0 to 3
 *
 * @retval None
 */
void HAL_USB_EndpointsUnstall(USB_Otg_T *pUSB_Otg, uint8_t ep);



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_VIN_H_ */
