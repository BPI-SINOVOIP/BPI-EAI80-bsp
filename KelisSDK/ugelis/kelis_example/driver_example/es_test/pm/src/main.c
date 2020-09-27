/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include "board.h"
#include <misc/printk.h>
#include "pm_system.h"
#include <gm_soc.h>
#include <gm_hal_rtc.h>
#include <gm_hal_dma.h>
#include <gm_hal_clock.h>

#include "pm_test_config.h"
#include "pm_wakeup_src.h"

static RET_CODE power_mode_test(void)
{
    RET_CODE ret = RET_OK;


#ifdef PM_NORMAL_TEST

    /*normal test*/

#ifdef CONFIG_TEST_HIRC_SLEEP_MODE_DAM_WAKEUP
    char *str = "CONFIG_TEST_HIRC_SLEEP_MODE_DAM_WAKEUP";
    ret = pm_sleepmode_dma_wakeup(FCLK_SOURCE_HIRC);
#endif

#ifdef CONFIG_TEST_KDPPLL_SLEEP_MODE_DAM_WAKEUP
    char *str = "CONFIG_TEST_KDPPLL_SLEEP_MODE_DAM_WAKEUP";
    ret = pm_sleepmode_dma_wakeup(FCLK_SOURCE_KDPPLL);
#endif


#ifdef CONFIG_TEST_HIRC_SLEEP_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_HIRC_SLEEP_MODE_RTC_WAKEUP";
    ret = pm_sleepmode_rtc_wakeup(FCLK_SOURCE_KDPPLL);
#endif
#ifdef CONFIG_TEST_KDPPLL_SLEEP_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_KDPPLL_SLEEP_MODE_RTC_WAKEUP";
    ret = pm_sleepmode_rtc_wakeup(FCLK_SOURCE_KDPPLL);
#endif

#ifdef CONFIG_TEST_HIRC_STOP1_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_HIRC_STOP1_MODE_RTC_WAKEUP";
    ret = pm_stop1_rtc_wakeup(FCLK_SOURCE_HIRC);
#endif
#ifdef CONFIG_TEST_KDPPLL_STOP1_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_KDPPLL_STOP1_MODE_RTC_WAKEUP";
    ret = pm_stop1_rtc_wakeup(FCLK_SOURCE_KDPPLL);
#endif

#ifdef CONFIG_TEST_HIRC_STOP2_MODE_WAKPIN_WAKEUP
    char *str = "CONFIG_TEST_HIRC_STOP2_MODE_WAKPIN_WAKEUP";
    ret = pm_stop2_wkpin_wakeup(FCLK_SOURCE_HIRC);
#endif
#ifdef CONFIG_TEST_KDPPLL_STOP2_MODE_WAKPIN_WAKEUP
    char *str = "CONFIG_TEST_KDPPLL_STOP2_MODE_WAKPIN_WAKEUP";
    ret = pm_stop2_wkpin_wakeup(FCLK_SOURCE_KDPPLL);
#endif

#ifdef CONFIG_TEST_HIRC_STOP2_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_HIRC_STOP2_MODE_RTC_WAKEUP";
    ret = pm_stop2_rtc_wakeup(FCLK_SOURCE_HIRC);
#endif

#ifdef CONFIG_TEST_KDPPLL_STOP2_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_KDPPLL_STOP2_MODE_RTC_WAKEUP";
    ret = pm_stop2_rtc_wakeup(FCLK_SOURCE_KDPPLL);
#endif


#ifdef CONFIG_TEST_HIRC_STOP2_MODE_PES_WAKEUP
    char *str = "CONFIG_TEST_HIRC_STOP2_MODE_PES_WAKEUP";
    ret = pm_stop2_pes_wakeup(FCLK_SOURCE_HIRC);
#endif
#ifdef CONFIG_TEST_KDPPLL_STOP2_MODE_PES_WAKEUP
    char *str = "CONFIG_TEST_KDPPLL_STOP2_MODE_PES_WAKEUP";
    ret = pm_stop2_pes_wakeup(FCLK_SOURCE_KDPPLL);
#endif

#ifdef CONFIG_TEST_STANDBY_MODE_WAKPIN_WAKEUP
    char *str = "CONFIG_TEST_STANDBY_MODE_WAKPIN_WAKEUP";
    ret = pm_standby_wkpin_wakeup();
#endif



#ifdef CONFIG_TEST_STANDBY_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_STANDBY_MODE_RTC_WAKEUP";
    ret = pm_standby_rtc_wakeup();
#endif


#ifdef CONFIG_TEST_STANDBY_MODE_IWDG_WAKEUP
    char *str = "CONFIG_TEST_STANDBY_MODE_IWDG_WAKEUP";
    ret = pm_standby_iwdg_wakeup();
#endif


#ifdef CONFIG_TEST_STANDBY_MODE_XRSTJ_WAKEUP
    char *str = "CONFIG_TEST_STANDBY_MODE_XRSTJ_WAKEUP";
    ret = pm_standby_xrstj_wakeup();
#endif

#ifdef CONFIG_TEST_SHUTDOWN_MODE_A7_WAKEUP
    char *str = "CONFIG_TEST_SHUTDOWN_MODE_A7_WAKEUP";
    ret = pm_shutdown_wkpinA7_wakeup();
#endif



#ifdef CONFIG_TEST_SHUTDOWN_MODE_A8_WAKEUP
    char *str = "CONFIG_TEST_SHUTDOWN_MODE_A8_WAKEUP";
    ret = pm_shutdown_wkpinA8_wakeup();
#endif

#ifdef CONFIG_TEST_SHUTDOWN_MODE_RTC_WAKEUP
    char *str = "CONFIG_TEST_SHUTDOWN_MODE_RTC_WAKEUP";
    ret = pm_shutdown_rtc_wakeup();
#endif

#ifdef CONFIG_TEST_SHUTDOWN_MODE_IWDG_WAKEUP
    char *str = "CONFIG_TEST_SHUTDOWN_MODE_IWDG_WAKEUP";
    ret = pm_shutdown_iwdg_wakeup();
#endif

#ifdef CONFIG_TEST_SHUTDOWN_MODE_XRSTJ_WAKEUP
    char *str = "CONFIG_TEST_SHUTDOWN_MODE_XRSTJ_WAKEUP";
    ret = pm_shutdown_xrstj_wakeup();
#endif


    if (ret == RET_OK)
    {
        PR_DEBUG("PASS <-##-> %s \n", str);
    }
    else
    {
        PR_DEBUG("Fail <-##-> %s \n", str);
    }

#endif

#ifdef PM_SWITECH_TEST
    pm_switch_test();
#endif



}


void main(void)
{
    PR_DEBUG("###Booup reset status=0x%x####\n", readl(RST_STATUS_REG));

    /*must add; if not, tick will  wake up system and not enter wfi mode */
#ifdef PM_NORMAL_TEST
    SysTick_Disable();
#endif
    power_mode_test();
}












