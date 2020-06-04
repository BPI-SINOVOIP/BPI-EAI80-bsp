/**
 * @file edgui_tabview.h
 *
 */

#ifndef EDGUI_TABVIEW_H
#define EDGUI_TABVIEW_H

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

#if EDGUI_USE_TABVIEW != 0

/*Testing of dependencies*/
#if EDGUI_USE_BTNM == 0
#error "edgui_tabview: edgui_btnm is required. Enable it in edgui_conf.h (EDGUI_USE_BTNM  1) "
#endif

#if EDGUI_USE_PAGE == 0
#error "edgui_tabview: edgui_page is required. Enable it in edgui_conf.h (EDGUI_USE_PAGE  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "../edgui_objx/edgui_win.h"
#include "../edgui_objx/edgui_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Position of tabview buttons. */
enum { EDGUI_TABVIEW_BTNS_POS_TOP, EDGUI_TABVIEW_BTNS_POS_BOTTOM, EDGUI_TABVIEW_BTNS_POS_LEFT, EDGUI_TABVIEW_BTNS_POS_RIGHT };
typedef uint8_t edgui_tabview_btns_pos_t;

/*Data of tab*/
typedef struct
{
    /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * btns;
    edgui_obj_t * indic;
    edgui_obj_t * content; /*A rectangle to show the current tab*/
    const char ** tab_name_ptr;
    edgui_point_t point_last;
    uint16_t tab_cur;
    uint16_t tab_cnt;
#if EDGUI_USE_ANIMATION
    uint16_t anim_time;
#endif
    uint8_t slide_enable : 1; /*1: enable horizontal sliding by touch pad*/
    uint8_t draging : 1;
    uint8_t drag_hor : 1;
    uint8_t scroll_ver : 1;
    uint8_t btns_hide : 1;
    edgui_tabview_btns_pos_t btns_pos : 2;
} edgui_tabview_ext_t;

enum {
    EDGUI_TABVIEW_STYLE_BG,
    EDGUI_TABVIEW_STYLE_INDIC,
    EDGUI_TABVIEW_STYLE_BTN_BG,
    EDGUI_TABVIEW_STYLE_BTN_REL,
    EDGUI_TABVIEW_STYLE_BTN_PR,
    EDGUI_TABVIEW_STYLE_BTN_TGL_REL,
    EDGUI_TABVIEW_STYLE_BTN_TGL_PR,
};
typedef uint8_t edgui_tabview_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
edgui_obj_t * edgui_tabview_create(edgui_obj_t * par, const edgui_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_tabview_clean(edgui_obj_t * obj);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a new tab with the given name
 * @param tabview pointer to Tab view object where to ass the new tab
 * @param name the text on the tab button
 * @return pointer to the created page object (edgui_page). You can create your content here
 */
edgui_obj_t * edgui_tabview_add_tab(edgui_obj_t * tabview, const char * name);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new tab
 * @param tabview pointer to Tab view object
 * @param id index of a tab to load
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_tabview_set_tab_act(edgui_obj_t * tabview, uint16_t id, edgui_anim_enable_t anim);

/**
 * Enable horizontal sliding with touch pad
 * @param tabview pointer to Tab view object
 * @param en true: enable sliding; false: disable sliding
 */
void edgui_tabview_set_sliding(edgui_obj_t * tabview, bool en);

/**
 * Set the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @param anim_time time of animation in milliseconds
 */
void edgui_tabview_set_anim_time(edgui_obj_t * tabview, uint16_t anim_time);

/**
 * Set the style of a tab view
 * @param tabview pointer to a tan view object
 * @param type which style should be set
 * @param style pointer to the new style
 */
void edgui_tabview_set_style(edgui_obj_t * tabview, edgui_tabview_style_t type, const edgui_style_t * style);

/**
 * Set the position of tab select buttons
 * @param tabview pointer to a tab view object
 * @param btns_pos which button position
 */
void edgui_tabview_set_btns_pos(edgui_obj_t * tabview, edgui_tabview_btns_pos_t btns_pos);

/**
 * Set whether tab buttons are hidden
 * @param tabview pointer to a tab view object
 * @param en whether tab buttons are hidden
 */
void edgui_tabview_set_btns_hidden(edgui_obj_t * tabview, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active tab
 * @param tabview pointer to Tab view object
 * @return the active tab index
 */
uint16_t edgui_tabview_get_tab_act(const edgui_obj_t * tabview);

/**
 * Get the number of tabs
 * @param tabview pointer to Tab view object
 * @return tab count
 */
uint16_t edgui_tabview_get_tab_count(const edgui_obj_t * tabview);
/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the tab (>= 0)
 * @return pointer to page (edgui_page) object
 */
edgui_obj_t * edgui_tabview_get_tab(const edgui_obj_t * tabview, uint16_t id);

/**
 * Get horizontal sliding is enabled or not
 * @param tabview pointer to Tab view object
 * @return true: enable sliding; false: disable sliding
 */
bool edgui_tabview_get_sliding(const edgui_obj_t * tabview);

/**
 * Get the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t edgui_tabview_get_anim_time(const edgui_obj_t * tabview);

/**
 * Get a style of a tab view
 * @param tabview pointer to a ab view object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_tabview_get_style(const edgui_obj_t * tabview, edgui_tabview_style_t type);

/**
 * Get position of tab select buttons
 * @param tabview pointer to a ab view object
 */
edgui_tabview_btns_pos_t edgui_tabview_get_btns_pos(const edgui_obj_t * tabview);

/**
 * Get whether tab buttons are hidden
 * @param tabview pointer to a tab view object
 * @return whether tab buttons are hidden
 */
bool edgui_tabview_get_btns_hidden(const edgui_obj_t * tabview);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TABVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TABVIEW_H*/
