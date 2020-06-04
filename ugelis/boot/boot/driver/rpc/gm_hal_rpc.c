
#include <gm_soc.h>
#include <gm_hal_rpc.h>
#include <gm_irq.h>
#include "gm_ll_rpc.h"

/*only slave can use uart debug for test*/
#ifdef GM_RPC_SLAVE
    #define RPC_DEBUG   1
    #define RPC_ERR     1
#endif

#ifdef RPC_DEBUG
    #define rpc_pr_debug       printf
#else
    #define rpc_pr_debug(...)  do{}while(0);
#endif

#ifdef RPC_ERR
    #define rpc_pr_err         printf
#else
    #define rpc_pr_err(...)    do{}while(0);
#endif


#define SLAVE_IMAGE_ADDR       (50*1024)       /* TODO GET IT LINKSRCIRPT*/
#define VETTOR_SIZE            (90*4)


RPC_Handle_T *ghRPCTable[] = {0};

static RPC_CPUId_T HAL_RPC_GetCPUID();
static void HAL_RPC_IRQHandler(void *param);
static void HAL_RPC_IrqConfig(RPC_CPUId_T id);
static void LL_RPC_MasterCopySlaveVectorTable();

static void LL_RPC_MasterCopySlaveVectorTable()
{
    /*copy image -> specsram*/
    if(HAL_RPC_GetCPUID() == RPC_MASTER_ID){
        memcpy((void *)SLAVE_SRAM_VECTOR_ADDR,(void *)SLAVE_IMAGE_ADDR,VETTOR_SIZE);
    }
}

static RPC_CPUId_T HAL_RPC_GetCPUID()
{
    RPC_CPUId_T  id ;

    if(LL_RPC_GET_CPU_ID() == LL_RPC_MASTER_ID){
        id = RPC_MASTER_ID;
    }else{
        id = RPC_SLAVE_ID;
    }

    return id;

}

static void HAL_RPC_IRQHandler(void *param)
{
    UNUSED(param);

    uint32_t pack = NULL;

    RPC_CPUId_T id = HAL_RPC_GetCPUID();
    RPC_Handle_T *hRPC = ghRPCTable[id];
    CPU_Device_T *pDev = hRPC->device;

    if(id == RPC_MASTER_ID){
        if(LL_RPC_MASTER_IRQ_ST()){
            /*clear master reveive irq*/
            LL_RPC_MASTER_CLEAR_IRQ();
        }else{
            rpc_pr_err("Err:mast cpu enter err irq");
            return ;
        }
    }else{
        if(LL_RPC_GET_SLAVE_IRQ_ST()){
            /*clear slave reveive irq*/
            LL_RPC_SLAVE_CLEAR_IRQ();
        }else{
            rpc_pr_err("Err:slave cpu enter err irq");
            return ;
        }

    }

    pack = LL_RPC_GET_DATA(pDev);

    /*set communicate status finsih */
    hRPC->status = RPC_FINISH;

    if(hRPC->callback){
        hRPC->callback(hRPC,RPC_EVENT_TX_GENERATE,pack);
    }

}

static inline void HAL_RPC_IrqConfig(RPC_CPUId_T id)
{
    if(id == RPC_MASTER_ID){
        HAL_RequestIrq(MASTER_IRQn, MASTER_IRQ_PRIORITY, HAL_RPC_IRQHandler, 0);
        HAL_IRQ_ENABLE(MASTER_IRQn);
    }else{
        HAL_RequestIrq(SLAVE_IRQn, SLAVE_IRQ_PRIORITY, HAL_RPC_IRQHandler, 0);
        HAL_IRQ_ENABLE(SLAVE_IRQn);
    }
}




RET_CODE HAL_RPC_WaitRespon(RPC_Handle_T *hRPC,uint32_t timeOutMs)
{
    RET_CODE ret = RET_OK;

    if(timeOutMs == TIMEOUT_WAIT_FOREVER){
        /*wait commnicate finish*/
        while(hRPC->status != RPC_FINISH){
            HAL_DelayMs(1);

        }
    }else if(timeOutMs == TIMEOUT_NO_WAIT){
        if(hRPC->status != RPC_FINISH){
            return RET_ONGOING;
        }
    }else{
        while((--timeOutMs)&&(hRPC->status != RPC_FINISH)){
            HAL_DelayMs(1);

        }
        if(timeOutMs<=0){
           return  RET_TIMEOUT;
        }
    }

    HAL_IrqLock();
    hRPC->status = RPC_IDLE;

    HAL_IrqUnlock();



    return ret;
}

void  HAL_RPC_MasterBootSlave()
{
    LL_RPC_MasterCopySlaveVectorTable();
    LL_RPC_MASTER_ENABLE_SLAVE_CLK();
    LL_RPC_MASTER_START_SLAVE();
}


RET_CODE HAL_RPC_Init(RPC_Handle_T *hRPC,  CPU_Device_T *pRPC, PRC_Config_T *pConfig)
{
    RET_CODE ret = RET_OK;

    if((!hRPC)||(!pConfig)||(!pRPC))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    hRPC->status = RPC_IDLE;

    hRPC->id = HAL_RPC_GetCPUID();
    /*backup hander*/
    ghRPCTable[hRPC->id] = hRPC;
    hRPC->device = pRPC;
    hRPC->callback = pConfig->callback;
    /*setup irq*/
    HAL_RPC_IrqConfig(hRPC->id);

out:

    return ret;
}

void HAL_RPC_SetPack(RPC_Handle_T *hRPC,uint32_t pack)
{
     LL_RPC_SET_DATA(hRPC->device,pack);
}

uint32_t HAL_RPC_GetPack(RPC_Handle_T *hRPC)
{
    return LL_RPC_GET_DATA(hRPC->device);
}


