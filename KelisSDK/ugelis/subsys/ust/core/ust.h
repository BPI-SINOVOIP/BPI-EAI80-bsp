
#ifndef _UST_H_
#define _UST_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis.h>
#include <misc/printk.h>


//#define UST_DEBUG 1
#define UST_ERR   1

#ifdef UST_DEBUG
    #define ust_pr_debug       printk
#else
    #define ust_pr_debug(...)  do{}while(0)
#endif

#ifdef UST_ERR
    #define ust_pr_err         printk
#else
    #define ust_pr_err(...)    do{}while(0)
#endif


static inline void *UST_MALLOCZ(uint32_t size)
{
	void *ptr = NULL;

	ptr = malloc_ext(0, size);
	if(!ptr){
		return NULL;
	}

	memset(ptr, 0, size);

	return  ptr;
}

static inline void UST_FREE(void *ptr)
{
	free_ext(0,ptr);
}


#ifdef __cplusplus
}
#endif

#endif /* _UST_H_ */




