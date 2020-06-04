;*********************************************************************************
; @file irq_relay.S
;
; @brief IRQ relay vector table and relay handler for Cortex-M0 or
;        Armv8-M baseline SoCs
;
; In certain ARMv6-M and Armv8-M baseline cores the vector table address can
; not be changed. Once the * vector table is occupied by bootloader, there
; will be no IRQ support in the chainloaded image.
;
; This program will link into bootloader, once an interrupt is coming,
; the bootloader can forward the interrupt to the chainloaded image. This
; will support DFU on those cores.
;
; Note: Currently support mcuboot only.
;*********************************************************************************
	AREA |.vector_relay_handler|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

		
	IF :DEF:CONFIG_SW_VECTOR_RELAY

	IMPORT _vector_table_pointer[DATA]
	IMPORT _main_stack[DATA]

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
__vector_relay_handler
	MRS		R0, IPSR
	LSLS	R0, R0, $0x02

	LDR		R1, =_vector_table_pointer
	LDR		R1, [R1]
	ADDS	R1, R1, R0
	LDR		R1, [R1]

	;******************************************************************************
	; The size of IRQ vector is 4 bytes, the offset within vector table
	; is the IRQ number times 4 (aka r0 << 2). As know as the r1 stored
	; the offset of real vector table, thus the (r1 = r1 + r0 << 2) will
	; be the real irq handle vector.
	;******************************************************************************

	BX	R1

	IMPORT __vector_relay_handler[CODE]

	AREA |.vector_relay_table|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8
__vector_relay_table
	DCD _main_stack + CONFIG_MAIN_STACK_SIZE
	DCD __reset
	DCD __vector_relay_handler	; nmi
	DCD __vector_relay_handler	; hard fault
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler	; svc
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler	; pendsv
	DCD __vector_relay_handler
	; End of system exception

	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	DCD __vector_relay_handler
	ENDIF
	
	ALIGN
	END
