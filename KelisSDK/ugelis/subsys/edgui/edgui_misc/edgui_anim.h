/**
 * @file anim.h
 *
 */

#ifndef ANIM_H
#define ANIM_H

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
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Can be used to indicate if animations are enabled or disabled in a case*/
enum {
    EDGUI_ANIM_OFF,
    EDGUI_ANIM_ON,
};

typedef uint8_t edgui_anim_enable_t;

/** Type of the animated value*/
typedef edgui_coord_t edgui_anim_value_t;

#if EDGUI_USE_ANIMATION

struct _edgui_anim_t;

/** Generic prototype of "animator" functions.
 * First parameter is the variable to animate.
 * Second parameter is the value to set.
 * Compatible with `edgui_xxx_set_yyy(obj, value)` functions
 * The `x` in `_xcb_t` means its not a fully generic prototype because
 * it doesn't receive `edgui_anim_t *` as its first argument*/
typedef void (*edgui_anim_exec_xcb_t)(void *, edgui_anim_value_t);

/** Same as `edgui_anim_exec_xcb_t` but receives `edgui_anim_t *` as the first parameter.
 * It's more consistent but less convenient. Might be used by binding generator functions.*/
typedef void (*edgui_anim_custom_exec_cb_t)(struct _edgui_anim_t *, edgui_anim_value_t);

/** Get the current value during an animation*/
typedef edgui_anim_value_t (*edgui_anim_path_cb_t)(const struct _edgui_anim_t *);

/** Callback to call when the animation is ready*/
typedef void (*edgui_anim_ready_cb_t)(struct _edgui_anim_t *);

/** Describes an animation*/
typedef struct _edgui_anim_t
{
    void * var;                  /**<Variable to animate*/
    edgui_anim_exec_xcb_t exec_cb;   /**< Function to execute to animate*/
    edgui_anim_path_cb_t path_cb;   /**< Function to get the steps of animations*/
    edgui_anim_ready_cb_t ready_cb; /**< Call it when the animation is ready*/
    int32_t start;               /**< Start value*/
    int32_t end;                 /**< End value*/
    uint16_t time;               /**< Animation time in ms*/
    int16_t act_time;            /**< Current time in animation. Set to negative to make delay.*/
    uint16_t playback_pause;     /**< Wait before play back*/
    uint16_t repeat_pause;       /**< Wait before repeat*/
#if EDGUI_USE_USER_DATA
    edgui_anim_user_data_t user_data; /**< Custom user data*/
#endif

    uint8_t playback : 1; /**< When the animation is ready play it back*/
    uint8_t repeat : 1;   /**< Repeat the animation infinitely*/
    /*Animation system use these - user shouldn't set*/
    uint8_t playback_now : 1; /**< Play back is in progress*/
    uint32_t has_run : 1;     /**< Indicates the animation has run in this round*/
} edgui_anim_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the animation module
 */
void edgui_anim_core_init(void);

/**
 * Initialize an animation variable.
 * E.g.:
 * edgui_anim_t a;
 * edgui_anim_init(&a);
 * edgui_anim_set_...(&a);
 * edgui_anim_create(&a);
 * @param a pointer to an `edgui_anim_t` variable to initialize
 */
void edgui_anim_init(edgui_anim_t * a);

/**
 * Set a variable to animate function to execute on `var`
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param var pointer to a variable to animate
 * @param exec_cb a function to execute.
 *                LittelvGL's built-in functions can be used.
 *                E.g. edgui_obj_set_x
 */
static __inline void edgui_anim_set_exec_cb(edgui_anim_t * a, void * var, edgui_anim_exec_xcb_t exec_cb)
{
    a->var     = var;
    a->exec_cb = exec_cb;
}

/**
 * Set the duration and delay of an animation
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param duration duration of the animation in milliseconds
 * @param delay delay before the animation in milliseconds
 */
static __inline void edgui_anim_set_time(edgui_anim_t * a, uint16_t duration, uint16_t delay)
{
    a->time     = duration;
    a->act_time = -delay;
}

/**
 * Set the start and end values of an animation
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param start the start value
 * @param end the end value
 */
static __inline void edgui_anim_set_values(edgui_anim_t * a, edgui_anim_value_t start, edgui_anim_value_t end)
{
    a->start = start;
    a->end   = end;
}

/**
 * Similar to `edgui_anim_set_var_and_cb` but `edgui_anim_custom_exec_cb_t` receives
 * `edgui_anim_t * ` as its first parameter instead of `void *`.
 * This function might be used when edgui is binded to other languages because
 * it's more consistent to have `edgui_anim_t *` as first parameter.
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param exec_cb a function to execute.
 */
static __inline void edgui_anim_set_custom_exec_cb(edgui_anim_t * a, edgui_anim_custom_exec_cb_t exec_cb)
{
    a->var     = a;
    a->exec_cb = (edgui_anim_exec_xcb_t)exec_cb;
}

/**
 * Set the path (curve) of the animation.
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param path_cb a function the get the current value of the animation.
 *                The built in functions starts with `edgui_anim_path_...`
 */
static __inline void edgui_anim_set_path_cb(edgui_anim_t * a, edgui_anim_path_cb_t path_cb)
{
    a->path_cb = path_cb;
}

/**
 * Set a function call when the animation is ready
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param ready_cb a function call when the animation is ready
 */
static __inline void edgui_anim_set_ready_cb(edgui_anim_t * a, edgui_anim_ready_cb_t ready_cb)
{
    a->ready_cb = ready_cb;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the back direction
 */
static __inline void edgui_anim_set_playback(edgui_anim_t * a, uint16_t wait_time)
{
    a->playback       = 1;
    a->playback_pause = wait_time;
}

/**
 * Disable playback. (Disabled after `edgui_anim_init()`)
 * @param a pointer to an initialized `edgui_anim_t` variable
 */
static __inline void edgui_anim_clear_playback(edgui_anim_t * a)
{
    a->playback = 0;
}

/**
 * Make the animation to start again when ready.
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the animation again
 */
static __inline void edgui_anim_set_repeat(edgui_anim_t * a, uint16_t wait_time)
{
    a->repeat       = 1;
    a->repeat_pause = wait_time;
}

/**
 * Disable repeat. (Disabled after `edgui_anim_init()`)
 * @param a pointer to an initialized `edgui_anim_t` variable
 */
static __inline void edgui_anim_clear_repeat(edgui_anim_t * a)
{
    a->repeat = 0;
}

/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
void edgui_anim_create(edgui_anim_t * a);

/**
 * Delete an animation of a variable with a given animator function
 * @param var pointer to variable
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all the animations of 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool edgui_anim_del(void * var, edgui_anim_exec_xcb_t exec_cb);

/**
 * Delete an aniamation by getting the animated variable from `a`.
 * Only animations with `exec_cb` will be deleted.
 * This function exist becasue it's logical that all anim functions receives an
 * `edgui_anim_t` as their first parameter. It's not practical in C but might makes
 * the API more conequent and makes easier to genrate bindings.
 * @param a pointer to an animation.
 * @param exec_cb a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all the animations of 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
static __inline bool edgui_anim_custom_del(edgui_anim_t * a, edgui_anim_custom_exec_cb_t exec_cb)
{
    return edgui_anim_del(a->var, (edgui_anim_exec_xcb_t)exec_cb);
}

/**
 * Get the number of currently running animations
 * @return the number of running animations
 */
uint16_t edgui_anim_count_running(void);

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint16_t edgui_anim_speed_to_time(uint16_t speed, edgui_anim_value_t start, edgui_anim_value_t end);

/**
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_linear(const edgui_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the start phase
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_ease_in(const edgui_anim_t * a);

/**
 * Calculate the current value of an animation slowing down the end phase
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_ease_out(const edgui_anim_t * a);

/**
 * Calculate the current value of an animation applying an "S" characteristic (cosine)
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_ease_in_out(const edgui_anim_t * a);

/**
 * Calculate the current value of an animation with overshoot at the end
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_overshoot(const edgui_anim_t * a);

/**
 * Calculate the current value of an animation with 3 bounces
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_bounce(const edgui_anim_t * a);

/**
 * Calculate the current value of an animation applying step characteristic.
 * (Set end value on the end of the animation)
 * @param a pointer to an animation
 * @return the current value to set
 */
edgui_anim_value_t edgui_anim_path_step(const edgui_anim_t * a);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_ANIMATION == 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_ANIM_H*/
