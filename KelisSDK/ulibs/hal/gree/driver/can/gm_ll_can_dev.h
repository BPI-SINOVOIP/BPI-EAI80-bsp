#ifndef _GM_LL_CAN_DEV_H_
#define _GM_LL_CAN_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include <gm_common.h>



/* CAN control register layout type definition */
typedef struct
{
    __IO uint32_t ID;                                           /* CAN identification register:offest: 0x0 */
    __IO uint32_t RESV0;                                        /* CAN reserver register:offset: 0x04 */
    __IO uint32_t CTRL;                                         /* CAN contral register:offset: 0x8 */
    __IO uint32_t RESV1;                                        /* CAN reserver register:offset: 0x0c */
    __IO uint32_t CFG;                                          /* CAN config register:offset: 0x10 */
    __IO uint32_t IE;                                           /* CAN interrupt enable register:offset: 0x14 */
    __IO uint32_t BUSTIM;                                       /* CAN bus tim register:offset  0x18 */
    __IO uint32_t RESV2;                                        /* CAN reserver register:offset: 0x1c */
//#if (defined(CHIP_GM6611))
//    __IO uint32_t AC0;                                          /* CAN acceptance code0 register:offset: 0x20 */
//    __IO uint32_t AMSK0;                                        /* CAN acceptance mask0 register:offset: 0x24 */
//    __IO uint32_t AC1;                                          /* CAN acceptance code1 register:offset: 0x28 */
//    __IO uint32_t AMSK1;                                        /* CAN acceptance mask1 register:offset: 0x2c */
//#elif (defined(CHIP_GM6611B))
    __IO uint32_t AC;                                           /* CAN acceptance code register:offset: 0x20 */
    __IO uint32_t AMSK;                                         /* CAN acceptance mask register:offset: 0x24 */
    __IO uint32_t ASEL;                                         /* CAN acceptance selection register:offset: 0x28 */
    __IO uint32_t RESV;                                         /* CAN reserved register:offset: 0x2c */
//#endif
    __IO uint32_t EWT;                                          /* CAN error warning threshold register:offset: 0x30 */
    __IO uint32_t RESV3;                                        /* CAN reserver register:offset: 0x34 */
    __IO uint32_t OUTCTRL;                                      /* CAN output control register:offset: 0x38 */
    __IO uint32_t RESV4;                                        /* CAN reserver register:offset: 0x3c */
    __IO uint32_t ST;                                           /* CAN status register:offset: 0x40 */
    __IO uint32_t INTST;                                        /* CAN interrupt register:offset: 0x44 */
    __IO uint32_t RBST;                                         /* CAN receive buffer status register:offset: 0x48 */
    __IO uint32_t RESV5;                                        /* CAN reserver register:offset: 0x4c */
    __IO uint32_t TXFRMINFO;                                    /* CAN TX frame information register:offset: 0x50 */
    __IO uint32_t TXID;                                         /* CAN TX identifier register:offset: 0x54 */
    __IO uint32_t TXDATA0;                                      /* CAN TX data byte part0 register:offset: 0x58 */
    __IO uint32_t TXDATA1;                                      /* CAN TX data byte part1 register:offset: 0x5c */
    __IO uint32_t RXFRMINFO;                                    /* CAN RX frame information register:offset: 0x60 */
    __IO uint32_t RXID;                                         /* CAN RX identifier register:offset: 0x64 */
    __IO uint32_t RXDATA0;                                      /* CAN RX data byte part0 register:offset: 0x68 */
    __IO uint32_t RXDATA1;                                      /* CAN RX data byte part1  register:offset: 0x6c */
    __IO uint32_t ALB;                                          /* CAN arbitration lost bit register:offset: 0x70 */
    __IO uint32_t ERRINFO;                                      /* CAN error information register:offset: 0x74 */
    __IO uint32_t ERRCNT;                                       /* CAN error counter register:offset: 0x78 */
#if (defined(CHIP_GM6611))
    __IO uint32_t RXFIFO[16];                                   /* CAN receive fifo register offset:0x80-0xbc */
#elif (defined(CHIP_GM6611B))
    __IO uint32_t RXFIFO[32];                                   /* CAN receive fifo register offset:0x80-0xfc */
#endif


} CAN_Device_T;


#define CAN0_DEV                                                ((CAN_Device_T *)CAN0_BASE)                            /* CAN device pointer */
#define CAN1_DEV                                                ((CAN_Device_T *)CAN1_BASE)                            /* CAN device pointer */


#ifdef __cplusplus
}
#endif
#endif  /*_GM_LL_CAN_DEV_H_*/
