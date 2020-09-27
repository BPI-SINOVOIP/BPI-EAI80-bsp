/*
 * Copyright (c) 2017 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <gm_common.h>
#include <gm_soc.h>
#include "arm_ll_flash_patch.h"

/*todo fix those magic number seting porting from arm test code*/
void HAL_FlashPatchConfig(void* address_to_patch, void* sram_map_address)
{

   unsigned thing1 = (unsigned) address_to_patch;
   unsigned thing2 = (unsigned) sram_map_address;

   // Each COMPx register matches one word within the 8-word REMAP region so
   // to rememp (in our case) new_function() requires three words to be
   // remapped (thus we use COMP0,1 and 2.
   FPB_Dev->COMP0 = ((thing1 & 0x1FFFFFFC) | 0x1);
   FPB_Dev->COMP1 = (((thing1 & 0x1FFFFFFC) + 0x4) | 0x1);
   FPB_Dev->COMP2 = (((thing1 & 0x1FFFFFFC) + 0x8) | 0x1);

   // set up the REMAP register
   // - [31:29] Reserved
   // - [28:5]  New address aligned to 8 word boundary
   // - [4:0]   Reserved
   FPB_Dev->REMAP = (thing2 & 0x1FFFFFE0);

   //printk("COMP0:0x%x COMP1:0x%x COMP2:0x%x\n",FPB->COMP0 ,FPB->COMP1,FPB->COMP2  );
   //printk("COMP0:0x%x COMP1:0x%x COMP2:0x%x\n",*(volatile uint32_t *)(0xe0002000+8),*(volatile uint32_t *)(0xe0002000+0x0c) ,*(volatile uint32_t *)(0xe0002000+0x10)  );
   // Enable the FPB
   // - [31:15]   Reserved
   // - [14:12]   NUM_CODE2 = 0
   // - [11:8]    NUM_LIT   = 0
   // - [7:4]     NUM_CODE1 = 6
   // - [1]       KEY       = 1
   // - [0]       ENABLE    = 1
   FPB_Dev->CTRL  = 0x00000063;

}


