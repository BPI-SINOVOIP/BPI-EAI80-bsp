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

#ifndef _GM_LL_BUS_MAP_H_
#define _GM_LL_BUS_MAP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_cpu.h>


#define MAPPINMG_REGION_POS             ((uint32_t) 0)
#define MAPPINMG_REGION_MASK            ((uint32_t) ((uint32_t)3 << MAPPINMG_REGION_POS))

#define HAL_MAPPINMG_REGION(val)	    IO_BITMASK_SET(CPU_DEV->MAP, MAPPINMG_REGION_MASK,val<<MAPPINMG_REGION_POS)




#ifdef __cplusplus
}
#endif

#endif /*_GM_LL_BUS_MAP_H_*/
