/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        pinmux.c
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



#include <kernel.h>
#include <device.h>
#include <init.h>
#include <pinmux.h>
#include <sys_io.h>

/*gree hal include*/
#include <gm_soc.h>
#include <gm_hal_gpio.h>

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

#if (defined(CONFIG_FLASH) || defined(CONFIG_SFLASH))
/** Configure pinmux for an available UART device */

static int sqi_flash_pinumx_config()
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_sflashPinMux[6] =
    {
        {PINMUX_GPIO_PIN_12, GPIOF, GPIOF12_AF4_EXT_SFLASH_MISO},
        {PINMUX_GPIO_PIN_13, GPIOF, GPIOF13_AF4_EXT_SFLASH_SCK},
        {PINMUX_GPIO_PIN_14, GPIOF, GPIOF14_AF4_EXT_SFLASH_HOLDJ},
        {PINMUX_GPIO_PIN_15, GPIOF, GPIOF15_AF4_EXT_SFLASH_WJ},
        {PINMUX_GPIO_PIN_0, GPIOG, GPIOG0_AF4_EXT_SFLASH_MOSI},
        {PINMUX_GPIO_PIN_1, GPIOG, GPIOG1_AF4_EXT_SFLASH_CSJ},
    };

    struct device *portg = device_get_binding(CONFIG_PINMUX_GM_PORTG_NAME);
    struct device *portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    pin_cfg.pin = BIT(s_sflashPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_sflashPinMux[0].alternate;
    ret = pinmux_pin_set(portf, s_sflashPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    /** UART3 GPIO Configuration */
    pin_cfg.pin = BIT(s_sflashPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_sflashPinMux[1].alternate;
    ret = pinmux_pin_set(portf, s_sflashPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_sflashPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_sflashPinMux[2].alternate;
    ret = pinmux_pin_set(portf, s_sflashPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_sflashPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_sflashPinMux[3].alternate;
    ret = pinmux_pin_set(portf, s_sflashPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_sflashPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_sflashPinMux[4].alternate;
    ret = pinmux_pin_set(portg, s_sflashPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = BIT(s_sflashPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_sflashPinMux[5].alternate;
    ret = pinmux_pin_set(portg, s_sflashPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

}

#endif

#if defined(CONFIG_UART0)
/** Configure pinmux for an available UART device */

static int uart0_pinmux_config(struct device *port1, struct device *port2)
{
    UNUSED(port1);
    UNUSED(port2);

    return 0;
}
#endif

#if defined(CONFIG_UART1)
/** Configure pinmux for an available UART device */

static int uart1_pinmux_config(struct device *port)
{
    int ret = 0;
    struct device *porte = device_get_binding(CONFIG_PINMUX_GM_PORTB_NAME);
    if (porte == NULL)
    {
        return -1;
    }

    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_uartPinMux[2] =
    {
        //following config can not work
        {PINMUX_GPIO_PIN_12, GPIOB, GPIOB12_AF1_UART1_TX},
        {PINMUX_GPIO_PIN_13, GPIOB, GPIOB13_AF1_UART1_RX},
    };


    pin_cfg.pin = BIT(s_uartPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_uartPinMux[0].alternate;
    ret = pinmux_pin_set(porte, s_uartPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_uartPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_uartPinMux[1].alternate;
    ret = pinmux_pin_set(porte, s_uartPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif

#if defined(CONFIG_UART2)
/** Configure pinmux for an available UART device */

static int uart2_pinmux_config(struct device *port)
{
    port = port;

    return 0;
}
#endif

#if defined(CONFIG_UART7)
/** Configure pinmux for an available UART device */

static int uart7_pinmux_config(struct device *port)
{
    int ret = 0;
    struct device *portx = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (portx == NULL)
    {
        return -1;
    }

    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_uartPinMux[2] =
    {
        //following config can not work
        {PINMUX_GPIO_PIN_8, GPIOC, GPIOC8_AF1_UART7_TX},
        {PINMUX_GPIO_PIN_9, GPIOC, GPIOC9_AF1_UART7_RX},
    };


    pin_cfg.pin = BIT(s_uartPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_uartPinMux[0].alternate;
    ret = pinmux_pin_set(portx, s_uartPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_uartPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_uartPinMux[1].alternate;
    ret = pinmux_pin_set(port, s_uartPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif


#if defined(CONFIG_CAN)
static int can0_pinmux_config()
{
    int ret = 0;
    struct device *porth = NULL;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_CANPinMux[2] =
    {
        {PINMUX_GPIO_PIN_14, GPIOH, GPIOH14_AF1_CAN0_RX},
        {PINMUX_GPIO_PIN_15, GPIOH, GPIOH15_AF1_CAN0_TX},
    };

    porth = device_get_binding(CONFIG_PINMUX_GM_PORTH_NAME);
    if (porth == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_CANPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[0].alternate;
    ret = pinmux_pin_set(porth, s_CANPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_CANPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[1].alternate;
    ret = pinmux_pin_set(porth, s_CANPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif

#if defined(CONFIG_MMC)
static int mmc_pinmux_config(struct device *port1, struct device *port2)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_mmcPinMux[10] =
    {
        {PINMUX_GPIO_PIN_2, GPIOG, GPIOG2_AF6_SDMMC_D2},
        {PINMUX_GPIO_PIN_3, GPIOG, GPIOG3_AF6_SDMMC_D3},
        {PINMUX_GPIO_PIN_4, GPIOG, GPIOG4_AF6_SDMMC_CMD},
        {PINMUX_GPIO_PIN_5, GPIOG, GPIOG5_AF6_SDMMC_D1},
        {PINMUX_GPIO_PIN_6, GPIOG, GPIOG6_AF6_SDMMC_D0},
        {PINMUX_GPIO_PIN_7, GPIOG, GPIOG7_AF6_SDMMC_CLK},
        {PINMUX_GPIO_PIN_8, GPIOG, GPIOG8_AF6_SDMMC_D4},
        {PINMUX_GPIO_PIN_9, GPIOG, GPIOG9_AF6_SDMMC_D5},
        {PINMUX_GPIO_PIN_10, GPIOG, GPIOG10_AF6_SDMMC_D6},
        {PINMUX_GPIO_PIN_12, GPIOG, GPIOG12_AF6_SDMMC_D7},
    };

    struct device *portg = device_get_binding(CONFIG_PINMUX_GM_PORTG_NAME);
    pin_cfg.pin = BIT(s_mmcPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[0].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    /** UART3 GPIO Configuration */
    pin_cfg.pin = BIT(s_mmcPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[1].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[2].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[3].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[4].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = BIT(s_mmcPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[5].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[6].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[6].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[6].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[7].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[7].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[7].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[8].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[8].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[8].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[9].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[9].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[9].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
    return 0;
}
#endif



#if defined(CONFIG_DCMI_0)
/** Configure pinmux for an available DCMI CAMERA device */

static int dcmi0_pinmux_config()
{
    int ret = 0;
    struct device *porte = NULL;
    struct device *porth = NULL;
    struct device *porti = NULL;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_VINPinMux[11] =
    {
        {PINMUX_GPIO_PIN_3, GPIOE, GPIOE3_AF3_VIN_D2},
        {PINMUX_GPIO_PIN_4, GPIOE, GPIOE4_AF3_VIN_D1},
        {PINMUX_GPIO_PIN_5, GPIOE, GPIOE5_AF3_VIN_D3},
        {PINMUX_GPIO_PIN_6, GPIOE, GPIOE6_AF3_VIN_D0},
        {PINMUX_GPIO_PIN_11, GPIOH, GPIOH11_AF2_VIN_D4},
        {PINMUX_GPIO_PIN_12, GPIOH, GPIOH12_AF2_VIN_PIXCLK},
        {PINMUX_GPIO_PIN_13, GPIOH, GPIOH13_AF2_VIN_D5},
        {PINMUX_GPIO_PIN_14, GPIOH, GPIOH14_AF2_VIN_D6},
        {PINMUX_GPIO_PIN_15, GPIOH, GPIOH15_AF2_VIN_D7},
        {PINMUX_GPIO_PIN_0, GPIOI, GPIOHI0_AF2_VIN_HSYNC},
        {PINMUX_GPIO_PIN_1, GPIOI, GPIOHI1_AF2_VIN_VSYNC},
    };

    porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[0].alternate;
    ret = pinmux_pin_set(porte, s_VINPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[1].alternate;
    ret = pinmux_pin_set(porte, s_VINPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[2].alternate;
    ret = pinmux_pin_set(porte, s_VINPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[3].alternate;
    ret = pinmux_pin_set(porte, s_VINPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    porth = device_get_binding(CONFIG_PINMUX_GM_PORTH_NAME);
    if (porth == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[4].alternate;
    ret = pinmux_pin_set(porth, s_VINPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[5].alternate;
    ret = pinmux_pin_set(porth, s_VINPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[6].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[6].alternate;
    ret = pinmux_pin_set(porth, s_VINPinMux[6].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[7].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[7].alternate;
    ret = pinmux_pin_set(porth, s_VINPinMux[7].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[8].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[8].alternate;
    ret = pinmux_pin_set(porth, s_VINPinMux[8].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[9].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[9].alternate;
    ret = pinmux_pin_set(porti, s_VINPinMux[9].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[10].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[10].alternate;
    ret = pinmux_pin_set(porti, s_VINPinMux[10].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif

#if defined(CONFIG_DCMI_1)
/** Configure pinmux for an available DCMI CAMERA device */

static int dcmi1_pinmux_config()
{
    int ret = 0;
    struct device *porta = NULL;
    struct device *portb = NULL;
    struct device *porte = NULL;
    struct device *portf = NULL;
    struct device *portg = NULL;
    struct device *portj = NULL;
    struct device *portk = NULL;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_VINPinMux[11] =
    {
        {PINMUX_GPIO_PIN_14, GPIOA, GPIOA14_AF1_VIN2_D0},
        {PINMUX_GPIO_PIN_15, GPIOA, GPIOA15_AF1_VIN2_D1},
        {PINMUX_GPIO_PIN_0, GPIOB, GPIOB0_AF1_VIN2_D2},
        {PINMUX_GPIO_PIN_14, GPIOE, GPIOE14_AF1_VIN2_D3},
        {PINMUX_GPIO_PIN_0, GPIOF, GPIOF0_AF1_VIN2_D4},
        {PINMUX_GPIO_PIN_8, GPIOG, GPIOG8_AF1_VIN2_D6},
        {PINMUX_GPIO_PIN_9, GPIOG, GPIOG9_AF1_VIN2_D7},
        {PINMUX_GPIO_PIN_10, GPIOG, GPIOG10_AF1_VIN2_VSYNC},
        {PINMUX_GPIO_PIN_8, GPIOJ, GPIOJ8_AF1_VIN2_HSYNC},
        {PINMUX_GPIO_PIN_10, GPIOJ, GPIOJ10_AF1_VIN2_PIXCLK},
        {PINMUX_GPIO_PIN_2, GPIOK, GPIOK2_AF1_TIM1_VIN2_D5},
    };

    porta = device_get_binding(CONFIG_PINMUX_GM_PORTA_NAME);
    if (porta == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[0].alternate;
    ret = pinmux_pin_set(porta, s_VINPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[1].alternate;
    ret = pinmux_pin_set(porta, s_VINPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portb = device_get_binding(CONFIG_PINMUX_GM_PORTB_NAME);
    if (portb == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[2].alternate;
    ret = pinmux_pin_set(portb, s_VINPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[3].alternate;
    ret = pinmux_pin_set(porte, s_VINPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if (portf == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[4].alternate;
    ret = pinmux_pin_set(portf, s_VINPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portg = device_get_binding(CONFIG_PINMUX_GM_PORTG_NAME);
    if (portg == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[5].alternate;
    ret = pinmux_pin_set(portg, s_VINPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[6].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[6].alternate;
    ret = pinmux_pin_set(portg, s_VINPinMux[6].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[7].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[7].alternate;
    ret = pinmux_pin_set(portg, s_VINPinMux[7].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portj = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
    if (portj == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[8].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[8].alternate;
    ret = pinmux_pin_set(portj, s_VINPinMux[8].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VINPinMux[9].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[9].alternate;
    ret = pinmux_pin_set(portj, s_VINPinMux[9].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portk = device_get_binding(CONFIG_PINMUX_GM_PORTK_NAME);
    if (portk == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VINPinMux[10].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VINPinMux[10].alternate;
    ret = pinmux_pin_set(portk, s_VINPinMux[10].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif

#if defined(CONFIG_DISPLAY_GM6721X)
/** Configure pinmux for an available DISPLAY device */

static int lcd_pinmux_config()
{
    int ret = 0;
    struct device *porta = NULL;
    struct device *portb = NULL;
    struct device *porte = NULL;
    struct device *portf = NULL;
    struct device *portg = NULL;
    struct device *porth = NULL;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_VOUTPinMux[28] =
    {
        {PINMUX_GPIO_PIN_12, GPIOA, GPIOA12_AF13_VOUT_HSYNC},
        {PINMUX_GPIO_PIN_13, GPIOA, GPIOA13_AF13_VOUT_VSYNC},
        {PINMUX_GPIO_PIN_14, GPIOA, GPIOA14_AF13_VOUT_R0},
        {PINMUX_GPIO_PIN_15, GPIOA, GPIOA15_AF13_VOUT_R1},
        {PINMUX_GPIO_PIN_0, GPIOB, GPIOA15_AF13_VOUT_R1},
        {PINMUX_GPIO_PIN_1, GPIOB, GPIOB1_AF13_VOUT_R3},
        {PINMUX_GPIO_PIN_2, GPIOB, GPIOB2_AF13_VOUT_R4},
        {PINMUX_GPIO_PIN_3, GPIOB, GPIOB3_AF13_VOUT_R5},
        {PINMUX_GPIO_PIN_4, GPIOB, GPIOB4_AF13_VOUT_R6},
        {PINMUX_GPIO_PIN_9, GPIOB, GPIOB9_AF13_VOUT_R7},
        {PINMUX_GPIO_PIN_14, GPIOE, GPIOE14_AF13_VOUT_G0},
        {PINMUX_GPIO_PIN_0, GPIOF, GPIOF0_AF13_VOUT_G1},
        {PINMUX_GPIO_PIN_5, GPIOF, GPIOF5_AF13_VOUT_G2},
        {PINMUX_GPIO_PIN_6, GPIOF, GPIOF6_AF13_VOUT_G3},
        {PINMUX_GPIO_PIN_7, GPIOF, GPIOF7_AF13_VOUT_G4},
        {PINMUX_GPIO_PIN_10, GPIOF, GPIOF10_AF13_VOUT_G5},
        {PINMUX_GPIO_PIN_11, GPIOF, GPIOF11_AF13_VOUT_G6},
        {PINMUX_GPIO_PIN_12, GPIOF, GPIOF12_AF13_VOUT_G7},
        {PINMUX_GPIO_PIN_6, GPIOG, GPIOG6_AF13_VOUT_OUT_CLK},
        {PINMUX_GPIO_PIN_7, GPIOG, GPIOG7_AF13_VOUT_DE},
        {PINMUX_GPIO_PIN_8, GPIOG, GPIOG8_AF13_VOUT_B0},
        {PINMUX_GPIO_PIN_9, GPIOG, GPIOG9_AF13_VOUT_B1},
        {PINMUX_GPIO_PIN_10, GPIOG, GPIOG10_AF13_VOUT_B2},
        {PINMUX_GPIO_PIN_11, GPIOG, GPIOG11_AF13_VOUT_B3},
        {PINMUX_GPIO_PIN_7, GPIOH, GPIOH7_AF13_VOUT_B4},
        {PINMUX_GPIO_PIN_8, GPIOH, GPIOH8_AF13_VOUT_B5},
        {PINMUX_GPIO_PIN_9, GPIOH, GPIOH9_AF13_VOUT_B6},
        {PINMUX_GPIO_PIN_10, GPIOH, GPIOH10_AF13_VOUT_B7},
    };

    porta = device_get_binding(CONFIG_PINMUX_GM_PORTA_NAME);
    if (porta == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[0].alternate;
    ret = pinmux_pin_set(porta, s_VOUTPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[1].alternate;
    ret = pinmux_pin_set(porta, s_VOUTPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[2].alternate;
    ret = pinmux_pin_set(porta, s_VOUTPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[3].alternate;
    ret = pinmux_pin_set(porta, s_VOUTPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portb = device_get_binding(CONFIG_PINMUX_GM_PORTB_NAME);
    if (portb == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[4].alternate;
    ret = pinmux_pin_set(portb, s_VOUTPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[5].alternate;
    ret = pinmux_pin_set(portb, s_VOUTPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[6].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[6].alternate;
    ret = pinmux_pin_set(portb, s_VOUTPinMux[6].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[7].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[7].alternate;
    ret = pinmux_pin_set(portb, s_VOUTPinMux[7].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[8].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[8].alternate;
    ret = pinmux_pin_set(portb, s_VOUTPinMux[8].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[9].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[9].alternate;
    ret = pinmux_pin_set(portb, s_VOUTPinMux[9].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[10].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[10].alternate;
    ret = pinmux_pin_set(porte, s_VOUTPinMux[10].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if (portf == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[11].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[11].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[11].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[12].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[12].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[12].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[13].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[13].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[13].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[14].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[14].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[14].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[15].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[15].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[15].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[16].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[16].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[16].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[17].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[17].alternate;
    ret = pinmux_pin_set(portf, s_VOUTPinMux[17].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    portg = device_get_binding(CONFIG_PINMUX_GM_PORTG_NAME);
    if (portg == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[18].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[18].alternate;
    ret = pinmux_pin_set(portg, s_VOUTPinMux[18].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[19].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[19].alternate;
    ret = pinmux_pin_set(portg, s_VOUTPinMux[19].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[20].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[20].alternate;
    ret = pinmux_pin_set(portg, s_VOUTPinMux[20].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[21].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[21].alternate;
    ret = pinmux_pin_set(portg, s_VOUTPinMux[21].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[22].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[22].alternate;
    ret = pinmux_pin_set(portg, s_VOUTPinMux[22].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[23].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[23].alternate;
    ret = pinmux_pin_set(portg, s_VOUTPinMux[23].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    porth = device_get_binding(CONFIG_PINMUX_GM_PORTH_NAME);
    if (porth == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[24].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[24].alternate;
    ret = pinmux_pin_set(porth, s_VOUTPinMux[24].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[25].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[25].alternate;
    ret = pinmux_pin_set(porth, s_VOUTPinMux[25].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[26].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[26].alternate;
    ret = pinmux_pin_set(porth, s_VOUTPinMux[26].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_VOUTPinMux[27].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_VOUTPinMux[27].alternate;
    ret = pinmux_pin_set(porth, s_VOUTPinMux[27].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif

#if defined(CONFIG_ADC_GM)
static int adc_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    int i = 0;
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    struct device *portb = device_get_binding(CONFIG_PINMUX_GM_PORTB_NAME);
#if 1
    static Device_Pinmux_T s_pinmux[] =
    {
        {PINMUX_GPIO_PIN_0,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_0 pinmux config paramter */
        {PINMUX_GPIO_PIN_1,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_1 pinmux config paramter */
#if 0
        {PINMUX_GPIO_PIN_2,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_2 pinmux config paramter */
        {PINMUX_GPIO_PIN_7,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_3 pinmux config paramter */
        {PINMUX_GPIO_PIN_8,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_4 pinmux config paramter */
        {PINMUX_GPIO_PIN_9,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_5 pinmux config paramter */
        {PINMUX_GPIO_PIN_10,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_6 pinmux config paramter */
        {PINMUX_GPIO_PIN_11,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_7 pinmux config paramter */ //
        {PINMUX_GPIO_PIN_14,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_8 pinmux config paramter */ //
        {PINMUX_GPIO_PIN_15,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_9 pinmux config paramter */

        {PINMUX_GPIO_PIN_12, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_10 pinmux config paramter */
        {PINMUX_GPIO_PIN_13, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_11 pinmux config paramter */
        {PINMUX_GPIO_PIN_14, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_12 pinmux config paramter */
        {PINMUX_GPIO_PIN_15, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_13 pinmux config paramter */

        {PINMUX_GPIO_PIN_12, GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_14 pinmux config paramter */
        {PINMUX_GPIO_PIN_13, GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_15 pinmux config paramter */
#endif
    };

#else
    static Device_Pinmux_T s_pinmux[] =
    {
        {PINMUX_GPIO_PIN_0,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_0 pinmux config paramter */
        {PINMUX_GPIO_PIN_1,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_1 pinmux config paramter */
        {PINMUX_GPIO_PIN_2,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_2 pinmux config paramter */
        {PINMUX_GPIO_PIN_7,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_3 pinmux config paramter */
        {PINMUX_GPIO_PIN_8,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_4 pinmux config paramter */
        {PINMUX_GPIO_PIN_9,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_5 pinmux config paramter */
        {PINMUX_GPIO_PIN_10,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_6 pinmux config paramter */
        {PINMUX_GPIO_PIN_11,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_7 pinmux config paramter */ //
        {PINMUX_GPIO_PIN_14,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_8 pinmux config paramter */ //
        {PINMUX_GPIO_PIN_15,  GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_9 pinmux config paramter */

        {PINMUX_GPIO_PIN_12, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_10 pinmux config paramter */
        {PINMUX_GPIO_PIN_13, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_11 pinmux config paramter */
        {PINMUX_GPIO_PIN_14, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_12 pinmux config paramter */
        {PINMUX_GPIO_PIN_15, GPIOB, GPIO_AF_NONE}, /* ADC_CHANNEL_13 pinmux config paramter */

        {PINMUX_GPIO_PIN_12, GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_14 pinmux config paramter */
        {PINMUX_GPIO_PIN_13, GPIOC, GPIO_AF_NONE}, /* ADC_CHANNEL_15 pinmux config paramter */
    };
#endif
    Device_Pinmux_T *pADCPinmux = &s_pinmux[0];

    if (portc == NULL || portb == NULL)
    {
        return -1;
    }

    memset(&pin_cfg, 0, sizeof(pin_cfg));
    pin_cfg.pin = BIT(pADCPinmux->pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = 0;

    for (i = 0; i < sizeof(s_pinmux) / sizeof(Device_Pinmux_T); i++)
    {
        pADCPinmux = &s_pinmux[i];
        pin_cfg.pin = BIT(pADCPinmux->pin);
        ret = pinmux_pin_set(portc, pADCPinmux->pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
    }


#if 0
    for (i = 0; i < 10; i++)
    {
        pADCPinmux = &s_pinmux[i];
        pin_cfg.pin = BIT(pADCPinmux->pin);
        ret = pinmux_pin_set(portc, pADCPinmux->pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
    }

    for (i = 0; i < 4; i++)
    {
        pADCPinmux = &s_pinmux[10 + i];
        pin_cfg.pin = BIT(pADCPinmux->pin);
        ret = pinmux_pin_set(portb, pADCPinmux->pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
    }

    for (i = 0; i < 2; i++)
    {
        pADCPinmux = &s_pinmux[14 + i];
        pin_cfg.pin = BIT(pADCPinmux->pin);
        ret = pinmux_pin_set(portc, pADCPinmux->pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
    }
#endif

    return ret;
}
#endif

#if defined(CONFIG_DAC_0)
static int dac0_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    int i = 0;
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);

    Device_Pinmux_T pinmux[] =
    {
        {PINMUX_GPIO_PIN_11,  GPIOC, GPIO_AF_NONE},/*GPIO_PIN_9*/
    };
    Device_Pinmux_T *pDacPinmux = &pinmux[0];

    if (portc == NULL)
    {
        return -1;
    }

    memset(&pin_cfg, 0, sizeof(pin_cfg));
    pin_cfg.pin = BIT(pDacPinmux->pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = 0;
    pDacPinmux = &pinmux[i];
    pin_cfg.pin = BIT(pDacPinmux->pin);
    ret = pinmux_pin_set(portc, pDacPinmux->pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return ret;
}
#endif

#if defined(CONFIG_DAC_1)
static int dac1_pinmux_config(void)
{
    int ret = 0;
#ifndef CONFIG_SDRAM_BOOT
    GPIO_PinConfig_T pin_cfg;
    int i = 0;
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);

    Device_Pinmux_T pinmux[] =
    {
        {PINMUX_GPIO_PIN_9,  GPIOC, GPIO_AF_NONE},
    };
    Device_Pinmux_T *pDacPinmux = &pinmux[0];

    if (portc == NULL)
    {
        return -1;
    }

    memset(&pin_cfg, 0, sizeof(pin_cfg));
    pin_cfg.pin = BIT(pDacPinmux->pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = 0;
    pDacPinmux = &pinmux[i];
    pin_cfg.pin = BIT(pDacPinmux->pin);
    ret = pinmux_pin_set(portc, pDacPinmux->pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
#endif

    return ret;
}
#endif

#if defined(CONFIG_ACMP)
static int acmp_pinmux_config(void)
{
    int ret = 0;
#if 0
    GPIO_PinConfig_T pin_cfg;
    int i = 0;
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);

    Device_Pinmux_T pinmux[] =
    {
        {PINMUX_GPIO_PIN_12,  GPIOC, GPIO_AF_NONE},
        {PINMUX_GPIO_PIN_13,  GPIOC, GPIO_AF_NONE},
    };
    Device_Pinmux_T *pACMPPinmux = &pinmux[0];

    if (portc == NULL)
    {
        return -1;
    }

    memset(&pin_cfg, 0, sizeof(pin_cfg));
    pin_cfg.pin = BIT(pACMPPinmux->pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = 0;

    for (i = 0; i < 2; i++)
    {
        pACMPPinmux = &pinmux[i];
        pin_cfg.pin = BIT(pACMPPinmux->pin);
        ret = pinmux_pin_set(portc, pACMPPinmux->pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
    }
#endif

    return ret;
}

static int acmp_configoutput_pinmux(void)
{
    int ret = 0;
#if 0
    GPIO_PinConfig_T pin_cfg;
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);

    Device_Pinmux_T pinmux[] =
    {
        {PINMUX_GPIO_PIN_14,  GPIOC, GPIOC14_AF10_ACMP_OUT}, /*XPC14 ,XPE7*/
    };
    Device_Pinmux_T *pPinmux = &pinmux[0];

    if (portc == NULL)
    {
        return -1;
    }

    memset(&pin_cfg, 0, sizeof(pin_cfg));
    pin_cfg.pin = BIT(pPinmux->pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = GPIOC14_AF10_ACMP_OUT;
    pPinmux = &pinmux[0];
    pin_cfg.pin = BIT(pPinmux->pin);
    ret = pinmux_pin_set(portc, pPinmux->pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
#endif
    return ret;
}
#endif

#if defined(CONFIG_TIM1)
/** Configure pinmux for an available TIM1 device */
static int tim1_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_timPinMux[10] =
    {
        {PINMUX_GPIO_PIN_1,  GPIOE, GPIOE1_AF5_TIM1_CH1N},
        {PINMUX_GPIO_PIN_2,  GPIOE, GPIOE2_AF5_TIM1_CH1},
        {PINMUX_GPIO_PIN_3,  GPIOE, GPIOE3_AF5_TIM1_CH2N},
        {PINMUX_GPIO_PIN_4,  GPIOE, GPIOE4_AF5_TIM1_CH2},
        {PINMUX_GPIO_PIN_5,  GPIOE, GPIOE5_AF5_TIM1_CH3N},
        {PINMUX_GPIO_PIN_6,  GPIOE, GPIOE6_AF5_TIM1_CH3},
        {PINMUX_GPIO_PIN_7,  GPIOE, GPIOE7_AF5_TIM1_CH4},
        {PINMUX_GPIO_PIN_8,  GPIOE, GPIOE8_AF5_TIM1_CH4N},
        {PINMUX_GPIO_PIN_9,  GPIOE, GPIOE9_AF5_TIM1_BKIN},
        {PINMUX_GPIO_PIN_10, GPIOE, GPIOE10_AF5_TIM1_ETR},
    };

    struct device *porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[0].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[1].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[2].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[3].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[4].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[5].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[6].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[6].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[6].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[7].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[7].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[7].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[8].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[8].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[8].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[9].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[9].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[9].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

}
#endif

#if defined(CONFIG_TIM2)
/** Configure pinmux for an available TIM2 device */
static int tim2_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_timPinMux[6] =
    {
        {PINMUX_GPIO_PIN_11,  GPIOE, GPIOE11_AF5_TIM2_CH1},
        {PINMUX_GPIO_PIN_12,  GPIOE, GPIOE12_AF5_TIM2_CH2},
        {PINMUX_GPIO_PIN_13,  GPIOE, GPIOE13_AF5_TIM2_CH3},
        {PINMUX_GPIO_PIN_14,  GPIOE, GPIOE14_AF5_TIM2_CH4},
        {PINMUX_GPIO_PIN_15,  GPIOE, GPIOE15_AF5_TIM2_ETR},
        {PINMUX_GPIO_PIN_0,   GPIOF, GPIOF0_AF5_TIM2_BKIN},
    };

    struct device *porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[0].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[1].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[2].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[3].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[4].alternate;
    ret = pinmux_pin_set(porte, s_timPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    struct device *portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if (portf == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[5].alternate;
    ret = pinmux_pin_set(portf, s_timPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

}
#endif

#if defined(CONFIG_TIM3)
/** Configure pinmux for an available TIM3 device */
static int tim3_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_timPinMux[5] =
    {
        {PINMUX_GPIO_PIN_1,  GPIOF, GPIOF1_AF5_TIM3_CH1},
        {PINMUX_GPIO_PIN_2,  GPIOF, GPIOF2_AF5_TIM3_CH2},
        {PINMUX_GPIO_PIN_3,  GPIOF, GPIOF3_AF5_TIM3_CH3},
        {PINMUX_GPIO_PIN_4,  GPIOF, GPIOF4_AF5_TIM3_CH4},
        {PINMUX_GPIO_PIN_5,  GPIOF, GPIOF5_AF5_TIM3_ETR},
    };

    struct device *portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if (portf == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[0].alternate;
    ret = pinmux_pin_set(portf, s_timPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[1].alternate;
    ret = pinmux_pin_set(portf, s_timPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[2].alternate;
    ret = pinmux_pin_set(portf, s_timPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[3].alternate;
    ret = pinmux_pin_set(portf, s_timPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[4].alternate;
    ret = pinmux_pin_set(portf, s_timPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

}
#endif

#if defined(CONFIG_TIM4)
/** Configure pinmux for an available TIM4 device */
static int tim4_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_timPinMux[5] =
    {
        {PINMUX_GPIO_PIN_11,  GPIOH, GPIOH11_AF5_TIM4_ETR},
        {PINMUX_GPIO_PIN_12,  GPIOH, GPIOH12_AF5_TIM4_CH1},
        {PINMUX_GPIO_PIN_13,  GPIOH, GPIOH13_AF5_TIM4_CH2},
        {PINMUX_GPIO_PIN_14,  GPIOH, GPIOH14_AF5_TIM4_CH3},
        {PINMUX_GPIO_PIN_15,  GPIOH, GPIOH15_AF5_TIM4_CH4},
    };

    struct device *porth = device_get_binding(CONFIG_PINMUX_GM_PORTH_NAME);
    if (porth == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[0].alternate;
    ret = pinmux_pin_set(porth, s_timPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[1].alternate;
    ret = pinmux_pin_set(porth, s_timPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[2].alternate;
    ret = pinmux_pin_set(porth, s_timPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[3].alternate;
    ret = pinmux_pin_set(porth, s_timPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[4].alternate;
    ret = pinmux_pin_set(porth, s_timPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif

#if defined(CONFIG_TIM5)
/** Configure pinmux for an available TIM5 device */
static int tim5_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_timPinMux[5] =
    {
        {PINMUX_GPIO_PIN_10, GPIOH, GPIOH10_AF5_TIM5_ETR},
        {PINMUX_GPIO_PIN_0,  GPIOI, GPIOI0_AF5_TIM5_CH1},
        {PINMUX_GPIO_PIN_1,  GPIOI, GPIOI1_AF5_TIM5_CH2},
        {PINMUX_GPIO_PIN_2,  GPIOI, GPIOI2_AF5_TIM5_CH3},
        {PINMUX_GPIO_PIN_3,  GPIOI, GPIOI3_AF5_TIM5_CH4},
    };

    struct device *porth = device_get_binding(CONFIG_PINMUX_GM_PORTH_NAME);
    if (porth == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[0].alternate;
    ret = pinmux_pin_set(porth, s_timPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    struct device *porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[1].alternate;
    ret = pinmux_pin_set(porti, s_timPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[2].alternate;
    ret = pinmux_pin_set(porti, s_timPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[3].alternate;
    ret = pinmux_pin_set(porti, s_timPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[4].alternate;
    ret = pinmux_pin_set(porti, s_timPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;

}
#endif

#if defined(CONFIG_TIM8)
/** Configure pinmux for an available TIM8 device */
static int tim8_pinmux_config(void)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_timPinMux[10] =
    {
        {PINMUX_GPIO_PIN_2,  GPIOC, GPIOC2_AF5_TIM8_BKIN},
        {PINMUX_GPIO_PIN_3,  GPIOC, GPIOC3_AF5_TIM8_ETR},
        {PINMUX_GPIO_PIN_4,  GPIOC, GPIOC4_AF5_TIM8_CH1N},
        {PINMUX_GPIO_PIN_5,  GPIOC, GPIOC5_AF5_TIM8_CH1},
        //{PINMUX_GPIO_PIN_8,  GPIOC, GPIOC8_AF5_TIM8_CH1N},
        {PINMUX_GPIO_PIN_9,  GPIOC, GPIOC9_AF5_TIM8_CH2N},
        {PINMUX_GPIO_PIN_10, GPIOC, GPIOC10_AF5_TIM8_CH2},
        {PINMUX_GPIO_PIN_11, GPIOC, GPIOC11_AF5_TIM8_CH3N},
        {PINMUX_GPIO_PIN_12, GPIOC, GPIOC12_AF5_TIM8_CH3},
        {PINMUX_GPIO_PIN_14, GPIOC, GPIOC14_AF5_TIM8_CH4},
        {PINMUX_GPIO_PIN_15, GPIOC, GPIOC15_AF5_TIM8_CH4N},
    };

    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (portc == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_timPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[0].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[1].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[2].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[3].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[4].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[5].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[6].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[6].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[6].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[7].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[7].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[7].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[8].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[8].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[8].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_timPinMux[9].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_timPinMux[9].alternate;
    ret = pinmux_pin_set(portc, s_timPinMux[9].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}
#endif


#if defined(CONFIG_USB_PINMUX)
/** Configure pinmux for an available usb device/Host */
static int usb_pinmux_config(struct device *port)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorder to not affect analog input*/
    pGPIO = GPIOH;
    GPIO_InitStruct.pin  = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;

    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        printf("Err:usb_ConfigPinmux\n");
    }

    return ret;
}
#endif

#if defined(CONFIG_I2C0)
static int i2c0_pinmux_config(void)
{
    int ret = 0;
#ifdef CHIP_GM6721
    struct device *porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }
#endif
#ifdef CHIP_F6721B
    struct device *porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }
#endif
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_i2cPinMux[2] =
    {
#ifdef CHIP_GM6721
        {PINMUX_GPIO_PIN_0, GPIOI, GPIOI0_AF0_SCL},
        {PINMUX_GPIO_PIN_1, GPIOI, GPIOI1_AF0_SDA},
#endif
#ifdef CHIP_F6721B
        //        {PINMUX_GPIO_PIN_5, GPIOB, GPIOB5_AF3_I2C0_SCL},
        //        {PINMUX_GPIO_PIN_7, GPIOB, GPIOB7_AF3_I2C0_SDA},
        {PINMUX_GPIO_PIN_2, GPIOI, GPIOI2_AF3_I2C0_SCL},
        {PINMUX_GPIO_PIN_3, GPIOI, GPIOI3_AF3_I2C0_SDA},
#endif
    };

    pin_cfg.pin = BIT(s_i2cPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_i2cPinMux[0].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_i2cPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(porti, s_i2cPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_i2cPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_i2cPinMux[1].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porti, s_i2cPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(porti, s_i2cPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    return ret;
}
#endif

#if defined(CONFIG_I2C1)
static int i2c1_pinmux_config(void)
{
    int ret = 0;
#ifdef CHIP_GM6721
    struct device *porta = device_get_binding(CONFIG_PINMUX_GM_PORTA_NAME);
    if (porta == NULL)
    {
        return -1;
    }
#endif
#ifdef CHIP_F6721B
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (portc == NULL)
    {
        return -1;
    }
#endif

    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_i2cPinMux[2] =
    {
#ifdef CHIP_GM6721
        {PINMUX_GPIO_PIN_13, GPIOA, GPIOA13_AF0_SCL},
        {PINMUX_GPIO_PIN_12, GPIOA, GPIOA12_AF0_SDA},
#endif
#ifdef CHIP_F6721B
        {PINMUX_GPIO_PIN_3, GPIOC, GPIOC3_AF1_I2C1_SDA},
        {PINMUX_GPIO_PIN_2, GPIOC, GPIOC2_AF1_I2C1_SCL},
#endif
    };

    pin_cfg.pin = BIT(s_i2cPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_i2cPinMux[0].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porta, s_i2cPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portc, s_i2cPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_i2cPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_i2cPinMux[1].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porta, s_i2cPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portc, s_i2cPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    return ret;
}
#endif

#if defined(CONFIG_I2C2)
static int i2c2_pinmux_config(void)
{
    int ret = 0;
#ifdef CHIP_GM6721
    struct device *porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte == NULL)
    {
        return -1;
    }
#endif
#ifdef CHIP_F6721B
    struct device *portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if (portf == NULL)
    {
        return -1;
    }
#endif

    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_i2cPinMux[2] =
    {
#ifdef CHIP_GM6721
        {PINMUX_GPIO_PIN_10, GPIOE, GPIOE10_AFD_SCL},
        {PINMUX_GPIO_PIN_12, GPIOE, GPIOE12_AF0_SDA},
#endif
#ifdef CHIP_F6721B
        {PINMUX_GPIO_PIN_6, GPIOF, GPIOF6_AF1_I2C2_SCL},
        {PINMUX_GPIO_PIN_5, GPIOF, GPIOF5_AF1_I2C2_SDA},
#endif
    };

    pin_cfg.pin = BIT(s_i2cPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_i2cPinMux[0].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porte, s_i2cPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portf, s_i2cPinMux[0].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_i2cPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_i2cPinMux[1].alternate;
#ifdef CHIP_GM6721
    ret = pinmux_pin_set(porte, s_i2cPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
#ifdef CHIP_F6721B
    ret = pinmux_pin_set(portf, s_i2cPinMux[1].pin, (unsigned int)&pin_cfg);
#endif
    if (ret)
    {
        return ret;
    }

    return ret;
}
#endif

static int pinmux_gm_initialize(struct device *port)
{
    int ret = 0;
    ARG_UNUSED(port);

#if defined(CONFIG_UART0)
    /** Configure pinmux for an available UART device */
    struct device *portj = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
    struct device *portk = device_get_binding(CONFIG_PINMUX_GM_PORTK_NAME);
    if ((portj != NULL) && (portk != NULL))
    {
        ret = uart0_pinmux_config(portj, portk);
    }
#endif

#if defined(CONFIG_UART1)
    /** Configure pinmux for an available UART device */
    struct device *porte = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte != NULL)
    {
        ret = uart1_pinmux_config(porte);
    }
#endif

#if defined(CONFIG_UART2)
    /** Configure pinmux for an available UART device */
    struct device *porte2 = device_get_binding(CONFIG_PINMUX_GM_PORTE_NAME);
    if (porte2 != NULL)
    {
        ret = uart2_pinmux_config(porte2);
    }
#endif

#if defined(CONFIG_UART7)
    /** Configure pinmux for an available UART device */
    struct device *portuart7 = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (portuart7 != NULL)
    {
        ret = uart7_pinmux_config(portuart7);
    }
#endif

#if defined(CONFIG_I2C0)
    i2c0_pinmux_config();
#endif

#if defined(CONFIG_I2C1)
    i2c1_pinmux_config();
#endif

#if defined(CONFIG_I2C2)
    //i2c2_pinmux_config();
#endif

#if defined(CONFIG_DCMI_0)
    /** Configure pinmux for an available VIN device */
    ret = dcmi0_pinmux_config();
    if (ret)
    {
        return ret;
    }
#endif

#if defined(CONFIG_DCMI_1)
    /** Configure pinmux for an available VIN device */
    ret = dcmi1_pinmux_config();
    if (ret)
    {
        return ret;
    }
#endif

#if defined(CONFIG_DISPLAY_GM6721X)
    /** Configure pinmux for an available VOUT device */
    ret = lcd_pinmux_config();
    if (ret)
    {
        return ret;
    }
#endif

#if defined(CONFIG_MMC)

    /** Configure pinmux for an available MMC device */
    struct device *portg = device_get_binding(CONFIG_PINMUX_GM_PORTG_NAME);
    struct device *portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if ((portg != NULL) && (portg != NULL))
    {

        ret = mmc_pinmux_config(portg, portf);
    }
#endif

#if (defined(CONFIG_FLASH) || defined(CONFIG_SFLASH))
    /** Configure pinmux for an available flash device  sqi flash*/
    sqi_flash_pinumx_config();
#endif

#if defined(CONFIG_ADC_GM)
    adc_pinmux_config();
#endif

#if defined(CONFIG_DAC_0)
    dac0_pinmux_config();
#endif

#if defined(CONFIG_DAC_1)
    dac1_pinmux_config();
#endif

#if defined(CONFIG_ACMP)
    acmp_pinmux_config();
    acmp_configoutput_pinmux();
#endif


#if defined(CONFIG_TIM1)
    /** Configure pinmux for an available TIM1 device */
    tim1_pinmux_config();
#endif

#if defined(CONFIG_TIM2)
    /** Configure pinmux for an available TIM2 device */
    tim2_pinmux_config();
#endif

#if defined(CONFIG_TIM3)
    /** Configure pinmux for an available TIM3 device */
    tim3_pinmux_config();
#endif

#if defined(CONFIG_TIM4)
    /** Configure pinmux for an available TIM4 device */
    tim4_pinmux_config();
#endif

#if defined(CONFIG_TIM5)
    /** Configure pinmux for an available TIM5 device */
    tim5_pinmux_config();
#endif

#if defined(CONFIG_TIM8)
    /** Configure pinmux for an available TIM8 device */
    tim8_pinmux_config();
#endif

#if defined(CONFIG_USB_PINMUX)
    /** Configure pinmux for an available TIM14 device */
    struct device *usb_porth = device_get_binding(CONFIG_PINMUX_GM_PORTH_NAME);
    if (usb_porth != NULL)
    {
        ret = usb_pinmux_config(usb_porth);
    }

#endif

#if defined(CONFIG_CAN)
    ret = can0_pinmux_config();
    if (ret)
    {
        return ret;
    }
#endif

    /* add other ip pinmux */
    return ret;


}



#ifdef CONFIG_DEVICE_POWER_MANAGEMENT

static int gm_pinmux_pm_control(struct device *device, uint32_t command, void *context)
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

            pinmux_gm_initialize(NULL);

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
             *should enable IRQ
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



/*todo this only remove compiler waring that pm_control not call*/
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
DEVICE_DEFINE(gm_pinmux, "pinmux_qfn40pin", pinmux_gm_initialize, gm_pinmux_pm_control, \
              NULL, NULL, PRE_KERNEL_1, DEVICE_PRIORITY_BOARD_PINMUX,  NULL) ;
#else

DEVICE_DEFINE(gm_pinmux, "pinmux_qfn40pin", pinmux_gm_initialize, NULL, \
              NULL, NULL, PRE_KERNEL_1, DEVICE_PRIORITY_BOARD_PINMUX,  NULL) ;

#endif



