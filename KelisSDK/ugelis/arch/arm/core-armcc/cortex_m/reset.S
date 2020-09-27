;*********************************************************************************
; @file
; @brief Reset handler
;
; Reset handler that prepares the system for running C code.
;*********************************************************************************
;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

	GET		..\inc\config.inc
	GET		..\inc\board.inc
	IMPORT  _interrupt_stack[CODE]
    IMPORT  HAL_DCACHE_InvalidAll
    IMPORT  HAL_DCACHE_Enable
    IMPORT  HAL_DCACHE_Disable
    IMPORT  HAL_ICACHE_Enable
    IMPORT  HAL_ICACHE_Disable
    IMPORT  HAL_ICACHE_Invalid
    IMPORT  _PrepC[CODE]

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT  __reset[CODE]
    EXPORT  __start[CODE]	
	EXPORT  __initial_sp
	EXPORT  __heap_base
	EXPORT  __heap_limit
		
Stack_Size		EQU     0x400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size      EQU     0x200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


;******************************************************************************
;                            MACRO DEFINITION
;******************************************************************************

MSP_SWITCH_TO_PSP_DEBUG			EQU		1
DOWNLOAD_DEBUG_CHECK			EQU		1
		
;*********************************************************************************
;
; @brief Reset vector
;
; Ran when the system comes out of reset. The processor is in thread mode with
; privileged level. At this point, the main stack pointer (MSP) is already
; pointing to a valid area in SRAM.
;
; Locking interrupts prevents anything but NMIs and hard faults from
; interrupting the CPU. A default NMI handler is already in place in the
; vector table, and the boot code should not generate hard fault, or we're in
; deep trouble.
;
; We want to use the process stack pointer (PSP) instead of the MSP, since the
; MSP is to be set up to point to the one-and-only interrupt stack during later
; boot. That would not be possible if in use for running C code.
;
; When these steps are completed, jump to _PrepC(), which will finish setting
; up the system for running C code.
;
; @return N/A
;*********************************************************************************

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text._reset_section|, CODE, READONLY, ALIGN=4
    THUMB
    REQUIRE8
    PRESERVE8
__reset

;*********************************************************************************
; The entry point is located at the __reset symbol, which
; is fetched by a XIP image playing the role of a bootloader, which jumps to
; it, not through the reset vector mechanism. Such bootloaders might want to
; search for a __start symbol instead, so create that alias here.
__start
	IF :DEF:CONFIG_PLATFORM_SPECIFIC_INIT
	BL _PlatformInit
	ENDIF

    ; lock interrupts: will get unlocked when switch to main task
    MOVS.N R0, #_EXC_IRQ_DEFAULT_PRIO
    MSR BASEPRI, R0

	IF :DEF:CONFIG_INIT_STACKS
    LDR R0, =_interrupt_stack
    LDR R1, =0xaa
    LDR R2, =CONFIG_ISR_STACK_SIZE
    BL memset
	ENDIF

    ;*********************************************************************************
    ; Set PSP and use it to boot without using MSP, so that it
    ; gets set to _interrupt_stack during initialisation.
    ;*********************************************************************************
    LDR R0, =_interrupt_stack
    LDR R1, =CONFIG_ISR_STACK_SIZE
    ADDS R0, R0, R1
    MSR PSP, R0
    MOVS.N R0, #2	; switch to using PSP (bit1 of CONTROL reg)
    MSR CONTROL, R0
    ;*********************************************************************************
    ; When changing the stack pointer, software must use an ISB instruction
    ; immediately after the MSR instruction. This ensures that instructions
    ; after the ISB instruction execute using the new stack pointer.
    ;*********************************************************************************
    ISB


;only gm6721xx do cache config
	IF :DEF:CONFIG_GM_HAL_CACHE

	IF :DEF:CONFIG_SDRAM_BOOT
    ;code:sdram data:sdram : i/d cache enable
    BL HAL_DCACHE_InvalidAll
    BL HAL_DCACHE_Enable
    BL HAL_ICACHE_Enable
    BL HAL_ICACHE_Invalid
	ELIF :DEF:CONFIG_XIP
    ;code:flash data:sram : icache enbale dcache disable
    BL HAL_ICACHE_Enable
    BL HAL_ICACHE_Invalid
    BL HAL_DCACHE_Disable
	ELSE
    ;code:sram data:sram : icache disable dcache disable
    BL HAL_ICACHE_Disable
    BL HAL_DCACHE_Disable
	ENDIF

	ENDIF


	IF :DEF:DOWNLOAD_DEBUG_CHECK
 ;
 ; boot crash check
 ; check wethter cpu enter hander mode,
 ; if so ,  boot sel issue.
 ; you should set boot from opt
 ; or you can try to compier your code to sdram
 ; if you luckly enough,maybe it can boot normaly;
 ;
 ;               asm code mean
 ; if( (*((uint32_t *)(0xe000ed04))) & (1<<11) )
 ;  {
 ;        while(1);//cpu enter hander mode,dead loop here for debug
 ;  }
 ;
 ;
    LDR R1, =0xe000ed04
    LDR R0, [R1]
    LDR R2, =0x800
    AND R0, R2
    CMP R0 ,#0
    BEQ normal_boot
    B .
	ENDIF


;
; IDE Maybe switch msp to psp fail
; check here,only for develop debug.
; release
; code will remove it
;
	IF :DEF:MSP_SWITCH_TO_PSP_DEBUG
    LDR R1, =_interrupt_stack
    LDR R0, = CONFIG_ISR_STACK_SIZE
    ADDS R1, R1, R0

    MRS R0, PSP

    CMP R1, R0
    BEQ normal_boot
    B .
	ENDIF

normal_boot
    B _PrepC
	
	ALIGN
    END
