/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_iwdg.h
 *
 * @author        kenny.cmt
 *
 * @version       1.0
 *
 * @date          2018/03/23
 *
 * @brief         Header file of iwdg driver.
 *
 * @note
 *    2018/03/21, kenny.cmt, V1.0
 *        Initial version.
 */
#ifndef _GM_HAL_IWDG_H_
#define _GM_HAL_IWDG_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <gm_common.h>
#include <iwdg/gm_ll_iwdg_dev.h>

/** @addtogroup GM67XX_HAL_Driver
  * @{
  */

/** @addtogroup IWDG
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup IWDG_Exported_Types IWDG Exported Types
  * @{
  */

#define HAL_IWDG_CLKSRC     32000  /*FPGA:SET 1MHZ,the chip :set 32khz*/
/**
  * @brief  IWDG Init structure definition
  */
typedef struct
{
    uint32_t Prescaler;  /*!< Select the prescaler of the IWDG.
                            This parameter can be a value of @ref IWDG_Prescaler */

    uint32_t Reload;     /*!< Specifies the IWDG down-counter reload value.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 0x0FFF */

} IWDG_InitTypeDef;

/**
  * @brief  IWDG Handle Structure definition
  */
typedef struct
{
    IWDG_TypeDef                 *Instance;  /*!< Register base address    */

    IWDG_InitTypeDef             Init;       /*!< IWDG required parameters */

} IWDG_HandleTypeDef;


/*
 * IWDG CLK division factor
 * IWDG = 32K/(IWDG_CLK_DIV);
 */
typedef enum
{
    HAL_IWDG_CLKDIV_FACTOR_4,
    HAL_IWDG_CLKDIV_FACTOR_8,
    HAL_IWDG_CLKDIV_FACTOR_16,
    HAL_IWDG_CLKDIV_FACTOR_32,
    HAL_IWDG_CLKDIV_FACTOR_64,
    HAL_IWDG_CLKDIV_FACTOR_128,
    HAL_IWDG_CLKDIV_FACTOR_256,
    HAL_IWDG_CLKDIV_FACTOR256
} HAL_IWDG_Clkdiv;



/* Exported constants --------------------------------------------------------*/
/** @defgroup IWDG_Exported_Constants IWDG Exported Constants
  * @{
  */

/** @defgroup IWDG_Prescaler IWDG Prescaler
  * @{
  */
#define IWDG_PRESCALER_4                0x00000000U                   /*!< IWDG prescaler set to 4   */
#define IWDG_PRESCALER_8                IWDG_PR_PR_0                  /*!< IWDG prescaler set to 8   */
#define IWDG_PRESCALER_16               IWDG_PR_PR_1                  /*!< IWDG prescaler set to 16  */
#define IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0) /*!< IWDG prescaler set to 32  */
#define IWDG_PRESCALER_64               IWDG_PR_PR_2                  /*!< IWDG prescaler set to 64  */
#define IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0) /*!< IWDG prescaler set to 128 */
#define IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1) /*!< IWDG prescaler set to 256 */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup IWDG_Exported_Macros IWDG Exported Macros
  * @{
  */

/**
  * @brief  Enable the IWDG peripheral.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
//#define __HAL_IWDG_START(__HANDLE__)                WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_ENABLE)
RET_CODE HAL_IWDG_START(IWDG_HandleTypeDef *hiwdg);

/**
  * @brief  Reload IWDG counter with value defined in the reload register
  *         (write access to IWDG_PR & IWDG_RLR registers disabled).
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
//#define __HAL_IWDG_RELOAD_COUNTER(__HANDLE__)       WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_RELOAD)
RET_CODE HAL_IWDG_RELOAD_COUNTER(IWDG_HandleTypeDef *hiwdg);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IWDG_Exported_Functions  IWDG Exported Functions
  * @{
  */

/** @defgroup IWDG_Exported_Functions_Group1 Initialization and Start functions
  * @{
  */
/* Initialization/Start functions  ********************************************/
RET_CODE HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg);
/**
  * @}
  */

/** @defgroup IWDG_Exported_Functions_Group2 IO operation functions
  * @{
  */
/* I/O operation functions ****************************************************/
RET_CODE HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg);
/**
  * @}
  */

/** @defgroup Get the selected prescaler of the IWD
  * @{
  */
uint32_t HAL_IWDG_GetPrescaler(IWDG_HandleTypeDef *hiwdg);
/**
  * @}
  */

/** @defgroup Get the specified IWDG down-counter reload value
  * @{
  */
uint32_t HAL_IWDG_GetReloadCounter(IWDG_HandleTypeDef *hiwdg);
/**
  * @}
  */

/** @defgroup Reset backup domain
  * @{
  */
void HAL_IWDG_BKD_SW_Reset(void);
/**
  * @}
  */


/** @defgroup  accord to divider value ,confirm to prescaler value
 * @{
 */
uint32_t HAL_IWDG_Get_Div_Value(uint32_t prescaler);
/**
  * @}
  */


/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup IWDG_Private_Constants IWDG Private Constants
  * @{
  */

/**
  * @brief  IWDG Key Register BitMask
  */
#define IWDG_KEY_RELOAD                 0x0000AAAAU  /*!< IWDG Reload Counter Enable   */
#define IWDG_KEY_ENABLE                 0x0000CCCCU  /*!< IWDG Peripheral Enable       */
#define IWDG_KEY_WRITE_ACCESS_ENABLE    0x00005555U  /*!< IWDG KR Write Access Enable  */
#define IWDG_KEY_WRITE_ACCESS_DISABLE   0x00000000U  /*!< IWDG KR Write Access Disable */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup IWDG_Private_Macros IWDG Private Macros
  * @{
  */

/**
  * @brief  Enable write access to IWDG_PR and IWDG_RLR registers.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
#define IWDG_ENABLE_WRITE_ACCESS(__HANDLE__)  WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_WRITE_ACCESS_ENABLE)

/**
  * @brief  Disable write access to IWDG_PR and IWDG_RLR registers.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
#define IWDG_DISABLE_WRITE_ACCESS(__HANDLE__) WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_WRITE_ACCESS_DISABLE)

/**
  * @brief  Check IWDG prescaler value.
  * @param  __PRESCALER__  IWDG prescaler value
  * @retval None
  */
#define IS_IWDG_PRESCALER(__PRESCALER__)      (((__PRESCALER__) == IWDG_PRESCALER_4)  || \
        ((__PRESCALER__) == IWDG_PRESCALER_8)  || \
        ((__PRESCALER__) == IWDG_PRESCALER_16) || \
        ((__PRESCALER__) == IWDG_PRESCALER_32) || \
        ((__PRESCALER__) == IWDG_PRESCALER_64) || \
        ((__PRESCALER__) == IWDG_PRESCALER_128)|| \
        ((__PRESCALER__) == IWDG_PRESCALER_256))

/**
  * @brief  Check IWDG reload value.
  * @param  __RELOAD__  IWDG reload value
  * @retval None
  */
#define IS_IWDG_RELOAD(__RELOAD__)            ((__RELOAD__) <= IWDG_RLR_RL)

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
#endif /* #ifndef _GM_HAL_UART_H_ */



