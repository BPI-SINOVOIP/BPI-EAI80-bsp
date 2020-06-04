#include <device.h>
#include <init.h>
#include <arch/cpu.h>
#include <cortex_m/exc.h>

#include "malloc_ext.h"
#include <misc/printk.h>

#ifdef __USE_KEIL_
    extern unsigned char Load$$VT_REGION$$Base;
    extern unsigned char Image$$VT_REGION$$Base;
    extern unsigned char Image$$VT_REGION$$Length;
    extern unsigned char Load$$VT_REGION$$Length;
    extern unsigned char Image$$VT_REGION$$Limit;
#endif
static uint32_t memtblsize[MEMORY_BANK];
static uint32_t memblksize[MEMORY_BANK];
static uint32_t memsize[MEMORY_BANK];

static uint8_t *membase = NULL;
static uint32_t *memmapbase = NULL;

struct _m_mallco_dev mallco_dev =
{
    .init    = mem_init_ext,
    .perused = mem_perused_ext,

};

void mem_area_config()
{

    /*|-----mem_start-------|*/
    /*|---------aline4------|*/
    /*|------manage_table---|*/
    /*|------manage_table---|*/
    /*|------manage_table---|*/
    /*|-------aline32-------|*/
    /*|------heap_start-----|*/
    /*|------malloc area----|*/
    /*|------malloc area----|*/
    /*|------heap_end-------|*/
    /*|-----mem_end---------|*/
    /**/
    uint32_t manage_table_entry_num;
    uint32_t manage_table_entry_start_address;
    uint32_t heap_start_address;
    uint32_t tatal_size;

#ifdef CONFIG_SDRAM_BOOT
#ifdef __USE_KEIL_
    uint32_t mem_start = (uint32_t)(((unsigned int)&Image$$VT_REGION$$Base) + ((unsigned int)&Load$$VT_REGION$$Length));
#else
    uint32_t mem_start = (uint32_t)(&_end);
#endif
    uint32_t mem_end   = MEM_RESVER_START;
#else
#if (defined(CONFIG_GM_HAL_RPC_SLAVE) && defined(CONFIG_SLAVE_SDRAM)) //slave run in sram and support using sdram 
    uint32_t mem_start = SLAVE_MEM_START;
    uint32_t mem_end   = SLAVE_MEM_END;
#endif
#endif

    tatal_size = mem_end - mem_start - ALINE_BYTE;
    /*table_entry_num*MEM_BLOCK_SIZE + table_entry_num*4 = tatal_size*/
    manage_table_entry_num = tatal_size / (4 + MEM_BLOCK_SIZE);

    manage_table_entry_start_address = mem_start;
    memmapbase = (uint32_t *)manage_table_entry_start_address;

    heap_start_address = mem_start + 4 * manage_table_entry_num;
    /*heap_start_address aline ALINE_BYTE byte*/
    heap_start_address = (heap_start_address + ALINE_BYTE) & (~(ALINE_BYTE - 1));
    membase = (uint8_t *)heap_start_address;

    memtblsize[MEMORY_BANK - 1] = manage_table_entry_num;
    memblksize[MEMORY_BANK - 1] = MEM_BLOCK_SIZE;
    memsize[MEMORY_BANK - 1] = manage_table_entry_num * MEM_BLOCK_SIZE;

    mallco_dev.membase[MEMORY_BANK - 1] = membase;
    mallco_dev.memmap[MEMORY_BANK - 1]  = memmapbase;
    mallco_dev.memrdy[MEMORY_BANK - 1] = 0;


}

void mem_init_ext(uint8_t memx)
{
    mem_area_config();

    if (memx != 0)
    {
        memx = 0;
    }
    /*memset_ext(mallco_dev.memmap[memx],0,memtblsize[memx]*4);*/
    memset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 4);
    mallco_dev.memrdy[memx] = 1;
}

/**
 * @brief Get memory used percent
 *
 * @param memx ,reserved for future.
 *
 * @return memory used percent(expand 10 X,0~1000,for 0.0%~100.0%)
 */
uint16_t mem_perused_ext(uint8_t memx)
{
    uint32_t used = 0;
    uint32_t i;

    if (memx != 0)
    {
        memx = 0;
    }

    for (i = 0; i < memtblsize[memx]; i++)
    {
        if (mallco_dev.memmap[memx][i])
        {
            used++;
        }
    }
    return (used * 1000) / (memtblsize[memx]);
}

/**
 *  @brief Get memory malloc address
 *
 *  @param memx ,reserved for future.
 *  @param size the length of the text in bytes
 *
 *  @return  memory alloc address
 */
uint32_t mem_malloc_ext(uint8_t memx, uint32_t size)
{
    signed long offset = 0;
    uint32_t nmemb;
    uint32_t cmemb = 0;
    uint32_t i;

    if (memx != 0)
    {
        memx = 0;
    }

    if (!mallco_dev.memrdy[memx])
    {
        mallco_dev.init(memx);
    }
    if (size == 0)
    {
        return 0XFFFFFFFF;
    }
    nmemb = size / memblksize[memx];
    if (size % memblksize[memx])
    {
        nmemb++;
    }
    for (offset = memtblsize[memx] - 1; offset >= 0; offset--)
    {
        if (!mallco_dev.memmap[memx][offset])
        {
            cmemb++;
        }
        else
        {
            cmemb = 0;
        }
        if (cmemb == nmemb)
        {
            for (i = 0; i < nmemb; i++)
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }
            return (offset * memblksize[memx]);
        }
    }
    return 0XFFFFFFFF;
}

uint8_t mem_free_ext(uint8_t memx, uint32_t offset)
{
    int i;

    if (memx != 0)
    {
        memx = 0;
    }

    if (!mallco_dev.memrdy[memx])
    {
        mallco_dev.init(memx);
        return 1;
    }
    if (offset < memsize[memx])
    {
        int index = offset / memblksize[memx];
        int nmemb = mallco_dev.memmap[memx][index];
        for (i = 0; i < nmemb; i++)
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }
        return 0;
    }
    else
    {
        return 2;
    }
}

/**
 *  @brief free memory
 *
 *  @param param memx ,reserved for future.
 *  @param ptr memory pointer
 *
 * @return  N/A
 */
void free_ext(uint8_t memx, void *ptr)
{
    uint32_t offset;
    if (ptr == NULL)
    {
        return;
    }

    if (memx != 0)
    {
        memx = 0;
    }

    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
    mem_free_ext(memx, offset);
}

/**
 *  @brief malloc memory
 *
 *  @param param memx ,reserved for future.
 *  @param size the length of the text in bytes
 *
 * @return  memory alloc address
 */
void *malloc_ext(uint8_t memx, uint32_t size)
{
    uint32_t offset;

    if (memx != 0)
    {
        memx = 0;
    }

    offset = mem_malloc_ext(memx, size);
    if (offset == 0XFFFFFFFF)
    {
        return NULL;
    }
    else
    {
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
    }
}

/**
 *  @brief realloc memory
 *
 *  @param param memx ,reserved for future.
 *  @param ptr old memory pointer
 *  @param size the length of the text in bytes
 *
 * @return new memory alloc address
 */
void *realloc_ext(uint8_t memx, void *ptr, uint32_t size)
{
    uint32_t offset;

    if (memx != 0)
    {
        memx = 0;
    }

    offset = mem_malloc_ext(memx, size);
    if (offset == 0XFFFFFFFF)
    {
        return NULL;
    }
    else
    {
        /*memcpy_ext((void*)((uint32_t)mallco_dev.membase[memx]+offset),ptr,size);*/
        memcpy((void *)((uint32_t)mallco_dev.membase[memx] + offset), ptr, size);
        free_ext(memx, ptr);
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
    }
}


static int memory_init_ext(struct device *port)
{
    printk("##################memory_init_ext#####################################\n");
    mem_init_ext(0);

    return 0;

}


SYS_INIT(memory_init_ext, PRE_KERNEL_1, DEVICE_PRIORITY_MEMORY_MANAGE);


