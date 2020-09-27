 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          clock.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/14
 *
 * @brief         Header file of clock driver.
 *
 * @note
 *    2016/07/04, wizephen.wang, V1.0.0
 *        Initial version.
 *    2017/12/25, wizephen.wang, V1.1.0
 *        Update code for GM6611B clock.
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>

#define OSC_MASK_EXTERNAL_HIGH     (0x01)     /**< External high oscillator mask */
#define OSC_MASK_INTERNAL_HIGH     (0x02)     /**< Internal high oscillator mask */
#define OSC_MASK_EXTERNAL_LOW      (0x04)     /**< External low oscillator mask */
#define OSC_MASK_INTERNAL_LOW      (0x08)     /**< Internal low oscillator mask */

#define OSC_EXTERNAL_HIGH_STATE    (0x03)     /**< External high oscillator state */
#define OSC_EXTERNAL_HIGH_ON       (0x01)     /**< External high oscillator on */
#define OSC_EXTERNAL_HIGH_OFF      (0x00)     /**< External high oscillator off */

#define OSC_INTERNAL_HIGH_STATE    (0x0c)     /**< Internal high oscillator state */
#define OSC_INTERNAL_HIGH_ON       (0x01<<2)  /**< Internal high oscillator on */
#define OSC_INTERNAL_HIGH_OFF      (0x00<<2)  /**< Internal high oscillator off */

#define OSC_EXTERNAL_LOW_STATE     (0x30)     /**< External low oscillator state */
#define OSC_EXTERNAL_LOW_ON        (0x01<<4)  /**< External low oscillator on */
#define OSC_EXTERNAL_LOW_OFF       (0x00<<4)  /**< External low oscillator off */

#define OSC_INTERNAL_LOW_STATE     (0xc0)     /**< Internal low oscillator state */
#define OSC_INTERNAL_LOW_ON        (0x01<<6)  /**< Internal low oscillator on */
#define OSC_INTERNAL_LOW_OFF       (0x00<<6)  /**< Internal low oscillator off */

#define CLOCK_MASK_SYSCLK          (0x01)     /**< System clock mask*/

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined (CHIP_GM6611) ||\
     defined (CHIP_GM6621) || defined (CHIP_GM6611B))

#if defined(CHIP_GM6601)
#define CLOCK_PERIPH_ETM_MASK      ((uint32_t)(0x00000001)) /**< Peripheral clock masked for ETM */
#define CLOCK_PERIPH_ADC_MASK      ((uint32_t)(0x00000002)) /**< Peripheral clock masked for ADC */
#define CLOCK_PERIPH_WDT_MASK      ((uint32_t)(0x00000004)) /**< Peripheral clock masked for WDT */
#define CLOCK_PERIPH_RTC2_MASK     ((uint32_t)(0x00000008)) /**< Peripheral clock masked for RTC2 */
#define CLOCK_PERIPH_RTC1_MASK     ((uint32_t)(0x00000010)) /**< Peripheral clock masked for RTC1 */
#define CLOCK_PERIPH_RTC0_MASK     ((uint32_t)(0x00000020)) /**< Peripheral clock masked for RTC0 */
#define CLOCK_PERIPH_UART2_MASK    ((uint32_t)(0x00000040)) /**< Peripheral clock masked for UART2 */
#define CLOCK_PERIPH_UART1_MASK    ((uint32_t)(0x00000080)) /**< Peripheral clock masked for UART1 */
#define CLOCK_PERIPH_UART0_MASK    ((uint32_t)(0x00000100)) /**< Peripheral clock masked for UART0 */

#define ETM_CLOCK_8MHZ             (0x0) /**< ETM clock is 8MHz */
#define ETM_CLOCK_48MHZ            (0x1) /**< ETM clock is 48MHz */

#define ADC_CLOCK_512KHZ           (0x0) /**< ADC clock is 512KHz */
#define ADC_CLOCK_1MHZ             (0x1) /**< ADC clock is 1MHz */
#define ADC_CLOCK_2MHZ             (0x2) /**< ADC clock is 2MHz */
#define ADC_CLOCK_4MHZ             (0x3) /**< ADC clock is 4MHz */
#define ADC_CLOCK_8MHZ             (0x4) /**< ADC clock is 8MHz */
#define ADC_CLOCK_16MHZ            (0x5) /**< ADC clock is 16MHz */
#define ADC_CLOCK_24MHZ            (0x6) /**< ADC clock is 24MHz */

#define WDT_CLOCK_FCLK             (0x0) /**< Watchdog clock is derived from FCLK */
#define WDT_CLOCK_OSCLS            (0x1) /**< Watchdog clock is derived from oscillator LS*/
#define WDT_CLOCK_IRCLS            (0x2) /**< Watchdog clock is derived from IRC LS*/
#define WDT_CLOCK_PLLSRC           (0x3) /**< Watchdog clock is derived from PLL*/

#define RTC2_CLOCK_PLLSRC          (0x0) /**< RTC2 clock is derived from PLL*/
#define RTC2_CLOCK_OSCLS           (0x1) /**< RTC2 clock is derived from oscillator LS*/
#define RTC2_CLOCK_IRCLS           (0x2) /**< RTC2 clock is derived from IRC LS*/
#define RTC2_CLOCK_PLL6X           (0x3) /**< RTC2 clock is derived from PLL multipled by 6*/

#define RTC1_CLOCK_PLLSRC          (0x0) /**< RTC1 clock is derived from PLL */
#define RTC1_CLOCK_OSCLS           (0x1) /**< RTC1 clock is derived from oscillator LS */
#define RTC1_CLOCK_IRCLS           (0x2) /**< RTC1 clock is derived from IRC LS */
#define RTC1_CLOCK_PLL6X           (0x3) /**< RTC1 clock is derived from PLL multipled by 6 */

#define RTC0_CLOCK_PLLSRC          (0x0) /**< RTC1 clock is derived from PLL */
#define RTC0_CLOCK_OSCLS           (0x1) /**< RTC1 clock is derived from oscillator LS */
#define RTC0_CLOCK_IRCLS           (0x2) /**< RTC1 clock is derived from IRC LS */
#define RTC0_CLOCK_PLL6X           (0x3) /**< RTC1 clock is derived from PLL multipled by 6 */

#define UART2_CLOCK_PLL6XBY26      (0x0) /**< UART2 clock is derived from PLL6x divided by 26 */
#define UART2_CLOCK_PLL6X          (0x1) /**< UART2 clock is derived from PLL6x*/

#define UART1_CLOCK_PLL6XBY26      (0x0) /**< UART2 clock is derived from PLL6x divided by 26 */
#define UART1_CLOCK_PLL6X          (0x1) /**< UART2 clock is derived from PLL6x */

#define UART0_CLOCK_PLL6XBY26      (0x0) /**< UART2 clock is derived from PLL6x divided by 26 */
#define UART0_CLOCK_PLL6X          (0x1) /**< UART2 clock is derived from PLL6x */

#define CLOCK_SEL1_ETM_POS         (24)
#define CLOCK_SEL1_ETM_MASK        ((uint32_t)(1 << CLOCK_SEL1_ETM_POS)) /**< ETM bitfield mask */
#define CLOCK_SEL1_ADC_POS         (20)
#define CLOCK_SEL1_ADC_MASK        ((uint32_t)(7 << CLOCK_SEL1_ADC_POS)) /**< ADC bitfield mask */
#define CLOCK_SEL1_WDT_POS         (16)
#define CLOCK_SEL1_WDT_MASK        ((uint32_t)(3 << CLOCK_SEL1_WDT_POS)) /**< Watchdog bitfield mask */

#define CLOCK_SEL1_RTC2_POS        (12)
#define CLOCK_SEL1_RTC2_MASK       ((uint32_t)(3 << CLOCK_SEL1_RTC2_POS)) /**< RTC2 bitfield mask */
#define CLOCK_SEL1_RTC1_POS        (8)
#define CLOCK_SEL1_RTC1_MASK       ((uint32_t)(3 << CLOCK_SEL1_RTC1_POS)) /**< RTC1 bitfield mask */
#define CLOCK_SEL1_RTC0_POS        (4)
#define CLOCK_SEL1_RTC0_MASK       ((uint32_t)(3 << CLOCK_SEL1_RTC0_POS)) /**< RTC0 bitfield mask */

#define CLOCK_SEL1_UART2_POS       (2)
#define CLOCK_SEL1_UART2_MASK      ((uint32_t)(1 << CLOCK_SEL1_UART2_POS)) /**< UART2 bitfield mask */
#define CLOCK_SEL1_UART1_POS       (1)
#define CLOCK_SEL1_UART1_MASK      ((uint32_t)(1 << CLOCK_SEL1_UART1_POS)) /**< UART1 bitfield mask */
#define CLOCK_SEL1_UART0_POS       (0)
#define CLOCK_SEL1_UART0_MASK      ((uint32_t)(1 << CLOCK_SEL1_UART0_POS)) /**< UART0 bitfield mask */

#define IRC8M_SOURCE_STABLE_POS    (5)
#define IRC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << IRC8M_SOURCE_STABLE_POS))  /**< IRC 8MHz stable mask */
#define IRC1K_SOURCE_STABLE_POS    (6)
#define IRC1K_SOURCE_STABLE_MASK   ((uint32_t)(1 << IRC1K_SOURCE_STABLE_POS))  /**< IRC 1KHz stable mask */
#define OSC8M_SOURCE_STABLE_POS    (7)
#define OSC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << OSC8M_SOURCE_STABLE_POS))  /**< OSC 8MHz stable mask */
#define OSC32K_SOURCE_STABLE_POS   (8)
#define OSC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << OSC32K_SOURCE_STABLE_POS)) /**< OSC 32.768KHz stable mask */
#define PLL_SOURCE_STABLE_POS      (9)
#define PLL_SOURCE_STABLE_MASK     ((uint32_t)(1 << PLL_SOURCE_STABLE_POS))    /**< PLL stable mask */

#define PLL_REFERENCE_CLOCK_POS    (8)
#define PLL_REFERENCE_CLOCK_MASK   ((uint32_t)(1 << PLL_REFERENCE_CLOCK_POS))  /**< PLL reference clock mask */

#endif


#if defined (CHIP_GM6601B)
#define CLOCK_PERIPH_ETM_MASK      ((uint32_t)(0x00000001)) /**< Peripheral clock masked for ETM */
#define CLOCK_PERIPH_ADC_MASK      ((uint32_t)(0x00000002)) /**< Peripheral clock masked for ADC */
#define CLOCK_PERIPH_WDT_MASK      ((uint32_t)(0x00000004)) /**< Peripheral clock masked for WDT */
#define CLOCK_PERIPH_RTC2_MASK     ((uint32_t)(0x00000008)) /**< Peripheral clock masked for RTC2 */
#define CLOCK_PERIPH_RTC1_MASK     ((uint32_t)(0x00000010)) /**< Peripheral clock masked for RTC1 */
#define CLOCK_PERIPH_RTC0_MASK     ((uint32_t)(0x00000020)) /**< Peripheral clock masked for RTC0 */
#define CLOCK_PERIPH_UART2_MASK    ((uint32_t)(0x00000040)) /**< Peripheral clock masked for UART2 */
#define CLOCK_PERIPH_UART1_MASK    ((uint32_t)(0x00000080)) /**< Peripheral clock masked for UART1 */
#define CLOCK_PERIPH_UART0_MASK    ((uint32_t)(0x00000100)) /**< Peripheral clock masked for UART0 */

#define ETM_CLOCK_8MHZ             (0x0) /**< ETM clock is 8MHz */
#define ETM_CLOCK_FLCK             (0x1) /**< ETM clock is FCLK */

#define ADC_CLOCK_512KHZ           (0x0) /**< ADC clock is 512KHz */
#define ADC_CLOCK_1MHZ             (0x1) /**< ADC clock is 1MHz */
#define ADC_CLOCK_2MHZ             (0x2) /**< ADC clock is 2MHz */
#define ADC_CLOCK_4MHZ             (0x3) /**< ADC clock is 4MHz */
#define ADC_CLOCK_8MHZ             (0x4) /**< ADC clock is 8MHz */
#define ADC_CLOCK_16MHZ            (0x5) /**< ADC clock is 16MHz */
#define ADC_CLOCK_24MHZ            (0x6) /**< ADC clock is 24MHz */

#define WDT_CLOCK_LIRC             (0x0) /**< Watchdog clock is derived from LIRC */
#define WDT_CLOCK_LOSC             (0x1) /**< Watchdog clock is derived from LOSC */
#define WDT_CLOCK_HIRC             (0x2) /**< Watchdog clock is derived from HIRC */
#define WDT_CLOCK_FLCK             (0x3) /**< Watchdog clock is derived from FCLK */

#define RTC2_CLOCK_PLLSRC          (0x0) /**< RTC2 clock is derived from PLL source clock*/
#define RTC2_CLOCK_LOSC            (0x1) /**< RTC2 clock is derived from LOSC */
#define RTC2_CLOCK_LIRC            (0x2) /**< RTC2 clock is derived from LIRC */
#define RTC2_CLOCK_FCLK            (0x3) /**< RTC2 clock is derived from FCLK */

#define RTC1_CLOCK_PLLSRC          (0x0) /**< RTC1 clock is derived from PLL source clock */
#define RTC1_CLOCK_LOSC            (0x1) /**< RTC1 clock is derived from LOSC */
#define RTC1_CLOCK_LIRC            (0x2) /**< RTC1 clock is derived from LIRC */
#define RTC1_CLOCK_FCLK            (0x3) /**< RTC1 clock is derived from FCLK */

#define RTC0_CLOCK_PLLSRC          (0x0) /**< RTC0 clock is derived from PLL source clock */
#define RTC0_CLOCK_LOSC            (0x1) /**< RTC0 clock is derived from LOSC */
#define RTC0_CLOCK_LIRC            (0x2) /**< RTC0 clock is derived from LIRC */
#define RTC0_CLOCK_FCLK            (0x3) /**< RTC0 clock is derived from FCLK */

#define UART2_CLOCK_PLL6XBY26      (0x0) /**< UART2 clock is derived from PLL6x divided by 26 */
#define UART2_CLOCK_PLL6X          (0x1) /**< UART2 clock is derived from PLL6x*/

#define UART1_CLOCK_PLL6XBY26      (0x0) /**< UART2 clock is derived from PLL6x divided by 26 */
#define UART1_CLOCK_PLL6X          (0x1) /**< UART2 clock is derived from PLL6x */

#define UART0_CLOCK_PLL6XBY26      (0x0) /**< UART2 clock is derived from PLL6x divided by 26 */
#define UART0_CLOCK_PLL6X          (0x1) /**< UART2 clock is derived from PLL6x */

#define CLOCK_SEL1_ETM_POS         (24)
#define CLOCK_SEL1_ETM_MASK        ((uint32_t)(1 << CLOCK_SEL1_ETM_POS)) /**< ETM bitfield mask */
#define CLOCK_SEL1_ADC_POS         (20)
#define CLOCK_SEL1_ADC_MASK        ((uint32_t)(7 << CLOCK_SEL1_ADC_POS)) /**< ADC bitfield mask */
#define CLOCK_SEL1_WDT_POS         (16)
#define CLOCK_SEL1_WDT_MASK        ((uint32_t)(3 << CLOCK_SEL1_WDT_POS)) /**< Watchdog bitfield mask */

#define CLOCK_SEL1_RTC2_POS        (12)
#define CLOCK_SEL1_RTC2_MASK       ((uint32_t)(3 << CLOCK_SEL1_RTC2_POS)) /**< RTC2 bitfield mask */
#define CLOCK_SEL1_RTC1_POS        (8)
#define CLOCK_SEL1_RTC1_MASK       ((uint32_t)(3 << CLOCK_SEL1_RTC1_POS)) /**< RTC1 bitfield mask */
#define CLOCK_SEL1_RTC0_POS        (4)
#define CLOCK_SEL1_RTC0_MASK       ((uint32_t)(3 << CLOCK_SEL1_RTC0_POS)) /**< RTC0 bitfield mask */

#define CLOCK_SEL1_UART2_POS       (2)
#define CLOCK_SEL1_UART2_MASK      ((uint32_t)(1 << CLOCK_SEL1_UART2_POS)) /**< UART2 bitfield mask */
#define CLOCK_SEL1_UART1_POS       (1)
#define CLOCK_SEL1_UART1_MASK      ((uint32_t)(1 << CLOCK_SEL1_UART1_POS)) /**< UART1 bitfield mask */
#define CLOCK_SEL1_UART0_POS       (0)
#define CLOCK_SEL1_UART0_MASK      ((uint32_t)(1 << CLOCK_SEL1_UART0_POS)) /**< UART0 bitfield mask */

#define HIRC8M_SOURCE_STABLE_POS    (24)
#define HIRC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << HIRC8M_SOURCE_STABLE_POS))  /**< IRC 8MHz stable mask */
#define LIRC32K_SOURCE_STABLE_POS   (25)
#define LIRC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << LIRC32K_SOURCE_STABLE_POS))  /**< IRC 1KHz stable mask */
#define HOSC8M_SOURCE_STABLE_POS    (26)
#define HOSC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << HOSC8M_SOURCE_STABLE_POS))  /**< OSC 8MHz stable mask */
#define LOSC32K_SOURCE_STABLE_POS   (27)
#define LOSC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << LOSC32K_SOURCE_STABLE_POS)) /**< OSC 32.768KHz stable mask */
#define PLL_SOURCE_STABLE_POS       (28)
#define PLL_SOURCE_STABLE_MASK      ((uint32_t)(1 << PLL_SOURCE_STABLE_POS))    /**< PLL stable mask */

#define PLL_REFERENCE_CLOCK_POS     (1)
#define PLL_REFERENCE_CLOCK_MASK    ((uint32_t)(1 << PLL_REFERENCE_CLOCK_POS))  /**< PLL reference clock mask */

#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
#define CLOCK_PERIPH_ETM_MASK      ((uint32_t)(0x00000001)) /**< Peripheral clock masked for ETM */
#define CLOCK_PERIPH_ADC_MASK      ((uint32_t)(0x00000002)) /**< Peripheral clock masked for ADC */
#define CLOCK_PERIPH_RTC2_MASK     ((uint32_t)(0x00000004)) /**< Peripheral clock masked for RTC2 */
#define CLOCK_PERIPH_RTC1_MASK     ((uint32_t)(0x00000008)) /**< Peripheral clock masked for RTC1 */
#define CLOCK_PERIPH_RTC0_MASK     ((uint32_t)(0x00000010)) /**< Peripheral clock masked for RTC0 */
#define CLOCK_PERIPH_UART2_MASK    ((uint32_t)(0x00000020)) /**< Peripheral clock masked for UART2 */
#define CLOCK_PERIPH_UART1_MASK    ((uint32_t)(0x00000040)) /**< Peripheral clock masked for UART1 */
#define CLOCK_PERIPH_UART0_MASK    ((uint32_t)(0x00000080)) /**< Peripheral clock masked for UART0 */
#define CLOCK_PERIPH_I2C0_MASK     ((uint32_t)(0x00000100)) /**< Peripheral clock masked for I2C0 */
#define CLOCK_PERIPH_I2C1_MASK     ((uint32_t)(0x00000200)) /**< Peripheral clock masked for I2C1 */
#define CLOCK_PERIPH_I2C2_MASK     ((uint32_t)(0x00000400)) /**< Peripheral clock masked for I2C2 */
#define CLOCK_PERIPH_HIRX_MASK     ((uint32_t)(0x00000800)) /**< Peripheral clock masked for HIRX */
#define CLOCK_PERIPH_LCD_MASK      ((uint32_t)(0x00001000)) /**< Peripheral clock masked for LCD */
#if defined (CHIP_GM6611B)
#define CLOCK_PERIPH_CALENDAR_MASK ((uint32_t)(0x00002000)) /**< Peripheral clock masked for CALENDAR */
#endif
#define RTC2_CLOCK_FCLK            (0x0) /**< RTC2 clock is derived from FLCK*/
#define RTC2_CLOCK_OSCLS           (0x1) /**< RTC2 clock is derived from oscillator LS*/

#define RTC1_CLOCK_FCLK            (0x0) /**< RTC1 clock is derived from FLCK*/
#define RTC1_CLOCK_OSCLS           (0x1) /**< RTC1 clock is derived from oscillator LS*/

#define RTC0_CLOCK_FCLK            (0x0) /**< RTC0 clock is derived from FLCK*/
#define RTC0_CLOCK_OSCLS           (0x1) /**< RTC0 clock is derived from oscillator LS*/

#define HIRX_CLOCK_HIRC            (0x0) /**< HIRX clock is derived from HIRC*/
#define HIRX_CLOCK_HOSC            (0x1) /**< HIRX clock is derived from HOSC*/
#define HIRX_CLOCK_LIRC            (0x2) /**< HIRX clock is derived from LIRC*/
#define HIRX_CLOCK_LOSC            (0x3) /**< HIRX clock is derived from LOSC*/

#define LCD_CLOCK_LIRC             (0x0) /**< LCD clock is derived from LIRC*/
#define LCD_CLOCK_LOSC             (0x1) /**< LCD clock is derived from LOSC*/

#define CLOCK_SEL1_UART0_POS       (0)
#define CLOCK_SEL1_UART0_MASK      ((uint32_t)(0x1F << CLOCK_SEL1_UART0_POS)) /**< UART0 bitfield mask */
#define CLOCK_SEL1_UART1_POS       (5)
#define CLOCK_SEL1_UART1_MASK      ((uint32_t)(0x1F << CLOCK_SEL1_UART1_POS)) /**< UART1 bitfield mask */
#define CLOCK_SEL1_UART2_POS       (10)
#define CLOCK_SEL1_UART2_MASK      ((uint32_t)(0x1F << CLOCK_SEL1_UART2_POS)) /**< UART2 bitfield mask */
#define CLOCK_SEL1_ETM_POS         (15)
#define CLOCK_SEL1_ETM_MASK        ((uint32_t)(7 << CLOCK_SEL1_ETM_POS)) /**< ETM bitfield mask */
#define CLOCK_SEL1_I2C0_POS        (18)
#define CLOCK_SEL1_I2C0_MASK       ((uint32_t)(7 << CLOCK_SEL1_I2C0_POS)) /**< I2C0 bitfield mask */
#define CLOCK_SEL1_I2C1_POS        (21)
#define CLOCK_SEL1_I2C1_MASK       ((uint32_t)(7 << CLOCK_SEL1_I2C1_POS)) /**< I2C1 bitfield mask */
#define CLOCK_SEL1_I2C2_POS        (24)
#define CLOCK_SEL1_I2C2_MASK       ((uint32_t)(7 << CLOCK_SEL1_I2C2_POS)) /**< I2C2 bitfield mask */
#define CLOCK_SEL1_HIRX_POS        (27)
#define CLOCK_SEL1_HIRX_MASK       ((uint32_t)(3 << CLOCK_SEL1_HIRX_POS)) /**< HIRX bitfield mask */
#define CLOCK_SEL1_RTC0_POS        (29)
#define CLOCK_SEL1_RTC0_MASK       ((uint32_t)(1 << CLOCK_SEL1_RTC0_POS)) /**< RTC0 bitfield mask */
#define CLOCK_SEL1_RTC1_POS        (30)
#define CLOCK_SEL1_RTC1_MASK       ((uint32_t)(1 << CLOCK_SEL1_RTC1_POS)) /**< RTC1 bitfield mask */
#define CLOCK_SEL1_RTC2_POS        (31)
#define CLOCK_SEL1_RTC2_MASK       ((uint32_t)(1U << CLOCK_SEL1_RTC2_POS)) /**< RTC2 bitfield mask */

#define CLOCK_SEL2_ADC_POS         (0)
#define CLOCK_SEL2_ADC_MASK        ((uint32_t)(0x7F << CLOCK_SEL2_ADC_POS)) /**< ADC bitfield mask */
#define CLOCK_SEL2_LCD_POS         (22)
#define CLOCK_SEL2_LCD_MASK        ((uint32_t)(3 << CLOCK_SEL2_LCD_POS)) /**< ADC bitfield mask */
#if defined (CHIP_GM6611B)
#define CLOCK_SEL2_CALENDAR_POS    (19)
#define CLOCK_SEL2_CALENDAR_MASK   ((uint32_t)(7 << CLOCK_SEL2_CALENDAR_POS)) /**< RTC Calendar bitfield mask */
#endif
#define HIRC8M_SOURCE_STABLE_POS    (24)
#define HIRC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << HIRC8M_SOURCE_STABLE_POS))  /**< IRC 8MHz stable mask */
#define LIRC32K_SOURCE_STABLE_POS   (25)
#define LIRC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << LIRC32K_SOURCE_STABLE_POS))  /**< IRC 8MHz stable mask */
#define HOSC8M_SOURCE_STABLE_POS    (26)
#define HOSC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << HOSC8M_SOURCE_STABLE_POS))  /**< OSC 8MHz stable mask */
#define LOSC32K_SOURCE_STABLE_POS   (27)
#define LOSC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << LOSC32K_SOURCE_STABLE_POS)) /**< OSC 32.768KHz stable mask */
#define PLL_SOURCE_STABLE_POS       (28)
#define PLL_SOURCE_STABLE_MASK      ((uint32_t)(1 << PLL_SOURCE_STABLE_POS))    /**< PLL stable mask */

#endif


#if defined(CHIP_GM6621)
#define CLOCK_PERIPH_I2C1_MASK      ((uint32_t)(0x00000001)) /**< Peripheral clock masked for I2C1 */
#define CLOCK_PERIPH_I2C0_MASK      ((uint32_t)(0x00000002)) /**< Peripheral clock masked for I2C0 */
#define CLOCK_PERIPH_ETM_MASK       ((uint32_t)(0x00000004)) /**< Peripheral clock masked for ETM*/
#define CLOCK_PERIPH_UART2_MASK     ((uint32_t)(0x00000008)) /**< Peripheral clock masked for UART2 */
#define CLOCK_PERIPH_UART1_MASK     ((uint32_t)(0x00000010)) /**< Peripheral clock masked for UART1 */
#define CLOCK_PERIPH_UART0_MASK     ((uint32_t)(0x00000020)) /**< Peripheral clock masked for UART0 */
#define CLOCK_PERIPH_IRTX_MASK      ((uint32_t)(0x00000040)) /**< Peripheral clock masked for IRTX */
#define CLOCK_PERIPH_WDT_MASK       ((uint32_t)(0x00000080)) /**< Peripheral clock masked for WDT */
#define CLOCK_PERIPH_ADC_MASK       ((uint32_t)(0x00000100)) /**< Peripheral clock masked for ADC */
#define CLOCK_PERIPH_LPSTM_MASK     ((uint32_t)(0x00000200)) /**< Peripheral clock masked for LPSTM */
#define CLOCK_PERIPH_RTC0_MASK      ((uint32_t)(0x00000400)) /**< Peripheral clock masked for RTC0 */

#define CLOCK_SEL1_I2C1_POS        (21)
#define CLOCK_SEL1_I2C1_MASK       ((uint32_t)(7 << CLOCK_SEL1_I2C1_POS)) /**< I2C1 bitfield mask */
#define CLOCK_SEL1_I2C0_POS        (18)
#define CLOCK_SEL1_I2C0_MASK       ((uint32_t)(7 << CLOCK_SEL1_I2C0_POS)) /**< I2C0 bitfield mask */
#define CLOCK_SEL1_ETM_POS         (15)
#define CLOCK_SEL1_ETM_MASK        ((uint32_t)(7 << CLOCK_SEL1_ETM_POS)) /**< ETM bitfield mask */

#define CLOCK_SEL2_ADC_DIV_POS     (0)
#define CLOCK_SEL2_ADC_DIV_MASK    ((uint32_t)(127 << CLOCK_SEL2_ADC_DIV_POS)) /**< ADC Divider bitfield mask */
#define CLOCK_SEL2_ADC_SRC_POS     (19)
#define CLOCK_SEL2_ADC_SRC_MASK    ((uint32_t)(1 << CLOCK_SEL2_ADC_SRC_POS)) /**< ADC Source bitfield mask */
#define CLOCK_SEL2_WDT_POS         (20)
#define CLOCK_SEL2_WDT_MASK        ((uint32_t)(3 << CLOCK_SEL2_WDT_POS)) /**< Watchdog bitfield mask */
#define CLOCK_SEL2_IRTX_SRC_POS    (22)
#define CLOCK_SEL2_IRTX_SRC_MASK   ((uint32_t)(3 << CLOCK_SEL2_IRTX_SRC_POS)) /**< IRTX  Source bitfield mask */
#define CLOCK_SEL2_IRTX_DIV_POS    (24)
#define CLOCK_SEL2_IRTX_DIV_MASK   ((uint32_t)(63 << CLOCK_SEL2_IRTX_DIV_POS)) /**< IRTX  Divider bitfield mask */

#define CLOCK_SEL3_LPSTM_POS       (0)
#define CLOCK_SEL3_LPSTM_MASK      ((uint32_t)(3 << CLOCK_SEL3_LPSTM_POS)) /**< LPSTM bitfield mask */

#define CLOCK_SEL1_RTC_POS         (29)
#define CLOCK_SEL1_RTC_MASK		   ((uint32_t)(7U << CLOCK_SEL1_RTC_POS)) /**< RTC bitfield mask */

#define CLOCK_SEL1_UART2_POS       (10)
#define CLOCK_SEL1_UART2_MASK      ((uint32_t)(31 << CLOCK_SEL1_UART2_POS)) /**< UART2 bitfield mask */
#define CLOCK_SEL1_UART1_POS       (5)
#define CLOCK_SEL1_UART1_MASK      ((uint32_t)(31 << CLOCK_SEL1_UART1_POS)) /**< UART1 bitfield mask */
#define CLOCK_SEL1_UART0_POS       (0)
#define CLOCK_SEL1_UART0_MASK      ((uint32_t)(31 << CLOCK_SEL1_UART0_POS)) /**< UART0 bitfield mask */

#define IRC8M_SOURCE_STABLE_POS    (24)
#define IRC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << IRC8M_SOURCE_STABLE_POS))  /**< IRC 8MHz stable mask */
#define IRC32K_SOURCE_STABLE_POS   (25)
#define IRC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << IRC32K_SOURCE_STABLE_POS))  /**< IRC 1KHz stable mask */
#define OSC8M_SOURCE_STABLE_POS    (26)
#define OSC8M_SOURCE_STABLE_MASK   ((uint32_t)(1 << OSC8M_SOURCE_STABLE_POS))  /**< OSC 8MHz stable mask */
#define OSC32K_SOURCE_STABLE_POS   (27)
#define OSC32K_SOURCE_STABLE_MASK  ((uint32_t)(1 << OSC32K_SOURCE_STABLE_POS)) /**< OSC 32.768KHz stable mask */
#define PLL_SOURCE_STABLE_POS      (28)
#define PLL_SOURCE_STABLE_MASK     ((uint32_t)(1 << PLL_SOURCE_STABLE_POS))    /**< PLL stable mask */

#define PLL_REFERENCE_CLOCK_POS    (8)
#define PLL_REFERENCE_CLOCK_MASK   ((uint32_t)(1 << PLL_REFERENCE_CLOCK_POS))  /**< PLL reference clock mask */

#endif


#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
/**
 * @brief Configure ETM Clock.
 * @param ETM Clock
 * @retval None
 */
#define CLOCK_PERIPH_ETM_CONFIG(ETMCLOCK)     IO_BITMASK_SET(SYSDEV->MCS,CLOCK_SEL1_ETM_MASK,  \
                                                            (ETMCLOCK << CLOCK_SEL1_ETM_POS))
/**
 * @brief Configure ADC Clock.
 * @param ADC Clock
 * @retval None
 */
#define CLOCK_PERIPH_ADC_CONFIG(ADCCLOCK)     IO_BITMASK_SET(SYSDEV->MCS,CLOCK_SEL1_ADC_MASK, \
                                                            (ADCCLOCK << CLOCK_SEL1_ADC_POS))
/**
 * @brief Configure WatchDog Clock.
 * @param WatchDog Clock
 * @retval None
 */
#define CLOCK_PERIPH_WDT_CONFIG(WDTCLOCK)     IO_BITMASK_SET(SYSDEV->MCS,CLOCK_SEL1_WDT_MASK,    \
                                                            (WDTCLOCK << CLOCK_SEL1_WDT_POS))
/**
 * @brief Configure RTC2 Clock.
 * @param RTC2 Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC2_CONFIG(RTC2CLOCK)   IO_BITMASK_SET(SYSDEV->MCS, CLOCK_SEL1_RTC2_MASK,  \
                                                            (RTC2CLOCK << CLOCK_SEL1_RTC2_POS))
/**
 * @brief Configure RTC1 Clock.
 * @param RTC1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC1_CONFIG(RTC1CLOCK)   IO_BITMASK_SET(SYSDEV->MCS, CLOCK_SEL1_RTC1_MASK,  \
                                                            (RTC1CLOCK << CLOCK_SEL1_RTC1_POS))
/**
 * @brief Configure RTC0 Clock.
 * @param RTC0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC0_CONFIG(RTC0CLOCK)   IO_BITMASK_SET(SYSDEV->MCS, CLOCK_SEL1_RTC0_MASK,  \
                                                            (RTC0CLOCK << CLOCK_SEL1_RTC0_POS))
/**
 * @brief Configure UART2 Clock.
 * @param UART2 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART2_CONFIG(UART2CLOCK) IO_BITMASK_SET(SYSDEV->MCS,  CLOCK_SEL1_UART2_MASK, \
                                                            (UART2CLOCK << CLOCK_SEL1_UART2_POS))
/**
 * @brief Configure UART1 Clock.
 * @param UART1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART1_CONFIG(UART1CLOCK) IO_BITMASK_SET(SYSDEV->MCS,  CLOCK_SEL1_UART1_MASK, \
                                                            (UART1CLOCK << CLOCK_SEL1_UART1_POS))
/**
 * @brief Configure UART0 Clock.
 * @param UART0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART0_CONFIG(UART0CLOCK) IO_BITMASK_SET(SYSDEV->MCS,  CLOCK_SEL1_UART0_MASK, \
                                                            (UART0CLOCK << CLOCK_SEL1_UART0_POS))
#endif


#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
/**
 * @brief Configure ETM Clock.
 * @param ETM Clock
 * @retval None
 */
#define CLOCK_PERIPH_ETM_CONFIG(ETMCLOCK)     IO_BITMASK_SET(SYSDEV->CLK1,CLOCK_SEL1_ETM_MASK,  \
                                                            (ETMCLOCK << CLOCK_SEL1_ETM_POS))
/**
 * @brief Configure ADC Clock.
 * @param ADC Clock
 * @retval None
 */
#define CLOCK_PERIPH_ADC_CONFIG(ADCCLOCK)     IO_BITMASK_SET(SYSDEV->CLK2,CLOCK_SEL2_ADC_MASK, \
                                                            (ADCCLOCK << CLOCK_SEL2_ADC_POS))

/**
 * @brief Configure RTC2 Clock.
 * @param RTC2 Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC2_CONFIG(RTC2CLOCK)   IO_BITMASK_SET(SYSDEV->CLK1, CLOCK_SEL1_RTC2_MASK,  \
                                                            ((uint32_t)(RTC2CLOCK << CLOCK_SEL1_RTC2_POS)))
/**
 * @brief Configure RTC1 Clock.
 * @param RTC1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC1_CONFIG(RTC1CLOCK)   IO_BITMASK_SET(SYSDEV->CLK1, CLOCK_SEL1_RTC1_MASK,  \
                                                            (RTC1CLOCK << CLOCK_SEL1_RTC1_POS))
/**
 * @brief Configure RTC0 Clock.
 * @param RTC0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC0_CONFIG(RTC0CLOCK)   IO_BITMASK_SET(SYSDEV->CLK1, CLOCK_SEL1_RTC0_MASK,  \
                                                            (RTC0CLOCK << CLOCK_SEL1_RTC0_POS))
/**
 * @brief Configure UART2 Clock.
 * @param UART2 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART2_CONFIG(UART2CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_UART2_MASK, \
                                                            (UART2CLOCK << CLOCK_SEL1_UART2_POS))
/**
 * @brief Configure UART1 Clock.
 * @param UART1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART1_CONFIG(UART1CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_UART1_MASK, \
                                                            (UART1CLOCK << CLOCK_SEL1_UART1_POS))
/**
 * @brief Configure UART0 Clock.
 * @param UART0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART0_CONFIG(UART0CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_UART0_MASK, \
                                                            (UART0CLOCK << CLOCK_SEL1_UART0_POS))

/**
 * @brief Configure I2C2 Clock.
 * @param I2C2 Clock
 * @retval None
 */
#define CLOCK_PERIPH_I2C2_CONFIG(I2C2CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_I2C2_MASK, \
                                                            (I2C2CLOCK << CLOCK_SEL1_I2C2_POS))

/**
 * @brief Configure I2C1 Clock.
 * @param I2C1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_I2C1_CONFIG(I2C1CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_I2C1_MASK, \
                                                           (I2C1CLOCK << CLOCK_SEL1_I2C1_POS))
/**
 * @brief Configure I2C0 Clock.
 * @param I2C0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_I2C0_CONFIG(I2C0CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_I2C0_MASK, \
                                                           (I2C0CLOCK << CLOCK_SEL1_I2C0_POS))

/**
 * @brief Configure HIRX Clock.
 * @param HIRX Clock
 * @retval None
 */
#define CLOCK_PERIPH_HIRX_CONFIG(HIRXCLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_HIRX_MASK, \
                                                           (HIRXCLOCK << CLOCK_SEL1_HIRX_POS))

#if defined (CHIP_GM6611B)
/**
 * @brief Configure Calendar Clock.
 * @param Calendar Clock
 * @retval None
 */
#define CLOCK_PERIPH_CALENDAR_CONFIG(CALENDARCLOCK) IO_BITMASK_SET(SYSDEV->CLK2,  CLOCK_SEL2_CALENDAR_MASK, \
                                                           (CALENDARCLOCK << CLOCK_SEL2_CALENDAR_POS))
#endif // end of defined (CHIP_GM6611B)

/**
 * @brief Configure LCD Clock.
 * @param LCD Clock
 * @retval None
 */
#define CLOCK_PERIPH_LCD_CONFIG(LCDCLOCK) IO_BITMASK_SET(SYSDEV->CLK2,  CLOCK_SEL2_LCD_MASK, \
                                                         (LCDCLOCK << CLOCK_SEL2_LCD_POS))

#endif
#endif


#if defined(CHIP_GM6621)
/**
 * @brief Configure ETM Clock.
 * @param ETM Clock
 * @retval None
 */
#define CLOCK_PERIPH_ETM_CONFIG(ETMCLOCK)     IO_BITMASK_SET(SYSDEV->CLK1,CLOCK_SEL1_ETM_MASK,  \
                                                            (ETMCLOCK << CLOCK_SEL1_ETM_POS))
/**
 * @brief Configure ADC Source Clock.
 * @param ADC Source Clock
 * @retval None
 */
#define CLOCK_PERIPH_ADC_SRC_CONFIG(ADCSRCCLOCK)     IO_BITMASK_SET(SYSDEV->CLK2,CLOCK_SEL2_ADC_SRC_MASK, \
                                                            (ADCSRCCLOCK << CLOCK_SEL2_ADC_SRC_POS))

/**
 * @brief Configure ADC Divider Clock.
 * @param ADC Source Clock
 * @retval None
 */
#define CLOCK_PERIPH_ADC_DIV_CONFIG(ADCDIVCLOCK)     IO_BITMASK_SET(SYSDEV->CLK2,CLOCK_SEL2_ADC_DIV_MASK, \
                                                            (ADCDIVCLOCK << CLOCK_SEL2_ADC_DIV_POS))

/**
 * @brief Configure UART2 Clock.
 * @param UART2 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART2_CONFIG(UART2CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_UART2_MASK, \
                                                            (UART2CLOCK << CLOCK_SEL1_UART2_POS))
/**
 * @brief Configure UART1 Clock.
 * @param UART1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART1_CONFIG(UART1CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_UART1_MASK, \
                                                            (UART1CLOCK << CLOCK_SEL1_UART1_POS))
/**
 * @brief Configure UART0 Clock.
 * @param UART0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_UART0_CONFIG(UART0CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_UART0_MASK, \
                                                            (UART0CLOCK << CLOCK_SEL1_UART0_POS))

/**
 * @brief Configure I2C1 Clock.
 * @param I2C1 Clock
 * @retval None
 */
#define CLOCK_PERIPH_I2C1_CONFIG(I2C1CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_I2C1_MASK, \
                                                           (I2C1CLOCK << CLOCK_SEL1_I2C1_POS))
/**
 * @brief Configure I2C0 Clock.
 * @param I2C0 Clock
 * @retval None
 */
#define CLOCK_PERIPH_I2C0_CONFIG(I2C0CLOCK) IO_BITMASK_SET(SYSDEV->CLK1,  CLOCK_SEL1_I2C0_MASK, \
                                                           (I2C0CLOCK << CLOCK_SEL1_I2C0_POS))

/**
 * @brief Configure IRTX Source Clock.
 * @param IRTX Source Clock
 * @retval None
 */
#define CLOCK_PERIPH_IRTX_SRC_CONFIG(IRTXSRCCLOCK) IO_BITMASK_SET(SYSDEV->CLK2,  CLOCK_SEL2_IRTX_SRC_MASK, \
                                                           (IRTXSRCCLOCK << CLOCK_SEL2_IRTX_SRC_POS))

/**
 * @brief Configure IRTX Divider Clock.
 * @param IRTX Clock
 * @retval None
 */
#define CLOCK_PERIPH_IRTX_DIV_CONFIG(IRTXDIVCLOCK) IO_BITMASK_SET(SYSDEV->CLK2,  CLOCK_SEL2_IRTX_DIV_MASK, \
                                                           (IRTXDIVCLOCK << CLOCK_SEL2_IRTX_DIV_POS))

/**
 * @brief Configure WatchDog Clock.
 * @param WatchDog Clock
 * @retval None
 */
#define CLOCK_PERIPH_WDT_CONFIG(WDTCLOCK)     IO_BITMASK_SET(SYSDEV->CLK1,CLOCK_SEL2_WDT_MASK,    \
                                                            (WDTCLOCK << CLOCK_SEL2_WDT_POS))

/**
 * @brief Configure LPSTM Clock.
 * @param WatchDog Clock
 * @retval None
 */
#define CLOCK_PERIPH_LPSTM_CONFIG(LPSTMCLOCK)     IO_BITMASK_SET(SYSDEV->CLK1,CLOCK_SEL3_LPSTM_MASK,    \
                                                            (LPSTMCLOCK << CLOCK_SEL3_LPSTM_POS))

/**
 * @brief Configure RTC Clock.
 * @param RTC Clock
 * @retval None
 */
#define CLOCK_PERIPH_RTC0_CONFIG(RTC0CLOCK)       IO_BITMASK_SET(AOPDEV->CSR1,CLOCK_SEL1_RTC_MASK,    \
                                                            (RTC0CLOCK << CLOCK_SEL1_RTC_POS))
#endif


/**
 * Peripheral reset
 */

#if defined(CHIP_GM6601)
typedef enum
{
    GPIO_RESET         = 0x00002000,  /**< GPIO peripheral reset */
    ADC_RESET          = 0x00001000,  /**< ADC peripheral reset */
    ETM_RESET          = 0x00000800,  /**< ETM peripheral reset */
    DMA_RESET          = 0x00000400,  /**< DMA peripheral reset */
    SPI_RESET          = 0x00000200,  /**< SPI peripheral reset */
    CRC_RESET          = 0x00000100,  /**< CRC peripheral reset */
    HWIR_RESET         = 0x00000080,  /**< Hardware IR peripheral reset */
    I2C2_RESET         = 0x00000040,  /**< I2C2 peripheral reset */
    UART2_RESET        = 0x00000020,  /**< UART2 peripheral reset */
    UART1_RESET        = 0x00000010,  /**< UART1 peripheral reset */
    UART0_RESET        = 0x00000008,  /**< UART0 peripheral reset */
    I2C1_RESET         = 0x00000004,  /**< I2C1 peripheral reset */
    I2C0_RESET         = 0x00000002,  /**< I2C0 peripheral reset */
    DBG_RESET          = 0x00000001   /**< Debug module reset */
} CLOCK_ResetPeripheralName_T;
#endif

#if defined(CHIP_GM6601B)
typedef enum
{
    ACMP_RESET         = 0x00002000,  /**< ACMP peripheral reset */
    ADC_RESET          = 0x00001000,  /**< ADC peripheral reset */
    ETM_RESET          = 0x00000800,  /**< ETM peripheral reset */
    DMA_RESET          = 0x00000400,  /**< DMA peripheral reset */
    SPI_RESET          = 0x00000200,  /**< SPI peripheral reset */
    CRC_RESET          = 0x00000100,  /**< CRC peripheral reset */
    HWIR_RESET         = 0x00000080,  /**< Hardware IR peripheral reset */
    GPIO_RESET         = 0x00000040,  /**< GPIO peripheral reset */
    UART2_RESET        = 0x00000020,  /**< UART2 peripheral reset */
    UART1_RESET        = 0x00000010,  /**< UART1 peripheral reset */
    UART0_RESET        = 0x00000008,  /**< UART0 peripheral reset */
    I2C2_RESET         = 0x00000004,  /**< I2C1 peripheral reset */
    I2C1_RESET         = 0x00000002,  /**< I2C1 peripheral reset */
    I2C0_RESET         = 0x00000001,  /**< I2C0 peripheral reset */
} CLOCK_ResetPeripheralName_T;
#endif


#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
typedef enum
{
    CAN_RESET          = 0x00010000,  /**< CAN peripheral reset */
    SPI1_RESET         = 0x00008000,  /**< SPI1 peripheral reset */
    LCD_RESET          = 0x00004000,  /**< LCD peripheral reset */
    ACMP_RESET         = 0x00002000,  /**< ACMP peripheral reset */
    ADC_RESET          = 0x00001000,  /**< ADC peripheral reset */
    ETM_RESET          = 0x00000800,  /**< ETM peripheral reset */
    DMA_RESET          = 0x00000400,  /**< DMA peripheral reset */
    SPI0_RESET         = 0x00000200,  /**< SPI0 peripheral reset */
    CRC_RESET          = 0x00000100,  /**< CRC peripheral reset */
    HWIR_RESET         = 0x00000080,  /**< Hardware IR peripheral reset */
    GPIO_RESET         = 0x00000040,  /**< GPIO peripheral reset */
    UART2_RESET        = 0x00000020,  /**< UART2 peripheral reset */
    UART1_RESET        = 0x00000010,  /**< UART1 peripheral reset */
    UART0_RESET        = 0x00000008,  /**< UART0 peripheral reset */
    I2C2_RESET         = 0x00000004,  /**< I2C2 peripheral reset */
    I2C1_RESET         = 0x00000002,  /**< I2C1 peripheral reset */
    I2C0_RESET         = 0x00000001,  /**< I2C0 peripheral reset */
} CLOCK_ResetPeripheralName_T;
#endif

#if defined(CHIP_GM6621)
typedef enum
{
    CAN_RESET          = 0x00010000,  /**< CAN peripheral reset */
    SPI1_RESET         = 0x00008000,  /**< SPI1 peripheral reset */
    LCD_RESET          = 0x00004000,  /**< LCD peripheral reset */
    ACMP_RESET         = 0x00002000,  /**< ACMP peripheral reset */
    ADC_RESET          = 0x00001000,  /**< ADC peripheral reset */
    ETM_RESET          = 0x00000800,  /**< ETM peripheral reset */
    DMA_RESET          = 0x00000400,  /**< DMA peripheral reset */
    SPI0_RESET         = 0x00000200,  /**< SPI0 peripheral reset */
    CRC_RESET          = 0x00000100,  /**< CRC peripheral reset */
    HWIR_RESET         = 0x00000080,  /**< Hardware IR peripheral reset */
    GPIO_RESET         = 0x00000040,  /**< GPIO peripheral reset */
    UART2_RESET        = 0x00000020,  /**< UART2 peripheral reset */
    UART1_RESET        = 0x00000010,  /**< UART1 peripheral reset */
    UART0_RESET        = 0x00000008,  /**< UART0 peripheral reset */
    I2C2_RESET         = 0x00000004,  /**< I2C2 peripheral reset */
    I2C1_RESET         = 0x00000002,  /**< I2C1 peripheral reset */
    I2C0_RESET         = 0x00000001,  /**< I2C0 peripheral reset */
} CLOCK_ResetPeripheralName_T;
#endif


/**
 * CPU clock source
 */
#if defined(CHIP_GM6601)
typedef enum
{
    SYSCLOCK_SOURCE_CLK8M   = 0x00,  /**< CPU clock is derived from IRC or OSC 8MHz */
    SYSCLOCK_SOURCE_PLL     = 0x01,  /**< CPU clock is derived from PLL clock */
    SYSCLOCK_SOURCE_PLLDIV2 = 0x02,  /**< CPU clock is derived from PLL clock divided by 2 */
    SYSCLOCK_SOURCE_OSCLS   = 0x03   /**< CPU clock is derived from OSC 32.768KHz */
} CLOCK_SysClockSource_T;
#endif

#if defined(CHIP_GM6601B)
typedef enum
{
    SYSCLOCK_SOURCE_HIRC8M   = 0x00,  /**< CPU clock is derived from HIRC 8MHz */
    SYSCLOCK_SOURCE_HOSC8M   = 0x01,  /**< CPU clock is derived from HOSC 8MHz */
    SYSCLOCK_SOURCE_LIRC32K  = 0x02,  /**< CPU clock is derived from LIRC 32KHz */
    SYSCLOCK_SOURCE_LOSC32K  = 0x03,  /**< CPU clock is derived from LOSC 32.768KHz */
    SYSCLOCK_SOURCE_PLL48M   = 0x04   /**< CPU clock is derived from PLL 48MHz */
} CLOCK_SysClockSource_T;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
typedef enum
{
    SYSCLOCK_SOURCE_HIRC8M   = 0x00,  /**< CPU clock is derived from HIRC 8MHz */
    SYSCLOCK_SOURCE_HOSC8M   = 0x01,  /**< CPU clock is derived from HOSC 8MHz */
    SYSCLOCK_SOURCE_LIRC32K  = 0x02,  /**< CPU clock is derived from LIRC 32KHz */
    SYSCLOCK_SOURCE_LOSC32K  = 0x03,  /**< CPU clock is derived from LOSC 32.768KHz */
    SYSCLOCK_SOURCE_PLL48M   = 0x04   /**< CPU clock is derived from PLL 48MHz */
} CLOCK_SysClockSource_T;
#endif

#if defined(CHIP_GM6621)
typedef enum
{
    SYSCLOCK_SOURCE_HIRC8M   = 0x00,  /**< CPU clock is derived from HIRC 8MHz */
    SYSCLOCK_SOURCE_HOSC8M   = 0x01,  /**< CPU clock is derived from HOSC 8MHz */
    SYSCLOCK_SOURCE_LIRC32K  = 0x02,  /**< CPU clock is derived from LIRC 32KHz */
    SYSCLOCK_SOURCE_LOSC32K  = 0x03,  /**< CPU clock is derived from LOSC 32.768KHz */
    SYSCLOCK_SOURCE_PLL48M   = 0x04   /**< CPU clock is derived from PLL 48MHz */
} CLOCK_SysClockSource_T;
#endif

/**
 * CPU clock prescaler
 */

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B))
typedef enum
{
    CLOCK_DIVIDED_1    = 0x00,  /**< CPU clock divided by 1 */
    CLOCK_DIVIDED_2    = 0x01,  /**< CPU clock divided by 2 */
    CLOCK_DIVIDED_4    = 0x02,  /**< CPU clock divided by 4 */
    CLOCK_DIVIDED_8    = 0x03,  /**< CPU clock divided by 8 */
    CLOCK_DIVIDED_16   = 0x04,  /**< CPU clock divided by 16 */
    CLOCK_DIVIDED_32   = 0x05,  /**< CPU clock divided by 32 */
    CLOCK_DIVIDED_64   = 0x06,  /**< CPU clock divided by 64 */
    CLOCK_DIVIDED_128  = 0x07   /**< CPU clock divided by 128 */
} CLOCK_Prescaler_T;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
typedef enum
{
    CLOCK_DIVIDED_1    = 0x00,  /**< CPU clock divided by 1 */
    CLOCK_DIVIDED_2    = 0x01,  /**< CPU clock divided by 2 */
    CLOCK_DIVIDED_4    = 0x03,  /**< CPU clock divided by 4 */
    CLOCK_DIVIDED_8    = 0x07,  /**< CPU clock divided by 8 */
    CLOCK_DIVIDED_16   = 0x0F,  /**< CPU clock divided by 16 */
    CLOCK_DIVIDED_32   = 0x1F,  /**< CPU clock divided by 32 */
    CLOCK_DIVIDED_64   = 0x3F,  /**< CPU clock divided by 64 */
    CLOCK_DIVIDED_128  = 0x7F   /**< CPU clock divided by 128 */
} CLOCK_Prescaler_T;
#endif

#if defined(CHIP_GM6621)
typedef enum
{
    CLOCK_DIVIDED_1    = 0x00,  /**< CPU clock divided by 1 */
    CLOCK_DIVIDED_2    = 0x01,  /**< CPU clock divided by 2 */
    CLOCK_DIVIDED_4    = 0x03,  /**< CPU clock divided by 4 */
    CLOCK_DIVIDED_8    = 0x07,  /**< CPU clock divided by 8 */
    CLOCK_DIVIDED_16   = 0x0F,  /**< CPU clock divided by 16 */
    CLOCK_DIVIDED_32   = 0x1F,  /**< CPU clock divided by 32 */
    CLOCK_DIVIDED_64   = 0x3F,  /**< CPU clock divided by 64 */
    CLOCK_DIVIDED_128  = 0x7F   /**< CPU clock divided by 128 */
} CLOCK_Prescaler_T;
#endif


/**
 * PLL reference clock
 */
typedef enum
{
    PLL_IRC_HSCLK  = 0x00,  /**< PLL reference clock is derived from HIRC 8MHz */
    PLL_OSC_HSCLK  = 0x01,  /**< PLL reference clock is derived from HOSC 8MHz */
#if defined(CHIP_GM6621)
    PLL_OSC_LSCLK = 0x02,    /**< PLL reference clock is derived from LOSC 32KHz */
#endif
} CLOCK_PllRefClk_T;

/**
 * PLL Configuration
 */
typedef struct
{
    CLOCK_PllRefClk_T referenceClock;   /**< PLL reference clock */

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B))
    uint32_t          pllBypassEnable;  /**< PLL bypass enbale */
#endif
} PLL_Config_T;

/**
 * Oscillator Configuration
 */
typedef struct
{
    uint32_t     oscMask;    /**< oscillator mask */
    uint32_t     oscState;   /**< oscillator state */
    PLL_Config_T pllConfig;  /**< PLL configuration */
} OSC_Config_T;

/**
 * Flash latency
 */
typedef enum
{
    FLASH_LATENCY_0  = 0x00, /**<  Wait 0 clock cycle when read(used when CPU clock is less than 25MHz) */
    FLASH_LATENCY_1  = 0x01, /**<  Wait 1 clock cycle when read(used when CPU clock ranges from 25MHz to 36MHz) */
    FLASH_LATENCY_2  = 0x02  /**<  Wait 2 clock cycle when read(used when CPU clock is more than 36MHz) */
} FLASH_LATNECY_T;

/**
 * Clock Configuration
 */
typedef struct
{
    uint32_t               clkMask;          /**< CPU clock mask */
    CLOCK_SysClockSource_T sysClkSource;     /**< System clock source */
    CLOCK_Prescaler_T      sysClkPrescaler;  /**< System clock frequency prescalers */
    FLASH_LATNECY_T        flashLatency;     /**< Flash read latency */
} CLOCK_Config_T;

/**
 * Peripheral Clock Configuration
 */
typedef struct
{
    uint32_t    periphClkMask;  /**< Peripheral clock mask */
    uint32_t    etmClock;       /**< Specify clock for ETM */

#ifndef CHIP_GM6621
    uint32_t    adcClock;       /**< Specify clock for ADC */
#else
    struct ADC_Config
    {
        uint32_t    adcSrcClock;      /**< Specify source clock for ADC */
        uint32_t    adcDivider;       /**< Specify divider for ADC */
    }adc_config_t;
#endif

#if defined(CHIP_GM6621)
    struct IRTX_Config
    {
        uint32_t    irtxSrcClock;      /**< Specify source clock for IRTX */
        uint32_t    irtxDivider;       /**< Specify divider for IRTX */
    }irtx_config_t;
#endif

#if (defined (CHIP_GM6601) || defined(CHIP_GM6601B) || defined (CHIP_GM6621))
    uint32_t    wdtClock;       /**< Specify clock for WatchDog */
#endif

#if (defined (CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined (CHIP_GM6621))
    uint32_t    rtc2Clock;      /**< Specify clock for RTC2 */
    uint32_t    rtc1Clock;      /**< Specify clock for RTC1 */
    uint32_t    rtc0Clock;      /**< Specify clock for RTC0 */
#endif
    uint32_t    uart2Clock;     /**< Specify clock for UART2 */
    uint32_t    uart1Clock;     /**< Specify clock for UART1 */
    uint32_t    uart0Clock;     /**< Specify clock for UART0 */

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    uint32_t    hirxClock;     /**< Specify clock for HIRX */
    uint32_t    i2c0Clock;     /**< Specify clock for I2C0 */
    uint32_t    i2c1Clock;     /**< Specify clock for I2C1 */
    uint32_t    i2c2Clock;     /**< Specify clock for I2C2 */

#if defined (CHIP_GM6611B)
	/* 32k or 8M is better */
    uint32_t    calendarClock; /**< Specify clock for Calendar */
#endif
    uint32_t    lcdClock;      /**< Specify clock for LCD */
#endif

#if defined(CHIP_GM6621)
    uint32_t    i2c0Clock;     /**< Specify clock for I2C0 */
    uint32_t    i2c1Clock;     /**< Specify clock for I2C1 */
    uint32_t    lpstmClock;    /**< Specify clock for LPSTM */
#endif

} PeriphClk_Config_T;

/**
 * @brief Disable individual peripheral's clock.
 *
 * Example code:
 *  Clock_PeripheralClockEnable(SYS_SCG_ETM);
 *
 * @param priClkGate  One peripheral clock gate.
 *
 * @retval            None.
 */
__STATIC_INLINE void Clock_PeripheralClockEnable(uint32_t priClkGate)
{
    SYSDEV->SCG |= priClkGate;
}

/**
 * @brief Enable individual peripheral's clock.
 *
 * Example code:
 *  Clock_PeripheralClockDisable(SYS_SCG_ETM);
 *
 * @param priClkGate   One peripheral clock gate.
 *
 * @retval             None.
 */
__STATIC_INLINE void Clock_PeripheralClockDisable(uint32_t priClkGate)
{
    SYSDEV->SCG &= ~(priClkGate);
}

/**
 * @brief Retrieve individual peripheral's clock status.
 *
 * @param    void.
 *
 * @retval   All peripheral's clock status.
 */
__STATIC_INLINE uint32_t Clock_GetPeripheralClockStatus(void)
{
    return (SYSDEV->SCG);
}


#if (defined (CHIP_GM6601) || defined (CHIP_GM6601B))
/**
 * @brief Enable PLL Bypass for core clock.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_PllBypassEnable(void)
{
    uint32_t tmp = Clock_GetPeripheralClockStatus();

    Clock_PeripheralClockDisable(0xFFFFFFFF);
    SYSDEV->SCS |= SYS_SCS_PLL_BYPASS;
    Clock_PeripheralClockEnable(tmp);
}

/**
 * @brief Disable PLL Bypass for core clock.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_PllBypassDisable(void)
{
    uint32_t tmp = Clock_GetPeripheralClockStatus();

    Clock_PeripheralClockDisable(0xFFFFFFFF);
    SYSDEV->SCS &= ~(SYS_SCS_PLL_BYPASS);
    Clock_PeripheralClockEnable(tmp);
}

#if defined(CHIP_GM6601)
/**
 * @brief Enable Core Switch Module.
 *
 * It must be called to ensure a stable clock switch for CPU core.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_CoreSwithModuleEnable(void)
{
    SYSDEV->SCS |= SYS_SCS_CORE_SWITCH;
}

/**
 * @brief Disable Core Switch Module.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_CoreSwithModuleDisable(void)
{
    SYSDEV->SCS &= ~(SYS_SCS_CORE_SWITCH);
}
#endif

/**
 * @brief Enable Watchdog Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for Watchdog.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_WdtClkSwithModuleEnable(void)
{
    SYSDEV->MCS |= SYS_MCS_WDT_SWITCH;
}

/**
 * @brief Disable Watchdog Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for Watchdog.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_WdtClkSwithModuleDisable(void)
{
    SYSDEV->MCS &= ~(SYS_MCS_WDT_SWITCH);
}

/**
 * @brief Enable RTC2 Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for RTC2.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_Rtc2ClkSwithModuleEnable(void)
{
    SYSDEV->MCS |= SYS_MCS_RTC2_SWITCH;
}

/**
 * @brief Disable RTC2 Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for RTC2.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_Rtc2ClkSwithModuleDisable(void)
{
    SYSDEV->MCS &= ~(SYS_MCS_RTC2_SWITCH);
}

/**
 * @brief Enable RTC1 Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for RTC1.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_Rtc1ClkSwithModuleEnable(void)
{
    SYSDEV->MCS |= SYS_MCS_RTC1_SWITCH;
}

/**
 * @brief Disable RTC1 Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for RTC1.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_Rtc1ClkSwithModuleDisable(void)
{
    SYSDEV->MCS &= ~(SYS_MCS_RTC1_SWITCH);
}

/**
 * @brief Enable RTC0 Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for RTC0.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_Rtc0ClkSwithModuleEnable(void)
{
    SYSDEV->MCS |= SYS_MCS_RTC0_SWITCH;
}

/**
 * @brief Disable RTC0 Clock Switch Module.
 *
 * It must be called to ensure a stable clock switch for RTC0.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE void Clock_Rtc0ClkSwithModuleDisable(void)
{
    SYSDEV->MCS &= ~(SYS_MCS_RTC0_SWITCH);
}
#endif

/**
 * @brief Specify a clock referece for PLL clock.
 *
 * @param refClock   PLL clock reference.
 *
 * @retval           None.
 */

__STATIC_INLINE void Clock_SetPllReferenceClock(CLOCK_PllRefClk_T refClock)
{
    if(refClock == PLL_IRC_HSCLK)
    {
#if defined(CHIP_GM6601)
        SYSDEV->SCS &= ~(SYS_SCS_PLLCLK_REF);
#endif

#if defined(CHIP_GM6601B)
        SYSDEV->PLL &= ~(SYS_PLL_PLLCLK_REF);
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        SYSDEV->PLL &= ~(SYS_PLL_PLLCLK_REF);
#endif

#if defined(CHIP_GM6621)
        SYSDEV->PLL &= ~(SYS_PLL_CLKREF_MASK);
        SYSDEV->PLL |= SYS_PLL_CLKREF_HIRC;
#endif
    }

    if(refClock == PLL_OSC_HSCLK)
    {
#if defined(CHIP_GM6601)
        SYSDEV->SCS |= SYS_SCS_PLLCLK_REF;
#endif

#if defined(CHIP_GM6601B)
        SYSDEV->PLL |= SYS_PLL_PLLCLK_REF;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        SYSDEV->PLL |= SYS_PLL_PLLCLK_REF;
#endif

#if defined(CHIP_GM6621)
        SYSDEV->PLL &= ~(SYS_PLL_CLKREF_MASK);
        SYSDEV->PLL |= SYS_PLL_CLKREF_HOSC;
#endif
    }

#if defined(CHIP_GM6621)
    if (refClock == PLL_OSC_LSCLK)
    {
        SYSDEV->PLL |= SYS_PLL_DIVSEL_LFEN;
        SYSDEV->PLL &= ~(SYS_PLL_CLKREF_MASK);
        SYSDEV->PLL |= SYS_PLL_CLKREF_LOSC;
    }
#endif
}

/**
 * @brief Get PLL reference clock.
 *
 * @param    void.
 *
 * @retval   None.
 */
__STATIC_INLINE CLOCK_PllRefClk_T Clock_GetPllReferenceClock(void)
{
#if defined(CHIP_GM6601)
    return (SYSDEV->SCS & SYS_SCS_PLLCLK_REF) ? PLL_OSC_HSCLK : PLL_IRC_HSCLK;
#endif

#if defined(CHIP_GM6601B)
    return (SYSDEV->PLL & SYS_PLL_PLLCLK_REF) ? PLL_OSC_HSCLK : PLL_IRC_HSCLK;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    return (SYSDEV->PLL & SYS_PLL_PLLCLK_REF) ? PLL_OSC_HSCLK : PLL_IRC_HSCLK;
#endif

#if defined(CHIP_GM6621)
    CLOCK_PllRefClk_T pllRefClk;

    if ((SYSDEV->PLL & SYS_PLL_CLKREF_MASK) == SYS_PLL_CLKREF_HIRC)
    {
        pllRefClk = PLL_IRC_HSCLK;
    }

    if ((SYSDEV->PLL & SYS_PLL_CLKREF_MASK) == SYS_PLL_CLKREF_HOSC)
    {
        pllRefClk = PLL_OSC_HSCLK;
    }

    if ((SYSDEV->PLL & SYS_PLL_CLKREF_MASK) == SYS_PLL_CLKREF_LOSC)
    {
        pllRefClk = PLL_OSC_LSCLK;
    }

    return pllRefClk;
#endif
}


/**
 * @brief Check whether a clock source is stable or not.
 *
 * @param srcClock      Clock source.
 *
 * @retval Clock source stable status.
 */
__STATIC_INLINE uint32_t Clock_IsNowStable(uint32_t srcClock)
{
    return (SYSDEV->SRS & srcClock);
}

/**
 * @brief A short time delay.
 *
 * @param delay   Delay time elapsed.
 *
 * @retval        None.
 */
__STATIC_INLINE void Hold_ResetStatusTillStable(uint8_t delay)
{
    while(delay --);
}

/**
 * @brief Reset individual peripheral.
 *
 * @param pripheral    One peripheral to be reset.
 * @param holdreset    A time delay routine to ensure a stable reset.
 * @param delay        Time delay.
 *
 **@retval             None.
 */
__STATIC_INLINE void Clock_ResetPeripherals(CLOCK_ResetPeripheralName_T pripheral, void (*holdreset)(uint8_t delay), uint8_t delay)
{
    SYSDEV->RST &= ~(pripheral);

    if (holdreset)
    {
        holdreset(delay);
    }

    SYSDEV->RST |= pripheral;
}


/**
 * @brief Enable system clock source.
 *
 * @param srcClock  Clock source.
 *
 * @retval None.
 */
__STATIC_INLINE void SysClock_Enable(CLOCK_SysClockSource_T srcClock)
{
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    /*it's deprecated for GM6611 & GM6601*/
    if (srcClock == SYSCLOCK_SOURCE_HIRC8M)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
    #endif

    #if (defined(CHIP_GM6611B))
        SYSDEV->IRC |= SYS_IRC_CRY8M;
    #endif

    #if (defined(CHIP_GM6621))
        AOPDEV->IRC &= ~(SYS_IRC_CRY8M);
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_HOSC8M)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
        SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
    #endif

    #if (defined(CHIP_GM6611B))
        SYSDEV->OSC |= (SYS_OSC_CRY8M);
        SYSDEV->ANASEL |= (SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
    #endif

    #if (defined(CHIP_GM6621))
        SYSDEV->OSC |= (SYS_OSC_CRY8M);
        SYSDEV->OSC |= (SYS_OSC_OUTSEL);
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_LIRC32K)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->IRC &= ~(SYS_IRC_CRY1K);
    #endif

    #if (defined(CHIP_GM6611B))
        /* Do nothing here, since LIRC is always powered on */
    #endif

    #if (defined(CHIP_GM6621))
        /* Do nothing here, since LIRC is always powered on */
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_LOSC32K)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->OSC &= ~(SYS_OSC_CRY32K);
        SYSDEV->ANASEL |= (SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
    #endif

    #if (defined(CHIP_GM6611B))
        SYSDEV->OSC |= SYS_OSC_CRY32K;
        SYSDEV->ANASEL |= (SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
    #endif

    #if (defined(CHIP_GM6621))
        /*For the case when both High and Low oscillator exist on board*/
        AOPDEV->LSCCTRL |= SYS_AOP_LSCCTRL_OSCPINSEL;

        AOPDEV->LSCCTRL |= SYS_AOP_CRY32K;
        AOPDEV->LSCCTRL |= SYS_OSC_OUTSEL;
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_PLL48M)
    {
    #if (defined(CHIP_GM6601B))
        /*Power on PLL clock*/
        SYSDEV->PLL &= ~(SYS_PLL_PDB);
    #endif

    #if (defined(CHIP_GM6611B))
        /*Power on PLL clock*/
        SYSDEV->PLL |= SYS_PLL_PWRON;
    #endif

    #if (defined(CHIP_GM6621))
        /*Power on PLL clock*/
        SYSDEV->PLL |= SYS_PLL_PWRON;
    #endif
    }
#endif
}

/**
 * @brief Disable system clock source.
 *
 * @param srcClock  Clock source.
 *
 * @retval None.
 */
__STATIC_INLINE void SysClock_Disable(CLOCK_SysClockSource_T srcClock)
{
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    /*it's deprecated for GM6611 & GM6601*/
	if (srcClock == SYSCLOCK_SOURCE_HIRC8M)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->IRC |= SYS_IRC_CRY8M;
    #endif

    #if (defined(CHIP_GM6611B))
        SYSDEV->IRC &= ~(SYS_IRC_CRY8M);
    #endif

    #if (defined(CHIP_GM6621))
        AOPDEV->IRC |= SYS_IRC_CRY8M;
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_HOSC8M)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->OSC |= SYS_OSC_CRY8M;
        SYSDEV->ANASEL &= ~(SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
    #endif

    #if (defined(CHIP_GM6611B))
        SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
        SYSDEV->ANASEL &= ~(SYS_ANASEL_HOSCIN | SYS_ANASEL_HOSCOUT);
    #endif

    #if (defined(CHIP_GM6621))
        SYSDEV->OSC &= ~(SYS_OSC_CRY8M);
        SYSDEV->OSC &= ~(SYS_OSC_OUTSEL);
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_LIRC32K)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->IRC |= SYS_IRC_CRY1K;
    #endif

    #if (defined(CHIP_GM6611B))
        /* Do nothing here, since LIRC is always powered on */
    #endif

    #if (defined(CHIP_GM6621))
        /* Do nothing here, since LIRC is always powered on */
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_LOSC32K)
    {
    #if (defined(CHIP_GM6601B))
        SYSDEV->OSC |= SYS_OSC_CRY32K;
        SYSDEV->ANASEL &= ~(SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
    #endif

    #if (defined(CHIP_GM6611B))
        SYSDEV->OSC &= ~(SYS_OSC_CRY32K);
        SYSDEV->ANASEL &= ~(SYS_ANASEL_LOSCIN | SYS_ANASEL_LOSCOUT);
    #endif

    #if (defined(CHIP_GM6621))
        /*For the case when both High and Low oscillator exist on board*/
        AOPDEV->LSCCTRL |= SYS_AOP_LSCCTRL_OSCPINSEL;

        AOPDEV->LSCCTRL &= ~(SYS_AOP_CRY32K);
        AOPDEV->LSCCTRL &= ~(SYS_OSC_OUTSEL);
    #endif
    }

    if (srcClock == SYSCLOCK_SOURCE_PLL48M)
    {
    #if (defined(CHIP_GM6601B))
        /*Power on PLL clock*/
        SYSDEV->PLL |= SYS_PLL_PDB;
    #endif

    #if (defined(CHIP_GM6611B))
        /*Power on PLL clock*/
        SYSDEV->PLL &= ~SYS_PLL_PWRON;
    #endif

    #if (defined(CHIP_GM6621))
        /*Power on PLL clock*/
        SYSDEV->PLL &= ~SYS_PLL_PWRON;
    #endif
    }
#endif
}

/**
 * @brief Configure oscillator (either external or internal) for CPU clock.
 *
 * By default, CPU works at a frequency of 8MHz derived from IRC 8MHz after resest. Typiccally,
 * for a better performance, CPU clock frequency is expected to be 48MHz. And this depends greatly on clock source,
 * and on that PLL bypass is not enabled and that the system clock precaler ratio is 1 (associated with CLOCK_Config).
 * Once PLL bypass is enabled, PLL clock is derived directly from IRC 8MHz or OSC 8MHz, otherwise,
 * PLL clock is IRC or OSC 8MHz multipled by 6 (48MHz). And PLL bypass is only valid for GM6601 and GM6601B.
 *
 * @param pConfig    A pointer to oscillator configuration structure.
 *
 * @retval RET_OK    Api was successfully executed.
 */
RET_CODE OSC_SetConfig(OSC_Config_T *pConfig);

/**
 * @brief Retrieve configuation of oscillator (either external or internal) for CPU clock.
 *
 * @param pConfig    A pointer to oscillator configuration structure.
 *
 * @retval RET_OK    Api was successfully executed.
 */
RET_CODE OSC_GetConfig(OSC_Config_T *pConfig);

/**
 * @brief Configure CPU clock frequency.
 *
 * By default, CPU works at a frequency of 8MHz derived from IRC 8MHz after resest. Typiccally,
 * for a better performance, CPU clock frequency is expected to be 48MHz. And this depends greatly on clock source,
 * and on that PLL bypass is not enabled and that the system clock precaler ratio is 1 (associated with OSC_Config).
 * Once PLL bypass is enabled, PLL clock is derived directly from IRC 8MHz or OSC 8MHz, otherwise,
 * PLL clock is IRC or OSC 8MHz multipled by 6 (48MHz). And PLL bypass is only valid for GM6601 and GM6601B.
 *
 * @param pConfig     A pointer to clock configuration structure.
 *
 * @retval RET_OK     Api was successfully executed.
 */
RET_CODE CLOCK_SetConfig(CLOCK_Config_T *pConfig);

/**
 * @brief Retrieve configuation of CPU clock frequency.
 *
 * @param pConfig     A pointer to clock configuration structure.
 *
 * @retval RET_OK     Api was successfully executed.
 */
RET_CODE CLOCK_GetConfig(CLOCK_Config_T *pConfig);


/**
 * @brief Configure clock frequency for each individual peripheral.
 *
 * @param pConfig     A pointer to peripheral clock configuration structure.
 *
 * @retval RET_OK     Api was successfully executed.
 */
RET_CODE PeriphClk_SetConfig(PeriphClk_Config_T *pConfig);

/**
 * @brief Retrieve configuation of CPU each individual peripheral.
 *
 * @param pConfig     A pointer to peripheral clock configuration structure.
 *
 * @retval RET_OK     Api was successfully executed.
 */
RET_CODE PeriphClk_GetConfig(PeriphClk_Config_T *pConfig);

#ifdef __cplusplus
}
#endif

#endif /*_CLOCK_H_*/
