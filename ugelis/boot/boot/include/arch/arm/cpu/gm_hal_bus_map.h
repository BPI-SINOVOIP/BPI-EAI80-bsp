 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_bus_map.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/04/09
 *
 * @brief         Header file of dma driver.
 *
 * @note
 *    2018/04/09, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#ifndef _GM_HAL_BUS_MAP_H_
#define _GM_HAL_BUS_MAP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_cpu_dev.h>

typedef enum
{
    MAP_NONE_EREGION                    = 0x0,
    /*
     *icmap==2'b01
     *0x0000_0000 - 0x0FFF_FFFF  map to 0x6000_0000 - 0x6FFF_FFFF
     *0x1000_0000 - 0x1FFF_FFFF  map to 0x7000_0000 - 0x7FFF_FFFF
     */
    MAP_PSRAM_NAND_REGION               = 0x1,
    /*
    * icmap==2'b11
    * 0x0000_0000 - 0x0FFF_FFFF  map to 0xC000_0000 - 0xCFFF_FFFF
    */
    MAP_SDRM_REGION                     = 0x3

} BUS_Mapping_T;


#define RUN_RAM_FUN __used __attribute__((__section__(".code_run_ram*")))
void RUN_RAM_FUN HAL_SetBusMapping(BUS_Mapping_T typde);



#ifdef __cplusplus
}
#endif

#endif /*_GM_HAL_CACHE_H_*/
