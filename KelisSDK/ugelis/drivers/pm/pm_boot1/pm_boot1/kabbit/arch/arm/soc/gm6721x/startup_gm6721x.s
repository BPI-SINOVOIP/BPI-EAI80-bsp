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

Heap_Size       EQU     0x00001000

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
                DCD     WDOG_IRQHandler            ; irq0
                DCD     LVD_IRQHandler
                DCD     RTC_IRQHandler
                DCD     DefaultISR
                DCD     DMA_Handler
                DCD     GPIOA_Handler              ; irq5
                DCD     GPIOB_Handler
                DCD     GPIOC_Handler
                DCD     GPIOD_Handler
                DCD     ACMP_Handler
                DCD     ADC0_Handler               ; irq10
                DCD     TIM1_Handler
                DCD     TIM2_Handler
                DCD     TIM3_Handler
                DCD     BASETIMER1_Handler
                DCD     I2C0_Handler                ; irq15
                DCD     I2C1_Handler
                DCD     I2C2_Handler
                DCD     UART0_Handler
                DCD     UART1_Handler
                DCD     UART2_Handler              ; irq20
                DCD     SPI0_Handler
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     USB_Handler                ; USB OTG Hnadler
                DCD     BASETIMER2_Handler         ; irq25
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     SPI1_Handler               ; irq30
                DCD     SPI2_Handler
                DCD     SPI3_Handler
                DCD     SPI4_Handler
                DCD     SPI5_Handler
                DCD     SAIA_Handler                 ; irq35
                DCD     SAIB_Handler
                DCD     DefaultISR
                DCD     Trng_Handler
                DCD     GPIOE_Handler
                DCD     GPIOF_Handler                 ; irq40
                DCD     GPIOG_Handler
                DCD     GPIOH_Handler
                DCD     GPIOI_Handler
                DCD     GPIOJ_Handler
                DCD     GPIOK_Handler                  ; irq45
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     ADC1_Handler
                DCD     ADC2_Handler                 ; irq50
                DCD     CAN0_Handler
                DCD     CAN1_Handler
                DCD     PMU_IRQHandler
                DCD     TIM4_Handler
                DCD     DefaultISR                 ; irq55
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     Crypto_Handler
                DCD     Hash_Handler
                DCD     UART3_Handler              ; irq60
                DCD     UART4_Handler              
                DCD     UART5_Handler              
                DCD     UART6_Handler              
                DCD     UART7_Handler              
                DCD     DefaultISR                 ; irq65
                DCD     TIM5_Handler
                DCD     TIM8_Handler
                DCD     DefaultISR
                DCD     DefaultISR
                DCD     RPC_IRQHandler                 ; irq70
                DCD     RPC_IRQHandler
                DCD     TIM9_Handler
                DCD     TIM10_Handler
                DCD     TIM11_Handler
                DCD     TIM12_Handler              ; irq75
                DCD     TIM13_Handler
                DCD     TIM14_Handler
                DCD     DefaultISR
                DCD     MMC0_Handler
                DCD     DefaultISR                 ; irq80
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
                ;IMPORT  SystemInit
                
                IMPORT  __main
                ;LDR     R0, =SystemInit
                ;BLX     R0
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
WDOG_IRQHandler\
                PROC
                EXPORT  WDOG_IRQHandler                      [WEAK]
                LDR     R0, =HAL_WDOG_IRQHandler
                BX      R0
                ENDP
LVD_IRQHandler\
                PROC
                EXPORT  LVD_IRQHandler                     [WEAK]
                LDR     R0, =HAL_LVD_IRQHandler
                BX      R0
                ENDP
RTC_IRQHandler\
                PROC
                EXPORT  RTC_IRQHandler                     [WEAK]
                LDR     R0, =HAL_RTC_IRQHandler
                BX      R0
                ENDP
USB_Handler\
                PROC
                EXPORT  USB_Handler                      [WEAK]
                LDR     R0, =HAL_USB_IRQHandler
                BX      R0
                ENDP

GPIOA_Handler\
                PROC
                EXPORT  GPIOA_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOA_InterruptHandler
                BX      R0
                ENDP
GPIOB_Handler\
                PROC
                EXPORT  GPIOB_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOB_InterruptHandler
                BX      R0
                ENDP
GPIOC_Handler\
                PROC
                EXPORT  GPIOC_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOC_InterruptHandler
                BX      R0
                ENDP
GPIOD_Handler\
                PROC
                EXPORT  GPIOD_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOD_InterruptHandler
                BX      R0
                ENDP
GPIOE_Handler\
                PROC
                EXPORT  GPIOE_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOE_InterruptHandler
                BX      R0
                ENDP
GPIOF_Handler\
                PROC
                EXPORT  GPIOF_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOF_InterruptHandler
                BX      R0
                ENDP
GPIOG_Handler\
                PROC
                EXPORT  GPIOG_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOG_InterruptHandler
                BX      R0
                ENDP
GPIOH_Handler\
                PROC
                EXPORT  GPIOH_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOH_InterruptHandler
                BX      R0
                ENDP
GPIOI_Handler\
                PROC
                EXPORT  GPIOI_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOI_InterruptHandler
                BX      R0
                ENDP
GPIOJ_Handler\
                PROC
                EXPORT  GPIOJ_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOJ_InterruptHandler
                BX      R0
                ENDP
GPIOK_Handler\
                PROC
                EXPORT  GPIOK_Handler                      [WEAK]
                LDR     R0, =HAL_GPIOK_InterruptHandler
                BX      R0
                ENDP

UART0_Handler\
                PROC
                EXPORT  UART0_Handler                      [WEAK]
                LDR     R0, =HAL_UART0_IRQHandler
                BX      R0
                ENDP

UART1_Handler\
                PROC
                EXPORT  UART1_Handler                      [WEAK]
                LDR     R0, =HAL_UART1_IRQHandler
                BX      R0
                ENDP

UART2_Handler\
                PROC
                EXPORT  UART2_Handler                      [WEAK]
                LDR     R0, =HAL_UART2_IRQHandler
                BX      R0
                ENDP

SPI0_Handler\
                PROC
                EXPORT  SPI0_Handler                      [WEAK]
                LDR     R0, =HAL_SPI0_IRQHandler
                BX      R0
                ENDP

SPI1_Handler\
                PROC
                EXPORT  SPI1_Handler                      [WEAK]
                LDR     R0, =HAL_SPI1_IRQHandler
                BX      R0
                ENDP

SPI2_Handler\
                PROC
                EXPORT  SPI2_Handler                      [WEAK]
                LDR     R0, =HAL_SPI2_IRQHandler
                BX      R0
                ENDP

SPI3_Handler\
                PROC
                EXPORT  SPI3_Handler                      [WEAK]
                LDR     R0, =HAL_SPI3_IRQHandler
                BX      R0
                ENDP

SPI4_Handler\
                PROC
                EXPORT  SPI4_Handler                      [WEAK]
                LDR     R0, =HAL_SPI4_IRQHandler
                BX      R0
                ENDP

SPI5_Handler\
                PROC
                EXPORT  SPI5_Handler                      [WEAK]
                LDR     R0, =HAL_SPI5_IRQHandler
                BX      R0
                ENDP

ADC0_Handler\
                PROC
                EXPORT  ADC0_Handler                      [WEAK]
                LDR     R0, =HAL_ADC0_InterruptHandler
                BX      R0
                ENDP

ADC1_Handler\
                PROC
                EXPORT  ADC1_Handler                      [WEAK]
                LDR     R0, =HAL_ADC1_InterruptHandler
                BX      R0
                ENDP

ADC2_Handler\
                PROC
                EXPORT  ADC2_Handler                      [WEAK]
                LDR     R0, =HAL_ADC2_InterruptHandler
                BX      R0
                ENDP

ACMP_Handler\
                PROC
                EXPORT  ACMP_Handler                      [WEAK]
                LDR     R0, =HAL_ACMP_InterruptHandler
                BX      R0
                ENDP

DMA_Handler\
                PROC
                EXPORT  DMA_Handler                      [WEAK]
                LDR     R0, =HAL_DMA_IRQHandler
                BX      R0
                ENDP

Trng_Handler\
                PROC
				EXPORT  Trng_Handler                     [WEAK]
				LDR     R0, =HAL_TRNG_IRQHandler
				BX      R0
				ENDP

Crypto_Handler\
                PROC
                EXPORT  Crypto_Handler                    [WEAK]
                LDR     R0, =HAL_CRYP_IRQHandler
                BX      R0
                ENDP

Hash_Handler\
                PROC
                EXPORT  Hash_Handler                     [WEAK]
                LDR     R0, =HAL_HASH_IRQHandler
                BX      R0
                ENDP

UART3_Handler\
                PROC
                EXPORT  UART3_Handler                      [WEAK]
                LDR     R0, =HAL_UART3_IRQHandler
                BX      R0
                ENDP

UART4_Handler\
                PROC
                EXPORT  UART4_Handler                      [WEAK]
                LDR     R0, =HAL_UART4_IRQHandler
                BX      R0
                ENDP

UART5_Handler\
                PROC
                EXPORT  UART5_Handler                      [WEAK]
                LDR     R0, =HAL_UART5_IRQHandler
                BX      R0
                ENDP

UART6_Handler\
                PROC
                EXPORT  UART6_Handler                      [WEAK]
                LDR     R0, =HAL_UART6_IRQHandler
                BX      R0
                ENDP

UART7_Handler\
                PROC
                EXPORT  UART7_Handler                      [WEAK]
                LDR     R0, =HAL_UART7_IRQHandler
                BX      R0
                ENDP

MMC0_Handler\
                PROC
				EXPORT  MMC0_Handler                     [WEAK]
				LDR     R0, =HAL_MMC0_IRQHandler
				BX      R0
				ENDP
I2C0_Handler\
                PROC
                EXPORT  I2C0_Handler                     [WEAK]
                LDR     R0, =HAL_I2C0_InterruptHandler
                BX      R0
                ENDP
I2C1_Handler\
                PROC
                EXPORT  I2C1_Handler                     [WEAK]
                LDR     R0, =HAL_I2C1_InterruptHandler
                BX      R0
                ENDP
I2C2_Handler\
                PROC
                EXPORT  I2C2_Handler                     [WEAK]
                LDR     R0, =HAL_I2C2_InterruptHandler
                BX      R0
                ENDP

PMU_IRQHandler\
                PROC
                EXPORT  PMU_IRQHandler                     [WEAK]
                LDR     R0, =HAL_PMU_IRQHandler
                BX      R0
                ENDP


RPC_IRQHandler\
                PROC
                EXPORT  RPC_IRQHandler                     [WEAK]
                LDR     R0, =HAL_RPC_IRQHandler
                BX      R0
                ENDP

TIM1_Handler\
                PROC
                EXPORT  TIM1_Handler                     [WEAK]
                LDR     R0, =HAL_TIM1_InterruptHandler
                BX      R0
                ENDP

TIM2_Handler\
                PROC
                EXPORT  TIM2_Handler                     [WEAK]
                LDR     R0, =HAL_TIM2_InterruptHandler
                BX      R0
                ENDP

TIM3_Handler\
                PROC
                EXPORT  TIM3_Handler                     [WEAK]
                LDR     R0, =HAL_TIM3_InterruptHandler
                BX      R0
                ENDP

TIM4_Handler\
                PROC
                EXPORT  TIM4_Handler                     [WEAK]
                LDR     R0, =HAL_TIM4_InterruptHandler
                BX      R0
                ENDP

TIM5_Handler\
                PROC
                EXPORT  TIM5_Handler                     [WEAK]
                LDR     R0, =HAL_TIM5_InterruptHandler
                BX      R0
                ENDP

TIM8_Handler\
                PROC
                EXPORT  TIM8_Handler                     [WEAK]
                LDR     R0, =HAL_TIM8_InterruptHandler
                BX      R0
                ENDP

TIM9_Handler\
                PROC
                EXPORT  TIM9_Handler                     [WEAK]
                LDR     R0, =HAL_TIM9_InterruptHandler
                BX      R0
                ENDP

TIM10_Handler\
                PROC
                EXPORT  TIM10_Handler                     [WEAK]
                LDR     R0, =HAL_TIM10_InterruptHandler
                BX      R0
                ENDP

TIM11_Handler\
                PROC
                EXPORT  TIM11_Handler                     [WEAK]
                LDR     R0, =HAL_TIM11_InterruptHandler
                BX      R0
                ENDP

TIM12_Handler\
                PROC
                EXPORT  TIM12_Handler                     [WEAK]
                LDR     R0, =HAL_TIM12_InterruptHandler
                BX      R0
                ENDP

TIM13_Handler\
                PROC
                EXPORT  TIM13_Handler                     [WEAK]
                LDR     R0, =HAL_TIM13_InterruptHandler
                BX      R0
                ENDP

TIM14_Handler\
                PROC
                EXPORT  TIM14_Handler                     [WEAK]
                LDR     R0, =HAL_TIM14_InterruptHandler
                BX      R0
                ENDP

BASETIMER1_Handler\
                PROC
                EXPORT  BASETIMER1_Handler                [WEAK]
                LDR     R0, =HAL_BASETIMER1_InterruptHandler
                BX      R0
                ENDP

BASETIMER2_Handler\
                PROC
                EXPORT  BASETIMER2_Handler                [WEAK]
                LDR     R0, =HAL_BASETIMER2_InterruptHandler
                BX      R0
                ENDP

CAN0_Handler\
                PROC
                EXPORT  CAN0_Handler                     [WEAK]
                LDR     R0, =HAL_CAN0_IRQHandler
                BX      R0
                ENDP


CAN1_Handler\
                PROC
                EXPORT  CAN1_Handler                     [WEAK]
                LDR     R0, =HAL_CAN1_IRQHandler
                BX      R0
                ENDP
SAIA_Handler\
                PROC
                EXPORT  SAIA_Handler                     [WEAK]
                LDR     R0, =HAL_SAIA_IRQHandler
                BX      R0
                ENDP
SAIB_Handler\
                PROC
                EXPORT  SAIB_Handler                     [WEAK]
                LDR     R0, =HAL_SAIB_IRQHandler
                BX      R0
                ENDP
Default_Handler\
                PROC
                EXPORT  Systick_InterruptHandler         [WEAK]
                EXPORT  HAL_WDOG_IRQHandler              [WEAK]
                EXPORT  HAL_LVD_IRQHandler               [WEAK]
                EXPORT  HAL_RTC_IRQHandler               [WEAK]
                EXPORT  HAL_USB_IRQHandler               [WEAK]
                EXPORT  HAL_DMA_IRQHandler               [WEAK]
                EXPORT  HAL_ADC0_InterruptHandler        [WEAK]
                EXPORT  HAL_ADC1_InterruptHandler        [WEAK]
                EXPORT  HAL_ADC2_InterruptHandler        [WEAK]
                EXPORT  HAL_CAN0_IRQHandler              [WEAK]
                EXPORT  HAL_CAN1_IRQHandler              [WEAK]
                EXPORT  DefaultISR                       [WEAK]

                EXPORT  HAL_UART0_IRQHandler             [WEAK]
                EXPORT  HAL_UART1_IRQHandler             [WEAK]
                EXPORT  HAL_UART2_IRQHandler             [WEAK]
                EXPORT  HAL_UART3_IRQHandler             [WEAK]
                EXPORT  HAL_UART4_IRQHandler             [WEAK]
                EXPORT  HAL_UART5_IRQHandler             [WEAK]
                EXPORT  HAL_UART6_IRQHandler             [WEAK]
                EXPORT  HAL_UART7_IRQHandler             [WEAK]

                EXPORT  HAL_SPI0_IRQHandler              [WEAK]
                EXPORT  HAL_SPI1_IRQHandler              [WEAK]
                EXPORT  HAL_SPI2_IRQHandler              [WEAK]
                EXPORT  HAL_SPI3_IRQHandler              [WEAK]
                EXPORT  HAL_SPI4_IRQHandler              [WEAK]
                EXPORT  HAL_SPI5_IRQHandler              [WEAK]

                EXPORT  HAL_GPIOA_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOB_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOC_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOD_InterruptHandler       [WEAK]

                EXPORT  HAL_GPIOE_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOF_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOG_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOH_InterruptHandler       [WEAK]

                EXPORT  HAL_GPIOI_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOJ_InterruptHandler       [WEAK]
                EXPORT  HAL_GPIOK_InterruptHandler       [WEAK]
                EXPORT  HAL_ACMP_InterruptHandler        [WEAK]

                EXPORT  HAL_TRNG_IRQHandler              [WEAK]
                EXPORT  HAL_CRYP_IRQHandler              [WEAK]
		        EXPORT  HAL_HASH_IRQHandler              [WEAK]

		        EXPORT  HAL_I2C0_InterruptHandler        [WEAK]
		        EXPORT  HAL_I2C1_InterruptHandler        [WEAK]
		        EXPORT  HAL_I2C2_InterruptHandler        [WEAK]

				EXPORT  HAL_TIM1_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM2_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM3_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM4_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM5_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM8_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM9_InterruptHandler        [WEAK]
				EXPORT  HAL_TIM10_InterruptHandler       [WEAK]
				EXPORT  HAL_TIM11_InterruptHandler       [WEAK]
				EXPORT  HAL_TIM12_InterruptHandler       [WEAK]
				EXPORT  HAL_TIM13_InterruptHandler       [WEAK]
				EXPORT  HAL_TIM14_InterruptHandler       [WEAK]

                EXPORT  HAL_BASETIMER1_InterruptHandler  [WEAK]
				EXPORT  HAL_BASETIMER2_InterruptHandler  [WEAK]

                EXPORT  HAL_PMU_IRQHandler               [WEAK]

                EXPORT  HAL_MMC0_IRQHandler              [WEAK]

                EXPORT  HAL_RPC_IRQHandler               [WEAK]
					
				EXPORT  HAL_SAIA_IRQHandler              [WEAK]
			    EXPORT  HAL_SAIB_IRQHandler              [WEAK]

Systick_InterruptHandler
HAL_WDOG_IRQHandler
HAL_LVD_IRQHandler
HAL_RTC_IRQHandler
HAL_USB_IRQHandler
HAL_DMA_IRQHandler
HAL_ADC0_InterruptHandler
HAL_ADC1_InterruptHandler
HAL_ADC2_InterruptHandler
HAL_CAN0_IRQHandler
HAL_CAN1_IRQHandler
HAL_UART0_IRQHandler
HAL_UART1_IRQHandler
HAL_UART2_IRQHandler
HAL_UART3_IRQHandler
HAL_UART4_IRQHandler
HAL_UART5_IRQHandler
HAL_UART6_IRQHandler
HAL_UART7_IRQHandler
HAL_SPI0_IRQHandler
HAL_SPI1_IRQHandler
HAL_SPI2_IRQHandler
HAL_SPI3_IRQHandler
HAL_SPI4_IRQHandler
HAL_SPI5_IRQHandler
HAL_GPIOA_InterruptHandler
HAL_GPIOB_InterruptHandler
HAL_GPIOC_InterruptHandler
HAL_GPIOD_InterruptHandler
HAL_GPIOE_InterruptHandler
HAL_GPIOF_InterruptHandler
HAL_GPIOG_InterruptHandler
HAL_GPIOH_InterruptHandler
HAL_GPIOI_InterruptHandler
HAL_GPIOJ_InterruptHandler
HAL_GPIOK_InterruptHandler
HAL_ACMP_InterruptHandler
HAL_TRNG_IRQHandler
HAL_CRYP_IRQHandler
HAL_HASH_IRQHandler
HAL_MMC0_IRQHandler
HAL_I2C0_InterruptHandler
HAL_I2C1_InterruptHandler
HAL_I2C2_InterruptHandler
HAL_TIM1_InterruptHandler
HAL_TIM2_InterruptHandler
HAL_TIM3_InterruptHandler
HAL_TIM4_InterruptHandler
HAL_TIM5_InterruptHandler
HAL_TIM8_InterruptHandler
HAL_TIM9_InterruptHandler
HAL_TIM10_InterruptHandler
HAL_TIM11_InterruptHandler
HAL_TIM12_InterruptHandler
HAL_TIM13_InterruptHandler
HAL_TIM14_InterruptHandler
HAL_BASETIMER1_InterruptHandler
HAL_BASETIMER2_InterruptHandler
HAL_PMU_IRQHandler
HAL_RPC_IRQHandler
HAL_SAIA_IRQHandler
HAL_SAIB_IRQHandler

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
