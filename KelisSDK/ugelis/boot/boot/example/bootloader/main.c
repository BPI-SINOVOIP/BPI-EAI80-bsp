/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file		  main.c
 *
 * @author		 richard.liu
 *
 * @version 	  1.0.0
 *
 * @date		  2018/05/07
 *
 * @brief		 Sample code for adc test case.
 *
 * @note
 *	  2018/05/07, richard.liu, V1.0.0
 *		  Initial version.
 */
#include <stdio.h>
#include <driver/gpio.h>
#include <gm_hal_uart.h>
#include <gm_hal_adc.h>
#include <gm_common.h>


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
}info_t;

typedef void (*Reboot_Fun_T)(void);

static void systick_disable()
{
    *(unsigned int*)0xE000E010 = 0;
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


int main(void)
{
    RET_CODE ret = RET_OK;
    info_t info;

		SysTick_Init();
		DebugConsoleInit(115200);  
    printk("Enter BOOT_1...\n"); 
	
	gm6721_sdram_init();

#if 0	
    /* get info */
    memcpy(&info, (uint32_t *)0x10000000, sizeof(info_t)); 
    printk("system_load_addr=0x%x, system_addr=0x%x, system_size=0x%x\n", info.system_load_addr, info.system_addr, info.system_size); 
#else
    info.system_load_addr = 0xC0010000;
    info.system_addr = 0x10010000;
    info.system_size = (200*1024);
#endif
    /* copy system to sdram */
    memcpy((uint32_t *)info.system_load_addr, (uint32_t *)info.system_addr, info.system_size);
    printk("load system to 0x%x done\n", info.system_load_addr); 

#if 0//mike
    printk("1\n");   
    /*for commpier ,so set right address*/
    HAL_MPU_SetRegionsAddr(&mpu_config);
    printk("2\n");  
    ret = HAL_MPU_MutiRegionConfig(&mpu_config);
    printk("3\n");  
    if(ret)
    {
        BOOT_PR_DEBUG("ERR:%d HAL_MPU_MutiRegionConfig\n",ret);
        printk("1\n");  
        return ret;
    }
    printk("4\n");  
//#else//justin
    #define MPU_FULL_ACC_NON_CACHEABLE    (0x320 << 16)   /*!< Attribute registerin MPU   */
    // Base address = Base address :OR: Region number :OR: VALID bit   
    MPU->RBAR = (((0x00000000 & 0xFFFFFFE0) | 0x1) |0x10) ;   
    // Attribute = Full access :OR: SRD = 0 :OR: Size = 4GB :OR: ENABLE   
    MPU->RASR = ((MPU_FULL_ACC_NON_CACHEABLE)|(0x1f<<1)| 0x1);   
    // Enable MPU   
    MPU->CTRL = 0x1;
#endif

        

    /*you must to do */
    //get pc
    //get sp
    //goto pc
    //HAL_SetBusMapping(MAP_SDRM_REGION);
    *(unsigned int*)0xE0042020 = 0x3;

    SetPC(info.system_load_addr);	
    //*(volatile uint32_t *)(0xE000ED0C) = 0x05FA0004;
    //while(1);
    
    return 0;
}