;*********************************************************************************
; @file
; @brief Populated vector table in ROM
;
; Vector table at the beginning of the image for starting system. The reset
; vector is the system entry point, ie. the first instruction executed.
;
; The table is populated with all the system exception handlers. The NMI vector
; must be populated with a valid handler since it can happen at any time. The
; rest should not be triggered until the kernel is ready to handle them.
;*********************************************************************************

	GET ..\inc\vector_tableh.inc
	GET ..\inc\board.inc

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

    IMPORT _main_stack[DATA]

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.exc_vector_table._vector_table_section|, CODE, READONLY, ALIGN=4
    THUMB
    REQUIRE8
    PRESERVE8
_vector_table
	;*********************************************************************************
     ; setting the _very_ early boot on the main stack allows to use memset
     ; on the interrupt stack when CONFIG_INIT_STACKS is enabled before
     ; switching to the interrupt stack for the rest of the early boot
     ;*********************************************************************************
    DCD _main_stack + CONFIG_MAIN_STACK_SIZE

    DCD __reset
    DCD __nmi
    DCD __hard_fault
    DCD __mpu_fault
    DCD __bus_fault
    DCD __usage_fault
	IF  :DEF:CONFIG_ARM_SECURE_FIRMWARE
    DCD __secure_fault
	ELSE
    DCD __reserved
	ENDIF
    DCD __reserved
    DCD __reserved
    DCD __reserved
    DCD __svc
    DCD __debug_monitor
    DCD __reserved
    DCD __pendsv
	IF  :DEF:CONFIG_CORTEX_M_SYSTICK
    DCD _timer_int_handler
	ELSE
    DCD __reserved
	ENDIF
	
	ALIGN
    END
