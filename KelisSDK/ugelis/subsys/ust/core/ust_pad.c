#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include "ust.h"


#define UST_GET_PAD_FROM_ELEMENT(pad,manage, name)  do{			                                                                \
                                                            uint8_t i = 0;    												    \
                                                            while((i < MAX_PAD_PORT) && (manage->padVaileTable[i] == true)) {	\
                                                                if (!strcmp(name, manage->padTable[i]->name)) {	     			\
                                                                    pad = manage->padTable[i];							        \
                                                                    break;													    \
                                                                }	                                                            \
                                                                i++;                                                            \
                                                            }						                                          	\
												     }while(0)







UST_Pad_T *UST_GetPadFromElement(UST_Element_T *element, UST_PadType_T type, char *name)
{
	UST_Pad_T *pad = NULL;
    UST_ElementPadManage_T  *padManage = NULL;

	if(type == UST_SRC_PAD){
        padManage = &(element->srcPadManage);

	}else if(type == UST_SINK_PAD){
        padManage = &(element->sinkPadManage);
	}

   	UST_GET_PAD_FROM_ELEMENT(pad, padManage, name);

	return pad;
}


int UST_AddPadToElement(UST_Element_T *element, UST_PadType_T type, UST_Pad_T *pad)
{
	int ret = -ENOSPC;
	uint8_t i = 0;
	UST_ElementPadManage_T 	*padManage = NULL;

	if((!element)||(!pad)){
		return -EINVAL;
	}

	if(type == UST_SRC_PAD){
		padManage = &(element->srcPadManage);
		element->type |= UST_ELEMENT_SRC;
	}else if(type == UST_SINK_PAD){
		padManage = &(element->sinkPadManage);
		element->type |= UST_ELEMENT_SINK;
	}

	for(i = 0 ; i< MAX_PAD_PORT; i++){
		if(padManage->padVaileTable[i] == false){
			padManage->padTable[i] 		= pad;
			padManage->padVaileTable[i] = true;
            pad->element = element;
			ret = 0;
			break;
		}
	}

	return ret;

}



UST_Pad_T *UST_AllocPad(char *name)
{
	UST_Pad_T *pad = UST_MALLOCZ(sizeof(UST_Pad_T));
	if(!pad){
		return NULL;
	}

    strcpy(pad->name, name);
    ust_pr_debug("[%s] PadName:%s \n",__FUNCTION__,pad->name);
    return pad;
}

void UST_PadConnect(UST_Pad_T *upElementSrcPad, UST_Pad_T *downElementSinkPad)
{
	upElementSrcPad->peerPad = downElementSinkPad;
	downElementSinkPad->peerPad = upElementSrcPad;
}


int UST_PadPush(UST_Pad_T *pad, UST_BufDesc_T *bufDesc)
{
	int ret  = 0;
	UST_Pad_T *peerPad = pad->peerPad;


	if(!bufDesc){
		return -EINVAL;
	}

	if((!peerPad)||(!peerPad->chainFunc)){
		return -EINVAL;
	}

	ret = peerPad->chainFunc(bufDesc,UST_PAD_TO_ELEMENT(peerPad));

	return ret;
}



