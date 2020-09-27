/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_otp.h
 *
 * @author        william.wang
 *
 * @version       1.0.0
 *
 * @date          2018/04/20
 *
 * @brief         Header file of otp driver.
 *
 * @note
 *    2018/04/20    william.wang     V1.0.0
 *        Update For GM6721

 */

#ifndef _GM_LL_PES_H_
#define _GM_LL_PES_H_
#include <gm_common.h>
#include<gm_soc.h>
#include"gm_ll_otp_dev.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup OTP_Work_Mode_reg_bit
 * @{
 */
#define OTP_WM_MODE_POS                     ((uint32_t) 0)  
#define OTP_WM_MODE_MASK					((uint32_t) ((uint32_t)0x07 << OTP_WM_MODE_POS))              /* OTP Work Mode pos */
#define OTP_WM_MODE_NORMAL                  ((uint32_t) ((uint32_t)0x0 << OTP_WM_MODE_POS))               /* OTP normal mode */  
#define OTP_WM_MODE_IMRDM                  	((uint32_t) ((uint32_t)0x1 << OTP_WM_MODE_POS))               /* OTP initial margin read mode */                          
#define OTP_WM_MODE_PGM                     ((uint32_t) ((uint32_t)0x2 << OTP_WM_MODE_POS))               /* OTP program mode */
#define OTP_WM_MODE_PMRDM                   ((uint32_t) ((uint32_t)0x4 << OTP_WM_MODE_POS))               /* OTP PGM margin read mode */

/**
 * @defgroup Macro Enable Register_bit
 * @{
 */
#define OTP_EN_POS       				 	((uint32_t) 1)
#define OTP_EN_MASK				            ((uint32_t) ((uint32_t)0x01 << OTP_EN_POS)) 				  
#define OTP_EN_ENABLE				        ((uint32_t) ((uint32_t)0x01 << OTP_EN_POS)) 				 
#define OTP_EN_DISABLE			        	((uint32_t) ((uint32_t)0x00 << OTP_EN_POS)) 				 


/**
 * @defgroup Cell Selection Register bit
 * @{
 */ 
#define OTP_CS_PTR_POS       				((uint32_t) 0)
#define OTP_CS_PTR_MASK				        ((uint32_t) ((uint32_t)0x01 << OTP_CS_PTR_POS)) 				  
#define OTP_CS_PTR_CEN				    	((uint32_t) ((uint32_t)0x00 << OTP_CS_PTR_POS)) 				  /* main cell and redundancy cell is enabled for operation */
#define OTP_CS_PTR_TREN			        	((uint32_t) ((uint32_t)0x01 << OTP_CS_PTR_POS)) 				  /* test row is enable for operation */
#define OTP_CS_PAS_POS       				((uint32_t) 1)
#define OTP_CS_PAS_MASK				        ((uint32_t) ((uint32_t)0x01 << OTP_CS_PAS_POS)) 				  
#define OTP_CS_PAS_MCEN				    	((uint32_t) ((uint32_t)0x00 << OTP_CS_PAS_POS)) 				  /* main cell is selected */
#define OTP_CS_PAS_RCEN			        	((uint32_t) ((uint32_t)0x01 << OTP_CS_PAS_POS)) 				  /* redundancy cell is selected*/

/**
 * @defgroup Address Register bit
 * @{
 */ 
#define OTP_ADDR_POS       					((uint32_t) 0)
#define OTP_ADDR_MASK				        ((uint32_t) ((uint32_t)0x01ff << OTP_ADDR_POS)) 				  /* OTP Address Register */

/**
 * @defgroup Control Register bit
 * @{
 */ 
#define OTP_CTRL_PROG_TRIG_POS				((uint32_t) 0)
#define OTP_CTRL_PROG_TRIG_MASK				((uint32_t) ((uint32_t)0x01 << OTP_CTRL_PROG_TRIG_POS)) 
#define OTP_CTRL_PROG_TRIG_ENABLE			((uint32_t) ((uint32_t)0x01 << OTP_CTRL_PROG_TRIG_POS))			  /* Trigger to start program OTP */
#define OTP_CTRL_PROG_TRIG_DISABLE			((uint32_t) ((uint32_t)0x00 << OTP_CTRL_PROG_TRIG_POS))			  /* NOT Trigger to start program OTP */
#define OTP_CTRL_ECC_TOG_POS				((uint32_t) 8)
#define OTP_CTRL_ECC_TOG_MASK				((uint32_t) ((uint32_t)0x01 << OTP_CTRL_ECC_TOG_POS)) 
#define OTP_CTRL_ECC_TOG_ENABLE				((uint32_t) ((uint32_t)0x00 << OTP_CTRL_ECC_TOG_POS))			   /* PWE toggle for ECC program is enabled */
#define OTP_CTRL_ECC_TOG_DISABLE			((uint32_t) ((uint32_t)0x01 << OTP_CTRL_ECC_TOG_POS))			   /* PWE toggle for ECC program is disabled */
#define OTP_CTRL_DB_SET_POS					((uint32_t) 16)
#define OTP_CTRL_DB_SET_MASK				((uint32_t) ((uint32_t)0x01 << OTP_CTRL_DB_SET_POS)) 
#define OTP_CTRL_DB_SET_ENABLE				((uint32_t) ((uint32_t)0x01 << OTP_CTRL_DB_SET_POS))				/*  request to enter deep standby mode*/
#define OTP_CTRL_DB_SET_DISABLE				((uint32_t) ((uint32_t)0x00 << OTP_CTRL_DB_SET_POS))				/* not request to enter deep standby mode*/

/**
 * @defgroup Status Register bit
 * @{
 */ 
#define OTP_STS_PROG_BSY_POS				((uint32_t) 0)
#define OTP_STS_PROG_BSY_BUSY				((uint32_t) ((uint32_t)0x01 << OTP_STS_PROG_BSY_POS))				/*  OTP is busy on programming*/
#define OTP_STS_PROG_PROT_POS				((uint32_t) 1)
#define OTP_STS_PROG_PROT_FORBIDDEN			((uint32_t) ((uint32_t)0x01 << OTP_STS_PROG_PROT_POS))				/*  the space is forbidden to be programmed, for it is written protection*/
#define OTP_STS_ACT_POS						((uint32_t) 8)
#define OTP_STS_ACT							((uint32_t) ((uint32_t)0x01 << OTP_STS_ACT_POS))					/*  OTP is in active status*/
#define OTP_STS_DB_VLD_POS					((uint32_t) 16)
#define OTP_STS_DB_VLD						((uint32_t) ((uint32_t)0x01 << OTP_STS_DB_VLD_POS))					/*  OTP is in deep standby mode*/

/**
 * @defgroup Program Data Register bit
 * @{
 */
#define OTP_DATA_POS       					((uint32_t) 0)
#define OTP_DATA_MASK				        ((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_DATA_POS))

/**
 * @defgroup Timing Part 0 Register bit
 * @{
 */
#define OTP_TIM0_TPPS_SET_POS				((uint32_t) 0)
#define OTP_TIM0_TPPS_SET_MASK				((uint32_t) ((uint32_t)0x00FF << OTP_TIM0_TPPS_SET_POS)) 			/*  TPPS_SET, setting of program mode setup time*/
#define OTP_TIM0_TPPH_SET_POS				((uint32_t) 8)
#define OTP_TIM0_TPPH_SET_MASK				((uint32_t) ((uint32_t)0x00FF << OTP_TIM0_TPPH_SET_POS)) 			/*  TPPH_SET, setting of program mode hold time*/
#define OTP_TIM0_TPPR_SET_POS				((uint32_t) 16)
#define OTP_TIM0_TPPR_SET_MASK				((uint32_t) ((uint32_t)0x03FF << OTP_TIM0_TPPR_SET_POS)) 			/*  TPPR_SET, setting of program mode recovery time*/

/**
 * @defgroup Timing Part 1 Register bit
 * @{
 */
#define OTP_TIM1_TPW_SET_POS				((uint32_t) 0)
#define OTP_TIM1_TPW_SET_MASK				((uint32_t) ((uint32_t)0x00FF << OTP_TIM1_TPW_SET_POS)) 			/*  TPW_SET, setting of program pulse width time*/
#define OTP_TIM1_TPWI_SET_POS				((uint32_t) 8)
#define OTP_TIM1_TPWI_SET_MASK				((uint32_t) ((uint32_t)0x003F << OTP_TIM1_TPWI_SET_POS)) 			/*  TPWI_SET, setting of program pulse interval time*/
#define OTP_TIM1_TCS_SET_POS				((uint32_t) 16)
#define OTP_TIM1_TCS_SET_MASK				((uint32_t) ((uint32_t)0x00FF << OTP_TIM1_TCS_SET_POS)) 			/*  TCS_SET, setting of chip enable */



/**
 * @defgroup Boot Read Status Register bit
 * @{
 */
#define OTP_BRST_00H_STS_POS				((uint32_t) 0)			  
#define OTP_BRST_00H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_00H_STS_POS)) 				/*  the region is successfully to be read out at 00H boot read stage*/
#define OTP_BRST_04H_STS_POS				((uint32_t) 1)			  
#define OTP_BRST_04H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_04H_STS_POS)) 				/*  the region is successfully to be read out at 04H boot read stage*/
#define OTP_BRST_08H_STS_POS				((uint32_t) 2)			  
#define OTP_BRST_08H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_08H_STS_POS))				/*  the region is successfully to be read out at 08H boot read stage*/
#define OTP_BRST_0CH_STS_POS				((uint32_t) 3)			  
#define OTP_BRST_0CH_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_0CH_STS_POS)) 				/*  the region is successfully to be read out at 0CH boot read stage*/
#define OTP_BRST_CHIPID_STS_POS				((uint32_t) 4)			  
#define OTP_BRST_CHIPID_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_CHIPID_STS_POS)) 			/*  the region is successfully to be read out at CHIPID boot read stage*/
#define OTP_BRST_20H_STS_POS				((uint32_t) 5)			  
#define OTP_BRST_20H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_20H_STS_POS)) 				/*  the region is successfully to be read out at 20H boot read stage*/
#define OTP_BRST_24H_STS_POS				((uint32_t) 6)			  
#define OTP_BRST_24H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_24H_STS_POS)) 				/*  the region is successfully to be read out at 24H boot read stage*/
#define OTP_BRST_28H_STS_POS				((uint32_t) 7)			  
#define OTP_BRST_28H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_28H_STS_POS))				/*  the region is successfully to be read out at 28H boot read stage*/
#define OTP_BRST_2CH_STS_POS				((uint32_t) 8)			  
#define OTP_BRST_2CH_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_2CH_STS_POS)) 				/*  the region is successfully to be read out at 2CH boot read stage*/
#define OTP_BRST_30H_STS_POS				((uint32_t) 9)			  
#define OTP_BRST_30H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_30H_STS_POS)) 				/*  the region is successfully to be read out at 30H boot read stage*/
#define OTP_BRST_34H_STS_POS				((uint32_t) 10)			  
#define OTP_BRST_34H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_34H_STS_POS)) 				/*  the region is successfully to be read out at 34H boot read stage*/
#define OTP_BRST_38H_STS_POS				((uint32_t) 11)			  
#define OTP_BRST_38H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_38H_STS_POS)) 				/*  the region is successfully to be read out at 38H boot read stage*/
#define OTP_BRST_3CH_STS_POS				((uint32_t) 12)			  
#define OTP_BRST_3CH_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_3CH_STS_POS)) 				/*  the region is successfully to be read out at 3CH boot read stage*/
#define OTP_BRST_40H_STS_POS				((uint32_t) 13)			  
#define OTP_BRST_40H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_40H_STS_POS)) 				/*  the region is successfully to be read out at 40H boot read stage*/
#define OTP_BRST_44H_STS_POS				((uint32_t) 14)			  
#define OTP_BRST_44H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_44H_STS_POS)) 				/*  the region is successfully to be read out at 44H boot read stage*/
#define OTP_BRST_48H_STS_POS				((uint32_t) 15)			  
#define OTP_BRST_48H_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_48H_STS_POS)) 				/*  the region is successfully to be read out at 48H boot read stage*/
#define OTP_BRST_4CH_STS_POS				((uint32_t) 16)			  
#define OTP_BRST_4CH_STS			        ((uint32_t) ((uint32_t)0x01 << OTP_BRST_4CH_STS_POS)) 				/*  the region is successfully to be read out at 4CH boot read stage*/
#define OTP_BRST_DBG_PW_STS_POS				((uint32_t) 17)			  
#define OTP_BRST_DBG_PW_STS			    	((uint32_t) ((uint32_t)0x01 << OTP_BRST_DBG_PW_STS_POS)) 			/*  the region is successfully to be read out at Debug Port Password read stage*/


/**
 * @defgroup Interrupt Enable Register bit
 * @{
 */
#define OTP_IE_PROG_INT_EN_POS				((uint32_t) 0)			  
#define OTP_IE_PROG_INT_EN_MASK			    ((uint32_t) ((uint32_t)0x01 << OTP_IE_PROG_INT_EN_POS)) 
#define OTP_IE_PROG_INT_EN			    	((uint32_t) ((uint32_t)0x01 << OTP_IE_PROG_INT_EN_POS)) 			/*  OTP program interrupt is not masked*/			

/**
 * @defgroup RAW Interrupt Enable Register bit
 * @{
 */
#define OTP_RAW_PROG_INT_STS_POS			((uint32_t) 0)			  
#define OTP_RAW_PROG_INT_STS			    ((uint32_t) ((uint32_t)0x01 << OTP_RAW_PROG_INT_STS_POS)) 

/**
 * @defgroup Interrupt Status Register bit
 * @{
 */
#define OTP_PROG_INT_STS_POS				((uint32_t) 0)			  
#define OTP_PROG_INT_STS			    	((uint32_t) ((uint32_t)0x01 << OTP_PROG_INT_STS_POS)) 

#define OTP_BR_00H_POS			    		((uint32_t) 0) 
#define OTP_BR_00H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_00H_POS)) 
#define OTP_BR_04H_POS			    		((uint32_t) 0) 
#define OTP_BR_04H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_04H_POS)) 
#define OTP_BR_08H_POS			    		((uint32_t) 0) 
#define OTP_BR_08H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_08H_POS)) 
#define OTP_BR_0CH_POS			    		((uint32_t) 0) 
#define OTP_BR_0CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_0CH_POS)) 
#define OTP_BR_10H_POS			    		((uint32_t) 0) 
#define OTP_BR_10H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_10H_POS)) 
#define OTP_BR_14H_POS			    		((uint32_t) 0) 
#define OTP_BR_14H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_14H_POS)) 
#define OTP_BR_18H_POS			    		((uint32_t) 0) 
#define OTP_BR_18H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_18H_POS)) 
#define OTP_BR_1CH_POS			    		((uint32_t) 0) 
#define OTP_BR_1CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_1CH_POS)) 
#define OTP_BR_20H_POS			    		((uint32_t) 0) 
#define OTP_BR_20H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_20H_POS)) 
#define OTP_BR_24H_POS			    		((uint32_t) 0) 
#define OTP_BR_24H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_24H_POS)) 
#define OTP_BR_28H_POS			    		((uint32_t) 0) 
#define OTP_BR_28H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_28H_POS)) 
#define OTP_BR_2CH_POS			    		((uint32_t) 0) 
#define OTP_BR_2CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_2CH_POS)) 
#define OTP_BR_30H_POS			    		((uint32_t) 0) 
#define OTP_BR_30H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_30H_POS)) 
#define OTP_BR_34H_POS			    		((uint32_t) 0) 
#define OTP_BR_34H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_34H_POS)) 
#define OTP_BR_38H_POS			    		((uint32_t) 0) 
#define OTP_BR_38H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_38H_POS)) 
#define OTP_BR_3CH_POS			    		((uint32_t) 0) 
#define OTP_BR_3CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_3CH_POS)) 
#define OTP_BR_40H_POS			    		((uint32_t) 0) 
#define OTP_BR_40H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_40H_POS)) 
#define OTP_BR_44H_POS			    		((uint32_t) 0) 
#define OTP_BR_44H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_44H_POS)) 
#define OTP_BR_48H_POS			    		((uint32_t) 0) 
#define OTP_BR_48H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_48H_POS)) 
#define OTP_BR_4CH_POS			    		((uint32_t) 0) 
#define OTP_BR_4CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_4CH_POS)) 


#define OTP_BR_00H_POS			    		((uint32_t) 0) 
#define OTP_BR_00H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_00H_POS)) 
#define OTP_BR_04H_POS			    		((uint32_t) 0) 
#define OTP_BR_04H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_04H_POS)) 
#define OTP_BR_08H_POS			    		((uint32_t) 0) 
#define OTP_BR_08H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_08H_POS)) 
#define OTP_BR_0CH_POS			    		((uint32_t) 0) 
#define OTP_BR_0CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_0CH_POS)) 
#define OTP_BR_10H_POS			    		((uint32_t) 0) 
#define OTP_BR_10H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_10H_POS)) 
#define OTP_BR_14H_POS			    		((uint32_t) 0) 
#define OTP_BR_14H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_14H_POS)) 
#define OTP_BR_18H_POS			    		((uint32_t) 0) 
#define OTP_BR_18H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_18H_POS)) 
#define OTP_BR_1CH_POS			    		((uint32_t) 0) 
#define OTP_BR_1CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_1CH_POS)) 
#define OTP_BR_20H_POS			    		((uint32_t) 0) 
#define OTP_BR_20H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_20H_POS)) 
#define OTP_BR_24H_POS			    		((uint32_t) 0) 
#define OTP_BR_24H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_24H_POS)) 
#define OTP_BR_28H_POS			    		((uint32_t) 0) 
#define OTP_BR_28H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_28H_POS)) 
#define OTP_BR_2CH_POS			    		((uint32_t) 0) 
#define OTP_BR_2CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_2CH_POS)) 
#define OTP_BR_30H_POS			    		((uint32_t) 0) 
#define OTP_BR_30H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_30H_POS)) 
#define OTP_BR_34H_POS			    		((uint32_t) 0) 
#define OTP_BR_34H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_34H_POS)) 
#define OTP_BR_38H_POS			    		((uint32_t) 0) 
#define OTP_BR_38H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_38H_POS)) 
#define OTP_BR_3CH_POS			    		((uint32_t) 0) 
#define OTP_BR_3CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_3CH_POS)) 
#define OTP_BR_40H_POS			    		((uint32_t) 0) 
#define OTP_BR_40H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_40H_POS)) 
#define OTP_BR_44H_POS			    		((uint32_t) 0) 
#define OTP_BR_44H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_44H_POS)) 
#define OTP_BR_48H_POS			    		((uint32_t) 0) 
#define OTP_BR_48H_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_48H_POS)) 
#define OTP_BR_4CH_POS			    		((uint32_t) 0) 
#define OTP_BR_4CH_MASK			    		((uint32_t) ((uint32_t)0xFFFFFFFF << OTP_BR_4CH_POS)) 
/**
 * @defgroup CHIP_GM6721_OTP_CLK_SRC
 * @{
 */
#define OTP_SRC_CLK_DIV512                 390625            /* OTP Srcclk:SYSCLK/(511+1) */
typedef enum
{
	OTP_WORKMODE_NORMAL = 0,
	OTP_WORKMODE_IMRDM = 1,
	OTP_WORKMODE_PGM = 2,
	OTP_WORKMODE_PMRDM = 4
}OTP_WORKMODE_T;

typedef enum
{
	OTP_CELLMODE_MAIN = 0,
	OTP_CELLMODE_REDUNDANCY = 1,
	OTP_CELLMODE_TESTROW
}OTP_CELLMODE_T;

/**
  * @brief  Start the  OTP Device
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_Enable(OTP_Device_T *pOTP)
{
    IO_BIT_SET(pOTP->EN, OTP_EN_ENABLE);
} 



/**
  * @brief  Stop the  OTP Device
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_Disable(OTP_Device_T *pOTP)
{
    IO_BIT_CLEAR(pOTP->CTRL, OTP_EN_ENABLE);
} 

/**
  * @brief  Set counter when an interrupt is generated
  * @param  pOTP OTP Instance
  * @param  winLen base number
  * @retval None
  */
__STATIC_INLINE void   LL_OTP_SetWorkMode(OTP_Device_T *pOTP,OTP_WORKMODE_T mode)
{
    IO_BITMASK_SET(pOTP->WM,OTP_WM_MODE_MASK,(uint32_t)mode);
}

__STATIC_INLINE OTP_WORKMODE_T LL_OTP_GetWorkMode(OTP_Device_T *pOTP)
{
    return (OTP_WORKMODE_T)((IO_BITS_GET(pOTP->WM,OTP_WM_MODE_MASK)) >> OTP_WM_MODE_POS);
}

/**
  * @brief  FeedOTP  
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_SetCellMode(OTP_Device_T *pOTP,OTP_CELLMODE_T cellmode)
{
    switch(cellmode)
    {
    	case OTP_CELLMODE_MAIN:
    		IO_BITMASK_SET(pOTP->CS,OTP_CS_PTR_MASK|OTP_CS_PAS_MASK,OTP_CS_PAS_MCEN);
    		break;
    		
    	case OTP_CELLMODE_REDUNDANCY:
    		IO_BITMASK_SET(pOTP->CS,OTP_CS_PTR_MASK|OTP_CS_PAS_MASK,OTP_CS_PAS_RCEN);
    		break;
    		
    	case OTP_CELLMODE_TESTROW:
    	default:
    		IO_BITMASK_SET(pOTP->CS,OTP_CS_PTR_MASK|OTP_CS_PAS_MASK,OTP_CS_PTR_TREN);
    		break;
    }
} 


/**
  * @brief  FeedOTP  
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_SetAddress(OTP_Device_T *pOTP,uint32_t addr)
{
	IO_BITMASK_SET(pOTP->ADDR,OTP_ADDR_MASK,(addr>>2));
} 

/**
  * @brief  clear otp irq which is equal to feedotp  
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE uint32_t  LL_OTP_GetAddress(OTP_Device_T *pOTP)
{
    return (uint32_t)(IO_READ_REG(pOTP->ADDR) & OTP_ADDR_MASK);
} 

/**
  * @brief  clear otp irq which is equal to feedotp  
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE uint32_t  LL_OTP_GetStatus(OTP_Device_T *pOTP)
{
    return (uint32_t)(IO_READ_REG(pOTP->ST));
} 


/**
  * @brief  FeedOTP  
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_SetData(OTP_Device_T *pOTP,uint32_t data)
{
	IO_BITMASK_SET(pOTP->PDIN,OTP_DATA_MASK,data);
} 


/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_ECCToggle_Enable(OTP_Device_T *pOTP)
{
    IO_BIT_CLEAR(pOTP->CTRL, OTP_CTRL_ECC_TOG_DISABLE);   
} 

/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_ECCToggle_Disable(OTP_Device_T *pOTP)
{
    IO_BIT_SET(pOTP->CTRL, OTP_CTRL_ECC_TOG_DISABLE);   
}

/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_ECC_TOG_Enable(OTP_Device_T *pOTP)
{
    IO_BIT_SET(pOTP->CTRL, OTP_CTRL_DB_SET_ENABLE);   
} 

/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_ECC_TOG_Disable(OTP_Device_T *pOTP)
{
    IO_BIT_CLEAR(pOTP->CTRL, OTP_CTRL_DB_SET_ENABLE);   
}

/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_Write_Start(OTP_Device_T *pOTP)
{
    IO_BIT_SET(pOTP->CTRL, OTP_CTRL_PROG_TRIG_ENABLE);   
}

/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE uint32_t  LL_OTP_GetData(OTP_MEMIO_T *pMemIO,uint32_t addr)
{
    return (uint32_t)(IO_READL((uint32_t)pMemIO + addr) & OTP_DATA_MASK);
}

/**
  * @brief  Open otp interrupt bit
  * @param  pOTP OTP Instance
  * @retval None
  */
__STATIC_INLINE void  LL_OTP_SetTimingData(OTP_Device_T *pOTP,uint32_t mTPW,uint32_t mTPWI,uint32_t mTCS,uint32_t mTPPS,uint32_t mTPPH,uint32_t mTPPR)
{
    IO_BITMASK_SET(pOTP->TIM0, OTP_TIM0_TPPS_SET_MASK|OTP_TIM0_TPPH_SET_MASK|OTP_TIM0_TPPR_SET_MASK,
    							((mTPPS & 0xff)<< OTP_TIM0_TPPS_SET_POS) | ((mTPPH & 0xff)<< OTP_TIM0_TPPH_SET_POS)|
    							((mTPPR & 0x3ff)<< OTP_TIM0_TPPR_SET_POS));   
    							
   	IO_BITMASK_SET(pOTP->TIM1, OTP_TIM1_TPW_SET_MASK|OTP_TIM1_TPWI_SET_MASK|OTP_TIM1_TCS_SET_MASK,
    							((mTPW & 0xff)<< OTP_TIM1_TPW_SET_POS) | ((mTPWI & 0x3f)<< OTP_TIM1_TPWI_SET_POS)|
    							((mTCS & 0xff)<< OTP_TIM1_TCS_SET_POS));  
}

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_OTP_H_ */
