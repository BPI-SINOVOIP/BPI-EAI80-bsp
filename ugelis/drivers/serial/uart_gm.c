/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_gm.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief
 *
 * @note
 *    2018/03/24, Mike.Zheng, V1.0.0
 *        Initial version.
 */

/*uGelis include files*/
#include <kernel.h>
#include <arch/cpu.h>
#include <misc/__assert.h>
#include <board.h>
#include <soc.h>
#include <init.h>
#include <uart.h>
#include <linker/sections.h>
#include "uart_gm.h"

/*gm hal include files*/

#include <gm_hal_uart.h>

static int uart_gm_init(struct device *dev);
static int uart_gm_poll_in(struct device *dev, unsigned char *c);
static unsigned char uart_gm_poll_out(struct device *dev, unsigned char c);
static int uart_gm_err_check(struct device *dev);

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    static void uart_gm_isr(void *arg);
    static void uart_gm_irq_config(struct device *dev);
    static int uart_gm_fifo_fill(struct device *dev, const uint8_t *tx_data, int size);
    static int uart_gm_fifo_read(struct device *dev, uint8_t *rx_data, const int size);
    static void uart_gm_irq_tx_enable(struct device *dev);
    static void uart_gm_irq_tx_disable(struct device *dev);
    static int uart_gm_irq_tx_ready(struct device *dev);
    static void uart_gm_irq_rx_enable(struct device *dev);
    static void uart_gm_irq_rx_disable(struct device *dev);
    static int uart_gm_irq_tx_complete(struct device *dev);
    static int uart_gm_irq_rx_ready(struct device *dev);
    static void uart_gm_irq_err_enable(struct device *dev);
    static void uart_gm_irq_err_disable(struct device *dev);
    static int uart_gm_irq_is_pending(struct device *dev);
    static int uart_gm_irq_update(struct device *dev);
    static void uart_gm_irq_callback_set(struct device *dev, uart_irq_callback_t cb);
#endif


#if defined(CONFIG_UART0)
static struct uart_gm_dev_data_t uart_gm_dev_data_0;
static const struct uart_device_config uart_gm_dev_cfg_0 =
{
    .config_u.base = (void *)(CONFIG_UART0_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART1)
static struct uart_gm_dev_data_t uart_gm_dev_data_1;
static const struct uart_device_config uart_gm_dev_cfg_1 =
{
    .config_u.base = (void *)(CONFIG_UART1_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART2)
static struct uart_gm_dev_data_t uart_gm_dev_data_2;
static const struct uart_device_config uart_gm_dev_cfg_2 =
{
    .config_u.base = (void *)(CONFIG_UART2_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART3)
static struct uart_gm_dev_data_t uart_gm_dev_data_3;
static const struct uart_device_config uart_gm_dev_cfg_3 =
{
    .config_u.base = (void *)(CONFIG_UART3_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART4)
static struct uart_gm_dev_data_t uart_gm_dev_data_4;
static const struct uart_device_config uart_gm_dev_cfg_4 =
{
    .config_u.base = (void *)(CONFIG_UART4_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART5)
static struct uart_gm_dev_data_t uart_gm_dev_data_5;
static const struct uart_device_config uart_gm_dev_cfg_5 =
{
    .config_u.base = (void *)(CONFIG_UART5_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART6)
static struct uart_gm_dev_data_t uart_gm_dev_data_6;
static const struct uart_device_config uart_gm_dev_cfg_6 =
{
    .config_u.base = (void *)(CONFIG_UART6_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

#if defined(CONFIG_UART7)
static struct uart_gm_dev_data_t uart_gm_dev_data_7;
static const struct uart_device_config uart_gm_dev_cfg_7 =
{
    .config_u.base = (void *)(CONFIG_UART7_BASE),
    .sys_clk_freq = CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .irq_config_func = uart_gm_irq_config,
#endif
};
#endif

static const struct uart_driver_api uart_gm_driver_api =
{
    .poll_in          = uart_gm_poll_in,
    .poll_out         = uart_gm_poll_out,
    .err_check        = uart_gm_err_check,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    .fifo_fill        = uart_gm_fifo_fill,
    .fifo_read        = uart_gm_fifo_read,
    .irq_tx_enable    = uart_gm_irq_tx_enable,
    .irq_tx_disable   = uart_gm_irq_tx_disable,
    .irq_tx_ready     = uart_gm_irq_tx_ready,
    .irq_rx_enable    = uart_gm_irq_rx_enable,
    .irq_rx_disable   = uart_gm_irq_rx_disable,
    .irq_tx_complete  = uart_gm_irq_tx_complete,
    .irq_rx_ready     = uart_gm_irq_rx_ready,
    .irq_err_enable   = uart_gm_irq_err_enable,
    .irq_err_disable  = uart_gm_irq_err_disable,
    .irq_is_pending   = uart_gm_irq_is_pending,
    .irq_update       = uart_gm_irq_update,
    .irq_callback_set = uart_gm_irq_callback_set,
#endif /* CONFIG_UART_INTERRUPT_DRIVEN */
};



#ifdef CONFIG_DEVICE_POWER_MANAGEMENT

static int gm_uart_pm_control(struct device *device, uint32_t command, void *context)
{
    int ret = 0;
    uint32_t set_stat ;
    static uint32_t  pm_dev_status = DEVICE_PM_ACTIVE_STATE;

    if (command == DEVICE_PM_SET_POWER_STATE)
    {
        /*todo:get set dev pm status*/
        set_stat = *((int *)context);
        /*bakup dev pm status*/
        pm_dev_status = set_stat;

        if (set_stat == DEVICE_PM_ACTIVE_STATE)
        {
            /*todo:do resume action*/
        }
        else if (set_stat == DEVICE_PM_LOW_POWER_STATE)
        {
            /*todo:only stop clk*/
        }
        else if (set_stat == DEVICE_PM_SUSPEND_STATE)
        {
            /*todo:pm reg lose,should bakup to resume
             *when resume should int pm modules to
             *work mode that enter standby place
            */
        }
        else if (set_stat == DEVICE_PM_OFF_STATE)
        {
            /*todo:close pm module power,and lose all the thing*/
        }
        else
        {
            return -EINVAL;
        }
    }
    else if (command == DEVICE_PM_GET_POWER_STATE)
    {
        *((uint32_t *)context) = pm_dev_status;
        return pm_dev_status;
    }
    return ret;
}
#endif


#if defined(CONFIG_UART0)
DEVICE_DEFINE(UART0_DEV_NAME, UART0_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_0, &uart_gm_dev_cfg_0, PRE_KERNEL_1,           \
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART1)
DEVICE_DEFINE(UART1_DEV_NAME, UART1_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_1, &uart_gm_dev_cfg_1, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART2)
DEVICE_DEFINE(UART2_DEV_NAME, UART2_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_2, &uart_gm_dev_cfg_2, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART3)
DEVICE_DEFINE(UART3_DEV_NAME, UART3_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_3, &uart_gm_dev_cfg_3, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART4)
DEVICE_DEFINE(UART4_DEV_NAME, UART4_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_4, &uart_gm_dev_cfg_4, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART5)
DEVICE_DEFINE(UART5_DEV_NAME, UART5_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_5, &uart_gm_dev_cfg_5, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART6)
DEVICE_DEFINE(UART6_DEV_NAME, UART6_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_6, &uart_gm_dev_cfg_6, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif

#if defined(CONFIG_UART7)
DEVICE_DEFINE(UART7_DEV_NAME, UART7_DRV_NAME, uart_gm_init, gm_uart_pm_control, \
              &uart_gm_dev_data_7, &uart_gm_dev_cfg_7, PRE_KERNEL_1,
              DEVICE_PRIORITY_UART, (void *)&uart_gm_driver_api);
#endif


static int uart_gm_init(struct device *dev)
{
    int ret = 0;
    UART_Config_T config;
    UART_Device_T *puart = DEV_STRUCT(dev);
    struct uart_gm_dev_data_t *dev_data = DEV_DATA(dev);

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    const struct uart_device_config *dev_cfg = DEV_CFG(dev);
#endif
    UART_Handle_T *uart_handle = &(dev_data->huart);

#if defined(CONFIG_UART0)
    if ((uint32_t)puart == CONFIG_UART0_BASE)
    {
        dev_data->uart_id      = UART_INDEX0;
        config.baudRate        = CONFIG_UART0_BAUDRATE;
        config.parity          = CONFIG_UART0_PARITY;
        config.wordLength      = CONFIG_UART0_DATABITS;
        config.stopBits        = CONFIG_UART0_STOPBITS;
        config.clockSrc        = CONFIG_UART0_CLOCKSRC;
    }
	
	/* modified UART0 clock source to be CLOCK_UART_SEL_PLLI2S, which generally is 48M
	Peripheral_Config_T periClkConfig;
    periClkConfig.peripheMask = PERIPHERAL_UART2_MASK;
    periClkConfig.peripheClkConfig.uartClkConfig.uartPrescaler = CLOCK_UART_DIV_1;
    periClkConfig.peripheClkConfig.uartClkConfig.uartSourceClk = CLOCK_UART_SEL_PLLI2S;
    HAL_CLOCK_PeripheralClkSetConfig(&periClkConfig);
	*/
#endif

#if defined(CONFIG_UART1)
    if ((uint32_t)puart == CONFIG_UART1_BASE)
    {
        dev_data->uart_id      = UART_INDEX1;
        config.baudRate        = CONFIG_UART1_BAUDRATE;
        config.parity          = CONFIG_UART1_PARITY;
        config.wordLength      = CONFIG_UART1_DATABITS;
        config.stopBits        = CONFIG_UART1_STOPBITS;
        config.clockSrc        = CONFIG_UART1_CLOCKSRC;
    }
#endif

#if defined(CONFIG_UART2)
    if ((uint32_t)puart == CONFIG_UART2_BASE)
    {
        dev_data->uart_id      = UART_INDEX2;
        config.baudRate        = CONFIG_UART2_BAUDRATE;
        config.parity          = CONFIG_UART2_PARITY;
        config.wordLength      = CONFIG_UART2_DATABITS;
        config.stopBits        = CONFIG_UART2_STOPBITS;
        config.clockSrc        = CONFIG_UART2_CLOCKSRC;
    }
#endif

#if defined(CONFIG_UART3)
    if ((uint32_t)puart == CONFIG_UART3_BASE)
    {
        dev_data->uart_id      = UART_INDEX3;
        config.baudRate        = CONFIG_UART3_BAUDRATE;
        config.parity          = CONFIG_UART3_PARITY;
        config.wordLength      = CONFIG_UART3_DATABITS;
        config.stopBits        = CONFIG_UART3_STOPBITS;
        config.clockSrc        = CONFIG_UART3_CLOCKSRC;
    }
#endif

#if defined(CONFIG_UART4)
    if ((uint32_t)puart == CONFIG_UART4_BASE)
    {
        dev_data->uart_id      = UART_INDEX4;
        config.baudRate        = CONFIG_UART4_BAUDRATE;
        config.parity          = CONFIG_UART4_PARITY;
        config.wordLength      = CONFIG_UART4_DATABITS;
        config.stopBits        = CONFIG_UART4_STOPBITS;
        config.clockSrc        = CONFIG_UART4_CLOCKSRC;
    }
#endif

#if defined(CONFIG_UART5)
    if ((uint32_t)puart == CONFIG_UART5_BASE)
    {
        dev_data->uart_id      = UART_INDEX5;
        config.baudRate        = CONFIG_UART5_BAUDRATE;
        config.parity          = CONFIG_UART5_PARITY;
        config.wordLength      = CONFIG_UART5_DATABITS;
        config.stopBits        = CONFIG_UART5_STOPBITS;
        config.clockSrc        = CONFIG_UART5_CLOCKSRC;
    }
#endif

#if defined(CONFIG_UART6)
    if ((uint32_t)puart == CONFIG_UART6_BASE)
    {
        dev_data->uart_id      = UART_INDEX6;
        config.baudRate        = CONFIG_UART6_BAUDRATE;
        config.parity          = CONFIG_UART6_PARITY;
        config.wordLength      = CONFIG_UART6_DATABITS;
        config.stopBits        = CONFIG_UART6_STOPBITS;
        config.clockSrc        = CONFIG_UART6_CLOCKSRC;
    }
#endif

#if defined(CONFIG_UART7)
    if ((uint32_t)puart == CONFIG_UART7_BASE)
    {
        dev_data->uart_id      = UART_INDEX7;
        config.baudRate        = CONFIG_UART7_BAUDRATE;
        config.parity          = CONFIG_UART7_PARITY;
        config.wordLength      = CONFIG_UART7_DATABITS;
        config.stopBits        = CONFIG_UART7_STOPBITS;
        config.clockSrc        = CONFIG_UART7_CLOCKSRC;
    }
#endif

    /* call gm hal lib */
    ret = HAL_UART_Init(uart_handle, puart, &config);
    if (ret)
    {
        return ret;
    }

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    /* Clear any pending UART interrupts: we only care about RX, TX: */
    dev_cfg->irq_config_func(dev);
#endif

    return 0;
}

static int uart_gm_poll_in(struct device *dev, unsigned char *c)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    while (!HAL_UART_DataReady(puart));
    *c = HAL_UART_GetChar(puart);

    return 0;
}

static unsigned char uart_gm_poll_out(struct device *dev, unsigned char c)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_PutChar(puart, c);

    return c;
}

static int uart_gm_err_check(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);
    return (HAL_UART_Error_Enabled(puart) && HAL_UART_Error_Ready(puart));
}


#ifdef CONFIG_UART_INTERRUPT_DRIVEN

static void uart_gm_isr(void *arg)
{
    struct device *uart_dev  = (struct device *)arg;
    struct uart_gm_dev_data_t *dev_data = DEV_DATA(uart_dev);
    UART_Device_T *puart = DEV_STRUCT(uart_dev);

    //we abandon this func, becoz it is too complicated
    //HAL_UART_IRQHandler(dev_data->uart_id);

    //clear tx interrupt status
    if (HAL_UART_TxInt_Ready(puart))
    {
        HAL_UART_Clear_TxInt(puart);
    }

    if (HAL_UART_Error_Ready(puart))
    {
        HAL_UART_Clear_ErrInt(puart);
    }

    if (dev_data->cb)
    {
        dev_data->cb(uart_dev);
    }
}

static void uart_gm_irq_config(struct device *dev)
{
    struct uart_gm_dev_data_t *dev_data = DEV_DATA(dev);
    uint8_t uart_id = dev_data->uart_id;

    if (uart_id == UART_INDEX0)
    {
#if defined(CONFIG_UART0)
        IRQ_CONNECT(CONFIG_UART0_IRQ_NUM,
                    CONFIG_UART0_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART0_DEV_NAME), 0);
        irq_enable(CONFIG_UART0_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX1)
    {
#if defined(CONFIG_UART1)
        IRQ_CONNECT(CONFIG_UART1_IRQ_NUM,
                    CONFIG_UART1_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART1_DEV_NAME), 0);
        irq_enable(CONFIG_UART1_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX2)
    {
#if defined(CONFIG_UART2)
        IRQ_CONNECT(CONFIG_UART2_IRQ_NUM,
                    CONFIG_UART2_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART2_DEV_NAME), 0);
        irq_enable(CONFIG_UART2_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX3)
    {
#if defined(CONFIG_UART3)
        IRQ_CONNECT(CONFIG_UART3_IRQ_NUM,
                    CONFIG_UART3_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART3_DEV_NAME), 0);
        irq_enable(CONFIG_UART3_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX4)
    {
#if defined(CONFIG_UART4)
        IRQ_CONNECT(CONFIG_UART4_IRQ_NUM,
                    CONFIG_UART4_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART4_DEV_NAME), 0);
        irq_enable(CONFIG_UART4_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX5)
    {
#if defined(CONFIG_UART5)
        IRQ_CONNECT(CONFIG_UART5_IRQ_NUM,
                    CONFIG_UART5_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART5_DEV_NAME), 0);
        irq_enable(CONFIG_UART5_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX6)
    {
#if defined(CONFIG_UART6)
        IRQ_CONNECT(CONFIG_UART6_IRQ_NUM,
                    CONFIG_UART6_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART6_DEV_NAME), 0);
        irq_enable(CONFIG_UART6_IRQ_NUM);
#endif
    }
    else if (uart_id == UART_INDEX7)
    {
#if defined(CONFIG_UART7)
        IRQ_CONNECT(CONFIG_UART7_IRQ_NUM,
                    CONFIG_UART7_IRQ_PRIORITY,
                    uart_gm_isr, DEVICE_GET(UART7_DEV_NAME), 0);
        irq_enable(CONFIG_UART7_IRQ_NUM);
#endif
    }
}

static int uart_gm_fifo_fill(struct device *dev, const uint8_t *tx_data, int size)
{
    int num_tx = 0;
    UART_Device_T *puart = DEV_STRUCT(dev);
    while (num_tx < size)
    {
        HAL_UART_PutChar(puart, tx_data[num_tx++]);
    }
    return num_tx;
}

static int uart_gm_fifo_read(struct device *dev, uint8_t *rx_data, const int size)
{
    int num_rx = 0;
    UART_Device_T *puart = DEV_STRUCT(dev);
    while (num_rx < size)
    {
        rx_data[num_rx++] = (uint8_t)(HAL_UART_GetChar(puart));
    }
    return num_rx;
}

static void uart_gm_irq_tx_enable(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_EnableInterrupts(puart, UART_TX_INT | UART_TX_FIFO_INT);
}

static void uart_gm_irq_tx_disable(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_DisableInterrupts(puart, UART_TX_INT | UART_TX_FIFO_INT);
}

static int uart_gm_irq_tx_ready(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    return HAL_UART_TxInt_Ready(puart);
}

static void uart_gm_irq_rx_enable(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_EnableInterrupts(puart, UART_RX_INT | UART_RX_FIFO_INT);
}

static void uart_gm_irq_rx_disable(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_DisableInterrupts(puart, UART_RX_INT | UART_RX_FIFO_INT);
}

static int uart_gm_irq_tx_complete(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    return HAL_UART_Tx_Complete(puart);
}

static int uart_gm_irq_rx_ready(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    return HAL_UART_RxInt_Ready(puart);
}

static void uart_gm_irq_err_enable(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_Enable_Error_Interrupts(puart);
}

static void uart_gm_irq_err_disable(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    HAL_UART_Disable_Error_Interrupts(puart);
}

static int uart_gm_irq_is_pending(struct device *dev)
{
    UART_Device_T *puart = DEV_STRUCT(dev);

    return ((HAL_UART_RxInt_Enabled(puart) && HAL_UART_RxInt_Ready(puart)) ||
            (HAL_UART_TxInt_Enabled(puart) && HAL_UART_TxInt_Ready(puart)));
}

static int uart_gm_irq_update(struct device *dev)
{
    return 1;
}

static void uart_gm_irq_callback_set(struct device *dev, uart_irq_callback_t cb)
{
    struct uart_gm_dev_data_t *dev_data = DEV_DATA(dev);
    dev_data->cb = cb;
}

#endif /* CONFIG_UART_INTERRUPT_DRIVEN */


