#include <kernel.h>
#include <device.h>
#include <init.h>
#include <soc.h>
#include <arch/cpu.h>
#include <cortex_m/exc.h>

static unsigned int gkey = 0;


/*impliemnt hal some common weak link function*/
uint32_t HAL_GetTick(void)
{
	return k_uptime_get_32();
}

void HAL_DelayMs(uint32_t ms)
{
	return k_sleep(ms);
}

void HAL_DelayUs(uint32_t us)
{
	return k_busy_wait(us);
}

void HAL_IrqLock(void)
{
    gkey = irq_lock();
}

void HAL_IrqUnlock(void)
{
    irq_unlock(gkey);
}
void HAL_DisableIrq(void)
{

}

void HAL_EnableIrq(void)
{

}



