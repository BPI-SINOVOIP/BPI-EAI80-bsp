/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _GM_IWDG_H_
#define _GM_IWDG_H_

#include <stdint.h>
#include <watchdog.h>
#include <gm_hal_iwdg.h>

/*device config*/
struct iwdg_gm_config{
	IWDG_TypeDef *base;
};

/* driver data */
struct iwdg_gm_data{

	/*wwdg peripheral handler*/
	IWDG_HandleTypeDef hiwdg;
};


/* counter reload trigger */
#define GM_IWDG_KR_RELOAD 0xaaaa
/* magic value for unlocking write access to PR and RLR */
#define GM_IWDG_KR_UNLOCK 0x5555
/* watchdog start */
#define GM_IWDG_KR_START  0xcccc
#if 0
/* IWDG_KR */
union __iwdg_kr {
	uint32_t val;
	struct {
		uint16_t key;
		uint16_t rsvd;
	} bit;
};

/* IWDG_PR */
union __iwdg_pr {
	uint32_t val;
	struct {
		uint32_t pr :3 __packed;
		uint32_t rsvd__3_31 :29 __packed;
	} bit;
};

/* IWDG_RLR */
union __iwdg_rlr {
	uint32_t val;
	struct {
		uint32_t rl :12 __packed;
		uint32_t rsvd__12_31 :20 __packed;
	} bit;
};

/* IWDG_SR */
union __iwdg_sr {
	uint32_t val;
	struct {
		uint32_t pvu :1 __packed;
		uint32_t rvu :1 __packed;
		uint32_t rsvd__2_31 :30 __packed;
	} bit;
};

/* IWDG_VER*/
union __iwdg_ver {
	uint32_t val;
	struct {
		uint32_t id__0_31 :32 __packed;
	} bit;
};

/* IWDG register map */
struct iwdg_gm {
	union __iwdg_kr kr;
	union __iwdg_pr pr;
	union __iwdg_rlr rlr;
	union __iwdg_sr sr;
	union __iwdg_ver ver;
};
#endif
#endif	/* _GM_IWDG_H_ */
