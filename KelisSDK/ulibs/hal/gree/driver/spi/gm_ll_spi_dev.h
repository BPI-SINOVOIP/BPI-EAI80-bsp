/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          spi_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 * 
 * @date          2016/07/08 
 *    
 * @brief         SPI low level driver header file. 
 * 
 * @note
 *    2016/07/08, wen.liu, V1.0.0
 *        Initial version.
 */
#ifndef _GM_LL_SPI_DEV_H_
#define _GM_LL_SPI_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SPI Control Register Layout Type Definition 
 */
typedef struct
{
    __IO uint32_t VER;                /**< SPI IP Version Register, offset: 0x0 */
    __IO uint32_t CTRL;               /**< SPI Control Register, offset: 0x4 */
    __IO uint32_t FRM;                /**< SPI Frame Configuartion Register, offset: 0x8 */
    __IO uint32_t STM;                /**< SPI Sample Timing Configuartion Register, offset: 0xc */
    __IO uint32_t RESV0[4];           /**< SPI Reserved Registers, offset: 0x10~0x1c  */
    __IO uint32_t CCNT;               /**< SPI Continuous Operation Counter, offset: 0x20 */
    __IO uint32_t DMY;                /**< SPI Dummy Configuartion Register, offset: 0x24 */
    __IO uint32_t BR;                 /**< SPI Baud Rate Configuartion Register, offset: 0x28 */
    __IO uint32_t SSCTRL;             /**< SPI SS(Slave Select) Control Register, offset: 0x2c */
    __IO uint32_t SSTM;               /**< SPI SS(Slave Select) Timing Configuration Register, offset: 0x30 */
    __IO uint32_t RESV1[3];           /**< SPI Reserved Registers, offset: 0x34~0x3c  */
    __IO uint32_t FFCFG;              /**< SPI FIFO Configuration Register, offset: 0x40 */
    __IO uint32_t FFLV;               /**< SPI FIFO Level Register, offset: 0x44 */
    __IO uint32_t TDMA;               /**< SPI Transmit DMA Control Register, offset: 0x48 */
    __IO uint32_t RDMA;               /**< SPI Receive DMA Control Register, offset: 0x4c */
    __IO uint32_t MTCH;               /**< SPI Match Register, offset: 0x50 */
    __IO uint32_t RESV2[3];           /**< SPI Reserved Registers, offset: 0x54~0x5c  */
    __IO uint32_t DR;                 /**< SPI Data Register, offset: 0x60 */
    __IO uint32_t SR;                 /**< SPI Status Register, offset: 0x64 */
    __IO uint32_t IS;                 /**< SPI Interrupt Enable Register, offset: 0x68 */
    __IO uint32_t IE;                 /**< SPI Interrupt Status Register, offset: 0x6c */ 
    __IO uint32_t CI;                 /**< SPI Combined Interrupt Enable/Status Register, offset: 0x70 */ 
} SPI_Device_T;

typedef struct
{   
	uint32_t startTime;       /* Start time by millisecond */  
	uint32_t wakeupPeriod;    /* Wakeup interval */   
	uint32_t timeout;         /* Expected timeout interval by millisecond */
} SPI_WaitTimer_T;

#define  SPI_CTRL_EN                 (0x00000001U)            /**< SPI Enable/Disable */
                                     
#define  SPI_CTRL_FMT                (0x00000006U)            /**< SPI Frame Format Selection, 0: Freescale; 1: TI;  
                                                                                               2: MICROWIRE; 3:  Reserved */
#define  SPI_CTRL_FMT_SHIFT          (1U)
#define  SPI_CTRL_FMT_FIELD(x)       (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_FMT_SHIFT)) & SPI_CTRL_FMT) 
#define  SPI_CTRL_FMT_FREESCALE      (0U << SPI_CTRL_FMT_SHIFT)
#define  SPI_CTRL_FMT_TI             (1U << SPI_CTRL_FMT_SHIFT)
#define  SPI_CTRL_FMT_MICROWIRE      (2U << SPI_CTRL_FMT_SHIFT)
                                     
#define  SPI_CTRL_MS                 (0x00000008U)   /**< SPI Master/Slave Mode Selection, 0: Slave; 1: Master */                                 
#define  SPI_CTRL_LSB                (0x00000010U)   /**< SPI Data Shift Direction, 0: MSB; 1: LSB */
#define  SPI_CTRL_CPHA               (0x00000020U)   /**< SPI Clock Phase, 0: First Clock Edge at the middle; 1: First Clock Edge at the start */
#define  SPI_CTRL_CPOL               (0x00000040U)   /**< SPI Clock Ploarity, 0: Idle Status is Low Level; 1: Idle Status is High Level; */
#define  SPI_CTRL_RCONT              (0x00000080U)   /**< SPI Continous Receiving Data Mode, 0: Disabled; 1: Enabled */                                     
#define  SPI_CTRL_TMODE              (0x00000300U)   /**< SPI Transmit Mode, 0: full-duplex; 1: half-duplex; 
                                                                             2: transmit only; 3: receive only */
#define  SPI_CTRL_TMODE_SHIFT        (8U)
#define  SPI_CTRL_TMODE_FIELD(x)     (((uint32_t)(((uint32_t)(x)) << SPI_CTRL_TMODE_SHIFT)) & SPI_CTRL_TMODE) 
#define  SPI_CTRL_TMODE_FULL_DUPLEX  (0U << SPI_CTRL_TMODE_SHIFT)
#define  SPI_CTRL_TMODE_HALF_DUPLEX  (1U << SPI_CTRL_TMODE_SHIFT)
#define  SPI_CTRL_TMODE_TX_ONLY      (2U << SPI_CTRL_TMODE_SHIFT)
#define  SPI_CTRL_TMODE_RX_ONLY      (3U << SPI_CTRL_TMODE_SHIFT)
                                     
#define  SPI_CTRL_XDATA              (0x00001000U)   /**< SPI Transmit Data, 0: Disabled; 1: Enabled */
#define  SPI_CTRL_XDMY               (0x00002000U)   /**< SPI Transmit Dummy, 0: Disabled; 1: Enabled */
#define  SPI_CTRL_XADDR              (0x00004000U)   /**< SPI Transmit Adddress, 0: Disabled; 1: Enabled */
#define  SPI_CTRL_XCMD               (0x00008000U)   /**< SPI Transmit Command, 0: Disabled; 1: Enabled */
#define  SPI_CTRL_XFRM               (SPI_CTRL_XDATA | SPI_CTRL_XDMY | SPI_CTRL_XADDR | SPI_CTRL_XCMD) /**< SPI Fixed Frames */

#define  SPI_CTRL_MFRM               (0x00010000U)   /**< SPI Transmit Multiple Data Frame, 0: Disabled; 1: Enabled */
#define  SPI_CTRL_CVSS               (0x00020000U)   /**< SPI Clock is valid when SS is issuing(only for TI mode), 0: Invalid; 1: Valid */
#define  SPI_CTRL_BIDIR              (0x01000000U)   /**< SPI single-wire bidirectional operation, 0: Seperate pins for input and output; */
#define  SPI_CTRL_BIDIROE            (0x02000000U)   /**< SPI single-wire bidirectional operation Output Enable, 0: Input; 1: Output */

                                     
#define  SPI_FRM_DATA                (0x0000001FU)   /**< SPI Data Frame Size 0~2: Reserved; 0x03~0x1F: (size+1) data bits */
#define  SPI_FRM_DATA_SHIFT          (0U)
#define  SPI_FRM_DATA_FIELD(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FRM_DATA_SHIFT)) & SPI_FRM_DATA) 
#define  SPI_FRM_DATA_GET(reg)       ((reg) & SPI_FRM_DATA) 
                                     
#define  SPI_FRM_CMD                 (0x00000F00U)             /**< SPI Command Frame Size, 0x00~0xF: (size+1) command bits */
#define  SPI_FRM_CMD_SHIFT           (8U)
#define  SPI_FRM_CMD_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << SPI_FRM_CMD_SHIFT)) & SPI_FRM_CMD) 
#define  SPI_FRM_CMD_GET(reg)        (((reg) & SPI_FRM_CMD) >> SPI_FRM_CMD_SHIFT) 
                                     
#define  SPI_FRM_ADDR                (0x001F0000U)             /**< SPI Address Frame Size 0~2: Reserved; 0x03~0x1F: (size+1) address bits */
#define  SPI_FRM_ADDR_SHIFT          (16U)
#define  SPI_FRM_ADDR_FIELD(x)       (((uint32_t)(((uint32_t)(x))<<SPI_FRM_ADDR_SHIFT))&SPI_FRM_ADDR) 
#define  SPI_FRM_ADDR_GET(reg)       (((reg)&SPI_FRM_ADDR)>>SPI_FRM_ADDR_SHIFT) 
                                     
#define  SPI_FRM_DUMMY               (0x1F000000U)             /**< SPI Dummy Frame Size 0~2: Reserved; 0x03~0x1F: (size+1) dummy bits */
#define  SPI_FRM_DUMMY_SHIFT         (24U)
#define  SPI_FRM_DUMMY_FIELD(x)      (((uint32_t)(((uint32_t)(x)) << SPI_FRM_DUMMY_SHIFT)) & SPI_FRM_DUMMY) 
#define  SPI_FRM_DUMMY_GET(reg)      (((reg) & SPI_FRM_DUMMY) >> SPI_FRM_DUMMY_SHIFT) 
                                     
#define  SPI_STM_BIT                 (0x0000001FU)             /**< SPI Bit Sample Delayed Clocks, 0x00~0x1F: (size+1) bit sample delayed clocks */
#define  SPI_STM_BIT_SHIFT           (0U)
#define  SPI_STM_BIT_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << SPI_STM_BIT_SHIFT)) & SPI_STM_BIT) 
#define  SPI_STM_BIT_GET(reg)        (((reg) & SPI_STM_BIT) >> SPI_STM_BIT_SHIFT)
                                     
#define  SPI_STM_FRM                 (0x00000F00U)            /**< SPI Frame Sample Delayed Frames, 0x00~0x0F: (size+1) bit sample delayed frames */
#define  SPI_STM_FRM_SHIFT           (8U)
#define  SPI_STM_FRM_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << SPI_STM_FRM_SHIFT)) & SPI_STM_FRM) 
#define  SPI_STM_FRM_GET(reg)        (((reg) & SPI_STM_FRM) >> SPI_STM_FRM_SHIFT)
                                     
#define  SPI_CCNT_MAX                (0xFFFFU)
                                     
#define  SPI_SSCTRL_SS0_EN           (0x00000001U)            /**< SPI Select SS0 pin, 0: SS0 pin Disabled; 1: SS0 pin Enabled */
#define  SPI_SSCTRL_SS1_EN           (0x00000002U)            /**< SPI Select SS1 pin, 0: SS1 pin Disabled; 1: SS1 pin Enabled */
#define  SPI_SSCTRL_SS0_OE           (0x00000004U)            /**< SPI SS0 pin Output Enable, 0: Input; 1: Output */
#define  SPI_SSCTRL_SS1_OE           (0x00000008U)            /**< SPI SS1 pin Output Enable, 0: Input; 1: Output */
#define  SPI_SSCTRL_SS0_PE           (0x00000100U)            /**< SPI SS0 pin User Program Enable, 0: Auto; 1: User Programed */
#define  SPI_SSCTRL_SS1_PE           (0x00000200U)            /**< SPI SS1 pin User Program Enable, 0: Auto; 1: User Programed */
#define  SPI_SSCTRL_SS0_PV           (0x00010000U)            /**< SPI SS0 pin User Program Value, 0: SS0 output Low Level; 1: SS0 output High Level */
#define  SPI_SSCTRL_SS1_PV           (0x00020000U)            /**< SPI SS1 pin User Program Value, 0: SS1 output Low Level; 1: SS1 output High Level */
#define  SPI_SSCTRL_MODEF_EN         (0x01000000U)            /**< SPI Master Mode-Fault Function Enable, 0: Disable; 1: Enable */
#define  SPI_SSCTRL_AUTO_TOGGLE      (0x02000000U)            /**< SPI SS Auto Toggled between frames, 0: Disable; 1: Enable */
                                                               
                                     
#define  SPI_SSTM_SETUP              (0x000000FFU)            /**< The amount of time between SS assertion and the beginning of a data transfer. */
#define  SPI_SSTM_SETUP_SHIFT        (0U)
#define  SPI_SSTM_SETUP_FIELD(x)     (((uint32_t)(((uint32_t)(x)) << SPI_SSTM_SETUP_SHIFT)) & SPI_SSTM_SETUP) 
                                     
#define  SPI_SSTM_HOLD               (0x0000FF00U)            /**< The amount of time between the end of a data transfer and SS de-assertion. */
#define  SPI_SSTM_HOLD_SHIFT         (8U)
#define  SPI_SSTM_HOLD_FIELD(x)      (((uint32_t)(((uint32_t)(x)) << SPI_SSTM_HOLD_SHIFT)) & SPI_SSTM_HOLD) 
                                     
#define  SPI_SSTM_FRMDLY             (0x00FF0000U)            /**< The minimum amount of time between frames. */
                                                             
#define  SPI_SSTM_FRMDLY_SHIFT       (16U)
#define  SPI_SSTM_FRMDLY_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << SPI_SSTM_FRMDLY_SHIFT)) & SPI_SSTM_FRMDLY) 
                                     
#define  SPI_SSTM_SSDLY              (0xFF000000U)           /**< The minimum amount of time that the SS is de-asserted between transfers. */
#define  SPI_SSTM_SSDLY_SHIFT        (24U)
#define  SPI_SSTM_SSDLY_FIELD(x)     (((uint32_t)(((uint32_t)(x)) << SPI_SSTM_SSDLY_SHIFT)) & SPI_SSTM_SSDLY) 
                                     
                                     
#define  SPI_FFCFG_TFTH              (0x000000FFU)           /**< Transmit FIFO Threshhold */
#define  SPI_FFCFG_TFTH_SHIFT        (0U)
#define  SPI_FFCFG_TFTH_FIELD(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FFCFG_TFTH_SHIFT)) & SPI_FFCFG_TFTH) 
#define  SPI_FFCFG_TFTH_GET(reg)     (((reg) & SPI_FFCFG_TFTH) >> SPI_STM_FRM_SHIFT)
                                     
#define  SPI_FFCFG_RFTH              (0x00FF0000U)           /**< Transmit FIFO Threshhold */
#define  SPI_FFCFG_RFTH_SHIFT        (16U)
#define  SPI_FFCFG_RFTH_FIELD(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FFCFG_RFTH_SHIFT)) & SPI_FFCFG_RFTH) 
#define  SPI_FFCFG_RFTH_GET(reg)     (((reg)&SPI_FFCFG_RFTH) >> SPI_STM_FRM_SHIFT)
                                     
#define  SPI_FFLV_TF                 (0x000000FFU)           /**< Transmit FIFO Level */
#define  SPI_FFLV_TF_SHIFT           (0U)
#define  SPI_FFLV_TF_GET(reg)        (((reg) & SPI_FFLV_TF) >> SPI_FFLV_TF_SHIFT)
                                     
#define  SPI_FFLV_RF                 (0x00FF0000U)           /**< Transmit FIFO Threshhold */
#define  SPI_FFLV_RF_SHIFT           (16U)
#define  SPI_FFLV_RF_GET(reg)        (((reg) & SPI_FFLV_RF) >> SPI_FFLV_RF_SHIFT)
                                     
#define  SPI_RDMA_RTH                (0x00FF0000U)           /**< RX DMA Request Threshhold */
#define  SPI_RDMA_RTH_SHIFT          (16U)
#define  SPI_RDMA_RTH_FIELD(x)       ((x) << SPI_RDMA_RTH_SHIFT)
#define  SPI_RDMA_RTH_GET(reg)       ((reg) >> SPI_RDMA_RTH_SHIFT)
                                     
#define  SPI_RDMA_EN                 (0x00000001U)
                                     
#define  SPI_TDMA_TTH                (0x00FF0000U)           /**< TX DMA Request Threshhold */
#define  SPI_TDMA_TTH_SHIFT          (16U)
#define  SPI_TDMA_TTH_FIELD(x)       ((x) << SPI_TDMA_TTH_SHIFT)
#define  SPI_TDMA_TTH_GET(reg)       ((reg) >> SPI_TDMA_TTH_SHIFT)
                                     
#define  SPI_TDMA_EN                 (0x00000001U)
                                     
#define  SPI_SR_BUSY                 (0x00000001U)           /**< SPI controller is busy, 0: Controller Disabled or idle; 1: Controller Active */
#define  SPI_SR_TFET                 (0x00000100U)           /**< SPI Transmit FIFO is Empty, 0: Not Empty; 1: Empty */
#define  SPI_SR_TFNFL                (0x00000400U)           /**< SPI Transmit FIFO is Not Full, 0: Full; 1: Not Full */
#define  SPI_SR_TFTH                 (0x00000800U)           /**< SPI Transmit FIFO Threshold, 0: Threshold Not Reached; 1: Threshold Reached */
                                                               
#define  SPI_SR_RFFL                 (0x00040000U)           /**< SPI Receive FIFO Full, 0: Not Full; 1: Full */
#define  SPI_SR_RFNET                (0x00080000U)           /**< SPI Receive FIFO Not Empty, 0: Empty; 1: Not Empty */
#define  SPI_SR_RFTH                 (0x00100000U)           /**< SPI Receive FIFO Threshold, 0: Threshold Not Reached; 1: Threshold Reached */
                                                               
#define  SPI_IS_TFOV                 (0x00000200U)           /**< SPI Transmit FIFO is Overflow, 0: Not Overflow; 1: Overflow */
#define  SPI_IS_TFTH                 (0x00000800U)           /**< SPI Transmit FIFO Threshold Interrupt 0: No Threshold Interrupt; 1: Threshold Interrupt */
#define  SPI_IS_RFUF                 (0x00010000U)           /**< SPI Receive FIFO UnderFlow Interrupt, 0: Not Underflow Interrupt; 1: Underflow Interrupt */
#define  SPI_IS_RFOV                 (0x00020000U)           /**< SPI Receive FIFO OverFlow Interrupt, 0: Not Overflow Interrupt; 1: Overflow Interrupt */
#define  SPI_IS_RFTH                 (0x00100000U)           /**< SPI Receive FIFO Threshold Interrupt, 0: No Threshold Interrupt; 1: Threshold Interrupt */
#define  SPI_IS_MODF                 (0x01000000U)           /**< SPI Mode Fault Interrupt, 0: No Mode Fault Interrupt; 1: Mode Fault Interrupt */
#define  SPI_IS_SLAUF                (0x02000000U)           /**< SPI Slave Underflow Interrupt, 0: No Slave Underflow Interrupt; 1: Slave Underflow Interrupt */
#define  SPI_IS_MTCH                 (0x04000000U)           /**< SPI RX Match Interrupt, 0: No RX Match Interrupt; 1: RX Match Interrupt */
#define  SPI_IS_SWKP                 (0x08000000U)           /**< SPI Slave Wakeup Interrupt, 0: No Slave Wakeup Interrupt; 1: Slave Wakeup Interrupt */
                                                               
#define  SPI_IE_TFET                 (0x00000100U)           /**< SPI Transmit FIFO Empty Interrupt Enable */
#define  SPI_IE_TFOV                 (0x00000200U)           /**< SPI Transmit FIFO Overflow Interrupt Enable */
#define  SPI_IE_TFTH                 (0x00000800U)           /**< SPI Transmit FIFO Threshold Interrupt Enable */
#define  SPI_IE_RFUF                 (0x00010000U)           /**< SPI Receive FIFO UnderFlow Interrupt Enable */
#define  SPI_IE_RFOV                 (0x00020000U)           /**< SPI Receive FIFO OverFlow Interrupt Enable */
#define  SPI_IE_RFFL                 (0x00040000U)           /**< SPI Receive FIFO Full Interrupt Enable */
#define  SPI_IE_RFTH                 (0x00100000U)           /**< SPI Receive FIFO Threshold Interrupt Enable */

#define  SPI_IE_DMARX                (0x01000000U)           /**< SPI Pseudo DMA RXInterrupt Enable */
#define  SPI_IE_DMATX                (0x01000000U)           /**< SPI Pseudo DMA TXInterrupt Enable */
                                                               
#define  SPI_CI_EN                   (0x00000001U)           /**< SPI Combined Interrupt Enable Register */
#define  SPI_CI_ST                   (0x00000100U)           /**< SPI Combined Interrupt Status Register */

/** SPI Device Pointer */
#define  SPI0_DEV                        ((SPI_Device_T *)SPI0_BASE)
#define  SPI1_DEV                        ((SPI_Device_T *)SPI1_BASE)
#define  SPI2_DEV                        ((SPI_Device_T *)SPI2_BASE)
#define  SPI3_DEV                        ((SPI_Device_T *)SPI3_BASE)
#define  SPI4_DEV                        ((SPI_Device_T *)SPI4_BASE)
#define  SPI5_DEV                        ((SPI_Device_T *)SPI5_BASE)

/** SPI Device Number */
#define  MAX_SPI_DEVICE              (6U)

/** All SPI Base Addresses */
#define ALL_SPI_DEVICES              SPI0_DEV,SPI1_DEV,SPI2_DEV,SPI3_DEV,SPI4_DEV,SPI5_DEV

/** All SPI Interrupts */
#define ALL_SPI_INTERRUPTS           SPI0_IRQn,SPI1_IRQn,SPI2_IRQn,SPI3_IRQn,SPI4_IRQn,SPI5_IRQn

/** All SPI Interrupts */
#define ALL_SPI_INTERRUPTS_PRIO      SPI0_IRQ_PRIORITY,SPI1_IRQ_PRIORITY,SPI2_IRQ_PRIORITY,SPI3_IRQ_PRIORITY,SPI4_IRQ_PRIORITY,SPI5_IRQ_PRIORITY

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _SPI_DEV_H_ */
