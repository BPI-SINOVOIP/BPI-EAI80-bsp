/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gpio_gm.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       GPIO source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <errno.h>
#include <device.h>
#include <gpio.h>
#include <init.h>
#include <kernel.h>
#include <sys_io.h>
#include <gm_common.h>
#include <gm_hal_gpio.h>
#include <misc/printk.h>
#include "gpio_gm.h"
#include "gpio_utils.h"

static void  gpio_clk_rst_init(void)
{
    HAL_GPIO_CLK_ENABLE();
    HAL_GPIO_SW_Reset();
}
static inline int gpio_gm_config(struct device *port,
                                 int access_op, uint32_t pin, int flags)
{
    const struct gpio_gm_config *gpio_config = DEV_CFG(port);
    uint32_t real_pin = 0;

    if (access_op == GPIO_ACCESS_BY_PIN)
    {

        GPIO_Device_T *pGPIO = (GPIO_Device_T *)gpio_config->port_base;
        GPIO_PinConfig_T gpio_cfg;

        real_pin = (1 << (pin & 0xFFFF));
        gpio_cfg.pin = real_pin;

        /*gpio_cfg.mode = flags & (~GPIO_INT) & (~(3 << GPIO_PUD_POS));*/
        gpio_cfg.mode = flags & (~(3 << GPIO_PUD_POS));//(~(3 << 0));//(~(3 << GPIO_PUD_POS));
        gpio_cfg.pull = (flags >> GPIO_PUD_POS) & 0x3;
        gpio_cfg.alternate = (pin >> 24) & 0xFF;

        PR_DEBUG("%s pin=0x%x, flags =0x%x, pull =0x%x, alt=0x%x \n", __FUNCTION__, gpio_cfg.pin, flags, gpio_cfg.pull, gpio_cfg.alternate);
        PR_DEBUG("%s pin=0x%x, mode =0x%x, pull =0x%x, alt=0x%x \n", __FUNCTION__,  gpio_cfg.pin, gpio_cfg.mode, gpio_cfg.pull, gpio_cfg.alternate);
        HAL_GPIO_Init((GPIO_Device_T *)pGPIO, &gpio_cfg);
        PR_DEBUG("%s port_base =0x%x\n", __FUNCTION__, (unsigned int)gpio_config->port_base);

    }
    else
    {
        return -ENOTSUP;
    }

    return 0;
}

static inline int gpio_gm_write(struct device *port,
                                int access_op, uint32_t pin, uint32_t value)
{
    uint32_t instance = 0;
    uint32_t position = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
    uint32_t pin_num = 0;
    uint32_t real_pin = 0;

    const struct gpio_gm_config *gpio_config = DEV_CFG(port);
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)gpio_config->port_base;
    real_pin = (1 << (pin & 0xFFFF));

    if (access_op != GPIO_ACCESS_BY_PIN)
    {
        return -ENOTSUP;
    }

    if (pGPIO == GPIOA)
    {
        instance = 0;
    }
    else if (pGPIO == GPIOB)
    {
        instance = 1;
    }
    else if (pGPIO == GPIOC)
    {
        instance = 2;
    }
    else if (pGPIO == GPIOD)
    {
        instance = 3;
    }
    else if (pGPIO == GPIOE)
    {
        instance = 4;
    }
    else if (pGPIO == GPIOF)
    {
        instance = 5;
    }
    else if (pGPIO == GPIOG)
    {
        instance = 6;
    }
    else if (pGPIO == GPIOH)
    {
        instance = 7;
    }
    else if (pGPIO == GPIOI)
    {
        instance = 8;
    }
    else if (pGPIO == GPIOJ)
    {
        instance = 9;
    }
    else if (pGPIO == GPIOK)
    {
        instance = 10;
    }

    for (position = 0; position < 16; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(real_pin) & ioPosition;
        if (ioCurrent == ioPosition)
        {
            pin_num = position + instance * 16;

            if (value == 0)
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_RESET);
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_SET);
            }
        }
    }

    return 0;
}


static inline int gpio_gm_read(struct device *port,
                               int access_op, uint32_t pin, uint32_t *value)
{
    const struct gpio_gm_config *gpio_config = DEV_CFG(port);
    uint32_t instance = 0;
    uint32_t position = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
    uint32_t pin_num = 0;
    uint32_t real_pin = 0;

    GPIO_Device_T *pGPIO = (GPIO_Device_T *)gpio_config->port_base;
    real_pin = (1 << (pin & 0xFFFF));

    if (access_op != GPIO_ACCESS_BY_PIN)
    {
        return -ENOTSUP;
    }

    if (pGPIO == GPIOA)
    {
        instance = 0;
    }
    else if (pGPIO == GPIOB)
    {
        instance = 1;
    }
    else if (pGPIO == GPIOC)
    {
        instance = 2;
    }
    else if (pGPIO == GPIOD)
    {
        instance = 3;
    }
    else if (pGPIO == GPIOE)
    {
        instance = 4;
    }
    else if (pGPIO == GPIOF)
    {
        instance = 5;
    }
    else if (pGPIO == GPIOG)
    {
        instance = 6;
    }
    else if (pGPIO == GPIOH)
    {
        instance = 7;
    }
    else if (pGPIO == GPIOI)
    {
        instance = 8;
    }
    else if (pGPIO == GPIOJ)
    {
        instance = 9;
    }
    else if (pGPIO == GPIOK)
    {
        instance = 10;
    }

    for (position = 0; position < 16; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(real_pin) & ioPosition;
        if (ioCurrent == ioPosition)
        {
            pin_num = position + instance * 16;
            *value = HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num));
        }
    }

    return 0;
}

static int gpio_irq_enable(struct device *dev)
{
    struct gpio_gm_data *data = DEV_DATA(dev);
    const struct gpio_gm_config *gpio_config = DEV_CFG(dev);
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)gpio_config->port_base;

    /* Enable Interrupt */
    HAL_GPIO_IRQ_ENABLE(pGPIO, data->pin_callback_enables);
    PR_DEBUG("gpio_irq_enable 0x%x\n", data->pin_callback_enables);

    return 0;

}

static int gpio_irq_disable(struct device *dev)
{
    struct gpio_gm_data *data = DEV_DATA(dev);
    const struct gpio_gm_config *gpio_config = DEV_CFG(dev);
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)gpio_config->port_base;

    /* Disable Interrupt */
    HAL_GPIO_IRQ_DISABLE(pGPIO, data->pin_callback_enables);
    PR_DEBUG("%s disable:[0x%x]\n", __FUNCTION__, data->pin_callback_enables);

    return 0;

}

static int gpio_gm_manage_callback(struct device *dev,
                                   struct gpio_callback *callback, bool set)
{
    struct gpio_gm_data *data = DEV_DATA(dev);

    _gpio_manage_callback(&data->callbacks, callback, set);

    return 0;
}

static int gpio_gm_enable_callback(struct device *dev,
                                   int access_op, uint32_t pin)
{
    struct gpio_gm_data *data = DEV_DATA(dev);

    if (access_op == GPIO_ACCESS_BY_PIN)
    {
        data->pin_callback_enables |= (1 << (pin & 0xFFFF));
        gpio_irq_enable(dev);
    }
    else
    {
        data->pin_callback_enables = 0xFFFFFFFF;
    }

    return 0;
}

static int gpio_gm_disable_callback(struct device *dev,
                                    int access_op, uint32_t pin)
{
    struct gpio_gm_data *data = DEV_DATA(dev);

    if (access_op == GPIO_ACCESS_BY_PIN)
    {
        data->pin_callback_enables &= ~(1 << (pin & 0xFFFF));
        gpio_irq_disable(dev);
    }
    else
    {
        data->pin_callback_enables = 0;
    }

    return 0;
}

#ifndef CONFIG_USE_HAL_TEST
static void gpio_gm_port_isr(void *arg)
{
    struct device *dev = arg;
    const struct gpio_gm_config *config = DEV_CFG(dev);
    struct gpio_gm_data *data = DEV_DATA(dev);
    uint32_t enabled_int;

    PR_DEBUG("enter gpio_gm_port_isr\n");
    GPIO_Device_T *base = (GPIO_Device_T *)config->port_base;

    enabled_int = data->pin_callback_enables & (base->ISFR);

    if (HAL_GPIO_EXTI_GET_IT(base, enabled_int))
    {
        HAL_GPIO_EXTI_CLEAR_IT(base, enabled_int);
    }

    /* Call the registered callbacks */
    _gpio_fire_callbacks(&data->callbacks, (struct device *)dev,
                         enabled_int);

}
#endif

static const struct gpio_driver_api api_funcs =
{
    .config = gpio_gm_config,
    .write = gpio_gm_write,
    .read = gpio_gm_read,
    .manage_callback = gpio_gm_manage_callback,
    .enable_callback = gpio_gm_enable_callback,
    .disable_callback = gpio_gm_disable_callback,

};

#ifdef CONFIG_GPIO_GM_A
static const struct gpio_gm_config gpio_gm_a_config =
{
    .port_base = GPIOA,
    .irq_num = GPIOA_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_a);
static struct gpio_gm_data gpio_gm_a_data;

static int gpio_gm_a_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOA, CONFIG_GPIO_GM_A_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_a), 0);

    irq_enable(EXCEPTION_GPIOA);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_a, CONFIG_GPIO_GM_A_NAME,
                    &gpio_gm_a_init, &gpio_gm_a_data,
                    &gpio_gm_a_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif

#ifdef CONFIG_GPIO_GM_B
static const struct gpio_gm_config gpio_gm_b_config =
{
    .port_base = GPIOB,
    .irq_num = GPIOB_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_b);
static struct gpio_gm_data gpio_gm_b_data;

static int gpio_gm_b_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOB, CONFIG_GPIO_GM_B_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_b), 0);

    irq_enable(EXCEPTION_GPIOB);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_b, CONFIG_GPIO_GM_B_NAME,
                    &gpio_gm_b_init, &gpio_gm_b_data,
                    &gpio_gm_b_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif /* CONFIG_GPIO_GM_B */

#ifdef  CONFIG_GPIO_GM_C
static const struct gpio_gm_config gpio_gm_c_config =
{
    .port_base = GPIOC,
    .irq_num = GPIOC_IRQn,
};


static struct device DEVICE_NAME_GET(gpio_gm_c);
static struct gpio_gm_data gpio_gm_c_data;
extern void printdebug(char ch);

static int gpio_gm_c_init(struct device *dev)
{
    ARG_UNUSED(dev);
    //printdebug('c');

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOC, CONFIG_GPIO_GM_C_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_c), 0);

    irq_enable(EXCEPTION_GPIOC);
#endif

    return 0;
}


DEVICE_AND_API_INIT(gpio_gm_c, CONFIG_GPIO_GM_C_NAME,
                    &gpio_gm_c_init, &gpio_gm_c_data,
                    &gpio_gm_c_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif

#ifdef CONFIG_GPIO_GM_D
static const struct gpio_gm_config gpio_gm_d_config =
{
    .port_base = GPIOD,
    .irq_num = GPIOD_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_d);
static struct gpio_gm_data gpio_gm_d_data;

static int gpio_gm_d_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOD, CONFIG_GPIO_GM_D_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_d), 0);

    irq_enable(EXCEPTION_GPIOD);
#endif
    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_d, CONFIG_GPIO_GM_D_NAME,
                    &gpio_gm_d_init, &gpio_gm_d_data,
                    &gpio_gm_d_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif


#ifdef CONFIG_GPIO_GM_E
static const struct gpio_gm_config gpio_gm_e_config =
{
    .port_base = GPIOE,
    .irq_num = GPIOE_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_e);
static struct gpio_gm_data gpio_gm_e_data;

static int gpio_gm_e_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();

#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOE, CONFIG_GPIO_GM_E_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_e), 0);

    irq_enable(EXCEPTION_GPIOE);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_e, CONFIG_GPIO_GM_E_NAME,
                    &gpio_gm_e_init, &gpio_gm_e_data,
                    &gpio_gm_e_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif


#ifdef CONFIG_GPIO_GM_F
static const struct gpio_gm_config gpio_gm_f_config =
{
    .port_base = GPIOF,
    .irq_num = GPIOF_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_f);
static struct gpio_gm_data gpio_gm_f_data;

static int gpio_gm_f_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOF, CONFIG_GPIO_GM_F_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_f), 0);

    irq_enable(EXCEPTION_GPIOF);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_f, CONFIG_GPIO_GM_F_NAME,
                    &gpio_gm_f_init, &gpio_gm_f_data,
                    &gpio_gm_f_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif

#ifdef CONFIG_GPIO_GM_G
static const struct gpio_gm_config gpio_gm_g_config =
{
    .port_base = GPIOG,
    .irq_num = GPIOG_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_g);
static struct gpio_gm_data gpio_gm_g_data;

static int gpio_gm_g_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOG, CONFIG_GPIO_GM_G_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_g), 0);

    irq_enable(EXCEPTION_GPIOG);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_g, CONFIG_GPIO_GM_G_NAME,
                    &gpio_gm_g_init, &gpio_gm_g_data,
                    &gpio_gm_g_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif

#ifdef CONFIG_GPIO_GM_H
static const struct gpio_gm_config gpio_gm_h_config =
{
    .port_base = GPIOH,
    .irq_num = GPIOH_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_h);
static struct gpio_gm_data gpio_gm_h_data;

static int gpio_gm_h_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOH, CONFIG_GPIO_GM_H_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_h), 0);

    irq_enable(EXCEPTION_GPIOH);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_h, CONFIG_GPIO_GM_H_NAME,
                    &gpio_gm_h_init, &gpio_gm_h_data,
                    &gpio_gm_h_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif


#ifdef CONFIG_GPIO_GM_I
static const struct gpio_gm_config gpio_gm_i_config =
{
    .port_base = GPIOI,
    .irq_num = GPIOI_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_i);
static struct gpio_gm_data gpio_gm_i_data;

static int gpio_gm_i_init(struct device *dev)
{
    ARG_UNUSED(dev);

#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOI, CONFIG_GPIO_GM_I_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_i), 0);

    irq_enable(EXCEPTION_GPIOI);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_i, CONFIG_GPIO_GM_I_NAME,
                    &gpio_gm_i_init, &gpio_gm_i_data,
                    &gpio_gm_i_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif

#ifdef CONFIG_GPIO_GM_J
static const struct gpio_gm_config gpio_gm_j_config =
{
    .port_base = GPIOJ,
    .irq_num = GPIOF_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_j);
static struct gpio_gm_data gpio_gm_j_data;

static int gpio_gm_j_init(struct device *dev)
{
    ARG_UNUSED(dev);

    gpio_clk_rst_init();
#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOJ, CONFIG_GPIO_GM_F_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_j), 0);

    irq_enable(EXCEPTION_GPIOJ);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_j, CONFIG_GPIO_GM_J_NAME,
                    &gpio_gm_j_init, &gpio_gm_j_data,
                    &gpio_gm_j_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif


#ifdef CONFIG_GPIO_GM_K
static const struct gpio_gm_config gpio_gm_k_config =
{
    .port_base = GPIOK,
    .irq_num = GPIOK_IRQn,
};

static struct device DEVICE_NAME_GET(gpio_gm_k);
static struct gpio_gm_data gpio_gm_k_data;

static int gpio_gm_k_init(struct device *dev)
{
    ARG_UNUSED(dev);
    gpio_clk_rst_init();

#ifndef CONFIG_USE_HAL_TEST
    IRQ_CONNECT(EXCEPTION_GPIOK, CONFIG_GPIO_GM_K_IRQ_PRI,
                gpio_gm_port_isr, DEVICE_GET(gpio_gm_k), 0);

    irq_enable(EXCEPTION_GPIOK);
#endif

    return 0;
}

DEVICE_AND_API_INIT(gpio_gm_k, CONFIG_GPIO_GM_K_NAME,
                    &gpio_gm_k_init, &gpio_gm_k_data,
                    &gpio_gm_k_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_GPIO,
                    &api_funcs);

#endif
