/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_otp_dev.h
 *
 * @author        william.wang
 *
 * @version       1.0.0
 *
 * @date          2018/03/20
 *
 * @brief         Header file of otp driver.
 *
 * @note
 *    2018/03/20    william.wang     V1.0.0
 *        Update For GM6721

 */



#ifndef _GM_LL_OTP_DEV_H_
#define _GM_LL_OTP_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __IO  volatile

/* OTP Register Layout Type Definition */
typedef struct
{
    __IO uint32_t MemIO;                     /* OTP Boot Read  0CH Register:offset:0xCC*/
} OTP_MEMIO_T;

/* OTP Register Layout Type Definition */
typedef struct
{
    __IO uint32_t VER;                      /* OTP Version Register:offset: 0x0 */
    __IO uint32_t RESV0[3];                 /* OTP RESV0:offset: 0x04~0x0C */
    __IO uint32_t WM;                      	/* OTP WorkMode Register:offset: 0x10 */
    __IO uint32_t EN;                     	/* OTP Macro Enable Register:offset 0x14 */
    __IO uint32_t CS;                       /* OTP Cell Selection Register:offset: 0x18 */
    __IO uint32_t ADDR;                     /* OTP Address Register:offset: 0x1C */
    __IO uint32_t CTRL;                     /* OTP Control Register:offset: 0x20 */
    __IO uint32_t ST;                       /* OTP Status Register:offset:0x24*/
    __IO uint32_t PDIN;               		/* OTP Program Data Register:offset:0x28 */
    __IO uint32_t RESV1;               		/* OTP RESV1 Register:offset:0x2C */
    __IO uint32_t TIM0;                     /* OTP Timing Part0 Register :offset: 0x30 */
    __IO uint32_t TIM1;               		/* OTP Timint Part1 Register:offset: 0x34 */
    __IO uint32_t RESV2[2];                 /* OTP RESV0:offset: 0x38~0x3C */
    __IO uint32_t BRST;                     /* OTP Boot Read Status Register:offset: 0x40 */
    __IO uint32_t IE;                     	/* OTP Interrupt Register:offset: 0x44 */
    __IO uint32_t RAWINT;                	/* OTP Raw Interrupt Status Register:offset:0x48 */
    __IO uint32_t INT;                		/* OTP Interrupt Status Register:offset:0x4C */
    __IO uint32_t RESV3[12];                /* OTP RESV0:offset: 0x50~0x7C */
    __IO uint32_t BR00;                     /* OTP Boot Read  00H Register:offset:0x80*/
    __IO uint32_t BR04;                     /* OTP Boot Read  04H Register:offset:0x84*/
    __IO uint32_t BR08;                     /* OTP Boot Read  08H Register:offset:0x88*/
    __IO uint32_t BR0C;                     /* OTP Boot Read  0CH Register:offset:0x8C*/
    __IO uint32_t BR10;                     /* OTP Boot Read  00H Register:offset:0x90*/
    __IO uint32_t BR14;                     /* OTP Boot Read  04H Register:offset:0x94*/
    __IO uint32_t BR18;                     /* OTP Boot Read  08H Register:offset:0x98*/
    __IO uint32_t BR1C;                     /* OTP Boot Read  0CH Register:offset:0x9C*/
    __IO uint32_t BR20;                     /* OTP Boot Read  00H Register:offset:0xA0*/
    __IO uint32_t BR24;                     /* OTP Boot Read  04H Register:offset:0xA4*/
    __IO uint32_t BR28;                     /* OTP Boot Read  08H Register:offset:0xA8*/
    __IO uint32_t BR2C;                     /* OTP Boot Read  0CH Register:offset:0xAC*/
    __IO uint32_t BR30;                     /* OTP Boot Read  00H Register:offset:0xB0*/
    __IO uint32_t BR34;                     /* OTP Boot Read  04H Register:offset:0xB4*/
    __IO uint32_t BR38;                     /* OTP Boot Read  08H Register:offset:0xB8*/
    __IO uint32_t BR3C;                     /* OTP Boot Read  0CH Register:offset:0xBC*/
    __IO uint32_t BR40;                     /* OTP Boot Read  00H Register:offset:0xC0*/
    __IO uint32_t BR44;                     /* OTP Boot Read  04H Register:offset:0xC4*/
    __IO uint32_t BR48;                     /* OTP Boot Read  08H Register:offset:0xC8*/
    __IO uint32_t BR4C;                     /* OTP Boot Read  0CH Register:offset:0xCC*/
} OTP_Device_T;

#define OTP_DEVICE_BASE_ADDRESS             (0x1FFFF000)
#define OTP_MEMIO_BASE_ADDRESS				(0x1FFF8000)

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_OTP_DEV_H_ */


