/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file        sysinit.c
 *
 * @author      wen.liu
 *
 * @version     1.0.0
 *
 * @date        2015/08/21
 *
 * @brief       System init routine.
 *
 * @note
 *    2016/05/17, wen.liu, V1.0.0
 *        Initial version.
 *
 *
 */

#if 0
    #include <kernel.h>
    #include <device.h>
    #include <init.h>
    #include <soc.h>
    #include <gm_hal_rpc.h>
#endif

/**
 * @brief System initialization function
 *
 * @retval None
 *
 * it is similar to start function
 */
void SystemInit(void)
{
    /*boot slave cpu*/
#ifdef CONFIG_GM_HAL_RPC_MASTER
    HAL_RPC_MasterBootSlave();
#endif

}







