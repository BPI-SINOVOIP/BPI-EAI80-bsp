
#ifndef _RPC_CORE_H_
#define _RPC_CORE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis/types.h>
#include <gm_hal_rpc.h>


//#define SLAVE_TRACE 1
#ifdef SLAVE_TRACE
    #define SlaveTrace       printk
#else
    #define SlaveTrace(...)  do{}while(0);
#endif


/* rpc cmd type */
typedef enum
{
    RPC_CMD_IMP,
    RPC_CMD_YOLO,
    RPC_PRE_PROCESSING,
    RPC_CMD_QR,
    RPC_CMD_MAX,
}RPC_Cmd_T;

/* rpc package struct */
typedef struct
{
    RPC_Cmd_T cmd;      /* rpc command */
    void *privData;     /* private data of different rpc plugin */
    RET_CODE result;    /* result of rpc */
    uint8_t resourceId; /* rpc resource id, user needn't care, rpc_core will auto request */
}RPC_Package_T;

#ifdef CONFIG_GM_HAL_RPC_MASTER

/* master functions */
RET_CODE RPC_MasterSendPack(RPC_Package_T *pack);
RET_CODE RPC_MasterWaitResp(RPC_Package_T *pack, uint32_t timeOutMs);
#endif

#ifdef CONFIG_GM_HAL_RPC_SLAVE
/* slave functions */
typedef RET_CODE (*RPC_Sub_Func_T)(RPC_Package_T *);
RET_CODE RPC_SlaveRegisterCmd(RPC_Cmd_T cmd, RPC_Sub_Func_T func);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UST_CORE_H_ */

















