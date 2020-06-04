;*********************************************************************************
; @file
; @brief Fault handlers for ARM Cortex-M
;
; Fault handlers for ARM Cortex-M processors.
;*********************************************************************************

	GET inc\memory.inc
	GET inc\config.inc
	GET inc\offset.inc
	IMPORT _Fault[CODE]

	EXPORT __hard_fault[CODE]
	EXPORT __mpu_fault[CODE]
	EXPORT __bus_fault[CODE]
	EXPORT __usage_fault[CODE]
	IF :DEF:CONFIG_ARM_SECURE_FIRMWARE
	EXPORT __secure_fault[CODE]
	ENDIF
	EXPORT __debug_monitor[CODE]
	EXPORT __reserved[CODE]

;*********************************************************************************
;
; @brief Fault handler installed in the fault and reserved vectors
;
; Entry point for the hard fault, MPU fault, bus fault, usage fault, debug
; monitor and reserved exceptions.
;
; Save the values of the MSP and PSP in r0 and r1 respectively, so the first
; and second parameters to the _Fault() C function that will handle the rest.
; This has to be done because at this point we do not know if the fault
; happened while handling an exception or not, and thus the ESF could be on
; either stack. _Fault() will find out where the ESF resides.
;
; Provides these symbols:
;
;   __hard_fault
;   __mpu_fault
;   __bus_fault
;   __usage_fault
;   __secure_fault
;   __debug_monitor
;   __reserved
;*********************************************************************************
;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text.__fault|, CODE, READONLY, ALIGN=4
    THUMB
    REQUIRE8
    PRESERVE8
		
__hard_fault
__mpu_fault
__bus_fault
__usage_fault
	IF :DEF:CONFIG_ARM_SECURE_FIRMWARE
__secure_fault
	ENDIF
__debug_monitor
__reserved

	; force unlock interrupts
	EORS.N R0, R0
	MSR BASEPRI, R0

	IF  :LNOT::DEF:CONFIG_ARM_SECURE_FIRMWARE
	; this checks to see if we are in a nested exception
	LDR IP, =_SCS_ICSR
	LDR IP, [IP]
	ANDS.W IP, #_SCS_ICSR_RETTOBASE

	ITE EQ			; is the RETTOBASE bit zero ?
		MRSEQ R0, MSP	; if so, we're not returning to thread mode,
				 ; thus this is a nested exception: the stack
				 ; frame is on the MSP
		MRSNE R0, PSP	; if not, we are returning to thread mode, thus
				 ;  this is not a nested exception: the stack
				 ; frame is on the PSP
	ELSE
	; RETTOBASE flag is not banked between security states.
	; Therefore, we cannot rely on this flag, to obtain the SP
	; in Secure state. Instead, we use the EXC_RETURN SPSEL flag.
	;
 	LDR R0, =0x4
	MOV R1, LR
	TST R1, R0
	BEQ _s_stack_frame_msp
	MRS R0, PSP
	BNE _s_stack_frame_endif
_s_stack_frame_msp
	MRS R0, MSP
_s_stack_frame_endif
	ENDIF ; CONFIG_ARM_SECURE_FIRMWARE


	IF :DEF:CONFIG_ARM_SECURE_FIRMWARE
	 ; In ARM Secure firmware, the stack pointer that is retrieved
	 ; above points to the Secure stack. However, the exeption may
	 ; have occurred in Non-Secure state.
	 ; To determine this we need to inspect the EXC_RETURN value
	 ; located in the LR. Therefore, we supply the LR value as an
	 ; argument to the fault handler.
	 ;
	MOV R1, LR
	ENDIF ; CONFIG_ARM_SECURE_FIRMWARE
	PUSH {LR}
	BL _Fault

	POP {PC}

	ALIGN
	END
