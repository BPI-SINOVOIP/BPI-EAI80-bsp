
#ifndef _UST_CORE_H_
#define _UST_CORE_H_


#ifdef __cplusplus
extern "C" {
#endif



#include <ugelis/types.h>

struct ust_bufdesc;
struct element;
enum pipeLinestatus;
struct ust_pad;

typedef int	(*UST_PadChainFunction)(struct ust_bufdesc *bufDesc, struct element *element);
typedef void(*UST_BufReleaseCb)(struct ust_bufdesc *bufDesc);
typedef int(*UST_ElementWrokStatusFun)(struct element *element,enum pipeLinestatus status);



/*element*/
#define MAX_PAD_PORT 3

typedef struct
{
    struct ust_pad  *padTable[MAX_PAD_PORT];
    uint8_t     padVaileTable[MAX_PAD_PORT];
}UST_ElementPadManage_T;

typedef enum
{
	UST_ELEMENT_NONE		  = 0x0,
	UST_ELEMENT_SRC 		  = 0x1,
	UST_ELEMENT_SINK		  = 0x2,
	UST_ELEMENT_SRC_SINK	  = (UST_ELEMENT_SRC|UST_ELEMENT_SINK),
}UST_ElementType_T;

typedef struct element
{
	/*pulic*/
    UST_ElementPadManage_T 	    srcPadManage;           /*src pad manage:use to chose pad*/
    UST_ElementPadManage_T      sinkPadManage; 			/*sink pad manage:use to chose pad*/
	void 					   *prv;		    		/*usr can use it */

	/*private*/
	char					    name[20];					/*element name*/
	UST_ElementWrokStatusFun    workStatusFun; 			/*element work callback fun*/
	sys_dlist_t  			    node;         			/*connent to gloab element list*/
	sys_dlist_t  			    pileLineNode; 			/*connent to pile line*/
	sys_dlist_t  	     		elementTypeNode; 		/*type node/src/sink/src-sink*/
	UST_ElementType_T           type;

}UST_Element_T;

typedef struct virelement
{
	#define PHY_ELEMENT_NUM 				2
	UST_Element_T element;
	UST_Element_T *phyelement[PHY_ELEMENT_NUM];
}UST_VirtualElement_T;


/*pad*/
typedef enum
{
	UST_SRC_PAD		= 0,
	UST_SINK_PAD	 ,
}UST_PadType_T;

typedef struct ust_pad
{
	/*pulic*/
	char 					name[20];
	struct ust_pad   		*peerPad;
	UST_PadChainFunction	chainFunc;
	/*private*/
	UST_Element_T           *element;
}UST_Pad_T;


/*pipeline*/
typedef enum pipeLinestatus
{
	UST_PIPE_NONE      =  0x00,
	UST_PIPE_READY            ,
  	UST_PIPE_PALY        	  ,
  	UST_PIPE_STOP             ,
} UST_PipeLineStatus_T;

typedef struct
{
	uint8_t          init;
	sys_dlist_t  	 sinkTypeList;
	sys_dlist_t  	 srcTypeList;
	sys_dlist_t  	 srcSrinkTypeList;
}UST_PipeLineElementTypeList_T;

typedef struct
{
	/*private*/
    struct k_thread      				thread;
	k_tid_t              				tid;
	UST_PipeLineStatus_T 				status;
	sys_dlist_t      	 				elementPipeList;  	/*mange element list*/
	UST_PipeLineElementTypeList_T       elementTypeManger;
}UST_PipeLine_T;


/*mem pool*/

typedef struct
{
	/*private*/
	uint32_t  num; /*bakup object num*/
	uint32_t  objSizeAlign4;
    uint8_t   *memvailtable;
	uint8_t   *basePoolAddress;
    uint8_t   *bakPoolAddress;
	uint32_t  freeObjectNum;
}UST_MemPool_T;


/*buf desc*/

typedef enum {
	UST_DATA_FROM_DEFAULT	= 0x00,
	UST_DATA_FROM_MALLOC	= 0x01,
	UST_DATA_FROM_MEMPOOL	= 0x02,
} UST_Data_Request_Type_T;





typedef struct ust_bufdesc
{
	/*public*/
    uint32_t length;
    uint8_t *data;
	UST_Data_Request_Type_T type;
	void *prv;
	/*private*/
	UST_BufReleaseCb cb;
    sys_dlist_t  node;
    uint8_t refcount;
    uint8_t using;

}UST_BufDesc_T;


/*queue*/
typedef enum
{
  UST_QUEUE_DROP       = 0x01,
  UST_QUEUE_WAIT 	         ,
  UST_QUEUE_CONVER           ,
} UST_QueueFullProcessAction_T;


typedef struct
{
	/*private*/
	UST_Element_T element;
	struct k_thread thread;
	k_tid_t tid;
	struct k_msgq msgQueue;
	s32_t timeoutMs;
}UST_QueueElement_T;

/*marco*/

#define MAX_PAD_PORT                                        3

#define UST_PAD_TO_ELEMENT(pad)   							(pad->element)


/*core*/





UST_PipeLine_T *UST_AllocPileLine();
int UST_SetPipeLineStatus(UST_PipeLine_T *pipeLine, UST_PipeLineStatus_T status);
static inline UST_PipeLineStatus_T UST_GetPipeLineStatus(UST_PipeLine_T *pipeLine);
sys_dlist_t *UST_GetGlobalElementList();


/*elment*/
UST_Element_T *UST_NormalElementMergeQueueElement(UST_Element_T *normalElement, UST_QueueElement_T *queueElement);
UST_Element_T *UST_AllocElement(char *name);
UST_Element_T *UST_GetElement(char *name);
void UST_InitElement(UST_Element_T *element,char *name);



/*pad*/
UST_Pad_T *UST_GetPadFromElement(UST_Element_T *element, UST_PadType_T type, char *name);
int UST_AddPadToElement(UST_Element_T * element,UST_PadType_T type,UST_Pad_T *pad);

UST_Pad_T *UST_AllocPad(char *name);

void UST_PadConnect(UST_Pad_T *upElementSrcPad, UST_Pad_T *downElementSinkPad);
int UST_PadPush(UST_Pad_T *pad, UST_BufDesc_T *bufDesc);


/*queue*/
static inline void UST_RegisterElementWorkStatusFun(UST_Element_T *element, UST_ElementWrokStatusFun fun);
UST_QueueElement_T *UST_AllocQueueElement(char *elementname,char *sinkPadNmae,char *srcPadNmae, uint8_t maxBufDesQueue);

/*mempool*/
UST_MemPool_T *UST_CreatObjectMemoryPool(uint32_t objNum, uint32_t objSize);
int UST_DeleteObjectMemoryPool(UST_MemPool_T   *memPool);
void *UST_AllocObjectFromMemoryPool(UST_MemPool_T *memPool);
void UST_FreeObjectToMemoryPool(uint8_t *ptr);



/*bufdesc*/
UST_BufDesc_T *UST_AllocBufDes();
void UST_FreeBufDes(UST_BufDesc_T *bufDesc);


/*core*/
static inline void UST_RegisterElementWorkStatusFun(UST_Element_T *element, UST_ElementWrokStatusFun fun)
{
	element->workStatusFun = fun;
}

static inline void UST_RegisterBufDescReleaseCb(UST_BufDesc_T *bufDesc, UST_BufReleaseCb fun)
{
	bufDesc->cb = fun;
}

static inline void UST_ElementAddToPipeLine(UST_PipeLine_T *pipeLine, UST_Element_T *element)
{
	sys_dlist_append(&(pipeLine->elementPipeList), &(element->pileLineNode));
}

static inline UST_PipeLineStatus_T UST_GetPipeLineStatus(UST_PipeLine_T *pipeLine)
{
	return pipeLine->status;
}

/*queue*/
static inline void UST_QueuePushBufdescTimeout(UST_QueueElement_T *queueElement, s32_t timeoutMs)
{
    queueElement->timeoutMs = timeoutMs;
}

/*pad*/
static inline void UST_RegisterPadChainFun(UST_Pad_T *pad,UST_PadChainFunction fun)
{
	pad->chainFunc = fun;
}

static inline void UST_RegisterElementToCore(UST_Element_T *element)
{
    sys_dlist_append(UST_GetGlobalElementList(),&element->node);
}

/*element*/
static inline int UST_SetElementStatus(UST_Element_T *element, UST_PipeLineStatus_T status)
{
    return element->workStatusFun(element,status);
}




#ifdef __cplusplus
}
#endif

#endif /* _UST_CORE_H_ */

















