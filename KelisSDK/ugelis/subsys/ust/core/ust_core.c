#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include "ust.h"


#define UST_STREAMER_CORE 60

#define UST_PIPELINE_STACK_SIZE    2048
K_THREAD_STACK_DEFINE(pipeLineThreadStack, UST_PIPELINE_STACK_SIZE);

static sys_dlist_t gElemnetList;

UST_MemPool_T *gMemPoolBufDesc = NULL;


static int UST_SetElementTypeToList(UST_PipeLine_T *pipeLine);
static void UST_PileLineThreadFun(void *p1, void *p2, void *p3);
static int UST_PipeLineDoFun(sys_dlist_t *list, UST_PipeLineStatus_T status);
static UST_MemPool_T *UST_CreatBufDesMemoryPool(uint32_t objNum, uint32_t objSize);
static int UST_PipeLineChangeStatus(UST_PipeLine_T *pipeLine,UST_PipeLineStatus_T status);
static void UST_PileLineElementTypeListInit(UST_PipeLineElementTypeList_T *elementTypeList);


/*todo Konfig*/
#define BUF_DES_MAX       150




sys_dlist_t *UST_GetGlobalElementList()
{
	return &gElemnetList;
}

static void UST_PileLineElementTypeListInit(UST_PipeLineElementTypeList_T *elementTypeList)
{
	elementTypeList->init = false;
	sys_dlist_init(&(elementTypeList->sinkTypeList));
	sys_dlist_init(&(elementTypeList->srcTypeList));
	sys_dlist_init(&(elementTypeList->srcSrinkTypeList));
}


/**
 * todo
 * 1:err process
 * 2:source cost over process: examle file eof
 */
static void UST_PileLineThreadFun(void *p1, void *p2, void *p3)
{
	UST_PipeLine_T *pileLine =(UST_PipeLine_T *)p1;
	UST_Element_T *srcElement = NULL;
	sys_dlist_t *elementTypeHeadList = NULL;
	UST_Pad_T 	*srcPad = NULL;
	int ret = 0;


	UST_SetElementTypeToList(pileLine);

    ust_pr_debug("#########[%s]: enter UST_PileLineThreadFun############\n",__FUNCTION__);

	elementTypeHeadList = &(pileLine->elementTypeManger.srcTypeList) ;
    if(!elementTypeHeadList){
        ust_pr_err("Err: [%s]: elementTypeHeadList is NULL\n",__FUNCTION__);
        return ;
    }

    srcElement = SYS_DLIST_PEEK_HEAD_CONTAINER(elementTypeHeadList, srcElement, elementTypeNode);
	if(!srcElement){
		ust_pr_err("Err: [%s]: srcElement is NULL\n",__FUNCTION__);
        return ;
	}
	/*call src pad */
	srcPad = srcElement->srcPadManage.padTable[0];

	while(1){
		/*src dont need bufdes,normalt The fist srcPad will
		  generate data to bufdesc*/
		ret = srcPad->chainFunc(NULL,srcElement);
		if(ret){
			ust_pr_err("!!!Err:[%s] src chain func, current thread maybe crash\n",__FUNCTION__);

		}
	}

}

UST_PipeLine_T *UST_AllocPileLine()
{
	UST_PipeLine_T *pileLine = NULL;

	/*alloc pipe line*/
	pileLine = UST_MALLOCZ(sizeof(UST_PipeLine_T));
	if(!pileLine){
		return NULL;
	}

	/*creat thread*/
	pileLine->tid = k_thread_create(&(pileLine->thread), pipeLineThreadStack, K_THREAD_STACK_SIZEOF(pipeLineThreadStack),
						             UST_PileLineThreadFun, pileLine, NULL, NULL,K_PRIO_PREEMPT(0),
						             0, K_FOREVER);

	/*mange element list*/
	sys_dlist_init(&pileLine->elementPipeList);
	/*mange different type element list*/
	UST_PileLineElementTypeListInit(&pileLine->elementTypeManger);

	return pileLine;
}



static int UST_PipeLineDoFun(sys_dlist_t *list, UST_PipeLineStatus_T status)
{
  	int ret = 0;
	UST_Element_T *element = NULL;

	SYS_DLIST_FOR_EACH_CONTAINER(list, element, elementTypeNode) {
		ret = element->workStatusFun(element,status);
		if(ret){
			break;
		}
	}
	return ret;
}

static int UST_PipeLineChangeStatus(UST_PipeLine_T *pipeLine,UST_PipeLineStatus_T status)
{
	int ret = 0;

	/*set element list type*/
	UST_SetElementTypeToList(pipeLine);

	/*call element status fun*/
	if((status == UST_PIPE_READY)||(status == UST_PIPE_PALY)){
        if(status == UST_PIPE_READY){
            k_thread_suspend(pipeLine->tid);
        }else{
            k_thread_resume(pipeLine->tid);
            k_thread_start(pipeLine->tid);
        }
		ret = UST_PipeLineDoFun(&(pipeLine->elementTypeManger.srcTypeList),status);
		if(ret){
			goto out;
		}
		ret = UST_PipeLineDoFun(&(pipeLine->elementTypeManger.srcSrinkTypeList),status);
		if(ret){
			goto out;
		}
		ret = UST_PipeLineDoFun(&(pipeLine->elementTypeManger.sinkTypeList),status);
		if(ret){
			goto out;
		}
	}else if(status == UST_PIPE_STOP){
		ret = UST_PipeLineDoFun(&(pipeLine->elementTypeManger.sinkTypeList),status);
		if(ret){
			goto out;
	    }

        k_thread_suspend(pipeLine->tid);

		ret = UST_PipeLineDoFun(&(pipeLine->elementTypeManger.srcTypeList),status);
		if(ret){
			goto out;
		}
		ret = UST_PipeLineDoFun(&(pipeLine->elementTypeManger.srcSrinkTypeList),status);
		if(ret){
			goto out;
		}
	}else{
		ret = -EINVAL;
	}
out:
	return ret;
}

int UST_SetPipeLineStatus(UST_PipeLine_T *pipeLine, UST_PipeLineStatus_T status)
{
	int ret = 0;
	static uint8_t oldStatus = UST_PIPE_NONE;

	if(oldStatus == status){
		return ret;
	}

	ret = UST_PipeLineChangeStatus(pipeLine,status);
	if(!ret){
		pipeLine->status = status;
        oldStatus = status;
	}

	return ret;

}

static int UST_SetElementTypeToList(UST_PipeLine_T *pipeLine)
{
	int ret = 0;
	sys_dlist_t *elementTypeHeadList = NULL;
    sys_dlist_t *elementPipeHeadList = NULL;
    UST_Element_T *element = NULL;



	if(pipeLine->elementTypeManger.init == false){
    	/*trave pile list and set to elementtype list,only set one time for pipeline*/
		pipeLine->elementTypeManger.init = true;
        elementPipeHeadList = &(pipeLine->elementPipeList);

		SYS_DLIST_FOR_EACH_CONTAINER(elementPipeHeadList, element, pileLineNode) {
          	/*trave pline element list*/

			if(element->type == UST_ELEMENT_SRC){
				elementTypeHeadList = &(pipeLine->elementTypeManger.srcTypeList) ;
			}else if(element->type == UST_ELEMENT_SINK){
				elementTypeHeadList = &(pipeLine->elementTypeManger.sinkTypeList) ;
			}else if((element->type == UST_ELEMENT_SRC_SINK)){
				elementTypeHeadList = &(pipeLine->elementTypeManger.srcSrinkTypeList);
			}else{
				return -EINVAL;
			}
			sys_dlist_append(elementTypeHeadList,&(element->elementTypeNode));
		}
	}

	return ret;
}





static UST_MemPool_T *UST_CreatBufDesMemoryPool(uint32_t objNum, uint32_t objSize)
{
	gMemPoolBufDesc = UST_CreatObjectMemoryPool(objNum,objSize);
	return gMemPoolBufDesc;
}


/*bufdesc*/
UST_BufDesc_T *UST_AllocBufDes()
{
	UST_BufDesc_T *bufDesc = NULL;
	if(!gMemPoolBufDesc){
		return NULL;
	}
	bufDesc =(UST_AllocObjectFromMemoryPool(gMemPoolBufDesc));
	if(bufDesc){
		memset(bufDesc,0,sizeof(UST_BufDesc_T));
        bufDesc->using = true; /*bufdes vaild*/
	}
	return bufDesc;
}


void UST_FreeBufDes(UST_BufDesc_T *bufDesc)
{
	if(!bufDesc){
		return ;
	}

    /*bufdes free already,dont free it again*/
    if(bufDesc->using == false){
        return;
    }

	if(bufDesc->cb){
		/*usre want to release resourse by self flow*/
		bufDesc->cb(bufDesc);
	}else{
		/*core release release resource flowe*/
		if(bufDesc->type == UST_DATA_FROM_MALLOC){
			UST_FREE(bufDesc->data);
		}else if(bufDesc->type == UST_DATA_FROM_MEMPOOL){
			UST_FreeObjectToMemoryPool((uint8_t *)(bufDesc->data));
		}
		UST_FreeObjectToMemoryPool((uint8_t *)bufDesc);
        /*set bufdes invaild,means it is free*/
        bufDesc->using = false;
	}
}


/*stream core*/
static int UST_CoreInit(struct device *dev)
{
	int ret = 0;
	sys_dlist_t *list = NULL;
	UST_MemPool_T *memBufPool = NULL;

	list = UST_GetGlobalElementList();
	sys_dlist_init(list);

	memBufPool = UST_CreatBufDesMemoryPool(BUF_DES_MAX, sizeof(UST_BufDesc_T));
	if(!memBufPool){
		ust_pr_err("Err [%s]: Alloc memBufPool NULL\n",__FUNCTION__);
		ret = -ENOMEM ;
		goto out;
	}
out:
	return ret;

}


SYS_INIT(UST_CoreInit, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);



