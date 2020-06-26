/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          main.c
 *
 * @author        Edward.Zhu
 *
 * @version       1.0.0
 *
 * @date          2019/05/16
 *
 * @brief         Test Code for TIM module driver layer.
 *
 * @note
 *    2019/04/26, Edward.Zhu, V1.0.0
 *        Initial version.
 */
#include <ugelis.h>
#include <misc/printk.h>
#include <gm_hal_gpio.h>
#include <board.h>
#include <device.h>
#include <gm_soc.h>
#include <misc/util.h>
#include <pinmux.h>
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

#define TC_PASS   0

#define  PERIOD_CAPTRUE_VALUE       0xFFFF        /* Period Value  */
#define  PERIOD_VALUE       0xFFF        /* Period Value  */
#define  PULSE1_VALUE       0x7FF        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       0x3FF        /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       0xFF         /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       0x7F         /* Capture Compare 4 Value  */
/*******************************************************************************
* Constants and macros
*******************************************************************************/

/* Captured Values */
uint32_t uwIC2Value1 = 0;       /* TIM_InputCapture */
uint32_t uwIC2Value2 = 0;       /* TIM_InputCapture */
uint32_t uwDiffCapture = 0;     /* TIM_InputCapture */
/* Capture index */
uint16_t uhCaptureIndex = 0;    /* TIM_InputCapture */

static struct device *tim2_dev;

/* TIM Requires a callback, there is no interrupt enable / disable. */
void tim_example_cb(void *TIMx, uint32_t event, uint32_t param)
{
    if (uhCaptureIndex == 0)
    {
        /* Get the 1st Input Capture value */
        uwIC2Value1 = tim_readcapturevalue_start(tim2_dev, TIMx_CHANNEL_2);
        uhCaptureIndex = 1;
    }
    else if (uhCaptureIndex == 1)
    {
        /* Get the 2nd Input Capture value */
        uwIC2Value2 = tim_readcapturevalue_start(tim2_dev, TIMx_CHANNEL_2);

        /* Capture computation */
        if (uwIC2Value2 > uwIC2Value1)
        {
            uwDiffCapture = (uwIC2Value2 - uwIC2Value1);
        }
        else if (uwIC2Value2 < uwIC2Value1)
        {
            uwDiffCapture = ((0xffff - uwIC2Value1) + uwIC2Value2) + 1;
        }
        else
        {
            uwDiffCapture = 0;
        }

        uhCaptureIndex = 0;
        uwIC2Value1 = 0;

    }

    uwIC2Value2 = uwIC2Value1;
}


int tim_pwmoutput_demo(void)
{
    struct device *tim_dev;
    tim_timebaseinit init;
    tim_outputcompareinit ocConfig;
    tim_bdtconfig bdtConfig;

    tim_dev = device_get_binding("TIM_1");

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

    tim_pwm_init(tim_dev, &init);

    ocConfig.ocMode = TIMx_OCMODE_PWM1;
    ocConfig.ocPolarity = TIMx_OCPOLARITY_HIGH;
    ocConfig.ocFastMode = TIMx_OCFAST_DISABLE;

    ocConfig.ocNPolarity = 0;
    ocConfig.ocIdleState = 0;
    ocConfig.ocNIdleState = 0;
    ocConfig.pulse = PULSE1_VALUE;
    tim_pwmchannel_cfg(tim_dev, &ocConfig, TIMx_CHANNEL_1);

    ocConfig.pulse = PULSE2_VALUE;
    tim_pwmchannel_cfg(tim_dev, &ocConfig, TIMx_CHANNEL_2);

    ocConfig.pulse = PULSE3_VALUE;
    tim_pwmchannel_cfg(tim_dev, &ocConfig, TIMx_CHANNEL_3);

    ocConfig.pulse = PULSE4_VALUE;
    tim_pwmchannel_cfg(tim_dev, &ocConfig, TIMx_CHANNEL_4);

    /* Set the deadtime value */
    //    bdtConfig.deadTime = DTx_DELAY_4;
    //    tim_bdt_config(tim_dev, &bdtConfig);

    tim_pwm_start(tim_dev, TIMx_CHANNEL_1);
    tim_pwm_start(tim_dev, TIMx_CHANNEL_2);
    tim_pwm_start(tim_dev, TIMx_CHANNEL_3);
    tim_pwm_start(tim_dev, TIMx_CHANNEL_4);

    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return TC_PASS;
}


int tim_inputcapture_demo(void)
{

    tim_timebaseinit init;
    tim_inputcaptureinit icConfig;

    tim2_dev = device_get_binding(CONFIG_TIM2_NAME);

    printf("================Enter func[%s] Test=================\n", __FUNCTION__);

    if (tim2_dev == NULL)
    {
        printf("Failed to get TIM device.");

        return -EINVAL;
    }

    init.prescaler = 0;
    init.period = PERIOD_CAPTRUE_VALUE;
    init.counterMode = TIMx_COUNTERMODE_UP;
    init.clockDivision = 0;
    init.callback = tim_example_cb;

    tim_inputcapture_init(tim2_dev, &init);

    icConfig.icPolarity = TIMx_ICPOLARITY_RISING;
    icConfig.icSelection = TIMx_ICSELECTION_DIRECTTI;
    icConfig.icPrescaler = TIMx_ICPSC_DIV1;
    icConfig.icFilter = 0;

    tim_icchannel_cfg(tim2_dev, &icConfig, TIMx_CHANNEL_2);

    tim_inputcapture_start(tim2_dev, TIMx_CHANNEL_2);
}

int main(void)
{
    printk("enter func[%s]... \n", __FUNCTION__);

    tim_pwmoutput_demo();
    tim_inputcapture_demo();

    while (1)
    {
        printk("SRAM BringUp:Hello World! %s\n", CONFIG_ARCH);
        k_sleep(1000);
    }
}

