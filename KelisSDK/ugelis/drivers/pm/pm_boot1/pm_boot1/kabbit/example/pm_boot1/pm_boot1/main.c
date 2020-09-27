/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          main.c
 *
 * @author       richard.liu
 *
 * @version       1.0.0
 *
 * @date          2018/05/07
 *
 * @brief        Sample code for adc test case.
 *
 * @note
 *    2018/05/07, richard.liu, V1.0.0
 *        Initial version.
 */
#include <stdio.h>
#include <gm_hal_uart.h>
#include <gm_common.h>
#include <gm_hal_reset.h>
#include <gm_hal_hash.h>
#include <gm_hal_clock.h>

#define HASH_TIMEOUT_VAL        5
#ifndef HW32_REG
    #define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#endif
#define DBG_PR_EN              1
#ifdef DBG_PR_EN
    #define DBG_PR       printf
#else
    #define DBG_PR(...)  do{}while(0)
#endif
#define writel(v, addr) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))
enum pm_frame_mode
{
    SLEEP_MODE           = 0,
    STANDBY_MODE,
    SHUTDOWN_MODE,
};

enum pm_frame_hash_enable_disable
{
    HASH_DISABLE         = 0,
    HASH_ENALBE,
};

enum pm_frame_wkup_pin_enable_disable
{
    WKUP_PIN_DISABLE         = 0,
    WKUP_PIN_ENALBE,
};

enum pm_frame_wkup_pin_valid
{
    PM_RISING_EDGE      = 0,
    PM__FALLING_EDGE,
};

struct pm_frame_wkup_pin_cfg
{
    enum pm_frame_wkup_pin_enable_disable  wkup_pin_enable_disable;
    enum pm_frame_wkup_pin_valid           wkup_pin_valid;
};

enum pm_frame_sdram_size
{
    sdram_8m      = 0,
    sdram_16m,
    sdram_32m,
    sdram_64m,
};

struct pm_frame_cfg
{
    enum pm_frame_mode lp_mode;
    enum pm_frame_hash_enable_disable hash_enable_diable;
    struct pm_frame_wkup_pin_cfg wkup_pin_xpa0;
    struct pm_frame_wkup_pin_cfg wkup_pin_xpb12;
    struct pm_frame_wkup_pin_cfg wkup_pin_xpb13;
    uint8_t pm_shutdown_status_pin_inv;
    enum pm_frame_sdram_size sdram_size;
};

#define PM_CFG  ((struct pm_frame_cfg *)(0xc0000200))

uint32_t hash_start_addr;
uint32_t hash_size;

uint8_t sha512Sum[64];
static HASH_Handle_T hashHandle;
static HASH_Config_T hash_config;
static volatile uint8_t dataTransDone = FALSE;
static volatile uint8_t dataTransErr = FALSE;
static volatile uint8_t dataAvailable = FALSE;

extern void gm6721b_ddr_init(void);
extern void dram_cfg_restore(void);
typedef struct
{
    uint32_t part_num;
    uint32_t info_addr;
    uint32_t info_size;
    uint32_t info_load_addr;
    uint32_t boot1_addr;
    uint32_t boot1_size;
    uint32_t boot1_load_addr;
    uint32_t system_addr;
    uint32_t system_size;
    uint32_t system_load_addr;
#ifdef CONFIG_DUAL_EN
    uint32_t slave_addr;
    uint32_t slave_size;
    uint32_t slave_load_addr;
    uint32_t slavebl_addr;
    uint32_t slavebl_size;
    uint32_t slavebl_load_addr;
#endif
} info_t;

typedef void (*Reboot_Fun_T)(void);

static void systick_disable()
{
    *(unsigned int *)0xE000E010 = 0;
}

typedef void (*pFunction)(void);
pFunction JumpToApplication;

static void SetMSP(uint32_t topOfMainStack)
{
    register uint32_t __regMainStackPointer __asm("msp");
    __regMainStackPointer = topOfMainStack;
}

static int SetPC(uint32_t addr)
{
    uint32_t jumpAddr = 0;

    {
        /* jump to SRAM to run */
        jumpAddr = (*((volatile uint32_t *)(addr + 4)));
    }
    /* Jump to user application */
    JumpToApplication = (pFunction)jumpAddr;
    /* Initialize user application's Stack Pointer */
    SetMSP(*((volatile uint32_t *)addr));
    SysTick_Disable();
    JumpToApplication();

    return 0;
}
static void PM_SetPC(uint32_t addr)
{
    pFunction JumpToApplication;
    JumpToApplication = (pFunction)(addr + 1);
    JumpToApplication();
}

__asm void deadloopfunc(void)
{
    b .
    bx lr
}

#if 0
__asm void __resume_cpu_context(void)
{
    //    ldr r0,=0xc0700000
    //    add r0,#52
    //    ldr r1,[r0]
    //    mov r13,r1
    ldr r0, = 0xc0700000
              add r0, #4
              ldmia r0!, {r1 - r7}
              push {r1 - r7}
              ldr r1, [r0]
              add r0, #4
              ldmia r0!, {r2 - r7}

              mov r8, r1
              mov r9, r2
              mov r10, r3
              mov r11, r4
              mov r12, r5
              mov lr, r7
              ldr r1, [r0]
              //    mov r2,#0x01
              //    LSL r2,r2,#24
              //    orr r1,r1,r2
              msr psr, r1
              add r0, #4
              vldmia r0!, {s0 - s31}
              ldr r1, [r0]
              vmsr fpscr, r1

              ldr r1, = 0xc0700000
                        ldr r0, [r1]
                        pop {r1 - r7}
                        bx lr;
}
#endif
__asm void __resume_back_context(void)
{
    ldr r0, = 0xc0000004
              ldr r1, [r0]
              mov lr, r1
              bx lr
}

#if 0
__asm void __resume_cpu_contex(void)
{
    ldr r0, = 0xc0000038
              ldr r1, [r0]
              msr MSP, r1

              ldr r0, = 0xc0000004
                        ldmia r0!, {r1 - r7}
                        push {r1 - r7}
                        ldmia r0!, {r1}
                        msr psr, r1

                        ldmia r0!, {r1 - r7}

                        mov r8, r1
                        mov r9, r2
                        mov r10, r3
                        mov r11, r4
                        mov r12, r5
                        //ldr r0,=0xc0000040
                        vldmia r0!, {s0 - s31}
                        ldmia r0!, {r1}
                        vmsr fpscr, r1
                        ldr r1, [r0]
                        mov lr, r1

                        ldr r1, = 0xc0000000
                                  ldr r0, [r1]
                                  pop {r1 - r7}

                                  bx lr

}
#endif

void HASH_Callback(struct HASH_Handle *hHash, HASH_Event_T event, unsigned long param)
{
    DBG_PR("Hash callback!!\n");
    if (event == HASH_EVENT_TRANS_DONE)
    {
        dataTransDone = TRUE;
    }
    else if (event == HASH_EVENT_TRANS_ERR)
    {
        dataTransErr = TRUE;
    }
    else if (event == HASH_EVENT_DIGEST_AVAILABEL)
    {
        dataAvailable = TRUE;
    }
}

void HASH_SHA512_for_sdram_Init(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint32_t i, addr;
    memset(config, 0, sizeof(HASH_Config_T));
    config->algo = HASH_ALGO_SHA512;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HASH;
    config->callback = HASH_Callback;
    HAL_HASH_Init(hHash, config);
    HAL_HASH_Start(hHash, (uint8_t *)(hash_start_addr), hash_size);
    while (dataTransDone != TRUE);
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, sha512Sum, HASH_TIMEOUT_VAL);
    DBG_PR("Hash Done!!\n");
    memcpy((uint8_t *)(0xc0000100), &sha512Sum[0], 64);

}
void HASH_SHA512_for_sdram_Check(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint32_t i, addr;

    memset(config, 0, sizeof(HASH_Config_T));
    config->algo = HASH_ALGO_SHA512;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HASH;
    config->callback = HASH_Callback;
    HAL_HASH_Init(hHash, config);
    //    HAL_HASH_Start(hHash,(uint8_t *)(0xc0000400),1024*1024*7);
    HAL_HASH_Start(hHash, (uint8_t *)(hash_start_addr), hash_size);
    while (dataTransDone != TRUE);
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, sha512Sum, HASH_TIMEOUT_VAL);
    DBG_PR("Hash Done!!\n");
    if (memcmp((uint8_t *)(0xc0000100), &sha512Sum[0], 64) != 0)
    {
        DBG_PR("Hash Fail!!\n");
        while (1);
    }
    else
    {
        DBG_PR("Hash OK!!\n");
    }
}

static void clk_swith_pll_200M()
{
    CLOCK_SysClockConfig_T SysClockConfigStructure;

    //    clk_hirc_trim();

    SysClockConfigStructure.sysClkPrescaler =  CLOCK_DIVIDED_1;
    SysClockConfigStructure.sysClkSource    =  SYSCLOCK_SOURCE_PLL;

    HAL_CLOCK_SYSCLKConfig(&SysClockConfigStructure);
}


int main(void)
{
    RET_CODE ret = RET_OK;
    info_t info;
    uint32_t delay;
    //    SDRAM_HandleTypeDef hsdram;
    //    SysTick_Init();
    uint32_t addr_begin = 0xC0700000;
    uint32_t addr_end = 0xC0800000;
    uint32_t addr;
    uint8_t data = 0;
    static uint8_t sdram_write_flag = 0;
    clk_swith_pll_200M();
    DebugConsoleInit(19200);
    printf("Enter BOOT_1...\n");
    //    while(1);
    /// while(1)

    //    if (HAL_Reset_CheckRestCause(RESET_STATUS_SDP_PENDING) == TRUE && HAL_Reset_CheckRestCause(RESET_STATUS_STANDBY_MODE) == TRUE)
    if (HAL_Reset_CheckRestCause(RESET_STATUS_STANDBY_MODE) == TRUE)
    {
        uint32_t resume_addr;

        SCB->VTOR = 0x20001000 & SCB_VTOR_TBLOFF_Msk;
        printf("SDRAM Restore!!\n");
        dram_cfg_restore();
        if (PM_CFG->hash_enable_diable == HASH_ENALBE)
        {
            hash_start_addr = 0xc0000400;
            if (PM_CFG->sdram_size == sdram_8m)
            {
                hash_size = 1024 * 1024 * 8 - 1024;
            }
            else if (PM_CFG->sdram_size == sdram_16m)
            {
                hash_size = 1024 * 1024 * 16 - 1024;
            }
            else if (PM_CFG->sdram_size == sdram_32m)
            {
                hash_size = 1024 * 1024 * 32 - 1024;
            }
            else if (PM_CFG->sdram_size == sdram_64m)
            {
                hash_size = 1024 * 1024 * 64 - 1024;
            }
            HAL_IRQ_DISABLE(RTC_IRQn);
            __enable_irq();
            HASH_SHA512_for_sdram_Check(&hashHandle, &hash_config);
            SCB->VTOR = 0xc0010000 & SCB_VTOR_TBLOFF_Msk;
        }
        *(unsigned int *)0xE0042020 = 0x3;
        __DSB();
        __ISB();
        __resume_back_context();
    }
    else
    {
        if (HAL_Reset_CheckRestCause(RESET_STATUS_SHUT_DOWN_MODE) == TRUE)
        {
            HAL_Reset_ClearRestCause(RESET_STATUS_SHUT_DOWN_MODE);
#define RTC_BASE                 (0x40009000)
            SYS_DEV->CFGPM |= (1 << 12);
            writel(0x1ACCE551, RTC_BASE + 0x40);//unlock register
            IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 3 << 8, 3 << 8);
            //            IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 1 << 5 | 1 << 2, 1 << 5 | 1 << 2);
        }
        gm6721b_ddr_init();
#if 0

        printf("OK!!\n");
        for (addr = addr_begin; addr < addr_end; addr++)
        {
            *(uint8_t *)(addr) = data;
            data += 0x01;
        }
        data = 0;
        for (addr = addr_begin; addr < addr_end; addr++)
        {
            *(uint8_t *)(addr) = data;
            data += 0x01;
        }
        *(unsigned int *)0xE0042020 = 0x3;
        printf("system will Reset!!\n");

        __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                   buffered write are completed before reset */
        SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                                 (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                                 SCB_AIRCR_VECTRESET_Msk);             /* Keep priority group unchanged */
        __DSB();                                                          /* Ensure completion of memory access */

        for (;;)                                                          /* wait until reset */
        {
            __NOP();
        }
#endif


#if 1
        /* get info */
        memcpy(&info, (uint32_t *)0x10000000, sizeof(info_t));
        printk("system_load_addr=0x%x, system_addr=0x%x, system_size=0x%x\n", info.system_load_addr, info.system_addr, info.system_size);
#else
        info.system_load_addr = 0xC0010000;
        info.system_addr = 0x10010000;
        info.system_size = (200 * 1024);
#endif
        /* copy system to sdram */
        memcpy((uint32_t *)info.system_load_addr, (uint32_t *)info.system_addr, info.system_size);
        printk("load system to 0x%x done\n", info.system_load_addr);

#ifdef CONFIG_DUAL_EN
        /* copy slave to sdram */
        memcpy((uint32_t *)info.slave_load_addr, (uint32_t *)info.slave_addr, info.slave_size);
        printk("load slave to 0x%x done\n", info.slave_load_addr);

        /* copy slave bloader to sram */
        memcpy((uint32_t *)info.slavebl_load_addr, (uint32_t *)info.slavebl_addr, info.slavebl_size);
        printk("load slavebl to 0x%x done\n", info.slavebl_load_addr);
#endif
#if 0//mike
        printk("1\n");
        /*for commpier ,so set right address*/
        HAL_MPU_SetRegionsAddr(&mpu_config);
        printk("2\n");
        ret = HAL_MPU_MutiRegionConfig(&mpu_config);
        printk("3\n");
        if (ret)
        {
            BOOT_PR_DEBUG("ERR:%d HAL_MPU_MutiRegionConfig\n", ret);
            printk("1\n");
            return ret;
        }
        printk("4\n");
        //#else//justin
#define MPU_FULL_ACC_NON_CACHEABLE    (0x320 << 16)   /*!< Attribute registerin MPU   */
        // Base address = Base address :OR: Region number :OR: VALID bit
        MPU->RBAR = (((0x00000000 & 0xFFFFFFE0) | 0x1) | 0x10) ;
        // Attribute = Full access :OR: SRD = 0 :OR: Size = 4GB :OR: ENABLE
        MPU->RASR = ((MPU_FULL_ACC_NON_CACHEABLE) | (0x1f << 1) | 0x1);
        // Enable MPU
        MPU->CTRL = 0x1;
#endif



        /*you must to do */
        //get pc
        //get sp
        //goto pc
        //HAL_SetBusMapping(MAP_SDRM_REGION);
        *(unsigned int *)0xE0042020 = 0x3;
        *(unsigned int *)0x40000014 = 0x1ACC0000;
        SetPC(info.system_load_addr);
        //*(volatile uint32_t *)(0xE000ED0C) = 0x05FA0004;
        while (1);
    }
    //gm6721_sdram_init();


    return 0;
}