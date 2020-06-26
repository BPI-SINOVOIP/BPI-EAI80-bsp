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
#include "zlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc/util.h>
#include <gm_common.h>






#define DATA_SIZE    1024*512
static  char srcdata[DATA_SIZE];


#define CHECK_ERR(err, msg) { \
        if (err != Z_OK) { \
            printf("%s ##########################error: %d###############################################\n", msg, err); \
        } \
    }

static int arraycmp(uint8_t *src, uint8_t *dst, uint32_t size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (src[i] != dst[i])
        {
            return -(i + 1);
        }
    }

    return 0;
}


void zlib_test(compr, comprLen, uncompr, uncomprLen)
Byte *compr, *uncompr;
uLong comprLen, uncomprLen;
{
    int err;
    uLong len = (uLong)sizeof(srcdata);
    memset(uncompr, 0xff, len);
    memset(compr, 0xff, len);

    printf("data size Len = %ld\n", len);
    err = compress2(compr, &comprLen, (const Bytef *)srcdata, len, 8);
    CHECK_ERR(err, "compress");
    printf("after compress len is: %ld\n", comprLen);

    strcpy((char *)uncompr, "garbage");
    err = uncompress(uncompr, &uncomprLen, compr, comprLen);
    CHECK_ERR(err, "uncompress");
    printf("after uncompress len is: %ld\n", uncomprLen);

    if (strcmp((char *)uncompr, srcdata))
    {
        fprintf(stderr, "############################uncompress faild####################################\n");
    }
    else
    {
        printf("##############################unpress OK!###############################################\n");
    }
}



int main(void)
{
    Byte *compr, *uncompr;
    uLong comprLen = (uLong)sizeof(srcdata);
    uLong uncomprLen = comprLen;
    uint32_t  i = 0;
    uint8_t val = 0;
    int       n = 1;

    while (1)
    {
        compr    = (Byte *)malloc_ext(0, (uInt)comprLen);
        uncompr  = (Byte *)malloc_ext(0, (uInt)uncomprLen);

        if (compr == Z_NULL || uncompr == Z_NULL)
        {
            printf("####################malloc memory failed!#########################\n");
            while (1);
        }


        for (i = 0; i < DATA_SIZE; i++)
        {
            if (val > n * 13)
            {
                val = 0;
            }

            srcdata[i] = val;
            val++;
        }
        zlib_test(compr, comprLen, uncompr, uncomprLen);
        memset(srcdata, 0x00, DATA_SIZE);
        val = 0;
        n++;
        if (n > 14)
        {
            n = 0;
        }
        free_ext(0, compr);
        free_ext(0, uncompr);

        k_sleep(1300);
    }

}

