/*
 * Copyright (c) 2018 Open-RnD Sp. z o.o.
 * Copyright (c) 2018 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief System/hardware module for STM32F4 processor
 */

#include <device.h>
#include <init.h>
#include <arch/cpu.h>
#include <cortex_m/exc.h>

/**
 * @brief Perform basic hardware initialization at boot.
 *
 * This needs to be run from the very beginning.
 * So the init priority has to be 0 (zero).
 *
 * @return 0
 */
static int gm6721x_init(struct device *arg)
{
    u32_t key;

    ARG_UNUSED(arg);

    key = irq_lock();

    _ClearFaults();

    /* Install default handler that simply resets the CPU
     * if configured in the kernel, NOP otherwise
     */
    NMI_INIT();

    irq_unlock(key);

    /*call hal SystemInit: flash clk and other system hw init*/
    SystemInit();

    return 0;
}


SYS_INIT(gm6721x_init, PRE_KERNEL_1, DEVICE_PRIORITY_SYSTEM_INIT);

