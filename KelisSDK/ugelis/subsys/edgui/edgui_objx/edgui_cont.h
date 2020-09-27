/**
 * @file edgui_cont.h
 *
 */

#ifndef EDGUI_CONT_H
#define EDGUI_CONT_H

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

#if EDGUI_USE_CONT != 0

#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Container layout options*/
enum {
    EDGUI_LAYOUT_OFF = 0, /**< No layout */
    EDGUI_LAYOUT_CENTER, /**< Center objects */
    EDGUI_LAYOUT_COL_L,  /**< Column left align*/
    EDGUI_LAYOUT_COL_M,  /**< Column middle align*/
    EDGUI_LAYOUT_COL_R,  /**< Column right align*/
    EDGUI_LAYOUT_ROW_T,  /**< Row top align*/
    EDGUI_LAYOUT_ROW_M,  /**< Row middle align*/
    EDGUI_LAYOUT_ROW_B,  /**< Row bottom align*/
    EDGUI_LAYOUT_PRETTY, /**< Put as many object as possible in row and begin a new row*/
    EDGUI_LAYOUT_GRID,   /**< Align same-sized object into a grid*/
    _EDGUI_LAYOUT_NUM
};
typedef uint8_t edgui_layout_t;

/**
 * How to resize the container around the children.
 */
enum {
    EDGUI_FIT_NONE,  /**< Do not change the size automatically*/
    EDGUI_FIT_TIGHT, /**< Shrink wrap around the children */
    EDGUI_FIT_FLOOD, /**< Align the size to the parent's edge*/
    EDGUI_FIT_FILL,  /**< Align the size to the parent's edge first but if there is an object out of it
                     then get larger */
    _EDGUI_FIT_NUM
};
typedef uint8_t edgui_fit_t;

typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext. */ /*Ext. of ancestor*/
    /*New data for this type */
    uint8_t layout : 4;     /*A layout from 'edgui_layout_t' enum*/
    uint8_t fit_left : 2;   /*A fit type from `edgui_fit_t` enum */
    uint8_t fit_right : 2;  /*A fit type from `edgui_fit_t` enum */
    uint8_t fit_top : 2;    /*A fit type from `edgui_fit_t` enum */
    uint8_t fit_bottom : 2; /*A fit type from `edgui_fit_t` enum */
} edgui_cont_ext_t;

/*Styles*/
enum {
    EDGUI_CONT_STYLE_MAIN,
};
typedef uint8_t edgui_cont_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
edgui_obj_t * edgui_cont_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'edgui_cont_layout_t'
 */
void edgui_cont_set_layout(edgui_obj_t * cont, edgui_layout_t layout);

/**
 * Set the fit policy in all 4 directions separately.
 * It tell how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param left left fit policy from `edgui_fit_t`
 * @param right right fit policy from `edgui_fit_t`
 * @param top top fit policy from `edgui_fit_t`
 * @param bottom bottom fit policy from `edgui_fit_t`
 */
void edgui_cont_set_fit4(edgui_obj_t * cont, edgui_fit_t left, edgui_fit_t right, edgui_fit_t top, edgui_fit_t bottom);

/**
 * Set the fit policy horizontally and vertically separately.
 * It tells how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param hor horizontal fit policy from `edgui_fit_t`
 * @param ver vertical fit policy from `edgui_fit_t`
 */
static __inline void edgui_cont_set_fit2(edgui_obj_t * cont, edgui_fit_t hor, edgui_fit_t ver)
{
    edgui_cont_set_fit4(cont, hor, hor, ver, ver);
}

/**
 * Set the fit policy in all 4 direction at once.
 * It tells how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param fit fit policy from `edgui_fit_t`
 */
static __inline void edgui_cont_set_fit(edgui_obj_t * cont, edgui_fit_t fit)
{
    edgui_cont_set_fit4(cont, fit, fit, fit, fit);
}

/**
 * Set the style of a container
 * @param cont pointer to a container object
 * @param type which style should be set (can be only `EDGUI_CONT_STYLE_MAIN`)
 * @param style pointer to the new style
 */
static __inline void edgui_cont_set_style(edgui_obj_t * cont, edgui_cont_style_t type, const edgui_style_t * style)
{
    (void)type; /*Unused*/
    edgui_obj_set_style(cont, style);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'edgui_cont_layout_t'
 */
edgui_layout_t edgui_cont_get_layout(const edgui_obj_t * cont);

/**
 * Get left fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_left(const edgui_obj_t * cont);

/**
 * Get right fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_right(const edgui_obj_t * cont);

/**
 * Get top fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_top(const edgui_obj_t * cont);

/**
 * Get bottom fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_bottom(const edgui_obj_t * cont);

/**
 * Get the style of a container
 * @param cont pointer to a container object
 * @param type which style should be get (can be only `EDGUI_CONT_STYLE_MAIN`)
 * @return pointer to the container's style
 */
static __inline const edgui_style_t * edgui_cont_get_style(const edgui_obj_t * cont, edgui_cont_style_t type)
{
    (void)type; /*Unused*/
    return edgui_obj_get_style(cont);
}

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_CONT*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_CONT_H*/
