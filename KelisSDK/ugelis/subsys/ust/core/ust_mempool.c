#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <ust_core.h>
#include "ust.h"


/*private*/
#define MEM_POOL_PTR_SIZE 4
#define MEM_ALINE_SIZE    4


 /*mem pool status*/
 typedef enum
 {
   UST_MEM_FREE       = 0x00,
   UST_MEM_USING            ,
 } UST_MemPoolStatus_T;

static inline uint32_t UST_ToIntegerMutiple(uint32_t n ,uint32_t mutiple)
{
    if(n % mutiple) {
        n = (1 + n / mutiple)*mutiple;
    }

    return n;
}





static int UST_InitMemPool(UST_MemPool_T *memPool, uint8_t *memPoolBaseAddress ,uint32_t objNum,uint32_t objSize)
{
     uint32_t i = 0;
     int ret = 0;
     UST_MemPool_T **poolAddress = NULL;

     memPool->objSizeAlign4      = UST_ToIntegerMutiple(objSize,MEM_ALINE_SIZE);
     memPool->freeObjectNum      = memPool->num = objNum;
     memPool->basePoolAddress    = memPoolBaseAddress;
     memPool->memvailtable       = UST_MALLOCZ(objNum);
     if(!memPool->memvailtable){
         ret = -ENOMEM;
         goto out;
     }
     memset(memPool->memvailtable,UST_MEM_FREE,objNum);
     for(i =0,poolAddress=(UST_MemPool_T **)memPoolBaseAddress ; i<objNum ; i++){
         *poolAddress = memPool;
          poolAddress = (UST_MemPool_T **)((uint32_t)(poolAddress)+(MEM_POOL_PTR_SIZE+memPool->objSizeAlign4));
    }

out:
    return ret;

}




UST_MemPool_T *UST_CreatObjectMemoryPool(uint32_t objNum, uint32_t objSize)
{
	UST_MemPool_T   *memPool = NULL;
	uint8_t   		*memPoolBaseAddress = NULL;
    uint32_t        objMutSize = 0;

	memPool = UST_MALLOCZ(sizeof(UST_MemPool_T));
	if(!memPool){
		return NULL;
	}

	/*
	 * todo how to fix  MemPool Ptr 4byte aline
	 * fisrt 4 byte use to save MemPool prt
	 */
	objMutSize = UST_ToIntegerMutiple(objSize,MEM_ALINE_SIZE); /*malloc 4alie size*/
	memPoolBaseAddress = (uint8_t *)(UST_MALLOCZ(((objMutSize+MEM_POOL_PTR_SIZE)*objNum) + MEM_ALINE_SIZE));
	if(!memPoolBaseAddress){
		return NULL;
	}
    memPool->bakPoolAddress = memPoolBaseAddress;
    memPoolBaseAddress = (uint8_t *)_ALIGN4((uint32_t)(memPoolBaseAddress));

    if(UST_InitMemPool(memPool,memPoolBaseAddress,objNum,objSize)){
		return NULL;
    }

 //   printk("objSizeAlign4:%d freeObjectNum:%d basePoolAddress:%p\n",memPool->objSizeAlign4,memPool->freeObjectNum,
 //   		 memPool->basePoolAddress);

    return memPool;
}


int UST_DeleteObjectMemoryPool(UST_MemPool_T   *memPool)
{
    if(!memPool){
        return -EINVAL;
    }

    UST_FREE(memPool->bakPoolAddress);
    UST_FREE(memPool->memvailtable);
    UST_FREE(memPool);

    return 0;
}


void *UST_AllocObjectFromMemoryPool(UST_MemPool_T *memPool)
{
	uint8_t i = 0;
    unsigned int key;
	uint8_t *memvailtable = memPool->memvailtable;

    key = irq_lock();
    
	if(!memPool->freeObjectNum){
        irq_unlock(key);    
		return NULL;
	}else{
		for(i = 0; i<memPool->num; i++ ){
			if(memvailtable[i] == UST_MEM_FREE){
				memvailtable[i] = UST_MEM_USING;
				memPool->freeObjectNum--;
				break;
			}
		}
	}
    
    irq_unlock(key);
	return ((memPool->basePoolAddress+i*(memPool->objSizeAlign4 + MEM_POOL_PTR_SIZE))+MEM_POOL_PTR_SIZE);
}

void UST_FreeObjectToMemoryPool(uint8_t *ptr)
{
    uint8_t i = 0;
    unsigned int key;

    UST_MemPool_T *memPool = *((UST_MemPool_T **)(ptr-MEM_POOL_PTR_SIZE));

    uint8_t *memvailtable = memPool->memvailtable;
    uint32_t checkaddressBase = (uint32_t)(memPool->basePoolAddress + MEM_POOL_PTR_SIZE);
    uint32_t length = memPool->objSizeAlign4 + MEM_POOL_PTR_SIZE;

    if(!ptr){
        return;
    }
    
    for(i = 0; i<memPool->num; i++ ){
    	if(checkaddressBase+i*length == (uint32_t)ptr){
            key = irq_lock();
    		memvailtable[i] = UST_MEM_FREE;
    		memPool->freeObjectNum++;
            irq_unlock(key);
    		break;
    	}
    }

    //    printk("+++++++++[%s]: memPool->freeObjectNum=%d\n",__FUNCTION__,memPool->freeObjectNum);
}




