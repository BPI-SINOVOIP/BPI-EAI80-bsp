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
 * @brief       adc source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_hal_gpio.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <pinmux.h>
#include <adc.h>

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

#define PORT    "GPIOA"
#define OUTPUT0_GPIO_PIN   0
#define OUTPUT1_GPIO_PIN   1
#define OUTPUT2_GPIO_PIN   2

#ifndef CONFIG_SDRAM_BOOT
    #define USER_SEQUECE_LEN  16
    #define USER_SINGLE_CHANNEL_LEN   1
    #define USER_INJECT_SEQUECE_LEN  2
#else
    #define USER_SEQUECE_LEN  2
    #define USER_SINGLE_CHANNEL_LEN   1
    #define USER_INJECT_SEQUECE_LEN  2
#endif

#define CNT   1

#define TC_PASS   0

int gpio_output_High(void)
{
    struct device *dev;

    dev = device_get_binding(PORT);

    gpio_pin_configure(dev, OUTPUT0_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(dev, OUTPUT1_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(dev, OUTPUT2_GPIO_PIN, GPIO_MODE_OUTPUT);

    gpio_pin_write(dev, OUTPUT0_GPIO_PIN, GPIO_PIN_SET);
    gpio_pin_write(dev, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);
    gpio_pin_write(dev, OUTPUT2_GPIO_PIN, GPIO_PIN_SET);

    return TC_PASS;
}

int adc_channel_sync_demo(int demo_sel)
{
    unsigned int convert_data = 0;
    struct device *adc_dev;
    static uint32_t adc_buffer[USER_SINGLE_CHANNEL_LEN];
    static struct adc_seq_table g_adc_table_handler;
    static struct adc_seq_entry sample;
    int i = 0;
    int j = 0;
    int cnt = 0;
    float vo;

    gpio_output_High();

    adc_dev = device_get_binding(CONFIG_ADC_0_NAME);

    PR_DEBUG("================Enter func[%s] Test=================\n", __FUNCTION__);

    if (adc_dev == NULL)
    {
        printk("Failed to get ADC device.");
        return -EINVAL;
    }
    sample.buffer = &adc_buffer[0];
    sample.buffer_length = USER_SINGLE_CHANNEL_LEN;
    sample.init.Resolution = ADC_RESOLUTION_12BIT;
    sample.init.NbrOfConversion = sample.buffer_length;
    sample.init.workMode = ADC_SINGLE_MODE;
    sample.init.dma_en = DISABLE;
    sample.init.trigermode = ADC_SW_TRIGER; /*ADC_SW_TRIGER, ADC_HW_TRIGER*/
    sample.init.timeOut = TIMEOUT_WAIT_FOREVER;
    sample.init.lowpowerEn = DISABLE;

    //sample.channel_cfg[0].rchn_sel = ADC_CHANNEL_15;
    sample.channel_cfg[0].rchn_sel = ADC_CHANNEL_0; //XPC4 VBAT-DET
    //sample.channel_cfg[0].rchn_sel = ADC_CHANNEL_1;

    if (sample.init.trigermode == ADC_HW_TRIGER)
    {
        sample.init.trigersel = ADC_HW_TRIGER_PESCH0;
    }
    for (i = 0; i < sample.init.NbrOfConversion; i++)
    {
        sample.channel_cfg[i].SamplingTime = ADC_SAMPLETIME_3CYCLES;
    }

    g_adc_table_handler.entries = &sample;

    while (1) //(cnt++ < CNT)
    {
        if ((sample.init.workMode == ADC_REPEAT_MODE) || (sample.init.trigermode == ADC_HW_TRIGER))
        {
            if (j == 0)
            {
                gm_adc_write_cfg(adc_dev, &g_adc_table_handler);
                gm_adc_enable(adc_dev);
                j++;
            }
        }
        else
        {
            gm_adc_write_cfg(adc_dev, &g_adc_table_handler);
            gm_adc_enable(adc_dev);
        }

        gm_adc_read(adc_dev, &g_adc_table_handler);
        convert_data = g_adc_table_handler.entries->buffer[0];
	//vo = 0.001573034 * convert_data;
	//vo = 1.573034 * convert_data;
	vo = 1.611328125 * convert_data;
        //PR_DEBUG("Channel:%d Convert Success Value:%d VBAT-DET=%f\n", sample.channel_cfg[0].rchn_sel, convert_data, vo);
        printf("Channel:%d Convert Success Value:%d VBAT-DET=%f\n", sample.channel_cfg[0].rchn_sel, convert_data, vo);

        k_sleep(1000);
    }
    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return TC_PASS;
}

int adc_sequence_async_demo(int demo_sel)
{
    unsigned int convert_data = 0;
    struct device *adc_dev;
    static uint32_t adc_buffer[USER_SEQUECE_LEN];
    static struct adc_seq_table g_adc_table_handler;
    static struct adc_seq_entry sample;
    int i = 0;
    int j = 0;
    int k = 0;
    int cnt = 0;

    gpio_output_High();

    adc_dev = device_get_binding(CONFIG_ADC_0_NAME);

    if (adc_dev == NULL)
    {
        printk("Failed to get ADC device.");
        return -EINVAL;
    }

    PR_DEBUG("================Enter func[%s] Test=================\n", __FUNCTION__);

    sample.buffer = &adc_buffer[0];
    sample.buffer_length = USER_SEQUECE_LEN;
    sample.init.Resolution = ADC_RESOLUTION_12BIT;
    sample.init.NbrOfConversion = sample.buffer_length;
    sample.init.workMode = ADC_SINGLE_MODE;
    sample.init.dma_en = DISABLE;
    sample.init.trigermode = ADC_SW_TRIGER; /*ADC_SW_TRIGER, ADC_HW_TRIGER*/
    sample.init.timeOut = TIMEOUT_WAIT_FOREVER;
    sample.init.lowpowerEn = DISABLE;

    if (sample.init.trigermode == ADC_HW_TRIGER)
    {
        sample.init.trigersel = ADC_HW_TRIGER_PESCH0;
    }
    for (i = 0; i < sample.init.NbrOfConversion; i++)
    {
        sample.channel_cfg[i].SamplingTime = ADC_SAMPLETIME_3CYCLES;
    }

    g_adc_table_handler.entries = &sample;

    while (cnt++ < CNT)
    {
        if ((sample.init.workMode == ADC_REPEAT_MODE) || (sample.init.trigermode == ADC_HW_TRIGER))
        {
            if (j == 0)
            {
                gm_adc_write_cfg(adc_dev, &g_adc_table_handler);
                gm_adc_enable(adc_dev);
                j++;
            }
        }
        else
        {
            gm_adc_write_cfg(adc_dev, &g_adc_table_handler);
            gm_adc_enable(adc_dev);
        }

        gm_adc_read(adc_dev, &g_adc_table_handler);

        for (k = 0; k < USER_SEQUECE_LEN; k++)
        {
            convert_data = g_adc_table_handler.entries->buffer[k];
            PR_DEBUG("Channel:%d Convert Success Value:%d\n", k, convert_data);
        }

        k_sleep(1000);
    }
    PR_DEBUG("================Exit func[%s] Test=================\n\n\n", __FUNCTION__);

    return TC_PASS;
}

void main(void)
{
    printk("SRAM BringUp:Hello World! (20200730 V1.1)%s\n", CONFIG_ARCH);
    printk("enter func[%s]... \n", __FUNCTION__);

    adc_channel_sync_demo(0);

    while (1)
    {
        printk("SRAM BringUp:Hello World! %s\n", CONFIG_ARCH);
        k_sleep(1000);
    }
}
