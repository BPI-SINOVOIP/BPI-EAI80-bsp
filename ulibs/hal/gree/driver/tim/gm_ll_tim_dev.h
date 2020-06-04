/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          tim_dev.h
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2018/03/19
 *
 * @brief         TIM device header file
 *
 * @note
 *    2018/03/19, edward.zhu, V1.0.0
 *        Initial version.
 *
 */
#ifndef _GM_LL_TIM_DEV_H_
#define _GM_LL_TIM_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/** TIM - Register Layout Typedef */
typedef struct {
    __IO uint32_t CR1;            /**< TIM control register 1, offset: 0x00 */
    __IO uint32_t CR2;            /**< TIM control register 2, offset: 0x04 */
    __IO uint32_t SMCR;           /**< TIM slave mode control register, offset: 0x08 */
    __IO uint32_t DIER;           /**< TIM dma/interrupt enable register, offset: 0x0C */
    __IO uint32_t SR;             /**< TIM status register, offset: 0x10 */
    __IO uint32_t EGR;            /**< TIM event generation register, offset: 0x14 */
    __IO uint32_t CCMR1;          /**< TIM capture/compare mode register 1, offset: 0x18 */
    __IO uint32_t CCMR2;          /**< TIM capture/compare mode register 2, offset: 0x1C */
    __IO uint32_t CCER;           /**< TIM capture/compare enable register: 0x20 */
    __IO uint32_t CNT;            /**< TIM counter register, offset: 0x24 */
    __IO uint32_t PSC;            /**< TIM prescaler register, offset: 0x28 */
    __IO uint32_t ARR;            /**< TIM auto-reload register, offset: 0x2C */
    __IO uint32_t RCR;            /**< TIM repetition counter register, offset: 0x30 */
    __IO uint32_t CCR1;           /**< TIM capture/compare register 1, offset: 0x34 */
    __IO uint32_t CCR2;           /**< TIM capture/compare register 2, offset: 0x38 */
    __IO uint32_t CCR3;           /**< TIM capture/compare register 3, offset: 0x3C */
    __IO uint32_t CCR4;           /**< TIM capture/compare register 4, offset: 0x40 */
    __IO uint32_t BDTR;           /**< TIM break and dead-time register, offset: 0x44 */
    __IO uint32_t DCR;            /**< TIM DMA control register, offset: 0x48 */
    __IO uint32_t DMAR;           /**< TIM DMA address for full transfer register, offset: 0x4C */
    __IO uint32_t DTLR1;          /**< TIM DMA transfer loop register 1, offset: 0x50 */
    __IO uint32_t DTLR2;          /**< TIM DMA transfer loop register 2, offset: 0x54 */
	__IO uint32_t OR;             /**< TIM option register, offset: 0x58 */
	__IO uint32_t CCR5;           /**< TIM compare register, offset: 0x5C */
	__IO uint32_t VER;            /**< TIM version register, offset: 0xFC */
} TIM_Device_T;

#ifdef __cplusplus
}
#endif

#endif /*  #ifndef _GM_LL_TIM_DEV_H_ */

