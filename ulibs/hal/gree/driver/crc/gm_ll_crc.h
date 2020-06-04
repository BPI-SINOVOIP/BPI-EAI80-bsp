/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          gm_ll_crc.h
 *
 * @author        bert.Ding
 *
 * @version       1.0.0
 * 
 * @date          2019/10/16 
 *    
 * @brief         CRC peripheral PHY device header file
 * 
 * @note
 *    2019/10/16 , bert.Ding, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _GM_LL_CRC_H_
#define _GM_LL_CRC_H_

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_crc_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************** CRC Control Register Bitfiled ******************/
#define CRC_CTRL_IE_POS                  ((uint32_t) 16)                                    /* CRC INT Enable Pos */
#define CRC_CTRL_IE                      ((uint32_t) ((uint32_t)1 << CRC_CTRL_IE_POS))      /* CRC INT Enable Filed */
#define CRC_CTRL_XOE_POS                 ((uint32_t) 12)                                    /* CRC Xor Output Enable Pos */
#define CRC_CTRL_XOE                     ((uint32_t) ((uint32_t)1 << CRC_CTRL_XOE_POS))     /* CRC Xor Output Enable Filed */
#define CRC_CTRL_RRO_POS                 ((uint32_t) 11)                                    /* Control Bit Reversal Of The Output CRC Calc Result POS */
#define CRC_CTRL_RRO                     ((uint32_t) ((uint32_t)1 << CRC_CTRL_RRO_POS))     /* Control Bit Reversal Of The Output CRC Calc Result Filed */
#define CRC_CTRL_RDI_POS                 ((uint32_t) 8)                                     /* Control The Reversal of The Bit Order Of The Input Data Pos */        
#define CRC_CTRL_RDI                     ((uint32_t) ((uint32_t)3 << CRC_CTRL_RDI_POS))     /* Control The Reversal of The Bit Order Of The Input Data Filed */
#define CRC_CTRL_DIW_POS                 ((uint32_t) 6)                                     /* The Input Data Width Pos */
#define CRC_CTRL_DIW                     ((uint32_t) ((uint32_t)3 << CRC_CTRL_DIW_POS))     /* The Input Data Width Filed */
#define CRC_CTRL_PS_POS                  ((uint32_t) 4)                                     /* Control The Size Of The Polynomial Pos */
#define CRC_CTRL_PS                      ((uint32_t) ((uint32_t)3 << CRC_CTRL_PS_POS))      /* Control The Size Of The Polynomial Filed */
#define CRC_CTRL_CRST_POS                ((uint32_t) 0)                                     /* Reset CRC Calculation Pos */
#define CRC_CTRL_CRST                    ((uint32_t) ((uint32_t)1 << CRC_CTRL_CRST_POS))    /* Reset CRC Calculation Filed */

/*************** CRC Status Register Bitfiled ******************/
#define CRC_ST_INT_POS					   ((uint32_t) 16)
#define CRC_ST_INT						   ((uint32_t) ((uint32_t)1 << CRC_ST_INT_POS))
#define CRC_ST_DNE_POS                     ((uint32_t) 0)                                   /* CRC Calculation Finish Pos */
#define CRC_ST_DNE                         ((uint32_t) ((uint32_t)1 << CRC_ST_DNE_POS))     /* CRC Calculation Finish Filed */

/**
 * CRC Accumulate Datawidth For One Time 
 */
typedef enum  
{
    DATA_PROCESS_WIDTH_8B  = 0x00,               	             /**< CRC Calc 8bit Datawidth For one time */    
    DATA_PROCESS_WIDTH_16B       ,                               /**< CRC Calc 16bit Datawidth For one time */
    DATA_PROCESS_WIDTH_32B                      	             /**< CRC Calc 32bit Datawidth For one time */
} CRC_ProcessWidth_T;

/**
 * @brief Set CRC Accumulate InitValue
 * @param pCRC CRC Devive Point
 * @param initValue CRC Accumulate InitValue
 * @retval None
 */
#define LL_SET_INITVAL(pCRC, initValue)            		         (pCRC->IVAL = initValue)

/**
 * @brief Set CRC Accumulate Polynomia
 * @param pCRC CRC Devive Point
 * @param value CRC Accumulate polynomia
 * @retval None
 */
 
#define LL_SET_POL(pCRC, value)                                     (pCRC->POL = value)     

/**
 * @brief Set CRC Accumulate mode
 * @param pCRC CRC Devive Point
 * @param polyLength CRC Accumulate Mode
 * @Pol_Bit_Width = CRC_POLYLENGTH_8B  : crc8
 * @Pol_Bit_Width = CRC_POLYLENGTH_16B : crc16
 * @Pol_Bit_Width = CRC_POLYLENGTH_32B : crc32
 * @Pol_Bit_Width = CRC_POLYLENGTH_7B  : crc7
 * @retval None
 */

#define LL_SET_POL_BITWIDTH(pCRC, polyLength)                      IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_PS,                 \
                                                                               (polyLength << CRC_CTRL_PS_POS))

/**
 * @brief Set HW Accumulate DataBit Width One Time
 * @param pCRC pCRC Devive Point
 * @param dataBitWidth  HW Accumulate DataBit Width One Time
 * @retval None
 */
#define LL_SET_DATA_PROCESS_BITWIDTH(pCRC, dataBitWidth)     		IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_DIW, 		        \
                                                                              (dataBitWidth << CRC_CTRL_DIW_POS))

/**
 * @brief Set Inputdata Inversion Mode
 * @param pCRC CRC Devive Point
 * @param intputInversion Inputdata Inversion Mode
 * @retval None
 */
#define LL_SET_INPUTDATA_INVERSION(pCRC,intputInversion)     		IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_RDI, 		        \
                                                         			   		  (intputInversion << CRC_CTRL_RDI_POS))

/**
 * @brief Set Outputdata Inversion Mode
 * @param pCRC CRC Devive Point
 * @param outputInversion Outputdata Inversion Mode
 * @retval None
 */
#define LL_SET_OUTPUTDATA_INVERSION(pCRC,outputInversion)     		IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_RRO, 	            \
                                                                	  		  (outputInversion << CRC_CTRL_RRO_POS))

/**
 * @brief Enable CRC Xorval 
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define LL_XORVAL_ENABLE(pCRC)				                        IO_BIT_SET(pCRC->CTRL, CRC_CTRL_XOE)

/**
 * @brief  Set CRC Xor Value 
 * @param pCRC CRC Devive Point
 * @param Value CRC Xor Value 
 * @retval None
 */
#define LL_SET_XORVAl(pCRC,Value)                	                (pCRC->XOR = Value)
													
/**
 * @brief  Load Initvalue And Ready To Accumulate
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define LL_CALC_RESTART(pCRC)                                      IO_BIT_SET(pCRC->CTRL, CRC_CTRL_CRST)

/**
 * @brief Enable CRC Interrupt 
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define LL_INTERRUPT_ENABLE(pCRC)                                  IO_BIT_SET(pCRC->CTRL, CRC_CTRL_IE)

/**
 * @brief Disable CRC Interrupt 
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define LL_INTERRUPT_DISABLE(pCRC)                                 IO_BIT_CLEAR(pCRC->CTRL, CRC_CTRL_IE)

#define CRC_DEV													   ((CRC_Device_T*)CRC_BASE)

#ifdef __cplusplus
  }
#endif
#endif /* #ifndef _GM_LL_IMP_H_ */
