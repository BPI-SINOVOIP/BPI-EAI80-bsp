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

char *kmalloc(int size);
void kfree(void *ptr);

void gui_clear_sync(int cnt);
void gui_refresh(int on);

void stream_init(void);
void stream_deinit(void);
int stream_start_videoview(void);

void gui_init(void *cb);
void gui_exit(void);

#ifndef __USE_KEIL_
    #define AI_SECTION __in_section_unique(mem_ai_data) __aligned(32)
#else
    #define AI_SECTION __attribute__((section(".mem_ai_data"),zero_init)) __aligned(32)
#endif

#endif //__COMMON_H__
