/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_ll_gpio.h
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

#ifndef _GPIO_DEV_H_
#define _GPIO_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_gpio_dev.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_GPIO_NUM        11

/* Get base address of GPIO controller register from decimal pin number */
#define GPIO_CTRL_BASE_FROM_PIN_NUMBER(pin)          \
    (((pin) < 16) ? (GPIOA_BASE)             \
     : (((pin) < 32) ? (GPIOB_BASE)           \
        : (((pin) < 48) ? (GPIOC_BASE)           \
           : (((pin) < 64) ? (GPIOD_BASE)           \
              : (((pin) < 80) ? (GPIOE_BASE)           \
                 : (((pin) < 96) ? (GPIOF_BASE)           \
                    : (((pin) < 112) ? (GPIOG_BASE)           \
                       : (((pin) < 128) ? (GPIOH_BASE)           \
                          : (((pin) < 144) ? (GPIOI_BASE)           \
                             : (((pin) < 160) ? (GPIOJ_BASE)           \
                                : (((pin) < 176) ? (GPIOK_BASE)           \
                                   : (0))))))))))))

#define GPIO_PIN_FROM_PIN_NUMBER(pin)      (((((uint32_t)pin) & 0x3FF) >> 2) << ((((uint32_t)pin) & 0x800)? 8: 0))
#define GPIO_DEVICE_FROM_PIN_NUMBER(pin)   (GPIO_Device_T *)(((uint32_t)pin) & 0xFFFFF000)

#define GPIO_BIT_ADDR(pin)        (GPIO_Pin_T)(GPIO_CTRL_BASE_FROM_PIN_NUMBER(pin) + \
        ((((pin&15)>>3) + 1)<<10) + ((1<<((pin&15) & 7))<<2))

#define PIN_GET(pin) ((*(pin) == 0)?0:1)
#define PIN_SET(pin) (*(pin) = (((((uint32_t)(pin))<<22)>>24)<<(((((uint32_t)(pin))>>11)&0x1)?8:0)))
#define PIN_CLEAR(pin) (*(pin) = 0)


/** GPIO device pointer */
#define GPIOA            ((GPIO_Device_T *)GPIOA_BASE)
#define GPIOB            ((GPIO_Device_T *)GPIOB_BASE)
#define GPIOC            ((GPIO_Device_T *)GPIOC_BASE)
#define GPIOD            ((GPIO_Device_T *)GPIOD_BASE)
#define GPIOE            ((GPIO_Device_T *)GPIOE_BASE)
#define GPIOF            ((GPIO_Device_T *)GPIOF_BASE)
#define GPIOG            ((GPIO_Device_T *)GPIOG_BASE)
#define GPIOH            ((GPIO_Device_T *)GPIOH_BASE)
#define GPIOI            ((GPIO_Device_T *)GPIOI_BASE)
#define GPIOJ            ((GPIO_Device_T *)GPIOJ_BASE)
#define GPIOK            ((GPIO_Device_T *)GPIOK_BASE)

/*******************************************************************************
 * Global function
 ******************************************************************************/
typedef __IO uint32_t *GPIO_Pin_T;

/**
  * @brief  GPIO bit set and bit reset enumeration
  */
typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState_T;

/**
  * @brief  GPIO pin direction enumeration
  */
typedef enum
{
    GPIO_PIN_OUTPUT = 0,   /**< pin as outout */
    GPIO_PIN_INPUT,        /**< pin as input  */
} GPIO_PinDir_T;

/**
  * @brief  Select the actions that the pin will take when the event occurs
  */

typedef enum
{
    GPIO_PIN_ACTION_CLEAR = 0,
    GPIO_PIN_ACTION_SET = 1,
    GPIO_PIN_ACTION_TOGGLE = 2,
    GPIO_PIN_ACTION_RESERVED = 3
} GPIO_ACTION_SELECT_T;


/**
  * @brief Select the channel of PES that the event comes from
  */

typedef enum
{
    PES_CHANNEL0 = 0,
    PES_CHANNEL1,
    PES_CHANNEL2,
    PES_CHANNEL3,
    PES_CHANNEL4,
    PES_CHANNEL5,
    PES_CHANNEL6,
    PES_CHANNEL7,
    PES_CHANNEL8,
    PES_CHANNEL9,
    PES_CHANNEL10,
    PES_CHANNEL11,
} GPIO_SELECT_PES_CHANNEL_T;

/**
  * @brief select the pin which responds the incoming event
  */

typedef enum
{
    PES_SEL_GPIO_PIN_0 = 0,
    PES_SEL_GPIO_PIN_1,
    PES_SEL_GPIO_PIN_2,
    PES_SEL_GPIO_PIN_3,
    PES_SEL_GPIO_PIN_4,
    PES_SEL_GPIO_PIN_5,
    PES_SEL_GPIO_PIN_6,
    PES_SEL_GPIO_PIN_7,
    PES_SEL_GPIO_PIN_8,
    PES_SEL_GPIO_PIN_9,
    PES_SEL_GPIO_PIN_10,
    PES_SEL_GPIO_PIN_11,
    PES_SEL_GPIO_PIN_12,
    PES_SEL_GPIO_PIN_13,
    PES_SEL_GPIO_PIN_14,
    PES_SEL_GPIO_PIN_15
} GPIO_PIN_SELECT_PES_T;

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

#define PINMUX_GPIO_PIN_0                 ((uint16_t)0x0000)  /**< PINMUX Pin 0 selected */
#define PINMUX_GPIO_PIN_1                 ((uint16_t)0x0001)  /**< PINMUX Pin 1 selected */
#define PINMUX_GPIO_PIN_2                 ((uint16_t)0x0002)  /**< PINMUX Pin 2 selected */
#define PINMUX_GPIO_PIN_3                 ((uint16_t)0x0003)  /**< PINMUX Pin 3 selected */
#define PINMUX_GPIO_PIN_4                 ((uint16_t)0x0004)  /**< PINMUX Pin 4 selected */
#define PINMUX_GPIO_PIN_5                 ((uint16_t)0x0005)  /**< PINMUX Pin 5 selected */
#define PINMUX_GPIO_PIN_6                 ((uint16_t)0x0006)  /**< PINMUX Pin 6 selected */
#define PINMUX_GPIO_PIN_7                 ((uint16_t)0x0007)  /**< PINMUX Pin 7 selected */
#define PINMUX_GPIO_PIN_8                 ((uint16_t)0x0008)  /**< PINMUX Pin 8 selected */
#define PINMUX_GPIO_PIN_9                 ((uint16_t)0x0009)  /**< PINMUX Pin 9 selected */
#define PINMUX_GPIO_PIN_10                ((uint16_t)0x000A)  /**< PINMUX Pin 10 selected */
#define PINMUX_GPIO_PIN_11                ((uint16_t)0x000B)  /**< PINMUX Pin 11 selected */
#define PINMUX_GPIO_PIN_12                ((uint16_t)0x000C)  /**< PINMUX Pin 12 selected */
#define PINMUX_GPIO_PIN_13                ((uint16_t)0x000D)  /**< PINMUX Pin 13 selected */
#define PINMUX_GPIO_PIN_14                ((uint16_t)0x000E)  /**< PINMUX Pin 14 selected */
#define PINMUX_GPIO_PIN_15                ((uint16_t)0x000F)  /**< PINMUX Pin 15 selected */

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

#define GPIOF0    GPIO_BIT_ADDR(80)      /**< GPIO pin 80 */
#define GPIOF1    GPIO_BIT_ADDR(81)      /**< GPIO pin 81 */
#define GPIOF2    GPIO_BIT_ADDR(82)      /**< GPIO pin 82 */
#define GPIOF3    GPIO_BIT_ADDR(83)      /**< GPIO pin 83 */
#define GPIOF4    GPIO_BIT_ADDR(84)      /**< GPIO pin 84 */
#define GPIOF5    GPIO_BIT_ADDR(85)      /**< GPIO pin 85 */
#define GPIOF6    GPIO_BIT_ADDR(86)      /**< GPIO pin 86 */
#define GPIOF7    GPIO_BIT_ADDR(87)      /**< GPIO pin 87 */
#define GPIOF8    GPIO_BIT_ADDR(88)      /**< GPIO pin 88 */
#define GPIOF9    GPIO_BIT_ADDR(89)      /**< GPIO pin 89 */
#define GPIOF10   GPIO_BIT_ADDR(90)      /**< GPIO pin 90 */
#define GPIOF11   GPIO_BIT_ADDR(91)      /**< GPIO pin 91 */
#define GPIOF12   GPIO_BIT_ADDR(92)      /**< GPIO pin 92 */
#define GPIOF13   GPIO_BIT_ADDR(93)      /**< GPIO pin 93 */
#define GPIOF14   GPIO_BIT_ADDR(94)      /**< GPIO pin 94 */
#define GPIOF15   GPIO_BIT_ADDR(95)      /**< GPIO pin 95 */

#define GPIOG0    GPIO_BIT_ADDR(96)      /**< GPIO pin 96 */
#define GPIOG1    GPIO_BIT_ADDR(97)      /**< GPIO pin 97 */
#define GPIOG2    GPIO_BIT_ADDR(98)      /**< GPIO pin 98 */
#define GPIOG3    GPIO_BIT_ADDR(99)      /**< GPIO pin 99 */
#define GPIOG4    GPIO_BIT_ADDR(100)     /**< GPIO pin 100 */
#define GPIOG5    GPIO_BIT_ADDR(101)     /**< GPIO pin 101 */
#define GPIOG6    GPIO_BIT_ADDR(102)     /**< GPIO pin 102 */
#define GPIOG7    GPIO_BIT_ADDR(103)     /**< GPIO pin 103 */
#define GPIOG8    GPIO_BIT_ADDR(104)     /**< GPIO pin 104 */
#define GPIOG9    GPIO_BIT_ADDR(105)     /**< GPIO pin 105 */
#define GPIOG10   GPIO_BIT_ADDR(106)     /**< GPIO pin 106 */
#define GPIOG11   GPIO_BIT_ADDR(107)     /**< GPIO pin 107 */
#define GPIOG12   GPIO_BIT_ADDR(108)     /**< GPIO pin 108 */
#define GPIOG13   GPIO_BIT_ADDR(109)     /**< GPIO pin 109 */
#define GPIOG14   GPIO_BIT_ADDR(110)     /**< GPIO pin 110 */
#define GPIOG15   GPIO_BIT_ADDR(111)     /**< GPIO pin 111 */

#define GPIOH0    GPIO_BIT_ADDR(112)      /**< GPIO pin 112 */
#define GPIOH1    GPIO_BIT_ADDR(113)      /**< GPIO pin 113 */
#define GPIOH2    GPIO_BIT_ADDR(114)      /**< GPIO pin 114 */
#define GPIOH3    GPIO_BIT_ADDR(115)      /**< GPIO pin 115 */
#define GPIOH4    GPIO_BIT_ADDR(116)      /**< GPIO pin 116 */
#define GPIOH5    GPIO_BIT_ADDR(117)      /**< GPIO pin 117 */
#define GPIOH6    GPIO_BIT_ADDR(118)      /**< GPIO pin 118 */
#define GPIOH7    GPIO_BIT_ADDR(119)      /**< GPIO pin 119 */
#define GPIOH8    GPIO_BIT_ADDR(120)      /**< GPIO pin 120 */
#define GPIOH9    GPIO_BIT_ADDR(121)      /**< GPIO pin 121 */
#define GPIOH10   GPIO_BIT_ADDR(122)      /**< GPIO pin 122 */
#define GPIOH11   GPIO_BIT_ADDR(123)      /**< GPIO pin 123 */
#define GPIOH12   GPIO_BIT_ADDR(124)      /**< GPIO pin 124 */
#define GPIOH13   GPIO_BIT_ADDR(125)      /**< GPIO pin 125 */
#define GPIOH14   GPIO_BIT_ADDR(126)      /**< GPIO pin 126 */
#define GPIOH15   GPIO_BIT_ADDR(127)      /**< GPIO pin 127 */

#define GPIOI0    GPIO_BIT_ADDR(128)      /**< GPIO pin 128 */
#define GPIOI1    GPIO_BIT_ADDR(129)      /**< GPIO pin 129 */
#define GPIOI2    GPIO_BIT_ADDR(130)      /**< GPIO pin 130 */
#define GPIOI3    GPIO_BIT_ADDR(131)      /**< GPIO pin 131 */
#define GPIOI4    GPIO_BIT_ADDR(132)      /**< GPIO pin 132 */
#define GPIOI5    GPIO_BIT_ADDR(133)      /**< GPIO pin 133 */
#define GPIOI6    GPIO_BIT_ADDR(134)      /**< GPIO pin 134 */
#define GPIOI7    GPIO_BIT_ADDR(135)      /**< GPIO pin 135 */
#define GPIOI8    GPIO_BIT_ADDR(136)      /**< GPIO pin 136 */
#define GPIOI9    GPIO_BIT_ADDR(137)      /**< GPIO pin 137 */
#define GPIOI10   GPIO_BIT_ADDR(138)      /**< GPIO pin 138 */
#define GPIOI11   GPIO_BIT_ADDR(139)      /**< GPIO pin 139 */
#define GPIOI12   GPIO_BIT_ADDR(140)      /**< GPIO pin 140 */
#define GPIOI13   GPIO_BIT_ADDR(141)      /**< GPIO pin 141 */
#define GPIOI14   GPIO_BIT_ADDR(142)      /**< GPIO pin 142 */
#define GPIOI15   GPIO_BIT_ADDR(143)      /**< GPIO pin 143 */

#define GPIOJ0    GPIO_BIT_ADDR(144)      /**< GPIO pin 144 */
#define GPIOJ1    GPIO_BIT_ADDR(145)      /**< GPIO pin 145 */
#define GPIOJ2    GPIO_BIT_ADDR(146)      /**< GPIO pin 146 */
#define GPIOJ3    GPIO_BIT_ADDR(147)      /**< GPIO pin 147 */
#define GPIOJ4    GPIO_BIT_ADDR(148)      /**< GPIO pin 148 */
#define GPIOJ5    GPIO_BIT_ADDR(149)      /**< GPIO pin 149 */
#define GPIOJ6    GPIO_BIT_ADDR(150)      /**< GPIO pin 150 */
#define GPIOJ7    GPIO_BIT_ADDR(151)      /**< GPIO pin 151 */
#define GPIOJ8    GPIO_BIT_ADDR(152)      /**< GPIO pin 152 */
#define GPIOJ9    GPIO_BIT_ADDR(153)      /**< GPIO pin 153 */
#define GPIOJ10   GPIO_BIT_ADDR(154)      /**< GPIO pin 154 */
#define GPIOJ11   GPIO_BIT_ADDR(155)      /**< GPIO pin 155 */
#define GPIOJ12   GPIO_BIT_ADDR(156)      /**< GPIO pin 156 */
#define GPIOJ13   GPIO_BIT_ADDR(157)      /**< GPIO pin 157 */
#define GPIOJ14   GPIO_BIT_ADDR(158)      /**< GPIO pin 158 */
#define GPIOJ15   GPIO_BIT_ADDR(159)      /**< GPIO pin 159 */

#define GPIOK0    GPIO_BIT_ADDR(160)      /**< GPIO pin 160 */
#define GPIOK1    GPIO_BIT_ADDR(161)      /**< GPIO pin 161 */
#define GPIOK2    GPIO_BIT_ADDR(162)      /**< GPIO pin 162 */
#define GPIOK3    GPIO_BIT_ADDR(163)      /**< GPIO pin 163 */
#define GPIOK4    GPIO_BIT_ADDR(164)      /**< GPIO pin 164 */
#define GPIOK5    GPIO_BIT_ADDR(165)      /**< GPIO pin 165 */
#define GPIOK6    GPIO_BIT_ADDR(166)      /**< GPIO pin 166*/
#define GPIOK7    GPIO_BIT_ADDR(167)      /**< GPIO pin 167*/
#define GPIOK8    GPIO_BIT_ADDR(168)      /**< GPIO pin 168*/


/**
  * @}
  */

/** @defgroup GPIOEx_alternate_function_selection GPIO extended alternate function selection
  * @brief alternate function mapping
  *        Elements values convention: 0xX00000yz
  *           - X  : 1 -- PINMUX_CTRL0 resistor; 2 -- PINMUX_CTRL1 register; 3 -- PINMUX_CTRL2 register
  *           - y,z  : Register bit number
  * @{
  */

#if (defined(CHIP_GM6721))
/**
  * @brief   AF 0 selection
  */

#define GPIO_AF_NONE            ((uint8_t)0x00)
#define GPIOA5_AF0_RTC_TAMP2    ((uint8_t)0x00)    /**< RTC_TAMP2 alternate function mapping */
#define GPIOA6_AF0_RTC_TAMP1    ((uint8_t)0x00)    /**< RTC_TAMP1 alternate function mapping */
#define GPIOA12_AF0_I2C2_SDA    ((uint8_t)0x00)    /**< I2C2_SDA alternate function mapping */
#define GPIOA13_AF0_I2C2_SCL    ((uint8_t)0x00)    /**< I2C2_SCL alternate function mapping */

#define GPIOC0_AF0TIM2_ETR      ((uint8_t)0x00)    /**< TIM2_ETR alternate function mapping */
#define GPIOC5_AF0_I2C2_SCL     ((uint8_t)0x00)    /**<I2C2_SCL alternate function mapping */
#define GPIOC6_AF0_I2C2_SDA     ((uint8_t)0x00)    /**< I2C2_SDA alternate function mapping */
#define GPIOC8_AF0_C2_SWDIO     ((uint8_t)0x00)    /**< SWD Data alternate function mapping */
#define GPIOC10_AF0_C2_SWCLK    ((uint8_t)0x00)    /**< SWD CLK alternate function mapping */

#define GPIOE5_AF0_I2C2_SDA     ((uint8_t)0x00)    /**< I2C2_SDA alternate function mapping */
#define GPIOE7_AF0_I2C2_SCL     ((uint8_t)0x00)    /**< I2C2_SCL alternate function mapping */
#define GPIOE8_AF0_I2C2_SDA     ((uint8_t)0x00)    /**< I2C2_SDA alternate function mapping */
#define GPIOE10_AF0_I2C3_SCL    ((uint8_t)0x00)    /**< I2C3_SCL alternate function mapping */
#define GPIOE11_AF0_I2C3_SCL    ((uint8_t)0x00)    /**< I2C3_SCL alternate function mapping */
#define GPIOE12_AF0_I2C3_SDA    ((uint8_t)0x00)    /**< I2C3_SDA alternate function mapping */

#define GPIOH0_AF0_I2C3_SDA     ((uint8_t)0x00)    /**< I2C3_SDA alternate function mapping */
#define GPIOH6_AF0_JTMS_SWDIO   ((uint8_t)0x00)    /**< JTMS_SWDIO alternate function mapping */

#define GPIOH14_AF0_JTCK_SWCLK  ((uint8_t)0x00)    /**< JTCK_SWCLK alternate function mapping */
#define GPIOH15_AF0_JTDI        ((uint8_t)0x00)     /**<JTDI alternate function mapping */

#define GPIOI0_AF0_I2C1_SCL     ((uint8_t)0x00)    /**< I2C1_SCL alternate function mapping */
#define GPIOI1_AF0_I2C1_SDA     ((uint8_t)0x00)     /**<I2C1_SDA alternate function mapping */

#define GPIOJ8_AF0_C2_SWCLK     ((uint8_t)0x00)    /**< C2_SWCLK alternate function mapping */
#define GPIOJ10_AF0_C2_SWDIO    ((uint8_t)0x00)     /**<C2_SWDIO alternate function mapping */

#define GPIOJ11_AF0_JTDO        ((uint8_t)0x00)    /**< JTDO alternate function mapping */
#define GPIOJ12_AF0_JTRSTN      ((uint8_t)0x00)     /**<JTRSTN alternate function mapping */

#define GPIOJ14_AF0_I2C1_SCL    ((uint8_t)0x00)    /**< I2C1_SCL alternate function mapping */
#define GPIOJ15_AF0_I2C1_SDA    ((uint8_t)0x00)     /**<I2C1_SDA alternate function mapping */

#define GPIOK1_AF0_I2C1_SCL     ((uint8_t)0x00)    /**< I2C1_SCL alternate function mapping */
#define GPIOK2_AF0_I2C1_SDA     ((uint8_t)0x00)     /**<I2C1_SDA alternate function mapping */


/**
  * @brief   AF 1 selection
  */
#define GPIOA1_AF1_VIN2_D0      ((uint8_t)0x01)    /**< VIN2_D[0] alternate function mapping */
#define GPIOA2_AF1_VIN2_D1      ((uint8_t)0x01)    /**< VIN2_D[1] alternate function mapping */
#define GPIOA3_AF1_VIN2_D2      ((uint8_t)0x01)    /**< VIN2_D[2] alternate function mapping */
#define GPIOA4_AF1_VIN2_D3      ((uint8_t)0x01)    /**< VIN2_D[3] alternate function mapping */
#define GPIOA6_AF1_RTC_OUT      ((uint8_t)0x01)    /**< RTC_OUT alternate function mapping */
#define GPIOA14_AF1_VIN2_D0     ((uint8_t)0x01)    /**< VIN2_D[0] CTSJ alternate function mapping */
#define GPIOA15_AF1_VIN2_D1     ((uint8_t)0x01)    /**< VIN2_D[1] alternate function mapping */

#define GPIOB0_AF1_VIN2_D2      ((uint8_t)0x01)    /**< VIN2_D[2] alternate function mapping */
#define GPIOB4_AF1_VIN2_D5      ((uint8_t)0x01)    /**< VIN2_D[5] alternate function mapping */
#define GPIOB5_AF1_VIN2_D4      ((uint8_t)0x01)    /**< VIN2_D[4] alternate function mapping */
#define GPIOB6_AF1_VIN2_D6      ((uint8_t)0x01)    /**< VIN2_D[6] alternate function mapping */
#define GPIOB7_AF1_VIN2_D7      ((uint8_t)0x01)    /**<VIN2_D[7] alternate function mapping */
#define GPIOB8_AF1_VIN2_VSYNC   ((uint8_t)0x01)    /**< VIN2_VSYNC alternate function mapping */

#define GPIOC0_AF1_TIM2_CH1     ((uint8_t)0x01)    /**< TIM2_CH1 alternate function mapping */
#define GPIOC1_AF1_TIM2_CH2     ((uint8_t)0x01)    /**< TIM2_CH2 alternate function mapping */
#define GPIOC2_AF1_TIM2_CH3     ((uint8_t)0x01)    /**< TIM2_CH3 alternate function mapping */
#define GPIOC7_AF1_TIM2_CH4     ((uint8_t)0x01)    /**<TIM2_CH4 alternate function mapping */
#define GPIOC9_AF1_TIM2_CH1     ((uint8_t)0x01)    /**< TIM2_CH1 alternate function mapping */
#define GPIOC10_AF1_TIM1_BKIN   ((uint8_t)0x01)    /**< TIM1_BKIN alternate function mapping */
#define GPIOC11_AF1_TIM1_CH1N   ((uint8_t)0x01)    /**<TIM1_CH1N alternate function mapping */
#define GPIOC14_AF1_TIM1_CH2N   ((uint8_t)0x01)    /**< TIM1_CH2N alternate function mapping */
#define GPIOC15_AF1_TIM1_CH3N   ((uint8_t)0x01)    /**<TIM1_CH3N alternate function mapping */

#define GPIOD14_AF1_TIM1_ETR    ((uint8_t)0x01)    /**< TIM1_ETR alternate function mapping */
#define GPIOD15_AF1_TIM1_CH1N   ((uint8_t)0x01)    /**< TIM1_CH1N alternate function mapping */

#define GPIOE0_AF1_TIM1_CH1     ((uint8_t)0x01)    /**< TIM1_CH1 alternate function mapping */
#define GPIOE1_AF1_TIM1_CH2N    ((uint8_t)0x01)    /**< TIM1_CH2N alternate function mapping */
#define GPIOE2_AF1_TIM1_CH2     ((uint8_t)0x01)    /**< TIM1_CH2 alternate function mapping */
#define GPIOE3_AF1_TIM1_CH3N    ((uint8_t)0x01)    /**< TIM1_CH3N alternate function mapping */
#define GPIOE4_AF1_TIM1_CH3     ((uint8_t)0x01)    /**< TIM1_CH3 alternate function mapping */
#define GPIOE5_AF1_TIM1_CH4     ((uint8_t)0x01)    /**< TIM1_CH4 alternate function mapping */
#define GPIOE6_AF1_TIM1_BKIN    ((uint8_t)0x01)    /**< TIM1_BKIN alternate function mapping */
#define GPIOE7_AF1_TIM2_CH3     ((uint8_t)0x01)    /**< TIM2_CH3 alternate function mapping */
#define GPIOE8_AF1_TIM2_CH4     ((uint8_t)0x01)    /**< TIM2_CH4 alternate function mapping */
#define GPIOE14_AF1_VIN2_D3     ((uint8_t)0x01)    /**< VIN2_D[3] alternate function mapping */

#define GPIOF0_AF1_VIN2_D4      ((uint8_t)0x01)    /**< VIN2_D[4] alternate function mapping */
#define GPIOF1_AF1_TIM1_BKIN    ((uint8_t)0x01)    /**< TIM1_BKIN alternate function mapping */
#define GPIOF2_AF1_TIM1_CH1N    ((uint8_t)0x01)    /**< TIM1_CH1N alternate function mapping */
#define GPIOF3_AF1_TIM1_CH2N    ((uint8_t)0x01)    /**< TIM1_CH2N alternate function mapping */
#define GPIOF4_AF1_TIM1_CH3N    ((uint8_t)0x01)    /**< TIM1_CH3N alternate function mapping */
#define GPIOF5_AF1_VIN2_D5      ((uint8_t)0x01)    /**< VIN2_D[5] alternate function mapping */
#define GPIOF8_AF1_VIN2_D5      ((uint8_t)0x01)    /**< VIN2_D[5] alternate function mapping */

#define GPIOG8_AF1_VIN2_D6      ((uint8_t)0x01)    /**< VIN2_D[6] alternate function mapping */
#define GPIOG9_AF1_VIN2_D7      ((uint8_t)0x01)    /**< VIN2_D[7] alternate function mapping */
#define GPIOG10_AF1_VIN2_VSYNC  ((uint8_t)0x01)    /**< VIN2_VSYNC alternate function mapping */

#define GPIOH0_AF1_MCO2         ((uint8_t)0x01)    /**< MCO2 alternate function mapping */
#define GPIOH1_AF1_MCO1         ((uint8_t)0x01)    /**< MCO1 alternate function mapping */

#define GPIOJ0_AF1_TIM1_VIN2_D0 ((uint8_t)0x01)    /**< VIN2_D[0] alternate function mapping */
#define GPIOJ2_AF1_TIM1_VIN2_D1 ((uint8_t)0x01)    /**< VIN2_D[1] alternate function mapping */
#define GPIOJ4_AF1_TIM1_VIN2_D2 ((uint8_t)0x01)    /**< VIN2_D[2] alternate function mapping */
#define GPIOJ8_AF1_VIN2_HSYNC   ((uint8_t)0x01)    /**< VIN2_HSYNC alternate function mapping */
#define GPIOJ10_AF1_VIN2_PIXCLK ((uint8_t)0x01)    /**< VIN2_PIXCLK alternate function mapping */
#define GPIOK2_AF1_TIM1_VIN2_D5 ((uint8_t)0x01)    /**< VIN2_D[5] alternate function mapping */


/**
  * @brief   AF 2 selection
  */
#define GPIOC0_AF2_TIM5_CH1     ((uint8_t)0x02)    /**< TIM5_CH1 alternate function mapping */
#define GPIOC1_AF2_TIM5_CH2     ((uint8_t)0x02)    /**<TIM5_CH2 alternate function mapping */
#define GPIOC2_AF2_TIM5_CH3     ((uint8_t)0x02)    /**< TIM5_CH3 alternate function mapping */
#define GPIOC7_AF2_TIM5_CH4     ((uint8_t)0x02)    /**< TIM5_CH4 alternate function mapping */
#define GPIOC9_AF2_TIM2_ETR     ((uint8_t)0x02)    /**< TIM2_ETR alternate function mapping */
#define GPIOC10_AF2_TIM3_CH1    ((uint8_t)0x02)    /**<TIM3_CH1 alternate function mapping */
#define GPIOC11_AF2_TIM3_CH2    ((uint8_t)0x02)    /**< TIM3_CH2 alternate function mapping */
#define GPIOC14_AF2_TIM3_CH3    ((uint8_t)0x02)    /**< TIM3_CH3 alternate function mapping */
#define GPIOC15_AF2_TIM3_CH4    ((uint8_t)0x02)    /**< TIM3_CH4 alternate function mapping */

#define GPIOE14_AF2_TIM5_CH1    ((uint8_t)0x02)    /**< TIM5_CH1 alternate function mapping */
#define GPIOE15_AF2_TIM5_CH2    ((uint8_t)0x02)    /**< TIM5_CH2 alternate function mapping */

#define GPIOF0_AF2_TIM5_CH3     ((uint8_t)0x02)    /**< TIM5_CH3 alternate function mapping */
#define GPIOF9_AF2_TIM4_CH1     ((uint8_t)0x02)    /**< TIM4_CH1 alternate function mapping */
#define GPIOF10_AF2_TIM4_CH2    ((uint8_t)0x02)    /**< TIM4_CH2 alternate function mapping */
#define GPIOF11_AF2_TIM4_CH3    ((uint8_t)0x02)    /**< TIM4_CH4 alternate function mapping */
#define GPIOF12_AF2_TIM4_CH4    ((uint8_t)0x02)    /**< TIM4_CH4 alternate function mapping */

#define GPIOG13_AF2_TIM3_CH1    ((uint8_t)0x02)    /**< TIM3_CH1 alternate function mapping */
#define GPIOG14_AF2_TIM3_CH2    ((uint8_t)0x02)    /**< TIM3_CH2 alternate function mapping */
#define GPIOG15_AF2_TIM3_CH3    ((uint8_t)0x02)    /**< TIM3_CH3 alternate function mapping */

#define GPIOH0_AF2_TIM3_CH4     ((uint8_t)0x02)    /**< TIM3_CH4 alternate function mapping */
#define GPIOH1_AF2_TIM1_CH1     ((uint8_t)0x02)    /**< TIM1_CH1 alternate function mapping */
#define GPIOH2_AF2_TIM1_CH2     ((uint8_t)0x02)    /**< TIM1_CH2 alternate function mapping */
#define GPIOH3_AF2_TIM1_CH3     ((uint8_t)0x02)    /**< TIM1_CH3 alternate function mapping */
#define GPIOH4_AF2_TIM1_CH4     ((uint8_t)0x02)    /**< TIM1_CH4 alternate function mapping */
#define GPIOH5_AF2_TIM1_ETR     ((uint8_t)0x02)    /**< TIM1_ETR alternate function mapping */

#define GPIOH10_AF2_TIM5_CH4    ((uint8_t)0x02)    /**< TIM5_CH4 alternate function mapping */
#define GPIOH15_AF2_TIM2_CH1    ((uint8_t)0x02)    /**< TIM2_CH1 alternate function mapping */


#define GPIOI5_AF2_TIM3_ETR     ((uint8_t)0x02)    /**< TIM3_ETR alternate function mapping */

#define GPIOJ11_AF2_TIM2_CH2    ((uint8_t)0x02)    /**< TIM2_CH2 alternate function mapping */
#define GPIOJ12_AF2_TIM3_CH1    ((uint8_t)0x02)    /**< TIM3_CH1 alternate function mapping */
#define GPIOJ13_AF2_TIM3_CH2    ((uint8_t)0x02)    /**< TIM3_CH2 alternate function mapping */

#define GPIOJ14_AF2_TIM4_CH1    ((uint8_t)0x02)    /**< TIM4_CH1 alternate function mapping */
#define GPIOJ15_AF2_TIM4_CH2    ((uint8_t)0x02)    /**< TIM4_CH2 alternate function mapping */

#define GPIOK1_AF2_TIM4_CH3     ((uint8_t)0x02)    /**< TIM4_CH3 alternate function mapping */
#define GPIOK2_AF2_TIM4_CH4     ((uint8_t)0x02)    /**< TIM4_CH4 alternate function mapping */
#define GPIOK3_AF2_TIM4_ETR     ((uint8_t)0x02)    /**< TIM4_ETR alternate function mapping */

/**
  * @brief   AF 3 selection
  */
#define GPIOA3_AF3_TIM9_CH1     ((uint8_t)0x03)    /**< TIM9_CH1 alternate function mapping */
#define GPIOA4_AF3_TIM9_CH2     ((uint8_t)0x03)    /**< TIM9_CH2 alternate function mapping */

#define GPIOB5_AF3_TIM10_CH1    ((uint8_t)0x03)    /**< TIM10_CH1 alternate function mapping */
#define GPIOB6_AF3_TIM11_CH1    ((uint8_t)0x03)    /**< TIM11_CH1 alternate function mapping */

#define GPIOC0_AF3_TIM8_ETR     ((uint8_t)0x03)    /**< TIM8_ETR alternate function mapping */
#define GPIOC2_AF3_TIM9_CH1     ((uint8_t)0x03)    /**< TIM9_CH1 alternate function mapping */

#define GPIOC7_AF3_TIM9_CH2     ((uint8_t)0x03)    /**< TIM9_CH2 alternate function mapping */
#define GPIOC9_AF3_TIM8_CH1N    ((uint8_t)0x03)    /**< TIM8_CH1N alternate function mapping */
#define GPIOC10_AF3_TIM8_BKIN   ((uint8_t)0x03)    /**< TIM8_BKIN alternate function mapping */
#define GPIOC11_AF3_TIM8_CH1N   ((uint8_t)0x03)    /**< TIM8_CH1N alternate function mapping */

#define GPIOC14_AF3_TIM8_CH2N   ((uint8_t)0x03)    /**< TIM8_CH2N alternate function mapping */
#define GPIOC15_AF3_TIM8_CH3N   ((uint8_t)0x03)    /**< TIM8_CH3N alternate function mapping */

#define GPIOF3_AF3_TIM8_CH2N    ((uint8_t)0x03)    /**< TIM8_CH2N alternate function mapping */
#define GPIOF4_AF3_TIM8_CH3N    ((uint8_t)0x03)    /**< TIM8_CH3N alternate function mapping */

#define GPIOG13_AF3_TIM8_CH1    ((uint8_t)0x03)    /**< TIM8_CH1 alternate function mapping */
#define GPIOG14_AF3_TIM8_CH2    ((uint8_t)0x03)    /**< TIM8_CH2 alternate function mapping */
#define GPIOG15_AF3_TIM8_CH3    ((uint8_t)0x03)    /**< TIM8_CH3 alternate function mapping */

#define GPIOH0_AF3_TIM8_CH4     ((uint8_t)0x03)    /**< TIM8_CH4 alternate function mapping */
#define GPIOH7_AF3_TIM8_CH1N    ((uint8_t)0x03)    /**< TIM8_CH1N alternate function mapping */
#define GPIOH8_AF3_TIM8_CH2N    ((uint8_t)0x03)    /**<TIM8_CH2N alternate function mapping */
#define GPIOH9_AF3_TIM8_CH3N    ((uint8_t)0x03)    /**< TIM8_CH3N alternate function mapping */

#define GPIOH12_AF3_TIM8_CH4    ((uint8_t)0x03)    /**< TIM8_CH4 alternate function mapping */
#define GPIOH13_AF3_TIM8_ETR    ((uint8_t)0x03)    /**<TIM8_ETR alternate function mapping */
#define GPIOH15_AF3_TIM2_ETR    ((uint8_t)0x03)    /**< TIM2_ETR alternate function mapping */

#define GPIOK1_AF3_TIM10_CH1    ((uint8_t)0x03)    /**< TIM10_CH1 alternate function mapping */
#define GPIOK2_AF3_TIM11_CH1    ((uint8_t)0x03)    /**< TIM11_CH1 alternate function mapping */
#define GPIOK5_AF3_TIM8_BKIN    ((uint8_t)0x03)    /**< TIM8_BKIN alternate function mapping */
#define GPIOK6_AF3_TIM8_CH1     ((uint8_t)0x03)    /**< TIM8_CH1 alternate function mapping */
#define GPIOK7_AF3_TIM8_CH2     ((uint8_t)0x03)    /**<TIM8_CH2 alternate function mapping */
#define GPIOK8_AF3_TIM8_CH3     ((uint8_t)0x03)    /**< TIM8_CH3 alternate function mapping */

/**
  * @brief   AF 4 selection
  */
#define GPIOA0_AF4_SPI4_SCK     ((uint8_t)0x04)    /**< SPI4_SCK alternate function mapping */
#define GPIOA2_AF4_SPI4_SS      ((uint8_t)0x04)    /**< SPI4_SS alternate function mapping */
#define GPIOA3_AF4_SPI4_MISO    ((uint8_t)0x04)    /**< SPI4_MISO alternate function mapping */
#define GPIOA4_AF4_SPI4_MOSI    ((uint8_t)0x04)    /**< SPI4_MOSI alternate function mapping */

#define GPIOB5_AF4_SPI5_SS      ((uint8_t)0x04)    /**< SPI5_SS alternate function mapping */
#define GPIOB6_AF4_SPI5_SCK     ((uint8_t)0x04)    /**< SPI5_SCK alternate function mapping */
#define GPIOB7_AF4_SPI5_MISO    ((uint8_t)0x04)    /**< SPI5_MISO alternate function mapping */
#define GPIOB8_AF4_SPI5_MOSI    ((uint8_t)0x04)    /**<SPI5_MOSI alternate function mapping */

#define GPIOB14_AF4_SPI2_MISO   ((uint8_t)0x04)    /**< SPI2_MISO alternate function mapping */
#define GPIOB15_AF4_SPI2_MOSI   ((uint8_t)0x04)    /**< SPI2_MOSI alternate function mapping */

#define GPIOC6_AF4_SPI5_SS      ((uint8_t)0x04)    /**< SPI5_SS alternate function mapping */

#define GPIOC8_AF4_SPI1_SS      ((uint8_t)0x04)    /**< SPI1_SS alternate function mapping */
#define GPIOC9_AF4_SPI1_SCK     ((uint8_t)0x04)    /**< SPI1_SCK alternate function mapping */
#define GPIOC10_AF4_SPI1_MISO   ((uint8_t)0x04)    /**< SPI1_MISO alternate function mapping */
#define GPIOC11_AF4_SPI1_MOSI   ((uint8_t)0x04)    /**< SPI1_MOSI alternate function mapping */

#define GPIOD7_AF4_SPI5_MOSI    ((uint8_t)0x04)    /**< SPI5_MOSI alternate function mapping */

#define GPIOE2_AF4_SPI4_SS      ((uint8_t)0x04)    /**<SPI4_SS alternate function mapping */
#define GPIOE3_AF4_SPI4_SCK     ((uint8_t)0x04)    /**< SPI4_SCK alternate function mapping */
#define GPIOE4_AF4_SPI4_MISO    ((uint8_t)0x04)    /**<SPI4_MISO alternate function mapping */
#define GPIOE5_AF4_SPI4_MOSI    ((uint8_t)0x04)    /**< SPI4_MOSI alternate function mapping */

#define GPIOE7_AF4_SPI2_SCK     ((uint8_t)0x04)    /**<SPI2_SCK alternate function mapping */
#define GPIOE10_AF4_SPI5_SCK    ((uint8_t)0x04)    /**< SPI5_SCK alternate function mapping */
#define GPIOE11_AF4_SPI5_MISO   ((uint8_t)0x04)    /**<SPI5_MISO alternate function mapping */

#define GPIOF1_AF4_SPI2_SS      ((uint8_t)0x04)    /**< SPI2_SS alternate function mapping */
#define GPIOF2_AF4_SPI2_SCK     ((uint8_t)0x04)    /**< SPI2_SCK alternate function mapping */
#define GPIOF3_AF4_SPI2_MISO    ((uint8_t)0x04)    /**< SPI2_MISO alternate function mapping */
#define GPIOF4_AF4_SPI2_MOSI    ((uint8_t)0x04)    /**< SPI2_MOSI alternate function mapping */

#define GPIOG12_AF4_SPI6_SS     ((uint8_t)0x04)    /**< SPI6_SS alternate function mapping */

#define GPIOH10_AF4_SPI2_SS     ((uint8_t)0x04)    /**< SPI2_SS alternate function mapping */
#define GPIOH11_AF4_SPI2_SCK    ((uint8_t)0x04)    /**< SPI2_SCK alternate function mapping */
#define GPIOH12_AF4_SPI2_MISO   ((uint8_t)0x04)    /**< SPI2_MISO alternate function mapping */
#define GPIOH13_AF4_SPI2_MOSI   ((uint8_t)0x04)    /**< SPI2_MOSI alternate function mapping */

#define GPIOH15_AF4_SAI_FS_A    ((uint8_t)0x04)    /**< SAI_FS_A alternate function mapping */

#define GPIOI0_AF4_SAI_BCK_A    ((uint8_t)0x04)    /**< SAI_BCK_A alternate function mapping */
#define GPIOI1_AF4_SAI_SD_B     ((uint8_t)0x04)    /**< SAI_SD_B alternate function mapping */
#define GPIOI2_AF4_SAI_SD_A     ((uint8_t)0x04)    /**< SAI_SD_A alternate function mapping */

#define GPIOI6_AF4_SPI2_SCK     ((uint8_t)0x04)    /**< SPI2_SCK alternate function mapping */
#define GPIOI9_AF4_SPI3_MOSI    ((uint8_t)0x04)    /**< SPI3_MOSI alternate function mapping */

#define GPIOJ2_AF4_SPI6_MISO    ((uint8_t)0x04)    /**< SPI6_MISO alternate function mapping */
#define GPIOJ3_AF4_SPI6_SCK     ((uint8_t)0x04)    /**< SPI6_SCK alternate function mapping */
#define GPIOJ4_AF4_SPI6_MOSI    ((uint8_t)0x04)    /**< SPI6_MOSI alternate function mapping */

#define GPIOJ11_AF4_SPI1_SCK    ((uint8_t)0x04)    /**< SPI1_SCK alternate function mapping */
#define GPIOJ12_AF4_SPI1_MISO   ((uint8_t)0x04)    /**< SPI1_MISO alternate function mapping */
#define GPIOJ13_AF4_SPI1_MOSI   ((uint8_t)0x04)    /**< SPI1_MOSI alternate function mapping */
#define GPIOK2_AF4_SPI2_SS      ((uint8_t)0x04)    /**< SPI2_SS alternate function mapping */


/**
  * @brief   AF 5 selection
  */
#define GPIOA0_AF5_SAI_MCLK_A   ((uint8_t)0x05)    /**< SAI_MCLK_A alternate function mapping */
#define GPIOA1_AF5_SAI_SD_B     ((uint8_t)0x05)    /**<SAI_SD_B alternate function mapping */
#define GPIOA2_AF5_SAI_FS_A     ((uint8_t)0x05)    /**< SAI_FS_A alternate function mapping */
#define GPIOA3_AF5_SAI_BCK_A    ((uint8_t)0x05)    /**< SAI_BCK_A alternate function mapping */
#define GPIOA4_AF5_SAI_SD_A     ((uint8_t)0x05)    /**< SAI_SD_A alternate function mapping */
#define GPIOB5_AF5_SAI_SD_B     ((uint8_t)0x05)    /**< SAI_SD_B alternate function mapping */
#define GPIOB6_AF5_SAI_MCLK_B   ((uint8_t)0x05)    /**<SAI_MCLK_B alternate function mapping */
#define GPIOB7_AF5_SAI_BCK_B    ((uint8_t)0x05)    /**< SAI_BCK_B alternate function mapping */
#define GPIOB8_AF5_SAI_FS_B     ((uint8_t)0x05)    /**<SAI_FS_B alternate function mapping */
#define GPIOB14_AF5_SAI_SD_A    ((uint8_t)0x05)    /**< SAI_SD_B alternate function mapping */
#define GPIOB15_AF5_SAI_SD_A    ((uint8_t)0x05)    /**< SAI_SD_A alternate function mapping */
#define GPIOC8_AF5_SPI3_SS      ((uint8_t)0x05)    /**< SPI3_SS alternate function mapping */
#define GPIOE7_AF5_SAI_BCK_A    ((uint8_t)0x05)    /**<SAI_BCK_A alternate function mapping */

#define GPIOF1_AF5_SAI_FS_A     ((uint8_t)0x05)    /**<SAI_FS_A alternate function mapping */
#define GPIOF2_AF5_SAI_BCK_A    ((uint8_t)0x05)    /**< SAI_BCK_A alternate function mapping */
#define GPIOF3_AF5_SAI_SD_B     ((uint8_t)0x05)    /**< SAI_SD_B alternate function mapping */
#define GPIOF4_AF5_SAI_SD_A     ((uint8_t)0x05)    /**<SAI_SD_A alternate function mapping */

#define GPIOG13_AF5_SAI_MCLK_A  ((uint8_t)0x05)    /**<SAI_MCLK_A alternate function mapping */
#define GPIOG14_AF5_SAI_MCLK_A  ((uint8_t)0x05)    /**< SAI_MCLK_A alternate function mapping */

#define GPIOH0_AF5_SAI_MCLK_A   ((uint8_t)0x05)    /**< SAI_MCLK_A alternate function mapping */
#define GPIOH10_AF5_SAI_FS_A    ((uint8_t)0x05)    /**<SAI_FS_A alternate function mapping */
#define GPIOH11_AF5_SAI_BCK_A    ((uint8_t)0x05)    /**<SAI_BCK_A alternate function mapping */
#define GPIOH12_AF5_SAI_SD_B    ((uint8_t)0x05)    /**<SAI_SD_B alternate function mapping */
#define GPIOH13_AF5_SAI_SD_A    ((uint8_t)0x05)    /**<SAI_SD_A alternate function mapping */

#define GPIOH15_AF5_SPI3_SS     ((uint8_t)0x05)    /**<SPI3_SS alternate function mapping */

#define GPIOI0_AF5_SPI3_SCK     ((uint8_t)0x05)    /**<SPI3_SCK alternate function mapping */
#define GPIOI1_AF5_SPI3_MISO    ((uint8_t)0x05)    /**<SPI3_MISO alternate function mapping */
#define GPIOI2_AF5_SPI3_MOSI    ((uint8_t)0x05)    /**<SPI3_MOSI alternate function mapping */
#define GPIOI6_AF5_SAI_BCK_A    ((uint8_t)0x05)    /**<SAI_BCK_A alternate function mapping */
#define GPIOI9_AF5_SAI_SD_A     ((uint8_t)0x05)    /**<SAI_SD_A alternate function mapping */

#define GPIOJ11_AF5_SPI3_SCK    ((uint8_t)0x05)    /**<SPI3_SCK alternate function mapping */
#define GPIOJ12_AF5_SPI3_MISO   ((uint8_t)0x05)    /**<SPI3_MISO alternate function mapping */
#define GPIOJ13_AF5_SPI3_MOSI   ((uint8_t)0x05)    /**<SPI3_MOSI alternate function mapping */
#define GPIOK2_AF5_SAI_FS_A     ((uint8_t)0x05)    /**<SAI_FS_A alternate function mapping */


/**
  * @brief   AF 6 selection
  */
#define GPIOC0_AF6_UART2_CTSJ   ((uint8_t)0x06)    /**< UART2_CTSJ alternate function mapping */
#define GPIOC1_AF6_UART2_RTSJ   ((uint8_t)0x06)    /**< UART2_RTSJ alternate function mapping */
#define GPIOC2_AF6_UART2_TX     ((uint8_t)0x06)    /**< UART2_TX alternate function mapping */

#define GPIOC7_AF6_UART2_RX     ((uint8_t)0x06)    /**< UART2_RX alternate function mapping */
#define GPIOC8_AF6_SAI_FS_A     ((uint8_t)0x06)    /**< SAI_FS_A alternate function mapping */

#define GPIOE5_AF6_UART3_RX     ((uint8_t)0x06)    /**< UART3_RX alternate function mapping */
#define GPIOE7_AF6_UART3_TX     ((uint8_t)0x06)    /**< UART3_TX alternate function mapping */

#define GPIOE8_AF6_UART3_RX     ((uint8_t)0x06)    /**< UART3_RX alternate function mapping */
#define GPIOE10_AF6_UART2_TX    ((uint8_t)0x06)    /**< UART2_TX alternate function mapping */
#define GPIOE12_AF6_UART2_RX    ((uint8_t)0x06)    /**< UART2_RX alternate function mapping */

#define GPIOF2_AF6_UART3_CTSJ   ((uint8_t)0x06)    /**< UART3_CTSJ alternate function mapping */
#define GPIOF3_AF6_UART3_RTSJ   ((uint8_t)0x06)    /**< UART3_RTSJ alternate function mapping */
#define GPIOF5_AF6_UART3_TX     ((uint8_t)0x06)    /**< UART3_TX alternate function mapping */
#define GPIOF6_AF6_UART3_RX     ((uint8_t)0x06)    /**< UART3_RX alternate function mapping */

#define GPIOF8_AF6_UART3_CTSJ   ((uint8_t)0x06)    /**< UART3_CTSJ alternate function mapping */
#define GPIOF9_AF6_UART3_RTSJ   ((uint8_t)0x06)    /**< UART3_RTSJ alternate function mapping */

#define GPIOH2_AF6_UART1_TX     ((uint8_t)0x06)    /**< UART1_TX alternate function mapping */
#define GPIOH3_AF6_UART1_RX     ((uint8_t)0x06)    /**< UART1_RX alternate function mapping */
#define GPIOH4_AF6_UART1_CTSJ   ((uint8_t)0x06)    /**< UART1_CTSJ alternate function mapping */
#define GPIOH5_AF6_UART1_RTSJ   ((uint8_t)0x06)    /**< UART1_RTSJ alternate function mapping */

#define GPIOI0_AF6_UART3_TX     ((uint8_t)0x06)    /**< UART3_TX alternate function mapping */
#define GPIOI1_AF6_UART3_RX     ((uint8_t)0x06)    /**< UART3_RX alternate function mapping */
#define GPIOI6_AF6_UART2_CTSJ   ((uint8_t)0x06)    /**< UART2_CTSJ alternate function mapping */
#define GPIOI7_AF6_UART2_RTSJ   ((uint8_t)0x06)    /**< UART2_RTSJ alternate function mapping */
#define GPIOI8_AF6_UART2_TX     ((uint8_t)0x06)    /**< UART2_TX alternate function mapping */
#define GPIOI9_AF6_UART2_RX     ((uint8_t)0x06)    /**< UART2_RX alternate function mapping */

#define GPIOJ10_AF6_UART1_RX    ((uint8_t)0x06)    /**< UART1_RX alternate function mapping */
#define GPIOJ11_AF6_SAI_BCK_A   ((uint8_t)0x06)    /**< SAI_BCK_A alternate function mapping */
#define GPIOJ12_AF6_SAI_SD_B    ((uint8_t)0x06)    /**< SAI_SD_B alternate function mapping */
#define GPIOJ13_AF6_SAI_SD_A    ((uint8_t)0x06)    /**< SAI_SD_A alternate function mapping */
#define GPIOJ14_AF6_UART1_TX    ((uint8_t)0x06)    /**< UART1_TX alternate function mapping */
#define GPIOJ15_AF6_UART1_RX    ((uint8_t)0x06)    /**< UART1_RX alternate function mapping */


/**
  * @brief   AF 7 selection
  */
#define GPIOA9_AF7_CAN1_RX      ((uint8_t)0x07)    /**< CAN1_RX alternate function mapping */
#define GPIOB5_AF7_CAN1_RX      ((uint8_t)0x07)    /**< UART7_RX alternate function mapping */
#define GPIOB6_AF7_CAN1_RX      ((uint8_t)0x07)    /**< UART7_TX alternate function mapping */
#define GPIOB7_AF7_TIM13_CH1    ((uint8_t)0x07)    /**< TIM13_CH1alternate function mapping */
#define GPIOB8_AF7_TIM14_CH1    ((uint8_t)0x07)    /**< TIM14_CH1 alternate function mapping */

#define GPIOC0_AF7_UART4_TX     ((uint8_t)0x07)    /**< UART4_TX alternate function mapping */
#define GPIOC1_AF7_UART4_RX     ((uint8_t)0x07)    /**< UART4_RX alternate function mapping */

#define GPIOC10_AF7_TIM13_CH1   ((uint8_t)0x07)    /**< TIM13_CH1 alternate function mapping */
#define GPIOC11_AF7_TIM14_CH1   ((uint8_t)0x07)    /**< TIM14_CH1 alternate function mapping */

#define GPIOD14_AF7_UART7_RX    ((uint8_t)0x07)    /**< UART7_RX alternate function mapping */
#define GPIOD15_AF7_UART7_TX    ((uint8_t)0x07)    /**< UART7_TX alternate function mapping */

#define GPIOE10_AF7_TIM12_CH1   ((uint8_t)0x07)    /**< TIM12_CH1 alternate function mapping */
#define GPIOE13_AF7_TIM12_CH2   ((uint8_t)0x07)    /**< TIM12_CH2 alternate function mapping */

#define GPIOF1_AF7_CAN2_RX      ((uint8_t)0x07)    /**< CAN2_RX alternate function mapping */
#define GPIOF2_AF7_CAN2_TX      ((uint8_t)0x07)    /**< CAN2_TX alternate function mapping */
#define GPIOF3_AF7_TIM12_CH1    ((uint8_t)0x07)    /**< TIM12_CH1 alternate function mapping */
#define GPIOF4_AF7_TIM12_CH2    ((uint8_t)0x07)    /**<TIM12_CH2 alternate function mapping */
#define GPIOF8_AF7_CAN2_DBG     ((uint8_t)0x07)    /**<CAN2_DBG alternate function mapping */

#define GPIOG12_AF7_UART6_RTSJ  ((uint8_t)0x07)    /**<UART6_RTSJ alternate function mapping */
#define GPIOG13_AF7_UART6_TX    ((uint8_t)0x07)    /**<UART6_TX alternate function mapping */
#define GPIOG14_AF7_UART6_RX    ((uint8_t)0x07)    /**<UART6_RX alternate function mapping */

#define GPIOH3_AF7_CAN1_DBG     ((uint8_t)0x07)    /**<CAN1_DBG alternate function mapping */
#define GPIOH4_AF7_CAN1_RX      ((uint8_t)0x07)    /**<CAN1_RX alternate function mapping */
#define GPIOH5_AF7_CAN1_TX      ((uint8_t)0x07)    /**<CAN1_TX alternate function mapping */
#define GPIOH7_AF7_CAN1_TX      ((uint8_t)0x07)    /**<CAN1_TX alternate function mapping */

#define GPIOI0_AF7_UART4_TX     ((uint8_t)0x07)    /**<UART4_TX alternate function mapping */
#define GPIOI1_AF7_UART4_RX     ((uint8_t)0x07)    /**<UART4_RX alternate function mapping */
#define GPIOI2_AF7_UART5_TX     ((uint8_t)0x07)    /**<UART5_TX alternate function mapping */
#define GPIOI3_AF7_CAN1_RX      ((uint8_t)0x07)    /**<CAN1_RX alternate function mapping */
#define GPIOI4_AF7_CAN1_TX      ((uint8_t)0x07)    /**<CAN1_TX alternate function mapping */
#define GPIOI5_AF7_UART5_RX     ((uint8_t)0x07)    /**<UART5_RX alternate function mapping */
#define GPIOI15_AF7_UART6_RX    ((uint8_t)0x07)    /**<UART6_RX alternate function mapping */

#define GPIOJ2_AF7_UART6_RTSJ   ((uint8_t)0x07)    /**<UART6_RTSJ alternate function mapping */
#define GPIOJ3_AF7_UART6_CTSJ   ((uint8_t)0x07)    /**<UART6_CTSJ alternate function mapping */
#define GPIOJ4_AF7_UART6_TX     ((uint8_t)0x07)    /**<UART6_TX alternate function mapping */

#define GPIOJ10_AF7_UART6_CTSJ  ((uint8_t)0x07)    /**<UART6_CTSJ alternate function mapping */
#define GPIOJ13_AF7_CAN2_RX     ((uint8_t)0x07)    /**<CAN2_RX alternate function mapping */
#define GPIOJ14_AF7_CAN2_TX     ((uint8_t)0x07)    /**<CAN2_TX alternate function mapping */

#define GPIOK1_AF7_CAN1_RX      ((uint8_t)0x07)    /**<CAN1_RX alternate function mapping */
#define GPIOK2_AF7_CAN1_TX      ((uint8_t)0x07)    /**<CAN1_TX alternate function mapping */
#define GPIOK3_AF7_UART8_RX     ((uint8_t)0x07)    /**<UART8_RXS alternate function mapping */
#define GPIOK4_AF7_UART8_TX     ((uint8_t)0x07)    /**<UART8_TX alternate function mapping */

/**
  * @brief   AF 8 selection
  */
#define GPIOA2_AF8_VOUT_B0      ((uint8_t)0x08)    /**< VOUT_B[0] alternate function mapping */
#define GPIOA3_AF8_VOUT_G0      ((uint8_t)0x08)    /**< VOUT_G[0] alternate function mapping */
#define GPIOA4_AF8_VOUT_G1      ((uint8_t)0x08)    /**< VOUT_G[1] alternate function mapping */

#define GPIOA9_AF8_VOUT_VSYNC   ((uint8_t)0x08)    /**< VOUT_VSYNC alternate function mapping */
#define GPIOA10_AF8_VOUT_HSYNC  ((uint8_t)0x08)    /**< VOUT_HSYNC alternate function mapping */
#define GPIOA15_AF8_VOUT_HSYNC  ((uint8_t)0x08)    /**< VOUT_HSYNC alternate function mapping */

#define GPIOB0_AF8_VOUT_VSYNC   ((uint8_t)0x08)    /**< VOUT_VSYNC alternate function mapping */
#define GPIOB9_AF8_VOUT_DE      ((uint8_t)0x08)    /**< VOUT_DE alternate function mapping */

#define GPIOC3_AF8_VOUT_R0      ((uint8_t)0x08)    /**< VOUT_R[0] alternate function mapping */
#define GPIOC4_AF8_VOUT_R1      ((uint8_t)0x08)    /**< VOUT_R[1] alternate function mapping */
#define GPIOC7_AF8_VOUT_B5      ((uint8_t)0x08)    /**< VOUT_B[5] alternate function mapping */
#define GPIOC8_AF8_VOUT_VSYNC   ((uint8_t)0x08)    /**< VOUT_VSYNC alternate function mapping */

#define GPIOC10_AF8_VOUT_G2     ((uint8_t)0x08)    /**< VOUT_G[2] alternate function mapping */
#define GPIOC14_AF8_VOUT_R3     ((uint8_t)0x08)    /**< VOUT_R[3] alternate function mapping */
#define GPIOC15_AF8_VOUT_R6     ((uint8_t)0x08)    /**< VOUT_R[6] alternate function mapping */

#define GPIOD1_AF8_VOUT_R0      ((uint8_t)0x08)    /**< VOUT_R[0] alternate function mapping */
#define GPIOD2_AF8_VOUT_R1      ((uint8_t)0x08)    /**< VOUT_R[1] alternate function mapping */
#define GPIOD3_AF8_VOUT_R2      ((uint8_t)0x08)    /**< VOUT_R[2] alternate function mapping */
#define GPIOD4_AF8_VOUT_R3      ((uint8_t)0x08)    /**<  VOUT_R[3] alternate function mapping */
#define GPIOD5_AF8_VOUT_R4      ((uint8_t)0x08)    /**<  VOUT_R[4] alternate function mapping */
#define GPIOD6_AF8_VOUT_R5      ((uint8_t)0x08)    /**< VOUT_R[5] alternate function mapping */


#define GPIOE2_AF8_VOUT_G3      ((uint8_t)0x08)    /**< VOUT_G[3] alternate function mapping */
#define GPIOE3_AF8_VOUT_B4      ((uint8_t)0x08)    /**< VOUT_B[4] alternate function mapping */
#define GPIOE4_AF8_VOUT_DE      ((uint8_t)0x08)    /**<  VOUT_DE] alternate function mapping */
#define GPIOE5_AF8_VOUT_OUT_CLK ((uint8_t)0x08)    /**<  VOUT_OUT_CLK alternate function mapping */
#define GPIOE6_AF8_VOUT_R7      ((uint8_t)0x08)    /**< VOUT_R[7] alternate function mapping */
#define GPIOE7_AF8_VOUT_G4      ((uint8_t)0x08)    /**< VOUT_G[4] alternate function mapping */
#define GPIOE8_AF8_VOUT_G5      ((uint8_t)0x08)    /**< VOUT_G[5]alternate function mapping */
#define GPIOE9_AF8_VOUT_R6      ((uint8_t)0x08)    /**<  VOUT_R[6] alternate function mapping */

#define GPIOE12_AF8_VOUT_R2     ((uint8_t)0x08)    /**<  VOUT_R[2]alternate function mapping */
#define GPIOE13_AF8_VOUT_R3     ((uint8_t)0x08)    /**< VOUT_R[3] alternate function mapping */
#define GPIOE14_AF8_VOUT_R4     ((uint8_t)0x08)    /**< VOUT_R[4]] alternate function mapping */
#define GPIOE15_AF8_VOUT_R5     ((uint8_t)0x08)    /**< VOUT_R[5]] alternate function mapping */

#define GPIOF0_AF8_VOUT_R6      ((uint8_t)0x08)    /**< VOUT_R[6]] alternate function mapping */
#define GPIOF7_AF8_VOUT_B3      ((uint8_t)0x08)    /**< VOUT_B[3]] alternate function mapping */

#define GPIOF13_VOUT_R7         ((uint8_t)0x08)
#define GPIOF14_VOUT_G0         ((uint8_t)0x08)
#define GPIOF15_VOUT_G1         ((uint8_t)0x08)
#define GPIOG0_VOUT_G2          ((uint8_t)0x08)
#define GPIOG1_VOUT_G3          ((uint8_t)0x08)
#define GPIOG2_VOUT_G4          ((uint8_t)0x08)
#define GPIOG3_VOUT_G5          ((uint8_t)0x08)
#define GPIOG4_VOUT_G6          ((uint8_t)0x08)
#define GPIOG5_VOUT_G7          ((uint8_t)0x08)

#define GPIOG10_VOUT_R7         ((uint8_t)0x08)
#define GPIOG11_VOUT_OUT_CLK    ((uint8_t)0x08)
#define GPIOG13_VOUT_HSYNC      ((uint8_t)0x08)
#define GPIOG14_VOUT_G6         ((uint8_t)0x08)
#define GPIOH1_VOUT_R6          ((uint8_t)0x08)
#define GPIOH4_VOUT_R4          ((uint8_t)0x08)
#define GPIOH5_VOUT_R5          ((uint8_t)0x08)

#define GPIOH7_VOUT_G2          ((uint8_t)0x08)
#define GPIOH8_VOUT_G3          ((uint8_t)0x08)
#define GPIOH9_VOUT_G4          ((uint8_t)0x08)
#define GPIOH10_VOUT_G5         ((uint8_t)0x08)
#define GPIOH11_VOUT_G6         ((uint8_t)0x08)
#define GPIOH12_VOUT_G7         ((uint8_t)0x08)

#define GPIOI0_VOUT_R2          ((uint8_t)0x08)
#define GPIOI6_VOUT_G7          ((uint8_t)0x08)
#define GPIOI9_VOUT_B2          ((uint8_t)0x08)
#define GPIOI11_VOUT_B0         ((uint8_t)0x08)
#define GPIOI12_VOUT_B1         ((uint8_t)0x08)
#define GPIOI13_VOUT_B2         ((uint8_t)0x08)
#define GPIOI14_VOUT_B3         ((uint8_t)0x08)
#define GPIOJ0_VOUT_B2          ((uint8_t)0x08)
#define GPIOJ1_VOUT_B3          ((uint8_t)0x08)
#define GPIOJ2_VOUT_B1          ((uint8_t)0x08)
#define GPIOJ5_VOUT_B4          ((uint8_t)0x08)
#define GPIOJ6_VOUT_B5          ((uint8_t)0x08)
#define GPIOJ7_VOUT_B6          ((uint8_t)0x08)
#define GPIOJ8_VOUT_B7          ((uint8_t)0x08)
#define GPIOJ9_VOUT_DE          ((uint8_t)0x08)
#define GPIOK1_VOUT_B6          ((uint8_t)0x08)
#define GPIOK2_VOUT_B7          ((uint8_t)0x08)
#define GPIOK5_VOUT_B4          ((uint8_t)0x08)
#define GPIOK6_VOUT_B5          ((uint8_t)0x08)
#define GPIOK7_VOUT_B6          ((uint8_t)0x08)
#define GPIOK8_VOUT_B7          ((uint8_t)0x08)

/**
  * @brief   AF 9 selection
  */
#define GPIOA0_AF9_PES_CH_EV_0        ((uint8_t)0x09)    /**< PES_CH_EV[0] alternate function mapping */
#define GPIOA1_AF9_PES_CH_EV_1        ((uint8_t)0x09)    /**< PES_CH_EV[1] alternate function mapping */
#define GPIOA2_AF9_PES_CH_EV_2        ((uint8_t)0x09)    /**< PES_CH_EV[2] alternate function mapping */
#define GPIOA3_AF9_PES_CH_EV_3        ((uint8_t)0x09)    /**< PES_CH_EV[3] alternate function mapping */
#define GPIOA4_AF9_PES_CH_EV_4        ((uint8_t)0x09)    /**< PES_CH_EV[4] alternate function mapping */
#define GPIOA5_AF9_PES_CH_EV_5        ((uint8_t)0x09)    /**<PES_CH_EV[5] alternate function mapping */
#define GPIOA6_AF9_PES_CH_EV_6        ((uint8_t)0x09)    /**< PES_CH_EV6] alternate function mapping */
#define GPIOA7_AF9_PES_CH_EV_7        ((uint8_t)0x09)    /**<PES_CH_EV[7] alternate function mapping */
#define GPIOA8_AF9_PES_CH_EV_8        ((uint8_t)0x09)    /**<PES_CH_EV[8] alternate function mapping */
#define GPIOA9_AF9_PES_CH_EV_9        ((uint8_t)0x09)    /**< PES_CH_EV[9] alternate function mapping */
#define GPIOA10_AF9_PES_CH_EV_10      ((uint8_t)0x09)    /**< PES_CH_EV[10] alternate function mapping */
#define GPIOA11_AF9_PES_CH_EV_11      ((uint8_t)0x09)    /**< PES_CH_EV[111] alternate function mapping */
#define GPIOA12_AF9_PES_CH_EV_0       ((uint8_t)0x09)    /**< PES_CH_EV[0] alternate function mapping */
#define GPIOA13_AF9_PES_CH_EV_1       ((uint8_t)0x09)    /**< PES_CH_EV[1] alternate function mapping */
#define GPIOA14_AF9_PES_CH_EV_2       ((uint8_t)0x09)    /**< PES_CH_EV[2] alternate function mapping */
#define GPIOA15_AF9_PES_CH_EV_3       ((uint8_t)0x09)    /**< PES_CH_EV[3] alternate function mapping */
#define GPIOB0_AF9_PES_CH_EV4         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB1_AF9_PES_CH_EV5         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB2_AF9_PES_CH_EV6         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB3_AF9_PES_CH_EV7         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB4_AF9_PES_CH_EV8         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB5_AF9_PES_CH_EV9         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB6_AF9_PES_CH_EV10        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB7_AF9_PES_CH_EV11        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB8_AF9_PES_CH_EV0         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB9_AF9_PES_CH_EV1         ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB10_AF9_PES_CH_EV2        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB11_AF9_PES_CH_EV3        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB12_AF9_PES_CH_EV4        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB13_AF9_PES_CH_EV5        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB14_AF9_PES_CH_EV6        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */
#define GPIOB15_AF9_PES_CH_EV7        ((uint8_t)0x09)    /**< ACMP0 OUT alternate function mapping */

#define GPIOC0_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOC1_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOC2_PES_CH_EV10          ((uint8_t)0x09)
#define GPIOC3_PES_CH_EV11          ((uint8_t)0x09)
#define GPIOC4_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOC5_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOC6_PES_CH_EV2           ((uint8_t)0x09)
#define GPIOC7_PES_CH_EV3           ((uint8_t)0x09)
#define GPIOC8_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOC9_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOC10_PES_CH_EV6          ((uint8_t)0x09)
#define GPIOC11_PES_CH_EV7          ((uint8_t)0x09)
#define GPIOC12_PES_CH_EV8          ((uint8_t)0x09)
#define GPIOC13_PES_CH_EV9          ((uint8_t)0x09)
#define GPIOC14_PES_CH_EV10         ((uint8_t)0x09)
#define GPIOC15_PES_CH_EV11         ((uint8_t)0x09)
#define GPIOD0_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOD1_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOD2_PES_CH_EV2           ((uint8_t)0x09)
#define GPIOD3_PES_CH_EV3           ((uint8_t)0x09)
#define GPIOD4_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOD5_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOD6_PES_CH_EV6           ((uint8_t)0x09)
#define GPIOD7_PES_CH_EV7           ((uint8_t)0x09)
#define GPIOD8_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOD9_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOD10_PES_CH_EV10         ((uint8_t)0x09)
#define GPIOD11_PES_CH_EV11         ((uint8_t)0x09)
#define GPIOD12_PES_CH_EV0          ((uint8_t)0x09)
#define GPIOD13_PES_CH_EV1          ((uint8_t)0x09)
#define GPIOD14_PES_CH_EV2          ((uint8_t)0x09)
#define GPIOD15_PES_CH_EV3          ((uint8_t)0x09)
#define GPIOE0_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOE1_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOE2_PES_CH_EV6           ((uint8_t)0x09)
#define GPIOE3_PES_CH_EV7           ((uint8_t)0x09)
#define GPIOE4_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOE5_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOE6_PES_CH_EV10          ((uint8_t)0x09)
#define GPIOE7_PES_CH_EV11          ((uint8_t)0x09)
#define GPIOE8_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOE9_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOE10_PES_CH_EV2          ((uint8_t)0x09)
#define GPIOE11_PES_CH_EV3          ((uint8_t)0x09)
#define GPIOE12_PES_CH_EV4          ((uint8_t)0x09)
#define GPIOE13_PES_CH_EV5          ((uint8_t)0x09)
#define GPIOE14_PES_CH_EV6          ((uint8_t)0x09)
#define GPIOE15_PES_CH_EV7          ((uint8_t)0x09)

#define GPIOF0_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOF1_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOF2_PES_CH_EV10          ((uint8_t)0x09)
#define GPIOF3_PES_CH_EV11          ((uint8_t)0x09)
#define GPIOF4_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOF5_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOF6_PES_CH_EV2           ((uint8_t)0x09)
#define GPIOF7_PES_CH_EV3           ((uint8_t)0x09)
#define GPIOF8_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOF9_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOF10_PES_CH_EV6          ((uint8_t)0x09)
#define GPIOF11_PES_CH_EV7          ((uint8_t)0x09)
#define GPIOF12_PES_CH_EV8          ((uint8_t)0x09)
#define GPIOF13_PES_CH_EV9          ((uint8_t)0x09)
#define GPIOF14_PES_CH_EV10         ((uint8_t)0x09)
#define GPIOF15_PES_CH_EV11         ((uint8_t)0x09)

#define GPIOG0_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOG1_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOG2_PES_CH_EV2           ((uint8_t)0x09)
#define GPIOG3_PES_CH_EV3           ((uint8_t)0x09)
#define GPIOG4_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOG5_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOG6_PES_CH_EV6           ((uint8_t)0x09)
#define GPIOG7_PES_CH_EV7           ((uint8_t)0x09)
#define GPIOG8_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOG9_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOG10_PES_CH_EV10         ((uint8_t)0x09)
#define GPIOG11_PES_CH_EV11         ((uint8_t)0x09)
#define GPIOG12_PES_CH_EV0          ((uint8_t)0x09)
#define GPIOG13_PES_CH_EV1          ((uint8_t)0x09)
#define GPIOG14_PES_CH_EV2          ((uint8_t)0x09)
#define GPIOG15_PES_CH_EV3          ((uint8_t)0x09)

#define GPIOH0_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOH1_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOH2_PES_CH_EV6           ((uint8_t)0x09)
#define GPIOH3_PES_CH_EV7           ((uint8_t)0x09)
#define GPIOH4_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOH5_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOH6_PES_CH_EV10          ((uint8_t)0x09)
#define GPIOH7_PES_CH_EV11          ((uint8_t)0x09)
#define GPIOH8_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOH9_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOH10_PES_CH_EV2          ((uint8_t)0x09)
#define GPIOH11_PES_CH_EV3          ((uint8_t)0x09)
#define GPIOH12_PES_CH_EV4          ((uint8_t)0x09)
#define GPIOH13_PES_CH_EV5          ((uint8_t)0x09)
#define GPIOH14_PES_CH_EV6          ((uint8_t)0x09)
#define GPIOH15_PES_CH_EV7          ((uint8_t)0x09)

#define GPIOI0_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOI1_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOI2_PES_CH_EV10          ((uint8_t)0x09)
#define GPIOI3_PES_CH_EV11          ((uint8_t)0x09)
#define GPIOI4_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOI5_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOI6_PES_CH_EV2           ((uint8_t)0x09)
#define GPIOI7_PES_CH_EV3           ((uint8_t)0x09)
#define GPIOI8_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOI9_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOI10_PES_CH_EV6          ((uint8_t)0x09)
#define GPIOI11_PES_CH_EV7          ((uint8_t)0x09)
#define GPIOI12_PES_CH_EV8          ((uint8_t)0x09)
#define GPIOI13_PES_CH_EV9          ((uint8_t)0x09)
#define GPIOI14_PES_CH_EV10         ((uint8_t)0x09)
#define GPIOI15_PES_CH_EV11         ((uint8_t)0x09)

#define GPIOJ0_PES_CH_EV0           ((uint8_t)0x09)
#define GPIOJ1_PES_CH_EV1           ((uint8_t)0x09)
#define GPIOJ2_PES_CH_EV2           ((uint8_t)0x09)
#define GPIOJ3_PES_CH_EV3           ((uint8_t)0x09)
#define GPIOJ4_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOJ5_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOJ6_PES_CH_EV6           ((uint8_t)0x09)
#define GPIOJ7_PES_CH_EV7           ((uint8_t)0x09)
#define GPIOJ8_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOJ9_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOJ10_PES_CH_EV10         ((uint8_t)0x09)
#define GPIOJ11_PES_CH_EV11         ((uint8_t)0x09)
#define GPIOJ12_PES_CH_EV0          ((uint8_t)0x09)
#define GPIOJ13_PES_CH_EV1          ((uint8_t)0x09)
#define GPIOJ14_PES_CH_EV2          ((uint8_t)0x09)
#define GPIOJ15_PES_CH_EV3          ((uint8_t)0x09)

#define GPIOK1_PES_CH_EV4           ((uint8_t)0x09)
#define GPIOK2_PES_CH_EV5           ((uint8_t)0x09)
#define GPIOK3_PES_CH_EV6           ((uint8_t)0x09)
#define GPIOK4_PES_CH_EV7           ((uint8_t)0x09)
#define GPIOK5_PES_CH_EV8           ((uint8_t)0x09)
#define GPIOK6_PES_CH_EV9           ((uint8_t)0x09)
#define GPIOK7_PES_CH_EV10          ((uint8_t)0x09)
#define GPIOK8_PES_CH_EV11          ((uint8_t)0x09)

/**
  * @brief   AF 10 selection
  */
#define GPIOA0_AF10_PMEM_A23          ((uint8_t)0x0A)
#define GPIOA1_AF10_PMEM_A19          ((uint8_t)0x0A)
#define GPIOA2_AF10_PMEM_A20          ((uint8_t)0x0A)
#define GPIOA3_AF10_PMEM_A21          ((uint8_t)0x0A)
#define GPIOA4_AF10_PMEM_A22          ((uint8_t)0x0A)
#define GPIOA9_AF10_PMEM_D30          ((uint8_t)0x0A)
#define GPIOA10_AF10_PMEM_D31         ((uint8_t)0x0A)
#define GPIOA12_AF10_PMEM_A0          ((uint8_t)0x0A)
#define GPIOA13_AF10_PMEM_A1          ((uint8_t)0x0A)
#define GPIOA14_AF10_PMEM_A2          ((uint8_t)0x0A)
#define GPIOB2_AF10_PMEM_A3           ((uint8_t)0x0A)
#define GPIOB3_AF10_PMEM_A4           ((uint8_t)0x0A)
#define GPIOB4_AF10_PMEM_A5           ((uint8_t)0x0A)
#define GPIOC14_AF10_ACMP_OUT         ((uint8_t)0x0A)
#define GPIOD8_AF10_PMEM_A6           ((uint8_t)0x0A)
#define GPIOD9_AF10_PMEM_A7           ((uint8_t)0x0A)
#define GPIOD10_AF10_PMEM_A8          ((uint8_t)0x0A)
#define GPIOD11_AF10_PMEM_A9          ((uint8_t)0x0A)
#define GPIOD12_AF10_PMEM_A10         ((uint8_t)0x0A)
#define GPIOD13_AF10_PMEM_A11         ((uint8_t)0x0A)
#define GPIOD14_AF10_PMEM_D4          ((uint8_t)0x0A)
#define GPIOD15_AF10_PMEM_D5          ((uint8_t)0x0A)
#define GPIOE0_AF10_PMEM_D6           ((uint8_t)0x0A)
#define GPIOE1_AF10_PMEM_D7           ((uint8_t)0x0A)
#define GPIOE2_AF10_PMEM_D8           ((uint8_t)0x0A)
#define GPIOE3_AF10_PMEM_D9           ((uint8_t)0x0A)
#define GPIOE4_AF10_PMEM_D10          ((uint8_t)0x0A)
#define GPIOE5_AF10_PMEM_D11          ((uint8_t)0x0A)
#define GPIOE6_AF10_PMEM_D12          ((uint8_t)0x0A)
#define GPIOE7_AF10_ACMP_OUT          ((uint8_t)0x0A)
#define GPIOE12_AF10_PMEM_D16         ((uint8_t)0x0A)
#define GPIOE13_AF10_PMEM_D17         ((uint8_t)0x0A)
#define GPIOE14_AF10_PMEM_D18         ((uint8_t)0x0A)
#define GPIOE15_AF10_PMEM_D19         ((uint8_t)0x0A)
#define GPIOF0_AF10_PMEM_D20          ((uint8_t)0x0A)
#define GPIOF5_AF10_PMEM_D13          ((uint8_t)0x0A)
#define GPIOF6_AF10_PMEM_D14          ((uint8_t)0x0A)
#define GPIOF7_AF10_PMEM_D15          ((uint8_t)0x0A)
#define GPIOF8_AF10_PMEM_D16          ((uint8_t)0x0A)
#define GPIOF9_AF10_PMEM_D17          ((uint8_t)0x0A)
#define GPIOF10_AF10_PMEM_D18         ((uint8_t)0x0A)
#define GPIOF11_AF10_PMEM_D0          ((uint8_t)0x0A)
#define GPIOF12_AF10_PMEM_D1          ((uint8_t)0x0A)
#define GPIOF13_AF10_SDMMC_CLK        ((uint8_t)0x0A)
#define GPIOF14_AF10_SDMMC_D0         ((uint8_t)0x0A)
#define GPIOF15_AF10_SDMMC_D1         ((uint8_t)0x0A)
#define GPIOG0_AF10_SDMMC_CMD         ((uint8_t)0x0A)
#define GPIOG1_AF10_SDMMC_D2          ((uint8_t)0x0A)
#define GPIOG2_AF10_SDMMC_D3          ((uint8_t)0x0A)
#define GPIOG3_AF10_SDMMC_D4          ((uint8_t)0x0A)
#define GPIOG4_AF10_SDMMC_D5          ((uint8_t)0x0A)
#define GPIOG6_AF10_PMEM_A12          ((uint8_t)0x0A)
#define GPIOG7_AF10_PMEM_A13          ((uint8_t)0x0A)
#define GPIOG8_AF10_PMEM_A14          ((uint8_t)0x0A)
#define GPIOG9_AF10_PMEM_A15          ((uint8_t)0x0A)
#define GPIOG13_AF10_SDMMC_D6         ((uint8_t)0x0A)
#define GPIOG14_AF10_SDMMC_D7         ((uint8_t)0x0A)
#define GPIOG15_AF10_SDMMC_D0         ((uint8_t)0x0A)
#define GPIOH0_AF10_SDMMC_D1          ((uint8_t)0x0A)
#define GPIOH1_AF10_OTG_FS_SOF        ((uint8_t)0x0A)
#define GPIOH7_AF10_PMEM_D21          ((uint8_t)0x0A)
#define GPIOH8_AF10_PMEM_D22          ((uint8_t)0x0A)
#define GPIOH9_AF10_PMEM_D23          ((uint8_t)0x0A)
#define GPIOH10_AF10_PMEM_D24         ((uint8_t)0x0A)
#define GPIOH11_AF10_PMEM_D25         ((uint8_t)0x0A)
#define GPIOH12_AF10_PMEM_D26         ((uint8_t)0x0A)
#define GPIOH13_AF10_PMEM_D27         ((uint8_t)0x0A)
#define GPIOI0_AF10_SDMMC_D2          ((uint8_t)0x0A)
#define GPIOI1_AF10_SDMMC_D3          ((uint8_t)0x0A)
#define GPIOI2_AF10_SDMMC_CLK         ((uint8_t)0x0A)
#define GPIOI3_AF10_PMEM_D2           ((uint8_t)0x0A)
#define GPIOI4_AF10_PMEM_D3           ((uint8_t)0x0A)
#define GPIOI5_AF10_SDMMC_CMD         ((uint8_t)0x0A)
#define GPIOI6_AF10_PMEM_CLK          ((uint8_t)0x0A)
#define GPIOI7_AF10_PMEM_NOE          ((uint8_t)0x0A)
#define GPIOI8_AF10_PMEM_NWE          ((uint8_t)0x0A)
#define GPIOI9_AF10_PMEM_NWAIT        ((uint8_t)0x0A)
#define GPIOI10_AF10_PMEM_NE1         ((uint8_t)0x0A)
#define GPIOI15_AF10_PMEM_NE2         ((uint8_t)0x0A)
#define GPIOJ0_AF10_PMEM_NE3          ((uint8_t)0x0A)
#define GPIOJ2_AF10_PMEM_NE4          ((uint8_t)0x0A)
#define GPIOJ3_AF10_PMEM_A24          ((uint8_t)0x0A)
#define GPIOJ4_AF10_PMEM_A25          ((uint8_t)0x0A)
#define GPIOJ15_AF10_PMEM_NL          ((uint8_t)0x0A)
#define GPIOK1_AF10_SDMMC_D4          ((uint8_t)0x0A)
#define GPIOK2_AF10_SDMMC_D5          ((uint8_t)0x0A)
#define GPIOK3_AF10_NBL0              ((uint8_t)0x0A)
#define GPIOK4_AF10_NBL1              ((uint8_t)0x0A)
#define GPIOK5_AF10_NBL2              ((uint8_t)0x0A)
#define GPIOK6_AF10_NBL3              ((uint8_t)0x0A)
#define GPIOK7_AF10_D28               ((uint8_t)0x0A)
#define GPIOK8_AF10_D29               ((uint8_t)0x0A)

/**
  * @brief   AF 11 selection
  */
#define GPIOA2_AF11_VIN_D4            ((uint8_t)0x0B)    /**< VIN_D[4] alternate function mapping */
#define GPIOA3_AF11_VIN_D6            ((uint8_t)0x0B)    /**< VIN_D[6] alternate function mapping */
#define GPIOA4_AF11_VIN_D7            ((uint8_t)0x0B)    /**< VIN_D[7] alternate function mapping */
#define GPIOB9_AF11_VIN_D11           ((uint8_t)0x0B)    /**< VIN_D[11] alternate function mapping */
#define GPIOC8_AF11_VIN_HSYNC         ((uint8_t)0x0B)
#define GPIOC10_AF11_VIN_PIXCLK       ((uint8_t)0x0B)
#define GPIOD7_AF11_VIN_D12           ((uint8_t)0x0B)
#define GPIOD14_AF11_NF_DAT4          ((uint8_t)0x0B)
#define GPIOD15_AF11_NF_DAT5          ((uint8_t)0x0B)
#define GPIOE0_AF11_NF_DAT6           ((uint8_t)0x0B)
#define GPIOE1_AF11_NF_DAT7           ((uint8_t)0x0B)
#define GPIOE10_AF11_NF_DAT6          ((uint8_t)0x0B)
#define GPIOE11_AF11_NF_DAT7          ((uint8_t)0x0B)
#define GPIOE12_AF11_NF_VIN_HSYNC     ((uint8_t)0x0B)
#define GPIOE13_AF11_NF_D0            ((uint8_t)0x0B)
#define GPIOE14_AF11_NF_D1            ((uint8_t)0x0B)
#define GPIOE15_AF11_NF_D2            ((uint8_t)0x0B)
#define GPIOF0_AF11_NF_D3             ((uint8_t)0x0B)
#define GPIOF1_AF11_NF_DQS            ((uint8_t)0x0B)
#define GPIOF8_AF11_NF_CLE            ((uint8_t)0x0B)
#define GPIOF9_AF11_NF_ALE            ((uint8_t)0x0B)
#define GPIOF10_AF11_NF_WPJ           ((uint8_t)0x0B)
#define GPIOF11_AF11_NF_DAT0          ((uint8_t)0x0B)
#define GPIOF12_AF11_NF_DAT1          ((uint8_t)0x0B)
#define GPIOF13_AF11_NF_DAT0          ((uint8_t)0x0B)
#define GPIOF14_AF11_NF_DAT1          ((uint8_t)0x0B)
#define GPIOF15_AF11_NF_DAT2          ((uint8_t)0x0B)
#define GPIOG0_AF11_NF_DAT3           ((uint8_t)0x0B)
#define GPIOG1_AF11_NF_DAT4           ((uint8_t)0x0B)
#define GPIOG2_AF11_NF_DAT5           ((uint8_t)0x0B)
#define GPIOG3_AF11_NF_DAT6           ((uint8_t)0x0B)
#define GPIOG4_AF11_NF_DAT7           ((uint8_t)0x0B)

#define GPIOG10_AF11_VIN_D12          ((uint8_t)0x0B)
#define GPIOG11_AF11_VIN_D13          ((uint8_t)0x0B)
#define GPIOG13_AF11_VIN_D0           ((uint8_t)0x0B)
#define GPIOG14_AF11_VIN_D1           ((uint8_t)0x0B)
#define GPIOG15_AF11_VIN_D2           ((uint8_t)0x0B)
#define GPIOH0_AF11_VIN_D3            ((uint8_t)0x0B)
#define GPIOH1_AF11_D_USB_VBUSEN      ((uint8_t)0x0B)
#define GPIOH2_AF11_VIN_D0          ((uint8_t)0x0B)
#define GPIOH3_AF11_VIN_D1          ((uint8_t)0x0B)
#define GPIOH8_AF11_VIN_D4          ((uint8_t)0x0B)
#define GPIOH9_AF11_VIN_D11         ((uint8_t)0x0B)
#define GPIOH10_AF11_VIN_D13        ((uint8_t)0x0B)
#define GPIOH11_AF11_VIN_D8         ((uint8_t)0x0B)
#define GPIOH12_AF11_VIN_D9         ((uint8_t)0x0B)
#define GPIOH13_AF11_VIN_D10        ((uint8_t)0x0B)
#define GPIOI0_AF11_VIN_D8          ((uint8_t)0x0B)
#define GPIOI1_AF11_VIN_D4          ((uint8_t)0x0B)
#define GPIOI2_AF11_VIN_D9          ((uint8_t)0x0B)
#define GPIOI3_AF11_NF_DAT2         ((uint8_t)0x0B)
#define GPIOI4_AF11_NF_DAT3         ((uint8_t)0x0B)
#define GPIOI5_AF11_VIN_D11         ((uint8_t)0x0B)
#define GPIOI6_AF11_VIN_D5          ((uint8_t)0x0B)
#define GPIOI7_AF11_NF_REJ          ((uint8_t)0x0B)
#define GPIOI8_AF11_NF_WEJ          ((uint8_t)0x0B)
#define GPIOI9_AF11_VIN_D10         ((uint8_t)0x0B)
#define GPIOI10_AF11_NF_CEJ         ((uint8_t)0x0B)
#define GPIOI15_AF11_VIN_VSYNC      ((uint8_t)0x0B)
#define GPIOJ0_AF11_VIN_D2          ((uint8_t)0x0B)
#define GPIOJ1_AF11_VIN_D3          ((uint8_t)0x0B)
#define GPIOJ10_AF11_VIN_D13        ((uint8_t)0x0B)
#define GPIOJ13_AF11_VIN_D10        ((uint8_t)0x0B)
#define GPIOJ14_AF11_VIN_D5         ((uint8_t)0x0B)
#define GPIOJ15_AF11_VIN_VSYNC      ((uint8_t)0x0B)
#define GPIOK1_AF11_VIN_D6          ((uint8_t)0x0B)
#define GPIOK2_AF11_VIN_D7          ((uint8_t)0x0B)
#define GPIOK3_AF11_VIN_D2          ((uint8_t)0x0B)
#define GPIOK4_AF11_VIN_D3          ((uint8_t)0x0B)
#define GPIOK5_AF11_VIN_D5          ((uint8_t)0x0B)
#define GPIOK6_AF11_VIN_VSYNC       ((uint8_t)0x0B)
#define GPIOK7_AF11_VIN_D6          ((uint8_t)0x0B)
#define GPIOK8_AF11_VIN_D7          ((uint8_t)0x0B)

/**
  * @brief   AF 12 selection
  */
#define GPIOA9_AF12_SDRAM_D30       ((uint8_t)0x0C)
#define GPIOA10_SDRAM_D31           ((uint8_t)0x0C)
#define GPIOA12_SDRAM_A0            ((uint8_t)0x0C)
#define GPIOA13_SDRAM_A1            ((uint8_t)0x0C)
#define GPIOA14_SDRAM_A2            ((uint8_t)0x0C)
#define GPIOB2_SDRAM_A3             ((uint8_t)0x0C)
#define GPIOB3_SDRAM_A4             ((uint8_t)0x0C)
#define GPIOB4_SDRAM_A5             ((uint8_t)0x0C)
#define GPIOB12_SDRAM_SDNWE         ((uint8_t)0x0C)
#define GPIOB14_SDRAM_SDNE0         ((uint8_t)0x0C)
#define GPIOB15_SDRAM_SDCKE0        ((uint8_t)0x0C)
#define GPIOC3_SDRAM_SDCKE0         ((uint8_t)0x0C)
#define GPIOC4_SDRAM_SDNE0          ((uint8_t)0x0C)
#define GPIOC6_SDRAM_SDNWE          ((uint8_t)0x0C)
#define GPIOD7_SDRAM_SDNRAS         ((uint8_t)0x0C)
#define GPIOD8_SDRAM_A6             ((uint8_t)0x0C)
#define GPIOD9_SDRAM_A7             ((uint8_t)0x0C)
#define GPIOD10_SDRAM_A8            ((uint8_t)0x0C)
#define GPIOD11_SDRAM_A9
#define GPIOD12_SDRAM_A10           ((uint8_t)0x0C)
#define GPIOD13_SDRAM_A11           ((uint8_t)0x0C)
#define GPIOD14_SDRAM_D4            ((uint8_t)0x0C)
#define GPIOD15_SDRAM_D5            ((uint8_t)0x0C)
#define GPIOE0_SDRAM_D6             ((uint8_t)0x0C)
#define GPIOE1_SDRAM_D7             ((uint8_t)0x0C)
#define GPIOE2_SDRAM_D8             ((uint8_t)0x0C)
#define GPIOE3_SDRAM_D9             ((uint8_t)0x0C)
#define GPIOE4_SDRAM_D10            ((uint8_t)0x0C)
#define GPIOE5_SDRAM_D11            ((uint8_t)0x0C)
#define GPIOE6_SDRAM_D12            ((uint8_t)0x0C)
#define GPIOE10_SDRAM_SDNE1         ((uint8_t)0x0C)
#define GPIOE11_SDRAM_SDCKE1        ((uint8_t)0x0C)
#define GPIOE12_SDRAM_D16           ((uint8_t)0x0C)
#define GPIOE13_SDRAM_D17           ((uint8_t)0x0C)
#define GPIOE14_SDRAM_D18           ((uint8_t)0x0C)
#define GPIOE15_SDRAM_D19           ((uint8_t)0x0C)
#define GPIOF0_SDRAM_D20            ((uint8_t)0x0C)
#define GPIOF5_SDRAM_D13            ((uint8_t)0x0C)
#define GPIOF6_SDRAM_D14            ((uint8_t)0x0C)
#define GPIOF7_SDRAM_D15            ((uint8_t)0x0C)
#define GPIOF8_EXT_SFLASH_CSJ       ((uint8_t)0x0C)
#define GPIOF9_EXT_SFLASH_WJ        ((uint8_t)0x0C)

#define GPIOF11_SDRAM_D0            ((uint8_t)0x0C)
#define GPIOF12_SDRAM_D1            ((uint8_t)0x0C)
#define GPIOG6_SDRAM_A12            ((uint8_t)0x0C)
#define GPIOG8_SDRAM_BA0            ((uint8_t)0x0C)
#define GPIOG9_SDRAM_BA1            ((uint8_t)0x0C)
#define GPIOG10_NF_RBJ              ((uint8_t)0x0C)
#define GPIOG12_SDRAM_SDCLK         ((uint8_t)0x0C)
#define GPIOG13_EXT_SFLASH_HOLDJ    ((uint8_t)0x0C)
#define GPIOG14_EXT_SFLASH_SCK      ((uint8_t)0x0C)
#define GPIOG15_EXT_SFLASH_MOSI     ((uint8_t)0x0C)
#define GPIOH0_EXT_SFLASH_MISO      ((uint8_t)0x0C)

#define GPIOH7_EXT_SDRAM_D21        ((uint8_t)0x0C)
#define GPIOH8_EXT_SDRAM_D22        ((uint8_t)0x0C)
#define GPIOH9_EXT_SDRAM_D23        ((uint8_t)0x0C)
#define GPIOH10_EXT_SDRAM_D24       ((uint8_t)0x0C)
#define GPIOH11_EXT_SDRAM_D25       ((uint8_t)0x0C)
#define GPIOH12_EXT_SDRAM_D26       ((uint8_t)0x0C)
#define GPIOH13_EXT_SDRAM_D27       ((uint8_t)0x0C)
#define GPIOI3_SDRAM_D2             ((uint8_t)0x0C)
#define GPIOI4_SDRAM_D3             ((uint8_t)0x0C)
#define GPIOI9_NF_WPJ               ((uint8_t)0x0C)
#define GPIOJ0_VOUT_G3              ((uint8_t)0x0C)

#define GPIOJ2_VOUT_B4              ((uint8_t)0x0C)
#define GPIOJ10_SDRAM_SDNCAS        ((uint8_t)0x0C)
#define GPIOJ13_SDRAM_SDCKE1        ((uint8_t)0x0C)
#define GPIOJ14_SDRAM_SDNE1         ((uint8_t)0x0C)
#define GPIOK3_SDRAM_NBL0           ((uint8_t)0x0C)
#define GPIOK4_SDRAM_NBL1           ((uint8_t)0x0C)
#define GPIOK5_SDRAM_NBL2           ((uint8_t)0x0C)
#define GPIOK6_SDRAM_NBL3           ((uint8_t)0x0C)
#define GPIOK7_SDRAM_D28            ((uint8_t)0x0C)
#define GPIOK8_SDRAM_D29            ((uint8_t)0x0C)

/**
  * @brief   AF 13 selection
  */
#define GPIOA0_AF13_VIN_D8          ((uint8_t)0x0D)    /**< VIN_D[8] alternate function mapping */
#define GPIOA1_AF13_SAI_MCLK_A      ((uint8_t)0x0D)    /**< SAI_MCLK_A alternate function mapping */
#define GPIOA2_AF13_SAI_FS_A        ((uint8_t)0x0D)    /**< SAI_FS_A alternate function mapping */
#define GPIOA3_AF13_SAI_BCK_A       ((uint8_t)0x0D)    /**< SAI_BCK_A alternate function mapping */
#define GPIOA4_AF13_SAI_SD_A        ((uint8_t)0x0D)    /**< SAI_SD_A alternate function mapping */

#define GPIOA12_AF13_VOUT_HSYNC     ((uint8_t)0x0D)    /**< VOUT_HSYNC alternate function mapping */
#define GPIOA13_AF13_VOUT_VSYNC     ((uint8_t)0x0D)    /**< VOUT_VSYNC alternate function mapping */
#define GPIOA14_AF13_VOUT_R0        ((uint8_t)0x0D)    /**< VOUT_R[0] alternate function mapping */
#define GPIOA15_AF13_VOUT_R1        ((uint8_t)0x0D)    /**< VOUT_R[1] alternate function mapping */

#define GPIOB0_AF13_VOUT_R2         ((uint8_t)0x0D)
#define GPIOB1_AF13_VOUT_R3         ((uint8_t)0x0D)
#define GPIOB2_AF13_VOUT_R4         ((uint8_t)0x0D)
#define GPIOB3_AF13_VOUT_R5         ((uint8_t)0x0D)
#define GPIOB4_AF13_VOUT_R6         ((uint8_t)0x0D)
#define GPIOB5_AF13_SAI_SD_B        ((uint8_t)0x0D)
#define GPIOB6_AF13_SAI_MCLK_B      ((uint8_t)0x0D)
#define GPIOB7_AF13_SAI_BCK_B       ((uint8_t)0x0D)
#define GPIOB8_AF13_SAI_FS_B        ((uint8_t)0x0D)
#define GPIOB9_AF13_VOUT_R7         ((uint8_t)0x0D)

#define GPIOB12_AF13_VIN_D9         ((uint8_t)0x0D)
#define GPIOB13_AF13_VIN_D10        ((uint8_t)0x0D)
#define GPIOB14_AF13_VIN_D11        ((uint8_t)0x0D)
#define GPIOB15_AF13_VIN_D12        ((uint8_t)0x0D)
#define GPIOC0_AF13_VIN_D13         ((uint8_t)0x0D)

#define GPIOC4_AF13_VIN_D2          ((uint8_t)0x0D)
#define GPIOC8_AF13_VIN_D0          ((uint8_t)0x0D)
#define GPIOC10_AF13_VIN_D1         ((uint8_t)0x0D)
#define GPIOC12_AF13_VIN_D2         ((uint8_t)0x0D)
#define GPIOC14_AF13_VIN_D3         ((uint8_t)0x0D)
#define GPIOD0_AF13_VIN_D4          ((uint8_t)0x0D)
#define GPIOD4_AF13_VOUT_B4         ((uint8_t)0x0D)
#define GPIOD6_AF13_VOUT_B5         ((uint8_t)0x0D)
#define GPIOD8_AF13_VOUT_B6         ((uint8_t)0x0D)
#define GPIOD10_AF13_VOUT_B7        ((uint8_t)0x0D)
#define GPIOD12_AF13_VOUT_R3        ((uint8_t)0x0D)
#define GPIOD14_AF13_VOUT_R4        ((uint8_t)0x0D)
#define GPIOE1_AF13_VOUT_R5         ((uint8_t)0x0D)
#define GPIOE2_AF13_VOUT_R6         ((uint8_t)0x0D)
#define GPIOE3_AF13_VOUT_R7         ((uint8_t)0x0D)
#define GPIOE5_AF13_UART3_RX        ((uint8_t)0x0D)
#define GPIOE7_AF13_UART3_TX        ((uint8_t)0x0D)
#define GPIOE10_AF13_I2C3_SCL       ((uint8_t)0x0D)
#define GPIOE12_AF13_I2C3_SDA       ((uint8_t)0x0D)
#define GPIOE14_AF13_VOUT_G0        ((uint8_t)0x0D)
#define GPIOF0_AF13_VOUT_G1         ((uint8_t)0x0D)
#define GPIOF1_AF13_SPI2_SS         ((uint8_t)0x0D)
#define GPIOF2_AF13_SPI2_SCK        ((uint8_t)0x0D)
#define GPIOF3_AF13_SPI2_MISO       ((uint8_t)0x0D)
#define GPIOF4_AF13_SPI2_MOSI       ((uint8_t)0x0D)
#define GPIOF5_AF13_VOUT_G2         ((uint8_t)0x0D)
#define GPIOF6_AF13_VOUT_G3         ((uint8_t)0x0D)
#define GPIOF7_AF13_VOUT_G4         ((uint8_t)0x0D)
#define GPIOF8_AF13_VOUT_G2         ((uint8_t)0x0D)
#define GPIOF9_AF13_VOUT_G3         ((uint8_t)0x0D)
#define GPIOF10_AF13_VOUT_G5        ((uint8_t)0x0D)
#define GPIOF11_AF13_VOUT_G6        ((uint8_t)0x0D)
#define GPIOF12_AF13_VOUT_G7        ((uint8_t)0x0D)

#define GPIOG6_AF13_VOUT_OUT_CLK    ((uint8_t)0x0D)
#define GPIOG7_AF13_VOUT_DE         ((uint8_t)0x0D)
#define GPIOG8_AF13_VOUT_B0         ((uint8_t)0x0D)
#define GPIOG9_AF13_VOUT_B1         ((uint8_t)0x0D)
#define GPIOG10_AF13_VOUT_B2        ((uint8_t)0x0D)
#define GPIOG11_AF13_VOUT_B3        ((uint8_t)0x0D)

#define GPIOG13_AF13_VOUT_G4        ((uint8_t)0x0D)
#define GPIOG14_AF13_VOUT_G5        ((uint8_t)0x0D)
#define GPIOG15_AF13_VOUT_G6        ((uint8_t)0x0D)
#define GPIOG16_AF13_VOUT_G7        ((uint8_t)0x0D)

#define GPIOH7_AF13_VOUT_B4         ((uint8_t)0x0D)
#define GPIOH8_AF13_VOUT_B5         ((uint8_t)0x0D)
#define GPIOH9_AF13_VOUT_B6         ((uint8_t)0x0D)
#define GPIOH10_AF13_VOUT_B7        ((uint8_t)0x0D)

#define GPIOI0_AF13_UART4_TX        ((uint8_t)0x0D)
#define GPIOI1_AF13_UART4_RX        ((uint8_t)0x0D)
#define GPIOI2_AF13_VIN_HSYNC       ((uint8_t)0x0D)
#define GPIOI3_AF13_VIN_PIXCLK      ((uint8_t)0x0D)
#define GPIOI4_AF13_VIN_VSYNC       ((uint8_t)0x0D)

#define GPIOI6_AF13_VIN_D5          ((uint8_t)0x0D)
#define GPIOI8_AF13_VIN_D6          ((uint8_t)0x0D)
#define GPIOI10_AF13_VIN_D7         ((uint8_t)0x0D)
#define GPIOI12_AF13_VOUT_HSYNC     ((uint8_t)0x0D)
#define GPIOI14_AF13_VOUT_VSYNC     ((uint8_t)0x0D)
#define GPIOJ0_AF13_VOUT_R0         ((uint8_t)0x0D)
#define GPIOJ2_AF13_VOUT_R1         ((uint8_t)0x0D)
#define GPIOJ4_AF13_VOUT_R2         ((uint8_t)0x0D)
#define GPIOJ8_AF13_UART1_RTSJ      ((uint8_t)0x0D)
#define GPIOJ10_AF13_UART1_CTSJ     ((uint8_t)0x0D)
#define GPIOJ14_AF13_UART1_TX       ((uint8_t)0x0D)
#define GPIOK2_AF13_UART1_RX        ((uint8_t)0x0D)

/**
  * @brief   AF 14 selection
  */
#define GPIOA1_AF14_VOUT_R0         ((uint8_t)0x0E)   /**< VOUT_R[0] alternate function mapping */
#define GPIOA2_AF14_VOUT_R1         ((uint8_t)0x0E)   /**< VOUT_R[1] alternate function mapping */
#define GPIOA3_AF14_VOUT_R2         ((uint8_t)0x0E)   /**< VOUT_R[2] alternate function mapping */
#define GPIOA4_AF14_VOUT_G0         ((uint8_t)0x0E)   /**< VOUT_G[0] alternate function mapping */

#define GPIOB4_AF14_VOUT_G2         ((uint8_t)0x0E)
#define GPIOB5_AF14_VOUT_G1         ((uint8_t)0x0E)
#define GPIOB6_AF14_VOUT_B0         ((uint8_t)0x0E)
#define GPIOB7_AF14_VOUT_B1         ((uint8_t)0x0E)
#define GPIOB8_AF14_VOUT_B2         ((uint8_t)0x0E)
#define GPIOB9_AF14_VOUT_R7         ((uint8_t)0x0E)
#define GPIOD4_AF14_VOUT_G3         ((uint8_t)0x0E)
#define GPIOD6_AF14_VOUT_G4         ((uint8_t)0x0E)
#define GPIOD8_AF14_VOUT_G5         ((uint8_t)0x0E)
#define GPIOD10_AF14_VOUT_G6        ((uint8_t)0x0E)
#define GPIOD12_AF14_NF_DQS         ((uint8_t)0x0E)
#define GPIOD14_AF14_NF_WPJ         ((uint8_t)0x0E)
#define GPIOE1_AF14_VOUT_G7         ((uint8_t)0x0E)
#define GPIOE3_AF14_VOUT_B3         ((uint8_t)0x0E)
#define GPIOE14_AF14_VOUT_B4        ((uint8_t)0x0E)
#define GPIOF0_AF14_VOUT_B5         ((uint8_t)0x0E)
#define GPIOG6_AF14_VOUT_OUT_CLK    ((uint8_t)0x0E)
#define GPIOG7_AF14_VOUT_DE         ((uint8_t)0x0E)
#define GPIOG8_AF14_VOUT_B6         ((uint8_t)0x0E)
#define GPIOG9_AF14_VOUT_B7         ((uint8_t)0x0E)
#define GPIOG10_AF14_UART7_RX       ((uint8_t)0x0E)
#define GPIOG11_AF14_UART7_TX       ((uint8_t)0x0E)
#define GPIOG13_AF14_NF_RBJ         ((uint8_t)0x0E)
#define GPIOG14_AF14_NF_REJ         ((uint8_t)0x0E)
#define GPIOG15_AF14_NF_WEJ         ((uint8_t)0x0E)
#define GPIOH0_AF14_NF_CEJ          ((uint8_t)0x0E)
#define GPIOI12_AF14_VOUT_HSYNC     ((uint8_t)0x0E)
#define GPIOI14_AF14_VOUT_VSYNC     ((uint8_t)0x0E)
#define GPIOJ0_AF14_VOUT_R3         ((uint8_t)0x0E)
#define GPIOJ2_AF14_VOUT_R4         ((uint8_t)0x0E)
#define GPIOJ4_AF14_VOUT_R5         ((uint8_t)0x0E)
#define GPIOJ6_AF14_VOUT_R6         ((uint8_t)0x0E)

/**
  * @brief   AF 15 selection
  */
#define GPIOA9_AF15_SDRAM_D24       ((uint8_t)0x0F)    /**< SDRAM_D[24] alternate function mapping */
#define GPIOA10_AF15_SDRAM_D25      ((uint8_t)0x0F)    /**< SDRAM_D[25] alternate function mapping */

#define GPIOC2_AF15_SDRAM_SDNE1     ((uint8_t)0x0F)    /**< SDRAM_SDNE[1] alternate function mapping */
#define GPIOC3_AF15_SDRAM_SDCKE1    ((uint8_t)0x0F)    /**< SDRAM_SDCKE[1] alternate function mapping */
#define GPIOC5_AF15_SDRAM_NBL2      ((uint8_t)0x0F)    /**<SDRAM_NBL[2] alternate function mapping */
#define GPIOC6_AF15_SDRAM_NBL3      ((uint8_t)0x0F)    /**< SDRAM_NBL[3] alternate function mapping */
#define GPIOC7_AF15_SDRAM_NBL0      ((uint8_t)0x0F)    /**< SDRAM_NBL[0] alternate function mapping */
#define GPIOC9_AF15_SDRAM_SDNWE     ((uint8_t)0x0F)    /**< SDRAM_SDNWE alternate function mapping */
#define GPIOC11_AF15_SDRAM_SDNCAS   ((uint8_t)0x0F)    /**< SDRAM_SDNCASalternate function mapping */
#define GPIOC13_AF15_SDRAM_SDNRAS   ((uint8_t)0x0F)    /**< SDRAM_SDNRAS alternate function mapping */
#define GPIOC15_AF15_SDRAM_SDNE0    ((uint8_t)0x0F)    /**< SDRAM_SDNE[0] alternate function mapping */
#define GPIOD1_AF15_SDRAM_BA0       ((uint8_t)0x0F)    /**< SDRAM_BA[0] alternate function mapping */
#define GPIOD2_AF15_SDRAM_D26       ((uint8_t)0x0F)    /**< SDRAM_D[26] alternate function mapping */
#define GPIOD3_AF15_SDRAM_BA1       ((uint8_t)0x0F)    /**< SDRAM_BA[1] alternate function mapping */
#define GPIOD4_AF15_SDRAM_D27       ((uint8_t)0x0F)    /**<  SDRAM_D[27] alternate function mapping */
#define GPIOD5_AF15_SDRAM_A10       ((uint8_t)0x0F)    /**< SDRAM_A[10] alternate function mapping */
#define GPIOD6_AF15_SDRAM_D28       ((uint8_t)0x0F)    /**< SDRAM_D[28] alternate function mapping */
#define GPIOD7_AF15_SDRAM_A0        ((uint8_t)0x0F)    /**< SDRAM_A[0] alternate function mapping */
#define GPIOD8_AF15_SDRAM_D29       ((uint8_t)0x0F)    /**<SDRAM_D[29] alternate function mapping */
#define GPIOD9_AF15_SDRAM_A1        ((uint8_t)0x0F)    /**< SDRAM_A[1] alternate function mapping */
#define GPIOD11_AF15_SDRAM_A2       ((uint8_t)0x0F)    /**< SDRAM_A[2] alternate function mapping */
#define GPIOD12_AF15_SDRAM_D30      ((uint8_t)0x0F)    /**< SDRAM_D[30] alternate function mapping */
#define GPIOD13_AF15_SDRAM_A3       ((uint8_t)0x0F)    /**<SDRAM_A[3] alternate function mapping */
#define GPIOD14_AF15_SDRAM_D31      ((uint8_t)0x0F)    /**< SDRAM_D[31] alternate function mapping */
#define GPIOD15_AF15_SDRAM_A4       ((uint8_t)0x0F)    /**< SDRAM_A[4] alternate function mapping */
#define GPIOE0_AF15_SDRAM_A12       ((uint8_t)0x0F)    /**< SDRAM_A[12] alternate function mapping */
#define GPIOE2_AF15_SDRAM_A5        ((uint8_t)0x0F)    /**< SDRAM_A[5] alternate function mapping */
#define GPIOE4_AF15_SDRAM_A6        ((uint8_t)0x0F)    /**<SDRAM_A[6] alternate function mapping */
#define GPIOE6_AF15_SDRAM_A7        ((uint8_t)0x0F)    /**< SDRAM_A[7] alternate function mapping */
#define GPIOE8_AF15_SDRAM_A8        ((uint8_t)0x0F)    /**< SDRAM_A[8] alternate function mapping */
#define GPIOE9_AF15_SDRAM_A9        ((uint8_t)0x0F)    /**< SDRAM_A[9] alternate function mapping */
#define GPIOE11_AF15_SDRAM_A11      ((uint8_t)0x0F)    /**< SDRAM_A[11]alternate function mapping */
#define GPIOE13_AF15_SDRAM_SDCKE0   ((uint8_t)0x0F)    /**< SDRAM_SDCKE[0] alternate function mapping */
#define GPIOE15_AF15SDRAM_NBL1      ((uint8_t)0x0F)    /**< SDRAM_NBL[1] alternate function mapping */
#define GPIOG12_AF15_SDRAM_SDCLK    ((uint8_t)0x0F)    /**< SDRAM_SDCLK alternate function mapping */
#define GPIOI5_AF15_SDRAM_D8        ((uint8_t)0x0F)    /**< SDRAM_D[8] alternate function mapping */
#define GPIOI7_AF15_SDRAM_D8        ((uint8_t)0x0F)    /**< SDRAM_D[9] alternate function mapping */
#define GPIOI9_AF15_SDRAM_D8        ((uint8_t)0x0F)    /**< SDRAM_D[10] alternate function mapping */
#define GPIOI11_AF15_SDRAM_D8       ((uint8_t)0x0F)    /**< SDRAM_D[11] alternate function mapping */
#define GPIOI13_AF15_SDRAM_D12      ((uint8_t)0x0F)    /**< SDRAM_D[12] alternate function mapping */
#define GPIOI14_AF15_SDRAM_D16      ((uint8_t)0x0F)    /**< SDRAM_D[16] alternate function mapping */
#define GPIOI15_AF15_SDRAM_D13      ((uint8_t)0x0F)    /**< SDRAM_D[13] alternate function mapping */

#define GPIOJ0_AF15_SDRAM_D17       ((uint8_t)0x0F)    /**< SDRAM_D[17] alternate function mapping */
#define GPIOJ1_AF15_SDRAM_D14       ((uint8_t)0x0F)    /**< SDRAM_D[14] alternate function mapping */
#define GPIOJ2_AF15_SDRAM_D18       ((uint8_t)0x0F)    /**< SDRAM_D[18] alternate function mapping */
#define GPIOJ3_AF15_SDRAM_D15       ((uint8_t)0x0F)    /**< SDRAM_D[15] alternate function mapping */
#define GPIOJ4_AF15_SDRAM_D19       ((uint8_t)0x0F)    /**< SDRAM_D[19] alternate function mapping */
#define GPIOJ5_AF15_SDRAM_D0        ((uint8_t)0x0F)    /**< SDRAM_D[0] alternate function mapping */
#define GPIOJ7_AF15_SDRAM_D1        ((uint8_t)0x0F)    /**< SDRAM_D[1] alternate function mapping */
#define GPIOJ9_AF15_SDRAM_D2        ((uint8_t)0x0F)    /**< SDRAM_D[2] alternate function mapping */
#define GPIOJ13_AF15_SDRAM_D3       ((uint8_t)0x0F)    /**< SDRAM_D[3] alternate function mapping */
#define GPIOJ15_AF15_SDRAM_D4       ((uint8_t)0x0F)    /**< SDRAM_D[4] alternate function mapping */
#define GPIOK1_AF15_SDRAM_D5        ((uint8_t)0x0F)    /**< SDRAM_D[5] alternate function mapping */
#define GPIOK3_AF15_SDRAM_D6        ((uint8_t)0x0F)    /**< SDRAM_D[6] alternate function mapping */
#define GPIOK4_AF15_SDRAM_D20       ((uint8_t)0x0F)    /**< SDRAM_D[20] alternate function mapping */
#define GPIOK5_AF15_SDRAM_D7        ((uint8_t)0x0F)    /**< SDRAM_D[7] alternate function mapping */
#define GPIOK6_AF15_SDRAM_D21       ((uint8_t)0x0F)    /**<SDRAM_D[21] alternate function mapping */
#define GPIOK7_AF15_SDRAM_D22       ((uint8_t)0x0F)    /**< SDRAM_D[22] alternate function mapping */
#define GPIOK8_AF15_SDRAM_D23       ((uint8_t)0x0F)    /**< SDRAM_D[23] alternate function mapping */
#define GPIOK9_AF15_INT_SFLASH_MOSI      ((uint8_t)0x0F)    /**< INT_SFLASH_MOSI alternate function mapping */
#define GPIOK10_AF15_INT_SFLASH_SCK      ((uint8_t)0x0F)    /**< INT_SFLASH_SCK alternate function mapping */
#define GPIOK11_AF15_INT_SFLASH_HOLDJ    ((uint8_t)0x0F)    /**< INT_SFLASH_HOLDJalternate function mapping */
#define GPIOK12_AF15_INT_SFLASH_CSJ      ((uint8_t)0x0F)    /**< INT_SFLASH_CSJ alternate function mapping */
#define GPIOK13_AF15_INT_SFLASH_MISO     ((uint8_t)0x0F)    /**< INT_SFLASH_MISO alternate function mapping */
#define GPIOK14_AF15_INT_SFLASH_WJ       ((uint8_t)0x0F)    /**< INT_SFLASH_WJ alternate function mapping */

#endif



#if (defined(CHIP_F6721B))
/**
  * @brief   AF 0 selection
  */
#define GPIOB9_AF0_UART0_TX                 ((uint8_t)0x00)    /**< UART0_TX alternate function mapping */
#define GPIOB9_AF0_UART0_RX                 ((uint8_t)0x00)    /**< UART0_RX alternate function mapping */
#define GPIOB10_AF0_UART0_RX                ((uint8_t)0x00)    /**< UART0_RX alternate function mapping */
#define GPIOB12_AF0_I2C1_SCL                ((uint8_t)0x00)    /**< I2C1_SCL alternate function mapping */
#define GPIOB13_AF0_I2C1_SDA                ((uint8_t)0x00)    /**< I2C1_SDA alternate function mapping */
#define GPIOC2_AF0_UART1_TX                 ((uint8_t)0x00)    /**< UART1_TX   alternate function mapping */
#define GPIOC2_AF0_UART1_RX                 ((uint8_t)0x00)    /**< UART1_RX   alternate function mapping */
#define GPIOC3_AF0_UART1_RX                 ((uint8_t)0x00)    /**< UART1_RX   alternate function mapping */
#define GPIOC4_AF0_UART1_CTSJ               ((uint8_t)0x00)    /**< UART1_CTSJ alternate function mapping */
#define GPIOC5_AF0_UART1_CTSJ               ((uint8_t)0x00)    /**< UART1_CTSJ alternate function mapping */
#define GPIOC10_AF0_MCO2                    ((uint8_t)0x00)    /**< MCO2     alternate function mapping */
#define GPIOC11_AF0_MCO1                    ((uint8_t)0x00)    /**< MCO1     alternate function mapping */
#define GPIOC14_AF0_I2C0_SCL                ((uint8_t)0x00)    /**< I2C0_SCL alternate function mapping */
#define GPIOC15_AF0_I2C0_SDA                ((uint8_t)0x00)    /**< I2C0_SDA alternate function mapping */
#define GPIOD2_AF0_I2C2_SCL                 ((uint8_t)0x00)    /**< I2C2_SCL alternate function mapping */
#define GPIOD3_AF0_I2C2_SDA                 ((uint8_t)0x00)    /**< I2C2_SDA alternate function mapping */

/**
  * @brief   AF 1 selection
  */
#define GPIOB6_AF1_UART3_TX                 ((uint8_t)0x01)    /**< UART3_TX alternate function mapping */
#define GPIOB6_AF1_UART3_RX                 ((uint8_t)0x01)    /**< UART3_RX alternate function mapping */
#define GPIOB5_AF1_UART3_TX                 ((uint8_t)0x01)    /**< UART3_TX alternate function mapping */
#define GPIOB7_AF1_UART3_RX                 ((uint8_t)0x01)    /**< UART3_RX alternate function mapping */
#define GPIOB9_AF1_I2C0_SCL                 ((uint8_t)0x01)    /**< I2C0_SCL alternate function mapping */
#define GPIOB10_AF1_I2C0_SDA                ((uint8_t)0x01)    /**< I2C0_SDA alternate function mapping */
#define GPIOB12_AF1_UART1_TX                ((uint8_t)0x01)    /**< UART1_TX alternate function mapping */
#define GPIOB12_AF1_UART1_RX                ((uint8_t)0x01)    /**< UART1_RX alternate function mapping */
#define GPIOB13_AF1_UART1_RX                ((uint8_t)0x01)    /**< UART1_RX alternate function mapping */
#define GPIOC2_AF1_I2C1_SCL                 ((uint8_t)0x01)    /**< I2C1_SCL alternate function mapping */
#define GPIOC3_AF1_I2C1_SDA                 ((uint8_t)0x01)    /**< I2C1_SDA alternate function mapping */
#define GPIOC4_AF1_UART4_TX                 ((uint8_t)0x01)    /**< UART4_TX alternate function mapping */
#define GPIOC4_AF1_UART4_RX                 ((uint8_t)0x01)    /**< UART4_RX alternate function mapping */
#define GPIOC5_AF1_UART4_RX                 ((uint8_t)0x01)    /**< UART4_RX alternate function mapping */
#define GPIOC8_AF1_UART7_TX                 ((uint8_t)0x01)    /**< UART7_TX alternate function mapping */
#define GPIOC8_AF1_UART7_RX                 ((uint8_t)0x01)    /**< UART7_RX alternate function mapping */
#define GPIOC9_AF1_UART7_RX                 ((uint8_t)0x01)    /**< UART7_RX alternate function mapping */
#define GPIOC10_AF1_UART6_TX                ((uint8_t)0x01)    /**< UART6_TX   alternate function mapping */
#define GPIOC10_AF1_UART6_RX                ((uint8_t)0x01)    /**< UART6_RX   alternate function mapping */
#define GPIOC11_AF1_UART6_RX                ((uint8_t)0x01)    /**< UART6_RX   alternate function mapping */
#define GPIOC12_AF1_UART2_TX                ((uint8_t)0x01)    /**< UART2_TX   alternate function mapping */
#define GPIOC12_AF1_UART2_RX                ((uint8_t)0x01)    /**< UART2_RX   alternate function mapping */
#define GPIOC13_AF1_UART2_RX                ((uint8_t)0x01)    /**< UART2_RX   alternate function mapping */
#define GPIOC14_AF1_UART2_CTSJ              ((uint8_t)0x01)    /**< UART2_CTSJ alternate function mapping */
#define GPIOC15_AF1_UART2_RTSJ              ((uint8_t)0x01)    /**< UART2_RTSJ alternate function mapping */
#define GPIOD0_AF1_UART5_TX                 ((uint8_t)0x01)    /**< UART5_TX   alternate function mapping */
#define GPIOD0_AF1_UART5_RX                 ((uint8_t)0x01)    /**< UART5_RX   alternate function mapping */
#define GPIOD1_AF1_UART5_RX                 ((uint8_t)0x01)    /**< UART5_RX   alternate function mapping */
#define GPIOD2_AF1_UART5_CTSJ               ((uint8_t)0x01)    /**< UART5_CTSJ alternate function mapping */
#define GPIOD3_AF1_UART5_RTSJ               ((uint8_t)0x01)    /**< UART5_RTSJ alternate function mapping */
#define GPIOD4_AF1_UART0_TX                 ((uint8_t)0x01)    /**< UART0_TX   alternate function mapping */
#define GPIOD4_AF1_UART0_RX                 ((uint8_t)0x01)    /**< UART0_RX   alternate function mapping */
#define GPIOD5_AF1_UART0_RX                 ((uint8_t)0x01)    /**< UART0_RX   alternate function mapping */
#define GPIOD6_AF1_UART0_CTSJ               ((uint8_t)0x01)    /**< UART0_CTSJ alternate function mapping */
#define GPIOD7_AF1_UART0_RTSJ               ((uint8_t)0x01)    /**< UART0_RTSJ alternate function mapping */
#define GPIOD8_AF1_SPI0_SS                  ((uint8_t)0x01)    /**< SPI0_SS    alternate function mapping */
#define GPIOD9_AF1_SPI0_SCK                 ((uint8_t)0x01)    /**< SPI0_SCK   alternate function mapping */
#define GPIOD10_AF1_SPI0_MISO               ((uint8_t)0x01)    /**< SPI0_MISO alternate function mapping */
#define GPIOD11_AF1_SPI0_MOSI               ((uint8_t)0x01)    /**< SPI0_MOSI alternate function mapping */
#define GPIOF5_AF1_I2C2_SDA                 ((uint8_t)0x01)    /**< I2C2_SDA alternate function mapping */
#define GPIOF6_AF1_I2C2_SCL                 ((uint8_t)0x01)    /**< I2C2_SCL alternate function mapping */
#define GPIOH12_AF1_C2_SWCLK                ((uint8_t)0x01)    /**< C2_SWCLK alternate function mapping */
#define GPIOH13_AF1_C2_SWDIO                ((uint8_t)0x01)    /**< C2_SWDIO alternate function mapping */
#define GPIOH14_AF1_CAN0_RX                 ((uint8_t)0x01)    /**< CAN0_RX  alternate function mapping */
#define GPIOH15_AF1_CAN0_TX                 ((uint8_t)0x01)    /**< CAN0_TX  alternate function mapping */
#define GPIOI0_AF1_CAN0_DBG                 ((uint8_t)0x01)    /**< CAN0_DBG alternate function mapping */

/**
  * @brief   AF 2 selection
  */
#define GPIOB6_AF2_JTRSTN                   ((uint8_t)0x02)    /**<JTRSTN      alternate function mapping */
#define GPIOB7_AF2_JTDO                     ((uint8_t)0x02)    /**<JTDO        alternate function mapping */
#define GPIOB8_AF2_JTDI                     ((uint8_t)0x02)    /**<JTDI        alternate function mapping */
#define GPIOB9_AF2_JTCK_SWCLK               ((uint8_t)0x02)    /**<JTCK_SWCLK  alternate function mapping */
#define GPIOB10_AF2_JTMS_SWDIO              ((uint8_t)0x02)    /**<JTMS_SWDIO    alternate function mapping */
#define GPIOB12_AF2_RTC_TAMP2               ((uint8_t)0x02)    /**<RTC_TAMP2     alternate function mapping */
#define GPIOB13_AF2_RTC_TAMP1               ((uint8_t)0x02)    /**<RTC_TAMP1     alternate function mapping */
#define GPIOC2_AF2_I2C1_SCL                 ((uint8_t)0x02)    /**< I2C1_SCL alternate function mapping */
#define GPIOC3_AF2_I2C1_SDA                 ((uint8_t)0x02)    /**< I2C1_SDA alternate function mapping */
#define GPIOC5_AF2_UART4_RX                 ((uint8_t)0x02)    /**< UART4_RX alternate function mapping */
#define GPIOC8_AF2_I2C2_SCL                 ((uint8_t)0x02)    /**< I2C2_SCL alternate function mapping */
#define GPIOC9_AF2_I2C2_SDA                 ((uint8_t)0x02)    /**< I2C2_SDA alternate function mapping */
#define GPIOC10_AF2_I2C0_SCL                ((uint8_t)0x02)    /**< I2C0_SCLalternate function mapping */
#define GPIOC11_AF2_I2C0_SDA                ((uint8_t)0x02)    /**< I2C0_SDA alternate function mapping */
#define GPIOC12_AF2_SPI0_SS                 ((uint8_t)0x02)    /**< SPI0_SS   alternate function mapping */
#define GPIOC13_AF2_SPI0_SCK                ((uint8_t)0x02)    /**< SPI0_SCK  alternate function mapping */
#define GPIOC14_AF2_SPI0_MISO               ((uint8_t)0x02)    /**< SPI0_MISO alternate function mapping */
#define GPIOC15_AF2_SPI0_MOSI               ((uint8_t)0x02)    /**< SPI0_MOSI alternate function mapping */
#define GPIOD0_AF2_SPI1_SS                  ((uint8_t)0x02)    /**< SPI1_SS  alternate function mapping */
#define GPIOD1_AF2_SPI1_SCK                 ((uint8_t)0x02)    /**< SPI1_SCK  alternate function mapping */
#define GPIOD2_AF2_SPI1_MISO                ((uint8_t)0x02)    /**< SPI1_MISO alternate function mapping */
#define GPIOD3_AF2_SPI1_MOSI                ((uint8_t)0x02)    /**< SPI1_MOSI alternate function mapping */
#define GPIOD5_AF2_UART0_RX                 ((uint8_t)0x02)    /**< UART0_RX     alternate function mapping */
#define GPIOD6_AF2_I2C1_SCL                 ((uint8_t)0x02)    /**< I2C1_SCL     alternate function mapping */
#define GPIOD7_AF2_I2C1_SDA                 ((uint8_t)0x02)    /**< I2C1_SDA     alternate function mapping */
#define GPIOD8_AF2_SPI0_SS                  ((uint8_t)0x02)    /**< SPI0_SS  alternate function mapping */
#define GPIOD9_AF2_SPI0_SCK                 ((uint8_t)0x02)    /**< SPI0_SCK  alternate function mapping */
#define GPIOD10_AF2_SPI0_MISO               ((uint8_t)0x02)    /**< SPI0_MISO    alternate function mapping */
#define GPIOD11_AF2_SPI0_MOSI               ((uint8_t)0x02)    /**< SPI0_MOSI    alternate function mapping */
#define GPIOD13_AF2_VOUT_B2                 ((uint8_t)0x02)    /**< VOUT_B2      alternate function mapping */
#define GPIOD14_AF2_VOUT_B1                 ((uint8_t)0x02)    /**< VOUT_B1      alternate function mapping */
#define GPIOD15_AF2_VOUT_B0                 ((uint8_t)0x02)    /**< VOUT_B0      alternate function mapping */
#define GPIOE0_AF2_VOUT_G1                  ((uint8_t)0x02)    /**< VOUT_G1      alternate function mapping */
#define GPIOE1_AF2_VOUT_DE                  ((uint8_t)0x02)    /**< VOUT_DE     alternate function mapping */
#define GPIOE2_AF2_VOUT_VSYNC               ((uint8_t)0x02)    /**< VOUT_VSYNC   alternate function mapping */
#define GPIOE3_AF2_VOUT_HSYNC               ((uint8_t)0x02)    /**< VOUT_HSYNC   alternate function mapping */
#define GPIOE4_AF2_VOUT_OUT_CLK             ((uint8_t)0x02)    /**< VOUT_OUT_CLK alternate function mapping */
#define GPIOE5_AF2_VOUT_B7                  ((uint8_t)0x02)    /**< VOUT_B7     alternate function mapping */
#define GPIOE6_AF2_VOUT_B6                  ((uint8_t)0x02)    /**< VOUT_B6     alternate function mapping */
#define GPIOE7_AF2_VOUT_B5                  ((uint8_t)0x02)    /**< VOUT_B5     alternate function mapping */
#define GPIOE8_AF2_VOUT_B4                  ((uint8_t)0x02)    /**< VOUT_B4     alternate function mapping */
#define GPIOE9_AF2_VOUT_B3                  ((uint8_t)0x02)    /**< VOUT_B3     alternate function mapping */
#define GPIOE10_AF2_VOUT_G7                 ((uint8_t)0x02)    /**< VOUT_G7 alternate function mapping */
#define GPIOE11_AF2_VOUT_G6                 ((uint8_t)0x02)    /**< VOUT_G6 alternate function mapping */
#define GPIOE12_AF2_VOUT_G5                 ((uint8_t)0x02)    /**< VOUT_G5 alternate function mapping */
#define GPIOE13_AF2_VOUT_G4                 ((uint8_t)0x02)    /**< VOUT_G4 alternate function mapping */
#define GPIOE14_AF2_VOUT_G3                 ((uint8_t)0x02)    /**< VOUT_G3 alternate function mapping */
#define GPIOE15_AF2_VOUT_G2                 ((uint8_t)0x02)    /**< VOUT_G2 alternate function mapping */
#define GPIOF0_AF2_VOUT_R7                  ((uint8_t)0x02)    /**< VOUT_R7 alternate function mapping */
#define GPIOF1_AF2_VOUT_R6                  ((uint8_t)0x02)    /**< VOUT_R6 alternate function mapping */
#define GPIOF2_AF2_VOUT_R5                  ((uint8_t)0x02)    /**< VOUT_R5 alternate function mapping */
#define GPIOF3_AF2_VOUT_R4                  ((uint8_t)0x02)    /**< VOUT_R4 alternate function mapping */
#define GPIOF4_AF2_VOUT_R3                  ((uint8_t)0x02)    /**< VOUT_R3 alternate function mapping */
#define GPIOF5_AF2_VOUT_R2                  ((uint8_t)0x02)    /**< VOUT_R2 alternate function mapping */
#define GPIOF6_AF2_VOUT_R1                  ((uint8_t)0x02)    /**< VOUT_R1 alternate function mapping */
#define GPIOF7_AF2_VOUT_R0                  ((uint8_t)0x02)    /**< VOUT_R0 alternate function mapping */
#define GPIOF8_AF2_VOUT_G0                  ((uint8_t)0x02)    /**< VOUT_G0 alternate function mapping */
#define GPIOG2_AF2_PDM_MCLK_A               ((uint8_t)0x02)    /**< PDM_MCLK_A alternate function mapping */
#define GPIOG3_AF2_PDM_FS_A                 ((uint8_t)0x02)    /**< PDM_FS_A      alternate function mapping */
#define GPIOG4_AF2_PDM_BCK_A                ((uint8_t)0x02)    /**< PDM_BCK_A alternate function mapping */
#define GPIOG5_AF2_PDM_SD_A                 ((uint8_t)0x02)    /**< PDM_SD_A      alternate function mapping */
#define GPIOG6_AF2_PDM_SD_B                 ((uint8_t)0x02)    /**< PDM_SD_B      alternate function mapping */
#define GPIOG7_AF2_PDM_MCLK_B               ((uint8_t)0x02)    /**< PDM_MCLK_B alternate function mapping */
#define GPIOG8_AF2_PDM_BCK_B                ((uint8_t)0x02)    /**< PDM_BCK_B alternate function mapping */
#define GPIOG9_AF2_PDM_FS_B                 ((uint8_t)0x02)    /**< PDM_FS_B      alternate function mapping */
#define GPIOG13_AF2_VIN_D10                ((uint8_t)0x02)    /**< VIN_D10 alternate function mapping */
#define GPIOG14_AF2_VIN_D11                ((uint8_t)0x02)    /**< VIN_D11 alternate function mapping */
#define GPIOG15_AF2_VIN_D12                ((uint8_t)0x02)    /**< VIN_D12 alternate function mapping */
#define GPIOH0_AF2_VIN_D13                 ((uint8_t)0x02)    /**< VIN_D13   alternate function mapping */
#define GPIOH1_AF2_OTG_FS_SOF               ((uint8_t)0x02)    /**< OTG_FS_SOF alternate function mapping */
#define GPIOH7_AF2_VIN_D2                  ((uint8_t)0x02)    /**< VIN_D2   alternate function mapping */
#define GPIOH8_AF2_VIN_D1                  ((uint8_t)0x02)    /**< VIN_D1   alternate function mapping */
#define GPIOH9_AF2_VIN_D3                  ((uint8_t)0x02)    /**< VIN_D3   alternate function mapping */
#define GPIOH10_AF2_VIN_D0                 ((uint8_t)0x02)    /**< VIN_D0      alternate function mapping */
#define GPIOH11_AF2_VIN_D4                 ((uint8_t)0x02)    /**< VIN_D4      alternate function mapping */
#define GPIOH12_AF2_VIN_PIXCLK             ((uint8_t)0x02)    /**< VIN_PIXCLK  alternate function mapping */
#define GPIOH13_AF2_VIN_D5                 ((uint8_t)0x02)    /**< VIN_D5      alternate function mapping */
#define GPIOH14_AF2_VIN_D6                 ((uint8_t)0x02)    /**< VIN_D6      alternate function mapping */
#define GPIOH15_AF2_VIN_D7                 ((uint8_t)0x02)    /**< VIN_D7      alternate function mapping */
#define GPIOHI0_AF2_VIN_HSYNC              ((uint8_t)0x02)    /**< VIN_HSYNC   alternate function mapping */
#define GPIOHI1_AF2_VIN_VSYNC              ((uint8_t)0x02)    /**< VIN_VSYNC   alternate function mapping */
#define GPIOHI2_AF2_VIN_D8                 ((uint8_t)0x02)    /**< VIN_D8      alternate function mapping */
#define GPIOHI3_AF2_VIN_D9                 ((uint8_t)0x02)    /**< VIN_D9      alternate function mapping */
#define GPIOHJ1_AF2_UART5_CTSJ              ((uint8_t)0x02)    /**< UART5_CTSJ   alternate function mapping */
#define GPIOHJ2_AF2_UART5_RTSJ              ((uint8_t)0x02)    /**< UART5_RTSJ   alternate function mapping */
#define GPIOHJ3_AF2_UART5_RX                ((uint8_t)0x02)    /**< UART5_RX     alternate function mapping */
#define GPIOHJ4_AF2_UART5_TX                ((uint8_t)0x02)    /**< UART5_TX     alternate function mapping */
#define GPIOHJ4_AF2_UART5_RX                ((uint8_t)0x02)    /**< UART5_RX     alternate function mapping */
#define GPIOHJ5_AF2_UART2_TX                ((uint8_t)0x02)    /**< UART2_TX     alternate function mapping */
#define GPIOHJ5_AF2_UART2_RX                ((uint8_t)0x02)    /**< UART2_RX     alternate function mapping */
#define GPIOHJ6_AF2_UART2_RX                ((uint8_t)0x02)    /**< UART2_RX     alternate function mapping */
#define GPIOHJ7_AF2_UART2_CTSJ              ((uint8_t)0x02)    /**< UART2_CTSJ   alternate function mapping */
#define GPIOHJ8_AF2_UART2_RTSJ              ((uint8_t)0x02)    /**< UART2_RTSJ   alternate function mapping */



/**
  * @brief   AF 3 selection
  */
#define GPIOB5_AF3_I2C0_SCL                 ((uint8_t)0x03)    /**< I2C0_SCL alternate function mapping */
#define GPIOB7_AF3_I2C0_SDA                 ((uint8_t)0x03)    /**< I2C0_SDA alternate function mapping */
#define GPIOB9_AF3_I2C0_SCL                 ((uint8_t)0x03)    /**< I2C0_SCL alternate function mapping */
#define GPIOB10_AF3_I2C0_SDA                ((uint8_t)0x03)    /**< I2C0_SDA alternate function mapping */
#define GPIOB13_AF3_RTC_OUT                 ((uint8_t)0x03)    /**< RTC_OUT  alternate function mapping */
#define GPIOC2_AF3_SPI0_SS                  ((uint8_t)0x03)    /**< SPI0_SS  alternate function mapping */
#define GPIOC3_AF3_SPI0_SCK                 ((uint8_t)0x03)    /**< SPI0_SCK  alternate function mapping */
#define GPIOC4_AF3_SPI0_MISO                ((uint8_t)0x03)    /**< SPI0_MISO alternate function mapping */
#define GPIOC5_AF3_SPI0_MOSI                ((uint8_t)0x03)    /**< SPI0_MOSI alternate function mapping */
#define GPIOC8_AF3_SPI1_SS                  ((uint8_t)0x03)    /**< SPI1_SS  alternate function mapping */
#define GPIOC9_AF3_SPI1_SCK                 ((uint8_t)0x03)    /**< SPI1_SCK  alternate function mapping */
#define GPIOC10_AF3_SPI1_MISO               ((uint8_t)0x03)    /**< SPI1_MISO    alternate function mapping */
#define GPIOC11_AF3_SPI1_MOSI               ((uint8_t)0x03)    /**< SPI1_MOSI    alternate function mapping */
#define GPIOC12_AF3_PDM_MCLK_A              ((uint8_t)0x03)    /**< PDM_MCLK_A   alternate function mapping */
#define GPIOC13_AF3_PDM_FS_A                ((uint8_t)0x03)    /**< PDM_FS_A     alternate function mapping */
#define GPIOC14_AF3_PDM_BCK_A               ((uint8_t)0x03)    /**< PDM_BCK_A    alternate function mapping */
#define GPIOC15_AF3_PDM_SD_A                ((uint8_t)0x03)    /**< PDM_SD_A     alternate function mapping */
#define GPIOD0_AF3_PDM_SD_B                 ((uint8_t)0x03)    /**< PDM_SD_B         alternate function mapping */
#define GPIOD1_AF3_PDM_MCLK_B               ((uint8_t)0x03)    /**< PDM_MCLK_B       alternate function mapping */
#define GPIOD2_AF3_PDM_BCK_B                ((uint8_t)0x03)    /**< PDM_BCK_B    alternate function mapping */
#define GPIOD3_AF3_PDM_FS_B                 ((uint8_t)0x03)    /**< PDM_FS_B         alternate function mapping */
#define GPIOD5_AF3_UART0_RX                 ((uint8_t)0x03)    /**< UART0_RX     alternate function mapping */
#define GPIOD6_AF3_UART0_CTRJ               ((uint8_t)0x03)    /**< UART0_CTRJ   alternate function mapping */
#define GPIOD7_AF3_UART0_RTRJ               ((uint8_t)0x03)    /**< UART0_RTRJ   alternate function mapping */
#define GPIOD8_AF3_SPI0_SS                  ((uint8_t)0x03)    /**< SPI0_SS      alternate function mapping */
#define GPIOD9_AF3_SPI0_SCK                 ((uint8_t)0x03)    /**< SPI0_SCK     alternate function mapping */
#define GPIOD10_AF3_SPI0_MISO               ((uint8_t)0x03)    /**< SPI0_MISO    alternate function mapping */
#define GPIOD11_AF3_SPI0_MOSI               ((uint8_t)0x03)    /**< SPI0_MOSI    alternate function mapping */
#define GPIOE1_AF3_I2C1_SCL                 ((uint8_t)0x03)    /**< I2C1_SCL     alternate function mapping */
#define GPIOE2_AF3_I2C1_SDA                 ((uint8_t)0x03)    /**< I2C1_SDA     alternate function mapping */
#define GPIOE3_AF3_VIN_D2                   ((uint8_t)0x03)    /**< VIN_D2       alternate function mapping */
#define GPIOE4_AF3_VIN_D1                   ((uint8_t)0x03)    /**< VIN_D1       alternate function mapping */
#define GPIOE5_AF3_VIN_D3                   ((uint8_t)0x03)    /**< VIN_D3       alternate function mapping */
#define GPIOE6_AF3_VIN_D0                   ((uint8_t)0x03)    /**< VIN_D0       alternate function mapping */
#define GPIOF5_AF3_UART2_RX                 ((uint8_t)0x03)    /**< UART2_RX     alternate function mapping */
#define GPIOF5_AF3_UART2_TX                 ((uint8_t)0x03)    /**< UART2_TX         alternate function mapping */
#define GPIOF6_AF3_UART2_RX                 ((uint8_t)0x03)    /**< UART2_RX     alternate function mapping */
#define GPIOF7_AF3_UART2_CTSJ               ((uint8_t)0x03)    /**< UART2_CTSJ   alternate function mapping */
#define GPIOF8_AF3_UART2_RTSJ               ((uint8_t)0x03)    /**< UART2_RTSJ   alternate function mapping */
#define GPIOF12_AF3_NF_DAT0                 ((uint8_t)0x03)    /**< NF_DAT0      alternate function mapping */
#define GPIOF13_AF3_NF_DAT1                 ((uint8_t)0x03)    /**< NF_DAT1      alternate function mapping */
#define GPIOF14_AF3_NF_DAT2                 ((uint8_t)0x03)    /**< NF_DAT2      alternate function mapping */
#define GPIOF15_AF3_NF_DAT3                 ((uint8_t)0x03)    /**< NF_DAT3      alternate function mapping */
#define GPIOG0_AF3_NF_DAT4                  ((uint8_t)0x03)    /**< NF_DAT4          alternate function mapping */
#define GPIOG2_AF3_NF_DAT5                  ((uint8_t)0x03)    /**< NF_DAT5          alternate function mapping */
#define GPIOG3_AF3_NF_DAT6                  ((uint8_t)0x03)    /**< NF_DAT6          alternate function mapping */
#define GPIOG4_AF3_NF_DAT7                  ((uint8_t)0x03)    /**< NF_DAT7          alternate function mapping */
#define GPIOG5_AF3_NF_RBJ                   ((uint8_t)0x03)    /**< NF_RBJ       alternate function mapping */
#define GPIOG6_AF3_NF_REJ                   ((uint8_t)0x03)    /**< NF_REJ       alternate function mapping */
#define GPIOG7_AF3_NF_CEJ                   ((uint8_t)0x03)    /**< NF_CEJ       alternate function mapping */
#define GPIOG8_AF3_NF_CLE                   ((uint8_t)0x03)    /**< NF_CLE       alternate function mapping */
#define GPIOG9_AF3_NF_ALE                   ((uint8_t)0x03)    /**< NF_ALE       alternate function mapping */
#define GPIOG10_AF3_NF_WEJ                  ((uint8_t)0x03)    /**< NF_WEJ       alternate function mapping */
#define GPIOG11_AF3_NF_DQS                  ((uint8_t)0x03)    /**< NF_DQS       alternate function mapping */
#define GPIOG12_AF3_NF_WPJ                  ((uint8_t)0x03)    /**< NF_WPJ       alternate function mapping */
#define GPIOH0_AF3_OTG_FS_SOF               ((uint8_t)0x03)    /**< OTG_FS_SOF   alternate function mapping */
#define GPIOH1_AF3_D_USB_VBUSEN             ((uint8_t)0x03)    /**< D_USB_VBUSEN  alternate function mapping */
#define GPIOH12_AF3_C2_SWCLK                ((uint8_t)0x03)    /**< C2_SWCLK     alternate function mapping */
#define GPIOH13_AF3_C2_SWDIO                ((uint8_t)0x03)    /**< C2_SWDIO     alternate function mapping */
#define GPIOH14_AF3_CAN0_RX                 ((uint8_t)0x03)    /**< CAN0_RX      alternate function mapping */
#define GPIOH15_AF3_CAN0_TX                 ((uint8_t)0x03)    /**< CAN0_TX      alternate function mapping */
#define GPIOI0_AF3_CAN0_DBG                 ((uint8_t)0x03)    /**< CAN0_DBG     alternate function mapping */
#define GPIOI2_AF3_I2C0_SCL                 ((uint8_t)0x03)    /**< I2C0_SCL     alternate function mapping */
#define GPIOI3_AF3_I2C0_SDA                 ((uint8_t)0x03)    /**< I2C0_SDA     alternate function mapping */
#define GPIOJ1_AF3_SAI_MCLK_A               ((uint8_t)0x03)    /**< SAI_MCLK_A   alternate function mapping */
#define GPIOJ2_AF3_SAI_FS_A                 ((uint8_t)0x03)    /**< SAI_FS_A     alternate function mapping */
#define GPIOJ3_AF3_SAI_BCK_A                ((uint8_t)0x03)    /**< SAI_BCK_A    alternate function mapping */
#define GPIOJ4_AF3_SAI_SD_A                 ((uint8_t)0x03)    /**< SAI_SD_A     alternate function mapping */
#define GPIOJ5_AF3_SAI_SD_B                 ((uint8_t)0x03)    /**< SAI_SD_B     alternate function mapping */
#define GPIOJ6_AF3_SAI_MCLK_B               ((uint8_t)0x03)    /**< SAI_MCLK_B   alternate function mapping */
#define GPIOJ7_AF3_SAI_BCK_B                ((uint8_t)0x03)    /**< SAI_BCK_B    alternate function mapping */
#define GPIOJ8_AF3_SAI_FS_B                 ((uint8_t)0x03)    /**< SAI_FS_B     alternate function mapping */

/**
  * @brief   AF 4 selection
  */
#define GPIOC12_AF4_SAI_MCLK_A              ((uint8_t)0x04)    /**< SAI_MCLK_A       alternate function mapping */
#define GPIOC13_AF4_SAI_FS_A                ((uint8_t)0x04)    /**< SAI_FS_A         alternate function mapping */
#define GPIOC14_AF4_SAI_BCK_A               ((uint8_t)0x04)    /**< SAI_BCK_A        alternate function mapping */
#define GPIOC15_AF4_SAI_SD_A                ((uint8_t)0x04)    /**< SAI_SD_A         alternate function mapping */
#define GPIOD0_AF4_SAI_SD_B                 ((uint8_t)0x04)    /**< SAI_SD_B         alternate function mapping */
#define GPIOD1_AF4_SAI_MCLK_B               ((uint8_t)0x04)    /**< SAI_MCLK_B       alternate function mapping */
#define GPIOD2_AF4_SAI_BCK_B                ((uint8_t)0x04)    /**< SAI_BCK_B        alternate function mapping */
#define GPIOD3_AF4_SAI_FS_B                 ((uint8_t)0x04)    /**< SAI_FS_B         alternate function mapping */
#define GPIOE1_AF4_UART1_TX                 ((uint8_t)0x04)    /**< UART1_TX         alternate function mapping */
#define GPIOE1_AF4_UART1_RX                 ((uint8_t)0x04)    /**< UART1_RX         alternate function mapping */
#define GPIOE2_AF4_UART1_RX                 ((uint8_t)0x04)    /**< UART1_RX         alternate function mapping */
#define GPIOE3_AF4_I2C2_SCL                 ((uint8_t)0x04)    /**< I2C2_SCL         alternate function mapping */
#define GPIOE4_AF4_I2C2_SDA                 ((uint8_t)0x04)    /**< I2C2_SDA         alternate function mapping */
#define GPIOE5_AF4_UART2_TX                 ((uint8_t)0x04)    /**< UART2_TX             alternate function mapping */
#define GPIOE5_AF4_UART2_RX                 ((uint8_t)0x04)    /**< UART2_RX             alternate function mapping */
#define GPIOE6_AF4_UART2_RX                 ((uint8_t)0x04)    /**< UART2_RX             alternate function mapping */
#define GPIOF5_AF4_SPI1_SS                  ((uint8_t)0x04)    /**< SPI1_SS          alternate function mapping */
#define GPIOF6_AF4_SPI1_SCK                 ((uint8_t)0x04)    /**< SPI1_SCK         alternate function mapping */
#define GPIOF7_AF4_SPI1_MISO                ((uint8_t)0x04)    /**< SPI1_MISO        alternate function mapping */
#define GPIOF8_AF4_SPI1_MOSI                ((uint8_t)0x04)    /**< SPI1_MOSI        alternate function mapping */
#define GPIOF12_AF4_EXT_SFLASH_MISO         ((uint8_t)0x04)    /**< EXT_SFLASH_MISO  alternate function mapping */
#define GPIOF13_AF4_EXT_SFLASH_SCK          ((uint8_t)0x04)    /**< EXT_SFLASH_SCK   alternate function mapping */
#define GPIOF14_AF4_EXT_SFLASH_HOLDJ        ((uint8_t)0x04)    /**< EXT_SFLASH_HOLDJ alternate function mapping */
#define GPIOF15_AF4_EXT_SFLASH_WJ           ((uint8_t)0x04)    /**< EXT_SFLASH_WJ    alternate function mapping */
#define GPIOG0_AF4_EXT_SFLASH_MOSI          ((uint8_t)0x04)    /**< EXT_SFLASH_MOSI      alternate function mapping */
#define GPIOG1_AF4_EXT_SFLASH_CSJ           ((uint8_t)0x04)    /**< EXT_SFLASH_CSJ   alternate function mapping */
#define GPIOH0_AF4_D_USB_VBUSEN             ((uint8_t)0x04)    /**< D_USB_VBUSEN     alternate function mapping */
#define GPIOI2_AF4_UART0_TX                 ((uint8_t)0x04)    /**< UART0_TX             alternate function mapping */
#define GPIOI2_AF4_UART0_RX                 ((uint8_t)0x04)    /**< UART0_RX         alternate function mapping */
#define GPIOI3_AF4_UART0_RX                 ((uint8_t)0x04)    /**< UART0_RX         alternate function mapping */
#define GPIOJ1_AF4_PDM_MCLK_A               ((uint8_t)0x04)    /**< PDM_MCLK_A       alternate function mapping */
#define GPIOJ2_AF4_PDM_FS_A                 ((uint8_t)0x04)    /**< PDM_FS_A             alternate function mapping */
#define GPIOJ3_AF4_PDM_BCK_A                ((uint8_t)0x04)    /**< PDM_BCK_A        alternate function mapping */
#define GPIOJ4_AF4_PDM_SD_A                 ((uint8_t)0x04)    /**< PDM_SD_A             alternate function mapping */
#define GPIOJ5_AF4_PDM_SD_B                 ((uint8_t)0x04)    /**< PDM_SD_B             alternate function mapping */
#define GPIOJ6_AF4_PDM_MCLK_B               ((uint8_t)0x04)    /**< PDM_MCLK_B           alternate function mapping */
#define GPIOJ7_AF4_PDM_BCK_B                ((uint8_t)0x04)    /**< PDM_BCK_B        alternate function mapping */
#define GPIOJ8_AF4_PDM_FS_B                 ((uint8_t)0x04)    /**< PDM_FS_B             alternate function mapping */


/**
  * @brief   AF 5 selection
  */
#define GPIOA0_AF5_SDRAM_D16                ((uint8_t)0x05)    /**< SDRAM_D16    alternate function mapping */
#define GPIOA1_AF5_SDRAM_D17                ((uint8_t)0x05)    /**< SDRAM_D17    alternate function mapping */
#define GPIOA2_AF5_SDRAM_D18                ((uint8_t)0x05)    /**< SDRAM_D18    alternate function mapping */
#define GPIOA3_AF5_SDRAM_D19                ((uint8_t)0x05)    /**< SDRAM_D19    alternate function mapping */
#define GPIOA4_AF5_SDRAM_D20                ((uint8_t)0x05)    /**< SDRAM_D20    alternate function mapping */
#define GPIOA5_AF5_SDRAM_D21                ((uint8_t)0x05)    /**< SDRAM_D21    alternate function mapping */
#define GPIOA6_AF5_SDRAM_D22                ((uint8_t)0x05)    /**< SDRAM_D22     alternate function mapping */
#define GPIOA7_AF5_SDRAM_D23                ((uint8_t)0x05)    /**< SDRAM_D23     alternate function mapping */
#define GPIOA8_AF5_SDRAM_A2                 ((uint8_t)0x05)    /**< SDRAM_A2      alternate function mapping */
#define GPIOA9_AF5_SDRAM_A3                 ((uint8_t)0x05)    /**< SDRAM_A3      alternate function mapping */
#define GPIOA10_AF5_SDRAM_A0                ((uint8_t)0x05)    /**< SDRAM_A0     alternate function mapping */
#define GPIOA11_AF5_SDRAM_A1                ((uint8_t)0x05)    /**< SDRAM_A1     alternate function mapping */
#define GPIOA12_AF5_SDRAM_BA1               ((uint8_t)0x05)    /**< SDRAM_BA1    alternate function mapping */
#define GPIOA13_AF5_SDRAM_A10               ((uint8_t)0x05)    /**< SDRAM_A10    alternate function mapping */
#define GPIOA14_AF5_SDRAM_BA0               ((uint8_t)0x05)    /**< SDRAM_BA0    alternate function mapping */
#define GPIOA15_AF5_SDRAM_SDNE1             ((uint8_t)0x05)    /**< SDRAM_SDNE1  alternate function mapping */
#define GPIOB0_AF5_SDRAM_SDNEAS             ((uint8_t)0x05)    /**< SDRAM_SDNEAS     alternate function mapping */
#define GPIOB1_AF5_SDRAM_SDNE0              ((uint8_t)0x05)    /**< SDRAM_SDNE0  alternate function mapping */
#define GPIOB2_AF5_SDRAM_SDNCAS             ((uint8_t)0x05)    /**< SDRAM_SDNCAS     alternate function mapping */
#define GPIOB3_AF5_SDRAM_NBL0               ((uint8_t)0x05)    /**< SDRAM_NBL0   alternate function mapping */
#define GPIOB4_AF5_SDRAM_SDNWE              ((uint8_t)0x05)    /**< SDRAM_SDNWE  alternate function mapping */
#define GPIOB5_AF5_SDRAM_NBL2               ((uint8_t)0x05)    /**< SDRAM_NBL2   alternate function mapping */
#define GPIOC2_AF5_TIM8_BKIN                ((uint8_t)0x05)    /**< TIM8_BKIN    alternate function mapping */
#define GPIOC3_AF5_TIM8_ETR                 ((uint8_t)0x05)    /**< TIM8_ETR         alternate function mapping */
#define GPIOC4_AF5_TIM8_CH1N                ((uint8_t)0x05)    /**< TIM8_CH1N    alternate function mapping */
#define GPIOC5_AF5_TIM8_CH1                 ((uint8_t)0x05)    /**< TIM8_CH1     alternate function mapping */
#define GPIOC8_AF5_TIM8_CH1N                ((uint8_t)0x05)    /**< TIM8_CH1N    alternate function mapping */
#define GPIOC9_AF5_TIM8_CH2N                ((uint8_t)0x05)    /**< TIM8_CH2N    alternate function mapping */
#define GPIOC10_AF5_TIM8_CH2                ((uint8_t)0x05)    /**< TIM8_CH2     alternate function mapping */
#define GPIOC11_AF5_TIM8_CH3N               ((uint8_t)0x05)    /**< TIM8_CH3N    alternate function mapping */
#define GPIOC12_AF5_TIM8_CH3                ((uint8_t)0x05)    /**< TIM8_CH3     alternate function mapping */
#define GPIOC14_AF5_TIM8_CH4                ((uint8_t)0x05)    /**< TIM8_CH4     alternate function mapping */
#define GPIOC15_AF5_TIM8_CH4N               ((uint8_t)0x05)    /**< TIM8_CH4N    alternate function mapping */
#define GPIOD4_AF5_PDM_MCLK_A               ((uint8_t)0x05)    /**< PDM_MCLK_A   alternate function mapping */
#define GPIOD5_AF5_PDM_FS_A                 ((uint8_t)0x05)    /**< PDM_FS_A         alternate function mapping */
#define GPIOD6_AF5_PDM_BCK_A                ((uint8_t)0x05)    /**< PDM_BCK_A    alternate function mapping */
#define GPIOD7_AF5_PDM_SD_A                 ((uint8_t)0x05)    /**< PDM_SD_A         alternate function mapping */
#define GPIOD8_AF5_PDM_SD_B                 ((uint8_t)0x05)    /**< PDM_SD_B         alternate function mapping */
#define GPIOD9_AF5_PDM_MCLK_B               ((uint8_t)0x05)    /**< PDM_MCLK_B       alternate function mapping */
#define GPIOD10_AF5_PDM_BCK_B               ((uint8_t)0x05)    /**< PDM_BCK_B    alternate function mapping */
#define GPIOD11_AF5_PDM_FS_B                ((uint8_t)0x05)    /**< PDM_FS_B     alternate function mapping */
#define GPIOE1_AF5_TIM1_CH1N                ((uint8_t)0x05)    /**< TIM1_CH1N    alternate function mapping */
#define GPIOE2_AF5_TIM1_CH1                 ((uint8_t)0x05)    /**< TIM1_CH1     alternate function mapping */
#define GPIOE3_AF5_TIM1_CH2N                ((uint8_t)0x05)    /**< TIM1_CH2N    alternate function mapping */
#define GPIOE4_AF5_TIM1_CH2                 ((uint8_t)0x05)    /**< TIM1_CH2     alternate function mapping */
#define GPIOE5_AF5_TIM1_CH3N                ((uint8_t)0x05)    /**< TIM1_CH3N    alternate function mapping */
#define GPIOE6_AF5_TIM1_CH3                 ((uint8_t)0x05)    /**< TIM1_CH3     alternate function mapping */
#define GPIOE7_AF5_TIM1_CH4                 ((uint8_t)0x05)    /**< TIM1_CH4         alternate function mapping */
#define GPIOE8_AF5_TIM1_CH4N                ((uint8_t)0x05)    /**< TIM1_CH4N    alternate function mapping */
#define GPIOE9_AF5_TIM1_BKIN                ((uint8_t)0x05)    /**< TIM1_BKIN    alternate function mapping */
#define GPIOE10_AF5_TIM1_ETR                ((uint8_t)0x05)    /**< TIM1_ETR     alternate function mapping */
#define GPIOE11_AF5_TIM2_CH1                ((uint8_t)0x05)    /**< TIM2_CH1     alternate function mapping */
#define GPIOE12_AF5_TIM2_CH2                ((uint8_t)0x05)    /**< TIM2_CH2     alternate function mapping */
#define GPIOE13_AF5_TIM2_CH3                ((uint8_t)0x05)    /**< TIM2_CH3     alternate function mapping */
#define GPIOE14_AF5_TIM2_CH4                ((uint8_t)0x05)    /**< TIM2_CH4     alternate function mapping */
#define GPIOE15_AF5_TIM2_ETR                ((uint8_t)0x05)    /**< TIM2_ETR     alternate function mapping */
#define GPIOF0_AF5_TIM2_BKIN                ((uint8_t)0x05)    /**< TIM2_BKIN    alternate function mapping */
#define GPIOF1_AF5_TIM3_CH1                 ((uint8_t)0x05)    /**< TIM3_CH1     alternate function mapping */
#define GPIOF2_AF5_TIM3_CH2                 ((uint8_t)0x05)    /**< TIM3_CH2     alternate function mapping */
#define GPIOF3_AF5_TIM3_CH3                 ((uint8_t)0x05)    /**< TIM3_CH3     alternate function mapping */
#define GPIOF4_AF5_TIM3_CH4                 ((uint8_t)0x05)    /**< TIM3_CH4     alternate function mapping */
#define GPIOF5_AF5_TIM3_ETR                 ((uint8_t)0x05)    /**< TIM3_ETR     alternate function mapping */
#define GPIOH10_AF5_TIM5_ETR                ((uint8_t)0x05)    /**< TIM5_ETR     alternate function mapping */
#define GPIOH11_AF5_TIM4_ETR                ((uint8_t)0x05)    /**< TIM4_ETR     alternate function mapping */
#define GPIOH12_AF5_TIM4_CH1                ((uint8_t)0x05)    /**< TIM4_CH1     alternate function mapping */
#define GPIOH13_AF5_TIM4_CH2                ((uint8_t)0x05)    /**< TIM4_CH2     alternate function mapping */
#define GPIOH14_AF5_TIM4_CH3                ((uint8_t)0x05)    /**< TIM4_CH3     alternate function mapping */
#define GPIOH15_AF5_TIM4_CH4                ((uint8_t)0x05)    /**< TIM4_CH4     alternate function mapping */
#define GPIOI0_AF5_TIM5_CH1                 ((uint8_t)0x05)    /**< TIM5_CH1     alternate function mapping */
#define GPIOI1_AF5_TIM5_CH2                 ((uint8_t)0x05)    /**< TIM5_CH2     alternate function mapping */
#define GPIOI2_AF5_TIM5_CH3                 ((uint8_t)0x05)    /**< TIM5_CH3     alternate function mapping */
#define GPIOI3_AF5_TIM5_CH4                 ((uint8_t)0x05)    /**< TIM5_CH4     alternate function mapping */
#define GPIOI4_AF5_SDRAM_NBL3               ((uint8_t)0x05)    /**< SDRAM_NBL3   alternate function mapping */
#define GPIOI5_AF5_SDRAM_SDCKE1             ((uint8_t)0x05)    /**< SDRAM_SDCKE1     alternate function mapping */
#define GPIOI6_AF5_SDRAM_NBL1               ((uint8_t)0x05)    /**< SDRAM_NBL1   alternate function mapping */
#define GPIOI7_AF5_SDRAM_SDCLK              ((uint8_t)0x05)    /**< SDRAM_SDCLK  alternate function mapping */
#define GPIOI8_AF5_SDRAM_SDCKE0             ((uint8_t)0x05)    /**< SDRAM_SDCKE0     alternate function mapping */
#define GPIOI9_AF5_SDRAM_A12                ((uint8_t)0x05)    /**< SDRAM_A12    alternate function mapping */
#define GPIOI10_AF5_SDRAM_A11               ((uint8_t)0x05)    /**< SDRAM_A11    alternate function mapping */
#define GPIOI11_AF5_SDRAM_A9                ((uint8_t)0x05)    /**< SDRAM_A9     alternate function mapping */
#define GPIOI12_AF5_SDRAM_A8                ((uint8_t)0x05)    /**< SDRAM_A8     alternate function mapping */
#define GPIOI13_AF5_SDRAM_A7                ((uint8_t)0x05)    /**< SDRAM_A7     alternate function mapping */
#define GPIOI14_AF5_SDRAM_A6                ((uint8_t)0x05)    /**< SDRAM_A6     alternate function mapping */
#define GPIOI15_AF5_SDRAM_A5                ((uint8_t)0x05)    /**< SDRAM_A5     alternate function mapping */
#define GPIOJ0_AF5_SDRAM_A4                 ((uint8_t)0x05)    /**< SDRAM_A4         alternate function mapping */
#define GPIOJ1_AF5_SDRAM_D24                ((uint8_t)0x05)    /**< SDRAM_D24    alternate function mapping */
#define GPIOJ2_AF5_SDRAM_D25                ((uint8_t)0x05)    /**< SDRAM_D25    alternate function mapping */
#define GPIOJ3_AF5_SDRAM_D26                ((uint8_t)0x05)    /**< SDRAM_D26    alternate function mapping */
#define GPIOJ4_AF5_SDRAM_D27                ((uint8_t)0x05)    /**< SDRAM_D27    alternate function mapping */
#define GPIOJ5_AF5_SDRAM_D28                ((uint8_t)0x05)    /**< SDRAM_D28    alternate function mapping */
#define GPIOJ6_AF5_SDRAM_D29                ((uint8_t)0x05)    /**< SDRAM_D29    alternate function mapping */
#define GPIOJ7_AF5_SDRAM_D30                ((uint8_t)0x05)    /**< SDRAM_D30    alternate function mapping */
#define GPIOJ8_AF5_SDRAM_D31                ((uint8_t)0x05)    /**< SDRAM_D31    alternate function mapping */
#define GPIOJ9_AF5_SDRAM_D9                 ((uint8_t)0x05)    /**< SDRAM_D9         alternate function mapping */
#define GPIOJ10_AF5_SDRAM_D8                ((uint8_t)0x05)    /**< SDRAM_D8     alternate function mapping */
#define GPIOJ11_AF5_SDRAM_D10               ((uint8_t)0x05)    /**< SDRAM_D10    alternate function mapping */
#define GPIOJ12_AF5_SDRAM_D11               ((uint8_t)0x05)    /**< SDRAM_D11    alternate function mapping */
#define GPIOJ13_AF5_SDRAM_D12               ((uint8_t)0x05)    /**< SDRAM_D12    alternate function mapping */
#define GPIOJ14_AF5_SDRAM_D13               ((uint8_t)0x05)    /**< SDRAM_D13    alternate function mapping */
#define GPIOJ15_AF5_SDRAM_D15               ((uint8_t)0x05)    /**< SDRAM_D15    alternate function mapping */
#define GPIOK0_AF5_SDRAM_D14                ((uint8_t)0x05)    /**< SDRAM_D14    alternate function mapping */
#define GPIOK1_AF5_SDRAM_D0                 ((uint8_t)0x05)    /**< SDRAM_D0         alternate function mapping */
#define GPIOK2_AF5_SDRAM_D2                 ((uint8_t)0x05)    /**< SDRAM_D2         alternate function mapping */
#define GPIOK3_AF5_SDRAM_D1                 ((uint8_t)0x05)    /**< SDRAM_D1         alternate function mapping */
#define GPIOK4_AF5_SDRAM_D3                 ((uint8_t)0x05)    /**< SDRAM_D3         alternate function mapping */
#define GPIOK5_AF5_SDRAM_D4                 ((uint8_t)0x05)    /**< SDRAM_D4     alternate function mapping */
#define GPIOK6_AF5_SDRAM_D5                 ((uint8_t)0x05)    /**< SDRAM_D5     alternate function mapping */
#define GPIOK7_AF5_SDRAM_D6                 ((uint8_t)0x05)    /**< SDRAM_D6     alternate function mapping */
#define GPIOK8_AF5_SDRAM_D7                 ((uint8_t)0x05)    /**< SDRAM_D7     alternate function mapping */

/**
  * @brief   AF 6 selection
  */
#define GPIOC14_AF6_UART6_TX                ((uint8_t)0x06)    /**< UART6_TX         alternate function mapping */
#define GPIOC14_AF6_UART6_RX                ((uint8_t)0x06)    /**< UART6_RX         alternate function mapping */
#define GPIOC15_AF6_UART6_RX                ((uint8_t)0x06)    /**< UART6_RX         alternate function mapping */
#define GPIOD2_AF6_UART7_TX                 ((uint8_t)0x06)    /**< UART7_TX         alternate function mapping */
#define GPIOD2_AF6_UART7_RX                 ((uint8_t)0x06)    /**< UART7_RX         alternate function mapping */
#define GPIOD3_AF6_UART7_RX                 ((uint8_t)0x06)    /**< UART7_RX         alternate function mapping */
#define GPIOK9_AF6_INT_SFLASH_MOSI          ((uint8_t)0x06)    /**< INT_SFLASH_MOSI  alternate function mapping */
#define GPIOK10_AF6_INT_SFLASH_SCK          ((uint8_t)0x06)    /**< INT_SFLASH_SCK   alternate function mapping */
#define GPIOK11_AF6_INT_SFLASH_HOLDJ        ((uint8_t)0x06)    /**< INT_SFLASH_HOLDJ alternate function mapping */
#define GPIOE10_AF6_VIN2_D2                 ((uint8_t)0x06)    /**< VIN2_D2          alternate function mapping */
#define GPIOE11_AF6_VIN2_D1                 ((uint8_t)0x06)    /**< VIN2_D1          alternate function mapping */
#define GPIOE12_AF6_VIN2_D3                 ((uint8_t)0x06)    /**< VIN2_D3          alternate function mapping */
#define GPIOE13_AF6_VIN2_D0                 ((uint8_t)0x06)    /**< VIN2_D0          alternate function mapping */
#define GPIOE14_AF6_VIN2_D4                 ((uint8_t)0x06)    /**< VIN2_D4          alternate function mapping */
#define GPIOE15_AF6_VIN2_PIXCLK             ((uint8_t)0x06)    /**< VIN2_PIXCLK      alternate function mapping */
#define GPIOF0_AF6_VIN2_D5                  ((uint8_t)0x06)    /**< VIN2_D5              alternate function mapping */
#define GPIOF1_AF6_VIN2_D6                  ((uint8_t)0x06)    /**< VIN2_D6              alternate function mapping */
#define GPIOF2_AF6_VIN2_D7                  ((uint8_t)0x06)    /**< VIN2_D7              alternate function mapping */
#define GPIOF3_AF6_VIN2_HSYNC               ((uint8_t)0x06)    /**< VIN2_HSYNC       alternate function mapping */
#define GPIOF4_AF6_VIN2_HSYNC               ((uint8_t)0x06)    /**< VIN2_HSYNC       alternate function mapping */
#define GPIOG2_AF6_SDMMC_D2                 ((uint8_t)0x06)    /**< SDMMC_D2         alternate function mapping */
#define GPIOG3_AF6_SDMMC_D3                 ((uint8_t)0x06)    /**< SDMMC_D3         alternate function mapping */
#define GPIOG4_AF6_SDMMC_CMD                ((uint8_t)0x06)    /**< SDMMC_CMD        alternate function mapping */
#define GPIOG5_AF6_SDMMC_D1                 ((uint8_t)0x06)    /**< SDMMC_D1         alternate function mapping */
#define GPIOG6_AF6_SDMMC_D0                 ((uint8_t)0x06)    /**< SDMMC_D0         alternate function mapping */
#define GPIOG7_AF6_SDMMC_CLK                ((uint8_t)0x06)    /**< SDMMC_CLK        alternate function mapping */
#define GPIOG8_AF6_SDMMC_D4                 ((uint8_t)0x06)    /**< SDMMC_D4         alternate function mapping */
#define GPIOG9_AF6_SDMMC_D5                 ((uint8_t)0x06)    /**< SDMMC_D5         alternate function mapping */
#define GPIOG10_AF6_SDMMC_D6                ((uint8_t)0x06)    /**< SDMMC_D6         alternate function mapping */
#define GPIOG12_AF6_SDMMC_D7                ((uint8_t)0x06)    /**< SDMMC_D7         alternate function mapping */
#define GPIOH12_AF6_PDM_MCLK_A              ((uint8_t)0x06)    /**< PDM_MCLK_A       alternate function mapping */
#define GPIOH13_AF6_PDM_FS_A                ((uint8_t)0x06)    /**< PDM_FS_A         alternate function mapping */
#define GPIOH14_AF6_PDM_BCK_A               ((uint8_t)0x06)    /**< PDM_BCK_A        alternate function mapping */
#define GPIOH15_AF6_PDM_SD_A                ((uint8_t)0x06)    /**< PDM_SD_A         alternate function mapping */
#define GPIOI0_AF6_PDM_SD_B                 ((uint8_t)0x06)    /**< PDM_SD_B             alternate function mapping */
#define GPIOI1_AF6_PDM_MCLK_B               ((uint8_t)0x06)    /**< PDM_MCLK_B           alternate function mapping */
#define GPIOI2_AF6_PDM_BCK_B                ((uint8_t)0x06)    /**< PDM_BCK_B        alternate function mapping */
#define GPIOI3_AF6_PDM_FS_B                 ((uint8_t)0x06)    /**< PDM_FS_B             alternate function mapping */
#define GPIOK12_AF6_INT_SFLASH_CSJ          ((uint8_t)0x06)    /**< INT_SFLASH_CSJ   alternate function mapping */
#define GPIOK13_AF6_INT_SFLASH_MISO         ((uint8_t)0x06)    /**< INT_SFLASH_MISO  alternate function mapping */
#define GPIOK14_AF6_INT_SFLASH_WJ           ((uint8_t)0x06)    /**< INT_SFLASH_WJ    alternate function mapping */


/**
  * @brief   AF 7 selection
  */
#define GPIOA0_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOA1_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOA2_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOA3_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOA4_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOA5_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOA6_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOA7_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOA8_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOA9_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOA10_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOA11_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOA12_AF7_PES_CH_EV0              ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOA13_AF7_PES_CH_EV1              ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOA14_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOA15_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOB0_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOB1_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOB2_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOB3_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOB4_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOB5_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOB6_AF7_PES_CH__EV10             ((uint8_t)0x07)    /**< PES_CH__EV10     alternate function mapping */
#define GPIOB7_AF7_PES_CH__EV11             ((uint8_t)0x07)    /**< PES_CH__EV11     alternate function mapping */
#define GPIOB8_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOB9_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOB10_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOB11_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOB12_AF7_PES_CH_EV4              ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOB13_AF7_PES_CH_EV5              ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOB14_AF7_PES_CH_EV6              ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOB15_AF7_PES_CH_EV7              ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOC0_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOC1_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOC2_AF7_PES_CH__EV10             ((uint8_t)0x07)    /**< PES_CH__EV10     alternate function mapping */
#define GPIOC3_AF7_PES_CH__EV11             ((uint8_t)0x07)    /**< PES_CH__EV11     alternate function mapping */
#define GPIOC4_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOC5_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOC6_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOC7_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOC8_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOC9_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOC10_AF7_PES_CH_EV6              ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOC11_AF7_PES_CH_EV7              ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOC12_AF7_PES_CH_EV8              ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOC13_AF7_PES_CH_EV9              ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOC14_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOC15_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOD0_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOD1_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOD2_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOD3_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOD4_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOD5_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOD6_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOD7_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOD8_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOD9_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOD10_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOD11_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOD12_AF7_PES_CH_EV0              ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOD13_AF7_PES_CH_EV1              ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOD14_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOD15_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOE0_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOE1_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOE2_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOE3_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOE4_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOE5_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOE6_AF7_PES_CH_EV10              ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOE7_AF7_PES_CH_EV11              ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOE8_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOE9_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOE10_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOE11_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOE12_AF7_PES_CH_EV4              ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOE13_AF7_PES_CH_EV5              ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOE14_AF7_PES_CH_EV6              ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOE15_AF7_PES_CH_EV7              ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOF0_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOF1_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOF2_AF7_PES_CH_EV10              ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOF3_AF7_PES_CH_EV11              ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOF4_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOF5_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOF6_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOF7_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOF8_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOF9_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOF10_AF7_PES_CH_EV6              ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOF11_AF7_PES_CH_EV7              ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOF12_AF7_PES_CH_EV8              ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOF13_AF7_PES_CH_EV9              ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOF14_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOF15_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOG0_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOG1_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOG2_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOG3_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOG4_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOG5_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOG6_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOG7_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOG8_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOG9_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOG10_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOG11_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOG12_AF7_PES_CH_EV0              ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOG13_AF7_PES_CH_EV1              ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOG14_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOG15_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOH0_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOH1_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOH2_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOH3_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOH4_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOH5_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOH6_AF7_PES_CH_EV10              ((uint8_t)0x07)    /**< PES_CH_EV10   alternate function mapping */
#define GPIOH7_AF7_PES_CH_EV11              ((uint8_t)0x07)    /**< PES_CH_EV11   alternate function mapping */
#define GPIOH8_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOH9_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOH10_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOH11_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOH12_AF7_PES_CH_EV4              ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOH13_AF7_PES_CH_EV5              ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOH14_AF7_PES_CH_EV6              ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOH15_AF7_PES_CH_EV7              ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOI0_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOI1_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOI2_AF7_PES_CH_EV10              ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOI3_AF7_PES_CH_EV11              ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOI4_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOI5_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOI6_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOI7_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOI8_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOI9_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOI10_AF7_PES_CH_EV6              ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOI11_AF7_PES_CH_EV7              ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOI12_AF7_PES_CH_EV8              ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOI13_AF7_PES_CH_EV9              ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOI14_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOI15_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOJ0_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOJ1_AF7_PES_CH_EV1               ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOJ2_AF7_PES_CH_EV2               ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOJ3_AF7_PES_CH_EV3               ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOJ4_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOJ5_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOJ6_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOJ7_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOJ8_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOJ9_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOJ10_AF7_PES_CH_EV10             ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOJ11_AF7_PES_CH_EV11             ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOJ12_AF7_PES_CH_EV0              ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */
#define GPIOJ13_AF7_PES_CH_EV1              ((uint8_t)0x07)    /**< PES_CH_EV1   alternate function mapping */
#define GPIOJ14_AF7_PES_CH_EV2              ((uint8_t)0x07)    /**< PES_CH_EV2   alternate function mapping */
#define GPIOJ15_AF7_PES_CH_EV3              ((uint8_t)0x07)    /**< PES_CH_EV3   alternate function mapping */
#define GPIOK0_AF7_PES_CH_EV4               ((uint8_t)0x07)    /**< PES_CH_EV4   alternate function mapping */
#define GPIOK1_AF7_PES_CH_EV5               ((uint8_t)0x07)    /**< PES_CH_EV5   alternate function mapping */
#define GPIOK2_AF7_PES_CH_EV6               ((uint8_t)0x07)    /**< PES_CH_EV6   alternate function mapping */
#define GPIOK3_AF7_PES_CH_EV7               ((uint8_t)0x07)    /**< PES_CH_EV7   alternate function mapping */
#define GPIOK4_AF7_PES_CH_EV8               ((uint8_t)0x07)    /**< PES_CH_EV8   alternate function mapping */
#define GPIOK5_AF7_PES_CH_EV9               ((uint8_t)0x07)    /**< PES_CH_EV9   alternate function mapping */
#define GPIOK6_AF7_PES_CH_EV10              ((uint8_t)0x07)    /**< PES_CH_EV10  alternate function mapping */
#define GPIOK7_AF7_PES_CH_EV11              ((uint8_t)0x07)    /**< PES_CH_EV11  alternate function mapping */
#define GPIOK8_AF7_PES_CH_EV0               ((uint8_t)0x07)    /**< PES_CH_EV0   alternate function mapping */

/**
  * @brief   AF 8 selection
  */
#define GPIOC10_AF8_VIN2_D2                 ((uint8_t)0x08)    /**< VIN2_D2      alternate function mapping */
#define GPIOC11_AF8_VIN2_D1                 ((uint8_t)0x08)    /**< VIN2_D1      alternate function mapping */
#define GPIOC12_AF8_VIN2_D3                 ((uint8_t)0x08)    /**< VIN2_D3      alternate function mapping */
#define GPIOC13_AF8_VIN2_D0                 ((uint8_t)0x08)    /**< VIN2_D0      alternate function mapping */
#define GPIOC14_AF8_VIN2_D4                 ((uint8_t)0x08)    /**< VIN2_D4      alternate function mapping */
#define GPIOC15_AF8_VIN2_PIXCLK             ((uint8_t)0x08)    /**< VIN2_PIXCLK  alternate function mapping */
#define GPIOD0_AF8_VIN2_D5                  ((uint8_t)0x08)    /**< VIN2_D5      alternate function mapping */
#define GPIOD1_AF8_VIN2_D6                  ((uint8_t)0x08)    /**< VIN2_D6      alternate function mapping */
#define GPIOD2_AF8_VIN2_D7                  ((uint8_t)0x08)    /**< VIN2_D7      alternate function mapping */
#define GPIOD3_AF8_VIN2_HSYNC               ((uint8_t)0x08)    /**< VIN2_HSYNC   alternate function mapping */
#define GPIOD4_AF8_VIN2_VSYHC               ((uint8_t)0x08)    /**< VIN2_VSYHC   alternate function mapping */
#define GPIOE1_AF8_PDM_MCLK_A               ((uint8_t)0x08)    /**< PDM_MCLK_A   alternate function mapping */
#define GPIOE2_AF8_PDM_FS_A                 ((uint8_t)0x08)    /**< PDM_FS_A         alternate function mapping */
#define GPIOE3_AF8_PDM_BCK_A                ((uint8_t)0x08)    /**< PDM_BCK_A    alternate function mapping */
#define GPIOE4_AF8_PDM_SD_A                 ((uint8_t)0x08)    /**< PDM_SD_A         alternate function mapping */
#define GPIOE5_AF8_PDM_SD_B                 ((uint8_t)0x08)    /**< PDM_SD_B         alternate function mapping */
#define GPIOE6_AF8_PDM_MCLK_B               ((uint8_t)0x08)    /**< PDM_MCLK_B       alternate function mapping */
#define GPIOE7_AF8_PDM_BCK_B                ((uint8_t)0x08)    /**< PDM_BCK_B    alternate function mapping */
#define GPIOE8_AF8_PDM_FS_B                 ((uint8_t)0x08)    /**< PDM_FS_B         alternate function mapping */

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
#define  GPIO_MODE_ANALOG                       ((uint32_t)0x00000003)   /**< Analog mode set 3*/
#define  GPIO_MODE_RESTORE_DEFAULT              ((uint32_t)0x00000004)   /**< PINMUX restore default mode */
#if defined(CHIP_F6721B)
#define  GPIO_MODE_ANALOG_2                                         ((uint32_t)0x00000005)      /**< Analog mode set 2*/
#endif

#define  GPIO_MODE_IT_LOW                       ((uint32_t)0x10000000)   /**< External Interrupt mode with low level trigger detection */
#define  GPIO_MODE_IT_FALLING                   ((uint32_t)0x10010000)   /**< External Interrupt mode with Falling edge trigger detection */
#define  GPIO_MODE_IT_HIGH                      ((uint32_t)0x10100000)   /**< External Interrupt mode with high level trigger detection */
#define  GPIO_MODE_IT_RISING                    ((uint32_t)0x10110000)   /**< External Interrupt mode with Rising edge trigger detection */

/**
  * @}
  */

/** @defgroup GPIO_pull_define GPIO pull define
  * @brief GPIO Pull-Up or Pull-Up  Activation
  * @{
  */
#define  GPIO_NOPULL        ((uint32_t)0x00000000)   /**< No Pull-up activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001)   /**< Pull-up activation */
#define  GPIO_PULLDOWN      ((uint32_t)0x00000002)   /**< Pull-down activation */
#define  GPIO_PULL_EABLE    ((uint32_t)0x00000001)
#define  GPIO_PULL_DISABLE  ((uint32_t)0x00000000)

#define  GPIO_PULL_MASK     ((uint32_t)0x00000003)

#define  GPIO_DRIVING_LEVEL0        ((uint32_t)0x00000000)
#define  GPIO_DRIVING_LEVEL1        ((uint32_t)0x00000001)
#define  GPIO_DRIVING_LEVEL2        ((uint32_t)0x00000002)
#define  GPIO_DRIVING_LEVEL3        ((uint32_t)0x00000003)
#define  GPIO_DRIVING_MASK          ((uint32_t)0x00000003)

/**
  * @}
  */


/** @defgroup FLT_clk_define input glitch filter clock define
  * @{
  */
#define  PAD_FLT_SRC_CLK                 ((uint32_t)0x00000000)   /**< slect clock source */

#define  PAD_FLT_CLK1                    ((uint32_t)0x00000000)   /**< slect clock1 */
#define  PAD_FLT_CLK2                    ((uint32_t)0x00000001)   /**< slect clock2 */
#define  PAD_FLT_CLK3                    ((uint32_t)0x00000002)   /**< slect clock3 */
#define  PAD_IRC_LS_CLK                  ((uint32_t)0x00000003)   /**< slect LIRC */

#define  DB_PARENT_SRC_CLK_SEL_LIRC                 (0)
#define  DB_PARENT_SRC_CLK_SEL_FCLK                 (1)

#define  SYS_FLTCPS_PARENT_CLK_SRC_MASK     ((uint32_t)0x00000003)
#define  SYS_FLTCPS_CLK_SRC_FIELD(x)        ((uint32_t)(((uint32_t)(x))&SYS_FLTCPS_PARENT_CLK_SRC_MASK))
#define  SYS_FLTCPS_CLK1_MASK               ((uint32_t)0x00000300)
#define  SYS_FLTCPS_CLK1_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<4)&SYS_FLTCPS_CLK1_MASK))
#define  SYS_FLTCPS_CLK2_MASK               ((uint32_t)0x00000700)
#define  SYS_FLTCPS_CLK2_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<8)&SYS_FLTCPS_CLK2_MASK))

#define  SYS_FLTCPS_CLK3_MASK               ((uint32_t)0x00003000)
#define  SYS_FLTCPS_CLK3_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<12)&SYS_FLTCPS_CLK3_MASK))


/**
  * @}
  */

/** @defgroup FLT_clk_psc_define Filter clock prescler define
  * @{
  */
#define  FLT_CLK1_PSC_DIV_2               ((uint32_t)0x00000000)   /**< Divided FILTER clock by 2, removed noise: 500ns */
#define  FLT_CLK1_PSC_DIV_4               ((uint32_t)0x00000001)   /**< Divided FILTER clock by 4, removed noise: 1us */
#define  FLT_CLK1_PSC_DIV_8               ((uint32_t)0x00000002)   /**< Divided FILTER clock by 8, removed noise: 2us */
#define  FLT_CLK1_PSC_DIV_16              ((uint32_t)0x00000003)   /**< Divided FILTER clock by 16, removed noise: 4us */

#define  FLT_CLK2_PSC_DIV_32              ((uint32_t)0x00000000)   /**< Divided FILTER clock by 32, removed noise: 8us */
#define  FLT_CLK2_PSC_DIV_64              ((uint32_t)0x00000001)   /**< Divided FILTER clock by 64, removed noise: 16us */
#define  FLT_CLK2_PSC_DIV_128             ((uint32_t)0x00000002)   /**< Divided FILTER clock by 128, removed noise: 32us */
#define  FLT_CLK2_PSC_DIV_256             ((uint32_t)0x00000003)   /**< Divided FILTER clock by 256, removed noise: 64us */
#define  FLT_CLK2_PSC_DIV_512             ((uint32_t)0x00000004)   /**< Divided FILTER clock by 512, removed noise: 128us */
#define  FLT_CLK2_PSC_DIV_1K              ((uint32_t)0x00000005)   /**< Divided FILTER clock by 1K, removed noise: 256us */
#define  FLT_CLK2_PSC_DIV_2K              ((uint32_t)0x00000006)   /**< Divided FILTER clock by 2K, removed noise: 512us */
#define  FLT_CLK2_PSC_DIV_4K              ((uint32_t)0x00000007)   /**< Divided FILTER clock by 4K, removed noise: 1ms */

#define  FLT_CLK3_PSC_DIV_8K              ((uint32_t)0x00000000)   /**< Divided FILTER clock by 8K, removed noise: 2ms */
#define  FLT_CLK3_PSC_DIV_16K             ((uint32_t)0x00000001)   /**< Divided FILTER clock by 16K, removed noise: 4ms */
#define  FLT_CLK3_PSC_DIV_32K             ((uint32_t)0x00000002)   /**< Divided FILTER clock by 32K, removed noise: 8ms */
#define  FLT_CLK3_PSC_DIV_64K             ((uint32_t)0x00000003)   /**< Divided FILTER clock by 64K, removed noise: 16ms */

/**
  * @}
  */

/**
  * @brief Sets input port glitch filter
  */
typedef struct
{
    uint32_t parent_clk_src_sel;             /**< Debounce source clock (DB_SRC_CLK) source setting */
    uint32_t child_clk_sel;                  /**<Debounce clockx source setting */
    uint32_t prescaler;                      /**< sourced by DB_SRC_CLK divide */
    uint8_t db_bypass_en;                    /**< IO debounce bypass control enable or disable */
} GPIO_InputGlitchFilter_T;


/**
  * @brief Sets input port glitch filter
  */
typedef struct
{
    uint32_t pin_sel;             /**< Select the pin which responds the incoming event */
    uint32_t channel_sel;         /**< Select the channel of PES that the event comes from */
    uint32_t action_sel;          /**< elect the actions that the pin will take when the event occurs */
    uint8_t event_action_en;      /**< The corresponding action on input event enable bit  */
} GPIO_PESConfig_T;


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
    uint32_t driving;
} GPIO_PinConfig_T;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GPIO_DEV_H_ */

