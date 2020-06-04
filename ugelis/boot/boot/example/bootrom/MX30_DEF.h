/*
 * COPYRIGHT (c) 2010-2016 MACRONIX INTERNATIONAL CO., LTD
 * MX30-series Parallel Flash Low Level Driver (LLD) Sample Code
 *
 * Flash device information.
 * This sample code provides a reference, not recommand for directing using.
 *
 * $Id: MX30_DEF.h,v 1.52 2016/03/16 09:44:14 mxclldb1 Exp $
 */
#ifndef __MX30_DEF_H__
#define __MX30_DEF_H__

//#include <8051.h>  /* Use MCU8051 */

/* BOOL Definition */
#define  TRUE   1
#define  FALSE  0

/* System Configuration */
#define  GPIO_8051  1
#define  Timer_8051 0

#define  NF_FLASH_WRITE_DMA_PIO 1
#define  NF_FLASH_WRITE_DMA_DMA 1
#define  NF_FLASH_READ_DMA_PIO 1
#define  NF_FLASH_READ_DMA_DMA 1

#define TOSHIBA_CHIP 0

//#define  NonSynchronousIO  1

/* Target Flash Device: MX30XXXX */
#define MX30LF4G18AC 

/* Timer Parameter */
#define  TIMEOUT    0
#define  TIMENOTOUT 1
#define  TPERIOD    240             // ns, The time period of timer count one
#define  COUNT_ONE_MICROSECOND  16  // us, The loop count value within one micro-second

/* Flash Register Parameter */

// system flags
#define  PASS        0
#define  FAIL        1
#define  BUSY        0
#define  READY       1
#define  PROTECTED   0
#define  UNPROTECTED 1




// status register [7:0]
#define  SR0_ChipStatus         0x01
#define  SR1_CacheProgramResult 0x02
// SR[2] - SR[4] not used 
#define  SR5_ReadyBusyForCtrl   0x20
#define  SR6_ReadyBusy          0x40
#define  SR7_WriteProtect       0x80
#ifdef TOSHIBA_CHIP
#define  NF_RESET_FLAG          0xA0
#else
#define  NF_RESET_FLAG          0xE0
#endif


/* Device Parameter ( Time uint: us, Condition: worst case )
   Please refer to data sheet for advanced information. */

//Define all the register of NF I/O control, which will use in HAL Device
#define     NF_CTRL_MMAP_BASE            0x70000000
#define     NF_VER                       NF_CTRL_MMAP_BASE
#define     NF_MR                        (NF_CTRL_MMAP_BASE+0x10)
#define     NF_CR                        (NF_CTRL_MMAP_BASE+0x14)     //NF Control Register
#define     NF_ECC_CR                    (NF_CTRL_MMAP_BASE+0x18)
#define     NF_DMA_CR                    (NF_CTRL_MMAP_BASE+0x1C)
#define     NF_PIO_DR                    (NF_CTRL_MMAP_BASE+0x20)     //NF PIO Data Register
#define     NF_DMA_DR                    (NF_CTRL_MMAP_BASE+0x24)     //NF DMA Data Register
#define     NF_RD_TIM                    (NF_CTRL_MMAP_BASE+0x30)
#define     NF_WR_TIM                    (NF_CTRL_MMAP_BASE+0x34)
#define     NF_EDO_RB_TIM                (NF_CTRL_MMAP_BASE+0x38)
#define     NF_DQS_TIM                   (NF_CTRL_MMAP_BASE+0x3C)
#define     FW_WR_REDU_W0                (NF_CTRL_MMAP_BASE+0x40)
#define     FW_WR_REDU_W1                (NF_CTRL_MMAP_BASE+0x44)
#define     NF_SR                        (NF_CTRL_MMAP_BASE+0x50)
#define     NF_ECC_SR                    (NF_CTRL_MMAP_BASE+0x54)
#define     NF_SEC_ERR_INFO              (NF_CTRL_MMAP_BASE+0x58)
#define     FW_RD_REDU_W0                (NF_CTRL_MMAP_BASE+0x60)
#define     FW_RD_REDU_W1                (NF_CTRL_MMAP_BASE+0x64)
#define     NF_LP_CR                     (NF_CTRL_MMAP_BASE+0x70)
#define     NF_SRAM_STR                  (NF_CTRL_MMAP_BASE+0x2000)

//Define the value of the IO register
#define     VAL_NF_CTRL_MMAP_BASE         *(volatile uint32_t *)NF_CTRL_MMAP_BASE
#define     VAL_NF_VER                    *(volatile uint32_t *)NF_VER
#define     VAL_NF_MR                     *(volatile uint32_t *)NF_MR
#define     VAL_NF_CR                     *(volatile uint32_t *)NF_CR
#define     VAL_NF_CEJ                    *(volatile uint8_t *)(NF_CR)
#define     VAL_NF_ALE                    *(volatile uint8_t *)(NF_CR+1)
#define     VAL_NF_CLE                    *(volatile uint8_t *)(NF_CR+2)

#define     VAL_NF_ECC_CR                 *(volatile uint32_t *)NF_ECC_CR


#define     VAL_NF_ECC_RST                *(volatile uint8_t *)(NF_ECC_CR)
#define     VAL_NF_ECC_MODE               *(volatile uint8_t *)(NF_ECC_CR+1)
#define     VAL_NF_BCH_MODE               *(volatile uint8_t *)(NF_ECC_CR+2)

#define     VAL_NF_DMA_CR                 *(volatile uint32_t *)NF_DMA_CR
#define     VAL_NF_DMA_CTRL               *(volatile uint8_t *)(NF_DMA_CR+1)
#define     VAL_NF_DMA_CFG                *(volatile uint8_t *)(NF_DMA_CR+2)

#define     VAL_NF_PIO_DR                 *(volatile uint8_t *)NF_PIO_DR
#define     VAL_NF_DMA_DR                 *(volatile uint32_t *)NF_DMA_DR
#define     VAL_NF_RD_TIM                 *(volatile uint8_t *)NF_RD_TIM
#define     VAL_NF_WR_TIM                 *(volatile uint8_t *)NF_WR_TIM
#define     VAL_NF_EDO_RB_TIM             *(volatile uint8_t *)NF_EDO_RB_TIM
#define     VAL_NF_DQS_TIM                *(volatile uint32_t *)NF_DQS_TIM
#define     VAL_FW_WR_REDU_W0             *(volatile uint32_t *)FW_WR_REDU_W0
#define     VAL_FW_WR_REDU_W1             *(volatile uint32_t *)FW_WR_REDU_W1
#define     VAL_NF_SR                     *(volatile uint32_t *)NF_SR
#define     VAL_NF_SR0                    *(volatile uint8_t *)NF_SR
#define     VAL_NF_SR1                    *(volatile uint8_t *)(NF_SR+1)
#define     VAL_NF_SR2                    *(volatile uint8_t *)(NF_SR+2)

#define     VAL_NF_ECC_SR                 *(volatile uint8_t *)NF_ECC_SR
#define     VAL_NF_SEC_ERR_INFO           *(volatile uint32_t *)NF_SEC_ERR_INFO
#define     VAL_FW_RD_REDU_W0             *(volatile uint32_t *)FW_RD_REDU_W0
#define     VAL_FW_RD_REDU_W1             *(volatile uint32_t *)FW_RD_REDU_W1
#define     VAL_NF_LP_CR                  *(volatile uint32_t *)NF_LP_CR
#define     VAL_NF_SRAM_STR               *(volatile uint32_t *)NF_SRAM_STR


//define the command of NF control


//define the bits of the control


#ifdef MX30LF4G18AC        
#define ID_CODE0              0xc2
#define ID_CODE1              0xdc
#define ID_CODE2              0x90
#define ID_CODE3              0x95
#define ID_CODE4              0x56
#define READ_ID_5BYTE         0
#define FLASH_SIZE            0x40000000
#define FLASH_TIMEOUT_VALUE   3000
#define FIRST_BYTE_LATENCY_tR 25
#define tRCBSY                25
#define tDBSY                 1
#define tCBSY                 600
#define BP_PROT_MODE
#endif



/* Basic Data Type Definition */
typedef  unsigned char  BOOL;
typedef  unsigned char  uint8;
typedef  unsigned int   uint16;
typedef  unsigned long  uint32;
typedef  uint32  uAddr;
#ifdef X16_DEVICE
	typedef  uint16  uBusWidth;
	#define ADDRESS_OFFSET 1
#else
	typedef  uint8  uBusWidth;
	#define ADDRESS_OFFSET 0
#endif

/*
    Address convert (MSB)
    The NAND flash address reserved A15-A12
    User can switch Address_Convert option
    to on/off address convert, default is off
*/
#define  Address_Convert   0

#if 0//def Address_Convert
    //32 bit( x8 mode): BYTE3 | BYTE2 | {4'b0, BYTE1} | BYTE0
    //32 bit(x16 mode): BYTE3 | BYTE2 | {5'b0, BYTE1} | BYTE0
    #define BYTE0_OFFSET  0
    #define BYTE1_MASK   (0x0F >> ADDRESS_OFFSET)  // Mask address A15-A12 of BYTE1
    #define BYTE1_OFFSET  8
    #define BYTE2_OFFSET 12 - ADDRESS_OFFSET
    #define BYTE3_OFFSET 20 - ADDRESS_OFFSET
    #define BYTE4_OFFSET 28 - ADDRESS_OFFSET
    #define PAGE_MASK    (0xFFF >> ADDRESS_OFFSET)
#else
    //32 bit: BYTE3 | BYTE2 | BYTE1 | BYTE0
    #define BYTE0_OFFSET  0
    #define BYTE1_MASK   0xFF
    #define BYTE1_OFFSET  8
    #define BYTE2_OFFSET 16
    #define BYTE3_OFFSET 24
    #define BYTE4_OFFSET 32
    #define PAGE_MASK    0xFFFF
#endif
#define BYTE_MASK 0xFF

/* GPIO port mapping */
#if 0//  def GPIO_8051

#define  IN_VALID       P3_0
#define  OUT_VALID      P3_1
#define  WRITE_EN       P3_2
#define  DUMP_SIG       P3_3
#define  CTRL0          P3_4
#define  CTRL1          P3_5

#define CLE_LOW();  CTRL1 = 0; CTRL0 = 0;
#define CLE_HIGH(); CTRL1 = 1; CTRL0 = 0;

#define ALE_LOW();  CTRL1 = 0; CTRL0 = 0;
#define ALE_HIGH(); CTRL1 = 0; CTRL0 = 1;

#define  toggle(); IN_VALID = 1; \
                   IN_VALID = 0;

#define  dump_sig(); DUMP_SIG = 1; \
                     DUMP_SIG = 0;
#endif  /* End of GPIO_8051 */

/* Program Parameter */
#ifdef READ_ID_5BYTE
static const uint8 ID_CODE[5] = {ID_CODE0, ID_CODE1, ID_CODE2, ID_CODE3, ID_CODE4};
#else
static const uint8 ID_CODE[4] = {ID_CODE0, ID_CODE1, ID_CODE2, ID_CODE3};
#endif

static const uint8 ONFI_PARA_SIGN[] = { 0x4F, 0x4E, 0x46, 0x49 };

#endif  // end of __MX30_DEF_H__

