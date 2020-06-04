/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_sqi.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>
#include <gm_hal_gpio.h>
#include <gpio.h>

#ifdef DEBUG_PRINTF
    #undef ERR_PRINTF
    #undef DEBUG_PRINTF
#endif

#define  ERR_PRINTF
#define  DEBUG_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printf
#endif


#define OUTPUT0_GPIO_PIN   0
#define OUTPUT1_GPIO_PIN   1
#define OUTPUT2_GPIO_PIN   2

#define SW0_INPUT_GPIO_PIN   0
#define SW1_INPUT_GPIO_PIN   1

#define PORT    "GPIOA"
#define PORT_C    "GPIOC"

#define CNT     2

int gpio_output_demo(void)
{
    struct device *dev;
    int i = 0;
    int err_flag = TC_PASS;
    int val0 = 0;

    PR_DEBUG("================Enter func[%s] Test=================\n", __FUNCTION__);
    dev = device_get_binding(PORT);

    gpio_pin_configure(dev, OUTPUT0_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(dev, OUTPUT1_GPIO_PIN, GPIO_MODE_OUTPUT);

    while (i++ < CNT)
    {

        gpio_pin_write(dev, OUTPUT0_GPIO_PIN, GPIO_PIN_RESET);

        gpio_pin_read(dev, OUTPUT0_GPIO_PIN, &val0);
        if (val0 != GPIO_PIN_RESET)
        {
            PR_ERR("Func:%s Failed, line=%d, cnt=%d\n", __FUNCTION__, __LINE__, i);
            err_flag = 1;
        }

        gpio_pin_write(dev, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);

        k_sleep(1000);

        gpio_pin_read(dev, OUTPUT0_GPIO_PIN, &val0);
        if (val0 != GPIO_PIN_RESET)
        {
            PR_ERR("Func:%s Failed, line=%d, cnt=%d\n", __FUNCTION__, __LINE__, i);
            err_flag = 1;
        }

        k_sleep(1000);
    }

    if (err_flag == TC_PASS)
    {
        PR_DEBUG("Func:%s PASS\n", __FUNCTION__);
    }
    else
    {
        PR_ERR("Func:%s Failed, err_flag=%d\n", __FUNCTION__, err_flag);
    }

    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return err_flag;
}


/*Using wire connect input pin and output pin*/
int gpio_input_output_demo(void)
{
    int val0 = 0;
    int val1 = 0;
    struct device *dev;
    struct device *b_dev;
    int cnt  = 0;
    int err_flag = TC_PASS;

    PR_DEBUG("================Enter func[%s] Test=================\n", __FUNCTION__);

    dev = device_get_binding(PORT);
    b_dev = device_get_binding(PORT_C);

    gpio_pin_configure(dev, SW0_INPUT_GPIO_PIN, GPIO_MODE_INPUT);
    gpio_pin_configure(dev, SW1_INPUT_GPIO_PIN, GPIO_MODE_INPUT);

    gpio_pin_configure(b_dev, OUTPUT0_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(b_dev, OUTPUT1_GPIO_PIN, GPIO_MODE_OUTPUT);

    gpio_pin_write(b_dev, OUTPUT0_GPIO_PIN, GPIO_PIN_SET);
    gpio_pin_write(b_dev, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);

    while (cnt++ < CNT)
    {
        gpio_pin_read(dev, SW0_INPUT_GPIO_PIN, &val0);
        if (val0 != GPIO_PIN_SET)
        {
            err_flag = 1;
            PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        }
        gpio_pin_read(dev, SW1_INPUT_GPIO_PIN, &val1);
        if (val1 != GPIO_PIN_SET)
        {
            err_flag = 1;
            PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        }
        k_sleep(1000);
    }


    if (err_flag == TC_PASS)
    {
        PR_DEBUG("Func:%s PASS\n", __FUNCTION__);
    }
    else
    {
        PR_ERR("Func:%s Failed, err_flag=%d\n", __FUNCTION__, err_flag);
    }

    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return TC_PASS;
}

void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    PR_DEBUG("Button pressed\n");
}

int gpio_interrupt_demo(void)
{
    int val ;
    struct device *dev;
    struct device *b_dev;
    static struct gpio_callback gpio_cb;
    int i = 0;

    PR_DEBUG("================Enter func[%s] Test=================\n", __FUNCTION__);

    dev = device_get_binding(PORT);
    b_dev = device_get_binding(PORT_C);

    gpio_pin_configure(b_dev, OUTPUT0_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_write(b_dev, OUTPUT0_GPIO_PIN, GPIO_PIN_SET);

    gpio_pin_configure(b_dev, OUTPUT1_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_write(b_dev, OUTPUT1_GPIO_PIN, GPIO_PIN_RESET);

    /* Set key pin as interrupt active flalling mode */
    gpio_pin_configure(dev, SW0_INPUT_GPIO_PIN, GPIO_INT | GPIO_PUD_PULL_DOWN | GPIO_MODE_IT_RISING);
    gpio_init_callback(&gpio_cb, button_pressed, BIT(SW0_INPUT_GPIO_PIN));

    gpio_add_callback(dev, &gpio_cb);
    gpio_pin_enable_callback(dev, SW0_INPUT_GPIO_PIN);

    while (i++ < CNT)
    {
        gpio_pin_read(dev, SW0_INPUT_GPIO_PIN, &val);
        PR_DEBUG("key value = [%d]\n", val);
        k_sleep(1000);
    }

    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return TC_PASS;
}


void gpio_test(void)
{
    zassert_true(gpio_output_demo() == TC_PASS, NULL);
    zassert_true(gpio_input_output_demo() == TC_PASS, NULL);
    zassert_true(gpio_interrupt_demo() == TC_PASS, NULL);
}