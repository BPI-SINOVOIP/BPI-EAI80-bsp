/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_rpc.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2018/04/17
 *
 * @brief         Header file of rpc driver.
 *
 * @note
 *    2018/04/17, Mike.Zheng, V1.0.0
 *        Initial version.
 */
#ifndef _GM_HAL_RPC_H_
#define _GM_HAL_RPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gm_hal_cpu_dev.h>

/*Declaration of RPC handle structure*/
struct RPC_Handle;

typedef void (*RPC_Callback_T)(struct RPC_Handle *hPRC, uint8_t event, ulong_t param);


typedef enum
{
    RPC_EVENT_TX_GENERATE                = 0x0,

} RPC_Event_T;


/**
 * General RPC configuration structure.
 */
typedef struct
{
    RPC_Callback_T callback;
} PRC_Config_T;


typedef enum
{
    RPC_MASTER_ID                        = 0x0,
    RPC_SLAVE_ID
} RPC_CPUId_T;


typedef enum
{
    RPC_IDLE                             = 0x0,
    RPC_BUSY                                  ,
    RPC_FINISH
} RPC_Status_T;


typedef struct RPC_Handle
{
    CPU_Device_T   *device;           /**< Pointer to CPU device handle */
    RPC_CPUId_T    id;                /**< CPU ID */
    RPC_Status_T   status;            /**< RPC Communicat Status */
    RPC_Callback_T callback;          /**< Callback definition to notify user in interrupt handler */
}RPC_Handle_T;

static inline void HAL_RPC_SendCmd()
{
    /* make sure all data transactions complete before next instruction is executed*/
    __DSB();

    /* trigger the remote processor*/
    __SEV();
}


void  HAL_RPC_MasterBootSlave();
RET_CODE HAL_RPC_WaitRespon(RPC_Handle_T *hRPC,uint32_t timeOutMs);
RET_CODE HAL_RPC_Init(RPC_Handle_T *hRPC,  CPU_Device_T *pRPC, PRC_Config_T *pConfig);
void HAL_RPC_SetPack(RPC_Handle_T *hRPC,uint32_t pack);
uint32_t HAL_RPC_GetPack(RPC_Handle_T *hRPC);


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_RPC_H_ */



