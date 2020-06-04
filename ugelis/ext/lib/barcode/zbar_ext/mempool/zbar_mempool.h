
#ifndef _ZBAR_MEMPOOL_H_
#define _ZBAR_MEMPOOL_H_


#ifdef __cplusplus
extern "C" {
#endif



#include <ugelis/types.h>

int zbar_mem_init(void);
int zbar_mem_deinit(void);
int zbar_mem_clear(void);
void *zbar_malloc(size_t size);
void zbar_free(void *ptr);
void *zbar_calloc(size_t nmemb, size_t size);
void *zbar_realloc(void *ptr, size_t requested_size);





#ifdef __cplusplus
}
#endif

#endif /* _ZBAR_MEMPOOL_H_ */

















