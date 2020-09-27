
#ifndef _RPC_IMP_H_
#define _RPC_IMP_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <ugelis/types.h>


typedef enum
{
    IMP_CMD_SOBEL,
    IMP_CMD_FILTER,
    IMP_CMD_BINARY,
    IMP_CMD_QR,
    IMP_CMD_MAX,
}IMP_Cmd_T;

typedef struct
{
    IMP_Cmd_T impCmd;
    uint8_t *pSrc;
    uint8_t *pDst;
    uint32_t width;
    uint32_t height;
}IMP_Data_T;

#define DEV_RPC_IMP_NAME			"RPC_IMP_DEV"

#ifdef __cplusplus
}
#endif

#endif /* _RPC_IMP_H_ */

















