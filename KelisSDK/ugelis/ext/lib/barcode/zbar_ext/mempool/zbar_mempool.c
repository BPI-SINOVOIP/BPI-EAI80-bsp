#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>

/* 
 * set 0 to use libc mem op, set 1 to use zbar private mempool. 
 * Code128 currently can't use zbar private mempool for unknown problem.
 */
/*
#ifdef ENABLE_CODE128
#define ZBAR_MEMPOOL_EN 0
#else
#define ZBAR_MEMPOOL_EN 0
#endif
*/
#define ZBAR_MEMPOOL_EN 1


#if ZBAR_MEMPOOL_EN

#define ZBAR_MEMPOOL_DEBUG(...) do{}while(0)
//#define ZBAR_MEMPOOL_DEBUG printf

//#define ZBAR_MEMPOOL_ERROR(...) do{}while(0)
#define ZBAR_MEMPOOL_ERROR printf


struct k_mutex mempool_mutex;


/*private*/
#define MEM_POOL_MAGIC_NUM 0x1A2B3C4D
#define MEM_POOL_PTR_SIZE  4
#define MEM_ALINE_SIZE     4

#define MEM_TYPE_64_NUM   20
#define MEM_TYPE_128_NUM  20
#define MEM_TYPE_512_NUM  20
#define MEM_TYPE_1K_NUM   10
#define MEM_TYPE_10K_NUM  10
#define MEM_TYPE_100K_NUM 2
#define MEM_TYPE_300K_NUM 2

typedef enum
{
    MEM_TYPE_64,
    MEM_TYPE_128,
    MEM_TYPE_512,
    MEM_TYPE_1K,
    MEM_TYPE_10K,
    MEM_TYPE_100K,
	MEM_TYPE_300K,
    MEM_TYPE_MAX
}MemPoolType_T;

/*mem pool status*/
typedef enum
{
    MEM_FREE       = 0x00,
    MEM_USING            ,
} MemPoolStatus_T;

/*mem pool struct*/
typedef struct
{
	/*private*/
    uint32_t  magicNum; /*0x1A2B3C4D*/
	uint32_t  num; /*bakup object num*/
	uint32_t  objSizeAlign4;
    uint8_t   *memvailtable;
	uint8_t   *basePoolAddress;
    uint8_t   *bakPoolAddress;
	uint32_t  freeObjectNum;
}MemPool_T;


/*static */MemPool_T *g_memPool[MEM_TYPE_MAX] = {NULL};

static uint8_t zbar_mempool_init_flat = 0;


static inline void *MALLOC(uint32_t size)
{
	void *ptr = NULL;

	ptr = malloc_ext(0, size);
	if(!ptr){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	memset(ptr, 0, size);

	return  ptr;
}

static inline void FREE(void *ptr)
{
	free_ext(0,ptr);
}

static inline uint32_t ToIntegerMutiple(uint32_t n ,uint32_t mutiple)
{
    if(n % mutiple) {
        n = (1 + n / mutiple)*mutiple;
    }

    return n;
}

static int InitMemPool(MemPool_T *memPool, uint8_t *memPoolBaseAddress ,uint32_t objNum,uint32_t objSize)
{
    uint32_t i = 0;
    int ret = 0;
    MemPool_T **poolAddress = NULL;

    memPool->magicNum           = MEM_POOL_MAGIC_NUM;
    memPool->objSizeAlign4      = ToIntegerMutiple(objSize,MEM_ALINE_SIZE);
    memPool->freeObjectNum      = memPool->num = objNum;
    memPool->basePoolAddress    = memPoolBaseAddress;
    memPool->memvailtable       = MALLOC(objNum);
    if(!memPool->memvailtable){
        ret = -ENOMEM;
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        goto out;
    }
    memset(memPool->memvailtable,MEM_FREE,objNum);
    for(i =0,poolAddress=(MemPool_T **)memPoolBaseAddress ; i<objNum ; i++){
        *poolAddress = memPool;
        poolAddress = (MemPool_T **)((uint32_t)(poolAddress)+(MEM_POOL_PTR_SIZE+memPool->objSizeAlign4));
    }

    out:
    return ret;

    }

static MemPool_T *CreatMemoryPool(uint32_t objNum, uint32_t objSize)
{
	MemPool_T   *memPool = NULL;
	uint8_t   		*memPoolBaseAddress = NULL;
    uint32_t        objMutSize = 0;

	memPool = MALLOC(sizeof(MemPool_T));
	if(!memPool){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	/*
	 * todo how to fix  MemPool Ptr 4byte aline
	 * fisrt 4 byte use to save MemPool prt
	 */
	objMutSize = ToIntegerMutiple(objSize,MEM_ALINE_SIZE); /*malloc 4alie size*/
	memPoolBaseAddress = (uint8_t *)(MALLOC(((objMutSize+MEM_POOL_PTR_SIZE)*objNum) + MEM_ALINE_SIZE));
	if(!memPoolBaseAddress){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}
    memPool->bakPoolAddress = memPoolBaseAddress;
    memPoolBaseAddress = (uint8_t *)_ALIGN4((uint32_t)(memPoolBaseAddress));

    if(InitMemPool(memPool,memPoolBaseAddress,objNum,objSize)){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
		return NULL;
    }

 //   printk("objSizeAlign4:%d freeObjectNum:%d basePoolAddress:%p\n",memPool->objSizeAlign4,memPool->freeObjectNum,
 //   		 memPool->basePoolAddress);

    return memPool;
}


static int DeleteMemoryPool(MemPool_T *memPool)
{
    if(!memPool){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        return -EINVAL;
    }

    FREE(memPool->bakPoolAddress);
    FREE(memPool->memvailtable);
    FREE(memPool);

    return 0;
}


static void *AllocFromMemoryPool(MemPool_T *memPool)
{
	uint8_t i = 0;
    unsigned int key;
	uint8_t *memvailtable = memPool->memvailtable;

    key = irq_lock();
    
	if(!memPool->freeObjectNum){
        irq_unlock(key);    
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}else{
		for(i = 0; i<memPool->num; i++ ){
			if(memvailtable[i] == MEM_FREE){
				memvailtable[i] = MEM_USING;
				memPool->freeObjectNum--;
				break;
			}
		}
	}

    irq_unlock(key);
    
    ZBAR_MEMPOOL_DEBUG("[AllocFromMemoryPool] size=%d, ptr=0x%x\n", memPool->objSizeAlign4, ((memPool->basePoolAddress+i*(memPool->objSizeAlign4 + MEM_POOL_PTR_SIZE))+MEM_POOL_PTR_SIZE));
	return ((memPool->basePoolAddress+i*(memPool->objSizeAlign4 + MEM_POOL_PTR_SIZE))+MEM_POOL_PTR_SIZE);
}

static void FreeToMemoryPool(uint8_t *ptr)
{
    uint8_t i = 0;
    unsigned int key;
    uint8_t in_pool = 0;

    ZBAR_MEMPOOL_DEBUG("[FreeToMemoryPool] ptr=0x%x\n", ptr);
    
    if(!ptr){
        //ZBAR_MEMPOOL_ERROR("[%s(%d)] ptr is NULL\n", __FUNCTION__, __LINE__);
        return;
    }

    MemPool_T *memPool = *((MemPool_T **)(ptr-MEM_POOL_PTR_SIZE));
    for(i = 0; i< MEM_TYPE_MAX; i++)
    {
        if(g_memPool[i] == memPool)
        {        
            in_pool = 1;
			break;
        }
    }
    if(in_pool == 0)
    {
        ZBAR_MEMPOOL_DEBUG("[%s(%d)] ptr not in pool\n", __FUNCTION__, __LINE__);
        return;
    }
    if(memPool->magicNum != MEM_POOL_MAGIC_NUM){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] memPool not correct\n", __FUNCTION__, __LINE__);
        return;
    }
        
    uint8_t *memvailtable = memPool->memvailtable;
    uint32_t checkaddressBase = (uint32_t)(memPool->basePoolAddress + MEM_POOL_PTR_SIZE);
    uint32_t length = memPool->objSizeAlign4 + MEM_POOL_PTR_SIZE;
    
    for(i = 0; i<memPool->num; i++ ){
    	if(checkaddressBase+i*length == (uint32_t)ptr){
            key = irq_lock();
            if(memvailtable[i] != MEM_FREE)
            {
        		memvailtable[i] = MEM_FREE;
        		memPool->freeObjectNum++;
                #if 0
                if(memPool->freeObjectNum > memPool->num)
                {
                    printf("something wrong!!!\n");
                }
                #endif
            }
            irq_unlock(key);
    		break;
    	}
    }
}


int zbar_mem_init(void)
{
    int ret = 0;

    if(zbar_mempool_init_flat)
    {
        return 0;
    }
    
    g_memPool[MEM_TYPE_64] = CreatMemoryPool(MEM_TYPE_64_NUM, 64);
    if(!g_memPool[MEM_TYPE_64]){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }
    
    g_memPool[MEM_TYPE_128] = CreatMemoryPool(MEM_TYPE_128_NUM, 128);
    if(!g_memPool[MEM_TYPE_128]){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }

    g_memPool[MEM_TYPE_512] = CreatMemoryPool(MEM_TYPE_512_NUM, 512);
    if(!g_memPool[MEM_TYPE_512]){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }

    g_memPool[MEM_TYPE_1K] = CreatMemoryPool(MEM_TYPE_1K_NUM, 1024);
    if(!g_memPool[MEM_TYPE_1K]){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }

    g_memPool[MEM_TYPE_10K] = CreatMemoryPool(MEM_TYPE_10K_NUM, 1024*10);
    if(!g_memPool[MEM_TYPE_10K]){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }

    g_memPool[MEM_TYPE_100K] = CreatMemoryPool(MEM_TYPE_100K_NUM, 1024*100);
    if(!g_memPool[MEM_TYPE_100K]){
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }
	
	g_memPool[MEM_TYPE_300K] = CreatMemoryPool(MEM_TYPE_300K_NUM, 1024*300);
    if(!g_memPool[MEM_TYPE_300K]){
        ZBAR_MEMPOOL_DEBUG("[%s(%d)] fail\n", __FUNCTION__, __LINE__);
        ret = -ENOMEM;
        goto out;
    }

    k_mutex_init(&mempool_mutex);
    
    zbar_mempool_init_flat = 1;

out:
    return ret;
}

/* FIXME: something wrong with the mempool when use dual core, so currently need to clear each time before zbar */
int zbar_mem_clear(void)
{
    g_memPool[MEM_TYPE_64]->freeObjectNum = MEM_TYPE_64_NUM;
    memset(g_memPool[MEM_TYPE_64]->memvailtable, MEM_FREE, MEM_TYPE_64_NUM);

    g_memPool[MEM_TYPE_128]->freeObjectNum = MEM_TYPE_128_NUM;
    memset(g_memPool[MEM_TYPE_128]->memvailtable, MEM_FREE, MEM_TYPE_128_NUM);

    g_memPool[MEM_TYPE_512]->freeObjectNum = MEM_TYPE_512_NUM;
    memset(g_memPool[MEM_TYPE_512]->memvailtable, MEM_FREE, MEM_TYPE_512_NUM);

    g_memPool[MEM_TYPE_1K]->freeObjectNum = MEM_TYPE_1K_NUM;
    memset(g_memPool[MEM_TYPE_1K]->memvailtable, MEM_FREE, MEM_TYPE_1K_NUM);

    g_memPool[MEM_TYPE_10K]->freeObjectNum = MEM_TYPE_10K_NUM;
    memset(g_memPool[MEM_TYPE_10K]->memvailtable, MEM_FREE, MEM_TYPE_10K_NUM);

    g_memPool[MEM_TYPE_100K]->freeObjectNum = MEM_TYPE_100K_NUM;
    memset(g_memPool[MEM_TYPE_100K]->memvailtable, MEM_FREE, MEM_TYPE_100K_NUM);

    g_memPool[MEM_TYPE_300K]->freeObjectNum = MEM_TYPE_300K_NUM;
    memset(g_memPool[MEM_TYPE_300K]->memvailtable, MEM_FREE, MEM_TYPE_300K_NUM);

}

int zbar_mem_deinit(void)
{
    int ret = 0;
    DeleteMemoryPool(g_memPool[MEM_TYPE_64]);
    DeleteMemoryPool(g_memPool[MEM_TYPE_128]);
    DeleteMemoryPool(g_memPool[MEM_TYPE_512]);
    DeleteMemoryPool(g_memPool[MEM_TYPE_1K]);
    DeleteMemoryPool(g_memPool[MEM_TYPE_10K]);
    DeleteMemoryPool(g_memPool[MEM_TYPE_100K]);
	DeleteMemoryPool(g_memPool[MEM_TYPE_300K]);
	zbar_mempool_init_flat = 0;
    return ret;
}

void *zbar_malloc(size_t size)
{
    void *ptr = NULL;

    k_mutex_lock(&mempool_mutex, K_FOREVER);
    
    if(size <= 64)
    {
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_64]);
    }
    else if((size > 64) && (size <= 128))
    {
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_128]);
    }
    else if((size > 128) && (size <= 512))
    {
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_512]);
    }
    else if((size > 512) && (size <= 1024))
    {
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_1K]);
    }
    else if((size > 1024) && (size <= 1024*10))
    {
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_10K]);
    }
    else if((size > 1024*10) && (size <= 1024*100))
    {
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_100K]);
    }
	else if((size > 1024*100) && (size <= 1024*300))
	{
        //ZBAR_MEMPOOL_DEBUG("[%s(%d)] size=%d\n", __FUNCTION__, __LINE__, size);
        ptr = AllocFromMemoryPool(g_memPool[MEM_TYPE_300K]);
    }
    else
    {
        ZBAR_MEMPOOL_ERROR("[zbar_malloc] fail!\n");
        ptr = NULL;
    }

    k_mutex_unlock(&mempool_mutex);

    return ptr;
}

void zbar_free(void *ptr)
{
    k_mutex_lock(&mempool_mutex, K_FOREVER);
    FreeToMemoryPool(ptr);
    k_mutex_unlock(&mempool_mutex);
    return;
}

void *zbar_calloc(size_t nmemb, size_t size)
{
    void *ptr = zbar_malloc(nmemb*size);
    if (ptr != NULL)
    {
        memset((uint8_t *)ptr, 0, nmemb*size);
    }
	return ptr;
}

void *zbar_realloc(void *ptr, size_t requested_size)
{
    uint32_t offset;
    void *ptr_new = NULL;
	if (ptr == NULL)
	{
	    ptr_new = zbar_malloc(requested_size);
		return ptr_new;
	}

    ptr_new = zbar_malloc(requested_size);
    if(ptr_new == NULL)
    {
        ZBAR_MEMPOOL_ERROR("[%s(%d)] fail!\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    else
    {
        memcpy(ptr_new, ptr, requested_size);
        zbar_free(ptr);
		ptr = NULL;
        return ptr_new;
    }
}

#else

int zbar_mem_init(void)
{
    return 0;
}

int zbar_mem_deinit(void)
{
    return 0;
}

int zbar_mem_clear(void)
{
    return 0;
}


void *zbar_malloc(size_t size)
{
    malloc(size);
}

void zbar_free(void *ptr)
{
    free(ptr);
}

void *zbar_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

void *zbar_realloc(void *ptr, size_t requested_size)
{
    return realloc(ptr, requested_size);
}
#endif
