#include "malloc_ext.h"

extern void SDRAM_Init(void);
extern void dram_init(void);

__align(32) uint8_t membase[MEM_MAX_SIZE]  __attribute__((at(0XC01F4000)));
uint32_t memmapbase[MEM_ALLOC_TABLE_SIZE] __attribute__((at(0XC01F4000 + MEM_MAX_SIZE)));

const uint32_t memtblsize[MEMORY_BANK] = {MEM_ALLOC_TABLE_SIZE};
const uint32_t memblksize[MEMORY_BANK] = {MEM_BLOCK_SIZE};
const uint32_t memsize[MEMORY_BANK] = {MEM_MAX_SIZE};

struct _m_mallco_dev mallco_dev =
{
    mem_init_ext,
    mem_perused_ext,
    {membase},    /*sdram memory poll*/
    {memmapbase}, /*sdram memory manage table*/
    {0},
};

void mem_init_ext(uint8_t memx)
{
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

void memory_init_ext(void)
{
#ifdef CONFIG_SDRAM
    dram_init();
    mem_init_ext(0);
#endif
}
