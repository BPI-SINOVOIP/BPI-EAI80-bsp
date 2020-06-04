;*********************************************************************************
; @file
; @brief ARM Cortex-M wrapper for ISRs with parameter
;
; Wrapper installed in vector table for handling dynamic interrupts that accept
; a parameter.
;*********************************************************************************
	
	GET inc\config.inc
	GET inc\offset.inc
;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

    IMPORT _sw_isr_table[DATA]
	IMPORT _IntExit[CODE]

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT _isr_wrapper[CODE]
		
;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;*********************************************************************************
;
; @brief Wrapper around ISRs when inserted in software ISR table
;
; When inserted in the vector table, _isr_wrapper() demuxes the ISR table using
; the running interrupt number as the index, and invokes the registered ISR
; with its corresponding argument. When returning from the ISR, it determines
; if a context switch needs to happen (see documentation for __pendsv()) and
; pends the PendSV exception if so: the latter will perform the context switch
; itself.
;
; @return N/A
;*********************************************************************************
_isr_wrapper
	PUSH {LR}		; lr is now the first item on the stack
	IF      :DEF:CONFIG_EXECUTION_BENCHMARKING
	BL read_timer_start_of_isr
	ENDIF
	IF :DEF:CONFIG_KERNEL_EVENT_LOGGER_INTERRUPT
	BL _sys_k_event_logger_interrupt
	ENDIF
	IF :DEF:CONFIG_KERNEL_EVENT_LOGGER_SLEEP
	BL _sys_k_event_logger_exit_sleep
	ENDIF
	IF :DEF:CONFIG_SYS_POWER_MANAGEMENT
	 ;*********************************************************************************
	 ; All interrupts are disabled when handling idle wakeup.  For tickless
	 ; idle, this ensures that the calculation and programming of the device
	 ; for the next timer deadline is not interrupted.  For non-tickless idle,
	 ; this ensures that the clearing of the kernel idle state is not
	 ; interrupted.  In each case, _sys_power_save_idle_exit is called with
	 ; interrupts disabled.
	 ;*********************************************************************************
	CPSID I  ; PRIMASK = 1

	; is this a wakeup from idle ? 
	LDR R2, =g_kernel
	; requested idle duration, in ticks 
	LDR R0, [R2, #_kernel_offset_to_idle]
	CMP R0, #0

	ITTT NE
	MOVNE	R1, #0
		; clear kernel idle state
		STRNE	R1, [R2, #_kernel_offset_to_idle]
		BLXNE	_sys_power_save_idle_exit

	CPSIE I		; re-enable interrupts (PRIMASK = 0)
	ENDIF

	MRS R0, IPSR	; get exception number

	SUB R0, R0, #16	; get IRQ number
	LSL R0, R0, #3	; table is 8-byte wide
	LDR R1, =_sw_isr_table
	ADD R1, R1, R0	; table entry: ISRs must have their MSB set to stay in thumb mode

	LDM R1!,{R0,R3}	; arg in r0, ISR in r3
	IF :DEF:CONFIG_EXECUTION_BENCHMARKING
	STM SP!,{R0-R3} ; Save r0 to r4 into stack
	PUSH {LR}
	BL read_timer_end_of_isr

	POP {LR}
	LDM SP!,{R0-R3} ; Restore r0 to r4 regs
	ENDIF
	BLX R3		; call ISR

	POP {LR}
	
	; exception return is done in _IntExit()
	B _IntExit
	
	ALIGN
	END
