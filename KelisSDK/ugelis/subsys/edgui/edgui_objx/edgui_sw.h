/**
 * @file edgui_sw.h
 *
 */

#ifndef EDGUI_SW_H
#define EDGUI_SW_H

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

#if EDGUI_USE_SW != 0

/*Testing of dependencies*/
#if EDGUI_USE_SLIDER == 0
#error "edgui_sw: edgui_slider is required. Enable it in edgui_conf.h (EDGUI_USE_SLIDER  1)"
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_slider.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_SW_MAX_VALUE 100

/**********************
 *      TYPEDEFS
 **********************/
/*Data of switch*/
typedef struct
{
    edgui_slider_ext_t slider; /*Ext. of ancestor*/
    /*New data for this type */
    const edgui_style_t * style_knob_off; /**< Style of the knob when the switch is OFF*/
    const edgui_style_t * style_knob_on;  /**< Style of the knob when the switch is ON (NULL to use the same as OFF)*/
    edgui_coord_t start_x;
    uint8_t changed : 1; /*Indicates the switch state explicitly changed by drag*/
    uint8_t slided : 1;
#if EDGUI_USE_ANIMATION
    uint16_t anim_time; /*switch animation time */
#endif
} edgui_sw_ext_t;

/**
 * Switch styles.
 */
enum {
    EDGUI_SW_STYLE_BG, /**< Switch background. */
    EDGUI_SW_STYLE_INDIC, /**< Switch fill area. */
    EDGUI_SW_STYLE_KNOB_OFF, /**< Switch knob (when off). */
    EDGUI_SW_STYLE_KNOB_ON, /**< Switch knob (when on). */
};
typedef uint8_t edgui_sw_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
edgui_obj_t * edgui_sw_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch object
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_sw_on(edgui_obj_t * sw, edgui_anim_enable_t anim);

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_sw_off(edgui_obj_t * sw, edgui_anim_enable_t anim);

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool edgui_sw_toggle(edgui_obj_t * sw, edgui_anim_enable_t anim);

/**
 * Set a style of a switch
 * @param sw pointer to a switch object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_sw_set_style(edgui_obj_t * sw, edgui_sw_style_t type, const edgui_style_t * style);

/**
 * Set the animation time of the switch
 * @param sw pointer to a  switch object
 * @param anim_time animation time
 * @return style pointer to a style
 */
void edgui_sw_set_anim_time(edgui_obj_t * sw, uint16_t anim_time);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the state of a switch
 * @param sw pointer to a switch object
 * @return false: OFF; true: ON
 */
static __inline bool edgui_sw_get_state(const edgui_obj_t * sw)
{
    return edgui_bar_get_value(sw) < EDGUI_SW_MAX_VALUE / 2 ? false : true;
}

/**
 * Get a style of a switch
 * @param sw pointer to a  switch object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_sw_get_style(const edgui_obj_t * sw, edgui_sw_style_t type);

/**
 * Get the animation time of the switch
 * @param sw pointer to a  switch object
 * @return style pointer to a style
 */
uint16_t edgui_sw_get_anim_time(const edgui_obj_t * sw);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_SW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_SW_H*/
