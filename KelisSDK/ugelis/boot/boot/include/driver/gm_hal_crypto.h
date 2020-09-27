/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file             crypto.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date          2018/02/26 
 *    
 * @brief          Header file of crypto engine module.
 * 
 * @note
 *    2018/02/26, terry.wu, V1.0.0
 *        Initial version.
 */

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <crypto/gm_ll_crypto.h>
#include <gm_hal_dma.h>

//#ifdef CHIP_GM6721

/* Declaration of TRNG handle structure */
struct CRYP_Handle;

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
  * CRYP phase structure definition
  */
typedef enum
{
	CRYP_PHASE_READY             = 0x01U,    /**< CRYP peripheral is ready for initialization. */
	CRYP_PHASE_PROCESS           = 0x02U,    /**< CRYP peripheral is in processing phase */
	CRYP_PHASE_FINAL             = 0x03U     /**< CRYP peripheral is in final phase
					                                                This is relevant only with CCM and GCM modes */
}CRYP_Phase_T;

/**
  * CRYP State structure definition
  */
typedef enum
{
	CRYP_STATE_RESET             = 0x00U,  /**< CRYP not yet initialized or disabled  */
	CRYP_STATE_READY             = 0x01U,  /**< CRYP initialized and ready for use    */
	CRYP_STATE_BUSY              = 0x02U,  /**< CRYP internal processing is ongoing   */
	CRYP_STATE_TIMEOUT           = 0x03U,  /**< CRYP timeout state                    */
	CRYP_STATE_ERROR             = 0x04U   /**< CRYP error state                      */
}CRYP_State_T;


/**
 * CRYP key size structure definition
 */
typedef enum
{
	CRYP_KEYSIZE_128B      = CRYP_CR_KEYSIZE_128BIT,          /**< 128bit key length */
	CRYP_KEYSIZE_192B      = CRYP_CR_KEYSIZE_192BIT,          /**< 192bit key length */
	CRYP_KEYSIZE_256B      = CRYP_CR_KEYSIZE_256BIT           /**< 256bit key length */
} CRYP_KeySize_T;

/**
 * InitVector/InitCounter size structure definition
 */
typedef enum
{
	CRYP_VECTSIZE_64B      = 0,          /**< 64bit init vector size */
	CRYP_VECTSIZE_128B                   /**< 128bit init vector size */ 
}CRYP_VectSize_T;

/**
 * CRYP data type structure definition
 */
typedef enum
{
	CRYP_DATATYPE_32B      = CRYP_CR_DATATYPE_32BIT,          /**< 32bit data type */
	CRYP_DATATYPE_16B      = CRYP_CR_DATATYPE_16BIT,          /**< 16bit data type */
	CRYP_DATATYPE_8B       = CRYP_CR_DATATYPE_8BIT,           /**< 8bit data type */
	CRYP_DATATYPE_1B       = CRYP_CR_DATATYPE_1BIT            /**< 1bit data type */
} CRYP_DataType_T;

/**
 * CRYP algorithm mode structure definition
 */
typedef enum
{
	CRYP_ALGOMODE_ECB      = CRYP_CR_ALGOMODE_ECB,          /**< electronic codebook mode */
	CRYP_ALGOMODE_CBC      = CRYP_CR_ALGOMODE_CBC,          /**< cipher block chaining */
	CRYP_ALGOMODE_CTR      = CRYP_CR_ALGOMODE_CTR,          /**< counter mode */
	CRYP_ALGOMODE_GCM      = CRYP_CR_ALGOMODE_GCM,          /**< Galois Counter mode */
	CRYP_ALGOMODE_CCM      = CRYP_CR_ALGOMODE_CCM           /**< counter with CBC */
}CRYP_ALGOMODE_T;

/**
 * CRYP event structure definition
 */
typedef enum
{
	CRYP_EVENT_INFIFO_TRANS_DONE = 0,                       /**< infifo transfer complete */
	CRYP_EVENT_OUTFIFO_TRANS_DONE                           /**< outfifo transfer complete */
}CRYP_Event_T;

typedef void (*CRYP_Callback_T)(struct CRYP_Handle * hCryp, CRYP_Event_T event, unsigned long param);

/**
  * CRYP Configuration Structure definition
  */
typedef struct
{
	uint32_t  priority;                 /**< the interrupt priority of CRYP */
	uint32_t  algoMode;                  /**< the algorithm mode of the CRYP */
	uint32_t  dataType;                 /**< 32-bit data, 16-bit data, 8-bit data or 1-bit string.
	                                                          This parameter can be a value of @ref CRYP_Data_Type */
	uint32_t  keySize;                  /**< Used only in AES mode only : 128, 192 or 256 bit key length. 
	                                                          This parameter can be a value of @ref CRYP_Key_Size */
	uint8_t * pKey;                     /**< The key used for encryption/decryption */
	uint8_t * pInitVect;                /**< The initialization vector used also as initialization
	                                                          counter in CTR mode */
	uint8_t   IVSize;                   /**< The size of initialization vector. This parameter (called nonce size in CCM)
                                                                 is used only in AES-128/192/256 encryption/decryption CCM mode */
	uint8_t   tagSize;                  /**< The size of returned authentication TAG. This parameter is used only in
                                                                 AES-128/192/256 encryption/decryption CCM mode */
	uint8_t * header;                   /**< The header used in GCM and CCM modes */
	uint32_t  headerSize;               /**< The size of header buffer in bytes, must be a multiple of 16 */
	uint8_t * pScratch;                 /**< Scratch buffer used to append the header. It's size must be equal to header size + 21 bytes.
	                                                          This parameter is used only in AES-128/192/256 encryption/decryption CCM mode */
	CRYP_Callback_T callback;           /**< callback to notify event from driver to application */
} CRYP_Config_T;

/**
  * CRYP handle Structure definition
  */   
typedef struct CRYP_Handle
{
	CRYPTO_Device_T         *Instance;		  /**< CRYP registers base address */
	CRYP_Config_T           *init;			  /**< CRYP required parameters */
	uint8_t                 *pCrypInBuffPtr;  /**< Pointer to CRYP processing (encryption, decryption,...) buffer */
	uint8_t                 *pCrypOutBuffPtr; /**< Pointer to CRYP processing (encryption, decryption,...) buffer */
	uint16_t                crypInCount;	  /**< Counter of inputed data */
	uint16_t                crypOutCount;	  /**< Counter of output data */
	HAL_Status_T            status; 		  /**< CRYP peripheral status */
	CRYP_Phase_T            phase;			  /**< CRYP peripheral phase */
	DMA_Handle_T            *hdmain;		  /**< CRYP In DMA handle parameters */
	DMA_Handle_T            *hdmaout;		  /**< CRYP Out DMA handle parameters */
	HAL_Lock_T              lock;			  /**< CRYP locking object */
	CRYP_State_T            state;			  /**< CRYP peripheral state */
	CRYP_Callback_T         callback;         /**< callback to notify event from driver to application */
} CRYP_Handle_T;


/**
  * @brief  Initializes the CRYP according to the specified
  *         parameters in the CRYP_Config_T.
  * @param  hCryp: pointer to a CRYP_Handle_T structure. 
  * @param config: pointer to a CRYP_Config_T structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_Init(CRYP_Handle_T * hCryp, CRYP_Config_T * config);

/**
  * @brief  DeInitializes the CRYP peripheral. 
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DeInit(CRYP_Handle_T * hCryp);

/**
  * @brief  Computes the authentication TAG.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  size: Total length of the plain/cyphertext buffer
  * @param  authTag: Pointer to the authentication buffer
  * @param  timeout: Timeout duration
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AESGCM_Final(CRYP_Handle_T * hCryp, uint32_t size, uint8_t * authTag, uint32_t timeout);

/**
  * @brief  Computes the authentication TAG for AES CCM mode.
  * @param  hcryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  authTag: Pointer to the authentication buffer
  * @param  timeout: Timeout duration
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AESCCM_Final(CRYP_Handle_T * hCryp, uint8_t * authTag, uint32_t timeout);

/**
  * @brief  Initializes the CRYP peripheral in AES encryption mode using Interrupt.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AES_Encrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint16_t size, uint8_t * pCypherData);

/**
  * @brief  Initializes the CRYP peripheral in AES decryption mode using Interrupt.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  size: Length of the cyphertext buffer, must be a multiple of 16.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_AES_Decrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint16_t size, uint8_t * pPlainData);

/**
  * @brief  Initializes the CRYP peripheral in DES encryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DES_Encrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint16_t size, uint8_t * pCypherData);

/**
  * @brief  Initializes the CRYP peripheral in DES decryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the cyphertext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_DES_Decrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint16_t size, uint8_t * pPlainData);

/**
  * @brief  Initializes the CRYP peripheral in TDES encryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_TDES_Encrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pPlainData, uint16_t size, uint8_t * pCypherData);

/**
  * @brief  Initializes the CRYP peripheral in TDES decryption mode using IT.
  * @param  hCryp: pointer to a CRYP_Handle_T structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  size: Length of the cyphertext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_Status_T HAL_CRYP_TDES_Decrypt_IT(CRYP_Handle_T * hCryp, uint8_t * pCypherData, uint16_t size, uint8_t * pPlainData);




//#endif

#ifdef __cplusplus
}
#endif

#endif
