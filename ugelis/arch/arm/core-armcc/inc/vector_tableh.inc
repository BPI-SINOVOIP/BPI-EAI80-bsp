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

	IMPORT  __start[CODE]
	IMPORT	__reset[CODE]
	IMPORT  __nmi[CODE]
	IMPORT	__hard_fault[CODE]
	IMPORT	__mpu_fault[CODE]
	IMPORT	__bus_fault[CODE]
	IMPORT	__usage_fault[CODE]
	IF :DEF:CONFIG_ARM_SECURE_FIRMWARE
	IMPORT	__secure_fault[CODE]
	ENDIF ; CONFIG_ARM_SECURE_FIRMWARE
	IMPORT	__svc[CODE]
	IMPORT	__debug_monitor[CODE]
	IMPORT	__pendsv[CODE]
	IMPORT	__reserved[CODE]
	IMPORT	_PrepC[CODE]
	IMPORT	_isr_wrapper[CODE]
	IMPORT  _timer_int_handler[CODE]	
	END