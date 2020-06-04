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



//#define ENTER_STOP1_MODE            1
//#define ENTER_STOP2_MODE            1
#define ENTER_STANDBY_MODE          1
//#define ENTER_SHUTDOWN_MODE         1

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

typedef enum
{
    WKPIN_XPA0           = 0,
    WKPIN_XPB12           = 1,
    WKPIN_XPB13           = 2,
} WAKE_PIN_TYPE_T;

#ifdef SDRAM_HASH_TEST
    uint8_t sha512Sum[64];
    static HASH_Handle_T hashHandle;
    static HASH_Config_T hash_config;
    static volatile uint8_t dataTransDone = FALSE;
    static volatile uint8_t dataTransErr = FALSE;
    static volatile uint8_t dataAvailable = FALSE;
#endif

#ifndef  ENTER_STANDBY_MODE
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

#ifdef SDRAM_HASH_TEST

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
    //    HAL_HASH_Start(hHash,(uint8_t *)(0xc0000400),1024*1024*7);
    HAL_HASH_Start(hHash, (uint8_t *)(HASH_ADDR_START), HASH_SIZE);
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
    HAL_HASH_Start(hHash, (uint8_t *)(HASH_ADDR_START), HASH_SIZE);
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
#endif



void pm_standby_wkpin_config(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    uint32_t data;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0; /*GPIOA0*/
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    data = readl(PWR_MOD_REG);
    data = data | (0x01 << 19);
    writel(data, PWR_MOD_REG);
    writel(0x01 << 26, RST_STATUS_REG);
}
void pm_shutdown_wkpin_config(void)
{

}
int main(void)
{
    //    HASH_SHA512_for_sdram_Init(&hashHandle, &config);
    //    DebugConsoleInit(19200);
    uint32_t delay;
    int32_t key;
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

}

