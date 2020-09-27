/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_btm.c
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2019/10/18
 *
 * @brief         BaseTimer Driver For F6721B.
 *
 * @note
 *    2019/10/18, edward, V1.2.0
 *        Compatible with F6721B
 */

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_btm.h"
#include "gm_ll_btm_dev.h"
#include "gm_hal_btm.h"
#include "gm_hal_reset.h"
#include "gm_hal_clock.h"
#include <gm_irq.h>

#define MASK_ALL                (0xFFFFFFFF)

static BTM_Driver_T sBtmDriverT;

/** @brief Pointers to BTM device for each instance. */
static BTM_Device_T *s_BTMDevice[MAX_BTM_NUM] = {BTM1, BTM2};

/** @brief Pointers to BTM IRQ number for each instance. */
const IRQn_Type s_BTMIrqs[] = {BASETIMER1_IRQn, BASETIMER2_IRQn};

void HAL_BASETIMER1_InterruptHandler(void);
void HAL_BASETIMER2_InterruptHandler(void);

void HAL_BTM_CommonIRQHandler(BTM_Device_T *pBTM)
{
    if (pBTM == BTM1)
    {
        if (IO_BIT_GET(BTM1->CTRL, BTM_CTRL_INT))
        {
            IO_BIT_SET(BTM1->CTRL, BTM_CTRL_INT);

            if (sBtmDriverT.counter_handle)
            {
                sBtmDriverT.counter_handle(0);
            }
        }
    }

    if (pBTM == BTM2)
    {
        if (IO_BIT_GET(BTM2->CTRL, BTM_CTRL_INT))
        {
            IO_BIT_SET(BTM2->CTRL, BTM_CTRL_INT);

            if (sBtmDriverT.counter_handle)
            {
                sBtmDriverT.counter_handle(1);
            }
        }
    }
}

void HAL_BASETIMER1_InterruptHandler(void)
{
    HAL_BTM_CommonIRQHandler(s_BTMDevice[0]);
}

void HAL_BASETIMER2_InterruptHandler(void)
{
    HAL_BTM_CommonIRQHandler(s_BTMDevice[1]);
}

/**
  * @brief  The following  function are used to reset BTM peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param rst_sel  Reset BTM1 or BTM2
  *
  * @retval None
  *
  */
void HAL_BTM_SW_Reset(int rst_sel)
{
    if (rst_sel == 0)
    {
        HAL_Reset_Module(RESET_BTM1_SW_RSTJ);
    }
    else if (rst_sel == 1)
    {
        HAL_Reset_Module(RESET_BTM2_SW_RSTJ);
    }
    else
    {
        HAL_Reset_Module(RESET_BTM1_SW_RSTJ);
        HAL_Reset_Module(RESET_BTM2_SW_RSTJ);
    }
}

/**
  * @brief  The following  functions are used to enable BTM peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param clken_sel   Enable BTM1 or BTM2 clock
  *
  * @retval None
  *
  */
void HAL_BTM_CLK_ENABLE(int clken_sel)
{
    if (clken_sel == 0)
    {
        HAL_CLOCK_PeripheralClockEnable1(PERIPHERAL_BTM1_MASK);
    }
    else if (clken_sel == 1)
    {
        HAL_CLOCK_PeripheralClockEnable1(PERIPHERAL_BTM2_MASK);
    }
    else
    {
        HAL_CLOCK_PeripheralClockEnable1(PERIPHERAL_BTM1_MASK);
        HAL_CLOCK_PeripheralClockEnable1(PERIPHERAL_BTM2_MASK);
    }
}

/**
  * @brief  The following  functions are used to disable BTM peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param clkdis_sel   Disable BTM1 or BTM2 clock
  *
  * @retval None
  *
  */
void HAL_BTM_CLK_DISABLE(int clkdis_sel)
{
    if (clkdis_sel == 0)
    {
        HAL_CLOCK_PeripheralClockDisable1(PERIPHERAL_BTM1_MASK);
    }
    else if (clkdis_sel == 1)
    {
        HAL_CLOCK_PeripheralClockDisable1(PERIPHERAL_BTM2_MASK);
    }
    else
    {
        HAL_CLOCK_PeripheralClockDisable1(PERIPHERAL_BTM1_MASK);
        HAL_CLOCK_PeripheralClockDisable1(PERIPHERAL_BTM2_MASK);
    }
}

void HAL_BTM_RequestIrq(int dev_sel)
{
    if (dev_sel == 0)
    {
        HAL_RequestIrq(BASETIMER1_IRQn, BASETIMER1_IRQ_PRIORITY, HAL_BASETIMER1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(BASETIMER1_IRQn);
    }

    else if (dev_sel == 1)
    {
        HAL_RequestIrq(BASETIMER2_IRQn, BASETIMER2_IRQ_PRIORITY, HAL_BASETIMER2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(BASETIMER2_IRQn);
    }

    else
    {
        HAL_RequestIrq(BASETIMER1_IRQn, BASETIMER1_IRQ_PRIORITY, HAL_BASETIMER1_InterruptHandler, 0);
        HAL_IRQ_ENABLE(BASETIMER1_IRQn);

        HAL_RequestIrq(BASETIMER2_IRQn, BASETIMER2_IRQ_PRIORITY, HAL_BASETIMER2_InterruptHandler, 0);
        HAL_IRQ_ENABLE(BASETIMER2_IRQn);
    }
}

void BTM_InterruptConfig(BTM_Device_T *pBTM)
{
#ifdef CONFIG_USE_HAL_TEST

#endif

#ifdef RABBIT_NO_OS_SYSTEM
    if (pBTM == BTM1)
    {
        HAL_IRQ_ENABLE(BASETIMER1_IRQn);
    }
    if (pBTM == BTM2)
    {
        HAL_IRQ_ENABLE(BASETIMER2_IRQn);
    }
#endif
}

void HAL_BTM_Init(BTM_Config_T *pConfig)
{
    BTM_Counter_T *counter;

    HAL_BTM_SW_Reset(2);

    HAL_BTM_CLK_ENABLE(2);

    HAL_BTM_RequestIrq(2);

    BTM_InterruptConfig(BTM1);

    BTM_InterruptConfig(BTM2);

    sBtmDriverT.btm_clk_freq = BTM_CLK;

    counter = &sBtmDriverT.btm_counter[0];
    counter->enable         = HAL_BTM1_Enable;
    counter->disable        = HAL_BTM1_Disable;
    counter->set_tick_value = HAL_BTM1_SetTickValue;
    counter->get_tick_value = HAL_BTM1_GetTickValue;
    counter->freq           = sBtmDriverT.btm_clk_freq; /* default */
    counter->set_prescaler  = HAL_BTM1_SetPrescaler;

    counter = &sBtmDriverT.btm_counter[1];
    counter->enable         = HAL_BTM2_Enable;
    counter->disable        = HAL_BTM2_Disable;
    counter->set_tick_value = HAL_BTM2_SetTickValue;
    counter->get_tick_value = HAL_BTM2_GetTickValue;
    counter->freq           = sBtmDriverT.btm_clk_freq; /* default */

    sBtmDriverT.counter_handle = NULL;//pConfig->counterCallback;
}

void HAL_BTM1Irq_Disable(void)
{
    IO_BIT_CLEAR(BTM1->CTRL, BTM_CTRL_IER);

    NVIC_DisableIRQ(BASETIMER1_IRQn);
}

void HAL_BTM2Irq_Disable(void)
{
    IO_BIT_CLEAR(BTM2->CTRL, BTM_CTRL_IER);
    NVIC_DisableIRQ(BASETIMER2_IRQn);
}

void HAL_BTM1_Enable(void)
{
    IO_BIT_SET(BTM1->CTRL, BTM_CTRL_IER);

    IO_BIT_SET(BTM1->CTRL, BTM_CTRL_EN);
}

void HAL_BTM2_Enable(void)
{
    IO_BIT_SET(BTM2->CTRL, BTM_CTRL_IER);

    IO_BIT_SET(BTM2->CTRL, BTM_CTRL_EN);
}

void HAL_BTM1_Disable(void)
{
    IO_BIT_CLEAR(BTM1->CTRL, BTM_CTRL_EN);

    HAL_BTM1Irq_Disable();
}

void HAL_BTM2_Disable(void)
{
    IO_BIT_CLEAR(BTM2->CTRL, BTM_CTRL_EN);

    HAL_BTM2Irq_Disable();
}

void HAL_BTM1_SetTickValue(uint32_t value)
{
    IO_BITMASK_SET(BTM1->COMPARE, MASK_ALL, value);
}

void HAL_BTM2_SetTickValue(uint32_t value)
{
    IO_BITMASK_SET(BTM2->COMPARE, MASK_ALL, value);
    IO_BITMASK_SET(BTM2->RTCCNT, MASK_ALL, value);
}

uint32_t HAL_BTM1_GetTickValue(void)
{
    return IO_BITS_GET(BTM1->RTCCNT, MASK_ALL);
}

uint32_t HAL_BTM2_GetTickValue(void)
{
    return IO_BITS_GET(BTM2->RTCCNT, MASK_ALL);
}

uint8_t HAL_BTM1_SetPrescaler(uint32_t value)
{
    IO_BITMASK_SET(BTM1->CTRL, BTM_CTRL_CLK_DIV, BTM_CTRL_CLKDIV_FIELD(value));

    return value;
}

uint32_t HAL_BTM_GetFrequence(int index)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return 0;
    }

    return sBtmDriverT.btm_counter[index].freq;
}

int HAL_BTM_SetCounter(int index, uint32_t count)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }

    sBtmDriverT.btm_counter[index].set_tick_value(count);

    return 0;
}

int HAL_BTM_StartCounter(int index)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }

    sBtmDriverT.btm_counter[index].enable();

    return 0;
}

int HAL_BTM_StopCounter(int index)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }

    sBtmDriverT.btm_counter[index].disable();

    return 0;
}

uint32_t HAL_BTM_GetCurrentCount(int index)
{
    return sBtmDriverT.btm_counter[index].get_tick_value();
}

int HAL_BTM_SetCounterPrescaler(int index, uint32_t value)
{
    uint32_t prescaler_value;

    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }

    if (value == BTM1_CLK_DIV_16)
    {
        prescaler_value = 16;
    }
    else if (value == BTM1_CLK_DIV_32)
    {
        prescaler_value = 32;
    }
    else if (value == BTM1_CLK_DIV_64)
    {
        prescaler_value = 64;
    }
    else if (value == BTM1_CLK_DIV_256)
    {
        prescaler_value = 256;
    }
    else
    {
        return -1;
    }

    sBtmDriverT.btm_counter[index].set_prescaler(value);
    sBtmDriverT.btm_counter[index].freq /= prescaler_value;

    return 0;
}
