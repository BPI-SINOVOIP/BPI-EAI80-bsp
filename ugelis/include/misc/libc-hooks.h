/*
 * Copyright (c) 2018, Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UGELIS_LIBC_HOOKS_H
#define UGELIS_LIBC_HOOKS_H

#include <toolchain.h>
#include <stdio.h>
#include <stddef.h>

/*
 * Private header for specifying accessory functions to the C library internals
 * that need to call into the kernel as system calls
 */

#ifdef CONFIG_NEWLIB_LIBC

/* syscall generation ignores preprocessor, ensure this is defined to ensure
 * we don't have compile errors
 */
#define _MLIBC_RESTRICT

__syscall int _ugelis_read(char *buf, int nbytes);

__syscall int _ugelis_write(char *buf, int nbytes);

#else
/* Minimal libc */
#if !defined(_MLIBC_RESTRICT_defined)
#define _MLIBC_RESTRICT_defined

#ifndef __USE_KEIL_
	#ifdef __cplusplus
		#define _MLIBC_RESTRICT __restrict__
	#else
		#define _MLIBC_RESTRICT restrict
	#endif
#else
	#define _MLIBC_RESTRICT __restrict
#endif
#endif

__syscall int _ugelis_fputc(int c, FILE *stream);

__syscall size_t _ugelis_fwrite(const void *_MLIBC_RESTRICT ptr, size_t size,
				size_t nitems, FILE *_MLIBC_RESTRICT stream);

#endif /* CONFIG_NEWLIB_LIBC */

#include <syscalls/libc-hooks.h>

#endif /* UGELIS_LIBC_HOOKS_H */
