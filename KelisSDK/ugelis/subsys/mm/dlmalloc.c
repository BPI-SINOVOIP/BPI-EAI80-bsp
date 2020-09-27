/*------------- includes -------------*/
#include <device.h>
#include <init.h>
#include <arch/cpu.h>
#include <cortex_m/exc.h>

#include "dlmemory.h"
#include "dlmalloc.h"
#include <misc/printk.h>

#ifdef __USE_KEIL_
extern unsigned char Load$$VT_REGION$$Base;
extern unsigned char Image$$VT_REGION$$Base;
extern unsigned char Image$$VT_REGION$$Length;
extern unsigned char Load$$VT_REGION$$Length;
extern unsigned char Image$$VT_REGION$$Limit;
#endif

static mspace 			g_heap_base;
static void 			*heap_start;
static unsigned int 	heap_size;

void dlmem_init()
{
	#ifdef __USE_KEIL_
		uint32_t mem_start 	= (uint32_t)(((unsigned int)&Image$$VT_REGION$$Base) + ((unsigned int)&Load$$VT_REGION$$Length));
	#else
		uint32_t mem_start 	= (uint32_t)(&_end);
	#endif
	
	mem_start &= 0xFFFFFFF8;	//确保memory pool大小是8字节对齐的
	
	heap_start 				= (void *)mem_start; 
    uint32_t mem_end   		= MEM_RESVER_START;
    heap_size 				= mem_end - mem_start - DLMEM_ALINE_BYTE;
    
	g_heap_base = create_mspace_with_base(heap_start, heap_size, 1);
}

void dlmem_uninit()
{
	destroy_mspace(g_heap_base);
}

int dlmem_check(void *mem)
{
	if(((((uint8_t *)(mem)) > heap_start) 
		&& (((uint8_t *)(mem)) < (heap_start + heap_size)) 
		&& (mspace_max_footprint(g_heap_base) == heap_size) 
	    && (mspace_usable_size(mem) != 0)))
    {
    	return 0;
    }
    
    return -1;
}	


void *dlmem_malloc(uint32_t size)		
{
	return mspace_malloc(g_heap_base, size);
}

void dlmem_free(void *mem)	
{
	mspace_free(g_heap_base, mem);
}

void *dlmem_realloc(void *mem,uint32_t size)		
{
	return mspace_realloc(g_heap_base, mem,size);
}

static int dlmem_dev_init(struct device *port)
{
    printk("##################dlmem_dev_init#####################################\n");
    dlmem_init(0);

    return 0;

}

SYS_INIT(dlmem_dev_init, PRE_KERNEL_1, 2);