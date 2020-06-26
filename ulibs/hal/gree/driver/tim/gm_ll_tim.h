/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          tim_dev.h
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2018/03/19
 *
 * @brief         TIM device header file
 *
 * @note
 *    2018/03/19, edward.zhu, V1.0.0
 *        Initial version.
 *    2019/4/1,   edward.zhu, V1.1.0
 *        second version.
 */
#ifndef _TIM_DEV_H_
#define _TIM_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_tim_dev.h"
#include "gm_hal_dma.h"
#include "gm_hal_acmp.h"
#include "gm_hal_pes.h"
#include <gm_soc.h>
#include <gm_basic_types.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(CHIP_GM6721)
#define MAX_TIM_NUM       14
#endif

#if defined(CHIP_F6721B)
#define MAX_TIM_NUM       8
#endif

#define USE_TIM_HAL_TEST

/*************** TIM Control1 Register Bitfiled ******************/

#define TIM_CR1_CKD_POS                ((uint32_t) 8)
#define TIM_CR1_CKD                    ((uint32_t) ((uint32_t)3 << TIM_CR1_CKD_POS))
#define TIM_CR1_CKD_0                  ((uint32_t) ((uint32_t)1 << TIM_CR1_CKD_POS))
#define TIM_CR1_CKD_1                  ((uint32_t) ((uint32_t)1 << TIM_CR1_CKD_POS))

#define TIM_CR1_ARPE_POS               ((uint32_t) 7)
#define TIM_CR1_ARPE                   ((uint32_t) ((uint32_t)1 << TIM_CR1_ARPE_POS))

#define TIM_CR1_CMS_POS                ((uint32_t) 5)
#define TIM_CR1_CMS                    ((uint32_t) ((uint32_t)3 << TIM_CR1_CMS_POS))
#define TIM_CR1_CMS_0                  ((uint32_t) ((uint32_t)1 << TIM_CR1_CMS_POS))
#define TIM_CR1_CMS_1                  ((uint32_t) ((uint32_t)2 << TIM_CR1_CMS_POS))

#define TIM_CR1_DIR_POS                ((uint32_t) 4)
#define TIM_CR1_DIR                    ((uint32_t) ((uint32_t)1 << TIM_CR1_DIR_POS))

#define TIM_CR1_OPM_POS                ((uint32_t) 3)
#define TIM_CR1_OPM                    ((uint32_t) ((uint32_t)1 << TIM_CR1_OPM_POS))

#define TIM_CR1_URS_POS                ((uint32_t) 2)
#define TIM_CR1_URS                    ((uint32_t) ((uint32_t)1 << TIM_CR1_URS_POS))

#define TIM_CR1_UDIS_POS               ((uint32_t) 1)
#define TIM_CR1_UDIS                   ((uint32_t) ((uint32_t)1 << TIM_CR1_UDIS_POS))

#define TIM_CR1_CEN_POS                ((uint32_t) 0)
#define TIM_CR1_CEN                    ((uint32_t) ((uint32_t)1 << TIM_CR1_CEN_POS))


/*************** TIM Control2 Register Bitfiled ******************/

#define TIM_CR2_OIS4N_POS              ((uint32_t) 15)
#define TIM_CR2_OIS4N                  ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS4N_POS))

#define TIM_CR2_OIS4_POS               ((uint32_t) 14)
#define TIM_CR2_OIS4                   ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS4_POS))

#define TIM_CR2_OIS3N_POS              ((uint32_t) 13)
#define TIM_CR2_OIS3N                  ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS3N_POS))

#define TIM_CR2_OIS3_POS               ((uint32_t) 12)
#define TIM_CR2_OIS3                   ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS3_POS))

#define TIM_CR2_OIS2N_POS              ((uint32_t) 11)
#define TIM_CR2_OIS2N                  ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS2N_POS))

#define TIM_CR2_OIS2_POS               ((uint32_t) 10)
#define TIM_CR2_OIS2                   ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS2_POS))

#define TIM_CR2_OIS1N_POS              ((uint32_t) 9)
#define TIM_CR2_OIS1N                  ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS1N_POS))

#define TIM_CR2_OIS1_POS               ((uint32_t) 8)
#define TIM_CR2_OIS1                   ((uint32_t) ((uint32_t)1 << TIM_CR2_OIS1_POS))

#define TIM_CR2_TI1S_POS               ((uint32_t) 7)
#define TIM_CR2_TI1S                   ((uint32_t) ((uint32_t)1 << TIM_CR2_TI1S_POS))

#define TIM_CR2_MMS_POS                ((uint32_t) 4)
#define TIM_CR2_MMS                    ((uint32_t) ((uint32_t)7 << TIM_CR2_MMS_POS))
#define TIM_CR2_MMS_0                  ((uint32_t) ((uint32_t)1 << TIM_CR2_MMS_POS))
#define TIM_CR2_MMS_1                  ((uint32_t) ((uint32_t)2 << TIM_CR2_MMS_POS))
#define TIM_CR2_MMS_2                  ((uint32_t) ((uint32_t)4 << TIM_CR2_MMS_POS))

#define TIM_CR2_CCDS_POS               ((uint32_t) 3)
#define TIM_CR2_CCDS                   ((uint32_t) ((uint32_t)1 << TIM_CR2_CCDS_POS))

#define TIM_CR2_CCUS_POS               ((uint32_t) 2)
#define TIM_CR2_CCUS                   ((uint32_t) ((uint32_t)1 << TIM_CR2_CCUS_POS))

#define TIM_CR2_CCPC_POS               ((uint32_t) 0)
#define TIM_CR2_CCPC                   ((uint32_t) ((uint32_t)1 << TIM_CR2_CCPC_POS))

#define TIM_CR2_CCPC_POS               ((uint32_t) 0)
#define TIM_CR2_CCPC                   ((uint32_t) ((uint32_t)1 << TIM_CR2_CCPC_POS))


/*************** TIM Slave Mode Control Register Bitfiled ******************/

#define TIM_SMCR_ETP_POS               ((uint32_t) 15)
#define TIM_SMCR_ETP                   ((uint32_t) ((uint32_t)1 << TIM_SMCR_ETP_POS))

#define TIM_SMCR_ECE_POS               ((uint32_t) 14)
#define TIM_SMCR_ECE                   ((uint32_t) ((uint32_t)1 << TIM_SMCR_ECE_POS))

#define TIM_SMCR_ETPS_POS              ((uint32_t) 12)
#define TIM_SMCR_ETPS                  ((uint32_t) ((uint32_t)3 << TIM_SMCR_ETPS_POS))
#define TIM_SMCR_ETPS_0                ((uint32_t) ((uint32_t)1 << TIM_SMCR_ETPS_POS))
#define TIM_SMCR_ETPS_1                ((uint32_t) ((uint32_t)2 << TIM_SMCR_ETPS_POS))

#define TIM_SMCR_ETF_POS               ((uint32_t) 8)
#define TIM_SMCR_ETF                   ((uint32_t) ((uint32_t)0xf << TIM_SMCR_ETF_POS))
#define TIM_SMCR_ETF_0                 ((uint32_t) ((uint32_t)1 << TIM_SMCR_ETF_POS))
#define TIM_SMCR_ETF_1                 ((uint32_t) ((uint32_t)2 << TIM_SMCR_ETF_POS))
#define TIM_SMCR_ETF_2                 ((uint32_t) ((uint32_t)4 << TIM_SMCR_ETF_POS))
#define TIM_SMCR_ETF_3                 ((uint32_t) ((uint32_t)8 << TIM_SMCR_ETF_POS))


#define TIM_SMCR_MSM_POS               ((uint32_t) 7)
#define TIM_SMCR_MSM                   ((uint32_t) ((uint32_t)1 << TIM_SMCR_MSM_POS))

#define TIM_SMCR_TS_POS                ((uint32_t) 4)
#define TIM_SMCR_TS                    ((uint32_t) ((uint32_t)7 << TIM_SMCR_TS_POS))
#define TIM_SMCR_TS_0                  ((uint32_t) ((uint32_t)1 << TIM_SMCR_TS_POS))
#define TIM_SMCR_TS_1                  ((uint32_t) ((uint32_t)2 << TIM_SMCR_TS_POS))
#define TIM_SMCR_TS_2                  ((uint32_t) ((uint32_t)4 << TIM_SMCR_TS_POS))

#define TIM_SMCR_SMS_POS               ((uint32_t) 0)
#define TIM_SMCR_SMS                   ((uint32_t) ((uint32_t)7 << TIM_SMCR_SMS_POS))
#define TIM_SMCR_SMS_0                 ((uint32_t) ((uint32_t)1 << TIM_SMCR_SMS_POS))
#define TIM_SMCR_SMS_1                 ((uint32_t) ((uint32_t)2 << TIM_SMCR_SMS_POS))
#define TIM_SMCR_SMS_2                 ((uint32_t) ((uint32_t)4 << TIM_SMCR_SMS_POS))


/*************** TIM DMA/Interrupt Enable Register Bitfiled ******************/

#define TIM_DIER_TDE_POS               ((uint32_t) 14)
#define TIM_DIER_TDE                   ((uint32_t) ((uint32_t)1 << TIM_DIER_TDE_POS))

#define TIM_DIER_COMDE_POS             ((uint32_t) 13)
#define TIM_DIER_COMDE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_COMDE_POS))

#define TIM_DIER_CC4DE_POS             ((uint32_t) 12)
#define TIM_DIER_CC4DE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC4DE_POS))

#define TIM_DIER_CC3DE_POS             ((uint32_t) 11)
#define TIM_DIER_CC3DE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC3DE_POS))

#define TIM_DIER_CC2DE_POS             ((uint32_t) 10)
#define TIM_DIER_CC2DE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC2DE_POS))

#define TIM_DIER_CC1DE_POS             ((uint32_t) 9)
#define TIM_DIER_CC1DE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC1DE_POS))

#define TIM_DIER_UDE_POS               ((uint32_t) 8)
#define TIM_DIER_UDE                   ((uint32_t) ((uint32_t)1 << TIM_DIER_UDE_POS))

#define TIM_DIER_BIE_POS               ((uint32_t) 7)
#define TIM_DIER_BIE                   ((uint32_t) ((uint32_t)1 << TIM_DIER_BIE_POS))

#define TIM_DIER_TIE_POS               ((uint32_t) 6)
#define TIM_DIER_TIE                   ((uint32_t) ((uint32_t)1 << TIM_DIER_TIE_POS))

#define TIM_DIER_COMIE_POS             ((uint32_t) 5)
#define TIM_DIER_COMIE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_COMIE_POS))

#define TIM_DIER_CC4IE_POS             ((uint32_t) 4)
#define TIM_DIER_CC4IE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC4IE_POS))

#define TIM_DIER_CC3IE_POS             ((uint32_t) 3)
#define TIM_DIER_CC3IE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC3IE_POS))

#define TIM_DIER_CC2IE_POS             ((uint32_t) 2)
#define TIM_DIER_CC2IE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC2IE_POS))

#define TIM_DIER_CC1IE_POS             ((uint32_t) 1)
#define TIM_DIER_CC1IE                 ((uint32_t) ((uint32_t)1 << TIM_DIER_CC1IE_POS))

#define TIM_DIER_UIE_POS               ((uint32_t) 0)
#define TIM_DIER_UIE                   ((uint32_t) ((uint32_t)1 << TIM_DIER_UIE_POS))


/*************** TIM Status Register Bitfiled ******************/

#define TIM_SR_CC4OF_POS               ((uint32_t) 12)
#define TIM_SR_CC4OF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC4OF_POS))

#define TIM_SR_CC3OF_POS               ((uint32_t) 11)
#define TIM_SR_CC3OF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC3OF_POS))

#define TIM_SR_CC2OF_POS               ((uint32_t) 10)
#define TIM_SR_CC2OF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC2OF_POS))

#define TIM_SR_CC1OF_POS               ((uint32_t) 9)
#define TIM_SR_CC1OF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC1OF_POS))

#define TIM_SR_BIF_POS                 ((uint32_t) 7)
#define TIM_SR_BIF                     ((uint32_t) ((uint32_t)1 << TIM_SR_BIF_POS))

#define TIM_SR_TIF_POS                 ((uint32_t) 6)
#define TIM_SR_TIF                     ((uint32_t) ((uint32_t)1 << TIM_SR_TIF_POS))

#define TIM_SR_COMIF_POS               ((uint32_t) 5)
#define TIM_SR_COMIF                   ((uint32_t) ((uint32_t)1 << TIM_SR_COMIF_POS))

#define TIM_SR_CC4IF_POS               ((uint32_t) 4)
#define TIM_SR_CC4IF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC4IF_POS))

#define TIM_SR_CC3IF_POS               ((uint32_t) 3)
#define TIM_SR_CC3IF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC3IF_POS))

#define TIM_SR_CC2IF_POS               ((uint32_t) 2)
#define TIM_SR_CC2IF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC2IF_POS))

#define TIM_SR_CC1IF_POS               ((uint32_t) 1)
#define TIM_SR_CC1IF                   ((uint32_t) ((uint32_t)1 << TIM_SR_CC1IF_POS))

#define TIM_SR_UIF_POS                 ((uint32_t) 0)
#define TIM_SR_UIF                     ((uint32_t) ((uint32_t)1 << TIM_SR_UIF_POS))


/*************** TIM Event Generation Register Bitfiled ******************/

#define TIM_EGR_BG_POS                 ((uint32_t) 7)
#define TIM_EGR_BG                     ((uint32_t) ((uint32_t)1 << TIM_EGR_BG_POS))

#define TIM_EGR_TG_POS                 ((uint32_t) 6)
#define TIM_EGR_TG                     ((uint32_t) ((uint32_t)1 << TIM_EGR_TG_POS))

#define TIM_EGR_COMG_POS               ((uint32_t) 5)
#define TIM_EGR_COMG                   ((uint32_t) ((uint32_t)1 << TIM_EGR_COMG_POS))

#define TIM_EGR_CC4G_POS               ((uint32_t) 4)
#define TIM_EGR_CC4G                   ((uint32_t) ((uint32_t)1 << TIM_EGR_CC4G_POS))

#define TIM_EGR_CC3G_POS               ((uint32_t) 3)
#define TIM_EGR_CC3G                   ((uint32_t) ((uint32_t)1 << TIM_EGR_CC3G_POS))

#define TIM_EGR_CC2G_POS               ((uint32_t) 2)
#define TIM_EGR_CC2G                   ((uint32_t) ((uint32_t)1 << TIM_EGR_CC2G_POS))

#define TIM_EGR_CC1G_POS               ((uint32_t) 1)
#define TIM_EGR_CC1G                   ((uint32_t) ((uint32_t)1 << TIM_EGR_CC1G_POS))

#define TIM_EGR_UG_POS                 ((uint32_t) 0)
#define TIM_EGR_UG                     ((uint32_t) ((uint32_t)1 << TIM_EGR_UG_POS))


/*************** TIM Capture/Compare1 Mode Register Bitfiled ******************/

#define TIM_CCMR1_OC2CE_POS            ((uint32_t) 15)
#define TIM_CCMR1_OC2CE                ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC2CE_POS))

#define TIM_CCMR1_OC2M_POS             ((uint32_t) 12)
#define TIM_CCMR1_OC2M                 ((uint32_t) ((uint32_t)7 << TIM_CCMR1_OC2M_POS))
#define TIM_CCMR1_OC2M_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC1M_POS))
#define TIM_CCMR1_OC2M_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR1_OC1M_POS))
#define TIM_CCMR1_OC2M_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR1_OC1M_POS))

#define TIM_CCMR1_OC2PE_POS            ((uint32_t) 11)
#define TIM_CCMR1_OC2PE                ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC2PE_POS))

#define TIM_CCMR1_OC2FE_POS            ((uint32_t) 10)
#define TIM_CCMR1_OC2FE                ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC2FE_POS))

#define TIM_CCMR1_OC2S_POS             ((uint32_t) 8)
#define TIM_CCMR1_OC2S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR1_OC2S_POS))

#define TIM_CCMR1_OC1CE_POS            ((uint32_t) 7)
#define TIM_CCMR1_OC1CE                ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC1CE_POS))

#define TIM_CCMR1_OC1M_POS             ((uint32_t) 4)
#define TIM_CCMR1_OC1M                 ((uint32_t) ((uint32_t)7 << TIM_CCMR1_OC1M_POS))
#define TIM_CCMR1_OC1M_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC1M_POS))
#define TIM_CCMR1_OC1M_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR1_OC1M_POS))
#define TIM_CCMR1_OC1M_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR1_OC1M_POS))

#define TIM_CCMR1_OC1PE_POS            ((uint32_t) 3)
#define TIM_CCMR1_OC1PE                ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC1PE_POS))

#define TIM_CCMR1_OC1FE_POS            ((uint32_t) 2)
#define TIM_CCMR1_OC1FE                ((uint32_t) ((uint32_t)1 << TIM_CCMR1_OC1FE_POS))

#define TIM_CCMR1_OC1S_POS             ((uint32_t) 0)
#define TIM_CCMR1_OC1S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR1_OC1S_POS))

/*----------------------------------------------------------------------------*/

#define TIM_CCMR1_IC2F_POS             ((uint32_t) 12)
#define TIM_CCMR1_IC2F                 ((uint32_t) ((uint32_t)0xf << TIM_CCMR1_IC2F_POS))
#define TIM_CCMR1_IC2F_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR1_IC2F_POS))
#define TIM_CCMR1_IC2F_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR1_IC2F_POS))
#define TIM_CCMR1_IC2F_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR1_IC2F_POS))
#define TIM_CCMR1_IC2F_3               ((uint32_t) ((uint32_t)8 << TIM_CCMR1_IC2F_POS))

#define TIM_CCMR1_IC2PSC_POS           ((uint32_t) 10)
#define TIM_CCMR1_IC2PSC               ((uint32_t) ((uint32_t)3 << TIM_CCMR1_IC2PSC_POS))
#define TIM_CCMR1_IC2PSC_0             ((uint32_t) ((uint32_t)1 << TIM_CCMR1_IC2PSC_POS))
#define TIM_CCMR1_IC2PSC_1             ((uint32_t) ((uint32_t)2 << TIM_CCMR1_IC2PSC_POS))

#define TIM_CCMR1_CC2S_POS             ((uint32_t) 8)
#define TIM_CCMR1_CC2S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR1_CC2S_POS))
#define TIM_CCMR1_CC2S_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR1_CC2S_POS))
#define TIM_CCMR1_CC2S_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR1_CC2S_POS))

#define TIM_CCMR1_IC1F_POS             ((uint32_t) 4)
#define TIM_CCMR1_IC1F                 ((uint32_t) ((uint32_t)0xf << TIM_CCMR1_IC1F_POS))
#define TIM_CCMR1_IC1F_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR1_IC1F_POS))
#define TIM_CCMR1_IC1F_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR1_IC1F_POS))
#define TIM_CCMR1_IC1F_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR1_IC1F_POS))
#define TIM_CCMR1_IC1F_3               ((uint32_t) ((uint32_t)8 << TIM_CCMR1_IC1F_POS))

#define TIM_CCMR1_IC1PSC_POS           ((uint32_t) 2)
#define TIM_CCMR1_IC1PSC               ((uint32_t) ((uint32_t)3 << TIM_CCMR1_IC1PSC_POS))
#define TIM_CCMR1_IC1PSC_0             ((uint32_t) ((uint32_t)1 << TIM_CCMR1_IC1PSC_POS))
#define TIM_CCMR1_IC1PSC_1             ((uint32_t) ((uint32_t)2 << TIM_CCMR1_IC1PSC_POS))

#define TIM_CCMR1_CC1S_POS             ((uint32_t) 0)
#define TIM_CCMR1_CC1S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR1_CC1S_POS))
#define TIM_CCMR1_CC1S_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR1_CC1S_POS))
#define TIM_CCMR1_CC1S_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR1_CC1S_POS))


/*************** TIM Capture/Compare2 Mode Register Bitfiled ******************/

#define TIM_CCMR2_OC4CE_POS            ((uint32_t) 15)
#define TIM_CCMR2_OC4CE                ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC4CE_POS))

#define TIM_CCMR2_OC4M_POS             ((uint32_t) 12)
#define TIM_CCMR2_OC4M                 ((uint32_t) ((uint32_t)7 << TIM_CCMR2_OC4M_POS))
#define TIM_CCMR2_OC4M_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC4M_POS))
#define TIM_CCMR2_OC4M_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR2_OC4M_POS))
#define TIM_CCMR2_OC4M_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR2_OC4M_POS))

#define TIM_CCMR2_OC4PE_POS            ((uint32_t) 11)
#define TIM_CCMR2_OC4PE                ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC4PE_POS))

#define TIM_CCMR2_OC4FE_POS            ((uint32_t) 10)
#define TIM_CCMR2_OC4FE                ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC4FE_POS))

#define TIM_CCMR2_OC4S_POS             ((uint32_t) 8)
#define TIM_CCMR2_OC4S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR2_OC4S_POS))

#define TIM_CCMR2_OC3CE_POS            ((uint32_t) 7)
#define TIM_CCMR2_OC3CE                ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC3CE_POS))

#define TIM_CCMR2_OC3M_POS             ((uint32_t) 4)
#define TIM_CCMR2_OC3M                 ((uint32_t) ((uint32_t)7 << TIM_CCMR2_OC3M_POS))
#define TIM_CCMR2_OC3M_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC3M_POS))
#define TIM_CCMR2_OC3M_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR2_OC3M_POS))
#define TIM_CCMR2_OC3M_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR2_OC3M_POS))

#define TIM_CCMR2_OC3PE_POS            ((uint32_t) 3)
#define TIM_CCMR2_OC3PE                ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC3PE_POS))

#define TIM_CCMR2_OC3FE_POS            ((uint32_t) 2)
#define TIM_CCMR2_OC3FE                ((uint32_t) ((uint32_t)1 << TIM_CCMR2_OC3FE_POS))

#define TIM_CCMR2_OC3S_POS             ((uint32_t) 0)
#define TIM_CCMR2_OC3S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR2_OC3S_POS))


/*----------------------------------------------------------------------------*/

#define TIM_CCMR2_IC4F_POS             ((uint32_t) 12)
#define TIM_CCMR2_IC4F                 ((uint32_t) ((uint32_t)0xf << TIM_CCMR2_IC4F_POS))
#define TIM_CCMR2_IC4F_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR2_IC4F_POS))
#define TIM_CCMR2_IC4F_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR2_IC4F_POS))
#define TIM_CCMR2_IC4F_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR2_IC4F_POS))
#define TIM_CCMR2_IC4F_3               ((uint32_t) ((uint32_t)8 << TIM_CCMR2_IC4F_POS))

#define TIM_CCMR2_IC4PSC_POS           ((uint32_t) 10)
#define TIM_CCMR2_IC4PSC               ((uint32_t) ((uint32_t)3 << TIM_CCMR2_IC4PSC_POS))
#define TIM_CCMR2_IC4PSC_0             ((uint32_t) ((uint32_t)1 << TIM_CCMR2_IC4PSC_POS))
#define TIM_CCMR2_IC4PSC_1             ((uint32_t) ((uint32_t)2 << TIM_CCMR2_IC4PSC_POS))

#define TIM_CCMR2_CC4S_POS             ((uint32_t) 8)
#define TIM_CCMR2_CC4S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR2_CC4S_POS))
#define TIM_CCMR2_CC4S_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR2_CC4S_POS))
#define TIM_CCMR2_CC4S_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR2_CC4S_POS))

#define TIM_CCMR2_IC3F_POS             ((uint32_t) 4)
#define TIM_CCMR2_IC3F                 ((uint32_t) ((uint32_t)0xf << TIM_CCMR2_IC3F_POS))
#define TIM_CCMR2_IC3F_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR2_IC3F_POS))
#define TIM_CCMR2_IC3F_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR2_IC3F_POS))
#define TIM_CCMR2_IC3F_2               ((uint32_t) ((uint32_t)4 << TIM_CCMR2_IC3F_POS))
#define TIM_CCMR2_IC3F_3               ((uint32_t) ((uint32_t)8 << TIM_CCMR2_IC3F_POS))

#define TIM_CCMR2_IC3PSC_POS           ((uint32_t) 2)
#define TIM_CCMR2_IC3PSC               ((uint32_t) ((uint32_t)3 << TIM_CCMR2_IC3PSC_POS))
#define TIM_CCMR2_IC3PSC_0             ((uint32_t) ((uint32_t)1 << TIM_CCMR2_IC3PSC_POS))
#define TIM_CCMR2_IC3PSC_1             ((uint32_t) ((uint32_t)2 << TIM_CCMR2_IC3PSC_POS))

#define TIM_CCMR2_CC3S_POS             ((uint32_t) 0)
#define TIM_CCMR2_CC3S                 ((uint32_t) ((uint32_t)3 << TIM_CCMR2_CC3S_POS))
#define TIM_CCMR2_CC3S_0               ((uint32_t) ((uint32_t)1 << TIM_CCMR2_CC3S_POS))
#define TIM_CCMR2_CC3S_1               ((uint32_t) ((uint32_t)2 << TIM_CCMR2_CC3S_POS))


/*************** TIM Capture/Compare Enable Register Bitfiled ******************/

#define TIM_CCER_CC4NP_POS             ((uint32_t) 15)
#define TIM_CCER_CC4NP                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC4NP_POS))

#define TIM_CCER_CC4NE_POS             ((uint32_t) 14)
#define TIM_CCER_CC4NE                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC4NE_POS))

#define TIM_CCER_CC4P_POS              ((uint32_t) 13)
#define TIM_CCER_CC4P                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC4P_POS))

#define TIM_CCER_CC4E_POS              ((uint32_t) 12)
#define TIM_CCER_CC4E                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC4E_POS))

#define TIM_CCER_CC3NP_POS             ((uint32_t) 11)
#define TIM_CCER_CC3NP                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC3NP_POS))

#define TIM_CCER_CC3NE_POS             ((uint32_t) 10)
#define TIM_CCER_CC3NE                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC3NE_POS))

#define TIM_CCER_CC3P_POS              ((uint32_t) 9)
#define TIM_CCER_CC3P                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC3P_POS))

#define TIM_CCER_CC3E_POS              ((uint32_t) 8)
#define TIM_CCER_CC3E                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC3E_POS))

#define TIM_CCER_CC2NP_POS             ((uint32_t) 7)
#define TIM_CCER_CC2NP                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC2NP_POS))

#define TIM_CCER_CC2NE_POS             ((uint32_t) 6)
#define TIM_CCER_CC2NE                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC2NE_POS))

#define TIM_CCER_CC2P_POS              ((uint32_t) 5)
#define TIM_CCER_CC2P                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC2P_POS))

#define TIM_CCER_CC2E_POS              ((uint32_t) 4)
#define TIM_CCER_CC2E                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC2E_POS))

#define TIM_CCER_CC1NP_POS             ((uint32_t) 3)
#define TIM_CCER_CC1NP                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC1NP_POS))

#define TIM_CCER_CC1NE_POS             ((uint32_t) 2)
#define TIM_CCER_CC1NE                 ((uint32_t) ((uint32_t)1 << TIM_CCER_CC1NE_POS))

#define TIM_CCER_CC1P_POS              ((uint32_t) 1)
#define TIM_CCER_CC1P                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC1P_POS))

#define TIM_CCER_CC1E_POS              ((uint32_t) 0)
#define TIM_CCER_CC1E                  ((uint32_t) ((uint32_t)1 << TIM_CCER_CC1E_POS))


/*************** TIM Counter Register Bitfiled ******************/

#define TIM_CNT_CNT_POS                ((uint32_t) 0)
#define TIM_CNT_CNT                    ((uint32_t) ((uint32_t)0xffff << TIM_CNT_CNT_POS))


/*************** TIM Prescaler Register Bitfiled ******************/

#define TIM_PSC_PSC_POS                ((uint32_t) 0)
#define TIM_PSC_PSC                    ((uint32_t) ((uint32_t)0xffff << TIM_PSC_PSC_POS))


/*************** TIM Auto-reload Register Bitfiled ******************/

#define TIM_ARR_ARR_POS                ((uint32_t) 0)
#define TIM_ARR_ARR                    ((uint32_t) ((uint32_t)0xffff << TIM_ARR_ARR_POS))


/*************** TIM Repetition Counter Register Bitfiled ******************/

#define TIM_RCR_REP_POS                ((uint32_t) 0)
#define TIM_RCR_REP                    ((uint32_t) ((uint32_t)0xff << TIM_RCR_REP_POS))


/*************** TIM Capture/Compare1 Register Bitfiled ******************/

#define TIM_CCR1_CCR1_POS              ((uint32_t) 0)
#define TIM_CCR1_CCR1                  ((uint32_t) ((uint32_t)0xffff << TIM_CCR1_CCR1_POS))


/*************** TIM Capture/Compare2 Register Bitfiled ******************/

#define TIM_CCR2_CCR2_POS              ((uint32_t) 0)
#define TIM_CCR2_CCR2                  ((uint32_t) ((uint32_t)0xffff << TIM_CCR2_CCR2_POS))


/*************** TIM Capture/Compare3 Register Bitfiled ******************/

#define TIM_CCR3_CCR3_POS              ((uint32_t) 0)
#define TIM_CCR3_CCR3                  ((uint32_t) ((uint32_t)0xffff << TIM_CCR3_CCR3_POS))


/*************** TIM Capture/Compare4 Register Bitfiled ******************/

#define TIM_CCR4_CCR4_POS              ((uint32_t) 0)
#define TIM_CCR4_CCR4                  ((uint32_t) ((uint32_t)0xffff << TIM_CCR4_CCR4_POS))


/*************** TIM Break and Dead-time Register Bitfiled ******************/

#define TIM_BDTR_MOE_POS               ((uint32_t) 15)
#define TIM_BDTR_MOE                   ((uint32_t) ((uint32_t)1 << TIM_BDTR_MOE_POS))

#define TIM_BDTR_AOE_POS               ((uint32_t) 14)
#define TIM_BDTR_AOE                   ((uint32_t) ((uint32_t)1 << TIM_BDTR_AOE_POS))

#define TIM_BDTR_BKP_POS               ((uint32_t) 13)
#define TIM_BDTR_BKP                   ((uint32_t) ((uint32_t)1 << TIM_BDTR_BKP_POS))

#define TIM_BDTR_BKE_POS               ((uint32_t) 12)
#define TIM_BDTR_BKE                   ((uint32_t) ((uint32_t)1 << TIM_BDTR_BKE_POS))

#define TIM_BDTR_OSSR_POS              ((uint32_t) 11)
#define TIM_BDTR_OSSR                  ((uint32_t) ((uint32_t)1 << TIM_BDTR_OSSR_POS))

#define TIM_BDTR_OSSI_POS              ((uint32_t) 10)
#define TIM_BDTR_OSSI                  ((uint32_t) ((uint32_t)1 << TIM_BDTR_OSSI_POS))

#define TIM_BDTR_LOCK_POS              ((uint32_t) 8)
#define TIM_BDTR_LOCK                  ((uint32_t) ((uint32_t)3 << TIM_BDTR_LOCK_POS))
#define TIM_BDTR_LOCK_0                ((uint32_t) ((uint32_t)1 << TIM_BDTR_LOCK_POS))
#define TIM_BDTR_LOCK_1                ((uint32_t) ((uint32_t)2 << TIM_BDTR_LOCK_POS))


#define TIM_BDTR_DTG_POS               ((uint32_t) 0)
#define TIM_BDTR_DTG                   ((uint32_t) ((uint32_t)0xff << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_0                 ((uint32_t) ((uint32_t)0x01 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_1                 ((uint32_t) ((uint32_t)0x02 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_2                 ((uint32_t) ((uint32_t)0x04 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_3                 ((uint32_t) ((uint32_t)0x08 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_4                 ((uint32_t) ((uint32_t)0x10 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_5                 ((uint32_t) ((uint32_t)0x20 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_6                 ((uint32_t) ((uint32_t)0x40 << TIM_BDTR_DTG_POS))
#define TIM_BDTR_DTG_7                 ((uint32_t) ((uint32_t)0x80 << TIM_BDTR_DTG_POS))


/*************** TIM DMA Control Register Bitfiled ******************/

#define TIM_DCR_DBL_POS                ((uint32_t) 8)
#define TIM_DCR_DBL                    ((uint32_t) ((uint32_t)0x1f << TIM_DCR_DBL_POS))
#define TIM_DCR_DBL_0                  ((uint32_t) ((uint32_t)0x01 << TIM_DCR_DBL_POS))
#define TIM_DCR_DBL_1                  ((uint32_t) ((uint32_t)0x02 << TIM_DCR_DBL_POS))
#define TIM_DCR_DBL_2                  ((uint32_t) ((uint32_t)0x04 << TIM_DCR_DBL_POS))
#define TIM_DCR_DBL_3                  ((uint32_t) ((uint32_t)0x08 << TIM_DCR_DBL_POS))
#define TIM_DCR_DBL_4                  ((uint32_t) ((uint32_t)0x10 << TIM_DCR_DBL_POS))

#define TIM_DCR_DBA_POS                ((uint32_t) 0)
#define TIM_DCR_DBA                    ((uint32_t) ((uint32_t)0x1f << TIM_DCR_DBA_POS))
#define TIM_DCR_DBA_0                  ((uint32_t) ((uint32_t)0x01 << TIM_DCR_DBA_POS))
#define TIM_DCR_DBA_1                  ((uint32_t) ((uint32_t)0x02 << TIM_DCR_DBA_POS))
#define TIM_DCR_DBA_2                  ((uint32_t) ((uint32_t)0x04 << TIM_DCR_DBA_POS))
#define TIM_DCR_DBA_3                  ((uint32_t) ((uint32_t)0x08 << TIM_DCR_DBA_POS))
#define TIM_DCR_DBA_4                  ((uint32_t) ((uint32_t)0x10 << TIM_DCR_DBA_POS))


/*************** TIM DMA Address For Full Transfer Register Bitfiled ******************/

#define TIM_DMAR_DMBA_POS              ((uint32_t) 0)
#define TIM_DMAR_DMBA                  ((uint32_t) ((uint32_t)0xffff << TIM_DMAR_DMBA_POS))


/*************** TIM DMA Transfer Loop1 Register Bitfiled ******************/

#define TIM_DTLR1_TD_DTL_POS           ((uint32_t) 24)
#define TIM_DTLR1_TD_DTL               ((uint32_t) ((uint32_t)0xff << TIM_DTLR1_TD_DTL_POS))

#define TIM_DTLR1_COMD_DTL_POS         ((uint32_t) 16)
#define TIM_DTLR1_COMD_DTL             ((uint32_t) ((uint32_t)0xff << TIM_DTLR1_COMD_DTL_POS))

#define TIM_DTLR1_CC4D_DTL_POS         ((uint32_t) 8)
#define TIM_DTLR1_CC4D_DTL             ((uint32_t) ((uint32_t)0xff << TIM_DTLR1_CC4D_DTL_POS))

#define TIM_DTLR1_CC3D_DTL_POS         ((uint32_t) 0)
#define TIM_DTLR1_CC3D_DTL             ((uint32_t) ((uint32_t)0xff << TIM_DTLR1_CC3D_DTL_POS))


/*************** TIM DMA Transfer Loop2 Register Bitfiled ******************/

#define TIM_DTLR2_CC2D_DTL_POS         ((uint32_t) 16)
#define TIM_DTLR2_CC2D_DTL             ((uint32_t) ((uint32_t)0xff << TIM_DTLR2_CC2D_DTL_POS))

#define TIM_DTLR2_CC1D_DTL_POS         ((uint32_t) 8)
#define TIM_DTLR2_CC1D_DTL             ((uint32_t) ((uint32_t)0xff << TIM_DTLR2_CC1D_DTL_POS))

#define TIM_DTLR2_UD_DTL_POS           ((uint32_t) 0)
#define TIM_DTLR2_UD_DTL               ((uint32_t) ((uint32_t)0xff << TIM_DTLR2_UD_DTL_POS))


/***************** TIM Option Register Bitfiled ***************************/
#define TIM_OP_CH2_PES_EN_POS          ((uint32_t) 4)
#define TIM_OP_CH2_PES_EN              ((uint32_t) ((uint32_t)1 << TIM_OP_CH2_PES_EN_POS))

#define TIM_OP_CH2_PES_EV_POS          ((uint32_t) 0)
#define TIM_OP_CH2_PES_EV              ((uint32_t) ((uint32_t)0xf << TIM_OP_CH2_PES_EV_POS))


/***************** TIM Compare Register Bitfiled **************************/
#define TIM_CCR5_CCR5_POS              ((uint32_t) 0)
#define TIM_CCR5_CCR5                  ((uint32_t) ((uint32_t)0xffff << TIM_CCR5_CCR5_POS))

/***************** TIM Version Register Bitfiled **************************/
#define TIM_VER_VER_POS                ((uint32_t) 0)
#define TIM_VER_VER                    ((uint32_t) ((uint32_t)0xffffffff << TIM_VER_VER_POS))


#define TIM1      ((TIM_Device_T *)TIM1_BASE)
#define TIM2      ((TIM_Device_T *)TIM2_BASE)
#define TIM3      ((TIM_Device_T *)TIM3_BASE)
#define TIM4      ((TIM_Device_T *)TIM4_BASE)
#define TIM5      ((TIM_Device_T *)TIM5_BASE)
#define TIM8      ((TIM_Device_T *)TIM8_BASE)

#if defined(CHIP_GM6721)
#define TIM9      ((TIM_Device_T *)TIM9_BASE)
#define TIM10     ((TIM_Device_T *)TIM10_BASE)
#define TIM11     ((TIM_Device_T *)TIM11_BASE)
#define TIM12     ((TIM_Device_T *)TIM12_BASE)
#define TIM13     ((TIM_Device_T *)TIM13_BASE)
#define TIM14     ((TIM_Device_T *)TIM14_BASE)
#endif

/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
/**
 * @}
 */

#define assert_param(expr) ((void)0U)
#define READ_REG(REG)         ((REG))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define STATIC_ISR_BY_ALIAS(IntNum, FuncName) void INT##IntNum##_Handler(void) __attribute__((alias(#FuncName)))
#define STATIC_ISR_BY_INTNUM(IntNum, FuncName) STATIC_ISR_BY_ALIAS(IntNum, FuncName)
#define STATIC_ISR(IntName, FuncName) STATIC_ISR_BY_INTNUM(IntName##_INTVEC, FuncName)

/************* TIM Instances : at least 1 capture/compare channel *************/
#if defined(CHIP_GM6721)
#define IS_TIM_CC1_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1)  || \
        ((INSTANCE) == TIM2)  || \
        ((INSTANCE) == TIM3)  || \
        ((INSTANCE) == TIM4)  || \
        ((INSTANCE) == TIM5)  || \
        ((INSTANCE) == TIM8)  || \
        ((INSTANCE) == TIM9)  || \
        ((INSTANCE) == TIM10) || \
        ((INSTANCE) == TIM11) || \
        ((INSTANCE) == TIM12) || \
        ((INSTANCE) == TIM13) || \
        ((INSTANCE) == TIM14))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_CC1_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1)  || \
        ((INSTANCE) == TIM2)  || \
        ((INSTANCE) == TIM3)  || \
        ((INSTANCE) == TIM4)  || \
        ((INSTANCE) == TIM5)  || \
        ((INSTANCE) == TIM8))
#endif

/************ TIM Instances : at least 2 capture/compare channels *************/
#if defined(CHIP_GM6721)
#define IS_TIM_CC2_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                       ((INSTANCE) == TIM2) || \
                                       ((INSTANCE) == TIM3) || \
                                       ((INSTANCE) == TIM4) || \
                                       ((INSTANCE) == TIM5) || \
                                       ((INSTANCE) == TIM8) || \
                                       ((INSTANCE) == TIM9) || \
                                       ((INSTANCE) == TIM12))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_CC2_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                       ((INSTANCE) == TIM2) || \
                                       ((INSTANCE) == TIM3) || \
                                       ((INSTANCE) == TIM4) || \
                                       ((INSTANCE) == TIM5) || \
                                       ((INSTANCE) == TIM8))
#endif

/************ TIM Instances : at least 3 capture/compare channels *************/
#define IS_TIM_CC3_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/************ TIM Instances : at least 4 capture/compare channels *************/
#define IS_TIM_CC4_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
                                       ((INSTANCE) == TIM2) || \
                                       ((INSTANCE) == TIM3) || \
                                       ((INSTANCE) == TIM4) || \
                                       ((INSTANCE) == TIM5) || \
                                       ((INSTANCE) == TIM8))

/******************** TIM Instances : Advanced-control timers *****************/
#define IS_TIM_ADVANCED_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM8))

/******************* TIM Instances : Timer input XOR function *****************/
#define IS_TIM_XOR_INSTANCE(INSTANCE)   (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/****** TIM Instances : master mode available (TIMx_CR2.MMS available )********/
#define IS_TIM_MASTER_INSTANCE(INSTANCE) (((INSTANCE) == TIM1)  || \
        ((INSTANCE) == TIM2)  || \
        ((INSTANCE) == TIM3)  || \
        ((INSTANCE) == TIM4)  || \
        ((INSTANCE) == TIM5)  || ((INSTANCE) == TIM8))

/*********** TIM Instances : Slave mode available (TIMx_SMCR available )*******/
#if defined(CHIP_GM6721)
#define IS_TIM_SLAVE_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8) || \
        ((INSTANCE) == TIM9) || \
        ((INSTANCE) == TIM12))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_SLAVE_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))
#endif

/***************** TIM Instances : external trigger input availabe ************/
#define IS_TIM_ETR_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
                                        ((INSTANCE) == TIM2) || \
                                        ((INSTANCE) == TIM3) || \
                                        ((INSTANCE) == TIM4) || \
                                        ((INSTANCE) == TIM5) || \
                                        ((INSTANCE) == TIM8))

/****************** TIM Instances : remapping capability **********************/
#if defined(CHIP_GM6721)
#define IS_TIM_REMAP_INSTANCE(INSTANCE) (((INSTANCE) == TIM2)  || \
        ((INSTANCE) == TIM5)  || \
        ((INSTANCE) == TIM11))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_REMAP_INSTANCE(INSTANCE) (((INSTANCE) == TIM2) || ((INSTANCE) == TIM5))
#endif

/******************* TIM Instances : output(s) available **********************/
#if defined(CHIP_GM6721)
#define IS_TIM_CCX_INSTANCE(INSTANCE, CHANNEL) \
    ((((INSTANCE) == TIM1) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM2) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM3) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM4) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM5) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM8) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM9) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2)))           \
     ||                                         \
     (((INSTANCE) == TIM10) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1)))           \
     ||                                         \
     (((INSTANCE) == TIM11) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1)))           \
     ||                                         \
     (((INSTANCE) == TIM12) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2)))           \
     ||                                         \
     (((INSTANCE) == TIM13) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1)))           \
     ||                                         \
     (((INSTANCE) == TIM14) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1))))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_CCX_INSTANCE(INSTANCE, CHANNEL) \
    ((((INSTANCE) == TIM1) &&                  \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM2) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM3) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM4) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM5) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4)))           \
     ||                                         \
     (((INSTANCE) == TIM8) &&                   \
      (((CHANNEL) == TIM_CHANNEL_1) ||          \
       ((CHANNEL) == TIM_CHANNEL_2) ||          \
       ((CHANNEL) == TIM_CHANNEL_3) ||          \
       ((CHANNEL) == TIM_CHANNEL_4))))

#endif


/************ TIM Instances : complementary output(s) available ***************/
#define IS_TIM_CCXN_INSTANCE(INSTANCE, CHANNEL)    \
    (((INSTANCE) == TIM1) && (((CHANNEL) == TIM_CHANNEL_1) || ((CHANNEL) == TIM_CHANNEL_2) || ((CHANNEL) == TIM_CHANNEL_3) || ((CHANNEL) == TIM_CHANNEL_4)))

/****************** TIM Instances : supporting counting mode selection ********/
#define IS_TIM_COUNTER_MODE_SELECT_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/****************** TIM Instances : supporting clock division *****************/
#if defined(CHIP_GM6721)
#define IS_TIM_CLOCK_DIVISION_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8) || \
        ((INSTANCE) == TIM9) || \
        ((INSTANCE) == TIM10)|| \
        ((INSTANCE) == TIM11)|| \
        ((INSTANCE) == TIM12)|| \
        ((INSTANCE) == TIM13)|| \
        ((INSTANCE) == TIM14))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_CLOCK_DIVISION_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))
#endif
/****************** TIM Instances : supporting commutation event generation ***/
#define IS_TIM_COMMUTATION_EVENT_INSTANCE(INSTANCE) (((INSTANCE) == TIM1)|| \
        ((INSTANCE) == TIM8))


/****************** TIM Instances : supporting OCxREF clear *******************/
#define IS_TIM_OCXREF_CLEAR_INSTANCE(INSTANCE)        (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/****** TIM Instances : supporting external clock mode 1 for ETRF input *******/
#if defined(CHIP_GM6721)
#define IS_TIM_CLOCKSOURCE_ETRMODE1_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8) || \
        ((INSTANCE) == TIM9) || \
        ((INSTANCE) == TIM12))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_CLOCKSOURCE_ETRMODE1_INSTANCE(INSTANCE) (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))
#endif

/****** TIM Instances : supporting external clock mode 2 for ETRF input *******/
#define IS_TIM_CLOCKSOURCE_ETRMODE2_INSTANCE(INSTANCE)        (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/****************** TIM Instances : supporting repetition counter *************/
#define IS_TIM_REPETITION_COUNTER_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM8))

/****************** TIM Instances : supporting encoder interface **************/
#if defined(CHIP_GM6721)
#define IS_TIM_ENCODER_INTERFACE_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8) || \
        ((INSTANCE) == TIM9) || \
        ((INSTANCE) == TIM12))
#endif

#if defined(CHIP_F6721B)
#define IS_TIM_ENCODER_INTERFACE_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))
#endif

/****************** TIM Instances : supporting Hall sensor interface **********/
#define IS_TIM_HALL_SENSOR_INTERFACE_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/****************** TIM Instances : supporting the break function *************/
#define IS_TIM_BREAK_INSTANCE(INSTANCE)  (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM8))

/****************** TIM Instances : supporting the break function *************/
#define IS_TIM_DMA_INSTANCE(INSTANCE)    (((INSTANCE) == TIM1) || \
        ((INSTANCE) == TIM2) || \
        ((INSTANCE) == TIM3) || \
        ((INSTANCE) == TIM4) || \
        ((INSTANCE) == TIM5) || \
        ((INSTANCE) == TIM8))

/** @defgroup TIM_IS_TIM_Definitions TIM Private macros to check input parameters
  * @{
  */
#define IS_TIM_COUNTER_MODE(MODE) (((MODE) == TIM_COUNTERMODE_UP)              || \
                                   ((MODE) == TIM_COUNTERMODE_DOWN)            || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED1)  || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED2)  || \
                                   ((MODE) == TIM_COUNTERMODE_CENTERALIGNED3))

#define IS_TIM_CLOCKDIVISION_DIV(DIV) (((DIV) == TIM_CLOCKDIVISION_DIV1) || \
                                       ((DIV) == TIM_CLOCKDIVISION_DIV2) || \
                                       ((DIV) == TIM_CLOCKDIVISION_DIV4))

#define IS_TIM_PERIOD(__PERIOD__)            ((__PERIOD__) <= 0xFFFFU)

#define IS_TIM_PRESCALER(__PRESCALER__)      ((__PRESCALER__) <= 0xFFFFU)


#define IS_TIM_PWM_MODE(MODE) (((MODE) == TIM_OCMODE_PWM1) || \
                               ((MODE) == TIM_OCMODE_PWM2))

#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMODE_TIMING)       || \
                              ((MODE) == TIM_OCMODE_ACTIVE)           || \
                              ((MODE) == TIM_OCMODE_INACTIVE)         || \
                              ((MODE) == TIM_OCMODE_TOGGLE)           || \
                              ((MODE) == TIM_OCMODE_FORCED_ACTIVE)    || \
                              ((MODE) == TIM_OCMODE_FORCED_INACTIVE))

#define IS_TIM_FAST_STATE(STATE) (((STATE) == TIM_OCFAST_DISABLE) || \
                                  ((STATE) == TIM_OCFAST_ENABLE))

#define IS_TIM_OC_POLARITY(POLARITY) (((POLARITY) == TIM_OCPOLARITY_HIGH) || \
                                      ((POLARITY) == TIM_OCPOLARITY_LOW))

#define IS_TIM_OCN_POLARITY(POLARITY) (((POLARITY) == TIM_OCNPOLARITY_HIGH) || \
                                       ((POLARITY) == TIM_OCNPOLARITY_LOW))

#define IS_TIM_OCIDLE_STATE(STATE) (((STATE) == TIM_OCIDLESTATE_SET) || \
                                    ((STATE) == TIM_OCIDLESTATE_RESET))

#define IS_TIM_OCNIDLE_STATE(STATE) (((STATE) == TIM_OCNIDLESTATE_SET) || \
                                     ((STATE) == TIM_OCNIDLESTATE_RESET))

#define IS_TIM_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                  ((CHANNEL) == TIM_CHANNEL_2) || \
                                  ((CHANNEL) == TIM_CHANNEL_3) || \
                                  ((CHANNEL) == TIM_CHANNEL_4) || \
                                  ((CHANNEL) == TIM_CHANNEL_ALL))

#define IS_TIM_OPM_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
                                      ((CHANNEL) == TIM_CHANNEL_2))

#define IS_TIM_COMPLEMENTARY_CHANNELS(CHANNEL) (((CHANNEL) == TIM_CHANNEL_1) || \
        ((CHANNEL) == TIM_CHANNEL_2) || \
        ((CHANNEL) == TIM_CHANNEL_3))

#define IS_TIM_IC_POLARITY(POLARITY) (((POLARITY) == TIM_ICPOLARITY_RISING)   || \
                                      ((POLARITY) == TIM_ICPOLARITY_FALLING)  || \
                                      ((POLARITY) == TIM_ICPOLARITY_BOTHEDGE))

#define IS_TIM_IC_SELECTION(SELECTION) (((SELECTION) == TIM_ICSELECTION_DIRECTTI) || \
                                        ((SELECTION) == TIM_ICSELECTION_INDIRECTTI) || \
                                        ((SELECTION) == TIM_ICSELECTION_TRC))

#define IS_TIM_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ICPSC_DIV1) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV2) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV4) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV8))

#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0xF)

#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMODE_SINGLE) || \
                               ((MODE) == TIM_OPMODE_REPETITIVE))

#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & 0xFFFF80FFU) == 0x00000000U) && ((SOURCE) != 0x00000000U))

#define IS_TIM_ENCODER_MODE(MODE) (((MODE) == TIM_ENCODERMODE_TI1) || \
                                   ((MODE) == TIM_ENCODERMODE_TI2) || \
                                   ((MODE) == TIM_ENCODERMODE_TI12))

#define IS_TIM_EVENT_SOURCE(SOURCE) ((((SOURCE) & 0xFFFFFF00U) == 0x00000000U) && ((SOURCE) != 0x00000000U))

#define IS_TIM_CLOCKSOURCE(CLOCK) (((CLOCK) == TIM_CLOCKSOURCE_INTERNAL) || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ETRMODE2) || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR0)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR1)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR2)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ITR3)     || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_TI1ED)    || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_TI1)      || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_TI2)      || \
                                   ((CLOCK) == TIM_CLOCKSOURCE_ETRMODE1))

#define IS_TIM_CLOCKPOLARITY(POLARITY) (((POLARITY) == TIM_CLOCKPOLARITY_INVERTED)    || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_NONINVERTED) || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_RISING)      || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_FALLING)     || \
                                        ((POLARITY) == TIM_CLOCKPOLARITY_BOTHEDGE))

#define IS_TIM_CLOCKPRESCALER(PRESCALER) (((PRESCALER) == TIM_CLOCKPRESCALER_DIV1) || \
        ((PRESCALER) == TIM_CLOCKPRESCALER_DIV2) || \
        ((PRESCALER) == TIM_CLOCKPRESCALER_DIV4) || \
        ((PRESCALER) == TIM_CLOCKPRESCALER_DIV8))

#define IS_TIM_CLOCKFILTER(ICFILTER)      ((ICFILTER) <= 0x0FU)

#define IS_TIM_CLEARINPUT_SOURCE(SOURCE)  (((SOURCE) == TIM_CLEARINPUTSOURCE_NONE) || \
        ((SOURCE) == TIM_CLEARINPUTSOURCE_ETR))

#define IS_TIM_CLEARINPUT_POLARITY(POLARITY)   (((POLARITY) == TIM_CLEARINPUTPOLARITY_INVERTED) || \
        ((POLARITY) == TIM_CLEARINPUTPOLARITY_NONINVERTED))

#define IS_TIM_CLEARINPUT_PRESCALER(PRESCALER)   (((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV1) || \
        ((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV2) || \
        ((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV4) || \
        ((PRESCALER) == TIM_CLEARINPUTPRESCALER_DIV8))

#define IS_TIM_CLEARINPUT_FILTER(ICFILTER) ((ICFILTER) <= 0x0FU)

#define IS_TIM_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM_LOCKLEVEL_OFF) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_1) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_2) || \
                                  ((LEVEL) == TIM_LOCKLEVEL_3))

#define IS_TIM_DEADTIME(DEADTIME)      ((DEADTIME) <= 0xFF)

#define IS_TIM_BREAK_STATE(STATE) (((STATE) == TIM_BREAK_ENABLE) || \
                                   ((STATE) == TIM_BREAK_DISABLE))

#define IS_TIM_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM_BREAKPOLARITY_LOW) || \
        ((POLARITY) == TIM_BREAKPOLARITY_HIGH))

#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM_AUTOMATICOUTPUT_ENABLE) || \
        ((STATE) == TIM_AUTOMATICOUTPUT_DISABLE))

#define IS_TIM_TRGO_SOURCE(SOURCE) (((SOURCE) == TIM_TRGO_RESET) || \
                                    ((SOURCE) == TIM_TRGO_ENABLE) || \
                                    ((SOURCE) == TIM_TRGO_UPDATE) || \
                                    ((SOURCE) == TIM_TRGO_OC1) || \
                                    ((SOURCE) == TIM_TRGO_OC1REF) || \
                                    ((SOURCE) == TIM_TRGO_OC2REF) || \
                                    ((SOURCE) == TIM_TRGO_OC3REF) || \
                                    ((SOURCE) == TIM_TRGO_OC4REF))

#define IS_TIM_SLAVE_MODE(MODE) (((MODE) == TIM_SLAVEMODE_DISABLE) || \
                                 ((MODE) == TIM_SLAVEMODE_GATED) || \
                                 ((MODE) == TIM_SLAVEMODE_RESET) || \
                                 ((MODE) == TIM_SLAVEMODE_TRIGGER) || \
                                 ((MODE) == TIM_SLAVEMODE_EXTERNAL1))

#define IS_TIM_MSM_STATE(STATE) (((STATE) == TIM_MASTERSLAVEMODE_ENABLE) || \
                                 ((STATE) == TIM_MASTERSLAVEMODE_DISABLE))

#define IS_TIM_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
        ((SELECTION) == TIM_TS_ITR1) || \
        ((SELECTION) == TIM_TS_ITR2) || \
        ((SELECTION) == TIM_TS_ITR3) || \
        ((SELECTION) == TIM_TS_TI1F_ED) || \
        ((SELECTION) == TIM_TS_TI1FP1) || \
        ((SELECTION) == TIM_TS_TI2FP2) || \
        ((SELECTION) == TIM_TS_ETRF))

#define IS_TIM_INTERNAL_TRIGGEREVENT_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
        ((SELECTION) == TIM_TS_ITR1) || \
        ((SELECTION) == TIM_TS_ITR2) || \
        ((SELECTION) == TIM_TS_ITR3) || \
        ((SELECTION) == TIM_TS_NONE))

#define IS_TIM_TRIGGERPOLARITY(POLARITY)     (((POLARITY) == TIM_TRIGGERPOLARITY_INVERTED   ) || \
        ((POLARITY) == TIM_TRIGGERPOLARITY_NONINVERTED) || \
        ((POLARITY) == TIM_TRIGGERPOLARITY_RISING     ) || \
        ((POLARITY) == TIM_TRIGGERPOLARITY_FALLING    ) || \
        ((POLARITY) == TIM_TRIGGERPOLARITY_BOTHEDGE   ))

#define IS_TIM_TRIGGERPRESCALER(PRESCALER)  (((PRESCALER) == TIM_TRIGGERPRESCALER_DIV1) || \
        ((PRESCALER) == TIM_TRIGGERPRESCALER_DIV2) || \
        ((PRESCALER) == TIM_TRIGGERPRESCALER_DIV4) || \
        ((PRESCALER) == TIM_TRIGGERPRESCALER_DIV8))

#define IS_TIM_TRIGGERFILTER(ICFILTER)     ((ICFILTER) <= 0x0FU)

#define IS_TIM_TI1SELECTION(TI1SELECTION)   (((TI1SELECTION) == TIM_TI1SELECTION_CH1) || \
        ((TI1SELECTION) == TIM_TI1SELECTION_XORCOMBINATION))

#define IS_TIM_DMA_BASE(BASE) (((BASE) == TIM_DMABASE_CR1) || \
                               ((BASE) == TIM_DMABASE_CR2) || \
                               ((BASE) == TIM_DMABASE_SMCR) || \
                               ((BASE) == TIM_DMABASE_DIER) || \
                               ((BASE) == TIM_DMABASE_SR) || \
                               ((BASE) == TIM_DMABASE_EGR) || \
                               ((BASE) == TIM_DMABASE_CCMR1) || \
                               ((BASE) == TIM_DMABASE_CCMR2) || \
                               ((BASE) == TIM_DMABASE_CCER) || \
                               ((BASE) == TIM_DMABASE_CNT) || \
                               ((BASE) == TIM_DMABASE_PSC) || \
                               ((BASE) == TIM_DMABASE_ARR) || \
                               ((BASE) == TIM_DMABASE_RCR) || \
                               ((BASE) == TIM_DMABASE_CCR1) || \
                               ((BASE) == TIM_DMABASE_CCR2) || \
                               ((BASE) == TIM_DMABASE_CCR3) || \
                               ((BASE) == TIM_DMABASE_CCR4) || \
                               ((BASE) == TIM_DMABASE_BDTR))


#define IS_TIM_DMA_LENGTH(LENGTH) (((LENGTH) == TIM_DMABURSTLENGTH_1TRANSFER) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_2TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_3TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_4TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_5TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_6TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_7TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_8TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_9TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_10TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_11TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_12TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_13TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_14TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_15TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_16TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_17TRANSFERS) || \
                                   ((LENGTH) == TIM_DMABURSTLENGTH_18TRANSFERS))

/*************** TIM Control1 Register ******************/

/**
 * @brief Enable/Disable counter
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CNT_ENABLE(TIMx)         IO_BIT_SET(TIMx->CR1, TIM_CR1_CEN)
#define LL_TIM_CNT_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->CR1, TIM_CR1_CEN)
#define LL_TIM_CNT_GET(TIMx)            IO_BIT_GET(TIMx->CR1, TIM_CR1_CEN)


/**
 * @brief Enable/Disable one pulse mode.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_OPM_ENABLE(TIMx)         IO_BIT_SET(TIMx->CR1, TIM_CR1_OPM)
#define LL_TIM_OPM_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->CR1, TIM_CR1_OPM)
#define LL_TIM_OPM_GET(TIMx)            IO_BIT_GET(TIMx->CR1, TIM_CR1_OPM)

/**
 * @brief Enable/Disable auto-reload preload mode
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_ARPE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CR1, TIM_CR1_ARPE)
#define LL_TIM_ARPE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CR1, TIM_CR1_ARPE)



/*************** TIM Control2 Register ******************/

/**
 * @brief Capture/Compare preload control enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CCPC_ENABLE(TIMx)        IO_BIT_SET(TIMx->CR2, TIM_CR2_CCPC)
#define LL_TIM_CCPC_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CR2, TIM_CR2_CCPC)

/**
 * @brief Capture/Compare control update selection enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CCUS_ENABLE(TIMx)        IO_BIT_SET(TIMx->CR2, TIM_CR2_CCUS)
#define LL_TIM_CCUS_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CR2, TIM_CR2_CCUS)

/**
 * @brief Enable Capture/Compare DMA Selection.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CCDS_ENABLE(TIMx)        IO_BIT_SET(TIMx->CR2, TIM_CR2_CCDS)
#define LL_TIM_CCDS_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CR2, TIM_CR2_CCDS)

/**
 * @brief Enable/Disable the Hall sensor interface (XOR function of the three inputs).
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_TI1S_ENABLE(TIMx)        IO_BIT_SET(TIMx->CR2, TIM_CR2_TI1S)
#define LL_TIM_TI1S_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CR2, TIM_CR2_TI1S)



/*************** TIM Slave Mode Control Register ******************/

/**
 * @brief Reset slave mode selection bit.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_SMS_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->SMCR, TIM_SMCR_SMS)

/**
 * @brief External clock mode 2 enable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_ECE_ENABLE(TIMx)         IO_BIT_SET(TIMx->SMCR, TIM_SMCR_ECE)


/*************** TIM DMA/Interrupt Enable Register ******************/

/**
 * @brief Update interrupt enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_UIE_GET(TIMx)            IO_BIT_GET(TIMx->DIER, TIM_DIER_UIE)
#define LL_TIM_UIE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_UIE)
#define LL_TIM_UIE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_UIE)

/**
 * @brief Capture/Compare 1/2/3/4 interrupt enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CC1IE_GET(TIMx)           IO_BIT_GET(TIMx->DIER, TIM_DIER_CC1IE)
#define LL_TIM_CC2IE_GET(TIMx)           IO_BIT_GET(TIMx->DIER, TIM_DIER_CC2IE)
#define LL_TIM_CC3IE_GET(TIMx)           IO_BIT_GET(TIMx->DIER, TIM_DIER_CC3IE)
#define LL_TIM_CC4IE_GET(TIMx)           IO_BIT_GET(TIMx->DIER, TIM_DIER_CC4IE)

#define LL_TIM_CC1IE_ENABLE(TIMx)        IO_BIT_SET(TIMx->DIER, TIM_DIER_CC1IE)
#define LL_TIM_CC2IE_ENABLE(TIMx)        IO_BIT_SET(TIMx->DIER, TIM_DIER_CC2IE)
#define LL_TIM_CC3IE_ENABLE(TIMx)        IO_BIT_SET(TIMx->DIER, TIM_DIER_CC3IE)
#define LL_TIM_CC4IE_ENABLE(TIMx)        IO_BIT_SET(TIMx->DIER, TIM_DIER_CC4IE)

#define LL_TIM_CC1IE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC1IE)
#define LL_TIM_CC2IE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC2IE)
#define LL_TIM_CC3IE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC3IE)
#define LL_TIM_CC4IE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC4IE)

/**
 * @brief Commutation interrupt enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_COMIE_GET(TIMx)           IO_BIT_GET(TIMx->DIER, TIM_DIER_COMIE)
#define LL_TIM_COMIE_ENABLE(TIMx)        IO_BIT_SET(TIMx->DIER, TIM_DIER_COMIE)
#define LL_TIM_COMIE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_COMIE)

/**
 * @brief Trigger interrupt enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_TIE_GET(TIMx)             IO_BIT_GET(TIMx->DIER, TIM_DIER_TIE)
#define LL_TIM_TIE_ENABLE(TIMx)          IO_BIT_SET(TIMx->DIER, TIM_DIER_TIE)
#define LL_TIM_TIE_DISABLE(TIMx)         IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_TIE)

/**
 * @brief Break interrupt enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_BIE_GET(TIMx)             IO_BIT_GET(TIMx->DIER, TIM_DIER_BIE)
#define LL_TIM_BIE_ENABLE(TIMx)          IO_BIT_SET(TIMx->DIER, TIM_DIER_BIE)
#define LL_TIM_BIE_DISABLE(TIMx)         IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_BIE)

/**
 * @brief Update DMA request enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_UDE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_UDE)
#define LL_TIM_UDE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_UDE)

/**
 * @brief Capture/Compare 1/2/3/4 DMA request enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */

#define LL_TIM_CC1DE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_CC1DE)
#define LL_TIM_CC2DE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_CC2DE)
#define LL_TIM_CC3DE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_CC3DE)
#define LL_TIM_CC4DE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_CC4DE)

#define LL_TIM_CC1DE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC1DE)
#define LL_TIM_CC2DE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC2DE)
#define LL_TIM_CC3DE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC3DE)
#define LL_TIM_CC4DE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_CC4DE)

/**
 * @brief Commutation DMA request enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_COMDE_ENABLE(TIMx)         IO_BIT_SET(TIMx->DIER, TIM_DIER_COMDE)
#define LL_TIM_COMDE_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_COMDE)

/**
 * @brief Trigger DMA request enable/disable.
 *
 * @param htim  pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 * @param instance  register base address
 *
 * @retval void
 */
#define LL_TIM_TDE_ENABLE(TIMx)           IO_BIT_SET(TIMx->DIER, TIM_DIER_TDE)
#define LL_TIM_TDE_DISABLE(TIMx)          IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_TDE)

/**
 * @brief Break DMA request enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_BDE_ENABLE(TIMx)           IO_BIT_SET(TIMx->DIER, TIM_DIER_BDE)
#define LL_TIM_BDE_DISABLE(TIMx)          IO_BIT_CLEAR(TIMx->DIER, TIM_DIER_BDE)



/*************** TIM Status Register ******************/

/**
 * @brief Update interrupt flag.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_UIF_GET(TIMx)              IO_BIT_GET(TIMx->SR, TIM_SR_UIF)

/**
 * @brief Clear update generation interrupt flag
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_ClearUgFlag(TIMx)          IO_BIT_SET(TIMx->SR, TIM_SR_UIF)

/**
 * @brief Capture/Compare 1/2/3/4 interrupt flag.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CC1IF_GET(TIMx)            IO_BIT_GET(TIMx->SR, TIM_SR_CC1IF)
#define LL_TIM_CC2IF_GET(TIMx)            IO_BIT_GET(TIMx->SR, TIM_SR_CC2IF)
#define LL_TIM_CC3IF_GET(TIMx)            IO_BIT_GET(TIMx->SR, TIM_SR_CC3IF)
#define LL_TIM_CC4IF_GET(TIMx)            IO_BIT_GET(TIMx->SR, TIM_SR_CC4IF)

/**
 * @brief Commutation interrupt flag.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_COMIF_GET(TIMx)            IO_BIT_GET(TIMx->SR, TIM_SR_COMIF)

/**
 * @brief Clear commutation generation interrupt flag
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_ClearComFlag(TIMx)         IO_BIT_SET(TIMx->SR, TIM_SR_COMIF)

/**
 * @brief Trigger interrupt flag.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_TIF_GET(TIMx)              IO_BIT_GET(TIMx->SR, TIM_SR_TIF)

/**
 * @brief Clear trigger generation interrupt flag
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_ClearTriggerFlag(TIMx)     IO_BIT_SET(TIMx->SR, TIM_SR_TIF)

/**
 * @brief Break interrupt flag.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_BIF_GET(TIMx)              IO_BIT_GET(TIMx->SR, TIM_SR_BIF)

/**
 * @brief Clear break generation interrupt flag
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_ClearBreakFlag(TIMx)       IO_BIT_SET(TIMx->SR, TIM_SR_BIF)

/*************** TIM Event Generation Register ******************/

/**
 * @brief Enable update generation
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_UG_ENABLE(TIMx)            IO_BIT_SET(TIMx->EGR, TIM_EGR_UG)

/**
 * @brief Enable commutation generation
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_COMG_ENABLE(TIMx)          IO_BIT_SET(TIMx->EGR, TIM_EGR_COMG)

/**
 * @brief Enable trigger generation
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_TG_ENABLE(TIMx)            IO_BIT_SET(TIMx->EGR, TIM_EGR_TG)


/*************** TIM Capture/Compare mode Register1/2 ******************/

/**
 * @brief Capture/Compare 1/2/3/4 selection.
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CC1S_GET(TIMx)            IO_BIT_GET(TIMx->CCMR1, TIM_CCMR1_CC1S)
#define LL_TIM_CC2S_GET(TIMx)            IO_BIT_GET(TIMx->CCMR1, TIM_CCMR1_CC2S)
#define LL_TIM_CC3S_GET(TIMx)            IO_BIT_GET(TIMx->CCMR2, TIM_CCMR2_CC3S)
#define LL_TIM_CC4S_GET(TIMx)            IO_BIT_GET(TIMx->CCMR2, TIM_CCMR2_CC4S)

/**
 * @brief Output compare 1/2/3/4 fast disable.
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_OC1FE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR1, TIM_CCMR1_OC1FE)
#define LL_TIM_OC2FE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR1, TIM_CCMR1_OC2FE)
#define LL_TIM_OC3FE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR2, TIM_CCMR2_OC3FE)
#define LL_TIM_OC4FE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR2, TIM_CCMR2_OC4FE)

/**
 * @brief Output compare 1/2/3/4 preload enable.
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_OC1PE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR1, TIM_CCMR1_OC1PE)
#define LL_TIM_OC2PE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR1, TIM_CCMR1_OC2PE)
#define LL_TIM_OC3PE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR2, TIM_CCMR2_OC3PE)
#define LL_TIM_OC4PE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR2, TIM_CCMR2_OC4PE)

/**
 * @brief Output compare 1/2/3/4 clear enable/disable.
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_OC1CE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR1, TIM_CCMR1_OC1CE)
#define LL_TIM_OC2CE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR1, TIM_CCMR1_OC2CE)
#define LL_TIM_OC3CE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR2, TIM_CCMR2_OC3CE)
#define LL_TIM_OC4CE_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCMR2, TIM_CCMR2_OC4CE)

#define LL_TIM_OC1CE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR1, TIM_CCMR1_OC1CE)
#define LL_TIM_OC2CE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR1, TIM_CCMR1_OC2CE)
#define LL_TIM_OC3CE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR2, TIM_CCMR2_OC3CE)
#define LL_TIM_OC4CE_DISABLE(TIMx)       IO_BIT_CLEAR(TIMx->CCMR2, TIM_CCMR2_OC4CE)

/**
 * @brief Reset Input Capture 1/2/3/4 Prescaler .
 *
 * @param TIMx pointer to a TIM_Handle_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_IC1PSC_RESET(TIMx)        IO_BIT_CLEAR(TIMx->CCMR1, TIM_CCMR1_IC1PSC)
#define LL_TIM_IC2PSC_RESET(TIMx)        IO_BIT_CLEAR(TIMx->CCMR1, TIM_CCMR1_IC2PSC)
#define LL_TIM_IC3PSC_RESET(TIMx)        IO_BIT_CLEAR(TIMx->CCMR2, TIM_CCMR2_IC3PSC)
#define LL_TIM_IC4PSC_RESET(TIMx)        IO_BIT_CLEAR(TIMx->CCMR2, TIM_CCMR2_IC4PSC)


/*************** TIM Capture/Compare Enable Register ******************/

/**
 * @brief Capture/Compare 1/2/3/4 output disable.
 *
 * @param TIMx  Pointer to a TIM_Device_T structure that contains the configuration information for TIM module registers .
 *
 * @retval void
 */
#define LL_TIM_CC1E_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->CCER, TIM_CCER_CC1E)
#define LL_TIM_CC2E_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->CCER, TIM_CCER_CC2E)
#define LL_TIM_CC3E_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->CCER, TIM_CCER_CC3E)
#define LL_TIM_CC4E_DISABLE(TIMx)        IO_BIT_CLEAR(TIMx->CCER, TIM_CCER_CC4E)

#define LL_TIM_CC1E_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCER, TIM_CCER_CC1E)
#define LL_TIM_CC2E_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCER, TIM_CCER_CC2E)
#define LL_TIM_CC3E_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCER, TIM_CCER_CC3E)
#define LL_TIM_CC4E_ENABLE(TIMx)        IO_BIT_SET(TIMx->CCER, TIM_CCER_CC4E)


/**
 * @brief Capture/Compare x/xN output mask(x = 1, 2, ,3 ,4).
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_CCXE_GET(TIMx)            IO_BIT_GET(TIMx->CCER, TIM_CCER_CCxE_MASK)
#define LL_TIM_CCXNE_GET(TIMx)           IO_BIT_GET(TIMx->CCER, TIM_CCER_CCxNE_MASK)


/*************** TIM Break and Dead-time Register ******************/

/**
 * @brief Main output enable/disable.
 *
 * @param TIMx pointer to a TIM_Device_T structure that contains the configuration information for TIM module.
 *
 * @retval void
 */
#define LL_TIM_MOE_ENABLE(TIMx)          IO_BIT_SET(TIMx->BDTR, TIM_BDTR_MOE)
#define LL_TIM_MOE_DISABLE(TIMx)         IO_BIT_CLEAR(TIMx->BDTR, TIM_BDTR_MOE)


/**
 * @brief operate 8/16/32-bit register address
 *
 * @param val  the value of 8-bit register.
 * @param address  register base address
 *
 * @retval void
 */
#define LL_TIM_Write_8BitsAddr(val, addr)           IO_WRITEB(val,addr)
#define LL_TIM_Write_16BitsAddr(val, addr)          IO_WRITEW(val,addr)
#define LL_TIM_Write_32BitsAddr(val, addr)          IO_WRITEL(val,addr)

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
        printf("assert_param error\n");
    }
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _TIM_DEV_H_ */


