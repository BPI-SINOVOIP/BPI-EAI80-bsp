#ifndef _GM_IRQ_H_
#define _GM_IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RABBIT_NO_OS_SYSTEM
#include <irq.h>
#endif

#if defined(CONFIG_USE_HAL_TEST) || defined(RABBIT_NO_OS_SYSTEM)
    #define HAL_IRQ_ENABLE(irqNUm) NVIC_EnableIRQ((IRQn_Type)irqNUm)
#else
    #define HAL_IRQ_ENABLE(irqNUm) (void)(0)
#endif

#if defined(CONFIG_USE_HAL_TEST) || defined(RABBIT_NO_OS_SYSTEM)
    #define HAL_IRQ_DISABLE(irqNUm) NVIC_DisableIRQ((IRQn_Type)irqNUm)
#else
    #define HAL_IRQ_DISABLE(irqNUm)  (void)(0)
#endif

#if defined(CONFIG_USE_HAL_TEST)
#define HAL_RequestIrq(irq_p, priority_p, isr_p, isr_param_p) \
            IRQ_CONNECT(irq_p, priority_p, isr_p, isr_param_p, 0)
#else
    #define HAL_RequestIrq(irq_p, priority_p, isr_p, isr_param_p)   (void)(0)
#endif

#ifdef __cplusplus
}
#endif

#endif
