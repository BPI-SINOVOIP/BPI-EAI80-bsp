/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        pinmux_gm.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       pinmux source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <errno.h>
#include <device.h>
#include <pinmux.h>

/*gree sdk include*/
#include <gm_common.h>
#include <gm_hal_gpio.h>

#define PORTA    GPIOA
#define PORTB    GPIOB
#define PORTC    GPIOC
#define PORTD    GPIOD
#define PORTE    GPIOE
#define PORTF    GPIOF
#define PORTG    GPIOG
#define PORTH    GPIOH
#define PORTI    GPIOI
#define PORTJ    GPIOJ
#define PORTK    GPIOK

struct pinmux_gm_config
{
    GPIO_Device_T *base;
    GPIO_PinConfig_T *cfg;
};

static int pinmux_gm_set(struct device *dev, uint32_t pin, uint32_t func)
{
    int ret = 0;
    struct pinmux_gm_config const *config = dev->config->config_info;
    GPIO_Device_T *base = config->base;

    GPIO_PinConfig_T *gpio_cfg = (GPIO_PinConfig_T *) func;

    ret = HAL_GPIO_Init(base, gpio_cfg);

    return ret;
}

static int pinmux_gm_get(struct device *dev, uint32_t pin, uint32_t *func)
{
    const struct pinmux_gm_config *config = dev->config->config_info;
    *func = (uint32_t) config->cfg;

    return 0;
}

static int pinmux_gm_pullup(struct device *dev, uint32_t pin, uint8_t func)
{
    return -ENOTSUP;
}

static int pinmux_gm_input(struct device *dev, uint32_t pin, uint8_t func)
{
    return -ENOTSUP;
}

static int pinmux_gm_init(struct device *dev)
{
    return 0;
}

static const struct pinmux_driver_api pinmux_gm_driver_api =
{
    .set = pinmux_gm_set,
    .get = pinmux_gm_get,
    .pullup = pinmux_gm_pullup,
    .input = pinmux_gm_input,
};

#ifdef CONFIG_PINMUX_GM_PORTA
static  struct pinmux_gm_config pinmux_gm_porta_config =
{
    .base = PORTA,
};

DEVICE_AND_API_INIT(pinmux_porta, CONFIG_PINMUX_GM_PORTA_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_porta_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTB
static  struct pinmux_gm_config pinmux_gm_portb_config =
{
    .base = PORTB,
};

DEVICE_AND_API_INIT(pinmux_portb, CONFIG_PINMUX_GM_PORTB_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portb_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTC
static  struct pinmux_gm_config pinmux_gm_portc_config =
{
    .base = PORTC,
};

DEVICE_AND_API_INIT(pinmux_portc, CONFIG_PINMUX_GM_PORTC_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portc_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTD
static  struct pinmux_gm_config pinmux_gm_portd_config =
{
    .base = PORTD,
};

DEVICE_AND_API_INIT(pinmux_portd, CONFIG_PINMUX_GM_PORTD_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portd_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTE
static  struct pinmux_gm_config pinmux_gm_porte_config =
{
    .base = PORTE,
};

DEVICE_AND_API_INIT(pinmux_porte, CONFIG_PINMUX_GM_PORTE_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_porte_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTF
static  struct pinmux_gm_config pinmux_gm_portf_config =
{
    .base = PORTF,
};

DEVICE_AND_API_INIT(pinmux_portf, CONFIG_PINMUX_GM_PORTF_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portf_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTG
static  struct pinmux_gm_config pinmux_gm_portg_config =
{
    .base = PORTG,
};

DEVICE_AND_API_INIT(pinmux_portg, CONFIG_PINMUX_GM_PORTG_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portg_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTH
static  struct pinmux_gm_config pinmux_gm_porth_config =
{
    .base = PORTH,
};

DEVICE_AND_API_INIT(pinmux_porth, CONFIG_PINMUX_GM_PORTH_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_porth_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTI
static  struct pinmux_gm_config pinmux_gm_porti_config =
{
    .base = PORTI,
};

DEVICE_AND_API_INIT(pinmux_porti, CONFIG_PINMUX_GM_PORTI_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_porti_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTJ
static  struct pinmux_gm_config pinmux_gm_portj_config =
{
    .base = PORTJ,
};

DEVICE_AND_API_INIT(pinmux_portj, CONFIG_PINMUX_GM_PORTJ_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portj_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif

#ifdef CONFIG_PINMUX_GM_PORTK
static  struct pinmux_gm_config pinmux_gm_portk_config =
{
    .base = PORTK,
};

DEVICE_AND_API_INIT(pinmux_portk, CONFIG_PINMUX_GM_PORTK_NAME,
                    &pinmux_gm_init,
                    NULL, &pinmux_gm_portk_config,
                    PRE_KERNEL_1, DEVICE_PRIORITY_PINMUX,
                    &pinmux_gm_driver_api);
#endif
