#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <gm_hal_cache.h>
#include <gm_hal_rpc.h>
#include <rpc_core.h>
#include "rpc_internal.h"

//#define MASTER_RPC_DEBUG 1

#ifdef MASTER_RPC_DEBUG
    #define MasterRpcDebug       printk
#else
    #define MasterRpcDebug(...)  do{}while(0);
#endif


typedef struct
{
    uint8_t busy;         /* 0:free, 1:busy */
    uint8_t interrupted;   /* 0:not got callback, 1:got callback */
    RET_CODE result;      /* result */ 
}RPC_Resource_Status_T;

static volatile RPC_Resource_Status_T gResourceStatus[RPC_MAX_RESOURCE_NUM];
static RPC_Handle_T gRpcHandle;
static struct k_sem gMasterSem;

static RET_CODE RPC_RequestResourceID(uint8_t *resourceId)
{
    uint8_t id = 0;
    uint8_t found = 0;

    k_sem_take(&gMasterSem, K_FOREVER);
    for(id=0; id<RPC_MAX_RESOURCE_NUM; id++)
    {
        if(gResourceStatus[id].busy == 0)
        {
            found = 1;
            break;
        }
    }
    if(!found)
    {
        k_sem_give(&gMasterSem);
        return RET_BUSY;
    }
    gResourceStatus[id].busy = 1;    
    k_sem_give(&gMasterSem);
    
    gResourceStatus[id].interrupted = 0;
    gResourceStatus[id].result = RET_ERROR;
    *resourceId = id;
    
    
    return RET_OK;
}

static void RPC_MasterCallback(RPC_Handle_T*hPRC, uint8_t event, ulong_t param)
{
    if(event == RPC_EVENT_TX_GENERATE)
    {
        HAL_DCACHE_InvalidAll();
        RPC_Resp_Data_T response = *(RPC_Resp_Data_T *)param;
        gResourceStatus[response.resourceId].interrupted = 1;
        gResourceStatus[response.resourceId].result = response.result;
    }

    return;
}

RET_CODE RPC_MasterSendPack(RPC_Package_T *pack)
{
    RET_CODE ret = RET_OK;
    RPC_Handle_T *pHandle = &gRpcHandle;
    
    uint8_t resourceId = 0;

    /* request slave id */
    ret = RPC_RequestResourceID(&resourceId);
    if(ret)
    {
        MasterRpcDebug("!! request resource id fail\n");
        return RET_BUSY;
    }
    pack->resourceId = resourceId;
    pack->result = RET_BUSY;
    
    HAL_DCACHE_Flush();
    /*master prepare pack*/
    HAL_RPC_SetPack(pHandle,(uint32_t)(pack));
    /*send cmd*/
    HAL_RPC_SendCmd();

    return RET_OK;
}

RET_CODE RPC_MasterWaitResp(RPC_Package_T *pack, uint32_t timeOutMs)
{
    RET_CODE ret = RET_OK;
    RPC_Handle_T *pHandle = &gRpcHandle;
    uint8_t resourceId = pack->resourceId;
    
    if(timeOutMs == TIMEOUT_WAIT_FOREVER)
    {
        /*wait thread interrypt come*/
        while(gResourceStatus[resourceId].interrupted != 1)
        {
            HAL_DelayMs(1);
        }
    }
    else if(timeOutMs == TIMEOUT_NO_WAIT)
    {
        MasterRpcDebug("!! RPC_MasterWaitResp not support TIMEOUT_NO_WAIT yet\n");
    }
    else
    {
        while((--timeOutMs)&&(gResourceStatus[resourceId].interrupted != 1))
        {
            HAL_DelayMs(1);
        }
        if(timeOutMs<=0)
        {
           ret = RET_TIMEOUT;
           MasterRpcDebug("!! sobel_slave_wait timeout\n");
        }
    }
    
    if(ret != RET_OK)
    {
        pack->result =  ret;
    }
    else
    {
        pack->result = gResourceStatus[resourceId].result;
    }
    unsigned int key = irq_lock();
    gResourceStatus[resourceId].busy = 0;
    irq_unlock(key);

    return ret;
}

static RET_CODE RPC_MasterInit(struct device *dev)
{
    RET_CODE ret = RET_OK;
    PRC_Config_T config;
    RPC_Handle_T *pHandle = &gRpcHandle;
        
    k_sem_init(&gMasterSem, 1, 1);
            
    config.callback = RPC_MasterCallback;
    ret = HAL_RPC_Init(pHandle, CPU_DEV, &config);
    if(ret != RET_OK)
    {
        MasterRpcDebug("Err:HAL_RPC_Init\n");
        return ret;
    }

    return RET_OK;
}

SYS_INIT(RPC_MasterInit, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);


