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

#define  ERR_PRINTF
#define  WARNING_PRINTF
#define  DEBUG_PRINTF

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

#define GPIO_LEVEL_TEST GPIO_DRIVING_LEVEL0
#define CNT     1

/*#define GPIO_CASE1 */

int gpio_output_demo(void)
{
    int i = 0;
    GPIO_PinConfig_T GPIO_InitStruct;
    int err_flag = TC_PASS;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_2 | GPIO_PIN_3; /*GPIOA0,GPIOA1*/
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

#ifdef GPIO_CASE1
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1; /*GPIOB0,GPIOB1*/
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

    for (i = 2; i < 4; i++)
    {
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_SET);
        if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)) != GPIO_PIN_SET)
        {
            PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
            err_flag = 1;
        }
        k_sleep(100);
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_RESET);
        if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)) != GPIO_PIN_RESET)
        {
            PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
            err_flag = 1;
        }
        k_sleep(100);
    }

#ifdef  GPIO_CASE1

    for (i = 0; i < 2; i++)
    {
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR((i + 16)), GPIO_PIN_SET);
        k_sleep(100);
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR((i + 16)), GPIO_PIN_RESET);
        k_sleep(100);
    }
#endif

    if (err_flag == TC_PASS)
    {
        PR_DEBUG("Func:%s PASS\n", __FUNCTION__);
    }
    else
    {
        PR_ERR("Func:%s Failed, err_flag=%d\n", __FUNCTION__, err_flag);
    }

    return err_flag;
}

/*
Using wire  connect GPIOA and GPIOC pin
*/
int gpio_input_output_demo(void)
{
    int i = 0;
    GPIO_PinConfig_T GPIO_InitStruct;
    int err_flag = TC_PASS;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin =  GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    for (i = 0; i < 1; i++)
    {
        HAL_GPIO_PinWrite(GPIOD5, GPIO_PIN_SET); /*GPIOD1*/

        if (HAL_GPIO_PinRead(GPIOD4) != GPIO_PIN_SET)/*GPIOD0*/
        {
            err_flag = 1;
            PR_WARNINR("read high level\n");
        }
        k_sleep(100);

        HAL_GPIO_PinWrite(GPIOD5, GPIO_PIN_RESET); /*GPIOD1*/

        if (HAL_GPIO_PinRead(GPIOD4) != GPIO_PIN_RESET)/*GPIOD0*/
        {
            err_flag = 1;
        }
        k_sleep(100);
    }

    if (err_flag == TC_PASS)
    {
        PR_DEBUG("Func:%s PASS\n", __FUNCTION__);
    }
    else
    {
        PR_DEBUG("Func:%s Failed\n", __FUNCTION__);
    }

    return err_flag;
}

/*Using wire connect  GPIOC10 and GPIOA0 for Test*/
int gpio_filter_demo(void)
{
    int i = 0;
    GPIO_InputGlitchFilter_T GPIO_InputGlitchFilter;
    GPIO_PinConfig_T GPIO_InitStruct;

    printk("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    memset(&GPIO_InputGlitchFilter, 0, sizeof(GPIO_InputGlitchFilter_T));
    GPIO_InputGlitchFilter.parent_clk_src_sel = DB_PARENT_SRC_CLK_SEL_FCLK;
    GPIO_InputGlitchFilter.child_clk_sel = PAD_FLT_CLK3;
    GPIO_InputGlitchFilter.prescaler =  FLT_CLK3_PSC_DIV_64K;
    GPIO_InputGlitchFilter.db_bypass_en = ENABLE;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
        GPIO_InitStruct.pin =  GPIO_PIN_0 ;
        GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
        GPIO_InitStruct.alternate = 0;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);*/

    HAL_GPIO_SetPinFilter(GPIOD4, &GPIO_InputGlitchFilter); /*GPIOC*/
    HAL_GPIO_PinWrite(GPIOD5, GPIO_PIN_SET); /*GPIOA*/

    while (i++ < 2)
    {
        if (HAL_GPIO_PinRead(GPIOD4)) /*GPIOC*/
        {
            //HAL_GPIO_PinWrite(GPIOD0, GPIO_PIN_SET);    /*GPIOD*/
            printf("HAL_GPIO_PinWrite(GPIOD4, GPIO_PIN_SET);\n");
        }
        else
        {
            //HAL_GPIO_PinWrite(GPIOD0, GPIO_PIN_RESET);
            printf("HAL_GPIO_PinWrite(GPIOD4, GPIO_PIN_RESET);\n");
        }
        k_sleep(100);
    }

    PR_DEBUG("Func:%s PASS\n", __FUNCTION__);

    return TC_PASS;
}


int gpio_interrupt_demo(void)
{
    int cnt = 0;
    GPIO_PinConfig_T GPIO_InitStruct;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    GPIO_InitStruct.pin = GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_IT_FALLING;//GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.pull = GPIO_PULLDOWN;
    GPIO_InitStruct.alternate = 0;
    // HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


    HAL_GPIO_RequestIrq(3);

    //HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);
    //HAL_GPIO_PinWrite(GPIOA1, GPIO_PIN_RESET);
    HAL_GPIO_PinWrite(GPIOD5, GPIO_PIN_RESET);
    while (cnt++ < CNT)
    {
        HAL_GPIO_PinWrite(GPIOD5, GPIO_PIN_SET);
        //HAL_GPIO_PinWrite(GPIOA1, GPIO_PIN_SET);

        k_sleep(1000);
        HAL_GPIO_PinWrite(GPIOD5, GPIO_PIN_RESET);
        //HAL_GPIO_PinWrite(GPIOA1, GPIO_PIN_RESET);
        k_sleep(1000);
    }


    PR_DEBUG("Func:%s PASS\n", __FUNCTION__);
    return TC_PASS;

}

void gpio_test(void)
{
    zassert_true(gpio_output_demo() == TC_PASS, NULL);
    zassert_true(gpio_input_output_demo() == TC_PASS, NULL);
    zassert_true(gpio_filter_demo() == TC_PASS, NULL);
    zassert_true(gpio_interrupt_demo() == TC_PASS, NULL);
}