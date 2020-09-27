/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_kdp.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/06/15
 *
 * @brief		KDP source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _KDP_DEV_H_
#define _KDP_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_kdp_dev.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
  * @brief Sets input port glitch filter
  */
typedef struct
{
	uint8_t kdp_en;
	uint8_t kdp_inten;
	uint8_t bram_hfwd_rw_en;    /**bram_hfwd_rw_en: block ram halfword write/read enable, when this bit is enable, kdp sram will be modify by halfword, only the chosen halfword of 32bit data is effective, the select halfword reference to bram_HL_sel
								0: disable
								1:enable
								bit5
								*/	
    uint8_t bram_HL_sel;        /**
    								bit6
								bram_HL_sel: block ram halfword data select
								0:select high 16bit
								1:select low 16bit
								*/
} KDP_Config_T;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _KDP_DEV_H_ */

