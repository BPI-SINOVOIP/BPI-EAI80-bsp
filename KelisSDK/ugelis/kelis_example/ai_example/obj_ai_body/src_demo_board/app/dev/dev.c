/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          dev.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Necessary initialization of the chip before normal work.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <gm_hal_vin.h>
#include <gm_hal_otp.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

#include <gm_hal_imp.h>
#include <fmath.h>
#include <display/fb_display.h>
#include "inc/config.h"

#define IMP_DEVICE (IMP_Device_T *)(0x40024000)

#define DEBUG_PRINT(...)
//#define DEBUG_PRINT printf

void ai_init_imp()
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    uint32_t input_offset = 0;
    uint32_t output_offset = 0;
    uint32_t input_pixfmt  = IMP_INPUT_RGB888;
    uint32_t output_pixfmt = FMT_RGB888;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);
}

#ifdef __USE_KEIL_
    extern unsigned char Load$$VT_REGION$$Base;
    extern unsigned char Image$$VT_REGION$$Base;
    extern unsigned char Image$$VT_REGION$$Length;
    extern unsigned char Load$$VT_REGION$$Length;
    extern unsigned char Image$$VT_REGION$$Limit;
    extern unsigned short inputImage[];
#endif

static int kmalloc_available_size = 0;
static int kmalloc_remain_size = 0;
void dev_kmalloc_statics()
{
#ifdef __USE_KEIL_
#define ALINE_BYTE 32
    int32_t mem_start = (uint32_t)(((unsigned int)&Image$$VT_REGION$$Base) + ((unsigned int)&Load$$VT_REGION$$Length));
    uint32_t mem_end   = MEM_RESERVE_START;
    kmalloc_available_size = mem_end - mem_start - ALINE_BYTE;
    kmalloc_remain_size = kmalloc_available_size;
    printf("kmalloc available size: 0x%x - 0x%x = %d\n", mem_end, mem_start, kmalloc_available_size);
#endif
}

int dev_available_size()
{
    return kmalloc_available_size - 1024;
}

int dev_init(void)
{
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);

    HAL_Reset_Module(RESET_STF_SW_RSTJ);
    HAL_Reset_Module(RESET_KDPSRAM_SW_RSTJ);//*(volatile uint32_t *)(0x40000090) |= 3 << 0;  /*fast kdp run 300MHZ*/
    HAL_Reset_Module(RESET_KDP_SW_RSTJ);
    HAL_Reset_Module(RESET_IMP_SW_RSTJ);

    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_STF);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_KDPSRAM);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_KDP);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_IMP);

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;

    *(volatile uint32_t *)(0x40000188) |= 1 << 0;

    HAL_CLOCK_AdjustPLLKDPDefaultDiv(2, 0, 0, 0);

    while ((*(volatile uint32_t *)(0x400001A4) & (1 << 5)) != (1 << 5));

    //*(volatile uint32_t *)(0x4003928C) = 0x00666606;
    /*
        6.4.    SDRAM Master AHB Bus Control Register (SDR_BC)

        Master1(VIN) AHB access Attribute
        Bit10: BL1_RD_PREFETCH_EN: Prefetch 16 bytes for burst=1 request.
        Bit9: BL4_RD_PREFETCH_EN: Prefetch 16x4 bytes for burst=4 request.
        Bit8: Reserved
    */

    *(volatile uint32_t *)(0x4003928C) &= ~0xF0;


    *(volatile uint32_t *)(0x4000009c) |= 2 << 16;

    /*Initial the SRAM specified for KDP310 */
    memset((void *)0x20020000, 0x00, 256 * 1024);

    ai_init_imp();

    dev_kmalloc_statics();
    return 0;
}


int asm inline fast_floorf_in(float x)
{
    VCVT.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

int inline fast_floorf(float x)
{
    return fast_floorf_in(x);
}
int asm inline fast_roundf_in(float x)
{
    VCVTR.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

int inline fast_roundf(float x)
{
    return fast_roundf_in(x);
}

int ai_imp_scaleup(uint32_t src, uint32_t dst, uint32_t src_width, uint32_t src_h, uint32_t dst_w, uint32_t dst_h)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t input_offset = 0;
    uint32_t output_offset = 0;
    uint32_t timeout = 0;

    float w_scale = 0.0;
    float h_scale = 0.0;
    ai_init_imp();
    pixel_num = src_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    w_scale = (float)pixel_num / (dst_w - output_offset);
    h_scale = (float)src_h / dst_h;

    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = src_h;
    scaleCfg.scaleWidth = (dst_w - output_offset);
    scaleCfg.scaleHeight = dst_h;
    scaleCfg.svi = fast_floorf(h_scale);
    scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi) * 256);
    scaleCfg.shi = fast_floorf(w_scale);
    scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi) * 256);

    HAL_IMP_Start(pIMP, (uint32_t)src, (uint32_t)dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);
    return 0;
}

int ai_imp_rgb2l(uint32_t src, uint32_t dst, uint32_t src_width, uint32_t src_h, struct device *ltdc)
{
    fb_convert_color(ltdc, src, dst, FMT_RGB888, FMT_L8, src_width, src_h);
    return 0;
}

int dev_exit(void)
{
    return 0;
}

char *kmalloc(int size)
{
    char *ptr = malloc_ext(0, size);
    if (ptr != 0)
    {
        kmalloc_remain_size -= size;
        DEBUG_PRINT("kmalloc 0x%x %10d,space:[%10d/%10d],remain: %10d\n", ptr, size, kmalloc_remain_size,
                    kmalloc_available_size, kmalloc_available_size - kmalloc_remain_size);
    }
    return ptr;
}

void kfree(char *ptr)
{
    DEBUG_PRINT("kfree 0x%x\n", ptr);
    free_ext(0, ptr);
}
