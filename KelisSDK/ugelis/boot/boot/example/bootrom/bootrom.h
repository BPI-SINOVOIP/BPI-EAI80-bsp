/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            bootrom.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/07/16 
 *    
 * @brief           This file implements IAP, IO access/modify in bootrom through UART0.
 * 
 * @note
 *    2016/07/18, terry.wu, V1.0.0
 *        Initial version.
 */

#ifndef _BOOTROM_H_
#define _BOOTROM_H_


typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;




#define INFO_ADDRESS_IN_FLASH   0x10000000

typedef struct
{
    uint32_t part_num;				//0x00000003
    uint32_t info_addr;				//0x10000000
    uint32_t info_size;				//0x00001000
    uint32_t info_load_addr;		//0x00000000
    uint32_t boot1_addr;			//0x10001000
    uint32_t boot1_size;			//0x00002e7c(not fixed)
    uint32_t boot1_load_addr;		//0x20001000
    uint32_t system_addr;			//0x10010000
    uint32_t system_size;			//0x00002cf0(not fixed)
    uint32_t system_load_addr;		//0xc0010000
}info_t;


#if 1
typedef struct
{
  volatile uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  volatile uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
  volatile uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
  volatile uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

#define SysTick_BASE                        (0xE000E010UL)                  /*!< SysTick Base Address */
#define SysTick                             ((SysTick_Type *) SysTick_BASE)   /*!< SysTick configuration struct */
#define SysTick_CTRL_ENABLE_Pos             (0U)                                            /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk             (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)           /*!< SysTick CTRL: ENABLE Mask */
#define SysTick_CTRL_CLKSOURCE_Pos          (2)                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk          (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */
#define SysTick_CTRL_TICKINT_Pos            (1U)                                            /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk            (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */



#endif

#define GET_WORD(reg)        (*((volatile uint32_t *)(reg)))
#define SET_WORD(reg, val)   (*((volatile uint32_t *)(reg)) = (val))
#define SET_BIT(reg, bit)    (*((volatile uint32_t *)(reg)) |= (bit))
#define CLEAR_BIT(reg, bit)  (*((volatile uint32_t *)(reg)) &= ~(bit))




/** Base addresses of UART Control Registers */
#define UART0_BASE      (0x40004000UL)
#define UART_DR         0x0
#define UART_BARD_DIV   0x04
#define UART_C3         0x10
#define UART_LS         0x14
#define UART_IE         0x1C

#define UART_RESET_BASE 0x40000078
#define UART_CLOCK_BASE 0x40000088

/** Data Ready */
#define UART_LS_DR      ((uint32_t)0x00000020)
/** UART Transmitter Holding Register is Empty */
#define UART_LS_THRE    ((uint32_t)0x00000040)


#define SYS_CLOCK            8000000//48000000

#define SYSTICK_US_TICKS     ((SYS_CLOCK)/1000000)
#define SYSTICK_MS_TICKS     ((SYS_CLOCK)/1000)
#define SYSTICK_RELOAD_VALUE SYSTICK_MS_TICKS
#define SysTick_IRQn         -1



/** CRC base address */
#define CRC_BASE        (0x4000a000UL)
#define CRC_CTRL        0x0
#define CRC_POL         0x04
#define CRC_INIT        0x08
#define CRC_DATA        0x0C
#define CRC_XOR         0x10
#define CRC_RESULT      0x14
#define CRC_STATUS      0x18

void UART_PutChar(uint8_t data);
uint8_t UART_GetChar(uint32_t tmoMs);
void FLASH_MassErase(void);
void FLASH_Program(uint32_t addr, uint32_t *pData, uint32_t len);
uint32_t CRC_Calculate8Bit(uint8_t * buf, uint32_t len);
void SysTick_DelayMS(uint32_t ms);

#endif


