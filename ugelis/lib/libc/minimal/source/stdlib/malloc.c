/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ugelis.h>
#include <init.h>
#ifdef __USE_KEIL_
#include <errno_ex.h>
#else
#include <errno.h>
#endif
#include <misc/mempool.h>
#include <string.h>
#include <logging/sys_log.h>

#ifdef CONFIG_MINIMAL_LIBC_EXT//Evan debug

void *malloc(size_t size)
{
#if 1
	return malloc_ext(0, size);
#else
    void *ptr = malloc_ext(0, size);
    if (ptr != NULL)
    {
        memset((uint8_t *)ptr, 0, size);
    }
	return ptr;
#endif
}

void *calloc(size_t nmemb, size_t size)
{
    void *ptr = malloc(nmemb*size);
    if (ptr != NULL)
    {
        memset((uint8_t *)ptr, 0, nmemb*size);
    }
	return ptr;
}

void *realloc(void *ptr, size_t requested_size)
{
	if (ptr == NULL)
	{
		return malloc(requested_size);
	}
    return realloc_ext(0, ptr, requested_size);
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
    // not support yet
    return NULL;
}

void free(void *ptr)
{
	free_ext(0, ptr); 
}




#else
#if (CONFIG_MINIMAL_LIBC_MALLOC_ARENA_SIZE > 0)
K_MUTEX_DEFINE(malloc_mutex);
SYS_MEM_POOL_DEFINE(z_malloc_mem_pool, &malloc_mutex, 16,
		    CONFIG_MINIMAL_LIBC_MALLOC_ARENA_SIZE, 1, 4, .data);

void *malloc(size_t size)
{
	void *ret;

	ret = sys_mem_pool_alloc(&z_malloc_mem_pool, size);
	if (!ret) {
		errno = ENOMEM;
	}

	return ret;
}

static int malloc_prepare(struct device *unused)
{
	ARG_UNUSED(unused);

#ifdef CONFIG_USERSPACE
	k_object_access_all_grant(&malloc_mutex);
#endif
	sys_mem_pool_init(&z_malloc_mem_pool);

	return 0;
}

SYS_INIT(malloc_prepare, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
#else /* No malloc arena */
void *malloc(size_t size)
{
	ARG_UNUSED(size);

	SYS_LOG_DBG("CONFIG_MINIMAL_LIBC_MALLOC_ARENA_SIZE is 0\n");
	errno = ENOMEM;

	return NULL;
}
#endif

void free(void *ptr)
{
	sys_mem_pool_free(ptr);
}

static bool size_t_mul_overflow(size_t a, size_t b, size_t *res)
{
#if __SIZEOF_SIZE_T__ == 4
	return __builtin_umul_overflow((unsigned int)a, (unsigned int)b,
				       (unsigned int *)res);
#else /* __SIZEOF_SIZE_T__ == 8 */
	return __builtin_umulll_overflow((unsigned long long)a,
					 (unsigned long long)b,
					 (unsigned long long *)res);
#endif
}

void *calloc(size_t nmemb, size_t size)
{
	if (size_t_mul_overflow(nmemb, size, &size)) {
		errno = ENOMEM;
		return NULL;
	}
	return malloc(size);
}

void *realloc(void *ptr, size_t requested_size)
{
	struct sys_mem_pool_block *blk;
	size_t block_size, total_requested_size;
	void *new_ptr;

	if (requested_size == 0) {
		return NULL;
	}

	/* Stored right before the pointer passed to the user */
	blk = (struct sys_mem_pool_block *)((char *)ptr - sizeof(*blk));

	/* Determine size of previously allocated block by its level.
	 * Most likely a bit larger than the original allocation
	 */
	block_size = _ALIGN4(blk->pool->base.max_sz);
	for (int i = 1; i <= blk->level; i++) {
		block_size = _ALIGN4(block_size / 4);
	}

	/* We really need this much memory */
	total_requested_size = requested_size +
		sizeof(struct sys_mem_pool_block);

	if (block_size >= total_requested_size) {
		/* Existing block large enough, nothing to do */
		return ptr;
	}

	new_ptr = malloc(requested_size);
	if (!new_ptr) {
		return NULL;
	}

	memcpy(new_ptr, ptr, block_size - sizeof(struct sys_mem_pool_block));
	free(ptr);

	return new_ptr;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	if (size_t_mul_overflow(nmemb, size, &size)) {
		errno = ENOMEM;
		return NULL;
	}
	return realloc(ptr, size);
}

#endif




