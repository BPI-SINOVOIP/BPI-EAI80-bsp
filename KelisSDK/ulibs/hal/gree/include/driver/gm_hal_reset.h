/**
*
* Copyright (C) 2019 Gree Microelectronics.  All Rights Reserved.
*
* @file          gm_hal_reset.h
*
* @author        chris.xie
*
* @version       1.0.0
*
* @date          2016/07/04
*
* @brief         Header file of reset driver.
*
* @note
*    2019/03/27, chris.xie, V1.0.0
*        Initial version.
*/

#ifndef _RESET_HAL_H_
#define _RESET_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_soc.h>
#include <soc_gm6721x.h>
#include <reset/gm_ll_reset.h>
#include <rtc/gm_ll_rtc.h>


typedef enum
{
    RESET_STATUS_POR_VOLTAGE         = 1,
    RESET_STATUS_WATCH_DOG           = (1 << 1),
    RESET_STATUS_LOCKUP              = (1 << 2),
    RESET_STATUS_SYSRESETREQ         = (1 << 3),
    RESET_STATUS_SDP_PENDING         = (1 << 4),
    RESET_STATUS_IWATCH_DOG          = (1 << 5),
    RESET_STATUS_BKD_RST             = (1 << 6),
    RESET_STATUS_STOP2_MODE          = (1 << 8),
    RESET_STATUS_STANDBY_MODE        = (1 << 9),
    RESET_STATUS_SHUT_DOWN_MODE      = (1 << 10),
    RESET_STATUS_SHUT_HOSC_LOSC_FLAG = (1 << 24),
    RESET_STATUS_PES_WK_PENDING      = (1 << 25),
    RESET_STATUS_IO_XPA0_PENDING     = (1 << 26),
} RESET_Cause_T;



typedef enum
{
    /*IPSWRST0*/
    RESET_SM4_SW_RSTJ   = 0,
    RESET_GPIO_SW_RSTJ,
    RESET_DMA_SW_RSTJ,
    RESET_CRC_SW_RSTJ,
    RESET_TRNG_SW_RSTJ,
    RESET_CRP_SW_RSTJ,
    RESET_NF_SW_RSTJ,
    RESET_SDR_SW_RSTJ,
    RESET_BKD_SW_RSTJ,
    RESET_LPSTM_SW_RSTJ,
    RESET_ACMP_SW_RSTJ,
    RESET_DAC_SW_RSTJ,
    RESET_ADC_SW_RSTJ,
    RESET_STF_SW_RSTJ,
    RESET_KDPSRAM_SW_RSTJ,
    RESET_KDP_SW_RSTJ,
    RESET_IMP_SW_RSTJ,
    RESET_VIN_SW_RSTJ,
    RESET_VOUT_SW_RSTJ,
    RESET_USB_SW_RSTJ,
    RESET_SDMMC_SW_RSTJ,
    RESET_CAN1_SW_RSTJ,
    RESET_CAN2_SW_RSTJ,
    /*IPSWRST1*/
    RESET_SPI1_SW_RSTJ,
    RESET_SPI2_SW_RSTJ,
    RESET_SPI3_SW_RSTJ,
    RESET_SPI4_SW_RSTJ,
    RESET_SPI5_SW_RSTJ,
    RESET_SPI6_SW_RSTJ,
    RESET_I2C1_SW_RSTJ,
    RESET_I2C2_SW_RSTJ,
    RESET_I2C3_SW_RSTJ,
    RESET_BTM1_SW_RSTJ,
    RESET_BTM2_SW_RSTJ,
    RESET_SAIA_SW_RSTJ,
    RESET_SAIB_SW_RSTJ,
    RESET_PMEM_SW_RSTJ,
    /*IPSWRST2*/
    RESET_UART1_SW_RSTJ,
    RESET_UART2_SW_RSTJ,
    RESET_UART3_SW_RSTJ,
    RESET_UART4_SW_RSTJ,
    RESET_UART5_SW_RSTJ,
    RESET_UART6_SW_RSTJ,
    RESET_UART7_SW_RSTJ,
    RESET_UART8_SW_RSTJ,
    RESET_TIM1_SW_RSTJ,
    RESET_TIM2_SW_RSTJ,
    RESET_TIM3_SW_RSTJ,
    RESET_TIM4_SW_RSTJ,
    RESET_TIM5_SW_RSTJ,
    RESET_TIM8_SW_RSTJ,
    RESET_TIM9_SW_RSTJ,
    RESET_TIM10_SW_RSTJ,
    RESET_TIM11_SW_RSTJ,
    RESET_TIM12_SW_RSTJ,
    RESET_TIM13_SW_RSTJ,
    RESET_TIM14_SW_RSTJ,

} RESET_IP_T;



#ifdef RABBIT_NO_OS_SYSTEM
static __inline uint8_t HAL_Reset_CheckRestCause(RESET_Cause_T cause)
{
    return  !!(SYS_DEV->SRS & cause);
}
#else
static inline uint8_t HAL_Reset_CheckRestCause(RESET_Cause_T cause)
{
    return  !!(SYS_DEV->SRS & cause);
}
#endif

/**
 * @Reset cause
 *
 * @param  IP Name
 *
 * @retval NULL
 */

RET_CODE HAL_Reset_ClearRestCause(RESET_Cause_T cause);


/**
 * @IP Reset Release
 *
 * @param  IP Name
 *
 * @retval NULL
 */
RET_CODE HAL_Reset_Module(RESET_IP_T IP);


#ifdef __cplusplus
}
#endif

#endif /*_RESET_HAL_H_*/
