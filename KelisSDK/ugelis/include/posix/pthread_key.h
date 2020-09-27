/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __POSIX_THREAD_KEY_H__
#define __POSIX_THREAD_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_PTHREAD_IPC
#include <misc/slist.h>
#include <ugelis/types.h>

typedef u32_t pthread_once_t;

/* pthread_key */
typedef void *pthread_key_t;

typedef struct pthread_key_obj {
	/* List of pthread_key_data objects that contain thread
	 * specific data for the key
	 */
	sys_slist_t key_data_l;

	/* Optional destructor that is passed to pthread_key_create() */
	void (*destructor)(void *);
} pthread_key_obj;

typedef struct pthread_thread_data {
	sys_snode_t node;

	/* Key and thread specific data passed to pthread_setspecific() */
	pthread_key_obj *key;
	void *spec_data;
} pthread_thread_data;

typedef struct pthread_key_data {
	sys_snode_t node;
	pthread_thread_data thread_data;
} pthread_key_data;

#endif /* CONFIG_PTHREAD_IPC */

#ifdef __cplusplus
}
#endif

#endif /* __POSIX_THREAD_KEY_H__ */
