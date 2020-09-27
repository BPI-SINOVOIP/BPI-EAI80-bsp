/**
 * @file edgui_preload.h
 *
 */

#ifndef EDGUI_PRELOAD_H
#define EDGUI_PRELOAD_H

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

#if EDGUI_USE_PRELOAD != 0

/*Testing of dependencies*/
#if EDGUI_USE_ARC == 0
#error "edgui_preload: edgui_arc is required. Enable it in edgui_conf.h (EDGUI_USE_ARC  1) "
#endif

#if EDGUI_USE_ANIMATION == 0
#error "edgui_preload: animations are required. Enable it in edgui_conf.h (EDGUI_USE_ANIMATION  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "../edgui_misc/edgui_anim.h"
#include "edgui_arc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Type of preloader.
 */
enum {
    EDGUI_PRELOAD_TYPE_SPINNING_ARC,
    EDGUI_PRELOAD_TYPE_FILLSPIN_ARC,
};
typedef uint8_t edgui_preload_type_t;

/**
 * Direction the preloader should spin.
 */
enum {
    EDGUI_PRELOAD_DIR_FORWARD,
    EDGUI_PRELOAD_DIR_BACKWARD,
};
typedef uint8_t edgui_preload_dir_t;

/*Data of pre loader*/
typedef struct
{
    edgui_arc_ext_t arc; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_anim_value_t arc_length;      /*Length of the spinning indicator in degree*/
    uint16_t time;                   /*Time of one round*/
    edgui_preload_type_t anim_type : 1; /*Type of the arc animation*/
    edgui_preload_dir_t anim_dir : 1;   /*Animation Direction*/
} edgui_preload_ext_t;

/*Styles*/
enum {
    EDGUI_PRELOAD_STYLE_MAIN,
};
typedef uint8_t edgui_preload_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a pre loader objects
 * @param par pointer to an object, it will be the parent of the new pre loader
 * @param copy pointer to a pre loader object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created pre loader
 */
edgui_obj_t * edgui_preload_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Set the length of the spinning  arc in degrees
 * @param preload pointer to a preload object
 * @param deg length of the arc
 */
void edgui_preload_set_arc_length(edgui_obj_t * preload, edgui_anim_value_t deg);

/**
 * Set the spin time of the arc
 * @param preload pointer to a preload object
 * @param time time of one round in milliseconds
 */
void edgui_preload_set_spin_time(edgui_obj_t * preload, uint16_t time);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_preload_set_style(edgui_obj_t * preload, edgui_preload_style_t type, const edgui_style_t * style);

/**
 * Set the animation type of a preloader.
 * @param preload pointer to pre loader object
 * @param type animation type of the preload
 *  */
void edgui_preload_set_type(edgui_obj_t * preload, edgui_preload_type_t type);

/**
 * Set the animation direction of a preloader
 * @param preload pointer to pre loader object
 * @param direction animation direction of the preload
 */
void edgui_preload_set_dir(edgui_obj_t * preload, edgui_preload_dir_t dir);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the arc length [degree] of the a pre loader
 * @param preload pointer to a pre loader object
 */
edgui_anim_value_t edgui_preload_get_arc_length(const edgui_obj_t * preload);

/**
 * Get the spin time of the arc
 * @param preload pointer to a pre loader object [milliseconds]
 */
uint16_t edgui_preload_get_spin_time(const edgui_obj_t * preload);

/**
 * Get style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_preload_get_style(const edgui_obj_t * preload, edgui_preload_style_t type);

/**
 * Get the animation type of a preloader.
 * @param preload pointer to pre loader object
 * @return animation type
 *  */
edgui_preload_type_t edgui_preload_get_type(edgui_obj_t * preload);

/**
 * Get the animation direction of a preloader
 * @param preload pointer to pre loader object
 * @return animation direction
 */
edgui_preload_dir_t edgui_preload_get_dir(edgui_obj_t * preload);

/*=====================
 * Other functions
 *====================*/

/**
 * Animator function  (exec_cb) to rotate the arc of spinner.
 * @param ptr pointer to preloader
 * @param val the current desired value [0..360]
 */
void edgui_preload_spinner_anim(void * ptr, edgui_anim_value_t val);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_PRELOAD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_PRELOAD_H*/
