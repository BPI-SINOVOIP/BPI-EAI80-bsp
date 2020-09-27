/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       Sample code for adc test case.
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <misc/printk.h>
#include <misc/printk.h>
#include <load_compress_image.h>
#include <gm_hal_sqi.h>
#include <gm_common.h>
#include <gm_soc.h>



/**
 *for test temp
 * you can malloc or set resver mem space for BUF
*/
//#define  BUF (0xc0000000+16*1024*1024 - 7*1024*1024) /*7MB Space*/
#define  FLASH_BIN_OFFSET   0x50000                     /*compress image flash offset*/


//char BUF[1024*1024*7]={0};





#define __malloc(size)          malloc_ext(0,(size))






void main(void)
{
    int ret = 0;
    unsigned int t_now, t_last, usetime;

    *(volatile uint32_t *)(0x1ffff0a8)  = 0xfffffc63;
    *(volatile uint32_t *)(0x1ffff0ac)  = 0xffd8ffff;
    *(volatile uint32_t *)(0x1ffff0b0)  = 0xff3fffff;

    uint8_t *uncompressbuf = __malloc(1024 * 1024 * 7);
    if (!uncompressbuf)
    {
        printk("Err malloc uncompress buf \n");
        return -1;
    }

    t_last = k_uptime_get_32();

    ret = ldimg_flash_compress_image(FLASH_BIN_OFFSET, uncompressbuf);
    if (ret)
    {
        printk("Err load_flash_compress_image\n");
        return ret;
    }
    t_now = k_uptime_get_32();


    printk("time  consume %d ms \n", t_now - t_last);


}


