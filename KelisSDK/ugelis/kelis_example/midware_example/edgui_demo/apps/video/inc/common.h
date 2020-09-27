/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          cmd.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Common definitions for more convenient use.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

static inline char *kmalloc(int size)
{
    return malloc_ext(0, size);
}

static inline void kfree(char *ptr)
{
    free_ext(0, ptr);
}

#endif //__COMMON_H__
