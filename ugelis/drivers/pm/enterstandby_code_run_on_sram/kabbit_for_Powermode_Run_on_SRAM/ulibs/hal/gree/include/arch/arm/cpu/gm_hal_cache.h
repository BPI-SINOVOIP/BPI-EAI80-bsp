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

#ifndef _GM_HAL_CACHE_H_
#define _GM_HAL_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <gm_common.h>

extern RET_CODE HAL_ICACHE_Enable(void);
extern RET_CODE HAL_ICACHE_Disable(void);
extern RET_CODE HAL_ICACHE_Invalid(void);
extern RET_CODE HAL_ICACHE_Flush(void);
extern RET_CODE HAL_DCACHE_Enable(void);
extern RET_CODE HAL_DCACHE_Disable(void);
extern RET_CODE HAL_DCACHE_Invalid(void);
extern RET_CODE HAL_DCACHE_Flush(void);
extern RET_CODE HAL_DCACHE_InvalidAll(void);

#ifdef __cplusplus
}
#endif

#endif /*_GM_CACHE_HAL_H_*/
