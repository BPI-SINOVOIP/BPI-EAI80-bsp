#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include "ust.h"



static int UST_QueueSinkPadChainFun(UST_BufDesc_T *bufDesc,struct element *element);
static int UST_QueueElementWorkStatusFun(struct element *element,enum pipeLinestatus status);

static int UST_QueueElementWorkStatusFun(struct element *element,enum pipeLinestatus status)
{
    int ret = 0;
	UST_QueueElement_T *queueElement = (UST_QueueElement_T *)(element);

    if(status == UST_PIPE_READY){
        k_thread_suspend(queueElement->tid);
        ust_pr_debug("[%s]: Queue Element:enter UST_PIPE_READY\n",__FUNCTION__);
    }else if(status == UST_PIPE_PALY){
        k_thread_resume(queueElement->tid);
        k_thread_start(queueElement->tid);
        ust_pr_debug("[%s]: Queue Element:enter UST_PIPE_PALY\n",__FUNCTION__);
    }else if(status == UST_PIPE_STOP){
        k_thread_suspend(queueElement->tid);
        ust_pr_debug("[%s]: Queue Element:enter UST_PIPE_STOP\n",__FUNCTION__);
    }else{
        ret = -EINVAL;
    }

    return ret;
}

static int UST_QueueSinkPadChainFun(UST_BufDesc_T *bufDesc,struct element *element)
{

       int ret = 0;
       UST_QueueElement_T *queueElement = (UST_QueueElement_T *)(element);

       ret = k_msgq_put(&(queueElement->msgQueue), (void*)&bufDesc, queueElement->timeoutMs);

       return ret;
}


static void UST_QueueThreadPushFun(void *p1, void *p2, void *p3)
{
    int ret = 0;
	uint32_t bufDesc = 0;
    UST_Pad_T *srcPad = NULL;

    UST_QueueElement_T *queueElement = p1;
    UST_ElementPadManage_T *psrcPadManage = NULL;

    while(1){
        /*get bufdes*/
        k_msgq_get(&(queueElement->msgQueue), (void*)(&bufDesc), K_FOREVER);
        /*get peerpad*/
        psrcPadManage = &(queueElement->element.srcPadManage);
        srcPad = psrcPadManage->padTable[0];

        /*push to next pad*/
        ret = UST_PadPush(srcPad, (UST_BufDesc_T *)bufDesc);
        if(ret){
            ust_pr_err("Err:[%s] UST_PadPush\n",__FUNCTION__);
            UST_FreeBufDes((UST_BufDesc_T *)bufDesc);
        }

    }


}




static int UST_MsqQueueAlloc(struct k_msgq *msgQueue,uint8_t maxBufDesQueue)
{
    char *msgBuf = NULL;
    int ret = 0;
    msgBuf = UST_MALLOCZ(sizeof(uint32_t)*maxBufDesQueue+4);
    if(!msgBuf){
        ret = -ENOMEM;
        goto out;
    }

    msgBuf = (char *)_ALIGN4((uint32_t)(msgBuf)); /*make sure msgBuf 4byte aline*/
    if((uint32_t )msgBuf % 4){
        ret = -ENOMEM;
        goto out;
    }


    k_msgq_init(msgQueue, msgBuf, sizeof(uint32_t),maxBufDesQueue);

out:
    return ret;
}






UST_QueueElement_T *UST_AllocQueueElement(char *elementname,char *sinkPadNmae,char *srcPadNmae, uint8_t maxBufDesQueue)
{

	UST_Pad_T *srcPad =NULL;
	UST_Pad_T *sinkPad=NULL;
	uint8_t *stack = NULL;
	UST_QueueElement_T *queueElement = NULL;

    #define QUEUE_STACK_SIZE    (2048+4)
	stack = UST_MALLOCZ(QUEUE_STACK_SIZE);
	if(!stack){
		return NULL;
	}
    /*4byte aline*/
    stack = (uint8_t *)(_ALIGN4((uint32_t)stack));

	queueElement = UST_MALLOCZ(sizeof(UST_QueueElement_T));
	if(!queueElement){
		return NULL;
	}

    UST_InitElement(&queueElement->element,elementname);
    /*regist element to core*/
    UST_RegisterElementToCore(&queueElement->element);

	/*creat task, but not start it*/
	queueElement->tid = k_thread_create(&(queueElement->thread), (k_thread_stack_t *)stack, QUEUE_STACK_SIZE,
			                            UST_QueueThreadPushFun, queueElement, NULL, NULL,
					     			    K_PRIO_COOP(15),0, K_FOREVER);

    if(UST_MsqQueueAlloc(&(queueElement->msgQueue),maxBufDesQueue)){
        return NULL;
    }


	/*creat src and srink pad*/
	srcPad = UST_MALLOCZ(sizeof(UST_Pad_T));
	if(!srcPad){
		return NULL;
	}

	sinkPad = UST_MALLOCZ(sizeof(UST_Pad_T));
	if(!sinkPad){
		return NULL;
	}

    /*set pad name*/
    strcpy(srcPad->name, srcPadNmae);
    strcpy(sinkPad->name, sinkPadNmae);

	/*add pads to element*/
    if(UST_AddPadToElement(&(queueElement->element),UST_SRC_PAD,srcPad)){
        return NULL;
    }

    if(UST_AddPadToElement(&(queueElement->element),UST_SINK_PAD,sinkPad)){
        return NULL;
    }

	/*set callback fun to core*/
	UST_RegisterPadChainFun(sinkPad,UST_QueueSinkPadChainFun);
	UST_RegisterElementWorkStatusFun(&(queueElement->element),UST_QueueElementWorkStatusFun);
    UST_QueuePushBufdescTimeout(queueElement, K_FOREVER);

	return queueElement;
}



