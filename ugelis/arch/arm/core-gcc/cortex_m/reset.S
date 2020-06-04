/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Reset handler
 *
 * Reset handler that prepares the system for running C code.
 */

#include <board.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <arch/cpu.h>
#include <offsets_short.h>
#include "vector_table.h"

_ASM_FILE_PROLOGUE

GTEXT(__reset)
GTEXT(memset)
GDATA(_interrupt_stack)
#if defined(CONFIG_PLATFORM_SPECIFIC_INIT)
GTEXT(_PlatformInit)
#endif

#define MSP_SWITCH_TO_PSP_DEBUG  1
#define DOWNLOAD_DEBUG_CHECK     1

/**
 *
 * @brief Reset vector
 *
 * Ran when the system comes out of reset. The processor is in thread mode with
 * privileged level. At this point, the main stack pointer (MSP) is already
 * pointing to a valid area in SRAM.
 *
 * Locking interrupts prevents anything but NMIs and hard faults from
 * interrupting the CPU. A default NMI handler is already in place in the
 * vector table, and the boot code should not generate hard fault, or we're in
 * deep trouble.
 *
 * We want to use the process stack pointer (PSP) instead of the MSP, since the
 * MSP is to be set up to point to the one-and-only interrupt stack during later
 * boot. That would not be possible if in use for running C code.
 *
 * When these steps are completed, jump to _PrepC(), which will finish setting
 * up the system for running C code.
 *
 * @return N/A
 */

SECTION_SUBSEC_FUNC(TEXT,_reset_section,__reset)

/*
 * The entry point is located at the __reset symbol, which
 * is fetched by a XIP image playing the role of a bootloader, which jumps to
 * it, not through the reset vector mechanism. Such bootloaders might want to
 * search for a __start symbol instead, so create that alias here.
 */
SECTION_SUBSEC_FUNC(TEXT,_reset_section,__start)

#if defined(CONFIG_PLATFORM_SPECIFIC_INIT)
    bl _PlatformInit
#endif

    /* lock interrupts: will get unlocked when switch to main task */
#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
    cpsid i
#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    movs.n r0, #_EXC_IRQ_DEFAULT_PRIO
    msr BASEPRI, r0
#else
#error Unknown ARM architecture
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */

#ifdef CONFIG_WDOG_INIT
    /* board-specific watchdog initialization is necessary */
    bl _WdogInit
#endif

#ifdef CONFIG_INIT_STACKS
    ldr r0, =_interrupt_stack
    ldr r1, =0xaa
    ldr r2, =CONFIG_ISR_STACK_SIZE
    bl memset
#endif

    /*
     * Set PSP and use it to boot without using MSP, so that it
     * gets set to _interrupt_stack during initialisation.
     */
    ldr r0, =_interrupt_stack
    ldr r1, =CONFIG_ISR_STACK_SIZE
    adds r0, r0, r1
    msr PSP, r0
    movs.n r0, #2	/* switch to using PSP (bit1 of CONTROL reg) */
    msr CONTROL, r0
    /*
     * When changing the stack pointer, software must use an ISB instruction
     * immediately after the MSR instruction. This ensures that instructions
     * after the ISB instruction execute using the new stack pointer.
    */
    isb


/*only gm6721xx do cache config*/
#ifdef CONFIG_GM_HAL_CACHE

#ifdef CONFIG_SDRAM_BOOT
    /*code:sdram data:sdram : i/d cache enable*/
    bl HAL_DCACHE_InvalidAll
    bl HAL_DCACHE_Enable
    bl HAL_ICACHE_Enable
    bl HAL_ICACHE_Invalid
#elif CONFIG_XIP
    /*code:flash data:sram : icache enbale dcache disable*/
    bl HAL_ICACHE_Enable
    bl HAL_ICACHE_Invalid
    bl HAL_DCACHE_Disable
#else
    /*code:sram data:sram : icache disable dcache disable*/
    bl HAL_ICACHE_Disable
    bl HAL_DCACHE_Disable
#endif

#endif


#ifdef  DOWNLOAD_DEBUG_CHECK
 /**
  * boot crash check
  * check wethter cpu enter hander mode,
  * if so ,  boot sel issue.
  * you should set boot from opt
  * or you can try to compier your code to sdram
  * if you luckly enough,maybe it can boot normaly;
  *
  *               asm code mean
  * if( (*((uint32_t *)(0xe000ed04))) & (1<<11) )
  *  {
  *        while(1);//cpu enter hander mode,dead loop here for debug
  *  }
  *
  */
    ldr r1, =0xe000ed04
    ldr r0, [r1]
    ldr r2, =0x800
    and r0, r2
    cmp r0 ,#0
    beq normal_boot
    b .
#endif


/*
 * IDE Maybe switch msp to psp fail
 * check here,only for develop debug.
 * release
 * code will remove it
 */
 #ifdef  MSP_SWITCH_TO_PSP_DEBUG
    ldr r1, =_interrupt_stack
    ldr r0, =CONFIG_ISR_STACK_SIZE
    adds r1, r1, r0

    mrs r0, PSP

    cmp r1, r0
    beq normal_boot
    b .
#endif

normal_boot:

    b _PrepC

