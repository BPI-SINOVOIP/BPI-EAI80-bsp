/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_cpu.h>
#include <gm_hal_bus_map.h>
#include <linker/linker-defs.h>
#include <drivers/system_timer.h>
#include <arch/arm/cortex_m/cmsis.h>



#define SDRAM_MAP   1         //0x0000_0000 - 0x0FFF_FFFF  map to 0xc000_0000 - 0x6FFF_FFFF
#ifdef SDRAM_MAP
    #define MAP_SRC       (0)
    #define MAP_DESC      (0xc0000000)
    #define LENGTH        (16*1024*1024)
#endif




static void sysTickStop(void)
{
    uint32_t reg;
    reg = SysTick->CTRL;
    reg &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    SysTick->CTRL = reg;
}

void set_map()
{
    BUS_Mapping_T bus_map;

    sysTickStop();

#ifdef  SDRAM_MAP
    bus_map = MAP_SDRM_REGION;
#endif

    HAL_SetBusMapping(bus_map);

}


int map_test()
{
    uint32_t i = 0;

    uint8_t  *p8_src  = (uint8_t *)MAP_SRC;
    uint16_t *p16_src = (uint16_t *)MAP_SRC;
    uint32_t *p32_src = (uint32_t *)MAP_SRC;

    uint8_t  *p8_des  = (uint8_t *)MAP_DESC;
    uint16_t *p16_des  = (uint16_t *)MAP_DESC;
    uint32_t *p32_des  = (uint32_t *)MAP_DESC;

    printk("SDRAM_MAP TEST\n");


    /*src write : desc read*/
    for (i = 0; i < LENGTH ; i++)
    {
        //printk("8 bit src write i:0x%08x\n",i);
        p8_src[i] = i;
    }

    for (i = 0; i < LENGTH ; i++)
    {
        //printk("8 bit desc read check i:0x%08x\n",i);
        if (p8_des[i] != i % 256)
        {
            printk("8bit err: index:%d p8_des[%d]:%d src write : desc read err\n", i, i, p8_des[i]);
            return -1;
        }
    }
    memset(p8_src, 0, LENGTH);
    memset(p8_des, 0, LENGTH);
    /*desc write : src read*/

    for (i = 0; i < LENGTH ; i++)
    {
        //printk("8 bit desc write i:0x%08x\n",i);
        p8_des[i] = i;
    }

    for (i = 0; i < LENGTH ; i++)
    {
        //printk("8 bit src read check i:0x%08x\n",i);
        if (p8_src[i] != i % 256)
        {
            printk("8bit err: index:%d des write : src read err\n", i);
            return -1;
        }

    }
    printk("###8 bit test pass####\n");



    /*src write : desc read*/
    for (i = 0; i < LENGTH / 2 ; i++)
    {
        //printk("16 bit src write i:0x%08x\n",i);
        p16_src[i] = i;
    }

    for (i = 0; i < LENGTH / 2 ; i++)
    {
        //printk("16 bit desc read check i:0x%08x\n",i);
        if (p16_des[i] != i % 65536)
        {
            printk("16bit err: index:%d src write : desc read err\n", i);
            return -1;
        }

    }
    memset(p8_src, 0, LENGTH);
    memset(p8_des, 0, LENGTH);
    /*desc write : src read*/

    for (i = 0; i < LENGTH / 2 ; i++)
    {
        //printk("16 bit desc write i:0x%08x\n",i);
        p16_des[i] = i;
    }

    for (i = 0; i < LENGTH / 2 ; i++)
    {
        //printk("16 bit src read check i:0x%08x\n",i);
        if (p16_src[i] != i % 65536)
        {
            printk("16bit err: index:%d des write : src read err\n", i);
            return -1;
        }

    }
    printk("###16 bit test pass####\n");


#ifdef SDRAM_MAP
    /*src write : desc read*/
    for (i = 0; i < LENGTH / 4 ; i++)
    {
        //printk("32 bit src write i:0x%08x\n",i);
        p32_src[i] = i;
    }

    for (i = 0; i < LENGTH / 4 ; i++)
    {
        //printk("32 bit desc read check i:0x%08x\n",i);
        if (p32_des[i] != i)
        {
            printk("32bit err: index:%d src write : desc read err\n", i);
            return -1;
        }

    }
    memset(p8_src, 0, LENGTH);
    memset(p8_des, 0, LENGTH);
    /*desc write : src read*/

    for (i = 0; i < LENGTH / 4 ; i++)
    {
        //printk("32 bit src write i:0x%08x\n",i);
        p32_des[i] = i;
    }

    for (i = 0; i < LENGTH / 4 ; i++)
    {
        //printk("32 bit src read check i:0x%08x\n",i);
        if (p32_src[i] != i)
        {
            printk("32bit err: index:%d desc write : src read err\n", i);
            return -1;
        }
    }
    printk("###32 bit test pass####\n");

#endif
    return 0;
}

int main()
{
    int ret = 0;

    printk("set bus map\n");
    /*ide help to init sdram*/
    set_map();

    ret = map_test();
    if (ret)
    {
        printk("map_test fail\n");
    }
    else
    {
        printk("map_test all pass\n");
    }

    return ret;
}



