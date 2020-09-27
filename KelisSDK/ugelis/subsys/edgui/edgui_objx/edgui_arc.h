/**
 * @file edgui_arc.h
 *
 */

#ifndef EDGUI_ARC_H
#define EDGUI_ARC_H

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

#if EDGUI_USE_ARC != 0

#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of arc*/
typedef struct
{
    /*New data for this type */
    edgui_coord_t angle_start;
    edgui_coord_t angle_end;
} edgui_arc_ext_t;

/*Styles*/
enum {
    EDGUI_ARC_STYLE_MAIN,
};
typedef uint8_t edgui_arc_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a arc objects
 * @param par pointer to an object, it will be the parent of the new arc
 * @param copy pointer to a arc object, if not NULL then the new object will be copied from it
 * @return pointer to the created arc
 */
edgui_obj_t * edgui_arc_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the start and end angles of an arc. 0 deg: bottom, 90 deg: right etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 * @param end the end angle [0..360]
 */
void edgui_arc_set_angles(edgui_obj_t * arc, uint16_t start, uint16_t end);

/**
 * Set a style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_arc_set_style(edgui_obj_t * arc, edgui_arc_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t edgui_arc_get_angle_start(edgui_obj_t * arc);

/**
 * Get the end angle of an arc.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
uint16_t edgui_arc_get_angle_end(edgui_obj_t * arc);

/**
 * Get style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_arc_get_style(const edgui_obj_t * arc, edgui_arc_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_ARC*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_ARC_H*/
