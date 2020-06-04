/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_sqi.h
 *
 * @author        Colin.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief
 *
 * @note
 *    2018/03/24, Colin.Yang, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_SQI_H_
#define _GM_LL_SQI_H_
#include <gm_common.h>
#include<gm_soc.h>
#include"gm_ll_sqi_dev.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    SQI_MOD0 = 0,
    SQI_MOD1 = 1,
    SQI_MOD2 = 2,
    SQI_MOD3 = 3,
    SQI_MOD4 = 4,
    SQI_MOD5 = 5,
    SQI_MOD6 = 6
} LL_SQI_RW_MODE_T;

typedef enum
{
    SQI_3BYTE_ADDR = 0,
    SQI_4BYTE_ADDR = 1,
    SQI_1BYTE_ADDR = 2,
    SQI_2BYTE_ADDR = 3
} LL_SQI_ADDR_MODE_T;

typedef enum
{
    SQI_DMY_BYTE_NUM1 = 0,
    SQI_DMY_BYTE_NUM2 = 1,
    SQI_DMY_BYTE_NUM3 = 2,
    SQI_DMY_BYTE_NUM4 = 3,
} SQI_DMY_BYTE_NUM_T;

#define SQI_ADDR_BASE        0x40001000//这个具体要放到哪个文件里面，待定
#define SQI                  ((SQI_Device_T *)(SQI_ADDR_BASE))    /* SQI reg base */

/**************************** SQI Version register bitfiled *********************************/


/**************************** SQI Baud Rate register bitfiled *********************************/
#define SQI_CLK_DIV_EXT_SET_POS                             ((uint32_t)16)
#define SQI_CLK_DIV_EXT_SET                                 ((uint32_t)((uint32_t)0x3F << SQI_CLK_DIV_EXT_SET_POS))
#define SQI_CLK_ODD_FRE_DIV_ENABLE_POSE                     ((uint32_t)8)
#define SQI_CLK_ODD_FRE_DIV_ENABLE                          ((uint32_t)((uint32_t)1 << SQI_CLK_ODD_FRE_DIV_ENABLE_POSE))
#define SQI_CLK_DIV_SET_POS                                 ((uint32_t)0)
#define SQI_CLK_DIV_SET                                     ((uint32_t)((uint32_t)0xF << SQI_CLK_DIV_SET_POS))

/**************************** SQI Configuration register bitfiled *********************************/
#define SQI_HOLDJ_EN_POS                                    ((uint32_t)29)
#define SQI_HOLDJ_EN                                        ((uint32_t)((uint32_t)1 << SQI_HOLDJ_EN_POS))
#define SQI_WJ_EN_POS                                       ((uint32_t)28)
#define SQI_WJ_EN                                           ((uint32_t)((uint32_t)1<<SQI_WJ_EN_POS))
#define SQI_FAST_RW_CNT_EN_POS                              ((uint32_t)21)
#define SQI_FAST_RW_CNT_EN                                  ((uint32_t)((uint32_t)1<<SQI_FAST_RW_CNT_EN_POS))
#define SQI_ADDR_BYTE_NUM_POS                               ((uint32_t)19)
#define SQI_ADDR_BYTE_NUM                                   ((uint32_t)((uint32_t)3<<SQI_ADDR_BYTE_NUM_POS))
#define SQI_RW_IO_MODE_POS                                  ((uint32_t)16)
#define SQI_RW_IO_MODE                                      ((uint32_t)((uint32_t)7<<SQI_RW_IO_MODE_POS))
#define SQI_CONTINUE_WRITE_EN_POS                           ((uint32_t)15)
#define SQI_CONTINUE_WRITE_EN                               ((uint32_t)((uint32_t)1<<SQI_CONTINUE_WRITE_EN_POS))
#define SQI_CONTINUE_READ_EN_POS                            ((uint32_t)14)
#define SQI_CONTINUE_READ_EN                                ((uint32_t)((uint32_t)1<<SQI_CONTINUE_READ_EN_POS))
#define SQI_DMY_NUM_POS                                     ((uint32_t)12)
#define SQI_DMY_NUM                                         ((uint32_t)((uint32_t)3<<SQI_DMY_NUM_POS))
#define SQI_SET_CODE_HIT_POS                                ((uint32_t)11)
#define SQI_SET_CODE_HIT                                    ((uint32_t)((uint32_t)1<<SQI_SET_CODE_HIT_POS))
#define SQI_SET_ADDR_HIT_POS                                ((uint32_t)10)
#define SQI_SET_ADDR_HIT                                    ((uint32_t)((uint32_t)1<<SQI_SET_ADDR_HIT_POS))
#define SQI_SET_DMY_HIT_POS                                 ((uint32_t)9)
#define SQI_SET_DMY_HIT                                     ((uint32_t)((uint32_t)1<<SQI_SET_DMY_HIT_POS))
#define SQI_SET_DATA_HIT_POS                                ((uint32_t)8)
#define SQI_SET_DATA_HIT                                    ((uint32_t)((uint32_t)1<<SQI_SET_DATA_HIT_POS))
#define SQI_INS_SET_POS                                     ((uint32_t)0)
#define SQI_INS_SET                                         ((uint32_t)((uint32_t)0xFF<<SQI_INS_SET_POS))


/**************************** SQI Dummy register bitfiled *********************************/

/**************************** SQI Counter register bitfiled *********************************/

/****************************SQI Address Configuration    *********************************/

#define SQI_4th_BYTE_ADDR_PE_POS                             ((uint32_t) 8)
#define SQI_4th_BYTE_ADDR_PE                                 ((uint32_t)((uint32_t)1<<SQI_4th_BYTE_ADDR_PE_POS))

/****************************  SQI Timing Control Register bitfiled *********************************/
#define SQI_CS_PROG_SET_POS                                  ((uint32_t) 25)
#define SQI_CS_PROG_SET                                      ((uint32_t)((uint32_t)3 << SQI_CS_PROG_SET_POS))
#define SQI_CS_PROG_EN_POS                                   ((uint32_t) 24)
#define SQI_CS_PROG_EN                                       ((uint32_t)((uint32_t)1 << SQI_CS_PROG_EN_POS))
#define SQI_CS_DIS_SET_POS                                   ((uint32_t) 17)
#define SQI_CS_DIS_SET                                       ((uint32_t)((uint32_t)0x7F << SQI_CS_DIS_SET_POS))
#define SQI_CS_DIS_PROG_EN_POS                               ((uint32_t) 16)
#define SQI_CS_DIS_PROG_EN                                   ((uint32_t)((uint32_t)1 << SQI_CS_DIS_PROG_EN_POS))
#define SQI_CS_HOLD_SET_POS                                  ((uint32_t) 9)
#define SQI_CS_HOLD_SET                                      ((uint32_t)((uint32_t)0x7F << SQI_CS_HOLD_SET_POS))
#define SQI_CS_HOLD_PROG_EN_POS                              ((uint32_t) 8)
#define SQI_CS_HOLD_PROG_EN                                  ((uint32_t)((uint32_t)1 << SQI_CS_HOLD_PROG_EN_POS))
#define SQI_CS_SETUP_SET_POS                                 ((uint32_t) 1)
#define SQI_CS_SETUP_SET                                     ((uint32_t)((uint32_t)0x7F << SQI_CS_SETUP_SET_POS))
#define SQI_CS_SETUP_PROG_EN_POS                             ((uint32_t) 0)
#define SQI_CS_SETUP_PROG_EN                                 ((uint32_t)((uint32_t)1 << SQI_CS_SETUP_PROG_EN_POS))

/****************************  SQI Prefetch Control Register bitfiled *********************************/
#define SQI_RD_PREFETCH_EN_POS                               ((uint32_t) 0)
#define SQI_RD_PREFETCH_EN                                   ((uint32_t)((uint32_t)1 << SQI_RD_PREFETCH_EN_POS))

/****************************  SQI CRC Control Register bitfiled *********************************/
#define SQI_CRC_CLEAR_EN_POS                                 ((uint32_t) 8)
#define SQI_CRC_CLEAR_EN                                     ((uint32_t)((uint32_t)1 << SQI_CRC_CLEAR_EN_POS))
#define SQI_CRC_VALID_POS                                    ((uint32_t) 0)
#define SQI_CRC_VALID                                        ((uint32_t)((uint32_t)1 << SQI_CRC_VALID_POS))








/*******************************Flash write*******************************************/
/**
* @brief Set clock DIV EXT
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_FLASH_SET(addr,val)                              IO_WRITEL(addr,val)


/**************************** SQI Baud Rate register *********************************/

/**
* @brief Set clock DIV EXT
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_CLK_DIV_EXT_SET(pSQI, val)                    IO_BITS_SET(pSQI->BR, val)

#define LL_SQI_SET_CLK_DIV_EXT(pSQI, val)                    IO_BITS_SET(pSQI->BR, (uint32_t)val << SQI_CLK_DIV_EXT_SET_POS)




/**
* @brief  Enable Clock ODD Frequence Divide
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CLK_ODD_FRE_DIV_ENABLE(pSQI)                  IO_BIT_SET(pSQI->BR, SQI_CLK_ODD_FRE_DIV_ENABLE)

/**
* @brief  Disable Clock ODD Frequence Divide
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CLK_ODD_FRE_DIV_DISABLE(pSQI)                 IO_BIT_CLEAR(pSQI->BR, SQI_CLK_ODD_FRE_DIV_ENABLE)

/**
* @brief Set clock DIV
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_CLK_DIV_SET(pSQI, val)                        IO_BITS_SET(pSQI->BR, val)
#define LL_SQI_SET_CLK_DIV(pSQI, val)                        IO_BITMASK_SET(pSQI->BR, SQI_CLK_DIV_SET,val << SQI_CLK_DIV_SET_POS)

#define LL_SQT_GET_CLK_BITS(pSQI, mask)                      IO_BITS_GET(pSQI->BR, mask)



/**************************** SQI Configuration register *********************************/


#define LL_SQT_GET_CFG_BITS(pSQI, mask)                      IO_BITS_GET(pSQI->CFG, mask)


/**
* @brief  Enable SQI HOLDJ Control
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_HOLDJ_ENABLE(pSQI)                            IO_BIT_SET(pSQI->CFG, SQI_HOLDJ_EN)

/**
* @brief  Disable SQI HOLDJ Control
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_HOLDJ_DISABLE(pSQI)                           IO_BIT_CLEAR(pSQI->CFG, SQI_HOLDJ_EN)

/**
* @brief  Enable SQI WJ Control
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_WJ_ENABLE(pSQI)                               IO_BIT_SET(pSQI->CFG, SQI_WJ_EN)

/**
* @brief  Disable SQI WJ Control
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_WJ_DISABLE(pSQI)                              IO_BIT_CLEAR(pSQI->CFG, SQI_WJ_EN)

/**
* @brief  Enable SQI Continue count Function
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_FAST_RW_CNT_ENABLE(pSQI)                     IO_BIT_SET(pSQI->CFG, SQI_FAST_RW_CNT_EN)

/**
* @brief  Disable SQI Continue count Function
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_FAST_RW_CNT_DISABLE(pSQI)                    IO_BIT_CLEAR(pSQI->CFG, SQI_FAST_RW_CNT_EN)

/**
* @brief Set Flash Address Number
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_ADDR_BYTE_NUM_SET(pSQI, val)                  IO_BITS_SET(pSQI->CFG, val)
#define LL_SQI_SET_ADDR_BYTE_NUM(pSQI, val)                  IO_BITMASK_SET(pSQI->CFG, SQI_ADDR_BYTE_NUM,(uint32_t)val << SQI_ADDR_BYTE_NUM_POS)

/**
* @brief Set dtr
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_RW_IO_MODE_SET(pSQI, val)                      IO_BITS_SET(pSQI->CFG, val)
#define LL_SQI_SET_RW_IO_MODE(pSQI, val)                      IO_BITMASK_SET(pSQI->CFG, SQI_RW_IO_MODE,(uint32_t)val << SQI_RW_IO_MODE_POS)

/**
* @brief  Enable SQI  Continue Write
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CONTINUE_WRITE_ENABLE(pSQI)                   IO_BIT_SET(pSQI->CFG, SQI_CONTINUE_WRITE_EN)

/**
* @brief  Disable SQI Continue Write
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CONTINUE_WRITE_DISABLE(pSQI)                  IO_BIT_CLEAR(pSQI->CFG, SQI_CONTINUE_WRITE_EN)

/**
* @brief  Enable SQI  Continue Read
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CONTINUE_READ_ENABLE(pSQI)                    IO_BIT_SET(pSQI->CFG, SQI_CONTINUE_READ_EN)

/**
* @brief  Disable SQI Continue Read
*
* @param  pSQI          pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CONTINUE_READ_DISABLE(pSQI)                   IO_BIT_CLEAR(pSQI->CFG, SQI_CONTINUE_READ_EN)

/**
* @brief Set SQI Dummy Number
<<<<<<< HEAD
*
* @param  pSQI        pSQI devive Point
*
=======
*
* @param  pSQI        pSQI devive Point
*
>>>>>>> b175883e2c5322d517123ce9f7972bcbdaef12e6
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_DMY_NUM_SET(pSQI, val)                         IO_BITS_SET(pSQI->CFG, val)
#define LL_SQI_SET_DMY_NUM(pSQI, val)                         IO_BITMASK_SET(pSQI->CFG, SQI_DMY_NUM,(uint32_t)val << SQI_DMY_NUM_POS)


/**
* @brief  Enable Code Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_SET_CODE_HIT(pSQI)                             IO_BIT_SET(pSQI->CFG, SQI_SET_CODE_HIT)

/**
* @brief  Disable  Code Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_DISABLE_CODE_HIT(pSQI)                        IO_BIT_CLEAR(pSQI->CFG, SQI_SET_CODE_HIT)

/**
* @brief  Enable Address Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_SET_ADDR_HIT(pSQI)                             IO_BIT_SET(pSQI->CFG, SQI_SET_ADDR_HIT)

/**
* @brief  Disable  Address Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_DISABLE_ADDR_HIT(pSQI)                        IO_BIT_CLEAR(pSQI->CFG, SQI_SET_ADDR_HIT)

/**
* @brief  Enable Dummy Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_SET_DMY_HIT(pSQI)                             IO_BIT_SET(pSQI->CFG, SQI_SET_DMY_HIT)

/**
* @brief  Disable  Dummy Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_DISABLE_DMY_HIT(pSQI)                         IO_BIT_CLEAR(pSQI->CFG, SQI_SET_DMY_HIT)

/**
* @brief  Enable Data Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_SET_DATA_HIT(pSQI)                            IO_BIT_SET(pSQI->CFG, SQI_SET_DATA_HIT)

/**
* @brief  Disable  Data Hit
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_DISABLE_DATA_HIT(pSQI)                        IO_BIT_CLEAR(pSQI->CFG, SQI_SET_DATA_HIT)


/**
* @brief Set the data stored here will be sent out first as the operation instruction
*
* @param  pSQI        pSQI devive Point
*
* @param  val             flash_ins value
*
* @retval void
*/
#define LL_SQI_COMMAND_SET(pSQI, val)                             IO_BITS_SET(pSQI->CFG, val)

/**
<<<<<<< HEAD
* @brief clear the data stored here will be sent out first as the operation instruction
*
* @param  pSQI        pSQI devive Point
*
* @param  val             flash_ins value
*
* @retval void
*/
#define LL_SQI_COMMAND_CLEAR(pSQI, val)                             IO_BIT_CLEAR(pSQI->CFG, val)



/**
=======
>>>>>>> b175883e2c5322d517123ce9f7972bcbdaef12e6
* @brief Enable SQI flash 4th byte address programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/


/****************************SQI Address Configuration    *********************************/



#define LL_SQI_4th_BYTE_ADDR_MODE_ENABLE(pSQI)                IO_BIT_SET(pSQI->ADDRCFG, SQI_4th_BYTE_ADDR_PE)

/**
* @brief  Disable  SQI flash 4th byte address programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_4th_BYTE_ADDR_MODE_DISABLE(pSQI)              IO_BIT_CLEAR(pSQI->ADDRCFG, SQI_4th_BYTE_ADDR_PE)


/**
* @brief Set SQI the 4th byte address
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_4th_BYTE_ADDR_SET(pSQI, val)                         IO_BITS_SET(pSQI->ADDRCFG, (uint32_t)val)



/**
* @brief Set SQI CS Program value
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_CS_PROG_SET(pSQI, val)                         IO_BITS_SET(pSQI->CFG, (uint32_t)val)


/**
* @brief Enable  CS programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_PROG_ENABLE(pSQI)                          IO_BIT_SET(pSQI->TCR, SQI_CS_PROG_EN)

/**
* @brief  Disable   CS programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_PROG_DISABLE(pSQI)                         IO_BIT_CLEAR(pSQI->TCR, SQI_CS_PROG_EN)

/**
* @brief Setting for CS dis-assertion time
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_CS_DIS_SET(pSQI, val)                          IO_BITS_SET(pSQI->TCR, val)

/**
* @brief Enable  CS dis-assertion time programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_DIS_PROG_ENABLE(pSQI)                      IO_BIT_SET(pSQI->TCR, SQI_CS_DIS_PROG_EN)

/**
* @brief  Disable   CS dis-assertion time programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_DIS_PROG_DISABLE(pSQI)                     IO_BIT_CLEAR(pSQI->TCR, SQI_CS_DIS_PROG_EN)

/**
* @brief Setting for  CS hold time
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_CS_HOLD_SET(pSQI, val)                        IO_BITMASK_SET(pSQI->TCR, SQI_CS_HOLD_SET, (val << SQI_CS_HOLD_SET_POS))

/**
* @brief Enable  CS hold time programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_HOLD_PROG_ENABLE(pSQI)                     IO_BIT_SET(pSQI->TCR, SQI_CS_HOLD_PROG_EN)

/**
* @brief  Disable   CS hold time programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_HOLD_PROG_DISABLE(pSQI)                    IO_BIT_CLEAR(pSQI->TCR, SQI_CS_HOLD_PROG_EN)

/**
* @brief Setting for  CS setup time
<<<<<<< HEAD
*
* @param  pSQI        pSQI devive Point
*
=======
*
* @param  pSQI        pSQI devive Point
*
>>>>>>> b175883e2c5322d517123ce9f7972bcbdaef12e6
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_CS_SETUP_SET(pSQI, val)                       IO_BITMASK_SET(pSQI->TCR, SQI_CS_SETUP_SET, (val << SQI_CS_SETUP_SET_POS))

/**
* @brief Enable  CS setup time programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_SETUP_PROG_ENABLE(pSQI)                    IO_BIT_SET(pSQI->TCR, SQI_CS_SETUP_PROG_EN)

/**
* @brief  Disable   CS setup time programmable function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CS_SETUP_PROG_DISABLE(pSQI)                   IO_BIT_CLEAR(pSQI->TCR, SQI_CS_SETUP_PROG_EN)

/**
* @brief Enable  Prefetch function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_RD_PREFETCH_ENABLE(pSQI)                      IO_BIT_SET(pSQI->PREFCTRL, SQI_RD_PREFETCH_EN)

/**
* @brief  Disable   Prefetch function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_RD_PREFETCH_DISABLE(pSQI)                     IO_BIT_CLEAR(pSQI->PREFCTRL, SQI_RD_PREFETCH_EN)

/**
* @brief Enable  SQI CRC Clear Function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CRC_CLEAR_ENABLE(pSQI)                        IO_BIT_SET(pSQI->CRCCTRL, SQI_CRC_CLEAR_EN)

/**
* @brief  Disable   SQI CRC Clear Function
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CRC_CLEAR_DISABLE(pSQI)                       IO_BIT_CLEAR(pSQI->CRCCTRL, SQI_CRC_CLEAR_EN)


/**
* @brief  Make SQI CRC Valid
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CRC_VALID(pSQI)                               IO_BIT_CLEAR(pSQI->CRCCTRL, SQI_CRC_VALID)

/**
* @brief Make SQI CRC Invalid
*
* @param  pSQI         pSQI devive Point
*
* @retval void
*/
#define LL_SQI_CRC_INVALID(pSQI)                             IO_BIT_SET(pSQI->CRCCTRL, SQI_CRC_VALID)

/**
* @brief Setting for CS dis-assertion time
*
* @param  pSQI        pSQI devive Point
*
* @param  val             Dtr value
*
* @retval void
*/
#define LL_SQI_FAST_RW_CNT_SET(pSQI, val)                          IO_WRITE_REG(pSQI->CNT, val)


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_SQI_H_ */
