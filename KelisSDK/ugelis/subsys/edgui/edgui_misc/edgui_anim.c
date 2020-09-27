/**
 * @file anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_anim.h"

#if EDGUI_USE_ANIMATION
#include <stddef.h>
#include <string.h>
#include "../edgui_hal/edgui_hal_tick.h"
#include "edgui_task.h"
#include "edgui_math.h"
#include "edgui_gc.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define EDGUI_ANIM_RESOLUTION 1024
#define EDGUI_ANIM_RES_SHIFT 10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_task(edgui_task_t * param);
static bool anim_ready_handler(edgui_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_task_run;
static bool anim_list_changed;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the animation module
 */
void edgui_anim_core_init(void)
{
    edgui_ll_init(&EDGUI_GC_ROOT(_edgui_anim_ll), sizeof(edgui_anim_t));
    last_task_run = edgui_tick_get();
    edgui_task_create(anim_task, EDGUI_DISP_DEF_REFR_PERIOD, EDGUI_TASK_PRIO_MID, NULL);
}

/**
 * Initialize an animation variable.
 * E.g.:
 * edgui_anim_t a;
 * edgui_anim_init(&a);
 * edgui_anim_set_...(&a);
 * edgui_anim_craete(&a);
 * @param a pointer to an `edgui_anim_t` variable to initialize
 */
void edgui_anim_init(edgui_anim_t * a)
{
    memset(a, 0, sizeof(edgui_anim_t));
    a->time    = 500;
    a->start   = 0;
    a->end     = 100;
    a->path_cb = edgui_anim_path_linear;
}
/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
void edgui_anim_create(edgui_anim_t * a)
{
    EDGUI_LOG_TRACE("animation create started")
    /* Do not let two animations for the  same 'var' with the same 'fp'*/
    if(a->exec_cb != NULL) edgui_anim_del(a->var, a->exec_cb); /*fp == NULL would delete all animations of var*/

    /*Add the new animation to the animation linked list*/
    edgui_anim_t * new_anim = edgui_ll_ins_head(&EDGUI_GC_ROOT(_edgui_anim_ll));
    edgui_mem_assert(new_anim);
    if(new_anim == NULL) return;

    /*Initialize the animation descriptor*/
    a->playback_now = 0;
    memcpy(new_anim, a, sizeof(edgui_anim_t));

    /*Set the start value*/
    if(new_anim->exec_cb) new_anim->exec_cb(new_anim->var, new_anim->start);

    /* Creating an animation changed the linked list.
     * It's important if it happens in a ready callback. (see `anim_task`)*/
    anim_list_changed = true;

    EDGUI_LOG_TRACE("animation created")
}

/**
 * Delete an animation of a variable with a given animator function
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to delete all the animations of 'var'
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool edgui_anim_del(void * var, edgui_anim_exec_xcb_t exec_cb)
{
    edgui_anim_t * a;
    edgui_anim_t * a_next;
    bool del = false;
    a        = edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_anim_ll));
    while(a != NULL) {
        /*'a' might be deleted, so get the next object while 'a' is valid*/
        a_next = edgui_ll_get_next(&EDGUI_GC_ROOT(_edgui_anim_ll), a);

        if(a->var == var && (a->exec_cb == exec_cb || exec_cb == NULL)) {
            edgui_ll_rem(&EDGUI_GC_ROOT(_edgui_anim_ll), a);
            edgui_mem_free(a);
            anim_list_changed = true; /*Read by `anim_task`. It need to know if a delete occurred in
                                         the linked list*/
            del = true;
        }

        a = a_next;
    }

    return del;
}

/**
 * Get the number of currently running animations
 * @return the number of running animations
 */
uint16_t edgui_anim_count_running(void)
{
    uint16_t cnt = 0;
    edgui_anim_t * a;
    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_anim_ll), a) cnt++;

    return cnt++;
}

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint16_t edgui_anim_speed_to_time(uint16_t speed, edgui_anim_value_t start, edgui_anim_value_t end)
{
    int32_t d     = EDGUI_MATH_ABS((int32_t)start - end);
    uint32_t time = (int32_t)((int32_t)(d * 1000) / speed);

    if(time > UINT16_MAX) time = UINT16_MAX;

    if(time == 0) {
        time++;
    }

    return time;
}

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_linear(const edgui_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t step;
    if(a->time == a->act_time) {
        step = EDGUI_ANIM_RESOLUTION; /*Use the last value if the time fully elapsed*/
    } else {
        step = ((int32_t)a->act_time * EDGUI_ANIM_RESOLUTION) / a->time;
    }

    /* Get the new value which will be proportional to `step`
     * and the `start` and `end` values*/
    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> EDGUI_ANIM_RES_SHIFT;
    new_value += a->start;

    return (edgui_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_ease_in(const edgui_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = edgui_bezier3(t, 0, 1, 1, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (edgui_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_ease_out(const edgui_anim_t * a)
{
    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = edgui_bezier3(t, 0, 1023, 1023, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (edgui_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_ease_in_out(const edgui_anim_t * a)
{
    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = edgui_bezier3(t, 0, 100, 924, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (edgui_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_overshoot(const edgui_anim_t * a)
{
    /*Calculate the current step*/

    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t step = edgui_bezier3(t, 0, 600, 1300, 1024);

    int32_t new_value;
    new_value = (int32_t)step * (a->end - a->start);
    new_value = new_value >> 10;
    new_value += a->start;

    return (edgui_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_bounce(const edgui_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t t;
    if(a->time == a->act_time)
        t = 1024;
    else
        t = (uint32_t)((uint32_t)a->act_time * 1024) / a->time;

    int32_t diff = (a->end - a->start);

    /*3 bounces has 5 parts: 3 down and 2 up. One part is t / 5 long*/

    if(t < 408) {
        /*Go down*/
        t = (t * 2500) >> 10; /*[0..1024] range*/
    } else if(t >= 408 && t < 614) {
        /*First bounce back*/
        t -= 408;
        t    = t * 5; /*to [0..1024] range*/
        t    = 1024 - t;
        diff = diff / 6;
    } else if(t >= 614 && t < 819) {
        /*Fall back*/
        t -= 614;
        t    = t * 5; /*to [0..1024] range*/
        diff = diff / 6;
    } else if(t >= 819 && t < 921) {
        /*Second bounce back*/
        t -= 819;
        t    = t * 10; /*to [0..1024] range*/
        t    = 1024 - t;
        diff = diff / 16;
    } else if(t >= 921 && t <= 1024) {
        /*Fall back*/
        t -= 921;
        t    = t * 10; /*to [0..1024] range*/
        diff = diff / 16;
    }

    if(t > 1024) t = 1024;

    int32_t step = edgui_bezier3(t, 1024, 1024, 800, 0);

    int32_t new_value;
    new_value = (int32_t)step * diff;
    new_value = new_value >> 10;
    new_value = a->end - new_value;

    return (edgui_anim_value_t)new_value;
}

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_step(const edgui_anim_t * a)
{
    if(a->act_time >= a->time)
        return a->end;
    else
        return a->start;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle the animations.
 * @param param unused
 */
static void anim_task(edgui_task_t * param)
{
    (void)param;

    edgui_anim_t * a;
    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_anim_ll), a)
    {
        a->has_run = 0;
    }

    uint32_t elaps = edgui_tick_elaps(last_task_run);

    a = edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_anim_ll));

    while(a != NULL) {
        /*It can be set by `edgui_anim_del()` typically in `end_cb`. If set then an animation delete
         * happened in `anim_ready_handler` which could make this linked list reading corrupt
         * because the list is changed meanwhile
         */
        anim_list_changed = false;

        if(!a->has_run) {
            a->has_run = 1; /*The list readying might be reseted so need to know which anim has run already*/
            a->act_time += elaps;
            if(a->act_time >= 0) {
                if(a->act_time > a->time) a->act_time = a->time;

                int32_t new_value;
                new_value = a->path_cb(a);

                /*Apply the calculated value*/
                if(a->exec_cb) a->exec_cb(a->var, new_value);

                /*If the time is elapsed the animation is ready*/
                if(a->act_time >= a->time) {
                    anim_ready_handler(a);
                }
            }
        }

        /* If the linked list changed due to anim. delete then it's not safe to continue
         * the reading of the list from here -> start from the head*/
        if(anim_list_changed)
            a = edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_anim_ll));
        else
            a = edgui_ll_get_next(&EDGUI_GC_ROOT(_edgui_anim_ll), a);
    }

    last_task_run = edgui_tick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 * @return true: animation delete occurred nnd the `EDGUI_GC_ROOT(_edgui_anim_ll)` has changed
 * */
static bool anim_ready_handler(edgui_anim_t * a)
{

    /*Delete the animation if
     * - no repeat and no play back (simple one shot animation)
     * - no repeat, play back is enabled and play back is ready */
    if((a->repeat == 0 && a->playback == 0) || (a->repeat == 0 && a->playback == 1 && a->playback_now == 1)) {

        /*Create copy from the animation and delete the animation from the list.
         * This way the `ready_cb` will see the animations like it's animation is ready deleted*/
        edgui_anim_t a_tmp;
        memcpy(&a_tmp, a, sizeof(edgui_anim_t));
        edgui_ll_rem(&EDGUI_GC_ROOT(_edgui_anim_ll), a);
        edgui_mem_free(a);
        anim_list_changed = true;

        /* Call the callback function at the end*/
        if(a_tmp.ready_cb != NULL) a_tmp.ready_cb(&a_tmp);
    }
    /*If the animation is not deleted then restart it*/
    else {
        a->act_time = -a->repeat_pause; /*Restart the animation*/
        /*Swap the start and end values in play back mode*/
        if(a->playback != 0) {
            /*If now turning back use the 'playback_pause*/
            if(a->playback_now == 0) a->act_time = -a->playback_pause;

            /*Toggle the play back state*/
            a->playback_now = a->playback_now == 0 ? 1 : 0;
            /*Swap the start and end values*/
            int32_t tmp;
            tmp      = a->start;
            a->start = a->end;
            a->end   = tmp;
        }
    }

    return anim_list_changed;
}
#endif
