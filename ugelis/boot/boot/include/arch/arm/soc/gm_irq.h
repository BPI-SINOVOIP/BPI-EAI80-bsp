#ifndef _GM_IRQ_H_
#define _GM_IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include<irq.h>

#define HAL_IRQ_ENABLE(irqNUm)  NVIC_EnableIRQ((IRQn_Type)irqNUm)
#define HAL_IRQ_DISABLE(irqNUm)  NVIC_DisableIRQ((IRQn_Type)irqNUm)

//#define HAL_RequestIrq(irq_p, priority_p, isr_p, isr_param_p) \
//	    IRQ_CONNECT(irq_p, priority_p, isr_p, isr_param_p, 0)




#ifdef __cplusplus
}
#endif


#endif










































