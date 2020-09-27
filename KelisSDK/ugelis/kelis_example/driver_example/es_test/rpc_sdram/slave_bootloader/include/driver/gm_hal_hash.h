/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file             hash.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 *
 * @date          2018/03/13
 *
 * @brief          Header file of hash module.
 *
 * @note
 *    2018/03/13, terry.wu, V1.0.0
 *        Initial version.
 */


#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <hash/gm_ll_hash.h>
#include <gm_hal_dma.h>

//#ifdef CHIP_GM6721


/* Declaration of HASH handle structure */
struct HASH_Handle;


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
  * HAL Lock structures definition
  */
typedef enum
{
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED   = 0x01U
} HAL_Lock_T;

/**
  * HASH phase structure definition
  */
typedef enum
{
    HASH_PHASE_READY             = 0x01U,    /**< HASH peripheral is ready for initialization. */
    HASH_PHASE_PROCESS           = 0x02U,    /**<HASH peripheral is in processing phase */
} HASH_Phase_T;

/**
  * HASH State structure definition
  */
typedef enum
{
    HASH_STATE_RESET             = 0x00U,  /**< HASH not yet initialized or disabled  */
    HASH_STATE_READY             = 0x01U,  /**< HASH initialized and ready for use    */
    HASH_STATE_BUSY              = 0x02U,  /**< HASH internal processing is ongoing   */
    HASH_STATE_TIMEOUT           = 0x03U,  /**< HASH timeout state                    */
    HASH_STATE_ERROR             = 0x04U   /**< HASH error state                      */
} HASH_State_T;

/**
 * HASH data type structure definition
 */
typedef enum
{
    HASH_DATATYPE_32B      = HASH_CFG_DATATYPE_32BIT,          /**< 32bit data type */
    HASH_DATATYPE_16B      = HASH_CFG_DATATYPE_16BIT,          /**< 16bit data type */
    HASH_DATATYPE_8B       = HASH_CFG_DATATYPE_8BIT,           /**< 8bit data type */
    HASH_DATATYPE_1B       = HASH_CFG_DATATYPE_1BIT            /**< 1bit data type */
} HASH_DataType_T;

/**
 * HASH algorithm structure definition
 */
typedef enum
{
    HASH_ALGO_SHA1         = HASH_CFG_ALGO_SHA1,               /**< SHA-1 */
    HASH_ALGO_SHA224       = HASH_CFG_ALGO_SHA224,             /**< SHA-224 */
    HASH_ALGO_SHA256       = HASH_CFG_ALGO_SHA256,             /**< SHA-256 */
    HASH_ALGO_SHA384       = HASH_CFG_ALGO_SHA384,             /**< SHA-384 */
    HASH_ALGO_SHA512       = HASH_CFG_ALGO_SHA512              /**< SHA-512 */
} HASH_Algo_T;

/**
 * HASH algorithm mode structure definition
 */
typedef enum
{
    HASH_ALGOMODE_HASH     = 0,               /**< HASH mode */
    HASH_ALGOMODE_HMAC                        /**< HMAC mode */
} HASH_AlgoMode_T;

/**
 * HASH Event structure definition
 */
typedef enum
{
    HASH_EVENT_TRANS_DONE  = 0,              /**< DMA transfer data complete */
    HASH_EVENT_TRANS_ERR                     /**< DMA transfer data error */
} HASH_Event_T;

typedef void (*HASH_Callback_T)(struct HASH_Handle *hHash, HASH_Event_T event, unsigned long param);

/**
  * HASH Configuration Structure definition
  */
typedef struct
{
    uint32_t               dataType;         /**< 32-bit data, 16-bit data, 8-bit data or 1-bit string.
                                                                       This parameter can be a value of @ref HASH_Data_Type */
    uint32_t               keySize;          /**< The key size is used only in HMAC operation */
    uint8_t                *pKey;            /**< The key is used only in HMAC operation */
    uint32_t               algo;             /**< the algorithm of the HASH */
    uint32_t               mode;             /**< HASH or HMAC mode */
    uint8_t                mulDmaTrans;      /**< enable mul DMA trransfer or not */
    HASH_Callback_T        callback;         /**< callback to notify event from driver to application */
} HASH_Config_T;

/**
  * HASH Handle structures definition
  */
typedef struct HASH_Handle
{
    HASH_Device_T         *Instance;          /**< HASH registers base address */
    HASH_Config_T         *init;              /**< HASH required parameters       */
    uint8_t               *pHashInBuffPtr;    /**< Pointer to input buffer        */
    uint8_t               *pHashOutBuffPtr;   /**< Pointer to input buffer        */
    uint32_t              hashBuffSize;       /**< Size of buffer to be processed */
    uint32_t              hashITCount;        /**< Counter of inputed data        */
    HAL_Status_T          status;             /**< HASH peripheral status         */
    HASH_Phase_T          phase;              /**< HASH peripheral phase          */
    DMA_Handle_T          *hdmain;            /**< HASH In DMA handle parameters  */
    HAL_Lock_T            lock;               /**< HASH locking object            */
    HASH_State_T          state;              /**< HASH peripheral state          */
    HASH_Callback_T       callback;           /**< callback to notify event from driver to application */
} HASH_Handle_T;

/**
  * @brief  Initializes the HASH according to the specified parameters in the
            HASH_Config_T and creates the associated handle.
  * @param  hHash: pointer to a HASH_Handle_T structure
  * @param config: contains the configuration information for HASH module
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_Init(HASH_Handle_T *hHash, HASH_Config_T *config);

/**
  * @brief  DeInitializes the HASH peripheral.
  * @note   This API must be called before starting a new processing.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_DeInit(HASH_Handle_T *hHash);

/**
  * @brief  Initializes the HASH peripheral then enables DMA to
            control data transfer. Use HAL_HASH_Finish() to get the digest.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64/128 bytes, the padding is managed by hardware.
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_Start_DMA(HASH_Handle_T *hHash, uint8_t *pInBuffer, uint32_t size);

/**
  * @brief  Returns the computed digest.
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  pOutBuffer: Pointer to the computed digest.
  * @param  timeout: Timeout value
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_Final(HASH_Handle_T *hHash, uint8_t *pOutBuffer, uint32_t timeout);

/**
  * @brief  enable multi DMA transfer or not
  * @param  hHash: pointer to a HASH_Handle_T structure that contains
  *         the configuration information for HASH module
  * @param  flag: TRUE to enable, FALSE to disable.
  * @retval HAL status
  */
HAL_Status_T HAL_HASH_SetMulDMATrans(HASH_Handle_T *hHash, uint8_t flag);



//#endif

#ifdef __cplusplus
}
#endif

#endif
