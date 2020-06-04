;*********************************************************************************
; @file
; @brief Thread context switching for ARM Cortex-M
;
; This module implements the routines necessary for thread context switching
; on ARM Cortex-M CPUs.
;*********************************************************************************

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

	GET inc\config.inc
	GET inc\board.inc
	GET	inc\offset.inc
	GET inc\memory.inc
    IMPORT _do_kernel_oops[CODE]
	IMPORT _k_neg_eagain[DATA]
    IMPORT _irq_do_offload[CODE]
	IMPORT _do_kernel_oops[CODE]
	IMPORT _IntExit[CODE]
	IMPORT g_kernel[DATA]

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT __svc[CODE]
	EXPORT __pendsv[CODE]
    EXPORT fp_register_touch[CODE]

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text|, CODE, READONLY, ALIGN=4
	THUMB
    REQUIRE8
    PRESERVE8



;*********************************************************************************
; Although automatic state preservation is enabled, the processor
; does not automatically save the volatile FP registers until they
; have first been touched. Perform a dummy move operation so that
; the stack frames are created as expected before any thread
; context switching can occur. It has to be surrounded by instruction
; synchronisation barriers to ensure that the whole sequence is
; serialized.
;*********************************************************************************

fp_register_touch        	
    ISB
    VMOV S0, R0
    ISB
    BX      LR

;*********************************************************************************
;
; @brief PendSV exception handler, handling context switches
;
; The PendSV exception is the only execution context in the system that can
; perform context switching. When an execution context finds out it has to
; switch contexts, it pends the PendSV exception.
;
; When PendSV is pended, the decision that a context switch must happen has
; already been taken. In other words, when __pendsv() runs, we *know* we have
; to swap *something*.
;*********************************************************************************

__pendsv
	IF :DEF:CONFIG_KERNEL_EVENT_LOGGER_CONTEXT_SWITCH
    ; Register the context switch
    PUSH {LR}
    BL _sys_k_event_logger_context_switch

    POP {LR}
	ENDIF ; CONFIG_KERNEL_EVENT_LOGGER_CONTEXT_SWITCH

    ; protect the kernel state while we play with the thread lists
    MOVS.N R0, #_EXC_IRQ_DEFAULT_PRIO
    MSR BASEPRI, R0

    ; load g_kernel into r1 and current k_thread into r2
    LDR R1, =g_kernel
    LDR R2, [R1, #_kernel_offset_to_current]

    ; addr of callee-saved regs in thread in r0
    LDR R0, =_thread_offset_to_callee_saved
    ADD R0, R2

    ; save callee-saved + psp in thread
    MRS IP, PSP

    STMIA R0, {R4-R11, IP}
	IF :DEF:CONFIG_FP_SHARING
    ADD R0, R2, #_thread_offset_to_preempt_float
    VSTMIA R0, {S16-S31}
	ENDIF ; CONFIG_FP_SHARING

     ;*********************************************************************************
     ; Prepare to clear PendSV with interrupts unlocked, but
     ; don't clear it yet. PendSV must not be cleared until
     ; the new thread is context-switched in since all decisions
     ; to pend PendSV have been taken with the current kernel
     ; state and this is what we're handling currently.
     ;*********************************************************************************
    LDR R7, =_SCS_ICSR
    LDR R6, =_SCS_ICSR_UNPENDSV

    ; g_kernel is still in r1

    ; fetch the thread to run from the ready queue cache
    LDR R2, [R1, #_kernel_offset_to_ready_q_cache]

    STR R2, [R1, #_kernel_offset_to_current]

     ;*********************************************************************************
     ; Clear PendSV so that if another interrupt comes in and
     ; decides, with the new kernel state baseed on the new thread
     ; being context-switched in, that it needs to reschedules, it
     ; will take, but that previously pended PendSVs do not take,
     ; since they were based on the previous kernel state and this
     ; has been handled.
     ;*********************************************************************************

    ; _SCS_ICSR is still in v4 and _SCS_ICSR_UNPENDSV in v3
    STR R6, [R7, #0]

    ; Restore previous interrupt disable state (irq_lock key)
    LDR R0, [R2, #_thread_offset_to_basepri]
    MOVS.N R3, #0
    STR R3, [R2, #_thread_offset_to_basepri]

    ; restore BASEPRI for the incoming thread
    MSR BASEPRI, R0

	IF :DEF:CONFIG_FP_SHARING
    ADD R0, R2, #_thread_offset_to_preempt_float
    VLDMIA R0, {S16-S31}
	ENDIF

	IF :DEF:CONFIG_MPU_STACK_GUARD
    ; r2 contains k_thread
    ADD R0, R2, #0
    PUSH {R2, LR}
	LDR   R0, =configure_mpu_stack_guard
    BLX  R0   ; call C routine which executes the offload BLX _irq_do_offload
    POP {R2, LR}
	ENDIF ; CONFIG_MPU_STACK_GUARD

	IF :DEF:CONFIG_BUILTIN_STACK_GUARD
    ; R2 contains k_thread
    ADD R0, R2, #0
    PUSH {R2, LR}
    BLX configure_builtin_stack_guard
    POP {R2, LR}
	ENDIF ; CONFIG_BUILTIN_STACK_GUARD

    ; load callee-saved + psp from thread
    ADD R0, R2, #_thread_offset_to_callee_saved
    LDMIA R0, {R4-R11, IP}

    MSR PSP, IP

	IF :DEF:CONFIG_EXECUTION_BENCHMARKING
    STM SP!,{R0-R3} ; Save regs r0 to r4 on stack
    PUSH {LR}
    BL read_timer_end_of_swap

    POP {LR}
    LDM SP!,{R0-R3} ; Load back regs ro to r4
	ENDIF ; CONFIG_EXECUTION_BENCHMARKING

    ; exc return
    BX LR

 ;******************************************************************************
 ;
 ; @brief Service call handler
 ;
 ; The service call (svc) is only used in __swap() to enter handler mode so we
 ; can go through the PendSV exception to perform a context switch.
 ;
 ; @return N/A
 ;******************************************************************************

__svc
    TST LR, #0x4    ; did we come from thread mode ? 
    ITE EQ  ; if zero (equal), came from handler mode
        MRSEQ R0, MSP   ; handler mode, stack frame is on MSP 
        MRSNE R0, PSP   ; thread mode, stack frame is on PSP

    LDR R1, [R0, #24]   ; grab address of PC from stack frame
    ; SVC is a two-byte instruction, point to it and read  encoding 
    LDRH R1, [R1, #-2]

    ;*********************************************************************************
    ; grab service call number:
    ; 0: Unused
    ; 1: irq_offload (if configured)
    ; 2: kernel panic or oops (software generated fatal exception)
    ; 3: System call
    ; Planned implementation of system calls for memory protection will
    ; expand this case.
    ;*********************************************************************************
    ANDS R1, #0xff

    CMP R1, #2
    BEQ _oops

	IF CONFIG_IRQ_OFFLOAD = 1
    PUSH {LR}
	LDR     R0, =_irq_do_offload
    BLX     R0   ; call C routine which executes the offload BLX _irq_do_offload
    POP {LR}

    ; exception return is done in _IntExit()
    B _IntExit
	ENDIF

_oops
    PUSH {LR}
	LDR   R0, =_do_kernel_oops
    BLX   R0 
    POP {PC}
	
	ALIGN
	END

