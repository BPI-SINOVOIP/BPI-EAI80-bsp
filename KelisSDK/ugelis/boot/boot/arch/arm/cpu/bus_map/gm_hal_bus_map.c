/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gpio.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/07/19
 *
 * @brief        GPIO APIs for application.
 *
 * @note
 *    2016/07/19, mark.lee, V1.0.0
 *        Initial version.
 */

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_hal_cpu.h>
#include <gm_hal_bus_map.h>
#include "gm_ll_bus_map.h"

void RUN_RAM_FUN HAL_SetBusMapping(BUS_Mapping_T typde)
{
    HAL_MAPPINMG_REGION(typde);
}


