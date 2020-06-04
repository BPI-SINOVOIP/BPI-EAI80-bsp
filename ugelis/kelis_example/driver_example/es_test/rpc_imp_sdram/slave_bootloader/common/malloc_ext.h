#ifndef __MALLOC_H
#define __MALLOC_H

//#include <ugelis.h>
#include <string.h>

#define SRAMEX   0
#define MEMORY_BANK     1

//#define CONFIG_SDRAM_MALLOC_SIZE  (25*1024*1024)
#define CONFIG_SDRAM_MALLOC_SIZE    (3*1024*1024)
#define MEM_BLOCK_SIZE          64
#define MEM_MAX_SIZE            CONFIG_SDRAM_MALLOC_SIZE
#define MEM_ALLOC_TABLE_SIZE    MEM_MAX_SIZE/MEM_BLOCK_SIZE

typedef unsigned short int uint16_t;
typedef unsigned char  uint8_t;
typedef unsigned  int uint32_t;

struct _m_mallco_dev
{
    void (*init)(uint8_t);
    uint16_t (*perused)(uint8_t);
    uint8_t     *membase[MEMORY_BANK];
    uint32_t *memmap[MEMORY_BANK];
    uint8_t  memrdy[MEMORY_BANK];
};
extern struct _m_mallco_dev mallco_dev;

void memset_ext(void *s, uint8_t c, uint32_t count);
void memcpy_ext(void *des, void *src, uint32_t n);
void mem_init_ext(uint8_t memx);
uint32_t mem_malloc_ext(uint8_t memx, uint32_t size);
uint8_t mem_free_ext(uint8_t memx, uint32_t offset);
uint16_t mem_perused_ext(uint8_t memx) ;

void free_ext(uint8_t memx, void *ptr);
void *malloc_ext(uint8_t memx, uint32_t size);
void *realloc_ext(uint8_t memx, void *ptr, uint32_t size);
#endif
