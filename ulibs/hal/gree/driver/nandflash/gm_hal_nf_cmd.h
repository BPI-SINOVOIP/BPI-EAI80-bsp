/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_nf.h
 *
 * @author        Colin.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/11/24
 *
 * @brief         Define the command for nand flash 
 *
 * @note
 *    2018/12/5, Colin.Yang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_HAL_NF_CMD_H_
#define _GM_HAL_NF_CMD_H_
	 
#ifdef __cplusplus
	 extern "C" {
#endif

#ifdef TOSHIBA_CHIP
#define  NF_RESET_FLAG           0xA0
#else
#define  NF_RESET_FLAG           0xE0
#endif

// status register [7:0]
#define  SR0_CHIP_STATUS         0x01
#define  SR1_CACHE_PROG_RESULT   0x02
// SR[2] - SR[4] not used 
#define  SR5_READY_BUSY_FOR_CTRL 0x20
#define  SR6_READY_BUSY          0x40
#define  SR7_WRITE_PROTECT       0x80

//Nand Flash Status
#define  NF_BUSY                    0
#define  NF_READY                   1

	 
#define NF_SPEC_BEGIN_OR_END_CMD    0x00
#define NF_RESET_CMD                0xFF
#define NF_ID_READ_CMD              0x90
#define NF_STATUS_RD_CMD	        0x70
#define NF_PAGE_RD_CMD              0x30
#define NF_PAGE_WR_CMD              0x80
#define NF_PAGE_WR_CONFIRM_CMD      0x10






#ifdef __cplusplus
	 }
#endif
#endif /* #ifndef _GM_HAL_NF_CMD_H_ */

