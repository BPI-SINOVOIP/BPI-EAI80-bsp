/**
 * @file edgui_win.h
 *
 */

#ifndef EDGUI_WIN_H
#define EDGUI_WIN_H

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

#if EDGUI_USE_WIN != 0

/*Testing of dependencies*/
#if EDGUI_USE_BTN == 0
#error "edgui_win: edgui_btn is required. Enable it in edgui_conf.h (EDGUI_USE_BTN  1) "
#endif

#if EDGUI_USE_LABEL == 0
#error "edgui_win: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#if EDGUI_USE_IMG == 0
#error "edgui_win: edgui_img is required. Enable it in edgui_conf.h (EDGUI_USE_IMG  1) "
#endif

#if EDGUI_USE_PAGE == 0
#error "edgui_win: edgui_page is required. Enable it in edgui_conf.h (EDGUI_USE_PAGE  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_cont.h"
#include "edgui_btn.h"
#include "edgui_label.h"
#include "edgui_img.h"
#include "edgui_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of window*/
typedef struct
{
    /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * page;                  /*Pointer to a page which holds the content*/
    edgui_obj_t * header;                /*Pointer to the header container of the window*/
    edgui_obj_t * title;                 /*Pointer to the title label of the window*/
    const edgui_style_t * style_btn_rel; /*Control button releases style*/
    const edgui_style_t * style_btn_pr;  /*Control button pressed style*/
    edgui_coord_t btn_size;              /*Size of the control buttons (square)*/
} edgui_win_ext_t;

/** Window styles. */
enum {
    EDGUI_WIN_STYLE_BG, /**< Window object background style. */
    EDGUI_WIN_STYLE_CONTENT, /**< Window content style. */
    EDGUI_WIN_STYLE_SB, /**< Window scrollbar style. */
    EDGUI_WIN_STYLE_HEADER, /**< Window titlebar background style. */
    EDGUI_WIN_STYLE_BTN_REL, /**< Same meaning as ordinary button styles. */
    EDGUI_WIN_STYLE_BTN_PR,
};
typedef uint8_t edgui_win_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a window objects
 * @param par pointer to an object, it will be the parent of the new window
 * @param copy pointer to a window object, if not NULL then the new object will be copied from it
 * @return pointer to the created window
 */
edgui_obj_t * edgui_win_create(edgui_obj_t * par, const edgui_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_win_clean(edgui_obj_t * obj);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_src an image source ('edgui_img_t' variable, path to file or a symbol)
 * @return pointer to the created button object
 */
edgui_obj_t * edgui_win_add_btn(edgui_obj_t * win, const void * img_src);

/*=====================
 * Setter functions
 *====================*/

/**
 * Can be assigned to a window control button to close the window
 * @param btn pointer to the control button on teh widows header
 * @param evet the event type
 */
void edgui_win_close_event_cb(edgui_obj_t * btn, edgui_event_t event);

/**
 * Set the title of a window
 * @param win pointer to a window object
 * @param title string of the new title
 */
void edgui_win_set_title(edgui_obj_t * win, const char * title);

/**
 * Set the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
void edgui_win_set_btn_size(edgui_obj_t * win, edgui_coord_t size);

/**
 * Set the layout of the window
 * @param win pointer to a window object
 * @param layout the layout from 'edgui_layout_t'
 */
void edgui_win_set_layout(edgui_obj_t * win, edgui_layout_t layout);

/**
 * Set the scroll bar mode of a window
 * @param win pointer to a window object
 * @param sb_mode the new scroll bar mode from  'edgui_sb_mode_t'
 */
void edgui_win_set_sb_mode(edgui_obj_t * win, edgui_sb_mode_t sb_mode);

/**
 * Set focus animation duration on `edgui_win_focus()`
 * @param win pointer to a window object
 * @param anim_time duration of animation [ms]
 */
void edgui_win_set_anim_time(edgui_obj_t * win, uint16_t anim_time);

/**
 * Set a style of a window
 * @param win pointer to a window object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_win_set_style(edgui_obj_t * win, edgui_win_style_t type, const edgui_style_t * style);

/**
 * Set drag status of a window. If set to 'true' window can be dragged like on a PC.
 * @param win pointer to a window object
 * @param en whether dragging is enabled
 */
void edgui_win_set_drag(edgui_obj_t * win, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the title of a window
 * @param win pointer to a window object
 * @return title string of the window
 */
const char * edgui_win_get_title(const edgui_obj_t * win);

/**
 * Get the content holder object of window (`edgui_page`) to allow additional customization
 * @param win pointer to a window object
 * @return the Page object where the window's content is
 */
edgui_obj_t * edgui_win_get_content(const edgui_obj_t * win);

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
edgui_coord_t edgui_win_get_btn_size(const edgui_obj_t * win);

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
edgui_obj_t * edgui_win_get_from_btn(const edgui_obj_t * ctrl_btn);

/**
 * Get the layout of a window
 * @param win pointer to a window object
 * @return the layout of the window (from 'edgui_layout_t')
 */
edgui_layout_t edgui_win_get_layout(edgui_obj_t * win);

/**
 * Get the scroll bar mode of a window
 * @param win pointer to a window object
 * @return the scroll bar mode of the window (from 'edgui_sb_mode_t')
 */
edgui_sb_mode_t edgui_win_get_sb_mode(edgui_obj_t * win);

/**
 * Get focus animation duration
 * @param win pointer to a window object
 * @return duration of animation [ms]
 */
uint16_t edgui_win_get_anim_time(const edgui_obj_t * win);

/**
 * Get width of the content area (page scrollable) of the window
 * @param win pointer to a window object
 * @return the width of the content area
 */
edgui_coord_t edgui_win_get_width(edgui_obj_t * win);

/**
 * Get a style of a window
 * @param win pointer to a button object
 * @param type which style window be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_win_get_style(const edgui_obj_t * win, edgui_win_style_t type);

/**
 * Get drag status of a window. If set to 'true' window can be dragged like on a PC.
 * @param win pointer to a window object
 * @return whether window is draggable
 */
static __inline bool edgui_win_get_drag(const edgui_obj_t * win)
{
    return edgui_obj_get_drag(win);
}

/*=====================
 * Other functions
 *====================*/

/**
 * Focus on an object. It ensures that the object will be visible in the window.
 * @param win pointer to a window object
 * @param obj pointer to an object to focus (must be in the window)
 * @param anim_en EDGUI_ANIM_ON focus with an animation; EDGUI_ANIM_OFF focus without animation
 */
void edgui_win_focus(edgui_obj_t * win, edgui_obj_t * obj, edgui_anim_enable_t anim_en);

/**
 * Scroll the window horizontally
 * @param win pointer to a window object
 * @param dist the distance to scroll (< 0: scroll right; > 0 scroll left)
 */
static __inline void edgui_win_scroll_hor(edgui_obj_t * win, edgui_coord_t dist)
{
    edgui_win_ext_t * ext = (edgui_win_ext_t *)edgui_obj_get_ext_attr(win);
    edgui_page_scroll_hor(ext->page, dist);
}
/**
 * Scroll the window vertically
 * @param win pointer to a window object
 * @param dist the distance to scroll (< 0: scroll down; > 0 scroll up)
 */
static __inline void edgui_win_scroll_ver(edgui_obj_t * win, edgui_coord_t dist)
{
    edgui_win_ext_t * ext = (edgui_win_ext_t *)edgui_obj_get_ext_attr(win);
    edgui_page_scroll_ver(ext->page, dist);
}

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_WIN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_WIN_H*/
