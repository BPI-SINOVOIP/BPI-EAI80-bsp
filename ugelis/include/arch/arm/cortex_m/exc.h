/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Cortex-M public exception handling
 *
 * ARM-specific kernel exception handling interface. Included by arm/arch.h.
 */

#ifndef _ARCH_ARM_CORTEXM_EXC_H_
#define _ARCH_ARM_CORTEXM_EXC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* for assembler, only works with constants */
#define _EXC_PRIO(pri) (((pri) << (8 - CONFIG_NUM_IRQ_PRIO_BITS)) & 0xff)

#if defined(CONFIG_CPU_CORTEX_M_HAS_PROGRAMMABLE_FAULT_PRIOS)
#define _EXCEPTION_RESERVED_PRIO 1
#else
#define _EXCEPTION_RESERVED_PRIO 0
#endif

#define _EXC_FAULT_PRIO 0
#ifdef CONFIG_ZERO_LATENCY_IRQS
#define _EXC_ZERO_LATENCY_IRQS_PRIO 0
#define _EXC_SVC_PRIO 1
#define _IRQ_PRIO_OFFSET (_EXCEPTION_RESERVED_PRIO + 1)
#else
#define _EXC_SVC_PRIO 0
#define _IRQ_PRIO_OFFSET (_EXCEPTION_RESERVED_PRIO)
#endif

#define _EXC_IRQ_DEFAULT_PRIO _EXC_PRIO(_IRQ_PRIO_OFFSET)

#ifdef _ASMLANGUAGE
GTEXT(_ExcExit);
GTEXT(fp_register_touch);

#else
#include <ugelis/types.h>

struct __esf {
	sys_define_gpr_with_alias(a1, r0);
	sys_define_gpr_with_alias(a2, r1);
	sys_define_gpr_with_alias(a3, r2);
	sys_define_gpr_with_alias(a4, r3);
	sys_define_gpr_with_alias(ip, r12);
	sys_define_gpr_with_alias(lr, r14);
	sys_define_gpr_with_alias(pc, r15);
	u32_t xpsr;
#ifdef CONFIG_FLOAT
	float s[16];
	u32_t fpscr;
	u32_t undefined;
#endif
};

typedef struct __esf NANO_ESF;

extern void _ExcExit(void);

/**
 * @brief display the contents of a exception stack frame
 *
 * @return N/A
 */

extern void sys_exc_esf_dump(NANO_ESF *esf);

/*
 *@ Although automatic state preservation is enabled, the processor
 * does not automatically save the volatile FP registers until they
 * have first been touched. Perform a dummy move operation so that
 * the stack frames are created as expected before any thread
 * context switching can occur. It has to be surrounded by instruction
 * synchronisation barriers to ensure that the whole sequence is
 * serialized.
 * return N/A
 */
extern void fp_register_touch(void);
 

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* _ARCH_ARM_CORTEXM_EXC_H_ */
