/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file dma_dev.h
 *
 * @version 1.0.0
 * 
 * @date 2016/03/28
 *    
 * @contents I2C peripheral device header file
 * 
 * @history
 *    Date                By             Comments          Ver.
 *	=========       ===============     =========       =========
 *	2016/03/28		 Wizephen.Wang        Create          1.0.0
 ************************************************************************************/
 
#ifndef _DMA_DEV_H_
#define _DMA_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "CMSDK_CM4.h"
#include "core_cm4.h"
#include <common.h>
#include <stdint.h>
     


//#ifdef CPU_GM6601

//  typedef struct 
//  {    
//      __IO uint32_t ENDMA;         // DMA enable, Channel enable     (0x00)
//      __IO uint32_t RERSERVED;
//      __IO uint32_t SAR;           // Source address                 (0x08)
//      __IO uint32_t DAR;           // Destination address            (0x0C)
//      __IO uint32_t CTRL_L;        // Control register Low           (0x10)
//      __IO uint32_t CTRL_H;        // Control register High          (0x14)
//      __IO uint32_t CFG_L;         // Configure register Low         (0x18)
//      __IO uint32_t CFG_H;         // Configure register High        (0x1C)
//      __IO uint32_t INTRAWST;      // Configure register High        (0x20)
//      __IO uint32_t INTST;         // Interrupt status               (0x24)
//      __IO uint32_t INTMASK;       // Interrupt mask, bitmask = 1, interrupt enable   (0x28)
//      __IO uint32_t INTCOMBST;     // Interrupt clear                (0x2C)
//      __IO uint32_t SWHS;          // Software handshake             (0x30)
//      __IO uint32_t VERSION;       // Version                        (0x34)
//  }DMA_Device_T;

typedef struct 
{    
    __IO uint32_t ENDMA;         // DMA enable, Channel enable     (0x00)
    __IO uint32_t ID;            // DMA ID                         (0x04)
    __IO uint32_t HS_CH_SRC_SEL; // DMA Handshake channel src sel  (0x08)
    __IO uint32_t HS_CH_DST_SEL; // DMA Handshake channel src sel  (0x0C)
    __IO uint32_t INTRAWST;      // DMA Int RAW Status             (0x10)
    __IO uint32_t INTST;         // Interrupt status & Clear       (0x14)
    __IO uint32_t INTMASK;       // Interrupt mask                 (0x18)
    __IO uint32_t SWHS;          // Software handshake             (0x1C)
    
    __IO uint32_t CH0_SAR;       // Channel 0 SAR                  (0x20)
    __IO uint32_t CH0_DAR;       // Channel 0 DAR                  (0x24)
    __IO uint32_t CH0_TRANS_CFG; // Channel 0 trans config         (0x28)
    __IO uint32_t CH0_BLK_TS;    // Channel 0 block size           (0x2C)
    __IO uint32_t CH0_HS_CFG;    // Channel 0 handshake config     (0x30)
    __IO uint32_t CH0_SGR;       // Channel 0 SRC Gather           (0x34)
    __IO uint32_t CH0_DSR;       // Channel 0 DST Scatter          (0x38)
    __IO uint32_t RESERVED;      //                                (0x3C)
    
    __IO uint32_t CH1_SAR;       // Channel 1 SAR                  (0x40)
    __IO uint32_t CH1_DAR;       // Channel 1 DAR                  (0x44)
    __IO uint32_t CH1_TRANS_CFG; // Channel 1 trans config         (0x48)
    __IO uint32_t CH1_BLK_TS;    // Channel 1 block size           (0x4C)
    __IO uint32_t CH1_HS_CFG;    // Channel 1 handshake config     (0x50)
    __IO uint32_t CH1_SGR;       // Channel 1 SRC Gather           (0x54)
    __IO uint32_t CH1_DSR;       // Channel 1 DST Scatter          (0x58)
    __IO uint32_t RESERVED1;     //                                (0x5C)
    
    __IO uint32_t CH2_SAR;       // Channel 2 SAR                  (0x60)
    __IO uint32_t CH2_DAR;       // Channel 2 DAR                  (0x64)
    __IO uint32_t CH2_TRANS_CFG; // Channel 2 trans config         (0x68)
    __IO uint32_t CH2_BLK_TS;    // Channel 2 block size           (0x6C)
    __IO uint32_t CH2_HS_CFG;    // Channel 2 handshake config     (0x70)
    __IO uint32_t CH2_SGR;       // Channel 2 SRC Gather           (0x74)
    __IO uint32_t CH2_DSR;       // Channel 2 DST Scatter          (0x78)
    __IO uint32_t RESERVED2;     //                                (0x7C)
    
    __IO uint32_t CH3_SAR;       // Channel 3 SAR                  (0x80)
    __IO uint32_t CH3_DAR;       // Channel 3 DAR                  (0x84)
    __IO uint32_t CH3_TRANS_CFG; // Channel 3 trans config         (0x88)
    __IO uint32_t CH3_BLK_TS;    // Channel 3 block size           (0x8C)
    __IO uint32_t CH3_HS_CFG;    // Channel 3 handshake config     (0x90)
    __IO uint32_t CH3_SGR;       // Channel 3 SRC Gather           (0x94)
    __IO uint32_t CH3_DSR;       // Channel 3 DST Scatter          (0x98)
    __IO uint32_t RESERVED3;     //                                (0x9C)
    
    __IO uint32_t CH4_SAR;       // Channel 4 SAR                  (0xA0)
    __IO uint32_t CH4_DAR;       // Channel 4 DAR                  (0xA4)
    __IO uint32_t CH4_TRANS_CFG; // Channel 4 trans config         (0xA8)
    __IO uint32_t CH4_BLK_TS;    // Channel 4 block size           (0xAC)
    __IO uint32_t CH4_HS_CFG;    // Channel 4 handshake config     (0xB0)
    __IO uint32_t CH4_SGR;       // Channel 4 SRC Gather           (0xB4)
    __IO uint32_t CH4_DSR;       // Channel 4 DST Scatter          (0xB8)

    __IO uint32_t DMA_PES_CTRL;  // DMA PES Control                (0xBC)
    
}DMA_Device_T;




/* Base addresses of I2C Peripheral Control Registers */
#define DMA0_BASE                               (0x40021000U)
#define DMA1_BASE                               (0x40020000U)
#define DMA2_BASE                               (0x40021000U)

/* I2C Device Pointer */
#define DMA0                                    ((DMA_Device_T *)DMA0_BASE)
#define DMA1                                    ((DMA_Device_T *)DMA1_BASE)
#define DMA2                                    ((DMA_Device_T *)DMA2_BASE)

//#endif

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/*DMA Enable register*/
#define DMA_ENDMA_DMAEN_POS            ((uint32_t) 0)
#define DMA_ENDMA_DMAEN_MASK           ((uint32_t) (1 << DMA_ENDMA_DMAEN_POS))
#define DMA_ENDMA_TRANSTRIG_POS        ((uint32_t) 8)
#define DMA_ENDMA_TRANSTRIG_MASK       ((uint32_t) (1 << DMA_ENDMA_TRANSTRIG_POS)) 


/*DMA Source Address configure reigster*/
/*DMA Destination Address configure reigster*/

/*DMA Control Low configure reigster*/
#define DMA_CTRL_L_INTEN_POS           ((uint32_t) 0)
#define DMA_CTRL_L_INTEN_MASK          ((uint32_t) (1 << DMA_CTRL_L_INTEN_POS))
/*Destination Transfer Width*/
#define DMA_CTRL_L_DTW_POS             ((uint32_t) 1)
#define DMA_CTRL_L_DTW_MASK            ((uint32_t) (0x3 << DMA_CTRL_L_DTW_POS))
#define DMA_CTRL_L_DTW_FIELD(x)        ((uint32_t) ((x) << DMA_CTRL_L_DTW_POS))

/*Source Transfer Width*/
#define DMA_CTRL_L_STW_POS             ((uint32_t) 4)
#define DMA_CTRL_L_STW_MASK            ((uint32_t) (0x3 << DMA_CTRL_L_STW_POS))
#define DMA_CTRL_L_STW_FIELD(x)        ((uint32_t) ((x) << DMA_CTRL_L_STW_POS))

/*Destination Address Increment*/
#define DMA_CTRL_L_DINC_POS            ((uint32_t) 7)
#define DMA_CTRL_L_DINC_MASK           ((uint32_t) (0x3 << DMA_CTRL_L_DINC_POS))
#define DMA_CTRL_L_DINC_FIELD(x)        ((uint32_t) ((x) << DMA_CTRL_L_DINC_POS))
/*Source Address Increment*/
#define DMA_CTRL_L_SINC_POS            ((uint32_t) 9)
#define DMA_CTRL_L_SINC_MASK           ((uint32_t) (0x3 << DMA_CTRL_L_SINC_POS))
#define DMA_CTRL_L_SINC_FIELD(x)       ((uint32_t) ((x) << DMA_CTRL_L_SINC_POS))
/*Destination Address Increment*/
#define DMA_CTRL_L_DSTMSIZE_POS        ((uint32_t) 11)
#define DMA_CTRL_L_DSTMSIZE_MASK       ((uint32_t) (1 << DMA_CTRL_L_DSTMSIZE_POS))
/*Source Address Increment*/
#define DMA_CTRL_L_SRCMSIZE_POS        ((uint32_t) 14)
#define DMA_CTRL_L_SRCMSIZE_MASK       ((uint32_t) (1 << DMA_CTRL_L_SRCMSIZE_POS))
/*Transfer Type*/
#define DMA_CTRL_L_TTFC_POS            ((uint32_t) 20)
#define DMA_CTRL_L_TTFC_MASK           ((uint32_t) (0x3 << DMA_CTRL_L_TTFC_POS))
#define DMA_CTRL_L_TTFC_FIELD(x)       ((uint32_t) ((x) << DMA_CTRL_L_TTFC_POS))


/*DMA Control High register*/

/*DMA Configure Low register*/
#define DMA_CFG_L_SUSP_POS             ((uint32_t) 8) 
#define DMA_CFG_L_SUSP_MASK            ((uint32_t) (1 << DMA_CFG_L_SUSP_POS))
#define DMA_CFG_L_FIFOEMP_POS          ((uint32_t) 9) 
#define DMA_CFG_L_FIFOEMP_MASK         ((uint32_t) (1 << DMA_CFG_L_FIFOEMP_POS)) 
#define DMA_CFG_L_HSSELDST_POS         ((uint32_t) 10) 
#define DMA_CFG_L_HSSELDST_MASK        ((uint32_t) (1 << DMA_CFG_L_HSSELDST_POS)) 
#define DMA_CFG_L_HSSELSRC_POS         ((uint32_t) 11) 
#define DMA_CFG_L_HSSELSRC_MASK        ((uint32_t) (1 << DMA_CFG_L_HSSELSRC_POS)) 
#define DMA_CFG_L_MAXBUSRT_POS         ((uint32_t) 20) 
#define DMA_CFG_L_RELDDST_POS          ((uint32_t) 30) 
#define DMA_CFG_L_RELDDST_MASK         ((uint32_t) (1 << DMA_CFG_L_RELDDST_POS)) 
#define DMA_CFG_L_RELDSRC_POS          ((uint32_t) 31) 
#define DMA_CFG_L_RELDSRC_MASK         ((uint32_t) (1 << DMA_CFG_L_RELDSRC_POS)) 

/*DMA Configure High register*/
#define DMA_CFG_H_FIFOMODE_POS         ((uint32_t) 1) 
#define DMA_CFG_H_FIFOMODE_MASK        ((uint32_t) (1 << DMA_CFG_H_FIFOMODE_POS))
#define DMA_CFG_H_SRCPER_POS           ((uint32_t) 7) 
#define DMA_CFG_H_SRCPER_MASK          ((uint32_t) (0xF << DMA_CFG_H_SRCPER_POS))
#define DMA_CFG_H_SRCPER_FIELD(x)      ((uint32_t) ((x) << DMA_CFG_H_SRCPER_POS))
#define DMA_CFG_H_DSTPER_POS           ((uint32_t) 11) 
#define DMA_CFG_H_DSTPER_MASK          ((uint32_t) (0xF << DMA_CFG_H_DSTPER_POS))
#define DMA_CFG_H_DSTPER_FIELD(x)      ((uint32_t) ((x) << DMA_CFG_H_DSTPER_POS))

#define UART2_DMA_TX_REQ               (0xF)
#define UART2_DMA_RX_REQ               (0xE)
#define UART1_DMA_TX_REQ               (0xD)
#define UART1_DMA_RX_REQ               (0xC)
#define UART0_DMA_TX_REQ               (0xB)                 
#define UART0_DMA_RX_REQ               (0xA)


/*DMA Interrupt/Clear Status register*/
#define DMA_INTST_TRCMPLT_POS          ((uint32_t) 4) 
#define DMA_INTST_TRCMPLT_MASK         ((uint32_t) (1 << DMA_INTST_TRCMPLT_POS))
#define DMA_INTST_BLKTRCMPLT_POS       ((uint32_t) 3) 
#define DMA_INTST_BLKTRCMPLT_MASK      ((uint32_t) (1 << DMA_INTST_BLKTRCMPLT_POS))
#define DMA_INTST_SRCTRCMPLT_POS       ((uint32_t) 2) 
#define DMA_INTST_SRCTRCMPLT_MASK      ((uint32_t) (1 << DMA_INTST_SRCTRCMPLT_POS))
#define DMA_INTST_DSTTRCMPLT_POS       ((uint32_t) 1) 
#define DMA_INTST_DSTTRCMPLT_MASK      ((uint32_t) (1 << DMA_INTST_DSTTRCMPLT_POS))
#define DMA_INTST_ERR_POS              ((uint32_t) 0) 
#define DMA_INTST_ERR_MASK             ((uint32_t) (1 << DMA_INTST_ERR_POS))

/*DMA Interrupt Mask register*/
#define DMA_INTMASK_TRCMPLT_POS        ((uint32_t) 4) 
#define DMA_INTMASK_TRCMPLT_MASK       ((uint32_t) (1 << DMA_INTMASK_TRCMPLT_POS))
#define DMA_INTMASK_BLKTRCMPLT_POS     ((uint32_t) 3) 
#define DMA_INTMASK_BLKTRCMPLT_MASK    ((uint32_t) (1 << DMA_INTMASK_BLKTRCMPLT_POS))
#define DMA_INTMASK_SRCTRCMPLT_POS     ((uint32_t) 2) 
#define DMA_INTMASK_SRCTRCMPLT_MASK    ((uint32_t) (1 << DMA_INTMASK_SRCTRCMPLT_POS))
#define DMA_INTMASK_DSTTRCMPLT_POS     ((uint32_t) 1) 
#define DMA_INTMASK_DSTTRCMPLT_MASK    ((uint32_t) (1 << DMA_INTMASK_DSTTRCMPLT_POS))
#define DMA_INTMASK_ERR_POS            ((uint32_t) 0) 
#define DMA_INTMASK_ERR_MASK           ((uint32_t) (1 << DMA_INTMASK_ERR_POS))

/*DMA Software Handshaking register*/
#define DMA_SWHS_REQSRC_POS           ((uint32_t) 0) 
#define DMA_SWHS_REQSRC_MASK          ((uint32_t) (1 << DMA_SWHS_REQSRC_POS))
#define DMA_SWHS_REQDST_POS           ((uint32_t) 1) 
#define DMA_SWHS_REQDST_MASK          ((uint32_t) (1 << DMA_SWHS_REQDST_POS))
#define DMA_SWHS_SGLREQSRC_POS        ((uint32_t) 2) 
#define DMA_SWHS_SGLREQSRC_MASK       ((uint32_t) (1 << DMA_SWHS_SGLREQSRC_POS))
#define DMA_SWHS_SGLREQDST_POS        ((uint32_t) 3) 
#define DMA_SWHS_SGLREQDST_MASK       ((uint32_t) (1 << DMA_SWHS_SGLREQDST_POS))
#define DMA_SWHS_LSTSRC_POS           ((uint32_t) 4) 
#define DMA_SWHS_LSTSRC_MASK          ((uint32_t) (1 << DMA_SWHS_LSTSRC_POS))
#define DMA_SWHS_LSTDST_POS           ((uint32_t) 5) 
#define DMA_SWHS_LSTDST_MASK          ((uint32_t) (1 << DMA_SWHS_LSTDST_POS))

/*DMA Enable Register*/
#define DMA_DEV_ISACTIVE(DEVICE)      (READ_BIT((DEVICE)->ENDMA,  DMA_ENDMA_DMAEN_MASK))
#define DMA_DEV_ENABLE(DEVICE)        (SET_BIT((DEVICE)->ENDMA,   DMA_ENDMA_DMAEN_MASK))
#define DMA_DEV_DISABLE(DEVICE)       (CLEAR_BIT((DEVICE)->ENDMA, DMA_ENDMA_DMAEN_MASK))
#define DMA_TRANS_TRIGGER(DEVICE)     (SET_BIT((DEVICE)->ENDMA,   DMA_ENDMA_TRANSTRIG_MASK))

/*DMA Control LOW Register*/
#define DMA_INTERRUPT_ENABLE(DEVICE)  (SET_BIT((DEVICE)->CTRL_L,  DMA_CTRL_L_INTEN_MASK))
#define DMA_INTERRUPT_DISABLE(DEVICE) (CLEAR_BIT((DEVICE)->CTRL_L,DMA_CTRL_L_INTEN_MASK))

/*DMA Control HIGH Register*/

/*DMA Configure LOW Register*/
#define DMA_SELECT_SW_HANDSHAKING(DEVICE)  (SET_BIT((DEVICE)->CFG_L,  DMA_CFG_L_HSSELDST_MASK))
#define DMA_SELECT_HW_HANDSHAKING(DEVICE)  (CLEAR_BIT((DEVICE)->CFG_L,DMA_CFG_L_HSSELDST_MASK))
#define DMA_SET_RELOADDST(DEVICE)          (SET_BIT((DEVICE)->CFG_L,  DMA_CFG_L_RELDDST_MASK))
#define DMA_CLEAR_RELOADDST(DEVICE)        (CLEAR_BIT((DEVICE)->CFG_L,DMA_CFG_L_RELDDST_MASK))
#define DMA_SET_RELOADSRC(DEVICE)          (SET_BIT((DEVICE)->CFG_L,  DMA_CFG_L_RELDSRC_MASK))
#define DMA_CLEAR_RELOADSRC(DEVICE)        (CLEAR_BIT((DEVICE)->CFG_L,DMA_CFG_L_RELDSRC_MASK))

#define DMA_SET_BLKSIZE(DEVICE, BLKSIZE)   do{                    \
                                              if(BLKSIZE > 1023)  \
                                                BLKSIZE = 1023;   \
                                              WRITE_REG((DEVICE)->CTRL_H, BLKSIZE); \
                                           }while(0)

/*DMA Interrupt & Clear Register*/
#define ALL_DMA_INTERRUPTS                 0xFF

#define DMA_CLEAR_TRCMPLTC(DEVICE)         (WRITE_REG((DEVICE)->INTST,DMA_INTST_TRCMPLT_MASK))
#define DMA_CLEAR_BLKTRCMPLT(DEVICE)       (WRITE_REG((DEVICE)->INTST,DMA_INTST_BLKTRCMPLT_MASK))
#define DMA_CLEAR_SRCTRCMPLT(DEVICE)       (WRITE_REG((DEVICE)->INTST,DMA_INTST_SRCTRCMPLT_MASK))
#define DMA_CLEAR_DSTTRCMPL(DEVICE)        (WRITE_REG((DEVICE)->INTST,DMA_INTST_DSTTRCMPLT_MASK))
#define DMA_CLEAR_ERR(DEVICE)              (WRITE_REG((DEVICE)->INTST,DMA_INTST_ERR_MASK))

#define DAM_CLEAR_ALL_INTERRUPTS(DEVICE, INTERRUPT)   (WRITE_REG((DEVICE)->INTST,INTERRUPT))

/*DMA Interrupt Mask Register*/
#define DMA_MASK_TRCMPLTC(DEVICE)          (SET_BIT((DEVICE)->INTMASK,DMA_INTMASK_TRCMPLT_MASK))
#define DMA_MASK_BLKTRCMPLT(DEVICE)        (SET_BIT((DEVICE)->INTMASK,DMA_INTMASK_BLKTRCMPLT_MASK))
#define DMA_MASK_SRCTRCMPLT(DEVICE)        (SET_BIT((DEVICE)->INTMASK,DMA_INTMASK_SRCTRCMPLT_MASK))
#define DMA_MASK_DSTTRCMPLT(DEVICE)        (SET_BIT((DEVICE)->INTMASK,DMA_INTMASK_DSTTRCMPLT_MASK))
#define DMA_MASK_ERR(DEVICE)               (SET_BIT((DEVICE)->INTMASK,DMA_INTMASK_ERR_MASK))

#define DMA_UNMASK_TRCMPLTC(DEVICE)        (CLEAR_BIT((DEVICE)->INTMASK,DMA_INTMASK_TRCMPLT_MASK))
#define DMA_UNMASK_BLKTRCMPLT(DEVICE)      (CLEAR_BIT((DEVICE)->INTMASK,DMA_INTMASK_BLKTRCMPLT_MASK))
#define DMA_UNMASK_SRCTRCMPLT(DEVICE)      (CLEAR_BIT((DEVICE)->INTMASK,DMA_INTMASK_SRCTRCMPLT_MASK))
#define DMA_UNMASK_DSTTRCMPLT(DEVICE)      (CLEAR_BIT((DEVICE)->INTMASK,DMA_INTMASK_DSTTRCMPLT_MASK))
#define DMA_UNMASK_ERR(DEVICE)             (CLEAR_BIT((DEVICE)->INTMASK,DMA_INTMASK_ERR_MASK))

#define DMA_MASK_ALL_INTERRUPTS(DEVICE, INTERRUPT)     (WRITE_REG((DEVICE)->INTMASK,INTERRUPT))

#if 0
#define DMA_UNMASK_ALL_INTERRUPTS(DEVICE)  do {                         \
                                           DMA_UNMASK_TRCMPLTC(DEVICE)  \
                                           DMA_UNMASK_BLKTRCMPLT(DEVICE)\
                                           DMA_UNMASK_SRCTRCMPLT(DEVICE)\
                                           DMA_UNMASK_SRCTRCMPLT(DEVICE)\
                                           DMA_UNMASK_ERR(DEVICE)       \
                                           }while(0)
#else
#define DMA_UNMASK_ALL_INTERRUPTS(DEVICE)  (WRITE_REG((DEVICE)->INTMASK,0x00))                                              
#endif
                                           
                                           

/*DMA Software Handshaking Control Register*/
#define DMA_SET_SRC_REQ(DEVICE)            (SET_BIT((DEVICE)->SWHS,  DMA_SWHS_REQSRC_MASK))
#define DMA_CLEAR_SRC_REQ(DEVICE)          (CLEAR_BIT((DEVICE)->SWHS,DMA_SWHS_REQSRC_MASK))
#define DMA_SET_DST_REQ(DEVICE)            (SET_BIT((DEVICE)->SWHS,  DMA_SWHS_REQDST_MASK))
#define DMA_CLEAR_DST_REQ(DEVICE)          (CLEAR_BIT((DEVICE)->SWHS,DMA_SWHS_REQDST_MASK))
#define DMA_SET_SRCSGLREQ(DEVICE)          (SET_BIT((DEVICE)->SWHS,  DMA_SWHS_SGLREQSRC_MASK))
#define DMA_CLEAR_SRCSGLREQ(DEVICE)        (CLEAR_BIT((DEVICE)->SWHS,DMA_SWHS_SGLREQSRC_MASK))
#define DMA_SET_DSTSGLREQ(DEVICE)          (SET_BIT((DEVICE)->SWHS,  DMA_SWHS_SGLREQDST_MASK))
#define DMA_CLEAR_DSTSGLREQ(DEVICE)        (CLEAR_BIT((DEVICE)->SWHS,DMA_SWHS_SGLREQDST_MASK))
#define DMA_SET_LSTSRCREQ(DEVICE)          (SET_BIT((DEVICE)->SWHS,  DMA_SWHS_LSTSRC_MASK))
#define DMA_CLEAR_LSTSRCREQ(DEVICE)        (CLEAR_BIT((DEVICE)->SWHS,DMA_SWHS_LSTSRC_MASK))
#define DMA_SET_LSTDSTREQ(DEVICE)          (SET_BIT((DEVICE)->SWHS,  DMA_SWHS_LSTDST_MASK))
#define DMA_CLEAR_LSTDSTREQ(DEVICE)        (CLEAR_BIT((DEVICE)->SWHS,DMA_SWHS_LSTDST_MASK))

#ifdef __cplusplus
}
#endif

#endif /*_DMA_DEV_H_*/

