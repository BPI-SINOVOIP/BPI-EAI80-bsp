/*
* Copyright (c) 2016 Linaro Limited.
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file Header file for OV7725 CMOS Image Sensor.
 */
#include <errno.h>
#include <kernel.h>
#include <ugelis.h>

#include <arch/cpu.h>
#include <misc/__assert.h>
#include <board.h>
#include <init.h>
#include <gpio.h>

#include "camera/gpio_sccb.h"

struct sccb_data
{
    struct device *scl_port;
    struct device *sda_port;
    uint8_t  scl_pin;
    uint8_t  sda_pin;
};

struct sccb_config
{
    char *scl_port_name;
    char *sda_port_name;
    uint8_t  scl_pin;
    uint8_t  sda_pin;
};

static void sccb_setsda_dir(struct device *dev, uint8_t dir)
{
    struct sccb_data *data = dev->driver_data;
    gpio_pin_configure(data->sda_port,
                       data->sda_pin,
                       dir | GPIO_PUD_PULL_UP);
}

static void sccb_setscl_dir(struct device *dev, uint8_t dir)
{
    struct sccb_data *data = dev->driver_data;
    gpio_pin_configure(data->scl_port,
                       data->scl_pin,
                       dir | GPIO_PUD_PULL_UP);
}

static void sccb_setsda_val(struct device *dev, uint8_t val)
{
    struct sccb_data *data = dev->driver_data;
    sccb_setsda_dir(dev, GPIO_DIR_OUT);
    gpio_pin_write(data->sda_port,
                   data->sda_pin,
                   val);
}

static void sccb_setscl_val(struct device *dev, uint8_t val)
{
    struct sccb_data *data = dev->driver_data;
    sccb_setscl_dir(dev, GPIO_DIR_OUT);
    gpio_pin_write(data->scl_port,
                   data->scl_pin,
                   val);
}

static uint8_t sccb_getsda(struct device *dev)
{
    struct sccb_data *data = dev->driver_data;
    uint32_t val;

    sccb_setsda_dir(dev, GPIO_DIR_IN);
    gpio_pin_read(data->sda_port,
                  data->sda_pin,
                  &val);

    return !!val;
}

static void sccb_delay(void)
{
    k_busy_wait(5);
}

int sccb_gm_start(struct device *dev)
{
    int ret;
    sccb_setsda_val(dev, 1);
    sccb_setscl_val(dev, 1);
    sccb_delay();
    //START:when CLK is high,DATA change form high to low
    sccb_setsda_val(dev, 0);
    sccb_delay();
    sccb_setscl_val(dev, 0);
    ret = 0;
    return ret;
}

int sccb_gm_stop(struct device *dev)
{
    int ret;
    sccb_setsda_val(dev, 0);
    sccb_delay();
    sccb_setscl_val(dev, 1);
    sccb_delay();
    sccb_setsda_val(dev, 1);
    sccb_delay();
    ret = 0;
    return ret;
}

int sccb_gm_nack(struct device *dev)
{
    int ret;
    sccb_delay();
    sccb_setsda_val(dev, 1);
    sccb_setscl_val(dev, 1);
    sccb_delay();
    sccb_setscl_val(dev, 0);
    sccb_delay();
    sccb_setsda_val(dev, 0);
    sccb_delay();
    ret = 0;
    return ret;
}

uint8_t sccb_gm_sendbyte(struct device *dev, uint8_t data)
{
    uint8_t i, res;

    for (i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            sccb_setsda_val(dev, 1);
        }
        else
        {
            sccb_setsda_val(dev, 0);
        }
        data <<= 1;
        sccb_delay();
        sccb_setscl_val(dev, 1);
        sccb_delay();
        sccb_setscl_val(dev, 0);
    }

    sccb_setsda_dir(dev, GPIO_DIR_IN);
    sccb_delay();
    sccb_setscl_val(dev, 1);
    sccb_delay();

    if (sccb_getsda(dev))
    {
        res = 1;
    }
    else
    {
        res = 0;
    }

    sccb_setscl_val(dev, 0);

    sccb_setsda_dir(dev, GPIO_DIR_OUT);

    return res;
}

uint8_t sccb_gm_readbyte(struct device *dev)
{
    uint8_t i, receive = 0;

    sccb_setsda_dir(dev, GPIO_DIR_IN);

    for (i = 0; i < 8; i++)
    {
        sccb_delay();
        sccb_setscl_val(dev, 1);
        receive <<= 1;
        if (sccb_getsda(dev))
        {
            receive++;
        }
        sccb_delay();
        sccb_setscl_val(dev, 0);
    }

    sccb_setsda_dir(dev, GPIO_DIR_OUT);

    return receive;
}

static const struct sccb_driver_api sccb_drv_api_funcs =
{
    .start = sccb_gm_start,
    .stop = sccb_gm_stop,
    .nack = sccb_gm_nack,
    .sendbyte = sccb_gm_sendbyte,
    .readbyte = sccb_gm_readbyte,
};

static int sccb_init(struct device *dev)
{
    struct sccb_data *data = dev->driver_data;
    const struct sccb_config *config = dev->config->config_info;
    int ret = 0;

    data->scl_port = device_get_binding(config->scl_port_name);
    data->sda_port = device_get_binding(config->sda_port_name);
    if ((!data->scl_port) | (!data->sda_port))
    {
        return -EINVAL;
    }
    data->scl_pin = config->scl_pin;
    data->sda_pin = config->sda_pin;
    return ret;
}

#if defined(CONFIG_SCCB_0)
static struct sccb_data sccb_dev_data_0;
static struct sccb_config sccb_dev_cfg_0 =
{
    .scl_port_name = CAMERA_0_GPIO_SCL_PORT,
    .sda_port_name = CAMERA_0_GPIO_SDA_PORT,
    .scl_pin = CAMERA_0_GPIO_SCL_PIN,
    .sda_pin = CAMERA_0_GPIO_SDA_PIN,
};

DEVICE_AND_API_INIT(sccb_0, CONFIG_CAMERA_0_SCCB_NAME,
                    sccb_init,
                    &sccb_dev_data_0,
                    &sccb_dev_cfg_0,
                    PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &sccb_drv_api_funcs);
#endif

#if defined(CONFIG_SCCB_1)
static struct sccb_data sccb_dev_data_1;
static struct sccb_config sccb_dev_cfg_1 =
{
    .scl_port_name = CAMERA_1_GPIO_SCL_PORT,
    .sda_port_name = CAMERA_1_GPIO_SDA_PORT,
    .scl_pin = CAMERA_1_GPIO_SCL_PIN,
    .sda_pin = CAMERA_1_GPIO_SDA_PIN,
};

DEVICE_AND_API_INIT(sccb_1, CONFIG_CAMERA_1_SCCB_NAME,
                    sccb_init,
                    &sccb_dev_data_1,
                    &sccb_dev_cfg_1,
                    PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &sccb_drv_api_funcs);
#endif
