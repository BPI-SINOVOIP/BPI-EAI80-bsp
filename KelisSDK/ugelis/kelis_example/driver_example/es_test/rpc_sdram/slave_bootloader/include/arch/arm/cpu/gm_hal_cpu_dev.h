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
#ifndef _GM_HAL_UART_DEV_H_
#define _GM_HAL_UART_DEV_H_
#include <gm_common.h>
#include <gm_soc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    __IO uint32_t MAP;                         /* CPU Bus Map Data Register: 0x00 */
    __IO uint32_t CACHECTR;                       /* CPU Resver Register: 0x04 */
    __IO uint32_t MWR;                         /* CPU Set Mail Box Register: 0x08 */
    __IO uint32_t MRD;                         /* CPU Get Mail Box Register: 0x0c */
    __IO uint32_t RESV1[4];                    /* CPU Resver Register: 0x10 - 0x1c */
    __IO uint32_t ID;                          /* CPU ID Register: 0x20 */
} CPU_Device_T;

#define CPU_DEV                                ((CPU_Device_T *)(CPU_BASE))    /* CPU reg base */



#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_UART_DEV_H_ */
