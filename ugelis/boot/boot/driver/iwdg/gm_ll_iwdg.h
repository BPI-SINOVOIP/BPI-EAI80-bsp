/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_iwdg.h
 *
 * @author        kenny.cmt
 *
 * @version       1.0
 *
 * @date          2018/03/23
 *
 * @brief         CMSIS Peripheral Access Layer for GM67xx
 *
 * @note
 *    2018/03/23, kenny.cmt, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_IWDG_H_
#define _GM_LL_IWDG_H_


#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <gm_common.h>
#include<gm_soc.h>
#include"gm_ll_iwdg_dev.h"

/** @addtogroup GM67xx_LL_Driver
  * @{
  */


/** @defgroup IWDG_LL IWDG
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup IWDG_LL_Private_Constants IWDG Private Constants
  * @{
  */

#define LL_IWDG_KEY_RELOAD                 0x0000AAAAU               /*!< IWDG Reload Counter Enable   */
#define LL_IWDG_KEY_ENABLE                 0x0000CCCCU               /*!< IWDG Peripheral Enable       */
#define LL_IWDG_KEY_WR_ACCESS_ENABLE       0x00005555U               /*!< IWDG KR Write Access Enable  */
#define LL_IWDG_KEY_WR_ACCESS_DISABLE      0x00000000U               /*!< IWDG KR Write Access Disable */

/**
  * @}
  */

  
  
/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/



/******************************************************************************/
/*                                                                            */
/*                           Independent WATCHDOG                             */
/*                                                                            */
/******************************************************************************/
/*******************  Bit definition for IWDG_KR register  ********************/
#define IWDG_KR_KEY_Pos     (0U)                                               
#define IWDG_KR_KEY_Msk     (0xFFFFU << IWDG_KR_KEY_Pos)                       /*!< 0x0000FFFF */
#define IWDG_KR_KEY         IWDG_KR_KEY_Msk                                    /*!<Key value (write only, read 0000h)  */

/*******************  Bit definition for IWDG_PR register  ********************/
#define IWDG_PR_PR_Pos      (0U)                                               
#define IWDG_PR_PR_Msk      (0x7U << IWDG_PR_PR_Pos)                           /*!< 0x00000007 */
#define IWDG_PR_PR          IWDG_PR_PR_Msk                                     /*!<PR[2:0] (Prescaler divider)         */
#define IWDG_PR_PR_0        (0x1U << IWDG_PR_PR_Pos)                           /*!< 0x01 */
#define IWDG_PR_PR_1        (0x2U << IWDG_PR_PR_Pos)                           /*!< 0x02 */
#define IWDG_PR_PR_2        (0x4U << IWDG_PR_PR_Pos)                           /*!< 0x04 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define IWDG_RLR_RL_Pos     (0U)                                               
#define IWDG_RLR_RL_Msk     (0xFFFU << IWDG_RLR_RL_Pos)                        /*!< 0x00000FFF */
#define IWDG_RLR_RL         IWDG_RLR_RL_Msk                                    /*!<Watchdog counter reload value        */

/*******************  Bit definition for IWDG_SR register  ********************/
#define IWDG_SR_PVU_Pos     (0U)                                               
#define IWDG_SR_PVU_Msk     (0x1U << IWDG_SR_PVU_Pos)                          /*!< 0x00000001 */
#define IWDG_SR_PVU         IWDG_SR_PVU_Msk                                    /*!<Watchdog prescaler value update      */
#define IWDG_SR_RVU_Pos     (1U)                                               
#define IWDG_SR_RVU_Msk     (0x1U << IWDG_SR_RVU_Pos)                          /*!< 0x00000002 */
#define IWDG_SR_RVU         IWDG_SR_RVU_Msk                                    /*!<Watchdog counter reload value update */



/** @defgroup IWDG_LL_Exported_Constants IWDG Exported Constants
  * @{
  */

/** @defgroup IWDG_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_IWDG_ReadReg function
  * @{
  */
#define LL_IWDG_SR_PVU                     IWDG_SR_PVU                           /*!< Watchdog prescaler value update */
#define LL_IWDG_SR_RVU                     IWDG_SR_RVU                           /*!< Watchdog counter reload value update */

/**
  * @}
  */

/** @defgroup IWDG_LL_EC_PRESCALER  Prescaler Divider
  * @{
  */
#define LL_IWDG_PRESCALER_4                0x00000000U                           /*!< Divider by 4   */
#define LL_IWDG_PRESCALER_8                (IWDG_PR_PR_0)                        /*!< Divider by 8   */
#define LL_IWDG_PRESCALER_16               (IWDG_PR_PR_1)                        /*!< Divider by 16  */
#define LL_IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0)         /*!< Divider by 32  */
#define LL_IWDG_PRESCALER_64               (IWDG_PR_PR_2)                        /*!< Divider by 64  */
#define LL_IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0)         /*!< Divider by 128 */
#define LL_IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1)         /*!< Divider by 256 */
/**
  * @}
  */

#define IWDG                                ((IWDG_TypeDef *)(IWDG_BASE))        /* IWDG reg base */


/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup IWDG_LL_Exported_Macros IWDG Exported Macros
  * @{
  */
#if 0
/** @defgroup IWDG_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in IWDG register
  * @param  __INSTANCE__ IWDG Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_IWDG_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in IWDG register
  * @param  __INSTANCE__ IWDG Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_IWDG_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */
#endif
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup IWDG_LL_Exported_Functions IWDG Exported Functions
  * @{
  */
/** @defgroup IWDG_LL_EF_Configuration Configuration
  * @{
  */

/**
  * @brief  Start the Independent Watchdog
  * @note   Except if the hardware watchdog option is selected
  * @rmtoll KR           KEY           LL_IWDG_Enable
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_Enable(IWDG_TypeDef *IWDGx)
{
  IO_WRITE_REG(IWDG->KR, LL_IWDG_KEY_ENABLE);
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  * @rmtoll KR           KEY           LL_IWDG_ReloadCounter
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_ReloadCounter(IWDG_TypeDef *IWDGx)
{
  IO_WRITE_REG(IWDG->KR, LL_IWDG_KEY_RELOAD);
}

/**
  * @brief  Enable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers
  * @rmtoll KR           KEY           LL_IWDG_EnableWriteAccess
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_EnableWriteAccess(IWDG_TypeDef *IWDGx)
{
  IO_WRITE_REG(IWDG->KR, LL_IWDG_KEY_WR_ACCESS_ENABLE);
}

/**
  * @brief  Disable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers
  * @rmtoll KR           KEY           LL_IWDG_DisableWriteAccess
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_DisableWriteAccess(IWDG_TypeDef *IWDGx)
{
  IO_WRITE_REG(IWDG->KR, LL_IWDG_KEY_WR_ACCESS_DISABLE);
}

/**
  * @brief  Select the prescaler of the IWDG
  * @rmtoll PR           PR            LL_IWDG_SetPrescaler
  * @param  IWDGx IWDG Instance
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_IWDG_PRESCALER_4
  *         @arg @ref LL_IWDG_PRESCALER_8
  *         @arg @ref LL_IWDG_PRESCALER_16
  *         @arg @ref LL_IWDG_PRESCALER_32
  *         @arg @ref LL_IWDG_PRESCALER_64
  *         @arg @ref LL_IWDG_PRESCALER_128
  *         @arg @ref LL_IWDG_PRESCALER_256
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_SetPrescaler(IWDG_TypeDef *IWDGx, uint32_t Prescaler)
{
  IO_WRITE_REG(IWDGx->PR, IWDG_PR_PR & Prescaler);
}

/**
  * @brief  Get the selected prescaler of the IWDG
  * @rmtoll PR           PR            LL_IWDG_GetPrescaler
  * @param  IWDGx IWDG Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_IWDG_PRESCALER_4
  *         @arg @ref LL_IWDG_PRESCALER_8
  *         @arg @ref LL_IWDG_PRESCALER_16
  *         @arg @ref LL_IWDG_PRESCALER_32
  *         @arg @ref LL_IWDG_PRESCALER_64
  *         @arg @ref LL_IWDG_PRESCALER_128
  *         @arg @ref LL_IWDG_PRESCALER_256
  */
__STATIC_INLINE uint32_t LL_IWDG_GetPrescaler(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(IO_READ_REG(IWDGx->PR));
}

/**
  * @brief  Specify the IWDG down-counter reload value
  * @rmtoll RLR          RL            LL_IWDG_SetReloadCounter
  * @param  IWDGx IWDG Instance
  * @param  Counter Value between Min_Data=0 and Max_Data=0x0FFF
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_SetReloadCounter(IWDG_TypeDef *IWDGx, uint32_t Counter)
{
  WRITE_REG(IWDGx->RLR, IWDG_RLR_RL & Counter);
}

/**
  * @brief  Get the specified IWDG down-counter reload value
  * @rmtoll RLR          RL            LL_IWDG_GetReloadCounter
  * @param  IWDGx IWDG Instance
  * @retval Value between Min_Data=0 and Max_Data=0x0FFF
  */
__STATIC_INLINE uint32_t LL_IWDG_GetReloadCounter(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(IO_READ_REG(IWDGx->RLR));
}


/**
  * @}
  */

/** @defgroup IWDG_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief  Check if flag Prescaler Value Update is set or not
  * @rmtoll SR           PVU           LL_IWDG_IsActiveFlag_PVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_PVU(IWDG_TypeDef *IWDGx)
{
  return (IO_BITS_GET(IWDGx->SR, IWDG_SR_PVU) == (IWDG_SR_PVU));
}

/**
  * @brief  Check if flag Reload Value Update is set or not
  * @rmtoll SR           RVU           LL_IWDG_IsActiveFlag_RVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_RVU(IWDG_TypeDef *IWDGx)
{
  return (IO_BITS_GET(IWDGx->SR, IWDG_SR_RVU) == (IWDG_SR_RVU));
}


/**
  * @brief  Check if all flags Prescaler, Reload & Window Value Update are reset or not
  * @rmtoll SR           PVU           LL_IWDG_IsReady\n
  *         SR           RVU           LL_IWDG_IsReady
  * @param  IWDGx IWDG Instance
  * @retval State of bits (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsReady(IWDG_TypeDef *IWDGx)
{
  return (IO_BITS_GET(IWDGx->SR, IWDG_SR_PVU | IWDG_SR_RVU) == 0U);
}

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */




#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_IWDG_H_ */
