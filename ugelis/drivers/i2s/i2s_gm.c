/*
 * Copyright (c) 2016, Gree Instruments Incorporated
 *
 */

#include <i2s.h>
#include <soc.h>
#include <misc/byteorder.h>
#include <misc/__assert.h>
#include <gm_hal_sai.h>
#include <arch/cpu.h>
#include <misc/printk.h>
#include <errno.h>
#include <kernel.h>
#include <misc/util.h>
#define SYS_LOG_NO_NEWLINE
#define SYS_LOG_LEVEL CONFIG_SYS_LOG_SAI_LEVEL
#include <logging/sys_log.h>
#include <string.h>
#include <init.h>
#include <device.h>
#include "i2s_gm.h"

#define DATA_SIZE 511
typedef void (*sai_irq_callback_t)(uint8_t event, unsigned long param);
#define TIMEOUT_NO_WAIT      0x00000000 /**< Transaction is over after function returns, it returns RET_ERROR if transaction is not finished. */
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF /**< Function is about to block till transaction is over */
#define TIMEOUT_NO_BLOCKING  0xFFFFFFFE /**< Function is never blocked, user will be notified by callback after transaction is over */
uint16_t s_dmaBufferLen = DATA_SIZE;
uint16_t s_dmabuffer[DATA_SIZE] = {0};

uint32_t s_saia_dmabuffer[DATA_SIZE] = {0};
uint32_t s_saib_dmabuffer[DATA_SIZE] = {0};
#define DEV_CFG(dev) \
    ((const struct sai_gm_config * const)(dev)->config->config_info)
#define DEV_DATA(dev) \
    ((struct sai_gm_data * const)(dev)->driver_data)
#define DEV_STRUCT(dev) \
    ((SAI_Block_TypeDef *)(DEV_CFG(dev))->base)

struct sai_gm_config
{
    uint32_t *base;
    uint32_t sys_clk_freq;
    void (*irq_config_func)(struct device *dev);
};
struct sai_gm_data sai_gm_data_2 ;
struct sai_gm_data sai_gm_data_0 ;
struct sai_gm_data
{
    sai_irq_callback_t cb; /**< Callback function pointer */
    SAI_HandleTypeDef sai_handle;
};
static void sai_gm_config_func_0(struct device *dev);
static void sai_gm_config_func_2(struct device *dev);

static SAI_HandleTypeDef SAIA_hdl;
static SAI_HandleTypeDef SAIB_hdl;

#ifdef CONFIG_SAI_INTERRUPT_DRIVEN
    static void sai_gm_isr(void *arg);
#endif
static void sai_gm_isr(void *arg)
{
    struct device *sai_dev = (struct device *)arg;
    struct sai_gm_data *dev_data = DEV_DATA(sai_dev);
    SAI_HandleTypeDef *sai_handle = &(dev_data->sai_handle);
    HAL_SAI_IRQHandler(sai_handle);
}

static int sai_gm_configure(struct device *dev, enum i2s_dir dir,
                            struct i2s_config *cfg)
{
    SAI_Block_TypeDef *pSAI = DEV_STRUCT(dev);
    struct sai_gm_data *priv =  DEV_DATA(dev);

    struct i2s_config *sai_cfg = NULL;
    sai_cfg = cfg;

    if (pSAI == SAI1_Block_A)
    {
        priv->sai_handle             = SAIA_hdl;
        priv->sai_handle.Instance    = SAI_ID_A;
        priv->sai_handle.pdmaBuffPtr = s_saia_dmabuffer;
        priv->sai_handle.dmaBufferLength = DATA_SIZE;
#if CONFIG_SAIA_INT
        sai_gm_config_func_0(dev);
#endif
        if (dir)
        {
            priv->sai_handle.Init.AudioMode = SAI_MODEMASTER_TX;
            priv->sai_handle.Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
            priv->sai_handle.Init.BCKOEn = SAI_BCK_ENABLE;
            priv->sai_handle.Init.MCKOEn = SAI_xCR1_MCKSOEN;
        }
        else
        {
            priv->sai_handle.Init.AudioMode = SAI_MODESLAVE_RX;
        }
    }
    else
    {
        priv->sai_handle             = SAIB_hdl;
        priv->sai_handle.Instance    = SAI_ID_B;
        priv->sai_handle.pdmaBuffPtr = s_saib_dmabuffer;
        priv->sai_handle.dmaBufferLength = DATA_SIZE;
#if CONFIG_SAIB_INT
        sai_gm_config_func_2(dev);
#endif
        if (!dir)
        {
            priv->sai_handle.Init.AudioMode = SAI_MODESLAVE_RX;
        }
        else
        {
            priv->sai_handle.Init.AudioMode = SAI_MODEMASTER_TX;
            priv->sai_handle.Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
            priv->sai_handle.Init.BCKOEn = SAI_BCK_ENABLE;
            priv->sai_handle.Init.MCKOEn = SAI_xCR1_MCKSOEN;
        }
    }
    priv->sai_handle.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_3QF;
    priv->sai_handle.Init.Synchro        = SAI_ASYNCHRONOUS;
    priv->sai_handle.Init.OutputDrive    = SAI_OUTPUTDRIVE_DISABLE;
    priv->sai_handle.Init.MCKNODIV       = SAI_MCKNODIV_DISABLE;
    priv->sai_handle.Init.MonoStereoMode = SAI_STEREOMODE;
    priv->sai_handle.Init.CompandingMode = SAI_NOCOMPANDING;
    priv->sai_handle.Init.TriState       = SAI_OUTPUT_NOTRELEASED;
    priv->sai_handle.Init.Protocol       = SAI_I2S_STANDARD;
    priv->sai_handle.Init.FirstBit       = SAI_FIRSTBIT_MSB;
    priv->sai_handle.Init.ClockSource    = SAI_PLL_11;
    priv->sai_handle.SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;

    if (sai_cfg->word_size == SAI_PROTOCOL_DATASIZE_32BIT)
    {
        priv->sai_handle.Init.DataSize = SAI_xCR1_DS;    //32bit
    }
    else if (sai_cfg->word_size == SAI_PROTOCOL_DATASIZE_24BIT)
    {
        priv->sai_handle.Init.DataSize = SAI_xCR1_DS_4;    //24bit
    }
    else if (sai_cfg->word_size == SAI_PROTOCOL_DATASIZE_16BITEXTENDED)
    {
        priv->sai_handle.Init.DataSize = SAI_xCR1_DS_3;    //20bit
    }
    else if (sai_cfg->word_size == SAI_PROTOCOL_DATASIZE_16BIT)
    {
        priv->sai_handle.Init.DataSize = SAI_xCR1_DS_2;    //16bit
    }
    else if (sai_cfg->word_size == SAI_PROTOCOL_DATASIZE_10BIT)
    {
        priv->sai_handle.Init.DataSize = SAI_xCR1_DS_1;    //10bit
    }
    else
    {
        priv->sai_handle.Init.DataSize = SAI_xCR1_DS_0;    //8bit
    }

    if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_8K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_8K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_11K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_11K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_16K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_22K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_22K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_32K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_32K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_44K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_44K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_48K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_96K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_96K;
    }
    else if (sai_cfg->frame_clk_freq == SAI_AUDIO_FREQUENCY_192K)
    {
        priv->sai_handle.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_192K;
    }

    if (HAL_SAI_InitProtocol(pSAI, &(priv->sai_handle)) != RET_OK)
    {
        return RET_ERROR;
    }
    return 0;
}
static int sai_gm_init(struct device *dev)
{
    return 0;
}

static int sai_gm_read(struct device *dev, void **mem_block, size_t *size)
{
    struct sai_gm_data *data = DEV_DATA(dev);
    int32_t status;
    struct sai_gm_data *priv =  DEV_DATA(dev);
    SAI_Block_TypeDef *pSAI = DEV_STRUCT(dev);
#if (CONFIG_SAIA_INT || CONFIG_SAIB_INT)
    {
        if ((pSAI == SAI1_Block_A) && (CONFIG_SAIA_INT))
        {
            status = HAL_SAI_Receive_IT(&(priv->sai_handle), (uint32_t *)(*mem_block), (uint16_t)size);
        }
        if ((pSAI == SAI1_Block_B) && (CONFIG_SAIB_INT))
        {
            status = HAL_SAI_Receive_IT(&(priv->sai_handle), (uint32_t *)(*mem_block), (uint16_t)size);
        }
    }
#endif
#if (CONFIG_SAIA_DMA || CONFIG_SAIB_DMA)
    {
        if ((pSAI == SAI1_Block_A) && (CONFIG_SAIA_DMA))
        {
            status = HAL_SAI_Receive_DMA(&(priv->sai_handle), SAI_MASTER_MODE, (uint32_t *)(*mem_block), (uint16_t)size);
        }
        if ((pSAI == SAI1_Block_B) && (CONFIG_SAIB_DMA))
        {
            status = HAL_SAI_Receive_DMA(&(priv->sai_handle), SAI_SLAVE_MODE, (uint32_t *)(*mem_block), (uint16_t)size);
        }
    }
#endif
    /* Iterate over all the messages */
    return status;
}
static int sai_gm_write(struct device *dev, void *mem_block, size_t size)
{
    struct sai_gm_data *data = DEV_DATA(dev);
    int32_t status;
    struct sai_gm_data *priv =  DEV_DATA(dev);
    SAI_Block_TypeDef *pSAI = DEV_STRUCT(dev);
#if (CONFIG_SAIA_INT || CONFIG_SAIB_INT)
    {
        if ((pSAI == SAI1_Block_A) && (CONFIG_SAIA_INT))
        {
            status = HAL_SAI_Transmit_IT(&(priv->sai_handle), mem_block, DATA_SIZE);
        }
        if ((pSAI == SAI1_Block_B) && (CONFIG_SAIB_INT))
        {
            status = HAL_SAI_Transmit_IT(&(priv->sai_handle), mem_block, DATA_SIZE);
        }
    }
#endif
#if (CONFIG_SAIA_DMA || CONFIG_SAIB_DMA)
    {
        if ((pSAI == SAI1_Block_A) && (CONFIG_SAIA_DMA))
        {
            status = HAL_SAI_Transmit_DMA(&(priv->sai_handle), SAI_MASTER_MODE, (uint32_t *)(mem_block), DATA_SIZE);
        }
        if ((pSAI == SAI1_Block_B) && (CONFIG_SAIB_DMA))
        {
            status = HAL_SAI_Transmit_DMA(&(priv->sai_handle), SAI_SLAVE_MODE, (uint32_t *)(mem_block), DATA_SIZE);
        }
    }
#endif
    /* Iterate over all the messages */
    return status;
}
static const struct i2s_driver_api sai_gm_driver_api =
{
    .configure = sai_gm_configure,
    .read      = sai_gm_read,
    .write     = sai_gm_write,
};

#ifdef CONFIG_SAIA
static const struct sai_gm_config sai_gm_config_0 =
{
    .base = CONFIG_SAIA_BASE,
    //.sys_clk_freq = I2C_CLOCK,
    .irq_config_func = sai_gm_config_func_0,
    //.default_cfg.raw = CONFIG_I2C_0_DEFAULT_CFG,
};

DEVICE_AND_API_INIT(SAIA_DEV_NAME, CONFIG_SAIA_NAME, &sai_gm_init,
                    &sai_gm_data_0, &sai_gm_config_0,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &sai_gm_driver_api);
static void sai_gm_config_func_0(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(SAIA_IRQn, 3,
                sai_gm_isr, DEVICE_GET(SAIA_DEV_NAME), 0);
    irq_enable(SAIA_IRQn);
}
#endif /* CONFIG_SAIA */

#ifdef CONFIG_SAIB
static const struct sai_gm_config sai_gm_config_2 =
{
    .base = CONFIG_SAIB_BASE,
    //.sys_clk_freq = I2C_CLOCK,
    .irq_config_func = sai_gm_config_func_2,
    //.default_cfg.raw = CONFIG_I2C_2_DEFAULT_CFG,
};

DEVICE_AND_API_INIT(SAIB_DEV_NAME, CONFIG_SAIB_NAME, &sai_gm_init,
                    &sai_gm_data_2, &sai_gm_config_2,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &sai_gm_driver_api);

static void sai_gm_config_func_2(struct device *dev)
{
    IRQ_CONNECT(SAIB_IRQn, 3,
                sai_gm_isr, DEVICE_GET(SAIB_DEV_NAME), 0);
    irq_enable(SAIB_IRQn);
}
#endif
