#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <gm_hal_rpc.h>
#include <rpc_core.h>
#include "rpc_internal.h"


//#define SLAVE_RPC_DEBUG 1

#ifdef SLAVE_RPC_DEBUG
    #define SlaveRpcDebug       printk
#else
    #define SlaveRpcDebug(...)  do{}while(0);
#endif


#define STACK_SIZE    (2048)

typedef struct
{
    struct k_thread thread;
    k_tid_t tid;
    uint32_t stack[STACK_SIZE/4];
    struct k_sem sem;
    RPC_Package_T pack;
    RPC_Resp_Data_T response;
}RPC_Slave_Resource_T;


static RPC_Handle_T gRpcHandle;
static RPC_Sub_Func_T gSlaveFuncList[RPC_CMD_MAX] = {NULL};
static RPC_Slave_Resource_T gSlaveResource[RPC_MAX_RESOURCE_NUM];


static void RPC_SlaveThreadFunc(void *p1, void *p2, void *p3)
{
    uint8_t resourceId = *(uint8_t*)p1;

    static RPC_Handle_T *pHandle = &gRpcHandle;
    RPC_Cmd_T cmd;
    RET_CODE ret = RET_ERROR;
    RPC_Package_T *pack = NULL;

    while(1)
    {
        k_sem_take(&gSlaveResource[resourceId].sem, K_FOREVER);

        SlaveTrace("[%s(%d)]\n", __FUNCTION__, __LINE__);
        pack = &gSlaveResource[resourceId].pack;
        cmd = pack->cmd;
        if(gSlaveFuncList[cmd] != NULL)
        {
            ret = gSlaveFuncList[cmd](pack);
        }

        gSlaveResource[resourceId].response.resourceId = resourceId;
        gSlaveResource[resourceId].response.result = ret;
        HAL_DCACHE_Flush();
        HAL_RPC_SetPack(pHandle,(uint32_t)(&gSlaveResource[resourceId].response));
        HAL_RPC_SendCmd();
        SlaveTrace("[%s(%d)]\n", __FUNCTION__, __LINE__);
    }

    return;
}

static void RPC_SlaveCallback(RPC_Handle_T*hPRC, uint8_t event, ulong_t param)
{
    uint8_t resourceId = 0;
    uint8_t *stack = NULL;

    if(event == RPC_EVENT_TX_GENERATE)
    {
        HAL_DCACHE_InvalidAll();
        SlaveRpcDebug("RPC_SlaveCallback: master---->slave\n");
    }
    //HAL_DCACHE_InvalidAll();
    RPC_Package_T *pack = (RPC_Package_T *)param;
    resourceId = pack->resourceId;
    SlaveTrace("[%s(%d)] resourceId=%d\n", __FUNCTION__, __LINE__, resourceId);
    memcpy(&gSlaveResource[resourceId].pack, pack, sizeof(RPC_Package_T));

    if(gSlaveResource[resourceId].tid == NULL)
    {
        gSlaveResource[resourceId].tid = k_thread_create(&gSlaveResource[resourceId].thread, (k_thread_stack_t *)gSlaveResource[resourceId].stack,
                    STACK_SIZE,
                    (k_thread_entry_t)RPC_SlaveThreadFunc, &resourceId, NULL, NULL,
                    15, 0, K_NO_WAIT/*K_FOREVER*/);
    }

    k_sem_give(&gSlaveResource[resourceId].sem);

    return;
}

RET_CODE RPC_SlaveRegisterCmd(RPC_Cmd_T cmd, RPC_Sub_Func_T func)
{
    if(gSlaveFuncList[cmd] != NULL)
    {
        SlaveRpcDebug("Err: RPC_SlaveRegisterCmd\n");
		return RET_ERROR;
	}
    gSlaveFuncList[cmd] = func;
    SlaveTrace("[%s(%d)] done\n", __FUNCTION__, __LINE__);
    return RET_OK;
}

static RET_CODE RPC_SlaveInit(struct device *dev)
{
    RET_CODE ret = RET_OK;
    PRC_Config_T config;
    RPC_Handle_T *pHandle = &gRpcHandle;
    int i = 0;

    /* init slave resource */
    for(i=0; i<RPC_MAX_RESOURCE_NUM; i++)
    {
        k_sem_init(&gSlaveResource[i].sem, 0, 1);
        gSlaveResource[i].tid = NULL;
    }

    config.callback = RPC_SlaveCallback;
    ret = HAL_RPC_Init(pHandle, CPU_DEV, &config);
    if(ret != RET_OK)
    {
        SlaveRpcDebug("Err:HAL_RPC_Init\n");
        return ret;
    }

    SlaveTrace("[%s(%d)] done\n", __FUNCTION__, __LINE__);

    return RET_OK;
}

SYS_INIT(RPC_SlaveInit, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);


