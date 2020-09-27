/**
 * @file edgui_bar.h
 *
 */

#ifndef EDGUI_BAR_H
#define EDGUI_BAR_H

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

#if EDGUI_USE_BAR != 0

#include "../edgui_core/edgui_obj.h"
#include "../edgui_misc/edgui_anim.h"
#include "edgui_cont.h"
#include "edgui_btn.h"
#include "edgui_label.h"

/*********************
 *      DEFINES
 *********************/

/** Bar animation start value. (Not the real value of the Bar just indicates process animation)*/
#define EDGUI_BAR_ANIM_STATE_START 0

/** Bar animation end value.  (Not the real value of the Bar just indicates process animation)*/
#define EDGUI_BAR_ANIM_STATE_END 256

/** Mark no animation is in progress */
#define EDGUI_BAR_ANIM_STATE_INV -1

/** log2(EDGUI_BAR_ANIM_STATE_END) used to normalize data*/
#define EDGUI_BAR_ANIM_STATE_NORM 8

/**********************
 *      TYPEDEFS
 **********************/

/** Data of bar*/
typedef struct
{
    /*No inherited ext, derived from the base object */

    /*New data for this type */
    int16_t cur_value; /*Current value of the bar*/
    int16_t min_value; /*Minimum value of the bar*/
    int16_t max_value; /*Maximum value of the bar*/
#if EDGUI_USE_ANIMATION
    edgui_anim_value_t anim_start;
    edgui_anim_value_t anim_end;
    edgui_anim_value_t anim_state;
    edgui_anim_value_t anim_time;
#endif
    uint8_t sym : 1;                /*Symmetric: means the center is around zero value*/
    const edgui_style_t * style_indic; /*Style of the indicator*/
} edgui_bar_ext_t;

/** Bar styles. */
enum {
    EDGUI_BAR_STYLE_BG, /** Bar background style. */
    EDGUI_BAR_STYLE_INDIC, /** Bar fill area style. */
};
typedef uint8_t edgui_bar_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
edgui_obj_t * edgui_bar_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_bar_set_value(edgui_obj_t * bar, int16_t value, edgui_anim_enable_t anim);

/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void edgui_bar_set_range(edgui_obj_t * bar, int16_t min, int16_t max);

/**
 * Make the bar symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param bar pointer to a bar object
 * @param en true: enable disable symmetric behavior; false: disable
 */
void edgui_bar_set_sym(edgui_obj_t * bar, bool en);

/**
 * Set the animation time of the bar
 * @param bar pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
void edgui_bar_set_anim_time(edgui_obj_t * bar, uint16_t anim_time);

/**
 * Set a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_bar_set_style(edgui_obj_t * bar, edgui_bar_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t edgui_bar_get_value(const edgui_obj_t * bar);

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t edgui_bar_get_min_value(const edgui_obj_t * bar);

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t edgui_bar_get_max_value(const edgui_obj_t * bar);

/**
 * Get whether the bar is symmetric or not.
 * @param bar pointer to a bar object
 * @return true: symmetric is enabled; false: disable
 */
bool edgui_bar_get_sym(edgui_obj_t * bar);

/**
 * Get the animation time of the bar
 * @param bar pointer to a bar object
 * @return the animation time in milliseconds.
 */
uint16_t edgui_bar_get_anim_time(edgui_obj_t * bar);

/**
 * Get a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_bar_get_style(const edgui_obj_t * bar, edgui_bar_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_BAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_BAR_H*/
