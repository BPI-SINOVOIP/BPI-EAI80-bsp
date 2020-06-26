#include <stdio.h>
#include <gm_hal_uart.h>
#include <gm_common.h>
#include <pinmux.h>
#include "pm_system.h"
#include "F6721B_ddr_init.h"
#include <gm_hal_hash.h>
#include <gm_hal_clock.h>

#ifdef RABBIT_NO_OS_SYSTEM
    #include <systick.h>
#endif
#define RTC_BASE                 (0x40009000)

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

#define readl(addr)     (*((volatile unsigned long  *)(addr)))
#define writel(v, addr) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))
#define SYSTEM_BASE              (0x40000000)
#define PWR_MOD_REG              (SYSTEM_BASE + 0x18 )
#define RST_STATUS_REG           (SYSTEM_BASE + 0x04 )



#define DMA_MODE_TEST   0


#define HASH_TIMEOUT_VAL        5      //ms


#define DBG_PR_EN              1
#ifdef DBG_PR_EN
    #define DBG_PR       printf
#else
    #define DBG_PR(...)  do{}while(0)
#endif

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))



uint8_t sha512Sum[64];
static HASH_Handle_T hashHandle;
static HASH_Config_T hash_config;
static volatile uint8_t dataTransDone = FALSE;
static volatile uint8_t dataTransErr = FALSE;
static volatile uint8_t dataAvailable = FALSE;

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


#define ARM_V7M_NVIC_E000E100_ARM_NUM_IRQ_PRIORITY_BITS     3
#define CONFIG_NUM_IRQ_PRIO_BITS            ARM_V7M_NVIC_E000E100_ARM_NUM_IRQ_PRIORITY_BITS
#define _EXC_PRIO(pri) (((pri) << (8 - CONFIG_NUM_IRQ_PRIO_BITS)) & 0xff)
#define _EXCEPTION_RESERVED_PRIO 1
#define _IRQ_PRIO_OFFSET (_EXCEPTION_RESERVED_PRIO)
#define _EXC_IRQ_DEFAULT_PRIO _EXC_PRIO(_IRQ_PRIO_OFFSET)
#define irq_lock() _arch_irq_lock()
#define irq_unlock(key) _arch_irq_unlock(key)

unsigned int _arch_irq_lock(void)
{
    unsigned int key;
    unsigned int tmp = _EXC_IRQ_DEFAULT_PRIO;

    __asm volatile
    {
        MRS key, BASEPRI
        MSR BASEPRI, tmp
        DSB
        ISB
    }

    return key;
}

void _arch_irq_unlock(unsigned int key)
{
    __asm volatile
    {
        MSR BASEPRI, key
        DSB
        ISB
    }
}


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
    //    HAL_HASH_Start(hHash,(uint8_t *)(0xc0000400),1024*1024*7);1024*1024*hash_size-1024
    HAL_HASH_Start(hHash, (uint8_t *)(hash_start_addr), hash_size);
    while (dataTransDone != TRUE)
    {
        //      DBG_PR("wait transfer done!!\n");
    }
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



void pm_standby_wkpin_config(struct pm_frame_wkup_pin_cfg *wkup_pin_cfg)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    uint32_t data;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0; /*GPIOA0*/
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    //    if (wkup_pin_cfg->wkup_pin_valid == PM_RISING_EDGE)
    //    {
    //        GPIO_InitStruct.pull = GPIO_PULLDOWN;
    //    }
    //    else
    //    {
    //        GPIO_InitStruct.pull = GPIO_PULLUP;
    //    }
    GPIO_InitStruct.pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    data = readl(PWR_MOD_REG);
    data = data | (0x01 << 19);
    data = data | (wkup_pin_cfg->wkup_pin_valid << 18);
    writel(data, PWR_MOD_REG);
    writel(0x01 << 26, RST_STATUS_REG);
}
void pm_shutdown_wkpin_xpb12_config(struct pm_frame_wkup_pin_cfg *wkup_pin_cfg)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_CLOCK_ClockCmd(CLOCK_LIRC_MASK, ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE));
    SYS_DEV->CFGPM |= (1 << 12);
    writel(0x1ACCE551, RTC_BASE + 0x40);//unlock register
    while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
    IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 0x3ff, ((1 << 8) | ((wkup_pin_cfg->wkup_pin_valid) << 1) | (0x01)));
    while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
    IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 3 << 10, 3 << 10);
    while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
}

void pm_shutdown_wkpin_xpb13_config(struct pm_frame_wkup_pin_cfg *wkup_pin_cfg)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_13;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_CLOCK_ClockCmd(CLOCK_LIRC_MASK, ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE));
    SYS_DEV->CFGPM |= (1 << 12);
    writel(0x1ACCE551, RTC_BASE + 0x40);//unlock register
    while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
    IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 0x3ff, ((1 << 9) | ((wkup_pin_cfg->wkup_pin_valid) << 4) | (0x01 << 3)));
    while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
    IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 3 << 10, 1 << 10);
    while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
}
int main(void)
{
    //    HASH_SHA512_for_sdram_Init(&hashHandle, &config);
    //    DebugConsoleInit(19200);
    uint32_t delay;
    int32_t key;

    if (PM_CFG->lp_mode == STANDBY_MODE)
    {
        if (PM_CFG->hash_enable_diable == HASH_ENALBE)
        {
            SCB->VTOR = 0x20001000 & SCB_VTOR_TBLOFF_Msk;
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
            DBG_PR("Code run on sram!!\n");
            //key = irq_lock();
            irq_unlock(0);
            __DSB();
            __ISB();
            HASH_SHA512_for_sdram_Init(&hashHandle, &hash_config);
            key = irq_lock();
        }
        if (PM_CFG->wkup_pin_xpa0.wkup_pin_enable_disable == WKUP_PIN_ENALBE)
        {
            pm_standby_wkpin_config(&PM_CFG->wkup_pin_xpa0);
        }
        DBG_PR("SDRAM Enter SelfRefresh!!\n");
        Enter_SDRAM_SelfRefresh();
        DBG_PR("Enter Standby Mode!!\n");
        goto_standby_mode();
    }
    else if (PM_CFG->lp_mode == SHUTDOWN_MODE)
    {
        if (PM_CFG->wkup_pin_xpb12.wkup_pin_enable_disable == WKUP_PIN_ENALBE)
        {
            pm_shutdown_wkpin_xpb12_config(&PM_CFG->wkup_pin_xpb12);
        }
        if (PM_CFG->wkup_pin_xpb13.wkup_pin_enable_disable == WKUP_PIN_ENALBE)
        {
            pm_shutdown_wkpin_xpb13_config(&PM_CFG->wkup_pin_xpb13);
        }
        while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
        IO_BITMASK_SET(*(volatile unsigned long *)(RTC_BASE + 0x58), 1 << 12, (PM_CFG->pm_shutdown_status_pin_inv) << 12);
        while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
        writel(0x12345678, RTC_BASE + 0x40);//lock register
        while (!(IO_BIT_GET(HW32_REG(RTC_BASE + 0x08), 0x1 << 22)));
        DBG_PR("Enter Shutdown Mode!!\n");
        //    delay = 16000;
        //    while(delay--);
        goto_shut_down_mode();
    }
    DBG_PR("Fail!!System  doesn't enter lower power mode!!\n,");
    while (1);
#if 0
#ifdef SDRAM_HASH_TEST
    SCB->VTOR = 0x20001000 & SCB_VTOR_TBLOFF_Msk;

    DBG_PR("Code run on sram!!\n");
    //key = irq_lock();
    irq_unlock(0);
    __DSB();
    __ISB();
    HASH_SHA512_for_sdram_Init(&hashHandle, &hash_config);
    key = irq_lock();
#endif
    DBG_PR("SDRAM Enter SelfRefresh!!\n");
    Enter_SDRAM_SelfRefresh();
#ifdef ENTER_STOP1_MODE

    __disable_irq();
    DBG_PR("Enter Stop1 Mode!!\n");
    goto_stop1_mode();
    //    DebugConsoleInit(19200);
    DBG_PR("Out of stop1 mode!!\n");
    //    SCB->VTOR = 0x20001000 & SCB_VTOR_TBLOFF_Msk;
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
    dram_cfg_restore();
    DBG_PR("SDRAM Out of SelfRefresh!!\n");
#ifdef SDRAM_HASH_TEST
    HAL_IRQ_DISABLE(RTC_IRQn);
    __enable_irq();
    HASH_SHA512_for_sdram_Check(&hashHandle, &hash_config);
    SCB->VTOR = 0xc0010000 & SCB_VTOR_TBLOFF_Msk;
    __disable_irq();
    HAL_IRQ_ENABLE(RTC_IRQn);
    //    HASH_SHA512_for_sdram_Check(&hashHandle, &config);
#endif
    __resume_cpu_contex();
#endif

#ifdef ENTER_STOP2_MODE
    __disable_irq();
    DBG_PR("Enter Stop2 Mode!!\n");
    goto_stop2_mode();
    //    DebugConsoleInit(19200);
    DBG_PR("Out of stop2 mode!!\n");
    DBG_PR("SDRAM Out of SelfRefresh!!\n");
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
    dram_cfg_restore();
#ifdef SDRAM_HASH_TEST
    HAL_IRQ_DISABLE(RTC_IRQn);
    __enable_irq();
    HASH_SHA512_for_sdram_Check(&hashHandle, &hash_config);
    SCB->VTOR = 0xc0010000 & SCB_VTOR_TBLOFF_Msk;
    __disable_irq();
    HAL_IRQ_ENABLE(RTC_IRQn);
#endif
    __resume_cpu_contex();
#endif

#ifdef ENTER_STANDBY_MODE
    pm_standby_wkpin_config();
    DBG_PR("Enter Standby Mode!!\n");
    goto_standby_mode();
    //    DebugConsoleInit(19200);
    //    DBG_PR("Out of Standby mode!!\n");
    //    DBG_PR("SDRAM Out of SelfRefresh!!\n");
    //    dram_cfg_restore();
    //    __resume_cpu_contex();
#endif

#ifdef ENTER_SHUTDOWN_MODE
    pm_shutdown_wkpin_config();
    DBG_PR("Enter Shutdown Mode!!\n");
    //    delay = 16000;
    //    while(delay--);
    goto_shut_down_mode();
    //    DebugConsoleInit(19200);
    //    DBG_PR("Out of Standby mode!!\n");
    //    DBG_PR("SDRAM Out of SelfRefresh!!\n");
    //    dram_cfg_restore();
    //    __resume_cpu_contex();
#endif
#endif

}

