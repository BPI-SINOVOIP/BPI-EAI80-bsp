;*********************************************************************************
; @file
; @brief ARM Cortex-M power management
;
;*********************************************************************************

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************


;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT _CpuIdleInit[WEAK,CODE]
	IF :DEF:CONFIG_SYS_POWER_MANAGEMENT
	EXPORT _NanoIdleValGet[WEAK,CODE]
	EXPORT _NanoIdleValClear[WEAK,CODE]
	ENDIF
	EXPORT k_cpu_idle[WEAK,CODE]
	EXPORT k_cpu_atomic_idle[WEAK,CODE]
		
;******************************************************************************
;                                 EQUATES
;******************************************************************************

_SCB_SCR 				EQU 0xE000ED10

_SCB_SCR_SEVONPEND 		EQU (1 << 4)
_SCB_SCR_SLEEPDEEP 		EQU (1 << 2)
_SCB_SCR_SLEEPONEXIT 	EQU (1 << 1)
_SCR_INIT_BITS 			EQU _SCB_SCR_SEVONPEND
	
;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;*********************************************************************************
;
; @brief Initialization of CPU idle
;
; Only called by kernel_arch_init(). Sets SEVONPEND bit once for the system's
; duration.
;
; @return N/A
;
; C function prototype:
;
; void _CpuIdleInit (void);
;*********************************************************************************

_CpuIdleInit
	LDR R1, =_SCB_SCR
	MOVS.N R2, #_SCR_INIT_BITS
	STR R2, [R1]
	BX LR

	IF :DEF:CONFIG_SYS_POWER_MANAGEMENT

;*********************************************************************************
;
; @brief Get the kernel idle setting
;
; Returns the kernel idle setting, in ticks. Only called by __systick().
;
; @return the requested number of ticks for the kernel to be idle
;
; C function prototype:
;
; s32_t _NanoIdleValGet (void);
;*********************************************************************************

_NanoIdleValGet
	LDR R0, =g_kernel
	LDR R0, [R0, #_kernel_offset_to_idle]
	BX LR

;*********************************************************************************
;
; @brief Clear the kernel idle setting
;
; Sets the kernel idle setting to 0. Only called by __systick().
;
; @return N/A
;
; C function prototype:
;
; void _NanoIdleValClear (void);
;*********************************************************************************

_NanoIdleValClear
	LDR R0, =g_kernel
	EORS.N R1, R1
	STR R1, [R0, #_kernel_offset_to_idle]
	BX LR

	ENDIF ; CONFIG_SYS_POWER_MANAGEMENT

;*********************************************************************************
;
; @brief Power save idle routine for ARM Cortex-M
;
; This function will be called by the kernel idle loop or possibly within
; an implementation of _sys_power_save_idle in the kernel when the
; '_sys_power_save_flag' variable is non-zero.  The ARM 'wfi' instruction
; will be issued, causing a low-power consumption sleep mode.
;
; @return N/A
;
; C function prototype:
;
; void k_cpu_idle (void);
;*********************************************************************************

k_cpu_idle
	IF :DEF:CONFIG_KERNEL_EVENT_LOGGER_SLEEP
	PUSH {LR}
	BL    _sys_k_event_logger_enter_sleep
	POP {R0}
	MOV LR, R0
	ENDIF

	; clear BASEPRI so wfi is awakened by incoming interrupts
	EORS.N R0, R0
	MSR BASEPRI, R0

	WFI

	BX LR

;*********************************************************************************
;
; @brief Atomically re-enable interrupts and enter low power mode
;
; INTERNAL
; The requirements for k_cpu_atomic_idle() are as follows:
; 1) The enablement of interrupts and entering a low-power mode needs to be
;    atomic, i.e. there should be no period of time where interrupts are
;    enabled before the processor enters a low-power mode.  See the comments
;    in k_lifo_get(), for example, of the race condition that occurs
;    if this requirement is not met.
;
; 2) After waking up from the low-power mode, the interrupt lockout state
;    must be restored as indicated in the 'imask' input parameter.
;
; @return N/A
;
; C function prototype:
;
; void k_cpu_atomic_idle (unsigned int imask);
;*********************************************************************************

k_cpu_atomic_idle
	IF :DEF:CONFIG_KERNEL_EVENT_LOGGER_SLEEP
	PUSH {LR}
	BL    _sys_k_event_logger_enter_sleep
	POP {R1}
	MOV LR, R1
	ENDIF

	 ;*********************************************************************************
	 ; Lock PRIMASK while sleeping: wfe will still get interrupted by
	 ; incoming interrupts but the CPU will not service them right away.
	 ;*********************************************************************************
	CPSID I

	 ;*********************************************************************************
	 ; No need to set SEVONPEND, it's set once in _CpuIdleInit() and never
	 ; touched again.
	 ;*********************************************************************************

	 ; r0: interrupt mask from caller
	; r1: zero, for setting BASEPRI (needs a register)
	EORS.N R1, R1

	; unlock BASEPRI so wfe gets interrupted by incoming interrupts
	; set BASEPRO r1
	MSR BASEPRI, r1

	WFE

	MSR BASEPRI, r0
	CPSIE I

	BX LR
	
	ALIGN
	END
