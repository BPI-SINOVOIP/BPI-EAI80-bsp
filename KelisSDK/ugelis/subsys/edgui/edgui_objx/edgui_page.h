/**
 * @file edgui_page.h
 *
 */

#ifndef EDGUI_PAGE_H
#define EDGUI_PAGE_H

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

#if EDGUI_USE_PAGE != 0

/*Testing of dependencies*/
#if EDGUI_USE_CONT == 0
#error "edgui_page: edgui_cont is required. Enable it in edgui_conf.h (EDGUI_USE_CONT  1) "
#endif

#include "edgui_cont.h"
#include "../edgui_core/edgui_indev.h"
#include "../edgui_misc/edgui_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Scrollbar modes: shows when should the scrollbars be visible*/
enum {
    EDGUI_SB_MODE_OFF    = 0x0, /**< Never show scrollbars*/
    EDGUI_SB_MODE_ON     = 0x1, /**< Always show scrollbars*/
    EDGUI_SB_MODE_DRAG   = 0x2, /**< Show scrollbars when page is being dragged*/
    EDGUI_SB_MODE_AUTO   = 0x3, /**< Show scrollbars when the scrollable container is large enough to be scrolled*/
    EDGUI_SB_MODE_HIDE   = 0x4, /**< Hide the scroll bar temporally*/
    EDGUI_SB_MODE_UNHIDE = 0x5, /**< Unhide the previously hidden scrollbar. Recover it's type too*/
};
typedef uint8_t edgui_sb_mode_t;

/** Edges: describes the four edges of the page*/
enum { EDGUI_PAGE_EDGE_LEFT = 0x1, EDGUI_PAGE_EDGE_TOP = 0x2, EDGUI_PAGE_EDGE_RIGHT = 0x4, EDGUI_PAGE_EDGE_BOTTOM = 0x8 };
typedef uint8_t edgui_page_edge_t;

/*Data of page*/
typedef struct
{
    edgui_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * scrl; /*The scrollable object on the background*/
    struct
    {
        const edgui_style_t * style; /*Style of scrollbars*/
        edgui_area_t hor_area;       /*Horizontal scrollbar area relative to the page. (Handled by the library) */
        edgui_area_t ver_area;       /*Vertical scrollbar area relative to the page (Handled by the library)*/
        uint8_t hor_draw : 1;     /*1: horizontal scrollbar is visible now (Handled by the library)*/
        uint8_t ver_draw : 1;     /*1: vertical scrollbar is visible now (Handled by the library)*/
        edgui_sb_mode_t mode : 3;    /*Scrollbar visibility from 'edgui_page_sb_mode_t'*/
    } sb;
#if EDGUI_USE_ANIMATION
    struct
    {
        edgui_anim_value_t state;    /*Store the current size of the edge flash effect*/
        const edgui_style_t * style; /*Style of edge flash effect (usually homogeneous circle)*/
        uint8_t enabled : 1;      /*1: Show a flash animation on the edge*/
        uint8_t top_ip : 1;       /*Used internally to show that top most position is reached (flash is In
                                     Progress)*/
        uint8_t bottom_ip : 1;    /*Used internally to show that bottom most position is reached (flash
                                     is In Progress)*/
        uint8_t right_ip : 1;     /*Used internally to show that right most position is reached (flash
                                     is In Progress)*/
        uint8_t left_ip : 1;      /*Used internally to show that left most position is reached (flash is
                                     In Progress)*/
    } edge_flash;

    uint16_t anim_time; /*Scroll animation time*/
#endif

    uint8_t scroll_prop : 1;    /*1: Propagate the scrolling the the parent if the edge is reached*/
    uint8_t scroll_prop_ip : 1; /*1: Scroll propagation is in progress (used by the library)*/
} edgui_page_ext_t;

enum {
    EDGUI_PAGE_STYLE_BG,
    EDGUI_PAGE_STYLE_SCRL,
    EDGUI_PAGE_STYLE_SB,
    EDGUI_PAGE_STYLE_EDGE_FLASH,
};
typedef uint8_t edgui_page_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
edgui_obj_t * edgui_page_create(edgui_obj_t * par, const edgui_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_page_clean(edgui_obj_t * obj);

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
edgui_obj_t * edgui_page_get_scrl(const edgui_obj_t * page);

/**
 * Get the animation time
 * @param page pointer to a page object
 * @return the animation time in milliseconds
 */
uint16_t edgui_page_get_anim_time(const edgui_obj_t * page);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb_mode the new mode from 'edgui_page_sb.mode_t' enum
 */
void edgui_page_set_sb_mode(edgui_obj_t * page, edgui_sb_mode_t sb_mode);

/**
 * Set the animation time for the page
 * @param page pointer to a page object
 * @param anim_time animation time in milliseconds
 */
void edgui_page_set_anim_time(edgui_obj_t * page, uint16_t anim_time);

/**
 * Enable the scroll propagation feature. If enabled then the page will move its parent if there is
 * no more space to scroll.
 * @param page pointer to a Page
 * @param en true or false to enable/disable scroll propagation
 */
void edgui_page_set_scroll_propagation(edgui_obj_t * page, bool en);

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Page
 * @param en true or false to enable/disable end flash
 */
void edgui_page_set_edge_flash(edgui_obj_t * page, bool en);

/**
 * Set the fit policy in all 4 directions separately.
 * It tell how to change the page size automatically.
 * @param page pointer to a page object
 * @param left left fit policy from `edgui_fit_t`
 * @param right right fit policy from `edgui_fit_t`
 * @param top bottom fit policy from `edgui_fit_t`
 * @param bottom bottom fit policy from `edgui_fit_t`
 */
static __inline void edgui_page_set_scrl_fit4(edgui_obj_t * page, edgui_fit_t left, edgui_fit_t right, edgui_fit_t top, edgui_fit_t bottom)
{
    edgui_cont_set_fit4(edgui_page_get_scrl(page), left, right, top, bottom);
}

/**
 * Set the fit policy horizontally and vertically separately.
 * It tell how to change the page size automatically.
 * @param page pointer to a page object
 * @param hot horizontal fit policy from `edgui_fit_t`
 * @param ver vertical fit policy from `edgui_fit_t`
 */
static __inline void edgui_page_set_scrl_fit2(edgui_obj_t * page, edgui_fit_t hor, edgui_fit_t ver)
{
    edgui_cont_set_fit2(edgui_page_get_scrl(page), hor, ver);
}

/**
 * Set the fit policyin all 4 direction at once.
 * It tell how to change the page size automatically.
 * @param page pointer to a button object
 * @param fit fit policy from `edgui_fit_t`
 */
static __inline void edgui_page_set_scrl_fit(edgui_obj_t * page, edgui_fit_t fit)
{
    edgui_cont_set_fit(edgui_page_get_scrl(page), fit);
}

/**
 * Set width of the scrollable part of a page
 * @param page pointer to a page object
 * @param w the new width of the scrollable (it ha no effect is horizontal fit is enabled)
 */
static __inline void edgui_page_set_scrl_width(edgui_obj_t * page, edgui_coord_t w)
{
    edgui_obj_set_width(edgui_page_get_scrl(page), w);
}

/**
 * Set height of the scrollable part of a page
 * @param page pointer to a page object
 * @param h the new height of the scrollable (it ha no effect is vertical fit is enabled)
 */
static __inline void edgui_page_set_scrl_height(edgui_obj_t * page, edgui_coord_t h)
{
    edgui_obj_set_height(edgui_page_get_scrl(page), h);
}

/**
 * Set the layout of the scrollable part of the page
 * @param page pointer to a page object
 * @param layout a layout from 'edgui_cont_layout_t'
 */
static __inline void edgui_page_set_scrl_layout(edgui_obj_t * page, edgui_layout_t layout)
{
    edgui_cont_set_layout(edgui_page_get_scrl(page), layout);
}

/**
 * Set a style of a page
 * @param page pointer to a page object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_page_set_style(edgui_obj_t * page, edgui_page_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @return the mode from 'edgui_page_sb.mode_t' enum
 */
edgui_sb_mode_t edgui_page_get_sb_mode(const edgui_obj_t * page);

/**
 * Get the scroll propagation property
 * @param page pointer to a Page
 * @return true or false
 */
bool edgui_page_get_scroll_propagation(edgui_obj_t * page);

/**
 * Get the edge flash effect property.
 * @param page pointer to a Page
 * return true or false
 */
bool edgui_page_get_edge_flash(edgui_obj_t * page);

/**
 * Get that width which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the width which still fits into the page
 */
edgui_coord_t edgui_page_get_fit_width(edgui_obj_t * page);

/**
 * Get that height which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the height which still fits into the page
 */
edgui_coord_t edgui_page_get_fit_height(edgui_obj_t * page);

/**
 * Get width of the scrollable part of a page
 * @param page pointer to a page object
 * @return the width of the scrollable
 */
static __inline edgui_coord_t edgui_page_get_scrl_width(const edgui_obj_t * page)
{
    return edgui_obj_get_width(edgui_page_get_scrl(page));
}

/**
 * Get height of the scrollable part of a page
 * @param page pointer to a page object
 * @return the height of the scrollable
 */
static __inline edgui_coord_t edgui_page_get_scrl_height(const edgui_obj_t * page)
{
    return edgui_obj_get_height(edgui_page_get_scrl(page));
}

/**
 * Get the layout of the scrollable part of a page
 * @param page pointer to page object
 * @return the layout from 'edgui_cont_layout_t'
 */
static __inline edgui_layout_t edgui_page_get_scrl_layout(const edgui_obj_t * page)
{
    return edgui_cont_get_layout(edgui_page_get_scrl(page));
}

/**
 * Get the left fit mode
 * @param page pointer to a page object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_page_get_scrl_fit_left(const edgui_obj_t * page)
{
    return edgui_cont_get_fit_left(edgui_page_get_scrl(page));
}

/**
 * Get the right fit mode
 * @param page pointer to a page object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_page_get_scrl_fit_right(const edgui_obj_t * page)
{
    return edgui_cont_get_fit_right(edgui_page_get_scrl(page));
}

/**
 * Get the top fit mode
 * @param page pointer to a page object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_page_get_scrl_fit_top(const edgui_obj_t * page)
{
    return edgui_cont_get_fit_top(edgui_page_get_scrl(page));
}

/**
 * Get the bottom fit mode
 * @param page pointer to a page object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_page_get_scrl_fit_bottom(const edgui_obj_t * page)
{
    return edgui_cont_get_fit_bottom(edgui_page_get_scrl(page));
}

/**
 * Get a style of a page
 * @param page pointer to page object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_page_get_style(const edgui_obj_t * page, edgui_page_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Find whether the page has been scrolled to a certain edge.
 * @param page Page object
 * @param edge Edge to check
 * @return true if the page is on the specified edge
 */
bool edgui_page_on_edge(edgui_obj_t * page, edgui_page_edge_t edge);

/**
 * Glue the object to the page. After it the page can be moved (dragged) with this object too.
 * @param obj pointer to an object on a page
 * @param glue true: enable glue, false: disable glue
 */
void edgui_page_glue_obj(edgui_obj_t * obj, bool glue);

/**
 * Focus on an object. It ensures that the object will be visible on the page.
 * @param page pointer to a page object
 * @param obj pointer to an object to focus (must be on the page)
 * @param anim_en EDGUI_ANIM_ON to focus with animation; EDGUI_ANIM_OFF to focus without animation
 */
void edgui_page_focus(edgui_obj_t * page, const edgui_obj_t * obj, edgui_anim_enable_t anim_en);

/**
 * Scroll the page horizontally
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll left; > 0 scroll right)
 */
void edgui_page_scroll_hor(edgui_obj_t * page, edgui_coord_t dist);

/**
 * Scroll the page vertically
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll down; > 0 scroll up)
 */
void edgui_page_scroll_ver(edgui_obj_t * page, edgui_coord_t dist);

/**
 * Not intended to use directly by the user but by other object types internally.
 * Start an edge flash animation. Exactly one `ext->edge_flash.xxx_ip` should be set
 * @param page
 */
void edgui_page_start_edge_flash(edgui_obj_t * page);
/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_PAGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_PAGE_H*/
