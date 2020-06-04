
#ifndef _LOAD_COMPRESS_IMAGEA_INTERNAL_H_
#define _LOAD_COMPRESS_IMAGEA_INTERNAL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis.h>
#include <misc/printk.h>
#include <misc/dlist.h>


//#define DEBUG_THREAD_TIME 1
//#define LDIMG_DEBUG 1

#define LDIMG_ERR   1


#ifndef LDIMG_DEBUG
#define ldimg_debug(...)      do {} while (0)
#else
#define ldimg_debug           printk
#endif

#ifndef LDIMG_ERR
#define ldimg_err(...)          do {} while (0)
#else
#define ldimg_err               printk
#endif

#define ldimg_malloc(size)          malloc_ext(0,(size))



#define FLASH_WAIT_BUF_TIMEOUT                  3 /*unint s*/
#define UNCOMPRESS_WAIT_BUF_TIMEOUT             3 /*uint  s*/

#define MAX_COMPRESS_IMAGE_OBJECT_NUM           8
#define  IMAGE_COMPRESS_PER_LENGTH              (64*1024+20)


/*todo for irqlock*/
#define LDIM_GET_HANDLER()                      (g_handle)

#define LDIM_SET_HANDLER(handler)               do{                         \
        g_handle = handler;     \
    }while(0);


struct compress_section_desc
{
    unsigned int index;
    unsigned int offset; /*compress image offset*/
    unsigned int length; /*real compress length*/
};





struct compress_head
{
    unsigned int size; /*head image size*/
    unsigned int compress_num;
    unsigned int section_desc_offset;
    struct compress_section_desc *section_desc; /*not write to file*/
};

/*this struct will not install*/
struct image_object
{
    sys_dlist_t node;
    uint8_t compress_image[IMAGE_COMPRESS_PER_LENGTH];

};

enum sync_event_type
{
    IMAGE_NOTIFY_TO_FLASH_THREAD            = 0,
    IMAGE_NOTIFY_TO_UNCOMPRESS_THREAD,
    FLASH_THREAD_NOTIFY_IAMGE,
    UNCOMPRESS_THREAD_NOTIFY_IAMGE,
};


#define EVENT_NUM (UNCOMPRESS_THREAD_NOTIFY_IAMGE + 1)


enum singal_status
{
    LDIMAGE_SIANGAL_SUCCESS              = 0,
    LDIMAGE_SIANGAL_FAIL,
};


struct ldim_handler
{
    struct device           *flash_dev;
    uint32_t                 image_flash_offset;
    /*for read for filesystem*/
    char                    *file;

    /*ringbuf headlist*/
    sys_dlist_t             head_list;

    /*uncpmpress*/
    struct compress_head head;
    uint8_t *uncompress;

    /*sigal event_signals*/
    struct k_poll_signal  event_signals[EVENT_NUM];
    struct k_poll_event   wait_events[EVENT_NUM];

    /*thead*/
    struct k_mutex          ldimg_mutex;
    k_tid_t                 read_flash_thread_id;
    k_tid_t                 uncompress_thread_id;
    struct k_thread         read_flash_thread;
    struct k_thread         uncompress_thread;
    struct k_sem            thread_sync_sem;
};


static inline void ldimg_sys_dlist_append(sys_dlist_t *list, sys_dnode_t *node)
{
    unsigned int key = irq_lock();
    sys_dlist_append(list, node);
    irq_unlock(key);
}


static inline sys_dlist_t *ldimg_sys_dlist_get(sys_dlist_t *list)
{
    sys_dnode_t *node = NULL;

    unsigned int key = irq_lock();
    node = sys_dlist_get(list);
    irq_unlock(key);

    return node;
}



#ifdef __cplusplus
}
#endif

#endif /* _LOAD_COMPRESS_IMAGEA_INTERNAL_H_ */




