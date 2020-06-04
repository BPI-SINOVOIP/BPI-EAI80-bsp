#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include "ust.h"

static void UST_InitPadManage(UST_ElementPadManage_T *mange);

/*todo use hash list*/
UST_Element_T *UST_GetElement(char *name)
{
	 UST_Element_T *element = NULL;

	 sys_dlist_t *gElemnetList = UST_GetGlobalElementList();
	 if(!gElemnetList){
		return NULL;
	 }

	SYS_DLIST_FOR_EACH_CONTAINER(gElemnetList, element, node) {
		if (!strcmp(name, element->name)) {
			return element;
		}
	}

	return NULL;
}



static void UST_InitPadManage(UST_ElementPadManage_T *mange)
{
	memset(&(mange->padTable[0]), 0, (sizeof(uint32_t)*MAX_PAD_PORT));
	memset(&(mange->padVaileTable[0]), false, (sizeof(uint8_t)*MAX_PAD_PORT));
}




void UST_InitElement(UST_Element_T *element,char *name)
{
	UST_InitPadManage(&element->sinkPadManage);
	UST_InitPadManage(&element->srcPadManage);

    strcpy(element->name,name);
	element->type = UST_ELEMENT_NONE;

	/*init for gloabe list*/
	sys_dlist_init(&element->node);

	/*init for connet to pileline element list*/
	sys_dlist_init(&element->pileLineNode);
	/*init for connet to type element list*/
	sys_dlist_init(&element->elementTypeNode);

}


UST_Element_T *UST_AllocElement(char *name)
{
	UST_Element_T *element= UST_MALLOCZ(sizeof(UST_Element_T));
	if(!element){
		return NULL;
	}

    UST_InitElement(element,name);

    /*regist element to core*/
    UST_RegisterElementToCore(element);

    ust_pr_debug("####[%s] Nmae:%s type:%d######\n",__FUNCTION__,name,element->type);

	return element;
}

















