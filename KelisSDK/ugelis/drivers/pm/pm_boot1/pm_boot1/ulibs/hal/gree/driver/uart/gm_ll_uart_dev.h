/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/02/25, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_LL_UART_DEV_H_
#define _GM_LL_UART_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UART Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t DR;                        /* UART Transmit/Recieve Data Register: 0x0 */
    __IO uint32_t TC;                        /* UART Timing Configuration Register: 0x4 */
    __IO uint32_t C1;                        /* UART Control Register 1: 0x8 */
    __IO uint32_t C2;                        /* UART Control Register 2: 0xc */
    __IO uint32_t C3;                        /* UART Control Register 3: 0x10 */
    __IO uint32_t LS;                        /* UART Line Status Register: 0x14 */
    __IO uint32_t TH;                        /* UART RX/TX Threshold Register: 0x18 */
    __IO uint32_t IE;                        /* UART Interrupt Enable Register: 0x1C */
    __IO uint32_t IS;                        /* UART Interrupt Status Register: 0x20 */
    __IO uint32_t DMA;                       /* UART DMA Control Register: 0x24 */
} UART_Device_T;

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_UART_DEV_H_ */
