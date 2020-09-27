/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        TerryLee
 *
 * @version       1.0.0
 *
 * @date          2018/04/03
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *        porting to ugelis system.
 */
#ifndef _GM_LL_MMC_HOST_H_
#define _GM_LL_MMC_HOST_H_

#include <gm_common.h>
#include <gm_soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USE_MMC_HAL_TEST                        1 /*for fpga test*/



#define  HIRC_CLK                                           8000000
#define  IIS_CLK_48MHZ                                48000000


#define MMC_HOST_FRE_MIN                        400000
#define MMC_HOST_MAX_BLK_COUNT                  0x100      /*todo*/

#define MMC_HOST_DAM_ADDRESS                    (MMC0_BASE+0x2c)

typedef enum{
    MMC_HOST_CMD_BC          = 0,
    MMC_HOST_CMD_BCR         = 1,
    MMC_HOST_CMD_AC          = 2,
    MMC_HOST_CMD_RD          = 4,
    MMC_HOST_CMD_WR          = 5,
}MMC_CmdType_T;

typedef enum{
    MMC_HOST_BUS_1BIT       = 0 ,
    MMC_HOST_BUS_4BIT           ,
    MMC_HOST_BUS_8BIT           ,/*NOT USE*/
}MMC_BusWidth_T;


typedef enum{
    MMC_RES_R0      =0,
    MMC_RES_R1_R1B    ,
    MMC_RES_R2        ,
    MMC_RES_R3_R4     ,
    MMC_RES_R5_R6_R7  ,
}MMC_ResType_T;


/*************** MMC CTRL register bitfiled ****************/
#define MMC_MMCCTRL_CLK_POS                      ((uint32_t) 24)
#define MMC_MMCCTRL_CLK_MASK                     ((uint32_t) ((uint32_t)0xff << MMC_MMCCTRL_CLK_POS))
#define MMC_MMCCTRL_DIR_POS                      ((uint32_t) 23)
#define MMC_MMCCTRL_DIR_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_DIR_POS))
#define MMC_MMCCTRL_IE_POS                       ((uint32_t) 22)
#define MMC_MMCCTRL_IE_MASK                      ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_IE_POS))
#define MMC_MMCCTRL_CMDINDEX_POS                 ((uint32_t) 16)
#define MMC_MMCCTRL_CMDINDEX_MASK                ((uint32_t) ((uint32_t)0x3f << MMC_MMCCTRL_CMDINDEX_POS))
#define MMC_MMCCTRL_STD3_POS                     ((uint32_t) 15)
#define MMC_MMCCTRL_STD3_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_STD3_POS))
#define MMC_MMCCTRL_STD0_POS                     ((uint32_t) 14)
#define MMC_MMCCTRL_STD0_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_STD0_POS))
#define MMC_MMCCTRL_STD2_POS                     ((uint32_t) 13)
#define MMC_MMCCTRL_STD2_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_STD2_POS))
#define MMC_MMCCTRL_STD_POS                      ((uint32_t) 12)
#define MMC_MMCCTRL_STD_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_STD_POS))
#define MMC_MMCCTRL_STCMD_POS                    ((uint32_t) 11)
#define MMC_MMCCTRL_STCMD_MASK                   ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_STCMD_POS))
#define MMC_MMCCTRL_STD1_POS                     ((uint32_t) 10)
#define MMC_MMCCTRL_STD1_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_STD1_POS))
#define MMC_MMCCTRL_CRC16_POS                    ((uint32_t) 9)
#define MMC_MMCCTRL_CRC16_MASK                   ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_CRC16_POS))
#define MMC_MMCCTRL_CRC7_POS                     ((uint32_t) 8)
#define MMC_MMCCTRL_CRC7_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_CRC7_POS))
#define MMC_MMCCTRL_BWC_POS                      ((uint32_t) 7)
#define MMC_MMCCTRL_BWC_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_BWC_POS))
#define MMC_MMCCTRL_CMDRTC_POS                   ((uint32_t) 4)
#define MMC_MMCCTRL_CMDRTC_MASK                  ((uint32_t) ((uint32_t)0x07 << MMC_MMCCTRL_CMDRTC_POS))
#define MMC_MMCCTRL_CMDTC_POS                    ((uint32_t) 1)
#define MMC_MMCCTRL_CMDTC_MASK                   ((uint32_t) ((uint32_t)0x07 << MMC_MMCCTRL_CMDTC_POS))
#define MMC_MMCCTRL_CMDSAT_POS                   ((uint32_t) 0)
#define MMC_MMCCTRL_CMDSAT_MASK                  ((uint32_t) ((uint32_t)0x01 << MMC_MMCCTRL_CMDSAT_POS))

/*************** MMC DBL register bitfiled ****************/

#define MMC_DBL_CLKFEN_POS                       ((uint32_t) 24)
#define MMC_DBL_CLKFEN_MASK                      ((uint32_t) ((uint32_t)0x01 << MMC_DBL_CLKFEN_POS))
#define MMC_DBL_BLKNUM_POS                       ((uint32_t) 16)
#define MMC_DBL_BLKNUM_MASK                      ((uint32_t) ((uint32_t)0xff << MMC_DBL_BLKNUM_POS))
#define MMC_DBL_BLKLENG_POS                      ((uint32_t) 0)
#define MMC_DBL_BLKLENG_MASK                     ((uint32_t) ((uint32_t)0xffff << MMC_DBL_BLKLENG_POS))

/*************** MMC PIO register bitfiled ****************/
#define MMC_PIO_EN_POS                           ((uint32_t) 18)
#define MMC_PIO_EN_MASK                          ((uint32_t) ((uint32_t)0x01 << MMC_PIO_EN_POS))
#define MMC_PIO_ST_POS                           ((uint32_t) 17)
#define MMC_PIO_ST_MASK                          ((uint32_t) ((uint32_t)0x01 << MMC_PIO_ST_POS))
#define MMC_PIO_DCLE_POS                         ((uint32_t) 16)
#define MMC_PIO_DCLE_MASK                        ((uint32_t) ((uint32_t)0x01 << MMC_PIO_DCLE_POS))
#define MMC_PIO_DA_POS                           ((uint32_t) 0)
#define MMC_PIO_DA_MASK                          ((uint32_t) ((uint32_t)0xffff << MMC_PIO_DA_POS))


/*************** MMC SDIOCTRL register bitfiled ****************/
#define MMC_SDIOCTRL_IS_POS                      ((uint32_t) 8)
#define MMC_SDIOCTRL_IS_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_IS_POS))
#define MMC_SDIOCTRL_STOP_POS                    ((uint32_t) 5)
#define MMC_SDIOCTRL_STOP_MASK                   ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_STOP_POS))
#define MMC_SDIOCTRL_RDEN_POS                    ((uint32_t) 4)
#define MMC_SDIOCTRL_RDEN_MASK                   ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_RDEN_POS))
#define MMC_SDIOCTRL_IE_POS                      ((uint32_t) 3)
#define MMC_SDIOCTRL_IE_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_IE_POS))
#define MMC_SDIOCTRL_8B_POS                      ((uint32_t) 2)
#define MMC_SDIOCTRL_8B_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_8B_POS))
#define MMC_SDIOCTRL_1B_POS                      ((uint32_t) 1)
#define MMC_SDIOCTRL_1B_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_1B_POS))
#define MMC_SDIOCTRL_4B_POS                      ((uint32_t) 0)
#define MMC_SDIOCTRL_4B_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_SDIOCTRL_4B_POS))

/*************** MMC Interrupt Staus register bitfiled ****************/
#define MMC_IS_SDIO_POS                          ((uint32_t) 4)
#define MMC_IS_SDIO_MASK                         ((uint32_t) ((uint32_t)0x01 << MMC_IS_SDIO_POS))
#define MMC_IS_CRC7_ERR_POS                      ((uint32_t) 3)
#define MMC_IS_CRC7_ERR_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_IS_CRC7_ERR_POS))
#define MMC_IS_CRC16_ERR_POS                     ((uint32_t) 2)
#define MMC_IS_CRC16_ERR_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_IS_CRC16_ERR_POS))
#define MMC_IS_DATA_END_POS                      ((uint32_t) 1)
#define MMC_IS_DATA_END_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_IS_DATA_END_POS))
#define MMC_IS_CMD_END_POS                       ((uint32_t) 0)
#define MMC_IS_CMD_END_MASK                      ((uint32_t) ((uint32_t)0x01 << MMC_IS_CMD_END_POS))

/*************** MMC Interrupt Enable register bitfiled ****************/
#define MMC_IE_SDIO_POS                         ((uint32_t) 4)
#define MMC_IE_SDIO_MASK                        ((uint32_t) ((uint32_t)0x01 << MMC_IE_SDIO_POS))
#define MMC_IE_CRC7_ERR_POS                     ((uint32_t) 3)
#define MMC_IE_CRC7_ERR_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_IE_CRC7_ERR_POS))
#define MMC_IE_CRC16_ERR_POS                    ((uint32_t) 2)
#define MMC_IE_CRC16_ERR_MASK                   ((uint32_t) ((uint32_t)0x01 << MMC_IE_CRC16_ERR_POS))
#define MMC_IE_DATA_END_POS                     ((uint32_t) 1)
#define MMC_IE_DATA_END_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_IE_DATA_END_POS))
#define MMC_IE_CMD_END_POS                      ((uint32_t) 0)
#define MMC_IE_CMD_END_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_IE_CMD_END_POS))
//#define MMC_IE_MASK_ALL                         (MMC_IE_SDIO_MASK|MMC_IE_CRC7_ERR_MASK|MMC_IE_CRC16_ERR_MASK|MMC_IE_CMD_END_MASK|MMC_IE_DATA_END_MASK)
#define MMC_IE_MASK_ALL                         (MMC_IE_CRC7_ERR_MASK|MMC_IE_CRC16_ERR_MASK|MMC_IE_CMD_END_MASK|MMC_IE_DATA_END_MASK)

/*************** MMC DMACTRL register bitfiled ****************/
#define MMC_DMACTRL_ST_POS                       ((uint32_t) 4)
#define MMC_DMACTRL_ST_MASK                      ((uint32_t) ((uint32_t)0x01 << MMC_DMACTRL_ST_POS))
#define MMC_DMACTRL_TXST_POS                     ((uint32_t) 3)
#define MMC_DMACTRL_TXST_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_DMACTRL_TXST_POS))
#define MMC_DMACTRL_RXST_POS                     ((uint32_t) 2)
#define MMC_DMACTRL_RXST_MASK                    ((uint32_t) ((uint32_t)0x01 << MMC_DMACTRL_RXST_POS))
#define MMC_DMACTRL_DIR_POS                      ((uint32_t) 1)
#define MMC_DMACTRL_DIR_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_DMACTRL_DIR_POS))
#define MMC_DMACTRL_STA_POS                      ((uint32_t) 0)
#define MMC_DMACTRL_STA_MASK                     ((uint32_t) ((uint32_t)0x01 << MMC_DMACTRL_STA_POS))


/*************** MMC ADITLCTRL register bitfiled ****************/
#define MMC_ADITLCTRL_DAEXN_POS                 ((uint32_t) 16)
#define MMC_ADITLCTRL_DAEXN_MASK                ((uint32_t) ((uint32_t)0xff << MMC_ADITLCTRL_DAEXN_POS))
#define MMC_ADITLCTRL_BUSEXDCLK_POS             ((uint32_t) 0)
#define MMC_ADITLCTRL_BUSEXDCLK_MASK            ((uint32_t) ((uint32_t)0xff << MMC_ADITLCTRL_BUSEXDCLK_POS))

/*************** MMC DLYCHN register bitfiled ****************/
#define MMC_DLYCHN_OUTPUT_POS                   ((uint32_t) 8)
#define MMC_DLYCHN_OUTPUT_MASK                  ((uint32_t) ((uint32_t)0x0f << MMC_DLYCHN_OUTPUT_POS))
#define MMC_DLYCHN_INPUT_POS                    ((uint32_t) 0)
#define MMC_DLYCHN_INPUT_MASK                   ((uint32_t) ((uint32_t)0x0f << MMC_DLYCHN_INPUT_POS))


#define LL_MMC_SET_BUS_CLK(pDev,val)            IO_BITMASK_SET(pDev->MMCCTRL, MMC_MMCCTRL_CLK_MASK,val<<MMC_MMCCTRL_CLK_POS)


#define LL_MMC_SET_DATA_DIR_WR(pDev)            IO_BIT_SET(pDev->MMCCTRL, 1<<MMC_MMCCTRL_DIR_POS)
#define LL_MMC_SET_DATA_DIR_RD(pDev)            IO_BIT_CLEAR(pDev->MMCCTRL, 1<<MMC_MMCCTRL_DIR_POS)

#define LL_MMC_IE_EN(pDev,val)                  IO_BIT_SET(pDev->MMCCTRL, MMC_MMCCTRL_IE_MASK)
#define LL_MMC_IE_DIS(pDev,val)                 IO_BIT_CLEAR(pDev->MMCCTRL, MMC_MMCCTRL_IE_MASK)
#define LL_MMC_SET_CMDINDEX(pDev,val)           IO_BITMASK_SET(pDev->MMCCTRL, MMC_MMCCTRL_CMDINDEX_MASK,val<<MMC_MMCCTRL_CMDINDEX_POS)
#define LL_MMC_GET_ST_DA3(pDev)                 (!!IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_STD3_MASK))
#define LL_MMC_GET_ST_DA2(pDev)                 (!!IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_STD2_MASK))
#define LL_MMC_GET_ST_DA1(pDev)                 (!!IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_STD1_MASK))
#define LL_MMC_GET_ST_DA0(pDev)                 (!!IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_STD0_MASK))
#define LL_MMC_DATA_BUSY(pDev)                  (!!IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_STD_MASK))
#define LL_MMC_CMD_BUSY(pDev)                   (!!IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_STCMD_MASK))
#define LL_MMC_CRC16_ERR(pDev)                  IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_CRC16_MASK)
#define LL_MMC_CRC7_ERR(pDev)                   IO_BITS_GET(pDev->MMCCTRL,MMC_MMCCTRL_CRC7_MASK)
#define LL_MMC_SET_BUS_WIDTH(pDev,val)          IO_BITMASK_SET(pDev->MMCCTRL,MMC_MMCCTRL_BWC_MASK,val<<MMC_MMCCTRL_BWC_POS)
#define LL_MMC_SET_RESP_TYPE(pDev,val)          IO_BITMASK_SET(pDev->MMCCTRL,MMC_MMCCTRL_CMDRTC_MASK,val<<MMC_MMCCTRL_CMDRTC_POS)
#define LL_MMC_SET_CMD_TYPE(pDev,val)           IO_BITMASK_SET(pDev->MMCCTRL,MMC_MMCCTRL_CMDTC_MASK,val<<MMC_MMCCTRL_CMDTC_POS)
#define LL_MMC_CMD_START(pDev)                  IO_BIT_SET(pDev->MMCCTRL, MMC_MMCCTRL_CMDSAT_MASK)

#define LL_MMC_CLK_ALWAYS_EN(pDev)              IO_BIT_SET(pDev->DBL, MMC_DBL_CLKFEN_MASK)
#define LL_MMC_CLK_ALWAYS_DIS(pDev)             IO_BIT_CLEAR(pDev->DBL, MMC_DBL_CLKFEN_MASK)

#define LL_MMC_CLK_HARDWARE_CTR(pDev)           IO_BIT_CLEAR(pDev->DBL, MMC_DBL_CLKFEN_MASK)
#define LL_MMC_SET_BLK_NUM(pDev,val)            IO_BITMASK_SET(pDev->DBL, MMC_DBL_BLKNUM_MASK,((val-1)<<MMC_DBL_BLKNUM_POS))
#define LL_MMC_SET_BLK_LENGTH(pDev,val)         IO_BITMASK_SET(pDev->DBL, MMC_DBL_BLKLENG_MASK,(val)<<MMC_DBL_BLKLENG_POS)

#define LL_MMC_PIO_MODE_EN(pDev,val)            IO_BITMASK_SET(pDev->PIO, MMC_PIO_EN_MASK,(val)<<MMC_PIO_EN_POS))
#define LL_MMC_PIO_GET_DATA_ST(pDev)            IO_BITMASK_SET(pDev->PIO, MMC_PIO_ST_MASK)
#define LL_MMC_PIO_CLEAR_DATA(pDev)             IO_BIT_SET(pDev->PIO, MMC_PIO_DCLE_MASK)
#define LL_MMC_PIO_SET_DATA(pDev,val)           IO_BITMASK_SET(pDev->PIO, MMC_PIO_DA_MASK,(val)<<MMC_PIO_DA_POS)


#define LL_MMC_SDIOCTRL_IRQ_PEND(pDev)          IO_BITS_GET(pDev->SDIOCTRL, MMC_SDIOCTRL_IS_MASK)
#define LL_MMC_SDIOCTRL_SDIO_STOP(pDev,val)     IO_BITMASK_SET(pDev->SDIOCTRL, MMC_SDIOCTRL_STOP_MASK, (val)<<MMC_SDIOCTRL_STOP_POS)
#define LL_MMC_SDIOCTRL_RD_WAIT_EN(pDev,val)    IO_BITMASK_SET(pDev->SDIOCTRL, MMC_SDIOCTRL_RDEN_MASK, (val)<<MMC_SDIOCTRL_RDEN_POS)
#define LL_MMC_SDIOCTRL_IRQ_EN(pDev,val)        IO_BIT_SET(pDev->SDIOCTRL, MMC_SDIOCTRL_IE_MASK)
#define LL_MMC_SDIOCTRL_IRQ_DIS(pDev,val)       IO_BIT_CLEAR(pDev->SDIOCTRL, MMC_SDIOCTRL_IE_MASK)
#define LL_MMC_SDIOCTRL_8BIT_EN(pDev,val)       IO_BITMASK_SET(pDev->SDIOCTRL, MMC_SDIOCTRL_8B_MASK,(val)<<MMC_SDIOCTRL_8B_POS)
#define LL_MMC_SDIOCTRL_1BIT_EN(pDev,val)       IO_BITMASK_SET(pDev->SDIOCTRL, MMC_SDIOCTRL_1B_MASK,(val)<<MMC_SDIOCTRL_1B_POS)
#define LL_MMC_SDIOCTRL_4BIT_EN(pDev,val)       IO_BITMASK_SET(pDev->SDIOCTRL, MMC_SDIOCTRL_4B_MASK,(val)<<MMC_SDIOCTRL_4B_POS)


#define LL_MMC_DMACTRL_BUSY(pDev)               IO_BIT_GET(pDev->DMACTRL,MMC_DMACTRL_ST_MASK)
#define LL_MMC_DMACTRL_TX_DONE(pDev)            IO_BIT_GET(pDev->DMACTRL,MMC_DMACTRL_TXST_MASK)
#define LL_MMC_DMACTRL_RX_DONE(pDev)            IO_BIT_GET(pDev->DMACTRL,MMC_DMACTRL_RXST_MASK)

#define LL_MMC_DMACTRL_SET_WR_DIR(pDev)         IO_BIT_SET(pDev->DMACTRL,MMC_DMACTRL_DIR_MASK)
#define LL_MMC_DMACTRL_SET_RD_DIR(pDev)         IO_BIT_CLEAR(pDev->DMACTRL,MMC_DMACTRL_DIR_MASK)
#define LL_MMC_DMACTRL_DMA_START(pDev)          IO_BIT_SET(pDev->DMACTRL,MMC_DMACTRL_STA_MASK)

#define LL_MMC_DMADATA_BUF(pDev)                (pDev->DMADATA)

#define LL_MMC_GET_CMD_RESP0(pDev)              (pDev->RESP0)
#define LL_MMC_GET_CMD_RESP1(pDev)              (pDev->RESP1)
#define LL_MMC_GET_CMD_RESP2(pDev)              (pDev->RESP2)
#define LL_MMC_GET_CMD_RESP3(pDev)              (pDev->RESP3)

#define LL_MMC_ADITLCTRL_DATA_BLOCK_NUM(pDev,val)  \
                                                IO_BITMASK_SET(pDev->ADITLCTRL,MMC_ADITLCTRL_DAEXN_MASK,\
                                                              (val)<< MMC_ADITLCTRL_DAEXN_POS)


#define LL_MMC_ADITLCTRL_CONFIG_BUS_CLK(pDev,val)  \
                                                IO_BITMASK_SET(pDev->ADITLCTRL,MMC_ADITLCTRL_BUSEXDCLK_MASK,\
                                                              (val)<< MMC_ADITLCTRL_BUSEXDCLK_POS)

#define LL_MMC_SET_ARG(pDev,val)                (pDev->ARG = val)

#define MMC_IE_ENABLE(pDev,val)                 IO_BITMASK_SET(pDev->IE,MMC_IE_MASK_ALL,(val))
#define MMC_IE_DISABLE(pDev,val)                IO_BITMASK_SET(pDev->IE,MMC_IE_MASK_ALL,~(val))
#define LL_MMC_IE_ENABLE_ALL(pDev)              MMC_IE_ENABLE(pDev,MMC_IE_MASK_ALL)


#define LL_MMC_SDIO_IRQ_PEDNING(pdev)           (pdev->IS & MMC_IS_SDIO_MASK)
#define LL_MMC_CRC7_ERR_PEDING(pdev)            (pdev->IS & MMC_IS_CRC7_ERR_MASK)
#define LL_MMC_CRC16_ERR_PEDING(pdev)           (pdev->IS & MMC_IS_CRC16_ERR_MASK)
#define LL_MMC_CMD_END_PEDING(pdev)             (pdev->IS & MMC_IS_CMD_END_MASK)
#define LL_MMC_DATA_END_PEDING(pdev)            (pdev->IS & MMC_IS_DATA_END_MASK)
#define LL_MMC_IRQ_PEDING(pdev)                 (pdev->IS & (MMC_IS_DATA_END_MASK|MMC_IS_CRC7_ERR_MASK|MMC_IS_CRC16_ERR_MASK|\
                                                             MMC_IS_CMD_END_MASK|MMC_IS_DATA_END_MASK))

#define LL_MMC_CLEAR_SDIO_IRQ(pdev)             (pdev->IS = MMC_IS_SDIO_MASK)
#define LL_MMC_CLEAR_CRC7_ERR_IRQ(pdev)         (pdev->IS = MMC_IS_CRC7_ERR_MASK)
#define LL_MMC_CLEAR_CRC16_ERR_IRQ(pdev)        (pdev->IS = MMC_IS_CRC16_ERR_MASK)
#define LL_MMC_CLEAR_CMD_END_IRQ(pdev)          (pdev->IS = MMC_IS_CMD_END_MASK)
#define LL_MMC_CLEAR_DATA_END_IRQ(pdev)         (pdev->IS = MMC_IS_DATA_END_MASK)


#define LL_MMC_SDIO_IRQ_ENABLE(pdev)             (pdev->IE & MMC_IE_SDIO_MASK)
#define LL_MMC_CRC7_ERR_IRQ_ENABLE(pdev)         (pdev->IE & MMC_IS_CRC7_ERR_MASK)
#define LL_MMC_CRC16_ERR_IRQ_ENABLE(pdev)        (pdev->IE & MMC_IS_CRC16_ERR_MASK)
#define LL_MMC_CMD_END_IRQ_ENABLE(pdev)          (pdev->IE & MMC_IS_CMD_END_MASK)
#define LL_MMC_DATA_END_IRQ_ENABLE(pdev)         (pdev->IE & MMC_IS_DATA_END_MASK)



/*SYS*/
#define LL_MMC_CLK_ENABLE(pDev)               (*((volatile uint32_t *)(0x40000080))|=(1<<25))
#define LL_MMC_CLK_DISABLE(pDev)              (*((volatile uint32_t *)(0x40000080))&=~(1<<25))

#define LL_MMC_RESET(pDev, us)                do{                                                       \
                                                    (*(volatile uint32_t *)(0x40000070))&=~(1<<25);   \
                                                    HAL_DelayUs(us);                                    \
                                                    (*(volatile uint32_t *)(0x40000070))|=(1<<25);    \
                                              }while(0)

/**MOD*/
#define   LL_MMC_BUS_CLK_STOP(pDev)              LL_MMC_SET_BUS_CLK(pDev,0)
#define   LL_MMC_BUS_CLK_START(pDev)             LL_MMC_SET_BUS_CLK(pDev,1)


#define LL_MMC_DLYCHN_OUTPUT(pDev,val)          IO_BITMASK_SET(pDev->DLYCHN,MMC_DLYCHN_OUTPUT_MASK,\
                                                              (val)<< MMC_DLYCHN_OUTPUT_POS)


#define LL_MMC_DLYCHN_INPUT(pDev,val)           IO_BITMASK_SET(pDev->DLYCHN,MMC_DLYCHN_INPUT_MASK,\
                                                              (val)<< MMC_DLYCHN_INPUT_POS)


#define LL_MMC_CBC_CURRENT_LENGTH(pDev)         (pDev->CBL)

#define LL_MMC_CBC_CURRENT_COUNT(pDev)          (pDev->CBC)

#define LL_MMC_GET_VERSION(pDev)                (pDev->VER)



#define MAX_MMC_HOST_DEVICE                     1


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_MMC_HOST_H_ */
