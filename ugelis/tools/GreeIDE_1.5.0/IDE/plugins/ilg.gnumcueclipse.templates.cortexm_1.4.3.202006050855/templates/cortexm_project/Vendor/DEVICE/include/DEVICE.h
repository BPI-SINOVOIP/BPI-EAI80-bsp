/**************************************************************************//**
 * @file     <Device>.h
 * @brief    CMSIS Cortex-M# Core Peripheral Access Layer Header File for
 *           Device <Device>
 * @version  V3.10
 * @date     23. November 2012
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2012 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#ifndef Device_H      /* ToDo: replace '<Device>' with your device name */
#define Device_H

#ifdef __cplusplus
 extern "C" {
#endif

/* ToDo: replace '<Device>' with your device name; add your doxyGen comment   */
/** @addtogroup <Device>_Definitions <Device> Definitions
  This file defines all structures and symbols for <Device>:
    - registers and bitfields
    - peripheral base address
    - peripheral ID
    - Peripheral definitions
  @{
*/


/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup <Device>_CMSIS Device CMSIS Definitions
  Configuration of the Cortex-M# Processor and Core Peripherals
  @{
*/

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex-M# Processor Exceptions Numbers ***************************************************/

#if defined(__ARM_ARCH_6M__)

/* ToDo: use this Cortex interrupt numbers if your device is a CORTEX-M0 device                   */
  NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt                        */
  HardFault_IRQn                = -13,      /*!<  3 Hard Fault Interrupt                          */
  SVCall_IRQn                   = -5,       /*!< 11 SV Call Interrupt                             */
  PendSV_IRQn                   = -2,       /*!< 14 Pend SV Interrupt                             */
  SysTick_IRQn                  = -1,       /*!< 15 System Tick Interrupt                         */

#elif defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

/* ToDo: use this Cortex interrupt numbers if your device is a CORTEX-M3 / Cortex-M4 device       */
  NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt                        */
  MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt                   */
  BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt                           */
  UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt                         */
  SVCall_IRQn                   = -5,       /*!< 11 SV Call Interrupt                             */
  DebugMonitor_IRQn             = -4,       /*!< 12 Debug Monitor Interrupt                       */
  PendSV_IRQn                   = -2,       /*!< 14 Pend SV Interrupt                             */
  SysTick_IRQn                  = -1,       /*!< 15 System Tick Interrupt                         */

#endif

/******  Device Specific Interrupt Numbers ********************************************************/
/* ToDo: add here your device specific external interrupt numbers
         according the interrupt handlers defined in startup_Device.s
         eg.: Interrupt for Timer#1       TIM1_IRQHandler   ->   TIM1_IRQn                        */
  DeviceInterrupt_IRQn        = 0,        /*!< Device Interrupt                                 */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M# Processor and Core Peripherals */
/* ToDo: set the defines according your Device                                                    */
/* ToDo: define the correct core revision
         __CM0_REV if your device is a CORTEX-M0 device
         __CM3_REV if your device is a CORTEX-M3 device
         __CM4_REV if your device is a CORTEX-M4 device                                           */
//#define __CM#_REV                 0x0201    /*!< Core Revision r2p1                               */
#if defined(__ARM_ARCH_6M__)
#define __CM0_REV                 0x0201
#elif defined(__ARM_ARCH_7M__)
#define __CM3_REV                 0x0201
#elif defined(__ARM_ARCH_7EM__)
#define __CM4_REV                 0x0201
#endif

#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */
/* ToDo: define __FPU_PRESENT if your devise is a CORTEX-M4                                       */
#define __FPU_PRESENT             0        /*!< FPU present or not                                */

/*@}*/ /* end of group <Device>_CMSIS */


/* ToDo: include the correct core_cm#.h file
         core_cm0.h if your device is a CORTEX-M0 device
         core_cm3.h if your device is a CORTEX-M3 device
         core_cm4.h if your device is a CORTEX-M4 device                                          */

//#include <core_cm#.h>                       /* Cortex-M# processor and core peripherals           */
#if defined(__ARM_ARCH_6M__)
#include <core_cm0.h>
#elif defined(__ARM_ARCH_7M__)
#include <core_cm3.h>
#elif defined(__ARM_ARCH_7EM__)
#include <core_cm4.h>
#endif

/* ToDo: include your system_<Device>.h file
         replace '<Device>' with your device name                                                 */
#include "system_$(deviceName).h"                /* <Device> System  include file                      */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/
/** @addtogroup <Device>_Peripherals <Device> Peripherals
  <Device> Device Specific Peripheral registers structures
  @{
*/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/* ToDo: add here your device specific peripheral access structure typedefs
         following is an example for a timer                                  */

/*------------- 16-bit Timer/Event Counter (TMR) -----------------------------*/
/** @addtogroup <Device>_TMR <Device> 16-bit Timer/Event Counter (TMR)
  @{
*/
typedef struct
{
  __IO uint32_t EN;                         /*!< Offset: 0x0000   Timer Enable Register           */
  __IO uint32_t RUN;                        /*!< Offset: 0x0004   Timer RUN Register              */
  __IO uint32_t CR;                         /*!< Offset: 0x0008   Timer Control Register          */
  __IO uint32_t MOD;                        /*!< Offset: 0x000C   Timer Mode Register             */
       uint32_t RESERVED0[1];
  __IO uint32_t ST;                         /*!< Offset: 0x0014   Timer Status Register           */
  __IO uint32_t IM;                         /*!< Offset: 0x0018   Interrupt Mask Register         */
  __IO uint32_t UC;                         /*!< Offset: 0x001C   Timer Up Counter Register       */
  __IO uint32_t RG0;                        /*!< Offset: 0x0020   Timer Register                  */
       uint32_t RESERVED1[2];
  __IO uint32_t CP;                         /*!< Offset: 0x002C   Capture register                */
} DeviceAbbreviation_TMR_TypeDef;
/*@}*/ /* end of group <Device>_TMR */


#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

/*@}*/ /* end of group <Device>_Peripherals */


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* ToDo: add here your device peripherals base addresses
         following is an example for timer                                    */
/** @addtogroup <Device>_MemoryMap <Device> Memory Mapping
  @{
*/

/* Peripheral and SRAM base address */
#define DeviceAbbreviation_FLASH_BASE       (0x00000000UL)                              /*!< (FLASH     ) Base Address */
#define DeviceAbbreviation_SRAM_BASE        (0x20000000UL)                              /*!< (SRAM      ) Base Address */
#define DeviceAbbreviation_PERIPH_BASE      (0x40000000UL)                              /*!< (Peripheral) Base Address */

/* Peripheral memory map */
#define DeviceAbbreviationTIM0_BASE         (DeviceAbbreviation_PERIPH_BASE)          /*!< (Timer0    ) Base Address */
#define DeviceAbbreviationTIM1_BASE         (DeviceAbbreviation_PERIPH_BASE + 0x0800) /*!< (Timer1    ) Base Address */
#define DeviceAbbreviationTIM2_BASE         (DeviceAbbreviation_PERIPH_BASE + 0x1000) /*!< (Timer2    ) Base Address */
/*@}*/ /* end of group <Device>_MemoryMap */


/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
/* ToDo: add here your device peripherals pointer definitions
         following is an example for timer                                    */

/** @addtogroup <Device>_PeripheralDecl <Device> Peripheral Declaration
  @{
*/

#define DeviceAbbreviation_TIM0        ((DeviceAbbreviation_TMR_TypeDef *) DeviceAbbreviationTIM0_BASE)
#define DeviceAbbreviation_TIM1        ((DeviceAbbreviation_TMR_TypeDef *) DeviceAbbreviationTIM0_BASE)
#define DeviceAbbreviation_TIM2        ((DeviceAbbreviation_TMR_TypeDef *) DeviceAbbreviationTIM0_BASE)
/*@}*/ /* end of group <Device>_PeripheralDecl */

/*@}*/ /* end of group <Device>_Definitions */

#ifdef __cplusplus
}
#endif

#endif  /* <Device>_H */
