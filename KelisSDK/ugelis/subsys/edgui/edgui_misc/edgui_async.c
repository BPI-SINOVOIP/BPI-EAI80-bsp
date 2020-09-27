/**
 * @file edgui_async.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_async.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void edgui_async_task_cb(edgui_task_t *task);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

edgui_res_t edgui_async_call(edgui_async_cb_t async_xcb, void * user_data)
{
    /*Allocate an info structure */
    edgui_async_info_t *info = edgui_mem_alloc(sizeof(edgui_async_info_t));
    
    if(info == NULL)
        return EDGUI_RES_INV;
    
    /* Create a new task */
    /* Use highest priority so that it will run before a refresh */
    edgui_task_t *task = edgui_task_create(edgui_async_task_cb, 0, EDGUI_TASK_PRIO_HIGHEST, info);
    
    if(task == NULL) {
        edgui_mem_free(info);
        return EDGUI_RES_INV;
    }
    
    info->cb = async_xcb;
    info->user_data = user_data;

    /* Set the task's user data */
    task->user_data = info;
    edgui_task_once(task);
    return EDGUI_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void edgui_async_task_cb(edgui_task_t *task)
{
    edgui_async_info_t *info = (edgui_async_info_t *)task->user_data;
    
    info->cb(info->user_data);
    
    edgui_mem_free(info);
}
