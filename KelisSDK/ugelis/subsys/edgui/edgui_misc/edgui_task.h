/**
 * @file edgui_task.c
 * An 'edgui_task'  is a void (*fp) (void* param) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to edgui_tasks.
 */

#ifndef EDGUI_TASK_H
#define EDGUI_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include "edgui_mem.h"
#include "edgui_ll.h"

/*********************
 *      DEFINES
 *********************/
#ifndef EDGUI_ATTRIBUTE_TASK_HANDLER
#define EDGUI_ATTRIBUTE_TASK_HANDLER
#endif
/**********************
 *      TYPEDEFS
 **********************/

struct _edgui_task_t;

/**
 * Tasks execute this type type of functions.
 */
typedef void (*edgui_task_cb_t)(struct _edgui_task_t *);

/**
 * Possible priorities for edgui_tasks
 */
enum {
    EDGUI_TASK_PRIO_OFF = 0,
    EDGUI_TASK_PRIO_LOWEST,
    EDGUI_TASK_PRIO_LOW,
    EDGUI_TASK_PRIO_MID,
    EDGUI_TASK_PRIO_HIGH,
    EDGUI_TASK_PRIO_HIGHEST,
    _EDGUI_TASK_PRIO_NUM,
};
typedef uint8_t edgui_task_prio_t;

/**
 * Descriptor of a edgui_task
 */
typedef struct _edgui_task_t
{
    uint32_t period; /**< How often the task should run */
    uint32_t last_run; /**< Last time the task ran */
    edgui_task_cb_t task_cb; /**< Task function */

    void * user_data; /**< Custom user data */

    uint8_t prio : 3; /**< Task priority */
    uint8_t once : 1; /**< 1: one shot task */
} edgui_task_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the edgui_task module
 */
void edgui_task_core_init(void);

//! @cond Doxygen_Suppress

/**
 * Call it  periodically to handle edgui_tasks.
 */
EDGUI_ATTRIBUTE_TASK_HANDLER void edgui_task_handler(void);

//! @endcond

/**
 * Create an "empty" task. It needs to initialzed with at least
 * `edgui_task_set_cb` and `edgui_task_set_period`
 * @return pointer to the craeted task
 */
edgui_task_t * edgui_task_create_basic(void);

/**
 * Create a new edgui_task
 * @param task_xcb a callback which is the task itself. It will be called periodically.
 *                 (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param period call period in ms unit
 * @param prio priority of the task (EDGUI_TASK_PRIO_OFF means the task is stopped)
 * @param user_data custom parameter
 * @return pointer to the new task
 */
edgui_task_t * edgui_task_create(edgui_task_cb_t task_xcb, uint32_t period, edgui_task_prio_t prio, void * user_data);

/**
 * Delete a edgui_task
 * @param task pointer to task_cb created by task
 */
void edgui_task_del(edgui_task_t * task);

/**
 * Set the callback the task (the function to call periodically)
 * @param task pointer to a task
 * @param task_cb the function to call periodically
 */
void edgui_task_set_cb(edgui_task_t * task, edgui_task_cb_t task_cb);

/**
 * Set new priority for a edgui_task
 * @param task pointer to a edgui_task
 * @param prio the new priority
 */
void edgui_task_set_prio(edgui_task_t * task, edgui_task_prio_t prio);

/**
 * Set new period for a edgui_task
 * @param task pointer to a edgui_task
 * @param period the new period
 */
void edgui_task_set_period(edgui_task_t * task, uint32_t period);

/**
 * Make a edgui_task ready. It will not wait its period.
 * @param task pointer to a edgui_task.
 */
void edgui_task_ready(edgui_task_t * task);

/**
 * Delete the edgui_task after one call
 * @param task pointer to a edgui_task.
 */
void edgui_task_once(edgui_task_t * task);

/**
 * Reset a edgui_task.
 * It will be called the previously set period milliseconds later.
 * @param task pointer to a edgui_task.
 */
void edgui_task_reset(edgui_task_t * task);

/**
 * Enable or disable the whole  edgui_task handling
 * @param en: true: edgui_task handling is running, false: edgui_task handling is suspended
 */
void edgui_task_enable(bool en);

/**
 * Get idle percentage
 * @return the edgui_task idle in percentage
 */
uint8_t edgui_task_get_idle(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
