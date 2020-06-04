/*
 * Copyright (c) 2016, Gree Instruments Incorporated
 *
 */

#include <i2c.h>
#include <soc.h>
#include <misc/byteorder.h>
#include <misc/__assert.h>
#include <gm_hal_i2c.h>
#include <arch/cpu.h>
#include <misc/printk.h>
#include <errno.h>
#include <kernel.h>
#include <misc/util.h>
#define SYS_LOG_NO_NEWLINE
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_I2C_LEVEL
#include <logging/sys_log.h>
#include <string.h>
#include <init.h>
#include "i2c_gm.h"

#define CONFIG_I2C_0_DEFAULT_CFG  0x0
#define CONFIG_I2C_1_DEFAULT_CFG  0x0
#define CONFIG_I2C_2_DEFAULT_CFG  0x0

#define DATA_SIZE 17
typedef void (*i2c_irq_callback_t)(uint8_t event, unsigned long param);
I2C_Handle_T g_i2cHandle;
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF
uint16_t s_dmaBufferLen = DATA_SIZE;
uint16_t s_dmabuffer[DATA_SIZE] = {0};
uint32_t s_mstTransCmplt = 0;
uint32_t s_slvTransCmplt = 0;
#define DEV_CFG(dev) \
    ((const struct i2c_gm_config * const)(dev)->config->config_info)
#define DEV_DATA(dev) \
    ((struct i2c_gm_data * const)(dev)->driver_data)
#define DEV_STRUCT(dev) \
    ((I2C_Device_T *)(DEV_CFG(dev))->base)

struct i2c_gm_config
{
    uint32_t *base;
    uint32_t sys_clk_freq;
    void (*irq_config_func)(struct device *dev);
    union dev_config default_cfg;
};

struct i2c_gm_data
{
#ifdef CONFIG_I2C_INTERRUPT_DRIVEN
    i2c_irq_callback_t cb; /**< Callback function pointer */
#endif /* CONFIG_UART_INTERRUPT_DRIVEN */
    I2C_Handle_T i2c_handle;
    I2C_Config_T i2cConfig;
};
static void i2c_gm_config_func_0(struct device *dev);
static void i2c_gm_config_func_1(struct device *dev);
static void i2c_gm_config_func_2(struct device *dev);

static I2C_Handle_T I2C0_hdl;
static I2C_Handle_T I2C1_hdl;
static I2C_Handle_T I2C2_hdl;

void set_flag_tx(uint8_t event, unsigned long param)
{
    if (event == I2C_EVENT_TRANSFER_DONE)
    {
        s_mstTransCmplt = 1;
    }

    if (event == I2C_EVENT_TX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_UNDERFLOW)
    {

    }

    if (event == I2C_EVENT_ARB_LOST)
    {

    }

    if (event == I2C_EVENT_TERMINATED)
    {

    }

    if (event == I2C_EVENT_DMAERROR)
    {

    }

    if (event == I2C_EVNET_TRANSFER_ERROR)
    {

    }
}
void i2cMasterCallback(uint8_t event, unsigned long param)
{
    set_flag_tx(event, param);
}
void set_flag_rx(uint8_t event, unsigned long param)
{
    if (event == I2C_EVENT_TRANSFER_DONE)
    {
        s_slvTransCmplt = 1;
    }

    if (event == I2C_EVENT_TX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_UNDERFLOW)
    {

    }

    if (event == I2C_EVENT_ARB_LOST)
    {

    }

    if (event == I2C_EVENT_TERMINATED)
    {

    }

    if (event == I2C_EVENT_DMAERROR)
    {

    }

    if (event == I2C_EVNET_TRANSFER_ERROR)
    {

    }
}
void i2cSlvCallback(uint8_t event, unsigned long param)
{
    set_flag_rx(event, param);
}
static struct i2c_gm_data i2c_gm_data_2 =
{
    .cb                 = i2cMasterCallback,
};
static struct i2c_gm_data i2c_gm_data_1 =
{
    .cb                 = i2cSlvCallback,
};
static struct i2c_gm_data i2c_gm_data_0 =
{
    .cb                 = i2cSlvCallback,
};
#ifdef CONFIG_I2C_INTERRUPT_DRIVEN
    static void i2c_gm_isr(void *arg);
#endif
static void i2c_gm_isr(void *arg)
{
    struct device *i2c_dev = (struct device *)arg;
    struct i2c_gm_data *dev_data = DEV_DATA(i2c_dev);
    I2C_Handle_T *i2c_handle = &(dev_data->i2c_handle);
    I2C_CommonIRQHandler(i2c_handle);
}
static int i2c_gm_init(struct device *dev)
{
    return 0;
}
static void gm_i2c_hal_tx_callback(I2C_Handle_T *hI2C, uint8_t event, unsigned long param)
{
    i2c_gm_data_2.cb(event, param);
}
static void gm_i2c_hal_rx_callback(I2C_Handle_T *hI2C, uint8_t event, unsigned long param)
{
    i2c_gm_data_0.cb(event, param);
}
static int i2c_gm_configure(struct device *dev, uint32_t dev_config_raw)
{
    I2C_Device_T *pI2c = DEV_STRUCT(dev);
    I2C_Config_T i2cConfig;
    struct i2c_gm_data *priv =  DEV_DATA(dev);

    union dev_config dev_config = (union dev_config)dev_config_raw;

    if ((unsigned int)pI2c == I2C0)
    {
        priv->i2c_handle           = I2C0_hdl;
        priv->i2c_handle.i2c_id   = I2C_ID_0;
    }
    else if ((unsigned int)pI2c == I2C1)
    {
        priv->i2c_handle           = I2C1_hdl;
        priv->i2c_handle.i2c_id   = I2C_ID_1;
    }
    else
    {
        priv->i2c_handle           = I2C2_hdl;
        priv->i2c_handle.i2c_id   = I2C_ID_2;
    }

    priv->i2cConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE;

    if (!dev_config.bits.is_master_device)
    {
        priv->i2cConfig.workMode     = I2C_SLAVE_MODE;
        priv->i2cConfig.localAddress = 0x56;
    }
    else
    {
        priv->i2cConfig.workMode     = I2C_MASTER_MODE;
    }
    if (dev_config.bits.int_mode)
    {
        if (!dev_config.bits.is_master_device)
        {
            priv->i2cConfig.callback     = gm_i2c_hal_rx_callback;
        }
        else
        {
            priv->i2cConfig.callback     = gm_i2c_hal_tx_callback;
        }
        if ((unsigned int)pI2c == I2C0)
        {
            //i2c_gm_config_func_0(dev);    //connect irq
        }

        if ((unsigned int)pI2c == I2C2)
        {
            //i2c_gm_config_func_2(dev);    //connect irq
        }
    }
    else
    {
        priv->i2cConfig.callback     = NULL;
    }
    if (dev_config.bits.dma_en)
    {
        priv->i2cConfig.enableDMA     = ENABLE;
    }
    else
    {
        priv->i2cConfig.enableDMA     = DISABLE;
    }
    if (priv->i2cConfig.enableDMA  == ENABLE)
    {
        priv->i2cConfig.pdmaBuffer      = s_dmabuffer;
        priv->i2cConfig.dmaBufferLength = s_dmaBufferLen;
    }
    if (dev_config.bits.use_10_bit_addr)
    {
        priv->i2cConfig.addrMode     = I2C_10BIT_MODE;
    }
    else if (!dev_config.bits.use_10_bit_addr)
    {
        priv->i2cConfig.addrMode     = I2C_7BIT_MODE;
    }

    switch (dev_config.bits.speed)
    {
        case I2C_SPEED_STANDARD:
            priv->i2cConfig.baudrate     = 100;
            priv->i2cConfig.speedMode    = I2C_STANDARD_MODE;
            break;
        case I2C_SPEED_FAST:
            priv->i2cConfig.baudrate     = 400;
            priv->i2cConfig.speedMode    = I2C_FAST_MODE;
            break;
        default:
            priv->i2cConfig.baudrate     = 100;
            priv->i2cConfig.speedMode    = I2C_STANDARD_MODE;
    }

    if (I2C_Init(pI2c, &(priv->i2cConfig), &(priv->i2c_handle)) != RET_OK)
    {
        return RET_ERROR;
    }
    return 0;
}

static uint32_t i2c_gm_convert_flags(int msg_flags)
{
    uint32_t flags = 0;

    if (!(msg_flags & I2C_MSG_STOP))
    {
        //flags |= kI2C_TransferNoStopFlag;
    }

    if (msg_flags & I2C_MSG_RESTART)
    {
        //flags |= kI2C_TransferRepeatedStartFlag;
    }

    return flags;
}

static int i2c_gm_transfer(struct device *dev, struct i2c_msg *msgs,
                           uint8_t num_msgs, uint16_t addr)
{
    struct i2c_gm_data *data = DEV_DATA(dev);
    int32_t status;
    struct i2c_gm_data *priv =  DEV_DATA(dev);
    int i = 0;
    /* Iterate over all the messages */
    for (i = 0; i < num_msgs; i++)
    {

        if ((priv->i2cConfig.workMode     == I2C_MASTER_MODE) && ((msgs->flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE))
        {
            status = I2C_MasterTransmit(&priv->i2c_handle, addr, msgs->buf, msgs->len, msgs->timeout);//TIMEOUT_WAIT_FOREVER

        }
        else if ((priv->i2cConfig.workMode     == I2C_MASTER_MODE) && ((msgs->flags & I2C_MSG_RW_MASK) == I2C_MSG_READ))
        {
            status = I2C_MasterReceive(&priv->i2c_handle, addr, msgs->buf, msgs->len, msgs->timeout);

        }
        else if ((priv->i2cConfig.workMode     == I2C_SLAVE_MODE) && ((msgs->flags & I2C_MSG_RW_MASK) == I2C_MSG_WRITE))
        {
            status = I2C_SlaveTransmit(&priv->i2c_handle, msgs->buf, msgs->len, msgs->timeout);//TIMEOUT_NO_BLOCKING

        }
        else if ((priv->i2cConfig.workMode     == I2C_SLAVE_MODE) && ((msgs->flags & I2C_MSG_RW_MASK) == I2C_MSG_READ))
        {
            status = I2C_SlaveReceive(&priv->i2c_handle, msgs->buf, msgs->len, msgs->timeout);
        }
        if (status != RET_OK)
        {
            return -EIO;
        }

        /* Wait for the transfer to complete */
        /* Move to the next message */
        msgs++;
    }

    return status;
}
static const struct i2c_driver_api i2c_gm_driver_api =
{
    .configure = i2c_gm_configure,
    .transfer = i2c_gm_transfer,
};
#ifdef CONFIG_I2C0

static const struct i2c_gm_config i2c_gm_config_0 =
{
    .base = CONFIG_I2C0_BASE,
    //.sys_clk_freq = I2C_CLOCK,
    .irq_config_func = i2c_gm_config_func_0,
    //.default_cfg.raw = CONFIG_I2C_0_DEFAULT_CFG,
};

DEVICE_AND_API_INIT(I2C0_DEV_NAME, I2C0_DRV_NAME, &i2c_gm_init,
                    &i2c_gm_data_0, &i2c_gm_config_0,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &i2c_gm_driver_api);

static void i2c_gm_config_func_0(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(I2C0_IRQn, 3,
                i2c_gm_isr, DEVICE_GET(I2C0_DEV_NAME), 0);

    irq_enable(I2C0_IRQn);
}
#endif /* CONFIG_I2C_0 */

#ifdef CONFIG_I2C1

static const struct i2c_gm_config i2c_gm_config_1 =
{
    .base = CONFIG_I2C1_BASE,
    //.sys_clk_freq = I2C_CLOCK,
    .irq_config_func = i2c_gm_config_func_1,
    //.default_cfg.raw = CONFIG_I2C_1_DEFAULT_CFG,
};

DEVICE_AND_API_INIT(I2C1_DEV_NAME, I2C1_DRV_NAME, &i2c_gm_init,
                    &i2c_gm_data_1, &i2c_gm_config_1,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &i2c_gm_driver_api);

static void i2c_gm_config_func_1(struct device *dev)
{
    IRQ_CONNECT(I2C1_IRQn, 3,
                i2c_gm_isr, DEVICE_GET(I2C1_DEV_NAME), 0);

    irq_enable(I2C1_IRQn);
}
#endif

#ifdef CONFIG_I2C2

static const struct i2c_gm_config i2c_gm_config_2 =
{
    .base = CONFIG_I2C2_BASE,
    //.sys_clk_freq = I2C_CLOCK,
    .irq_config_func = i2c_gm_config_func_2,
    //.default_cfg.raw = CONFIG_I2C_2_DEFAULT_CFG,
};

DEVICE_AND_API_INIT(I2C2_DEV_NAME, I2C2_DRV_NAME, &i2c_gm_init,
                    &i2c_gm_data_2, &i2c_gm_config_2,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &i2c_gm_driver_api);

static void i2c_gm_config_func_2(struct device *dev)
{
    IRQ_CONNECT(I2C2_IRQn, 3,
                i2c_gm_isr, DEVICE_GET(I2C2_DEV_NAME), 0);

    irq_enable(I2C2_IRQn);
}
#endif
