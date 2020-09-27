#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include <ust_src_element.h>
#include "src_element.h"


static int gm_prepare_queue_element();
static int gm_link_pad_element_to_queue();
static int gm_prepare_src_element(struct device *dev);
static int gm_ust_src_element_init(struct device *dev);
static int gm_ust_srcpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element);
static int gm_ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status);




static int gm_ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY){
        ust_src_elet_pr_debug("[%s]: SRC Element:enter UST_PIPE_READY\n",__FUNCTION__);
	}else if(status == UST_PIPE_PALY){
        ust_src_elet_pr_debug("[%s]: SRC Element:UST_PIPE_PALY\n",__FUNCTION__);
	}else if(status == UST_PIPE_STOP){
        ust_src_elet_pr_debug("[%s]: SRC Element:enter UST_PIPE_STOP\n",__FUNCTION__);
	}

    return ret;
}


static int gm_ust_srcpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    #define TEST_SINK_DATA_INTT_VAL 0
    int ret = 0;
    static uint8_t testDataVal = TEST_SINK_DATA_INTT_VAL;
    UST_MemPool_T *memPool =(UST_MemPool_T *)(element->prv);
    uint8_t *data = NULL;
    UST_Pad_T *currentSrcPad= NULL;
    UST_BufDesc_T *pushBufdesc = NULL;

    UNUSED(bufDesc);

    k_sleep(1000);
	/*alloc bufdes*/
    pushBufdesc = UST_AllocBufDes();
    if(!pushBufdesc){
        ret = - ENOMEM;
        goto out;
    }
	/*alloc data and prepare bufdesc*/
    data = UST_AllocObjectFromMemoryPool(memPool);

    memset(data,testDataVal,UST_SRC_DATA_SIZE);

    pushBufdesc->data   = data;
    pushBufdesc->length = UST_SRC_DATA_SIZE;
    pushBufdesc->type   = UST_DATA_FROM_MEMPOOL;
    pushBufdesc->prv    = NULL;

	/*get currnet elemet peer pad and push to it*/
    currentSrcPad = element->srcPadManage.padTable[0];

    ust_src_elet_pr_debug("\nsrc element sendnext data:%d\n",data[0]);

    ret = UST_PadPush(currentSrcPad, pushBufdesc);
    if(ret){
        ust_src_elet_pr_err("Err:%s UST_PadPush Need Trace\n",__FUNCTION__);
        UST_FreeBufDes(pushBufdesc);
        goto out;
    }

    /*update testDataVal for next push*/
    testDataVal++;

out:
    return ret;
}

static int gm_prepare_src_element(struct device *dev)
{
    int ret = 0;

    UST_MemPool_T *memPool = NULL;
    UST_Element_T *element = NULL;
    UST_Pad_T *srcPad = NULL;

    /*alloc src element and config it*/
    element = UST_AllocElement(ELEMENT_SRC);
    if(!element){
        ret = -ENOMEM;
        goto out;
    }

    element->prv = dev;
    UST_RegisterElementWorkStatusFun(element, gm_ust_element_workstatus_fun);

    /*alloc pad and config it*/
    srcPad = UST_AllocPad(ELEMENT_SRC_PAD_SRC);
    if(!srcPad){
        ret = -ENOMEM;
        goto out;
    }
    /*set srcpad chian cb*/
    UST_RegisterPadChainFun(srcPad, gm_ust_srcpad_chain_fun);

    /*add src pad to element*/
    ret = UST_AddPadToElement(element,UST_SRC_PAD,srcPad);
    if(ret){
        goto out;
    }

    /*alloc data mempool for queue later*/
    memPool = UST_CreatObjectMemoryPool(UST_SRC_QUEUE_MAX_OBJECT_NUM, UST_SRC_DATA_SIZE);
    if(!memPool){
        ret = -ENOMEM;
        goto out;
    }
    element->prv = memPool;

    ust_src_elet_pr_debug("#### SRC: element:%p  srcPad:%p memPool:%p####\n",element,srcPad,memPool);

out:

    return ret;
}




static int gm_prepare_queue_element()
{
#ifndef CONFIG_SRC_ELEMENT_QUEUE
    return 0;
#else
    /*has queue enter*/
    UST_QueueElement_T *queueElement = NULL;
    int ret = 0;
    queueElement = UST_AllocQueueElement(ELEMENT_SRC_QUEUE,ELEMENT_SRC_QUEUE_PAD_SINK,
                                         ELEMENT_SRC_QUEUE_PAD_SRC, CONFIG_SRC_ELEMENT_QUEUE_BUFMAX_NUM);

    if(!queueElement){
        ust_src_elet_pr_err("!!!Err: [%s] UST_AllocQueueElement NULL\n",__FUNCTION__);
        ret = -ENOMEM;
        goto out;
    }
out:
    return ret;

#endif

}



static int gm_link_pad_element_to_queue()
{
#ifndef CONFIG_SRC_ELEMENT_QUEUE
    return 0;
#else
    /*has queue enter*/
    int ret = 0;
    UST_Pad_T *srcElementSrcPad = NULL;
    UST_Pad_T *queueElementSinkPad = NULL;
    UST_QueueElement_T *queueElement = NULL;
    UST_Element_T *srcElement = NULL;

    srcElement = UST_GetElement(ELEMENT_SRC);
    if(!srcElement){
        ret = -ENOMEM;
        goto out;
    }
    queueElement = (UST_QueueElement_T *)(UST_GetElement(ELEMENT_SRC_QUEUE));
    if(!queueElement){
        ret = -ENOMEM;
        goto out;
    }

    srcElementSrcPad = UST_GetPadFromElement(srcElement, UST_SRC_PAD, ELEMENT_SRC_PAD_SRC);
    if(!srcElementSrcPad){
        ret = -ENOMEM;
        goto out;
    }

    queueElementSinkPad = UST_GetPadFromElement((UST_Element_T *)queueElement, UST_SINK_PAD, ELEMENT_SRC_QUEUE_PAD_SINK);
    if(!queueElementSinkPad){
        ret = -ENOMEM;
        goto out;
    }

    UST_PadConnect(srcElementSrcPad, queueElementSinkPad);

out:
    return ret;

#endif

}


static int gm_ust_src_element_init(struct device *dev)
{

	int ret = 0;

    ret = gm_prepare_src_element(dev);
    if(ret){
        ust_src_elet_pr_err("!!!Err: [%s] gm_prepare_src_element\n",__FUNCTION__);
        goto out;
    }

    ret = gm_prepare_queue_element(dev);
    if(ret){
        ust_src_elet_pr_err("!!!Err: [%s] gm_prepare_queue_element\n",__FUNCTION__);
        goto out;
    }

    ret = gm_link_pad_element_to_queue();
    if(ret){
        ust_src_elet_pr_err("!!!Err: [%s] gm_link_pad_element_to_queue\n",__FUNCTION__);
        goto out;
    }

out:
	return ret;
}



DEVICE_DEFINE(element_sink, DEV_SRC_ELEMENT_NAME, gm_ust_src_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;








