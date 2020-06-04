/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <gm_soc.h>
#include <gm_hal_otp.h>

volatile int count = 0;
#define __CPU_DEAD_LOOP__     do{count++;}while(1);  //if no count++, it will not stop at this line with 'Suspend' button pressed in GreeIDE
#define __CPU_HALT_DEBUG__    __asm ("bkpt 0")
#define GATE_ALL_IP_CLOCK     0xFFFFFFFF

#define HW32_REG *(volatile unsigned int *)
//#define SWD_TEST_CASE_700

static unsigned int index = 0;
typedef void (* FuncPtr)(void);

uint32_t CortexM0_Instr[2] =
{
    0x47700000,    /*valid instruction*/
    0x4770f123     /*invalid instruction*/
};

typedef struct
{
    uint32_t WTDLoad;
    uint32_t WTDValue;
    uint32_t WTDCtrl;
    uint32_t WTDIntClear;
    uint32_t WTDRis;
    uint32_t WTDMis;
} WTD_Device_T;

#define WTD_CTRL_BASE     0x4003E000
#define WTD               ((WTD_Device_T *) WTD_CTRL_BASE)
#define WTD_LOADVAL       120000000 //about 2.5s


static ALWAYS_INLINE void sysTickStop(void)
{
    uint32_t reg;

    /*
     * Disable the counter and its interrupt while preserving the
     * remaining bits.
     */
    reg = SysTick->CTRL;
    reg &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL = reg;
}

static ALWAYS_INLINE void sysTickStart(void)
{
    uint32_t reg;

    /*
     * Disable the counter and its interrupt while preserving the
     * remaining bits.
     */
    reg = SysTick->CTRL;
    reg |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL = reg;
}



void CPU_DeadLoop(void)
{
    printk("SWD_TEST_CASE_100: CPU Deadloop \n");
    __CPU_DEAD_LOOP__;
}

void CPU_DeadPrint(void)
{
    while (1)
    {
        printk("SWD_TEST_CASE_101: Do nothing but print messages 2 \n");
    }
}

void CPU_HitBreakpoint(void)
{
    printk("SWD_TEST_CASE_102: CPU Hits a Breakpoint \n");
    __CPU_HALT_DEBUG__;
}

void CPU_Wr2UnalignedAddr(void)
{
    printk("\n SWD Test Case 3: CPU Writes to an unaligned Address to Generate Hardfault\n");
    *(volatile uint32_t *)(0x40011001) = 0x12345678;
}

void CPU_RdfromUnalignedAddr(void)
{
    uint32_t rdval = 0;
    printk("\n SWD Test Case 4: CPU Reads from an unaligned Address to Generate Hardfault\n");
    rdval = *(volatile uint32_t *)(0x40011001);
}

void CPU_Wr2UndefinedSpace(void)
{
    printk("\n SWD Test Case 5: CPU Writes to an undefined Address to Generate Hardfault\n");
    *(volatile uint32_t *)(0xFFFFFFFF) = 0x12345678;
}

void CPU_RdfromUndefinedSpace(void)
{
    uint32_t rdval = 0;
    printk("\n SWD Test Case 6: CPU Reads from an undefined Address to Generate Hardfault\n");
    rdval = *(volatile uint32_t *)(0xFFFFFFFF);
}

void CPU_Wr2ReservedSpace(void)
{
    printk("\n SWD Test Case 5: CPU Writes to an undefined Address to Generate Hardfault\n");
    *(volatile uint32_t *)(0x18000000) = 0x12345678;
}

void CPU_RdfromReservedSpace(void)
{
    uint32_t rdval = 0;
    printk("\n SWD Test Case 6: CPU Reads from an undefined Address to Generate Hardfault\n");
    rdval = *(volatile uint32_t *)(0x40009C00);
}



void CPU_EnterSleepMode(void)
{
    printk("\n SWD Test Case 7: CPU Enters Sleep Mode\n");

    sysTickStop();
    SCB->SCR &= ~(1UL << 2);
    __WFI();

    /*wakeup by systick */
    printk("\n SWD Test Case 7: Exit from Sleep Mode by SysTick Interrupt\n");
}

void CPU_EnterDeepSleepMode(void)
{
    printk("\n SWD Test Case 8: CPU Enters Deep-Sleep Mode\n");

    sysTickStop();
    SCB->SCR |= (1UL << 2);
    __WFI();

    /*waken up by systick in systick.c at 1 ms interval*/
    printk("\n SWD Test Case 8: Exit from Sleep Mode by SysTick Interrupt\n");
}

void CPU_EnterSleepModeforEver(void)
{
    printk("\n SWD Test Case 15: CPU Enters Sleep Mode forever\n");

    sysTickStop();
    SCB->SCR &= ~(1UL << 2);
    __WFI();

    printk("\n SWD Test Case 15: should not run here\n");
}

void CPU_EnterDeepSleepModeforEver(void)
{
    printk("\n SWD Test Case 16: CPU Enters Deep Sleep Mode forever\n");

    sysTickStop();
    __DSB();
    __ISB();

    //here disable all IP's clock
    *(volatile uint32_t *)(0x40000080) &= ~(GATE_ALL_IP_CLOCK);
    *(volatile uint32_t *)(0x40000084) &= ~(GATE_ALL_IP_CLOCK);
    *(volatile uint32_t *)(0x40000088) &= ~(GATE_ALL_IP_CLOCK);

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
    __DSB();
    __ISB();

    printk("\n SWD Test Case 16: should not run here\n");
}

void CPU_Stop1Mode(void)
{
    printk("SWD_TEST_CASE_206: CPU Enters Stop1 Mode\n");
    sysTickStop();

    *(volatile unsigned int *)0x40000018 &= ((~(0x03 << 4)) | (0 << 4));
    *(volatile unsigned int *)0x40000018 |= 0x01;
    SCB->SCR |= (1UL << 2);
    __WFI();

    printk("SWD_TEST_CASE_206: should not run here\n");
}

void CPU_Stop2Mode(void)
{
    printk("SWD_TEST_CASE_207: CPU Enters Stop2 Mode\n");
    sysTickStop();

    *(volatile unsigned int *)0x40000018 &= ((~(0x03 << 4)) | (1 << 4));
    *(volatile unsigned int *)0x40000018 |= 0x01;
    SCB->SCR |= (1UL << 2);
    __WFI();
    printk("SWD_TEST_CASE_207: should not run here\n");
}

void CPU_StandByMode(void)
{
    printk("SWD_TEST_CASE_208: CPU Enters StandBy Mode\n");
    sysTickStop();

    *(volatile unsigned int *)0x40000018 &= ((~(0x03 << 4)) | (2 << 4));
    *(volatile unsigned int *)0x40000018 |= 0x01;
    SCB->SCR |= (1UL << 2);
    __WFI();
    printk("SWD_TEST_CASE_208: should not run here\n");
}

void CPU_ShutDownMode(void)
{
    printk("SWD_TEST_CASE_209: CPU Enters StutDown Mode\n");
    sysTickStop();

    *(volatile unsigned int *)0x40000018 &= ((~(0x03 << 4)) | (3 << 4));
    *(volatile unsigned int *)0x40000018 |= 0x01;
    SCB->SCR |= (1UL << 2);
    __WFI();
    printk("SWD_TEST_CASE_209: should not run here\n");
}



void CPU_WTDReboot(void)
{
    //printk("\n SWD Test Case 10: CPU Always Reboots due to Watchdog\n");
    printk("reboot due to watchdog.");

    /*First unlock watchdog register*/
    *(volatile uint32_t *)(WTD_CTRL_BASE + 0xC00) = 0x1ACCE551;

    WTD->WTDLoad = 1;
    WTD->WTDCtrl |= (1 << 31 | 1 << 0);  //enable reset

    while (1);
}

static int counter = 0;
static void WTD_Handler(void)
{
    //clear interrupt, this value wirte in watchdog SPEC.
    WTD->WTDIntClear = 0xadcddcba;

    counter++;
    printk("\nReload value for watchdog for %d time(s)\n", counter);
    *(volatile uint32_t *)(WTD_CTRL_BASE + 0xC00) = 0x1ACCE551;
}

//NOTICE: call WTD_Handler before reboot
void CPU_WTDTrigInterrupt()
{
    printk("\n SWD Test Case 14: CPU Always Interrupts due to Watchdog\n");
    HAL_RequestIrq(WDOG_IRQn, WDOG_IRQ_PRIORITY, WTD_Handler, 0);
    HAL_IRQ_ENABLE(WDOG_IRQn);

    *(volatile uint32_t *)(WTD_CTRL_BASE + 0xC00) = 0x1ACCE551;
    WTD->WTDLoad = WTD_LOADVAL;
    WTD->WTDCtrl |= (1 << 31 | 1 << 0);  //enable reset

    while (1);
}



void CPU_InvalidInstr(void)
{
    printk("\n SWD Test Case 13: CPU Executes Invalid Instruction\n");
    ((FuncPtr)(1 + (uint32_t)(&CortexM0_Instr[1])))();
}

void CPU_ErrorbyAddrMap(void)
{
    printk("\n SWD Test Case 18: Modify adress-mapping to cause CPU's unexpected execution\n");
    *(volatile uint32_t *)(0x40000014) = 0x1ACC0004;  //this will occurr hard fault
}

unsigned int address_test_write(unsigned int addr, unsigned int wdata)
{
    //"DSB" ;Ensure bus fault occurred before leaving this subroutine
    __asm__ __volatile__(
                    "STR R1,[R0];"
                    "DSB;"
                    "BX LR;"
    );
}

unsigned int address_test_read(unsigned int addr)
{
    __asm__ __volatile__(
                    "LDR R1,[R0];"
                    "DSB;"
                    "MOVS R0, R1;"
                    "BX LR;"
    );
}

static volatile int temp_data;

typedef struct res_addr
{
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t step;
} res_addr_t;

//all reserved address in GM6721
res_addr_t res_addr_arr[] =
{
    {0x18000000, 0x1FFEFFFF, 65536},
    {0x20060800, 0x21007FFF, 65536},
    {0x30000000, 0x3FFFFFFF, 65536},
    {0x40009C00, 0x40009FFF, 512},
    {0x4001B000, 0x4001FFFF, 512},
    {0x40021000, 0x40021FFF, 512},
    {0x40022000, 0x40023FFF, 512},
    {0x40028000, 0x4002FFFF, 512},
    {0x40035000, 0x40035FFF, 512},
    {0x4003A000, 0x4003AFFF, 512},
    {0x4003F000, 0x4003FFFF, 512},
    {0x40046000, 0x40046FFF, 512},
    {0x40047000, 0x40047FFF, 512},
    {0x40048000, 0x40048FFF, 512},
    {0x40049000, 0x40049FFF, 512},
    {0x4004A000, 0x4004AFFF, 512},
    {0x4004B000, 0x41FFFFFF, 65536},
    {0x44000000, 0x45FFFFFF, 65536},
    {0x46002000, 0x4FFFFFFF, 65536},
    {0x50040000, 0x5FFFFFFF, 65536},
    {0x60000000, 0xBFFFFFFF, 65536},
    {0xE0000000, 0xFFFFFFFF, 65536},
};

void CPU_Wr2AllReservedSpace(void)
{
    printk("SWD_TEST_CASE_402: Write to a Reserveed Space \n");

    if (index < (sizeof(res_addr_arr) / sizeof(res_addr_arr[0])))
    {
        res_addr_t res_addr = res_addr_arr[index];
        for (uint32_t addr = res_addr.start_addr; addr < res_addr.end_addr; addr += res_addr.step)
        {
            printk("SWD_TEST_CASE_402: addr = 0x%x\n", addr);
            temp_data = address_test_write(addr, 0x12345678);
        }
        index++;
    }
}

void CPU_RdAllReservedSpace(void)
{
    printk("SWD_TEST_CASE_403: Read from a Reserveed Space \n");

    if (index < (sizeof(res_addr_arr) / sizeof(res_addr_arr[0])))
    {
        res_addr_t res_addr = res_addr_arr[index];
        for (uint32_t addr = res_addr.start_addr; addr < res_addr.end_addr; addr += res_addr.step)
        {
            printk("SWD_TEST_CASE_403: addr = 0x%x\n", addr);
            //temp_data = address_test_read(addr);
            temp_data = *(volatile uint32_t *)(addr);
        }
        index++;
    }
}

void CPU_LockupAndReset(void)
{
    printk("CPU_LockupAndReset: Lockup and Reset \n");
    *(volatile uint32_t *)(0x40000014) |= (1 << 4);  //lockup function, 1: enable reset; 0: disable reset
    ((FuncPtr)(1 + (uint32_t)(&CortexM0_Instr[1])))();
}

void CPU_SysRequestReset(void)
{
#if defined(SWD_TEST_CASE_700)
    HW32_REG(0x40000070) &= ~(1 << 0);
    HW32_REG(0x40000070) |= (1 << 0);

#else
    printk("CPU_SysRequestReset: System Request Reset \n");

    // Use DMB to wait until all outstanding memory accesses are completed
    __DMB();

    // Read back PRIGROUP and merge with SYSRESETREQ
    SCB->AIRCR = (0x05FA0004 | (SCB->AIRCR & 0x700));  //SCB->AIRCR: 0xE000ED0C
    while (1); // Wait until reset happen
#endif
}

typedef enum ModeType
{
    OpenMode = 0,
    PasswordMode0,
    PasswordMode1,
    CloseMode,
    SecureMode
} ModeType_T;

RET_CODE My_OTP_SetDBGMode(OTP_Handle_T *pOTPHandle, ModeType_T mode)
{
    RET_CODE ret;

    if (mode == OpenMode)
    {
        //openmode: 0x1fff8034[2:0] = 3'B111
        //in fact, no need to write, default is openmode after otp reset
        ret = HAL_OTP_WriteData(pOTPHandle, SYS_CFG1MFC_OFFSET, 0xFFFFFFFF);
    }

    if (mode == PasswordMode0)
    {
        //password mode0: 0x1fff8034 [0] = 0
        ret = HAL_OTP_WriteData(pOTPHandle, SYS_CFG1MFC_OFFSET, 0xFFFFFFFE);
    }

    if (mode == PasswordMode1)
    {
        //password mode1: 0x1fff8034 [1] = 0
        ret = HAL_OTP_WriteData(pOTPHandle, SYS_CFG1MFC_OFFSET, 0xFFFFFFFD);
    }

    if (mode == CloseMode)
    {
        //close mode: 0x1fff8034 [2] = 0
        ret = HAL_OTP_WriteData(pOTPHandle, SYS_CFG1MFC_OFFSET, 0xFFFFFFFB);
    }

    if (mode == SecureMode)
    {
        //enable secure mode, 0x1fff8030[2:0] = 3’B100
        ret = HAL_OTP_WriteData(pOTPHandle, SYS_CFG0_OFFSET, 0xFFFFFFFC);
    }

    return ret;
}

#define  gpOTPDevice        ((OTP_Device_T *)OTP_DEVICE_BASE_ADDRESS)
#define  gpOTPMemIO         ((OTP_MEMIO_T *)OTP_MEMIO_BASE_ADDRESS)

void OTP_SwdModeTest(void)
{
    int                 i;
    RET_CODE            ret;
    OTP_ARRAY4_T        rarray4;
    OTP_ARRAY4_T        warray4;
    OTP_Handle_T        gOTPHandle;
    OTP_Config_T        gOTPConfig;

    ret = HAL_OTP_Init(&gOTPHandle, gpOTPDevice, gpOTPMemIO, &gOTPConfig);
    if (ret != RET_OK)
    {
        printk("hal OTP init failed!\n");
        return ;
    }

    ModeType_T mode = OpenMode;
    ret = My_OTP_SetDBGMode(&gOTPHandle, mode);
    if (ret != RET_OK)
    {
        printk("OTP set debug mode failed!\n");
        return ;
    }

    warray4.array[0] = 0x99999999;
    warray4.array[1] = 0x99999999;
    warray4.array[2] = 0x99999999;
    warray4.array[3] = 0x99999999;

    ret = HAL_OTP_SetDBGPW(&gOTPHandle, &warray4);
    if (ret != RET_OK)
    {
        printk("hal OTP set debug port password failed!\n");
        return ;
    }

    ret = HAL_OTP_GetDBGPW(&gOTPHandle, &rarray4);
    if (ret != RET_OK)
    {
        printk("hal OTP get debug port password failed!\n");
        return ;
    }

    for (i = 0; i < 4; i++)
    {
        if (rarray4.array[i] != warray4.array[i])
        {
            printk(" debug port password match failed!\n");
            printk("warray4.array[%d] = %d\n", i, warray4.array[i]);
            printk("rarray4.array[%d] = %d\n", i, rarray4.array[i]);
        }
    }

    printk("set debug port password ok!\n");
}

//not test in GM6721 AI chip
void OTP_SecureModeTest(void)
{
    /* NOTICE: this test case need to run with other elfs:
     * totally 3 elf: 2 rabbit elf and 1 this ugelis elf
     *
     * this elf need to run in sram
     *
     * 2 rabbit elf is:
     * ugelis/Source/rabbit/example/bootloader_secure_verify and bootrom_secure_verify
     *
     */

    RET_CODE            ret;
    OTP_Handle_T        gOTPHandle;
    OTP_Config_T        gOTPConfig;

    ret = HAL_OTP_Init(&gOTPHandle, gpOTPDevice, gpOTPMemIO, &gOTPConfig);
    if (ret != RET_OK)
    {
        printk("hal OTP init failed!\n");
        return ;
    }

    //Notice: otp dword(32bit) just can write 1 time!!!
    //so, for 0x1fff8030, bit2:0, bit8 and bit16 need to write at one time!!!
    uint32_t val = HAL_OTP_GetSysCfg0(&gOTPHandle);
    ret = HAL_OTP_WriteData(&gOTPHandle, SYS_CFG0_OFFSET, ((val & 0xFFFFFFF8) | 0x4)); //0x30[2:0]=100

    //terrylee@20190307, 0x30[8] is for MCU chip, now we can abandont it becoz GM6721 is AI chip
    //ret = HAL_OTP_WriteData(&gOTPHandle, SYS_CFG0_OFFSET, ((val & 0xFFFEFEF8) | 0x4)); //0x30[2:0]=100, 0x30[16]=0, 0x30[8]=0
    //ret = HAL_OTP_WriteData(&gOTPHandle, SYS_CFG0_OFFSET, (((val & 0xFFFEFEF8) | 0x4) | 0x100)); //0x30[2:0]=100, 0x30[16]=0, 0x30[8]=1

    val = HAL_OTP_GetSysCfg0(&gOTPHandle);
    printk("after write 0x30: %x\n", val);


    val = HAL_OTP_GetSysCfg1MFC(&gOTPHandle);
    ret = HAL_OTP_WriteData(&gOTPHandle, SYS_CFG1MFC_OFFSET, (val & 0xFFFFFFFC)); //0x34[0,1]=00
    //ret = HAL_OTP_WriteData(&gOTPHandle, SYS_CFG1MFC_OFFSET, (val | 0x3)); //0x34[0,1]=11

    val = HAL_OTP_GetSysCfg1MFC(&gOTPHandle);
    printk("after write 0x34: %x\n", val);
}


void OTP_OnSlaveCpuTest(void)
{
    uint32_t i;

    OTP_SwdModeTest();
    HW32_REG(0x2001c000 + 0x0) = 0x2001c0f0;
    HW32_REG(0x2001c000 + 0x4) = 0x00000009;
    HW32_REG(0x2001c000 + 0x8) = 0xe7fee7fe;   //while(1);
    HW32_REG(0x2001c000 + 0x0c) = 0xe7fee7fe;  //while(1);
    HW32_REG(0x2001c000 + 0x10) = 0xe7fee7fe;  //while(1);
    HW32_REG(0x2001c000 + 0x14) = 0xe7fee7fe;  //while(1);
    HW32_REG(0x2001c000 + 0x18) = 0xe7fee7fe;  //while(1);
    HW32_REG(0x2001c000 + 0x1c) = 0xe7fee7fe;  //while(1);
    HW32_REG(0x2001c000 + 0x20) = 0xe7fee7fe;  //while(1);

    //enable the slave cm4 clock
    HW32_REG(0x40000080) |= (1 << 0);

    //release the slave cm4 reset
    HW32_REG(0x40000070) |= (1 << 0);
}

void SWD_CryptoTest(void)
{
    /*
     * just use jmem to test this.
     * do not use this following code, it is dangerous.
     * becoz when finish burning this code, it will run at soon.
     * so key0-3 was changed, and then we will disconnect with cpu, so download failed.
     */
    int tag = (*(volatile uint32_t *)0xE0042044) & 0x20;  //check bit5
    if (tag == 0x20)
    {
        printk("\n Password can input. \n");

        *(volatile uint32_t *)(0xE0042030) = 1;
        *(volatile uint32_t *)(0xE0042034) = 2;
        *(volatile uint32_t *)(0xE0042038) = 3;
        *(volatile uint32_t *)(0xE004203c) = 4;

        printk("\n Password input complete. \n");

        int complete = *(volatile uint32_t *)(0xE0042044) & 0x2;
        if (complete == 0x2)
        {
            printk("\n Password input complete confirmed! \n");

            //test case: this should not write successfully.
            *(volatile uint32_t *)(0xE0042038) = 8;
        }
        else
        {
            printk("\n Password input complete failed! \n");
        }

        int key_match = (*(volatile uint32_t *)0xE0042044) & 0x1;
        if (key_match == 0x1)
        {
            printk("\n Password Correct! \n");
        }
        else
        {
            printk("\n Password Wrong! \n");
        }

        printk("\n Finished Test. \n");
    }
}

//#define SWD_TEST_CASE_100
//#define SWD_TEST_CASE_101
//#define SWD_TEST_CASE_102
//#define SWD_TEST_CASE_103
//#define SWD_TEST_CASE_104
//#define SWD_TEST_CASE_105
//#define SWD_TEST_CASE_106
//#define SWD_TEST_CASE_107
//#define SWD_TEST_CASE_108
//
//#define SWD_TEST_CASE_200
//#define SWD_TEST_CASE_201
//#define SWD_TEST_CASE_202
//#define SWD_TEST_CASE_203
//#define SWD_TEST_CASE_206
//#define SWD_TEST_CASE_207
//#define SWD_TEST_CASE_208
//#define SWD_TEST_CASE_209
//
//#define SWD_TEST_CASE_300
//#define SWD_TEST_CASE_301
//
//#define SWD_TEST_CASE_400
//#define SWD_TEST_CASE_401
//#define SWD_TEST_CASE_402
//#define SWD_TEST_CASE_403
//#define SWD_TEST_CASE_405
//#define SWD_TEST_CASE_406
//
//#define SWD_TEST_CASE_500
//
//#define SWD_TEST_CASE_600
#define SWD_TEST_CASE_601

void testcode()
{
#if defined(SWD_TEST_CASE_402)
    printk("testcode SWD_TEST_CASE_402 index=%d\n", index);
    //will return back and continue execute SWD_TEST_CASE_402
#endif

#if defined(SWD_TEST_CASE_403)
    printk("testcode SWD_TEST_CASE_403 index=%d\n", index);
#endif

#if defined(SWD_TEST_CASE_405)
    printk("testcode SWD_TEST_CASE_405\n");
    ((FuncPtr)(1 + (uint32_t)(&CortexM0_Instr[1])))();
#endif
}




int main(void)
{
    printk("===SWD/JTAG fpga test start===\n");

#ifdef SWD_TEST_CASE_700
    OTP_OnSlaveCpuTest();
#endif

#ifdef SWD_TEST_CASE_100
    CPU_DeadLoop();
#endif

#ifdef SWD_TEST_CASE_101
    CPU_DeadPrint();
#endif

#ifdef SWD_TEST_CASE_102
    CPU_HitBreakpoint();
#endif

#ifdef SWD_TEST_CASE_103
    CPU_Wr2UnalignedAddr();
#endif

#ifdef SWD_TEST_CASE_104
    CPU_RdfromUnalignedAddr();
#endif

#ifdef SWD_TEST_CASE_105
    CPU_Wr2UndefinedSpace();
#endif

#ifdef SWD_TEST_CASE_106
    CPU_RdfromUndefinedSpace();
#endif

#ifdef SWD_TEST_CASE_107
    CPU_Wr2ReservedSpace();
#endif

#ifdef SWD_TEST_CASE_108
    CPU_RdfromReservedSpace();
#endif

#ifdef SWD_TEST_CASE_200
    CPU_EnterSleepMode();
#endif

#ifdef SWD_TEST_CASE_201
    CPU_EnterDeepSleepMode();
#endif

#ifdef SWD_TEST_CASE_202
    CPU_EnterSleepModeforEver();
#endif

#ifdef SWD_TEST_CASE_203
    CPU_EnterDeepSleepModeforEver();
#endif

#ifdef SWD_TEST_CASE_206
    CPU_Stop1Mode();
#endif

#ifdef SWD_TEST_CASE_207
    CPU_Stop2Mode();
#endif

#ifdef SWD_TEST_CASE_208
    CPU_StandByMode();
#endif

#ifdef SWD_TEST_CASE_209
    CPU_ShutDownMode();
#endif

#ifdef SWD_TEST_CASE_300
    CPU_WTDReboot();
#endif

#ifdef SWD_TEST_CASE_301
    CPU_WTDTrigInterrupt();
#endif

#ifdef SWD_TEST_CASE_400
    CPU_InvalidInstr();
#endif

#ifdef SWD_TEST_CASE_401
    CPU_ErrorbyAddrMap();
#endif

#ifdef SWD_TEST_CASE_402
    CPU_Wr2AllReservedSpace();
#endif

#ifdef SWD_TEST_CASE_403
    CPU_RdAllReservedSpace();
#endif

#ifdef SWD_TEST_CASE_405
    CPU_LockupAndReset();
#endif

#ifdef SWD_TEST_CASE_406
    CPU_SysRequestReset();
#endif, 0x30[16]=0, 0x30[8]=0

#ifdef SWD_TEST_CASE_500
    SWD_CryptoTest();
#endif

#ifdef SWD_TEST_CASE_600
    //dram_init();
    OTP_SwdModeTest();
#endif

#ifdef SWD_TEST_CASE_601
    OTP_SecureModeTest();
#endif

    __CPU_HALT_DEBUG__;
    __CPU_DEAD_LOOP__;

    return 0;
}


