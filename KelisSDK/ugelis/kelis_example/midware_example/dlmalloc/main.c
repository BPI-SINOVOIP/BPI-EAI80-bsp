/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		main.c
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		Sample code for adc test case.
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#include <ugelis.h>
#include <misc/printk.h>
#include <dlmalloc.h>

int main(void)
{
	int    i;
	void 		*dlmem[15];
	uint32_t  	dmsize[15] = {0x10,0x100,0x1000,0x10000,0x100000,0x20,0x200,0x2000,0x20000,0x200000,0x30,0x300,0x3000,0x30000,0x300000};
	
	while(1)
	{
		for(i = 0;i < 15;i++)
		{
			dlmem[i] = dlmem_malloc(dmsize[i]);
			printf("===dlmalloc func[%s] Test===dlmem[%d]=%p==========\n\n\n",__FUNCTION__, i,dlmem[i]);
		}
		
		k_sleep(1000);
		
		for(i = 0;i < 15;i++)
		{
			 dlmem_free(dlmem[i]);
		}

		k_sleep(1000);
	}
	
	return 0;

}

