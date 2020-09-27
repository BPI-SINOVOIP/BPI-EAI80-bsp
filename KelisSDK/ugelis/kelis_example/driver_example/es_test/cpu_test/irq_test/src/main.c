/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *
 * Set up three software IRQs: the ISR for each will print that it runs and
 * then release a semaphore. The task then verifies it can obtain all three
 * semaphores.
 *
 * The ISRs are installed at build time, directly in the vector table.
 */


#include <arch/cpu.h>
#include <arch/arm/cortex_m/cmsis.h>
#include <linker/sections.h>
#include <misc/printk.h>


void isr0(void)
{
    printk("%s ran!\n", __func__);
}


void isr1(void)
{
    printk("%s ran!\n", __func__);
}

void isr3(void)
{
    printk("%s ran!\n", __func__);
}

void isr5(void)
{
    printk("%s ran!\n", __func__);
}


void isr6(void)
{
    printk("%s ran!\n", __func__);
}

void isr7(void)
{
    printk("%s ran!\n", __func__);
}


void isr8(void)
{
    printk("%s ran!\n", __func__);
}


void isr9(void)
{
    printk("%s ran!\n", __func__);
}

void isr14(void)
{
    printk("%s ran!\n", __func__);
}



void isr15(void)
{
    printk("%s ran!\n", __func__);
}


void isr116(void)
{
    printk("%s ran!\n", __func__);
}

void isr17(void)
{
    printk("%s ran!\n", __func__);
}



void isr20(void)
{
    printk("%s ran!\n", __func__);
}


void isr21(void)
{
    printk("%s ran!\n", __func__);
}


void isr22(void)
{
    printk("%s ran!\n", __func__);
}

void isr23(void)
{
    printk("%s ran!\n", __func__);
}


void isr24(void)
{
    printk("%s ran!\n", __func__);
}

void isr25(void)
{
    printk("%s ran!\n", __func__);
}

void isr26(void)
{
    printk("%s ran!\n", __func__);
}

void isr27(void)
{
    printk("%s ran!\n", __func__);
}


void isr28(void)
{
    printk("%s ran!\n", __func__);
}

void isr29(void)
{
    printk("%s ran!\n", __func__);
}

void isr30(void)
{
    printk("%s ran!\n", __func__);
}


void isr31(void)
{
    printk("%s ran!\n", __func__);
}


void isr32(void)
{
    printk("%s ran!\n", __func__);
}


void isr33(void)
{
    printk("%s ran!\n", __func__);
}


void isr34(void)
{
    printk("%s ran!\n", __func__);
}

void isr37(void)
{
    printk("%s ran!\n", __func__);
}



void isr39(void)
{
    printk("%s ran!\n", __func__);
}

void isr40(void)
{
    printk("%s ran!\n", __func__);
}




void isr41(void)
{
    printk("%s ran!\n", __func__);
}

void isr42(void)
{
    printk("%s ran!\n", __func__);
}



void isr43(void)
{
    printk("%s ran!\n", __func__);
}

void isr44(void)
{
    printk("%s ran!\n", __func__);
}

void isr45(void)
{
    printk("%s ran!\n", __func__);
}


void isr49(void)
{
    printk("%s ran!\n", __func__);
}

void isr50(void)
{
    printk("%s ran!\n", __func__);
}


void isr53(void)
{
    printk("%s ran!\n", __func__);
}

void isr54(void)
{
    printk("%s ran!\n", __func__);
}


void isr55(void)
{
    printk("%s ran!\n", __func__);
}

void isr56(void)
{
    printk("%s ran!\n", __func__);
}

void isr57(void)
{
    printk("%s ran!\n", __func__);
}


void isr60(void)
{
    printk("%s ran!\n", __func__);
}


void isr61(void)
{
    printk("%s ran!\n", __func__);
}

void isr62(void)
{
    printk("%s ran!\n", __func__);
}

void isr63(void)
{
    printk("%s ran!\n", __func__);
}

void isr64(void)
{
    printk("%s ran!\n", __func__);
}


void isr16(void)
{
    printk("%s ran!\n", __func__);
}

void isr78(void)
{
    printk("%s ran!\n", __func__);
}


void isr10(void)
{
    printk("%s ran!\n", __func__);
}




void main()
{
    uint32_t i ;




    IRQ_CONNECT(0, 0, isr0, 0, 0);
    irq_enable(0);
    NVIC_SetPendingIRQ(0);


    IRQ_CONNECT(1, 0, isr1, 0, 0);
    irq_enable(1);
    NVIC_SetPendingIRQ(1);


    IRQ_CONNECT(3, 0, isr3, 0, 0);
    irq_enable(3);
    NVIC_SetPendingIRQ(3);

    IRQ_CONNECT(5, 0, isr5, 0, 0);
    irq_enable(5);
    NVIC_SetPendingIRQ(5);

    IRQ_CONNECT(6, 0, isr6, 0, 0);
    irq_enable(6);
    NVIC_SetPendingIRQ(6);

    IRQ_CONNECT(7, 0, isr7, 0, 0);
    irq_enable(7);
    NVIC_SetPendingIRQ(7);


    IRQ_CONNECT(8, 0, isr8, 0, 0);
    irq_enable(8);
    NVIC_SetPendingIRQ(8);

    IRQ_CONNECT(9, 0, isr9, 0, 0);
    irq_enable(9);
    NVIC_SetPendingIRQ(9);

    IRQ_CONNECT(10, 0, isr10, 0, 0);
    irq_enable(10);
    NVIC_SetPendingIRQ(10);


    IRQ_CONNECT(14, 0, isr14, 0, 0);
    irq_enable(14);
    NVIC_SetPendingIRQ(14);


    IRQ_CONNECT(15, 0, isr15, 0, 0);
    irq_enable(15);
    NVIC_SetPendingIRQ(15);

    IRQ_CONNECT(16, 0, isr16, 0, 0);
    irq_enable(16);
    NVIC_SetPendingIRQ(16);

    IRQ_CONNECT(17, 0, isr17, 0, 0);
    irq_enable(17);
    NVIC_SetPendingIRQ(17);



    IRQ_CONNECT(20, 0, isr20, 0, 0);
    irq_enable(20);
    NVIC_SetPendingIRQ(20);


    IRQ_CONNECT(21, 0, isr21, 0, 0);
    irq_enable(21);
    NVIC_SetPendingIRQ(21);

    IRQ_CONNECT(22, 0, isr22, 0, 0);
    irq_enable(22);
    NVIC_SetPendingIRQ(22);

    IRQ_CONNECT(23, 0, isr23, 0, 0);
    irq_enable(23);
    NVIC_SetPendingIRQ(23);


    IRQ_CONNECT(24, 0, isr24, 0, 0);
    irq_enable(24);
    NVIC_SetPendingIRQ(24);

    IRQ_CONNECT(25, 0, isr25, 0, 0);
    irq_enable(25);
    NVIC_SetPendingIRQ(25);

    IRQ_CONNECT(26, 0, isr26, 0, 0);
    irq_enable(26);
    NVIC_SetPendingIRQ(26);

    IRQ_CONNECT(27, 0, isr27, 0, 0);
    irq_enable(27);
    NVIC_SetPendingIRQ(27);

    IRQ_CONNECT(28, 0, isr28, 0, 0);
    irq_enable(28);
    NVIC_SetPendingIRQ(28);


    IRQ_CONNECT(29, 0, isr29, 0, 0);
    irq_enable(29);
    NVIC_SetPendingIRQ(29);


    IRQ_CONNECT(30, 0, isr30, 0, 0);
    irq_enable(30);
    NVIC_SetPendingIRQ(30);

    IRQ_CONNECT(31, 0, isr31, 0, 0);
    irq_enable(31);
    NVIC_SetPendingIRQ(31);

    IRQ_CONNECT(32, 0, isr32, 0, 0);
    irq_enable(32);
    NVIC_SetPendingIRQ(32);

    IRQ_CONNECT(33, 0, isr33, 0, 0);
    irq_enable(33);
    NVIC_SetPendingIRQ(33);

    IRQ_CONNECT(34, 0, isr34, 0, 0);
    irq_enable(34);
    NVIC_SetPendingIRQ(34);


    IRQ_CONNECT(37, 0, isr37, 0, 0);
    irq_enable(37);
    NVIC_SetPendingIRQ(37);


    IRQ_CONNECT(39, 0, isr39, 0, 0);
    irq_enable(39);
    NVIC_SetPendingIRQ(39);

    IRQ_CONNECT(40, 0, isr40, 0, 0);
    irq_enable(40);
    NVIC_SetPendingIRQ(40);

    IRQ_CONNECT(41, 0, isr41, 0, 0);
    irq_enable(41);
    NVIC_SetPendingIRQ(41);


    IRQ_CONNECT(42, 0, isr42, 0, 0);
    irq_enable(42);
    NVIC_SetPendingIRQ(42);

    IRQ_CONNECT(43, 0, isr43, 0, 0);
    irq_enable(43);
    NVIC_SetPendingIRQ(43);

    IRQ_CONNECT(44, 0, isr44, 0, 0);
    irq_enable(44);
    NVIC_SetPendingIRQ(44);

    IRQ_CONNECT(45, 0, isr45, 0, 0);
    irq_enable(45);
    NVIC_SetPendingIRQ(45);

    IRQ_CONNECT(49, 0, isr49, 0, 0);
    irq_enable(49);
    NVIC_SetPendingIRQ(49);

    IRQ_CONNECT(50, 0, isr50, 0, 0);
    irq_enable(50);
    NVIC_SetPendingIRQ(50);

    IRQ_CONNECT(53, 0, isr53, 0, 0);
    irq_enable(53);
    NVIC_SetPendingIRQ(53);

    IRQ_CONNECT(54, 0, isr54, 0, 0);
    irq_enable(54);
    NVIC_SetPendingIRQ(54);

    IRQ_CONNECT(55, 0, isr55, 0, 0);
    irq_enable(55);
    NVIC_SetPendingIRQ(55);

    IRQ_CONNECT(56, 0, isr56, 0, 0);
    irq_enable(56);
    NVIC_SetPendingIRQ(56);

    IRQ_CONNECT(57, 0, isr57, 0, 0);
    irq_enable(57);
    NVIC_SetPendingIRQ(57);


    IRQ_CONNECT(60, 0, isr60, 0, 0);
    irq_enable(60);
    NVIC_SetPendingIRQ(60);


    IRQ_CONNECT(61, 0, isr61, 0, 0);
    irq_enable(61);
    NVIC_SetPendingIRQ(61);

    IRQ_CONNECT(62, 0, isr62, 0, 0);
    irq_enable(62);
    NVIC_SetPendingIRQ(62);

    IRQ_CONNECT(63, 0, isr63, 0, 0);
    irq_enable(63);
    NVIC_SetPendingIRQ(63);


    IRQ_CONNECT(64, 0, isr64, 0, 0);
    irq_enable(64);
    NVIC_SetPendingIRQ(64);


    IRQ_CONNECT(78, 0, isr78, 0, 0);
    irq_enable(78);
    NVIC_SetPendingIRQ(78);


    printk("%s irq test success!\n", __func__);
    do
    {

        i++;
    } while (1);

}
