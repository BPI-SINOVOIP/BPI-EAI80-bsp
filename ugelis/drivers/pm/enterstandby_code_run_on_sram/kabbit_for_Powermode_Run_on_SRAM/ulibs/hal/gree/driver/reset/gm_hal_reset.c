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


#include <gm_soc.h>
#include <gm_hal_reset.h>
#include "gm_ll_reset.h"

static lock_t g_lock1;
static lock_t g_lock2;

RET_CODE HAL_Reset_Module(RESET_IP_T IP)
{
    RET_CODE ret = RET_OK;
    ret = HAL_TryLock(&g_lock1);
    if (ret)
    {
        return ret;
    }
    switch (IP)
    {
        /*IPSWRST0*/
        case RESET_SM4_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_SM4_SET_MASK);
            break;
        case RESET_GPIO_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_GPIO_SET_MASK);
            break;
        case RESET_DMA_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_DMA_SET_MASK);
            break;
        case RESET_CRC_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_CRC_SET_MASK);
            break;
        case RESET_TRNG_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_TRNG_SET_MASK);
            break;
        case RESET_CRP_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_CRP_SET_MASK);
            break;
        case RESET_NF_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_NF_SET_MASK);
            break;
        case RESET_SDR_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_SDR_SET_MASK);
            break;
        case RESET_BKD_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE_CLEAR(SYS_DEV, SYS_IP_SW_RESET_BKD_SET_MASK);
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_BKD_SET_MASK);
            while (!LL_RESET_BKD(SYS_DEV, SYS_RESET_STATUS_BKD_RST_SET_MASK));
            break;
        case RESET_LPSTM_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_LPSTM_SET_MASK);
            break;
        case RESET_ACMP_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_ACMP_SET_MASK);
            break;
        case RESET_DAC_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_DAC_SET_MASK);
            break;
        case RESET_ADC_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_ADC_SET_MASK);
            break;
        case RESET_STF_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_STF_SET_MASK);
            break;
        case RESET_KDPSRAM_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_KDPSRAM_SET_MASK);
            break;
        case RESET_KDP_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_KDP_SET_MASK);
            break;
        case RESET_IMP_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_IMP_SET_MASK);
            break;
        case RESET_VIN_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE_CLEAR(SYS_DEV, SYS_IP_SW_RESET_VIN_SET_MASK);
            while (LL_RESET_VIN_CLEAR_STATUS(SYS_DEV, SYS_IP_SW_RESET_VIN_SET_MASK));
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_VIN_SET_MASK);
            break;
        case RESET_VOUT_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_VOUT_SET_MASK);
            break;
        case RESET_USB_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_USB_SET_MASK);
            break;
        case RESET_SDMMC_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_SDMMC_SET_MASK);
            break;
        case RESET_CAN1_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_CAN1_SET_MASK);
            break;
        case RESET_CAN2_SW_RSTJ:
            LL_RESET_IPSWRST0_MODULE(SYS_DEV, SYS_IP_SW_RESET_CAN2_SET_MASK);
            break;
        /*IPSWRST1*/
        case RESET_SPI1_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SPI1_SET_MASK);
            break;
        case RESET_SPI2_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SPI2_SET_MASK);
            break;
        case RESET_SPI3_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SPI3_SET_MASK);
            break;
        case RESET_SPI4_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SPI4_SET_MASK);
            break;
        case RESET_SPI5_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SPI5_SET_MASK);
            break;
        case RESET_SPI6_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SPI6_SET_MASK);
            break;
        case RESET_I2C1_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_I2C1_SET_MASK);
            break;
        case RESET_I2C2_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_I2C2_SET_MASK);
            break;
        case RESET_I2C3_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_I2C3_SET_MASK);
            break;
        case RESET_BTM1_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_BTM1_SET_MASK);
            break;
        case RESET_BTM2_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_BTM2_SET_MASK);
            break;
        case RESET_SAIA_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SAIA_SET_MASK);
            break;
        case RESET_SAIB_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_SAIB_SET_MASK);
            break;
        case RESET_PMEM_SW_RSTJ:
            LL_RESET_IPSWRST1_MODULE(SYS_DEV, SYS_IP_SW_RESET_PMEM_SET_MASK);
            break;
        /*IPSWRST2*/
        case RESET_UART1_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART1_SET_MASK);
            break;
        case RESET_UART2_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART2_SET_MASK);
            break;
        case RESET_UART3_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART3_SET_MASK);
            break;
        case RESET_UART4_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART4_SET_MASK);
            break;
        case RESET_UART5_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART5_SET_MASK);
            break;
        case RESET_UART6_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART6_SET_MASK);
            break;
        case RESET_UART7_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART7_SET_MASK);
            break;
        case RESET_UART8_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_UART8_SET_MASK);
            break;
        case RESET_TIM1_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM1_SET_MASK);
            break;
        case RESET_TIM2_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM2_SET_MASK);
            break;
        case RESET_TIM3_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM3_SET_MASK);
            break;
        case RESET_TIM4_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM4_SET_MASK);
            break;
        case RESET_TIM5_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM5_SET_MASK);
            break;
        case RESET_TIM8_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM8_SET_MASK);
            break;
        case RESET_TIM9_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM9_SET_MASK);
            break;
        case RESET_TIM10_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM10_SET_MASK);
            break;
        case RESET_TIM11_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM11_SET_MASK);
            break;
        case RESET_TIM12_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM12_SET_MASK);
            break;
        case RESET_TIM13_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM13_SET_MASK);
            break;
        case RESET_TIM14_SW_RSTJ:
            LL_RESET_IPSWRST2_MODULE(SYS_DEV, SYS_IP_SW_RESET_TIM14_SET_MASK);
            break;
        default:
            break;
    }
    HAL_UnLock(&g_lock1);
    return RET_OK;
}

RET_CODE HAL_Reset_ClearRestCause(RESET_Cause_T cause)
{
    RET_CODE ret = RET_OK;
    HAL_TryLock(&g_lock2);
    if (ret)
    {
        return ret;
    }
    SYS_DEV->SRS = cause;
    HAL_UnLock(&g_lock2);
    return RET_OK;
}














