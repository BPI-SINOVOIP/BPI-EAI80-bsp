/**
 * @file edgui_slider.h
 *
 */

#ifndef EDGUI_SLIDER_H
#define EDGUI_SLIDER_H

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

#if EDGUI_USE_SLIDER != 0

/*Testing of dependencies*/
#if EDGUI_USE_BAR == 0
#error "edgui_slider: edgui_bar is required. Enable it in edgui_conf.h (EDGUI_USE_BAR  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_bar.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of slider*/
typedef struct
{
    edgui_bar_ext_t bar; /*Ext. of ancestor*/
    /*New data for this type */
    const edgui_style_t * style_knob; /*Style of the knob*/
    int16_t drag_value;            /*Store a temporal value during press until release (Handled by the library)*/
    uint8_t knob_in : 1;           /*1: Draw the knob inside the bar*/
} edgui_slider_ext_t;

/** Built-in styles of slider*/
enum {
    EDGUI_SLIDER_STYLE_BG, /** Slider background style. */
    EDGUI_SLIDER_STYLE_INDIC, /** Slider indicator (filled area) style. */
    EDGUI_SLIDER_STYLE_KNOB, /** Slider knob style. */
};
typedef uint8_t edgui_slider_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a slider objects
 * @param par pointer to an object, it will be the parent of the new slider
 * @param copy pointer to a slider object, if not NULL then the new object will be copied from it
 * @return pointer to the created slider
 */
edgui_obj_t * edgui_slider_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the slider
 * @param slider pointer to a slider object
 * @param value new value
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
static __inline void edgui_slider_set_value(edgui_obj_t * slider, int16_t value, edgui_anim_enable_t anim)
{
    edgui_bar_set_value(slider, value, anim);
}

/**
 * Set minimum and the maximum values of a bar
 * @param slider pointer to the slider object
 * @param min minimum value
 * @param max maximum value
 */
static __inline void edgui_slider_set_range(edgui_obj_t * slider, int16_t min, int16_t max)
{
    edgui_bar_set_range(slider, min, max);
}

/**
 * Set the animation time of the slider
 * @param slider pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
static __inline void edgui_slider_set_anim_time(edgui_obj_t * slider, uint16_t anim_time)
{
    edgui_bar_set_anim_time(slider, anim_time);
}

/**
 * Set the 'knob in' attribute of a slider
 * @param slider pointer to slider object
 * @param in true: the knob is drawn always in the slider;
 *           false: the knob can be out on the edges
 */
void edgui_slider_set_knob_in(edgui_obj_t * slider, bool in);

/**
 * Set a style of a slider
 * @param slider pointer to a slider object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_slider_set_style(edgui_obj_t * slider, edgui_slider_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a slider
 * @param slider pointer to a slider object
 * @return the value of the slider
 */
int16_t edgui_slider_get_value(const edgui_obj_t * slider);

/**
 * Get the minimum value of a slider
 * @param slider pointer to a slider object
 * @return the minimum value of the slider
 */
static __inline int16_t edgui_slider_get_min_value(const edgui_obj_t * slider)
{
    return edgui_bar_get_min_value(slider);
}

/**
 * Get the maximum value of a slider
 * @param slider pointer to a slider object
 * @return the maximum value of the slider
 */
static __inline int16_t edgui_slider_get_max_value(const edgui_obj_t * slider)
{
    return edgui_bar_get_max_value(slider);
}

/**
 * Give the slider is being dragged or not
 * @param slider pointer to a slider object
 * @return true: drag in progress false: not dragged
 */
bool edgui_slider_is_dragged(const edgui_obj_t * slider);

/**
 * Get the 'knob in' attribute of a slider
 * @param slider pointer to slider object
 * @return true: the knob is drawn always in the slider;
 *         false: the knob can be out on the edges
 */
bool edgui_slider_get_knob_in(const edgui_obj_t * slider);

/**
 * Get a style of a slider
 * @param slider pointer to a slider object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_slider_get_style(const edgui_obj_t * slider, edgui_slider_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_SLIDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_SLIDER_H*/
