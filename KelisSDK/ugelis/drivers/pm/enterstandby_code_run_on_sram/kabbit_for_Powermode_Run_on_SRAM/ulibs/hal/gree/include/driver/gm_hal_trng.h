/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file             trng.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 *
 * @date          2018/03/26
 *
 * @brief          Header file of trng module.
 *
 * @note
 *    2018/03/26, terry.wu, V1.0.0
 *        Initial version.
 */

#ifndef _TRNG_H_
#define _TRNG_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <trng/gm_ll_trng.h>

//#ifdef CHIP_GM6721


/* Declaration of TRNG handle structure */
struct TRNG_Handle;

/**
  * HAL Status structures definition
  */
typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_Status_T;


/**
  * TRNG State Structure definition
  */
typedef enum
{
    TRNG_STATE_RESET     = 0x00U,               /**< TRNG not yet initialized or disabled */
    TRNG_STATE_READY     = 0x01U,               /**< TRNG initialized and ready for use   */
    TRNG_STATE_BUSY      = 0x02U,               /**< TRNG internal process is ongoing     */
    TRNG_STATE_TIMEOUT   = 0x03U,               /**< TRNG timeout state                   */
    TRNG_STATE_ERROR     = 0x04U                /**< TRNG error state                     */
} TRNG_State_T;

/**
 * TRNG callback event type structure
 */
typedef enum
{
    TRNG_EVENT_DATA_VALID        = 0x0,     /**< random number is available */
    TRNG_EVENT_ERROR                        /**< there may be Von Neumann error/CRNGT error/Autocorrelation error happen */
} TRNG_Event_T;

/**
 * TRNG error type structure
 */
typedef enum
{
    TRNG_ERR_AUTOCORR            = TRNG_ISR_AUTOCORRERR,   /**< Autocorrelation error */
    TRNG_ERR_CRNGT               = TRNG_ISR_CRNGTERR,      /**< CRNGT error*/
    TRNG_ERR_VN                  = TRNG_ISR_VNERR          /**< Von Neumann error */
} TRNG_Error_T;

typedef void (*TRNG_Callback_T)(struct TRNG_Handle *hTrng, TRNG_Event_T event, unsigned long param);

/**
  * TRNG Handle Structure definition
  */
typedef struct TRNG_Handle
{
    TRNG_Device_T                *Instance;          /**< Register base address   */
    lock_t                       lock;              /**< TRNG locking object      */
    TRNG_State_T                 state;             /**< TRNG communication state */
    uint32_t                     randomNumber[6];   /**< Last Generated TRNG Data */
    uint8_t                      size;              /**< the valid size of random number */
    uint32_t                     priority;          /**< interrupt priority */
    TRNG_Callback_T              callback;          /**< driver indicate which event happen */
} TRNG_Handle_T;


/**
  * @brief  Initializes the TRNG peripheral and creates the associated handle.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_Init(TRNG_Handle_T *hTrng);

/**
  * @brief  DeInitializes the TRNG peripheral.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_DeInit(TRNG_Handle_T *hTrng);

/**
  * @brief  Generates  random number, maximum size is 192bit.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @param  random: pointer to generated random number variable if successful.
  * @param  size: the len of random to get
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_GenerateRandomNumber(TRNG_Handle_T *hTrng, uint32_t *random, uint8_t size);

/**
  * @brief  Generates  random number, maximum size is 192bit with interrupt mode.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_GenerateRandomNumber_IT(TRNG_Handle_T *hTrng);

/**
  * @brief  Read latest generated random number.
  * @param  hTrng: pointer to a TRNG_Handle_T structure that contains
  *                the configuration information for TRNG.
  * @param  random: pointer to generated random number variable if successful.
  * @param  size: the len of random to get
  * @retval HAL status
  */
HAL_Status_T HAL_TRNG_ReadLastRandomNumber(TRNG_Handle_T *hTrng, uint32_t *random, uint8_t size);


//#endif

#ifdef __cplusplus
}
#endif

#endif
