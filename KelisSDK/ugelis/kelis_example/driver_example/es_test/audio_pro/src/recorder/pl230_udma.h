/*====================================================================
 *
 * Copyright (C) 2017 Gree Microelectronics.  All Rights Reserved.
 *
 * @file dma_dev.h
 *
 * @version 1.0.0
 *
 * @date 2017/12/21
 *
 * @contents pl230_udma peripheral  header file
 *
 * @history
 *    Date                By             Comments          Ver.
 *  =========       ===========     =========       =========
 *  2017/12/21       Justin      Create            1.0.0
 ************************************************************************************/

#ifndef _PL230_UDMA_DEV_H_
#define _PL230_UDMA_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "CMSDK_CM4.h"
//#include "core_cm4.h"
//#include <common.h>
#include <stdint.h>
#include "sys_sai.h"

typedef struct
{
    __IO uint32_t  SRC_END ; //Source End Pointer
    __IO uint32_t  DES_END ; //Destination End Pointer
    __IO uint32_t  CTRL    ; //Control
    __IO uint32_t  RESERVED; //Unused

} PL230_UDMA_CH_T;

typedef struct
{
    PL230_UDMA_CH_T  Prim_CH[16] ;    //Primary data struct of all 16 channel
    PL230_UDMA_CH_T  Alter_CH[16];    //Alternate data struct of all 16 channel
} PL230_UDMA_Control_T;

typedef struct
{
    __IO uint32_t DMA_STATUS                 ;   //0x000 RO 0x-0nn0000a DMA status
    __IO uint32_t DMA_CFG                    ;   //0x004 WO - DMA configuration
    __IO uint32_t CTRL_BASE_PTR              ;   //0x008 R/W 0x00000000 Channel control data base pointer
    __IO uint32_t ALT_CTRL_BASE_PTR          ;   //0x00C RO 0x000000nnb Channel alternate control data base pointer
    __IO uint32_t DMA_WAITONREQ_STATUS       ;   //0x010 RO 0x00000000 Channel wait on request status
    __IO uint32_t CHNL_SW_REQUEST            ;   //0x014 WO - Channel software request
    __IO uint32_t CHNL_USEBURST_SET          ;   //0x018 R/W 0x00000000 Channel useburst set
    __IO uint32_t CHNL_USEBURST_CLR          ;   //0x01C WO - Channel useburst clear
    __IO uint32_t CHNL_REQ_MASK_SET          ;   //0x020 R/W 0x00000000 Channel request mask set
    __IO uint32_t CHNL_REQ_MASK_CLR          ;   //0x024 WO - Channel request mask clear
    __IO uint32_t CHNL_ENABLE_SET            ;   //0x028 R/W 0x00000000 Channel enable set
    __IO uint32_t CHNL_ENABLE_CLR            ;   //0x02C WO - Channel enable clear
    __IO uint32_t CHNL_PRI_ALT_SET           ;   //0x030 R/W 0x00000000 Channel primary-alternate set
    __IO uint32_t CHNL_PRI_ALT_CLR           ;   //0x034 WO - Channel primary-alternate clear
    __IO uint32_t CHNL_PRIORITY_SET          ;   //0x038 R/W 0x00000000 Channel priority set
    __IO uint32_t CHNL_PRIORITY_CLR          ;   //0x03C WO - Channel priority clear
    __IO uint32_t RESERVED0[3]               ;   //- 0x040-0x48 - - Reserved
    __IO uint32_t ERR_CLR                    ;   //0x04C R/W 0x00000000 Bus error clear
    __IO uint32_t RESERVED1[876]             ;   //- 0x050-0xDFC - - Reserved
    __IO uint32_t RESERVED2[19]              ;   //- 0xE00-0xE48 - - See Chapter 4 Programmer¡¯s Model for Test
    __IO uint32_t RESERVED3[97]              ;   //- 0xE4C-0xFCC - - Reserved
    __IO uint32_t PERIPH_ID_4                ;   //0xFD0 RO 0x04 Peripheral identification 4
    __IO uint32_t RESERVED4[3]               ;   //- 0xFD4-0xFDC - - Reserved
    __IO uint32_t PERIPH_ID_0                ;   //0xFE0 RO 0x30 Peripheral identification 0
    __IO uint32_t PERIPH_ID_1                ;   //0xFE4 RO 0xB2 Peripheral identification 1
    __IO uint32_t NO_INC                     ;   //0xFE8 RO 0x-Bc Peripheral identification 2
    __IO uint32_t IE                         ;   //0xFEC RO 0x00 Peripheral identification 3
    __IO uint32_t INTFLAG                    ;   //0xFF0 RO 0x0D PrimeCell identification 0
    __IO uint32_t CH_CFG0                    ;   //0xFF4 RO 0xF0 PrimeCell identification 1
    __IO uint32_t CH_CFG1                    ;   //0xFF8 RO 0x05 PrimeCell identification 2
    __IO uint32_t VERSION                    ;   //0xFFC RO 0xB1 PrimeCell identification 3
} PL230_UDMA_Dev_T;



#define PL230_UDMA_BASE                         (0x40020000U)

#define UDMA                                    ((PL230_UDMA_Dev_T *)PL230_UDMA_BASE)


//
// data size config
//
#define DMA_BYTE_MODE       0
#define DMA_HALFWORD_MODE   1
#define DMA_WORD_MODE       2

//
// memory to memory mode or not
//
#define DMA_NON_MEM2MEM     0
#define DMA_MEM2MEM         1

//
// burst length config
//
#define NO_BURST            0
#define BURST2              1
#define BURST4              2
#define BURST8              3

//
// channel interrupt enable or not
//
#define INT_DISABLE         0
#define INT_ENABLE          1

//
// use low or high priority
//
#define LOW_PRIORITY        0
#define HIGH_PRIORITY       1

//
// use primary or alter control data struct
//
#define PRIMARY             0
#define ALTER               1

//
// src/dst address change or not
//
#define ALL_CHANGE          0
#define SRC_NO_CHANGE       1
#define DST_NO_CHANGE       2
#define ALL_NO_CHANGE       3

//
// channel select
//
#define DMA_CH0             0
#define DMA_CH1             1
#define DMA_CH2             2
#define DMA_CH3             3
#define DMA_CH4             4
#define DMA_CH5             5
#define DMA_CH6             6
#define DMA_CH7             7
#define DMA_CH8             8
#define DMA_CH9             9
#define DMA_CH10            10
#define DMA_CH11            11
#define DMA_CH12            12
#define DMA_CH13            13
#define DMA_CH14            14
#define DMA_CH15            15

//
// device select
//
#define DMA_DEV0            0
#define DMA_DEV1            1
#define DMA_DEV2            2
#define DMA_DEV3            3
#define DMA_DEV4            4
#define DMA_DEV5            5
#define DMA_DEV6            6
#define DMA_DEV7            7





#ifdef __cplusplus
}
#endif

#endif /*_PL230_UDMA_DEV_H_*/

