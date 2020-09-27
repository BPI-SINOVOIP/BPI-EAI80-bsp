/**
 * @file edgui_async.h
 *
 */

#ifndef EDGUI_ASYNC_H
#define EDGUI_ASYNC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "edgui_task.h"
#include "edgui_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Type for async callback.
 */
typedef void (*edgui_async_cb_t)(void *);

typedef struct _edgui_async_info_t {
    edgui_async_cb_t cb;
    void *user_data;
} edgui_async_info_t;

struct _edgui_obj_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Call an asynchronous function the next time edgui_task_handler() is run. This function is likely to return
 * **before** the call actually happens!
 * @param task_xcb a callback which is the task itself.
 *                 (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param user_data custom parameter
 */
edgui_res_t edgui_async_call(edgui_async_cb_t async_xcb, void * user_data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEMPL_H*/
