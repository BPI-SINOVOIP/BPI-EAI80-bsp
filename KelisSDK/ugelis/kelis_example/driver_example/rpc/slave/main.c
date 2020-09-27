/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>

#include <gm_hal_rpc.h>

#define SLAVE_RPC_DEBUG 1
#define SALVE_RPC_ERR 1

#ifdef SLAVE_RPC_DEBUG
    #define slave_rpc_prdebug       printk
#else
    #define slave_rpc_prdebug(...)  do{}while(0);
#endif

#ifdef SALVE_RPC_ERR
    #define slave_rpc_prerr        printk
#else
    #define slave_rpc_prerr(...)  do{}while(0);
#endif


typedef struct
{
    uint8_t *pData ;
    uint8_t  length;
} RPC_PACKAGE_T;


#define SYAC_TIME       1000// 1S
#define TEST_DATA_SIZE  100
#define WAIT_CMD_TIMEOUT  2000 //2s


static RPC_Handle_T s_handle;

#define SLAVE_CHECK_FIRST_DATA_VALUE  1
#define SLAVE_SEND_FIRST_DATA_VALUE   0


static RET_CODE slave_check_receive_master_result(RPC_PACKAGE_T *pack)
{
    /*slave receive master mesg [111][333][555]*/
    static uint8_t check_data= SLAVE_CHECK_FIRST_DATA_VALUE;
    uint32_t j = 0;
    uint8_t *pBuf = pack->pData;
    for(j=0; j<pack->length; j++){
         if((pBuf[j] != check_data)||(pack->length != TEST_DATA_SIZE)){
            slave_rpc_prerr("pBuf[%d]:%d\n",j,pBuf[j]);
            return RET_ERROR;
        }
    }
    check_data += 2;

    return RET_OK;
}

static void slave_update_send_pack(RPC_PACKAGE_T *pack)
{
    /*master send [000][222][444]*/
    static uint8_t send_data = SLAVE_SEND_FIRST_DATA_VALUE;
    memset(pack->pData,send_data,pack->length);

    send_data += 2;
}

 void slave_rpc_callback(RPC_Handle_T*hPRC, uint8_t event, ulong_t param)
 {

    static int cnt = 0;
    RET_CODE ret = RET_OK;
    RPC_PACKAGE_T *pack = (RPC_PACKAGE_T *)param;
    if(event == RPC_EVENT_TX_GENERATE){
        slave_rpc_prdebug("slave_rpc_callback: cmd: master---->slave\n");
    }
    /*read reset and check:only for test ,irq should not do so much things*/
    ret = slave_check_receive_master_result(pack);
    if(ret != RET_OK){
        slave_rpc_prerr("ERR:slave_check_receive_master_result\n");
        do{k_busy_wait(100);}while(1);
    }else{
        slave_rpc_prdebug("Success:%d slave_check_receive_master_result\n",++cnt);
    }
 }


/*
 * slave get master data [1111..][3333...][55555...][777777...] and check result
 * slave update data to [000...][2222...][44444...][666666...] and then send to master
 */
int main(void)
{
    RET_CODE ret = RET_OK;
    RPC_PACKAGE_T *pack = NULL;
    uint8_t *ptestdata = NULL;
    PRC_Config_T config;
    static RPC_Handle_T *phandle = &s_handle;

    k_sleep(100);

    slave_rpc_prdebug("slave test .........\n");

    config.callback = slave_rpc_callback;
    ret = HAL_RPC_Init(phandle,CPU_DEV, &config);
    if(ret != RET_OK){
        slave_rpc_prerr("Err:HAL_RPC_Init\n");
        return ret;
    }

    pack = k_malloc(sizeof(RPC_PACKAGE_T));
    if(!pack){
        slave_rpc_prerr("Err:slave k_malloc RPC_PACKAGE_T\n");
        return -EFAULT;
    }

    ptestdata =  k_malloc(TEST_DATA_SIZE);
    if(!ptestdata){
        slave_rpc_prerr("Err:slave k_malloc ptestdata\n");
        return -EFAULT;
    }
    memset(ptestdata,SLAVE_SEND_FIRST_DATA_VALUE,TEST_DATA_SIZE);

    /*prepare pack*/
    pack->pData  = ptestdata;
    pack->length = TEST_DATA_SIZE;

    while(1){

        slave_update_send_pack(pack);
        slave_rpc_prdebug("\nslave send cmd .........\n");
        /*slave prepare pack*/
        HAL_RPC_SetPack(phandle,(uint32_t)(pack));
        /*send cmd*/
        HAL_RPC_SendCmd();
        slave_rpc_prdebug("slave wait cmd .........\n");
        /*wait master respone*/
        ret = HAL_RPC_WaitRespon(phandle,TIMEOUT_WAIT_FOREVER);
        if(ret){
            slave_rpc_prerr("!! timeout:slvae wait master cmd\n");
            return ret;
        }
    }

    return 0;
}
