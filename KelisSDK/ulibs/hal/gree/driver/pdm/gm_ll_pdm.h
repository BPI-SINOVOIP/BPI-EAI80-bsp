/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file           gm_ll_pdm.h
 *
 * @author      Robert_hu
 *
 * @version     v1.0.0
 *
 * @date        2019/11/01
 *
 * @brief       PDM Source file
 *
 * @note
 *              2019/11/01, Robert_hu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_LL_PDM_H_
    #define _GM_LL_PDM_H_
#endif
#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_pdm_dev.h"
#include "gm_hal_dma.h"
#include "gm_hal_pdm.h"
#include <gm_soc.h>
#include <gm_basic_types.h>

/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
/**
 * @}
 */

#if (defined(CHIP_F6721B))

#define PDM_SW_RSTJ                                 (0xf << 20)

#define PDM_CLK_EN                                  (0xf << 20)



/*************** PDM xConfiguration register 1 bitfiled ******************/

#define PDM_MCK_ENABLE_POS                          ((uint32_t) 20)   /* PDM mck enable  pos */
#define PDM_MCK_ENABLE                              ((uint32_t) ((uint32_t)0x01 << PDM_MCK_ENABLE_POS)) /* PDM mck enable filed */

#define PDM_BCK_ENABLE_POS                          ((uint32_t) 19)   /* PDM bck enable  pos */
#define PDM_BCK_ENABLE                              ((uint32_t) ((uint32_t)0x01 << PDM_BCK_ENABLE_POS)) /* PDM bck enable filed */

#define PDM_DMA_ENABLE_POS                          ((uint32_t) 17)   /* PDM DMA enable  pos */
#define PDM_DMA_ENABLE                              ((uint32_t) ((uint32_t)0x01 << PDM_DMA_ENABLE_POS)) /* PDM DMA enable filed */

#define PDM_ENABLE_POS                              ((uint32_t) 16)   /* PDM enable  pos */
#define PDM_ENABLE                                  ((uint32_t) ((uint32_t)0x01 << PDM_ENABLE_POS)) /* PDM ENABLE filed */

#define PDM_DATASIZE_POS                            ((uint32_t) 5)   /* PDM DATASIZE  pos */
#define PDM_DATASIZE_8                              ((uint32_t) ((uint32_t)0x02 << PDM_DATASIZE_POS)) /* pdm datasize 8bit  filed */
#define PDM_DATASIZE_10                             ((uint32_t) ((uint32_t)0x03 << PDM_DATASIZE_POS)) /* pdm datasize 10bit  filed */
#define PDM_DATASIZE_16                             ((uint32_t) ((uint32_t)0x04 << PDM_DATASIZE_POS)) /* pdm datasize  16bit  filed */
#define PDM_DATASIZE_20                             ((uint32_t) ((uint32_t)0x05 << PDM_DATASIZE_POS)) /* pdm datasize 20bit  filed */
#define PDM_DATASIZE_24                             ((uint32_t) ((uint32_t)0x06 << PDM_DATASIZE_POS)) /* pdm datasize 24bit  filed */
#define PDM_DATASIZE_32                             ((uint32_t) ((uint32_t)0x07 << PDM_DATASIZE_POS)) /* pdm datasize 32bit  filed */

/*************** PDM xConfiguration register 2 bitfiled ******************/

#define PDM_FIFO_FIUSH_POS                          ((uint32_t) 3)   /* PDM fifo flush  pos */
#define PDM_FIFO_FIUSH_EN                           ((uint32_t) ((uint32_t)0x01 << PDM_FIFO_FIUSH_POS))  /* PDM fifo flush enable filed */

#define PDM_FIFO_THRESHOLD_POS                      ((uint32_t) 0)   /* PDM fifo threshold pos */
#define PDM_FIFO_THRESHOLD_EMPTY                    ((uint32_t) ((uint32_t)0x00 << PDM_FIFO_THRESHOLD_POS)) /* PDM fifo threshold empty pos*/
#define PDM_FIFO_THRESHOLD_1                        ((uint32_t) ((uint32_t)0x01 << PDM_FIFO_THRESHOLD_POS)) /* PDM fifo threshold 1/4 pos*/
#define PDM_FIFO_THRESHOLD_2                        ((uint32_t) ((uint32_t)0x02 << PDM_FIFO_THRESHOLD_POS)) /* PDM fifo threshold 1/2 pos*/
#define PDM_FIFO_THRESHOLD_3                        ((uint32_t) ((uint32_t)0x03 << PDM_FIFO_THRESHOLD_POS)) /* PDM fifo threshold 3/4 pos*/
#define PDM_FIFO_THRESHOLD_FULL                     ((uint32_t) ((uint32_t)0x04 << PDM_FIFO_THRESHOLD_POS)) /* PDM fifo threshold full pos*/

/*************** PDM xSlot register bitfiled ******************/

#define PDM_SLOT_ENABLE_POS                         ((uint32_t) 16) /* PDM slot enable pos */
#define PDM_SLOT_ENABLE                             ((uint32_t) ((uint32_t)0xFFFF << PDM_SLOT_ENABLE_POS)) /* PDM slot enable filed */

#define PDM_NB_SLOT_POS                             ((uint32_t) 8) /* PDM number of slots  pos */
#define PDM_NB_SLOT                                 ((uint32_t) ((uint32_t)0xf << PDM_NB_SLOT_POS)) /* PDM number slot all filed */
#define PDM_NB_SLOT_1                               ((uint32_t) ((uint32_t)0x0 << PDM_NB_SLOT_POS)) /* PDM number slot 1 filed */
#define PDM_NB_SLOT_2                               ((uint32_t) ((uint32_t)0x1 << PDM_NB_SLOT_POS)) /* PDM number slot 2 filed */

#define PDM_SLOT_SIZE_POS                           ((uint32_t) 6) /* PDM slot size  pos */
#define PDM_SLOTSZ                                  ((uint32_t) ((uint32_t)0x3 << PDM_SLOT_SIZE_POS)) /* PDM  slot size all filed */
#define PDM_SLOTSZ_16                               ((uint32_t) ((uint32_t)0x1 << PDM_SLOT_SIZE_POS)) /* PDM  slot size16bit filed */
#define PDM_SLOTSZ_32                               ((uint32_t) ((uint32_t)0x2 << PDM_SLOT_SIZE_POS)) /* PDM  slot size 32bit filed */

/*************** PDM xIM register bitfiled ******************/

#define PDM_FREQ_POS                                ((uint32_t) 3) /* PDM FIFO overflow interrupt enable bit pos */
#define PDM_FTH_FREQ_ENABLE                         ((uint32_t) ((uint32_t)0x01 << PDM_FREQ_POS)) /* PDM FIFO overflow interrupt enable bit filed */

#define PDM_OVRUDRIE_POS                            ((uint32_t) 0) /* PDM number of slots  pos */
#define PDM_OVRUDRIE_ENABLE                         ((uint32_t) ((uint32_t)0x1 << PDM_OVRUDRIE_POS)) /* PDM number slot all filed */


/*************** PDM xStatus registerr bitfiled ******************/

#define PDM_FIFO_LEVEL_THR_POS                      ((uint32_t) 16) /* PDM fifo level threshold pos  */
#define PDM_FIFO_LEVEL_THR_empty                    ((uint32_t) ((uint32_t)0x0 << PDM_FIFO_LEVEL_THR_POS))/* PDM fifo level threshold empty filed */
#define PDM_FIFO_LEVEL_THR_1                        ((uint32_t) ((uint32_t)0x1 << PDM_FIFO_LEVEL_THR_POS))/* PDM fifo level threshold 1/4 filed */
#define PDM_FIFO_LEVEL_THR_2                        ((uint32_t) ((uint32_t)0x2 << PDM_FIFO_LEVEL_THR_POS))/* PDM fifo level threshold 1/2 filed */
#define PDM_FIFO_LEVEL_THR_3                        ((uint32_t) ((uint32_t)0x3 << PDM_FIFO_LEVEL_THR_POS))/* PDM fifo level threshold 3/4 filed */
#define PDM_FIFO_LEVEL_THR_4                        ((uint32_t) ((uint32_t)0x4 << PDM_FIFO_LEVEL_THR_POS))/* PDM fifo level threshold not full filed */
#define PDM_FIFO_LEVEL_THR_full                     ((uint32_t) ((uint32_t)0x5 << PDM_FIFO_LEVEL_THR_POS))/* PDM fifo level threshold full filed */

#define PDM_FIFO_REQUEST_POS                        ((uint32_t) 3) /* PDM fifo request enable pos */
#define PDM_FIFO_REQUEST_ENABLE                     ((uint32_t) ((uint32_t)0x01 << PDM_FIFO_REQUEST_POS)) /* PDM fifo request enable filed */

/*************** PDM xClear flag register  bitfiled ******************/

#define PDM_CLFSDET_POS                             ((uint32_t) 6) /* PDM clear late frame synchronization detection flag*/
#define PDM_CLFSDET_EN                              ((uint32_t) ((uint32_t)0x01 << PDM_CLFSDET_POS))  /* PDM clear late frame synchronization detection filed */

#define PDM_CAFSDET_POS                             ((uint32_t) 5) /* PDM  Clear anticipated frame synchronization detection  flag*/s
#define PDM_CAFSDET_EN                              ((uint32_t) ((uint32_t)0x01 << PDM_CAFSDET_POS))  /* PDM  Clear anticipated frame synchronization detection filed */

#define PDM_CCNRDY_POS                              ((uint32_t) 4)  /*PDM Clear codec not ready flag pos */
#define PDM_CCNRDY_EN                               ((uint32_t) ((uint32_t)0x01 << PDM_CCNRDY_POS))  /*PDM Clear codec not ready flag filed */

#define PDM_CWCKCFG_POS                             ((uint32_t) 2)  /*PDM Clear wrong clock configuration flag  pos */
#define PDM_CWCKCFG_EN                              ((uint32_t) ((uint32_t)0x01 << PDM_CWCKCFG_POS))  /*PDM Clear wrong clock configuration flag filed */

#define PDM_CMUTEDET_POS                            ((uint32_t) 1)  /*PDM Mute detection flag pos */
#define PDM_CMUTEDET_EN                             ((uint32_t) ((uint32_t)0x01 << PDM_CMUTEDET_POS))  /*PDM Mute detection flag filed */

#define PDM_COVRUDR_POS                             ((uint32_t) 0)  /*PDM Clear overrun / underrun pos */
#define PDM_COVRUDR_EN                              ((uint32_t) ((uint32_t)0x01 << PDM_COVRUDR_POS))  /*PDM Clear overrun / underrun filed */

/*************** PDM xClock Control register  bitfiled ******************/

#define PDM_BCKDIV_POS                              ((uint32_t) 0) /* PDM bckdiv pos */
#define PDM_BCKDIV_MAX                              ((uint32_t) ((uint32_t)0xFF << PDM_BCKDIV_POS)) /* PDM bckdiv filed */

/***************PDM xPDM   bitfiled ******************/

#define PDM_CID_ENABLE_POS                          ((uint32_t) 18) /* PDM CID_ENABLE pos */
#define PDM_CID_ENABLE                              ((uint32_t) ((uint32_t)0x01 << PDM_CID_ENABLE_POS)) /* PDM CID_ENABLE filed*/

#define PDM_CAPT_ENABLE_POS                         ((uint32_t) 17) /* PDM CAPT_ENABLE pos */
#define PDM_CAPT_ENABLE                             ((uint32_t) ((uint32_t)0x01 << PDM_CAPT_ENABLE_POS)) /* PDM CAPT_ENABLE filed*/

#define PDM_CHANNEL_FLAG_POS                        ((uint32_t) 16) /* PDM CHANNEL_FLAG pos */
#define PDM_CHANNEL_LEFT                            ((uint32_t) ((uint32_t)0x0 << PDM_CHANNEL_FLAG_POS)) /* PDM channel left flag filed*/
#define PDM_CHANNEL_RIGHT                           ((uint32_t) ((uint32_t)0x1 << PDM_CHANNEL_FLAG_POS)) /* PDM channel right flag filed*/

#define PDM_DUAL_CH_POS                             ((uint32_t) 14) /* PDM DUAL_CH pos */
#define PDM_DUAL_CH                                 ((uint32_t) ((uint32_t)0x3 << PDM_DUAL_CH_POS)) /* PDM DUAL_CH filed*/
#define PDM_DUAL_CH_LEFT                            ((uint32_t) ((uint32_t)0x2 << PDM_DUAL_CH_POS)) /* PDM DUAL_CH left  filed*/
#define PDM_DUAL_CH_RIGHT                           ((uint32_t) ((uint32_t)0x1 << PDM_DUAL_CH_POS)) /* PDM DUAL_CH right filed*/

#define PDM_DECIMATION_POS                          ((uint32_t) 4) /* PDM Select the trim bits of CIC filter pos */
#define PDM_DECIMATION                              ((uint32_t) ((uint32_t)0x3FF << PDM_DECIMATION_POS)) /* PDM Select the trim bits of CIC filter filed*/

#define PDM_SHIFT_POS                               ((uint32_t) 0) /* PDM Select the number of interceptions required pos */
#define PDM_SHIFT_1                                 ((uint32_t) ((uint32_t)0x1 << PDM_SHIFT_POS)) /* PDM Select the number of interceptions required filed*/
#define PDM_SHIFT_2                                 ((uint32_t) ((uint32_t)0x2 << PDM_SHIFT_POS)) /* PDM Select the number of interceptions required filed*/
#define PDM_SHIFT_4                                 ((uint32_t) ((uint32_t)0x4 << PDM_SHIFT_POS)) /* PDM Select the number of interceptions required filed*/
#define PDM_SHIFT_8                                 ((uint32_t) ((uint32_t)0x8 << PDM_SHIFT_POS)) /* PDM Select the number of interceptions required filed*/


#define PDM_SHIFT_LEVEL_1      PDM_SHIFT_1
#define PDM_SHIFT_LEVEL_2      PDM_SHIFT_2
#define PDM_SHIFT_LEVEL_3      (PDM_SHIFT_1 | PDM_SHIFT_2)
#define PDM_SHIFT_LEVEL_4      PDM_SHIFT_4
#define PDM_SHIFT_LEVEL_5      (PDM_SHIFT_1 | PDM_SHIFT_4)
#define PDM_SHIFT_LEVEL_6      (PDM_SHIFT_2 | PDM_SHIFT_4)
#define PDM_SHIFT_LEVEL_7      (PDM_SHIFT_1 | PDM_SHIFT_2 | PDM_SHIFT_4)
#define PDM_SHIFT_LEVEL_8      PDM_SHIFT_8
#define PDM_SHIFT_LEVEL_9      (PDM_SHIFT_1 | PDM_SHIFT_8)
#define PDM_SHIFT_LEVEL_10     (PDM_SHIFT_2 | PDM_SHIFT_8)
#define PDM_SHIFT_LEVEL_11     (PDM_SHIFT_1 | PDM_SHIFT_2 | PDM_SHIFT_4)
#define PDM_SHIFT_LEVEL_12     (PDM_SHIFT_4 | PDM_SHIFT_8)


#define PDM_FTH_EMPTY    PDM_FIFO_THRESHOLD_EMPTY
#define PDM_FTH_1        PDM_FIFO_THRESHOLD_1
#define PDM_FTH_2        PDM_FIFO_THRESHOLD_2
#define PDM_FTH_3        PDM_FIFO_THRESHOLD_3
#define PDM_FTH_FULL     PDM_FIFO_THRESHOLD_FULL

/**
 * @brief Disable pdm
 *
 * @param pPDM               PDM devive point
 *
 * @retval                      None
 */
#define PDM_DISABLE(pPDM)                          IO_BIT_CLEAR(pPDM->CFG1, PDM_ENABLE)

/**
 * @brief Enable PDM slot
 *
 * @param pPDM                PDM devive point
 * @param valule                enable or disable
 *
 * @retval                      None
 */
#define _PDM_SLOT_ENABLE(pPDM)                      IO_BIT_SET(pPDM->SLOT, PDM_SLOT_ENABLE)
#define _PDM_SLOT_DISABLE(pPDM)                     IO_BIT_CLEAR(pPDM->SLOT, PDM_SLOT_ENABLE)

/**
 * @brief Enable PDM DMA inversion
 *
 * @param pPDM                 PDM device point
 *
 * @retval                      None
 */
#define _PDM_DMA_ENABLE(pPDM)                      IO_BIT_SET(pPDM->CFG1, PDM_DMA_ENABLE)
#define _PDM_DMA_DISABLE(pPDM)                     IO_BIT_CLEAR(pPDM->CFG1, PDM_DMA_ENABLE)
/**
 * @brief Enable pdm mck
 *
 * @param  pPDM               PDM device point
 *
 * @retval                      None
 */
#define _PDM_MCK_ENABLE(pPDM)                      IO_BIT_SET(pPDM->CFG1, PDM_MCK_ENABLE)
#define _PDM_MCK_DISABLE(pPDM)                     IO_BIT_CLEAR(pPDM->CFG1, PDM_MCK_ENABLE)

/**
 * @brief Enable pdm bck
 *
 * @param  pPDM               PDM device point
 *
 * @retval                      None
 */
#define _PDM_BCK_ENABLE(pPDM)                      IO_BIT_SET(pPDM->CFG1, PDM_BCK_ENABLE)
#define _PDM_BCK_DISABLE(pPDM)                     IO_BIT_CLEAR(pPDM->CFG1, PDM_BCK_ENABLE)

/**
 * @brief Enable PDM device
 *
 * @param pPDM                PDM device point
 *
 * @retval                      None
 */
#define _PDM_ENABLE(pPDM)                          IO_BITS_SET(pPDM->CFG1, PDM_ENABLE)

/**
 *  PDM events
 */
typedef enum
{
    PDM_RECEIVE_EVENT              = 0x00,             /**< When pdm receive data completed which return the event to ur */
    PDM_RECEIVE_EVENT_ERROR,
    PDM_DMAMST_RECEIVE_EVENT,
    PDM_DMASLV_RECEIVE_EVENT,
} PDM_Event_T;


/*
 * PDM peripheral state
 */
typedef enum
{
    PDM_STATE_IDLE       = 0,  /**< PDM is in idle state */
    PDM_STATE_READY,           /**< PDM is ready to use */
    PDM_STATE_MASTER_RX,       /**< Master receives data to slave */
} PDM_State_T;
/*
 * PDM CH_FLAG
 */
typedef enum
{
    PDM_CH_LEFT       = 0,
    PDM_CH_RIGHT,
} PDM_CH_FLAG_T;
/*
 * PDM BCK
 */
typedef enum
{
    PDM_BCK_256K  = 187,
    PDM_BCK_512K  = 93,
    PDM_BCK_1M    = 47,
    PDM_BCK_1_5M  = 31,
    PDM_BCK_2M    = 23,
    PDM_BCK_3M    = 15,
} PDM_BCK_T;
/*
 * PDM FS
 */

typedef enum
{
    PDM_FS_8K   = 8000U,
    PDM_FS_11K  = 11025U,
    PDM_FS_16K  = 16000U,
    PDM_FS_22K  = 22050U,
    PDM_FS_32K  = 32000U,
    PDM_FS_44K  = 44100U,
    PDM_FS_48K  = 48000U,
    PDM_FS_96K  = 96000U,
    PDM_FS_192K = 192000U,
} PDM_FS_T;
/*
 * PDM Dual_Ch
 */

typedef enum
{
    PDM_Dual_LR = 0,   /**<double channel*/
    PDM_Dual_R,        /**<right channel  */
    PDM_Dual_L,        /**<left channel*/
} PDM_Dual_Ch_T;

/**
 * PDM error code type
 */
typedef enum
{
    PDM_ERRCODE_NOERROR               = 0x00,  /**< No error */
    PDM_ERRCODE_TIMEOUT,                       /**< Timeout */
    PDM_ERRCODE_DMAERROR,                      /**< Error caused by DMA */
    PDM_ERRCODE_NOTREADY,                      /**< PDMis not ready */
    PDM_ERRCODE_RECEIVE,                       /**< PDM receive error */
} PDM_ErrorCode_T;
/**
* PDM working mode
*/
typedef enum
{
    PDM_MASTER_MODE   = 0x00,  /**< Master mode */
    PDM_SLAVE_MODE    = 0x01   /**< Slave mode */
} PDM_WorkMode_T;

typedef enum
{
    HAL_PDM_OK       = 0x00U,
    HAL_PDM_ERROR    = 0x01U,
    HAL_PDM_BUSY     = 0x02U,
    HAL_PDM_TIMEOUT  = 0x03U,
    HAL_PDM_INVPARAM = 0x04U
} HAL_StatusTypeDef;

/**
 * ACMP user handle structure.
 */

struct PDM_Handle_T;

typedef void (*PDM_Callback_T)(DMA_Handle_T *hDMA, PDM_Event_T event, unsigned long Param);
typedef void (*PDM_Callback_INT_T)(struct PDM_Handle_T *pPDM, PDM_Event_T event, unsigned long param);

typedef struct
{
    uint32_t                    DataSize;    /**<PDM receive data of size */
    uint8_t                     Fifo_Thr;    /**<PDM  FIFO threshold  */
    uint8_t                     Cid;         /**<PDM  cid  */
    uint8_t                     Capt;        /**<PDM  capt */
    uint8_t                     CH_FLAG;     /**<PDM  CH_FLAG */
    uint16_t                    DECIMATION;  /**<PDM  DECIMATION */
    uint16_t                    Shift_Level; /**<PDM  Shift_Level */
    uint32_t                    bck;         /**<PDM  bck */
    uint32_t                    bckdiv;      /**<PDM  bckdiv */
    uint32_t                     NBSlot;      /**<PDM  Number of slots in an audio frame  */
    uint8_t                     SlotSize;    /**<PDM  Slot size in an audio frame   */
    uint32_t                    FS;          /**<PDM  receive data of frequency */
    uint8_t                     Dual_Ch;     /**<PDM  receive data of channel */
    uint32_t                    *pdmaBuffer;/**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                                                             is the direction, must be 16Bit aligned by user app */
    uint32_t                    dmaBufferLength;  /**< DMA buffer length*/
    uint8_t           enableDMA;    /**< PDM DMA usage option */
    PDM_Callback_T               callback;    /**< DMA tranfers Driver send usr event to call this function */
    PDM_Callback_INT_T           pdm_callback;    /**< PDM interrupt mode callback  to call this function */
    uint32_t         param; /* for user cb */
} PDM_Config_T;

typedef struct PDM_Handle
{
    PDM_Device_T      *device;                       /**< Pointer to pdm device */
    PDM_Config_T      config;
    DMA_Handle_T      *hDMA      ;   /**< DMA device handler */
    uint16_t          rxLength;                      /**< Data length for receiving */
    PDM_State_T       state;                         /**< PDM data-receive  state */
    PDM_ErrorCode_T  ErrorCode;                      /**< PDM error code */
    PDM_Callback_INT_T           pdm_callback;    /**< PDM interrupt mode callback  to call this function */
    uint8_t          enableDMA;    /**< PDM DMA usage option */
    uint32_t         param; /* for user cb */
} PDM_Handle_T;


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_LL_ACMP_H_ */

