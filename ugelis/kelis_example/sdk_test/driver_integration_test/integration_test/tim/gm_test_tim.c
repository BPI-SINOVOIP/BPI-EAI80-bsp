/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Edward.Zhu
 *
 * @version     v1.0.0
 *
 * @date        2019/12/29
 *
 * @brief       tim demo source file
 *
 * @note
 *              2019/12/29, Edward.Zhu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_common.h>
#include <board.h>
#include <ztest.h>
#include <gm_hal_tim.h>
#include "gm_test_tim.h"
#include <tim.h>

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

#define  PERIOD_VALUE       0xFFFF        /* Period Value  */
#define  PULSE1_VALUE       0x7FFF        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       0x3FFF        /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       0xFFF         /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       0x7FF         /* Capture Compare 4 Value  */
/*******************************************************************************
* Constants and macros
*******************************************************************************/

static int gm_test_tim_pwmoutput(void)
{
    struct device *tim_dev;
    tim_timebaseinit init;
    tim_outputcompareinit oc_config;
    tim_bdtconfig bdt_config;

    tim_dev = device_get_binding(CONFIG_TIM1_NAME);

    printf("================Enter func[%s] Test=================\n", __FUNCTION__);

    if (tim_dev == NULL)
    {
        printf("Failed to get TIM device.");

        return -EINVAL;
    }

    init.prescaler = 3;
    init.period = PERIOD_VALUE;
    init.clockDivision = 0;
    init.counterMode = TIMx_COUNTERMODE_UP;

    tim_pwm_init(tim_dev);

    oc_config.ocMode = TIMx_OCMODE_PWM1;
    oc_config.ocPolarity = TIMx_OCPOLARITY_HIGH;
    oc_config.ocFastMode = TIMx_OCFAST_DISABLE;

    oc_config.pulse = PULSE1_VALUE;
    tim_pwmchannel_cfg(tim_dev, &oc_config, TIMx_CHANNEL_1);

    oc_config.pulse = PULSE2_VALUE;
    tim_pwmchannel_cfg(tim_dev, &oc_config, TIMx_CHANNEL_2);

    oc_config.pulse = PULSE3_VALUE;
    tim_pwmchannel_cfg(tim_dev, &oc_config, TIMx_CHANNEL_3);

    oc_config.pulse = PULSE4_VALUE;
    tim_pwmchannel_cfg(tim_dev, &oc_config, TIMx_CHANNEL_4);

    /* Set the deadtime value */
    bdt_config.deadTime = DTx_DELAY_4;
    tim_bdt_config(tim_dev, &bdt_config);

    tim_pwm_start(tim_dev, TIMx_CHANNEL_1);
    tim_pwm_start(tim_dev, TIMx_CHANNEL_2);
    tim_pwm_start(tim_dev, TIMx_CHANNEL_3);
    tim_pwm_start(tim_dev, TIMx_CHANNEL_4);

    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return TC_PASS;
}

static int gm_test_tim_inputcapture(void)
{
    struct device *tim_dev;
    tim_timebaseinit init;
    tim_inputcaptureinit ic_config;

    tim_dev = device_get_binding(CONFIG_TIM3_NAME);

    printf("================Enter func[%s] Test=================\n", __FUNCTION__);

    if (tim_dev == NULL)
    {
        printf("Failed to get TIM device.");

        return -EINVAL;
    }

    init.prescaler = 0;
    init.period = PERIOD_VALUE;
    init.counterMode = TIMx_COUNTERMODE_UP;

    tim_inputcapture_init(tim_dev);

    ic_config.icPolarity = TIMx_ICPOLARITY_RISING;
    ic_config.icSelection = TIMx_ICSELECTION_DIRECTTI;
    ic_config.icPrescaler = TIMx_ICPSC_DIV1;
    ic_config.icFilter = 0;

    tim_icchannel_cfg(tim_dev, &ic_config, TIMx_CHANNEL_2);

    tim_inputcapture_start(tim_dev, TIMx_CHANNEL_2);
}

void gm_test_tim(void)
{
    printf("================GM_TIM Test Start=================\n\n\n");

    zassert_true(gm_test_tim_pwmoutput() == TC_PASS, NULL);
    zassert_true(gm_test_tim_inputcapture() == TC_PASS, NULL);

    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    printf("================GM_TIM Test  End==================\n\n\n");
}

