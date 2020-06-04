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
 * @brief       BOARD PINMUX source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/


#include <stdio.h>
#include <gm_common.h>
#include <gm_hal_gpio.h>
//#include <gm_hal_spi.h>
//#include <gm_hal_tim.h>
//#include <gm_hal_i2c.h>
#include <uart/gm_ll_uart.h>
#include <gpio/gm_ll_gpio.h>

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

RET_CODE UartPinMuxConfig(UART_Device_T *pUART);
// RET_CODE I2CPinMuxConfig(I2C_Device_T *pI2C);
// RET_CODE ACMP_ConfigPinmux(void);
// RET_CODE acmp_configoutput_pinmux(void);
// RET_CODE SFlash_ConfigPinmux(void);
// RET_CODE SPI_ConfigPinmux(SPI_Device_T *pSPIDev);
// RET_CODE DAC_ConfigPinmux(void);
// RET_CODE LCD_ConfigPinmux(void);
// RET_CODE USB_ConfigPinmux(void);
//RET_CODE TIM_ConfigPinmux(TIM_Device_T *TIMx);
// RET_CODE CAN0_ConfigPinmux(void);
// RET_CODE DAC_ConfigPinmux(void);
// RET_CODE MMC_ConfigPinmux(void);
