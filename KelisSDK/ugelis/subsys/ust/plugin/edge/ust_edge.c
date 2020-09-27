#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <imlib.h>

#include <ust_core.h>
#include <ust_edge.h>

#include <rpc_core.h>
#include <rpc_plugin_imp.h>


//#define UST_EDGE_DEBUG 1
#ifdef UST_EDGE_DEBUG
    #define ust_edge_pr_debug       printk
#else
    #define ust_edge_pr_debug(...)  do{}while(0)
#endif


#define UST_EDGE_RPC 1


#define DEV_UST_EDGE_NAME  	               "ust_edge_dev"
#define UST_EDGE_QUEUE_MAX_OBJECT_NUM       5
#define UST_EDGE_DATA_SIZE                  17


static int ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY)
    {
        ust_edge_pr_debug("[ust_edge][%s]: Plugin Ready\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_PALY)
    {
        ust_edge_pr_debug("[ust_edge][%s]: Plugin Play\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_STOP)
	{
        ust_edge_pr_debug("[ust_edge][%s]: Plugin Stop\n", __FUNCTION__);
	}

    return ret;

}


static int ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    int ret = 0;
    uint8_t *data = NULL;
    UST_Pad_T *currentSrcPad = NULL;
    UST_BufDesc_T *currentBufdesc = NULL;
    uint8_t *image_data = bufDesc->data;
    uint32_t image_width = *(uint32_t *)bufDesc->prv;
    uint32_t image_height = bufDesc->length / image_width;

    ust_edge_pr_debug("[ust_edge][%s] BufDes[in]:%p \n", __FUNCTION__, bufDesc);

    /*alloc current bufdes*/
    currentBufdesc = UST_AllocBufDes();
    if(!currentBufdesc)
    {
        ret = - ENOMEM;
        goto out;
    }

    /*copy pre element data(+1) to cuurent element */
    data = malloc(bufDesc->length);
    if(!data)
    {
        ret = -ENOMEM;
        return ret;
    }

    /*prepare current currentBufdesc*/
    currentBufdesc->data   = data;
    currentBufdesc->length = image_width*image_height;
    currentBufdesc->type   = UST_DATA_FROM_MALLOC;
    currentBufdesc->prv    = &image_width;
    currentSrcPad = element->srcPadManage.padTable[0];

    /* sobel */
    //uint32_t time_start = k_uptime_get_32();
    #ifdef UST_EDGE_RPC
    RPC_Package_T pack;
    IMP_Data_T imp_data;
    /* prepare private data */
    imp_data.impCmd = IMP_CMD_SOBEL;
    imp_data.pSrc = image_data;
    imp_data.pDst = data;
    imp_data.width = image_width;
    imp_data.height = image_height/2;
    /* prepare rpc pack */
    pack.cmd = RPC_CMD_IMP;
    pack.privData = &imp_data;
    ret = RPC_MasterSendPack(&pack);
    if(ret != RET_OK)
    {
        ust_edge_pr_debug("!!! RPC_MasterSendPack wrong \n");
        UST_FreeBufDes(currentBufdesc);
        ret = -ENOMEM;
        return ret;
    }
    
    imlib_sobel(image_data+(image_height/2)*image_width, image_height/2, image_width, data+(image_height/2)*image_width);
    
    //if(RPC_MasterWaitResp(&pack, TIMEOUT_WAIT_FOREVER) != RET_OK)
    if((RPC_MasterWaitResp(&pack, 2000) != RET_OK) || (pack.result != RET_OK))
    {
        ust_edge_pr_debug("!!! sobel_slave wrong \n");
        UST_FreeBufDes(currentBufdesc);
        ret = -ENOMEM;
        return ret;
    }
    #else
    imlib_sobel(image_data, image_height, image_width, data);
    #endif
    //uint32_t time_end = k_uptime_get_32();
    //printk("[edge cost]: %d ms\n", time_end-time_start);


    /*free pre bufdes*/
    UST_FreeBufDes(bufDesc);

    /*push current bufdesc to perr pad*/
    ust_edge_pr_debug("[ust_edge][%s] BufDes[out]:%p \n", __FUNCTION__, currentBufdesc);
    UST_PadPush (currentSrcPad, currentBufdesc);
    

out:
    return ret;
}



static int ust_prepare_element(struct device *dev)
{
	UST_Element_T *element = NULL;
	UST_Pad_T *srcPad = NULL;
	UST_Pad_T *sinkPad = NULL;
	int ret = 0;

	/*alloc src element and config it*/
    element = UST_AllocElement(ELEMENT_EDGE);
	if(!element){
        ust_edge_pr_debug("[ust_edge][%s]: Err:  UST_AllocElement NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	element->prv = dev;
	UST_RegisterElementWorkStatusFun(element, ust_element_workstatus_fun);

	/*alloc pad and config it*/
	srcPad = UST_AllocPad(ELEMENT_EDGE_PAD_SRC);
	if(!srcPad){
        ust_edge_pr_debug("[ust_edge][%s]: Err:  UST_AllocSrcPad NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	sinkPad = UST_AllocPad(ELEMENT_EDGE_PAD_SINK);
	if(!sinkPad){
        ust_edge_pr_debug("[ust_edge][%s]: Err:  UST_AllocSinkPad NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	/*set sinkpad chian cb*/
	UST_RegisterPadChainFun(sinkPad, ust_sinkpad_chain_fun);

	/*add sink/src pad to element*/
	ret = UST_AddPadToElement(element,UST_SRC_PAD,srcPad);
	if(ret){
        ust_edge_pr_debug("[ust_edge][%s]: Err:  UST_AddSrcPadToElement\n", __FUNCTION__);
        goto out;
	}
	ret = UST_AddPadToElement(element,UST_SINK_PAD,sinkPad);
	if(ret){
        ust_edge_pr_debug("[ust_edge][%s]: Err:  UST_AddSinkPadToElement\n", __FUNCTION__);
        goto out;
	}


out:
	return ret;
}

static int ust_prepare_queue()
{
#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
    /*alloc queue*/
    UST_QueueElement_T *queueElement = NULL;
    int ret = 0;
    queueElement = UST_AllocQueueElement(ELEMENT_EDGE_QUEUE,ELEMENT_EDGE_QUEUE_PAD_SINK,
                                         ELEMENT_EDGE_QUEUE_PAD_SRC, CONFIG_UST_PLUGIN_EDGE_QUEUE_MAX_NUM);

    if(!queueElement){
        ust_edge_pr_debug("[ust_edge][%s] UST_AllocQueueElement NULL\n",__FUNCTION__);
        ret = -ENOMEM;
        goto out;
    }

    /*link queue*/
    UST_Pad_T *srcElementSrcPad = NULL;
    UST_Pad_T *queueElementSinkPad = NULL;
    UST_Element_T *srcElement = NULL;

    srcElement = UST_GetElement(ELEMENT_EDGE);
    if(!srcElement){
        ret = -ENOMEM;
        goto out;
    }
    queueElement = (UST_QueueElement_T *)(UST_GetElement(ELEMENT_EDGE_QUEUE));
    if(!queueElement){
        ret = -ENOMEM;
        goto out;
    }

    srcElementSrcPad = UST_GetPadFromElement(srcElement, UST_SRC_PAD, ELEMENT_EDGE_PAD_SRC);
    if(!srcElementSrcPad){
        ret = -ENOMEM;
        goto out;
    }

    queueElementSinkPad = UST_GetPadFromElement((UST_Element_T *)queueElement, UST_SINK_PAD, ELEMENT_EDGE_QUEUE_PAD_SINK);
    if(!queueElementSinkPad){
        ret = -ENOMEM;
        goto out;
    }

    UST_PadConnect(srcElementSrcPad, queueElementSinkPad);
out:
    return ret;

#else
    return 0;
#endif

}

static int ust_element_init(struct device *dev)
{

	int ret = 0;

    ret = ust_prepare_element(dev);
    if(ret){
        ust_edge_pr_debug("[ust_edge][%s]: Err:  ust_prepare_element\n", __FUNCTION__);
        goto out;
    }

    ret = ust_prepare_queue();
    if(ret){
        ust_edge_pr_debug("[ust_edge][%s] ust_prepare_queue\n",__FUNCTION__);
        goto out;
    }

out:
	return ret;
}




DEVICE_DEFINE(plugin_edge, DEV_UST_EDGE_NAME, ust_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;














