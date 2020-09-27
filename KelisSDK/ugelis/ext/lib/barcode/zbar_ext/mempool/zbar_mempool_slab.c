#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>


#define ZBAR_MEMPOOL_EN 1


#if ZBAR_MEMPOOL_EN

//#define ZBAR_MEMPOOL_DEBUG(...) do{}while(0)
#define ZBAR_MEMPOOL_DEBUG printf

//#define ZBAR_MEMPOOL_ERROR(...) do{}while(0)
#define ZBAR_MEMPOOL_ERROR printf

#define EVAN_DEBUG(...) do{}while(0)
//#define EVAN_DEBUG printf
int zbar_malloc_cnt = 0;
int zbar_free_cnt = 0;
int zbar_mempool_free_num = 0;


struct k_mutex mempool_mutex;

#define MEM_TYPE_64_NUM   20
#define MEM_TYPE_128_NUM  20
#define MEM_TYPE_512_NUM  20
#define MEM_TYPE_1K_NUM   10
#define MEM_TYPE_10K_NUM  10
#define MEM_TYPE_100K_NUM 2

typedef enum
{
    MEM_TYPE_64,
    MEM_TYPE_128,
    MEM_TYPE_512,
    MEM_TYPE_1K,
    MEM_TYPE_10K,
    MEM_TYPE_100K,
    MEM_TYPE_MAX
}MemPoolType_T;


#define MEM_POOL_MAGIC_NUM 0x1A2B3C4D


typedef struct
{
    struct k_mem_slab *slab;
    uint32_t flag;
    uint8_t buf[64];
}mempool_64_obj_t;

typedef struct
{
    struct k_mem_slab *slab;
    uint32_t flag;
    uint8_t buf[128];
}mempool_128_obj_t;

typedef struct
{
    struct k_mem_slab *slab;
    uint32_t flag;
    uint8_t buf[512];
}mempool_512_obj_t;

typedef struct
{
    struct k_mem_slab *slab;
    uint32_t flag;
    uint8_t buf[1024];
}mempool_1K_obj_t;

typedef struct
{
    struct k_mem_slab *slab;
    uint32_t flag;
    uint8_t buf[1024*10];
}mempool_10K_obj_t;

typedef struct
{
    struct k_mem_slab *slab;
    uint32_t flag;
    uint8_t buf[1024*100];
}mempool_100K_obj_t;



#define ZBAR_WAIT_BUF_TIMEOUT                  3 /*unint s*/


K_MEM_SLAB_DEFINE(zbar_mempool_64, sizeof(mempool_64_obj_t), MEM_TYPE_64_NUM, 4);
K_MEM_SLAB_DEFINE(zbar_mempool_128, sizeof(mempool_128_obj_t), MEM_TYPE_128_NUM, 4);
K_MEM_SLAB_DEFINE(zbar_mempool_512, sizeof(mempool_512_obj_t), MEM_TYPE_512_NUM, 4);
K_MEM_SLAB_DEFINE(zbar_mempool_1K, sizeof(mempool_1K_obj_t), MEM_TYPE_1K_NUM, 4);
K_MEM_SLAB_DEFINE(zbar_mempool_10K, sizeof(mempool_10K_obj_t), MEM_TYPE_10K_NUM, 4);
K_MEM_SLAB_DEFINE(zbar_mempool_100K, sizeof(mempool_100K_obj_t), MEM_TYPE_100K_NUM, 4);


static uint8_t zbar_mempool_init_flat = 0;
int zbar_mem_init(void)
{
    if(zbar_mempool_init_flat)
    {
        return 0;
    }
	k_mutex_init(&mempool_mutex);

    printf("&zbar_mempool_64=0x%x\n", &zbar_mempool_64);
    printf("&zbar_mempool_128=0x%x\n", &zbar_mempool_128);
    printf("&zbar_mempool_512=0x%x\n", &zbar_mempool_512);
    printf("&zbar_mempool_1K=0x%x\n", &zbar_mempool_1K);
    printf("&zbar_mempool_10K=0x%x\n", &zbar_mempool_10K);
    printf("&zbar_mempool_100K=0x%x\n", &zbar_mempool_100K);
    
    zbar_mempool_init_flat = 1;
    return 0;
}

int zbar_mem_deinit(void)
{
	zbar_mempool_init_flat = 0;
    return 0;
}

void *zbar_malloc(size_t size)
{
    void *ptr = NULL;
    mempool_64_obj_t *mempool_64_obj;
    mempool_128_obj_t *mempool_128_obj;
    mempool_512_obj_t *mempool_512_obj;
    mempool_1K_obj_t *mempool_1K_obj;
    mempool_10K_obj_t *mempool_10K_obj;
    mempool_100K_obj_t *mempool_100K_obj;

    unsigned int key = irq_lock();
    
    if(size <= 64)
    {
        if (k_mem_slab_alloc(&zbar_mempool_64, (void **)&mempool_64_obj, K_SECONDS(ZBAR_WAIT_BUF_TIMEOUT)))
        {
            ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc form zbar_mempool_64 fail!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            mempool_64_obj->slab = &zbar_mempool_64;
            mempool_64_obj->flag = MEM_POOL_MAGIC_NUM;
            ptr = (void *)mempool_64_obj->buf;
            ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, mempool_64_obj->slab, ptr);
        }
    }
    else if((size > 64) && (size <= 128))
    {
        if (k_mem_slab_alloc(&zbar_mempool_128, (void **)&mempool_128_obj, K_SECONDS(ZBAR_WAIT_BUF_TIMEOUT)))
        {
            ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc form zbar_mempool_128 fail!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            mempool_128_obj->slab = &zbar_mempool_128;
            mempool_128_obj->flag = MEM_POOL_MAGIC_NUM;
            ptr = (void *)mempool_128_obj->buf;
            ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, mempool_128_obj->slab, ptr);
        }
    }
    else if((size > 128) && (size <= 512))
    {
        if (k_mem_slab_alloc(&zbar_mempool_512, (void **)&mempool_512_obj, K_SECONDS(ZBAR_WAIT_BUF_TIMEOUT)))
        {
            ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc form zbar_mempool_512 fail!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            mempool_512_obj->slab = &zbar_mempool_512;
            mempool_512_obj->flag = MEM_POOL_MAGIC_NUM;
            ptr = (void *)mempool_512_obj->buf;
            ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, mempool_512_obj->slab, ptr);
        }
    }
    else if((size > 512) && (size <= 1024))
    {
        if (k_mem_slab_alloc(&zbar_mempool_1K, (void **)&mempool_1K_obj, K_SECONDS(ZBAR_WAIT_BUF_TIMEOUT)))
        {
            ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc form zbar_mempool_1K fail!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            mempool_1K_obj->slab = &zbar_mempool_1K;
            mempool_1K_obj->flag = MEM_POOL_MAGIC_NUM;
            ptr = (void *)mempool_1K_obj->buf;
            ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, mempool_1K_obj->slab, ptr);
        }
    }
    else if((size > 1024) && (size <= 1024*10))
    {
        if (k_mem_slab_alloc(&zbar_mempool_10K, (void **)&mempool_10K_obj, K_SECONDS(ZBAR_WAIT_BUF_TIMEOUT)))
        {
            ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc form zbar_mempool_10K fail!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            mempool_10K_obj->slab = &zbar_mempool_10K;
            mempool_10K_obj->flag = MEM_POOL_MAGIC_NUM;
            ptr = (void *)mempool_10K_obj->buf;
            ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, mempool_10K_obj->slab, ptr);
        }
    }
    else if((size > 1024*10) && (size <= 1024*100))
    {
        if (k_mem_slab_alloc(&zbar_mempool_100K, (void **)&mempool_100K_obj, K_SECONDS(ZBAR_WAIT_BUF_TIMEOUT)))
        {
            ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc form zbar_mempool_100K fail!\n", __FUNCTION__, __LINE__);
        }
        else
        {
            mempool_100K_obj->slab = &zbar_mempool_100K;
            mempool_100K_obj->flag = MEM_POOL_MAGIC_NUM;
            ptr = (void *)mempool_100K_obj->buf;
            ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, mempool_100K_obj->slab, ptr);
        }
    }
    else
    {
        ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: alloc unknown size fail!\n", __FUNCTION__, __LINE__);
    }

    irq_unlock(key);

    return ptr;
}

void zbar_free(void *ptr)
{
    uint32_t flag = 0;
    uint32_t mempool_x_obj_addr = 0;
    uint32_t *mempool_x_obj = NULL;
    struct k_mem_slab *slab = NULL;

    unsigned int key = irq_lock();
    
    flag = *(uint32_t *)((uint32_t)ptr - sizeof(slab));
    if(flag != MEM_POOL_MAGIC_NUM)
    {
        ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: free mem(0x%x) not in zbar mempool!\n", __FUNCTION__, __LINE__, ptr);
        k_mutex_unlock(&mempool_mutex);
        return;
    }
    
    mempool_x_obj_addr = ((uint32_t)ptr - sizeof(uint32_t) - sizeof(slab));
    mempool_x_obj = (uint32_t *)mempool_x_obj_addr;
    slab = (struct k_mem_slab *)(*(uint32_t *)mempool_x_obj_addr);
    if((slab == &zbar_mempool_64) || (slab == &zbar_mempool_128) || 
        (slab == &zbar_mempool_512) || (slab == &zbar_mempool_1K) || 
        (slab == &zbar_mempool_10K) || (slab == &zbar_mempool_100K))
    {
        ZBAR_MEMPOOL_DEBUG("[%s(%d)] slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, slab, ptr);
        k_mem_slab_free(slab, (void **)&mempool_x_obj);
    }
    else
    {
        ZBAR_MEMPOOL_ERROR("[%s(%d)] Err: free mem not correct! slab=0x%x, ptr=0x%x\n", __FUNCTION__, __LINE__, slab, ptr);
    }

    irq_unlock(key);

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

void zbar_mem_test()
{
    int i = 0;
    char *ptr[50] = {NULL};
    uint32_t time_1, time_2;

    if(zbar_mem_init()  != 0)
    {
        printf("zbar_mem_init fail\n");
    }

    while(1)
    {
        time_1 = k_uptime_get_32();
        for(i=0; i<MEM_TYPE_64_NUM; i++)
        {
            ptr[i] = zbar_malloc(64);
        }
        for(i=0; i<MEM_TYPE_64_NUM; i++)
        {
            zbar_free(ptr[i]);
            ptr[i] = NULL;
        }
        
        for(i=0; i<MEM_TYPE_512_NUM; i++)
        {
            ptr[i] = zbar_malloc(512);
        }
        for(i=0; i<MEM_TYPE_512_NUM; i++)
        {
            zbar_free(ptr[i]);
            ptr[i] = NULL;
        }

        for(i=0; i<MEM_TYPE_1K_NUM; i++)
        {
            ptr[i] = zbar_malloc(1024);
        }
        for(i=0; i<MEM_TYPE_1K_NUM; i++)
        {
            zbar_free(ptr[i]);
            ptr[i] = NULL;
        }

        for(i=0; i<MEM_TYPE_10K_NUM; i++)
        {
            ptr[i] = zbar_malloc(1024*10);
        }
        for(i=0; i<MEM_TYPE_10K_NUM; i++)
        {
            zbar_free(ptr[i]);
            ptr[i] = NULL;
        }

        for(i=0; i<MEM_TYPE_100K_NUM; i++)
        {
            ptr[i] = zbar_malloc(1024*100);
        }
        for(i=0; i<MEM_TYPE_100K_NUM; i++)
        {
            zbar_free(ptr[i]);
            ptr[i] = NULL;
        }

        time_2 = k_uptime_get_32();
        printf("========================\n%d\n========================\n", time_2-time_1);
    }
    
    return;
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
