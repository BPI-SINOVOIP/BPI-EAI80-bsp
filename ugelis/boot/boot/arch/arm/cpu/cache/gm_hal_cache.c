/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gpio.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/07/19
 *
 * @brief        GPIO APIs for application.
 *
 * @note
 *    2016/07/19, mark.lee, V1.0.0
 *        Initial version.
 */

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_hal_cpu.h>
#include "gm_ll_cache.h"


RET_CODE HAL_ICACHE_Enable()
{
    RET_CODE ret = RET_OK;
    int32_t timeout = 1000000;/*1 s*/

    HAL_DisableIrq();
    /*icahce not by pass*/
    LL_ICACHE_SET_BYPASS(CPU_DEV,FALSE);

    LL_ICACHE_ENABLE(CMSDK_ICACHE,FALSE);
    LL_ICACHE_ENABLE(CMSDK_ICACHE,TRUE);

    while(((LL_ICACHE_GET_STATUS(CMSDK_ICACHE) & LL_ICACHE_STATUS_MASK)!=ICACHE_ENABLE)&&(--timeout));
    HAL_EnableIrq();
    if(timeout <= 0){
        ret = RET_TIMEOUT;
    }

    return ret;
}

RET_CODE HAL_ICACHE_Disable()
{
    RET_CODE ret = RET_OK;
    int32_t timeout = 1000000;/*1 s*/

    HAL_DisableIrq();

    LL_ICACHE_ENABLE(CMSDK_ICACHE,FALSE);

    while(((LL_ICACHE_GET_STATUS(CMSDK_ICACHE) & LL_ICACHE_STATUS_MASK)!=ICACHE_DISABLE)&&(--timeout));

    if(timeout<=0){
        ret = RET_TIMEOUT;
        goto out;
    }

    /*icache by pass*/
    LL_ICACHE_SET_BYPASS(CPU_DEV,TRUE);

    HAL_EnableIrq();
out:
    return ret;
}

RET_CODE HAL_ICACHE_Invalid()
{
    return RET_OK;
}


RET_CODE HAL_ICACHE_Flush()
{

    return RET_OK;
}


RET_CODE HAL_DCACHE_Enable()
{
    RET_CODE ret = RET_OK;

    HAL_DisableIrq();
    /*invalid the data line*/
    HAL_DACHE_INVALID_DATA_ALL_LINE();

    /*dcache not by pass*/
    LL_DCACHE_SET_BYPASS(CPU_DEV,FALSE);

    HAL_EnableIrq();

    return ret;
}


RET_CODE HAL_DCACHE_Disable()
{
    RET_CODE ret = RET_OK;

    HAL_DisableIrq();

    HAL_DACHE_INVALID_DATA_ALL_LINE();

    /*dcache by pass*/
    LL_DCACHE_SET_BYPASS(CPU_DEV,TRUE);

    HAL_EnableIrq();

    return ret;
}

RET_CODE HAL_DCACHE_InvalidAll()
{
    RET_CODE ret = RET_OK;

    HAL_DisableIrq();
    HAL_DACHE_INVALID_DATA_ALL_LINE();
    HAL_EnableIrq();

    return ret;
}


RET_CODE HAL_DCACHE_InvalidLine(uint32_t lineNum)
{
    RET_CODE ret = RET_OK;

    register uint32_t i = lineNum;

    HAL_DisableIrq();
    HAL_DACHE_INVALID_DATA_LINE(i);
    HAL_EnableIrq();

    return ret;

}


RET_CODE HAL_DCACHE_Flush()
{
    RET_CODE ret = RET_OK;

    return ret;

}


