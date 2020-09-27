/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          main.c
 *
 * @author        Mark.Lee
 *
 * @version       1.0.0
 *
 * @date          2020/02/29
 *
 * @brief         Main file to power manager ai demo
 *
 * @note
 *    2020/02/29, Mark.lee, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <kernel.h>
#include <board.h>
#include <gm_hal_sqi.h>
#include <gm_hal_gpio.h>
#include <easynet/easynet.h>
#include "protocol/protocol.h"

dnn_context_t dnn_context;
power_mgr_t  pm_info;

/***************************************************
 *
 * Flash Parameters
 *
 ***************************************************/
#define BITBAND(addr, bitnum) ((addr&0xf0000000)+0x2000000+((addr&0xfffff)<<5)+(bitnum<<2))
#define MEMADDR(addr)  *((__IO uint32_t*)(addr))

#define OPS_0_BIN_FLASH_SIZE      (0xA000)
#define OPS_1_BIN_FLASH_SIZE      (0xA000)

#define AI_FLASH_START_OFFSET   (0x00050000)
#define OPS_0_BIN_FLASH_START     (AI_FLASH_START_OFFSET)
#define OPS_1_BIN_FLASH_START    (OPS_0_BIN_FLASH_START+OPS_0_BIN_FLASH_SIZE)

float input_data[] = {72, 1130, 28.9, 40, 36.0};
float x_first_mean[] = {47.51237338, 1014.23820121,   25.93180353,   54.93038075,   35.86950494};
float x_first_scale[] = {22.75608658, 225.82341429,   6.30319354,  16.98228263,   6.98244563};
float y_first_mean[] = {2351.37976238};
float y_first_scale[] = {997.31648361};

float x_second_mean[] = {1014.23820121,   25.93180353,   54.93038075,   35.86950494, 2351.37976238};
float x_second_scale[] = {225.82341429,   6.30319354,  16.98228263,   6.98244563, 997.31648361};
float y_second_mean[] = {47.51237338};
float y_second_scale[] = {22.75608658};

typedef struct dev_flash
{
    SQI_Device_T *sqi_dev;
    struct SQI_Handle handler;
    struct SQI_Config config;
} dev_flash_t;

typedef struct ops_info
{
    int size;
    int src;
    int dst;
} ops_info_t;

typedef struct easynet_worker
{
    char *ops;
    struct easynet_dev *dev;
} easynet_worker_t;

int32_t flash_init(dev_flash_t *dev)
{
    int32_t ret;

    dev->sqi_dev = (SQI_Device_T *)EFLASH_CTRL_BASE;
    memset(&dev->config, 0, sizeof(struct SQI_Config));
    //    dev->config.bit_mode = SQI_4BIT_MODE;
    //    dev->config.read_mode = SQI_READ_BY_CPU;
    //    dev->config.clk_div = SCLK_DIV;
    ret  = HAL_SQI_Init(&dev->handler, dev->sqi_dev, &dev->config);

    return ret;
}

int32_t flash_dev_read(dev_flash_t *dev, uint32_t src_addr, uint32_t dest_addr, uint32_t read_size)
{
    return HAL_SQI_Read(&dev->handler, src_addr, (void *)dest_addr, read_size);
}

/** load ops stream **/
void load_ops_file(dev_flash_t *dev, ops_info_t *info)
{
    flash_dev_read(dev, info->src, info->dst, info->size);
}

static float *predict(easynet_worker_t *easynet)
{
    int ret = 0;
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(easynet->dev);
        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                easynet_process_update(easynet->ops, &inputImage[0], &OutputImage[0]);
                return OutputImage;
            }
            return 0;
        }
    } while (1);

    return 0;
}

static void predict_process(easynet_worker_t *easynet)
{
    float *vector = 0;
    int i = 0;
    uint32_t start_time, end_time;

    while (1)
    {
        int ret = 0;
        ret = k_sem_take(&dnn_context.predict_lock, 5000);
        if (ret == 0)
        {
            start_time = k_uptime_get_32();
            if (dnn_context.reset == 0)
            {
                input_data[0] = dnn_context.freq;
                input_data[1] = dnn_context.indoorfan_speed;
                input_data[2] = dnn_context.indoor_temp;
                input_data[3] = dnn_context.humidity;
                input_data[4] = dnn_context.outdoor_temp;

                for (i = 0; i < 5; i++)
                {
                    input_data[i]  -= x_first_mean[i];
                    input_data[i]  /= x_first_scale[i];

                    //printf("input data: %lf \n", input_data[i]);
                }

                easynet->ops = dnn_first_weight;
                easynet->dev = easynet_process_init(easynet->ops);
                vector = predict(easynet);
                if (vector)
                {
                    /* network is finished, to do post work */
                    vector[0]  = vector[0] * y_first_scale[0];
                    vector[0]  += y_first_mean[0];
                    //printf("simulator output data: %lf \n", vector[i]);

                    dnn_context.coolingcapacity = vector[0];
                }

                soft_powermanager_operation(&pm_info, &dnn_context);

                input_data[0] = pm_info.output_indoorfan_speed;
                input_data[1] = dnn_context.indoor_temp;
                input_data[2] = dnn_context.humidity;
                input_data[3] = dnn_context.outdoor_temp;
                input_data[4] = pm_info.output_coolingcapacity;

                for (i = 0; i < 5; i++)
                {
                    input_data[i]  -= x_second_mean[i];
                    input_data[i]  /= x_second_scale[i];

                    //printf("input data: %lf \n", input_data[i]);
                }

                easynet->ops = dnn_second_weight;
                easynet->dev = easynet_process_init(easynet->ops);
                vector = predict(easynet);
                if (vector)
                {
                    /* network is finished, to do post work */
                    vector[0]  = vector[0] * y_second_scale[0];
                    vector[0]  += y_second_mean[0];
                    //printf("simulator output data: %lf \n", vector[0]);

                    /* power manager post processing */
                    if (pm_info.status == AIR_CHANGE && pm_info.capacity_status == AIR_DOWN)
                    {
                        if (dnn_context.freq >= FREQ_LIMIT)
                        {
                            pm_info.output_freq = (vector[0] < FREQ_LIMIT) ? FREQ_LIMIT : vector[0];
                        }
                        else
                        {
                            pm_info.output_freq = dnn_context.freq;
                        }
                    }
                    else if (pm_info.status == AIR_CHANGE && pm_info.capacity_status == AIR_UP)
                    {
                        pm_info.output_freq = (vector[0] > FREQ_LIMIT) ? FREQ_LIMIT : vector[0];
                    }
                    else
                    {
                        pm_info.output_freq = (vector[0] > dnn_context.freq) ? dnn_context.freq : vector[0];
                    }

                    protocol_send(&pm_info, &dnn_context);
                }
            }
            else
            {
                soft_powermanager_operation(&pm_info, &dnn_context);
            }

            end_time = k_uptime_get_32();
            //printf("cost: %d ms\n", end_time - start_time);
        }
    }
}

static int easynet_worker_init(easynet_worker_t *easynet, void *param)
{
    easynet->ops = param;
    easynet->dev = easynet_process_init(easynet->ops);

    return 0;
}

int main()
{
    easynet_worker_t *easynet = malloc_ext(0, sizeof(easynet_worker_t));
    dev_flash_t *flash_dev = malloc_ext(0, sizeof(dev_flash_t));
    GPIO_PinConfig_T GPIO_InitStruct;
    ops_info_t info;
    int i = 0;

    *(volatile uint32_t *)0x400002CC &= ~(0xF);
    *(volatile uint32_t *)0x400002CC |= 0xA;                  //Configure HOSC pinmux
    *(volatile uint32_t *)0x40000178 = 0x31;                  //Enable HOSC
    //while(!(*(volatile uint32_t*)0x400001a4 && 0x4));       //Wait HOSC stable

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    GPIO_InitStruct.pin = GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOB, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOB_BASE, 5)) = 0x0;
    MEMADDR(BITBAND(GPIOB_BASE, 7)) = 0x0;
    MEMADDR(BITBAND(GPIOB_BASE, 12)) = 0x0;
    MEMADDR(BITBAND(GPIOB_BASE, 13)) = 0x0;
    MEMADDR(BITBAND(GPIOB_BASE, 14)) = 0x0;
    MEMADDR(BITBAND(GPIOB_BASE, 15)) = 0x0;

    GPIO_InitStruct.pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOC, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOC_BASE, 2)) = 0x0;
    MEMADDR(BITBAND(GPIOC_BASE, 3)) = 0x0;
    MEMADDR(BITBAND(GPIOC_BASE, 4)) = 0x0;
    MEMADDR(BITBAND(GPIOC_BASE, 5)) = 0x0;
    MEMADDR(BITBAND(GPIOC_BASE, 8)) = 0x0;
    MEMADDR(BITBAND(GPIOC_BASE, 10)) = 0x0;
    MEMADDR(BITBAND(GPIOC_BASE, 11)) = 0x0;

    GPIO_InitStruct.pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOE, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOE_BASE, 1)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 3)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 4)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 5)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 7)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 8)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 9)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 10)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 11)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 12)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 13)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 14)) = 0x0;
    MEMADDR(BITBAND(GPIOE_BASE, 15)) = 0x0;

    GPIO_InitStruct.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOF, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOF_BASE, 0)) = 0x0;
    MEMADDR(BITBAND(GPIOF_BASE, 1)) = 0x0;
    MEMADDR(BITBAND(GPIOF_BASE, 2)) = 0x0;
    MEMADDR(BITBAND(GPIOF_BASE, 3)) = 0x0;
    MEMADDR(BITBAND(GPIOF_BASE, 4)) = 0x0;

    GPIO_InitStruct.pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOH, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOH_BASE, 4)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 5)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 7)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 8)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 9)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 10)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 11)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 12)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 14)) = 0x0;
    MEMADDR(BITBAND(GPIOH_BASE, 15)) = 0x0;

    GPIO_InitStruct.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOI, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOI_BASE, 0)) = 0x0;
    MEMADDR(BITBAND(GPIOI_BASE, 1)) = 0x0;
    MEMADDR(BITBAND(GPIOI_BASE, 2)) = 0x0;
    MEMADDR(BITBAND(GPIOI_BASE, 3)) = 0x0;

    GPIO_InitStruct.pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    if (HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct) != RET_OK)
    {
        return RET_ERROR;
    }

    MEMADDR(BITBAND(GPIOJ_BASE, 1)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 2)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 3)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 4)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 5)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 6)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 7)) = 0x0;
    MEMADDR(BITBAND(GPIOJ_BASE, 8)) = 0x0;

    flash_init(flash_dev);

    info.src   = OPS_0_BIN_FLASH_START;
    info.size  = OPS_0_BIN_FLASH_SIZE;
    info.dst   = dnn_first_weight;

    load_ops_file(flash_dev, &info);

    info.src   = OPS_1_BIN_FLASH_START;
    info.size  = OPS_1_BIN_FLASH_SIZE;
    info.dst   = dnn_second_weight;

    load_ops_file(flash_dev, &info);

    soft_powermanager_init(&pm_info);
    protocol_start(&dnn_context);

    easynet_worker_init(easynet, dnn_first_weight);

    easynet->dev->input = input_data;

#if 0
    dnn_context.freq = input_data[0];
    dnn_context.indoorfan_speed = input_data[1];
    dnn_context.indoor_temp = input_data[2];
    dnn_context.humidity = input_data[3];
    dnn_context.outdoor_temp = input_data[4];
    dnn_context.target_temp = 22;
    dnn_context.coolingcapacity = 0;
    dnn_context.totalpower = 0;
    dnn_context.reset = 0;

    dnn_context.target_indoorfan_speed = dnn_context.indoorfan_speed;
    dnn_context.temp_delta = (int)((dnn_context.indoor_temp - dnn_context.target_temp) * 10);
    dnn_context.temperature = (int)(dnn_context.indoor_temp * 10);

    k_sem_give(&dnn_context.predict_lock);
#endif

    predict_process(easynet);

    free_ext(0, easynet);
    easynet = NULL;

    free_ext(0, flash_dev);
    flash_dev = NULL;

    return 0;
}
