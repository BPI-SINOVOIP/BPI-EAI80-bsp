/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

extern void sema_api_main(void);
extern void semaphore_main(void);
extern void mslab_main(void);
extern void thread_main(void);
extern void irq_offload_main(void);
extern void pipe_main(void);
extern void mbox_main(void);
extern void msgq_main(void);
extern void context_main(void);
extern void queue_main(void);
extern void stack_main(void);
extern void pending_main(void);
extern void timer_main(void);

/*test case main entry*/
void test_main(void)
{
	timer_main();
}
