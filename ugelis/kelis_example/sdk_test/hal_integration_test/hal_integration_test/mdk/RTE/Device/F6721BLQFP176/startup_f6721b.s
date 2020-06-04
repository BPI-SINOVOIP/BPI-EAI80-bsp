;******************** (C) COPYRIGHT 2015 Gree Microelectronics ********************
;* File Name          : startup_gm6721x.s
;* Author             : Wizephen.Wang
;* Version            : V1.0.0
;* Date               : 2019/03/25
;* Description        : GM6721 startup file for arm-cc.
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

Stack_Size      EQU     0x00001000

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
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     MemManage_Handler          ; MPU Fault Handler
                DCD     BusFault_Handler           ; Bus Fault Handler
                DCD     UsageFault_Handler         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                ; External Interrupts
                DCD     DefaultISR                 ; irq0   
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                 ; irq5   
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                 ; irq10
                DCD     DefaultISR                    
                DCD     DefaultISR                    
                DCD     DefaultISR                    
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq15
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq20
                DCD     DefaultISR           
                DCD     DefaultISR           
                DCD     DefaultISR           
                DCD     USB_Handler                ; USB OTG Hnadler
                DCD     DefaultISR                 ; irq25
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq30
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq35
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                     
                DCD     DefaultISR                 ; irq40
                DCD     DefaultISR                   
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                     
                DCD     DefaultISR                 ; irq45
                DCD     DefaultISR                     
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq50
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq55
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR                 ; irq60
                DCD     DefaultISR                   
                DCD     DefaultISR                   
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR              
                DCD     DefaultISR             
                DCD     DefaultISR             
                DCD     DefaultISR             
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     DefaultISR                          
                DCD     0xFFFFFFFF                          
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
                EXPORT  Reset_Handler                    [WEAK]			
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

NMI_Handler\
                PROC
                EXPORT  NMI_Handler                      [WEAK]
                B       .                                
                ENDP
HardFault_Handler\
                PROC                                     
                EXPORT  HardFault_Handler                [WEAK]
                B       .                                
                ENDP                                     
MemManage_Handler\
                PROC                                     
                EXPORT  MemManage_Handler                [WEAK]
                B       .                                
                ENDP
BusFault_Handler\
                PROC                                     
                EXPORT  BusFault_Handler                 [WEAK]
                B       .                                
                ENDP
UsageFault_Handler\
                PROC                                     
                EXPORT  UsageFault_Handler               [WEAK]
                B       .                                
                ENDP
SVC_Handler\
                PROC                                     
                EXPORT  SVC_Handler                      [WEAK]
                B       .                                
                ENDP
DebugMon_Handler\
                PROC                                     
                EXPORT  DebugMon_Handler                 [WEAK]
                B       .                                
                ENDP
PendSV_Handler\
                PROC                                     
                EXPORT  PendSV_Handler                   [WEAK]
                B       .                                
                ENDP
SysTick_Handler\
                PROC                                     
                EXPORT  SysTick_Handler                  [WEAK]
                LDR     R0, =Systick_InterruptHandler
                BX      R0
                ENDP
USB_Handler\
                PROC
                EXPORT  USB_Handler                      [WEAK]
                LDR     R0, =HAL_USB_IRQHandler          
                BX      R0                               
                ENDP

Default_Handler\
                PROC                                     
                EXPORT  Systick_InterruptHandler         [WEAK]
                EXPORT  HAL_USB_IRQHandler               [WEAK]                
                EXPORT  DefaultISR                       [WEAK]
Systick_InterruptHandler					
HAL_USB_IRQHandler
DefaultISR
                LDR    R0, =DefaultISR
                BX     R0
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