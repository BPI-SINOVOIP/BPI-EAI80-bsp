/**
 * @file edgui_list.h
 *
 */

#ifndef EDGUI_LIST_H
#define EDGUI_LIST_H

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

#if EDGUI_USE_LIST != 0

/*Testing of dependencies*/
#if EDGUI_USE_PAGE == 0
#error "edgui_list: edgui_page is required. Enable it in edgui_conf.h (EDGUI_USE_PAGE  1) "
#endif

#if EDGUI_USE_BTN == 0
#error "edgui_list: edgui_btn is required. Enable it in edgui_conf.h (EDGUI_USE_BTN  1) "
#endif

#if EDGUI_USE_LABEL == 0
#error "edgui_list: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_page.h"
#include "edgui_btn.h"
#include "edgui_label.h"
#include "edgui_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of list*/
typedef struct
{
    edgui_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    const edgui_style_t * styles_btn[_EDGUI_BTN_STATE_NUM]; /*Styles of the list element buttons*/
    const edgui_style_t * style_img;                     /*Style of the list element images on buttons*/
    uint16_t size;                                    /*the number of items(buttons) in the list*/

    uint8_t single_mode : 1; /* whether single selected mode is enabled */

#if EDGUI_USE_GROUP
    edgui_obj_t * last_sel;     /* The last selected button. It will be reverted when the list is focused again */
    edgui_obj_t * selected_btn; /* The button is currently being selected*/
    /*Used to make the last clicked button pressed (selected) when the list become focused and
     * `click_focus == 1`*/
    edgui_obj_t * last_clicked_btn;
#endif
} edgui_list_ext_t;

/** List styles. */
enum {
    EDGUI_LIST_STYLE_BG, /**< List background style */
    EDGUI_LIST_STYLE_SCRL, /**< List scrollable area style. */
    EDGUI_LIST_STYLE_SB, /**< List scrollbar style. */
    EDGUI_LIST_STYLE_EDGE_FLASH, /**< List edge flash style. */
    EDGUI_LIST_STYLE_BTN_REL, /**< Same meaning as the ordinary button styles. */
    EDGUI_LIST_STYLE_BTN_PR,
    EDGUI_LIST_STYLE_BTN_TGL_REL,
    EDGUI_LIST_STYLE_BTN_TGL_PR,
    EDGUI_LIST_STYLE_BTN_INA,
};
typedef uint8_t edgui_list_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
edgui_obj_t * edgui_list_create(edgui_obj_t * par, const edgui_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_list_clean(edgui_obj_t * obj);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @return pointer to the new list element which can be customized (a button)
 */
edgui_obj_t * edgui_list_add_btn(edgui_obj_t * list, const void * img_src, const char * txt);

/**
 * Remove the index of the button in the list
 * @param list pointer to a list object
 * @param index pointer to a the button's index in the list, index must be 0 <= index <
 * edgui_list_ext_t.size
 * @return true: successfully deleted
 */
bool edgui_list_remove(const edgui_obj_t * list, uint16_t index);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set single button selected mode, only one button will be selected if enabled.
 * @param list pointer to the currently pressed list object
 * @param mode enable(true)/disable(false) single selected mode.
 */
void edgui_list_set_single_mode(edgui_obj_t * list, bool mode);

#if EDGUI_USE_GROUP

/**
 * Make a button selected
 * @param list pointer to a list object
 * @param btn pointer to a button to select
 *            NULL to not select any buttons
 */
void edgui_list_set_btn_selected(edgui_obj_t * list, edgui_obj_t * btn);
#endif

/**
 * Set the scroll bar mode of a list
 * @param list pointer to a list object
 * @param sb_mode the new mode from 'edgui_page_sb_mode_t' enum
 */
static __inline void edgui_list_set_sb_mode(edgui_obj_t * list, edgui_sb_mode_t mode)
{
    edgui_page_set_sb_mode(list, mode);
}

/**
 * Enable the scroll propagation feature. If enabled then the List will move its parent if there is
 * no more space to scroll.
 * @param list pointer to a List
 * @param en true or false to enable/disable scroll propagation
 */
static __inline void edgui_list_set_scroll_propagation(edgui_obj_t * list, bool en)
{
    edgui_page_set_scroll_propagation(list, en);
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param list pointer to a List
 * @param en true or false to enable/disable end flash
 */
static __inline void edgui_list_set_edge_flash(edgui_obj_t * list, bool en)
{
    edgui_page_set_edge_flash(list, en);
}

/**
 * Set scroll animation duration on 'list_up()' 'list_down()' 'list_focus()'
 * @param list pointer to a list object
 * @param anim_time duration of animation [ms]
 */
static __inline void edgui_list_set_anim_time(edgui_obj_t * list, uint16_t anim_time)
{
    edgui_page_set_anim_time(list, anim_time);
}

/**
 * Set a style of a list
 * @param list pointer to a list object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_list_set_style(edgui_obj_t * list, edgui_list_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get single button selected mode.
 * @param list pointer to the currently pressed list object.
 */
bool edgui_list_get_single_mode(edgui_obj_t * list);

/**
 * Get the text of a list element
 * @param btn pointer to list element
 * @return pointer to the text
 */
const char * edgui_list_get_btn_text(const edgui_obj_t * btn);
/**
 * Get the label object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
edgui_obj_t * edgui_list_get_btn_label(const edgui_obj_t * btn);

/**
 * Get the image object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
edgui_obj_t * edgui_list_get_btn_img(const edgui_obj_t * btn);

/**
 * Get the next button from list. (Starts from the bottom button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the next after it.
 * @return pointer to the next button or NULL when no more buttons
 */
edgui_obj_t * edgui_list_get_prev_btn(const edgui_obj_t * list, edgui_obj_t * prev_btn);

/**
 * Get the previous button from list. (Starts from the top button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the previous before it.
 * @return pointer to the previous button or NULL when no more buttons
 */
edgui_obj_t * edgui_list_get_next_btn(const edgui_obj_t * list, edgui_obj_t * prev_btn);

/**
 * Get the index of the button in the list
 * @param list pointer to a list object. If NULL, assumes btn is part of a list.
 * @param btn pointer to a list element (button)
 * @return the index of the button in the list, or -1 of the button not in this list
 */
int32_t edgui_list_get_btn_index(const edgui_obj_t * list, const edgui_obj_t * btn);

/**
 * Get the number of buttons in the list
 * @param list pointer to a list object
 * @return the number of buttons in the list
 */
uint16_t edgui_list_get_size(const edgui_obj_t * list);

#if EDGUI_USE_GROUP
/**
 * Get the currently selected button. Can be used while navigating in the list with a keypad.
 * @param list pointer to a list object
 * @return pointer to the selected button
 */
edgui_obj_t * edgui_list_get_btn_selected(const edgui_obj_t * list);
#endif

/**
 * Get the scroll bar mode of a list
 * @param list pointer to a list object
 * @return scrollbar mode from 'edgui_page_sb_mode_t' enum
 */
static __inline edgui_sb_mode_t edgui_list_get_sb_mode(const edgui_obj_t * list)
{
    return edgui_page_get_sb_mode(list);
}

/**
 * Get the scroll propagation property
 * @param list pointer to a List
 * @return true or false
 */
static __inline bool edgui_list_get_scroll_propagation(edgui_obj_t * list)
{
    return edgui_page_get_scroll_propagation(list);
}

/**
 * Get the scroll propagation property
 * @param list pointer to a List
 * @return true or false
 */
static __inline bool edgui_list_get_edge_flash(edgui_obj_t * list)
{
    return edgui_page_get_edge_flash(list);
}

/**
 * Get scroll animation duration
 * @param list pointer to a list object
 * @return duration of animation [ms]
 */
static __inline uint16_t edgui_list_get_anim_time(const edgui_obj_t * list)
{
    return edgui_page_get_anim_time(list);
}

/**
 * Get a style of a list
 * @param list pointer to a list object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const edgui_style_t * edgui_list_get_style(const edgui_obj_t * list, edgui_list_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void edgui_list_up(const edgui_obj_t * list);
/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void edgui_list_down(const edgui_obj_t * list);

/**
 * Focus on a list button. It ensures that the button will be visible on the list.
 * @param btn pointer to a list button to focus
 * @param anim EDGUI_ANOM_ON: scroll with animation, EDGUI_ANIM_OFF: without animation
 */
void edgui_list_focus(const edgui_obj_t * btn, edgui_anim_enable_t anim);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_LIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_LIST_H*/
