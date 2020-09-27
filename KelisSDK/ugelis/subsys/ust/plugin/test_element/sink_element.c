#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include <ust_sink_element.h>
#include"sink_element.h"

static int gm_ust_sink_element_init(struct device *dev);
static int gm_ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element);
static int gm_ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status);

static int gm_ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY){
        ust_sink_elet_pr_debug("[%s]: Sink Element:enter UST_PIPE_READY\n",__FUNCTION__);
	}else if(status == UST_PIPE_PALY){
        ust_sink_elet_pr_debug("[%s]: Sink Element:enter UST_PIPE_PALY\n",__FUNCTION__);
	}else if(status == UST_PIPE_STOP){
        ust_sink_elet_pr_debug("[%s]: Sink Element:enter UST_PIPE_STOP\n",__FUNCTION__);
	}


    return ret;
}

static int gm_ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    int ret = 0;

    k_sleep(1000);
    /*free pre bufdes*/
    UST_FreeBufDes(bufDesc);

   return ret;
}

static int gm_ust_sink_element_init(struct device *dev)
{
	UST_Element_T *element = NULL;
	UST_Pad_T *sinkPad = NULL;
	int ret = 0;

	/*alloc element and config it*/
    element = UST_AllocElement(ELEMENT_SRINK);
	if(!element){
		ust_sink_elet_pr_err("!!!Err: %s UST_AllocElement NULL\n",__FUNCTION__);
        return -ENOMEM;
	}

	element->prv = dev;
	UST_RegisterElementWorkStatusFun(element, gm_ust_element_workstatus_fun);

	/*alloc pad and config it*/
	sinkPad = UST_AllocPad(ELEMENT_SINK_PAD_SINK);
	if(!sinkPad){
		ust_sink_elet_pr_err("!!!Err: %s UST_AllocSinkPad NULL\n",__FUNCTION__);
        return -ENOMEM;
	}
	/*set srcpad chian cb*/
	UST_RegisterPadChainFun(sinkPad, gm_ust_sinkpad_chain_fun);

	/*add sink pad to element*/
	ret = UST_AddPadToElement(element,UST_SINK_PAD,sinkPad);
	if(ret){
		ust_sink_elet_pr_err("!!!Err: %s UST_AddSinkPadToElement\n",__FUNCTION__);
        goto out;
	}

    ust_sink_elet_pr_debug("#### Sink: element:%p sinkPad:%p####\n",element,sinkPad);

out:
	return ret;
}




DEVICE_DEFINE(element_sink, ELEMENT_SRINK_NAME, gm_ust_sink_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;



