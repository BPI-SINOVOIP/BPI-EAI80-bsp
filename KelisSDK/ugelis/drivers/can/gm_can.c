#include <string.h>
#include <errno.h>
#include <board.h>
#include <kernel.h>
#include <arch/cpu.h>
#include <misc/__assert.h>
#include <misc/printk.h>
#include <init.h>
#include <uart.h>
#include "gm_can.h"



#define CAN_DEBUG     1
#define CAN_ERR       1

#define DEBUG()  __asm("BKPT")


#ifndef CAN_DEBUG
    #define can_debug(...)      do {} while (0)
#else
    #define can_debug           printk
#endif

#ifndef CAN_ERR
    #define can_err(...)        do {} while (0)
#else
    #define can_err             printk
#endif



static uint32_t s_filterCode[MAX_FILTER_NUM] = {
    0x0fffffff, 0x17ffffff, 0x1bffffff, 0x1dffffff,
    0x1effffff, 0x1f7fffff, 0x1fbfffff, 0x1fefffff,
    0x1ff7ffff, 0x1ffbffff, 0x1ffdffff, 0x1ffeffff,
    0x1fff7fff, 0x1fffbfff, 0x1fffdfff, 0xabcddecb
};
static uint32_t s_filterMask[MAX_FILTER_NUM] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};




static  int gm_can_send(struct device *dev, struct can_msg *msg, s32_t timeout);
static int gm_can_get_msg(struct device *dev,struct can_msg *msg, s32_t timeout);
static int gm_can_set_callback(struct device *dev, can_rx_tx_cb_t cb);

static int gm_can_init(struct device *dev);
static int gm_can_hal_init(struct device *dev);
static void gm_can_isr(uint32_t param);
static int gm_can_irq_config(struct device *dev);
static void gm_can_hal_filterinit(CAN_FilterConfig_T *pFilter);
static void gm_can_hal_callback(CAN_Handle_T *hCAN, CAN_Event_T event, unsigned long param);




static const struct can_driver_api can_driver_api_funcs = {
    .can_send              = gm_can_send,
    .can_get_msg           = gm_can_get_msg,
    .can_set_callback      = gm_can_set_callback,  
};

static CAN_Handle_T can_handle0;



static  struct can_gm_data_t can_data0 ={
    .can_handle         = &can_handle0,
    .cb                 = NULL,
};


static void gm_can_hal_callback(CAN_Handle_T *hCAN, CAN_Event_T event, unsigned long param)

{
	can_data0.cb(event,param);
}

static const struct can_gm_config_t can_config0 = {
    .base                                  = (u32_t)CAN0_BASE_ADDRESS,
	.config.callback                       = gm_can_hal_callback,
    .config.errWarnThreshold               = CONFIG_CAN0_ERRWARNTHRESHOLD,
    .config.workMode                       = CONFIG_CAN0_WORKMODE,
    .config.baudRateAttribute.prescaler    = 48000000,
    .config.baudRateAttribute.baudRate     = CONFIG_CAN0_BAUDRATE,        /* 20kbps */
    .config.baudRateAttribute.sample       = CONFIG_CAN0_SAMPLE,
    .config.baudRateAttribute.sjw          = CONFIG_CAN0_SJW,
    .config.baudRateAttribute.bs1          = CONFIG_CAN0_BS1,
    .config.baudRateAttribute.bs2          = CONFIG_CAN0_BS2,
    
};




DEVICE_DEFINE(gm_can, DEV_CAN0_NAME, gm_can_init, NULL, \
              &can_data0, &can_config0, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  &can_driver_api_funcs) ;





static int gm_can_init(struct device *dev)
{
    int ret = 0;
    /* mutex for mul task acess hardware*/
	can_claim_init(dev);

    ret = gm_can_irq_config(dev);
    if(ret){
        can_err("%s:gm_can_irq_config err:%d\n",__FUNCTION__,ret);
        goto out;
    }

    ret = gm_can_hal_init(dev);
    if(!ret){
        can_debug("can init ok\n");
    }else{
        can_debug("can init fail\n");
    }

out:
    return ret;
}



static int gm_can_hal_init(struct device *dev)
{
    int ret = 0;
    struct can_gm_data_t *data = DEV_DATA(dev);
    CAN_Handle_T *hCAN = data->can_handle;
    
    gm_can_hal_filterinit(&(DEV_CFG(dev)->config.filter));
    ret = HAL_CAN_Init((CAN_Device_T *)(DEV_STRUCT(dev)),&(DEV_CFG(dev)->config),hCAN);
    if(ret){
        can_err("Err:%d HAL_CAN_Init\n",ret);
        goto out;
    }
    
    can_debug("CAN Init OK\n");

out:

    return ret;
}


static void gm_can_hal_filterinit(CAN_FilterConfig_T *pFilter)
{
    uint8_t i= 0 ;

    for (i = 0; i < MAX_FILTER_NUM; i++)
    {
        pFilter->filterCode[i] = s_filterCode[i];
        pFilter->filterMask[i] = s_filterMask[i];

    }
}



static int gm_can_get_msg(struct device *dev,struct can_msg *msg,s32_t timeout)
{
	int ret = 0;
	int i = 0;
    struct can_gm_data_t *data = DEV_DATA(dev);
    CAN_Handle_T *hCAN  = data->can_handle;
    CAN_FrameInfor_T *pRxFrameInfor = NULL;
    ret = HAL_CAN_GetMessage(hCAN,&pRxFrameInfor,timeout);
    msg->id   = pRxFrameInfor->id;
    for(i=0;i<CAN_MAX_DLC;i++)
    {
        msg->data[i] = pRxFrameInfor->data[i]; 
    }                              
    msg->dlc  = pRxFrameInfor->dataLength;        
    msg->r0   = pRxFrameInfor->r0;                
    msg->r1   = pRxFrameInfor->r1;              
    msg->info = pRxFrameInfor->frameFormat;
	msg->type = pRxFrameInfor->frameType;
    return ret;
}

static  int gm_can_send(struct device *dev, struct can_msg *msg,s32_t timeout)
{
    int ret = 0;
	int i=0;
    CAN_FrameInfor_T txFrameInfor; 
    
    struct can_gm_data_t *data = DEV_DATA(dev);
    CAN_Handle_T *hCAN  = data->can_handle;
    
    txFrameInfor.dataLength  = msg->dlc;
    txFrameInfor.frameFormat = msg->info;
    txFrameInfor.id          = msg->id;
	txFrameInfor.frameType   = msg->type;
    for(i=0;i<8;i++)
    {
    	txFrameInfor.data[i]=msg->data[i];
    }
   
    can_claim_host(dev);

    ret = HAL_CAN_TransferMessage(hCAN, &txFrameInfor,timeout);
    if(ret != 0) 
    {
         can_err("Err:%s\n",__FUNCTION__);
         can_release_host(dev);
         return ret;        
    }

    can_release_host(dev);
    return ret;
}


static  int gm_can_set_callback(struct device *dev, can_rx_tx_cb_t cb)
{
    struct can_gm_data_t * data = DEV_DATA(dev);

    data->cb = cb;

    return 0;
}


static void gm_can_isr(uint32_t param)
{
    uint8_t  index = 0;

	
    struct device *dev = (struct device *)param;
    if(DEV_STRUCT(dev)== CAN0_DEV){
        index = 0;
    }

    HAL_CAN_IrqHandler();
}

static int gm_can_irq_config(struct device *dev)
{
    int ret = 0;

    if(DEV_STRUCT(dev)== CAN0_DEV){
        IRQ_CONNECT(CAN0_IRQn, CAN0_IRQ_PRIORITY, gm_can_isr, DEVICE_GET(gm_can), 0);
        irq_enable(CAN0_IRQn);
    }else{
        ret = - ENXIO;
    }
    return ret;
}










