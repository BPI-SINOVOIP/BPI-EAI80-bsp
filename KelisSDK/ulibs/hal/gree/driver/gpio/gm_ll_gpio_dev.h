/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_gpio_dev.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		GPIO Source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _GM_LL_GPIO_DEV_H_
#define _GM_LL_GPIO_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	__IO uint32_t ID_VER;        /**< 5.2.18.	GPIOx ID version Register , offset: 0xFFC */
} GPIO_Device_T;

typedef struct {
    __IO uint32_t IO_BPS_REG[6];             /**< IO Debounced bypass control Regsiters: 0x220~0x234 */
    __IO uint32_t IO_OD_REG[6];              /**< IO Pad Open Drain Controller Register : 0x238~0x24C */
    __IO uint32_t IO_IE_REG[6];              /**< IO Input Enable Control Regsiters: 0x250~0x264 */
    __IO uint32_t IO_PU_REG[6];              /**< IO Pull Up Control Regsiters: 0x268~0x27C */
    __IO uint32_t IO_PD_REG[6];              /**< IO Pull DownControl Regsiters: 0x7C~0x90 */
    __IO uint32_t IO_DS_REG[11];             /**< IO Driven Strength Control Regsiters: 0x298~0x2C0*/
    __IO uint32_t IO_MODE_REG[11];           /**< IO Mode Control Regsiters:0x2C4~0x2EC*/
    __IO uint32_t IO_AF_REG[22];             /**< IO Alternate Function Control Regsiters:0x2F0~0x344*/

} SYSIO_Device_T;

#define  SYSIO                               ((SYSIO_Device_T *)(0x40000220u))


typedef struct {
    __IO uint32_t DB_CLK_DIV_CTRL;			/**<  sourced by DB_SRC_CLK divide Register : 0x400000BC */
	__IO uint32_t DB_CLK_SRC_CTRL;			/**<  group IO debounce clock source setting Register : 0x400000C0 */
} SYS_DB_CLK_Device_T;

#define  SYSDBCLKREG                       ((SYS_DB_CLK_Device_T *)(0x400000BCu))


typedef struct {
    __IO uint32_t IP_RST_CTRL[3];
	__IO uint32_t RES;
    __IO uint32_t CLK_EN_CTRL[3];              /**< Clock enable register Register : 0x80~0x88 */
	__IO uint32_t RES1;
	__IO uint32_t CLK_SRC_CTRL[11];            /* System clock control register and  Clock source control register: 0x90~0xB8*/

} SYS_RST_CLK_Device_T;

#define  SYSRSTCLKREG                       ((SYS_RST_CLK_Device_T *)(0x40000070u))

#ifdef __cplusplus
}
#endif

#endif
