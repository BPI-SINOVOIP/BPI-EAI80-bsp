;*********************************************************************************
; @file
; @brief ARM Cortex-M exception/interrupt exit API
;
;
; Provides functions for performing kernel handling when exiting exceptions or
; interrupts that are installed directly in the vector table (i.e. that are not
; wrapped around by _isr_wrapper()).
;*********************************************************************************

	GET inc\config.inc
	GET inc\offset.inc
	GET inc\memory.inc
;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

	IMPORT  g_kernel[DATA]
	IF :DEF:CONFIG_TIMESLICING
	IMPORT  _update_time_slice_before_swap[CODE]
	ENDIF
	

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT _ExcExit[WEAK,CODE]    
    EXPORT  _IntExit[WEAK,CODE]
		
;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text._HandlerModeExit|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8
		
;*********************************************************************************
;
; @brief Kernel housekeeping when exiting interrupt handler installed
;            directly in vector table
;
; Kernel allows installing interrupt handlers (ISRs) directly into the vector
; table to get the lowest interrupt latency possible. This allows the ISR to be
; invoked directly without going through a software interrupt table. However,
; upon exiting the ISR, some kernel work must still be performed, namely
; possible context switching. While ISRs connected in the software interrupt
; table do this automatically via a wrapper, ISRs connected directly in the
; vector table must invoke _IntExit() as the *very last* action before
; returning.
;
; e.g.
;
; void myISR(void)
;     {
;     printk("in %s\n", __FUNCTION__);
;     doStuff();
;     _IntExit();
;     }
;
; @return N/A
;*********************************************************************************

_IntExit

; _IntExit falls through to _ExcExit (they are aliases of each other)

;*********************************************************************************
;
; @brief Kernel housekeeping when exiting exception handler installed
;            directly in vector table
;
; See _IntExit().
;
; @return N/A
;*********************************************************************************

_ExcExit

	IF :DEF:CONFIG_PREEMPT_ENABLED
    LDR R0, =g_kernel

    LDR R1, [R0, #_kernel_offset_to_current]

    LDR R0, [R0, #_kernel_offset_to_ready_q_cache]
    CMP R0, R1
    BEQ _EXIT_EXC

	IF :DEF:CONFIG_TIMESLICING
    PUSH {LR}
    BL _update_time_slice_before_swap
	
    POP {LR}
	ENDIF

    ; context switch required, pend the PendSV exception
    LDR R1, =_SCS_ICSR
    LDR R2, =_SCS_ICSR_PENDSV
    STR R2, [R1]

_ExcExitWithGdbStub

_EXIT_EXC
	ENDIF

	IF :DEF:CONFIG_STACK_SENTINEL
    PUSH {LR}
    BL _check_stack_sentinel

    POP {LR}
	ENDIF ; CONFIG_STACK_SENTINEL

    BX LR
	
	ALIGN
	END
