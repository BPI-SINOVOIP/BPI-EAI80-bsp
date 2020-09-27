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
 */
 
#ifndef _GM_LL_DMA_DEV_H_
#define _GM_LL_DMA_DEV_H_

#include <gm_common.h>


#ifdef __cplusplus
 extern "C" {
#endif

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
__IO uint32_t PES_CFG0                   ;   //0xFE0 RO 0x30 PES CFG0  
__IO uint32_t PES_CFG1                   ;   //0xFE4 RO 0xB2 PES CFG1  
__IO uint32_t NO_INC                     ;   //0xFE8 RO 0x-Bc Peripheral identification 2  
__IO uint32_t IE                         ;   //0xFEC RO 0x00 Peripheral identification 3  
__IO uint32_t IS                         ;   //0xFF0 RO 0x0D PrimeCell identification 0  
__IO uint32_t CH_CFG0                    ;   //0xFF4 RO 0xF0 PrimeCell identification 1  
__IO uint32_t CH_CFG1                    ;   //0xFF8 RO 0x05 PrimeCell identification 2  
__IO uint32_t VERSION                    ;   //0xFFC RO 0xB1 PrimeCell identification 3  
}DMA_Device_T;

#ifdef __cplusplus
}
#endif

#endif /*_GM_LL_DMA_DEV_H_*/



