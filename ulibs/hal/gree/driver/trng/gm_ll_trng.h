/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file             trng_dev.h
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

#ifndef _TRNG_DEV_H_
#define _TRNG_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif



#include <gm_common.h>
#include <gm_soc.h>
#include <gm_basic_types.h>
#ifndef RABBIT_NO_OS_SYSTEM
#include <misc/printk.h>
#endif

//#ifdef CHIP_GM6721


/*-----------------------
 * Print macros
 *-----------------------*/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
#define PR_DEBUG(...)       do {} while (0)
#else
#define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
#define PR_ERR(...)        do {} while (0)
#else
#define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
#define PR_WARNINR(...)    do{} while (0)
#else
#define PR_WARNINR         printf
#endif
 

/**
 * @brief True random number generator
 */
typedef struct
{
	__IO uint32_t VER;                   /**< TRNG version register, address offset: 0x00 */
	__IO uint32_t reserved1[63];
	__IO uint32_t IMR;                   /**< Interrupt Mask Register, address offset: 0x100 */
	__IO uint32_t ISR;                   /**< Interrupt Status Register, address offset: 0x104 */
	__IO uint32_t ICR;                   /**< Interrupt Clear Register, address offset: 0x108 */
	__IO uint32_t CFG;                   /**< TRNG Configuration Register, address offset: 0x10C */
	__IO uint32_t VALID;                 /**< TRNG Valid register, address offset: 0x110 */
	__IO uint32_t DATA[6];               /**< Entropy Holding Register, offset: 0x114 */
	__IO uint32_t SRCEN;                 /**< Random Source Enable Register, offset: 0x12C */
	__IO uint32_t SAMCNT;                /**< Sample Count Register, offset: 0x130 */
	__IO uint32_t AUTOCORR;              /**< Autocorrelation Register, offset: 0x134 */
	__IO uint32_t DBG;                   /**< Debug Control Register, offset: 0x138 */
	__IO uint32_t reserved2;
	__IO uint32_t RST;                   /**< Reset Register, offset: 0x140 */
	__IO uint32_t reserved3[29];
	__IO uint32_t BUSY;                  /**< Busy Register, offset: 0x1B8 */
	__IO uint32_t RST_BITS_CNT;          /**< Reset Bits Counter Register, 0x1BC */
	__IO uint32_t reserved4[8];
	__IO uint32_t BIST_CNTR[3];          /**< BIST Counter Register, offset: 0x1E0 ~0x1E8*/
}TRNG_Device_T;


/** TRNG device */
#define TRNG               ((TRNG_Device_T *)(TRNG_BASE))

/******************* Bits definition for IMR register  ********************/
#define TRNG_IMR_EHRVALID_Pos                  (0U)
#define TRNG_IMR_EHRVALID_Msk                  (0x1 << TRNG_IMR_EHRVALID_Pos)
#define TRNG_IMR_EHRVALID_INT                  TRNG_IMR_EHRVALID_Msk

#define TRNG_IMR_AUTOCORRERR_Pos               (1U)
#define TRNG_IMR_AUTOCORRERR_Msk               (0x1 << TRNG_IMR_AUTOCORRERR_Pos)
#define TRNG_IMR_AUTOCORRERR_INT               TRNG_IMR_AUTOCORRERR_Msk

#define TRNG_IMR_CRNGTERR_Pos                  (2U)
#define TRNG_IMR_CRNGTERR_Msk                  (0x1 << TRNG_IMR_CRNGTERR_Pos)
#define TRNG_IMR_CRNGTERR_INT                  TRNG_IMR_CRNGTERR_Msk

#define TRNG_IMR_VNERR_Pos                     (3U)
#define TRNG_IMR_VNERR_Msk                     (0x1 << TRNG_IMR_VNERR_Pos)
#define TRNG_IMR_VNERR_INT                     TRNG_IMR_VNERR_Msk

/******************* Bits definition for ISR register  ********************/
#define TRNG_ISR_EHRVALID_Pos                  (0U)
#define TRNG_ISR_EHRVALID_Msk                  (0x1 << TRNG_ISR_EHRVALID_Pos)
#define TRNG_ISR_EHRVALID                      TRNG_ISR_EHRVALID_Msk

#define TRNG_ISR_AUTOCORRERR_Pos               (1U)
#define TRNG_ISR_AUTOCORRERR_Msk               (0x1 << TRNG_ISR_AUTOCORRERR_Pos)
#define TRNG_ISR_AUTOCORRERR                   TRNG_ISR_AUTOCORRERR_Msk

#define TRNG_ISR_CRNGTERR_Pos                  (2U)
#define TRNG_ISR_CRNGTERR_Msk                  (0x1 << TRNG_ISR_CRNGTERR_Pos)
#define TRNG_ISR_CRNGTERR                      TRNG_ISR_CRNGTERR_Msk

#define TRNG_ISR_VNERR_Pos                     (3U)
#define TRNG_ISR_VNERR_Msk                     (0x1 << TRNG_ISR_VNERR_Pos)
#define TRNG_ISR_VNERR                         TRNG_ISR_VNERR_Msk

/******************* Bits definition for ICR register  ********************/
#define TRNG_ICR_EHRVALID_Pos                  (0U)
#define TRNG_ICR_EHRVALID_Msk                  (0x1 << TRNG_ICR_EHRVALID_Pos)
#define TRNG_ICR_EHRVALID_CLR                  TRNG_ICR_EHRVALID_Msk

#define TRNG_ICR_AUTOCORRERR_Pos               (1U)
#define TRNG_ICR_AUTOCORRERR_Msk               (0x1 << TRNG_ICR_AUTOCORRERR_Pos)
#define TRNG_ICR_AUTOCORRERR_CLR               TRNG_ICR_AUTOCORRERR_Msk

#define TRNG_ICR_CRNGTERR_Pos                  (2U)
#define TRNG_ICR_CRNGTERR_Msk                  (0x1 << TRNG_ICR_CRNGTERR_Pos)
#define TRNG_ICR_CRNGTERR_CLR                  TRNG_ICR_CRNGTERR_Msk

#define TRNG_ICR_VNERR_Pos                     (3U)
#define TRNG_ICR_VNERR_Msk                     (0x1 << TRNG_ICR_VNERR_Pos)
#define TRNG_ICR_VNERR_CLR                     TRNG_ICR_VNERR_Msk

/******************* Bits definition for CFG register  ********************/
#define TRNG_CFG_RNDSRC_Pos                    (0U)
#define TRNG_CFG_RNDSRC_Msk                    (0x3 << TRNG_CFG_RNDSRC_Pos)
#define TRNG_CFG_RNDSRC_SHORTEST               (0x00000000)
#define TRNG_CFG_RNDSRC_SHORT                  (0x01 << TRNG_CFG_RNDSRC_Pos)
#define TRNG_CFG_RNDSRC_LONG                   (0x02 << TRNG_CFG_RNDSRC_Pos)
#define TRNG_CFG_RNDSRC_LONGEST                (0x03 << TRNG_CFG_RNDSRC_Pos)

/******************* Bits definition for VALID register  ********************/
#define TRNG_VALID_EHRVALID_Pos                (0U)
#define TRNG_VALID_EHRVALID_Msk                (0x1 << TRNG_VALID_EHRVALID_Pos)
#define TRNG_VALID_EHRVALID                    TRNG_VALID_EHRVALID_Msk

/******************* Bits definition for SRCEN register  ********************/
#define TRNG_SRCEN_RNDSRCEN_Pos                (0U)
#define TRNG_SRCEN_RNDSRCEN_Msk                (0x1 << TRNG_SRCEN_RNDSRCEN_Pos)
#define TRNG_SRNEN_RNDSRC_EN                   TRNG_SRCEN_RNDSRCEN_Msk

/******************* Bits definition for DBG register  ********************/
#define TRNG_DBG_VNCBYPASS_Pos                 (1U)
#define TRNG_DBG_VNCBYPASS_Msk                 (0x1 << TRNG_DBG_VNCBYPASS_Pos)
#define TRNG_DBG_VNCBYPASS                     TRNG_DBG_VNCBYPASS_Msk

#define TRNG_DBG_CRNGTBYPASS_Pos               (2U)
#define TRNG_DBG_CRNGTBYPASS_Msk               (0x1 << TRNG_DBG_CRNGTBYPASS_Pos)
#define TRNG_DBG_CRNGTBYPASS                   TRNG_DBG_CRNGTBYPASS_Msk

#define TRNG_DBG_AUTOCORRBYPASS_Pos            (3U)
#define TRNG_DBG_AUTOCORRBYPASS_Msk            (0x1 << TRNG_DBG_AUTOCORRBYPASS_Pos)
#define TRNG_DBG_AUTOCORRBYPASS                TRNG_DBG_AUTOCORRBYPASS_Msk

/******************* Bits definition for RST register  ********************/
#define TRNG_RST_SOFTRESET_Pos                 (0U)
#define TRNG_RST_SOFTRESET_Msk                 (0x1 << TRNG_RST_SOFTRESET_Pos)
#define TRNG_RST_SOFTRESET                     TRNG_RST_SOFTRESET_Msk

/******************* Bits definition for BUSY register  ********************/
#define TRNG_BUSY_STATUS_Pos                   (0U)
#define TRNG_BUSY_STATUS_Msk                   (0x1 << TRNG_BUSY_STATUS_Pos)
#define TRNG_BUSY_STATUS_BUSY                  TRNG_BUSY_STATUS_Msk

/******************* Bits definition for RST_BITS_CNT register  ********************/
#define TRNG_RST_BITS_CNT_Pos                  (0U)
#define TRNG_RST_BITS_CNT_Msk                  (0x1 << TRNG_RST_BITS_CNT_Pos)
#define TRNG_RST_BITS_CNT                      TRNG_RST_BITS_CNT_Msk


/**
  * @brief  Enables the TRNG peripheral.
  * @param  __HANDLE__: TRNG Handle
  * @retval None
  */
#define HAL_TRNG_ENABLE(__HANDLE__) (IO_BITMASK_SET((__HANDLE__)->Instance->SRCEN, TRNG_SRCEN_RNDSRCEN_Msk, TRNG_SRNEN_RNDSRC_EN))

/**
  * @brief  Disables the TRNG peripheral.
  * @param  __HANDLE__: TRNG Handle
  * @retval None
  */
#define HAL_TRNG_DISABLE(__HANDLE__) (IO_BIT_CLEAR((__HANDLE__)->Instance->SRCEN, TRNG_SRCEN_RNDSRCEN_Msk))


/**
  * @brief  Enables the TRNG interrupts.
  * @param  __HANDLE__: TRNG Handle
  * @param irg: the interrupt to be enable
  * @retval None
  */
#define HAL_TRNG_ENABLE_IT(__HANDLE__, irq) (IO_BIT_CLEAR((__HANDLE__)->Instance->IMR, irq))

/**
  * @brief  Disables the TRNG interrupts.
  * @param  __HANDLE__: TRNG Handle
  * @param irg: the interrupt to be disable
  * @retval None
  */
#define HAL_TRNG_DISABLE_IT(__HANDLE__, irq) (IO_BIT_SET((__HANDLE__)->Instance->IMR, irq))


//#endif

#ifdef __cplusplus
}
#endif

#endif
