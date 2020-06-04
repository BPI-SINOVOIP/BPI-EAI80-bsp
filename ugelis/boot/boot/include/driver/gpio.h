/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gpio.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/07/07
 *
 * @brief         Header file of gpio driver.
 *
 * @note
 *    2015/10/09, wen.liu, V1.0.0
 *        Initial version.
 *    2016/07/07, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GPIO_H_
#define _GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gm_common.h"

typedef __IO uint32_t *GPIO_Pin_T;

/**
  * @brief  GPIO bit set and bit reset enumeration
  */
typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
}GPIO_PinState_T;

/**
  * @brief  GPIO pin direction enumeration
  */
typedef enum
{
    GPIO_PIN_OUTPUT = 0,   /**< pin as outout */
    GPIO_PIN_INPUT,        /**< pin as input  */
} GPIO_PinDir_T;


/** @defgroup GPIO_pins_define GPIO pins define
  * @{
  */
#define GPIO_PIN_0                 ((uint16_t)0x0001)  /**< Pin 0 selected */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /**< Pin 1 selected */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /**< Pin 2 selected */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /**< Pin 3 selected */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /**< Pin 4 selected */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /**< Pin 5 selected */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /**< Pin 6 selected */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /**< Pin 7 selected */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /**< Pin 8 selected */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /**< Pin 9 selected */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /**< Pin 10 selected */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /**< Pin 11 selected */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /**< Pin 12 selected */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /**< Pin 13 selected */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /**< Pin 14 selected */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /**< Pin 15 selected */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /**< All pins selected */

#define GPIO_PIN_MASK              ((uint32_t)0x0000FFFF) /**< PIN mask for assert test */

#define IS_GPIO_PIN(PIN)           (((PIN) & GPIO_PIN_MASK) != (uint32_t)0x00)
/**
  * @}
  */

/** @defgroup GPIO_bit_addr GPIO bit mask address define
  * @{
  */
#define GPIOA0    GPIO_BIT_ADDR(0)       /**< GPIO pin 00 */
#define GPIOA1    GPIO_BIT_ADDR(1)       /**< GPIO pin 01 */
#define GPIOA2    GPIO_BIT_ADDR(2)       /**< GPIO pin 02 */
#define GPIOA3    GPIO_BIT_ADDR(3)       /**< GPIO pin 03 */
#define GPIOA4    GPIO_BIT_ADDR(4)       /**< GPIO pin 04 */
#define GPIOA5    GPIO_BIT_ADDR(5)       /**< GPIO pin 05 */
#define GPIOA6    GPIO_BIT_ADDR(6)       /**< GPIO pin 06 */
#define GPIOA7    GPIO_BIT_ADDR(7)       /**< GPIO pin 07 */
#define GPIOA8    GPIO_BIT_ADDR(8)       /**< GPIO pin 08 */
#define GPIOA9    GPIO_BIT_ADDR(9)       /**< GPIO pin 09 */
#define GPIOA10   GPIO_BIT_ADDR(10)      /**< GPIO pin 10 */
#define GPIOA11   GPIO_BIT_ADDR(11)      /**< GPIO pin 11 */
#define GPIOA12   GPIO_BIT_ADDR(12)      /**< GPIO pin 12 */
#define GPIOA13   GPIO_BIT_ADDR(13)      /**< GPIO pin 13 */
#define GPIOA14   GPIO_BIT_ADDR(14)      /**< GPIO pin 14 */
#define GPIOA15   GPIO_BIT_ADDR(15)      /**< GPIO pin 15 */
#define GPIOB0    GPIO_BIT_ADDR(16)      /**< GPIO pin 16 */
#define GPIOB1    GPIO_BIT_ADDR(17)      /**< GPIO pin 17 */
#define GPIOB2    GPIO_BIT_ADDR(18)      /**< GPIO Pin 18 */
#define GPIOB3    GPIO_BIT_ADDR(19)      /**< GPIO pin 19 */
#define GPIOB4    GPIO_BIT_ADDR(20)      /**< GPIO pin 20 */
#define GPIOB5    GPIO_BIT_ADDR(21)      /**< GPIO pin 21 */
#define GPIOB6    GPIO_BIT_ADDR(22)      /**< GPIO pin 22 */
#define GPIOB7    GPIO_BIT_ADDR(23)      /**< GPIO pin 23 */
#define GPIOB8    GPIO_BIT_ADDR(24)      /**< GPIO pin 24 */
#define GPIOB9    GPIO_BIT_ADDR(25)      /**< GPIO pin 25 */
#define GPIOB10   GPIO_BIT_ADDR(26)      /**< GPIO pin 26 */
#define GPIOB11   GPIO_BIT_ADDR(27)      /**< GPIO pin 27 */
#define GPIOB12   GPIO_BIT_ADDR(28)      /**< GPIO pin 28 */
#define GPIOB13   GPIO_BIT_ADDR(29)      /**< GPIO pin 29 */
#define GPIOB14   GPIO_BIT_ADDR(30)      /**< GPIO pin 30 */
#define GPIOB15   GPIO_BIT_ADDR(31)      /**< GPIO pin 31 */

#define GPIOC0    GPIO_BIT_ADDR(32)      /**< GPIO pin 32 */
#define GPIOC1    GPIO_BIT_ADDR(33)      /**< GPIO pin 33 */
#define GPIOC2    GPIO_BIT_ADDR(34)      /**< GPIO pin 34 */
#define GPIOC3    GPIO_BIT_ADDR(35)      /**< GPIO pin 35 */
#define GPIOC4    GPIO_BIT_ADDR(36)      /**< GPIO pin 36 */
#define GPIOC5    GPIO_BIT_ADDR(37)      /**< GPIO pin 37 */
#define GPIOC6    GPIO_BIT_ADDR(38)      /**< GPIO pin 38 */
#define GPIOC7    GPIO_BIT_ADDR(39)      /**< GPIO pin 39 */
#define GPIOC8    GPIO_BIT_ADDR(40)      /**< GPIO pin 40 */
#define GPIOC9    GPIO_BIT_ADDR(41)      /**< GPIO pin 41 */
#define GPIOC10   GPIO_BIT_ADDR(42)      /**< GPIO pin 42 */
#define GPIOC11   GPIO_BIT_ADDR(43)      /**< GPIO pin 43 */
#define GPIOC12   GPIO_BIT_ADDR(44)      /**< GPIO pin 44 */
#define GPIOC13   GPIO_BIT_ADDR(45)      /**< GPIO pin 45 */
#define GPIOC14   GPIO_BIT_ADDR(46)      /**< GPIO pin 46 */
#define GPIOC15   GPIO_BIT_ADDR(47)      /**< GPIO pin 47 */

#define GPIOD0    GPIO_BIT_ADDR(48)      /**< GPIO pin 48 */
#define GPIOD1    GPIO_BIT_ADDR(49)      /**< GPIO pin 49 */
#define GPIOD2    GPIO_BIT_ADDR(50)      /**< GPIO pin 50 */
#define GPIOD3    GPIO_BIT_ADDR(51)      /**< GPIO pin 51 */
#define GPIOD4    GPIO_BIT_ADDR(52)      /**< GPIO pin 52 */
#define GPIOD5    GPIO_BIT_ADDR(53)      /**< GPIO pin 53 */
#define GPIOD6    GPIO_BIT_ADDR(54)      /**< GPIO pin 54 */
#define GPIOD7    GPIO_BIT_ADDR(55)      /**< GPIO pin 55 */
#define GPIOD8    GPIO_BIT_ADDR(56)      /**< GPIO pin 56 */
#define GPIOD9    GPIO_BIT_ADDR(57)      /**< GPIO pin 57 */
#define GPIOD10   GPIO_BIT_ADDR(58)      /**< GPIO pin 58 */
#define GPIOD11   GPIO_BIT_ADDR(59)      /**< GPIO pin 59 */
#define GPIOD12   GPIO_BIT_ADDR(60)      /**< GPIO pin 60 */
#define GPIOD13   GPIO_BIT_ADDR(61)      /**< GPIO pin 61 */
#define GPIOD14   GPIO_BIT_ADDR(62)      /**< GPIO pin 62 */
#define GPIOD15   GPIO_BIT_ADDR(63)      /**< GPIO pin 63 */

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define GPIOE0    GPIO_BIT_ADDR(64)      /**< GPIO pin 64 */
#define GPIOE1    GPIO_BIT_ADDR(65)      /**< GPIO pin 65 */
#define GPIOE2    GPIO_BIT_ADDR(66)      /**< GPIO pin 66 */
#define GPIOE3    GPIO_BIT_ADDR(67)      /**< GPIO pin 67 */
#define GPIOE4    GPIO_BIT_ADDR(68)      /**< GPIO pin 68 */
#define GPIOE5    GPIO_BIT_ADDR(69)      /**< GPIO pin 69 */
#define GPIOE6    GPIO_BIT_ADDR(70)      /**< GPIO pin 70 */
#define GPIOE7    GPIO_BIT_ADDR(71)      /**< GPIO pin 71 */
#define GPIOE8    GPIO_BIT_ADDR(72)      /**< GPIO pin 72 */
#define GPIOE9    GPIO_BIT_ADDR(73)      /**< GPIO pin 73 */
#define GPIOE10   GPIO_BIT_ADDR(74)      /**< GPIO pin 74 */
#define GPIOE11   GPIO_BIT_ADDR(75)      /**< GPIO pin 75 */
#define GPIOE12   GPIO_BIT_ADDR(76)      /**< GPIO pin 76 */
#define GPIOE13   GPIO_BIT_ADDR(77)      /**< GPIO pin 77 */
#define GPIOE14   GPIO_BIT_ADDR(78)      /**< GPIO pin 78 */
#define GPIOE15   GPIO_BIT_ADDR(79)      /**< GPIO pin 79 */
#endif

#if defined(CHIP_GM6611B)
#define GPIOF0   GPIO_BIT_ADDR(80)      /**< GPIO pin 80 */
#define GPIOF1   GPIO_BIT_ADDR(81)      /**< GPIO pin 81 */
#define GPIOF2   GPIO_BIT_ADDR(82)      /**< GPIO pin 82 */
#endif
/**
  * @}
  */

/** @defgroup GPIOEx_alternate_function_selection GPIO extended alternate function selection
  * @brief alternate function mapping
  *        Elements values convention: 0xX00000yz
  *           - X  : 1 -- PINMUX_CTRL0 resistor; 2 -- PINMUX_CTRL1 register; 4 -- PINMUX_CTRL2 register
  *           - y,z  : Register bit number
  * @{
  */
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define GPIOA0_AF1_CANTX        ((uint32_t)0x20000000)  /**< CAN TX alternate function mapping */
#define GPIOA1_AF1_CANDBG       ((uint32_t)0x20000001)  /**< CAN DBG alternate function mapping */

#define GPIOA2_AF0_I2C2CLK      ((uint32_t)0x10000000)  /**< I2C2 CLOCK alternate function mapping */
#define GPIOA2_AF1_UART2RX      ((uint32_t)0x20000002)  /**< UART2 RX alternate function mapping */

#define GPIOA3_AF0_I2C2SDA      ((uint32_t)0x10000000)  /**< I2C2 SDA alternate function mapping */
#define GPIOA3_AF1_UART2TX      ((uint32_t)0x20000002)  /**< UART2 TX alternate function mapping */

#define GPIOA4_AF0_SPI1MISO     ((uint32_t)0x10000001)  /**< SPI1 MISO alternate function mapping */
#define GPIOA4_AF1_SPI1SS1      ((uint32_t)0x20000003)  /**< SPI1 SS1 alternate function mapping */
#define GPIOA4_AF1_ETM3EXTCLK   ((uint32_t)0x20000018)  /**< ETM3 EXT CLK alternate function mapping */

#define GPIOA5_AF0_SPI1SS0      ((uint32_t)0x10000001)  /**< SPI1 SS0 alternate function mapping */
#define GPIOA5_AF1_SPI1MOSI     ((uint32_t)0x20000004)  /**< SPI1 MOSI alternate function mapping */
#define GPIOA5_AF1_ETM3TC       ((uint32_t)0x20000019)  /**< ETM3 TC IN alternate function mapping */

#define GPIOA6_AF0_SPI1SCK      ((uint32_t)0x10000001)  /**< SPI1 SCK alternate function mapping */
#define GPIOA6_AF1_SPI1SCK      ((uint32_t)0x20000004)  /**< SPI1 SCK alternate function mapping */
#define GPIOA6_AF1_ETM3CH1      ((uint32_t)0x2000001A)  /**< ETM3 CH1 alternate function mapping */

#define GPIOA7_AF0_SPI1MOSI     ((uint32_t)0x10000001)  /**< SPI1 MOSI alternate function mapping */
#define GPIOA7_AF1_SPI1SS0      ((uint32_t)0x20000004)  /**< SPI1 SS0 alternate function mapping */
#define GPIOA7_AF1_ETM3CH2      ((uint32_t)0x2000001B)  /**< ETM3 CH2 alternate function mapping */

#define GPIOA8_AF0_SPI1SS1      ((uint32_t)0x10000002)  /**< SPI1 SS1 alternate function mapping */
#define GPIOA8_AF1_SPI1MISO     ((uint32_t)0x20000004)  /**< SPI1 MISO alternate function mapping */

#define GPIOA9_AF0_ETM3EXTCLK   ((uint32_t)0x10000003)  /**< ETM3 EXT CLK alternate function mapping */
#define GPIOA9_AF1_SPI0MISO     ((uint32_t)0x2000000C)  /**< SPI0 MISO alternate function mapping */

#define GPIOA10_AF0_ETM3TC      ((uint32_t)0x10000004)  /**< ETM3 TC IN alternate function mapping */
#define GPIOA10_AF1_SPI0SS0     ((uint32_t)0x2000000C)  /**< SPI0 SS0 alternate function mapping */

#define GPIOA11_AF0_ETM3CH1     ((uint32_t)0x10000005)  /**< ETM3 CH1 alternate function mapping */
#define GPIOA11_AF1_SPI0SCK     ((uint32_t)0x2000000C)  /**< SPI0 SCK alternate function mapping */

#define GPIOA12_AF0_ETM3CH2     ((uint32_t)0x10000006)  /**< ETM3 CH2 alternate function mapping */
#define GPIOA12_AF1_SPI0MOSI    ((uint32_t)0x2000000C)  /**< SPI0 MOSI alternate function mapping */

#define GPIOA13_AF1_SPI0SS1     ((uint32_t)0x2000000D)  /**< SPI0 SS1 alternate function mapping */

#define GPIOA15_AF0_ACMP        ((uint32_t)0x10000007)  /**< ACMP OUT alternate function mapping */
#define GPIOB0_AF0_IR           ((uint32_t)0x10000008)  /**< IR alternate function mapping */

#define GPIOB1_AF0_UART2RX      ((uint32_t)0x10000009)  /**< UART2 RX alternate function mapping */
#define GPIOB1_AF1_I2C2SCL      ((uint32_t)0x20000005)  /**< I2C2 SCL alternate function mapping */

#define GPIOB2_AF0_UART2TX      ((uint32_t)0x10000009)  /**< UART2 TX alternate function mapping */
#define GPIOB2_AF1_I2C2SDA      ((uint32_t)0x20000005)  /**< I2C2 SDA alternate function mapping */

#define GPIOC10_AF0_I2C0SCL     ((uint32_t)0x1000000A)  /**< I2C0 SCL alternate function mapping */
#define GPIOC10_AF1_UART0RX     ((uint32_t)0x20000006)  /**< UART0 RX alternate function mapping */

#define GPIOC11_AF0_I2C0SDA     ((uint32_t)0x1000000A)  /**< I2C0 SDA alternate function mapping */
#define GPIOC11_AF1_UART0TX     ((uint32_t)0x20000006)  /**< UART0 TX alternate function mapping */

#define GPIOC12_AF0_UART0RX     ((uint32_t)0x1000000B)  /**< UART0 RX alternate function mapping */
#define GPIOC12_AF1_I2C0SCL     ((uint32_t)0x20000007)  /**< I2C0 SCL alternate function mapping */

#define GPIOC13_AF0_UART0TX     ((uint32_t)0x1000000B)  /**< UART0 TX alternate function mapping */
#define GPIOC13_AF1_I2C0SDA     ((uint32_t)0x20000007)  /**< I2C0 SDA alternate function mapping */

#define GPIOC14_AF0_CANRX       ((uint32_t)0x1000000C)  /**< CAN RX alternate function mapping */
#define GPIOC14_AF1_ETM0EXTCLK  ((uint32_t)0x2000001C)  /**< ETM0 EXT CLK alternate function mapping */

#define GPIOC15_AF0_CANTX       ((uint32_t)0x1000000C)  /**< CAN TX alternate function mapping */
#define GPIOC15_AF1_ETM0TC      ((uint32_t)0x2000001D)  /**< ETM0 TC IN alternate function mapping */

#define GPIOD0_AF0_CANDBG       ((uint32_t)0x1000000D)  /**< CAN DBG alternate function mapping */

#define GPIOD1_AF0_ETM0EXTCLK   ((uint32_t)0x1000000E)  /**< ETM0 EXT CLK alternate function mapping */
#define GPIOD2_AF0_ETM0TC       ((uint32_t)0x1000000F)  /**< ETM0 TC IN alternate function mapping */
#define GPIOD3_AF0_ETM0CH1      ((uint32_t)0x10000010)  /**< ETM0 CH1 alternate function mapping */
#define GPIOD4_AF0_ETM0CH2      ((uint32_t)0x10000011)  /**< ETM0 CH2 alternate function mapping */

#define GPIOD5_AF0_I2C1SCL      ((uint32_t)0x10000012)  /**< I2C1 SCL alternate function mapping */
#define GPIOD5_AF1_UART1RX      ((uint32_t)0x20000008)  /**< UART1 RX alternate function mapping */
#define GPIOD5_AF1_ETM1EXTCLK   ((uint32_t)0x2000000E)  /**< ETM1 EXT CLK alternate function mapping */
#define GPIOD5_AF1_ETM0CH1      ((uint32_t)0x2000001E)  /**< ETM0 CH1 alternate function mapping */

#define GPIOD6_AF0_I2C1SDA      ((uint32_t)0x10000012)  /**< I2C1 SDA alternate function mapping */
#define GPIOD6_AF1_UART1TX      ((uint32_t)0x20000008)  /**< UART1 TX alternate function mapping */
#define GPIOD6_AF1_ETM1TC       ((uint32_t)0x2000000F)  /**< ETM1 TC IN alternate function mapping */
#define GPIOD6_AF1_ETM0CH2      ((uint32_t)0x2000001F)  /**< ETM0 CH2 alternate function mapping */

#define GPIOD7_AF1_ETM1CH1      ((uint32_t)0x20000010)  /**< ETM1 CH1 alternate function mapping */
#define GPIOD8_AF1_ETM1CH2      ((uint32_t)0x20000011)  /**< ETM1 CH2 alternate function mapping */

#define GPIOD10_AF2_SWDCLK      ((uint32_t)0x40000000)  /**< SWD CLK alternate function mapping */
#define GPIOD11_AF2_SWDDIO      ((uint32_t)0x40000000)  /**< SWD Data alternate function mapping */

#define GPIOD12_AF0_NMIJ        ((uint32_t)0x10000014)  /**< NMIJ alternate function mapping */

#define GPIOD13_AF0_UART1RX     ((uint32_t)0x10000015)  /**< UART1 RX alternate function mapping */
#define GPIOD13_AF1_I2C1SCL     ((uint32_t)0x20000009)  /**< I2C1 SCL alternate function mapping */

#define GPIOD14_AF0_UART1TX     ((uint32_t)0x10000015)  /**< UART1 TX alternate function mapping */
#define GPIOD14_AF1_I2C1SDA     ((uint32_t)0x20000009)  /**< I2C1 SDA alternate function mapping */

#define GPIOE0_AF0_ETM1EXTCLK   ((uint32_t)0x10000016)  /**< ETM1 EXT CLK alternate function mapping */
#define GPIOE1_AF0_ETM1TC       ((uint32_t)0x10000017)  /**< ETM1 TC IN alternate function mapping */
#define GPIOE2_AF0_ETM1CH1      ((uint32_t)0x10000018)  /**< ETM1 CH1 alternate function mapping */
#define GPIOE3_AF0_ETM1CH2      ((uint32_t)0x10000019)  /**< ETM1 CH2 alternate function mapping */

#define GPIOE4_AF0_SPI0MISO     ((uint32_t)0x1000001A)  /**< SPI0 MISO alternate function mapping */
#define GPIOE4_AF1_SPI0SS1      ((uint32_t)0x2000000A)  /**< SPI0 SS1 alternate function mapping */
#define GPIOE4_AF1_ETM2EXTCLK   ((uint32_t)0x20000012)  /**< ETM2 EXT CLK alternate function mapping */

#define GPIOE5_AF0_SPI0SS0      ((uint32_t)0x1000001A)  /**< SPI0 SS0 alternate function mapping */
#define GPIOE5_AF1_SPI0MOSI     ((uint32_t)0x2000000B)  /**< SPI0 MOSI alternate function mapping */
#define GPIOE5_AF1_ETM2TC       ((uint32_t)0x20000013)  /**< ETM2 TC IN alternate function mapping */

#define GPIOE6_AF0_SPI0SCK      ((uint32_t)0x1000001A)  /**< SPI0 SCK alternate function mapping */
#define GPIOE6_AF1_SPI0SCK      ((uint32_t)0x2000000B)  /**< SPI0 SCK alternate function mapping */
#define GPIOE6_AF1_ETM2CH1      ((uint32_t)0x20000014)  /**< ETM2 CH1 alternate function mapping */

#define GPIOE7_AF0_SPI0MOSI     ((uint32_t)0x1000001A)  /**< SPI0 MOSI alternate function mapping */
#define GPIOE7_AF1_SPI0SS0      ((uint32_t)0x2000000B)  /**< SPI0 SS0 alternate function mapping */
#define GPIOE7_AF1_ETM2CH2      ((uint32_t)0x20000015)  /**< ETM2 CH2 alternate function mapping */

#define GPIOE8_AF0_SPI0SS1      ((uint32_t)0x1000001B)  /**< SPI0 SS1 alternate function mapping */
#define GPIOE8_AF1_SPI0MISO     ((uint32_t)0x2000000B)  /**< SPI0 MISO alternate function mapping */

#define GPIOE10_AF0_ETM2EXTCLK  ((uint32_t)0x1000001C)  /**< ETM2 EXT CLK alternate function mapping */
#define GPIOE10_AF1_SPI1MISO    ((uint32_t)0x20000016)  /**< SPI1 MISO alternate function mapping */

#define GPIOE11_AF0_ETM2TC      ((uint32_t)0x1000001D)  /**< ETM2 TC IN alternate function mapping */
#define GPIOE11_AF1_SPI1SS0     ((uint32_t)0x20000016)  /**< SPI1 SS0 alternate function mapping */

#define GPIOE12_AF0_ETM2CH1     ((uint32_t)0x1000001E)  /**< ETM2 CH1 alternate function mapping */
#define GPIOE12_AF1_SPI1SCK     ((uint32_t)0x20000016)  /**< SPI1 SCK alternate function mapping */

#define GPIOE13_AF0_ETM2CH2     ((uint32_t)0x1000001F)  /**< ETM2 CH2 alternate function mapping */
#define GPIOE13_AF1_SPI1MOSI    ((uint32_t)0x20000016)  /**< SPI1 MOSI alternate function mapping */

#define GPIOE14_AF1_SPI1SS1     ((uint32_t)0x20000017)  /**< SPI1 SS1 alternate function mapping */

#define GPIOE15_AF1_CANTX       ((uint32_t)0x20000000)  /**< CAN TX alternate function mapping */
#define GPIOE15_AF2_TSTCLK      ((uint32_t)0x40000008)  /**< source clocks output alternate function mapping */

#define GPIOF1_AF2_ETMACH1      ((uint32_t)0x40000009)  /**< ETMA_CH1 alternate function mapping */
#define GPIOF1_AF2_UART0RX      ((uint32_t)0x4000000B)  /**< UART0 RX alternate function mapping */
#define GPIOF1_AF2_I2C0CLK      ((uint32_t)0x4000000C)  /**< I2C0 CLK alternate function mapping */

#define GPIOF2_AF2_ETMACH2       ((uint32_t)0x4000000A) /**< ETMA_CH2 alternate function mapping */
#define GPIOF2_AF2_UART0TX       ((uint32_t)0x4000000B) /**< UART0 TX alternate function mapping */
#define GPIOF2_AF2_I2C0DATA      ((uint32_t)0x4000000C) /**< I2C0 DATA alternate function mapping */
#endif

#if (defined(CHIP_GM6601))
#define GPIOA2_AF0_UART2TX      ((uint32_t)0x10000010)  /**< UART2 TX alternate function mapping */
#define GPIOA2_AF0_UART2RX      ((uint32_t)0x10000011)  /**< UART2 RX alternate function mapping */

#define GPIOA3_AF0_UART2RX      ((uint32_t)0x10000010)  /**< UART2 RX alternate function mapping */
#define GPIOA3_AF0_UART2TX      ((uint32_t)0x10000011)  /**< UART2 TX alternate function mapping */

#define GPIOA7_AF0_ETM3CH1      ((uint32_t)0x10000003)  /**< ETM3 CH1 alternate function mapping */

#define GPIOA8_AF0_NMIJ         ((uint32_t)0x1000001E)  /**< NMIJ alternate function mapping */
#define GPIOA8_AF0_IR           ((uint32_t)0x1000001B)  /**< IR alternate function mapping */

#define GPIOA9_AF0_ETM3CH2      ((uint32_t)0x10000003)  /**< ETM3 CH2 alternate function mapping */

#define GPIOA10_AF0_I2C1SDA     ((uint32_t)0x10000016)  /**< I2C1 SDA alternate function mapping */
#define GPIOA10_AF0_UART2TX     ((uint32_t)0x10000012)  /**< UART2 TX alternate function mapping */

#define GPIOA11_AF0_I2C1SCL     ((uint32_t)0x10000015)  /**< I2C1 SCL alternate function mapping */
#define GPIOA11_AF0_UART2RX     ((uint32_t)0x10000012)  /**< UART2 RX alternate function mapping */
#define GPIOA11_AF0_SPICSJ0     ((uint32_t)0x10000004)  /**< SPI CSJ0 alternate function mapping */

#define GPIOA12_AF0_SWDCLK      ((uint32_t)0x1000001F)  /**< SWD CLK alternate function mapping */
#define GPIOA12_AF0_UART0TX     ((uint32_t)0x10000006)  /**< UART0 TX alternate function mapping */
#define GPIOA12_AF0_UART0RX     ((uint32_t)0x10000008)  /**< UART0 RX alternate function mapping */

#define GPIOA13_AF0_SWDDIO      ((uint32_t)0x1000001F)  /**< SWD Data alternate function mapping */
#define GPIOA13_AF0_UART0RX     ((uint32_t)0x10000006)  /**< UART0 RX alternate function mapping */
#define GPIOA13_AF0_UART0TX     ((uint32_t)0x10000008)  /**< UART0 TX alternate function mapping */
#define GPIOA13_AF0_SPICSJ1     ((uint32_t)0x10000005)  /**< SPI CSJ1 alternate function mapping */

#define GPIOA14_AF0_SPICLK      ((uint32_t)0x1000001D)  /**< SPI CLK alternate function mapping */

#define GPIOA15_AF0_I2C0SDA     ((uint32_t)0x10000014)  /**< I2C0 SDA alternate function mapping */
#define GPIOA15_AF0_SPIMISO     ((uint32_t)0x1000001D)  /**< SPI MISO alternate function mapping */

#define GPIOB0_AF0_I2C0SCL      ((uint32_t)0x10000013)  /**< I2C0 SCL alternate function mapping */
#define GPIOB0_AF0_SPIMOSI      ((uint32_t)0x1000001D)  /**< SPI MOSI alternate function mapping */

#define GPIOB1_AF0_ETM1EXTCLK   ((uint32_t)0x10000001)  /**< ETM1 EXT CLK alternate function mapping */
#define GPIOB1_AF0_SPICSJ0      ((uint32_t)0x1000000B)  /**< SPI CSJ0 alternate function mapping */

#define GPIOB2_AF0_ETM1TCIN     ((uint32_t)0x10000001)  /**< ETM1 TC IN alternate function mapping */
#define GPIOB2_AF0_SPICSJ1      ((uint32_t)0x1000000E)  /**< SPI CSJ1 alternate function mapping */

#define GPIOB3_AF0_ETM1CH1      ((uint32_t)0x10000001)  /**< ETM1 CH1 alternate function mapping */
#define GPIOB4_AF0_ETM1CH2      ((uint32_t)0x10000001)  /**< ETM1 CH2 alternate function mapping */

#define GPIOC5_AF0_ETM0EXTCLK   ((uint32_t)0x10000000)  /**< ETM0 EXT CLK alternate function mapping */
#define GPIOC6_AF0_ETM0TCIN     ((uint32_t)0x10000000)  /**< ETM0 TC IN alternate function mapping */

#define GPIOC7_AF0_ETM0CH1      ((uint32_t)0x10000000)  /**< ETM0 CH1 alternate function mapping */
#define GPIOC8_AF0_ETM0CH2      ((uint32_t)0x10000000)  /**< ETM0 CH2 alternate function mapping */

#define GPIOC9_AF0_UART1TX      ((uint32_t)0x1000000A)  /**< UART1 TX alternate function mapping */
#define GPIOC9_AF0_UART1RX      ((uint32_t)0x1000000D)  /**< UART1 RX alternate function mapping */
#define GPIOC9_AF0_ETM4EXTCLK   ((uint32_t)0x10000019)  /**< ETM4 EXT CLK alternate function mapping */

#define GPIOC10_AF0_UART1RX     ((uint32_t)0x1000000A)  /**< UART1 RX alternate function mapping */
#define GPIOC10_AF0_UART1TX     ((uint32_t)0x1000000D)  /**< UART1 TX alternate function mapping */
#define GPIOC10_AF0_ETM4TCIN    ((uint32_t)0x10000019)  /**< ETM4 TC IN alternate function mapping */

#define GPIOC11_AF0_ETM4CH1     ((uint32_t)0x10000019)  /**< ETM4 CH1 alternate function mapping */
#define GPIOC11_AF1_ETM2CH1     ((uint32_t)0x20000002)  /**< ETM2 CH1 alternate function mapping */

#define GPIOC12_AF0_ETM5CH1     ((uint32_t)0x1000001A)  /**< ETM5 CH1 alternate function mapping */
#define GPIOC12_AF1_ETM3CH1     ((uint32_t)0x20000003)  /**< ETM3 CH1 alternate function mapping */

#define GPIOC13_AF0_ETM2EXTCLK  ((uint32_t)0x10000002)  /**< ETM2 EXT CLK alternate function mapping */
#define GPIOC13_AF1_ETM0CH1     ((uint32_t)0x20000000)  /**< ETM0 CH1 alternate function mapping */

#define GPIOC14_AF0_ETM2TCIN    ((uint32_t)0x10000002)  /**< ETM2 TC IN alternate function mapping */
#define GPIOC14_AF1_ETM1CH1     ((uint32_t)0x20000001)  /**< ETM1 CH1 alternate function mapping */

#define GPIOC15_AF0_ETM2CH1     ((uint32_t)0x10000002)  /**< ETM2 CH1 alternate function mapping */
#define GPIOC15_AF1_ETM4CH1     ((uint32_t)0x20000004)  /**< ETM4 CH1 alternate function mapping */

#define GPIOD0_AF0_ETM2CH2      ((uint32_t)0x10000002)  /**< ETM2 CH2 alternate function mapping */
#define GPIOD0_AF1_ETM5CH1      ((uint32_t)0x20000005)  /**< ETM4 CH1 alternate function mapping */

#define GPIOD4_AF0_I2C2SDA      ((uint32_t)0x10000017)  /**< I2C2 SDA alternate function mapping */
#define GPIOD4_AF0_UART1TX      ((uint32_t)0x1000000C)  /**< UART1 TX alternate function mapping */
#define GPIOD4_AF0_UART1RX      ((uint32_t)0x1000000F)  /**< UART1 RX alternate function mapping */
#define GPIOD4_AF0_ETM5EXTCLK   ((uint32_t)0x1000001A)  /**< ETM5 EXT CLK alternate function mapping */

#define GPIOD5_AF0_I2C2SCL      ((uint32_t)0x10000018)  /**< I2C2 SCL alternate function mapping */
#define GPIOD5_AF0_UART1RX      ((uint32_t)0x1000000C)  /**< UART1 RX alternate function mapping */
#define GPIOD5_AF0_UART1TX      ((uint32_t)0x1000000F)  /**< UART1 TX alternate function mapping */
#define GPIOD5_AF0_ETM5TCIN     ((uint32_t)0x1000001A)  /**< ETM5 TC IN alternate function mapping */

#define GPIOD6_AF0_UART0TX      ((uint32_t)0x10000006)  /**< UART0 TX alternate function mapping */
#define GPIOD6_AF0_UART0RX      ((uint32_t)0x10000009)  /**< UART0 RX alternate function mapping */

#define GPIOD7_AF0_UART0RX      ((uint32_t)0x10000007)  /**< UART0 RX alternate function mapping */
#define GPIOD7_AF0_UART0TX      ((uint32_t)0x10000009)  /**< UART0 TX alternate function mapping */

#define GPIOD8_AF0_ETM3EXTCLK   ((uint32_t)0x10000003)  /**< ETM3 EXT CLK alternate function mapping */
#define GPIOD8_AF0_ACMP         ((uint32_t)0x1000001C)  /**< ACMP OUT alternate function mapping */
#define GPIOD8_AF1_PORRSTJ      ((uint32_t)0x20000007)  /**< POR reset status alternate function mapping */

#define GPIOD9_AF0_ETM3TCIN     ((uint32_t)0x10000003)  /**< ETM3 TC IN alternate function mapping */
#define GPIOD9_AF1_TSTCLK       ((uint32_t)0x20000006)  /**< source clocks output alternate function mapping */
#endif

#if (defined(CHIP_GM6601B))
#define GPIOA2_AF0_UART2TX      ((uint32_t)0x10000010)  /**< UART2 TX alternate function mapping */
#define GPIOA2_AF0_UART2RX      ((uint32_t)0x10000011)  /**< UART2 RX alternate function mapping */

#define GPIOA3_AF0_UART2RX      ((uint32_t)0x10000010)  /**< UART2 RX alternate function mapping */
#define GPIOA3_AF0_UART2TX      ((uint32_t)0x10000011)  /**< UART2 TX alternate function mapping */

#define GPIOA7_AF0_ETM3CH1      ((uint32_t)0x10000003)  /**< ETM3 CH1 alternate function mapping */

#define GPIOA8_AF0_NMIJ         ((uint32_t)0x1000001E)  /**< NMIJ alternate function mapping */
#define GPIOA8_AF0_IR           ((uint32_t)0x1000001B)  /**< IR alternate function mapping */

#define GPIOA9_AF0_ETM3CH2      ((uint32_t)0x10000003)  /**< ETM3 CH2 alternate function mapping */

#define GPIOA10_AF0_I2C1SDA     ((uint32_t)0x10000016)  /**< I2C1 SDA alternate function mapping */
#define GPIOA10_AF0_UART2TX     ((uint32_t)0x10000012)  /**< UART2 TX alternate function mapping */

#define GPIOA11_AF0_I2C1SCL     ((uint32_t)0x10000015)  /**< I2C1 SCL alternate function mapping */
#define GPIOA11_AF1_UART2RX     ((uint32_t)0x20000008)  /**< UART2 RX alternate function mapping */
#define GPIOA11_AF0_SPICSJ0     ((uint32_t)0x10000004)  /**< SPI CSJ0 alternate function mapping */

#define GPIOA12_AF2_SWDCLK      ((uint32_t)0x40000000)  /**< SWD CLK alternate function mapping */
#define GPIOA12_AF0_UART0TX     ((uint32_t)0x10000006)  /**< UART0 TX alternate function mapping */
#define GPIOA12_AF0_UART0RX     ((uint32_t)0x10000008)  /**< UART0 RX alternate function mapping */

#define GPIOA13_AF2_SWDDIO      ((uint32_t)0x40000000)  /**< SWD Data alternate function mapping */
#define GPIOA13_AF1_UART0RX     ((uint32_t)0x20000009)  /**< UART0 RX alternate function mapping */
#define GPIOA13_AF1_UART0TX     ((uint32_t)0x2000000A)  /**< UART0 TX alternate function mapping */
#define GPIOA13_AF0_SPICSJ1     ((uint32_t)0x10000005)  /**< SPI CSJ1 alternate function mapping */

#define GPIOA14_AF0_SPICLK      ((uint32_t)0x1000001D)  /**< SPI CLK alternate function mapping */

#define GPIOA15_AF0_I2C0SDA     ((uint32_t)0x10000014)  /**< I2C0 SDA alternate function mapping */
#define GPIOA15_AF0_SPIMISO     ((uint32_t)0x1000001D)  /**< SPI MISO alternate function mapping */

#define GPIOB0_AF0_I2C0SCL      ((uint32_t)0x10000013)  /**< I2C0 SCL alternate function mapping */
#define GPIOB0_AF0_SPIMOSI      ((uint32_t)0x1000001D)  /**< SPI MOSI alternate function mapping */

#define GPIOB1_AF1_ETM1EXTCLK   ((uint32_t)0x2000000B)  /**< ETM1 EXT CLK alternate function mapping */
#define GPIOB1_AF0_SPICSJ0      ((uint32_t)0x1000000B)  /**< SPI CSJ0 alternate function mapping */

#define GPIOB2_AF1_ETM1TCIN     ((uint32_t)0x2000000C)  /**< ETM1 TC IN alternate function mapping */
#define GPIOB2_AF0_SPICSJ1      ((uint32_t)0x1000000E)  /**< SPI CSJ1 alternate function mapping */

#define GPIOB3_AF0_ETM1CH1      ((uint32_t)0x10000001)  /**< ETM1 CH1 alternate function mapping */
#define GPIOB4_AF0_ETM1CH2      ((uint32_t)0x10000001)  /**< ETM1 CH2 alternate function mapping */

#define GPIOC5_AF0_ETM0EXTCLK   ((uint32_t)0x10000000)  /**< ETM0 EXT CLK alternate function mapping */
#define GPIOC6_AF0_ETM0TCIN     ((uint32_t)0x10000000)  /**< ETM0 TC IN alternate function mapping */

#define GPIOC7_AF0_ETM0CH1      ((uint32_t)0x10000000)  /**< ETM0 CH1 alternate function mapping */
#define GPIOC8_AF0_ETM0CH2      ((uint32_t)0x10000000)  /**< ETM0 CH2 alternate function mapping */

#define GPIOC9_AF0_UART1TX      ((uint32_t)0x1000000A)  /**< UART1 TX alternate function mapping */
#define GPIOC9_AF0_UART1RX      ((uint32_t)0x1000000D)  /**< UART1 RX alternate function mapping */
#define GPIOC9_AF1_ETM4EXTCLK   ((uint32_t)0x20000014)  /**< ETM4 EXT CLK alternate function mapping */

#define GPIOC10_AF0_UART1RX     ((uint32_t)0x1000000A)  /**< UART1 RX alternate function mapping */
#define GPIOC10_AF0_UART1TX     ((uint32_t)0x1000000D)  /**< UART1 TX alternate function mapping */
#define GPIOC10_AF1_ETM4TCIN    ((uint32_t)0x2000000D)  /**< ETM4 TC IN alternate function mapping */

#define GPIOC11_AF0_ETM4CH1     ((uint32_t)0x10000019)  /**< ETM4 CH1 alternate function mapping */
#define GPIOC11_AF1_ETM2CH1     ((uint32_t)0x20000002)  /**< ETM2 CH1 alternate function mapping */

#define GPIOC12_AF0_ETM5CH1     ((uint32_t)0x1000001A)  /**< ETM5 CH1 alternate function mapping */
#define GPIOC12_AF1_ETM3CH1     ((uint32_t)0x20000003)  /**< ETM3 CH1 alternate function mapping */

#define GPIOC13_AF1_ETM2EXTCLK  ((uint32_t)0x2000000E)  /**< ETM2 EXT CLK alternate function mapping */
#define GPIOC13_AF1_ETM0CH1     ((uint32_t)0x20000000)  /**< ETM0 CH1 alternate function mapping */

#define GPIOC14_AF1_ETM2TCIN    ((uint32_t)0x2000000F)  /**< ETM2 TC IN alternate function mapping */
#define GPIOC14_AF1_ETM1CH1     ((uint32_t)0x20000001)  /**< ETM1 CH1 alternate function mapping */

#define GPIOC15_AF0_ETM2CH1     ((uint32_t)0x10000002)  /**< ETM2 CH1 alternate function mapping */
#define GPIOC15_AF1_ETM4CH1     ((uint32_t)0x20000004)  /**< ETM4 CH1 alternate function mapping */

#define GPIOD0_AF0_ETM2CH2      ((uint32_t)0x10000002)  /**< ETM2 CH2 alternate function mapping */
#define GPIOD0_AF1_ETM5CH1      ((uint32_t)0x20000005)  /**< ETM4 CH1 alternate function mapping */

#define GPIOD4_AF0_I2C2SDA      ((uint32_t)0x10000017)  /**< I2C2 SDA alternate function mapping */
#define GPIOD4_AF0_UART1TX      ((uint32_t)0x1000000C)  /**< UART1 TX alternate function mapping */
#define GPIOD4_AF0_UART1RX      ((uint32_t)0x1000000F)  /**< UART1 RX alternate function mapping */
#define GPIOD4_AF1_ETM5EXTCLK   ((uint32_t)0x20000012)  /**< ETM5 EXT CLK alternate function mapping */

#define GPIOD5_AF0_I2C2SCL      ((uint32_t)0x10000018)  /**< I2C2 SCL alternate function mapping */
#define GPIOD5_AF0_UART1RX      ((uint32_t)0x1000000C)  /**< UART1 RX alternate function mapping */
#define GPIOD5_AF0_UART1TX      ((uint32_t)0x1000000F)  /**< UART1 TX alternate function mapping */
#define GPIOD5_AF1_ETM5TCIN     ((uint32_t)0x20000013)  /**< ETM5 TC IN alternate function mapping */

#define GPIOD6_AF0_UART0TX      ((uint32_t)0x10000007)  /**< UART0 TX alternate function mapping */
#define GPIOD6_AF0_UART0RX      ((uint32_t)0x10000009)  /**< UART0 RX alternate function mapping */

#define GPIOD7_AF0_UART0RX      ((uint32_t)0x10000007)  /**< UART0 RX alternate function mapping */
#define GPIOD7_AF0_UART0TX      ((uint32_t)0x10000009)  /**< UART0 TX alternate function mapping */

#define GPIOD8_AF1_ETM3EXTCLK   ((uint32_t)0x20000010)  /**< ETM3 EXT CLK alternate function mapping */
#define GPIOD8_AF0_ACMP         ((uint32_t)0x1000001C)  /**< ACMP OUT alternate function mapping */
#define GPIOD8_AF1_PORRSTJ      ((uint32_t)0x20000007)  /**< POR reset status alternate function mapping */

#define GPIOD9_AF1_ETM3TCIN     ((uint32_t)0x20000011)  /**< ETM3 TC IN alternate function mapping */
#define GPIOD9_AF1_TSTCLK       ((uint32_t)0x20000006)  /**< source clocks output alternate function mapping */
#endif

#if (defined(CHIP_GM6621))
/**
  * @brief   AF 0 selection
  */
#define GPIOA1_AF0_I2C1SDA      ((uint8_t)0x00)    /**< I2C1 SDA alternate function mapping */
#define GPIOA2_AF0_I2C1SCL      ((uint8_t)0x00)    /**< I2C1 SCL alternate function mapping */
#define GPIOA3_AF0_UART0CTSJ    ((uint8_t)0x00)    /**< UART0 CTSJ alternate function mapping */
#define GPIOA4_AF0_UART0RTSJ    ((uint8_t)0x00)    /**< UART0 RTSJ alternate function mapping */
#define GPIOA5_AF0_UART0RX      ((uint8_t)0x00)    /**< UART0 RX alternate function mapping */
#define GPIOA6_AF0_UART0TX      ((uint8_t)0x00)    /**< UART0 TX OR HALF alternate function mapping */
#define GPIOA7_AF0_IR           ((uint8_t)0x00)    /**< IR alternate function mapping */
#define GPIOA8_AF0_IR           ((uint8_t)0x00)    /**< IR alternate function mapping */
#define GPIOA9_AF0_IR           ((uint8_t)0x00)    /**< IR alternate function mapping */
#define GPIOA10_AF0_IR          ((uint8_t)0x00)    /**< IR alternate function mapping */
#define GPIOA15_AF0_I2C0SDA     ((uint8_t)0x00)    /**< I2C0 SDA alternate function mapping */

#define GPIOB0_AF0_I2C1SCL      ((uint8_t)0x00)    /**< I2C1 SCL alternate function mapping */
#define GPIOB1_AF0_I2C1SDA      ((uint8_t)0x00)    /**< I2C1 SDA alternate function mapping */
#define GPIOB7_AF0_I2C1SCL      ((uint8_t)0x00)    /**< I2C1 SCL alternate function mapping */
#define GPIOB8_AF0_I2C1SDA      ((uint8_t)0x00)    /**< I2C1 SDA alternate function mapping */

#define GPIOC3_AF0_SWDDIO       ((uint8_t)0x00)    /**< SWD Data alternate function mapping */
#define GPIOC6_AF0_SWDCLK       ((uint8_t)0x00)    /**< SWD CLK alternate function mapping */
#define GPIOC7_AF0_SPI0SS0      ((uint8_t)0x00)    /**< SPI0 SS0 alternate function mapping */
#define GPIOC8_AF0_SPI0SS1      ((uint8_t)0x00)    /**< SPI0 SS1 alternate function mapping */
#define GPIOC12_AF0_SPI0SCK     ((uint8_t)0x00)    /**< SPI0 SCK alternate function mapping */
#define GPIOC13_AF0_SPI0MISO    ((uint8_t)0x00)    /**< SPI0 MISO alternate function mapping */
#define GPIOC14_AF0_SPI0MOSI    ((uint8_t)0x00)    /**< SPI0 MOSI alternate function mapping */
#define GPIOC15_AF0_I2C0SCL     ((uint8_t)0x00)    /**< I2C0 SCL alternate function mapping */

#define GPIOD0_AF0_I2C0SDA      ((uint8_t)0x00)    /**< I2C0 SDA alternate function mapping */
#define GPIOD2_AF0_I2C0SCL      ((uint8_t)0x00)    /**< I2C0 SCL alternate function mapping */
#define GPIOD3_AF0_I2C0SDA      ((uint8_t)0x00)    /**< I2C0 SDA alternate function mapping */
#define GPIOD6_AF0_I2C0SCL      ((uint8_t)0x00)    /**< I2C0 SCL alternate function mapping */

/**
  * @brief   AF 1 selection
  */
#define GPIOA1_AF1_SPI0MISO     ((uint8_t)0x01)    /**< SPI0 MISO alternate function mapping */
#define GPIOA2_AF1_SPI0MOSI     ((uint8_t)0x01)    /**< SPI0 MOSI alternate function mapping */
#define GPIOA3_AF1_SPI0SCK      ((uint8_t)0x01)    /**< SPI0 SCK alternate function mapping */
#define GPIOA4_AF1_SPI0SS0      ((uint8_t)0x01)    /**< SPI0 SS0 alternate function mapping */
#define GPIOA5_AF1_SPI0SS1      ((uint8_t)0x01)    /**< SPI0 SS1 alternate function mapping */
#define GPIOA10_AF1_UART0CTSJ   ((uint8_t)0x01)    /**< UART0 CTSJ alternate function mapping */
#define GPIOA12_AF1_UART0TX     ((uint8_t)0x01)    /**< UART0 TX OR HALF alternate function mapping */
#define GPIOA13_AF1_UART0RX     ((uint8_t)0x01)    /**< UART0 RX alternate function mapping */
#define GPIOA15_AF1_UART0RTSJ   ((uint8_t)0x01)    /**< UART0 RTSJ alternate function mapping */

#define GPIOB0_AF1_UART0TX      ((uint8_t)0x01)    /**< UART0 TX OR HALF alternate function mapping */
#define GPIOB1_AF1_UART0RX      ((uint8_t)0x01)    /**< UART0 RX alternate function mapping */
#define GPIOB6_AF1_UART0RTSJ    ((uint8_t)0x01)    /**< UART0 RTSJ alternate function mapping */
#define GPIOB7_AF1_UART0CTSJ    ((uint8_t)0x01)    /**< UART0 CTSJ alternate function mapping */
#define GPIOB8_AF1_UART0RTSJ    ((uint8_t)0x01)    /**< UART0 RTSJ alternate function mapping */
#define GPIOB15_AF1_UART1TX     ((uint8_t)0x01)    /**< UART1 TX OR HALF alternate function mapping */

#define GPIOC0_AF1_UART1RX      ((uint8_t)0x01)    /**< UART1 RX alternate function mapping */
#define GPIOC1_AF1_UART1CTSJ    ((uint8_t)0x01)    /**< UART1 CTSJ alternate function mapping */
#define GPIOC2_AF1_UART1RTSJ    ((uint8_t)0x01)    /**< UART1 RTSJ alternate function mapping */
#define GPIOC8_AF1_UART0TX      ((uint8_t)0x01)    /**< UART0 TX OR HALF alternate function mapping */
#define GPIOC9_AF1_UART0RX      ((uint8_t)0x01)    /**< UART0 RX alternate function mapping */
#define GPIOC10_AF1_UART0CTSJ   ((uint8_t)0x01)    /**< UART0 CTSJ alternate function mapping */
#define GPIOC11_AF1_UART0RTSJ   ((uint8_t)0x01)    /**< UART0 RTSJ alternate function mapping */
#define GPIOC15_AF1_UART1TX     ((uint8_t)0x01)    /**< UART1 TX OR HALF alternate function mapping */

#define GPIOD0_AF1_UART1RX      ((uint8_t)0x01)    /**< UART1 RX alternate function mapping */
#define GPIOD2_AF1_UART1RTSJ    ((uint8_t)0x01)    /**< UART1 RTSJ alternate function mapping */
#define GPIOD3_AF1_UART1CTSJ    ((uint8_t)0x01)    /**< UART1 CTSJ alternate function mapping */

/**
  * @brief   AF 2 selection
  */
#define GPIOA2_AF2_SPI0MISO     ((uint8_t)0x02)    /**< SPI0 MISO alternate function mapping */
#define GPIOA3_AF2_SPI0MOSI     ((uint8_t)0x02)    /**< SPI0 MOSI alternate function mapping */
#define GPIOA4_AF2_UART2CTSJ    ((uint8_t)0x02)    /**< UART2 CTSJ alternate function mapping */
#define GPIOA5_AF2_UART2RTSJ    ((uint8_t)0x02)    /**< UART2 RTSJ alternate function mapping */
#define GPIOA6_AF2_UART2TX      ((uint8_t)0x02)    /**< UART2 TX OR HALF alternate function mapping */
#define GPIOA7_AF2_UART2RX      ((uint8_t)0x02)    /**< UART2 RX alternate function mapping */
#define GPIOA8_AF2_SPI0SS0      ((uint8_t)0x02)    /**< SPI0 SS0 alternate function mapping */
#define GPIOA9_AF2_SPI0SCK      ((uint8_t)0x02)    /**< SPI0 SCK alternate function mapping */
#define GPIOA10_AF2_SPI0MISO    ((uint8_t)0x02)    /**< SPI0 MISO alternate function mapping */
#define GPIOA11_AF2_SPI0MOSI    ((uint8_t)0x02)    /**< SPI0 MOSI alternate function mapping */
#define GPIOA12_AF2_SPI1SS1     ((uint8_t)0x02)    /**< SPI1 SS1 alternate function mapping */
#define GPIOA13_AF2_SPI1SS0     ((uint8_t)0x02)    /**< SPI1 SS0 alternate function mapping */
#define GPIOA14_AF2_SPI1SCK     ((uint8_t)0x02)    /**< SPI1 SCK alternate function mapping */
#define GPIOA15_AF2_SPI1MISO    ((uint8_t)0x02)    /**< SPI1 MISO alternate function mapping */

#define GPIOB0_AF2_SPI0SCK      ((uint8_t)0x02)    /**< SPI0 SCK alternate function mapping */
#define GPIOB5_AF2_SPI1SS1      ((uint8_t)0x02)    /**< SPI1 SS1 alternate function mapping */
#define GPIOB6_AF2_SPI1SS0      ((uint8_t)0x02)    /**< SPI1 SS0 alternate function mapping */
#define GPIOB7_AF2_SPI1SCK      ((uint8_t)0x02)    /**< SPI1 SCK alternate function mapping */
#define GPIOB8_AF2_SPI1MISO     ((uint8_t)0x02)    /**< SPI1 MISO alternate function mapping */
#define GPIOB9_AF2_SPI1MOSI     ((uint8_t)0x02)    /**< SPI1 MOSI alternate function mapping */
#define GPIOB15_AF2_SPI0SS0     ((uint8_t)0x02)    /**< SPI0 SS0 alternate function mapping */

#define GPIOC0_AF2_SPI0SCK      ((uint8_t)0x02)    /**< SPI0 SCK alternate function mapping */
#define GPIOC1_AF2_SPI0MISO     ((uint8_t)0x02)    /**< SPI0 MISO alternate function mapping */
#define GPIOC2_AF2_SPI0MOSI     ((uint8_t)0x02)    /**< SPI0 MOSI alternate function mapping */
#define GPIOC6_AF2_UART2TX      ((uint8_t)0x02)    /**< UART2 TX OR HALF alternate function mapping */
#define GPIOC7_AF2_UART2RX      ((uint8_t)0x02)    /**< UART2 RX alternate function mapping */
#define GPIOC8_AF2_UART2CTSJ    ((uint8_t)0x02)    /**< UART2 CTSJ alternate function mapping */
#define GPIOC9_AF2_UART2RTSJ    ((uint8_t)0x02)    /**< UART2 RTSJ alternate function mapping */

#define GPIOD3_AF2_SPI1SS0      ((uint8_t)0x02)    /**< SPI1 SS0 alternate function mapping */
#define GPIOD4_AF2_SPI1SS1      ((uint8_t)0x02)    /**< SPI1 SS1 alternate function mapping */
#define GPIOD6_AF2_SPI1MOSI     ((uint8_t)0x02)    /**< SPI1 MOSI alternate function mapping */

/**
  * @brief   AF 3 selection
  */
#define GPIOA0_AF3_LPETMIN0     ((uint8_t)0x03)    /**< LPETM IN0 alternate function mapping */
#define GPIOA1_AF3_LPETMOUT     ((uint8_t)0x03)    /**< LPETM OUT alternate function mapping */
#define GPIOA2_AF3_LPETMIN1     ((uint8_t)0x03)    /**< LPETM IN1 alternate function mapping */
#define GPIOA3_AF3_LPETMEXTTRIG ((uint8_t)0x03)    /**< LPETM EXTTRIG alternate function mapping */
#define GPIOA4_AF3_ETM2EXTCLK   ((uint8_t)0x03)    /**< ETM2 EXTCLK alternate function mapping */
#define GPIOA5_AF3_ETM21EXTCLK  ((uint8_t)0x03)    /**< ETM21 EXTCLK alternate function mapping */
#define GPIOA6_AF3_ETM21CH0     ((uint8_t)0x03)    /**< ETM21 CH0 alternate function mapping */
#define GPIOA7_AF3_ETM21CH1     ((uint8_t)0x03)    /**< ETM21 CH1 alternate function mapping */
#define GPIOA8_AF3_ETM21CH0     ((uint8_t)0x03)    /**< ETM21 CH0 alternate function mapping */
#define GPIOA9_AF3_ETM22CH1     ((uint8_t)0x03)    /**< ETM21 CH1 alternate function mapping */
#define GPIOA10_AF3_ETM22CH0    ((uint8_t)0x03)    /**< ETM22 CH0 alternate function mapping */
#define GPIOA11_AF3_ETM22CH1    ((uint8_t)0x03)    /**< ETM22 CH1 alternate function mapping */
#define GPIOA12_AF3_EXTCLK      ((uint8_t)0x03)    /**< ETM21 OR ETM22 EXTCLK alternate function mapping */

//#define GPIOB0_AF3_ETM2CH2      ((uint8_t)0x03)    /**< ETM2 CH2 alternate function mapping */
//#define GPIOB1_AF3_ETM2CH3      ((uint8_t)0x03)    /**< ETM2 CH3 alternate function mapping */
#define GPIOB7_AF3_ETM21CH0     ((uint8_t)0x03)    /**< ETM21 CH0 alternate function mapping */
#define GPIOB8_AF3_ETM21CH1     ((uint8_t)0x03)    /**< ETM21 CH1 alternate function mapping */
#define GPIOB10_AF3_ETM22CH0    ((uint8_t)0x03)    /**< ETM22 CH0 alternate function mapping */
#define GPIOB11_AF3_ETM22CH1    ((uint8_t)0x03)    /**< ETM22 CH1 alternate function mapping */
#define GPIOB12_AF3_ETM22EXTCLK ((uint8_t)0x03)    /**< ETM22 EXTCLK alternate function mapping */
#define GPIOB13_AF3_ETM21EXTCLK ((uint8_t)0x03)    /**< ETM21 EXTCLK alternate function mapping */

#define GPIOC7_AF3_ETM2CH0       ((uint8_t)0x03)    /**< ETM2 CH0 alternate function mapping */
#define GPIOC12_AF3_ETM2CH1      ((uint8_t)0x03)    /**< ETM2 CH1 alternate function mapping */
#define GPIOC14_AF3_LPETMIN0     ((uint8_t)0x03)    /**< LPETM IN0 alternate function mapping */
#define GPIOC15_AF3_LPETMEXTTRIG ((uint8_t)0x03)    /**< LPETM EXTTRIG alternate function mapping */

#define GPIOD0_AF3_LPETMIN1     ((uint8_t)0x03)    /**< LPETM IN1 alternate function mapping */
#define GPIOD6_AF3_LPETMOUT     ((uint8_t)0x03)    /**< LPETM OUT alternate function mapping */

/**
  * @brief   AF 4 selection
  */
#define GPIOA4_AF4_ETM2CH0      ((uint8_t)0x04)    /**< ETM2 CH0 alternate function mapping */
#define GPIOA5_AF4_ETM2CH1      ((uint8_t)0x04)    /**< ETM2 CH1 alternate function mapping */
#define GPIOA6_AF4_ETM2CH2      ((uint8_t)0x04)    /**< ETM2 CH2 alternate function mapping */
#define GPIOA7_AF4_ETM2CH3      ((uint8_t)0x04)    /**< ETM2 CH3 alternate function mapping */
#define GPIOA8_AF4_ETM22EXTCLK  ((uint8_t)0x04)    /**< ETM22 EXTCLK alternate function mapping */
#define GPIOA9_AF4_ETM2EXTCLK   ((uint8_t)0x04)    /**< ETM2 EXTCLK alternate function mapping */

#define GPIOB0_AF4_ETM2CH2      ((uint8_t)0x04)    /**< ETM2 CH2 alternate function mapping */
#define GPIOB1_AF4_ETM2CH3      ((uint8_t)0x04)    /**< ETM2 CH3 alternate function mapping */

#define GPIOC7_AF4_ETM2EXTCLK   ((uint8_t)0x04)    /**< ETM2 EXTCLK alternate function mapping */
#define GPIOC12_AF4_ETM22EXTCLK ((uint8_t)0x04)    /**< ETM22 EXTCLK alternate function mapping */
#define GPIOC13_AF4_ETM22CH0    ((uint8_t)0x04)    /**< ETM22 CH0 alternate function mapping */
#define GPIOC14_AF4_ETM22CH1    ((uint8_t)0x04)    /**< ETM22 CH1 alternate function mapping */

#define GPIOD5_AF4_RTCTEMP1     ((uint8_t)0x04)    /**< RTC TEMP1 alternate function mapping */

/**
  * @brief   AF 5 selection
  */
#define GPIOA0_AF5_PESCH4       ((uint8_t)0x05)    /**< PES CH4 alternate function mapping */
#define GPIOA1_AF5_PESCH5       ((uint8_t)0x05)    /**< PES CH5 alternate function mapping */
#define GPIOA4_AF5_RTCTEMP2     ((uint8_t)0x05)    /**< RTC TEMP2 alternate function mapping */
#define GPIOA5_AF5_PESCH6       ((uint8_t)0x05)    /**< PES CH6 alternate function mapping */
#define GPIOA9_AF5_ETM2CH0      ((uint8_t)0x05)    /**< ETM2 CH0 alternate function mapping */
#define GPIOA10_AF5_PESCH7      ((uint8_t)0x05)    /**< PES CH7 alternate function mapping */
#define GPIOA11_AF5_PESCH8      ((uint8_t)0x05)    /**< PES CH8 alternate function mapping */
#define GPIOA12_AF5_PESCH9      ((uint8_t)0x05)    /**< PES CH9 alternate function mapping */
#define GPIOA14_AF5_PESCH10     ((uint8_t)0x05)    /**< PES CH10 alternate function mapping */

#define GPIOB1_AF5_PESCH11      ((uint8_t)0x05)    /**< PES CH11 alternate function mapping */
#define GPIOB6_AF5_PESCH0       ((uint8_t)0x05)    /**< PES CH0 output alternate function mapping */
#define GPIOB8_AF5_PESCH0       ((uint8_t)0x05)    /**< PES CH0 input alternate function mapping */
#define GPIOB8_AF5_RTCOUT       ((uint8_t)0x05)    /**< RTC OUT alternate function mapping */
#define GPIOB14_AF5_PESCH1      ((uint8_t)0x05)    /**< PES CH1 alternate function mapping */

#define GPIOC0_AF5_NMI          ((uint8_t)0x05)    /**< NMI alternate function mapping */
#define GPIOC1_AF5_PESCH2       ((uint8_t)0x05)    /**< PES CH2 alternate function mapping */
#define GPIOC2_AF5_PESCH3       ((uint8_t)0x05)    /**< PES CH3 alternate function mapping */
#define GPIOC7_AF5_PESCH0       ((uint8_t)0x05)    /**< PES CH0 alternate function mapping */
#define GPIOC12_AF5_PESCH1      ((uint8_t)0x05)    /**< PES CH1 alternate function mapping */
#define GPIOC13_AF5_PESCH2      ((uint8_t)0x05)    /**< PES CH2 alternate function mapping */

#define GPIOD1_AF5_BOOT0        ((uint8_t)0x05)    /**< BOOT0 alternate function mapping */
#define GPIOD3_AF5_PESCH3       ((uint8_t)0x05)    /**< PES CH3 alternate function mapping */
#define GPIOD5_AF5_RTCOUT       ((uint8_t)0x05)    /**< RTC OUT alternate function mapping */

/**
  * @brief   AF 6 selection
  */
#define GPIOA10_AF6_ACMP0       ((uint8_t)0x06)    /**< ACMP0 OUT alternate function mapping */
#define GPIOA11_AF6_ACMP1       ((uint8_t)0x06)    /**< ACMP1 OUT alternate function mapping */

#define GPIOB14_AF6_MCO         ((uint8_t)0x06)    /**< MCO alternate function mapping */
#define GPIOB15_AF6_MCO         ((uint8_t)0x06)    /**< MCO alternate function mapping */

#define GPIOC1_AF6_ACMP0        ((uint8_t)0x06)    /**< ACMP0 OUT alternate function mapping */
#define GPIOC2_AF6_ACMP1        ((uint8_t)0x06)    /**< ACMP1 OUT alternate function mapping */

/**
  * @brief   AF 7 selection
  */
#define GPIOA4_AF7_ACMP0        ((uint8_t)0x07)    /**< ACMP0 OUT alternate function mapping */
#define GPIOA6_AF7_ACMP1        ((uint8_t)0x07)    /**< ACMP1 OUT alternate function mapping */
#define GPIOA11_AF7_ACMP0       ((uint8_t)0x07)    /**< ACMP0 OUT alternate function mapping */

#endif

/**
  * @}
  */


/** @defgroup GPIO_mode_define GPIO mode define
  * @brief GPIO configuration mode
  *        Elements values convention: 0xX0yz000Y
  *           - X  : GPIO mode or EXTI mode
  *           - y  : Interrupt polarity
  *           - z  : Interrupt type
  *           - Y  : IO Direction mode (input, output, alternate or analog)
  * @{
  */
#define  GPIO_MODE_INPUT                        ((uint32_t)0x00000000)   /**< Input floating mode */
#define  GPIO_MODE_OUTPUT                       ((uint32_t)0x00000001)   /**< Output push pull mode */
#define  GPIO_MODE_AF                           ((uint32_t)0x00000002)   /**< alternate function mode */
#define  GPIO_MODE_ANALOG                       ((uint32_t)0x00000003)   /**< Analog mode */

#define  GPIO_MODE_IT_LOW                       ((uint32_t)0x10000000)   /**< External Interrupt mode with low level trigger detection */
#define  GPIO_MODE_IT_FALLING                   ((uint32_t)0x10010000)   /**< External Interrupt mode with Falling edge trigger detection */
#define  GPIO_MODE_IT_HIGH                      ((uint32_t)0x10100000)   /**< External Interrupt mode with high level trigger detection */
#define  GPIO_MODE_IT_RISING                    ((uint32_t)0x10110000)   /**< External Interrupt mode with Rising edge trigger detection */

/**
  * @}
  */

/** @defgroup GPIO_pull_define GPIO pull define
  * @brief GPIO Pull-Up or Pull-Up X2 Activation
  * @{
  */
#if (defined(CHIP_GM6601))
#define  GPIO_NOPULL        ((uint32_t)0x00000000)   /**< No Pull-up activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001)   /**< Pull-up activation */
#define  GPIO_PULLUPX2      ((uint32_t)0x00000003)   /**< Pull-Up X2 activation */
#define  GPIO_PULL_MASK     ((uint32_t)0x00000003)
#endif

#if (defined(CHIP_GM6601B))
#define  GPIO_NOPULL        ((uint32_t)0x00000000)   /**< No Pull-up activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001)   /**< Pull-up activation */
#define  GPIO_PULL_MASK     ((uint32_t)0x00000003)
#endif

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define  GPIO_NOPULL        ((uint32_t)0x00000000)   /**< No Pull-up activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001)   /**< Pull-up activation */
#define  GPIO_PULL_MASK     ((uint32_t)0x00000001)
#endif

#if (defined(CHIP_GM6621))
#define  GPIO_NOPULL        ((uint32_t)0x00000000)   /**< No Pull-up activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001)   /**< Pull-up activation */
#define  GPIO_PULLDOWN      ((uint32_t)0x00000002)   /**< Pull-down activation */
#define  GPIO_PULL_MASK     ((uint32_t)0x00000003)
#endif

/**
  * @}
  */


/** @defgroup FLT_clk_define input glitch filter clock define
  * @{
  */
#define  PAD_FLT_CLK0                    ((uint32_t)0x00000000)   /**< slect clock0 */
#define  PAD_FLT_CLK1                    ((uint32_t)0x00000001)   /**< slect clock1 */
#define  PAD_FLT_CLK2                    ((uint32_t)0x00000002)   /**< slect clock2 */
#define  PAD_IRC_LS_CLK                  ((uint32_t)0x00000003)   /**< slect IRC clock */
/**
  * @}
  */

/** @defgroup FLT_clk_psc_define Filter clock prescler define
  * @{
  */
#define  FLT_CLK0_PSC_DIV_2               ((uint32_t)0x00000000)   /**< Divided FILTER clock by 2, removed noise: 500ns */
#define  FLT_CLK0_PSC_DIV_4               ((uint32_t)0x00000001)   /**< Divided FILTER clock by 4, removed noise: 1us */
#define  FLT_CLK0_PSC_DIV_8               ((uint32_t)0x00000002)   /**< Divided FILTER clock by 8, removed noise: 2us */
#define  FLT_CLK0_PSC_DIV_16              ((uint32_t)0x00000003)   /**< Divided FILTER clock by 16, removed noise: 4us */

#define  FLT_CLK1_PSC_DIV_32              ((uint32_t)0x00000000)   /**< Divided FILTER clock by 32, removed noise: 8us */
#define  FLT_CLK1_PSC_DIV_64              ((uint32_t)0x00000001)   /**< Divided FILTER clock by 64, removed noise: 16us */
#define  FLT_CLK1_PSC_DIV_128             ((uint32_t)0x00000002)   /**< Divided FILTER clock by 128, removed noise: 32us */
#define  FLT_CLK1_PSC_DIV_256             ((uint32_t)0x00000003)   /**< Divided FILTER clock by 256, removed noise: 64us */
#define  FLT_CLK1_PSC_DIV_512             ((uint32_t)0x00000004)   /**< Divided FILTER clock by 512, removed noise: 128us */
#define  FLT_CLK1_PSC_DIV_1K              ((uint32_t)0x00000005)   /**< Divided FILTER clock by 1K, removed noise: 256us */
#define  FLT_CLK1_PSC_DIV_2K              ((uint32_t)0x00000006)   /**< Divided FILTER clock by 2K, removed noise: 512us */
#define  FLT_CLK1_PSC_DIV_4K              ((uint32_t)0x00000007)   /**< Divided FILTER clock by 4K, removed noise: 1ms */

#define  FLT_CLK2_PSC_DIV_8K              ((uint32_t)0x00000000)   /**< Divided FILTER clock by 8K, removed noise: 2ms */
#define  FLT_CLK2_PSC_DIV_16K             ((uint32_t)0x00000001)   /**< Divided FILTER clock by 16K, removed noise: 4ms */
#define  FLT_CLK2_PSC_DIV_32K             ((uint32_t)0x00000002)   /**< Divided FILTER clock by 32K, removed noise: 8ms */
#define  FLT_CLK2_PSC_DIV_64K             ((uint32_t)0x00000003)   /**< Divided FILTER clock by 64K, removed noise: 16ms */

/**
  * @}
  */

/**
  * @brief Sets input port glitch filter
  */
typedef struct
{
    uint8_t bypass;                  /**< Specifies the GPIO pins to be configured.
                                        This parameter can be any value of @ref GPIO_pins_define */
    uint32_t clock;                  /**< Specifies the filter clock.
                                        This parameter can be a value of @ref FLT_clk_define */
    uint32_t prescaler;              /**< Specifies the filter clock division.
                                        This parameter can be a value of @ref FLT_clk_psc_define */
}GPIO_InputGlitchFilter_T;

/**
  * @brief   GPIO Init structure definition
  */
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured.
                                        This parameter can be any value of @ref GPIO_pins_define */
    uint32_t mode;                 /**< Specifies the operating mode for the selected pins.
                                        This parameter can be a value of @ref GPIO_mode_define */
    uint32_t pull;                 /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins.
                                        This parameter can be a value of @ref GPIO_pull_define */
    uint32_t alternate;            /**< Peripheral to be connected to the selected pins
                                        This parameter can be a value of @ref GPIOEx_alternate_function_selection */
}GPIO_PinConfig_T;

/**
 * @brief  Initializes GPIO single pin with user specified attribute.
 *
 * @param pGPIO   GPIO base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE GPIO_Init(GPIO_Device_T *pGPIO, GPIO_PinConfig_T *pConfig);

/**
 * @brief  Deinitializes specific GPIO single pin to default state. Gpio interrupt enabled for this pin is also disabled.
 *
 * @param pGPIO      GPIO base address.
 * @param pin        Specifies the GPIO pins to be configured.This parameter can be any value of @ref GPIO_pins_define.
 * @param alternate  Peripheral to be connected to the selected pins.
 *                   This parameter can be a value of @ref GPIOEx_alternate_function_selection.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE GPIO_DeInit(GPIO_Device_T *pGPIO, uint16_t pin, uint32_t alternate);

/**
 * @brief  Set or clear specified pin.
 *
 * This function allows atomic modification of GPIO level.
 *
 * @param  pin    Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @param  state  Specifies expected level of the selected bit. It is low level if this parameter is zero.
 * @retval None
 */
void GPIO_PinWrite(GPIO_Pin_T pin, GPIO_PinState_T state);

/**
 * @brief  Read the specified port pin.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @retval      Level of the specified port pin. It is zero if the pin is at low level.
 */
GPIO_PinState_T GPIO_PinRead(GPIO_Pin_T pin);

/**
 * @brief  Toggle the specified port pin.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @retval None
 */
void GPIO_PinToggle(GPIO_Pin_T pin);

/**
 * @brief  Sets the specified port pin direction.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @param  direction  Specifies the pin direction.
 * @retval None
 */
void GPIO_SetPinDirection(GPIO_Pin_T pin, GPIO_PinDir_T direction);

/**
 * @brief  Sets the specified port pin glitch filter.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @param  pFilter  config input port pin filter.
 *
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE GPIO_SetPinFilter(GPIO_Pin_T pin, GPIO_InputGlitchFilter_T *pFilter);

/**
  * @brief  Checks whether the specified EXTI line is asserted or not.
  * @param pGPIO   GPIO base address.
  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#define GPIO_EXTI_GET_IT(pGPIO, __EXTI_LINE__) (pGPIO->ISFR & (__EXTI_LINE__))

/**
  * @brief  Clears the EXTI's line pending bits.
  * @param pGPIO   GPIO base address.
  * @param  __EXTI_LINE__: specifies the EXTI lines to clear.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define GPIO_EXTI_CLEAR_IT(pGPIO, __EXTI_LINE__) (pGPIO->ISFR = (__EXTI_LINE__))

#if (defined(CHIP_GM6601B))
/**
 * @brief GPIO Trigger ADC.
 *
 * @param pGPIO     GPIO base address.
 * @param pin       Specifies the GPIO pins to be configured.
                    This parameter can be any value of @ref GPIO_pins_define
 * @param enable    whether trigger ADC or not.
 * @retval None
 */
void GPIO_TriggerADC(GPIO_Device_T *pGPIO, uint32_t pin,  uint8_t enable);
#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GPIO_H_ */
