/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Memory allocation functions.
 *
 */
#include <kernel.h>
#include "xalloc.h"
#include <dlmalloc.h>

#if 0
// returns null pointer without error if size==0
void *xalloc(uint32_t size)
{
    void *mem = k_malloc(size);
    if (size && (mem == NULL)) {
        return NULL;
    }
    return mem;
}

// returns null pointer without error if size==0
void *xalloc_try_alloc(uint32_t size)
{
    void *mem = k_malloc(size);
    if (size && (mem == NULL)) {
        return NULL;
    }
    return mem;
}

// returns null pointer without error if size==0
void *xalloc0(uint32_t size)
{
    void *mem = k_malloc(size);
    if (size && (mem == NULL)) {
        return NULL;
    }
    memset(mem, 0, size);
    return mem;
}

// returns without error if mem==null
void xfree(void *mem)
{
    k_free(mem);
}

// returns null pointer without error if size==0
// allocs if mem==null and size!=0
// frees if mem!=null and size==0
void *xrealloc(void *mem, uint32_t size)
{
    void *reptr;

    reptr = k_malloc(size);
    if(reptr == NULL)
    {
		return NULL;
    }
    else
    {
        memcpy(reptr, mem, size);
        k_free(mem);
		
        return reptr;
    }
}
#else
// returns null pointer without error if size==0
void *xalloc(uint32_t size)
{
    void *mem = dlmem_malloc(size);
    if (size && (mem == NULL)) {
        return NULL;
    }
    return mem;
}

// returns null pointer without error if size==0
void *xalloc_try_alloc(uint32_t size)
{
    void *mem = dlmem_malloc(size);
    if (size && (mem == NULL)) {
        return NULL;
    }
    return mem;
}

// returns null pointer without error if size==0
void *xalloc0(uint32_t size)
{
    void *mem = dlmem_malloc(size);
    if (size && (mem == NULL)) {
        return NULL;
    }
    memset(mem, 0, size);
    return mem;
}

// returns without error if mem==null
void xfree(void *mem)
{
    if(mem)
	{
		dlmem_free(mem);
	}
}

// returns null pointer without error if size==0
// allocs if mem==null and size!=0
// frees if mem!=null and size==0
void *xrealloc(void *mem, uint32_t size)
{
    return dlmem_realloc(mem, size);
}

#endif
