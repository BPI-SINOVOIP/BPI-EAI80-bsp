/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          dma_dev.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/04
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/07/04, wizephen.wang, V1.0.0
 *        Initial version.
 *    2017/06/12, evan.wu, V1.0.1
 *        Support 6621.
 *    2019/04/17, chris.xie, V1.0.2
 *        update code.
 */

#ifndef _GM_LL_DMA_H_
#define _GM_LL_DMA_H_

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_dma_dev.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Base addresses of DMA Registers */
#define DMA_BASE                                DMA1_BASE

/* DMA Device Pointer */
#define DMA_DEV                                 ((DMA_Device_T*)DMA_BASE)

/* Max DMA channel number */
#define  DMA_MAX_CHANNEL                        (16U)

/* Max DMA device per channel */
#define  DMA_MAX_DEV_PER_CH                     (8)

/* Maximum DMA block size */
#define MAX_BLOCK_SIZE_LENGTH                   (0x1000000)


/** @defgroup DMA_Exported_Macros DMA Exported Macros
 * @{
 */

/********************** DMA Status Register Bitfield ************************/
#if 0
#define DMA_STATUS_IDLE                  ((uint32_t) 0x00000001)     /**< DMA idle */
#define DMA_STATUS_READING_CH_CTRL_DATA  ((uint32_t) 0x00000002)     /**< DMA reading channel controller data */
#define DMA_STATUS_READING_SRC_END_POINT ((uint32_t) 0x00000003)     /**< DMA reading source data end pointer */
#define DMA_STATUS_READING_DST_END_POINT ((uint32_t) 0x00000004)     /**< DMA reading destination data end pointer */
#define DMA_STATUS_READING_SRC_DATA      ((uint32_t) 0x00000005)     /**< DMA reading source data */
#define DMA_STATUS_READING_DST_DATA      ((uint32_t) 0x00000006)     /**< DMA reading destination data */
#define DMA_STATUS_WAITING_REQ_CLR       ((uint32_t) 0x00000007)     /**< DMA waiting for request to clear */
#define DMA_STATUS_WAITING_CH_CTRL_DATA  ((uint32_t) 0x00000008)     /**< DMA waiting channel controller data */
#define DMA_STATUS_STALLED               ((uint32_t) 0x00000009)     /**< DMA stalled */
#define DMA_STATUS_SCATTER_GATHER_TRAN   ((uint32_t) 0x0000000A)     /**< DMA peripheral scatter gather transition */
#define DMA_STATUS ((uint32_t) 0x00000010)/**< DMA status */
#endif
#define DMA_IS_ENABLE                   ((uint32_t) 0x00000000)     /**< DMA is enable */


/********************** DMA Configuration Register Bitfield ************************/
#define DMA_CFG_EN                      ((uint32_t) 0x00000001)     /**< DMA enable */


/**
 * @brief Check whether DMA is active.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define LL_DMA_DEV_ISACTIVE(DEVICE)      (IO_BIT_GET((DEVICE)->DMA_STATUS, DMA_IS_ENABLE))

/**
 * @brief Enable DMA peripheral.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define LL_DMA_DEV_ENABLE(DEVICE)        (IO_BIT_SET((DEVICE)->DMA_CFG, DMA_CFG_EN))

/**
 * @brief Disable DMA peripheral.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define LL_DMA_DEV_DISABLE(DEVICE)       (IO_BIT_CLEAR((DEVICE)->DMA_CFG, DMA_CFG_EN))

/* Check whether DMA channel x is active */
#define LL_DMA_CHANNEL_ISACTIVE(DEVICE, CHANNEL)  (IO_BIT_GET((DEVICE)->CHNL_ENABLE_SET, 1 << CHANNEL))

/* Enable DMA channel x */
#define LL_DMA_CHANNEL_ENABLE(DEVICE, CHANNEL)    (IO_BIT_SET((DEVICE)->CHNL_ENABLE_SET, 1 << CHANNEL))

/* Disable DMA channel x */
#define LL_DMA_CHANNEL_DISABLE(DEVICE, CHANNEL)   (IO_WRITE_REG((DEVICE)->CHNL_ENABLE_CLR, 1 << CHANNEL))//(IO_BIT_SET((DEVICE)->CHNL_ENABLE_CLR, 1<<CHANNEL))

/* Enable DMA channel x interrupt */
#define LL_DMA_INTERRUPT_ENABLE(DEVICE, CHANNEL)  (IO_BIT_SET((DEVICE)->IE, 1 << CHANNEL))

/* Disable DMA channel x interrupt */
#define LL_DMA_INTERRUPT_DISABLE(DEVICE, CHANNEL) (IO_BIT_CLEAR((DEVICE)->IE, 1 << CHANNEL))

/* Clear DMA transaction complete interrupt flag of channel x */
#define LL_DMA_CLEAR_TRCMPLT(DEVICE, CHANNEL)     (IO_BIT_CLEAR((DEVICE)->IS, 1 << CHANNEL))

/* Clear DMA transaction error flag of channel x */
#define LL_DMA_CLEAR_ERR(DEVICE, CHANNEL)   (IO_BIT_SET((DEVICE)->ERR_CLR, 1 << CHANNEL))

/*Primary control data struct base address*/
#define LL_DMA_CTRL_BASE_PTR(DEVICE, VALUE)   (IO_WRITE_REG((DEVICE)->CTRL_BASE_PTR, VALUE))

/*Alternate  control data struct base address*/
#define LL_DMA_ALT_CTRL_BASE_PTR(DEVICE, VALUE)   (IO_WRITE_REG((DEVICE)->ALT_CTRL_BASE_PTR, VALUE))

/*Channel request mask set : disable the channel req*/
#define LL_DMA_CHAL_REQ_MASK_SET(DEVICE, CHANNEL)  (IO_WRITE_REG((DEVICE)->CHNL_REQ_MASK_SET, 1 << CHANNEL))

/*Channel request mask clear : enable the channel req*/
#define LL_DMA_CHAL_REQ_MASK_CLR(DEVICE, CHANNEL)  (IO_WRITE_REG((DEVICE)->CHNL_REQ_MASK_CLR, CHANNEL))

/*Use the Alter data struct*/
#define LL_DMA_CHAL_PRI_ALT_SET(DEVICE, CHANNEL)  (IO_WRITE_REG((DEVICE)->CHNL_PRI_ALT_SET, 1 << CHANNEL))

/*Use the Primary data struct*/
#define LL_DMA_CHAL_PRI_ALT_CLR(DEVICE, CHANNEL)  (IO_WRITE_REG((DEVICE)->CHNL_PRI_ALT_CLR, 1 << CHANNEL))

/*Set channel priority*/
#define LL_DMA_CHAL_PRIORITY_SET(DEVICE, CHANNEL)  (IO_WRITE_REG((DEVICE)->CHNL_PRIORITY_SET, 1 << CHANNEL))

/*Clear channel priority*/
#define LL_DMA_CHAL_PRIORITY_CLR(DEVICE, CHANNEL)  (IO_WRITE_REG((DEVICE)->CHNL_PRIORITY_CLR, 1 << CHANNEL))

/*Set Channel config register0 (channel select)*/
#define LL_DMA_CHAL_CH_CFG0_CHAL(DEVICE, VALUE)  (IO_BIT_CLEAR((DEVICE)->CH_CFG0,0xf << (VALUE)))

/*Set Channel config register0 (dev select)*/
#define LL_DMA_CHAL_CH_CFG0_DEV(DEVICE, VALUE)  (IO_BIT_SET((DEVICE)->CH_CFG0, VALUE))

/*Set Channel config register1 (channel select)*/
#define LL_DMA_CHAL_CH_CFG1_CHAL(DEVICE, VALUE)  (IO_BIT_CLEAR((DEVICE)->CH_CFG1, 0xf << (VALUE)))

/*Set Channel config register1 (dev select)*/
#define LL_DMA_CHAL_CH_CFG1_DEV(DEVICE, VALUE)  (IO_BIT_SET((DEVICE)->CH_CFG1, VALUE))

/*Config the src/dst address change(channel) */
#define LL_DMA_NO_INC_CHAL_SELECT(DEVICE, VALUE)  (IO_BIT_CLEAR((DEVICE)->NO_INC, 0x03 << (VALUE)))

/*Config the src/dst address change (increment mode)*/
#define LL_DMA_NO_INC_INC_SELECT(DEVICE, VALUE)  (IO_BIT_SET((DEVICE)->NO_INC, VALUE))

/*Channel Software request create*/
#define LL_DMA_CHAL_SW_REQUEST_SET(DEVICE, CHANNEL)  (IO_BIT_SET((DEVICE)->CHNL_SW_REQUEST, 1<<CHANNEL))

/*Channel Software request don't create*/
#define LL_DMA_CHAL_SW_REQUEST_CLR(DEVICE,CHANNEL)  (IO_BIT_CLEAR((DEVICE)->CHNL_SW_REQUEST, 1<<CHANNEL))

/*PES0 Config */
#define LL_DMA_GET_PES_CFG0_BIT(DEVICE,VALUE)  (IO_BIT_GET((DEVICE)->PES_CFG0,VALUE))

/*PES1 Config */
#define LL_DMA_GET_PES_CFG1_BIT(DEVICE,VALUE)  (IO_BIT_GET((DEVICE)->PES_CFG1,VALUE))

/*PES0 Config */
#define LL_DMA_PES_CFG0(DEVICE,VALUE)  (IO_WRITE_REG((DEVICE)->PES_CFG0,VALUE))

/*PES1 Config */
#define LL_DMA_PES_CFG1(DEVICE,VALUE)  (IO_WRITE_REG((DEVICE)->PES_CFG1,VALUE))


#ifdef __cplusplus
}
#endif

#endif /*_GM_LL_DMA_H_*/



