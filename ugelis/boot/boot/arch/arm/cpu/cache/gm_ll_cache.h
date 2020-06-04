 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          dma.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/04
 *
 * @brief         Header file of dma driver.
 *
 * @note
 *    2016/07/04, wizephen.wang, V1.0.0
 *        Initial version.
 *    2017/06/12, evan.wu, V1.0.1
 *        Support 6621.
 */

#ifndef _GM_LL_CACHE_H_
#define _GM_LL_CACHE_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>


typedef struct{
    __IO   uint32_t  CCR      ; //0x00  Configuration and Control Register
    __IO   uint32_t  SR       ; //0x04  Status Register
    __IO   uint32_t  IRQMASK  ; //0x08  Interrupt Request Mask register
    __IO   uint32_t  IRQSTAT  ; //0x0C  Interrupt Request Status register
    __IO   uint32_t  HWPARAMS ; //0x10  Hardware Parameters register
    __IO   uint32_t  CSHR     ; //0x14  Cache Statistic Hit Register
    __IO   uint32_t  CSMR     ; //0x18  Cache Statistic Miss Register
} CMSDK_ICACHE_TypeDef;


typedef struct
{
    __IO   uint32_t  DCCR ;  // Not implemented
    __IO   uint32_t  DCBPR;  // Not implemented
    __IO   uint32_t  DCBFR;
    __IO   uint32_t  DCBIR;
    __IO   uint32_t  DCBWR;  // Not implemented
    __IO   uint32_t  DCBLR;  // Not implemented
    __IO   uint32_t  RESERVED0;  // Not implemented
    __IO   uint32_t  RESERVED1;  // Not implemented
    __IO   uint32_t  CTRL;  // Not implemented
    __IO   uint32_t  RESERVED2;  // Not implemented
    __IO   uint32_t  RESERVED3;  // Not implemented
    __IO   uint32_t  RESERVED4;  // Not implemented
    __IO   uint32_t  RESERVED5;  // Not implemented
    __IO   uint32_t  RESERVED6;  // Not implemented
    __IO   uint32_t  RESERVED7;  // Not implemented
    __IO   uint32_t  CORE_ID;    // Not implemented
} CMSDK_DCACHE_TypeDef;


typedef enum
{
    ICACHE_DISABLE                = 0x0,
    ICACHE_ENABLEING                   ,
    ICACHE_ENABLE                      ,
    ICACHE_DISABLING
} ICACHE_Status_T;


#define CMSDK_DCACHE                            ((CMSDK_DCACHE_TypeDef   *) 0xE0042000)
#define CMSDK_ICACHE                            ((CMSDK_ICACHE_TypeDef   *) (0xE0042000 + 0x100))

//#define DCACHE_16k 1
#ifdef  DCACHE_16k
    #define DCACH_SIZE                          (16*1024)
#else
    #define DCACH_SIZE                          (8*1024)
#endif

#define DACHE_PER_LINE_SIZE                     16
#define DCACHE_LINE_NUM                         (DCACH_SIZE/DACHE_PER_LINE_SIZE)


#define HAL_DACHE_INVALID_DATA_ALL_LINE()       do{                                                       \
                                                    register uint32_t n;                                 \
                                                    for(n=0;n<DCACHE_LINE_NUM;n++) {                     \
                                                        CMSDK_DCACHE->DCBIR = n*DACHE_PER_LINE_SIZE ;    \
                                                    }                                                    \
                                                }while(0)

#define HAL_DACHE_INVALID_DATA_LINE(i)          CMSDK_DCACHE->DCBIR = (i)*DACHE_PER_LINE_SIZE



#define LL_ICACHE_BYPASS_POS                    ((uint32_t) 0)
#define LL_ICACHE_BYPASS_MASK                   ((uint32_t) ((uint32_t)1 << LL_ICACHE_BYPASS_POS))

#define LL_DCACHE_BYPASS_POS                    ((uint32_t) 1)
#define LL_DCACHE_BYPASS_MASK                   ((uint32_t) ((uint32_t)1 << LL_DCACHE_BYPASS_POS))

#define LL_ICACHE_STATUS_POS                    0
#define LL_ICACHE_STATUS_MASK                   ((uint32_t) ((uint32_t)3 << LL_ICACHE_STATUS_POS))

#define ll_ICACHE_EN_POS                        0
#define LL_ICACHE_EN_MASK                       ((uint32_t) ((uint32_t)1 << ll_ICACHE_EN_POS))



#define LL_ICACHE_SET_BYPASS(pDev,val)          IO_BITMASK_SET(pDev->CACHECTR,LL_ICACHE_BYPASS_MASK ,(!val)<<LL_ICACHE_BYPASS_POS)



#define LL_DCACHE_SET_BYPASS(pDev,val)          IO_BITMASK_SET(pDev->CACHECTR,LL_DCACHE_BYPASS_MASK ,(!val)<<LL_DCACHE_BYPASS_POS)

#define LL_ICACHE_ENABLE(pdev,val)              IO_BITMASK_SET(pdev->CCR,LL_ICACHE_EN_MASK,val<<ll_ICACHE_EN_POS)

#define LL_ICACHE_GET_STATUS(pdev)              (pdev->SR)




#ifdef __cplusplus
}
#endif

#endif /*_GM_CACHE_HAL_H_*/
