/**
 * @file edgui_task.c
 * An 'edgui_task'  is a void (*fp) (void* param) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to edgui_tasks.
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "edgui_task.h"
#include "../edgui_hal/edgui_hal_tick.h"
#include "edgui_gc.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define IDLE_MEAS_PERIOD 500 /*[ms]*/
#define DEF_PRIO EDGUI_TASK_PRIO_MID
#define DEF_PERIOD 500

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_task_exec(edgui_task_t * task);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool edgui_task_run  = false;
static uint8_t idle_last = 0;
static bool task_deleted;
static bool task_created;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the edgui_task module
 */
void edgui_task_core_init(void)
{
    edgui_ll_init(&EDGUI_GC_ROOT(_edgui_task_ll), sizeof(edgui_task_t));

    /*Initially enable the edgui_task handling*/
    edgui_task_enable(true);
}

/**
 * Call it  periodically to handle edgui_tasks.
 */
EDGUI_ATTRIBUTE_TASK_HANDLER void edgui_task_handler(void)
{
    EDGUI_LOG_TRACE("edgui_task_handler started");

    /*Avoid concurrent running of the task handler*/
    static bool task_handler_mutex = false;
    if(task_handler_mutex) return;
    task_handler_mutex = true;

    static uint32_t idle_period_start = 0;
    static uint32_t handler_start     = 0;
    static uint32_t busy_time         = 0;

    if(edgui_task_run == false) {
        task_handler_mutex = false; /*Release mutex*/
        return;
    }

    handler_start = edgui_tick_get();

    /* Run all task from the highest to the lowest priority
     * If a lower priority task is executed check task again from the highest priority
     * but on the priority of executed tasks don't run tasks before the executed*/
    edgui_task_t * task_interrupter = NULL;
    edgui_task_t * next;
    bool end_flag;
    do {
        end_flag                 = true;
        task_deleted             = false;
        task_created             = false;
        EDGUI_GC_ROOT(_edgui_task_act) = edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_task_ll));
        while(EDGUI_GC_ROOT(_edgui_task_act)) {
            /* The task might be deleted if it runs only once ('once = 1')
             * So get next element until the current is surely valid*/
            next = edgui_ll_get_next(&EDGUI_GC_ROOT(_edgui_task_ll), EDGUI_GC_ROOT(_edgui_task_act));

            /*We reach priority of the turned off task. There is nothing more to do.*/
            if(((edgui_task_t *)EDGUI_GC_ROOT(_edgui_task_act))->prio == EDGUI_TASK_PRIO_OFF) {
                break;
            }

            /*Here is the interrupter task. Don't execute it again.*/
            if(EDGUI_GC_ROOT(_edgui_task_act) == task_interrupter) {
                task_interrupter = NULL; /*From this point only task after the interrupter comes, so
                                            the interrupter is not interesting anymore*/
                EDGUI_GC_ROOT(_edgui_task_act) = next;
                continue; /*Load the next task*/
            }

            /*Just try to run the tasks with highest priority.*/
            if(((edgui_task_t *)EDGUI_GC_ROOT(_edgui_task_act))->prio == EDGUI_TASK_PRIO_HIGHEST) {
                edgui_task_exec(EDGUI_GC_ROOT(_edgui_task_act));
            }
            /*Tasks with higher priority then the interrupted shall be run in every case*/
            else if(task_interrupter) {
                if(((edgui_task_t *)EDGUI_GC_ROOT(_edgui_task_act))->prio > task_interrupter->prio) {
                    if(edgui_task_exec(EDGUI_GC_ROOT(_edgui_task_act))) {
                        task_interrupter =
                            EDGUI_GC_ROOT(_edgui_task_act); /*Check all tasks again from the highest priority */
                        end_flag = false;
                        break;
                    }
                }
            }
            /* It is no interrupter task or we already reached it earlier.
             * Just run the remaining tasks*/
            else {
                if(edgui_task_exec(EDGUI_GC_ROOT(_edgui_task_act))) {
                    task_interrupter = EDGUI_GC_ROOT(_edgui_task_act); /*Check all tasks again from the highest priority */
                    end_flag         = false;
                    break;
                }
            }

            if(task_deleted) break; /*If a task was deleted then this or the next item might be corrupted*/
            if(task_created) break; /*If a task was created then this or the next item might be corrupted*/

            EDGUI_GC_ROOT(_edgui_task_act) = next; /*Load the next task*/
        }
    } while(!end_flag);

    busy_time += edgui_tick_elaps(handler_start);
    uint32_t idle_period_time = edgui_tick_elaps(idle_period_start);
    if(idle_period_time >= IDLE_MEAS_PERIOD) {

        idle_last         = (uint32_t)((uint32_t)busy_time * 100) / IDLE_MEAS_PERIOD; /*Calculate the busy percentage*/
        idle_last         = idle_last > 100 ? 0 : 100 - idle_last;                    /*But we need idle time*/
        busy_time         = 0;
        idle_period_start = edgui_tick_get();
    }

    task_handler_mutex = false; /*Release the mutex*/

    EDGUI_LOG_TRACE("edgui_task_handler ready");
}
/**
 * Create an "empty" task. It needs to initialzed with at least
 * `edgui_task_set_cb` and `edgui_task_set_period`
 * @return pointer to the craeted task
 */
edgui_task_t * edgui_task_create_basic(void)
{
    edgui_task_t * new_task = NULL;
    edgui_task_t * tmp;

    /*Create task lists in order of priority from high to low*/
    tmp = edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_task_ll));

    /*It's the first task*/
    if(NULL == tmp) {
        new_task = edgui_ll_ins_head(&EDGUI_GC_ROOT(_edgui_task_ll));
        edgui_mem_assert(new_task);
        if(new_task == NULL) return NULL;
    }
    /*Insert the new task to proper place according to its priority*/
    else {
        do {
            if(tmp->prio <= DEF_PRIO) {
                new_task = edgui_ll_ins_prev(&EDGUI_GC_ROOT(_edgui_task_ll), tmp);
                edgui_mem_assert(new_task);
                if(new_task == NULL) return NULL;
                break;
            }
            tmp = edgui_ll_get_next(&EDGUI_GC_ROOT(_edgui_task_ll), tmp);
        } while(tmp != NULL);

        /*Only too high priority tasks were found. Add the task to the end*/
        if(tmp == NULL) {
            new_task = edgui_ll_ins_tail(&EDGUI_GC_ROOT(_edgui_task_ll));
            edgui_mem_assert(new_task);
            if(new_task == NULL) return NULL;
        }
    }

    new_task->period  = DEF_PERIOD;
    new_task->task_cb = NULL;
    new_task->prio    = DEF_PRIO;

    new_task->once     = 0;
    new_task->last_run = edgui_tick_get();

    new_task->user_data = NULL;

    task_created = true;

    return new_task;
}

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
edgui_task_t * edgui_task_create(edgui_task_cb_t task_cb, uint32_t period, edgui_task_prio_t prio, void * user_data)
{
    edgui_task_t * new_task = edgui_task_create_basic();
    edgui_mem_assert(new_task);
    if(new_task == NULL) return NULL;

    edgui_task_set_cb(new_task, task_cb);
    edgui_task_set_period(new_task, period);
    edgui_task_set_prio(new_task, prio);
    new_task->user_data = user_data;

    return new_task;
}

/**
 * Set the callback the task (the function to call periodically)
 * @param task pointer to a task
 * @param task_cb teh function to call periodically
 */
void edgui_task_set_cb(edgui_task_t * task, edgui_task_cb_t task_cb)
{
    task->task_cb = task_cb;
}

/**
 * Delete a edgui_task
 * @param task pointer to task created by task
 */
void edgui_task_del(edgui_task_t * task)
{
    edgui_ll_rem(&EDGUI_GC_ROOT(_edgui_task_ll), task);

    edgui_mem_free(task);

    if(EDGUI_GC_ROOT(_edgui_task_act) == task) task_deleted = true; /*The active task was deleted*/
}

/**
 * Set new priority for a edgui_task
 * @param task pointer to a edgui_task
 * @param prio the new priority
 */
void edgui_task_set_prio(edgui_task_t * task, edgui_task_prio_t prio)
{
    if(task->prio == prio) return;

    /*Find the tasks with new priority*/
    edgui_task_t * i;
    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_task_ll), i)
    {
        if(i->prio <= prio) {
            if(i != task) edgui_ll_move_before(&EDGUI_GC_ROOT(_edgui_task_ll), task, i);
            break;
        }
    }

    /*There was no such a low priority so far then add the node to the tail*/
    if(i == NULL) {
        edgui_ll_move_before(&EDGUI_GC_ROOT(_edgui_task_ll), task, NULL);
    }

    task->prio = prio;
}

/**
 * Set new period for a edgui_task
 * @param task pointer to a edgui_task
 * @param period the new period
 */
void edgui_task_set_period(edgui_task_t * task, uint32_t period)
{
    task->period = period;
}

/**
 * Make a edgui_task ready. It will not wait its period.
 * @param task pointer to a edgui_task.
 */
void edgui_task_ready(edgui_task_t * task)
{
    task->last_run = edgui_tick_get() - task->period - 1;
}

/**
 * Delete the edgui_task after one call
 * @param task pointer to a edgui_task.
 */
void edgui_task_once(edgui_task_t * task)
{
    task->once = 1;
}

/**
 * Reset a edgui_task.
 * It will be called the previously set period milliseconds later.
 * @param task pointer to a edgui_task.
 */
void edgui_task_reset(edgui_task_t * task)
{
    task->last_run = edgui_tick_get();
}

/**
 * Enable or disable the whole edgui_task handling
 * @param en: true: edgui_task handling is running, false: edgui_task handling is suspended
 */
void edgui_task_enable(bool en)
{
    edgui_task_run = en;
}

/**
 * Get idle percentage
 * @return the edgui_task idle in percentage
 */
uint8_t edgui_task_get_idle(void)
{
    return idle_last;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute task if its the priority is appropriate
 * @param task pointer to edgui_task
 * @return true: execute, false: not executed
 */
static bool edgui_task_exec(edgui_task_t * task)
{
    bool exec = false;

    /*Execute if at least 'period' time elapsed*/
    uint32_t elp = edgui_tick_elaps(task->last_run);
    if(elp >= task->period) {
        task->last_run = edgui_tick_get();
        task_deleted   = false;
        task_created   = false;
        if(task->task_cb) task->task_cb(task);

        /*Delete if it was a one shot edgui_task*/
        if(task_deleted == false) { /*The task might be deleted by itself as well*/
            if(task->once != 0) {
                edgui_task_del(task);
            }
        }
        exec = true;
    }

    return exec;
}
