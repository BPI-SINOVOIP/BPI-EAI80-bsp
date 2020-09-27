/**
 * @file edgui_templ.h
 *
 */

/* TODO Remove these instructions
 * Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 *                    templ -> object short name with lower case(e.g. btn, label etc)
 *                    TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef EDGUI_TEMPL_H
#define EDGUI_TEMPL_H

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

#if EDGUI_USE_TEMPL != 0

#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/
typedef struct
{
    edgui_ANCESTOR_ext_t ANCESTOR; /*Ext. of ancestor*/
    /*New data for this type */
} edgui_templ_ext_t;

/*Styles*/
enum {
    EDGUI_TEMPL_STYLE_X,
    EDGUI_TEMPL_STYLE_Y,
};
typedef uint8_t edgui_templ_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a template objects
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
edgui_obj_t * edgui_templ_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a template.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_templ_set_style(edgui_obj_t * templ, edgui_templ_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a template.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 */
edgui_style_t * edgui_templ_get_style(const edgui_obj_t * templ, edgui_templ_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TEMPL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEMPL_H*/
