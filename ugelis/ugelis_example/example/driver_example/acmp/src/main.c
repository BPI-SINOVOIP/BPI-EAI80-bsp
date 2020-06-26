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
 * @brief       gpio demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include "board.h"
#include <gm_hal_gpio.h>
#include <gm_hal_acmp.h>
#include <gm_hal_dac.h>
#include <acmp.h>
#include <gpio.h>
#include <dac.h>

#define PORT    "GPIOK"
#define OUTPUT2_GPIO_PIN   2

ACMP_Handle_T acmp_hanle;
struct acmp_config_data acmp_cfg;
struct dac_config_data daccfg;

static void ACMP_Callback(ACMP_Handle_T *pHandle, ACMP_Event_T event, unsigned long Param)
{
    static int cnt = 0;

    printk("negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", cnt++);
    if (event == ACMP_MATCH_EVENT)
    {
        printk("ACMP_MATCH_EVENT cnt=[%d]\n", cnt++);
    }
}

RET_CODE acmp_demo(struct acmp_config_data *cfg)
{
    int i = 0;
    struct device *gpio_dev;
    struct device *acmp_dev;
    int j = 0;

    acmp_dev = device_get_binding(CONFIG_ACMP_0_NAME);
    cfg->callback = ACMP_Callback;
    if (acmp_dev == NULL)
    {
        printk("Failed to get acmp device.");
        return -EINVAL;
    }

    acmp_config(acmp_dev, cfg);
    acmp_enable(acmp_dev);

    //gpio_dev = device_get_binding(PORT);
    //gpio_pin_configure(gpio_dev, OUTPUT2_GPIO_PIN, GPIO_MODE_OUTPUT);

    while (j++ < 10)
    {
        acmp_read(acmp_dev, cfg);

        printf("Func[%s] read value=%d \n", __FUNCTION__, cfg->result);

        if (i % 2 == 0)
        {
            //gpio_pin_write(gpio_dev, OUTPUT2_GPIO_PIN, GPIO_PIN_RESET);
        }
        else
        {
            //gpio_pin_write(gpio_dev, OUTPUT2_GPIO_PIN, GPIO_PIN_SET);
        }
        k_sleep(1000);
        i++;

    }
    printf("exit func[%s]... \n", __FUNCTION__);

    return 0;

}

int dac_cfg(struct dac_config_data *cfg)
{
    RET_CODE ret = RET_OK;
    unsigned int dac_out = 0;

    struct device *dac_dev0;
    struct device *dac_dev1;

    if (cfg->config_data.dacSel == DAC0_SEL)
    {
        dac_dev0 = device_get_binding(CONFIG_DAC_0_NAME);
        dac_dev0 = device_get_binding(CONFIG_DAC_0_NAME);
        if (dac_dev0 == NULL)
        {
            printk("Failed to get dac0 device.");
            return -EINVAL;
        }

        dac_config(dac_dev0, cfg);
        dac_enable(dac_dev0);
        dac_triger(dac_dev0, cfg);
        dac_read(dac_dev0, cfg);
    }
    else if (cfg->config_data.dacSel == DAC1_SEL)
    {
        dac_dev1 = device_get_binding(CONFIG_DAC_1_NAME);
        if (dac_dev1 == NULL)
        {
            printk("Failed to get dac1 device.");
            return -EINVAL;
        }
        dac_config(dac_dev1, cfg);
        dac_enable(dac_dev1);
        dac_triger(dac_dev1, cfg);
        dac_read(dac_dev1, cfg);
    }
    else if (cfg->config_data.dacSel == DAC_DUAL)
    {
        dac_dev0 = device_get_binding(CONFIG_DAC_0_NAME);
        dac_dev1 = device_get_binding(CONFIG_DAC_1_NAME);
        if (dac_dev0 == NULL)
        {
            printk("Failed to get dac0 device.");
            return -EINVAL;
        }
        if (dac_dev1 == NULL)
        {
            printk("Failed to get dac1 device.");
            return -EINVAL;
        }

        dac_config(dac_dev0, cfg);
        dac_enable(dac_dev0);
        dac_triger(dac_dev0, cfg);
        dac_read(dac_dev0, cfg);

        printf("Func[%s]: dac_out0 = [0x%x], dacSel = %d\n", __FUNCTION__, dac_out, cfg->config_data.dacSel);

        dac_config(dac_dev1, cfg);
        dac_enable(dac_dev1);
        dac_triger(dac_dev1, cfg);
        dac_read(dac_dev1, cfg);
        printf("Func[%s]: dac_out1 = [0x%x], dacSel = %d\n", __FUNCTION__, dac_out, cfg->config_data.dacSel);
    }

    return cfg->dac_data_out;
}

int demo(int demo_select)
{
    RET_CODE ret = 0;

    GPIO_PinConfig_T GPIO_InitStruct;
    DAC_Handle_T dac_handle;

    memset(&acmp_cfg, 0, sizeof(acmp_cfg));
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;

    memset(&daccfg.config_data, 0, sizeof(daccfg.config_data));

    if (demo_select == 1)
    {
        daccfg.config_data.dacSel = DAC0_SEL;
        daccfg.config_data.dacEn = ENABLE;
        daccfg.config_data.samholdEn = DISABLE;
        daccfg.config_data.waveType = DAC_WAVE_DISABLE;
        daccfg.config_data.msAM = 0;
        daccfg.config_data.dacDMAEn = DISABLE;
        daccfg.config_data.resolution = DAC_DATA_MODE_12BIT;
        daccfg.config_data.dacSoftTrigleEn = ENABLE;
        daccfg.config_data.dacComConfig.dacHoldData = 0xFFF;
        daccfg.config_data.dacComConfig.dacHoldSampTime = 0;
        daccfg.config_data.dacComConfig.dacHoldTime = 0;
        daccfg.config_data.dacComConfig.dacHoldRefTime = 0;
        daccfg.config_data.dacComConfig.dacDAC1DualHoldData = 0;
        daccfg.config_data.dacComConfig.dacDAC0DualHoldData = 0;
        daccfg.config_data.triggerMode = DAC_SINGLE_SOFT_TRIG;
        dac_cfg(&daccfg);
    }

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);

    /*
     ACMP2(GPIO15)  ACMP3(GPIO12)      N
     ACMP0(GPIOC13) ACMP1(GPIOC14)   P
    */

    if (demo_select == 0)
    {
        acmp_cfg.acmp_config.achyst_sel = 0;
        acmp_cfg.acmp_config.blankTimesel = NO_BLANKING;
        acmp_cfg.acmp_config.triggerMode = ANALOG_MATCH_FALLINGRISING;
        acmp_cfg.acmp_config.negativePin = ANALOG_COMPARE_PIN_ACMP3;
        acmp_cfg.acmp_config.positivePin = ANALOG_COMPARE_PIN_ACMP0;
        acmp_cfg.acmp_config.outputInversionEnable = ENABLE;
        acmp_cfg.acmp_config.outputPinEnable = DISABLE;
        acmp_cfg.acmp_config.filterThr = 255;
        acmp_demo(&acmp_cfg);
    }
    else if (demo_select == 1)
    {
        acmp_cfg.acmp_config.achyst_sel = 0;
        acmp_cfg.acmp_config.blankTimesel = NO_BLANKING;
        acmp_cfg.acmp_config.triggerMode = ANALOG_MATCH_FALLINGRISING;
        acmp_cfg.acmp_config.negativePin = ANALOG_COMPARE_DAC1_OUT;
        acmp_cfg.acmp_config.positivePin = ANALOG_COMPARE_PIN_ACMP1;
        acmp_cfg.acmp_config.outputInversionEnable = ENABLE;
        acmp_cfg.acmp_config.outputPinEnable = DISABLE;
        acmp_cfg.acmp_config.filterThr = 255;
        acmp_demo(&acmp_cfg);
    }

    return 0;
}

void gm_test_acmp(void)
{
    demo(0);
    demo(1);
}

void main(void)
{
    gm_test_acmp();
}
