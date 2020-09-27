/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_otp_memio.h
 *
 * @author        william.wang
 *
 * @version       1.0.0
 *
 * @date          2018/04/20
 *
 * @brief         Header file of otp memdev driver.
 *
 * @note
 *    2018/04/20    william.wang     V1.0.0
 *        Update For GM6721

 */

#ifndef _GM_LL_OTP_MEMIO_H_
#define _GM_LL_OTP_MEMIO_H_
#include <gm_common.h>
#include<gm_soc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define  WP1_OFFSET						0x00                    /* OTP Write Protection1 Register:offset:0x00*/
#define  WP2_OFFSET						0x04                    /* OTP Write Protection2 Register:offset:0x04*/
#define  RP_OFFSET						0x08              	    /* OTP Read Protection Register:offset:0x08*/
#define  WP_SEL_OFFSET					0x0C		            /* OTP Write Protection Selection Register:offset:0x0C*/
#define  CHIPID1_OFFSET					0x10                    /* OTP Chipid 1 Register:offset:0x10*/
#define  CHIPID1_SIZE					4
#define  TRIM_OFFSET					0X20                    /* OTP Trim Register:offset:0x20*/
#define  TRIM_SIZE						4
#define  SYS_CFG0_OFFSET               	0x30    				/* OTP System Config0 Register:offset:0x30*/
#define  SYS_CFG1MFC_OFFSET				0x34               		/* OTP System Config1 MFC Register:offset:0x34*/
#define  SYS_CFG1CSTM_OFFSET			0x38               		/* OTP System Config1 CSTM Register:offset:0x38*/
#define  SYS_CFG1SEL_OFFSET				0x3C               		/* OTP System Config1 Selection Register:offset:0x3C*/
#define  SYS_CFG2_OFFSET				0x40               		/* OTP System Config2 Register:offset:0x40*/
#define  SYS_CFG3_OFFSET				0x44               		/* OTP System Config3 Register:offset:0x44*/
#define  SYS_CFG4_OFFSET				0x48               		/* OTP System Config4 Register:offset:0x48*/
#define  SYS_CFG5_OFFSET                0X4c  					/* OTP System Config5 Register:offset:0x4C*/
#define  HUK_OFFSET						0X50                    /* OTP HUK Register:offset:0x50*/
#define  HUK_SIZE						8
#define  ROTPK_OFFSET					0x70                 	/* OTP ROTPK Register:offset:0x70*/
#define  ROTPK_SIZE						8
#define  EK_OFFSET						0x90                    /* OTP EK Register:offset:0x90*/
#define  EK_SIZE						8
#define  SSK_OFFSET						0xB0					/* OTP SSK Register:offset:0xB0*/
#define  SSK_SIZE						8                    
#define  FW_VER_OFFSET					0xD0			   		/* OTP FW_VER Register:offset:0xD0*/
#define  FW_VER_SIZE					4
#define  DBG_PW_OFFSET					0xE0				   	/* OTP Debug Port Password Register:offset:0xE0*/
#define  DBG_PW_SIZE					4
#define  CHIPID2_OFFSET					0XF0				   	/* OTP ChipID 2 Register:offset:0xF0*/
#define  CHIPID2_SIZE					4
#define  BOOTROM_OFFSET					0x100                   /* OTP BootRom Register:offset:0x100*/
#define  BOOTROM_SIZE					0x100

/**
 * @defgroup Write Protection1 Register
 * @{
 */
#define OTP_MEMIO_WP0_ZONE0_POS            					((uint32_t) 0)  
#define OTP_MEMIO_WP0_ZONE0_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE0_POS))              /* WP0_ZONE0_DISABLE */
#define OTP_MEMIO_WP0_ZONE2_POS            					((uint32_t) 2)  
#define OTP_MEMIO_WP0_ZONE2_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE2_POS))              /* WP0_ZONE1_DISABLE */
#define OTP_MEMIO_WP0_ZONE3_POS            					((uint32_t) 3)  
#define OTP_MEMIO_WP0_ZONE3_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE3_POS))              /* WP0_ZONE2_DISABLE */
#define OTP_MEMIO_WP0_ZONE4_POS            					((uint32_t) 4)  
#define OTP_MEMIO_WP0_ZONE4_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE4_POS))              /* WP0_ZONE3_DISABLE */
#define OTP_MEMIO_WP0_ZONE5_POS            					((uint32_t) 5)  
#define OTP_MEMIO_WP0_ZONE5_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE5_POS))              /* WP0_ZONE4_DISABLE */
#define OTP_MEMIO_WP0_ZONE6_POS            					((uint32_t) 6)  
#define OTP_MEMIO_WP0_ZONE6_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE6_POS))              /* WP0_ZONE5_DISABLE */
#define OTP_MEMIO_WP0_ZONE7_POS            					((uint32_t) 7)  
#define OTP_MEMIO_WP0_ZONE7_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE7_POS))              /* WP0_ZONE6_DISABLE */
#define OTP_MEMIO_WP0_ZONE8_POS            					((uint32_t) 8)  
#define OTP_MEMIO_WP0_ZONE8_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE8_POS))              /* WP0_ZONE7_DISABLE */
#define OTP_MEMIO_WP0_ZONE9_POS            					((uint32_t) 9)  
#define OTP_MEMIO_WP0_ZONE9_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE9_POS))              /* WP0_ZONE8_DISABLE */
#define OTP_MEMIO_WP0_ZONE10_POS           				 	((uint32_t) 10)  
#define OTP_MEMIO_WP0_ZONE10_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE10_POS))              /* WP0_ZONE9_DISABLE */
#define OTP_MEMIO_WP0_ZONE11_POS           				 	((uint32_t) 11)  
#define OTP_MEMIO_WP0_ZONE11_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE11_POS))              /* WP0_ZONE10_DISABLE */
#define OTP_MEMIO_WP0_ZONE12_POS           				 	((uint32_t) 12)  
#define OTP_MEMIO_WP0_ZONE12_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE12_POS))              /* WP0_ZONE11_DISABLE */
#define OTP_MEMIO_WP0_ZONE13_POS           				 	((uint32_t) 13)  
#define OTP_MEMIO_WP0_ZONE13_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE13_POS))              /* WP0_ZONE12_DISABLE */
#define OTP_MEMIO_WP0_ZONE14_POS           				 	((uint32_t) 14)  
#define OTP_MEMIO_WP0_ZONE14_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE14_POS))              /* WP0_ZONE13_DISABLE */
#define OTP_MEMIO_WP0_ZONE15_POS           				 	((uint32_t) 15)  
#define OTP_MEMIO_WP0_ZONE15_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE15_POS))              /* WP0_ZONE14_DISABLE */
#define OTP_MEMIO_WP0_ZONE16_POS           				 	((uint32_t) 16)  
#define OTP_MEMIO_WP0_ZONE16_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE16_POS))              /* WP0_ZONE15_DISABLE */
#define OTP_MEMIO_WP0_ZONE17_POS           				 	((uint32_t) 17)  
#define OTP_MEMIO_WP0_ZONE17_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE17_POS))              /* WP0_ZONE16_DISABLE */
#define OTP_MEMIO_WP0_ZONE18_POS           				 	((uint32_t) 18)  
#define OTP_MEMIO_WP0_ZONE18_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE18_POS))              /* WP0_ZONE17_DISABLE */
#define OTP_MEMIO_WP0_ZONE19_POS           				 	((uint32_t) 19)  
#define OTP_MEMIO_WP0_ZONE19_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE19_POS))              /* WP0_ZONE18_DISABLE */
#define OTP_MEMIO_WP0_ZONE20_POS           				 	((uint32_t) 20)  
#define OTP_MEMIO_WP0_ZONE20_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE20_POS))              /* WP0_ZONE19_DISABLE */
#define OTP_MEMIO_WP0_ZONE21_POS           				 	((uint32_t) 21)  
#define OTP_MEMIO_WP0_ZONE21_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE21_POS))              /* WP0_ZONE20_DISABLE */
#define OTP_MEMIO_WP0_ZONE22_POS           				 	((uint32_t) 22)  
#define OTP_MEMIO_WP0_ZONE22_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE22_POS))              /* WP0_ZONE21_DISABLE */
#define OTP_MEMIO_WP0_ZONE23_POS           				 	((uint32_t) 23)  
#define OTP_MEMIO_WP0_ZONE23_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE23_POS))              /* WP0_ZONE22_DISABLE */
#define OTP_MEMIO_WP0_ZONE24_POS           				 	((uint32_t) 24)  
#define OTP_MEMIO_WP0_ZONE24_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE24_POS))              /* WP0_ZONE23_DISABLE */
#define OTP_MEMIO_WP0_ZONE25_POS           				 	((uint32_t) 25)  
#define OTP_MEMIO_WP0_ZONE25_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE25_POS))              /* WP0_ZONE24_DISABLE */
#define OTP_MEMIO_WP0_ZONE26_POS           				 	((uint32_t) 26)  
#define OTP_MEMIO_WP0_ZONE26_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE26_POS))              /* WP0_ZONE25_DISABLE */
#define OTP_MEMIO_WP0_ZONE27_POS           				 	((uint32_t) 27)  
#define OTP_MEMIO_WP0_ZONE27_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE27_POS))              /* WP0_ZONE26_DISABLE */
#define OTP_MEMIO_WP0_ZONE28_POS           				 	((uint32_t) 28)  
#define OTP_MEMIO_WP0_ZONE28_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE28_POS))              /* WP0_ZONE27_DISABLE */
#define OTP_MEMIO_WP0_ZONE29_POS           				 	((uint32_t) 29)  
#define OTP_MEMIO_WP0_ZONE29_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE29_POS))              /* WP0_ZONE28_DISABLE */
#define OTP_MEMIO_WP0_ZONE30_POS           				 	((uint32_t) 30)  
#define OTP_MEMIO_WP0_ZONE30_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE30_POS))              /* WP0_ZONE29_DISABLE */
#define OTP_MEMIO_WP0_ZONE31_POS           				 	((uint32_t) 31)  
#define OTP_MEMIO_WP0_ZONE31_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP0_ZONE31_POS))              /* OTP_MEMIO_WP0_ZONE0_POS */

/**
 * @defgroup Write Protection2 Register
 * @{
 */
#define OTP_MEMIO_WP1_ZONE1_POS           					((uint32_t) 1)  
#define OTP_MEMIO_WP1_ZONE1_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE1_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WP1_ZONE8_POS           					((uint32_t) 8)  
#define OTP_MEMIO_WP1_ZONE8_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE8_POS))              /* WP1_ZONE7_DISABLE */
#define OTP_MEMIO_WP1_ZONE10_POS          					((uint32_t) 10)  
#define OTP_MEMIO_WP1_ZONE10_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE10_POS))              /* WP1_ZONE9_DISABLE */
#define OTP_MEMIO_WP1_ZONE11_POS          			 		((uint32_t) 11)  
#define OTP_MEMIO_WP1_ZONE11_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE11_POS))              /* WP1_ZONE10_DISABLE */
#define OTP_MEMIO_WP1_ZONE12_POS          	 				((uint32_t) 12)  
#define OTP_MEMIO_WP1_ZONE12_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE12_POS))              /* WP1_ZONE11_DISABLE */
#define OTP_MEMIO_WP1_ZONE13_POS          	 				((uint32_t) 13)  
#define OTP_MEMIO_WP1_ZONE13_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE13_POS))              /* WP1_ZONE12_DISABLE */
#define OTP_MEMIO_WP1_ZONE14_POS          	 				((uint32_t) 14)  
#define OTP_MEMIO_WP1_ZONE14_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE14_POS))              /* WP1_ZONE13_DISABLE */
#define OTP_MEMIO_WP1_ZONE15_POS          			 		((uint32_t) 15)  
#define OTP_MEMIO_WP1_ZONE15_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE15_POS))              /* WP1_ZONE14_DISABLE */
#define OTP_MEMIO_WP1_ZONE16_POS          	 				((uint32_t) 16)  
#define OTP_MEMIO_WP1_ZONE16_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE16_POS))              /* WP1_ZONE15_DISABLE */
#define OTP_MEMIO_WP1_ZONE17_POS          	 			    ((uint32_t) 17)  
#define OTP_MEMIO_WP1_ZONE17_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE17_POS))              /* WP1_ZONE16_DISABLE */
#define OTP_MEMIO_WP1_ZONE18_POS          	 				((uint32_t) 18)  
#define OTP_MEMIO_WP1_ZONE18_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE18_POS))              /* WP1_ZONE17_DISABLE */
#define OTP_MEMIO_WP1_ZONE19_POS          	 				((uint32_t) 19)  
#define OTP_MEMIO_WP1_ZONE19_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE19_POS))              /* WP1_ZONE18_DISABLE */
#define OTP_MEMIO_WP1_ZONE20_POS          			 		((uint32_t) 20)  
#define OTP_MEMIO_WP1_ZONE20_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE20_POS))              /* WP1_ZONE19_DISABLE */
#define OTP_MEMIO_WP1_ZONE21_POS          	 				((uint32_t) 21)  
#define OTP_MEMIO_WP1_ZONE21_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE21_POS))              /* WP1_ZONE20_DISABLE */
#define OTP_MEMIO_WP1_ZONE24_POS          	 				((uint32_t) 24)  
#define OTP_MEMIO_WP1_ZONE24_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE24_POS))              /* WP1_ZONE23_DISABLE */
#define OTP_MEMIO_WP1_ZONE31_POS          	 				((uint32_t) 31)  
#define OTP_MEMIO_WP1_ZONE31_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WP1_ZONE31_POS))              /* OTP_MEMIO_WP1_ZONE0_POS */


/**
 * @defgroup Read Protection Register
 * @{
 */
#define OTP_MEMIO_RP_ZONE14_POS            					((uint32_t) 14)  
#define OTP_MEMIO_RP_ZONE14_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE14_POS))              /* OTP_MEMIO_RP_ZONE14_DISABLE */
#define OTP_MEMIO_RP_ZONE15_POS            					((uint32_t) 15)  
#define OTP_MEMIO_RP_ZONE15_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE15_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE16_POS            					((uint32_t) 16)  
#define OTP_MEMIO_RP_ZONE16_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE16_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE17_POS            					((uint32_t) 17)  
#define OTP_MEMIO_RP_ZONE17_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE17_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE18_POS            					((uint32_t) 18)  
#define OTP_MEMIO_RP_ZONE18_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE18_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE19_POS            					((uint32_t) 19)  
#define OTP_MEMIO_RP_ZONE19_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE19_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE20_POS            					((uint32_t) 20)  
#define OTP_MEMIO_RP_ZONE20_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE20_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE21_POS            					((uint32_t) 21)  
#define OTP_MEMIO_RP_ZONE21_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE21_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */
#define OTP_MEMIO_RP_ZONE23_POS            					((uint32_t) 23)  
#define OTP_MEMIO_RP_ZONE23_DIS								((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_RP_ZONE23_POS))              /* OTP_MEMIO_RP_ZONE15_DISABLE */


/**
 * @defgroup Write Protection Selection Register
 * @{
 */
#define OTP_MEMIO_WPS_ZONE1_SEL_POS         				((uint32_t) 1)  
#define OTP_MEMIO_WPS_ZONE1_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE1_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE1_SEL_WP0							((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE1_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE1_SEL_WP1							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE1_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE8_SEL_POS         				((uint32_t) 8)  
#define OTP_MEMIO_WPS_ZONE8_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE8_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE8_SEL_WP0							((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE8_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE8_SEL_WP1							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE8_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE10_SEL_POS        				((uint32_t) 10)  
#define OTP_MEMIO_WPS_ZONE10_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE10_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE10_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE10_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE10_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE10_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE11_SEL_POS        				((uint32_t) 11)  
#define OTP_MEMIO_WPS_ZONE11_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE11_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE11_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE11_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE11_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE11_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE12_SEL_POS        				((uint32_t) 12)  
#define OTP_MEMIO_WPS_ZONE12_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE12_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE12_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE12_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE12_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE12_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE13_SEL_POS        				((uint32_t) 13)  
#define OTP_MEMIO_WPS_ZONE13_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE13_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE13_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE13_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE13_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE13_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE14_SEL_POS        				((uint32_t) 14)  
#define OTP_MEMIO_WPS_ZONE14_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE14_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE14_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE14_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE14_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE14_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE15_SEL_POS        				((uint32_t) 15)  
#define OTP_MEMIO_WPS_ZONE15_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE15_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE15_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE15_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE15_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE15_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE16_SEL_POS        				((uint32_t) 16)  
#define OTP_MEMIO_WPS_ZONE16_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE16_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE16_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE16_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE16_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE16_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE17_SEL_POS        				((uint32_t) 17)  
#define OTP_MEMIO_WPS_ZONE17_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE17_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE17_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE17_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE17_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE17_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE18_SEL_POS        				((uint32_t) 18)  
#define OTP_MEMIO_WPS_ZONE18_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE18_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE18_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE18_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE18_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE18_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE19_SEL_POS        				((uint32_t) 19)  
#define OTP_MEMIO_WPS_ZONE19_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE19_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE19_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE19_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE19_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE19_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE20_SEL_POS        				((uint32_t) 20)  
#define OTP_MEMIO_WPS_ZONE20_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE20_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE20_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE20_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE20_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE20_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE21_SEL_POS        				((uint32_t) 21)  
#define OTP_MEMIO_WPS_ZONE21_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE21_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE21_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE21_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE21_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE21_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE24_SEL_POS        				((uint32_t) 24)  
#define OTP_MEMIO_WPS_ZONE24_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE24_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE24_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE24_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE24_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE24_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE31_SEL_POS        				((uint32_t) 31)  
#define OTP_MEMIO_WPS_ZONE31_SEL_MASK						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE31_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE31_SEL_WP0						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_WPS_ZONE31_SEL_POS))              /* WP1_ZONE0_DISABLE */
#define OTP_MEMIO_WPS_ZONE31_SEL_WP1						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_WPS_ZONE31_SEL_POS))              /* WP1_ZONE0_DISABLE */

/**
 * @defgroup OTP Trim0 Register
 * @{
 */
#define OTP_MEMIO_TRIM0_BGNM_TM_POS       					((uint32_t) 0)
#define OTP_MEMIO_TRIM0_BGNM_TM_MASK						((uint32_t) ((uint32_t)0x3F << OTP_MEMIO_TRIM0_BGNM_TM_POS))  
#define OTP_MEMIO_TRIM0_BGLP_TM_POS       					((uint32_t) 6)
#define OTP_MEMIO_TRIM0_BGLP_TM_MASK						((uint32_t) ((uint32_t)0x3F << OTP_MEMIO_TRIM0_BGLP_TM_POS)) 
#define OTP_MEMIO_TRIM0_BGBK_TM_POS       					((uint32_t) 12)
#define OTP_MEMIO_TRIM0_BGBK_TM_MASK						((uint32_t) ((uint32_t)0x3F << OTP_MEMIO_TRIM0_BGBK_TM_POS)) 	
				  
/**
 * @defgroup trim1 Register_bit
 * @{
 */
#define OTP_MEMIO_TRIM1_TSOUT_CODE_POS       				((uint32_t) 0)
#define OTP_MEMIO_TRIM1_TSOUT_CODE_MASK						((uint32_t) ((uint32_t)0xFFF << OTP_MEMIO_TRIM1_TSOUT_CODE_POS))  	

/**
 * @defgroup trim2 Register_bit
 * @{
 */
#define OTP_MEMIO_TRIM2_HIRC_TMR_POS       					((uint32_t) 0)
#define OTP_MEMIO_TRIM2_HIRC_TMR_MASK						((uint32_t) ((uint32_t)0x1F << OTP_MEMIO_TRIM2_HIRC_TMR_POS))  		 
#define OTP_MEMIO_TRIM2_HIRC_TMI_POS       					((uint32_t) 5)
#define OTP_MEMIO_TRIM2_HIRC_TMI_MASK						((uint32_t) ((uint32_t)0x3F << OTP_MEMIO_TRIM2_HIRC_TMI_POS))  	

/**
 * @defgroup CRC(trim) Register_bit
 * @{
 */
#define OTP_MEMIO_TRIM_CRC_VAL0_POS       					((uint32_t) 0)
#define OTP_MEMIO_TRIM_CRC_VAL0_MASK						((uint32_t) ((uint32_t)0xFF << OTP_MEMIO_TRIM_CRC_VAL0_POS))  	
#define OTP_MEMIO_TRIM_CRC_VAL1_POS       					((uint32_t) 8)
#define OTP_MEMIO_TRIM_CRC_VAL1_MASK						((uint32_t) ((uint32_t)0xFF << OTP_MEMIO_TRIM_CRC_VAL1_POS))  
#define OTP_MEMIO_TRIM_CRC_VAL2_POS       					((uint32_t) 16)
#define OTP_MEMIO_TRIM_CRC_VAL2_MASK						((uint32_t) ((uint32_t)0xFF << OTP_MEMIO_TRIM_CRC_VAL2_POS))  

/**
 * @defgroup System Config0 Register
 * @{
 */
#define OTP_MEMIO_SYSCFG0_BOOT_SEL_POS       				((uint32_t) 0)
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_BIT						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_BOOT_SEL_POS))  
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_PIN						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_BOOT_SEL_POS)) 
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_POS       				((uint32_t) 1)
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_MASK					((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG0_BOOT_MODE_POS))  
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_DRAM					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_BOOT_MODE_POS))  
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_SRAM					((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_BOOT_MODE_POS)) 
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_OTP						((uint32_t) ((uint32_t)0x02 << OTP_MEMIO_SYSCFG0_BOOT_MODE_POS)) 
#define OTP_MEMIO_SYSCFG0_BOOT_MODE_SFLASH					((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG0_BOOT_MODE_POS)) 
#define OTP_MEMIO_SYSCFG0_SEC_BOOT_POS       				((uint32_t) 3)
#define OTP_MEMIO_SYSCFG0_SEC_BOOT_EN						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_SEC_BOOT_POS))  
#define OTP_MEMIO_SYSCFG0_SEC_BOOT_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_SEC_BOOT_POS)) 
#define OTP_MEMIO_SYSCFG0_ES_MODE_POS       				((uint32_t) 4)
#define OTP_MEMIO_SYSCFG0_ES_MODE_MASK						((uint32_t) ((uint32_t)0xF << OTP_MEMIO_SYSCFG0_ES_MODE_POS))   
#define OTP_MEMIO_SYSCFG0_ES_MODE_EN						((uint32_t) ((uint32_t)0xF << OTP_MEMIO_SYSCFG0_ES_MODE_POS)) 
#define OTP_MEMIO_SYSCFG0_ES_MODE_DIS						((uint32_t) ((uint32_t)0x0 << OTP_MEMIO_SYSCFG0_ES_MODE_POS))  
#define OTP_MEMIO_SYSCFG0_DBGPW_MODE_POS       				((uint32_t) 8)
#define OTP_MEMIO_SYSCFG0_DBGPW_MODE_BIT					((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_DBGPW_MODE_POS))  
#define OTP_MEMIO_SYSCFG0_DBGPW_MODE_PIN					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_DBGPW_MODE_POS)) 
#define OTP_MEMIO_SYSCFG0_DBG_FORBID_POS       				((uint32_t) 9)
#define OTP_MEMIO_SYSCFG0_DBG_FORBID_EN						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_DBG_FORBID_POS))  
#define OTP_MEMIO_SYSCFG0_DBG_FORBID_DIS					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_DBG_FORBID_POS)) 
#define OTP_MEMIO_SYSCFG0_SF_SCRAM_POS       				((uint32_t) 10)
#define OTP_MEMIO_SYSCFG0_SF_SCRAM_EN						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_SF_SCRAM_POS))  
#define OTP_MEMIO_SYSCFG0_SF_SCRAM_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_SF_SCRAM_POS)) 
#define OTP_MEMIO_SYSCFG0_SDR_SCRAM_POS       				((uint32_t) 11)
#define OTP_MEMIO_SYSCFG0_SDR_SCRAM_EN						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_SDR_SCRAM_POS))  
#define OTP_MEMIO_SYSCFG0_SDR_SCRAM_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_SDR_SCRAM_POS)) 
#define OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_POS      			((uint32_t) 12)
#define OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_MASK				((uint32_t) ((uint32_t)0xF << OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_POS))   
#define OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_EN				((uint32_t) ((uint32_t)0xF << OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_POS)) 
#define OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_DIS				((uint32_t) ((uint32_t)0x0 << OTP_MEMIO_SYSCFG0_SHADOW_REG_PROG_POS)) 
#define OTP_MEMIO_SYSCFG0_INT_SF_POS       					((uint32_t) 16)
#define OTP_MEMIO_SYSCFG0_INT_SF_EN							((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_INT_SF_POS))  
#define OTP_MEMIO_SYSCFG0_INT_SF_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_INT_SF_POS)) 
#define OTP_MEMIO_SYSCFG0_ALL_SF_POS       					((uint32_t) 17)
#define OTP_MEMIO_SYSCFG0_ALL_SF_EN							((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_ALL_SF_POS))  
#define OTP_MEMIO_SYSCFG0_ALL_SF_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_ALL_SF_POS)) 
#define OTP_MEMIO_SYSCFG0_CNN_POS       					((uint32_t) 18)
#define OTP_MEMIO_SYSCFG0_CNN_EN							((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_CNN_POS))  
#define OTP_MEMIO_SYSCFG0_CNN_DIS							((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_CNN_POS)) 
#define OTP_MEMIO_SYSCFG0_DCACHE_BYPASS_POS       			((uint32_t) 19)
#define OTP_MEMIO_SYSCFG0_DCACHE_BYPASS_EN					((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_DCACHE_BYPASS_POS))  
#define OTP_MEMIO_SYSCFG0_DCACHE_BYPASS_DIS					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_DCACHE_BYPASS_POS)) 
#define OTP_MEMIO_SYSCFG0_ICACHE_BYPASS_POS       			((uint32_t) 20)
#define OTP_MEMIO_SYSCFG0_ICACHE_BYPASS_EN					((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_ICACHE_BYPASS_POS))  
#define OTP_MEMIO_SYSCFG0_ICACHE_BYPASS_DIS					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_ICACHE_BYPASS_POS)) 
#define OTP_MEMIO_SYSCFG0_BOOT_FMODE_POS       				((uint32_t) 21)
#define OTP_MEMIO_SYSCFG0_BOOT_FMODE_NAND					((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_BOOT_FMODE_POS))  
#define OTP_MEMIO_SYSCFG0_BOOT_FMODE_NOR					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG0_BOOT_FMODE_POS)) 
#define OTP_MEMIO_SYSCFG0_TRIM_EN_POS       				((uint32_t) 22)
#define OTP_MEMIO_SYSCFG0_TRIM_EN_MASK						((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG0_TRIM_EN_POS))  
#define OTP_MEMIO_SYSCFG0_TRIM_EN							((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG0_TRIM_EN_POS))  
#define OTP_MEMIO_SYSCFG0_TRIM_DIS							((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG0_TRIM_EN_POS)) 
#define OTP_MEMIO_SYSCFG0_BOND_OPTION_POS       			((uint32_t) 24)
#define OTP_MEMIO_SYSCFG0_BOND_OPTION_MASK					((uint32_t) ((uint32_t)0xFF << OTP_MEMIO_SYSCFG0_BOND_OPTION_POS))  


/**
 * @defgroup System Config1 MFC Register
 * @{
 */
#define OTP_MEMIO_SYSCFG1_DBGSEC_MODE_POS       			((uint32_t) 0)
#define OTP_MEMIO_SYSCFG1_DBGSEC_MODE_NON					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG1_DBGSEC_MODE_POS))  
#define OTP_MEMIO_SYSCFG1_DBGSEC_MODE						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG1_DBGSEC_MODE_POS)) 


/**
 * @defgroup Config1 Selection Register
 * @{
 */
#define OTP_MEMIO_SYSCFG1SEL_DBGSEC_MODE_SRC_POS       		((uint32_t) 0)
#define OTP_MEMIO_SYSCFG1SEL_DBGSEC_MODE_SRC_E				((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG1SEL_DBGSEC_MODE_SRC_POS))  
#define OTP_MEMIO_SYSCFG1SEL_DBGSEC_MODE_SRC_D				((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG1SEL_DBGSEC_MODE_SRC_POS)) 

/**
 * @defgroup System Config2 Register
 * @{
 */
#define OTP_MEMIO_SYSCFG2_WDT_TO_THR_POS       				((uint32_t) 0)
#define OTP_MEMIO_SYSCFG2_WDT_TO_THR_MASK					((uint32_t) ((uint32_t)0xFFFFFF << OTP_MEMIO_SYSCFG2_IWDT_TO_THR_POS))  
#define OTP_MEMIO_SYSCFG2_WDT_ENJ_POS       				((uint32_t) 28)
#define OTP_MEMIO_SYSCFG2_WDT_ENJ_EN						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG2_WDT_ENJ_POS))  
#define OTP_MEMIO_SYSCFG2_WDT_ENJ_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG2_WDT_ENJ_POS)) 
                                                    		
/**
 * @defgroup System Config3 Register
 * @{
 */
#define OTP_MEMIO_SYSCFG3_IWDT_TO_THR_POS       			((uint32_t) 0)
#define OTP_MEMIO_SYSCFG3_IWDT_TO_THR_MASK					((uint32_t) ((uint32_t)0xFFF << OTP_MEMIO_SYSCFG3_IWDT_TO_THR_POS))  
#define OTP_MEMIO_SYSCFG3_IWDT_ENJ_POS       				((uint32_t) 12)
#define OTP_MEMIO_SYSCFG3_IWDT_ENJ_EN						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG3_IWDT_ENJ_POS))  
#define OTP_MEMIO_SYSCFG3_IWDT_ENJ_DIS						((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG3_IWDT_ENJ_POS)) 


/**
 * @defgroup System Config4 Register
 * @{
 */
#define OTP_MEMIO_SYSCFG4_CRYP_DBG_POS       				((uint32_t) 0)
#define OTP_MEMIO_SYSCFG4_CRYP_DBG_MASK						((uint32_t) ((uint32_t)0x0F << OTP_MEMIO_SYSCFG4_CRYP_DBG_POS))  
#define OTP_MEMIO_SYSCFG4_CRYP_DBG_EN						((uint32_t) ((uint32_t)0x0F << OTP_MEMIO_SYSCFG4_CRYP_DBG_POS))  
#define OTP_MEMIO_SYSCFG4_CRYP_DBG_DIS						((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG4_CRYP_DBG_POS)) 
#define OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_POS       			((uint32_t) 4)
#define OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_MASK					((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_POS))  
#define OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_NOSIP				((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_POS))  
#define OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_MXICSIP				((uint32_t) ((uint32_t)0x02 << OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_POS)) 
#define OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_WINBSIP				((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_POS)) 
#define OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_ESMTSIP				((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG4_INT_SF_IOSEL_POS)) 
#define OTP_MEMIO_SYSCFG4_INT_SDR_IOENJ_POS       			((uint32_t) 6)
#define OTP_MEMIO_SYSCFG4_INT_SDR_IOENJ_EN					((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG4_INT_SDR_IOENJ_POS))  
#define OTP_MEMIO_SYSCFG4_INT_SDR_IOENJ_DIS					((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG4_INT_SDR_IOENJ_POS)) 
#define OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_POS     			((uint32_t) 7)
#define OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_MASK				((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_POS))  
#define OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_HK3				((uint32_t) ((uint32_t)0x03 << OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_POS))  
#define OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_HK2				((uint32_t) ((uint32_t)0x02 << OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_POS)) 
#define OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_HK1				((uint32_t) ((uint32_t)0x01 << OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_POS)) 
#define OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_HK0				((uint32_t) ((uint32_t)0x00 << OTP_MEMIO_SYSCFG4_SF_SCRAM_RK_SEL_POS))
                                                   	
/* OTP Register:offset 0x8000H~0x8FFFH*/


#ifdef __cplusplus
}
#endif
#endif /* _GM_LL_OTP_MEMIO_H_ */
