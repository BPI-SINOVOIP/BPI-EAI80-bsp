/**
 * @file edgui_tileview.h
 *
 */

#ifndef EDGUI_TILEVIEW_H
#define EDGUI_TILEVIEW_H

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

#if EDGUI_USE_TILEVIEW != 0

#include "../edgui_objx/edgui_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of tileview*/
typedef struct
{
    edgui_page_ext_t page;
    /*New data for this type */
    const edgui_point_t * valid_pos;
    uint16_t valid_pos_cnt;
#if EDGUI_USE_ANIMATION
    uint16_t anim_time;
#endif
    edgui_point_t act_id;
    uint8_t drag_top_en : 1;
    uint8_t drag_bottom_en : 1;
    uint8_t drag_left_en : 1;
    uint8_t drag_right_en : 1;
    uint8_t drag_hor : 1;
    uint8_t drag_ver : 1;
} edgui_tileview_ext_t;

/*Styles*/
enum {
    EDGUI_TILEVIEW_STYLE_MAIN,
};
typedef uint8_t edgui_tileview_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a tileview objects
 * @param par pointer to an object, it will be the parent of the new tileview
 * @param copy pointer to a tileview object, if not NULL then the new object will be copied from it
 * @return pointer to the created tileview
 */
edgui_obj_t * edgui_tileview_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Register an object on the tileview. The register object will able to slide the tileview
 * @param tileview pointer to a Tileview object
 * @param element pointer to an object
 */
void edgui_tileview_add_element(edgui_obj_t * tileview, edgui_obj_t * element);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the valid position's indices. The scrolling will be possible only to these positions.
 * @param tileview pointer to a Tileview object
 * @param valid_pos array width the indices. E.g. `edgui_point_t p[] = {{0,0}, {1,0}, {1,1}`. Only the
 * pointer is saved so can't be a local variable.
 * @param valid_pos_cnt numner of elements in `valid_pos` array
 */
void edgui_tileview_set_valid_positions(edgui_obj_t * tileview, const edgui_point_t * valid_pos, uint16_t valid_pos_cnt);

/**
 * Set the tile to be shown
 * @param tileview pointer to a tileview object
 * @param x column id (0, 1, 2...)
 * @param y line id (0, 1, 2...)
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_tileview_set_tile_act(edgui_obj_t * tileview, edgui_coord_t x, edgui_coord_t y, edgui_anim_enable_t anim);

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param tileview pointer to a Tileview
 * @param en true or false to enable/disable end flash
 */
static __inline void edgui_tileview_set_edge_flash(edgui_obj_t * tileview, bool en)
{
    edgui_page_set_edge_flash(tileview, en);
}

/**
 * Set the animation time for the Tile view
 * @param tileview pointer to a page object
 * @param anim_time animation time in milliseconds
 */
static __inline void edgui_tileview_set_anim_time(edgui_obj_t * tileview, uint16_t anim_time)
{
    edgui_page_set_anim_time(tileview, anim_time);
}

/**
 * Set a style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_tileview_set_style(edgui_obj_t * tileview, edgui_tileview_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scroll propagation property
 * @param tileview pointer to a Tileview
 * @return true or false
 */
static __inline bool edgui_tileview_get_edge_flash(edgui_obj_t * tileview)
{
    return edgui_page_get_edge_flash(tileview);
}

/**
 * Get the animation time for the Tile view
 * @param tileview pointer to a page object
 * @return animation time in milliseconds
 */
static __inline uint16_t edgui_tileview_get_anim_time(edgui_obj_t * tileview)
{
    return edgui_page_get_anim_time(tileview);
}

/**
 * Get style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be get
 * @return style pointer to the style
 */
const edgui_style_t * edgui_tileview_get_style(const edgui_obj_t * tileview, edgui_tileview_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TILEVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TILEVIEW_H*/
