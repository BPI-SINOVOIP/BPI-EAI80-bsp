#include <errno.h>
#include <stdio.h>
#include <kernel.h>
#include <board.h>
#include <device.h>
#include <init.h>
#include <dma.h>

/*gm hal include files*/
#include <gm_hal_dma.h>

/* manual define here currently, should move to dts in the feauture */
#define CONFIG_DMA_IRQ_PRI 0
#define CONFIG_DMA_IRQn 4//DMA_IRQn

#define CONFIG_DMA_NAME "DMA"

extern void HAL_DMA_IRQHandler(void);

static void dma_gm_config_irq(struct device *dev);

/* use one isr for all dma streams, maybe need divide to absolute isr for each stream */
static void dma_gm_isr(void *arg)
{
    HAL_DMA_IRQHandler();
    return;
}

int dma_gm_init(struct device *dev)
{
    dma_gm_config_irq(dev);

    return 0;
}



static void dma_gm_config_irq(struct device *dev)
{
    ARG_UNUSED(dev);

    /* DMA1 stream 0~7 */
    IRQ_CONNECT(CONFIG_DMA_IRQn, CONFIG_DMA_IRQ_PRI,
                dma_gm_isr, NULL, 0);
    irq_enable(CONFIG_DMA_IRQn);
}



SYS_INIT(dma_gm_init, PRE_KERNEL_1, DEVICE_PRIORITY_DMA);


