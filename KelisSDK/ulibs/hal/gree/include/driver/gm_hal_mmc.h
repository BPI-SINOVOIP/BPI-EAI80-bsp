/*
* Copyright 2008,2010 Freescale Semiconductor, Inc
* Andy Fleming
*
* Based (loosely) on the Linux code
*
* SPDX-License-Identifier:	GPL-2.0+
*/

#ifndef _GM_HAL_MMC_H_
#define _GM_HAL_MMC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
//#include <gm_list.h>



#include <gm_hal_dma.h>
#include <mmc/host/gm_ll_mmc_dev.h>

/*Declaration of MMC handle structure*/
struct MMC_Handle_T;


typedef enum
{
    MMC_SD                                        = 0x0,
    MMC_SDIO                                    = 0x1,
    MMC_EMMC
} MMC_TYPE_T;


typedef enum
{
    MMC_BUS_WIDTH_1                     = 0x0,  /**< CAN frame type is data frame */
    MMC_BUS_WIDTH_4                     ,       /**< CAN frame type is data frame */
    MMC_BUS_WIDTH_8                     ,       /**< CAN frame type is data frame */
} MMC_BUS_WIDTH_T;

typedef enum{
    MMC_STATUS_SUCCESS               =0,
    MMC_STATUS_IDEL                          ,
    MMC_STATUS_CRC16_ERR               ,
    MMC_STATUS_CRC7_ERR                 ,
    MMC_STATUS_CMD_FINISH            ,
    MMC_STATUS_DATA_FINISH          ,
    MMC_STATUS_TIMEOUT                  ,
    MMC_STATUS_LOCK                        ,
    MMC_STATUS_BUSY                        ,
    MMC_STATUS_DAM_ERR                 ,
    MMC_STATUS_CARD_ERR                ,
    MMC_STATUS_DAM_FINISH
} MMC_TransferStatus_T;

typedef void (*MMC_Callback_T)(struct MMC_Handle_T *hMMC, uint8_t event, ulong_t param);


typedef struct
{
    MMC_BUS_WIDTH_T                       busWitch;
    MMC_TYPE_T                            type;          /**< CPU ID */
    uint8_t                               removable;
} MMC_Config_T;


typedef enum
{
    MMC_OFF_SLOT    = 0,
    MMC_IN_SLOT        ,

} MMC_SLOT_STATUS_T;


typedef struct MMC_Handle_T
{
    MMC_Device_T                          *device;            /**< Pointer to MMC device handle */
    MMC_TYPE_T                            type;
    void                                  *mmc;               /*point to abstruct mmc struct*/
    DMA_Handle_T                          TxDMAHandle;
    DMA_Handle_T                          RxDMAHandle;
    uint32_t                              IRQn;               /*point to mmc_host*/
    uint8_t                               waitDmaFinish;
    volatile uint8_t                      transferStatus;
    uint8_t                               forceClk;
    uint8_t                               removable;
    uint8_t                               wpvaild;
    MMC_SLOT_STATUS_T                     cardInSlot;

}MMC_Handle_T;


RET_CODE HAL_MMC_Init(MMC_Handle_T *hMMC,  MMC_Device_T *pMMC, MMC_Config_T *pConfig);
uint32_t HAL_MMC_Erase(MMC_Handle_T *hMMC, uint32_t start, uint32_t blkcnt);
RET_CODE HAL_MMC_Indentifier(MMC_Handle_T *hMMC);
RET_CODE HAL_MMC_IsActiveStatus(MMC_Handle_T *hMMC);
uint32_t HAL_MMC_Read(MMC_Handle_T *hMMC, uint32_t start, uint32_t blkcnt, uint8_t *buf);
uint32_t HAL_MMC_Write(MMC_Handle_T *hMMC, uint32_t start, uint32_t blkcnt, const uint8_t *buf);
uint64_t HAL_MMC_GetCapcity(MMC_Handle_T *hMMC);
MMC_SLOT_STATUS_T HAL_MMC_GetCd(MMC_Handle_T *hMMC);
void HAL_MMC_SetCd (MMC_Handle_T *hMMC, MMC_SLOT_STATUS_T status);

#ifndef USE_HAL_TEST
 void HAL_MMC_IRQHandler(uint32_t param);
#endif


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_MMC_H_ */

