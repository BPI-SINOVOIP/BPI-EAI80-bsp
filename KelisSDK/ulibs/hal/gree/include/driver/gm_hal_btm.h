/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_btm.h
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2019/10/18
 *
 * @brief         Header file of F6721B BTM Driver
 *
 * @note
 *    2019/10/18, edward.zhu, V1.0.0
 *        Support F6721B
 */


#ifndef _GM_HAL_BTM_H_
#define _GM_HAL_BTM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <btm/gm_ll_btm.h>

#define COUNTER_CNT 2
#define BTM_CLK   SYS_CLOCK

typedef void (*BTM_Callback)(int);
typedef void (*TimerOper)();
typedef void (*TimerOperWithArg)(uint32_t value);
typedef uint32_t (*TimerOperWithReturn)(void);

typedef enum
{
    BTM1_CLK_DIV_16 = 0U,
    BTM1_CLK_DIV_32,
    BTM1_CLK_DIV_64,
    BTM1_CLK_DIV_256
} BTM_PSC_T;

typedef struct
{
    BTM_Callback  counterCallback;
} BTM_Config_T;

typedef struct
{
    TimerOper enable;
    TimerOper disable;
    TimerOperWithArg set_tick_value;
    TimerOperWithReturn get_tick_value;
    TimerOperWithArg set_prescaler;
    uint32_t freq;
} BTM_Counter_T;

typedef struct
{
    uint32_t  btm_clk_freq;
    BTM_Counter_T btm_counter[COUNTER_CNT];
    BTM_Callback  counter_handle;

} BTM_Driver_T;

void HAL_BTM_Init(BTM_Config_T *pConfig);

void HAL_BTM1Irq_Enable(void);
void HAL_BTM1Irq_Disable(void);
void HAL_BTM1_Enable(void);
void HAL_BTM1_Disable(void);
void HAL_BTM1_SetTickValue(uint32_t value);
uint32_t HAL_BTM1_GetTickValue(void);
uint8_t HAL_BTM1_SetPrescaler(uint32_t value);

void HAL_BTM2Irq_Enable(void);
void HAL_BTM2Irq_Disable(void);
void HAL_BTM2_Enable(void);
void HAL_BTM2_Disable(void);
void HAL_BTM2_SetTickValue(uint32_t value);
uint32_t HAL_BTM2_GetTickValue(void);

uint32_t HAL_BTM_GetFrequence(int index);
int HAL_BTM_SetCounter(int id, uint32_t counter);
uint32_t HAL_BTM_GetCounter(int id);
int HAL_BTM_StartCounter(int id);
int HAL_BTM_StopCounter(int id);
uint32_t HAL_BTM_GetCurrentCounter(int index);
int HAL_BTM_SetCounterPrescaler(int id, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif //_GM_HAL_BTM_H_
