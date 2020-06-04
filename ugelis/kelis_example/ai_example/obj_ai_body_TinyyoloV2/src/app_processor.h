/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        app_processor.h
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/10/15
 *
 * @brief       app_processor source file
 *
 * @note
 *              2019/10/15, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processor.h"
#include <stdint.h>
#include <gm_common.h>
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include <board.h>
#include "ai_common.h"

//#define OBJ_PRINTF printk
#define OBJ_PRINTF(...)  do{} while(0);
//#define OBJ_DBG_PRINTF printf
#define OBJ_DBG_PRINTF do{} while(0);

struct Infrared_led_mgr
{
    uint8_t dark_cnt;
    uint8_t normal_cnt;
    uint8_t onoff;
    uint8_t *gray_pic;
};

void *malloc_ext(uint8_t memx, uint32_t size);
void free_ext(uint8_t memx, void *ptr);

void rgb_split2rrggbb(uint8_t *src, uint8_t *dst, int w, int h);
void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h);
