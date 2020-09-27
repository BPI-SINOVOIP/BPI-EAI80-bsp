;
; @file
; @brief Definitions for the boot vector table
;
;
; Definitions for the boot vector table.
;
; System exception handler names all have the same format:
;
;   __<exception name with underscores>
;
; No other symbol has the same format, so they are easy to spot.
;

CONFIG_CORTEX_M_SYSTICK								EQU     1
CONFIG_PRIORITY_CEILING 							EQU     0
CONFIG_NUM_METAIRQ_PRIORITIES 						EQU     0
CONFIG_MAIN_STACK_SIZE 								EQU     2048
CONFIG_IDLE_STACK_SIZE 								EQU     2048
CONFIG_ISR_STACK_SIZE 								EQU     1024
ARM_V7M_NVIC_E000E100_ARM_NUM_IRQ_PRIORITY_BITS		EQU     3
ARM_V7M_NVIC_E000E100_BASE_ADDRESS					EQU     0xe000e100
ARM_V7M_NVIC_E000E100_SIZE							EQU     3072
_EXCEPTION_RESERVED_PRIO 							EQU		1
_IRQ_PRIO_OFFSET 									EQU 	(_EXCEPTION_RESERVED_PRIO)
CONFIG_NUM_IRQ_PRIO_BITS							EQU     ARM_V7M_NVIC_E000E100_ARM_NUM_IRQ_PRIORITY_BITS		
_EXC_IRQ_DEFAULT_PRIO 								EQU     (((_IRQ_PRIO_OFFSET) << (8 - CONFIG_NUM_IRQ_PRIO_BITS)) & 0xff)
	
	END