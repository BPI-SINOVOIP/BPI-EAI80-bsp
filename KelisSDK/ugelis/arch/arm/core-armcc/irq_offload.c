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
 * @file Software interrupts utility code - ARM implementation
 */

#include <kernel.h>
#include <irq_offload.h>

volatile irq_offload_routine_t offload_routine;
static void *offload_param;

/* Called by __svc */
void _irq_do_offload(void)
{
	offload_routine(offload_param);
}

void irq_offload(irq_offload_routine_t routine, void *parameter)
{
#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE) && defined(CONFIG_ASSERT)
	/* Cortex M0 hardfaults if you make a SVC call with interrupts
	 * locked.
	 */
	unsigned int key;

	__asm__ volatile("mrs %0, PRIMASK;" : "=r" (key) : : "memory");
	__ASSERT(key == 0, "irq_offload called with interrupts locked\n");
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE && CONFIG_ASSERT */

	k_sched_lock();
	offload_routine = routine;
	offload_param = parameter;

	__asm volatile 
	{
		svc #_SVC_CALL_IRQ_OFFLOAD
		DSB
		ISB
	}

	offload_routine = NULL;
	k_sched_unlock();
}
