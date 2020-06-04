;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

    IMPORT  _k_syscall_table
	IMPORT  g_kernel
	IMPORT  memset
	IMPORT  configure_mpu_mem_domain
	IMPORT _thread_entry

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT  _arm_userspace_enter[WEAK,CODE]    
    EXPORT  _arm_do_syscall[WEAK,CODE] 
		
;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8
		
;******************************************************************************
;
; User space entry function
;
; This function is the entry point to user mode from privileged execution.
; The conversion is one way, and threads which transition to user mode do
; not transition back later, unless they are doing system calls.
;
;******************************************************************************
_arm_userspace_enter
    ; move user_entry to lr
    MOV LR, R0

    ; set stack to priviliged stack
    LDR R0, =g_kernel
    LDR R0, [R0, #_kernel_offset_to_current]
    LDR R0, [R0, #_thread_offset_to_priv_stack_start]     ;  priv stack ptr
    LDR IP, =CONFIG_PRIVILEGED_STACK_SIZE
    ADD R0, R0, IP

    MOV IP, SP
    MSR PSP, R0

    ; load up stack info from user stack
    LDR R0, [IP]
    LDR IP, [IP, #4]

	IF :DEF:CONFIG_MPU_REQUIRES_POWER_OF_TWO_ALIGNMENT
    ; Guard is taken out of size, so adjust beginning and size of stack
    SUBS IP, #MPU_GUARD_ALIGN_AND_SIZE
	ENDIF

    ; push args to stack
    PUSH {R0,R1,R2,R3,IP,LR}

    ; clear the user stack area to clean out privileged data
    ; from right past the guard right up to the end
    MOV R2, IP
	IF :DEF:CONFIG_INIT_STACKS
    LDR R1,=0xaaaaaaaa
	ELSE
    EORS.N R1, R1
	ENDIF
    BL memset

    ;  setup arguments to configure_mpu_mem_domain
    LDR R0, =g_kernel
    LDR R0, [R0, #_kernel_offset_to_current]
    BL configure_mpu_mem_domain

    ;  setup arguments configure_mpu_user_context
    LDR R0, =g_kernel
    LDR R0, [R0, #_kernel_offset_to_current]
    BL configure_mpu_user_context

    POP {R0,R1,R2,R3,IP,LR}

    ;  r0 contains user stack start, ip contains user stack size
    ADD R0, R0, IP   ;  calculate top of stack

    ;  set stack to user stack
    MSR PSP, R0

    ;  restore r0
    MOV R0, LR

    ;  change processor mode to unprivileged */
    MRS IP, CONTROL
    ORRS IP, IP, #1
    MSR CONTROL, IP

    ;  ISB is not strictly necessary here (stack pointer is not being
    ;  touched), but it's recommended to avoid executing pre-fetched
    ;  instructions with the previous privilege.
    ;******************************************************************************  
    ISB

    ;  jump to _thread_entry entry
    LDR IP, =_thread_entry
    BX IP

;******************************************************************************  
;  
;   Userspace system call function
;  
;   This function is used to do system calls from unprivileged code.  This
;  function is responsible for the following:
;  1) Fixing up bad syscalls
;  2) Configuring privileged stack and loading up stack arguments
;  3) Dispatching the system call
;  4) Restoring stack and calling back to the caller of the SVC
;  
;******************************************************************************  
_arm_do_syscall
    ;******************************************************************************  
    ;   r0-r5 contain arguments
    ;   r6 contains call_id
    ;   r8 contains original LR
    ;******************************************************************************  
    LDR IP, =_SYSCALL_BAD
    CMP R6, IP
    BNE valid_syscall

    ;   BAD SYSCALL path
    ;   fixup stack frame on unprivileged stack, adding ssf
    MOV IP, SP
    PUSH {R4,R5,IP,LR}
    B dispatch_syscall

valid_syscall
    ;   setup priviliged stack
    PUSH {R6}
    LDR R6, =g_kernel
    LDR R6, [R6, #_kernel_offset_to_current]
    LDR IP, [R6, #_thread_offset_to_priv_stack_start]    ; priv stack ptr
    LDR R6, =CONFIG_PRIVILEGED_STACK_SIZE
    ADD IP, R6
    POP {R6}
    SUBS IP, #8
    STR SP, [IP, #0]
    STR LR, [IP, #4]

    ;   switch to privileged stack
    MSR PSP, IP

    ;   push args to complete stack frame
    PUSH {R4,R5}

dispatch_syscall
    LDR IP, =_k_syscall_table
    LSL R6, #2
    ADD IP, R6
    LDR IP, [IP]	;  load table address
    ;  execute function from dispatch table
    BLX IP

    ;   restore LR
    LDR LR, [SP,#12]

    ;   set stack back to unprivileged stack
    LDR IP, [SP,#8]
    MSR PSP, IP

    ;   drop privileges by setting bit 0 in CONTROL
    MRS IP, CONTROL
    ORRS IP, IP, #1
    MSR CONTROL, IP

    ;******************************************************************************
    ;   ISB is not strictly necessary here (stack pointer is not being
    ;   touched), but it's recommended to avoid executing pre-fetched
    ;   instructions with the previous privilege.
    ;******************************************************************************
    ISB

    ;****************************************************************************** 
     ; Zero out volatile (caller-saved) registers so as to not leak state from
     ; kernel mode. The C calling convention for the syscall handler will
     ; restore the others to original values.
    ;******************************************************************************
    MOV R1, #0
    MOV R2, #0
    MOV R3, #0

    ;******************************************************************************
     ; return back to original function that called SVC, add 1 to force thumb
     ; mode
    ;******************************************************************************
    MOV IP, r8
    ORRS IP, IP, #1
    BX IP
	
	ALIGN
	END
