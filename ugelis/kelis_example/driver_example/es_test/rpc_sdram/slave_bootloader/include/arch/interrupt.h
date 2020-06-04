/***********************************************************************
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file interrupt.h
 *
 * @version 1.0.0
 *
 * @date 2015/08/21
 *
 * @contents Definition of vector table and interrupt handler.
 *
 * @history
 *  Date                    By              Comments                    Ver.
 *  =========       =========        =========           =========
 *  2015/08/21      Wen Liu          Create                  1.0.0
 **********************************************************************/

#ifndef __INTERRUPT_H
#define __INTERRUPT_H     1

#define STATIC_ISR_BY_ALIAS(IntNum, FuncName) void INT##IntNum##_Handler(void) __attribute__((alias(#FuncName)))
#define STATIC_ISR_BY_INTNUM(IntNum, FuncName) STATIC_ISR_BY_ALIAS(IntNum, FuncName)
#define STATIC_ISR(IntName, FuncName) STATIC_ISR_BY_INTNUM(IntName##_INTVEC, FuncName)

#ifndef MAX_ACTIVE_INT_NUM
    #define MAX_ACTIVE_INT_NUM 8                        /* Default number of interrupt handlers used by application */
#endif

typedef void (*INT_Callback_T)(void *Param);
typedef void pointer(void);

void Default_INTHanlder(void);

extern void __startup(void);
extern unsigned long __initial_sp[];
extern void Reset_Handler(void);

#define INTVEC_00      (pointer*)__initial_sp          /* Arm Cortex-M Initial SP */
#define INTVEC_01      Reset_Handler                             /* Arm Cortex-M Initial PC */
#ifndef INTVEC_02
    #define INTVEC_02      INT02_Handler
#endif
#ifndef INTVEC_03
    #define INTVEC_03      INT03_Handler
#endif
#ifndef INTVEC_04
    #define INTVEC_04      INT04_Handler
#endif
#ifndef INTVEC_05
    #define INTVEC_05      INT05_Handler
#endif
#ifndef INTVEC_06
    #define INTVEC_06      INT06_Handler
#endif
#ifndef INTVEC_07
    #define INTVEC_07      INT07_Handler
#endif
#ifndef INTVEC_08
    #define INTVEC_08      INT08_Handler
#endif
#ifndef INTVEC_09
    #define INTVEC_09      INT09_Handler
#endif
#ifndef INTVEC_10
    #define INTVEC_10      INT10_Handler
#endif
#ifndef INTVEC_11
    #define INTVEC_11      INT11_Handler
#endif
#ifndef INTVEC_12
    #define INTVEC_12      INT12_Handler
#endif
#ifndef INTVEC_13
    #define INTVEC_13      INT13_Handler
#endif
#ifndef INTVEC_14
    #define INTVEC_14      INT14_Handler
#endif
#ifndef INTVEC_15
    #define INTVEC_15      INT15_Handler
#endif
#ifndef INTVEC_16
    #define INTVEC_16      INT16_Handler
#endif
#ifndef INTVEC_17
    #define INTVEC_17      INT17_Handler
#endif
#ifndef INTVEC_18
    #define INTVEC_18      INT18_Handler
#endif
#ifndef INTVEC_19
    #define INTVEC_19      INT19_Handler
#endif
#ifndef INTVEC_20
    #define INTVEC_20      INT20_Handler
#endif
#ifndef INTVEC_21
    #define INTVEC_21      INT21_Handler
#endif
#ifndef INTVEC_22
    #define INTVEC_22      INT22_Handler
#endif
#ifndef INTVEC_23
    #define INTVEC_23      INT23_Handler
#endif
#ifndef INTVEC_24
    #define INTVEC_24      INT24_Handler
#endif
#ifndef INTVEC_25
    #define INTVEC_25      INT25_Handler
#endif
#ifndef INTVEC_26
    #define INTVEC_26      INT26_Handler
#endif
#ifndef INTVEC_27
    #define INTVEC_27      INT27_Handler
#endif
#ifndef INTVEC_28
    #define INTVEC_28      INT28_Handler
#endif
#ifndef INTVEC_29
    #define INTVEC_29      INT29_Handler
#endif
#ifndef INTVEC_30
    #define INTVEC_30      INT30_Handler
#endif
#ifndef INTVEC_31
    #define INTVEC_31      INT31_Handler
#endif
#ifndef INTVEC_32
    #define INTVEC_32      INT32_Handler
#endif
#ifndef INTVEC_33
    #define INTVEC_33      INT33_Handler
#endif
#ifndef INTVEC_34
    #define INTVEC_34      INT34_Handler
#endif
#ifndef INTVEC_35
    #define INTVEC_35      INT35_Handler
#endif
#ifndef INTVEC_36
    #define INTVEC_36      INT36_Handler
#endif
#ifndef INTVEC_37
    #define INTVEC_37      INT37_Handler
#endif
#ifndef INTVEC_38
    #define INTVEC_38      INT38_Handler
#endif
#ifndef INTVEC_39
    #define INTVEC_39      INT39_Handler
#endif
#ifndef INTVEC_40
    #define INTVEC_40      INT40_Handler
#endif
#ifndef INTVEC_41
    #define INTVEC_41      INT41_Handler
#endif
#ifndef INTVEC_42
    #define INTVEC_42      INT42_Handler
#endif
#ifndef INTVEC_43
    #define INTVEC_43      INT43_Handler
#endif
#ifndef INTVEC_44
    #define INTVEC_44      INT44_Handler
#endif
#ifndef INTVEC_45
    #define INTVEC_45      INT45_Handler
#endif
#ifndef INTVEC_46
    #define INTVEC_46      INT46_Handler
#endif
#ifndef INTVEC_47
    #define INTVEC_47      INT47_Handler
#endif

#define INTVEC(Num) void INT##Num##_Handler(void)

INTVEC(02);
INTVEC(03);
INTVEC(04);
INTVEC(05);
INTVEC(06);
INTVEC(07);
INTVEC(08);
INTVEC(09);
INTVEC(10);
INTVEC(11);
INTVEC(12);
INTVEC(13);
INTVEC(14);
INTVEC(15);
INTVEC(16);
INTVEC(17);
INTVEC(18);
INTVEC(19);
INTVEC(20);
INTVEC(21);
INTVEC(22);
INTVEC(23);
INTVEC(24);
INTVEC(25);
INTVEC(26);
INTVEC(27);
INTVEC(28);
INTVEC(29);
INTVEC(30);
INTVEC(31);
INTVEC(32);
INTVEC(33);
INTVEC(34);
INTVEC(35);
INTVEC(36);
INTVEC(37);
INTVEC(38);
INTVEC(39);
INTVEC(40);
INTVEC(41);
INTVEC(42);
INTVEC(43);
INTVEC(44);
INTVEC(45);
INTVEC(46);
INTVEC(47);

RET_CODE INT_SetCallBack(uint8_t IntNum, INT_Callback_T pfnCallback);
void INT_CallBack(uint8_t IntNum, void *Param);

#endif /*__INTERRUPT_H*/

