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
#include <display/fb_display.h>
#include <watchdog.h>

#include "inc/config.h"
#include "dev.h"

#define IMP_DEVICE (IMP_Device_T *)(0x40024000)

#define DEBUG_PRINT(...)
//#define DEBUG_PRINT printf

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
    HAL_Reset_Module(RESET_STF_SW_RSTJ);
    HAL_Reset_Module(RESET_KDPSRAM_SW_RSTJ);
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
    dev_kmalloc_statics();
    return 0;
}

int ai_imp_scale(void *ltdc, scaler_param_t *param)
{
    fb_scaler(ltdc, (struct fb_scalerinfo *)param);
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
    else
    {
        printf("malloc error size %d\n", size);
    }
    return ptr;
}

void kfree(void *ptr)
{
    DEBUG_PRINT("kfree 0x%x\n", ptr);
    free_ext(0, ptr);
}
