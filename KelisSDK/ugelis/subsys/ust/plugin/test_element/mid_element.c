#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include <ust_mid_element.h>
#include "mid_element.h"


#define UST_MID_MALLOC(size)       malloc_ext(0,size)


static int gm_ust_middle_element_init(struct device *dev);
static int gm_ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element);
static int gm_ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status);



static int gm_ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY){
        ust_mid_elet_pr_debug("[%s]: MID Element:enter UST_PIPE_READY\n",__FUNCTION__);
	}else if(status == UST_PIPE_PALY){
        ust_mid_elet_pr_debug("[%s]: MID Element:enter UST_PIPE_PALY\n",__FUNCTION__);
	}else if(status == UST_PIPE_STOP){
        ust_mid_elet_pr_debug("[%s]: MID Element:enter UST_PIPE_STOP\n",__FUNCTION__);
	}

    return ret;

}


static int gm_ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    int ret = 0;
    uint8_t *data = NULL;
    UST_Pad_T *currentSrcPad= NULL;
    UST_BufDesc_T *currentBufdesc = NULL;

    k_sleep(1000);
    /*alloc current bufdes*/
    currentBufdesc = UST_AllocBufDes();
    if(!currentBufdesc){
        ret = - ENOMEM;
        goto out;
    }


    /*copy pre element data(+1) to cuurent element */
    data = UST_MID_MALLOC(bufDesc->length);
    if(!data){
        ret = -ENOMEM;
        goto out;
    }

    /*prepare current currentBufdesc*/
    currentBufdesc->data   = data;
    currentBufdesc->length = bufDesc->length;
    currentBufdesc->type   = UST_DATA_FROM_MALLOC;
    currentSrcPad = element->srcPadManage.padTable[0];
    bufDesc->data[0]++; /*get data and make it ++ to push to next*/
    memset(data,bufDesc->data[0],bufDesc->length);

    ust_mid_elet_pr_debug("\nmid element getpre data:%d  sendnextdata:%d\n",--bufDesc->data[0],data[0]);
    /*free pre bufdes*/
    UST_FreeBufDes(bufDesc);

    /*push current bufdesc to perr pad*/
    ret = UST_PadPush (currentSrcPad, currentBufdesc);
    if(ret){
        ust_mid_elet_pr_err("!!!Err:%s UST_PadPush Need Trace\n",__FUNCTION__);
        UST_FreeBufDes(currentBufdesc);
        goto out;
    }



out:
    return ret;
}


static int gm_ust_middle_element_init(struct device *dev)
{
	UST_Element_T *element = NULL;
	UST_Pad_T *srcPad = NULL;
	UST_Pad_T *sinkPad = NULL;
	int ret = 0;

	/*alloc src element and config it*/
    element = UST_AllocElement(ELEMENT_MIDDLE);
	if(!element){
		ust_mid_elet_pr_err("!!!Err: %s UST_AllocElement NULL\n",__FUNCTION__);
        return -ENOMEM;
	}
	element->prv = dev;
	UST_RegisterElementWorkStatusFun(element, gm_ust_element_workstatus_fun);

	/*alloc pad and config it*/
	srcPad = UST_AllocPad(ELEMENT_MIDDLE_PAD_SRC);
	if(!srcPad){
		ust_mid_elet_pr_err("!!!Err: %s UST_AllocSrcPad NULL\n",__FUNCTION__);
        return -ENOMEM;
	}

	sinkPad = UST_AllocPad(ELEMENT_MIDDLE_PAD_SINK);
	if(!sinkPad){
		ust_mid_elet_pr_err("!!!Err: %s UST_AllocSinkPad NULL\n",__FUNCTION__);
        return -ENOMEM;
	}

	/*set sinkpad chian cb*/
	UST_RegisterPadChainFun(sinkPad, gm_ust_sinkpad_chain_fun);

	/*add sink/src pad to element*/
	ret = UST_AddPadToElement(element,UST_SRC_PAD,srcPad);
	if(ret){
		ust_mid_elet_pr_err("!!!Err: %s UST_AddSrcPadToElement\n",__FUNCTION__);
        goto out;
	}
	ret = UST_AddPadToElement(element,UST_SINK_PAD,sinkPad);
	if(ret){
		ust_mid_elet_pr_err("!!!Err: %s UST_AddSinkPadToElement\n",__FUNCTION__);
        goto out;
	}

    ust_mid_elet_pr_debug("#### Middle: element:%p  srcPad:%p  sinkPad:%p####\n",element,srcPad,sinkPad);


out:
	return ret;
}




DEVICE_DEFINE(element_middle, DEV_MIDDLE_ELEMENT_NAME, gm_ust_middle_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;














