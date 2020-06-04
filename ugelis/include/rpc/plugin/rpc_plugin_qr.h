
#ifndef _RPC_QR_H_
#define _RPC_QR_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <ugelis/types.h>


typedef struct
{
    uint8_t *iscn;
    uint8_t *img;
}QR_Data_T;

#define DEV_RPC_QR_NAME			"RPC_QR_DEV"

#ifdef __cplusplus
}
#endif

#endif /* _RPC_QR_H_ */

















