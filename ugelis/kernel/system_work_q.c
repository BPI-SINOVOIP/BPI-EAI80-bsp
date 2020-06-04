/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          system_work_q.c
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
 *
 * System workqueue.
 */

#include <kernel.h>
#include <init.h>

K_THREAD_STACK_DEFINE(sys_work_q_stack, CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE);

struct k_work_q k_sys_work_q;

static int k_sys_work_q_init(struct device *dev)
{
	ARG_UNUSED(dev);

	k_work_q_start(&k_sys_work_q,
		       sys_work_q_stack,
		       K_THREAD_STACK_SIZEOF(sys_work_q_stack),
		       CONFIG_SYSTEM_WORKQUEUE_PRIORITY);

	return 0;
}

SYS_INIT(k_sys_work_q_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
