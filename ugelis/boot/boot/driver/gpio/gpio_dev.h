/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gpio_dev.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/07/19 
 *    
 * @brief         GPIO device header file
 * 
 * @note
 *    2016/07/19, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GPIO_DEV_H_
#define _GPIO_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (defined(CHIP_GM6601) || defined(CHIP_GM6621) || defined(CHIP_GM6601B))
#define MAX_GPIO_NUM        4
#elif (defined(CHIP_GM6611))
#define MAX_GPIO_NUM        5
#elif (defined(CHIP_GM6611B))
#define MAX_GPIO_NUM        6
#endif

/* Get base address of GPIO controller register from decimal pin number */
#if (defined(CHIP_GM6601) || defined(CHIP_GM6621) || defined(CHIP_GM6601B))
#define GPIO_CTRL_BASE_FROM_PIN_NUMBER(pin)			 \
			(((pin) < 16) ? (GPIOA_BASE)             \
			: (((pin) < 32) ? (GPIOB_BASE)           \
			: (((pin) < 48) ? (GPIOC_BASE)           \
			: (((pin) < 64) ? (GPIOD_BASE)           \
			: (0)))))
#elif defined(CHIP_GM6611)
#define GPIO_CTRL_BASE_FROM_PIN_NUMBER(pin)			 \
			(((pin) < 16) ? (GPIOA_BASE)             \
			: (((pin) < 32) ? (GPIOB_BASE)           \
			: (((pin) < 48) ? (GPIOC_BASE)           \
			: (((pin) < 64) ? (GPIOD_BASE)           \
            : (((pin) < 80) ? (GPIOE_BASE)           \
			: (0))))))
#elif defined(CHIP_GM6611B)
#define GPIO_CTRL_BASE_FROM_PIN_NUMBER(pin)			 \
			(((pin) < 16) ? (GPIOA_BASE)             \
			: (((pin) < 32) ? (GPIOB_BASE)           \
			: (((pin) < 48) ? (GPIOC_BASE)           \
			: (((pin) < 64) ? (GPIOD_BASE)           \
            : (((pin) < 80) ? (GPIOE_BASE)           \
            : (((pin) < 96) ? (GPIOF_BASE)           \
			: (0)))))))
#endif


#define GPIO_PIN_FROM_PIN_NUMBER(pin)      (((((uint32_t)pin) & 0x3FF) >> 2) << ((((uint32_t)pin) & 0x800)? 8: 0))
#define GPIO_DEVICE_FROM_PIN_NUMBER(pin)   (GPIO_Device_T *)(((uint32_t)pin) & 0xFFFFF000)
			
#define GPIO_BIT_ADDR(pin)        (GPIO_Pin_T)(GPIO_CTRL_BASE_FROM_PIN_NUMBER(pin) + \
                                   ((((pin&15)>>3) + 1)<<10) + ((1<<((pin&15) & 7))<<2))

#define PIN_GET(pin) ((*(pin) == 0)?0:1)
#define PIN_SET(pin) (*(pin) = (((((uint32_t)(pin))<<22)>>24)<<(((((uint32_t)(pin))>>11)&0x1)?8:0)))
#define PIN_CLEAR(pin) (*(pin) = 0)

#if (defined(CHIP_GM6601))
/** GPIO - Register Layout Typedef */
typedef struct {
    __IO uint32_t DR;            /**< Offset: 0x0, read: input sampled at pin, write: to data output */
    __IO uint32_t DOR;           /**< Offset: 0x4, read: input sampled at pin, write: to data output */
    __IO uint32_t OBSR;          /**< GPIO output bit set register, offset: 0x8 */
    __IO uint32_t OBCR;          /**< GPIO output bit clear register, offset: 0xC */
    __IO uint32_t OMCR;          /**< GPIO output mode control registers, offset: 0x10 */
    __IO uint32_t IMCR;          /**< GPIO input mode control registers, offset: 0x14 */
    __IO uint32_t AFSR;          /**< GPIO alternate function set registers, offset: 0x18 */
    __IO uint32_t AFCR;          /**< GPIO alternate function clear registers, offset: 0x1c */
    __IO uint32_t IER;           /**< External interrupt enabled register, offset: 0x20 */
    __IO uint32_t IDR;           /**< External interrupt disabled register, offset: 0x24 */
    __IO uint32_t ITSR;          /**< External interrupt type set register, offset: 0x28 */
    __IO uint32_t ITCR;          /**< External interrupt type clear register, offset: 0x2c */
    __IO uint32_t IPSR;          /**< External interrupt polarity set register, offset: 0x30 */
    __IO uint32_t IPCR;          /**< External interrupt polarity clear register, offset: 0x34 */
    __IO uint32_t ISFR;          /**< External interrupt status flag register, offset: 0x38 */
} GPIO_Device_T;
#elif (defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
/** GPIO - Register Layout Typedef */
typedef struct {
    __IO uint32_t DR;            /**< Offset: 0x0, read: input sampled at pin, write: to data output */
    __IO uint32_t DOR;           /**< Offset: 0x4, read: input sampled at pin, write: ro data output */
    __IO uint32_t OBSR;          /**< GPIO output bit set/reset register, offset: 0x8 */
    __IO uint32_t OBTR;          /**< GPIO output bit toggle register, offset: 0xC */
    __IO uint32_t OMCR;          /**< GPIO output mode control registers, offset: 0x10 */
    __IO uint32_t IMCR;          /**< GPIO input mode control registers, Offset: 0x14 */
    __IO uint32_t AFSR;          /**< GPIO alternate function set registers, offset: 0x18 */
    __IO uint32_t AFCR;          /**< GPIO alternate function clear registers, offset: 0x1c */
    __IO uint32_t IER;           /**< External interrupt enabled register, offset: 0x20 */
    __IO uint32_t IDR;           /**< External interrupt disabled register, offset: 0x24 */
    __IO uint32_t ITSR;          /**< External interrupt type set register, offset: 0x28 */
    __IO uint32_t ITCR;          /**< External interrupt type clear register, offset: 0x2c */
    __IO uint32_t IPSR;          /**< External interrupt polarity set register, offset: 0x30 */
    __IO uint32_t IPCR;          /**< External interrupt polarity clear register, offset: 0x34 */
    __IO uint32_t ISFR;          /**< External interrupt Status flag register, offset: 0x38 */
    __IO uint32_t TRIGEN;        /**< GPIOX trigger ADC enable register, offset: 0x3C */
} GPIO_Device_T;
#elif (defined(CHIP_GM6621))
/** GPIO - Register Layout Typedef */
typedef struct {
    __IO uint32_t DR;            /**< Offset: 0x0, read: input sampled at pin, write: to data output */
    __IO uint32_t DOR;           /**< Offset: 0x4, read: input sampled at pin, write: to data output */
    __IO uint32_t OBSR;          /**< GPIO output bit set/reset register, offset: 0x8 */
    __IO uint32_t OBTR;          /**< GPIO output bit toggle register, offset: 0xC */
    __IO uint32_t OMCR;          /**< GPIO output mode control registers, offset: 0x10 */
    __IO uint32_t IMCR;          /**< GPIO input mode control registers, offset: 0x14 */
    __IO uint32_t RESERVED0[2];  /**< Reserved register: 0x18~0x1c */
    __IO uint32_t IER;           /**< External interrupt enabled register, offset: 0x20 */
    __IO uint32_t IDR;           /**< External interrupt disabled register, offset: 0x24 */
    __IO uint32_t ITSR;          /**< External interrupt type set register, offset: 0x28 */
    __IO uint32_t ITCR;          /**< External interrupt type clear register, offset: 0x2c */
    __IO uint32_t IPSR;          /**< External interrupt polarity set register, offset: 0x30 */
    __IO uint32_t IPCR;          /**< External interrupt polarity clear register, offset: 0x34 */
    __IO uint32_t ISFR;          /**< External interrupt status flag register, offset: 0x38 */
    __IO uint32_t ATE;           /**< ADC trigger enable register, offset: 0x3C */
    __IO uint32_t RESERVED1[752];/**< Reserved register: 0x40~0xBFC */
    __IO uint32_t EAC;           /**< GPIO event action configure register, offset: 0xC00 */
} GPIO_Device_T;
#endif

/** GPIO device pointer */
#define GPIOA            ((GPIO_Device_T *)GPIOA_BASE)
#define GPIOB            ((GPIO_Device_T *)GPIOB_BASE)
#define GPIOC            ((GPIO_Device_T *)GPIOC_BASE)
#define GPIOD            ((GPIO_Device_T *)GPIOD_BASE)


#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define GPIOE            ((GPIO_Device_T *)GPIOE_BASE)
#endif

#if (defined(CHIP_GM6611B))
#define GPIOF            ((GPIO_Device_T *)GPIOF_BASE)
#endif

/*******************************************************************************
 * Global function
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GPIO_DEV_H_ */

