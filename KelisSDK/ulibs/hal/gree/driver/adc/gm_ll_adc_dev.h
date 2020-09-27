/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_adc_dev.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		adc source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _GM_LL_ADC_DEV_H_
#define _GM_LL_ADC_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ADC control register layout type definition */
typedef struct
{
    __IO uint32_t CTRL;                       /* ADC control register:offset: 0x0 */
    __IO uint32_t TRIG;                       /* ADC triger register:offset: 0x4 */
    __IO uint32_t ST;                         /* ADC interrupt status register:offset: 0x8 */
    __IO uint32_t COM_CHN;                    /* ADC Auto Compare channel select register:offset: 0xc */
    __IO uint32_t DRST;                       /* ADC convert data result register:offset: 0x10 */
    __IO uint32_t COMPH;                      /* ADC compare highvalue register:offset: 0x14 */
    __IO uint32_t COMPL;                      /* ADC compare lowvalue register:offset: 0x18 */
    __IO uint32_t IE;                         /* ADC interrupt enable register:offset: 0x1c */
    __IO uint32_t DMACTR;                     /* ADC DMA contral register:offset: 0x20 */
    __IO uint32_t OVSAMPCTR;                  /* ADC oversample register:offset: 0x24 */
    __IO uint32_t HWTRIGCTR;                  /* ADC hardware trigger register:offset: 0x28 */
	__IO uint32_t JCHN;                       /* ADC injected channel selection register :offset: 0x2C */
    __IO uint32_t JDRST[4];                   /* ADC injected data result register :offset: 0x30--0x3C */
	__IO uint32_t SAMPTM[4];			      /* ADC injected Sample Time Register :offset: 0x40--0x4C */
	__IO uint32_t RCHN0;			          /* ADC Regular Channel Selection Register :offset: 0x50 */
    __IO uint32_t RCHN1;			          /* ADC Regular Channel Selection Register :offset: 0x54 */
    __IO uint32_t RCHN2;			          /* ADC Regular Channel Selection Register :offset: 0x58 */
}ADC_Device_T;

typedef struct
{
    __IO uint32_t EXCHEN;                     /* ADC external channel register : 0x4000C900 */
    __IO uint32_t ADC_MCTRL;				  /* Multi Mode Controller Register : 0x4000C904 */
    __IO uint32_t RES[7];
    __IO uint32_t VER;                        /* ADC IP version register:offset:0x4000C920 */
}ADC_Common_TypeDef;

#ifdef __cplusplus
}
#endif

#endif /*  #ifndef _GM_LL_ADC_DEV_H_ */
