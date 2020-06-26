/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file pdm_example.c
 *
 * @version 1.0.0
 *
 * @date 2019/11/02
 *
 * @contents Verification and example code of PDM.
 *
 * @history
 *  Date               By             Comments          Ver.
 *  =========       =========        =========        =========
 *  2019/11/02    Robert_hu       Create           1.0.0
 **********************************************************************/
#include <gm_common.h>
//#include "stdio.h"
#include <gm_hal_pdm.h>
#include <pinmux.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#include <gm_hal_rtc.h>
#include <gm_hal_dma.h>
#include <gm_hal_gpio.h>
#include <pdm.h>
#include <ugelis.h>
#include <device.h>
#include <init.h>
/******************************************************************************
* Constants and macros
******************************************************************************/
#define PDM_MASTER_DEVICE_B       (PDMB_DEV)
#define PDM_MASTER_DEVICE_A       (PDMA_DEV)
#define PDM_MAX_DATALEN         16000 * 2
#define RECORD_TIMES            2
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;
/******************************************************************************
* Global variables
******************************************************************************/
uint32_t s_mstReceCmplt = 0;
uint32_t a_mstReceCmplt = 0;
uint32_t b_mstReceCmplt = 0;
PDM_Handle_T    g_PDMSlvHandle;
static int16_t  g_rdbuf[PDM_MAX_DATALEN] = {0}; // 1 second buffer.
static int16_t  g_pdma_rdbuf[PDM_MAX_DATALEN] = {0}; // 1 second buffer.
static int16_t  g_pdmb_rdbuf[PDM_MAX_DATALEN] = {0}; // 1 second buffer.
static int16_t  g_copybuf_a[PDM_MAX_DATALEN * RECORD_TIMES] = {0};
static int16_t  g_copybuf_b[PDM_MAX_DATALEN * RECORD_TIMES] = {0};
static int32_t s_dmabuffer[PDM_MAX_DATALEN] = {0};
static uint32_t s_dmaBufferLen = PDM_MAX_DATALEN;
static int32_t pingpong_offset_a = 0;
static int32_t pingpong_offset_b = 0;
PDM_Handle_T    g_PDMSlvHandle_a;
PDM_Handle_T    g_PDMSlvHandle_b;

void pdmAReceiveCallback(struct PDM_Handle_T *pPDM, uint8_t event, unsigned long param)
{
    if (event == PDM_DMAMST_RECEIVE_EVENT)
    {
        a_mstReceCmplt += 1 ;
        memcpy(&g_copybuf_a[pingpong_offset_a], &g_pdma_rdbuf[0], (PDM_MAX_DATALEN / 2)*sizeof(int16_t));
        pingpong_offset_a += PDM_MAX_DATALEN / 2 ;
    }
    if (event == PDM_DMASLV_RECEIVE_EVENT)
    {
        a_mstReceCmplt += 1 ;
        memcpy(&g_copybuf_a[pingpong_offset_a], &g_pdma_rdbuf[PDM_MAX_DATALEN / 2], (PDM_MAX_DATALEN / 2)*sizeof(int16_t));
        pingpong_offset_a += PDM_MAX_DATALEN / 2 ;
    }
}
void pdmBReceiveCallback(struct PDM_Handle_T *pPDM, uint8_t event, unsigned long param)
{
    if (event == PDM_DMAMST_RECEIVE_EVENT)
    {
        b_mstReceCmplt += 1;
        memcpy(&g_copybuf_b[pingpong_offset_b], &g_pdmb_rdbuf[0], (PDM_MAX_DATALEN / 2)*sizeof(int16_t));
        pingpong_offset_b += PDM_MAX_DATALEN / 2;
    }
    if (event == PDM_DMASLV_RECEIVE_EVENT)
    {
        b_mstReceCmplt += 1;
        memcpy(&g_copybuf_b[pingpong_offset_b], &g_pdmb_rdbuf[PDM_MAX_DATALEN / 2], (PDM_MAX_DATALEN / 2)*sizeof(int16_t));
        pingpong_offset_b += PDM_MAX_DATALEN / 2;
    }
}
/** Configure pinmux for an available PDM device */
RET_CODE PDMAPinMuxConfig()
{
    int ret = 0;
    struct device *portj = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
    if (portj == NULL)
    {
        return -1;
    }
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_pdmPinMux[2] =
    {
        {PINMUX_GPIO_PIN_3, GPIOJ, GPIOJ3_AF4_PDM_BCK_A},
        {PINMUX_GPIO_PIN_4, GPIOJ, GPIOJ4_AF4_PDM_SD_A},
    };
    pin_cfg.pin = BIT(s_pdmPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[0].alternate;
    ret = pinmux_pin_set(portj, s_pdmPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_pdmPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[1].alternate;
    ret = pinmux_pin_set(portj, s_pdmPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
    return 0;
}
RET_CODE PDMBPinMuxConfig()
{
    int ret = 0;
    struct device *portj = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
    if (portj == NULL)
    {
        return -1;
    }
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_pdmPinMux[2] =
    {
        {PINMUX_GPIO_PIN_7, GPIOJ, GPIOJ7_AF4_PDM_BCK_B},
        {PINMUX_GPIO_PIN_5, GPIOJ, GPIOJ5_AF4_PDM_SD_B},
    };
    pin_cfg.pin = BIT(s_pdmPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[0].alternate;
    ret = pinmux_pin_set(portj, s_pdmPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_pdmPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[1].alternate;
    ret = pinmux_pin_set(portj, s_pdmPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
    return 0;
}
void pdm_clk_choose(void)
{
    HW32_REG(0x40000198) |= (1 << 0); //D_PLLPDM_EN
    HW32_REG(0x40000190) |= (1 << 0); //D_PLLI2S_EN
    HW32_REG(0x400000ac) |= (1 << 4); //PDMA_CLK_SEL  48M
    HW32_REG(0x400000ac) |= (1 << 6); //PDMB_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 12); //PDMC_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 14); //PDMD_CLK_SEL
}

int single_mode()
{
    int ret = 0;
    PDM_Config_T pdm_config;
    RET_CODE retval;
    struct device *pdm_dev;
    printf("test_single_mode Start\n");


    pdm_dev = device_get_binding(DEV_PDMA_NAME);
    if (pdm_dev == NULL)
    {
        printk("Failed to get PDM device.");
        return 1;
    }

    pdm_clk_choose();
    PDMBPinMuxConfig();
    pdm_config.DataSize            = PDM_DATASIZE_16;
    pdm_config.Fifo_Thr            = PDM_FIFO_THRESHOLD_FULL;
    pdm_config.Cid                 = DISABLE;
    pdm_config.CH_FLAG             = PDM_CH_RIGHT;
    pdm_config.Capt                = ENABLE;
    pdm_config.Shift_Level         = PDM_SHIFT_LEVEL_6;
    pdm_config.bckdiv              = PDM_BCK_1M;
    pdm_config.FS                  = PDM_FS_16K;
    pdm_config.Dual_Ch             = PDM_Dual_R;  //right channel
    pdm_config.NBSlot              = PDM_NB_SLOT_2;
    pdm_config.SlotSize            = PDM_SLOTSZ_32;
    pdm_config.enableDMA           = TRUE;
    pdm_config.dmaBufferLength     = s_dmaBufferLen;
    pdm_config.pdmaBuffer          = s_dmabuffer;
    pdm_config.pdm_callback        = pdmBReceiveCallback;

    retval = HAL_PDM_Init(&g_PDMSlvHandle, PDM_MASTER_DEVICE_B, &pdm_config);
    if (retval != HAL_PDM_OK)
    {
        printk("HAL_PDM_Init() fail %d!\n", retval);
        return HAL_PDM_ERROR;
    }
    retval = HAL_PDM_Receive_DMA(&g_PDMSlvHandle, PDM_MASTER_MODE, g_rdbuf, PDM_MAX_DATALEN);
    if (retval != HAL_PDM_OK)
    {
        printk("HAL_PDM_Receive_DMA() fail %d!\n", retval);
        return HAL_PDM_ERROR;
    }

    while (1)
    {
        // Record nRecordTimes seconds
        if (s_mstReceCmplt == RECORD_TIMES * 2)
        {
            break;
        }
    }
    printk("s_mstReceCmplt = %d\n", s_mstReceCmplt);
    printk("pingpong_offset_b = %d\n", pingpong_offset_b);
    s_mstReceCmplt = 0;
    HAL_DMA_Abort(g_PDMSlvHandle.hDMA);
    printf("test_pdm_dma Stop\n");

    return HAL_PDM_OK;
}
int double_mode()
{
    int ret = 0;
    PDM_Config_T pdm_config_a;
    PDM_Config_T pdm_config_b;
    RET_CODE retval;
    struct device *pdm_dev;
    printk("test_stereo_mode Start\n");


    pdm_dev = device_get_binding(DEV_PDMA_NAME);
    if (pdm_dev == NULL)
    {
        printk("Failed to get PDM device.");
        return 1;
    }

    pdm_clk_choose();
    PDMAPinMuxConfig();
    PDMBPinMuxConfig();
    pdm_config_b.DataSize            = PDM_DATASIZE_16;
    pdm_config_b.Fifo_Thr            = PDM_FIFO_THRESHOLD_FULL;
    pdm_config_b.Cid                 = DISABLE;
    pdm_config_b.CH_FLAG             = PDM_CH_RIGHT;
    pdm_config_b.Capt                = ENABLE;
    pdm_config_b.Shift_Level         = PDM_SHIFT_LEVEL_6;
    pdm_config_b.bckdiv              = PDM_BCK_1M;
    pdm_config_b.FS                  = PDM_FS_16K;
    pdm_config_b.Dual_Ch             = PDM_Dual_R;  //right channel
    pdm_config_b.NBSlot              = PDM_NB_SLOT_2;
    pdm_config_b.SlotSize            = PDM_SLOTSZ_32;
    pdm_config_b.enableDMA           = TRUE;
    pdm_config_b.dmaBufferLength     = NULL;
    pdm_config_b.pdmaBuffer          = NULL;
    pdm_config_b.pdm_callback        = pdmBReceiveCallback;

    /*init pdm b*/
    retval = HAL_PDM_Init(&g_PDMSlvHandle_b, PDM_MASTER_DEVICE_B, &pdm_config_b);
    if (retval != HAL_PDM_OK)
    {
        printk("HAL_PDM_Init() fail %d!\n", retval);
        return HAL_PDM_ERROR;
    }

    pdm_config_a.DataSize            = PDM_DATASIZE_16;
    pdm_config_a.Fifo_Thr            = PDM_FIFO_THRESHOLD_FULL;
    pdm_config_a.Cid                 = DISABLE;
    pdm_config_a.CH_FLAG             = PDM_CH_RIGHT;
    pdm_config_a.Capt                = ENABLE;
    pdm_config_a.Shift_Level         = PDM_SHIFT_LEVEL_6;
    pdm_config_a.bckdiv              = PDM_BCK_1M;
    pdm_config_a.FS                  = PDM_FS_16K;
    pdm_config_a.Dual_Ch             = PDM_Dual_R;  //right channel
    pdm_config_a.NBSlot              = PDM_NB_SLOT_2;
    pdm_config_a.SlotSize            = PDM_SLOTSZ_32;
    pdm_config_a.enableDMA           = TRUE;
    pdm_config_a.dmaBufferLength     = NULL;
    pdm_config_a.pdmaBuffer          = NULL;
    pdm_config_a.pdm_callback          = pdmAReceiveCallback;
    /*init pdm a*/

    retval = HAL_PDM_Init(&g_PDMSlvHandle_a, PDM_MASTER_DEVICE_A, &pdm_config_a);
    if (retval != HAL_PDM_OK)
    {
        printk("HAL_PDM_Init() fail %d!\n", retval);
        return HAL_PDM_ERROR;
    }


    retval = HAL_PDM_Receive_DMA(&g_PDMSlvHandle_b, PDM_MASTER_MODE, g_pdmb_rdbuf, PDM_MAX_DATALEN);
    if (retval != HAL_PDM_OK)
    {
        printk("HAL_PDM_Receive_DMA() fail %d!\n", retval);
        return HAL_PDM_ERROR;
    }

    retval = HAL_PDM_Receive_DMA(&g_PDMSlvHandle_a, PDM_MASTER_MODE, g_pdma_rdbuf, PDM_MAX_DATALEN);
    if (retval != HAL_PDM_OK)
    {
        printk("HAL_PDM_Receive_DMA() fail %d!\n", retval);
        return HAL_PDM_ERROR;
    }

    while (1)
    {
        // Record nRecordTimes seconds
        if ((a_mstReceCmplt >= RECORD_TIMES * 2) && (b_mstReceCmplt >= RECORD_TIMES * 2))
        {
            break;
        }
    }
    printk("a_mstReceCmplt = %d\n", a_mstReceCmplt);
    printk("b_mstReceCmplt = %d\n", b_mstReceCmplt);
    printk("pingpong_offset_a = %d\n", pingpong_offset_a);
    printk("pingpong_offset_b = %d\n", pingpong_offset_b);
    a_mstReceCmplt = 0;
    b_mstReceCmplt = 0;
    HAL_DMA_Abort(g_PDMSlvHandle_a.hDMA);
    HAL_DMA_Abort(g_PDMSlvHandle_b.hDMA);
    printk("test_pdm_dma Stop\n");

    return HAL_PDM_OK;
}
int stereo_mode()
{
    int ret = 0;
    PDM_Config_T pdm_config;
    RET_CODE retval;
    struct device *pdm_dev;
    printk("test_stereo_mode Start\n");


    pdm_dev = device_get_binding(DEV_PDMA_NAME);
    if (pdm_dev == NULL)
    {
        printk("Failed to get PDM device.");
        return 1;
    }
    pdm_clk_choose();
    PDMAPinMuxConfig();
    while (1)
    {
        pdm_config.DataSize            = PDM_DATASIZE_32;
        pdm_config.Fifo_Thr            = PDM_FIFO_THRESHOLD_FULL;
        pdm_config.Cid                 = DISABLE;
        pdm_config.CH_FLAG             = PDM_CH_RIGHT;
        pdm_config.Capt                = ENABLE;
        pdm_config.Shift_Level         = PDM_SHIFT_LEVEL_6;
        pdm_config.bckdiv              = PDM_BCK_1M;
        pdm_config.FS                  = PDM_FS_16K;
        pdm_config.Dual_Ch             = PDM_Dual_LR;  //double channel
        pdm_config.NBSlot              = PDM_NB_SLOT_2;
        pdm_config.SlotSize            = PDM_SLOTSZ_32;
        pdm_config.enableDMA           = TRUE;
        pdm_config.dmaBufferLength     = s_dmaBufferLen;
        pdm_config.pdmaBuffer          = s_dmabuffer;
        pdm_config.pdm_callback        = pdmAReceiveCallback;

        retval = HAL_PDM_Init(&g_PDMSlvHandle, PDM_MASTER_DEVICE_A, &pdm_config);
        if (retval != HAL_PDM_OK)
        {
            printk("HAL_PDM_Init() fail %d!\n", retval);
            return HAL_PDM_ERROR;
        }
        retval = HAL_PDM_Receive_DMA(&g_PDMSlvHandle, PDM_MASTER_MODE, g_pdma_rdbuf, PDM_MAX_DATALEN);

        if (retval != HAL_PDM_OK)
        {
            printk("HAL_PDM_Receive_DMA() fail %d!\n", retval);
            return HAL_PDM_ERROR;
        }

        while (1)
        {
            // Record nRecordTimes seconds
            if (a_mstReceCmplt == RECORD_TIMES * 2)
            {
                break;
            }
        }
        printk("a_mstReceCmplt = %d\n");
        printk("pingpong_offset_a = %d\n", pingpong_offset_a);
        a_mstReceCmplt = 0;
        pingpong_offset_a = 0;
        HAL_DMA_Abort(g_PDMSlvHandle.hDMA);
        HAL_PDM_Exit(&g_PDMSlvHandle);
        printk("test_stereo_mode Stop\n");
    }
    return HAL_PDM_OK;
}
int main()
{
    //single_mode();
    //double_mode();
    stereo_mode();
    while (1);
}
