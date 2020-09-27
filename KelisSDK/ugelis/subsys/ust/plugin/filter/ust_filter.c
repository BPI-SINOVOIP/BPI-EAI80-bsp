#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <imlib.h>

#include <ust_core.h>
#include <ust_filter.h>

#include <rpc_core.h>
#include <rpc_plugin_imp.h>

//#define UST_FILTER_DEBUG 1
#ifdef UST_FILTER_DEBUG
    #define ust_filter_pr_debug       printk
#else
    #define ust_filter_pr_debug(...)  do{}while(0)
#endif

//#define UST_FILTER_RPC


#define DEV_UST_FILTER_NAME  	               "ust_filter_dev"
#define UST_FILTER_QUEUE_MAX_OBJECT_NUM       5
#define UST_FILTER_DATA_SIZE                  17



static int ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY)
    {
        ust_filter_pr_debug("[ust_filter][%s]: Plugin Ready\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_PALY)
    {
        ust_filter_pr_debug("[ust_filter][%s]: Plugin Play\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_STOP)
	{
        ust_filter_pr_debug("[ust_filter][%s]: Plugin Stop\n", __FUNCTION__);
	}

    return ret;

}


static int ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    int ret = 0;
    
    UST_Pad_T *currentSrcPad = NULL;
    UST_BufDesc_T *currentBufdesc = NULL;
    int ksize = 1;
    
    uint8_t *image_data = bufDesc->data;
    uint32_t image_width = *(uint32_t *)bufDesc->prv;
    uint32_t image_height = bufDesc->length / image_width;
    

    ust_filter_pr_debug("[ust_filter][%s] BufDes[in]:%p \n", __FUNCTION__, bufDesc);

    /* median filter */
    //uint32_t time_start = k_uptime_get_32();
    #ifdef UST_FILTER_RPC
    RPC_Package_T pack;
    IMP_Data_T imp_data;
    imp_data.impCmd = IMP_CMD_FILTER;
    imp_data.pSrc = image_data;
    imp_data.pDst = image_data;
    imp_data.width = image_width;
    imp_data.height = image_height/2;
    pack.cmd = RPC_CMD_IMP;
    pack.privData = &imp_data;
    ret = RPC_MasterSendPack(&pack);
    if(ret != RET_OK)
    {
        ust_filter_pr_debug("!!! RPC_MasterSendPack wrong \n");
        UST_FreeBufDes(currentBufdesc);
        ret = -ENOMEM;
        return ret;
    }
    
    image_t img;
    img.bpp = IMAGE_BPP_GRAYSCALE;//IMAGE_BPP_RGB565;
    img.w = image_width; img.h = image_height/2;
    img.data = image_data+image_width*image_height/2;
    imlib_median_filter(&img, ksize, 12, false, 0, false, NULL);
    
    //if(RPC_MasterWaitResp(&pack, TIMEOUT_WAIT_FOREVER) != RET_OK)
    if((RPC_MasterWaitResp(&pack, 2000) != RET_OK) || (pack.result != RET_OK))
    {
        ust_filter_pr_debug("!!! filter_slave wrong \n");
        UST_FreeBufDes(currentBufdesc);
        ret = -ENOMEM;
        return ret;
    }
    #else
    image_t img;
    img.bpp = IMAGE_BPP_GRAYSCALE;//IMAGE_BPP_RGB565;
    img.w = image_width; img.h = image_height;
    img.data = image_data;//image_addr;
    imlib_median_filter(&img, ksize, 12, false, 0, false, NULL);
    #endif
    //uint32_t time_end = k_uptime_get_32();
    //printk("[filter cost]: %d ms\n", time_end-time_start);

    /*prepare current currentBufdesc*/
    currentBufdesc = bufDesc;
    currentSrcPad = element->srcPadManage.padTable[0];

    /*push current bufdesc to perr pad*/
    ust_filter_pr_debug("[ust_filter][%s] BufDes[out]:%p \n", __FUNCTION__, currentBufdesc);
    UST_PadPush (currentSrcPad, currentBufdesc);

    return ret;
}


static int ust_prepare_element(struct device *dev)
{
	UST_Element_T *element = NULL;
	UST_Pad_T *srcPad = NULL;
	UST_Pad_T *sinkPad = NULL;
	int ret = 0;

	/*alloc src element and config it*/
    element = UST_AllocElement(ELEMENT_FILTER);
	if(!element){
        ust_filter_pr_debug("[ust_filter][%s]: Err:  UST_AllocElement NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	element->prv = dev;
	UST_RegisterElementWorkStatusFun(element, ust_element_workstatus_fun);

	/*alloc pad and config it*/
	srcPad = UST_AllocPad(ELEMENT_FILTER_PAD_SRC);
	if(!srcPad){
        ust_filter_pr_debug("[ust_filter][%s]: Err:  UST_AllocSrcPad NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	sinkPad = UST_AllocPad(ELEMENT_FILTER_PAD_SINK);
	if(!sinkPad){
        ust_filter_pr_debug("[ust_filter][%s]: Err:  UST_AllocSinkPad NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	/*set sinkpad chian cb*/
	UST_RegisterPadChainFun(sinkPad, ust_sinkpad_chain_fun);

	/*add sink/src pad to element*/
	ret = UST_AddPadToElement(element,UST_SRC_PAD,srcPad);
	if(ret){
        ust_filter_pr_debug("[ust_filter][%s]: Err:  UST_AddSrcPadToElement\n", __FUNCTION__);
        goto out;
	}
	ret = UST_AddPadToElement(element,UST_SINK_PAD,sinkPad);
	if(ret){
        ust_filter_pr_debug("[ust_filter][%s]: Err:  UST_AddSinkPadToElement\n", __FUNCTION__);
        goto out;
	}


out:
	return ret;
}

static int ust_prepare_queue()
{
#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
    /*alloc queue*/
    UST_QueueElement_T *queueElement = NULL;
    int ret = 0;
    queueElement = UST_AllocQueueElement(ELEMENT_FILTER_QUEUE,ELEMENT_FILTER_QUEUE_PAD_SINK,
                                         ELEMENT_FILTER_QUEUE_PAD_SRC, CONFIG_UST_PLUGIN_FILTER_QUEUE_MAX_NUM);

    if(!queueElement){
        ust_filter_pr_debug("[ust_filter][%s] UST_AllocQueueElement NULL\n",__FUNCTION__);
        ret = -ENOMEM;
        goto out;
    }

    /*link queue*/
    UST_Pad_T *srcElementSrcPad = NULL;
    UST_Pad_T *queueElementSinkPad = NULL;
    UST_Element_T *srcElement = NULL;

    srcElement = UST_GetElement(ELEMENT_FILTER);
    if(!srcElement){
        ret = -ENOMEM;
        goto out;
    }
    queueElement = (UST_QueueElement_T *)(UST_GetElement(ELEMENT_FILTER_QUEUE));
    if(!queueElement){
        ret = -ENOMEM;
        goto out;
    }

    srcElementSrcPad = UST_GetPadFromElement(srcElement, UST_SRC_PAD, ELEMENT_FILTER_PAD_SRC);
    if(!srcElementSrcPad){
        ret = -ENOMEM;
        goto out;
    }

    queueElementSinkPad = UST_GetPadFromElement((UST_Element_T *)queueElement, UST_SINK_PAD, ELEMENT_FILTER_QUEUE_PAD_SINK);
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
        ust_filter_pr_debug("[ust_filter][%s]: Err:  ust_prepare_element\n", __FUNCTION__);
        goto out;
    }

    ret = ust_prepare_queue();
    if(ret){
        ust_filter_pr_debug("[ust_filter][%s] ust_prepare_queue\n",__FUNCTION__);
        goto out;
    }

out:
	return ret;
}


DEVICE_DEFINE(plugin_filter, DEV_UST_FILTER_NAME, ust_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;














