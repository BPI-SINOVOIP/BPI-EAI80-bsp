/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief        
 *
 * @note
 *    2018/03/24, Mike.Zheng, V1.0.0
 *        Initial version.
 */

/**
 * @file
 * @brief ARM Cortex-M interrupt initialization
 *
 * The ARM Cortex-M architecture provides its own k_thread_abort() to deal with
 * different CPU modes (handler vs thread) when a thread aborts. When its entry
 * point returns or when it aborts itself, the CPU is in thread mode and must
 * call _Swap() (which triggers a service call), but when in handler mode, the
 * CPU must exit handler mode to cause the context switch, and thus must queue
 * the PendSV exception.
 */

#include <toolchain.h>
#include <linker/sections.h>
#include <kernel.h>
#include <arch/cpu.h>
#include <arch/arm/cortex_m/cmsis.h>

/**
 *
 * @brief Initialize interrupts
 *
 * Ensures all interrupts have their priority set to _EXC_IRQ_DEFAULT_PRIO and
 * not 0, which they have it set to when coming out of reset. This ensures that
 * interrupt locking via BASEPRI works as expected.
 *
 * @return N/A
 */

void _IntLibInit(void)
{
	int irq = 0;

	for (; irq < CONFIG_NUM_IRQS; irq++) {
		NVIC_SetPriority((IRQn_Type)irq, _IRQ_PRIO_OFFSET);
	}
}
