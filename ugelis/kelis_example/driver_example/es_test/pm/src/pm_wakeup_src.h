/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _WAKUP_SRC_H_
#define _WAKUP_SRC_H_



#ifdef __cplusplus
extern "C" {
#endif

#include "pm_test_config.h"


/*clk config*/
typedef enum
{
    FCLK_SOURCE_HIRC          = 0,
    FCLK_SOURCE_KDPPLL           = 1,
} FCLK_SourceType_T;


typedef enum
{
    WKPIN_XPA0           = 0,
    WKPIN_XPA7           = 1,
    WKPIN_XPA8           = 2,
} WAKE_PIN_TYPE_T;


RET_CODE  pm_sleepmode_dma_wakeup(FCLK_SourceType_T type);
RET_CODE  pm_sleepmode_rtc_wakeup(FCLK_SourceType_T type);
RET_CODE  pm_stop1_rtc_wakeup(FCLK_SourceType_T type);
RET_CODE  pm_stop2_wkpin_wakeup(FCLK_SourceType_T type);
RET_CODE  pm_stop2_wkpin_uninterrupt_wakeup(FCLK_SourceType_T type);
RET_CODE  pm_stop2_rtc_wakeup(FCLK_SourceType_T type);
RET_CODE  pm_stop2_pes_wakeup(FCLK_SourceType_T type);
RET_CODE pm_standby_wkpin_wakeup();
RET_CODE pm_standby_wkpin_untinterrupt_wakeup();
RET_CODE pm_standby_rtc_wakeup();
RET_CODE pm_standby_iwdg_wakeup();
RET_CODE pm_standby_xrstj_wakeup();
RET_CODE pm_shutdown_wkpinA7_wakeup();
RET_CODE pm_shutdown_wkpinA7_uninterrupt_wakeup();
RET_CODE pm_shutdown_wkpinA8_wakeup();
RET_CODE pm_shutdown_rtc_wakeup();
RET_CODE pm_shutdown_iwdg_wakeup();
RET_CODE pm_shutdown_xrstj_wakeup();
void pm_switch_test(void);




#ifdef __cplusplus
}
#endif

#endif


















