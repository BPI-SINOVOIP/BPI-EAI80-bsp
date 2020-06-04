
#ifndef _RPC_INTERNAL_H_
#define _RPC_INTERNAL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis/types.h>


#define RPC_MAX_RESOURCE_NUM 5

/* rpc internal package for slave response */
typedef struct
{
    uint8_t resourceId;         /* rpc resource id from slave*/
    RET_CODE result;    /* rpc resource result from slave */
}RPC_Resp_Data_T;


#ifdef __cplusplus
}
#endif

#endif /* _RPC_INTERNAL_H_ */

















