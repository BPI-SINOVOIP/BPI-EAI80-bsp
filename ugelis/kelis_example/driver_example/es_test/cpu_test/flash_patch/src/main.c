/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 *            (C) COPYRIGHT 2009-2010 ARM Limited.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 *
 *      SVN Information
 *
 *
 *      Revision            : $Revision: 119445 $
 *
 *      Release information : cortexm4_r0p1_00rel0
 *-----------------------------------------------------------------------------
 */

////////////////////////////////////////////////////////////////////////////////
//
// Cortex-M4 Integration Kit Flash Patch test
//
////////////////////////////////////////////////////////////////////////////////
//
// Simple example test case which shows how to use the Flash Patch Functionality
// to replace a function in your code.
//

#include <ugelis.h>
#include <misc/printk.h>

#include <gm_soc.h>
#include <gm_hal_cpu.h>
#include <linker/linker-defs.h>






#define CODE_ALINNED        __attribute__ ((aligned (64)))
#define DATA_ALINNED        __attribute__ ((aligned (64)))


void CheckHex(char *name, uint32_t actual, uint32_t expected);
int original_function(void);
extern void new_function(void);

void  copy_patch_code(void *new_fn_addr, void *sram_addr);


void patch(void *address_to_patch, void *sram_map_address);


volatile uint32_t Errors = 0;
volatile int data = 0;
volatile int exp_data = 0;


#define NEW_DATA_EXCEPT     3
volatile uint32_t  tests_data = 1;


/*SAVE REAMAP SRAM ADDRESS*/
volatile char gsram_buf[64] DATA_ALINNED = {0};


int CODE_ALINNED original_function(void)
{
    tests_data = 0xdeaddead;
    return tests_data;

}


/* #define NEW_DATA_EXCEPT 3 */
void CODE_ALINNED new_function(void)
{
    __asm__ volatile(
                    "ldr  r3, =tests_data; \n\t"
                    "movs r2 , #3; \n\t"
                    "str  r2, [r3, #0]; \n\t"
                    "ldr	r0, [r3, #0];\n\t"
                    "bx	lr;\n\t"
    );
}


int main(void)
{
    // copy the new function to the sram aline address
    copy_patch_code(new_function, (void *)gsram_buf);
    // Banner
    printk(("Cortex-M4 - Flash Patch testing\n"));

    //-----------------------------------------------------------------
    // Exercise the Flash Patchin functionality
    //-----------------------------------------------------------------

    data = original_function();
    exp_data = 0xdeaddead;
    CheckHex("Before Flash Patch, Value returned by original_function() is ",
             data, exp_data);

    HAL_FlashPatchConfig((void *)original_function, (void *)(gsram_buf));

    /**
     * if patch function ok ,it will call new_function
     * and set tests_data =  NEW_DATA_EXCEPT
     */
    original_function();
    data = tests_data;
    exp_data = NEW_DATA_EXCEPT;
    CheckHex("After Flash Patch, Value returned by original_function() is",
             data, exp_data);

    if (Errors)
    {
        printk("flash patch test fail\n");
    }
    else
    {
        printk("flash patch success\n");
    }
    return Errors;
}


//-----------------------------------------------------------------
// Functions and Handlers
//-----------------------------------------------------------------



void CheckHex(char *name, uint32_t actual, uint32_t expected)
{

    if (actual != expected)
    {
        Errors++;
    }

}


void   copy_patch_code(void *new_fn_addr, void *sram_addr)
{

    unsigned int ptr_fn_addr = (unsigned int) new_fn_addr;
    unsigned int ptr_fn_dest = (unsigned int)sram_addr;

    // make sure it's word aligned
    ptr_fn_addr = (ptr_fn_addr & 0xFFFFFFFC);

    *(uint32_t *)ptr_fn_dest = *(uint32_t *)ptr_fn_addr;

    ptr_fn_addr += 4;
    ptr_fn_dest += 4;

    *(uint32_t *)ptr_fn_dest = *(uint32_t *)ptr_fn_addr;

    ptr_fn_addr += 4;
    ptr_fn_dest += 4;

    *(uint32_t *)ptr_fn_dest = *(uint32_t *)ptr_fn_addr;

    ptr_fn_addr += 4;
    ptr_fn_dest += 4;

    *(uint32_t *)ptr_fn_dest = *(uint32_t *)ptr_fn_addr;

}


