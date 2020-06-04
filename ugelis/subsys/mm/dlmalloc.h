/*-------------------------------------------------------------------------

		FileName    :Heap_c.h
		Module      :
		Description :
		By          :

-------------------------------------------------------------------------*/
#ifndef __DL_MALLOC_H__
#define __DL_MALLOC_H__

#include <ugelis.h>
#include <string.h>
#include <linker/linker-defs.h>
#include <board.h>

#define DLMEM_BLOCK_SIZE				64
#define DLMEM_ALINE_BYTE                32


typedef unsigned short int 				uint16_t;
typedef unsigned char  					uint8_t;
typedef unsigned  int 					uint32_t;

void 	dlmem_init();
void 	dlmem_uninit();
int 	dlmem_check(void *mem);
void 	*dlmem_malloc(uint32_t size);
void 	dlmem_free(void *mem);
void 	*dlmem_realloc(void *mem,uint32_t size);

#endif

