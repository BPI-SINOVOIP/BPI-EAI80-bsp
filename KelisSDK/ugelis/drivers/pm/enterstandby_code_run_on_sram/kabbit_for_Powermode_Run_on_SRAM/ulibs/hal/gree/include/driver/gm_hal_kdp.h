/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_kdp.h
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       kdp source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_HAL_KDP_H_
#define _GM_HAL_KDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <kdp/gm_ll_kdp_dev.h>
#include <kdp/gm_ll_kdp.h>

typedef enum
{
    SEL_HIGT_16BIT = 0,
    SEL_LOW_16BIT = 1,
} CNN_SEL_CONFIG;

/**
 * @brief  KDP CNN Start.
 *
 * @param void
 * @return void.
 */

void HAL_CNN_Start(void);
/**
 * @brief  Initializes GPIO single pin with user specified attribute.
 *
 * @param pGPIO   GPIO base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */

void HAL_CNN_Wait(void);

/**
 * @brief  Initializes GPIO single pin with user specified attribute.
 *
 * @param pGPIO   GPIO base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */

void HAL_StreamFiFo_init(uint32_t *src, uint32_t len, uint8_t burst_type, uint8_t burst_unlen, uint8_t continue_mode);

/**
 * @brief  Initializes GPIO single pin with user specified attribute.
 *
 * @param pGPIO   GPIO base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */
int HAL_DMA_Send(unsigned int pkt_length, unsigned int base_address);

/**
 * @brief  Initializes GPIO single pin with user specified attribute.
 *
 * @param pGPIO   GPIO base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */
int HAL_IMP_Trans(int src_addr, int src_offset, int drc_addr, int drc_offset, int image_w, int image_h, int rd16_wr32_en, int rd32_wr16_en);

void HAL_CNN_Bram_hfwd_rw_en(char enable);
void HAL_CNN_Bram_HL_sel(CNN_SEL_CONFIG sel_high);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_KDP_H_ */

