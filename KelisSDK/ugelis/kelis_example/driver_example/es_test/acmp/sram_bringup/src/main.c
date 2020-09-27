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
  * @brief       ACMP Source file
  *
  * @note
  *              2019/01/24, Richard.Liu, v1.0.0
  *                  Initial version.
  *
  **/

#include <ugelis.h>
#include <misc/printk.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <acmp.h>
#include <gm_hal_gpio.h>
#include "pes_init.h"

//#define TEST_CASE1  1
#define TEST_CASE2  0
#define TEST_CASE3  0
//#define TEST_CASE4  0
//#define TEST_CASE5  1

#define TEST_CASE6  0

//#define TEST_CASE7_APP  1

#define TEST_CASE8_ACMP2PES 1

#define TEST_CASE8  0

#define TEST_CASE9  0
#define TEST_CASE10  0

#define PR_DEBUG  printk

/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Reference variables
*******************************************************************************/
struct gls_data
{
    struct device *acmp;
    struct acmp_config_data acmp_data;
};

struct gls_data g_acmp_handle;

/*******************************************************************************
* Local function prototypes
*******************************************************************************/

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
static void gpio_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 1);
}

static void gpio_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 1);
}

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbac;
    HW32_REG(0x1ffff0ac) = 0xff1fffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;

}

static void ACMP_Callback(ACMP_Handle_T *pHandle, ACMP_Event_T event, unsigned long Param)
{
    static int cnt = 0;

    printk("negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", cnt++);
    if (event == ACMP_MATCH_EVENT)
    {
        printk("negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", cnt++);
    }
}

/*
    ACMP3(GPIO12)  ACMP0(GPIOC13)    ACMP1(GPIOC14) ACMP2(GPIOC15)
*/
static int acmp_demo(void)
{
    unsigned int data = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    GPIO_PinConfig_T GPIO_InitStruct;

    printk("enter func[%s]... \n", __FUNCTION__);
    g_acmp_handle.acmp = device_get_binding(CONFIG_ACMP_0_NAME);
    g_acmp_handle.acmp_data.callback = ACMP_Callback;

    if (g_acmp_handle.acmp == NULL)
    {
        printk("Failed to get acmp device.");
        return -EINVAL;
    }

    g_acmp_handle.acmp_data.acmp_config.outputInversionEnable = ENABLE;
    //g_acmp_handle.acmp_data.acmp_config.trigerAdcEnable = DISABLE;
    g_acmp_handle.acmp_data.acmp_config.filterThr = 255;

    g_acmp_handle.acmp_data.acmp_config.negativePin = ANALOG_COMPARE_PIN_ACMP2;
    g_acmp_handle.acmp_data.acmp_config.positivePin = ANALOG_COMPARE_PIN_ACMP0;
    g_acmp_handle.acmp_data.acmp_config.outputPinEnable = ENABLE;
    g_acmp_handle.acmp_data.acmp_config.triggerMode = ANALOG_MATCH_FALLINGRISING;
    g_acmp_handle.acmp_data.acmp_config.blankTimesel = NO_BLANKING;

    acmp_config(g_acmp_handle.acmp, &g_acmp_handle.acmp_data);
    acmp_enable(g_acmp_handle.acmp);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_PinWrite(GPIO_BIT_ADDR((0 + 16)), GPIO_PIN_SET);

    while (1)
    {
        acmp_read(g_acmp_handle.acmp, &g_acmp_handle.acmp_data);
        if (j % 2 == 0)
        {
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR((0 + 16)), GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR((0 + 16)), GPIO_PIN_SET);
        }
        k_sleep(200);
        j++;

    }
    printf("exit func[%s]... \n", __FUNCTION__);

    return 0;
}

/*
    ACMP3(GPIO12)  ACMP0(GPIOC13)    ACMP1(GPIOC14) ACMP2(GPIOC15)
*/
static int acmp2pes_demo(void)
{
    unsigned int data = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    uint32_t    error   = 0;
    GPIO_PinConfig_T GPIO_InitStruct;

    printk("enter func[%s]... \n", __FUNCTION__);
    g_acmp_handle.acmp = device_get_binding(CONFIG_ACMP_0_NAME);
    g_acmp_handle.acmp_data.callback = ACMP_Callback;

    if (g_acmp_handle.acmp == NULL)
    {
        printk("Failed to get acmp device.");
        return -EINVAL;
    }

    g_acmp_handle.acmp_data.acmp_config.outputInversionEnable = ENABLE;
    g_acmp_handle.acmp_data.acmp_config.filterThr = 255;

    g_acmp_handle.acmp_data.acmp_config.negativePin = ANALOG_COMPARE_PIN_ACMP2;
    g_acmp_handle.acmp_data.acmp_config.positivePin = ANALOG_COMPARE_PIN_ACMP0;
    g_acmp_handle.acmp_data.acmp_config.outputPinEnable = ENABLE;
    g_acmp_handle.acmp_data.acmp_config.triggerMode = ANALOG_MATCH_RISING;
    g_acmp_handle.acmp_data.acmp_config.blankTimesel = NO_BLANKING;
    g_acmp_handle.acmp_data.acmp_config.achyst_sel = ACHYST_SEL_0;

    acmp_config(g_acmp_handle.acmp, &g_acmp_handle.acmp_data);
    acmp_enable(g_acmp_handle.acmp);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_PinWrite(GPIO_BIT_ADDR((1 + 16)), GPIO_PIN_RESET); /*GPIOB1*/

    do
    {
        k_sleep(200);
        j++;

        for (i = 0; i < 12; i++)
        {
            pes_recieve_initial(i, MASTER_ACMP, 0, PES_ASNY_EN, 0, DETECT_EN);
            //acmp_ac0_src_low_init();
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR((0 + 16)), GPIO_PIN_RESET); /*GPIOB0*/
            sam_ac0_out(0);
            //acmp_ac0_src_high_init();
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR((0 + 16)), GPIO_PIN_SET); /*GPIOB0*/
            sam_ac0_out(1);
            error += pes_recieve_detect(i);
            pes_recieve_initial(i, 0, 0, 0, 0, 0);
        }

        if (error != 0)
        {
            printf("\n*** ERROR !!!! ACMP event to PES test failed ***\n");
        }
        else
        {
            printf("\n*** SUCCESS !!!! ACMP event to PES test passed ***\n");
        }
    } while (0);
    printf("exit func[%s]... \n", __FUNCTION__);

    return 0;
}

int acmp2pes_demo__(void)
{
    int i = 0;
    int read_value;
    uint32_t    prog_val       = 0;
    uint32_t    new_prog_val   = 0;
    uint32_t    reg_addr        ;
    uint32_t    error   = 0;

    printf("\n **** ACMP event to PES test begin ****\n");

    //ACMP clock enable
    HW32_REG(0x40000080)    |= 1 << 10;
    //ACMP reset enable
    HW32_REG(0x40000070)    |= 1 << 10;

    HW32_REG(ACMP_BASE_ADDR + 0x04) |= 1 << 0 ;
    AC0_EVNT_OUT_SET;

    for (i = 0; i < 12; i++)
    {
        pes_recieve_initial(i, MASTER_ACMP, 0, PES_ASNY_EN, 0, DETECT_EN);
        acmp_ac0_src_low_init();
        sam_ac0_out(0);
        acmp_ac0_src_high_init();
        sam_ac0_out(1);
        error += pes_recieve_detect(i);
        pes_recieve_initial(i, 0, 0, 0, 0, 0);
    }

    if (error != 0)
    {
        printf("\n*** ERROR !!!! ACMP event to PES test failed ***\n");
    }
    else
    {
        printf("\n*** SUCCESS !!!! ACMP event to PES test passed ***\n");
    }

    return 0;

}

static void dump_reg(ACMP_Device_T *pACMP, bool flag)
{
    if (flag != 0)
    {
        PR_DEBUG("pACMP->CS=0x%x\n", pACMP->CS);
        PR_DEBUG("pACMP->FILTTHR=0x%x\n", pACMP->FILTTHR);
    }
}


#if defined(TEST_CASE1)
void acmp_reg_access(ACMP_Device_T *pACMP) //   0x4000D004   = 0x 000F2001
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pACMP, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");
    pACMP->CS = 0xFFFFFFFF;
    pACMP->FILTTHR = 0xFFFF;
    dump_reg(pACMP, 1);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE4)
void acmp_clk_test(ACMP_Device_T *pACMP)
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pACMP, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");
    pACMP->CS = 0xFFFFFFFF;
    pACMP->FILTTHR = 0xFFFF;
    dump_reg(pACMP, 1);
    PR_DEBUG("===========================\n\n");
    PR_DEBUG("after ACMP clock gate reg: func[%s]\n", __FUNCTION__);

    IO_BITMASK_SET(SYSRSTCLKREG->CLK_EN_CTRL[0], (1 << 10), (0 << 10)); // gating clock
    dump_reg(pACMP, 1);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}

#endif

#if defined(TEST_CASE5)
void acmp_reset_test(ACMP_Device_T *pACMP)
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pACMP, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");
    pACMP->CS = 0xFFFFFFFF;
    pACMP->FILTTHR = 0xFFFF;
    dump_reg(pACMP, 1);
    PR_DEBUG("===========================\n\n");
    PR_DEBUG("after ACMP RESET reg: func[%s]\n", __FUNCTION__);

    IO_BITMASK_SET(SYSRSTCLKREG->IP_RST_CTRL[0], (1 << 10), (0 << 10));
    ACMP_RESET();
    dump_reg(pACMP, 1);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

void main(void)
{
    struct device *gpiob;
    int i = 0;
    uint32_t val = 0;

    patch_config();
    gpio_clk_enable();
    k_sleep(100);
    gpio_reset_release();
    k_sleep(100);

#ifdef TEST_CASE1
    ACMP_CLKEN();
    ACMP_RESET();
    acmp_reg_access(ACMP_DEV);

#endif

#ifdef TEST_CASE4
    ACMP_CLKEN();
    ACMP_RESET();
    acmp_clk_test(ACMP_DEV);
#endif

#ifdef TEST_CASE5
    ACMP_CLKEN();
    ACMP_RESET();
    acmp_reset_test(ACMP_DEV);
#endif

    printk("\n acmp demo \n");
#ifdef TEST_CASE8_ACMP2PES
    ACMP_CLKEN();
    ACMP_RESET();
    acmp2pes_demo();
#endif

#ifdef TEST_CASE7_APP
    ACMP_CLKEN();
    ACMP_RESET();
    acmp_demo();
#endif

    return;
}

