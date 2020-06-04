/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>

#include <gm_hal_rpc.h>

#define MASTER_RPC_DEBUG 1
#define MASTER_RPC_ERR 1


#ifdef MASTER_RPC_DEBUG
    #define master_rpc_prdebug       printk
#else
    #define master_rpc_prdebug(...)  do{}while(0);
#endif

#ifdef MASTER_RPC_ERR
    #define master_rpc_prerr        printk
#else
    #define master_rpc_prerr(...)  do{}while(0);
#endif


typedef struct
{
    uint8_t *pData ;
    uint8_t  length;
} RPC_PACKAGE_T;


#define SYAC_TIME       1000// 1S
#define TEST_DATA_SIZE  100




static RPC_Handle_T s_handle;
static uint8_t *s_data = NULL;

#define MASTER_CHECK_FIRST_DATA_VALUE  0
#define MASTER_SEND_FIRST_DATA_VALUE   1


static RET_CODE master_check_receive_slave_result(RPC_PACKAGE_T *pack)
{
    /*master receive slave mesg [000][222][444]*/
    static uint8_t check_data= MASTER_CHECK_FIRST_DATA_VALUE;
    uint32_t j = 0;
    uint8_t *buf = pack->pData;

    for(j=0; j<pack->length; j++){
         if((buf[j] != check_data)||(pack->length != TEST_DATA_SIZE)){
            master_rpc_prerr("pBuf[%d]:%d\n",j,buf[j]);
            return RET_ERROR;
        }
    }
    check_data += 2;

    return RET_OK;
}

static void master_update_send_pack(RPC_PACKAGE_T *pack)
{
    /*master send [111][333][555]*/
    static uint8_t send_data = MASTER_SEND_FIRST_DATA_VALUE;
    memset(pack->pData,send_data,pack->length);
    send_data += 2;
}


 void master_rpc_callback(RPC_Handle_T*hPRC, uint8_t event, ulong_t param)
 {
    RET_CODE ret = RET_OK;
    RPC_PACKAGE_T *pack = (RPC_PACKAGE_T *)param;

    if(event == RPC_EVENT_TX_GENERATE){
        s_data = pack->pData;
        master_rpc_prdebug("master_rpc_callback: cmd: slave---->master\n");
    }
    /*read reset and check:only for test ,irq should not do so much things*/
    ret = master_check_receive_slave_result(pack);
    if(ret != RET_OK){
        master_rpc_prerr("Err:master_check_receive_slave_result\n");
    }else{
        master_rpc_prdebug("Success:master_check_receive_slave_result\n");
    }
 }


/*
 * master get slave data [0000...][2222...][44444...][666666...] and check result
 * master update data to [1111...][3333...][55555...][777777...] and then send to slave
 */



int main(void)
{
    RET_CODE ret = RET_OK;
    PRC_Config_T config;


    RPC_PACKAGE_T *pack = NULL;
    static RPC_Handle_T *phandle = &s_handle;

    config.callback = master_rpc_callback;
    ret = HAL_RPC_Init(phandle,CPU_DEV, &config);
    if(ret != RET_OK){
        master_rpc_prerr("Err:HAL_RPC_Init\n");
        return ret;
    }

    pack = k_malloc(sizeof(RPC_PACKAGE_T));
    if(!pack){
        master_rpc_prerr("Err:k_malloc RPC_PACKAGE_T\n");
        return -EFAULT;
    }


    master_rpc_prdebug("master wait slave bootup send cmd\n");

    /*wait slave request*/

    master_rpc_prdebug("###########master wait slave send 1st packet##############\n");
    ret = HAL_RPC_WaitRespon(phandle,TIMEOUT_WAIT_FOREVER);

    if(ret){
        master_rpc_prerr("wait slave cmd timeout\n");
        return ret;
    }

    /*prepare pack*/
    pack->pData  = s_data;
    pack->length = TEST_DATA_SIZE;

    while(1){
        master_rpc_prdebug("\n");
        master_update_send_pack(pack);
        /*master prepare pack*/
        HAL_RPC_SetPack(phandle,(uint32_t)(pack));
        master_rpc_prdebug("master send cmd\n");
        /*send cmd*/
        HAL_RPC_SendCmd();
        /*wait slave respone*/
        master_rpc_prdebug("master wait slave respone\n");
        ret = HAL_RPC_WaitRespon(phandle,TIMEOUT_WAIT_FOREVER);
        if(ret){
            master_rpc_prerr("!! timeout:master wait slave cmd\n");
            return ret;
        }
    }

    return 0;
}
