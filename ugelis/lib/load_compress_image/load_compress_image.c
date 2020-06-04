#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <stddef.h>
#include <device.h>
#include <init.h>
#include "load_compress_image_internal.h"
#include <device.h>
#include <board.h>
#include <gm_hal_sqi.h>
#include <gm_common.h>
#include <misc/util.h>


static struct ldim_handler *g_handle;
/*for flash temp*/
static struct SQI_Handle  gSQIHandle;
static struct SQI_Config  gSQIConfig;


K_MEM_SLAB_DEFINE(compress_image_pool, sizeof(struct image_object),
                  MAX_COMPRESS_IMAGE_OBJECT_NUM, 4);


#define     LDIMG_READ_FLASH_THREAE_STACK_SIZE    1024
K_THREAD_STACK_DEFINE(read_flash_thread_stack, LDIMG_READ_FLASH_THREAE_STACK_SIZE);

#define     LDIMG_UNCOMPRESS_THREAD_STACK_SIZE    1024
K_THREAD_STACK_DEFINE(uncompress_thread_stack, LDIMG_UNCOMPRESS_THREAD_STACK_SIZE);


static int ldimg_read_flash(uint32_t address, char *buf, uint32_t len);

static int ldimg_uncompress_image(uint8_t *compress, uint32_t compress_len,
                                  uint8_t *uncompress, uint32_t *uncompress_len);
static int ldimg_init_thread_sync_event(struct ldim_handler *g_handle);

static int ldimg_init(struct device *dev);

static int ldimg_send_signal(enum sync_event_type event_type, enum singal_status status);

static int ldima_wait_event(struct ldim_handler *handler, enum sync_event_type type);
static int ldimag_triger_2thread_to_start();

static int ldimag_wait_2thread_finish(struct ldim_handler *handler);

static void ldimg_read_flash_thread(void *p1, void *p2, void *p3);
static void ldimg_uncompress_thread(void *p1, void *p2, void *p3);


static int ldimg_init_flash()
{
    int ret = 0;
    ret = HAL_SQI_Init(&gSQIHandle, (SQI_Device_T *)EFLASH_CTRL_BASE, NULL);
    return ret;
}


static int ldimg_read_flash(uint32_t offset, char *buf, uint32_t len)
{
    int ret = 0;

    /*flash read*/
    ret = HAL_SQI_Read(&gSQIHandle, offset, (void *)buf, len);
    if (ret)
    {
        ldimg_err("[%s] Err HAL_SQI_Read\n", __FUNCTION__);
    }
out:
    return ret;
}


int ldimg_uncompress_image(uint8_t *compress, uint32_t compress_len,
                           uint8_t *uncompress, uint32_t *uncompress_len)
{
    int ret = 0;

    ret = img_uncompress(uncompress, uncompress_len, compress, compress_len);

    return ret;
}

static int ldimg_init_thread_sync_event(struct ldim_handler *g_handle)
{
    uint8_t i ;
    struct k_poll_signal *event_signals = g_handle->event_signals;
    struct k_poll_event *wait_events = g_handle->wait_events;

    for (i = 0; i < EVENT_NUM; i++)
    {
        k_poll_signal_init(&event_signals[i]);
        k_poll_event_init(&wait_events[i], K_POLL_TYPE_SIGNAL, K_POLL_MODE_NOTIFY_ONLY, &event_signals[i]);
    }

}


static int ldimg_init(struct device *dev)
{
    int ret = 0;
    struct ldim_handler *handler = NULL;

    handler = ldimg_malloc(sizeof(struct ldim_handler));
    if (!handler)
    {
        ldimg_err("%s ldimg_malloc Err\n", __FUNCTION__);
        return  -ENOMEM;
    }
    LDIM_SET_HANDLER(handler);

    /*init head list*/
    sys_dlist_init(&handler->head_list);


    /*creat mutex*/
    k_mutex_init(&handler->ldimg_mutex);
    /*init singal envet*/
    ldimg_init_thread_sync_event(handler);


    /*creat two task*/

    handler->read_flash_thread_id  = k_thread_create(&(handler->read_flash_thread), read_flash_thread_stack,
                                     K_THREAD_STACK_SIZEOF(read_flash_thread_stack),
                                     ldimg_read_flash_thread, handler, NULL, NULL, K_PRIO_PREEMPT(0),
                                     0, K_NO_WAIT);


    handler->uncompress_thread_id  =  k_thread_create(&(handler->uncompress_thread), uncompress_thread_stack,
                                      K_THREAD_STACK_SIZEOF(uncompress_thread_stack),
                                      ldimg_uncompress_thread, handler, NULL, NULL, K_PRIO_PREEMPT(1),
                                      0, K_NO_WAIT);

out:
    return ret;
}


static int ldimg_send_signal(enum sync_event_type event_type, enum singal_status status)
{
    int ret = 0;
    struct ldim_handler *handler = LDIM_GET_HANDLER();
    struct k_poll_signal *signal = (struct k_poll_signal *)(handler->wait_events[event_type].obj);

    ret = k_poll_signal(signal, status);

    return ret;
}




static int ldima_wait_event(struct ldim_handler *handler, enum sync_event_type type)
{
    int ret = 0;

    struct k_poll_event *event = &(handler->wait_events[type]);
    struct k_poll_signal *signal = NULL;
    enum singal_status status = LDIMAGE_SIANGAL_FAIL;

    k_poll(event, 1, K_FOREVER);
    if (event->state == K_POLL_STATE_SIGNALED)
    {
        signal = (struct k_poll_signal *)event->obj;
        status = (enum singal_status)signal->result;
    }
    else
    {
        ldimg_err("%s singal Imp Err\n", __FUNCTION__);
        ret =  -EINVAL;
        goto out;
    }

    if (status == LDIMAGE_SIANGAL_FAIL)
    {
        ret = -EINVAL;
    }

out:
    /*clear for next time wait event*/
    signal->signaled = 0;
    event->state = K_POLL_STATE_NOT_READY;
    return ret;

}

static int ldimag_triger_2thread_to_start()
{
    int ret = 0;
    ret = ldimg_send_signal(IMAGE_NOTIFY_TO_FLASH_THREAD, LDIMAGE_SIANGAL_SUCCESS);
    if (ret)
    {
        goto out;
    }
    ret = ldimg_send_signal(IMAGE_NOTIFY_TO_UNCOMPRESS_THREAD, LDIMAGE_SIANGAL_SUCCESS);

out:
    return ret;
}


static int ldimag_wait_2thread_finish(struct ldim_handler *handler)
{
    int ret = 0;

    ret = ldima_wait_event(handler, FLASH_THREAD_NOTIFY_IAMGE);
    if (ret)
    {
        goto out;
    }
    /*wait uncompress data finish singal*/
    ret = ldima_wait_event(handler, UNCOMPRESS_THREAD_NOTIFY_IAMGE);
    if (ret)
    {
        goto out;
    }

    ldimg_debug("\nimage wait 2thread ok\n");

out:
    return ret;
}



int ldimg_flash_compress_image(uint32_t image_flash_offset, uint8_t *uncompress)
{
    /*readimge head*/
    int ret = 0;
    uint32_t i = 0;
    struct compress_head *head = NULL;
    unsigned int compress_num;
    unsigned int section_desc_offset;
    struct compress_section_desc *section_desc;
    struct ldim_handler *handler;

    handler = LDIM_GET_HANDLER();
    k_mutex_lock(&handler->ldimg_mutex, K_FOREVER);

    handler->uncompress = uncompress;
    head = &(handler->head);
    handler->image_flash_offset = image_flash_offset;

    k_sem_init(&handler->thread_sync_sem, 0, MAX_COMPRESS_IMAGE_OBJECT_NUM);

    ret = ldimg_init_flash();
    if (ret)
    {
        ldimg_err("[%s] Err HAL_SQI_Init\n", __FUNCTION__);
        goto out;
    }

    ret = ldimg_read_flash(image_flash_offset, (uint8_t *)head, sizeof(struct compress_head));
    if (ret)
    {
        ldimg_err("[%s] Err ldimg_read_flash\n", __FUNCTION__, ret);
        goto out;
    }


    head->section_desc = ldimg_malloc(sizeof(struct compress_section_desc) * (head->compress_num));
    if (!head->section_desc)
    {
        ret = -ENOMEM;
        goto out;
    }

    /*init flash*/


    ret = ldimg_read_flash(head->section_desc_offset + image_flash_offset, (uint8_t *)(head->section_desc),
                           (sizeof(struct compress_section_desc) * (head->compress_num)));
    if (ret)
    {
        ldimg_err("[%s] Err ldimg_read_flash\n", __FUNCTION__, ret);
        goto out;
    }

    ret = ldimag_triger_2thread_to_start();
    if (ret)
    {
        ldimg_err("Err:%d ldimag_triger_2thread_to_process\n", ret);
        goto out;
    }

    ret = ldimag_wait_2thread_finish(handler);
    if (ret)
    {
        ldimg_err("Err:%d ldimag_wait_2thread_finish\n", ret);
        goto out;

    }


out:

    k_mutex_unlock(&handler->ldimg_mutex);

    return ret;
}


static void ldimg_read_flash_thread(void *p1, void *p2, void *p3)
{
    uint32_t i = 0;
    int ret = 0;
    uint8_t *compress_buf = NULL;
    struct image_object *image_object;
    struct ldim_handler *handler = (struct ldim_handler *)p1;
    uint8_t list_size = sizeof(sys_dlist_t);
    uint32_t compress_num;// = handler->head.compress_num;
    struct k_sem *sem = &(handler->thread_sync_sem);
    struct compress_section_desc *section_desc = NULL;//handler->head.section_desc;
    sys_dlist_t *head_list = &handler->head_list;
    enum singal_status status = LDIMAGE_SIANGAL_SUCCESS;
#ifdef DEBUG_THREAD_TIME
    int cnt = 0;
    unsigned int t_now, t_last, usetime;
#endif

    do
    {
        /*wait singal:image->flash_thread start*/
        ret = ldima_wait_event(handler, IMAGE_NOTIFY_TO_FLASH_THREAD);
        if (ret)
        {
            status = LDIMAGE_SIANGAL_FAIL;
            goto out;
        }
        else
        {
            status = LDIMAGE_SIANGAL_SUCCESS;
        }

        compress_num = handler->head.compress_num;
        section_desc = handler->head.section_desc;



        /*read compress image from flash*/
        for (i = 0; i < compress_num; i++)
        {

#ifdef DEBUG_THREAD_TIME
            t_last = k_uptime_get_32();
#endif

            /*get free buf*/
            if (k_mem_slab_alloc(&compress_image_pool, (void **)&image_object, K_SECONDS(FLASH_WAIT_BUF_TIMEOUT)))
            {
                ldimg_err("Err:alloc form compress_image_pool timeout\n");
                status = LDIMAGE_SIANGAL_FAIL;
                goto out;
            }

            compress_buf = (uint8_t *)((uint32_t)image_object + list_size);

            ret = ldimg_read_flash(section_desc[i].offset + handler->image_flash_offset, compress_buf, section_desc[i].length);
            if (ret)
            {
                status = LDIMAGE_SIANGAL_FAIL;
                goto out;
            }
#ifdef DEBUG_THREAD_TIME
            t_now = k_uptime_get_32();
            ldimg_debug("\n##[%s] generate cnt:%d image_object:%p costMs:%d\n ", __FUNCTION__, ++cnt, image_object, (t_now - t_last));
#endif
            /*add to list*/
            ldimg_sys_dlist_append(head_list, &image_object->node);

            /*realse sync sempore*/
            k_sem_give(sem);


        }
out:
        /*send singal->main thread finish*/
        ret = ldimg_send_signal(FLASH_THREAD_NOTIFY_IAMGE, status);
        if (ret)
        {
            ldimg_err("Err:%d send signal FLASH_THREAD_NOTIFY_IAMGE\n", ret);

        }
    } while (1);
}

static void ldimg_uncompress_thread(void *p1, void *p2, void *p3)
{
    uint32_t i = 0;
    int ret = 0;
    uint8_t *compress_buf = NULL;
    struct image_object *image_object = NULL;
    struct ldim_handler *handler = (struct ldim_handler *)p1;
    uint8_t list_size = sizeof(sys_dlist_t);
    uint32_t compress_num ;//= handler->head.compress_num;
    struct k_sem *sem = &(handler->thread_sync_sem);
    struct compress_section_desc *section_desc = NULL;//handler->head.section_desc;
    sys_dlist_t *head_list = &handler->head_list;
    sys_dlist_t *node = NULL;
    uint8_t *uncompress = handler->uncompress;
    uint32_t uncompress_len = 0;
    enum singal_status status = LDIMAGE_SIANGAL_SUCCESS;

#ifdef DEBUG_THREAD_TIME
    int cnt = 0;
    unsigned int t_now, t_last, usetime;
#endif


    do
    {
        /*wait singal:image->uncompress thead start*/
        ret = ldima_wait_event(handler, IMAGE_NOTIFY_TO_UNCOMPRESS_THREAD);
        if (ret)
        {
            status = LDIMAGE_SIANGAL_FAIL;
            goto out;
        }
        else
        {
            status = LDIMAGE_SIANGAL_SUCCESS;
        }

        compress_num =  handler->head.compress_num;
        section_desc =  handler->head.section_desc;


        /*read compress image from flash*/
        for (i = 0; i < compress_num; i++)
        {



            /*wait date to uncompress*/
            if (ret = k_sem_take(sem, K_SECONDS(FLASH_WAIT_BUF_TIMEOUT)))
            {
                ldimg_err("Err:ldimg_uncompress_thread wait uncompress data timeout\n");
                status = LDIMAGE_SIANGAL_FAIL;
                goto out;
            }
            /*get data and uncompress it */
            node = ldimg_sys_dlist_get(head_list);
            if (!node)
            {
                ldimg_err("Err:sync sem thread\n");
                status = LDIMAGE_SIANGAL_FAIL;
                goto out;
            }

            image_object = (struct image_object *)node;

            uncompress_len = IMAGE_COMPRESS_PER_LENGTH;
#ifdef DEBUG_THREAD_TIME
            t_last = k_uptime_get_32();
#endif
            ret = ldimg_uncompress_image(image_object->compress_image,
                                         section_desc[i].length, uncompress, &uncompress_len);
            if (ret)
            {
                ldimg_err("Err:%d ldimg_uncompress_image\n", ret);
                status = LDIMAGE_SIANGAL_FAIL;
                goto out;
            }
            /*for next loop*/
            uncompress = uncompress + uncompress_len;
#ifdef DEBUG_THREAD_TIME
            t_now = k_uptime_get_32();
            ldimg_debug("\n[%s] free cnt:%d image_object:%p costMs:%d\n ", __FUNCTION__, ++cnt, image_object, (t_now - t_last));
#endif
            k_mem_slab_free(&compress_image_pool, (void **)&image_object);



        }

out:
        /*send singal->main thread finish*/
        ret = ldimg_send_signal(UNCOMPRESS_THREAD_NOTIFY_IAMGE, status);
        if (ret)
        {
            ldimg_err("Err:%d send signal UNCOMPRESS_THREAD_NOTIFY_IAMGE\n", ret);
        }

    } while (1);
}



SYS_INIT(ldimg_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);


