;******************** (C) COPYRIGHT 2015 Gree Microelectronics ********************
;* File Name          : startup_gma0164qp64.s
;* Author             : Wen Liu
;* Version            : V1.0.0
;* Date               : 2015/08/20
;* Description        : GMA0164QP64 startup file for arm-cc.
;*                      This module do following:
;*                      - Set the initial SP & PC
;*                      - Build vector table with external interrupt handlers
;*                      - Branches to main routine
;*******************************************************************************


; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000300

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp   ; Top of Stack
                DCD     Reset_Handler  ; Reset Handler
                DCD     INT02_Handler  ;
                DCD     INT03_Handler  ;
                DCD     INT04_Handler  ;
                DCD     INT05_Handler  ;
                DCD     INT06_Handler  ;
                DCD     INT07_Handler  ;
                DCD     INT08_Handler  ;
                DCD     INT09_Handler  ;
                DCD     INT10_Handler  ;
                DCD     INT11_Handler  ;
                DCD     INT12_Handler  ;
                DCD     INT13_Handler  ;
                DCD     INT14_Handler  ;
                DCD     INT15_Handler  ;

                ; External Interrupts
                DCD     INT16_Handler  ;
                DCD     INT17_Handler  ;
                DCD     INT18_Handler  ;
                DCD     INT19_Handler  ;
                DCD     INT20_Handler  ;
                DCD     INT21_Handler  ;
                DCD     INT22_Handler  ;
                DCD     INT23_Handler  ;
                DCD     INT24_Handler  ;
                DCD     INT25_Handler  ;
                DCD     INT26_Handler  ;
                DCD     INT27_Handler  ;
                DCD     INT28_Handler  ;
                DCD     INT29_Handler  ;
                DCD     INT30_Handler  ;
                DCD     INT31_Handler  ;
                DCD     INT32_Handler  ;
                DCD     INT33_Handler  ;
                DCD     INT34_Handler  ;
                DCD     INT35_Handler  ;
                DCD     INT36_Handler  ;
                DCD     INT37_Handler  ;
                DCD     INT38_Handler  ;
                DCD     INT39_Handler  ;
                DCD     INT40_Handler  ;
                DCD     INT41_Handler  ;
                DCD     INT42_Handler  ;
                DCD     INT43_Handler  ;
                DCD     INT44_Handler  ;
                DCD     INT45_Handler  ;
                DCD     INT46_Handler  ;
                DCD     INT47_Handler  ;
                DCD     INT48_Handler  ;
                DCD     INT49_Handler  ;
                DCD     INT50_Handler  ;
                DCD     INT51_Handler  ;
                DCD     INT52_Handler  ;
                DCD     INT53_Handler  ;
                DCD     INT54_Handler  ;
                DCD     INT55_Handler  ;
                DCD     INT56_Handler  ;
                DCD     INT57_Handler  ;
                DCD     INT58_Handler  ;
                DCD     INT59_Handler  ;
                DCD     INT60_Handler  ;
                DCD     INT61_Handler  ;
                DCD     INT62_Handler  ;
                DCD     INT63_Handler  ;
                DCD     INT64_Handler  ;
                DCD     INT65_Handler  ;
                DCD     INT66_Handler  ;
                DCD     INT67_Handler  ;
                DCD     INT68_Handler  ;
__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors

                ;IF      :LNOT::DEF:RAM_TARGET
                ;AREA    |.ARM.__at_0x400|, CODE, READONLY
                ;DCB     0xFF,       0xFF,       0xFF,       0xFF
                ;DCB     0xFF,       0xFF,       0xFF,       0xFF
                ;DCD 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
                ;AREA    |.ARM.__at_0x420|, CODE, READONLY
                ;DCB     0xB1,       0xB1,       0xFF,       0xFF
                ;DCD 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
                ;ENDIF

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]

				;;copy data to SRAM and then do-mapping
				;MOVS 	R2, #0x01
				;MOVS 	R0, R2, LSL#28		;R0 = 0x10000000
				
				;MOVS 	R2, #0x01
				;MOVS 	R1, R2, LSL#29		;R1 = 0x20000000
				
				;MOVS 	R2, #0x01
				;MOVS 	R5, R2, LSL#13		;R5 = 0x2000 8KB
				
				;MOVS 	R3, #0x00
;LOOP1			;0~8KB is mapped to the first 8K of main flash
				;LDR 	R2, [R0, R3]
				;STR 	R2, [R1, R3]
				;ADDS 	R3, #0x04
				;CMP 	R3, R5
				;BNE		LOOP1
				
				;;0x00000000 is remapped to 0x20000000
				;MOVS 	R2, #0x01
				;MOVS	R0, R2, LSL#30
				;ADDS	R0, #0x14		;R0 = 0x40000014
				
				;LDR		R2, =0x1ACC0000
				;STR		R2, [R0, #0]

				;DSB
				;ISB

                BL      label1
label1
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

Default_Handler PROC
                ;IMPORT  Default_INTHanlder
                EXPORT  INT02_Handler     [WEAK]
                EXPORT  INT03_Handler     [WEAK]
                EXPORT  INT04_Handler     [WEAK]
                EXPORT  INT05_Handler     [WEAK]
                EXPORT  INT06_Handler     [WEAK]
                EXPORT  INT07_Handler     [WEAK]
                EXPORT  INT08_Handler     [WEAK]
                EXPORT  INT09_Handler     [WEAK]
                EXPORT  INT10_Handler     [WEAK]
                EXPORT  INT11_Handler     [WEAK]
                EXPORT  INT12_Handler     [WEAK]
                EXPORT  INT13_Handler     [WEAK]
                EXPORT  INT14_Handler     [WEAK]
                EXPORT  INT15_Handler     [WEAK]
                EXPORT  INT16_Handler     [WEAK]
                EXPORT  INT17_Handler     [WEAK]
                EXPORT  INT18_Handler     [WEAK]
                EXPORT  INT19_Handler     [WEAK]
                EXPORT  INT20_Handler     [WEAK]
                EXPORT  INT21_Handler     [WEAK]
                EXPORT  INT22_Handler     [WEAK]
                EXPORT  INT23_Handler     [WEAK]
                EXPORT  INT24_Handler     [WEAK]
                EXPORT  INT25_Handler     [WEAK]
                EXPORT  INT26_Handler     [WEAK]
                EXPORT  INT27_Handler     [WEAK]
                EXPORT  INT28_Handler     [WEAK]
                EXPORT  INT29_Handler     [WEAK]
                EXPORT  INT30_Handler     [WEAK]
                EXPORT  INT31_Handler     [WEAK]
                EXPORT  INT32_Handler     [WEAK]
                EXPORT  INT33_Handler     [WEAK]
                EXPORT  INT34_Handler     [WEAK]
                EXPORT  INT35_Handler     [WEAK]
                EXPORT  INT36_Handler     [WEAK]
                EXPORT  INT37_Handler     [WEAK]
                EXPORT  INT38_Handler     [WEAK]
                EXPORT  INT39_Handler     [WEAK]
                EXPORT  INT40_Handler     [WEAK]
                EXPORT  INT41_Handler     [WEAK]
                EXPORT  INT42_Handler     [WEAK]
                EXPORT  INT43_Handler     [WEAK]
                EXPORT  INT44_Handler     [WEAK]
                EXPORT  INT45_Handler     [WEAK]
                EXPORT  INT46_Handler     [WEAK]
                EXPORT  INT47_Handler     [WEAK]
                EXPORT  INT48_Handler     [WEAK]
                EXPORT  INT49_Handler     [WEAK]
                EXPORT  INT50_Handler     [WEAK]
                EXPORT  INT51_Handler     [WEAK]
                EXPORT  INT52_Handler     [WEAK]
                EXPORT  INT53_Handler     [WEAK]
                EXPORT  INT54_Handler     [WEAK]
                EXPORT  INT55_Handler     [WEAK]
                EXPORT  INT56_Handler     [WEAK]
                EXPORT  INT57_Handler     [WEAK]
                EXPORT  INT58_Handler     [WEAK]
                EXPORT  INT59_Handler     [WEAK]
                EXPORT  INT60_Handler     [WEAK]
                EXPORT  INT61_Handler     [WEAK]
                EXPORT  INT62_Handler     [WEAK]
                EXPORT  INT63_Handler     [WEAK]
                EXPORT  INT64_Handler     [WEAK]
                EXPORT  INT65_Handler     [WEAK]
                EXPORT  INT66_Handler     [WEAK]
                EXPORT  INT67_Handler     [WEAK]
                EXPORT  INT68_Handler     [WEAK]
INT02_Handler
INT03_Handler
INT04_Handler
INT05_Handler
INT06_Handler
INT07_Handler
INT08_Handler
INT09_Handler
INT10_Handler
INT11_Handler
INT12_Handler
INT13_Handler
INT14_Handler
INT15_Handler
INT16_Handler
INT17_Handler
INT18_Handler
INT19_Handler
INT20_Handler
INT21_Handler
INT22_Handler
INT23_Handler
INT24_Handler
INT25_Handler
INT26_Handler
INT27_Handler
INT28_Handler
INT29_Handler
INT30_Handler
INT31_Handler
INT32_Handler
INT33_Handler
INT34_Handler
INT35_Handler
INT36_Handler
INT37_Handler
INT38_Handler
INT39_Handler
INT40_Handler
INT41_Handler
INT42_Handler
INT43_Handler
INT44_Handler
INT45_Handler
INT46_Handler
INT47_Handler
INT48_Handler
INT49_Handler
INT50_Handler
INT51_Handler
INT52_Handler
INT53_Handler
INT54_Handler
INT55_Handler
INT56_Handler
INT57_Handler
INT58_Handler
INT59_Handler
INT60_Handler
INT61_Handler
INT62_Handler
INT63_Handler
INT64_Handler
INT65_Handler
INT66_Handler
INT67_Handler
INT68_Handler
                B        .
                ;LDR     R0, =Default_INTHanlder
                ;BX      R0

                ENDP


                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
;*****END OF FILE*****