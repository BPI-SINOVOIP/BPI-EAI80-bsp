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

CONFIG_GM_HAL_CACHE							EQU		1
CONFIG_SDRAM_BOOT							EQU     1	
;CONFIG_XIP									EQU     1	
CONFIG_IRQ_OFFLOAD							EQU     1	
CONFIG_PREEMPT_ENABLED						EQU     1
CONFIG_TIMESLICING							EQU     1
CONFIG_FP_SHARING 							EQU     1	
	
	END