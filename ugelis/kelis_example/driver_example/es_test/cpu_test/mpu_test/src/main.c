/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <misc/printk.h>
#include <linker/linker-defs.h>
#include <gm_soc.h>
#include <gm_hal_cpu.h>

/*code run sram ,data run sram*/


#define MPU_DEBUG_PRINTF           1

#ifndef MPU_DEBUG_PRINTF
    #define MPU_PR_DEBUG(...)      do {} while (0)
#else
    #define MPU_PR_DEBUG           printf
#endif



/*region 0 flash*/
/* Privileged Read Only, Unprivileged Read Only */

//#define TEST_PRI_R_0         1
//#define TEST_USR_R_0         1
//#define TEST_USR_W_0         1
//#define TEST_PRI_W_0         1





/*region 1 sram0*/
/* Privileged Read Write, Unprivileged No Access */

//#define TEST_PRI_RW_1       1
//#define TEST_USR_W_1        1
//#define TEST_USR_R_1        1




/*region 2 sram1*/
/* Privileged Read Write, Unprivileged Read Only */
//#define TEST_PRI_RW_2       1
//#define TEST_USR_W_2        1
//#define TEST_USR_R_2        1



/*region 3 sram2*/
/* Privileged Read Write, Unprivileged Read Write */
//#define TEST_PRI_RW_3       1
//#define TEST_USR_RW_3       1

/*region 4 sdram0 */
/* Privileged Read Only, Unprivileg ed Read Only */
//#define TEST_PRI_R_4       1
//#define TEST_USR_R_4       1
//#define TEST_PRI_W_4       1
//#define TEST_USR_W_4       1



/*region 5 sdram1 */
/* Privileged Read Only, Unprivileged No Access */
//#define TEST_PRI_W_5       1
//#define TEST_PRI_R_5       1
//#define TEST_USR_W_5       1
//#define TEST_USR_R_5       1


/*region 6 sdram2 */
/* Privileged Read Only, Unprivileged Read Only */
//#define TEST_PRI_W_6       1
//#define TEST_PRI_R_6       1
//#define TEST_USR_W_6       1
//#define TEST_USR_R_6         1



/*region 7 sdram3 */
/* Privileged Read Only, Unprivileged Read Only */
//#define TEST_PRI_W_7       1
#define TEST_PRI_R_7       1
//#define TEST_USR_W_7       1
//#define TEST_USR_R_7       1






//#define DEBUG_DELAY 1
void test_delay(uint32_t us)
{
#ifdef DEBUG_DELAY
    k_busy_wait(1000);
#endif
}



/*****************************************flash regio - 0*********************************************************/

#define ADDR_TEMP               0

#define FLASH_REGIO_SIZE         (1024*64)
#define FLASH_MPU_ADDR          ((uint32_t)(0x10000000)) //ALINE 64K
#define REGIO_FLASH_ATTR        (MPU_DATA_P_RO_U_RO |MPU_REGION_64K)/* for debgu rw ok */



/*****************************************sram regio 1-3 *********************************************************/

#define SRAM_OFFSET             (0)
#define SARM2_MPU_ADDR          (0x20000000+SRAM_OFFSET)
#define SRAM2_REGIO_SIZE        (1024*64)
#define SRAM2_REGIO_ATTR        (MPU_DATA_P_RW_U_RW|MPU_REGION_64K) /* Privileged Read Write, Unprivileged Read Write */
/*code run sram ,data run sram*/


#define SARM0_MPU_ADDR          (SARM2_MPU_ADDR+SRAM2_REGIO_SIZE)
#define SRAM0_REGIO_SIZE        (1024*8)
#define SRAM0_REGIO_ATTR        (MPU_DATA_P_RW_U_NA|MPU_REGION_8K)  /* Privileged Read Write, Unprivileged No Access */


#define SARM1_MPU_ADDR          (SARM0_MPU_ADDR+SRAM0_REGIO_SIZE)
#define SRAM1_REGIO_SIZE        (1024*8)
#define SRAM1_REGIO_ATTR        (MPU_DATA_P_RW_U_RO|MPU_REGION_8K) /* Privileged Read Write, Unprivileged Read Only */


/*****************************************sdrm regio 4-6 *********************************************************/

#define SDRAM_OFFSET            (1024*1024*4)
#define SDARM0_MPU_ADDR         (0xc0000000+SDRAM_OFFSET)
#define SDRAM0_REGIO_SIZE       (1024*1024*2)
#define SDRAM0_REGIO_ATTR       (MPU_DATA_RO|MPU_REGION_2M)/* Privileged Read Only, Unprivileged Read Only */

#define SDARM1_MPU_ADDR         (SDARM0_MPU_ADDR+SDRAM0_REGIO_SIZE)
#define SDRAM1_REGIO_SIZE       (1024*1024*2)
#define SDRAM1_REGIO_ATTR       (MPU_DATA_P_RO_U_NA|MPU_REGION_2M) /* Privileged Read Only, Unprivileged No Access */


#define SDARM2_MPU_ADDR         (SDARM1_MPU_ADDR + SDRAM1_REGIO_SIZE)
#define SDRAM2_REGIO_SIZE       (1024*1024*2)
#define SDRAM2_REGIO_ATTR       (MPU_DATA_P_RO_U_RO|MPU_REGION_2M) /* Privileged Read Only, Unprivileged Read Only */


#define SDARM3_MPU_ADDR        (SDARM2_MPU_ADDR+SDRAM2_REGIO_SIZE)
#define SDRAM3_REGIO_SIZE      (1024*16)
#define SDRAM3_REGIO_ATTR      (MPU_DATA_P_RW_U_RW|MPU_REGION_16K) /* Privileged Read Only, Unprivileged Read Only */



/*****************************************SRAM Data regio 7 *********************************************************/





static  MPU_REGION_T mpu_regions[] =
{

    /* Region 0 */
    /* Privileged No Access, Unprivileged No Access */
    MPU_REGION_ENTRY("FLASH",
                     ADDR_TEMP,
                     REGIO_FLASH_ATTR),

    /* Region 1 */
    /* Privileged Read Write, Unprivileged No Access */
    MPU_REGION_ENTRY("RAM0",
                     ADDR_TEMP,
                     SRAM0_REGIO_ATTR),


    /* Region 2 */
    /* Privileged Read Write, Unprivileged Read Only */
    MPU_REGION_ENTRY("RAM1",
                     ADDR_TEMP,
                     SRAM1_REGIO_ATTR),


    /* Region 3 */
    /* Privileged Read Write, Unprivileged Read Write */
    MPU_REGION_ENTRY("RAM2",
                     ADDR_TEMP,
                     SRAM2_REGIO_ATTR),

    /* Region 4 */ /* Privileged Read Only, Unprivileged Read Only */
    MPU_REGION_ENTRY("SDRAM0",

                     ADDR_TEMP,
                     SDRAM0_REGIO_ATTR),

    /* Region 5 */
    /* Privileged Read Only, Unprivileged No Access */
    MPU_REGION_ENTRY("SDRAM1",
                     ADDR_TEMP,
                     SDRAM1_REGIO_ATTR),

    /* Region 6 */
    /* Privileged Read Only, Unprivileged Read Only */
    MPU_REGION_ENTRY("SDRAM2",
                     ADDR_TEMP,
                     SDRAM2_REGIO_ATTR),

    /* Region 7 */
    /* Privileged Read Write, Unprivileged Read Write */
    MPU_REGION_ENTRY("SDRAM3",
                     ADDR_TEMP,
                     SDRAM3_REGIO_ATTR),
};

MPU_CONFIG_T mpu_config =
{
    .numRegions = ARRAY_SIZE(mpu_regions),
    .mpuRegions = mpu_regions,
};



/*becare full ;mustbe call at thead pridev mode*/
static ALWAYS_INLINE void SwitchToUserMode()
{
    uint32_t val;
    __asm__ volatile("mrs %0, CONTROL\n\t" : "=r"(val));
    /*keep priver use psp*/
    val |= 1;
    /*switch to user mode and  use psp*/
    __asm__ volatile("msr CONTROL, %0\n\t" :  : "r"(val));
}




#ifdef TEST_PRI_R_0
/* Privileged Read Only, Unprivileged Read Only */


void mpu_test()
{
    uint32_t addr = FLASH_MPU_ADDR + (FLASH_REGIO_SIZE >> 1);
    MPU_PR_DEBUG("test Privileged read  access\n");
    IO_READL(addr);
    test_delay(1000);
    MPU_PR_DEBUG("Pass:test Privileged read  access\n");

}
#endif


#ifdef TEST_USR_R_0
/* Privileged Read Only, Unprivileged Read Only */


void mpu_test()
{
    uint32_t addr = FLASH_MPU_ADDR + (FLASH_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test user read access\n");

    SwitchToUserMode();

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("Pass:test user read access\n");
}
#endif



#ifdef TEST_PRI_W_0
/* Privileged Read Only, Unprivileged Read Only */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = FLASH_MPU_ADDR + (FLASH_REGIO_SIZE >> 1);
    MPU_PR_DEBUG("test Privileged write not access:except enter MPU FAULT\n");
    IO_WRITEL(val, addr);
    test_delay(1000);
    MPU_PR_DEBUG("Err :Not except\n");
}
#endif


#ifdef TEST_USR_W_0
/* Privileged Read Only, Unprivileged Read Only */
void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = FLASH_MPU_ADDR + (FLASH_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test usr write not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Err :Not except\n");
}
#endif





#ifdef TEST_PRI_RW_1
/* Privileged Read Write, Unprivileged No Access */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SARM0_MPU_ADDR + (SRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged Read Write access\n");

    IO_WRITEL(val, addr);
    test_delay(1000);
    if (val != IO_READL(addr))
    {
        MPU_PR_DEBUG("Err:Privileged read access ok ,but read result is fail\n");
    }
    test_delay(1000);

    MPU_PR_DEBUG("PASS: Privileged Read Write\n");
}
#endif


#ifdef TEST_USR_W_1
/* Privileged Read Write, Unprivileged No Access */

void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SARM0_MPU_ADDR + (SRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR write not access:except enter MPU FAULT\n");

    SwitchToUserMode();

    test_delay(1000);

    MPU_PR_DEBUG("Fail: test usr writr unaccess\n");
}
#endif


#ifdef TEST_USR_R_1
/* Privileged Read Write, Unprivileged No Access */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SARM0_MPU_ADDR + (SRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR Read not access:except enter MPU FAULT\n");
    SwitchToUserMode();
    test_delay(1000);
    IO_WRITEL(val, addr);
    MPU_PR_DEBUG("Fail: test usr read unaccess\n");
}
#endif


#ifdef TEST_PRI_RW_2
/* Privileged Read Write, Unprivileged Read Only */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SARM1_MPU_ADDR + (SRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged Read Write access\n");

    IO_WRITEL(val, addr);
    test_delay(1000);
    if (val != IO_READL(addr))
    {
        MPU_PR_DEBUG("Err:Privileged read access ok ,but read result is fail\n");
    }
    test_delay(1000);

    MPU_PR_DEBUG("PASS: Privileged Read Write\n");
}

#endif

#ifdef  TEST_USR_W_2
/* Privileged Read Write, Unprivileged Read Only */
void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SARM1_MPU_ADDR + (SRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR write not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    test_delay(1000);

    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test usr writr unaccess\n");
}

#endif


#ifdef TEST_USR_R_2
/* Privileged Read Write, Unprivileged Read Only */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SARM1_MPU_ADDR + (SRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR Read not access\n");

    SwitchToUserMode();
    test_delay(1000);

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("PASS: test usr read access\n");
}

#endif



#ifdef TEST_PRI_RW_3
/* Privileged Read Write, Unprivileged Read Write */
void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SARM2_MPU_ADDR + (SRAM2_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged Read Write access\n");

    IO_WRITEL(val, addr);
    test_delay(1000);
    if (val != IO_READL(addr))
    {
        MPU_PR_DEBUG("Err:Privileged read access ok ,but read result is fail\n");
    }
    test_delay(1000);

    MPU_PR_DEBUG("PASS: Privileged Read Write\n");
}

#endif


#ifdef TEST_USR_RW_3
/* Privileged Read Write, Unprivileged Read Write */
void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SARM2_MPU_ADDR + (SRAM2_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Usr Read Write access\n");

    SwitchToUserMode();
    test_delay(1000);
    IO_WRITEL(val, addr);
    test_delay(1000);
    if (val != IO_READL(addr))
    {
        MPU_PR_DEBUG("Err:Usr read access ok ,but read result is fail\n");
    }
    test_delay(1000);

    MPU_PR_DEBUG("PASS: Usr Read Write\n");
}

#endif


/******************************************************/

#ifdef TEST_PRI_R_4
/* Privileged Read Only, Unprivileged Read Only */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SDARM0_MPU_ADDR + (SDRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged write not access:except enter MPU FAULT\n");
    test_delay(1000);
    IO_WRITEL(val, addr);
    test_delay(1000);
    MPU_PR_DEBUG("Fail: test Privileged write not access\n");
}

#endif


#ifdef TEST_USR_R_4
/* Privileged Read Only, Unprivileged Read Only */

void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM0_MPU_ADDR + (SDRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USER Read access\n");

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("PASS: test Privileged Read access\n");
}

#endif


#ifdef TEST_USR_W_4
/* Privileged Read Only, Unprivileged Read Only */

void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM0_MPU_ADDR + (SDRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR write not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    test_delay(1000);

    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test usr writr unaccess\n");
}


#endif





#ifdef TEST_PRI_W_4
/* Privileged Read Only, Unprivileged Read Only */
void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SDARM0_MPU_ADDR + (SDRAM0_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged write not access:except enter MPU FAULT\n");
    test_delay(1000);
    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("PASS: test Privileged Read access\n");

}

#endif





/*****************************************************/

#ifdef TEST_PRI_W_5
/* Privileged Read Only, Unprivileged No Access */
void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM1_MPU_ADDR + (SDRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged write not access:except enter MPU FAULT\n");
    test_delay(1000);
    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test Privileged write not access\n");
}

#endif

#ifdef TEST_PRI_R_5
/* Privileged Read Only, Unprivileged No Access */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SDARM1_MPU_ADDR + (SDRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged Read access\n");

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("PASS: test Privileged Read access\n");
}

#endif

#ifdef TEST_USR_W_5
/* Privileged Read Only, Unprivileged No Access */
void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM1_MPU_ADDR + (SDRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR write not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    test_delay(1000);

    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test usr writr unaccess\n");
}


#endif

#ifdef TEST_USR_R_5

/* Privileged Read Only, Unprivileged No Access */


void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM1_MPU_ADDR + (SDRAM1_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR read not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    test_delay(1000);
    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test usr read unaccess\n");
}

#endif


#ifdef TEST_PRI_W_6
/* Privileged Read Only, Unprivileged Read Only */

void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM2_MPU_ADDR + (SDRAM2_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged write not access:except enter MPU FAULT\n");

    test_delay(1000);
    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test Privileged write not access\n");

}

#endif

#ifdef TEST_PRI_R_6
/* Privileged Read Only, Unprivileged Read Only */

void mpu_test()
{

    uint32_t val = 0xaa;
    uint32_t addr = SDARM2_MPU_ADDR + (SDRAM2_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test Privileged Read access\n");

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("PASS: test Privileged Read access\n");


}

#endif

#ifdef TEST_USR_W_6
/* Privileged Read Only, Unprivileged Read Only */


void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SDARM2_MPU_ADDR + (SDRAM2_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR write not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    test_delay(1000);

    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test usr write unaccess\n");

}


#endif

#ifdef TEST_USR_R_6

/* Privileged Read Only, Unprivileged No Access */
void mpu_test()
{

    uint32_t val = 0xaa;
    static uint32_t addr = SDARM2_MPU_ADDR + (SDRAM2_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test USR read access\n");

    SwitchToUserMode();

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("Fail: test Usr No Access\n");



}

#endif




#ifdef TEST_PRI_R_7
/* Privileged Read Write, Unprivileged Read Write */
void mpu_test()
{
    uint32_t addr = SDARM3_MPU_ADDR + (SDRAM3_REGIO_SIZE >> 1);
    MPU_PR_DEBUG("test Privileged read  access\n");
    IO_READL(addr);
    test_delay(1000);
    MPU_PR_DEBUG("Pass:test Privileged read  access\n");
}
#endif


#ifdef TEST_USR_R_7
/* Privileged Read Write, Unprivileged Read Write */
void mpu_test()
{

    uint32_t addr = SDARM3_MPU_ADDR + (SDRAM3_REGIO_SIZE >> 1);

    MPU_PR_DEBUG("test user read access\n");

    SwitchToUserMode();

    IO_READL(addr);
    test_delay(1000);

    MPU_PR_DEBUG("Pass:test user read access\n");

}
#endif



#ifdef TEST_PRI_W_7
/* Privileged Read Write, Unprivileged Read Write */

void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SDARM3_MPU_ADDR + (SDRAM3_REGIO_SIZE >> 1);
    MPU_PR_DEBUG("test Privileged write not access:except enter MPU FAULT\n");
    IO_WRITEL(val, addr);
    test_delay(1000);
    MPU_PR_DEBUG("Err :Not except\n");
}
#endif


#ifdef TEST_USR_W_7
/* Privileged Read Write, Unprivileged Read Write */
void mpu_test()
{
    uint32_t val = 0xaa;
    uint32_t addr = SDARM3_MPU_ADDR + (SDRAM3_REGIO_SIZE >> 1);


    MPU_PR_DEBUG("test usr write not access:except enter MPU FAULT\n");

    SwitchToUserMode();
    IO_WRITEL(val, addr);
    test_delay(1000);

    MPU_PR_DEBUG("Err :Not except\n");
}
#endif



/*##############################################################################################*/

static void HAL_MPU_SetRegionsAddr(MPU_CONFIG_T *pconfig)
{

    pconfig->mpuRegions[0].base = FLASH_MPU_ADDR;
    pconfig->mpuRegions[1].base = SARM0_MPU_ADDR;
    pconfig->mpuRegions[2].base = SARM1_MPU_ADDR;
    pconfig->mpuRegions[3].base = SARM2_MPU_ADDR;
    pconfig->mpuRegions[4].base = SDARM0_MPU_ADDR;
    pconfig->mpuRegions[5].base = SDARM1_MPU_ADDR;
    pconfig->mpuRegions[6].base = SDARM2_MPU_ADDR;
    pconfig->mpuRegions[7].base = SDARM3_MPU_ADDR;



}
uint32_t i = 1;
int main(void)
{
    RET_CODE ret = RET_OK;

    MPU_PR_DEBUG("TEST_PRI_R_0:this mpu test\n");
    /*for commpier ,so set right address*/
    HAL_MPU_SetRegionsAddr(&mpu_config);
    ret = HAL_MPU_MutiRegionConfig(&mpu_config);
    if (ret)
    {
        MPU_PR_DEBUG("ERR:%d HAL_MPU_MutiRegionConfig\n", ret);
        return ret;
    }

    mpu_test();

    do
    {
        i++;
    } while (1);

    return 0;
}
